
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

#ifndef AnalogPin_h
#define AnalogPin_h

#include <Energia.h>
#include <GPIO.h>

#include <driverlib/sysctl.h>


typedef struct {
	uint32_t	channelSelect;
	char		port;
	uint8_t		pin; } AnalogChannelPins;

#ifdef PART_TM4C1294NCPDT

	#define AnalogTableLength 20
	static AnalogChannelPins AnalogChannel[ AnalogTableLength ]   = {
		{ ADC_CTL_CH0,	'E', 3 },
		{ ADC_CTL_CH1,	'E', 2 },
		{ ADC_CTL_CH2,	'E', 1 },
		{ ADC_CTL_CH3,	'E', 0 },
		{ ADC_CTL_CH4,	'D', 7 },
		{ ADC_CTL_CH5,	'D', 6 },
		{ ADC_CTL_CH6,	'D', 5 },
		{ ADC_CTL_CH7,	'D', 4 },
		{ ADC_CTL_CH8,	'E', 5 },
		{ ADC_CTL_CH9,	'E', 4 },
		{ ADC_CTL_CH10,	'B', 4 },
		{ ADC_CTL_CH11,	'B', 5 },
		{ ADC_CTL_CH12,	'D', 3 },
		{ ADC_CTL_CH13,	'D', 2 },
		{ ADC_CTL_CH14,	'D', 1 },
		{ ADC_CTL_CH15,	'D', 0 },
		{ ADC_CTL_CH16,	'K', 0 },
		{ ADC_CTL_CH17,	'K', 1 },
		{ ADC_CTL_CH18,	'K', 2 },
		{ ADC_CTL_CH19,	'K', 3 }, };
  
#elif defined( PART_LM4F120H5QR )

	#define AnalogTableLength 12
	static AnalogChannelPins AnalogChannel[ AnalogTableLength ]   = {
		{ ADC_CTL_CH0,	'E', 3 },
		{ ADC_CTL_CH1,	'E', 2 },
		{ ADC_CTL_CH2,	'E', 1 },
		{ ADC_CTL_CH3,	'E', 0 },
		{ ADC_CTL_CH4,	'D', 3 },
		{ ADC_CTL_CH5,	'D', 2 },
		{ ADC_CTL_CH6,	'D', 1 },
		{ ADC_CTL_CH7,	'D', 0 },
		{ ADC_CTL_CH8,	'E', 5 },
		{ ADC_CTL_CH9,	'E', 4 },
		{ ADC_CTL_CH10,	'B', 4 },
		{ ADC_CTL_CH11,	'B', 5 } };

#endif


class AnalogPin : public GpioPin {
	uint32_t	analogAverage;
	uint32_t	reportedAnalogValue;
	uint32_t	softwareOvesampling;

	public:
		AnalogPin(
			char*		data,
			String*		msgPtr );
		~AnalogPin();

		AnalogPin*			nextAnalogPin;

		AnalogChannelPins*	aid;

		virtual uint32_t	analogValue();
		virtual void		averageSample( uint32_t sample );

		void				fmtPin(			String*	msgPtr );
		virtual void		reportAnalog(	String*	msgPtr );
		bool				enabled;
};

#endif
