
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

#include <CurrentFeedback.h>
#include <AnalogPin.h>
#include <BLDC3.h>

#include <inc/hw_gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>

//#define DebugDelete 1

CurrentFeedback::CurrentFeedback(
	char*		data,
	String*		msgPtr ) {

	csa		= NULL;
	csb		= NULL;
	csc		= NULL;

	powerLevelsU.powerLevels	= 0;
	maxCurrentScale				= 0;
		
	char	csaPinString	[8];
	char	csbPinString	[8];
	char	cscPinString	[8];

	sscanf( data,
		" %s %s %s %x %lu",
		csaPinString,
		csbPinString,
		cscPinString,
		&powerLevelsU.powerLevels,
		&maxCurrentScale );

	*msgPtr		+= "\n csA";
	csa			= new AnalogPin(
		csaPinString,
		msgPtr );
	if ( ! csa->valid() )
		return;

	*msgPtr		+= "\n csB";
	csb			= new AnalogPin(
		csbPinString,
		msgPtr );
	if ( ! csb->valid() )
		return;

	*msgPtr		+= "\n csC";
	csc			= new AnalogPin(
		cscPinString,
		msgPtr );
	if ( ! csc->valid() ) {
		delete csc;
		csc		= NULL;
		return; };
	
	*msgPtr		+= "\n Current Limits\n  Scaling ";
	*msgPtr		+= String( maxCurrentScale );
	*msgPtr		+= "\n  Holding ";
	*msgPtr		+= String( powerLevelsU.st.holdCurrent );
	*msgPtr		+= "\n  Running ";
	*msgPtr		+= String( powerLevelsU.st.constantCurrent );
	*msgPtr		+= "\n  Accelerating ";
	*msgPtr		+= String( powerLevelsU.st.acceleratingCurrent );
	*msgPtr		+= "\n  Decelerating ";
	*msgPtr		+= String( powerLevelsU.st.deceleratingCurrent ); };

bool
	CurrentFeedback::valid() {
	
	return
		csa
	&&	csb
	&&	csc; };

CurrentFeedback::~CurrentFeedback() {

#ifdef DebugDelete
	Serial.println( " delete CurrentFeedback" );
#endif
	if ( csa )
		delete csa;

	if ( csb )
		delete csb;

	if ( csc )
		delete csc; };

bool
	CurrentFeedback::currentLimitExceeded( BLDC3* motor ) {

	return
		motorCurrent( motor ) > maxCurrentCompare; };

uint32_t
	CurrentFeedback::motorCurrent( BLDC3* motor ) {
	
	// Only need to check current on low phase
	if ( csa->enabled )
		return
			csa->analogValue();

	if ( csb->enabled )
		return
			csb->analogValue();

	return
		csa->analogValue(); };

void
	CurrentFeedback::motorStateChanged(
		Axis::MotorState motorState ) {

	switch ( motorState ) {

		case Axis::HOLDING :
		maxCurrentCompare =
				maxCurrentScale
			*	powerLevelsU.st.holdCurrent;
		break;

		case Axis::CONSTANT_SPEED :
		maxCurrentCompare =
				maxCurrentScale
			*	powerLevelsU.st.constantCurrent;
		break;

		case Axis::ACCELERATING :
		maxCurrentCompare =
				maxCurrentScale
			*	powerLevelsU.st.acceleratingCurrent;
		break;

		case Axis::DECELERATING :
		maxCurrentCompare =
				maxCurrentScale
			*	powerLevelsU.st.deceleratingCurrent;
		break;

		default:
		maxCurrentCompare = 0; }; };
