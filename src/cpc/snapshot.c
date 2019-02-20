/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *  
 *  This file is part of the Arnold emulator source code distribution.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/********************/
/* snapshot support */
/********************/

/* The data common to snapshots 1,2,3 are in CPCEMU compatible form: */
/* - ink values are stored with bits 5,6,7 = 0 */
/* - PPI Control is the I/O state and mode of the ports and not the last value written to this port */
/* - PPI port C is  the last value written to this port */
/* - PPI port B is the inputs to this port */
/* - PPI port A is the inputs to this port */
/* - ram configuration does not have bit 7 and bit 6 set to 1 */

/* Version 3 has the additional chunks as written by No$CPC v1.8 */


#include "headers.h"

#include "snapshot.h"
#include "z80/z80.h"
#include "cpc.h"
#include "snapv3.h"
#include "fdi.h"

extern char *Z80MemoryBase;
extern char *Amstrad_ExtraRam;

/* get a register pair from memory */
#define SNAPSHOT_GET_REGISTER_PAIR(ptr,offset) \
	((ptr[offset+1] & 0x0ff)<<8) | (ptr[offset+0] & 0x0ff);

/* get a register from memory */
#define SNAPSHOT_GET_REGISTER(ptr,offset) \
	(ptr[offset] & 0x0ff)

/* put a register pair to memory */
#define SNAPSHOT_PUT_REGISTER_PAIR(ptr,offset,data) \
	ptr[offset+0] = data & 0x0ff; \
	ptr[offset+1] = (data>>8) & 0x0ff

/* put a register to memory */
#define SNAPSHOT_PUT_REGISTER(ptr,offset,data) \
	ptr[offset+0] = data & 0x0ff

#define SNAPSHOT_CRTC_FLAGS_HSYNC_STATE (1<<0)
#define SNAPSHOT_CRTC_FLAGS_VSYNC_STATE (1<<1)
#define SNAPSHOT_CRTC_FLAGS_VADJ_STATE (1<<7)

/* insert snapshot */
int Snapshot_Insert(const unsigned char *pSnapshot, const unsigned long SnapshotLength)
{
		BOOL Status = ARNOLD_STATUS_UNRECOGNISED;
        SNAPSHOT_HEADER *pSnapshotHeader;

        /* failed so quit */
        if (pSnapshot!=NULL)
        {
                pSnapshotHeader = (SNAPSHOT_HEADER *)pSnapshot;

                /* snapshot length must be at least the size of snapshot header */
                if (SnapshotLength>sizeof(SNAPSHOT_HEADER))
                {
                        /* is snapshot header text present? */
                        if (memcmp(pSnapshotHeader, SNAPSHOT_HEADER_TEXT, 8)==0)
                        {
                                /* header present */

                                /* check version */
                                if ((pSnapshotHeader->Version>=1) && (pSnapshotHeader->Version<=3))
                                {
                                        char    *pSnapshotData;
                                        int i;

										/* mem size to read from snapshot; 64k or 128k */
                                        int MemSizeBytes;

										/* mem size claimed to be in snapshot */
										int MemSizeInSnapshot;

										{
											/* if version 1, 2 or 3, memory size is defined in header */

											int MemSizeK;

		                                    /* get memory dump size */
											MemSizeK = (pSnapshotHeader->MemSizeLow & 0x0ff) | ((pSnapshotHeader->MemSizeHigh & 0x0ff)<<8);

											MemSizeInSnapshot = MemSizeK<<10;

											/* round size up to 64 or 128K */
											/* will ignore data for sizes larger than 128k */
											if (MemSizeK<=64)
											{
												MemSizeK=64;
											}
											else
											if (MemSizeK<=128)
											{
												MemSizeK=128;
											}
											else
											{
												MemSizeK=128;
											}
										
											MemSizeBytes = MemSizeK<<10;
										}


										/* can either force 64k expansion ram or choose to ignore extra data? */
										/* for now, will force 64k expansion ram */

										/* 64k? */
										if (MemSizeBytes!=(64<<10))
										{
											unsigned long RamConfig;

											/* get ram config */
											RamConfig = CPC_GetRamConfig();
											
											/* has 64k expansion ram? */
											if ((RamConfig & CPC_RAM_CONFIG_64K_RAM)==0)
											{
												/* no, force 64k expansion ram */
												CPC_SetRamConfig(RamConfig | CPC_RAM_CONFIG_64K_RAM);
											}
										}

                                        /* file must be at least the size of the header + the number of bytes
                                        it claims to contain */
                                        if (SnapshotLength>=(sizeof(SNAPSHOT_HEADER)+MemSizeBytes))
                                        {
                                                /* Appears snapshot is valid. Setup Z80 and copy memory */

											int CPCType;
											int Register;

                                                /**** INIT Z80 ****/
                                                /* setup Z80 */
                                                char *pRegs = (char *)pSnapshotHeader + 0x011;

                                                /* reset CPC */
                                                CPC_Reset();

												/* get CPC type */
												CPCType = ((char *)pSnapshotHeader)[0x06d];
                                               

												if (
													(pSnapshotHeader->Version==1) ||
													(pSnapshotHeader->Version==2))
												{
													/* version 1 and 2 did not know about CPC+ so do not allow them
													to be set */
													
													/* set CPC type accordingly */
													switch (CPCType)
													{
														case 0:
														{
															CPC_SetHardware(CPC_HW_CPC);
//															CPC_SetCPCType(CPC_TYPE_CPC464);
														}
														break;

														case 1:
														{
															CPC_SetHardware(CPC_HW_CPC);
//															CPC_SetCPCType(CPC_TYPE_CPC664);
														}
														break;

														/* for unknown or other values, assume cpc6128 */
														default:
														case 2:
														{
															CPC_SetHardware(CPC_HW_CPC);
//															CPC_SetCPCType(CPC_TYPE_CPC6128);
														}
														break;

													}
												}
												else
												if (pSnapshotHeader->Version==3)
												{
													/* version 3 understands CPC+ */
													/* allow them to be set */

													/* set CPC type accordingly */
													switch (CPCType)
													{
														case 0:
														{
															CPC_SetHardware(CPC_HW_CPC);
//															CPC_SetCPCType(CPC_TYPE_CPC464);
														}
														break;

														case 1:
														{
															CPC_SetHardware(CPC_HW_CPC);
//															CPC_SetCPCType(CPC_TYPE_CPC664);
														}
														break;

														case 2:
														{
															CPC_SetHardware(CPC_HW_CPC);
//															CPC_SetCPCType(CPC_TYPE_CPC6128);

														}
														break;


														/* 6 is GX4000 */
														case 6:
														case 5:
														{
															CPC_SetHardware(CPC_HW_CPCPLUS);
//															CPC_SetCPCType(CPC_TYPE_464PLUS);
														}
														break;

														/* for unknown etc assume 6128+ */
														default:
														case 4:
														{
															CPC_SetHardware(CPC_HW_CPCPLUS);
//															CPC_SetCPCType(CPC_TYPE_6128PLUS);
														}
														break;
													}
												}

												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,0);
												Z80_SetReg(Z80_AF, Register);

												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,2);
												Z80_SetReg(Z80_BC, Register);

												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,4);
												Z80_SetReg(Z80_DE, Register);
												
												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,6);
												Z80_SetReg(Z80_HL, Register);

												Z80_SetReg(Z80_R, SNAPSHOT_GET_REGISTER(pRegs,8));

												Z80_SetReg(Z80_I, SNAPSHOT_GET_REGISTER(pRegs,9));

                                                Z80_SetReg(Z80_IFF1, pRegs[10] & 1);

                                                Z80_SetReg(Z80_IFF2, pRegs[11] & 1);

												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,12);	
												Z80_SetReg(Z80_IX, Register);

												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,14);
												Z80_SetReg(Z80_IY, Register);
								
												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,16);
												Z80_SetReg(Z80_SP, Register);
												
												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,18);
												Z80_SetReg(Z80_PC, Register);

												Register = pRegs[20] & 3;

												if ((Register<0) || (Register>2))
												{
													Register = 0;
												}

												Z80_SetReg(Z80_IM, Register);

												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,21);
												Z80_SetReg(Z80_AF2, Register);

												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,23);
												Z80_SetReg(Z80_BC2, Register);

												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,25);
												Z80_SetReg(Z80_DE2, Register);

												Register = SNAPSHOT_GET_REGISTER_PAIR(pRegs,27);
												Z80_SetReg(Z80_HL2, Register);


                                                /**** GATE ARRAY ****/
                                                /* initialise colour palette */
                                                for (i=0; i<17; i++)
                                                {
                                                        unsigned char HwColourIndex = ((char *)pSnapshotHeader)[0x02f + i];

                                                        /* pen select */
                                                        GateArray_Write(i);
                                                
                                                        /* write colour for pen */
                                                        GateArray_Write((HwColourIndex & 0x01f) | 0x040);
                                                }

                                                /* pen select */
                                                GateArray_Write(((char *)pSnapshotHeader)[0x02e] & 0x01f);
                                                /* mode and rom configuration select */
                                                GateArray_Write((((char *)pSnapshotHeader)[0x040] & 0x03f) | 0x080);
                                                /* ram configuration select */
                                                PAL_WriteConfig((((char *)pSnapshotHeader)[0x041] & 0x03f) | 0x0c0);

                                                /**** CRTC ****/
                                                /* initialise CRTC */
                                                for (i=0; i<18; i++)
                                                {
                                                        unsigned char CRTCRegData = ((char *)pSnapshotHeader)[0x043 + i];

                                                        CRTC_RegisterSelect(i);

                                                        CRTC_WriteData(CRTCRegData & 0x0ff);
                                                }

                                                /* select CRTC register */
                                                CRTC_RegisterSelect(((char *)pSnapshotHeader)[0x042]);

                                                /**** ROM INDEX ****/
                                                /* select expansion rom */
                                                ROM_Select(((char *)pSnapshotHeader)[0x055]);


                                                /**** PSG ****/
                                                /* setup PSG registers */
                                                for (i=0; i<16; i++)
                                                {
                                                        unsigned char PSGRegData = ((char *)pSnapshotHeader)[0x05b + i];

                                                        PSG_RegisterSelect(i);

                                                        PSG_WriteData(PSGRegData);
                                                }

                                                /* select PSG register */
                                                PSG_RegisterSelect(((char *)pSnapshotHeader)[0x05a]);

                                                /**** PPI ****/

                                                /* set control */
                                                PPI_WriteControl(((char *)pSnapshotHeader)[0x059]);

                                                /* set port A data */
                                                PPI_SetPortADataFromSnapshot(((char *)pSnapshotHeader)[0x056]);
                                                /* set port B data */
                                                PPI_SetPortBDataFromSnapshot(((char *)pSnapshotHeader)[0x057]);
                                                /* set port C data */
 												PPI_SetPortCDataFromSnapshot(((char *)pSnapshotHeader)[0x058]);


												if (pSnapshotHeader->Version==3)
												{
													unsigned long CRTC_Flags;
													int CRTCType;
													CRTC_INTERNAL_STATE *pCRTC_State;

													pCRTC_State = CRTC_GetInternalState();

													FDI_SetMotorState(((char *)pSnapshotHeader)[0x09c]&0x01);

													Printer_WriteDataByte(((char *)pSnapshotHeader)[0x0a1]);

													CRTCType = ((char *)pSnapshotHeader)[0x0a4];

													if ((CRTCType<0) || (CRTCType>=5))
													{
														CRTCType = 0;
													}

													CRTC_SetType(CRTCType);

													pCRTC_State->HCount = ((char *)pSnapshotHeader)[0x0a9] & 0x0ff;
													pCRTC_State->LineCounter = ((char *)pSnapshotHeader)[0x0ab] & 0x07f;
													pCRTC_State->RasterCounter = ((char *)pSnapshotHeader)[0x0ac] & 0x01f;
																	
													CRTC_Flags = (((char *)pSnapshotHeader)[0x0b0] & 0x0ff) | ((((char *)pSnapshotHeader)[0x0b1] & 0x0ff)<<8);

													pCRTC_State->CRTC_Flags &=~CRTC_VS_FLAG;
													if (CRTC_Flags & SNAPSHOT_CRTC_FLAGS_VSYNC_STATE)
													{
														pCRTC_State->VerticalSyncCount = ((char *)pSnapshotHeader)[0x0af];
														pCRTC_State->CRTC_Flags |= CRTC_VS_FLAG;
													}

													pCRTC_State->CRTC_Flags &=~CRTC_HS_FLAG;
													if (CRTC_Flags & SNAPSHOT_CRTC_FLAGS_HSYNC_STATE)
													{
														pCRTC_State->HorizontalSyncCount = ((char *)pSnapshotHeader)[0x0ae];
														pCRTC_State->CRTC_Flags |= CRTC_HS_FLAG;
													}

													pCRTC_State->CRTC_Flags &=~CRTC_VADJ_FLAG;
													if (CRTC_Flags & SNAPSHOT_CRTC_FLAGS_VADJ_STATE)
													{
														pCRTC_State->VertAdjustCount = ((char *)pSnapshotHeader)[0x0ad];
														pCRTC_State->CRTC_Flags |= CRTC_VADJ_FLAG;
													}
													
													GateArray_SetInterruptLineCount(((char *)pSnapshotHeader)[0x0b3]);
												
													if (((char *)pSnapshotHeader)[0x0b4]&0x01)
													{
														Z80_SetInterruptRequest();
													}
													else
													{
														Z80_ClearInterruptRequest();
													}

													GateArray_SetVsyncSynchronisationCount(((char *)pSnapshotHeader)[0x0b2]);
												}


                                                /**** INIT RAM ****/
												/* THIS NEEDS TO TAKE INTO ACCOUNT ADDITIONAL RAM settings */
                                                /* setup RAM */
                                                pSnapshotData = ((char *)pSnapshotHeader + sizeof(SNAPSHOT_HEADER));

												/* file size must have at least 64k in it, otherwise it will
												not get to here */
												memcpy(Z80MemoryBase, pSnapshotData, 64<<10);

												/* 128k snapshot? */
												if (MemSizeBytes==(128<<10))
												{
													/* extra ram setup */
													if (Amstrad_ExtraRam!=NULL)
													{
														/* copy data into it */
														memcpy(Amstrad_ExtraRam, pSnapshotData + (64<<10), (64<<10));
													}
												}
												
												Status = ARNOLD_STATUS_OK;

												if (pSnapshotHeader->Version ==3)
												{
													/* must be at least enough data for a chunk header after the memory dump */
													if (SnapshotLength>(MemSizeInSnapshot+sizeof(SNAPSHOT_HEADER)+sizeof(RIFF_CHUNK)))
													{
														/* some chunks after the data */
														RIFF_CHUNK *pChunk = (RIFF_CHUNK *)((char *)pSnapshotHeader + sizeof(SNAPSHOT_HEADER)+MemSizeInSnapshot);
														unsigned long SizeRemaining;

														SizeRemaining = SnapshotLength - MemSizeInSnapshot - sizeof(SNAPSHOT_HEADER);

														while (SizeRemaining!=0)
														{
															int ChunkSize;

															/* get length of this chunk's data in bytes */
															ChunkSize = Riff_GetChunkLength(pChunk);

															if (ChunkSize==0)
																break;

															/* chunk size bad? */
															if (ChunkSize>(SizeRemaining-sizeof(RIFF_CHUNK)))
															{
																/* size of chunk is greater than size of data remaining */
																Status = FALSE;
																break;
															}

															/* attempt to handle this chunk */
															SnapshotV3_HandleChunk(pChunk,ChunkSize);

															/* update chunk pointer */
															pChunk = (RIFF_CHUNK *)((unsigned long)pChunk + ChunkSize + sizeof(RIFF_CHUNK));
														
															/* update size of data remaining */
															SizeRemaining -= (ChunkSize + sizeof(RIFF_CHUNK));
														}
													}
												}

                                        }

                                }

                        }
                }
        }
        
        return Status;
}

/* calculate the size of the output snapshot based on the passed parameters */
unsigned long Snapshot_CalculateOutputSize(int SnapshotSizeInK, int Version)
{
	unsigned long SnapshotSize;

	/* fixed size snapshot header */
	SnapshotSize = sizeof(SNAPSHOT_HEADER);

	SnapshotSize += SnapshotSizeInK*1024;
	
	if (Version==3)
	{
		/* V3 stuff */

		/* CPC+ hardware? */
		if (CPC_GetHardware()==CPC_HW_CPCPLUS)
		{
			SnapshotSize += SnapshotV3_CPCPlus_CalculateOutputSize();
		}
	}

	return SnapshotSize;
}



/* fills the supplied pre-allocated buffer with snapshot data. It is the responsibility
of the calling function to allocate and free the data */
void Snapshot_GenerateOutputData(unsigned char *pBuffer, int SnapshotSizeInK, int Version)
{
    int             SnapshotMemorySize;
    int             SnapshotSize;

	/* override snapshot size for version 1 */
	if (SnapshotSizeInK==128)
	{
		/* extra ram allocated? */
		if (Amstrad_ExtraRam==NULL)
		{
			/* no */
			return;
		}

		/* has 64k ram expansion? */
		if ((CPC_GetRamConfig() & CPC_RAM_CONFIG_64K_RAM)==0)
		{
			/* no */
			return;
		}
	}


    /* initially only write 64K snapshots */
    SnapshotMemorySize = SnapshotSizeInK*1024;

    /* calculate size of output file */
    SnapshotSize = SnapshotMemorySize + sizeof(SNAPSHOT_HEADER);

    {
        int i;
        unsigned char SnapshotHeader[256];
		int CPCType;

        /* clear header */
        memset(&SnapshotHeader, 0, sizeof(SNAPSHOT_HEADER));

        /* setup header */
        memcpy(&SnapshotHeader, SNAPSHOT_HEADER_TEXT, 8);

        /* set version */
        SnapshotHeader[0x010] = (unsigned char)Version;

		switch (Version)
		{
			case 1:
			case 2:
			{
				CPCType = 2;
#if 0
				/* version 2 doesn't recognise KC Compact or CPC+! */
				/* convert my CPC type to Snapshot CPC type index. */						
				switch (1)	//CPC_GetCPCType())
				{
					case CPC_TYPE_CPC464:
					{
						CPCType = 0;
					}
					break;

					case CPC_TYPE_CPC664:
					{
						CPCType = 1;
					}
					break;

					case CPC_TYPE_CPC6128:
					{
						CPCType = 2;
					}
					break;

					default:
					{
						/* unknown */
						CPCType = 3;
					}
					break;
				}
#endif
			}
			break;

			default:
			case 3:
			{
				/* version 3 doesn't recognise KC Compact */
				/* convert my CPC type index to Snapshot CPC type index. */
				if (CPC_GetHardware()==CPC_HW_CPC)
				{
					CPCType = 2;
				}
				else
				{
					CPCType = 4;
				}

#if 0
				
				/* NOTE: GX4000 not supported by Arnold at this time, so is not defined in this file */
				switch (1)	//CPC_GetCPCType())
				{
					case CPC_TYPE_CPC464:
					{
						CPCType = 0;
					}
					break;

					case CPC_TYPE_CPC664:
					{
						CPCType = 1;
					}
					break;

					case CPC_TYPE_CPC6128:
					{
						CPCType = 2;
					}
					break;

					case CPC_TYPE_6128PLUS:
					{
						CPCType = 4;
					}
					break;

					case CPC_TYPE_464PLUS:
					{
						CPCType = 5;
					}
					break;

					default:
					{
						CPCType = 3;
					}
					break;
				}
#endif
			}
			break;
		}

        /* get cpc type */
        SnapshotHeader[0x06d]  = (unsigned char)CPCType;

        /* ***** Z80 CPU ***** */
        {
				int Register;

                char *pRegs = (char *)&SnapshotHeader[0x011];
        
				Register = Z80_GetReg(Z80_AF);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,0, Register);

				Register = Z80_GetReg(Z80_BC);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,2, Register);

				Register = Z80_GetReg(Z80_DE);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,4, Register);

				Register = Z80_GetReg(Z80_HL);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,6, Register);

				pRegs[8] = (char)Z80_GetReg(Z80_R);
				pRegs[9] = (char)Z80_GetReg(Z80_I);

				 /* iff0 */
                /* iff1 */

                if (Z80_GetReg(Z80_IFF1)!=0)
				{
					pRegs[10] = 1;
				}
				
                if (Z80_GetReg(Z80_IFF2)!=0)
				{
                        pRegs[11] = 1;
                }
 		
				Register = Z80_GetReg(Z80_IX);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,12, Register);

				Register = Z80_GetReg(Z80_IY);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,14, Register);

				Register = Z80_GetReg(Z80_SP);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,16, Register);

				Register = Z80_GetReg(Z80_PC);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,18, Register);

				Register = Z80_GetReg(Z80_IM);
				pRegs[20] = (char)(Register & 0x03);
				
				Register = Z80_GetReg(Z80_AF2);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,21, Register);

				Register = Z80_GetReg(Z80_BC2);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,23, Register);

				Register = Z80_GetReg(Z80_DE2);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,25, Register);

				Register = Z80_GetReg(Z80_HL2);

				SNAPSHOT_PUT_REGISTER_PAIR(pRegs,27, Register);

        }

        /**** GATE ARRAY ****/
        /* store colour palette */
        for (i=0; i<17; i++)
        {
           SnapshotHeader[0x02f + i] = (unsigned char)((GateArray_GetPaletteColour(i) & 0x01f));
        }

        /* store selected pen */
        SnapshotHeader[0x02e] = (unsigned char)(GateArray_GetSelectedPen());

        /* store multi-configuration */
        SnapshotHeader[0x040] = (unsigned char)((GateArray_GetMultiConfiguration()&0x01f)|0x080);

        /* store ram configuration */
        SnapshotHeader[0x041] = (unsigned char)(PAL_GetRamConfiguration() & 0x03f);

        /**** CRTC ****/
        /* get CRTC status */
        for (i=0; i<18; i++)
        {
                SnapshotHeader[0x043 + i] = CRTC_GetRegisterData(i);
        }

        /* select CRTC register */
        SnapshotHeader[0x042] = (unsigned char)(CRTC_GetSelectedRegister() & 0x01f);

        /**** ROM INDEX ****/
        /* select expansion rom */
        SnapshotHeader[0x055] = (unsigned char)ROM_GetSelectedROM();


        /**** PSG ****/
        /* get PSG status */
        for (i=0; i<16; i++)
        {
                SnapshotHeader[0x05b + i] = (unsigned char)PSG_GetRegisterData(i);
        }

        /* select PSG register */
        SnapshotHeader[0x05a] = (unsigned char)(PSG_GetSelectedRegister() & 0x0f);
        
        /**** PPI ****/
        /* get PPI control */
        SnapshotHeader[0x059] = (unsigned char)PPI_GetControlForSnapshot();

        /* get port A data */
        SnapshotHeader[0x056] = (unsigned char)PPI_GetPortADataForSnapshot();
        /* get port B data */
        SnapshotHeader[0x057] = (unsigned char)PPI_GetPortBDataForSnapshot();
        /* get port C data */
        SnapshotHeader[0x058] = (unsigned char)PPI_GetPortCDataForSnapshot();


        /* set memory size */
        SnapshotHeader[0x06b] = (unsigned char)(SnapshotMemorySize>>10); 
        SnapshotHeader[0x06c] = (unsigned char)((SnapshotMemorySize>>10)>>8);

		if (Version==3)
		{
			unsigned long CRTC_Flags;

			/* put V3 stuff into header */
			CRTC_INTERNAL_STATE *pCRTC_State;

			pCRTC_State = CRTC_GetInternalState();

			if (FDI_GetMotorState())
			{
				SnapshotHeader[0x09c] = 1;
			}
			else
			{
				SnapshotHeader[0x09c] = 0;
			}

			/* last byte written to printer port */
			SnapshotHeader[0x0a1] = Printer_GetDataByte();

			/* crtc type */
			SnapshotHeader[0x0a4] = CPC_GetCRTCType();

			/* crtc internal state */
			SnapshotHeader[0x0a9] = pCRTC_State->HCount;
			SnapshotHeader[0x0ab] = pCRTC_State->LineCounter;
			SnapshotHeader[0x0ac] = pCRTC_State->RasterCounter;
			SnapshotHeader[0x0ad] = pCRTC_State->VertAdjustCount;

			CRTC_Flags = 0;

			if (pCRTC_State->CRTC_Flags & CRTC_VS_FLAG)
			{
				SnapshotHeader[0x0af] = pCRTC_State->VerticalSyncCount & 0x0f;
				CRTC_Flags |= SNAPSHOT_CRTC_FLAGS_VSYNC_STATE;
			}

			if (pCRTC_State->CRTC_Flags & CRTC_HS_FLAG)
			{
				SnapshotHeader[0x0ae] = pCRTC_State->HorizontalSyncCount & 0x0f;
				CRTC_Flags |= SNAPSHOT_CRTC_FLAGS_HSYNC_STATE;
			}

			if (pCRTC_State->CRTC_Flags & CRTC_VADJ_FLAG)
			{
				SnapshotHeader[0x0ad] = pCRTC_State->VertAdjustCount & 0x01f;
				CRTC_Flags |= SNAPSHOT_CRTC_FLAGS_VADJ_STATE;
			}


			SnapshotHeader[0x0b0] = CRTC_Flags & 0x0ff;
			SnapshotHeader[0x0b1] = (CRTC_Flags>>8) & 0x0ff;				

			SnapshotHeader[0x0b3] = GateArray_GetInterruptLineCount();

			if (Z80_GetInterruptRequest())
			{
				SnapshotHeader[0x0b4] = 1;
			}
			else
			{
				SnapshotHeader[0x0b4] = 0;
			}

			SnapshotHeader[0x0b2] = GateArray_GetVsyncSynchronisationCount();
		}


		memcpy(pBuffer, SnapshotHeader, sizeof(SNAPSHOT_HEADER));
		pBuffer+=sizeof(SNAPSHOT_HEADER);

		/* copy base 64k out to snapshot */
		memcpy(pBuffer, Z80MemoryBase, (64<<10));
		pBuffer+=(64<<10);

		if (SnapshotSizeInK==128)
		{
			memcpy(pBuffer, Amstrad_ExtraRam, (64<<10));
			pBuffer+=(64<<10);
		}

		if (Version==3)
		{
			/* poke V3 stuff into header */

			/* CPC+ hardware? */
			if (CPC_GetHardware()==CPC_HW_CPCPLUS)
			{
				pBuffer = SnapshotV3_CPCPlus_WriteChunk(pBuffer);
			}
		}
    }
}
