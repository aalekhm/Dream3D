#ifndef WIDGETDEF_H
#define WIDGETDEF_H

#include <windows.h>

typedef void*		H_WND;
typedef void*		H_FONT;
typedef int		L_RESULT;

#define IDC__ARROW				0x00
#define IDC__IBEAM				0x01
#define IDC__WAIT				0x02
#define IDC__CROSS				0x03
#define IDC__UPARROW			0x04
#define IDC__SIZE            		0x05
#define IDC__ICON				0x06
#define IDC__SIZENWSE			0x07
#define IDC__SIZENESW			0x08
#define IDC__SIZEWE				0x09
#define IDC__SIZENS				0x0A
#define IDC__SIZEALL			0x0B
#define IDC__NO              		0x0C


//WButton
#define WM_BTN_LBUTTONDOWN	1000
#define WM_BTN_LBUTTONUP			WM_BTN_LBUTTONDOWN + 1
#define WM_BTN_RBUTTONDOWN	WM_BTN_LBUTTONUP	 + 1
#define WM_BTN_RBUTTONUP		WM_BTN_RBUTTONDOWN + 1
#define WM_BTN_MBUTTONDOWN	WM_BTN_RBUTTONUP + 1
#define WM_BTN_MBUTTONUP		WM_BTN_MBUTTONDOWN + 1
#define WM_BTN_END					WM_BTN_MBUTTONUP + 1

//WCheckbox
#define WM_CBX_CHECKED			WM_BTN_END + 1
#define WM_CBX_UNCHECKED		WM_CBX_CHECKED + 1
#define WM_CBX_END					WM_CBX_UNCHECKED + 1		

//WComboBox
#define WM_CBN_SELENDOK			WM_CBX_END + 1
#define WM_CBN_END					WM_CBN_SELENDOK + 1

//WTextBox

/* Messages */
#define WM__GETTEXTLENGTH						0x001
#define WM__GETTEXT									0x002
#define WM__SETTEXT									0x003
#define WM__APPENDTEXT								0x004
#define WM__GETRECT									0x005
#define WM__GETPOS									0x006
#define WM__SETPOS										0x007
#define WM__GETSIZE									0x008
#define WM__SETSIZE									0x009
#define WM__SETRECT									0x00A
#define WM__CREATE									0x00B
#define WM__PAINT										0x00C

#define WM__ERR											-1
#define WM__OKAY										1

/*
 * Button messages
 */
#define BM__CLICK										0x100
#define BM__ENABLE									0x101
#define BM__DISABLE									0x102
#define BM__GET_STATE								0x103

/*
 * Combo Box messages
 */
#define CBM__ADDSTRING								0x200
#define CBM__DELETESTRING							0x201
#define CBM__FINDSTRING								0x202
#define CBM__FINDSTRINGEXACT						0x203
#define CBM__GETCOUNT								0x204
#define CBM__GETCURSEL								0x205
#define CBM__SETCURSEL								0x206
#define CB__GETDROPPEDCONTROLRECT		0x207
#define CB__GETDROPPEDSTATE					0x208
#define CB__GETDROPPEDWIDTH					0x209
#define CB__GETEDITSEL								0x20A
#define CB__SETEDITSEL								0x20B
#define CB__GETLBTEXTLEN							0x20C
#define CB__GETLBTEXT								0x20D
#define CB__INSERTSTRING							0x20E
#define CB__LIMITTEXT									0x20F//ToDo
#define CB__RESETCONTENT							0x210
#define CB__SELECTSTRING							0x211
#define CB__SETITEMDATA							0x212
#define CB__SHOWDROPDOWN						0x213

#define CB__ERR											-1
#define CB__OKAY											1

/*
 * Tabbed Pane messages
 */
typedef struct {

	LPSTR		lpzText;
	int			cchTextMax;
	LPARAM		lParam;

} TCIM_, *LPTCIM_;

#define TAB__GETCURSEL								0x300
#define TAB__SETCURSEL								0x301
#define TAB__GETITEMCOUNT						0x302
#define TAB__GETITEM									0x303
#define TAB__INSERTITEM								0x304
#define TAB__DELETEITEM							0x305
#define TAB__SETITEM									0x306
#define TAB__SETITEMSIZE							0x307
#define TAB__SETITEMTITLE							0x308

/*
 * Tree View messages
 */
#define TVM__DELETEITEM							0x400
#define TVM__EDITLABEL								0x401
#define TVM__GETCOUNT								0x402
#define TVM__GETEDITCONTROL					0x403
#define TVM__GETINDENT								0x404
#define TVM__GETITEM									0x405
#define TVM__GETITEMHEIGHT						0x406
#define TVM__GETITEMRECT							0x407
#define TVM__GETNEXTITEM							0x408
#define TVM__INSERTITEM								0x409
#define TVM__SELECTITEM							0x40A
#define TVM__SETINDENT								0x40B
#define TVM__SETITEM									0x40C
#define TVM__SETITEMHEIGHT						0x40D
#define TVM__SETLINECOLOR						0x40E
#define TVM__SETTEXTCOLOR						0x40F
#define TVM__SHOWINFOTIP							0x410
#define TVM__SORTCHILDREN						0x411

#endif