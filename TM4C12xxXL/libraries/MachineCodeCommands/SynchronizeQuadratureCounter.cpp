
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

#include <SynchronizeQuadratureCounter.h>
#include <Machine.h>
#include <MachineCommands.h>
#include <Axis.h>
#include <DRV8305Axis.h>
#include <Timers.h>
#include <BLDC3.h>
#include <QuadratureFeedback.h>
#include <QuadratureCounter1X.h>


const char SynchronizeQuadratureCounter::myName[] =
	"SynchronizeQuadratureCounter";

const char*
	SynchronizeQuadratureCounter::name() {
	
	return
		myName; };

SynchronizeQuadratureCounter::SynchronizeQuadratureCounter(
	char*	data ) {

	axisName		= ' ';
	phase			= 0;
	torque			= 512;	// 12.5%
	delay			= 500;	// ms
    sscanf( data,
		" %c %lu %lu %lu",
		&axisName,
		&phase,
		&torque,
		&delay );

	delay				*= 1000;	// ms to µS
	syncQuadratureState	= SyncQuadratureINIT;
	success				= false; };

bool	// return true when completed
	SynchronizeQuadratureCounter::execute(
		Machine*	machine ) {

	switch ( syncQuadratureState ) {

		case SyncQuadratureINIT :
		if ( pauseState == PauseWAIT_FOR_CONTINUATION ) {
			syncQuadratureState		= SyncQuadratureEND;	// Assume error
			axis8305 = (DRV8305Axis*)machine->findAxis(
				axisName,
				NULL );
			if (	axis8305
				&&	axis8305->is8305()
				&&	axis8305->motor
				&&	axis8305->quadratureFeedback
				&&	phase	< BLDC3::MS50_360
				&&	torque	<= 4096
				&&	delay	>= 1000
				&&	delay	< 1000000 ) {

				restart();
				return
					false; }; };
		break;

		case SyncQuadratureWait :
		// 5 second timout
		if ( SysTick::microSecondTimer - startTime >= delay ) {
			motor->assertedVoltage				= 0;
			motor->assertTorque();

			axis8305->drv8305AxisState			= AXIS_RUNNING;
			quadratureFeedback->stepCounter->readScaledCount();	// zero
			quadratureFeedback->synchronized	= true;
			
			success								= true;
			syncQuadratureState					= SyncQuadratureEND; };

		return
			false;

		case SyncQuadratureEND :
		continueExecution = true;
		break; };

	return
		Pause::execute( machine ); };

void
	SynchronizeQuadratureCounter::restart() {

	axis8305->indexerOffsetF				= 0.0;

	quadratureFeedback						= axis8305->quadratureFeedback;
	quadratureFeedback->synchronized		= false;
	quadratureFeedback->fractionalCount		= 0.0;
	quadratureFeedback->integralError		= 0.0;

	motor						= axis8305->motor;
	motor->armMagneticAngle		= phase;
	motor->assertedLeadAngle	= 0;
	motor->assertedVoltage		= torque;
	motor->assertTorque();

	syncQuadratureState			= SyncQuadratureWait;
	startTime					= SysTick::microSecondTimer; };

void
	SynchronizeQuadratureCounter::report(
		Machine*	machine,
		String*		msgPtr ) {
	
	*msgPtr	+= success ?
		" BLDC Quadrature counter synchronized\n"
	:	"E BLDC Quadrature counter synchronization failed\n";

	char data[ 40 ];
	snprintf( data, 40,
		"%c%c%c",
		AXIS_CONTROL,
		SYNCHRONIZE_QUADRATURE_CTR,
		axisName );
	*msgPtr  += String( data ); };

void
	SynchronizeQuadratureCounter::reportInterpolationStatus(
		Machine*	machine,
		String*		msgPtr ) {

	*msgPtr  += INTERPOLATOR_SYNCHRONIZING_QUADRATURE_COUNTER; };
/*
SynchronizeQuadratureCounter::~SynchronizeQuadratureCounter() {
	
	Serial.println( "\nE delete SynchronizeQuadratureCounter" ); };
*/
