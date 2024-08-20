#include <windows.h>
#include "VWOLFD.h"

#define RESULT
#define IN
#define OUT
#define INOUT

#define VXD_FAILURE	FALSE
#define	VXD_SUCCESS	TRUE

// VxD variables
extern HANDLE hVxD;
extern wolfState myWolfState;

//VxD functions
RESULT BOOL WolfVxDOpen();

void WolfVxDInit();
void WolfVxDCleanup();

void WolfVxDDogSpawn ();
void WolfVxDDogEnable ();
void WolfVxDDogDisable ();
void WolfVxDDogToggle ();

void WolfVxDVideoSplash ();
void WolfVxDVideoVoodoo ();
void WolfVxDVideoGrid ();

void WolfVxDPollState ();