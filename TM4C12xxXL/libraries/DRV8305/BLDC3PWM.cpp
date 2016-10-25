
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

#include <BLDC3PWM.h>
#include <StepCounter.h>

#include <InputPin.h>
#include <OutputPin.h>
#include <PwmOutputPin.h>
#include <AnalogPin.h>

#include <Timers.h>
#include <DRV8305Axis.h>
#include <MachineCommands.h>
#include <HallFeedback.h>

#include <inc/hw_gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>
#include <inc/hw_timer.h>

//#define DebugDelete 1

#define PWM_Switching 12	// 4096

BLDC3PWM::BLDC3PWM(
	char*			data,
	String*			msgPtr,
	DRV8305Axis*	aaxis )
	
	:	BLDC3(
			data,
			msgPtr,
			aaxis ) {

	phaseApin		= NULL;
	phaseBpin		= NULL;
	phaseCpin		= NULL;

	char	phaseApinString		[8];
	char	phaseBpinString		[8];
	char	phaseCpinString		[8];

	sscanf( data + parseLength,
		" %s %s %s",
		phaseApinString,
		phaseBpinString,
		phaseCpinString );

	// Recommended switching rate for 8305 is 200 kHZ
	// but driver cannot respond that fast in 3-PWM mode
	//	uint32_t period		= Timers::frequencyToPeriod( 200,000 );

	// but for efficiency use a power of 2 period - 29,296 Hz
	uint32_t period		= ( 1 << PWM_Switching ) - 1;	// 4095

	*msgPtr		+= "\n inHA - Phase A";
	phaseApin	= new PwmOutputPin(
		phaseApinString,
		msgPtr );
	if ( ! configurePin( phaseApin, msgPtr, period ) )
		return;

	*msgPtr		+= "\n inHB - Phase B";
	phaseBpin	= new PwmOutputPin(
		phaseBpinString,
		msgPtr );
	if ( ! configurePin( phaseBpin, msgPtr, period ) )
		return;

	*msgPtr		+= "\n inHC - Phase C";
	phaseCpin	= new PwmOutputPin(
		phaseCpinString,
		msgPtr );

	if ( ! configurePin( phaseCpin, msgPtr, period ) ) {
		delete phaseCpin;
		phaseCpin	= NULL;
		return; };

/*	Didn't work
	HWREG( TIMER0_BASE + TIMER_O_SYNC ) =
			TIMER_4A_SYNC
		||	TIMER_4B_SYNC
		||	TIMER_5A_SYNC;
*/
	};

bool
	BLDC3PWM::configurePin(
		PwmOutputPin*	phasePin,
		String*			msgPtr,
		uint32_t		period ) {

	if ( ! phasePin->configureTimerBase( msgPtr ) )
		return
			false;

	phasePin->fmtPin(	msgPtr );
	phasePin->setPeriod( period );
	phasePin->setOnTime( 0 );
	phasePin->enable();

	return
		true; };

bool
	BLDC3PWM::valid() {
	
	return
		phaseApin
	&&	phaseBpin
	&&	phaseCpin; };


BLDC3PWM::~BLDC3PWM() {

#ifdef DebugDelete
	Serial.println( " delete BLDC3PWM" );
#endif

	if ( phaseApin )
		delete phaseApin;

	if ( phaseBpin )
		delete phaseBpin;

	if ( phaseCpin )
		delete phaseCpin; };

void
	BLDC3PWM::activeFreewheel() {

	stepTorque			= 0;
//	feedbackTorque		= 0;

	phaseApin->setOnTime( 0 );
	phaseBpin->setOnTime( 0 );
	phaseCpin->setOnTime( 0 );

/*
	phaseApin->disable();
	phaseApin->setPin( false );

	phaseBpin->disable();
	phaseBpin->setPin( false );

	phaseCpin->disable();
	phaseCpin->setPin( false );
*/
};

void
	BLDC3PWM::assertTorque(
		uint32_t	torqueA,
		uint32_t	torqueB,
		uint32_t	torqueC ) {

/* TODO: 
Serial.print( " A " );
Serial.print( torqueA );
Serial.print( " B " );
Serial.print( torqueB );
Serial.print( " C " );
Serial.println( torqueC );
*/
	phaseApin->setOnTime( torqueA >> PWM_Switching );
	phaseBpin->setOnTime( torqueB >> PWM_Switching );
	phaseCpin->setOnTime( torqueC >> PWM_Switching ); };
