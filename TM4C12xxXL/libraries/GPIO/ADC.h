
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

#ifndef ADC_h
#define ADC_h

#include "Energia.h"

class	AnalogPin;


class ADC {
	// sequenceNumbers 0-3 are for ADC0_BASE
	// sequenceNumbers 4-8 are for ADC1_BASE
	static AnalogPin*	sequencerReg[ 8 ][ 8 ];
	static bool			adcRunning;

	public:

	static void
		configure();

	static void
		configureBase(		uint32_t adcBase );

	static void
		addAnalogPin(		AnalogPin* analogPin );

	static void
		removeAnalogPin(	AnalogPin* analogPin );

	static void
		intHandler(
			uint32_t	adcBase,
			uint32_t	sequenceNumber,
			uint32_t	fifo );

	static void
		triggerNextSequence(
			uint32_t	adcBase,
			uint32_t	sequenceNumber );

};

#endif
