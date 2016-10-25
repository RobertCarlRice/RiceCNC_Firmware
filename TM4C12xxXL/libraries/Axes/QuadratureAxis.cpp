
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

#include <QuadratureAxis.h>
#include <OutputPin.h>


QuadratureAxis::QuadratureAxis(
	char*		data,
	String*		msgPtr )

	:	Axis(
			data,
			msgPtr ) {

	char        phase1pinString[8];
	char        phase2pinString[8];

	sscanf( data + parseLength + 1,
		"%s %s",
		phase1pinString,
		phase2pinString );

	phase1pin     = new OutputPin( phase1pinString, msgPtr );
	phase2pin     = new OutputPin( phase2pinString, msgPtr );
 
	// validate
	if (	phase1pin->valid()
		&&	phase2pin->valid() ) {

		phaseAngle	= FS0; // set initial phase
		phase1pin->setPin( false);
		phase2pin->setPin( false);
    
		*msgPtr		+= " P1";
		phase1pin->fmtPin( msgPtr );

		*msgPtr		+= " P2";
		phase2pin->fmtPin( msgPtr ); }

	else {
		delete phase1pin;
		phase1pin   = NULL;
      
		delete phase2pin;
		phase2pin   = NULL; }; };


QuadratureAxis::~QuadratureAxis() {

	if ( phase1pin )
		delete phase1pin;
	if ( phase2pin )
		delete phase2pin; };

bool
	QuadratureAxis::isConfigured() {

	return phase1pin && phase2pin; };

void
	QuadratureAxis::motorStepIsr() { // return true if position was processed

	updateVelocity();

	// Discrete half-step mode
	// grey code is faster with no delay for direction change or pulse reset
	if ( indexerOffsetF >= 1.0 ) {
		isrStepMotor( true ); // Step immediately
		stepped( 1.0 );
		return; };
    
	if ( indexerOffsetF <= -1.0 ) {
		isrStepMotor( false ); // Step immediately
		stepped( -1.0 ); };	};


void
	QuadratureAxis::isrStepMotor( bool direction ) {

	switch( phaseAngle ) { 
		case  FS0 : // neither pin active

		if ( direction ) {
			phase1pin->setPin( true );
			phaseAngle       = FS1; }

		else {
			phase2pin->setPin( true );
			phaseAngle       = FS3; };

		break;

		case  FS1 : // phase1pin active

		if ( direction ) {
			phase2pin->setPin( true );
			phaseAngle       = FS2; }

		else {
			phase1pin->setPin( false );
			phaseAngle       = FS0; };

		break;

		case  FS2 : // phase1pin and phase2pin active

		if ( direction ) {
			phase1pin->setPin( false );
			phaseAngle       = FS3; }

		else {
			phase2pin->setPin( false );
			phaseAngle       = FS1; };
			break;
  
		case  FS3 : // phase2pin active

		if ( direction ) {
			phase2pin->setPin( false );
			phaseAngle       = FS0; }

		else {
			phase1pin->setPin( true );
			phaseAngle       = FS2; };

		break; }; };
