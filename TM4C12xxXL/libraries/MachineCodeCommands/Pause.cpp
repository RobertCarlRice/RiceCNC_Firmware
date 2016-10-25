
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

#include <Pause.h>
#include <Machine.h>
#include <SysTick.h>
#include <Spindle.h>
#include <Coolants.h>
#include <MachineCommands.h>

// derived classes
#include	<AbortCommand.h>
#include	<DoorAjarCommand.h>
#include	<LimitCommand.h>
#include	<MachineNotOkCommand.h>
#include	<StallCommand.h>

const char Pause::myName[] =
	"Pause";

const char*
	Pause::name() {
	
	return
		myName; };

Pause::Pause() {
	pauseState			= PauseINIT;
	continueExecution	= false;
};

bool	// return true when completed
	Pause::execute(
		Machine* machine ) {

	uint32_t	elapsedTime;
	Coolant*	coolant;
	
	switch ( pauseState ) {

		case PauseINIT :

		if ( continueExecution ) {
			return
				true; };

		// collect current states
		spindleRunning	= false;
		if ( machine->spindle )
			spindleRunning	= machine->spindle->running();

		for (	int i = 0;
				i < 2;
				i++ ) {
			coolantRunning[ i ]		= false;
			coolant					= machine->coolants[ i ];
			if ( coolant )
				coolantRunning[ i ]		= coolant->on; };

		pauseState	= PauseWAIT_FOR_IDLE;
		break;


		case PauseWAIT_FOR_IDLE :

		if ( continueExecution ) {
			return
				true; };

		if ( machine->isSeekingPosition() ) // Wait for axes to go idle
			break;

		if ( spindleRunning ) {
			machine->spindle->setSpindleOn( false );
			startTime	= SysTick::microSecondTimer;
			pauseState	= PauseWAIT_FOR_SPINDLE_STOP;
			break; };

		pauseState	= PauseWAIT_FOR_CONTINUATION;	
		break;


		case PauseWAIT_FOR_SPINDLE_STOP :

		if ( continueExecution ) {
			pauseState	= PauseWAIT_FOR_CONTINUATION;
			break; };

		elapsedTime		= SysTick::microSecondTimer - startTime;
		if ( elapsedTime >= machine->spindle->offDelayMs * 1000 ) {

			for (	int i = 0;
					i < 2;
					i++ ) {

				if ( coolantRunning[ i ] ) {
					coolant	= machine->coolants[ i ];
					coolant->setCoolantOn( false ); }; };

			pauseState		= PauseWAIT_FOR_CONTINUATION; };
		break;


		case PauseWAIT_FOR_CONTINUATION :
		if ( continueExecution ) {
			for (	int i = 0;
					i < 2;
					i++ ) {

				if ( coolantRunning[ i ] ) {
					coolant	= machine->coolants[ 0 ];
					coolant->setCoolantOn( true ); }; };
				
			if ( spindleRunning ) {
				machine->spindle->setSpindleOn(	true );
				startTime	= SysTick::microSecondTimer;
				pauseState	= PauseWAIT_FOR_SPINDLE_START; }

			else
				return
					true; };
		break;


		case PauseWAIT_FOR_SPINDLE_START :

		elapsedTime	= SysTick::microSecondTimer - startTime;
		if ( elapsedTime >= machine->spindle->onDelayMs * 1000 )
			return
				true;
		break; };

 	return
		false; };

void
	Pause::continueInterpolation(
		Machine*	machine ) {

	continueExecution	= true; };

void
	Pause::reportInterpolationStatus(
		Machine*	machine,
		String*		msgPtr ) {

	*msgPtr  += INTERPOLATOR_PAUSED; };
