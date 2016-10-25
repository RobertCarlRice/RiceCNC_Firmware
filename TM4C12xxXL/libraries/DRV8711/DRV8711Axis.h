
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

#ifndef DRV8711Axis_h
#define DRV8711Axis_h

#include "Energia.h"
#include <Axis.h>


class	SsiDeviceQueue;
class	OutputPin;
class	AnalogPin;


// Use SPI control - Inherit directly from Axis
class	DRV8711Axis : public Axis {

	enum DRV8711UpdateParam {
		IsGain,
		DeadTime,
		ReverseDirection,
		BemfSampleTime,
		OffTime,
		BlankingTime,
		AdaptiveBlanking,
		TransitionTime,
		DecayMode,
		StallDetectThreshold,
		StallDelay,
		BemfDivider,
		OutputCurrentThreshold,
		OutputCurrentDelay,
		LowGateDelay,
		HighGateDelay,
		LowGateThreshold,
		HighGateThreshold,
		HoldTorque,
		ConstantTorque,
		AcceleratingTorque,
		DeceleratingTorque,
		AdaptiveMicroStepping,
		MinStallDetectSpeed,
		EnableStallDetect };

	public:

    DRV8711Axis( char* data, String* msgPtr );

	virtual void		updateAxis(	char* data, String* msgPtr );

    virtual				~DRV8711Axis();

    SSI*				ssi;


	protected: // overridden virtual functions
	
    virtual bool		usesSSI( SSI* aSsi );
    virtual bool		isConfigured();

    virtual void		motorStepIsr(); // called on axis interrupt

    virtual void		motorStateChanged();
	virtual void		reportAxisStatus(	String* msgPtr );
	virtual void		reportAnalog(		String* msgPtr );

	int32_t				microStepStopPointI();


	private:

	OutputPin*			chipSelectPin;
	AnalogPin*			potPin;

    bool				initialize(  String*  msg );

	void
		increaseResolution();
	
	void
		decreaseResolution();

	bool
		setRegister (
			uint16_t	ctrlRegister,
			String*		msgPtr );

    // Callbacks
    void				gpDone( uint32_t data );
  
    uint32_t			deviceNum;
	
	bool				reverseDirection;
	
// C++ packs bit fields are in reverse sequence starting with the low order bit
	union {
		struct {
			uint datum					: 8;
			uint param					: 8; } st;
		uint16_t	updateReg; } updateRegU;

	union {
		struct {
			uint enableMotor			: 1;
			uint reverseDirection		: 1;
			uint forceStep				: 1;
			uint microStep				: 4;
			uint exStall				: 1; // External not useful - board holds active
			uint isGain					: 2;			
			uint deadTime				: 2;
	
			uint addr					: 3; // 0
			uint read					: 1; } st;
		uint16_t	ctrlReg; } ctrlRegU;

	union {
		struct {
			uint torque					: 8;
			uint bemfSampleTime			: 3;
			uint reserved				: 1;

			uint addr					: 3; // 1
			uint read					: 1; } st;
		uint16_t	torqueReg; } torqueRegU;
		
	union {
		struct {
			uint offTime				: 8;
			uint bypassIndexer			: 1;
			uint reserved				: 3;

			uint addr					: 3; // 2
			uint read					: 1; } st;
		uint16_t	offReg; } offRegU;
		
	union {
		struct {
			uint blankingTime			: 8;
			uint adaptiveBlanking		: 1;
			uint reserved				: 3;

			uint addr					: 3; // 3
			uint read					: 1; } st;
		uint16_t	blankReg; } blankRegU;
		
	union {
		struct {
			uint transitionTime			: 8;
			uint decayMode				: 3;
			uint reserved				: 1;

			uint addr					: 3; // 4
			uint read					: 1; } st;
		uint16_t	decayReg; } decayRegU;
		
	union {
		struct {
			uint stallDetectThreshold	: 8;
			uint stallDelay				: 2;
			uint bemfDivider			: 2;

			uint addr					: 3; // 5
			uint read					: 1; } st;
		uint16_t	stallReg; } stallRegU;
		
	union {
		struct {
			uint outputCurrentThreshold	: 2;
			uint outputCurrentDelay		: 2;
			uint lowGateDelay			: 2;
			uint highGateDelay			: 2;
			uint lowGateThreshold		: 2;
			uint highGateThreshold		: 2;

			uint addr					: 3; // 6
			uint read					: 1; } st;
		uint16_t	driveReg; } driveRegU;
	
	// Status Register
	union {
		struct {
			uint overtemperatureShutdown		: 1;	// 0x01
			uint channelAovercurrentShutdown	: 1;	// 0x02 // latched - write 0 to clear
			uint channelBovercurrentShutdown	: 1;	// 0x04 // latched - write 0 to clear
			uint channelApreDriverFault			: 1;	// 0x08 // latched - write 0 to clear
			uint channelBpreDriverFault			: 1;	// 0x10 // latched - write 0 to clear
			uint underVoltageLockout			: 1;	// 0x20
			uint stallDetect					: 1;	// 0x40
			uint latchedStallDetect				: 1;	// 0x80 // latched - write 0 to clear
			uint reserved						: 4;

			uint addr							: 3; // 7
			uint read							: 1; } st;
		uint16_t	statusReg; } statusRegU;

	// Additional data not stored in 8711 registers
	union {
		struct {
			uint holdTorque				: 8;
			uint constantTorque			: 8;
			uint acceleratingTorque		: 8;
			uint deceleratingTorque		: 8; } st;
		uint32_t	powerLevels; } powerLevelsU;
		
	union {
		struct {
			uint minStallDetectSpeed	: 16; // Steps per second
			uint enableStallDetect		: 1;
			uint adaptiveMicroStepping	: 1; } st;
		uint32_t	options; } optionsU;

    uint32_t	microStepSizeU;
    float		microStepSizeF;
    float		halfMicroStepSizeF;
    uint32_t	indexerPositionU;
    uint32_t	lastPosU;
	
	float		minStallDetectSpeedF;

    uint16_t	reportedStatus;
    uint16_t	reportedMode;
	
	// Analog
    uint32_t	reportedAnalog;

    bool		configured;

	uint16_t
		readRegister (
			uint16_t ctrlRegister );

	void
		writeRegister (
			uint16_t ctrlRegister );
};

#endif
