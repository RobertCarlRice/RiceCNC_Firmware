
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

#include <QuadratureFeedback.h>
#include <QuadratureCounter1X.h>
#include <QuadratureCounter4X.h>
#include <InputPin.h>
#include <DRV8305Axis.h>
#include <BLDC3.h>
#include <MachineCommands.h>
#include <QEI4X.h>

#include <inc/hw_gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>

//#define DebugDelete 1

QuadratureFeedback::QuadratureFeedback(
	char*			data,
	String*			msgPtr,
	DRV8305Axis*	aDrv8305axis,
	char			counterType ) {

	drv8305axis		= aDrv8305axis;
	bldc3			= drv8305axis->motor;

	stepCounter			= NULL;
	synchronized		= false;
	priorError			= 0.0;
	integralError		= 0.0;

	leading				= false;
	stepAdjustment		= 0;

	voltageProportion	= 0.0;
	voltageIntegral		= 0.0;
	voltageDerivative	= 0.0;

	stallDetectVoltage	= 1024;	// 25%
	fractionalCount		= 0.0;
	adaptAngle			= 0;

	char	q1PinString[8];
	char	q2PinString[8];

	sscanf( data,
		" %s %s %lu %lu %f %f %f %lu",
		q1PinString,
		q2PinString,

		&ppr,
		&adaptAngle,
		&voltageProportion,
		&voltageIntegral,
		&voltageDerivative,
		&stallDetectVoltage );

	*msgPtr		+= "\n Voltage Proportion ";
	*msgPtr		+= String( voltageProportion );

	*msgPtr		+= "\n Voltage Integral ";
	*msgPtr		+= String( voltageIntegral );

	*msgPtr		+= "\n Voltage Derivative ";
	*msgPtr		+= String( voltageDerivative );

	*msgPtr		+= "\n Stall Detect Voltage ";
	*msgPtr		+= String( stallDetectVoltage );

	*msgPtr		+= "\n PPR ";
	*msgPtr		+= String( ppr );

	*msgPtr		+= " Adapt Angle ";
	*msgPtr		+= String( adaptAngle );

	adaptAngle	*= BLDC3::MS50_360;
	adaptAngle	/= 360;

	if (	voltageProportion == 0.0
		&&	voltageIntegral == 0.0 ) {
		
		*msgPtr		+= "\n PI Invalid ";
		return; };
		
	float	scale	=	// 1.0 for 600 ppr
		float(	bldc3->polePairs
				*	BLDC3::MS50_360 )
			/	ppr;

	if ( q1PinString[ 0 ] == 'Q' ) {
		scale			/= 4.0;
		*msgPtr			+= " Scaling ";
		*msgPtr			+= String( scale );
		
		stepCounter		= new QEI4X( scale );
		*msgPtr			+= "\n QEI4X";
		return; };
	
	// Not high speed
    InputPin* q1InputPin =
		new InputPin(
				q1PinString,
				msgPtr );

	InputPin* q2InputPin =
		new InputPin(
				q2PinString,
				msgPtr );

	if (	q1InputPin->valid()
		&&	q2InputPin->valid() ) {

		*msgPtr		+= "\n Q1";
		q1InputPin->fmtPin( msgPtr );

		*msgPtr		+= "\n Q2";
		q2InputPin->fmtPin( msgPtr );

		if ( counterType == FEEDBACK_TYPE_QUADRATURE_4X ) {
			*msgPtr		+= " 4X";
			scale		/= 4.0;

			stepCounter = new QuadratureCounter4X(
				q1InputPin,
				q2InputPin,
				scale ); }

		else {
			*msgPtr		+= " 1X";
			stepCounter = new QuadratureCounter1X(
				q1InputPin,
				q2InputPin,
				scale ); };

		*msgPtr		+= " Scaling ";
		*msgPtr		+= String( scale ); }

	else {
		delete q1InputPin;
		delete q2InputPin; }; };

bool
	QuadratureFeedback::valid() {
	
	return
		stepCounter != NULL; };

QuadratureFeedback::~QuadratureFeedback() {

#ifdef DebugDelete
	Serial.println( " delete QuadratureFeedback" );
#endif
	// stepCounter owns q1InputPin & q2InputPin
	if ( stepCounter )
		delete stepCounter; };


bool
	QuadratureFeedback::calculateFeedback(	// 2 kHz
		Machine*	machine ) {

	if ( ! synchronized )
		return
			false;

// Moved from motorStepIsr()
	float count			= stepCounter->readScaledCount();
	if ( count ) {
		drv8305axis->stepped( count );

		fractionalCount		+= count;
		int32_t deltaCount	= drv8305axis->roundToInt( fractionalCount );

		if ( deltaCount ) {
			fractionalCount		-= deltaCount;
			bldc3->stepArmature( deltaCount ); }; };
//
	float error				= drv8305axis->indexerOffsetF;
	integralError			+= error;

	float derivativeError	= error - priorError;
	priorError				= error;

	// Adaptive logic
	if (	adaptAngle
		&&	bldc3->assertedVoltage > 50 ) {

		if ( leading ) {	// adjust at 1 kHz rate
			leading						= false;
			bldc3->assertedLeadAngle	-= adaptAngle;

			if		( derivativeError > 0.0 ) {
				stepAdjustment--;
				stepCounter->adjust( -1 );
//				stepCounter->stepCount++;
			}

			else if ( derivativeError < 0.0 ) {
				stepAdjustment++;
				stepCounter->adjust( 1 );
//				stepCounter->stepCount--;
			}; }

		else {
			leading						= true;
			bldc3->assertedLeadAngle	+= adaptAngle; }; };

	int32_t torque	=
			error			*	voltageProportion
		+	integralError	*	voltageIntegral
		+	derivativeError	*	voltageDerivative;

	bldc3->assertedVoltage		= abs( torque );

	bldc3->assertedLeadAngle	+= torque >= 0 ?
			BLDC3::MS50_90
		:	-BLDC3::MS50_90;

	int32_t overVoltage =
			bldc3->assertedVoltage
		-	stallDetectVoltage;

	bool stall	= false;

	if ( overVoltage > 0 ) {
		bldc3->assertedVoltage			-= overVoltage;
		drv8305axis->drv8305AxisState	= AXIS_VOLTAGE_LIMITED;
		drv8305axis->setFlag( VELOCITY_CHANGED );

		// at max torque either can't keep up with load
		// or motor is being driven overspeed by inertial load
		if ( torque >= 0.0 ) {
			drv8305axis->mvStepsPerSecF	-=
				overVoltage
			*	voltageIntegral;

			if ( drv8305axis->mvStepsPerSecF < 0 )
				stall = true; }

		else {
			drv8305axis->mvStepsPerSecF	+=
				overVoltage
			*	voltageIntegral;

			if ( drv8305axis->mvStepsPerSecF > 0 )
				stall = true; }; };

	if ( stall ) {
		drv8305axis->drv8305AxisState	= AXIS_STALLED;
		machine->stallDetected();
		drv8305axis->setTargetVelocity( 0.0 );
		drv8305axis->enable8305( false );
		return
			false; };

	bldc3->assertTorque();
	return
		true; };
