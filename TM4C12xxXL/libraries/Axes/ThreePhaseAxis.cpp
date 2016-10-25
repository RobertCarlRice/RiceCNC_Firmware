
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

#include <ThreePhaseAxis.h>
#include <OutputPin.h>


ThreePhaseAxis::ThreePhaseAxis(
	char*		data,
	String*		msgPtr )

	:	Axis(
			data,
			msgPtr ) {

	char        phaseApinString[8];
	char        phaseBpinString[8];
	char        phaseCpinString[8];
	char        halfStepPinString[8];

	sscanf( data + parseLength + 1,
		"%s %s %s %s",
		phaseApinString,
		phaseBpinString,
		phaseCpinString,
		halfStepPinString );

	phaseApin		= new OutputPin(
		phaseApinString,
		msgPtr );

	phaseBpin		= new OutputPin(
		phaseBpinString,
		msgPtr );

	phaseCpin		= new OutputPin(
		phaseCpinString,
		msgPtr );
 
	halfStepPin		= new OutputPin(
		halfStepPinString,
		msgPtr );

	halfStep		= false;

	if ( ! halfStepPin->valid() ) {
		delete halfStepPin;
		halfStepPin	= NULL; };

	// validate
	if (	phaseApin->valid()
		&&	phaseBpin->valid()
		&&	phaseCpin->valid() ) {

		phaseAngle	= PSA; // set initial phase
		phaseApin->setPin( true);
		phaseBpin->setPin( false);
		phaseCpin->setPin( false);
    
		*msgPtr		+= " PA";
		phaseApin->fmtPin( msgPtr );

		*msgPtr		+= " PB";
		phaseBpin->fmtPin( msgPtr );

		*msgPtr		+= " PC";
		phaseCpin->fmtPin( msgPtr );
		
		if ( halfStepPin ) {
			*msgPtr		+= " Half Step";
			halfStepPin->fmtPin( msgPtr ); }; }

	else {
		delete phaseApin;
		phaseApin   = NULL;
      
		delete phaseBpin;
		phaseBpin   = NULL;
      
		delete phaseCpin;
		phaseCpin   = NULL; }; };


ThreePhaseAxis::~ThreePhaseAxis() {

	if ( phaseApin )
		delete phaseApin;

	if ( phaseBpin )
		delete phaseBpin;

	if ( phaseCpin )
		delete phaseCpin;

	if ( halfStepPin )
		delete halfStepPin; };

bool
	ThreePhaseAxis::isConfigured() {

	return
		phaseApin
	&&	phaseBpin
	&&	phaseCpin; };

void
	ThreePhaseAxis::motorStepIsr() { // return true if position was processed

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
	ThreePhaseAxis::isrStepMotor( bool direction ) {

	if ( halfStep ) {
		halfStepPin->setPin( false );
		halfStep	= false;
		return; };

	if ( halfStepPin ) {
		halfStepPin->setPin( true );
		halfStep	= true; };

	switch( phaseAngle ) { 
		case  PSA :

		if ( direction ) {
			phaseBpin->setPin( true );
			phaseAngle       = PSAB; }

		else {
			phaseCpin->setPin( true );
			phaseAngle       = PSCA; };

		break;

		case  PSAB :

		if ( direction ) {
			phaseApin->setPin( false );
			phaseAngle       = PSB; }

		else {
			phaseBpin->setPin( false );
			phaseAngle       = PSA; };

		break;

		case  PSB :

		if ( direction ) {
			phaseCpin->setPin( true );
			phaseAngle       = PSBC; }

		else {
			phaseApin->setPin( true );
			phaseAngle       = PSAB; };
			break;
  
		case  PSBC :

		if ( direction ) {
			phaseBpin->setPin( false );
			phaseAngle       = PSC; }

		else {
			phaseCpin->setPin( false );
			phaseAngle       = PSB; };

		break; 
  
		case  PSC :

		if ( direction ) {
			phaseApin->setPin( true );
			phaseAngle       = PSCA; }

		else {
			phaseBpin->setPin( true );
			phaseAngle       = PSBC; };

		break; 
  
		case  PSCA :

		if ( direction ) {
			phaseCpin->setPin( false );
			phaseAngle       = PSA; }

		else {
			phaseApin->setPin( false );
			phaseAngle       = PSC; };

		break; }; };
