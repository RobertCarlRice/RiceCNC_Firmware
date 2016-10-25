# RiceCNC_Firmware
RiceCNC Firmware

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

This project has no targets. To compile this project, point Energia preferences
Sketchbook locations to the TM4C12xxXL, then double-click the TM4C12xxXL.ino file.
The RiceCNC iTunes app contains the latest release of this firmware and logic to flash
the firmware to the TM4C1294XL. However, for testing firmware changes, you can flash
directly using Energia.

If everything else works, you will still get compile errors referencing the routines
HardwareSerial.pending() and HardwareSerial.txCapacity(). I added these methods directly
to my Energia library as a quick solution. When using an ethernet connection, I wanted to
send debug messages over the serial link but not block if there was no serial connection.
Someone may want to add these methods to Energia release:

In HardwareSerial.h:
	public:
		HardwareSerial(void);
		HardwareSerial(unsigned long);
		void begin(unsigned long);
		void setBufferSize(unsigned long, unsigned long);
		void setModule(unsigned long);
		void setPins(unsigned long);
		void end(void);
		virtual int available(void);
		virtual int pending(void); // Added by RCR 2/10/2015
		virtual int txCapacity(void); // Added by RCR 6/22/2015
		virtual int peek(void);
		virtual int read(void);
		virtual void flush(void);
		void UARTIntHandler(void);
		virtual size_t write(uint8_t c);
		operator bool();
		using Print::write; // pull in write(str) and write(buf, size) from Print

In HardwareSerial.cpp:
// Added by RCR 2/10/2015
int HardwareSerial::pending(void)
{
    return txWriteIndex >= txReadIndex ?
		txWriteIndex - txReadIndex :
		txWriteIndex - txReadIndex + txBufferSize;
};

int HardwareSerial::txCapacity(void)
{
    return txWriteIndex >= txReadIndex ?
		txReadIndex - txWriteIndex + txBufferSize:
		txReadIndex - txWriteIndex;
};
