
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

#include <StepDirectionCounter.h>
#include <QuadratureCounter4X.h>
#include <HallCounter.h>
#include <MachineCommands.h>


void
	Axis::addSlaveInput(
		char*		data,
		String*		msgPtr ) {
	
	if ( slaveStepCounter )
		delete slaveStepCounter;
	slaveStepCounter	= NULL;

	*msgPtr		+= " slave ";
	switch ( *data ) {

		case COUNTER_TYPE_STEP_DIRECTION :
		addStepDirectionCounter(
			data + 1,
			msgPtr );
		break;

		case COUNTER_TYPE_QUADRATURE :
		addQuadratureCounter(
			data + 1,
			msgPtr );
		break;

		case COUNTER_TYPE_HALL :
		addHallCounter(
			data + 1,
			msgPtr );
		break;

		default :
		*msgPtr		+= "invalid"; }; };


void
	Axis::addStepDirectionCounter(
		char*		data,
		String*		msgPtr ) {

	char	directionInputPinString[8]; // Q1
	char	stepInputPinString[8];		// Q2
	float	scale		= 1.0;

	sscanf( data,
		" %s %s %f",
		directionInputPinString,
		stepInputPinString,
		&scale );

	InputPin* directionInputPin	= new InputPin(
		directionInputPinString,
		msgPtr );

	InputPin* stepInputPin		= new InputPin(
		stepInputPinString,
		msgPtr );

	if (	directionInputPin->valid()
		&&	stepInputPin->valid() ) {

		*msgPtr		+= "direction";
		directionInputPin->fmtPin( msgPtr );

		*msgPtr		+= "  step";
		stepInputPin->fmtPin( msgPtr );

		*msgPtr		+= "  scale ";
		*msgPtr		+= String( scale );

		slaveStepCounter	= new StepDirectionCounter(
			directionInputPin,
			stepInputPin,
			scale ); }

	else {
		delete directionInputPin;
		delete stepInputPin; }; };


void
	Axis::addQuadratureCounter(
		char*		data,
		String*		msgPtr ) {

	char	q1PinString[8];
	char	q2PinString[8];
	float	scale		= 1.0;

	sscanf( data,
		" %s %s %f",
		q1PinString,
		q2PinString,
		&scale );

	InputPin* q1InputPin	= new InputPin(
		q1PinString,
		msgPtr );

	InputPin* q2InputPin	= new InputPin(
		q2PinString,
		msgPtr );

	if (	q1InputPin->valid()
		&&	q2InputPin->valid() ) {

		*msgPtr		+= "Q1";
		q1InputPin->fmtPin( msgPtr );

		*msgPtr		+= "  Q2";
		q2InputPin->fmtPin( msgPtr );

		*msgPtr		+= "  scale ";
		*msgPtr		+= String( scale );

		slaveStepCounter	= new QuadratureCounter4X(
			q1InputPin,
			q2InputPin,
			scale ); }
	
	else {
		delete q1InputPin;
		delete q2InputPin; }; };


void
	Axis::addHallCounter(
		char*		data,
		String*		msgPtr ) {

	char	hallAPinString[8];
	char	hallBPinString[8];
	char	hallCPinString[8];
	float	scale		= 1.0;

	sscanf( data,
		" %s %s %s %f",
		hallAPinString,
		hallBPinString,
		hallCPinString,
		&scale );

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

		slaveStepCounter	= new HallCounter(
			hallAInputPin,
			hallBInputPin,
			hallCInputPin,
			scale ); }

	else {
		delete hallAInputPin;
		delete hallBInputPin;
		delete hallCInputPin; }; };


void
	Axis::addSlaveInput() {

	if ( slaveStepCounter )
		interpolationMicrostepBuffer
			+= slaveStepCounter->readScaledCount();	};
