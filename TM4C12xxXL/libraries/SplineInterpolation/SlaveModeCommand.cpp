
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

#include <SlaveModeCommand.h>
#include <Machine.h>
#include <MachineCommands.h>
#include <SysTick.h>
#include <MainCommands.h>
#include <InterpolationTimer.h>
#include <StepCounter.h>
#include <Axis.h>

const char SlaveModeCommand::myName[] =
	"SlaveMode";

const char*
	SlaveModeCommand::name() {

	return
		myName; };

SlaveModeCommand::SlaveModeCommand(
	char*			data,
	Axis*			aFirstMachineAxis,
	String*			msgPtr ) {

	firstMachineAxis	= aFirstMachineAxis;

	aborting		= false;
	starting		= true;
	nextCommand		= NULL; };

SlaveModeCommand::~SlaveModeCommand() {};

void
	SlaveModeCommand::abort(
		Machine* machine ) {

	aborting	= true; };


bool // Called on interpolation ISR
	SlaveModeCommand::execute(
		Machine* machine ) {

	if ( starting ) {
		// read the counts to clear them
		Axis* axisPtr  = firstMachineAxis;
		while ( axisPtr ) {
			StepCounter* slaveStepCounter	= axisPtr->slaveStepCounter;
			if ( slaveStepCounter )
				slaveStepCounter->readScaledCount();

			axisPtr   = axisPtr->nextAxis; };

		starting	= false; };

	if ( aborting )
		return
			true;

	Axis* axisPtr  = firstMachineAxis;
	while ( axisPtr ) {
		axisPtr->addSlaveInput();
		axisPtr   = axisPtr->nextAxis; };

	return	// User must abort to exit
		false; };

void
	SlaveModeCommand::reportInterpolationStatus(
		Machine*	machine,
		String*		msgPtr ) {

	*msgPtr		+= INTERPOLATOR_SLAVE; };

void
	SlaveModeCommand::report(
		Machine*	machine,
		String*		msgPtr ) {

	*msgPtr		+= " Exit Slave Mode"; };
