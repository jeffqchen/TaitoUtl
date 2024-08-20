#ifdef _DEBUG
	#define DEBUGWOLF
#endif

#include "wolfVxDComm.h"
#include "wolflib.h"

BYTE * const twVideoSwitch	= (BYTE *) CPLD_VIDEOSWITCH;
BYTE * const twWatchdog		= (BYTE *) CPLD_WATCHDOG;

BOOL enableWatchdog = TRUE;
extern wolfState myWolfState;

void twPetDog ()	//Only used by ring3 mode
{
#ifdef DEBUGWOLF
OutputDebugString(">>>WOLF>>> twPetDog()\n");
#endif

	if (enableWatchdog == TRUE)
	{
		
#ifdef DEBUGWOLF
OutputDebugString(">>>WOLF>>> Pet dog\n");
#endif

		*twWatchdog = DOGFOOD;
	}

	Sleep(DOG_INTERVAL);
	return;
}

void twToggleDog()
{
	
#ifdef DEBUGWOLF
OutputDebugString(">>>WOLF>>> twToggleDog()\n");
#endif

	if(0 == appMode)
	{
		enableWatchdog = !enableWatchdog;
	} else {
		WolfVxDDogToggle();
	}
	return;
}

void twEnableDog()
{

#ifdef DEBUGWOLF
OutputDebugString(">>>WOLF>>> twEnableDog()\n");
#endif

	if(0 == appMode)
	{
		enableWatchdog = TRUE;
	} else {
		WolfVxDDogEnable();
	}
	return;
}

void twDisableDog()
{
	
#ifdef DEBUGWOLF
OutputDebugString(">>>WOLF>>> twDisableDog()\n");
#endif
	if(0 == appMode)
	{
		enableWatchdog = FALSE;
	} else {
		WolfVxDDogDisable();
	}
	return;
}

BOOL twPollDogState ()
{
#ifdef DEBUGWOLF
OutputDebugString(">>>WOLF>>> twPollDogState()\n");
#endif

	if(0 == appMode)
	{
		return enableWatchdog;
	} else
	{
		WolfVxDPollState();
		return myWolfState.dogState;
	}
}

int twSwitchVideoMode (int mode)
{
	
#ifdef DEBUGWOLF
OutputDebugString(">>>WOLF>>> twSwitchVideoMode()\n");
#endif

	if(0 == appMode)
	{
		switch (mode) {
			case 0:
				*twVideoSwitch = VIDEO_SPLASH;		//Default splash screen
				return 0;
			case 1:
				*twVideoSwitch = VIDEO_MONW_GAME;		//Voodoo output
				return 0;
			case 2:
				*twVideoSwitch = VIDEO_GRID;		//Grid test mode
				return 0;
			default:
				*twVideoSwitch = VIDEO_SPLASH;		//Invalid mode, revert to splash screen
				return 1;
		}
	} else {
		switch (mode) {
			case 0:
				WolfVxDVideoSplash ();		//Default splash screen
				return 0;
			case 1:
				WolfVxDVideoVoodoo ();		//Voodoo output
				return 0;
			case 2:
				WolfVxDVideoGrid ();		//Grid test mode
				return 0;
			default:
				WolfVxDVideoSplash ();		//Invalid mode, revert to splash screen
				return 1;
		}
	}
}

int twPollVideoMode ()
{
	
#ifdef DEBUGWOLF
OutputDebugString(">>>WOLF>>> twPollVideoMode()\n");
#endif
	BYTE state;

	if(0 == appMode)
	{
		state = (BYTE) *twVideoSwitch;
	} else {
		WolfVxDPollState ();
		state = (BYTE) myWolfState.videoMode;
	}

	switch (state) {
		case VIDEO_SPLASH:
			return 0;
		case VIDEO_VOODOO_LOGO:
		case VIDEO_MONW_GAME:
		case VIDEO_MONW_TEST:
			return 1;
		case VIDEO_GRID:
			return 2;
		default:
			return -1;	//Non-standard video state
	}
}