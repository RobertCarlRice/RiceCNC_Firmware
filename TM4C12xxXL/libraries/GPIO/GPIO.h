
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

#ifndef GPIO_h
#define GPIO_h

#include <Energia.h>

#include <driverlib/sysctl.h>

class InputPin;

typedef struct {
	uint32_t	sysCtl;
	uint32_t	base;
	uint32_t	interruptVectorNumber; } gpioBaseStruct;

#ifdef PART_TM4C1294NCPDT
	#define BaseTableLength 17
	static gpioBaseStruct GPIOBaseVector[ BaseTableLength ]   = {
		{ SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, 16 },
		{ SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, 17 },
		{ SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, 18 },
		{ SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, 19 },
		{ SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, 20 },
		{ SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, 46 },
		{ SYSCTL_PERIPH_GPIOG, GPIO_PORTG_BASE, 47 },
		{ SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE, 48 },
		{ 0, 0 },
		{ SYSCTL_PERIPH_GPIOJ, GPIO_PORTJ_BASE, 67 },
		{ SYSCTL_PERIPH_GPIOK, GPIO_PORTK_BASE, 68 },
		{ SYSCTL_PERIPH_GPIOL, GPIO_PORTL_BASE, 69 },
		{ SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, 88 },
		{ SYSCTL_PERIPH_GPION, GPIO_PORTN_BASE, 89 },
		{ 0, 0 },
		{ SYSCTL_PERIPH_GPIOP, GPIO_PORTP_BASE, 93 },
		{ SYSCTL_PERIPH_GPIOQ, GPIO_PORTQ_BASE, 100 } };
  
#elif defined( PART_LM4F120H5QR )
	#define BaseTableLength 6
	static gpioBaseStruct GPIOBaseVector[ BaseTableLength ]	= {
		{ SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, 16 },
		{ SYSCTL_PERIPH_GPIOB, GPIO_PORTB_BASE, 17 },
		{ SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, 18 },
		{ SYSCTL_PERIPH_GPIOD, GPIO_PORTD_BASE, 19 },
		{ SYSCTL_PERIPH_GPIOE, GPIO_PORTE_BASE, 20 },
		{ SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, 46 } };

#endif


class GpioPin {
	public:
		GpioPin(
			char*		data,
			String*		msgPtr );
		virtual		~GpioPin();

		gpioBaseStruct* baseStruct;

		uint32_t		gpioBase;
		uint32_t		pinSelectMask;
		uint32_t		parseLength;
	
		char			driveCurrent;
		char			activeMode;
		bool			activeLow;
		char			portC;

		uint8_t			mask;
		uint8_t			pin;

		// Analog
		virtual uint32_t	analogValue() {
			return 0; };
	
		virtual void		averageSample(
			uint32_t sample ) {}; // sample is 12 bits
	
		virtual void		reportAnalog(
			String*	msgPtr ) {};

		uint16_t		id();

		bool			valid();

		virtual bool	readPin();

		virtual void	fmtPin(
			String*	msgPtr ) {};
		virtual void	reportPinChange(
			String*	msgPtr ) {};

		virtual void	setPin( bool ) {}; // Can be called at interrupt level
		virtual void	togglePin() {};
};

#endif
