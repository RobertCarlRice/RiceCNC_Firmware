
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

#include <DiscreteAxis.h>
#include <OutputPin.h>


DiscreteAxis::DiscreteAxis(
	char*		data,
	String*		msgPtr )

	:	Axis(
			data,
			msgPtr ) {

	char        coilApin1String[8];
	char        coilApin2String[8];
	char        coilBpin1String[8];
	char        coilBpin2String[8];

	sscanf( data + parseLength + 1,
		"%s %s %s %s",
		coilApin1String,
		coilApin2String,
		coilBpin1String,
		coilBpin2String );

	coilApin1     = new OutputPin( coilApin1String, msgPtr );
	coilApin2     = new OutputPin( coilApin2String, msgPtr );
	coilBpin1     = new OutputPin( coilBpin1String, msgPtr );
	coilBpin2     = new OutputPin( coilBpin2String, msgPtr );
 
	// validate
	if (	coilApin1->valid()
		&&	coilApin2->valid()
		&&	coilBpin1->valid()
		&&	coilBpin2->valid() ) {

		phaseAngle      = HS0; // set initial phase
		coilApin1->setPin( true);
		coilApin2->setPin( false);
		coilBpin1->setPin( false);
		coilBpin2->setPin( false);
    
		*msgPtr  += " A1";
		coilApin1->fmtPin( msgPtr );

		*msgPtr  += " A2";
		coilApin2->fmtPin( msgPtr );

		*msgPtr  += " B1";
		coilBpin1->fmtPin( msgPtr );

		*msgPtr  += " B2";
		coilBpin2->fmtPin( msgPtr ); }

	else {
		delete coilApin1;
		coilApin1   = NULL;
      
		delete coilApin2;
		coilApin2   = NULL;
      
		delete coilBpin1;
		coilBpin1   = NULL;

		delete coilBpin2;
		coilBpin2   = NULL; }; };

DiscreteAxis::~DiscreteAxis() {
	if ( coilApin1 )
		delete coilApin1;

	if ( coilApin2 )
		delete coilApin2;

	if ( coilBpin1 )
		delete coilBpin1;

	if ( coilBpin2 )
		delete coilBpin2; };

bool
	DiscreteAxis::isConfigured() {

	return
		coilApin1
	&&	coilApin2
	&&	coilBpin1
	&&	coilBpin2; };

void
	DiscreteAxis::motorStepIsr() { // return true if position was processed

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
	DiscreteAxis::isrStepMotor( bool direction ) {

	switch( phaseAngle ) { 
		case  HS0 : // coilApin1 active

		if ( direction ) {
			coilBpin1->setPin( true );
			phaseAngle       = HS1; }

		else {
			coilBpin2->setPin( true );
			phaseAngle       = HS7; };

		break;

		case  HS1 : // coilApin1 and coilBpin1 active

		if ( direction ) {
			coilApin1->setPin( false );
			phaseAngle       = HS2; }

		else {
			coilBpin1->setPin( false );
			phaseAngle       = HS0; };

		break;
  
		case  HS2 : // coilBpin1 active

		if ( direction ) {
			coilApin2->setPin( true );
			phaseAngle       = HS3; }

		else {
			coilApin1->setPin( true );
			phaseAngle       = HS1; };

		break;

		case  HS3 : // coilApin2 and coilBpin1 active

		if ( direction ) {
			coilBpin1->setPin( false );
			phaseAngle       = HS4; }

		else {
			coilApin2->setPin( false );
			phaseAngle       = HS2; };

		break;
  
		case  HS4 : // coilApin2 active

		if ( direction ) {
			coilBpin2->setPin( true );
			phaseAngle       = HS5; }

		else {
			coilBpin1->setPin( true );
			phaseAngle       = HS3; };

		break;

		case  HS5 : // coilApin2 and coilBpin2 active

		if ( direction ) {
			coilApin2->setPin( false );
			phaseAngle       = HS6; }

		else {
			coilBpin2->setPin( false );
			phaseAngle       = HS4; };

		break;
  
		case  HS6 : // coilBpin2 active

		if ( direction ) {
			coilApin1->setPin( true );
			phaseAngle       = HS7; }

		else {
			coilApin2->setPin( true );
			phaseAngle       = HS5; };

		break;

		case  HS7 : // coilApin1 and coilBpin2 active

		if ( direction ) {
			coilBpin2->setPin( false );
			phaseAngle       = HS0; }

		else {
			coilApin1->setPin( false );
			phaseAngle       = HS6; };

		break; }; };
