
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
#include <LimitSwitch.h>
#include <AxisTimer.h>


void
	Axis::addLimitSwitch(
		char*		data,
		String*		msgPtr ) {
	
	LimitSwitch* limitSwitch = new LimitSwitch( data, msgPtr );
	if ( limitSwitch->valid() ) {
		limitSwitch->fmtPin( msgPtr );

		*msgPtr		+= " Limit ";

		if ( limitSwitch->high ) {
			*msgPtr		+= "high";
			if ( limitSwHighPin )
				delete limitSwHighPin;
			limitSwHighPin = limitSwitch; }

		else {
			*msgPtr		+= "low";
			if ( limitSwLowPin )
				delete limitSwLowPin;
			limitSwLowPin = limitSwitch; }; }

	else
		delete limitSwitch; };

bool
	Axis::isSwitchLimited() {

	if (	limitSwLowPin
		&&	limitSwLowPin->readPin() ) {

		switchLimited( SWITCH_LIMIT_LOW ); 
		return
			true; };

	if	(	limitSwHighPin
		&&	limitSwHighPin->readPin() ) {

		switchLimited( SWITCH_LIMIT_HIGH ); 
		return
			true; };

	switch ( axisState ) {

		case SWITCH_LIMIT_HIGH :
		AxisTimer::disableAxisInterrupts();
			stopMotor();
			setAxisState( IDLE );
		AxisTimer::enableAxisInterrupts();
		break;
		
		case SWITCH_LIMIT_LOW :
		AxisTimer::disableAxisInterrupts();
			stopMotor();
			setAxisState( IDLE );
		AxisTimer::enableAxisInterrupts(); };

	return
		false; };


void
	Axis::switchLimited( AxisState axisState ) {
	
	AxisTimer::disableAxisInterrupts();
		// Abort interpolation movement
		microStepTargetOffsetI	= 0;
		microStepTargetOffsetF	= 0.0;

		// Brake and move slowly off limit switch
		float backOffVelocity	= stepsPerUnitDistanceF / 60.0;
		if ( axisState == SWITCH_LIMIT_HIGH )
			backOffVelocity = -backOffVelocity;

		runMotor( backOffVelocity );
		setAxisState( axisState );
	AxisTimer::enableAxisInterrupts(); };


void
	Axis::reportDigital( String* msgPtr ) {

	if ( limitSwHighPin )
		limitSwHighPin->reportPinChange( msgPtr );

	if ( limitSwLowPin ) 
		limitSwLowPin->reportPinChange( msgPtr );
	
	reportHallPinChange( msgPtr ); };

