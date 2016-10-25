
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

#include <Timers.h>
#include <GPIO.h>
#include <SysTick.h>
#include <AxisTimer.h>
#include <SoftPwmTimer.h>
#include <InterpolationTimer.h>
#include <MainCommands.h>

#include <driverlib/timer.h>
#include <driverlib/SysTick.h>
#include <inc/hw_ints.h>
#include <driverlib/sysctl.h>

bool	Timers::displayRed	  = true;
bool	Timers::displayBlue   = true;
bool	Timers::displayGreen  = true;
bool	Timers::displayD4	  = true;

void
	Timers::enableTimers() {
	Timers::enableLedOutputs();

	SysTick::StartSysTickInterrupts();

	InterpolationTimer::startInterpolationTimerInterrupts();
	// Configure the remaining timers for PWM output
	// TIMER0_BASE is axis & SoftPFM
	Timers::configureTimerSplitPWM(
		SYSCTL_PERIPH_TIMER1,
		TIMER1_BASE );
	Timers::configureTimerSplitPWM(
		SYSCTL_PERIPH_TIMER2,
		TIMER2_BASE );
	// TIMER3_BASE is interpolation
	Timers::configureTimerSplitPWM(
		SYSCTL_PERIPH_TIMER4,
		TIMER4_BASE );
	Timers::configureTimerSplitPWM(
		SYSCTL_PERIPH_TIMER5,
		TIMER5_BASE );
	};

void
	Timers::enableLedOutputs() {

#ifdef PART_TM4C1294NCPDT

    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPION ); // Enable GPIO port N
    GPIOPinTypeGPIOOutput(
		GPIO_PORTN_BASE,
		GPIO_PIN_0 | GPIO_PIN_1 ); // LED D2 D1 output pins

    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF ); // Enable GPIO port F
    GPIOPinTypeGPIOOutput(
		GPIO_PORTF_BASE,
		GPIO_PIN_0 | GPIO_PIN_4 ); // LED D4 D3 output pins

#elif defined( PART_LM4F120H5QR )

    SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF ); // Enable GPIO port F
    GPIOPinTypeGPIOOutput( 
		GPIO_PORTF_BASE,
		GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 ); // LED output pins 1, 2 & 3 };

#endif
}; 

void
	Timers::configureTimerSplitPWM(
		uint32_t sysctrl,
		uint32_t timerBase ) {

	ROM_SysCtlPeripheralEnable( sysctrl );

	TimerConfigure(
		timerBase,
			TIMER_CFG_SPLIT_PAIR
		|	TIMER_CFG_A_PWM
		|	TIMER_CFG_B_PWM ); };

void
	Timers::setInterpolationLED( uint32_t set ) { // D2 x11-85

#ifdef PART_TM4C1294NCPDT
	Timers::setD1LED( set ); // D2 x11-85

#elif defined( PART_LM4F120H5QR )
	Timers::setRedLED( set );

#endif
};

void
	Timers::setAxisLED( uint32_t set ) { // D2 x11-85

#ifdef PART_TM4C1294NCPDT
	Timers::setD2LED( set ); // D2 x11-85

#elif defined( PART_LM4F120H5QR )
	Timers::setBlueLED( set );

#endif
};

void
	Timers::setLoopLED( uint32_t set ) { // D2 x11-85

#ifdef PART_TM4C1294NCPDT
	Timers::setD3LED( set ); // D2 x11-85

#elif defined( PART_LM4F120H5QR )
	Timers::setGreenLED( set );

#endif
};

#ifdef PART_TM4C1294NCPDT

void
	Timers::setD1LED( uint32_t set ) { // D1 x11-87

	if ( Timers::displayRed ) // Stop using once assigned to another function
		GPIOPinWrite(
			GPIO_PORTN_BASE,
			GPIO_PIN_1,
			set ); };

void
	Timers::setD2LED( uint32_t set ) { // D2 x11-85

    if ( Timers::displayBlue ) // Stop using once assigned to another function
		GPIOPinWrite(
			GPIO_PORTN_BASE,
			GPIO_PIN_0,
			set ); };

void
	Timers::setD3LED( uint32_t set ) { // D3 x11-64

    if ( Timers::displayGreen ) // Stop using once assigned to another function
		GPIOPinWrite(
			GPIO_PORTF_BASE,
			GPIO_PIN_4,
			set ); };

void
	Timers::setD4LED( uint32_t set ) { // D3 x11-64

    if ( Timers::displayD4 ) // Stop using once assigned to another function
		GPIOPinWrite(
			GPIO_PORTF_BASE,
			GPIO_PIN_0,
			set ); };

#elif defined( PART_LM4F120H5QR )

void
	Timers::setRedLED( uint32_t set ) {

    if ( Timers::displayRed ) // Stop using once assigned to another function
		GPIOPinWrite(
			GPIO_PORTF_BASE,
			GPIO_PIN_1,
			set ); };

void
	Timers::setBlueLED( uint32_t set ) {

    if ( Timers::displayBlue ) // Stop using once assigned to another function
		GPIOPinWrite(
			GPIO_PORTF_BASE,
			GPIO_PIN_2,
			set ); };

void
	Timers::setGreenLED( uint32_t set ) {

    if ( Timers::displayGreen ) // Stop using once assigned to another function
		GPIOPinWrite(
			GPIO_PORTF_BASE,
			GPIO_PIN_3,
			set ); };

#endif

void
	Timers::checkPinConflict( GpioPin* gpioPin ) {

#ifdef PART_TM4C1294NCPDT
	switch ( gpioPin->portC ) {
	
		case 'N':
		switch ( gpioPin->pin ) {
			case 0:
			Timers::displayBlue   = false;
			break;
			
			case 1:
			Timers::displayRed    = false; }
		break;

		case 'F':
		switch ( gpioPin->pin ) {
			case 0:
			Timers::displayD4     = false;
			break;
			
			case 4:
			Timers::displayGreen  = false; }; };

#elif defined( PART_LM4F120H5QR )
	if ( gpioPin->portC == 'F' ) {
		switch ( gpioPin->pin ) { // LED pin used for a different function
			case 1:
			Timers::displayRed   = false;
			break;

			case 2:
			Timers::displayBlue  = false;
			break;

			case 3:
			Timers::displayGreen = false; }; };
#endif
};

uint32_t
	Timers::frequencyToPeriod( uint32_t frequency ) {

	//  return SysCtlClockGet() / frequency - 1; }; // significant overhead
	return
		ClockFrequency	// 120,000,000
	/	frequency
	-	1; };			// returns 599 for 200 kHz
