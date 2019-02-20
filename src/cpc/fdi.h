#ifndef __FLOPPY_DISC_INTERFACE_HEADER_INCLUDED__
#define __FLOPPY_DISC_INTERFACE_HEADER_INCLUDED__

#include "cpcglob.h"
#include "device.h"
#include "fdd.h"

typedef struct
{
	/* drive and side we are actually accessing */
	FDD *drive;

	/* index of physical drive selected */
	unsigned long PhysicalDrive;
	/* index of physical side selected */
	unsigned long PhysicalSide;

	int Density;
} FDI;

void FDI_SwapDrives(void);
/* convert from FDC drive output to floppy disc interface output */
void FDI_SetPhysicalDrive(unsigned long);
/* convert from FDC side output to floppy disc interface output */
void FDI_SetPhysicalSide(unsigned long);

BOOL FDI_GetMotorState(void);
void FDI_SetMotorState(int);

unsigned long FDI_GetSelectedDriveFlags(void);

/* translate from fdc drive select outputs to fdi drive select outputs and
perform a step on that drive */
void	FDI_PerformStep(unsigned long DriveIndex, int Direction);
unsigned long	FDI_GetNextID(CHRN *);
void			FDI_ReadSector(char *);
void			FDI_WriteSector(char *, int Mark);
void			FDI_EmptyTrack(void);
void			FDI_AddSector(CHRN *, int N, int Filler);
void			FDI_SetDensity(int);

#endif

