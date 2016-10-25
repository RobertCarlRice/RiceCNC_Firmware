
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

#ifndef Spindle_h
#define Spindle_h

#include "Energia.h"
class OutputPin;
class PwmOutputPin;
class Machine;

class Spindle {
	Machine*	machine;

	struct {
		uint	spindleOn	: 1;
		uint	spindleCCW	: 1;
		uint	pulseOn		: 1; } flags;
	
	float		setScaledSpeed;
	char		useAxis;
  
	// independent mode
	int32_t			pwmFrequency;    // PWM
	PwmOutputPin*	runPin;
	OutputPin*		directionPin;

	void		setAxisSpeed();

	public:

    Spindle(
		Machine*	myMachine,
		char*		data,
		String*		msgPtr );
    ~Spindle();
  
	uint32_t	onDelayMs;
	uint32_t	offDelayMs;

	bool		isValid();

    void        setSpindleDirection(	bool );
    void        setSpindleOn(			bool );
    void        setSpindleSpeed(		float scaledSpeed );
    bool		counterClockwise();
    bool		running(); };

#endif
