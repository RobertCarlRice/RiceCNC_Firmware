
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

#include <Machine.h>
#include <MachineCommands.h>
#include <Axis.h>
#include <Probe.h>
#include <InterpolationTimer.h>
#include <AnalogPin.h>
#include <InputPin.h>
#include <Command.h>
#include <MachineOK.h>
#include <DoorAjar.h>
#include <DoorAjarCommand.h>
#include <MainCommands.h>
#include <MySerial.h>
#include <InterpolationTimer.h>
#include <AxisTimer.h>
#include <MachineNotOkCommand.h>
#include <OutputPin.h>


void
	Machine::background(
		String* msgPtr ) {

	if ( checkSendFlag( SendAxisUnconfigured ) )
		*msgPtr  +=
			"E Axis not Configured\n";

	checkDoorAjar();
	checkMachineOK();

	while ( firstCommandInQueue != currentCommand ) {
		firstCommandInQueue->report( this, msgPtr );

		Command* copyCommand  = firstCommandInQueue;
		firstCommandInQueue   = firstCommandInQueue->nextCommand;
		delete copyCommand; };

	periodicReporting( msgPtr ); };


bool
	Machine::checkSendFlag(
		uint32_t flag ) {

	if ( sendFlags & flag ) {
		IntMasterDisable();
			sendFlags &= ~flag;
		IntMasterEnable();

		return
			true; }
  
	else
		return
			false; };


void
	Machine::checkDoorAjar() {

	if (	doorAjar
		&&	doorAjar->isAjar()
		&&	(	! currentCommand
			||	! currentCommand->isDoorAjarCommand() ) )

		insertAtTopBG( new DoorAjarCommand() ); };

void
	Machine::checkMachineOK() {

	if (	machineOK
		&&	machineOK->checkMachineNotOk()
		&&	(	! currentCommand
			||	! currentCommand->isMachineNotOkCommand() ) )

		insertAtTopBG( new MachineNotOkCommand() ); };

void
	Machine::periodicReporting(
		String* msgPtr ) {

	if  ( ( SysTick::microSecondTimer - lastStatusTime ) >
			MainCommands::periodicReportingInterval ) {

		reportAxisStatus(			msgPtr );
		reportInterpolationStatus(	msgPtr );
		reportAnalog(				msgPtr );
		reportDigital(				msgPtr );

		lastStatusTime	= SysTick::microSecondTimer; };	};

void
	Machine::reportAxisStatus(
		String* msgPtr ) {

	Axis* axisPtr  = firstAxis;
	while ( axisPtr ) {
		String msg;
		axisPtr->reportAxisStatus( &msg );
		if ( msg.length() )
			*msgPtr
				+=	msg
				+	"\n";
        axisPtr   = axisPtr->nextAxis; }; };


void
	Machine::reportInterpolationStatus(
		String* msgPtr ) {

	String msg;

	InterpolationTimer::disableInterpolationInterrupts();
		if ( currentCommand )
			currentCommand->reportInterpolationStatus( this, &msg );
	InterpolationTimer::enableInterpolationInterrupts();

	if ( ! currentCommand ) {
		if ( isSeekingPosition() )
			msg   = String( INTERPOLATOR_STOPPED );

		else if ( interpolationBusy ) {
			msg   = String( INTERPOLATOR_IDLE );
			interpolationBusy = false; }; };

	if ( msg.length() )
		*msgPtr
			+=	String( INTERPOLATOR_STATUS_REPORT )
			+	msg
			+	"\n"; };

void
	Machine::reportInterpolationStatus(
		StepControl*	stepControl,
		String*			msgPtr ) {

	char  data[ 60 ];
	snprintf( data, 60,
		"%c %lu%c t%.1f%%",
		INTERPOLATOR_RUNNING,
		stepControl->number,
		stepControl->status,
		100.0	* stepControl->t ); // percent
	*msgPtr  += String( data ); };

void
	Machine::reportAnalog(
		String* msgPtr ) {

	String msg;

	// aux analog inputs
	AnalogPin* analogPtr   = firstAnalog;
	while ( analogPtr ) {
		analogPtr->reportAnalog( &msg );
		analogPtr   = analogPtr->nextAnalogPin; };
 
	// BOOST-DRV8711 Potentiometers
	collectMessage( &Axis::reportAnalog, &msg );

	if ( msg.length() )
		*msgPtr
			+=	String( AUXILARY_CONTROL )
			+	String( ANALOG_REPORT )
			+	msg
			+	"\n"; };


void
	Machine::reportDigital(
		String* msgPtr ) {

	String msg;

	if ( machineOK )
		machineOK->reportDigital( &msg );

	if ( probe )
		probe->reportDigital( &msg );

	if ( doorAjar )
		doorAjar->reportDigital( &msg );

	// Limit switches
	collectMessage( &Axis::reportDigital, &msg );

	// Aux digital
	InputPin* inputPinPtr   = firstDigital;
	while ( inputPinPtr ) {
		inputPinPtr->reportPinChange( &msg );
		inputPinPtr  = inputPinPtr->nextInputPin; };

	if ( msg.length() )
		*msgPtr
			+=	String( AUXILARY_CONTROL )
			+	String( DIGITAL_REPORT )
			+	msg
			+	"\n"; };


void
	Machine::collectMessage(
		CollectMessage	method,
		String*			msgPtr ) {

	Axis* axisPtr  = firstAxis;
	while ( axisPtr ) {
        (axisPtr->*method)( msgPtr );
        axisPtr   = axisPtr->nextAxis; }; };


void
	Machine::reportSplineCompleted(
		StepControl*	stepControl,
		String*			msgPtr ) {

	char  data[ 30 ];

	if ( stepControl->t == 1.0 )
		snprintf( data, 30,
			"\n%c%c%lu",
			POSITION_CONTROL,
			SPLINE_COMPLETED,
			stepControl->number );

	else
		snprintf( data, 30,
			"\n%c%c%lu %0.3f%%",
			POSITION_CONTROL,
			SPLINE_COMPLETED,
			stepControl->number,
			stepControl->t * 100.0 );

	*msgPtr  += String( data ); };

