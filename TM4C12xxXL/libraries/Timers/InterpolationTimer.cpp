
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

#include <InterpolationTimer.h>
#include <SysTick.h>
#include <Timers.h>
#include <Machine.h>
#include <MainCommands.h>
#include <RiceCNC_Main.h>

#include <driverlib/timer.h>
#include <driverlib/sysctl.h>
#include <inc/hw_ints.h>
#include <inc/hw_timer.h>

#ifdef PART_TM4C1294NCPDT
  #define MaxInterpolationTimerPeriod  240000  // 120,000,000 / 240,000 = 500 Hz min frequency
  #define MinInterpolationTimerPeriod   60000  // 120,000,000 /  60,000 = 2 kHz max frequency

#elif defined( PART_LM4F120H5QR )
  #define MaxInterpolationTimerPeriod   80000  //  80,000,000 / 160,000 = 500 Hz min frequency
  #define MinInterpolationTimerPeriod   40000  //  80,000,000 /  40,000 = 2 kHz max frequency

#endif

volatile float		InterpolationTimer::interpolationInterruptPeriodSecF	= 0;

volatile uint32_t	InterpolationTimer::interpolationLedInterruptCount	= 0;
uint32_t			InterpolationTimer::watchDogTimer					= 0;

void
	InterpolationTimer::disableInterpolationInterrupts() {

	TimerIntDisable(
		InterpolationTimerBase,
		InterpolationTimerTimeout ); };

void
	InterpolationTimer::enableInterpolationInterrupts() {
	
	TimerIntEnable(
		InterpolationTimerBase,
		InterpolationTimerTimeout ); };


void
	InterpolationTimer::startInterpolationTimerInterrupts() {

	// Configure Timer for axes isr  
	SysCtlPeripheralEnable( InterpolationTimerPeriferal );
	TimerConfigure(
		InterpolationTimerBase,
		TIMER_CFG_PERIODIC );
  
	// The period of the motor stepping timer is varied
	// dynamically to use a percentage of CPU time
	TimerLoadSet(
		InterpolationTimerBase,
		InterpolationTimerLoad,
		Timers::frequencyToPeriod( MainCommands::interpolationInterruptRate ) );
  
	TimerIntRegister(
		InterpolationTimerBase,
		InterpolationTimerLoad,
		InterpolationTimer::interpolationTimerInterruptHandler );

	//  Set Timer 3A Interrupt priority to InterpolationTimer3PriorityLevel
	//	InterpolationTimer3PriorityLevel = 6
	//	INT_TIMER3A = 51 - PRI12 48..51
	#ifdef PART_TM4C1294NCPDT // IntPrioritySet not working for this µP
		HWREG( NVIC_PRI12 ) = ( HWREG( NVIC_PRI12 ) & ~NVIC_PRI12_INT51_M ) |
		InterpolationTimer3PriorityLevel << NVIC_PRI12_INT51_S; // 29 bit shift
 
	#elif defined( PART_LM4F120H5QR )
		IntPrioritySet(
			InterpolationTimerInt,
			InterpolationTimer3PriorityLevel );

	#endif

	InterpolationTimer::interpolationLedInterruptCount	= 0;
	
	InterpolationTimer::enableInterpolationInterrupts();
	TimerEnable(
		InterpolationTimerBase,
		InterpolationTimerLoad ); };

// Timer 0A has higher interrupt priority
void
	InterpolationTimer::interpolationTimerInterruptHandler(void) { // Axis isr

	TimerIntClear(
		InterpolationTimerBase,
		InterpolationTimerTimeout );

	uint32_t interruptTime  = SysTick::microSecondTimer;

	if ( MainCommands::hostTimeout &&
		InterpolationTimer::watchDogTimer >= MainCommands::hostTimeout ) {

#if defined( PART_TM4C1294NCPDT )  // Connected LaunchPad
		MainCommands::softwareReset( MyEEProm::HostMessageTimeout );
#else
		MainCommands::softwareReset();
#endif
		return; }; // Ignore interpolation interrupts stopping interpolation

	InterpolationTimer::watchDogTimer++; // Interpolation is running

	Timers::setInterpolationLED(  false );

	if ( Machine::firstMachine ) {
		Machine::firstMachine->interpolationISR(); };

	Timers::setInterpolationLED(
		++InterpolationTimer::interpolationLedInterruptCount >> 11 ); };

bool
	InterpolationTimer::interpolationInterruptActive() { // Timer 3A Int 35

	return
		( HWREG( NVIC_ACTIVE1 ) & 1 << (35-32) ) != 0; }; // ACTIVE1 Ints 32..63

// Axis can interrupt interpolation - charge time only to axis
void
	InterpolationTimer::startAxisInterrupt() {
	
	if ( InterpolationTimer::interpolationInterruptActive() )
		// restore the interpolation LED
		Timers::setInterpolationLED(
			InterpolationTimer::interpolationLedInterruptCount >> 11 ); };

void
	InterpolationTimer::endAxisInterrupt( uint32_t elapsedTime ) {

	if ( InterpolationTimer::interpolationInterruptActive() )
		// restore the interpolation LED
		Timers::setInterpolationLED( false); };
