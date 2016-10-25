
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

#include <MySerial.h>
#include <Machine.h>
#include <MainCommands.h>
#include <Timers.h>


#define vitualComBaudrate 115200
#define CommandBufferSize 1024

char  serialCommandString[ CommandBufferSize ];
char* serialCommandPtr    = serialCommandString;

void
	MySerial::setup() {

	Serial.setBufferSize( 1024, 1024 ); // default is 256
	Serial.begin( vitualComBaudrate );
	Serial.flush();

	String msg = "";
	MainCommands::signOnMsg( &msg );
	Serial.println( msg ); };


void
	MySerial::loop() {

	while ( Serial.available() ) {
		Timers::setLoopLED( false );

		char inChar    = (char) Serial.read();

		if ( inChar == '\n' ) {
			if ( serialCommandPtr != serialCommandString ) { // ignore empty lines
				*serialCommandPtr  = '\0'; // NULL terminate
			
				String msg = "";
				MainCommands::processCommand( serialCommandString, &msg );

				// Since the command was received via serial link block if necessary sending the reply
				if ( msg.length() > 0 )
					Serial.println( msg );
			
				serialCommandPtr   = serialCommandString; }; } // reset

		else if ( inChar == '\r' ||
			serialCommandPtr == &serialCommandString[ CommandBufferSize ] ) {} // ignore

		else {
			*serialCommandPtr  = inChar;
			serialCommandPtr++; }; }; };

void
	MySerial::background() {

	// pending method added by RCR 2/10/2015 to avoid blocking on a background message
	if ( Serial.txCapacity() < 128 )
		return;
 
	String msg = "";

	if ( Machine::firstMachine )
		Machine::firstMachine->background( &msg );

	if ( msg.length() )
		sendMessageNonBlocking( &msg ); };

void
	MySerial::sendMessageNonBlocking( String* msgPtr ) { // Debug messages
	
	// Just ignore the message if it would cause blocking
	if ( Serial.txCapacity() > msgPtr->length() )
		Serial.println( *msgPtr ); };
		
bool
	MySerial::isBusy() {

	return Serial.available(); };

