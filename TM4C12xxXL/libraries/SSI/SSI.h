
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

#ifndef SSI_h
#define SSI_h

#include "Energia.h"
#include <driverlib/sysctl.h>

class SsiDeviceQueue;
class OutputPin;

// Parameters for L6470 compatibility - L6470 Max shift clock is 5 Mhz
// Freescale, inverted clock, delayed, 8 bit framing
#define ST_Protocol  SSI_CR0_FRF_MOTO | SSI_CR0_SPO | SSI_CR0_SPH | SSI_CR0_DSS_8
//#define TI_Protocol  SSI_CR0_FRF_MOTO | SSI_CR0_DSS_16

typedef struct {
	char        portC;
	uint8_t     pin;
	uint32_t	config; } PortStruct;

typedef struct {
	uint32_t    sysCtl;
	uint32_t    base;
	PortStruct  txPin;
	PortStruct  rxPin;
	PortStruct  clkPin; } baseStruct;


#ifdef PART_TM4C1294NCPDT
  static baseStruct base[4]   = {
  // Can't use hardware generated frame select
	{ SYSCTL_PERIPH_SSI0,
      SSI0_BASE,
      { 'A', 4, GPIO_PA4_SSI0XDAT0 },	// tx
      { 'A', 5, GPIO_PA5_SSI0XDAT1 },	// rx
      { 'A', 2, GPIO_PA2_SSI0CLK } },	// clk

    { SYSCTL_PERIPH_SSI1,
      SSI1_BASE,
      { 'E', 4, GPIO_PE4_SSI1XDAT0 },
      { 'E', 5, GPIO_PE5_SSI1XDAT1 },
      { 'B', 5, GPIO_PB5_SSI1CLK } },
 
    { SYSCTL_PERIPH_SSI2,				// Pinout on BoosterPack 1
      SSI2_BASE,
      { 'D', 1, GPIO_PD1_SSI2XDAT0 },	// D6
      { 'D', 0, GPIO_PD0_SSI2XDAT1 },	// D7
      { 'D', 3, GPIO_PD3_SSI2CLK } },	// A7
 
    { SYSCTL_PERIPH_SSI3,				// Pinout on BoosterPack 2
      SSI3_BASE,
      { 'Q', 2, GPIO_PQ2_SSI3XDAT0 },	// D6
      { 'Q', 3, GPIO_PQ3_SSI3XDAT1 },	// D7
      { 'Q', 0, GPIO_PQ0_SSI3CLK } } };	// A7


#elif defined( PART_LM4F120H5QR )

  static baseStruct base[4]   = {
    { SYSCTL_PERIPH_SSI0,
      SSI0_BASE,
      { 'A', 4, GPIO_PA4_SSI0RX },
      { 'A', 5, GPIO_PA5_SSI0TX },
      { 'A', 2, GPIO_PA2_SSI0CLK } },

    { SYSCTL_PERIPH_SSI1,
      SSI1_BASE,
      { 'F', 0, GPIO_PF0_SSI1RX },
      { 'F', 1, GPIO_PF1_SSI1TX },
      { 'F', 2, GPIO_PF2_SSI1CLK } },
 
    { SYSCTL_PERIPH_SSI2,
      SSI2_BASE,
      { 'B', 6, GPIO_PB6_SSI2RX },		// D6
      { 'B', 7, GPIO_PB7_SSI2TX },		// D7
      { 'B', 4, GPIO_PB4_SSI2CLK } },	// A7
 
    { SYSCTL_PERIPH_SSI3,
      SSI3_BASE,
      { 'D', 2, GPIO_PD2_SSI3RX },
      { 'D', 3, GPIO_PD3_SSI3TX },
      { 'D', 0, GPIO_PD0_SSI3CLK } } };

#endif


class SSI {
  
	public:
	SSI(
		char*		data,
		String*		msgPtr );
    virtual		~SSI();

	static SSI*		firstSsi;
    SSI*			nextSSI;

	bool			configured;
    uint32_t		number;
	uint32_t		parseLength;
	
	
	OutputPin*		chipSelectPin;

	volatile uint32_t	buffersInQueue;
    
	static SSI*
		ssiWithNumber( uint32_t ssiNumber );

    SsiDeviceQueue*	firstSsiDeviceQueue;
    SsiDeviceQueue*
		ssiDeviceQueueWithNumber( uint32_t deviceNum );

    uint32_t	ssiBase;
	uint32_t	shiftClockFrequency;

	void
		sendWord( uint16_t data ); // data is right justified

	void
		waitUntilIdle();

	uint16_t
		readWord();

	void
		eotIntHandler();
        
	void
		enableSsiInterrupt();

	void
		disableSsiInterrupt();

    void
		startSpiTransfer();

	void
		selectUsingPin( OutputPin* pin );

	void
		deselect();

    void
		configurePin( PortStruct*, String* msgPtr );

    void
		configureSSI( uint32_t protocol );

	private:
	char		configType;

    uint32_t	devices;

	void
		configureGPIOs( baseStruct* myBase, String* msgPtr );

    bool
		autoDetectDevices();
    bool
		detectDevices();

    void
		create_SsiDeviceQueues();
/*
    void
		configureInterrupts();
*/
    void
		startSsiInterrupt();
    void
		endSsiInterrupt();
};

#endif

