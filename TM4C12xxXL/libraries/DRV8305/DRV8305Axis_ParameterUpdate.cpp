
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

#include <DRV8305Axis.h>
#include <SsiDeviceQueue.h>
#include <MainCommands.h>
#include <SSI.h>
#include <MachineCommands.h>
#include <OutputPin.h>
#include <AxisTimer.h>
#include <ADC.h>
#include <BLDC3PWM.h>
#include <BLDC3PFM.h>
#include <QuadratureFeedback.h>
#include <HallFeedback.h>
#include <CurrentFeedback.h>
#include <VoltageFeedback.h>
#include <InterpolationTimer.h>

#include <SysTick.h>

#include <inc/hw_gpio.h>
#include <driverlib/ssi.h>


void
	DRV8305Axis::updateAxis(
		char*		data,
		String*		msgPtr ) {

	*msgPtr		+= " Update 8305 ";
	data++;

	// Supports multiple parameter updates, but host only updates one at a time 
	uint32_t  length	= 0;
	while( sscanf( data,
		" %x%ln",
		&updateRegU.updateReg,
		&length ) > 0 ) { // returns -1 at eof

		switch( updateRegU.st.param ) {

		// Operation
			case ChargePumpThreshold :
			icOperationRegU.st.chargePumpUvThreshold =
				updateRegU.st.datum;
			*msgPtr		+= "Charge Pump Threshold";
			setRegister(
				icOperationRegU.icOperationReg,
				msgPtr );
			break;

			case WatchDogDelay :
			icOperationRegU.st.watchDogDelay =
				updateRegU.st.datum;
			*msgPtr		+= "WatchDog Delay";
			setRegister(
				icOperationRegU.icOperationReg,
				msgPtr );
			break;

			case SenseAmpClampEnable :
			icOperationRegU.st.enableSNSclamp =
				! updateRegU.st.datum;
			*msgPtr		+= "Sense Clamp Enable";
			setRegister(
				icOperationRegU.icOperationReg,
				msgPtr );
			break;

			case GateDriverFaultEnable :
			icOperationRegU.st.disableGateDriverFault =
				! updateRegU.st.datum;
			*msgPtr		+= "Gate Driver Fault Enable";
			setRegister(
				icOperationRegU.icOperationReg,
				msgPtr );
			break;

			case MotorUndervoltageLockoutEnable :
			icOperationRegU.st.disablePvddUvFault =
				! updateRegU.st.datum;
			*msgPtr		+= "Motor Undervoltage Lockout Enable";
			setRegister(
				icOperationRegU.icOperationReg,
				msgPtr );
			break;

			case OverTempSense :
			icOperationRegU.st.enableOts =
				updateRegU.st.datum;
			*msgPtr		+= "Over Heat Lockout Enable";
			setRegister(
				icOperationRegU.icOperationReg,
				msgPtr );
			break;

			case OverCurrentProtectionSenseEnable :
			icOperationRegU.st.disableSNSocp =
				! updateRegU.st.datum;
			*msgPtr		+= "Over Current Lockout Enable";
			setRegister(
				icOperationRegU.icOperationReg,
				msgPtr );
			break;

		// DriverFET
			case VdsDeadTime :
			gateDriverRegU.st.deadTime =
				updateRegU.st.datum;
			*msgPtr		+= "VDS Dead Time";
			setRegister(
				gateDriverRegU.gateDriverReg,
				msgPtr );
			break;

			case VdsMode :
			gateDriverRegU.st.pwmMode =
				updateRegU.st.datum;
			*msgPtr		+= "PWM Mode";
			setRegister(
				gateDriverRegU.gateDriverReg,
				msgPtr );
			break;

			case VdsThreshold :
			vdsSenseControlRegU.st.vdsLevel	=
				updateRegU.st.datum;
			*msgPtr		+= "VDS Threshold";
			setRegister(
				vdsSenseControlRegU.vdsSenseControlReg,
				msgPtr );
			break;

			case VdsDeglitchTime :
			gateDriverRegU.st.vdsSenseDeglitch =
				updateRegU.st.datum;
			*msgPtr		+= "VDS Deglitch";
			setRegister(
				gateDriverRegU.gateDriverReg,
				msgPtr );
			break;

			case VdsBlankingTime :
			gateDriverRegU.st.vdsSenseBlanking =
				updateRegU.st.datum;
			*msgPtr		+= "VDS Blanking";
			setRegister(
				gateDriverRegU.gateDriverReg,
				msgPtr );
			break;

			case DeadTimeCompensation :
			if ( motor ) {
				motor->minimumPulseWidth = updateRegU.st.datum;
				*msgPtr		+= "Dead Time Compensation"; };
			break;

		// CurrentSense
			case CSA_Gain :
			currentSenseRegU.st.currentSense1Gain =
				updateRegU.st.datum;
			*msgPtr		+= "CSA Gain";
			setRegister(
				currentSenseRegU.currentSenseReg,
				msgPtr );
			break;

			case CSB_Gain :
			currentSenseRegU.st.currentSense2Gain =
				updateRegU.st.datum;
			*msgPtr		+= "CSB Gain";
			setRegister(
				currentSenseRegU.currentSenseReg,
				msgPtr );
			break;

			case CSC_Gain :
			currentSenseRegU.st.currentSense3Gain =
				updateRegU.st.datum;
			*msgPtr		+= "CSC Gain";
			setRegister(
				currentSenseRegU.currentSenseReg,
				msgPtr );
			break;

			case CSBlankingTime :
			currentSenseRegU.st.currentSenseBlanking =
				updateRegU.st.datum;
			*msgPtr		+= "CS Blanking";
			setRegister(
				currentSenseRegU.currentSenseReg,
				msgPtr );
			break;

			case MaxCurrent :
			if ( currentFeedback ) {
				currentFeedback->maxCurrentScale =
					updateRegU.st.datum;
				motorStateChanged();
				*msgPtr		+= "Max Current Limit"; };
			break;

			case HoldingCurrentLimit :
			if ( currentFeedback ) {
				uint8_t holdTorque = updateRegU.st.datum;
				currentFeedback->powerLevelsU.st.holdCurrent =
					holdTorque;
				motorStateChanged();
				*msgPtr		+= "Holding Current"; };
			break;

			case RunningCurrentLimit :
			if ( currentFeedback ) {
				currentFeedback->powerLevelsU.st.constantCurrent =
					updateRegU.st.datum;
				motorStateChanged();
				*msgPtr		+= "Running Current"; };
			break;

			case AcceleratingCurrentLimit :
			if ( currentFeedback ) {
				currentFeedback->powerLevelsU.st.acceleratingCurrent =
					updateRegU.st.datum;
				motorStateChanged();
				*msgPtr		+= "Accelerating Current"; };
			break;

			case DeceleratingCurrentLimit :
			if ( currentFeedback ) {
				currentFeedback->powerLevelsU.st.deceleratingCurrent =
					updateRegU.st.datum;
				motorStateChanged();
				*msgPtr		+= "Decelerating Current"; };
			break;

		// High Side Gate
			case HSPeakSource :
			hsGateDriveRegU.st.peakSourceHS	=
				updateRegU.st.datum;
			*msgPtr		+= "HS Peak Source";
			setRegister(
				hsGateDriveRegU.hsGateDriveReg,
				msgPtr );
			break;

			case HSSourceDuration :
			hsGateDriveRegU.st.peakSourceTimeHS	=
				updateRegU.st.datum;
			*msgPtr		+= "HS Source Duration";
			setRegister(
				hsGateDriveRegU.hsGateDriveReg,
				msgPtr );
			break;

			case HSPeakSink :
			hsGateDriveRegU.st.peakSinkHS =
				updateRegU.st.datum;
			*msgPtr		+= "HS Peak Sink";
			setRegister(
				hsGateDriveRegU.hsGateDriveReg,
				msgPtr );
			break;

		// Low Side Gate
			case LSPeakSource :
			lsGateDriveRegU.st.peakSourceLS	=
				updateRegU.st.datum;
			*msgPtr		+= "LS Peak Source";
			setRegister(
				lsGateDriveRegU.lsGateDriveReg,
				msgPtr );
			break;

			case LSSourceDuration :
			lsGateDriveRegU.st.peakSourceTimeLS	=
				updateRegU.st.datum;
			*msgPtr		+= "LS Source Duration";
			setRegister(
				lsGateDriveRegU.lsGateDriveReg,
				msgPtr );
			break;

			case LSPeakSink :
			lsGateDriveRegU.st.peakSinkLS =
				updateRegU.st.datum;
			*msgPtr		+= "LS Peak Sink";
			setRegister(
				lsGateDriveRegU.lsGateDriveReg,
				msgPtr );
			break;

			case HoldingTorque :
			if ( motor ) {
				uint8_t holdVoltage	= updateRegU.st.datum;
				motor->powerLevelsU.st.holdVoltage = holdVoltage;
				motor->holdTorque	= holdVoltage << 5;
				motorStateChanged();
				*msgPtr		+= "Max Holding Voltage ";
				*msgPtr		+= String( holdVoltage ); };
			break;

			case RunningTorque :
			if ( motor ) {
				uint8_t constantVoltage = updateRegU.st.datum;
				motor->powerLevelsU.st.constantVoltage = constantVoltage;
				motorStateChanged();
				*msgPtr		+= "Max Running Voltage ";
				*msgPtr		+= String( constantVoltage ); };
			break;

			case AcceleratingTorque :
			if ( motor ) {
				uint8_t acceleratingVoltage = updateRegU.st.datum;
				motor->powerLevelsU.st.acceleratingVoltage = acceleratingVoltage;
				motorStateChanged();
				*msgPtr		+= "Max Accelerating Voltage ";
				*msgPtr		+= String( acceleratingVoltage ); };
			break;

			case DeceleratingTorque :
			if ( motor ) {
				uint8_t deceleratingVoltage = updateRegU.st.datum;
				motor->powerLevelsU.st.deceleratingVoltage = deceleratingVoltage;
				motorStateChanged();
				*msgPtr		+= "Max Decelerating Voltage ";
				*msgPtr		+= String( deceleratingVoltage ); };
			break;

			case HallAngleProportion :
			if ( hallFeedback ) {
				hallFeedback->angleProportion =
					float ( updateRegU.st.datum ) / 256.0;
				*msgPtr		+= "Hall Voltage Proportion ";
				*msgPtr		+= String( hallFeedback->angleProportion ); };
			break;

			case HallVoltageIntegral :
			if ( hallFeedback ) {
				hallFeedback->voltageIntegral =
					float ( updateRegU.st.datum ) / 256.0;
				*msgPtr		+= "Hall Voltage Integral ";
				*msgPtr		+= String( hallFeedback->voltageIntegral ); };
			break;

			case HallAngleDerivative :
			if ( hallFeedback ) {
				hallFeedback->angleDerivative =
					float ( updateRegU.st.datum ) / 256.0;
				*msgPtr		+= "Hall Voltage Derivative ";
				*msgPtr		+= String( hallFeedback->angleDerivative ); };
			break;

			case HallAngleIntegral :
			if ( hallFeedback ) {
				hallFeedback->angleIntegral =
					float ( updateRegU.st.datum ) / 256.0;
				*msgPtr		+= "Hall SpeedFeeback Proportion ";
				*msgPtr		+= String( hallFeedback->angleIntegral ); };
			break;

			case QuadratureCPR :
			if ( quadratureFeedback ) {
				quadratureFeedback->ppr = updateRegU.st.datum;
				*msgPtr		+= "Quadrature PPR ";
				*msgPtr		+= String( quadratureFeedback->ppr ); };
			break;

			case QuadratureVoltageProportion :
			if ( quadratureFeedback ) {
				quadratureFeedback->voltageProportion =
					float ( updateRegU.st.datum ) / 256.0;
				*msgPtr		+= "Quadrature Voltage Proportion ";
				*msgPtr		+= String( quadratureFeedback->voltageProportion ); };
			break;

			case QuadratureVoltageIntegral :
			if ( quadratureFeedback ) {
				quadratureFeedback->voltageIntegral =
					float ( updateRegU.st.datum ) / 256.0;
				*msgPtr		+= "Quadrature Voltage Integral ";
				*msgPtr		+= String( quadratureFeedback->voltageIntegral ); };
			break;

			case StallDetectVoltage :
			if ( hallFeedback ) {
				hallFeedback->stallDetectVoltage =
					updateRegU.st.datum;
				*msgPtr		+= "Stall Detect Voltage ";
				*msgPtr		+= String( hallFeedback->stallDetectVoltage ); };
			break;

			case SpeedFeebackProportion :
			if ( hallFeedback ) {
				hallFeedback->speedProportion =
					float ( updateRegU.st.datum ) / 256.0;
				*msgPtr		+= "Hall SpeedFeeback Proportion ";
				*msgPtr		+= String( hallFeedback->speedProportion ); };
			break;
		};
		data	+= length; };

	*msgPtr			+= "\n"; };
