
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

#include <SoftPwmTimer.h>
#include "Timers.h"
#include <MainCommands.h>
#include <SoftPwmOutputPin.h>

#include <driverlib/timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>

#include <inc/hw_ints.h>
#include <inc/hw_timer.h>

//volatile uint32_t	SoftPwmTimer::pwmLedInterruptCount	= 0;

void
	SoftPwmTimer::disableSoftPwmInterrupts() {

	TimerIntDisable(
		SoftPwmTimerBase,
		SoftPwmTimerTimeout ); };

void
	SoftPwmTimer::enableSoftPwmInterrupts() {
	
	TimerIntEnable(
		SoftPwmTimerBase,
		SoftPwmTimerTimeout ); };

void
	SoftPwmTimer::startSoftPwmTimerInterrupts( uint32_t hz ) {

	TimerLoadSet(
		SoftPwmTimerBase,
		SoftPwmTimerLoad,
		Timers::frequencyToPeriod( hz ) );
  
	TimerIntRegister(
		SoftPwmTimerBase,
		SoftPwmTimerLoad,
		SoftPwmTimer::SoftPwmTimerInterruptHandler );

	//	INT_TIMER0B = 36 - PRI9 36..39
#ifdef PART_TM4C1294NCPDT // IntPrioritySet not working for this µP

	HWREG( NVIC_PRI9 ) &= ~NVIC_PRI9_INT36_M;
	HWREG( NVIC_PRI9 ) |= SoftPwmTimerPriorityLevel << NVIC_PRI9_INT36_S;

#elif defined( PART_LM4F120H5QR )

	IntPrioritySet(
		SoftPwmTimerInt,
		SoftPwmTimerPriorityLevel );

#endif

	SoftPwmTimer::enableSoftPwmInterrupts();

	TimerEnable(
		SoftPwmTimerBase,
		SoftPwmTimerLoad ); };

void
	SoftPwmTimer::SoftPwmTimerInterruptHandler( void ) {

	TimerIntClear(
		SoftPwmTimerBase,
		SoftPwmTimerTimeout );

//	Timers::setD4LED( ++SoftPwmTimer::pwmLedInterruptCount >> 14 );
	SoftPwmOutputPin::pwmInterrupt(); };

