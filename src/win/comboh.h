#ifndef __COMBO_BOX_HELPER__
#define __COMBO_BOX_HELPER__

/* combo box helper macros */

/* get number of items in combo box */
#define ComboBoxHelper_GetCount(hwnd) \
	SendMessage(hwnd, CB_GETCOUNT, (WPARAM)0,(LPARAM)0)

/* set data for item */
#define ComboBoxHelper_SetItemData(hwnd, index, data) \
	SendMessage(hwnd, CB_SETITEMDATA, (WPARAM)index, (LPARAM)data)

/* insert a string at end of combo list */
#define ComboBoxHelper_InsertString(hwnd,index,string) \
	SendMessage(hwnd, CB_INSERTSTRING, (WPARAM)index, (LPARAM)(LPCTSTR)string)

/* get index of current selection in combo box */
#define ComboBoxHelper_GetCurSel(hwnd) \
	SendMessage(hwnd, CB_GETCURSEL, (WPARAM)0,(LPARAM)0)

/* get index of current selection in combo box */
#define ComboBoxHelper_SetCurSel(hwnd,index) \
	SendMessage(hwnd, CB_SETCURSEL, (WPARAM)index, (LPARAM)0)

#endif