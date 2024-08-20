#include <windows.h>

typedef unsigned char	BYTE;

extern int appMode;	// Application running mode
					//	0: ring 3 user mode
					//	1: VxD mode

//extern wolfState myWolfState;

//Dog Functions
void twPetDog();
void twToggleDog();
void twEnableDog();
void twDisableDog();
BOOL twPollDogState ();

//Video Functions
int twSwitchVideoMode (int);
int twPollVideoMode ();
