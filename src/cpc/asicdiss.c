/**************************************/
/* ASIC CPC+ DMA					  */
/* dissassemble instruction to buffer */
#include "z80\z80.h"
#include "gendiss.h"

int		ASIC_DMA_GetOpcodeCount(int Address)
{
	return 2;
}


void	ASIC_DMA_DissassembleInstruction(int Address, char *pDissString)
{
		Z80_WORD        Command;
        int     CommandOpcode;

		/* make it even */
		Address = Address & (0x0ffff^1);

        /* fetch command */
        Command = Z80_RD_BASE_WORD(Address);

        /* get opcode */
        CommandOpcode = (Command>>12) & 0x07;

		/* dissassemble */
		if (CommandOpcode == 0x0)
		{
			/* LOAD R,D */
            unsigned long Register,Data;

            Register = (Command>>8) & 0x0f;

            Data = Command & 0x0ff;

			pDissString = Diss_strcat(pDissString,"LOAD");
			pDissString = Diss_space(pDissString);
			pDissString = Diss_WriteHexByte(pDissString,Register);
			pDissString = Diss_comma(pDissString);
			pDissString = Diss_WriteHexByte(pDissString,Data);

		}
		else
		{
			/* PAUSE? */
			 if ((CommandOpcode & (1<<0))!=0)
		     {
                /* PAUSE n */

				unsigned long PauseCount = Command & 0x0fff;

				pDissString = Diss_strcat(pDissString,"PAUSE");
				pDissString = Diss_space(pDissString);
				pDissString = Diss_WriteHexWord(pDissString,PauseCount);

				/* clear bit */
				CommandOpcode &=~(1<<0);

				/* any other bits set? */
				if (CommandOpcode!=0)
				{
					pDissString = Diss_colon(pDissString);
				}
			 }



			/* REPEAT? */
             if ((CommandOpcode & (1<<1))!=0)
             {
                 /* REPEAT n */

				 unsigned long RepeatCount = Command & 0x0fff;

				 pDissString = Diss_strcat(pDissString,"REPEAT");
				 pDissString = Diss_space(pDissString);
				 Diss_WriteHexWord(RepeatCount);
				 CommandOpcode &=~(1<<1);

	 			 /* any other bits set? */
				 if (CommandOpcode!=0)
				 {
					 pDissString = Diss_colon(pDissString);
				 }
			 }


			/* NOP, LOOP, INT or STOP? */
            if ((CommandOpcode & (1<<2))!=0)
            {
				Command &=0x01 | 0x0010 | 0x0020;

				if (Command == 0)
				{
					pDissString = Diss_strcat(pDissString,"NOP");
				}
				else
				{

					/* NOP, LOOP, INT, STOP */
					if (Command & 0x0001)
					{
						/* LOOP */

						pDissString = Diss_strcat(pDissString,"LOOP");

						Command &=~0x0001;

       					if (Command!=0)
						{
							pDissString = Diss_colon(pDissString);
						}
					}


					if (Command & 0x0010)
					{
						/* INT */

						pDissString = Diss_strcat(pDissString,"INT");

						Command &=~0x0010;

						if (Command !=0)
						{
							pDissString = Diss_colon(pDissString);
						}
					}



					if (Command & 0x0020)
					{
						/* STOP */
						pDissString = Diss_strcat(pDissString,"STOP");
					}
				}
            }
   		}

	Diss_endstring(pDissString);
}
