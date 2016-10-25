
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

#include <MachineCommands.h>
#include <Machine.h>
#include <SetSpindleDirection.h>
#include <SetSpindleRun.h>
#include <SetSpindleSpeed.h>
#include <Spindle.h>
#include <InterpolationTimer.h>


void
	Machine::spindleControl(
		char*	commandPtr,
		String* msgPtr ) {

	char* data	= commandPtr + 1;

	switch ( *commandPtr ) {
		case CONFIGURE_SPINDLE :
		configureSpindle(
			data,
			msgPtr );
		break;

// in-band - acknowledge immediately
		case SET_SPINDLE_DIRECTION :
		queueCommand(
			new SetSpindleDirection( data ) );
		*msgPtr
			+=	String( SPINDLE_CONTROL )
			+	String( SET_SPINDLE_DIRECTION );
		break;

		case SPINDLE_START_STOP :
		queueCommand(
			new SetSpindleRun( data ) );
		*msgPtr
			+=	String( SPINDLE_CONTROL )
			+	String( SPINDLE_START_STOP );
		break;

		case SET_SPINDLE_SPEED :
		queueCommand(
			new SetSpindleSpeed( data ) );
		*msgPtr
			+=	String( SPINDLE_CONTROL )
			+	String( SET_SPINDLE_SPEED );
		break;

// out-of-band
		case SPINDLE_SPEED_OVERRIDE :
		spindleSpeedOverride(
			data,
			msgPtr );
		break;

		case SPINDLE_RUN_OVERRIDE :
		spindleRunOverride(
			data,
			msgPtr );
		break;

		case SPINDLE_DIRECTION_OVERRIDE :
		spindleDirectionOverride(
			data,
			msgPtr );
		break;
		
		default :
		*msgPtr
			+=	*commandPtr
			+	"E Invalid Machine Command - Ignored"; }; };

void
	Machine::configureSpindle(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( SPINDLE_CONTROL )
		+	String( CONFIGURE_SPINDLE );

	if ( spindle )
		delete spindle;
  
	spindle = new Spindle(
		this,
		data,
		msgPtr );
	if ( ! spindle->isValid() ) { // must at least have run pin
		delete spindle;
		spindle		= NULL;
		*msgPtr		+= " No Spindle"; }; };


void
	Machine::spindleSpeedOverride(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( SPINDLE_CONTROL )
		+	String( SPINDLE_SPEED_OVERRIDE );

	if ( spindle ) {
		float spindleSpeed  = 0.0;
		sscanf( data,
			" %f",
			&spindleSpeed );

		InterpolationTimer::disableInterpolationInterrupts();
			spindle->setSpindleSpeed( spindleSpeed );
		InterpolationTimer::enableInterpolationInterrupts();
		
		*msgPtr	+= " Spindle speed ";
		*msgPtr	+= String( spindleSpeed ); }; };


void
	Machine::spindleRunOverride(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( SPINDLE_CONTROL )
		+	String( SPINDLE_RUN_OVERRIDE );

	if ( spindle ) {
		uint32_t spindleRun  = 0;
		sscanf( data,
			"%lu",
			&spindleRun );
  
		InterpolationTimer::disableInterpolationInterrupts();
			spindle->setSpindleOn( spindleRun > 0 );
		InterpolationTimer::enableInterpolationInterrupts(); }; };


void
	Machine::spindleDirectionOverride(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( SPINDLE_CONTROL )
		+	String( SPINDLE_DIRECTION_OVERRIDE );

	if ( spindle ) {
		uint32_t spindleDirection  = 0;
		sscanf( data,
			"% lu",
			&spindleDirection );

		InterpolationTimer::disableInterpolationInterrupts();
			spindle->setSpindleDirection( spindleDirection > 0 ); // true is clockwise
		InterpolationTimer::enableInterpolationInterrupts(); }; };
