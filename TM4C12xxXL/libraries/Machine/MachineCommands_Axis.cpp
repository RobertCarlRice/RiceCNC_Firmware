
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
#include <AdjustAxisPosition.h>
#include <CancelTransforms.h>
#include <LinkAxisCommand.h>
#include <SetRotationalDiameter.h>
#include <dSpinAxis.h>
#include <DRV8711Axis.h>
#include <DRV8305Axis.h>
#include <DiscreteAxis.h>
#include <QuadratureAxis.h>
#include <ThreePhaseAxis.h>
#include <TorchHeightControl.h>
#include <StepDirectionAxis.h>
#include <PowerStepAxis.h>
#include <SetHallSensorPhasing.h>
#include <SynchronizeQuadratureCounter.h>


void
	Machine::axisControl(
		char*		commandPtr,
		String*		msgPtr ) {

	char* data	= commandPtr + 1;
	
	*msgPtr
		+=	String( AXIS_CONTROL )
		+	String( *commandPtr );

	switch ( *commandPtr ) {

// in-band - acknowledge immediately
		case ADJUST_AXIS_POSITION: // not used
		queueCommand(
			new AdjustAxisPosition( data ) );
		break;

		case CANCEL_TRANSFORMS:
		queueCommand(
			new CancelTransforms() );
		break;

		case LINK_AXIS:
		queueCommand(
			new LinkAxisCommand( data ) );
		break;

		case SET_ROTATIONAL_DIAMETER:
		queueCommand(
			new SetRotationalDiameter( data ) );
		break;

		case SET_HALL_PHASING:
		queueCommand(
			new SetHallSensorPhasing( data ) );
		break;

		case SYNCHRONIZE_QUADRATURE_CTR:
		queueCommand(
			new SynchronizeQuadratureCounter( data ) );
		break;

		case REMOVE_AXIS_COMMAND :
		removeAxisCommand(
			data,
			msgPtr );
		break;

		case CONFIGURE_AXIS :
		configureAxis(
			data,
			msgPtr );
		break;

		case ADD_LIMIT_SWITCH :
		addLimitSwitch(
			data,
			msgPtr );
		break;

		case ADD_SLAVE_INPUT :
		addSlaveInput(
			data,
			msgPtr );
		break;

		case ADD_CLOSED_LOOP :
		addClosedLoop(
			data,
			msgPtr );
		break;

		case ADD_MOTOR :
		addMotor(
			data,
			msgPtr );
		break;
		
		case UPDATE_AXIS :
		updateAxis(
			data,
			msgPtr );
		break;

		default :
		*msgPtr
			+=	" Invalid Machine Command - Ignored"; }; };


void
	Machine::configureAxis(
		char*		data,
		String*		msgPtr ) {

	removeAxis(
		*data,
		msgPtr );

	Axis*	axisPtr;
	switch ( *(data + 2) ) { // Switch on the interface type

		case AXIS_TYPE_POWER_STEP :
		axisPtr   = new PowerStepAxis(
			data,
			msgPtr );
		break;

		case AXIS_TYPE_dSPIN :
		axisPtr   = new dSpinAxis(
			data,
			msgPtr );
		break;

		case AXIS_TYPE_DRV8711 :
		axisPtr   = new DRV8711Axis(
			data,
			msgPtr );
		break;

		case AXIS_TYPE_DRV8305 :
		axisPtr   = new DRV8305Axis(
			data,
			msgPtr );
		break;

		case AXIS_TYPE_DISCRETE :
		axisPtr   = new DiscreteAxis(
			data,
			msgPtr );
		break;

		case AXIS_TYPE_QUADRATURE :
		axisPtr   = new QuadratureAxis(
			data,
			msgPtr );
		break;

		case AXIS_TYPE_THREE_PHASE :
		axisPtr   = new ThreePhaseAxis(
			data,
			msgPtr );
		break;

		case AXIS_TYPE_TORCH :
		axisPtr   = new TorchHeightControl(
			data,
			msgPtr,
			this );
		break;

		case AXIS_TYPE_STEP_DIR :
		default :
		axisPtr   = new StepDirectionAxis(
			data,
			msgPtr ); };

	if ( axisPtr->isConfigured() ) {
		axisPtr->nextAxis	= firstAxis;  // insert at top of chain - order not significant
		firstAxis			= axisPtr;

		axisPtr->addMotorStepInterrupt(); }

	else
		delete axisPtr; };


void
	Machine::addLimitSwitch(
		char*		data,
		String*		msgPtr ) {

	char axisNameC	= *data; // first char

	Axis* axisPtr	= firstAxis;
	while( axisPtr ) {
		if ( axisPtr->axisNameC == axisNameC ) {
			axisPtr->addLimitSwitch(
				data + 1,
				msgPtr );
			return; };
		axisPtr = axisPtr->nextAxis; };

	*msgPtr  += "\nE axis not configured\n"; };


void
	Machine::addSlaveInput(
		char*		data,
		String*		msgPtr ) {

	char axisNameC	= *data; // first char

	Axis* axisPtr	= firstAxis;
	while( axisPtr ) {
		if ( axisPtr->axisNameC == axisNameC ) {
			axisPtr->addSlaveInput(
				data + 1,
				msgPtr );
			return; };
		axisPtr = axisPtr->nextAxis; };

	*msgPtr  += "\nE axis not configured\n"; };

void
	Machine::addClosedLoop(
		char*		data,
		String*		msgPtr ) {

	char axisNameC	= *data; // first char

	Axis* axisPtr	= firstAxis;
	while( axisPtr ) {
		if ( axisPtr->axisNameC == axisNameC ) {
			axisPtr->addClosedLoop(
				data + 1,
				msgPtr );
			return; };
		axisPtr = axisPtr->nextAxis; };

	*msgPtr  += "\nE axis not configured\n"; };


void
	Machine::addMotor(
		char*		data,
		String*		msgPtr ) {

	char axisNameC	= *data; // first char

	Axis* axisPtr	= firstAxis;
	while( axisPtr ) {
		if ( axisPtr->axisNameC == axisNameC ) {
			axisPtr->addMotor(
				data + 1,
				msgPtr );
			return; };
		axisPtr = axisPtr->nextAxis; };

	*msgPtr  += "\nE axis not configured\n"; };


void
	Machine::updateAxis(
		char*		data,
		String*		msgPtr ) {

	char axisNameC	= *data; // first char

	Axis* axisPtr	= firstAxis;
	while( axisPtr ) {
		if ( axisPtr->axisNameC == axisNameC ) {
			axisPtr->updateAxis(
				data,
				msgPtr );
			return; };
		axisPtr = axisPtr->nextAxis; };

	*msgPtr  += "\nE axis not configured\n"; };


void
	Machine::removeAxisCommand(
		char*		data,
		String*		msgPtr ) {

	char axisNameC = *data;
	removeAxis( axisNameC, msgPtr );

	*msgPtr
		+=	String( axisNameC )
		+	" Unconfigured";  };
  
void
	Machine::removeAxis(
		char		axisNameC,
		String*		msgPtr ) {

	Axis*	existingAxisPtr;
	IntMasterDisable();
		Axis** axisPtrPtr  = &firstAxis;
		while( true ) {
			existingAxisPtr		= *axisPtrPtr;
			if ( ! existingAxisPtr )
				break;
			if ( existingAxisPtr->axisNameC == axisNameC ) {
				*axisPtrPtr		= existingAxisPtr->nextAxis; // Bypass
				break; };
			axisPtrPtr = &existingAxisPtr->nextAxis; };
	IntMasterEnable();

	if ( existingAxisPtr ) {
		abort(); // Stop anything that is running

		*msgPtr
			+=	" Removing prior axis "
			+	String( existingAxisPtr->axisNameC )
			+	"\n";
		delete existingAxisPtr; }; };
