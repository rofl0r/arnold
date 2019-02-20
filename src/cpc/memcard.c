#if 0
/*************************/
/**** RAM7 MEMCARD    ****/

static unsigned char *MemCard_Ram;

void	MemCard_Write(Z80_WORD Port, Z80_BYTE Data)
{
	/* detect memcard write */
	unsigned char Upper;

	Upper = Port>>8;

	if ((Upper & 0x0fc)==0x07c)
	{
		int BankIndex;

		BankIndex = Upper & 0x03;

	}
}
#endif

