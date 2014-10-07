#include "cassette.h"

#ifndef CASSETTE_NOSAMPLE
#include "sampload.h"
#endif

#include "tzx.h"


void	Cassette_Init(void)
{
//	TZX_Write_Initialise("out.tmp");
	TapeImage_Init();

}

void	Cassette_Finish(void)
{
//	TZX_Write_End();


}

static unsigned long CPC_CassetteType = CASSETTE_TYPE_NONE;

void	CPC_SetCassetteType(int CassetteType)
{
	CPC_CassetteType = CassetteType;
}

unsigned long CPC_GetCassetteType(void)
{
	return CPC_CassetteType;
}

unsigned long Cassette_Read(unsigned long NopsPassed)
{
	unsigned long CassetteReadBit = 0;

	switch (CPC_CassetteType)
	{
#ifndef CASSETTE_NOSAMPLE
		case CASSETTE_TYPE_SAMPLE:
		{
			CassetteReadBit = Sample_GetDataByteTimed(NopsPassed);
		}
		break;
#endif

		case CASSETTE_TYPE_TAPE_IMAGE:
		{
			unsigned long TStatesPassed;

			/* 4 t-states per NOP */
			TStatesPassed = NopsPassed<<2;
        

			CassetteReadBit = TapeImage_GetBit(TStatesPassed);
		}
		break;
	
		default:
			break;
	}

	return CassetteReadBit;
}


void	Cassette_Write(unsigned long NopsPassed, unsigned long State)
{
//	TZX_Write(NopsPassed, State);
}


			
void	Tape_Remove()
{
	TapeImage_Remove();
#ifndef CASSETTE_NOSAMPLE
	Sample_Close();
#endif
}
