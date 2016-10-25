
// © RiceMotion ( Robert Carl Rice ) 2012-2016 - (GPLv3)

// As of 10/23/2016 RiceMotion is releasing this firmware to the open source community
// under GNU General Public License v3 (GPL-3) to encourage support from other developers
// while ensuring that the firmware portion of this application remains free.
// Details of the GPLv3 license are available at the website:
// https://tldrlegal.com/license/gnu-general-public-license-v3-(gpl-3)

// Note that his firmware is incomplete without a complementary host softwae application.
// RiceMotion offers a Mac desktop host application, RiceCNC, on the iTunes store.
// but encourages the development of compatable host software for other systems.
// For example, a web based (HTTP) interface could be useful.
// Host applications may be chargeable, but firmware modifications must be
// coordintated through GITHUB.

// This firmware is currently edited via XCODE and compiled using Energia.

// © RiceMotion ( Robert Carl Rice ) 2012-2016 - (GPLv3)

#include <SysTick.h>
#include "Timers.h"
#include "AxisTimer.h"
#include "InterpolationTimer.h"
#include "MainCommands.h"

#include <driverlib/timer.h>
#include <driverlib/SysTick.h>
#include <inc/hw_ints.h>

volatile uint32_t	SysTick::microSecondTimer		= 0;

/*
wiring.c initializes SysTick to 100 Khz
Here I reinitialize it to 1 MHz ( microsecond ) and change the handler
This disables the Energia delay( milliseconds ) and the delayMicroseconds ( µS ) functions
that use the SysTick timer
It also disables the registerSysTickCb function

The micro() and millis() functions will still work as they use Timer5
*/

void
	SysTick::StartSysTickInterrupts() {

	// Use SysTick to drive microsecond counter
	SysTickPeriodSet( Timers::frequencyToPeriod( 1000000 ) ); // 1 µSec interrupts (119)
	SysTickIntRegister(
		SysTick::SysTickInterruptHandler );
	ROM_IntPrioritySet(
		FAULT_SYSTICK,
		SysTickPriorityLevel );
	SysTick::microSecondTimer		= 0;
	SysTickEnable();
	SysTickIntEnable(); };


void
	SysTick::SysTickInterruptHandler(void) {

	// At SysTickPriorityLevel priority level the timer interrupts are disabled
	// SysTick interrupt flag is automatically cleared
	SysTick::microSecondTimer++; };
