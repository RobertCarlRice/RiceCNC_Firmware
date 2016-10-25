
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

#ifndef SsiDeviceQueue_h
#define SsiDeviceQueue_h

#include "Energia.h"

class SSI;
class SsiBuffer;

/*
The L6470 needs an 800 ns pause (CS positive) between each byte to process each command and setup
the response. Since the normal SPI framing is only one clock period (0.2 ns) between bytes,
daisy-chaining the dSpins is more efficient.
At 5 MHZ clock, it take only 1.6µs to transfer the 8 bits.
The L6470 needs 350ns to respond to chip select. CS is ignored without motor power
*/

class SsiDeviceQueue {

	public:
    SsiDeviceQueue( SSI* ssi, uint32_t number );     // constructor
    virtual ~SsiDeviceQueue();   // destructor - will perform hard stop

	SSI*				ssi;
    uint32_t			number;
    SsiDeviceQueue*     nextSsiDeviceQueue; // chain for SSI

    SsiBuffer*			startPointer; // pending queue
    SsiBuffer*			currentCR;    // pending queue
//    SsiBuffer*  setupBuffer;

// dSpins get alternate calls to getByte and receiveByte from SSI TxEOT ISR
// And pass the calls on to the active buffer if one
    byte
		getByte();
 
    void
		receiveByte( uint32_t data );
// End Interrupt Service Routines

    void
		dequeue();

    void
		queue( SsiBuffer* request );


	private:
};

#endif
