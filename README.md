# Taito Wolf System Utility for Windows 98

This is a systray application for the Taito Wolf System running Windows 98. It provides JAMMA video output switching functionality, as well as toggling the watchdog service.

-------------

## Functionality

### JAMMA Output Switching

The Taito Wolf System arcade board shows a static splash screen when powered on. This utility allows you to switch between different outputs (Splash / Grid / Vodooo).

### Watchdog Toggle

The Taito Wolf System arcade board has a 3-pin output (Connector R) that connects to the PC motherboard's reset switch pins. It triggers every 64 seconds if the watchdog on the management CPLD is not kicked. This also resets the JAMMA output to the default splash screen.

This utility provides watchdog maintenance and toggling functionality to prevent the aforementioned watchdog reset action.

-------------

## Operation

The utility shows a splash screen for 3 seconds, while initilizes and putting itself in the system tray area.

*If the companion VxD driver is not installed, you will see a warning message on the splash screen, and the utility will run in degraded mode in user space.*

The systray icon color indicates the state of operation of the utility:

- Watchdog is running
  - Blue: Voodoo is output
  - Red: Logo splash screen is output
  - Yellow: Test grid screen is output
- Watchdog is not running
  - Gray (video output switching is not available in this mode)

When right-clicked on the icon, a pop-up menu is shown. You can choose among the 3 video output modes, and disable the watchdog just in case it's necessary.

The About menu item will show the about information window similar to the splash screen.

### Normal Mode v.s. Degraded Mode

This utility is designed to work with the [vwolfd VxD driver](https://github.com/jeffqchen/vwolfd) for more reliable operations. The VxD code running in the kernel space provides guaranteed time slice allotted for watchdog upkeeping as well as safer handling of output controls.

However, when the VxD is found to be not present on startup, the utility will instead run in user space. The functionalities are not impacted, however, when Windows becomes very busy, the watchdog upkeeping might lapse and lead to inadvertent failing over back to a reset state.

-------------

## Installation

The .exe file does not need installation to run.

However, I have provided an INF file to install the VxD alongside the Windows utility as a system device in the TaitoUtl project. Paired with the VxD driver, the watchdog functionality will work more reliably.

You can find it over [Here](https://github.com/jeffqchen/TaitoUtl/releases)

-------------

## How To Build On Your Own

This project is written under Visual Studio 6. You can facilitate Visual Studio 6 to view and compile the project.

-------------

## Technical Details

The Taito Wolf System is controlled by the onboard CPLD U46. It's responsible of various funcctionalities of the system. This project invovled the video switching and hardware watchdog functionalities.

## Video Switching

The JAMMA edge video output of this board can be switched among 3 modes - Logo Splash / Test Grid / Voodoo

On power up, U73 CPLD will read the splash screen data on the EPROM U71 and present it on the video output on the JAMMA video output. The output will stay in this state unless U46 CPLD received certain commands on address 0XCB600.

From my experiments, odd numbers below 0x30 will trigger the splash screen, and even numbers below 0x30 will trigger the screen test grid screen.

When fed a value equal or greater than 0x30, the video output is switched to the 3Dfx Voodoo, which will show the actual 3D image generated. More specifically, the following values were used by the game code:
0x30: During 3Dfx spinning logo
0x3D: During normal gameplay
0x31: During the test mode

## Watchdog

The arcade board has a built-in timer-based watchdog that kicks in every 64 seconds to reset the video output as well as triggering connector R, which is connected to the reset pins to the PC motherboard below. In order to surpress the trigger, address 0xCB200 needs to be fed SOMETHING freqently.

In the game code, this address is fed 0xFF on every frame. However from the testing, it seems every few seconds is fine. Also considering not interfering with the actual game code, I settled with once per second. This value can be adjusted easily in the source code.

Curiously, 0xCB200 is also the 2P button data is read from. My guess is that it's not the value but the action of writing to this address that kicks the watchdog.
