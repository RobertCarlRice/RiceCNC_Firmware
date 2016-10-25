
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

#include <Axis.h>
#include <InputPin.h>
#include <ClosedLoopCounter.h>
#include <QuadratureCounter4X.h>
#include <QuadratureCounter1X.h>
#include <HallCounter.h>
#include <MachineCommands.h>
#include <BLDC3PWM.h>
#include <BLDC3PFM.h>
#include <MotorVoltage.h>


void
	Axis::addClosedLoop(
		char*		data,
		String*		msgPtr ) {

	if ( closedLoopStepCounter )
		delete closedLoopStepCounter;
	closedLoopStepCounter	= NULL;

	switch ( *data ) {

		case FEEDBACK_TYPE_QUADRATURE_1X :
		addQuadratureFeedback(
			data + 1,
			msgPtr,
			FEEDBACK_TYPE_QUADRATURE_1X );
		break;

		case FEEDBACK_TYPE_QUADRATURE_4X :
		addQuadratureFeedback(
			data + 1,
			msgPtr,
			FEEDBACK_TYPE_QUADRATURE_4X );
		break;

		case FEEDBACK_TYPE_HALL :
		addHallFeedback(
			data + 1,
			msgPtr );
		break;

		case FEEDBACK_TYPE_VOLTAGE_SENSE :
		addVoltageFeedback(
			data + 1,
			msgPtr );
		break;

		case FEEDBACK_TYPE_CURRENT_SENSE :
		addCurrentFeedback(
			data + 1,
			msgPtr );
		break;

		case FEEDBACK_TYPE_MOTOR_VOLTAGE :
		MotorVoltage::New(
			data + 1,
			msgPtr );
		break;

		default :
		*msgPtr		+= " feedback type invalid"; }; };

void
	Axis::addVoltageFeedback(
		char*		data,
		String*		msgPtr ) {

	*msgPtr		+= " BEMF feedback not yet implemented"; };

void
	Axis::addCurrentFeedback(
		char*		data,
		String*		msgPtr ) {

	*msgPtr		+= " Current feedback not yet implemented"; };

void
	Axis::addQuadratureFeedback(
		char*		data,
		String*		msgPtr,
		char		counterType ) {

	char		q1PinString[8];
	char		q2PinString[8];

	float		scale	= 1.0;
	uint32_t	correct	= 50;

	sscanf( data,
		" %s %s %f %lu",
		q1PinString,
		q2PinString,
		&scale,
		&correct );

    InputPin* q1InputPin	=
		new InputPin( q1PinString, msgPtr );

	InputPin* q2InputPin	=
		new InputPin( q2PinString, msgPtr );

	if (	q1InputPin->valid()
		&&	q2InputPin->valid() ) {

		*msgPtr		+= "Q1";
		q1InputPin->fmtPin( msgPtr );

		*msgPtr		+= "  Q2";
		q2InputPin->fmtPin( msgPtr );

		*msgPtr		+= "  scale ";
		*msgPtr		+= String( scale );

		*msgPtr		+= "  adjust ";
		*msgPtr		+= String( correct );

		StepCounter* stepCounter;
		if ( counterType == FEEDBACK_TYPE_QUADRATURE_4X )
			stepCounter = new QuadratureCounter4X(
				q1InputPin,
				q2InputPin,
				scale);
		
		else
			stepCounter		= new QuadratureCounter1X(
				q1InputPin,
				q2InputPin,
				scale);

		closedLoopStepCounter	= new ClosedLoopCounter(
			stepCounter,
			float( correct ) / 100.0 );

		*msgPtr		+= " Quadrature Feedback Added"; }

	else {
		delete q1InputPin;
		delete q2InputPin; }; };


void
	Axis::addHallFeedback(
		char*		data,
		String*		msgPtr ) {

	char		hallAPinString[8];
	char		hallBPinString[8];
	char		hallCPinString[8];

	float		scale	= 1.0;
	uint32_t	correct	= 50;

	sscanf( data,
		" %s %s %s %f %lu",
		hallAPinString,
		hallBPinString,
		hallCPinString,
		&scale,
		&correct );

	InputPin* hallAInputPin	= new InputPin(
		hallAPinString,
		msgPtr );

	InputPin* hallBInputPin	= new InputPin(
		hallBPinString,
		msgPtr );

	InputPin* hallCInputPin	= new InputPin(
		hallCPinString,
		msgPtr );

	if (	hallAInputPin->valid()
		&&	hallBInputPin->valid()
		&&	hallCInputPin->valid() ) {

		*msgPtr		+= "HallA";
		hallAInputPin->fmtPin( msgPtr );

		*msgPtr		+= "  HallB";
		hallBInputPin->fmtPin( msgPtr );

		*msgPtr		+= "  HallC";
		hallCInputPin->fmtPin( msgPtr );

		*msgPtr		+= "  scale ";
		*msgPtr		+= String( scale );
		
		StepCounter* stepCounter	= new HallCounter(
			hallAInputPin,
			hallBInputPin,
			hallCInputPin,
			scale );

		closedLoopStepCounter		= new ClosedLoopCounter(
			stepCounter,
			float( correct ) / 100.0 );

		*msgPtr		+= " Hall Feedback Added"; }

	else {
		delete hallAInputPin;
		delete hallBInputPin;
		delete hallCInputPin; }; };

void
	Axis::interpolationIsr(
		Machine*	machine ) {

	if ( closedLoopStepCounter )
		closedLoopStepCounter->interpolationIsr(
			machine,
			this ); };
