
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

#include <PwmOutputPin.h>
#include <Timers.h>
#include <ADC.h>
#include <Spindle.h>

#include <inc/hw_adc.h>
#include <driverlib/adc.h>
#include <inc/hw_gpio.h>
#include <driverlib/timer.h>
#include <inc/hw_timer.h>


uint16_t PwmOutputPin::assignedTimers = 0;

PwmOutputPin::PwmOutputPin(
	char*		data,
	String*		msgPtr )

	:	OutputPin(
			data,
			msgPtr ) {

	configureBase	= NULL; };

PwmOutputPin::~PwmOutputPin() {
#ifdef DebugDelete
	Serial.println( " delete PwmOutputPin" );
#endif

	if ( configureBase ) {
		assignedTimers &= ~configureBase->mask;
		disable(); }; };

bool
	PwmOutputPin::isPWM() {
	
	return
		configureBase != NULL; };

void
	PwmOutputPin::enable() {
	
	if ( configureBase ) {
//	TimerEnable
		HWREG( configureBase->timerBase + TIMER_O_CTL ) |=
			configureBase->mask & 0xFF ?
				TIMER_CTL_TAEN
			:	TIMER_CTL_TBEN; }; };
	
void
	PwmOutputPin::disable() {
	
	if ( configureBase ) {
//	TimerDisable(
		HWREG( configureBase->timerBase + TIMER_O_CTL ) &=
			configureBase->mask & 0xFF ?
				~TIMER_CTL_TAEN
			:	~TIMER_CTL_TBEN; }; };

#define TimerA	0x1
#define TimerB	0x100

bool
	PwmOutputPin::configureTimerBase( String* msgPtr ) {

	// Configure Timer for spindle
#ifdef PART_TM4C1294NCPDT
	static const GpioTimerConfigure gpioTimerBase[] = {
		// Timer 0 is used for motor stepping @ 15 kHz
//		{ 'A', 0, TIMER0_BASE, TimerA<<0, GPIO_PA0_T0CCP0 }, // U0Rx
		{ 'D', 0, TIMER0_BASE, TimerA<<0, GPIO_PD0_T0CCP0 }, //
		{ 'L', 4, TIMER0_BASE, TimerA<<0, GPIO_PL4_T0CCP0 }, //
//		{ 'A', 1, TIMER0_BASE, TimerB<<0, GPIO_PA1_T0CCP1 }, // U0Tx
		{ 'D', 1, TIMER0_BASE, TimerB<<0, GPIO_PD1_T0CCP1 }, //
		{ 'L', 5, TIMER0_BASE, TimerB<<0, GPIO_PL5_T0CCP1 }, //

		{ 'D', 2, TIMER1_BASE, TimerA<<1, GPIO_PD2_T1CCP0 }, //
		{ 'A', 2, TIMER1_BASE, TimerA<<1, GPIO_PA2_T1CCP0 }, //
//		{ 'L', 6, TIMER1_BASE, TimerA<<1, GPIO_PL6_T1CCP0 }, // Not available
		{ 'D', 3, TIMER1_BASE, TimerB<<1, GPIO_PD3_T1CCP1 }, //
		{ 'A', 3, TIMER1_BASE, TimerB<<1, GPIO_PA3_T1CCP1 }, //
//		{ 'L', 7, TIMER1_BASE, TimerB<<1, GPIO_PL7_T1CCP1 }, // Not available

		{ 'A', 4, TIMER2_BASE, TimerA<<2, GPIO_PA4_T2CCP0 }, //
		{ 'M', 0, TIMER2_BASE, TimerA<<2, GPIO_PM0_T2CCP0 }, //
		{ 'A', 5, TIMER2_BASE, TimerB<<2, GPIO_PA5_T2CCP1 }, //
		{ 'M', 1, TIMER2_BASE, TimerB<<2, GPIO_PM1_T2CCP1 }, //

		// Timer 3 is used for interpolation @ 2kHz
		{ 'A', 6, TIMER3_BASE, TimerA<<3, GPIO_PA6_T3CCP0 },
		{ 'M', 2, TIMER3_BASE, TimerA<<3, GPIO_PM2_T3CCP0 },
		{ 'D', 4, TIMER3_BASE, TimerA<<3, GPIO_PD4_T3CCP0 },
		{ 'A', 7, TIMER3_BASE, TimerB<<3, GPIO_PA7_T3CCP1 },
		{ 'M', 3, TIMER3_BASE, TimerB<<3, GPIO_PM3_T3CCP1 },
		{ 'D', 5, TIMER3_BASE, TimerB<<3, GPIO_PD5_T3CCP1 },

		{ 'M', 4, TIMER4_BASE, TimerA<<4, GPIO_PM4_T4CCP0 }, // InHB bp1-B9
//		{ 'B', 0, TIMER4_BASE, TimerA<<4, GPIO_PB0_T4CCP0 }, // Not available
		{ 'D', 6, TIMER4_BASE, TimerA<<4, GPIO_PD6_T4CCP0 }, //
		{ 'M', 5, TIMER4_BASE, TimerB<<4, GPIO_PM5_T4CCP1 }, //
		{ 'B', 1, TIMER4_BASE, TimerB<<4, GPIO_PB1_T4CCP1 }, //
		{ 'D', 7, TIMER4_BASE, TimerB<<4, GPIO_PD7_T4CCP1 }, // InHA bp1-B7

		{ 'M', 6, TIMER5_BASE, TimerA<<5, GPIO_PM6_T5CCP0 }, //
		{ 'B', 2, TIMER5_BASE, TimerA<<5, GPIO_PB2_T5CCP0 }, // InHC bp1-B10
		{ 'M', 7, TIMER5_BASE, TimerB<<5, GPIO_PM7_T5CCP1 }, //
		{ 'B', 3, TIMER5_BASE, TimerB<<5, GPIO_PB3_T5CCP1 }, //
		{ 0 } };
	
#elif defined( PART_LM4F120H5QR )
	static const GpioTimerConfigure gpioTimerBase[] = {
		// Timer 0 is used for motor stepping @ 15 kHz
		{ 'B', 6, TIMER0_BASE, TimerA<<0, GPIO_PB6_T0CCP0 },
		{ 'B', 7, TIMER0_BASE, TimerA<<8, GPIO_PB7_T0CCP1 },

		{ 'B', 4, TIMER1_BASE, TimerA<<1, GPIO_PB4_T1CCP0 },
		{ 'F', 2, TIMER1_BASE, TimerA<<1, GPIO_PF2_T1CCP0 },
		{ 'B', 5, TIMER1_BASE, TimerB<<1, GPIO_PB5_T1CCP1 },
		{ 'B', 5, TIMER1_BASE, TimerB<<1, GPIO_PB5_T1CCP1 },
		{ 'F', 3, TIMER1_BASE, TimerB<<1, GPIO_PF3_T1CCP1 },

		{ 'B', 0, TIMER2_BASE, TimerA<<2, GPIO_PB0_T2CCP0 },
		{ 'F', 4, TIMER2_BASE, TimerA<<2, GPIO_PF4_T2CCP0 },
		{ 'B', 1, TIMER2_BASE, TimerB<<2, GPIO_PB1_T2CCP1 },

		// Timer 3 is used for interpolation @ 2kHz
		{ 'B', 2, TIMER3_BASE, TimerA<<3, GPIO_PB2_T3CCP0 },
		{ 'B', 3, TIMER3_BASE, TimerB<<3, GPIO_PB3_T3CCP1 },

		{ 'C', 0, TIMER4_BASE, TimerA<<4, GPIO_PC0_T4CCP0 },
		{ 'C', 1, TIMER4_BASE, TimerB<<4, GPIO_PC1_T4CCP1 },

		{ 'C', 2, TIMER5_BASE, TimerA<<5, GPIO_PC2_T5CCP0 },
		{ 'C', 3, TIMER5_BASE, TimerB<<5, GPIO_PC3_T5CCP1 },
		{ 0 } };
#endif

	uint index = 0;
	while ( true ) {
		const GpioTimerConfigure* base = &gpioTimerBase[ index ];
		if ( base->portC == 0 )
			break;

		if (	base->portC == portC
			&&	base->pin	== pin ) {

			if (	base->timerBase == InterpolationTimerBase ) {
				*msgPtr		+= "E Timer assignment conflicts with interpolation timer 3";
				return
					NULL; };	// Can't use

			if (	base->timerBase == AxisTimerBase ) {
				*msgPtr		+= "E Timer assignment conflicts with motor stepping timer 0";
				return
					NULL; };	//  Can't use

			if (	assignedTimers & base->mask ) {
				*msgPtr		+= "E PWM Timer already in use";
				return
					NULL; };	//  Can't use

			configureBase	= base;
			assignedTimers	|= base->mask;

			ROM_GPIOPinConfigure( configureBase->pinConfigure );
			ROM_GPIOPinTypeTimer(
				GPIOBaseVector[ portC - 'A' ].base,
				1 << pin );

			return
				true; };

		index++; };

	*msgPtr		+= "E Timer configuration invalid";
	return
		false; };

void
	PwmOutputPin::setPeriod( uint32_t aPeriod ) {
	
	period	= aPeriod;

	if ( configureBase ) {
//	Bypass TimerLoadSet & TimerPrescaleSet
		if ( configureBase->mask & 0xFF ) {
			HWREG( configureBase->timerBase + TIMER_O_TAILR )	= period;
			HWREG( configureBase->timerBase + TIMER_O_TAPR)		= period >> 16; }

		else {
			HWREG( configureBase->timerBase + TIMER_O_TBILR )	= period;
			HWREG( configureBase->timerBase + TIMER_O_TBPR)		= period >> 16; };

		setOnTime( period >> 1 ); }; };	// 50%

void
	PwmOutputPin::setOnTime( uint32_t onTime ) {

	if ( onTime >= period )
		onTime	= period - 1;

	// count down match value
	if ( ! activeLow )
		onTime	= period - onTime - 1;

// TODO: Just so I can see the pulse
	if ( onTime < 1 )
		onTime = 1;

//	Bypass TimerMatchSet & TimerPrescaleMatchSet
	if ( configureBase->mask & 0xFF ) {
		HWREG( configureBase->timerBase + TIMER_O_TAMATCHR )	= onTime;
        HWREG( configureBase->timerBase + TIMER_O_TAPMR)		= onTime >> 16; }

	else {
		HWREG( configureBase->timerBase + TIMER_O_TBMATCHR )	= onTime;
        HWREG( configureBase->timerBase + TIMER_O_TBPMR)		= onTime >> 16; }; };
