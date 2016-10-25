
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

#include <Dwell.h>
#include <Machine.h>
#include <Axis.h>
#include <Timers.h>
#include <MachineCommands.h>

const char Dwell::myName[] =
	"Dwell";

const char*
	Dwell::name() {
	
	return
		myName; };

Dwell::Dwell( char* data ) {
    float dwell			= 0.0;			// seconds
    sscanf( data, "%f", &dwell );

    dwellTime			= dwell * 1000000; // convert to microseconds
    pathTime			= 0;
	continueExecution	= false; };

bool
	Dwell::execute( Machine* machine ) {

	if ( continueExecution )
		return
			true;

	if ( pathTime == 0 )
		pathTime   = SysTick::microSecondTimer;

    if ( machine->isSeekingPosition() ) { // Wait for axes to go idle
		pathTime   = SysTick::microSecondTimer;
		return
			false; };
 
    return
		SysTick::microSecondTimer - pathTime >= dwellTime; };

void
	Dwell::reportInterpolationStatus(
		Machine*	machine,
		String*		msgPtr ) {

	*msgPtr  += INTERPOLATOR_DWELLING; };

void
	Dwell::continueInterpolation( Machine* machine ) {

	continueExecution  = true; };

void
	Dwell::report(
		Machine*	machine,
		String*		msgPtr ) {

    machine->reportInterpolationStatus( msgPtr ); };
