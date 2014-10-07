/* dll.cpp : Defines the entry point for the DLL application. */

#include "precomp.h"
#include "dll.h"
#define DEBUG
#include <dprintf.h>

static HINSTANCE MOD_hInstance = NULL;
extern HINSTANCE ResourceInstance;

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
   switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
         ResourceInstance = MOD_hInstance = hModule;
         dprintf("Arnold DllMain");
         break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
   }
   return TRUE;
}

