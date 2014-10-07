#include "memory.h"
#include "vrtxram.h"
#include "cpc.h"
#include <stdio.h>
#include <stdlib.h>

/***********/
/* VORTEX  */

/*
Ram board is internal.
Ram board is designed for CPC464.

The GateArray IC and OS ROM EPROM must be removed. The RAM board connects to the main
PCB at the socket for ROM and GateArray.
Replace GateArray and OS ROM into the positions on the ram board.

Each RAM block is 32K.

2 I/O ports are used for access

FBBD: block select:
bit 7: ??
bit 6: ??
bit 5: set to 1
bit 4..0: block number

bit 6,7 mirror functions from port 7fxx?

7fxx:
bit 7=1, bit 6=0 shared with rom configuration. bit 5 was unused in Gate-Array.

Now:
bit 7: 1
bit 6: ram access address (0=low, 0000-7fff, 1=high 8000-ffff)
bit 5: ram card select (1=selected, 0=not selected)

lower rom must be paged out for lower 16k to be accessible.
upper rom must be paged out for upper 16k to be accessible.
*/

static unsigned char Vortex_RamExpansion_Block;
/* allocated ram */
static unsigned char *Vortex_RamExpansion_Ram = NULL;
/* pointer to current block */
static unsigned char *Vortex_RamExpansion_RamPtr = NULL;
static unsigned long Vortex_RamExpansion_BaseMemoryBlock;

static MemoryBlock Vortex_RamExpansion_ReadMemoryBlocks[4];
static MemoryBlock Vortex_RamExpansion_WriteMemoryBlocks[4];

void	Vortex_Initialise(void)
{
	Vortex_RamExpansion_Ram = malloc(512*1024);
}

void	Vortex_Finish(void)
{
	if (Vortex_RamExpansion_Ram!=NULL)
	{
		free(Vortex_RamExpansion_Ram);
		Vortex_RamExpansion_Ram = NULL;
	}
}

static void Vortex_RemoveMemoryBlocks(void)
{
	int i;

	for (i=0; i<4; i++)
	{
//		MemoryBlock_RemoveMemoryBlock(&Vortex_RamExpansion_ReadMemoryBlocks[i]);
	}

	for (i=0; i<4; i++)
	{
//		MemoryBlock_RemoveMemoryBlock(&Vortex_RamExpansion_WriteMemoryBlocks[i]);
	}
}

static void Vortex_RamExpansion_UpdateMemoryConfiguration(void)
{
	int i;

	unsigned long AddressAdjustment;
	
	/* pointer to ram data */
	Vortex_RamExpansion_RamPtr = Vortex_RamExpansion_Ram + (Vortex_RamExpansion_Block<<15);

	/* calculate address adjustment */
	AddressAdjustment = (Vortex_RamExpansion_BaseMemoryBlock<<14);

	for (i=0; i<4; i++)
	{
//		MemoryBlock_RemoveMemoryBlock(&Vortex_RamExpansion_ReadMemoryBlocks[i]);
	}

	for (i=0; i<4; i++)
	{
//		MemoryBlock_RemoveMemoryBlock(&Vortex_RamExpansion_WriteMemoryBlocks[i]);
	}

	
	for (i=0; i<4; i++)
	{
//		Vortex_RamExpansion_ReadMemoryBlocks[i].pPtr = RamPtr - Adjustment;
		AddressAdjustment+=(1<<11);
	}
}

void	Vortex_RamPageWrite(Z80_WORD Port, Z80_BYTE Data)
{
	Z80_BYTE Block;

	Block = Data & 0x01f;

	if (Block!=Vortex_RamExpansion_Block)
	{
		Vortex_RamExpansion_Block = Block;
	
		Vortex_RamExpansion_UpdateMemoryConfiguration();
	}
}

void	Vortex_RamConfigurationWrite(Z80_WORD Port, Z80_BYTE Data)
{
	/* ram expansion selected? */
	if (Data & ((1<<5)|(1<<7)))
	{
		unsigned long BaseMemoryBlock;

		if (Data & (1<<6))
		{
			/* page ram into memory range 0x08000-0x0ffff */
			BaseMemoryBlock = 4;
		}
		else
		{
			/* page ram into memory range 0x00000-0x07fff */
			BaseMemoryBlock = 0;
		}

		/* base block changed? */
		if (Vortex_RamExpansion_BaseMemoryBlock!=BaseMemoryBlock)
		{
			/* set new base */
			Vortex_RamExpansion_BaseMemoryBlock = BaseMemoryBlock;

			Vortex_RamExpansion_UpdateMemoryConfiguration();
		}
	}
}


static CPCPortWrite vortexPortWrite[2]=
{
	{0x0ffff,0x0fbbd, Vortex_RamPageWrite},
	{0x0ff00,0x07f00, Vortex_RamConfigurationWrite},
};

void	Vortex_Install(void)
{
	/* install port write handlers */
	CPC_InstallWritePort(&vortexPortWrite[0]);
	CPC_InstallWritePort(&vortexPortWrite[1]);
}

