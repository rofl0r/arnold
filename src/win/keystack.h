#ifndef __KEY_STACK_HEADER_INCLUDED__
#define __KEY_STACK_HEADER_INCLUDED__

#include <windows.h>

#define MAX_KEYS 256

typedef struct
{
	/* handle of each opened key */
	HKEY Stack[MAX_KEYS];
	/* number of opened keys */
	int	Count;
} KeyStack;

/* initialises the stack to hold registry keys */
void	KeyStack_Init(KeyStack *pKeyStack);
/* close all the keys on the stack; closes keys in the reverse order that they were opened */
void	KeyStack_Close(KeyStack *pKeyStack);
/* put a key onto the stack */
void	KeyStack_Push(KeyStack *pKeyStack, HKEY Key);
/* get handle of key at top of stack */
HKEY	KeyStack_GetTopItem(KeyStack *pStack);
/* open a key and automatically push onto stack */
BOOL	KeyStack_OpenKey(KeyStack *pStack, int Mode, char *pKeyName);

#endif