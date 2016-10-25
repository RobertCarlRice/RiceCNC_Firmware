
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

#include <StepDirectionAxis.h>
#include <OutputPin.h>


StepDirectionAxis::StepDirectionAxis(
	char*		data,
	String*		msgPtr )

	:	Axis(
		data,
		msgPtr ) {

	char        directionPinString[8];
	char        stepPinString[8];
	
	// Could be inherited by DRV8711Axis but using SPI control instead
	uint32_t	myParseLength;

	sscanf( data + parseLength + 1,
		"%s %s%ln",
		directionPinString,
		stepPinString,
		&myParseLength );

	parseLength		+= myParseLength + 1;
	
	directionPin	= new OutputPin( directionPinString, msgPtr );
	stepPin			= new OutputPin( stepPinString, msgPtr );

	if (	directionPin->valid()
		&&	stepPin->valid() ) {

		directionPin->setPin( false);
		stepPin->setPin( false);

		*msgPtr		+= "\n dir";
		directionPin->fmtPin( msgPtr );

		*msgPtr		+= "\n step";
		stepPin->fmtPin( msgPtr );
    
		phaseAngle		= Quiescent; }

	else {
		delete directionPin;
		delete stepPin;
		directionPin	= NULL;
		stepPin			= NULL; };
  
	if ( isConfigured() )
		*msgPtr		+= " StepDir Configured"; };


StepDirectionAxis::~StepDirectionAxis() {

	if ( directionPin )
		delete directionPin;

	if ( stepPin )
		delete stepPin; };

bool
	StepDirectionAxis::isConfigured() {

	return directionPin && stepPin; };

void
	StepDirectionAxis::motorStepIsr() { // called on axis interrupt

	if ( ! directionPin )
		return;
  
	updateVelocity();

	switch( phaseAngle ) {

		case  StateStepPositive :
		stepPin->setPin( true ); // toggle the step pin
		stepped( 1.0 );
		phaseAngle		= NeedsPulseReset;
		break;		// Can't do anything else until step is reset

		case  StateStepNegative :
		stepPin->setPin( true ); // toggle the step pin
		stepped( -1.0 );
		phaseAngle		= NeedsPulseReset;
		break;     // Can't do anything else until step is reset

		case  NeedsPulseReset :
		stepPin->setPin( false ); // reset the step pin
		// idle for now but continue to calculate direction for next step
		phaseAngle		= Quiescent;
		// fall through to Quiescent

		case Quiescent :
		if		( indexerOffsetF >= 1.0 ) {
			directionPin->setPin( true );
			phaseAngle       = StateStepPositive; } // Step on next interrupt

		else if ( indexerOffsetF <= -1.0 ) {
			directionPin->setPin( false );
			phaseAngle       = StateStepNegative; }; };	}; // Step on next interrupt
