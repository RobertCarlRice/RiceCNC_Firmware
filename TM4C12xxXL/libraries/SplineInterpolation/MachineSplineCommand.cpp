
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

#include <MachineSplineCommand.h>
#include <Machine.h>
#include <MachineCommands.h>
#include <SysTick.h>
#include <MainCommands.h>
#include "InterpolationTimer.h"

const char MachineSpline::myName[] =
	"MachineSpline";

const char*
	MachineSpline::name() {

	return
		myName; };

MachineSpline::MachineSpline(
	char*			data,
	Axis*			firstMachineAxis,
	String*			msgPtr ) {

	stepControl.number	= 0;
	uint32_t length		= 0;
	sscanf( data,
		"%lu%ln",
		&stepControl.number,
		&length );
	data				+= length;

	firstMachineSplineAxis	= NULL;
	char   coordinate[20];

	while( sscanf( data,
			" %s%ln",
			&coordinate,
			&length ) > 0 ) { // returns -1 at eof

		MachineSplineAxis* splineAxisPtr	= new MachineSplineAxis(
			coordinate,
			firstMachineAxis,
			msgPtr );
    
		if ( splineAxisPtr->valid() ) {
			splineAxisPtr->nextSplineAxis   = firstMachineSplineAxis;
			firstMachineSplineAxis          = splineAxisPtr; }

		else
			delete splineAxisPtr;

		data				+= length; };

	aborting				= false;

	stepControl.t					= 0.0;
	stepControl.status				= ' ';
	stepControl.progress.minimum	= 0;
	stepControl.progress.maximum	= 0;

	nextCommand				= NULL; };

MachineSpline::~MachineSpline() {
	while ( firstMachineSplineAxis ) {
        MachineSplineAxis* splineAxisPtr	= firstMachineSplineAxis;
        firstMachineSplineAxis				= splineAxisPtr->nextSplineAxis;
        delete splineAxisPtr; }; };

void
	MachineSpline::trackPadJogAbort(
		Machine* machine ) {

	aborting	= true; };


bool // This method performs machine position linear interpolation
	MachineSpline::execute(
		Machine* machine ) {

	if ( aborting )
		return
			true;

	// The target changes only for the axes included in the interpolation
	machine->adjustStepSize( &stepControl );
	calculateTarget();	// Calucalte the target machine position for interpolated axes

	return
		stepControl.t == 1.0; };

void
	MachineSpline::report(
		Machine*	machine,
		String*		msgPtr ) {

	machine->reportSplineCompleted( &stepControl, msgPtr );
	*msgPtr
		+= "\n"; };


void
	MachineSpline::reportInterpolationStatus(
		Machine*	machine,
		String*		msgPtr ) {

	machine->reportInterpolationStatus( &stepControl, msgPtr ); };

void
	MachineSpline::calculateTarget() { // return interpolation speed acceleration

	if ( stepControl.t > 1.0 )
		stepControl.t = 1.0;
 
	// Calculate a target position for each axis
	MachineSplineAxis* splineAxisPtr  = firstMachineSplineAxis;
	while ( splineAxisPtr ) {
		splineAxisPtr->calculateTarget( stepControl.t );
		splineAxisPtr   = splineAxisPtr->nextSplineAxis; }; };
