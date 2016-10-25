
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

#include <DRV8711Axis.h>


void
	DRV8711Axis::updateAxis(
		char*		data,
		String*		msgPtr ) {

	*msgPtr				+= " Update 8711 ";
	data++;

	// Supports multiple parameter updates, but host only updates one at a time 
	uint32_t  length	= 0;
	while( sscanf( data,
		" %x%ln",
		&updateRegU.updateReg,
		&length ) > 0 ) { // returns -1 at eof

		switch( updateRegU.st.param ) {
	
			case IsGain :
			ctrlRegU.st.isGain						= updateRegU.st.datum;
			*msgPtr			+= "Gain";
			break;

			case DeadTime :
			ctrlRegU.st.deadTime					= updateRegU.st.datum;
			*msgPtr			+= "DeadTime";
			break;

			case ReverseDirection :
			ctrlRegU.st.reverseDirection			= updateRegU.st.datum;
			reverseDirection						= ctrlRegU.st.reverseDirection;
			*msgPtr			+= "ReverseDirection";
			break;

			case BemfSampleTime :
			torqueRegU.st.bemfSampleTime			= updateRegU.st.datum;
			*msgPtr			+= "BemfSampleTime";
			break;

			case OffTime :
			offRegU.st.offTime						= updateRegU.st.datum;
			*msgPtr			+= "Off";
			setRegister(	offRegU.offReg,	msgPtr );
			break;

			case BlankingTime :
			blankRegU.st.blankingTime				= updateRegU.st.datum;
			*msgPtr			+= "Blanking";
			setRegister(	blankRegU.blankReg,	msgPtr );
			break;

			case AdaptiveBlanking :
			blankRegU.st.adaptiveBlanking			= updateRegU.st.datum;
			*msgPtr			+= "Blanking";
			setRegister(	blankRegU.blankReg,	msgPtr );
			break;

			case TransitionTime :
			decayRegU.st.transitionTime				= updateRegU.st.datum;
			*msgPtr			+= "Decay";
			setRegister(	decayRegU.decayReg,	msgPtr );
			break;

			case DecayMode :
			decayRegU.st.decayMode					= updateRegU.st.datum;
			*msgPtr			+= "Decay";
			setRegister(	decayRegU.decayReg,	msgPtr );
			break;

			case StallDetectThreshold :
			stallRegU.st.stallDetectThreshold		= updateRegU.st.datum;
			*msgPtr			+= "Stall";
			setRegister(	stallRegU.stallReg,	msgPtr );
			break;

			case StallDelay :
			stallRegU.st.stallDelay					= updateRegU.st.datum;
			*msgPtr			+= "Stall";
			setRegister(	stallRegU.stallReg,	msgPtr );
			break;

			case BemfDivider :
			stallRegU.st.bemfDivider				= updateRegU.st.datum;
			*msgPtr			+= "Stall";
			setRegister(	stallRegU.stallReg,	msgPtr );
			break;

			case OutputCurrentThreshold :
			driveRegU.st.outputCurrentThreshold		= updateRegU.st.datum;
			*msgPtr			+= "Drive";
			setRegister(	driveRegU.driveReg,	msgPtr );
			break;

			case OutputCurrentDelay :
			driveRegU.st.outputCurrentDelay			= updateRegU.st.datum;
			*msgPtr			+= "Drive";
			setRegister(	driveRegU.driveReg,	msgPtr );
			break;

			case LowGateDelay :
			driveRegU.st.lowGateDelay				= updateRegU.st.datum;
			*msgPtr			+= "Drive";
			setRegister(	driveRegU.driveReg,	msgPtr );
			break;

			case HighGateDelay :
			driveRegU.st.highGateDelay				= updateRegU.st.datum;
			*msgPtr			+= "Drive";
			setRegister(	driveRegU.driveReg,	msgPtr );
			break;

			case LowGateThreshold :
			driveRegU.st.lowGateThreshold			= updateRegU.st.datum;
			*msgPtr			+= "Drive";
			setRegister(	driveRegU.driveReg,	msgPtr );
			break;

			case HighGateThreshold :
			driveRegU.st.highGateThreshold			= updateRegU.st.datum;
			*msgPtr			+= "Drive";
			setRegister(	driveRegU.driveReg,	msgPtr );
			break;

			case HoldTorque :
			powerLevelsU.st.holdTorque				= updateRegU.st.datum;
			*msgPtr			+= "Hold";
			break;

			case ConstantTorque :
			powerLevelsU.st.constantTorque			= updateRegU.st.datum;
			*msgPtr			+= "Constant";
			break;

			case AcceleratingTorque :
			powerLevelsU.st.acceleratingTorque		= updateRegU.st.datum;
			*msgPtr			+= "Accel";
			break;

			case DeceleratingTorque :
			powerLevelsU.st.deceleratingTorque		= updateRegU.st.datum;
			*msgPtr			+= "Decel";
			break;

			case AdaptiveMicroStepping :
			optionsU.st.adaptiveMicroStepping		= updateRegU.st.datum;
			*msgPtr			+= "Microstep";
			break;
			
			case MinStallDetectSpeed :
			optionsU.st.minStallDetectSpeed			= updateRegU.st.datum;
			*msgPtr			+= "StallDetectSpeed";
			
			case EnableStallDetect :
			optionsU.st.enableStallDetect			= updateRegU.st.datum;
			*msgPtr			+= "EnableStallDetect";
		};
		data	+= length; };

	*msgPtr			+= "\n"; };
