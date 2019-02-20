/* UI for archive support */
#include "zipsupui.h"
#include "mylistvw.h"
#include <commctrl.h>
#include "resource.h"
#include <tchar.h>
#include <shlwapi.h>
#include "cpcemu.h"

static file_list *theList;
static archive_file_open *theFileOpen;

/* the current path we are looking at; in terms of ZIP charset */
static char CurrentDirectory[MAX_PATH];

/* NOTE: Unsure which charset the zip filenames are in; so we do conversion for display
purposes ONLY, and for return to app */

/*--------------------------------------------------------------------------------------------*/
static HIMAGELIST ArchiveUI_GetSystemImageList(void)
{
	SHFILEINFO FileInfo;
	
	memset(&FileInfo, 0, sizeof(FileInfo));

	/* get type name and icon using the extension */
	return (HIMAGELIST)SHGetFileInfo(_T("C:\\"),
					0,
					&FileInfo,
					sizeof(FileInfo),
					SHGFI_SYSICONINDEX|SHGFI_ICON|SHGFI_SMALLICON);
}

/*--------------------------------------------------------------------------------------------*/
/* look for path separator '/' */
static void ArchiveUI_PopPath(void)
{
	unsigned long nLength = strlen(CurrentDirectory);
	unsigned long i;

	for (i=nLength-2; i!=0; i--)
	{
		char ch;

		ch = CurrentDirectory[i];

		if (ch=='/')
		{
			/* terminate string */
			i++;
			break;
		}
	}
	CurrentDirectory[i]='\0';
}
/*--------------------------------------------------------------------------------------------*/
static void ArchiveUI_GetDisplayName(const char *sName, char *sDisplayName)
{
	int nStart;
	int nEnd;
	int nLength;

	nEnd = strlen(sName);
	nStart = 0;

	if (sName[nEnd-1]=='/')
	{
		nEnd--;
	}

	for (nStart=nEnd-1; nStart!=0; nStart--)
	{
		if (sName[nStart]=='/')
		{
			nStart++;
			break;
		}
	}

	nLength = nEnd-nStart;

	strncpy(sDisplayName, &sName[nStart], nLength);
	sDisplayName[nLength]='\0';
}

/*--------------------------------------------------------------------------------------------*/
/* zlib holds the full path of any files and directories. Directories are stored with a final '/'
separator */
static BOOL ArchiveUI_IsDirectory(const char *sName)
{
	unsigned long nLength = strlen(sName);
	if (nLength!=0)
	{
		if (sName[nLength-1]=='/')
			return TRUE;
	}
	return FALSE;
}

/*--------------------------------------------------------------------------------------------*/
/* return true if item is in current directory */
static BOOL ArchiveUI_IsItemInCurrentDirectory(const char *sItem, const char *sCurrentDirectory)
{
	int nItemStringLen = strlen(sItem);
	int nDirectoryStringLen = strlen(sCurrentDirectory);
	int  nItemDirStringLen;
	int nStart;

	/* if length of item string is less than length of directory string then
	can't be in current directory, because if it was in current directory
	the item string should be at least the length of the current directory string */
	if (nItemStringLen<nDirectoryStringLen)
		return FALSE;

	nStart = nItemStringLen-1;

	/* if the item specifies a directory then there will be a final slash, start looking for
	slash before that */
	if (sItem[nItemStringLen-1]=='/')
		nStart--;

	nItemDirStringLen = 0;

	/* find next slash beginning from end of string */
	for (;nStart>=0; nStart--)
	{
		if (sItem[nStart]=='/')
		{
			nItemDirStringLen = nStart+1;
			break;
		}
	}

	/* must be of same length */
	if (nItemDirStringLen!=nDirectoryStringLen)
		return FALSE;

	/* do actual memory comparison */
	return (strncmp(sItem, sCurrentDirectory, nItemDirStringLen)==0);
}

/*--------------------------------------------------------------------------------------------*/
static void ArchiveUI_AddItem(HWND hListView, const char *sName, DWORD nSize, DWORD dwAttributes, LPVOID pData)
{
	SHFILEINFO FileInfo;
	TCHAR SizeBuffer[64];
	/* get extension */
	LPCTSTR sExtension;
	int nItem;
	char sDisplayName[MAX_PATH];
	int iIcon;
	TCHAR sDisplayNameUI[MAX_PATH];
#ifdef _UNICODE
	const TCHAR *sUnicode;
#endif

	ArchiveUI_GetDisplayName(sName, sDisplayName);

#ifdef _UNICODE
	/* convert to unicode for display */
	sUnicode = ConvertMultiByteToUnicode(sDisplayName);
	if (sUnicode)
	{
		/* copy to buffer */
		_tcsncpy(sDisplayNameUI,sUnicode, MAX_PATH);
		/* free */
		free((void *)sUnicode);
	}
#else
	strcpy(sDisplayNameUI, sDisplayName);
#endif

	/* get display name and icon */
	memset(&FileInfo, 0, sizeof(FileInfo));

	if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0)
	{
		SHGetFileInfo(_T("Dummy"),
						FILE_ATTRIBUTE_DIRECTORY,
						&FileInfo,
						sizeof(FileInfo),
						SHGFI_USEFILEATTRIBUTES|SHGFI_SYSICONINDEX|SHGFI_ICON|SHGFI_SMALLICON);
	
		iIcon = FileInfo.iIcon;
	}
	else
	{
		SHGetFileInfo(sDisplayNameUI,
						dwAttributes,
						&FileInfo,
						sizeof(FileInfo),
						SHGFI_USEFILEATTRIBUTES|SHGFI_SYSICONINDEX|SHGFI_ICON|SHGFI_SMALLICON);
	
		iIcon = FileInfo.iIcon;
	}

	SHGetFileInfo(sDisplayNameUI,
					dwAttributes,
					&FileInfo,
					sizeof(FileInfo),
					SHGFI_USEFILEATTRIBUTES|SHGFI_DISPLAYNAME);


	nItem = ListView_GetItemCount(hListView);

	/* set text for first column */
	MyListView_AddItemWithIcon(hListView, FileInfo.szDisplayName,0, nItem, pData, FileInfo.iIcon);

	if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY)==0)
	{
		SizeBuffer[0] = _T('\0');
		StrFormatByteSize(nSize, SizeBuffer, (sizeof(SizeBuffer)/sizeof(TCHAR)));

		/* set text for second column */
		MyListView_AddItem(hListView, SizeBuffer, 1, nItem, NULL);

		/* get pointer to extension */
		sExtension = PathFindExtension(sDisplayNameUI);
	}
	else
	{
		/* must have some text for the name, otherwise Windows
		gives wrong type name */
		sExtension = _T("Dummy");
	}

	/* get type name */
	memset(&FileInfo, 0, sizeof(FileInfo));
	SHGetFileInfo(sExtension,
					dwAttributes,
					&FileInfo,
					sizeof(FileInfo),
					SHGFI_USEFILEATTRIBUTES|SHGFI_TYPENAME);

	/* set text for third column */
	MyListView_AddItem(hListView, FileInfo.szTypeName, 2, nItem, NULL);
}

/*--------------------------------------------------------------------------------------------*/
/* the list of files returned by the zlib library are all fully qualified and may not
be in alphabetical order. We parse the list to extract only those files in the "current" directory */
static void ArchiveUI_UpdateList(HWND hListView, const char *sDirectory)
{
	file_list_linked_item *current;

	/* set wait cursor */
	HCURSOR hPreviousCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

	/* disable redrawing of the image list; faster to fill */
	SendMessage(hListView,WM_SETREDRAW,(WPARAM)FALSE, (LPARAM)0);

	/* collect all items into list view; then sort */
	ListView_DeleteAllItems(hListView);

	/* add "back" item */
	if (strlen(sDirectory)!=0)
	{
		ArchiveUI_AddItem(hListView, "..", 0, FILE_ATTRIBUTE_DIRECTORY, 0);
	}
	current = theList->first;

	while (current!=NULL)
	{
		const file_list_item *item = &current->item;

		if (ArchiveUI_IsItemInCurrentDirectory(item->name, CurrentDirectory))
		{
			ArchiveUI_AddItem(hListView, item->name, item->nSize, item->nAttributes,(LPVOID)item);
		}

		current = current->next;
	}

	SendMessage(hListView,WM_SETREDRAW,(WPARAM)TRUE, (LPARAM)0);

	InvalidateRect(hListView, NULL, TRUE);

	/* remove wait cursor */
	if (hPreviousCursor)
	{
		SetCursor(hPreviousCursor);
	}
}


/*--------------------------------------------------------------------------------------------*/
static void ArchiveUI_SetChosenFilename(HWND hListView, int nIndex)
{
	/* got selected item */
	LVITEM lvitem;

	lvitem.mask = LVIF_PARAM;
	lvitem.iItem = nIndex;
	lvitem.iSubItem = 0;

	if (ListView_GetItem(hListView, &lvitem))
	{
		/* got the item */
		file_list_item *item = (file_list_item *)lvitem.lParam;

		/* no conversion here, use zip charset */
		theFileOpen->pFilename = item->name;
	}
}


/*--------------------------------------------------------------------------------------------*/

static BOOL CALLBACK  ArchiveUI_DialogProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
        case WM_INITDIALOG:
		{
			/* list view is report style, and shares image lists. */
			/* If the list view didn't share the image list, then it would
			delete it when it was destroyed. We don't want that as we are
			using the system image list */
			HWND hListView = GetDlgItem(hwnd, IDC_LIST_FILES);
			HIMAGELIST hImageList;
	
			/* create columns */
			MyListView_AddColumn(hListView, _T("Name"), 0);
			MyListView_AddColumn(hListView, _T("Size"), 0);
			MyListView_AddColumn(hListView, _T("Type"), 0);

			/* get the system image list; contains icons for files etc */
			hImageList = ArchiveUI_GetSystemImageList();

			ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT);

			/* set the image list for the list view */
			ListView_SetImageList(hListView, hImageList,LVSIL_NORMAL);
			ListView_SetImageList(hListView, hImageList,LVSIL_SMALL);

			ArchiveUI_UpdateList(hListView, CurrentDirectory);
			
            return TRUE;
		}

		case WM_NOTIFY:
		{
			int idCtrl = (int)wParam;
			LPNMHDR pNMHDR = (LPNMHDR)lParam;

			if (idCtrl == IDC_LIST_FILES)
			{
				HWND hListView = GetDlgItem(hwnd, IDC_LIST_FILES);

				/* notify from the list view */
				if (pNMHDR->code == NM_DBLCLK)
				{
					LPNMLISTVIEW pNMListView = (LPNMLISTVIEW )pNMHDR;
					/* double click with left button? */

					/* find item hit */
					LVHITTESTINFO HitTest;

					memset(&HitTest, 0, sizeof(HitTest));
					HitTest.pt = pNMListView->ptAction;
					if (ListView_HitTest(hListView,&HitTest)!=-1)
					{
						LVITEM Item;

						/* get the item's data */
						Item.mask = LVIF_PARAM;
						Item.iItem = HitTest.iItem;
						Item.iSubItem = 0;

						if (ListView_GetItem(hListView,&Item))
						{
							if (Item.lParam == 0)
							{
								/* go back up a directory */
								ArchiveUI_PopPath();
								ArchiveUI_UpdateList(hListView, CurrentDirectory);
							}
							else
							{
								file_list_item *item = (file_list_item *)Item.lParam;

								if ((item->nAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0)
								{
									/* set this as the new directory */
									strcpy(CurrentDirectory,item->name);
									ArchiveUI_UpdateList(hListView, CurrentDirectory);
								}
								else
								{
									ArchiveUI_SetChosenFilename(hListView, HitTest.iItem);
									EndDialog(hwnd, TRUE);
								}
							}
						}
					}
				}
			}
		}
		break;


		case WM_COMMAND:
		{
			switch LOWORD(wParam)
	        {
				case IDOK:
				{
					int Index;
					HWND hListView = GetDlgItem(hwnd, IDC_LIST_FILES);
					/* get first selected item */
					Index = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);

					if (Index==-1)
					{
						/* if there are no selected items then display an error */
						MessageBox(hwnd,_T("No items are selected"),_T("Arnold"), MB_ICONEXCLAMATION|MB_OK);
					}
					else
					{
						ArchiveUI_SetChosenFilename(hListView, Index);
						EndDialog(hwnd, TRUE);
					}
				}
				return TRUE;

				case IDCANCEL:
				{
					EndDialog(hwnd, FALSE);
				}
				return TRUE;



				default:
					break;
			}
		}
		break;

		case WM_DESTROY:
		{
			HWND hListView = GetDlgItem(hwnd, IDC_LIST_FILES);

			SendMessage(hListView,WM_SETREDRAW,(WPARAM)FALSE, (LPARAM)0);

			ListView_DeleteAllItems(hListView);
			
			SendMessage(hListView,WM_SETREDRAW,(WPARAM)FALSE, (LPARAM)0);
		}
		break;	

	default:
		break;
	
	}
	
	return FALSE;
}

/*--------------------------------------------------------------------------------------------*/
BOOL	ArchiveUI_Show(HWND hParent, file_list *fileList, archive_file_open *open)
{
	/* get instance handle */
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);
	file_list_linked_item *current;

	theList = fileList;
	theFileOpen = open;

	CurrentDirectory[0] = _T('\0');

	/* mark directories */
	current = theList->first;

	while (current!=NULL)
	{
		file_list_item *item = &current->item;

		DWORD dwAttributes = FILE_ATTRIBUTE_NORMAL;

		/* is this a directory? */
		if (ArchiveUI_IsDirectory(item->name))
		{
			dwAttributes = FILE_ATTRIBUTE_DIRECTORY;
		}

		item->nAttributes = dwAttributes;

		current = current->next;
	}


	/* create and show the archive dialog */
	return DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG_ARCHIVE),hParent,ArchiveUI_DialogProc);
}