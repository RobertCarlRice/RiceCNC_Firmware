
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

#include <Machine.h>
#include <MachineCommands.h>
#include <Axis.h>
#include <Probe.h>
#include <InterpolationTimer.h>
#include <AnalogPin.h>
#include <InputPin.h>
#include <Command.h>
#include <MachineOK.h>
#include <DoorAjar.h>
#include <DoorAjarCommand.h>
#include <MainCommands.h>
#include <MySerial.h>
#include <InterpolationTimer.h>
#include <AxisTimer.h>
#include <MachineNotOkCommand.h>
#include <OutputPin.h>

#define	StartingStepSize	0.0001


Machine* Machine::firstMachine = NULL;

Machine::Machine(
	char*		data,
	String*		msgPtr ) {

	firstAxis				= NULL;
	firstCommandInQueue		= NULL;
	currentCommand			= NULL;
	firstAnalog				= NULL;
	firstDigital			= NULL;

	// Init GpioPin
	machineOK				= NULL;
	doorAjar				= NULL;
	spindle					= NULL;
	probe					= NULL;
	alarmPin				= NULL;
	watchdogPin				= NULL;

	char		motorPowerPinString [8];
	uint32_t	msDelay;

	sscanf( data,
		" %s %lu",
		&motorPowerPinString,
		&msDelay );

	motorPowerPin			= new OutputPin( data, msgPtr );
	if ( motorPowerPin->valid() ) {
		*msgPtr
			+=	" Motor Power";
		motorPowerPin->fmtPin( msgPtr );
		motorPowerPin->setPin( true );
		
		msDelay				*= 1000;
		uint32_t startTime	= SysTick::microSecondTimer;
		while ( ( SysTick::microSecondTimer - startTime ) < msDelay ); }

	else {
		delete motorPowerPin;
		motorPowerPin		= NULL; };

	sendFlags				= 0;
	interpolationBusy		= false;
	stepSize				= StartingStepSize;

	for ( int i = 0; i < AuxOutputs; i++ )
		coolants[ i ] = NULL;
  
	maxInterpolationSpeed	= 0.0;
	sqFeedrateIPS			= 0.0;	// avoid square root for efficiency

	statusReportingInterval	= 0;
	lastStatusTime			= 0;

	*msgPtr
		+=	" Machine created\n"; };


Machine::~Machine() {
	abort();
	clearAnalog();
	clearDigital();
  
	while ( firstAxis ) {
		IntMasterDisable();
			Axis* axisPtr	= firstAxis;
			firstAxis		= axisPtr->nextAxis;
		IntMasterEnable();
		delete axisPtr; };
  
	if ( machineOK )
		delete machineOK;

	if ( spindle )
		delete spindle;
	
	if ( probe )
		delete probe;
	
	if ( doorAjar )
		delete doorAjar;

	if ( alarmPin )
		delete alarmPin;
  
	if ( watchdogPin )
		delete watchdogPin;
  
	if ( motorPowerPin )
		delete motorPowerPin;
  
	for ( int i = 0; i < AuxOutputs; i++ )
		if ( coolants[ i ] )
			delete coolants[ i ]; };

void
	Machine::deleteAllAxesUsingSsi(
		SSI* ssi ) {

	if ( Machine::firstMachine )
		Machine::firstMachine->deleteSSI( ssi ); };

void
	Machine::clearAnalog() {

	InterpolationTimer::disableInterpolationInterrupts();
		while( firstAnalog ) { // execution queue
			AnalogPin* analogPtr	= firstAnalog;
			firstAnalog				= firstAnalog->nextAnalogPin;
			delete analogPtr; };
	InterpolationTimer::enableInterpolationInterrupts(); };


void
	Machine::clearDigital() {

	InterpolationTimer::disableInterpolationInterrupts();
		while( firstDigital ) { // execution queue
			InputPin* inputPinPtr	= firstDigital;
			firstDigital			= firstDigital->nextInputPin;
			delete inputPinPtr; };
	InterpolationTimer::enableInterpolationInterrupts(); };


void
	Machine::cancelTransforms() {

	broadcastToAxes( &Axis::deleteLinks ); };


void
	Machine::setSendFlag(
		uint32_t flag ) {

	IntMasterDisable();
        sendFlags |= flag;
	IntMasterEnable(); };

bool
	Machine::isSeekingPosition() { // wait for all axes to go idle

	return
		detect( &Axis::isSeekingPosition ); };


bool
	Machine::detect(
		Detect method ) {
	
	Axis* axisPtr  = firstAxis;
	while ( axisPtr ) {
        if ( (axisPtr->*method)() )
			return
				true;
        axisPtr   = axisPtr->nextAxis; };

	return
		false; };

void
	Machine::broadcastToAxes(
		BroadcastToAxes method ) {

	Axis* axisPtr  = firstAxis;
	while ( axisPtr ) {
        (axisPtr->*method)();
        axisPtr   = axisPtr->nextAxis; }; };
        
Axis*
	Machine::findAxis(
		char		axisNameC,
		String*		msgPtr ) {

	if ( axisNameC >= 97 )
		axisNameC -= 32; // x->X, y->Y, z->Z, a->A, b->B, c->C

	Axis * axisPtr = firstAxis;
	while ( axisPtr ) {
		if ( axisPtr->axisNameC == axisNameC )
			return
				axisPtr; 
		axisPtr = axisPtr->nextAxis; };

	if ( msgPtr )
		*msgPtr	+= "E Axis not configured";
	else
		setSendFlag( SendAxisUnconfigured );

	return
		NULL; };


void
	Machine::deleteSSI(
		SSI* ssi ) {

	// delete any axes using this SSI
	Axis** axisPtrPtr  = &firstAxis;
	while ( true ) {
		Axis* axisPtr    = *axisPtrPtr;
		if ( ! axisPtr ) break;
    
		if ( axisPtr->usesSSI( ssi ) ) {
			*axisPtrPtr		= axisPtr->nextAxis; // Bypass
			delete axisPtr; }
 
		else
			axisPtrPtr  = &axisPtr->nextAxis; }; };

