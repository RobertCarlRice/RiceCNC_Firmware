
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

#ifndef Timers_h
#define Timers_h

#include <Energia.h>
#include <SysTick.h>

class GpioPin;

#if defined( PART_TM4C1294NCPDT )	// Connected LaunchPad
  #define ClockFrequency			120000000

#elif defined( PART_LM4F120H5QR )	// LaunchPad
  #define ClockFrequency			80000000

#endif

// Interrupt priority levels:
#define SysTickPriorityLevel				1
#define SoftPwmTimerPriorityLevel			2
#define GPIO_PriorityLevel					3	// Analog
#define SSI_TxEotPriorityLevel				4
#define AxisTimerPriorityLevel				5
#define ThreePhaseTimerPriorityLevel		5
#define InterpolationTimer3PriorityLevel	6

// Split Timer 0 Axis A, SoftPWM B
#define AxisTimerBase				TIMER0_BASE
#define AxisTimerPeriferal			SYSCTL_PERIPH_TIMER0
#define AxisTimerTimeout			TIMER_TIMA_TIMEOUT
#define AxisTimerLoad				TIMER_A
#define AxisTimerInt				INT_TIMER0A

#define SoftPwmTimerBase			TIMER0_BASE
#define SoftPwmTimerPeriferal		SYSCTL_PERIPH_TIMER0
#define SoftPwmTimerTimeout			TIMER_TIMB_TIMEOUT
#define SoftPwmTimerLoad			TIMER_B
#define SoftPwmTimerInt				INT_TIMER0B

// Split Timer 3 Interpolation A, ThreePhase B
#define InterpolationTimerBase		TIMER3_BASE
#define InterpolationTimerPeriferal SYSCTL_PERIPH_TIMER3
#define InterpolationTimerTimeout	TIMER_TIMA_TIMEOUT
#define InterpolationTimerLoad		TIMER_A
#define InterpolationTimerInt		INT_TIMER3A

#define ThreePhaseTimerBase			TIMER3_BASE
#define ThreePhaseTimerPeriferal	SYSCTL_PERIPH_TIMER3
#define ThreePhaseTimerTimeout		TIMER_TIMB_TIMEOUT
#define ThreePhaseTimerLoad			TIMER_B
#define ThreePhaseTimerInt			INT_TIMER3B


class Timers {
	static void
		enableLedOutputs();

	public:	
	Timers();
	
	static	bool	displayD4;	
	static	bool	displayRed;
	static	bool	displayBlue;
	static	bool	displayGreen;

	static void
		enableTimers();

	static void
		configureTimerSplitPWM(
			uint32_t timerBase,
			uint32_t period );

	static void
		setInterpolationLED( uint32_t set );

	static void
		setAxisLED( uint32_t set );

	static void
		setLoopLED( uint32_t set );

	static void
		checkPinConflict( GpioPin* gpioPin );

#ifdef PART_TM4C1294NCPDT

	static void
		setD1LED( uint32_t set );

	static void // Interpolation
		setD2LED( uint32_t set );

	static void
		setD3LED( uint32_t set );

	static void
		setD4LED( uint32_t set ); // D3 x11-64

#elif defined( PART_LM4F120H5QR )

	static void
		setRedLED( uint32_t set );

	static void // Interpolation
		setBlueLED( uint32_t set );

	static void
		setGreenLED( uint32_t set );

#endif

	static uint32_t
		frequencyToPeriod( uint32_t frequency );
};

#endif
