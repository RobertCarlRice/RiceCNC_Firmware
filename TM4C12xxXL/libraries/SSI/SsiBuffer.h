
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

#ifndef SsiBuffer_h
#define SsiBuffer_h

#include "Energia.h"
#include <driverlib/interrupt.h>

/*
The L6470 needs an 800 ns pause (CS positive) between each byte to process each command and setup
the response. Since the normal SPI framing is only one clock period (0.2 ns) between bytes,
daisy-chaining the dSpins is more efficient.
At 5 MHZ clock, it take only 1.6µs to transfer the 8 bits.
The L6470 needs 350ns to respond to chip select. CS is ignored without motor power
*/

class SsiDeviceQueue;

class SsiBuffer {

	public:

    SsiBuffer( SsiDeviceQueue* aSsiDeviceQueue );
	virtual ~SsiBuffer();

    SsiBuffer*		nextSsiBuffer;

    SsiDeviceQueue*	ssiDeviceQueue;

    bool
		isInQueue();

// dSpin
    byte
		getByte();

    void
		receiveByte( byte receivedData ); // TxEOT interrupt

// BOOST-DRV8711
	uint32_t
		getWord();

    void
		receiveWord( uint16_t receivedData );


	protected:

    uint32_t	mask;
    byte		command;
    uint8_t		dataBytes;
    byte*		bytePointer;
  
    union { // Byte order is little-endian
		uint32_t      dataWord;
		byte          dataByte[4]; } myData;

	virtual void
		rxComplete() {};

// dSpin
    void
		queueWith( uint8_t aCommand );

    void
		queueWith(
			uint8_t		aCommand,
			uint32_t	data,
			uint32_t	aMask );

    virtual void
		getParam( uint8_t param ) {};

    virtual bool
		setParam(
			uint8_t		param,
			uint32_t	value,
			String*		msg ) {};

// BOOST-DRV8711
	void
		queueWithRegister( uint16_t aRegister );

    void
		queue();
};

#endif
