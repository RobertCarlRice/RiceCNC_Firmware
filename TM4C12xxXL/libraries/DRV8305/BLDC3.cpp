
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

#include <BLDC3.h>
#include <StepCounter.h>

#include <InputPin.h>
#include <OutputPin.h>
#include <SoftPwmOutputPin.h>
#include <AnalogPin.h>

#include <Timers.h>
#include <DRV8305Axis.h>
#include <MachineCommands.h>
#include <HallFeedback.h>
#include <CurrentFeedback.h>
#include <QuadratureFeedback.h>

#include <inc/hw_gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>

//#define DebugDelete 1

/* To understand this eight-step commutation scheme think of a triangle rolling
slowly on a flat surface. One corner (sometimes two) is referenced to zero for
efficiency, while the level of the other two verticies is calculated.
Starting from flat, the top vertex will first climb from 60° to 90°, then
continue down to 0°
*/

#define Scaling 4096	// scaled to 12 bits 1/64 microstep

bool
	BLDC3::configured() {
	
	return
		valid(); };

bool
	BLDC3::valid() {

	return
		false; };


BLDC3::BLDC3(
	char*			data,
	String*			msgPtr,
	DRV8305Axis*	aaxis ) {

	axis				= aaxis;

	holdTorque			= 0;
	stepTorque			= 0;

	lrRatio				= 0.0;
	polePairs			= 1;

	armMagneticAngle	= MS50_0;

	speedLeadAngle		= 0;
	assertedLeadAngle	= 0;
	assertedVoltage		= 0;

	powerLevelsU.powerLevels	= 0;
	minimumPulseWidth			= 0;
	
	parseLength					= 0;
	sscanf( data,
		"%c %lu %x %f %lu%ln",
		&motorType,
		&polePairs,
		&powerLevelsU.powerLevels,
		&lrRatio,
		&minimumPulseWidth,

		&parseLength );

	lrRatio		*= 1.047;	// Steps per second to radians per second
	
	*msgPtr		+= "\n Motor Type ";
	*msgPtr		+= String( motorType );

	*msgPtr		+= "\n Pole pairs ";
	*msgPtr		+= String( polePairs );

	*msgPtr		+= "\n Voltage\n   Holding ";
	*msgPtr		+= String( powerLevelsU.st.holdVoltage );
	holdTorque	= powerLevelsU.st.holdVoltage << 5;
	stepTorque	= holdTorque;

	*msgPtr		+= "\n   Running ";
	*msgPtr		+= String( powerLevelsU.st.constantVoltage );
	*msgPtr		+= "\n   Accelerating ";
	*msgPtr		+= String( powerLevelsU.st.acceleratingVoltage );
	*msgPtr		+= "\n   Decelerating ";
	*msgPtr		+= String( powerLevelsU.st.deceleratingVoltage );

	*msgPtr		+= "\n   Minimum Pulse Wdith ";
	*msgPtr		+= String( minimumPulseWidth );
};

BLDC3::~BLDC3() {

#ifdef DebugDelete
	Serial.println( " delete BLDC3" );
#endif
};

void
	BLDC3::interpolationIsr(
		Machine*	machine ) {	// 2 kHz
	
	// adjust speedLeadAngle to correspond with current motor speed
	// This is atan function assumes the the change will not
	// be more than a microstep with each motor step interrupt
	float tan	= axis->mvStepsPerSecF * lrRatio;

	if ( speedLeadAngle >= 0 ) {
		if		( tanVector50[ speedLeadAngle ] > tan )
			speedLeadAngle--;
		else if ( tanVector50[ speedLeadAngle + 1 ] <= tan )
			speedLeadAngle++; }

	else {	// speedLeadAngle < 0
		if		( -tanVector50[ -speedLeadAngle ] < tan )
			speedLeadAngle++;
		else if ( -tanVector50[ -speedLeadAngle + 1 ] >= tan )
			speedLeadAngle--; }

	assertedLeadAngle	= speedLeadAngle;

	QuadratureFeedback* quadratureFeedback =
		axis->quadratureFeedback;
	if (	quadratureFeedback
		&&	quadratureFeedback->calculateFeedback( machine ) )
		return;

	HallFeedback* hallFeedback = axis->hallFeedback;
	if (	hallFeedback
		&&	hallFeedback->calculateHallFeedback( machine ) )
		return;

	// Open-loop
	if ( stepTorque == holdTorque )
		assertedVoltage	= stepTorque;

	else {
		float ratio	=
			fabs ( axis->mvStepsPerSecF )
		/	axis->maxStepsPerSecF;
		assertedVoltage	=
			stepTorque * ratio
		+	holdTorque * ( 1.0 - ratio ); }; };

void
	BLDC3::stepArmature( int32_t steps ) {	// 15 kHz

	armMagneticAngle	+= steps;

	if ( armMagneticAngle >= MS50_360 ) {
		if ( steps >= 0 )
			armMagneticAngle	-= MS50_360;

		else
			armMagneticAngle	+= MS50_360; }; };

void
	BLDC3::assertTorque() {

	int32_t assertAngle	=
			assertedLeadAngle
		+	armMagneticAngle;

	// Normalize
	while ( assertAngle < 0 )
		assertAngle		+= MS50_360;

	while ( assertAngle >= MS50_360 )
		assertAngle		-= MS50_360;

	uint32_t angleA		= assertAngle < MS50_180 ?
			assertAngle
		:	MS50_360	- assertAngle;
	uint32_t torqueA	= angleA >= MS50_120 ?
			0
		:		assertedVoltage
			*	commutationVector50[ angleA ];

	uint32_t angleB		= assertAngle >= MS50_120 ?
			assertAngle	- MS50_120
		:	MS50_120	- assertAngle;
	uint32_t torqueB	= angleB >= MS50_120 ?
			0
		:		assertedVoltage
			*	commutationVector50[ angleB ];

	uint32_t angleC		= assertAngle >= MS50_240 ?
			assertAngle	- MS50_240
		:	MS50_240	- assertAngle;
	uint32_t torqueC	= angleC >= MS50_120 ?
			0
		:		assertedVoltage
			*	commutationVector50[ angleC ];
/*
	CurrentFeedback* currentFeedback = axis->currentFeedback;
	if ( currentFeedback ) {
		currentFeedback->csa->enabled = torqueA == 0;
		currentFeedback->csb->enabled = torqueB == 0;
		currentFeedback->csc->enabled = torqueC == 0; };

Serial.print( " V " );
Serial.print( assertedVoltage );
Serial.print( "  < " );
Serial.print( assertAngle );

Serial.print( "  A " );
Serial.print( angleA );
Serial.print( ", " );
Serial.print( torqueA );

Serial.print( "  B " );
Serial.print( angleB );
Serial.print( ", " );
Serial.print( torqueB );

Serial.print( "  C " );
Serial.print( angleC );
Serial.print( ", " );
Serial.println( torqueC );
*/
	assertTorque(	// virtual function
		torqueA + minimumPulseWidth,
		torqueB + minimumPulseWidth,
		torqueC + minimumPulseWidth ); };

void
	BLDC3::motorStateChanged( Axis::MotorState motorState ) {

	// scale to 12 bits  100% torque = 128
	switch ( motorState ) {
		case Axis::HOLDING :
		stepTorque = holdTorque;
		break;

		case Axis::CONSTANT_SPEED :
		stepTorque = powerLevelsU.st.constantVoltage << 5;
		break;

		case Axis::ACCELERATING :
		stepTorque = powerLevelsU.st.acceleratingVoltage << 5;
		break;

		case Axis::DECELERATING :
		stepTorque = powerLevelsU.st.deceleratingVoltage << 5;
		break;
		
		default:
		stepTorque = 0; }; };
