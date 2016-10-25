
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

#ifndef DRV8305Axis_h
#define DRV8305Axis_h

#include "Energia.h"
#include <Axis.h>

class	SsiDeviceQueue;
class	OutputPin;
class	BLDC3;

class	HallFeedback;
class	CurrentFeedback;
class	VoltageFeedback;
class	QuadratureFeedback;

//#define DebugMotorStepISR 1

// Use SPI control - Inherit directly from Axis
class	DRV8305Axis : public Axis {

#ifdef DebugMotorStepISR
	uint32_t	debugCounter;
#endif

	enum UpdateParam {

	// Operation
		ChargePumpThreshold,
		WatchDogDelay,
		SenseAmpClampEnable,
		GateDriverFaultEnable,
		MotorUndervoltageLockoutEnable,
		OverTempSense,
		OverCurrentProtectionSenseEnable,

	// CurrentSense 7
		MaxCurrent,
		HoldingCurrentLimit,
		RunningCurrentLimit,
		AcceleratingCurrentLimit,
		DeceleratingCurrentLimit,
		CSBlankingTime,
		CSA_Gain,
		CSB_Gain,
		CSC_Gain,

	// DriverFET 16
		VdsDeadTime,
		VdsBlankingTime,
		VdsDeglitchTime,
		VdsThreshold,
		VdsMode,
		DeadTimeCompensation,

	// High Side Gate 21
		HSPeakSource,
		HSSourceDuration,
		HSPeakSink,

	// Low Side Gate 24
		LSPeakSource,
		LSSourceDuration,
		LSPeakSink,
	
	// Torque 27
		HoldingTorque,
		RunningTorque,
		AcceleratingTorque,
		DeceleratingTorque,
	
	// Hall Feedback
		HallAngleDerivative,
		HallAngleProportion,
		HallAngleIntegral,
		HallVoltageIntegral,
		StallDetectVoltage,
		SpeedFeebackProportion,
	
	// Quadrature Feedback
		QuadratureCPR,
		QuadratureVoltageProportion,
		QuadratureVoltageIntegral };

	union {
		struct {
			uint8_t	param;
			uint	datum	: 24; } st;
		uint32_t	updateReg; } updateRegU;

	public:

    DRV8305Axis(
		char*		data,
		String*		msgPtr );

    virtual			~DRV8305Axis();

	virtual bool
		is8305() {

		return
			true; };

    SSI*			ssi;

	BLDC3*				motor;
	HallFeedback*		hallFeedback;
	QuadratureFeedback*	quadratureFeedback;
	CurrentFeedback*	currentFeedback;
	VoltageFeedback*	voltageFeedback;

	char		drv8305AxisState;

	void
		enable8305(
			bool	enable );

	void
		addMotor(
			char*		data,
			String*		msgPtr );

	void
		addQuadratureFeedback(
			char*		data,
			String*		msgPtr,
			char		counterType );

	void
		addHallFeedback(
			char*		data,
			String*		msgPtr );

	void
		reportHallPinChange( String* msgPtr );

	void
		addVoltageFeedback(
			char*		data,
			String*		msgPtr );

	void
		addCurrentFeedback(
			char*		data,
			String*		msgPtr );

	virtual void
		updateAxis(
			char*		data,
			String*		msgPtr );

    virtual void
		motorStateChanged();

	protected: // overridden virtual functions
	
    virtual bool		usesSSI(
		SSI*		aSsi );
    virtual bool		isConfigured();

	virtual void		interpolationIsr(	// 2 kHz
		Machine*	machine );

    virtual void		motorStepIsr(); // called on axis interrupt (15kHz)

	virtual void		reportAxisStatus(
		String*		msgPtr );

	int32_t				microStepStopPointI();

	private:

	OutputPin*			chipSelectPin;
	OutputPin*			enablePin;

    void				configureDrv8305(
		uint32_t,
		OutputPin*,
		OutputPin* );
    bool				initialize(
		String*		msg );

	void
		sendRegister ( // Write only
			uint16_t	ctrlRegister );

	void
		increaseResolution();
	
	void
		decreaseResolution();

	bool
		setRegister (
			uint16_t	ctrlRegister,
			String*		msgPtr );

    // Callbacks
    void		gpDone( uint32_t data );
  
    uint32_t	deviceNum;
	
	bool		reverseDirection;
	
	uint32_t	reportedWdStatus;
	uint32_t	reportedOvStatus;
	uint32_t	reportedIcStatus;
	uint32_t	reportedGdStatus;
	
// C++ packs bit fields are in reverse sequence starting with the low order bit
	union {	// 1 Warning and Watchdog Reset Register
		struct {
			uint overTempWarn			: 1;
			uint temp135C				: 1;
			uint temp125C				: 1;
			uint temp105C				: 1;
			uint chargePumpUnderVoltage	: 1;
			uint vdsStatus				: 1;
			uint pvddOverVoltage		: 1;
			uint pvddUnderVoltage		: 1;
			uint temp175C				: 1;
			uint reserved				: 1;
			uint latchedFault			: 1;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	warnWatchdogReg; }
			warnWatchdogRegU;
		
	union {	// 2 OV/VDS Faults Register
		struct {
			uint snsAocp				: 1;
			uint snsBocp				: 1;
			uint snsCocp				: 1;
			uint reserved				: 2;
			uint vdsFetClow				: 1;
			uint vdsFetChigh			: 1;
			uint vdsFetBlow				: 1;
			uint vdsFetBhigh			: 1;
			uint vdsFetAlow				: 1;
			uint vdsFetAhigh			: 1;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	ovFaultsReg; }
			ovFaultsRegU;
		
	union {	// 3 IC Faults
		struct {
			uint vcphOvAbs				: 1;
			uint vcphOv					: 1;
			uint vcphUv					: 1;
			uint reserved				: 1;
			uint vcpLsdUvLow			: 1;
			uint avddUvLow				: 1;
			uint vregUv					: 1;
			uint reserved2				: 1;
			uint overTemp				: 1;
			uint watchDog				: 1;
			uint pvddUvlo2				: 1;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	icFaultsReg; }
			icFaultsRegU;
		
	union {	// 4 Gate Driver VGS Faults
		struct {
			uint reserved				: 5;
			uint fetClowVgs				: 1;
			uint fetChighVgs			: 1;
			uint fetBlowVgs				: 1;
			uint fetBhighVgs			: 1;
			uint fetAlowVgs				: 1;
			uint fetAhighVgs			: 1;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	gateDriverFaultReg; }
			gateDriverFaultRegU;
		
	union {	// 5 HS Gate Driver Control Register
		struct {
			uint peakSourceHS			: 4;
			uint peakSinkHS				: 4;
			uint peakSourceTimeHS		: 2;
			uint reserved				: 1;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	hsGateDriveReg; }
			hsGateDriveRegU;
		
	union {	// 6 LS Gate Driver Control Register
		struct {
			uint peakSourceLS			: 4;
			uint peakSinkLS				: 4;
			uint peakSourceTimeLS		: 2;
			uint reserved				: 1;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	lsGateDriveReg; }
			lsGateDriveRegU;
		
	union {	// 7 Gate Driver Control Register
		struct {
			uint vdsSenseDeglitch		: 2;
			uint vdsSenseBlanking		: 2;
			uint deadTime				: 3;
			uint pwmMode				: 2;
			uint commutationMode		: 1;
			uint reserved				: 1;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	gateDriverReg; }
			gateDriverRegU;
	
	union {	// 9 IC Operation Register
		struct {
			uint chargePumpUvThreshold	: 1;
			uint clearFaults			: 1;
			uint sleep					: 1;
			uint watchDogEnable			: 1;
			uint disableSNSocp			: 1;
			uint watchDogDelay			: 2;
			uint enableSNSclamp			: 1;
			uint disableGateDriverFault	: 1;
			uint disablePvddUvFault		: 1;
			uint enableOts				: 1;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	icOperationReg; }
			icOperationRegU;
	
	union {	// A Shunt Amplifier Register
		struct {
			uint currentSense1Gain		: 2;
			uint currentSense2Gain		: 2;
			uint currentSense3Gain		: 2;
			uint currentSenseBlanking	: 2;
			uint currentSense1Cal		: 1;
			uint currentSense2Cal		: 1;
			uint currentSense3Cal		: 1;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	currentSenseReg; }
			currentSenseRegU;

	union {	// B Voltage Regulator Control Register
		struct {
			uint vregUvLevel			: 2;
			uint disableVregPower		: 1;
			uint sleepDelay				: 2;
			uint reserved				: 3;
			uint vrefScaling			: 2;
			uint reserved2				: 1;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	voltageRegulatorControlReg; }
			voltageRegulatorControlRegU;
		
	union {	// C VDS Sense Control Register
		struct {
			uint vdsMode				: 3;
			uint vdsLevel				: 5;
			uint reserved				: 3;

			uint addr					: 4;
			uint read					: 1; } st;
		uint16_t	vdsSenseControlReg; }
			vdsSenseControlRegU;

    uint16_t	reportedStatus;
    uint16_t	reportedMode;

    bool		configured;

	uint16_t
		readRegister (
			uint16_t ctrlRegister );

	void
		writeRegister (
			uint16_t ctrlRegister );
};

#endif
