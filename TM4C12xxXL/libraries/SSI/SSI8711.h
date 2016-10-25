
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

#ifndef SSI8711_h
#define SSI8711_h

#include "Energia.h"
#include <SSI.h>

#include <driverlib/sysctl.h>

class OutputPin;

// Parameters for L6470 compatibility - L6470 Max shift clock is 5 Mhz
// Freescale, inverted clock, delayed, 8 bit framing
#define TI_8711_Protocol  SSI_CR0_FRF_MOTO | SSI_CR0_DSS_16


class SSI8711 : public SSI {
  
	public:
	SSI8711(
		char*		data,
		String*		msgPtr );
    ~SSI8711();

	private:
    OutputPin*	resetPin;
	OutputPin*	sleepPin;
    OutputPin*	directionPin;
	OutputPin*	stepPin; };

#endif

