
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

#include <MachineCommands.h>
#include <Machine.h>
#include <OutputPin.h>
#include <MachineOK.h>


void
	Machine::handshakeControl(
		char*		commandPtr,
		String*		msgPtr ) {

	char* data	= commandPtr + 1;

	switch ( *commandPtr ) {
	
		case SET_ALARM :
		configureAlarm(
			data,
			msgPtr );
		break;

		case SET_WATCHDOG :
		configureWatchdog(
			data,
			msgPtr );
		break;

		case SET_MACHINE_OK :
		configureMachineOK(
			data,
			msgPtr );
		break;

		default :
		*msgPtr
			+=	*commandPtr
			+	"E Invalid Machine Command - Ignored"; }; };


void
	Machine::configureAlarm(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( HANDSHAKE_CONTROL )
		+	String( SET_ALARM );
  
	if ( alarmPin )
		delete alarmPin;

	alarmPin		= new OutputPin( data, msgPtr );
	if ( alarmPin->valid() ) {
		alarmPin->setPin( false );

		*msgPtr		+= " Alarm pin ";
		alarmPin->fmtPin( msgPtr ); }
 
	else {
		*msgPtr		+= " No Alarm";
		delete alarmPin;
		alarmPin    = NULL; }; };

void
	Machine::configureWatchdog(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( HANDSHAKE_CONTROL )
		+	String( SET_WATCHDOG );
  
	if ( watchdogPin )
		delete watchdogPin;

	watchdogPin		= new OutputPin( data, msgPtr );
	if ( watchdogPin->valid() ) {
		watchdogPin->setPin( true );

		*msgPtr		+= " Watchdog pin ";
		watchdogPin->fmtPin( msgPtr ); }
 
	else {
		*msgPtr		+= " No Watchdog";
		delete watchdogPin;
		watchdogPin    = NULL; }; };


void
	Machine::configureMachineOK(
		char*		data,
		String*		msgPtr ) {

	*msgPtr
		+=	String( HANDSHAKE_CONTROL )
		+	String( SET_MACHINE_OK );

	if ( machineOK )
		delete machineOK;

	machineOK     = new MachineOK();
	if ( ! machineOK->validate( data, msgPtr ) ) {
		delete machineOK;
		machineOK   = NULL;
		*msgPtr		+= " No Machine OK input"; }; };
