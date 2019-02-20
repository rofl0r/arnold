#include "keystack.h"


/******************************************************************************************/

/* initialises the stack to hold registry keys */
void	KeyStack_Init(KeyStack *pKeyStack)
{
	pKeyStack->Count = 0;
}
/******************************************************************************************/

/* close all the keys on the stack; closes keys in the reverse order that they were opened */
void	KeyStack_Close(KeyStack *pKeyStack)
{
	int i;

	/* close all the keys put onto the Reg key stack */
	for (i=pKeyStack->Count-1; i>=0; i--)
	{
		RegCloseKey(pKeyStack->Stack[i]);
	}
	pKeyStack->Count = 0;
}
/******************************************************************************************/

void	KeyStack_Push(KeyStack *pKeyStack, HKEY Key)
{
	pKeyStack->Stack[pKeyStack->Count] = Key;
	pKeyStack->Count++;
}
/******************************************************************************************/
HKEY	KeyStack_GetTopItem(KeyStack *pStack)
{
	return pStack->Stack[pStack->Count-1];
}


BOOL	KeyStack_OpenKey(KeyStack *pStack, int Mode, char *pKeyName)
{
	HKEY ParentKey = KeyStack_GetTopItem(pStack);
	HKEY CurrentKey;
	BOOL bSuccess = FALSE;

	/* attempt to open this key for reading */
	if (RegOpenKeyEx(ParentKey,pKeyName, 0, Mode, &CurrentKey)==ERROR_SUCCESS)
	{
		KeyStack_Push(pStack,CurrentKey);
		bSuccess = TRUE;
	}

	return bSuccess;
}

