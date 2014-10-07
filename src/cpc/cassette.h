#ifndef __CASSETTE_HEADER_INCLUDED__
#define __CASSETTE_HEADER_INCLUDED__


typedef enum
{
	CASSETTE_TYPE_SAMPLE = 0,
	CASSETTE_TYPE_TAPE_IMAGE,
	CASSETTE_TYPE_NONE
} CPC_CASSETTE_TYPE_ID;

void	CPC_SetCassetteType(int);

unsigned long Cassette_Read(unsigned long NopsPassed);
void	Cassette_Write(unsigned long NopsPassed, unsigned long State);
void	Cassette_Init(void);
void	Cassette_Finish(void);

unsigned long CPC_GetCassetteType(void);


void	Tape_Remove(void);


#endif

