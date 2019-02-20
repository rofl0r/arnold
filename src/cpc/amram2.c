#if 0
/*************************/
/*** AMRAM/AMRAM 2     ***/

static unsigned char *Amram_RamState = 0;

/* amram - fbf1, fbf0 */

void	Amram_Write(Z80_WORD Port, Z80_BYTE Data)
{
		Amram_RamState = Data;
}

CPCPortWrite amramPortWrite={0x0fbf1,0x0fbf1, Amram_Write};

void	Amram_Install(void)
{
	CPC_PortWriteInstall(&amramPortWrite);


}
#endif


