
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

#include <MyEEProm.h>
#include <MySerial.h>

#include "driverlib/eeprom.h"
#include <driverlib/sysctl.h>
#include "driverlib/emac.h"

#include "inc/hw_eeprom.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_emac.h"


MyEEProm::Un MyEEProm::U = {};

void
	MyEEProm::readEEProm() {
	
    // Get address from EEPROM
    SysCtlPeripheralEnable( SYSCTL_PERIPH_EEPROM0 ); // defined as 2048
    SysCtlDelay( 4 );	// Startup time
    EEPROMInit();		// Reset from power fail

#ifdef DEBUG
	char data[ 80 ];
	snprintf( data, 80,
		" Copy %d words form EEProm to address %p",
		sizeof( Un ),
		(void*)&U.firstEEPromWord );
	String msg = String( data );
	MySerial::sendMessageNonBlocking( &msg );
#endif

    MyEEProm::MyEEPROMRead(
		&U.firstEEPromWord,
		0,
		sizeof( Un ) ); // ROM_EEPROMRead hangs

#ifdef DEBUG
    String msg = "\n EEPROM read";
    MyEEProm::debugEePromCopyDump( &msg );
	MySerial::sendMessageNonBlocking( &msg );
#endif
	};


void
	MyEEProm::debugEePromCopyDump( String* msgPtr ) {
	
    *msgPtr  += "\n  Key";
    MyEEProm::dumpBytes( &MyEEProm::U.S.useKey,		    1, msgPtr );

    *msgPtr  += "\n  MAC";
    MyEEProm::dumpBytes( MyEEProm::U.S.macAddress,      6, msgPtr );

    *msgPtr  += "\n  IP";
    MyEEProm::dumpBytes( MyEEProm::U.S.myIpAddress,     4, msgPtr );

    *msgPtr  += "\n  Router";
    MyEEProm::dumpBytes( MyEEProm::U.S.routerAddress,   4, msgPtr );

    *msgPtr  += "\n  Mask";
    MyEEProm::dumpBytes( MyEEProm::U.S.subnetMask,      4, msgPtr );

    *msgPtr  += "\n  Port ";
    *msgPtr  += String( MyEEProm::U.S.port );

    *msgPtr  += "\n\n  Remote IP";
    MyEEProm::dumpBytes( MyEEProm::U.S.remoteIpAddress, 4, msgPtr );

    *msgPtr  += "\n  Remote Port ";
    *msgPtr  += String( MyEEProm::U.S.remote_Port ); };

void
	MyEEProm::dumpBytes( byte* buffer, int count, String* msgPtr ) {
	
    char  data[4];
    while ( count-- ) {
      snprintf( data, 4, " %02X", *buffer++ );
      *msgPtr  += String( data ); }; };


void
	MyEEProm::MyEEPROMRead(
		uint32_t *pui32Data,
		uint32_t ui32PromAddress,
		uint32_t ui32Count)	{

    // Set the block and offset appropriately to read the first word.
    HWREG( EEPROM_EEBLOCK )  = ui32PromAddress >> 6;
    HWREG( EEPROM_EEOFFSET ) = ui32PromAddress >> 2 & 0x0F;

    // Convert byte count to a word count.
    ui32Count >>= 2;

    // Read each word in turn.
    while( ui32Count-- ) {
        // Read the next word through the autoincrementing register.
		uint32_t data = HWREG( EEPROM_EERDWRINC );

#ifdef DEBUG
	char msgdata[ 20 ];
	snprintf( msgdata, 20,
		" Read 0x%X",
		data );
	String msg = String( data );
	MySerial::sendMessageNonBlocking( &msg );
#endif
		
		*pui32Data++ = data;
        // Do we need to move to the next block?  This is the case if the
        // offset register has just wrapped back to 0.  Note that we only
        // write the block register if we have more data to read.  If this
        // register is written, the hardware expects a read or write operation
        // next.  If a mass erase is requested instead, the mass erase will
        // fail.
        if ( ui32Count && HWREG( EEPROM_EEOFFSET ) == 0 )
             HWREG( EEPROM_EEBLOCK )  += 1; };
};

void
	MyEEProm::writeEEProm() {

#ifdef DEBUG
    // Check parameters in a debug build.
	char data[ 40 ];
	snprintf( data, 40,
		" MyEEPROMProgram %d",
		sizeof( Un ) );
	String msg = String( data );
	MySerial::sendMessageNonBlocking( &msg );
#endif

    MyEEProm::MyEEPROMProgram(
		&U.firstEEPromWord,
		0,
		sizeof( Un ) ); // ROM_EEPROMRead hangs
	};

void
	MyEEProm::setUseKey( bool use ) {
	
	MyEEProm::U.S.useKey = use ?
		UseKey :
		0;
	MyEEProm::MyEEPROMProgram(
		(uint32_t*) &U.firstEEPromWord,
		0,
		4); };

uint32_t
	MyEEProm::MyEEPROMProgram(
		uint32_t *pui32Data,
		uint32_t ui32PromAddress,
		uint32_t ui32Count)
{
    // Make sure the EEPROM is idle before we start.
	while( HWREG( EEPROM_EEDONE ) & EEPROM_EEDONE_WORKING );

    // Set the block and offset appropriately to program the first word.
    HWREG( EEPROM_EEBLOCK )		= ui32PromAddress >> 6;
    HWREG( EEPROM_EEOFFSET )	= ui32PromAddress >> 2 & 0x0F;

    // Convert the byte count to a word count.
    ui32Count >>= 2;

    // Write each word in turn.
    while( ui32Count-- ){
		uint32_t data = *pui32Data++;

#ifdef DEBUG
	char msgdata[ 20 ];
	snprintf( msgdata, 20,
		" Write 0x%X",
		data );
	String msg = String( data );
	MySerial::sendMessageNonBlocking( &msg );
#endif

		// Write the next word through the autoincrementing register.
        HWREG( EEPROM_EERDWRINC ) = data;
        // Wait for the write to complete.
		while ( HWREG( EEPROM_EEDONE ) & EEPROM_EEDONE_WORKING );
        // Make sure we completed the write without errors.  Note that we
        // must check this per-word because write permission can be set per
        // block resulting in only a section of the write not being performed.
        if ( HWREG( EEPROM_EEDONE ) & ( EEPROM_EEDONE_NOPERM | EEPROM_EEDONE_INVPL ) )
             break; // An error occurred.
        // Do we need to move to the next block?  This is the case if the
        // offset register has just wrapped back to 0.  Note that we only
        // write the block register if we have more data to read.  If this
        // register is written, the hardware expects a read or write operation
        // next.  If a mass erase is requested instead, the mass erase will
        // fail.
        if( ui32Count && HWREG( EEPROM_EEOFFSET ) == 0)
            HWREG( EEPROM_EEBLOCK ) += 1;
    }
    // Return the current status to the caller.
    return( HWREG( EEPROM_EEDONE ) );
};

MyEEProm::SoftResetReason
	MyEEProm::softResetReason() {
	
	return
		U.S.softResetReason; };

void
	MyEEProm::recordResetReason(
		SoftResetReason	softResetReason ) {
	
	U.S.softResetReason	= softResetReason;
	MyEEProm::writeEEProm(); };

void
	MyEEProm::readMacAddress( byte* mac ) {

	ROM_SysCtlPeripheralEnable(	SYSCTL_PERIPH_EMAC0 );
	ROM_SysCtlPeripheralReset(	SYSCTL_PERIPH_EMAC0 );
	while( ! ROM_SysCtlPeripheralReady( SYSCTL_PERIPH_EMAC0 ) ) {};

	ROM_SysCtlPeripheralEnable(	SYSCTL_PERIPH_EPHY0 );
	ROM_SysCtlPeripheralReset(	SYSCTL_PERIPH_EPHY0 );
	while( ! ROM_SysCtlPeripheralReady( SYSCTL_PERIPH_EPHY0 ) ) {};

//	EMACReset( EMAC0_BASE );
	EMACAddrGet( EMAC0_BASE, 0, mac ); // Get MAC from hardware PHY
	};
	
void
	MyEEProm::setEthernetAddresses(
		char*	data,
		String* msgPtr ) {
 
    char    myIpAddress    [20]  = "";
    char    routerAddress  [20]  = "";
    char    subnetMask     [20]  = "";
    uint16_t  port				 = 8888;

    sscanf( data+1,
		"%s %s %s %u",
		myIpAddress,
		routerAddress,
		subnetMask,
		&port );

#ifdef DEBUG
    snprintf( data, 100,
		" Parameters\n  IP %s\n  Router %s\n  Mask %s\n  Port %u",
		myIpAddress,
		routerAddress,
		subnetMask,
		port );
    *msgPtr  += String( data );
#endif

	byte	mac[ 6 ];
	readMacAddress( mac );

    snprintf( data, 100,
		" MAC %02X:%02X:%02X:%02X:%02X:%02X",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
    *msgPtr  += String( data );

    bool change  = false;

    if ( MyEEProm::U.S.useKey != UseKey ) {
		MyEEProm::U.S.useKey = UseKey;
		change    = true; };

    compare( MyEEProm::U.S.macAddress, mac, 6, &change );
    // Parse the ip addresses
    parseIpAddress( myIpAddress,   MyEEProm::U.S.myIpAddress,   &change );
    parseIpAddress( routerAddress, MyEEProm::U.S.routerAddress, &change );
    parseIpAddress( subnetMask,    MyEEProm::U.S.subnetMask,    &change );
    
    if ( port != MyEEProm::U.S.port ) {
		MyEEProm::U.S.port = port;
		change    = true; };
 
    if ( change ) {
		*msgPtr   += "\n Recording changes\n ";
#ifdef DEBUG
		MyEEProm::debugEePromCopyDump( msgPtr );
#endif
		MyEEProm::writeEEProm(); }

    else
		*msgPtr   += "\n Already recorded\n "; };


void
	MyEEProm::parseIpAddress(
		char*		stringPtr,
		byte*		readByte,
		bool*	change ) {
	
    byte ip[]   = { 0, 0, 0, 0 };
    char* ptr   = stringPtr;
    for ( int count = 0; count < 4; count++ ) {
		ip[ count ]  = strtol( ptr, &ptr, 10 );
		if ( *ptr != '.' && *ptr != ',' ) break;
		ptr++; };
 
    compare( readByte, ip, 4, change ); };


void
	MyEEProm::compare(
		byte*		readByte,
		byte*		writeByte,
		int			length,
		bool*		change ) {

    while ( length-- ) {
		if ( *writeByte  != *readByte ) {
			*change = true;
			*readByte     = *writeByte; };
		readByte++;
		writeByte++; }; };

void
	MyEEProm::recordConnectionFrom(
		IPAddress	remote_IP,
		uint16_t	remote_port ) {
	
	uint32_t  rawRemote = uint32_t( remote_IP );

	bool change      = false;
	MyEEProm::compare(
		MyEEProm::U.S.remoteIpAddress,
		(byte*) &rawRemote,
		4,
		&change );
	
	if ( remote_port != MyEEProm::U.S.remote_Port ) {
		MyEEProm::U.S.remote_Port = remote_port;
		change      = true; };

	if ( change ) {
		String msg = " Recording remote address and port";
		MySerial::sendMessageNonBlocking( &msg );
		MyEEProm::writeEEProm(); };
	};

bool
	MyEEProm::remoteIsRecorded (
		IPAddress	remote_IP,
		uint16_t	remote_port ) {

	uint32_t	compareRawRemote	= uint32_t( remote_IP );
	byte*		compareRemoteBytes	= (byte*) &compareRawRemote;

	byte*		recordedRemoteBytes	= MyEEProm::U.S.remoteIpAddress;

	for ( int i = 0; i < 4; i++ ) {
		if ( *compareRemoteBytes++ != *recordedRemoteBytes++ )
			return false; };

	if ( remote_port != MyEEProm::U.S.remote_Port ) {
		return false; };
	
	return true; };

