
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

#include <Spindle.h>
#include <Timers.h>
#include <PwmOutputPin.h>
#include <Axis.h>
#include <Machine.h>

#include <driverlib/timer.h>


Spindle::Spindle(
	Machine*	myMachine,
	char*		data,
	String*		msgPtr ) {

	machine		= myMachine;
	char        runPinString[8];
	char        directionPinString[8];

	pwmFrequency		= -1;
	setScaledSpeed		= 0.0;
	onDelayMs			= 1000;
	offDelayMs			= 1000;

	sscanf( data,
		"%c %ld %ld %ld %s %s",
		&useAxis,
		&onDelayMs,
		&offDelayMs,
		// independent mode
		&pwmFrequency,
		runPinString,
		directionPinString );

	flags.spindleOn		= 0;
	flags.spindleCCW	= 0;
	flags.pulseOn		= 0;

	runPin			= NULL;
	directionPin    = NULL;

	snprintf( data, 40,
		" Delay On %lums Off %lums",
		onDelayMs,
		offDelayMs );
	*msgPtr  += String( data );
 
	switch ( useAxis ) {
		case 'X' :
		case 'Y' :
		case 'Z' :
		case 'A' :
		case 'B' :
		case 'C' :
		case 'U' :
		case 'V' :
		case 'W' :

		*msgPtr	+= " Spindle is Axis ";
		*msgPtr	+= String( useAxis );
		return; };

	useAxis		= 0;

	runPin		= new PwmOutputPin(
		runPinString,
		msgPtr );
	if ( ! runPin->valid() ) {
		delete runPin;
		runPin    = NULL;
		return; };
	
	runPin->setPin( false);

	*msgPtr		+= " Spindle run";
	runPin->fmtPin( msgPtr );

	if ( pwmFrequency >= 0 ) {	// PWM Capable
		*msgPtr		+= " PWM frequency";

		if ( runPin->configureTimerBase( msgPtr ) ) {
			if ( pwmFrequency > 0 ) {
				snprintf( data, 40,
					" %lu",
					pwmFrequency );
				*msgPtr  += String( data );

				runPin->setPeriod( Timers::frequencyToPeriod( pwmFrequency ) ); }

			else {
				*msgPtr  += " variable";
				setSpindleSpeed( 500 ); }; }; }; // default speed

	directionPin  = new OutputPin(
		directionPinString,
		msgPtr );
	if ( directionPin->valid() ) {
		directionPin->setPin( false);

		*msgPtr			+= " direction";
		directionPin->fmtPin( msgPtr ); }
 
	else {
		delete directionPin;
		directionPin    = NULL; }; };


Spindle::~Spindle() {
	if ( runPin )
		delete runPin;

	if ( directionPin )
		delete directionPin; };

bool
	Spindle::isValid() {
	
	return
			useAxis
		||	runPin; };

void
	Spindle::setSpindleDirection( bool clockwise ) {

	flags.spindleCCW	= clockwise ?
			0
		:	1;

	if ( useAxis )
		setAxisSpeed();

	else if ( directionPin )
		directionPin->setPin( clockwise ); };

bool
	Spindle::counterClockwise() {

	return
		flags.spindleCCW != 0; };

bool
	Spindle::running() {

	return
		flags.spindleOn != 0; };


void
	Spindle::setSpindleOn(
		bool		run ) {

	flags.spindleOn	= run ?
			1
		:	0;

	if ( useAxis )
		setAxisSpeed();
  
	else if ( runPin->isPWM() ) {
		if ( run )
			runPin->enable();
		else
			runPin->disable(); }

	else
		runPin->setPin( run ); };


void
	Spindle::setSpindleSpeed( float scaledSpeed ) {

	setScaledSpeed	= fabs( scaledSpeed );

	if ( useAxis )
		setAxisSpeed();	// Units are StepsPerSecond

	else if ( runPin->isPWM() ) {

		if ( pwmFrequency ) // Units are % duty cycle
			runPin->setOnTime(
					runPin->period
				*	setScaledSpeed
				/	100.0 );

		else // Units are Hertz
			runPin->setPeriod( Timers::frequencyToPeriod( setScaledSpeed ) ); }; };

void
	Spindle::setAxisSpeed() {

	Axis* axis = machine->findAxis(
		useAxis,
		NULL );
	if ( axis )
		if ( flags.spindleOn ) {
			axis->setTargetVelocity( flags.spindleCCW ?
				-setScaledSpeed
			:	setScaledSpeed ); }

		else
			axis->setTargetVelocity( 0.0 );	};
