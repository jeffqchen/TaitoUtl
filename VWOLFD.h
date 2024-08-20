// VWOLFD.h - include file for VxD VWOLFD

//===========================================
// For building VxD driver
//===========================================
#ifdef BUILD_VXD

#include <basedef.h>
#include <vmm.h>
#include <vwin32.h>

#endif
//==========================================
// Public definitions
//==========================================

// Device details
#define CPLD_WATCHDOG		0x000CB201	// Address to kick the arcade watchdog 
#define CPLD_VIDEOSWITCH	0x000CB600	// Address to switch JAMMA video output
										// and prevent reset action every 64 seconds
#define DOGFOOD				0xFF		// Data to write into the watchdog.
#define DOG_INTERVAL		1000		// Watchdog refresh interval in ms
#define VIDEO_SPLASH		0x00		// Video mode that shows a static red splash screen
#define VIDEO_GRID			0x02		// Video mode that shows a grid pattern

#define VIDEO_VOODOO_LOGO	0x30		// Video mode when monw is showing voodoo splash
#define VIDEO_MONW_GAME		0x3D		// Video mode during monw normal gameplay
#define VIDEO_MONW_TEST		0x31		// Video mode during monw test mode

// Device message commands
#define VWOLFD_Get_Version	0xFF10
#define VWOLFD_Dog_Spawn	0xFF09

#define VWOLFD_Dog_Toggle	0xFF06
#define VWOLFD_Dog_Enable	0xFF07
#define VWOLFD_Dog_Disable	0xFF08

#define VWOLFD_Video_Splash	0xFF00
#define VWOLFD_Video_Voodoo	0xFF01
#define VWOLFD_Video_Grid	0xFF02

#define VWOLFD_Poll_State	0xFF03
#define VWOLFD_Set_State	0xFF04

// Data types for driver communication
typedef struct versionNumber {
	int	major;
	int	minor;
} VERNUM;

typedef VERNUM *lpVerNum;

typedef struct wolfState {
	BOOL	dogState;
	BYTE	videoMode;
} WOLFSTATE;

typedef WOLFSTATE *lpWolfState;

//===========================================
// For building VxD driver
//===========================================
#ifdef BUILD_VXD

//Device properties
#define VWOLFD_Major		1
#define VWOLFD_Minor		0
#define VWOLFD_DeviceID		TAITO_WOLF_WATCHDOG
#define VWOLFD_Init_Order	UNDEFINED_INIT_ORDER


typedef DIOCPARAMETERS *LPDIOC;

// Global variables
BYTE	* const	dogHouse;	//Address to feed the watchdog
BYTE	* const	videoSwitch;//Address to change the video output on JAMMA edge

BOOL	enableWatchdog;
int		videoMode;			// Video mode

HTIMEOUT dogTimer;			//TimeOut handle for watchdog thread

// Function prototypes
void	_cdecl		watchdogThread ();

void	_stdcall	vwolfdGetVersion (LPDIOC);

void	_stdcall	vwolfdDogSpawn ();
void	_stdcall	vwolfdDogEnable ();
void	_stdcall	vwolfdDogDisable ();
void	_stdcall	vwolfdDogToggle ();

void	_stdcall	vwolfdVideoSplash ();
void	_stdcall	vwolfdVideoVoodoo ();
void	_stdcall	vwolfdVideoGrid ();

void	_stdcall	vwolfdPollState	(lpDIOCParms);
void	_stdcall	vwolfdSetState	(lpDIOCParms);

#endif

