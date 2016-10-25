
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

#include <SSI8305.h>
#include <SsiDeviceQueue.h>
#include <Machine.h>
#include <AxisTimer.h>
#include <OutputPin.h>
#include <Timers.h>

#include <inc/hw_ssi.h>
#include <inc/hw_gpio.h>
#include <driverlib/ssi.h>



SSI8305::SSI8305(
	char*		data,
	String*		msgPtr )
	
	:	SSI(
			data,
			msgPtr ) {

	if ( configured ) {
		configureSSI( TI_8305_Protocol );

		snprintf( data, 40,
			"\n SSI %lu configured for BOOST-DRV8305\n",
			number ); 
		*msgPtr     += String( data ); }; };

SSI8305::~SSI8305() {};
