
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

#include <Command.h>
#include <Machine.h>

// derived classes
#include <AdjustAxisPosition.h>
#include <CancelTransforms.h>
#include <Dwell.h>
#include <LinkAxisCommand.h>
#include <Pause.h>
#include	<AbortCommand.h>
#include	<DoorAjarCommand.h>
#include	<LimitCommand.h>
#include	<MachineNotOkCommand.h>
#include	<StallCommand.h>
#include <PlasmaStartStop.h>
#include <SetArcVector.h>
#include <SetCoolant.h>
#include <SetFeedrate.h>
#include <SetMaxInterpolationSpeed.h>
#include <SetRotationalDiameter.h>
#include <SetSpindleDirection.h>
#include <SetSpindleRun.h>
#include <SetSpindleSpeed.h>
#include <SlaveModeCommand.h>
#include <SplineCommand.h>
#include	<ProbingSplineCommand.h>

const char Command::myName[] =
	"Command";

const char*
	Command::name() {
	
	return
		myName; };

Command::~Command() {};

bool
	Command::isDoorAjarCommand() {

	return
		false; };

bool
	Command::isMachineNotOkCommand() {

	return
		false; };

bool
	Command::isLimitCommand() {

	return
		false; };

bool
	Command::isStallCommand() {

	return
		false; };

bool
	Command::execute( Machine* machine ) {

    return
		true; };
