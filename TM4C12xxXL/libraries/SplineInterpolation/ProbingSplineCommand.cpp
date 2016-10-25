
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

#include <ProbingSplineCommand.h>
#include <Machine.h>
#include <MachineCommands.h>
#include <Axis.h>
#include <SysTick.h>
#include <MainCommands.h>
#include "InterpolationTimer.h"
#include <Probe.h>

const char ProbingSplineCommand::myName[] =
	"ProbingSpline";

const char*
	ProbingSplineCommand::name() {

	return
		myName; };

// Initializer
ProbingSplineCommand::ProbingSplineCommand(
	char*		data,
	Axis*		firstMachineAxis,
	String*		msgPtr  )

:	Spline(
	data,
	firstMachineAxis,
	msgPtr ) {

	probeStop	= false; };


bool // This method performs program position linear and Bezier interpolation
	ProbingSplineCommand::execute(
		Machine* machine ) {

	if ( ! machine->probe )
		return
			true;

	if ( machine->probe->probeStop() ) {
		probeStop	= true;

		return
			true; };

	return
		Spline::execute( machine ); };


void
	ProbingSplineCommand::report(
		Machine*	machine,
		String*		msgPtr ) {

	machine->reportSplineCompleted(
		&stepControl,
		msgPtr );

	if ( aborting )
		*msgPtr	+= " Aborted";

	if ( ! machine->probe )
		*msgPtr	+= " P0";

	else if ( ! probeStop )
		*msgPtr	+= " P1";

	else if ( stepControl.t )
		*msgPtr	+= " P2";

	else
		*msgPtr	+= " P3";

	*msgPtr	+= "\n"; };
