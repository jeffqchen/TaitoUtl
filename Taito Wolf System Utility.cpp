// Taito Wolf System Utility.cpp : Defines the entry point for the application.
//
#ifdef _DEBUG
	#define _DEBUGDOG
#endif

#include "stdafx.h"
#include "Taito Wolf System Utility.h"

#include "wolflib.h"
#include "wolfVxDComm.h"


#define MAX_LOADSTRING 100
#define WM_USER_SHELLICON WM_USER + 1

#define ICONUPDATEINTERVAL 1000
// Win32 functions
ATOM registerClass(HINSTANCE hInstance);
BOOL InitAppInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//App functions
void iconUpdateThread (void *ignore);
void userDogThread (void *ignore);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Splash(HWND, UINT, WPARAM, LPARAM);
void CALLBACK SplashTimer(HWND, UINT, WPARAM, LPARAM);

// Win32 variables
HINSTANCE hInst;
NOTIFYICONDATA wolfApp;
HMENU hPopMenu;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
TCHAR szApplicaionToolTip[MAX_LOADSTRING];
HWND hSplashDlg;
HWND hAbtDlg;
HBRUSH hbrush = GetSysColorBrush(COLOR_3DFACE);
UINT timerHandle;
HICON hTrayIcon;

// App functionality variables
BOOL iconUpdateThreadAlive = TRUE;
BOOL userDogThreadAlive = FALSE;

int appMode = 0;	// Application running mode
				//	0: ring 3 user mode
				//	1: VxD mode



int APIENTRY _tWinMain(	HINSTANCE	hInstance,
						HINSTANCE	hPrevInstance,
						LPSTR		lpCmdLine,
						int			nCmdShow)
{
#ifdef _DEBUG
OutputDebugString(">>> _tWinMain()\n");
#endif

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
OutputDebugString("Detecting VxD.\n");
#endif

	// Initialize VxD handle and app mode 
	if (VXD_FAILURE == WolfVxDOpen()) // VxD not detected, run in user mode
	{

#ifdef _DEBUG
OutputDebugString("VxD not detected.\n");
#endif

		appMode = 0;	//	0: ring 3 user mode

		//Enable watchdog for the first time
		userDogThreadAlive = TRUE;
		twEnableDog();

#ifdef _DEBUG
OutputDebugString("Spawning user dog thread.\n");
#endif

		//Start user dog thread
		_beginthread(userDogThread, 0, NULL);

		Sleep(500);

	} else {

#ifdef _DEBUG
OutputDebugString("VxD detected.\n");
#endif

		appMode = 1;	//	1: VxD mode
		WolfVxDInit();
	}

	twSwitchVideoMode(1);	//Set video output to Voodoo

	//=================== Register instance class ===================
	MSG msg;

	LoadString(hInstance, IDS_WOLFTRAY, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

#ifdef _DEBUG
OutputDebugString("Register class.\n");
#endif

	registerClass(hInstance);

	if (!InitAppInstance (hInstance, nCmdShow))
	{

#ifdef _DEBUG
OutputDebugString("Failed to register class.\n");
#endif

		return FALSE;
	}

	_beginthread(iconUpdateThread, 0, NULL);

#ifdef _DEBUG
OutputDebugString("Entering message loop.\n");
#endif

	//=================== Main message loop ===================
	while (GetMessage(&msg, NULL, 0,0))
	{
		DispatchMessage(&msg);
	}

#ifdef _DEBUG
OutputDebugString("Entering end of life.\n");
#endif

	//=================== End of app lifetime ===================
	//Switch back to splash and disble dog

	if (0 == appMode)
	{
		twDisableDog();
		twSwitchVideoMode(0);
		
		//Tell dog thread it's time to end
		userDogThreadAlive = FALSE;

		DeleteObject(hbrush);

	} else {
		WolfVxDCleanup();
	}

	iconUpdateThreadAlive = FALSE;
	Sleep (1500);

#ifdef _DEBUG
OutputDebugString("Bye.\n");
#endif

	return (int) msg.wParam;
}

void iconUpdateThread (void *ignore)
{
#ifdef _DEBUG
OutputDebugString(">>> iconUpdateThread()\n");
#endif

	while (TRUE == iconUpdateThreadAlive)
	{
		if (FALSE == twPollDogState())
		{

#ifdef _DEBUGDOG
OutputDebugString(">>>DOG>>> Dog disabled.\n");
#endif

			hTrayIcon = LoadIcon(hInst, (LPCTSTR)MAKEINTRESOURCE(IDI_TRAYGRAY));
			wolfApp.hIcon = hTrayIcon;

			LoadString (hInst, IDS_DOGDISABLED, wolfApp.szTip, MAX_LOADSTRING);

		} else {

#ifdef _DEBUGDOG
OutputDebugString(">>>DOG>>> Dog enabled.\n");
#endif
	
			hTrayIcon = LoadIcon(hInst, (LPCTSTR)MAKEINTRESOURCE(IDI_TRAY));
			wolfApp.hIcon = hTrayIcon;

			switch (twPollVideoMode())
			{
			case 0:

#ifdef _DEBUGDOG
OutputDebugString(">>>DOG>>> Splash detected.\n");
#endif

				LoadString (hInst, IDS_SPLASH, wolfApp.szTip, MAX_LOADSTRING);
				hTrayIcon = LoadIcon(hInst, (LPCTSTR)MAKEINTRESOURCE(IDI_TRAYRED));

				break;

			case 1:
				
#ifdef _DEBUGDOG
OutputDebugString(">>>DOG>>> Voodoo detected.\n");
#endif
				LoadString (hInst, IDS_VOODOO, wolfApp.szTip, MAX_LOADSTRING);
				hTrayIcon = LoadIcon(hInst, (LPCTSTR)MAKEINTRESOURCE(IDI_TRAYBLUE));
				break;

			case 2:
				
#ifdef _DEBUGDOG
OutputDebugString(">>>DOG>>> Grid detected.\n");
#endif
				LoadString (hInst, IDS_TEST, wolfApp.szTip, MAX_LOADSTRING);
				hTrayIcon = LoadIcon(hInst, (LPCTSTR)MAKEINTRESOURCE(IDI_TRAYELLOW));
				break;
			}
		}

#ifdef _DEBUGDOG
OutputDebugString(">>>DOG>>> Update icon.\n");
#endif
		wolfApp.hIcon = hTrayIcon;
		Shell_NotifyIcon(NIM_MODIFY, &wolfApp);
		DestroyIcon(wolfApp.hIcon);

		Sleep(ICONUPDATEINTERVAL);
	}

#ifdef _DEBUG
OutputDebugString("Ending dog thread.\n");
#endif

	_endthread();
	return;
}

void userDogThread (void *ignore)
{
#ifdef _DEBUG
OutputDebugString(">>> userDogThread()\n");
#endif

	while (TRUE == userDogThreadAlive)
	{
		twPetDog();
	}

#ifdef _DEBUG
OutputDebugString("Ending dog thread.\n");
#endif


	_endthread();
	return;
}

ATOM registerClass (HINSTANCE hInstance)
{
#ifdef _DEBUG
OutputDebugString(">>> registerClass()\n");
#endif

	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof (WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ICON1));
	wcex.hCursor		= LoadCursor (NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= MAKEINTRESOURCE (IDC_WOLFTRAY);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE (IDI_TRAY));

	return RegisterClassEx(&wcex);
}

BOOL InitAppInstance(HINSTANCE hInstance, int nCmdShow)
{
#ifdef _DEBUG
OutputDebugString(">>> InitAppInstance()\n");
#endif

	HWND hWnd;

	hInst = hInstance;

	hWnd = CreateWindow (szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	
	if (!hWnd)
	{

#ifdef _DEBUG
OutputDebugString("Failed to create main window.\n");
#endif
		return FALSE;
	}

	hTrayIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_ICON1));

	wolfApp.cbSize = sizeof(NOTIFYICONDATA);
	wolfApp.hWnd = (HWND) hWnd;
	wolfApp.uID = IDI_TAITOWOLF;
	wolfApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	wolfApp.hIcon = hTrayIcon;
	wolfApp.uCallbackMessage = WM_USER_SHELLICON;

	LoadString (hInstance, IDS_APPTOOLTIP, wolfApp.szTip, MAX_LOADSTRING);
	Shell_NotifyIcon(NIM_ADD, &wolfApp);
	DestroyIcon(wolfApp.hIcon);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	POINT lpClickPoint;

	UINT uFlag1, uFlag2, uFlag3;

	switch (message)
	{
		case WM_CREATE: // Show splash screen for 3 seconds

#ifdef _DEBUG
OutputDebugString("Showing startup splash screen.\n");
#endif

			hSplashDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SPLASH), hWnd, (DLGPROC)Splash);
			ShowWindow (hSplashDlg, SW_SHOW);
			SetForegroundWindow (hSplashDlg);
			SetTimer (hSplashDlg, timerHandle, 3000, (TIMERPROC)Splash);
			break;

		case WM_USER_SHELLICON:

			switch(LOWORD(lParam))
			{
			case WM_LBUTTONDBLCLK:

#ifdef _DEBUG
OutputDebugString("Showing splash screen.\n");
#endif

				ShowWindow (hSplashDlg, SW_SHOW);
				SetForegroundWindow (hSplashDlg);
				SetTimer (hSplashDlg, timerHandle, 3000, (TIMERPROC)Splash);
				break;

			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:

#ifdef _DEBUG
OutputDebugString("Show menu.\n");
#endif

				uFlag1 = MF_BYPOSITION | MF_STRING;
				uFlag2 = MF_BYPOSITION | MF_STRING;
				uFlag3 = MF_BYPOSITION | MF_STRING;

				GetCursorPos (&lpClickPoint);
				hPopMenu = CreatePopupMenu();
				
				InsertMenu (hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING | MF_DISABLED, ID_WOLFTRAYMENU_OUTPUT, _T("==== Output ===="));
				
				if (twPollDogState() == FALSE)
				{
					uFlag1 |= MF_GRAYED;
					uFlag2 |= MF_GRAYED;
					uFlag3 |= MF_GRAYED;
				}

				switch(twPollVideoMode())
				{
				case 0:
					uFlag2 |= MF_CHECKED;
					break;
				case 1:
					uFlag1 |= MF_CHECKED;
					break;
				case 2:
					uFlag3 |= MF_CHECKED;
					break;
				}
				InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag1, IDM_VOODOO, _T("&Voodoo"));
				InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag2, IDM_SPLASH, _T("&Splash Screen"));
				InsertMenu(hPopMenu, 0xFFFFFFFF, uFlag3, IDM_TEST, _T("&Test Pattern"));

				InsertMenu (hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));

				if (twPollDogState() == FALSE)
				{
					InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_TOGGLEDOG, _T("Enable &Watchdog"));
				} else {
					InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_TOGGLEDOG, _T("Disable &Watchdog"));
				}

				InsertMenu (hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));

				InsertMenu (hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_ABOUT, _T("&About"));
				InsertMenu (hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("E&xit"));

				SetForegroundWindow (hWnd);
				TrackPopupMenu (hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);

				return TRUE;
			}
			break;

		case WM_COMMAND:

#ifdef _DEBUG
OutputDebugString("Process menu command.\n");
#endif

			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);

			switch (wmId)
			{
			case IDM_VOODOO:

#ifdef _DEBUG
OutputDebugString("Switch to Voodoo.\n");
#endif

				twSwitchVideoMode(1);
				break;
			case IDM_SPLASH:

#ifdef _DEBUG
OutputDebugString("Switch to splash.\n");
#endif

				twSwitchVideoMode(0);
				break;
			case IDM_TEST:

#ifdef _DEBUG
OutputDebugString("Switch to grid.\n");
#endif

				twSwitchVideoMode(2);
				break;
			case IDM_TOGGLEDOG:

#ifdef _DEBUG
OutputDebugString("Toggle dog state.\n");
#endif

				twToggleDog();
				break;
			case IDM_ABOUT:

#ifdef _DEBUG
OutputDebugString("Show About window.\n");
#endif

				if (!IsWindow(hAbtDlg))
				{

#ifdef _DEBUG
OutputDebugString("About window doesn't exist, spawning new one.\n");
#endif

					hAbtDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, (DLGPROC)About);
				}
				ShowWindow (hAbtDlg, SW_SHOW);
				SetForegroundWindow (hAbtDlg);
				break;
			case IDM_EXIT:

#ifdef _DEBUG
OutputDebugString("Execute exit.\n");
#endif

				Shell_NotifyIcon (NIM_DELETE, &wolfApp);
				DestroyWindow (hWnd);
				break;
			default:
				return DefWindowProc (hWnd, message, wParam, lParam);
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc (hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
		if (0 == appMode)	//ring3 mode
		{

#ifdef _DEBUG
OutputDebugString("In user mode, show red text.\n");
#endif

			HWND textHndl = GetDlgItem(hDlg, IDC_NOVXDA);
			ShowWindow (textHndl, SW_SHOW);
		} else {

#ifdef _DEBUG
OutputDebugString("In VxD mode, don't show red text.\n");
#endif

			HWND textHndl = GetDlgItem(hDlg, IDC_NOVXDA);
			ShowWindow (textHndl, SW_HIDE);
		}
		return (INT_PTR)TRUE;

	case WM_CTLCOLORSTATIC:

		if (GetDlgItem(hDlg, IDC_NOVXDA) == (HWND)lParam)
		{

#ifdef _DEBUG
OutputDebugString("Create hbrush for red text.\n");
#endif
			HDC hdc = (HDC)wParam;
			SetTextColor(hdc ,RGB(200,0,0));
			SetBkColor(hdc, (COLORREF) hbrush);
			SetBkMode(hdc, TRANSPARENT);
			return (COLORREF) hbrush;
		}
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{

#ifdef _DEBUG
OutputDebugString("Close About dialog.\n");
#endif

			EndDialog (hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Splash (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		if (0 == appMode)	//ring3 mode
		{

#ifdef _DEBUG
OutputDebugString("In user mode, show red text.\n");
#endif

			HWND textHndl = GetDlgItem(hDlg, IDC_NOVXDS);
			ShowWindow (textHndl, SW_SHOW);
		} else {

#ifdef _DEBUG
OutputDebugString("In VxD mode, don't show red text.\n");
#endif

			HWND textHndl = GetDlgItem(hDlg, IDC_NOVXDS);
			ShowWindow (textHndl, SW_HIDE);
		}
		return (INT_PTR)TRUE;

	case WM_CTLCOLORSTATIC:
		if (GetDlgItem(hDlg, IDC_NOVXDS) == (HWND)lParam)
		{

#ifdef _DEBUG
OutputDebugString("Create hbrush for red text.\n");
#endif

			HDC hdc = (HDC)wParam;
			SetTextColor(hdc ,RGB(200,0,0));
			SetBkColor(hdc, (COLORREF) hbrush);
			SetBkMode(hdc, TRANSPARENT);
			return (COLORREF) hbrush;
		}
		break;

	case WM_TIMER:
		{

#ifdef _DEBUG
OutputDebugString("Time to close splash window.\n");
#endif

			EndDialog (hDlg, 0);
			KillTimer (hDlg, timerHandle);
		}
		break;
	}
	return (INT_PTR)FALSE;
}
