
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

#ifndef StepDirectionAxis_h
#define StepDirectionAxis_h

#include "Energia.h"
#include "Axis.h"

class OutputPin;


class StepDirectionAxis : public Axis { // Step/Direction interface axis  
	public:

    StepDirectionAxis( char* data, String* msgPtr );

    virtual		~StepDirectionAxis();

    void		motorStepIsr();
 
    bool		isConfigured();
    
	protected: // make available to DRV8711Axis
    OutputPin*	directionPin;        // ACTIVE_HIGH is left to right, front to back, down or clockwise
    OutputPin*	stepPin;             // ACTIVE_HIGH is low to high transition

	private:
    volatile enum PulseState {
		Quiescent,
		StateStepPositive,
		StateStepNegative,
		NeedsPulseReset } phaseAngle;
};

#endif
