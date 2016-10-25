
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

#include <HallFeedback.h>
#include <InputPin.h>
#include <SysTick.h>
#include <BLDC3.h>
#include <PwmOutputPin.h>
#include <DRV8305Axis.h>
#include <Axis.h>
#include <MachineCommands.h>

#include <inc/hw_gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>

//#define DebugDelete 1

HallFeedback::HallFeedback(
	char*			data,
	String*			msgPtr,
	DRV8305Axis*	aAxis ) {

	axis		= aAxis;
	bldc3		= axis->motor;

	hallA		= NULL;
	hallB		= NULL;
	hallC		= NULL;

	char		hallAPinString[8];
	char		hallBPinString[8];
	char		hallCPinString[8];

// Voltage PID
	angleProportion			= 0.0;
	voltageIntegral			= 0.0;
	angleDerivative			= 0.0;

// Angle PID
	angleIntegral		= 0.0;
//	angleIntegral		= 0.0;
//	angleDerivative		= 0.0;

	speedProportion		= 0.0;

	error				= 0;
	priorError			= 0;
	integralError		= 0;
	expirationCounter	= 0;
	stallDetectVoltage	= 1024;	// 25%

	feedbackAngle		= 0;
	feedbackTorque		= 0;

	synchronized		= false;

	sscanf( data,
		" %s %s %s %f %f %f %f %lu %f",
		hallAPinString,
		hallBPinString,
		hallCPinString,

// Voltage PID
		&angleProportion,
		&voltageIntegral,
		&angleDerivative,

// Angle PID
		&angleIntegral,
//		&angleIntegral,
//		&angleDerivative,

		&stallDetectVoltage,
		&speedProportion );

	*msgPtr		+= "\n Angle Proportion ";
	*msgPtr		+= String( angleProportion );

	*msgPtr		+= "\n Angle Integral ";
	*msgPtr		+= String( angleIntegral );

	*msgPtr		+= "\n Angle Derivative ";
	*msgPtr		+= String( angleDerivative );

	*msgPtr		+= "\n Voltage Integral ";
	*msgPtr		+= String( voltageIntegral );

	*msgPtr		+= "\n Stall Detect Voltage ";
	*msgPtr		+= String( stallDetectVoltage );

	*msgPtr		+= "\n Speed Proportion ";
	*msgPtr		+= String( speedProportion );

	if (	angleProportion	== 0.0
		&&	voltageIntegral	== 0.0 ) {
		
		*msgPtr	+= " PI Invalid";
		return; };

	*msgPtr		+= "\n HallA";
	hallA	= new InputPin(
		hallAPinString,
		msgPtr );
	if ( hallA->valid() )
		hallA->fmtPin( msgPtr );
	else {
		*msgPtr		+= " invalid";
		return; };

	*msgPtr		+= "\n HallB";
	hallB	= new InputPin(
		hallBPinString,
		msgPtr );
	if ( hallB->valid() )
		hallB->fmtPin( msgPtr );
	else {
		*msgPtr		+= " invalid";
		return; };

	*msgPtr		+= "\n HallC";
	hallC	= new InputPin(
		hallCPinString,
		msgPtr );
	if ( hallC->valid() )
		hallC->fmtPin( msgPtr );
	else {
		delete hallC;
		hallC = NULL;
		*msgPtr		+= " invalid";
		return; };

	// initialize pin interrupt callback
	hallA->stepCounter	= this;
	hallB->stepCounter	= this;
	hallC->stepCounter	= this;

	hallState	= HallStart;

	hallA->enableBothEdgeInterrupt();
	hallB->enableBothEdgeInterrupt();
	hallC->enableBothEdgeInterrupt();

	initPhaseTransitionTable(); };
	
void
	HallFeedback::initPhaseTransitionTable() {

	synchronized	= false;
	axis->enable8305( true );

	for ( int i = 0;
		i < 12;
		i++ ) {

		phaseTransition[ i ] = 0xFFFF; }; };

bool
	HallFeedback::valid() {
	
	return
		hallA
	&&	hallB
	&&	hallC; };

HallFeedback::~HallFeedback() {

#ifdef DebugDelete
	Serial.println( " delete HallFeedback" );
#endif

	if ( hallA )
		delete hallA;

	if ( hallB )
		delete hallB;

	if ( hallC )
		delete hallC; };

void
	HallFeedback::reportHallPinChange(
		String* msgPtr ) {
	
	if ( hallA )
		hallA->reportPinChange( msgPtr );

	if ( hallB )
		hallB->reportPinChange( msgPtr );

	if ( hallC )
		hallC->reportPinChange( msgPtr ); };

void
	HallFeedback::readHallPhasing(
		char*		data,
		String*		msgPtr ) {

	for ( int i = 0;
		i < 12;
		i++ ) {

		snprintf( data, 10,
			" %x",
			phaseTransition[ i ] );
		*msgPtr		+= String( data ); };

	*msgPtr		+= "\n"; };

void
	HallFeedback::pinISR(
		InputPin* pin ) {

	switch ( hallState ) {

		case	HallStart :
		
		if		( pin == hallA )
			hallState = pin->readPin() ?
				S_A1
			:	S_A0;

		else if ( pin == hallB )
			hallState = pin->readPin() ?
				S_B1
			:	S_B0;

		else if ( pin == hallC )
			hallState = pin->readPin() ?
				S_C1
			:	S_C0;
		break;

		case	S_A1 :
		
		if		( pin == hallA ) {
			hallState = S_A0;
			break; }

		case	CW_A1 :

		if		( pin == hallA ) {
			if	( ! pin->readPin() )
				hallState = CCW_A0; }

		else if ( pin == hallB )
			hallState = pin->readPin() ?
				CW_B1
			:	CCW_B0;

		else
			hallState = pin->readPin() ?
				CCW_C1
			:	CW_C0;
		break;

		case	S_B1 :
		
		if		( pin == hallB ) {
			hallState = S_B0;
			break; }

		case	CW_B1 :

		if		( pin == hallB ) {
			if	( ! pin->readPin() )
				hallState = CCW_B0; }

		else if ( pin == hallC )
			hallState = pin->readPin() ?
				CW_C1
			:	CCW_C0;

		else
			hallState = pin->readPin() ?
				CCW_A1
			:	CW_A0;
		break;

		case	S_C1 :
		
		if		( pin == hallC ) {
			hallState = S_C0;
			break; }

		case	CW_C1 :

		if		( pin == hallC ) {
			if	( ! pin->readPin() )
				hallState = CCW_C0; }

		else if ( pin == hallA )
			hallState = pin->readPin() ?
				CW_A1
			:	CCW_A0;

		else
			hallState = pin->readPin() ?
				CCW_B1
			:	CW_B0;
		break;
	
		case	S_A0 :
		
		if		( pin == hallA ) {
			hallState = S_A0;
			break; }

		case	CW_A0 :

		if		( pin == hallA ) {
			if	( pin->readPin() )
				hallState = CCW_A1; }

		else if ( pin == hallB )
			hallState = pin->readPin() ?
				CW_B1
			:	CCW_B0;

		else
			hallState = pin->readPin() ?
				CW_C1
			:	CCW_C0;
		break;

		case	S_B0 :
		
		if		( pin == hallB ) {
			hallState = S_B0;
			break; }

		case	CW_B0 :

		if		( pin == hallB ) {
			if	( pin->readPin() )
				hallState = CCW_B1; }

		else if ( pin == hallC )
			hallState = pin->readPin() ?
				CW_C1
			:	CCW_C0;

		else
			hallState = pin->readPin() ?
				CW_A1
			:	CCW_A0;
		break;

		case	S_C0 :
		
		if		( pin == hallC ) {
			hallState = S_C0;
			break; }

		case	CW_C0 :

		if		( pin == hallC ) {
			if	( pin->readPin() )
				hallState = CCW_C1; }

		else if ( pin == hallA )
			hallState = pin->readPin() ?
				CW_A1
			:	CCW_A0;

		else
			hallState = pin->readPin() ?
				CW_B1
			:	CCW_B0;
		break;

		case	CCW_A1 :

		if		( pin == hallA ) {
			if	( ! pin->readPin() )
				hallState = CW_A0; }

		else if ( pin == hallB )
			hallState = pin->readPin() ?
				CW_B1
			:	CCW_B0;
		
		else
			hallState = pin->readPin() ?
				CCW_C1
			:	CW_C0;
		break;

		case	CCW_B1 :

		if		( pin == hallB ) {
			if	( ! pin->readPin() )
				hallState = CW_B0; }

		else if ( pin == hallC )
			hallState = pin->readPin() ?
				CW_C1
			:	CCW_C0;

		else
			hallState = pin->readPin() ?
				CCW_A1
			:	CW_A0;
		break;

		case	CCW_C1 :

		if		( pin == hallC ) {
			if	( ! pin->readPin() )
				hallState = CW_C0; }

		else if ( pin == hallA )
			hallState = pin->readPin() ?
				CW_A1
			:	CCW_A0;

		else
			hallState = pin->readPin() ?
				CCW_B1
			:	CW_B0;
		break;
	
		case	CCW_A0 :

		if		( pin == hallA ) {
			if	( pin->readPin() )
				hallState = CW_A1; }

		else if ( pin == hallB )
			hallState = pin->readPin() ?
				CCW_B1
			:	CW_B0;

		else
			hallState = pin->readPin() ?
				CCW_C1
			:	CW_C0;
		break;

		case	CCW_B0 :

		if		( pin == hallB ) {
			if	( pin->readPin() )
				hallState = CW_B1; }

		else if ( pin == hallC )
			hallState = pin->readPin() ?
				CCW_C1
			:	CW_C0;

		else
			hallState = pin->readPin() ?
				CCW_A1
			:	CW_A0;
		break;

		case	CCW_C0 :

		if		( pin == hallC ) {
			if	( pin->readPin() )
				hallState = CW_C1; }

		else if ( pin == hallA )
			hallState = pin->readPin() ?
				CCW_A1
			:	CW_A0;

		else
			hallState = pin->readPin() ?
				CCW_B1
			:	CW_B0;
		break; };

	if ( hallState >= HallStart )
		return;

	if ( synchronized )
		calculateFeedback();

	else if ( phaseTransition[ hallState ] == 0xFFFF ) { // initial transition
		phaseTransition[ hallState ] =
			bldc3->armMagneticAngle;
		if ( initCount() == 12 )
			synchronized	= true; }; };

void
	HallFeedback::calculateFeedback() {
	
	// New sample - reset counter
	expirationCounter		= 600;	// decremented @ 2 kHz rate

	uint32_t currentPhase	= bldc3->armMagneticAngle;
	uint32_t priorPhase		= phaseTransition[ hallState ];

	int32_t change	= currentPhase - priorPhase;
	if ( change == 0 )
		return;

	if		( change >= BLDC3::MS50_180 )
		change	-= BLDC3::MS50_360;

	else if ( change < -BLDC3::MS50_180 )
		change	+= BLDC3::MS50_360;

// error is positive if motor needs to go faster CW or slower CCW
	error			+= change;
	phaseTransition[ hallState ] = currentPhase;
	
	integralError	+= error;

	feedbackTorque	= abs (
			integralError	*	voltageIntegral );

	feedbackAngle =
			change			*	angleDerivative
		+	error			*	angleProportion
		+	integralError	*	angleIntegral;

	if ( feedbackAngle > BLDC3::MS50_90 )
		feedbackAngle	= BLDC3::MS50_90;
	
	else if ( feedbackAngle < -BLDC3::MS50_90 )
		feedbackAngle	= -BLDC3::MS50_90;
	
	feedbackAngle	-= error;

Serial.print( " e" );
Serial.print( error );
Serial.print( " E" );
Serial.print( integralError );
Serial.print( " <" );
Serial.print( feedbackAngle );
Serial.print( " t" );
Serial.println( feedbackTorque );
	};

bool
	HallFeedback::calculateHallFeedback(
		Machine*	machine ) {

	if ( expirationCounter == 0 )
		return
			false;
	expirationCounter--;
	
	bldc3->assertedVoltage		=
			feedbackTorque
		+	bldc3->holdTorque;

	bldc3->assertedLeadAngle	+= feedbackAngle;

	int32_t overVoltage =
			bldc3->assertedVoltage
		-	stallDetectVoltage;

	if ( overVoltage > 0 ) {
		bldc3->assertedVoltage	-= overVoltage;
		axis->drv8305AxisState	= AXIS_VOLTAGE_LIMITED;
		axis->setFlag( VELOCITY_CHANGED );

		// at max torque either can't keep up with load
		// or motor is being driven overspeed by inertial load
		if ( feedbackTorque >= 0.0 ) {
			axis->mvStepsPerSecF	-=
				overVoltage
			*	speedProportion;

			if ( axis->mvStepsPerSecF > 0 )
				return
					true; }

		else {
			axis->mvStepsPerSecF	+=
				overVoltage
			*	speedProportion;

			if ( axis->mvStepsPerSecF < 0 )
				return
					true; };

		// Shutdown
		axis->drv8305AxisState			= AXIS_STALLED;
		machine->stallDetected();
		axis->setTargetVelocity( 0.0 );
		synchronized		= false;
		expirationCounter	= 0;
		axis->enable8305( false );
		return
			false; };
	
	return
		true; };
	
uint
	HallFeedback::initCount() {
	
	uint count	= 0;
	for ( int i = 0;
			i < 12;
			i++ ) {
		if ( phaseTransition[ i ] != 0xFFFF )
			count++; };

	if ( count == 12 ) {
		axis->drv8305AxisState	= AXIS_RUNNING; };

	return
		count; };
