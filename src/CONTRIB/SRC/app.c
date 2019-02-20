/* app.c */

#include "../../win/precomp.h"
#include "..\include\emushell.h"

extern const EMUAPI* EMUSHELL_ArnoldInternal(void);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int iCmdShow)
{
   /* Run Arnold the normal way */
   if (0)
   {
      ArnoldMain(hInstance, iCmdShow, __argc, __targv);
   }
   /*MessageBox(NULL, pCmdLine, "", MB_OK);*/
   /* Run Arnold like CPC Loader does */
   if (1)
   {
      EMUHANDLE handle = (EMUSHELL_ArnoldInternal()->create)(NULL);
      if (handle)
      {
         (*handle->api->run)(handle, NULL, __argc, __targv);
         (*handle->api->close)(&handle);
      }
   }
}

