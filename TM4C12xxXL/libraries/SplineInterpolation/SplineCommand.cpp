
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

#include <SplineCommand.h>
#include <Machine.h>
#include <MachineCommands.h>
#include <Axis.h>
#include <SysTick.h>
#include <MainCommands.h>
#include <InterpolationTimer.h>
#include <Probe.h>
#include <SplineAxis.h>

// derived classes
#include <ProbingSplineCommand.h>

const char Spline::myName[] =
	"Spline";

const char*
	Spline::name() {

	return
		myName; };

// Initializer
Spline::Spline(
	char*		data,
	Axis*		firstMachineAxis,
	String*		msgPtr  ) {

	stepControl.number	= 0;
	uint32_t length		= 0;

	sscanf( data,
		"%lu%ln",
		&stepControl.number,
		&length );
	data		+= length;

	firstAxis	= NULL;

	char	coordinate[40];

	while( sscanf( data,
		" %s%ln",
		&coordinate,
		&length ) > 0 ) { // returns -1 at eof

		SplineAxis* splineAxisPtr	= new SplineAxis(
			coordinate,
			firstMachineAxis,
			msgPtr );
	
		if ( splineAxisPtr->valid() ) {
			splineAxisPtr->nextSplineAxis	= firstAxis;
			firstAxis						= splineAxisPtr; }

		else
			delete splineAxisPtr;

		data	+= length; };

	aborting	= false;

	stepControl.t					= 0.0;
	stepControl.status				= ' ';
	stepControl.progress.minimum	= 0;
	stepControl.progress.maximum	= 0;

	nextCommand		= NULL; };


Spline::~Spline() {
	while ( firstAxis ) {
        SplineAxis* splineAxisPtr	= firstAxis;
        firstAxis					= splineAxisPtr->nextSplineAxis;
        delete splineAxisPtr; }; };

void
	Spline::broadcastToSplineAxes( BroadcastToSplineAxes method ) {

	SplineAxis* splineAxisPtr  = firstAxis;
	while ( splineAxisPtr ) {
        (splineAxisPtr->*method)();
        splineAxisPtr   = splineAxisPtr->nextSplineAxis; }; };

void
	Spline::trackPadJogAbort(
		Machine*	machine ) {

	aborting = true; };


bool // This method performs program position linear and Bezier interpolation
	Spline::execute(
		Machine* machine ) {

	if ( aborting )
		return
			true;

	// The target changes for the axes included in the interpolation and linked axes
	machine->adjustStepSize( &stepControl );
	calculateProgramTarget();	// Calucalte the target position for interpolated axes

	return
		stepControl.t == 1.0; };


void
	Spline::report(
		Machine*	machine,
		String*		msgPtr ) {

	machine->reportSplineCompleted(
		&stepControl,
		msgPtr );

	if ( aborting )
		*msgPtr	+= " Aborted";

	*msgPtr	+= "\n"; };

void
	Spline::reportInterpolationStatus(
		Machine*	machine,
		String*		msgPtr ) {

	machine->reportInterpolationStatus(
		&stepControl,
		msgPtr ); };

void
	Spline::calculateProgramTarget() { // Calculate the target positions for new t

	if ( stepControl.t > 1.0 )
		stepControl.t = 1.0;

	// Intermediates for optimization
	float	t2	=
			stepControl.t
		*	stepControl.t;		// t squared

	float	it1	=
			1.0
		-	stepControl.t;		// 1 - t
	
	float	p2f	=
			3.0
		*	stepControl.t
		*	it1
		*	it1;

	float	p3f	=
			3.0
		*	t2
		*	it1;

	float	p4f	=
			t2
		*	stepControl.t;		// t cubed

	// Calculate a target position for each axis
	SplineAxis* splineAxisPtr  = firstAxis;
	while ( splineAxisPtr ) {
		splineAxisPtr->calculateProgramTarget(
			stepControl.t,
			p2f,
			p3f,
			p4f );
		splineAxisPtr	= splineAxisPtr->nextSplineAxis; }; };
