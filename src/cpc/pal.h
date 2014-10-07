#ifndef __RAM_PAL_HEADER_INCLUDED__
#define __RAM_PAL_HEADER_INCLUDED__


typedef struct
{
	unsigned long RamConfig;

	unsigned char **pChosenRamConfig;

	/* these are the addressess for ram r/w i.e. when rom is not paged in */
	unsigned char	*RamConfigRead[8];
	unsigned char	*RamConfigWrite[8];

	/* these are the addressess when rom is active */
	unsigned char	*RomConfigRead[8];
	unsigned char	*RomConfigWrite[8];
} PAL16L8;

void	PAL_Initialise(void);
void	PAL_Reset(void);
int		PAL_GetRamConfiguration(void);
void	PAL_WriteConfig(int Function);

#endif
