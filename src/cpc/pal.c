#include "pal.h"
#include "garray.h"

PAL16L8				PAL;

extern unsigned char *RamConfigurationTable[64*4];

void PAL_Initialise(void)
{
    PAL.pChosenRamConfig = &RamConfigurationTable[0];
}

void PAL_Reset(void)
{
	PAL_WriteConfig(0x0c0);
}

int  PAL_GetRamConfiguration(void)
{
    return PAL.RamConfig;
}


void	PAL_WriteConfig(int Function)
{
	if ((Function & 0x0c0)==0x0c0)
	{
        /* function 11xxxxxx */
        int     Config;

        PAL.RamConfig = (unsigned char)Function;

        Config = Function & 0x03f;
        Config = Config<<2;

        PAL.pChosenRamConfig = &RamConfigurationTable[Config];
	
		GateArray_RethinkMemory();
	}
}
