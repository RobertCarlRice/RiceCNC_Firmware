
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

#include <AxisTimer.h>
#include "SysTick.h"
#include "Timers.h"
#include "Machine.h"
#include <MainCommands.h>
#include <Axis.h>
#include <InterpolationTimer.h>
#include <SoftPwmOutputPin.h>

#include <driverlib/timer.h>
#include <driverlib/sysctl.h>
#include <inc/hw_ints.h>
#include <inc/hw_timer.h>


volatile float		AxisTimer::axisInterruptPerioduSF	= 0;

// volatile uint32_t	AxisTimer::axisInterruptTime		= 0;
volatile uint32_t	AxisTimer::axisTimerPeriod			= 0;
volatile uint32_t	AxisTimer::axisLedInterruptCount	= 0;

void
	AxisTimer::disableAxisInterrupts() {

	TimerIntDisable(
		AxisTimerBase,
		AxisTimerTimeout ); };

void
	AxisTimer::enableAxisInterrupts() {
	
	TimerIntEnable(
		AxisTimerBase,
		AxisTimerTimeout ); };

void
	AxisTimer::startAxisTimerInterrupts( uint32_t hz ) {

	// Configure Timer for axes isr. Also configure timer B for SoftPFM if used
	SysCtlPeripheralEnable( AxisTimerPeriferal );
	TimerConfigure(
		AxisTimerBase,
		TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC );
  
//	uint32_t hz	= MainCommands::motorStepInterruptRate * 1000;
	AxisTimer::axisTimerPeriod   =
		Timers::frequencyToPeriod( hz );

	TimerLoadSet(
		AxisTimerBase,
		AxisTimerLoad,
		AxisTimer::axisTimerPeriod );
  
	TimerIntRegister(
		AxisTimerBase,
		AxisTimerLoad,
		AxisTimer::AxisTimerInterruptHandler );

	//  Set Timer 0A Interrupt priority to AxisTimerPriorityLevel
	//	INT_TIMER0A = 35 - PRI8 32..35
#ifdef PART_TM4C1294NCPDT // IntPrioritySet not working for this µP

	HWREG( NVIC_PRI8 ) =
		( HWREG( NVIC_PRI8 ) & ~NVIC_PRI8_INT35_M )
	|	AxisTimerPriorityLevel << NVIC_PRI8_INT35_S; // 29 bit shift

	axisInterruptPerioduSF =
		float( AxisTimer::axisTimerPeriod )
	/	120.0;

#elif defined( PART_LM4F120H5QR )
	IntPrioritySet(
		AxisTimerInt,
		AxisTimerPriorityLevel );

	axisInterruptPerioduSF =
		float( AxisTimer::axisTimerPeriod )
	/	80.0;

#endif

	AxisTimer::enableAxisInterrupts();

	TimerEnable(
		AxisTimerBase,
		AxisTimerLoad ); };

// Timer 0A has higher interrupt priority
void
	AxisTimer::AxisTimerInterruptHandler( void ) { // Axis isr

	TimerIntClear(
		AxisTimerBase,
		AxisTimerTimeout );

	Axis::motorStepInterrupt();

	Timers::setAxisLED( ++AxisTimer::axisLedInterruptCount >> 13 ); };

void
	AxisTimer::startAxisInterrupt() {
	
//	AxisTimer::axisInterruptTime   = SysTick::microSecondTimer;
	
	Timers::setAxisLED( false );
//	InterpolationTimer::startAxisInterrupt();
};

void
	AxisTimer::endAxisInterrupt() {

	/* Add SSI interrupt processing time to Axis interrupt processing time
	uint32_t elapsedTime =
			SysTick::microSecondTimer
		-	AxisTimer::axisInterruptTime;
	InterpolationTimer::endAxisInterrupt( elapsedTime );
	*/
	Timers::setAxisLED( ++AxisTimer::axisLedInterruptCount >> 13 ); };

bool
	AxisTimer::axisInterruptActive() { // Timer 0A Int 19

	return
		( HWREG( NVIC_ACTIVE0 ) & 1 << 19 ) != 0; }; // ACTIVE0 Ints 0..31

