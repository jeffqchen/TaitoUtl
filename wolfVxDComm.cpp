#ifdef _DEBUG
	#define DEBUGVXD
#endif

#include "wolfVxdComm.h"

HANDLE hVxD = 0;
wolfState myWolfState = {FALSE, 0x0};
DWORD cbRet;

RESULT BOOL WolfVxDOpen ()
{
	hVxD = CreateFile ("\\\\.\\VWOLFD",							//lpFileName
						(GENERIC_READ | GENERIC_WRITE),			//dwDesiredAccess
						(FILE_SHARE_READ | FILE_SHARE_WRITE),	//dwShareMode,
						NULL,									//lpSecurityAttributes
						OPEN_EXISTING,							//dwCreationDisposition
						FILE_FLAG_DELETE_ON_CLOSE,				//dwFlagsAndAttributes
						NULL);									//hTemplateFile

	if (INVALID_HANDLE_VALUE == hVxD)
	{
		return VXD_FAILURE;
	}

	return VXD_SUCCESS;
}

void WolfVxDInit ()
{
	DeviceIoControl (IN		hVxD,	//Handle
					 IN		VWOLFD_Dog_Spawn,	//dwIoControlCode
					 IN		NULL,	//lpInBuffer
					 IN		0,		//nInBufferSize
					 OUT	NULL,	//lpOutBuffer
					 IN		0,		//nOutBufferSize
					 OUT	0,		//lpBytesReturned
					 IN		NULL);	//lpOverlapped
	return;
}

void WolfVxDCleanup ()
{
	WolfVxDVideoGrid ();
	CloseHandle(hVxD);
	return;
}

void WolfVxDDogEnable ()
{
	DeviceIoControl (hVxD,
					 VWOLFD_Dog_Enable,
					 IN		NULL,
					 IN		0,
					 OUT	NULL,
					 IN		0,
					 OUT	0,
					 IN		NULL);
	return;

}

void WolfVxDDogDisable ()
{
	DeviceIoControl (hVxD,
					 VWOLFD_Dog_Disable,
					 IN		NULL,
					 IN		0,
					 OUT	NULL,
					 IN		0,
					 OUT	0,
					 IN		NULL);
	return;
}

void WolfVxDDogToggle ()
{
	DeviceIoControl (hVxD,
					 VWOLFD_Dog_Toggle,
					 IN		NULL,
					 IN		0,
					 OUT	NULL,
					 IN		0,
					 OUT	0,
					 IN		NULL);
	return;
}

void WolfVxDVideoSplash ()
{
	DeviceIoControl (hVxD,
					 VWOLFD_Video_Splash,
					 IN		NULL,
					 IN		0,
					 OUT	NULL,
					 IN		0,
					 OUT	0,
					 IN		NULL);
	return;
}

void WolfVxDVideoVoodoo ()
{
	DeviceIoControl (hVxD,
					 VWOLFD_Video_Voodoo,
					 IN		NULL,
					 IN		0,
					 OUT	NULL,
					 IN		0,
					 OUT	0,
					 IN		NULL);
	return;
}

void WolfVxDVideoGrid ()
{
	DeviceIoControl (hVxD,
					 VWOLFD_Video_Grid,
					 IN		NULL,
					 IN		0,
					 OUT	NULL,
					 IN		0,
					 OUT	0,
					 IN		NULL);
	return;
}

void WolfVxDPollState ()
{
	DeviceIoControl (hVxD,
					 VWOLFD_Poll_State,
					 (LPVOID)NULL,
					 0,
					 (lpWolfState)&myWolfState,
					 2,
					 &cbRet,
					 NULL);
}