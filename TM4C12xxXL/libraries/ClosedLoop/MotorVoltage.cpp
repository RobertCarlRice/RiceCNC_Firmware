
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

#include <MotorVoltage.h>
#include <AnalogPin.h>

MotorVoltage*	MotorVoltage::motorVoltage	= NULL;

void
	MotorVoltage::New(
		char*		data,
		String*		msgPtr ) {
	
	if ( motorVoltage )
		delete motorVoltage;
	
	motorVoltage = new MotorVoltage(
		data,
		msgPtr );
	
	if ( ! motorVoltage->valid() ) {
		delete motorVoltage;
		motorVoltage = NULL; };	};
	
bool
	MotorVoltage::Low() {
	
	return
		motorVoltage
	&&	motorVoltage->low(); };


MotorVoltage::MotorVoltage(
	char*		data,
	String*		msgPtr ) {

	if ( pvddPin )
		delete pvddPin;
	
	char pvddPinString	[8];
	thresholdPct	= 0;
	maxValue		= 0;

	sscanf( data,
		" %s %lu",
		pvddPinString,
		&thresholdPct );
	
	*msgPtr		+= "\n pvdd";
	pvddPin		= new AnalogPin(
		pvddPinString,
		msgPtr );
	if ( ! pvddPin->valid() ) {
		delete pvddPin;
		pvddPin = NULL; };
		
	*msgPtr		+= " threshold % ";
	*msgPtr		+= String( thresholdPct ); };

bool
	MotorVoltage::valid() {
	
	return
		pvddPin
	&&	thresholdPct; };

bool
	MotorVoltage::low() {
	
	uint32_t analogValue = pvddPin->analogValue();
	if ( analogValue > maxValue )
		maxValue	= analogValue;

	return
		analogValue < ( maxValue * thresholdPct / 100 ); };
