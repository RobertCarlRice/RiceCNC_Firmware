
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

#ifndef PowerStepDefines_h
#define PowerStepDefines_h


// If we have an error, return the status code from the stepper controller.  Return 0 on no error
// getStatus() return may be non-zero even if there are no errors

/*****************************************************************
Example code for the STMicro PowerStep01 stepper motor driver.
This code is public domain beerware/Sunny-D-ware. If you find it useful and
run into me someday, I'd appreciate a cold one.

12/12/2011- Mike Hord, SparkFun Electronics

The breakout board for the PowerStep chip has 7 data lines:
BSYN	- this line is LOW when the chip is busy; busy generally means things
   like executing a move command.
STBY	- drag low to reset the device to default conditions. Also should be
   performed after power up to ensure a known-good initial state.
FLGN	- when the PowerStep raises a flag it usually means an error has occurred
STCK	- used as a step clock input; the direction (and activation of this input
   is done by setting registers on the chip.
SDI		- SPI data FROM the µC TO the PowerStep
SDO		- SPI data TO the µC FROM the PowerStep
CSN		- active-low slave select for the SPI bus
CK		- data clock for the SPI bus


ERRATA: IMPORTANT!!! READ THIS!!!
There are several errors in the datasheet for the L6470:
- the internal oscillator is specified as 16MHz +/- 3%. Experimentally, it
   seems to be more like a 6% tolerance.

- when transitioning from one movement command to another, it may be necessary
   to include a PowerStep_softStop() between the two to ensure proper operation. For
   example, if PowerStep_Move(FWD, 800) is used to move 800 steps FWD, and
   immediately after that, with no soft stop between them, a PowerStep_Run(FWD, 200)
   command is issued, the 'run' command will execute with a speed based on the
   value in the 'MAX_SPEED' register, the way the 'move' command did, and NOT
   with the speed passed to it by the function call.

2)  ABS_POS should be expanded to byte boundary, i.e., 24 bits
3)  Reading SPEED is "glichy" - values do not continuously ascend or decend while accelerating or decelerating
4)  dir bit in status does not change to relect the current motor direction
5)  FS_SPD reads as zero - cannot verify set value
6)  KVAL_RUN reads as 7 bits, i.e., mask 0x7F - cannot verify set value
7)  KVAL_ACC reads as 7 bits, i.e., mask 0x7F - cannot verify set value
8)  KVAL_DEC reads as 7 bits, i.e., mask 0x7F - cannot verify set value
9)  KVAL_HOLD reads as zero - cannot verify set value
10) STALL_TH reads back as 4 bits, i.e., mask 0x0F - cannot verify set value
11) CONFIG set 129 returns 1 - POW_SR not confirmed
12) EL_POS data appears to be non-sequential - need further documentation 
13) EL_POS shold be expanded to byte boundary, i.e., 16 bits

 
Initial release.
*****************************************************************/

// constant definitions for overcurrent thresholds. Write these values to 
//  register PowerStep_OCD_TH to set the level at which an overcurrent even occurs.
enum PowerStepOvercurrentThresholdValues {
  PowerStepOCD_31_25mV,		// 0x00
  PowerStepOCD_62_5mV,		// 0x01
  PowerStepOCD_93_75mV,		// 0x02
  PowerStepOCD_125mV,		// 0x03
  PowerStepOCD_156_25mV,	// 0x04
  PowerStepOCD_187_5mV,		// 0x05
  PowerStepOCD_218_75mV,	// 0x06
  PowerStepOCD_250_mV,		// 0x07
  PowerStepOCD_281_25mV,	// 0x08
  PowerStepOCD_312_5mV,		// 0x09
  PowerStepOCD_343_75mV,	// 0x0A
  PowerStepOCD_375mV,		// 0x0B
  PowerStepOCD_406_25mV,	// 0x0C
  PowerStepOCD_437_5mV,		// 0x0D
  PowerStepOCD_468_75mV,	// 0x0E
  PowerStepOCD_500mV,		// 0x0F
  PowerStepOCD_531_25mV,	// 0x10
  PowerStepOCD_562_5mV,		// 0x11
  PowerStepOCD_593_75mV,	// 0x12
  PowerStepOCD_625mV,		// 0x13
  PowerStepOCD_656_25mV,	// 0x14
  PowerStepOCD_687_5mV,		// 0x15
  PowerStepOCD_718_75mV,	// 0x16
  PowerStepOCD_750_mV,		// 0x17
  PowerStepOCD_781_25mV,	// 0x18
  PowerStepOCD_812_5mV,		// 0x19
  PowerStepOCD_843_75mV,	// 0x1A
  PowerStepOCD_875mV,		// 0x1B
  PowerStepOCD_906_25mV,	// 0x1C
  PowerStepOCD_937_5mV,		// 0x1D
  PowerStepOCD_968_75mV,	// 0x1E
  PowerStepOCD_1V };		// 0x1F};

enum PowerStepControlMode {
	PowerStepVoltageMode,		// 0
	PowerStepCurrentMode };		// 1

// STEP_MODE option values.
// First comes the "microsteps per step" options...
enum PowerStepStepModeSelect {
	PowerStepFullStep,				// 0
	PowerStepHalfStep,				// 1
	PowerStepQuarterStep,			// 2
	PowerStepEighthStep,			// 3
	PowerStepSixteenthStep,			// 4
	PowerStepThirtySecondStep,		// 5
	PowerStepSixtyFourthStep,		// 6
	PowerStepOneTwentyEighthStep };	// 7

// ...next, define the SYNC_EN bit. When set, the BUSYN pin will instead
//  output a clock related to the full-step frequency as defined by the
//  SYNC_SEL bits below.
enum PowerStepSyncClockEnable {
	PowerStepSyncClockDisable,		// 0
	PowerStepSyncClockEnable };		// 1

// ...last, define the SYNC_SEL modes. The clock output is defined by
//  the full-step frequency and the value in these bits- see the datasheet
//  for a matrix describing that relationship (page 46).
enum PowerStepSyncClockSelect {
	PowerStepSyncFullStep,				// 0
	PowerStepSyncHalfStep,				// 1
	PowerStepSyncQuarterStep,			// 2
	PowerStepSyncEighthStep,			// 3
	PowerStepSyncSixteenthStep,			// 4
	PowerStepSyncThirtySecondStep,		// 5
	PowerStepSyncSixtyFourthStep,		// 6
	PowerStepSyncOneTwentyEighthStep };	// 7

// Bit names for the ALARM_EN register.
//  Each of these bits defines one potential alarm condition.
//  When one of these conditions occurs and the respective bit in ALARM_EN is set,
//  the FLAG pin will go low. The register must be queried to determine which event
//  caused the alarm.
union PowerStepAlarmEnable {
	struct {
		uint overcurrent			: 1;
		uint thermalShutdown		: 1;
		uint thermalWarning			: 1;
		uint underVoltage			: 1;
		uint underVoltageLockout	: 1;
		uint adcUnderVoltageLockout	: 1;
		uint stallDetect			: 1; // vm only
		uint switchActive			: 1;
		uint commandError			: 1; } st;
	uint8_t alarmEnable; };

union PowerStepGateConfig1 {
	struct {
		uint tcc				: 5;
		uint iGate				: 3;
		uint tBoost				: 3;
		uint clockSourceMonitor	: 1;
		uint reserved			: 4; } st;
	uint16_t gateConfig1; };

enum PowerStepGateCurrent {
	PowerStepGate4mA,			// 0
	PowerStepGate4mA_1,			// 1
	PowerStepGate8mA,			// 2
	PowerStepGate16mA,			// 3
	PowerStepGate24mA,			// 4
	PowerStepGate32mA,			// 5
	PowerStepGate64mA,			// 6
	PowerStepGate96mA };		// 7

enum PowerStepConstantCurrentTime {
	PowerStepGateTcc125ns,		// 0
	PowerStepGateTcc250ns,		// 1
	PowerStepGateTcc375ns,		// 2
	PowerStepGateTcc500ns,		// 3
	PowerStepGateTcc625ns,		// 4
	PowerStepGateTcc750ns,		// 5
	PowerStepGateTcc875ns,		// 6
	PowerStepGateTcc1ms,		// 7
	PowerStepGateTcc1125ns,		// 8
	PowerStepGateTcc1250ns,		// 9
	PowerStepGateTcc1375ns,		// 0x0A
	PowerStepGateTcc1500ns,		// 0x0B
	PowerStepGateTcc1625ns,		// 0x0C
	PowerStepGateTcc1750ns,		// 0x0D
	PowerStepGateTcc1875ns,		// 0x0E
	PowerStepGateTcc2ms,		// 0x0F
	PowerStepGateTcc2125ns,		// 0x10
	PowerStepGateTcc2250ns,		// 0x11
	PowerStepGateTcc2375ns,		// 0x12
	PowerStepGateTcc2500ns,		// 0x13
	PowerStepGateTcc2625ns,		// 0x14
	PowerStepGateTcc2750ns,		// 0x15
	PowerStepGateTcc2875ns,		// 0x16
	PowerStepGateTcc3ms,		// 0x17
	PowerStepGateTcc3125ns,		// 0x18
	PowerStepGateTcc3250ns,		// 0x19
	PowerStepGateTcc3375ns,		// 0x1A
	PowerStepGateTcc3500ns,		// 0x1B
	PowerStepGateTcc3625ns,		// 0x1C
	PowerStepGateTcc3750ns,		// 0x1D
	PowerStepGateTcc3750ns_1,	// 0x1E
	PowerStepGateTcc3750ns_2 };	// 0x1F

enum PowerStepOverboostTime {
	PowerStepOverboost0ns,			// 0
	PowerStepOverboost62_83_125ns,	// 1
	PowerStepOverboost125ns,		// 2
	PowerStepOverboost250ns,		// 3
	PowerStepOverboost375ns,		// 4
	PowerStepOverboost500ns,		// 5
	PowerStepOverboost750ns,		// 6
	PowerStepOverboost1ms };		// 7

enum PowerStepClockSourceMonitor {
	PowerStepClockSourceMonitorDisable,		// 0
	PowerStepClockSourceMonitorEnable };	// 1

union PowerStepGateConfig2 {
	struct {
		uint tdt				: 5;
		uint tBlank				: 3; } st;
	uint8_t gateConfig2; };

enum PowerStepDeadTime {
	PowerStepGateTdt125ns,		// 0
	PowerStepGateTdt250ns,		// 1
	PowerStepGateTdt375ns,		// 2
	PowerStepGateTdt500ns,		// 3
	PowerStepGateTdt625ns,		// 4
	PowerStepGateTdt750ns,		// 5
	PowerStepGateTdt875ns,		// 6
	PowerStepGateTdt1ms,		// 7
	PowerStepGateTdt1125ns,		// 8
	PowerStepGateTdt1250ns,		// 9
	PowerStepGateTdt1375ns,		// 0x0A
	PowerStepGateTdt1500ns,		// 0x0B
	PowerStepGateTdt1625ns,		// 0x0C
	PowerStepGateTdt1750ns,		// 0x0D
	PowerStepGateTdt1875ns,		// 0x0E
	PowerStepGateTdt2ms,		// 0x0F
	PowerStepGateTdt2125ns,		// 0x10
	PowerStepGateTdt2250ns,		// 0x11
	PowerStepGateTdt2375ns,		// 0x12
	PowerStepGateTdt2500ns,		// 0x13
	PowerStepGateTdt2625ns,		// 0x14
	PowerStepGateTdt2750ns,		// 0x15
	PowerStepGateTdt2875ns,		// 0x16
	PowerStepGateTdt3ms,		// 0x17
	PowerStepGateTdt3125ns,		// 0x18
	PowerStepGateTdt3250ns,		// 0x19
	PowerStepGateTdt3375ns,		// 0x1A
	PowerStepGateTdt3500ns,		// 0x1B
	PowerStepGateTdt3625ns,		// 0x1C
	PowerStepGateTdt3750ns,		// 0x1D
	PowerStepGateTdt3875ns,		// 0x1E
	PowerStepGateTdt4ms };		// 0x1F

enum PowerStepBlankingTime {
	PowerStepGateTblank125ns,	// 0
	PowerStepGateTblank250ns,	// 1
	PowerStepGateTblank375ns,	// 2
	PowerStepGateTblank500ns,	// 3
	PowerStepGateTblank625ns,	// 4
	PowerStepGateTblank750ns,	// 5
	PowerStepGateTblank875ns,	// 6
	PowerStepGateTblank1000ns };	// 7

union PowerStepConfigReg {
	struct {
		uint oscSelect		: 4;
		uint swMode			: 1;
		uint enVsComp		: 1;
		uint reserved		: 1;
		uint ocShutdown		: 1;
		uint uvLoVal		: 1;
		uint vccVal			: 1; } st;
	struct {
		uint common			: 10;
		uint fPwmDec		: 3;
		uint fPwmInt		: 3; } vm;
	struct {
		uint common						: 10;
		uint targetSwitchingFrequency	: 5;
		uint predictiveCurrentControl	: 1; } cm;
	uint16_t	configReg; };

// Oscillator options.
// The PowerStep needs to know what the clock frequency is because it uses that for some
//  calculations during operation.
enum PowerStepOscSelect {
	PowerStepConfigOscInt16MHz,					//	0x00 Internal 16MHz, no output
	PowerStepConfigOscInt16MHz_1,				//	0x01 Internal 16MHz, no output
	PowerStepConfigOscInt16MHz_2,				//	0x02 Internal 16MHz, no output
	PowerStepConfigOscInt16MHz_3,				//	0x03 Internal 16MHz, no output
	PowerStepConfigOscExt8MHz,					//	0x04 External 8MHz crystal
	PowerStepConfigOscExt16MHz,					//	0x05 External 16MHz crystal
	PowerStepConfigOscExt24MHz,					//	0x06 External 24MHz crystal
	PowerStepConfigOscExt32MHz,					//	0x07 External 32MHz crystal
	PowerStepConfigOscInt16MHzOutput2MHz,		//	0x08 Default; internal 16MHz, 2MHz output
	PowerStepConfigOscInt16MHzOutput4MHz,		//	0x09 Internal 16MHz, 4MHz output
	PowerStepConfigOscInt16MHzOutput8MHz,		//	0x0A Internal 16MHz, 8MHz output
	PowerStepConfigOscInt16MHzOutput16MHz,		//	0x0B Internal 16MHz, 16MHz output
	PowerStepConfigOscExt8MHzOutputInverted,	//  0x0C External 8MHz crystal, output inverted
	PowerStepConfigOscExt16MHzOutputInverted,	//	0x0D External 16MHz crystal, output inverted
	PowerStepConfigOscExt24MHzOutputInverted,	//	0x0E External 24MHz crystal, output inverted
	PowerStepConfigOscExt32MHzOutputInverted };	//	0x0F External 32MHz crystal, output inverted

// Configure the functionality of the external switch input
enum PowerStepSwitchAction {
	PowerStepSwitchActionHardStop,				// 0 Default; hard stop motor on switch.
	PowerStepSwitchActionUser };				// 1 Tie to the GoUntil and ReleaseSW
		// commands to provide jog function. See page 25 of datasheet

// Configure the motor voltage compensation mode (see page 34 of datasheet)
enum PowerStepVoltageCompensation {
	PowerStepVoltageCompensationDisable,		//	0 Disable motor voltage compensation.
	PowerStepVoltageCompensationEnable };		//	1 Enable motor voltage compensation.

// Configure overcurrent detection event handling
enum PowerStepOvercurrentShutdown {
	PowerStepOvecurrentShutdownDisable,			// 0 Bridges do NOT shutdown on OC detect
	PowerStepOvecurrentShutdownEnable };		// 1 Bridges shutdown on OC detect

enum PowerStepVoltageRegulatorOutput {
	PowerStepVoltageRegulator7500mV,			// 0 7.5 V
	PowerStepVoltageRegulator15V };				// 1 15 V

enum PowerStepUnderVoltageLockoutValues {
	PowerStepUnderVoltageLockout6V,				// 0 VccthOn-6.9V, VccthOff-6.3V, DV_BOOTThOn-6V, DV_BOOTThOff_5.5V
	PowerStepUnderVoltageLockout10V };			// 1 VccthOn-10.4V, VccthOff-10V, DV_BOOTThOn-9.2V, DV_BOOTThOff_8.8V

enum PowerStepExtAdcPeakCurrentAdjust {
	PowerStepExtAdcPeakCurrentAdjustDisabled,	// 0
	PowerStepExtAdcPeakCurrentAdjustEnabled };	// 1

enum PowerStepPredictiveCurrentControl {
	PowerStepPredictiveCurrentControlDisabled,	// 0
	PowerStepPredictiveCurrentControlEnabled };	// 1

enum PowerStepCurrentModeTargetSwitchingFrequency {	// tsw
	PowerStepTargetSwitching250kHz,				// 0
	PowerStepTargetSwitching250kHz_1,			// 1
	PowerStepTargetSwitching125kHz,				// 2
	PowerStepTargetSwitching83kHz,				// 3
	PowerStepTargetSwitching75kHz,				// 4
	PowerStepTargetSwitching50kHz,				// 5
	PowerStepTargetSwitching42kHz,				// 6
	PowerStepTargetSwitching36kHz,				// 7
	PowerStepTargetSwitching31kHz,				// 8
	PowerStepTargetSwitching28kHz,				// 9
	PowerStepTargetSwitching25kHz,				// 10
	PowerStepTargetSwitching23kHz,				// 11
	PowerStepTargetSwitching21kHz,				// 12
	PowerStepTargetSwitching19kHz,				// 13
	PowerStepTargetSwitching18kHz,				// 14
	PowerStepTargetSwitching17kHz,				// 15
	PowerStepTargetSwitching16kHz,				// 16
	PowerStepTargetSwitching15kHz,				// 17
	PowerStepTargetSwitching14kHz,				// 18
	PowerStepTargetSwitching13kHz,				// 19
	PowerStepTargetSwitching12500Hz,			// 20
	PowerStepTargetSwitching11904Hz,			// 21
	PowerStepTargetSwitching11363Hz,			// 22
	PowerStepTargetSwitching10869Hz,			// 23
	PowerStepTargetSwitching10416Hz,			// 24
	PowerStepTargetSwitching10kHz,				// 25
	PowerStepTargetSwitching9615Hz,				// 26
	PowerStepTargetSwitching9259Hz,				// 27
	PowerStepTargetSwitching8928Hz,				// 28
	PowerStepTargetSwitching8620Hz,				// 29
	PowerStepTargetSwitching8333Hz,				// 30
	PowerStepTargetSwitching8064Hz };			// 31

// Integer divisors for PWM sinewave generation
//  See page 32 of the datasheet for more information on this.
enum PowerStepPwmGain {
	PowerStepConfigPWM_MUL_0_625,				// 0
	PowerStepConfigPWM_MUL_0_75,				// 1
	PowerStepConfigPWM_MUL_0_875,				// 2
	PowerStepConfigPWM_MUL_1,					// 3
	PowerStepConfigPWM_MUL_1_25,				// 4
	PowerStepConfigPWM_MUL_1_5,					// 5
	PowerStepConfigPWM_MUL_1_75,				// 6
	PowerStepConfigPWM_MUL_2 };					// 7

// Multiplier for the PWM sinewave frequency
enum PowerStepPwmDivider {
	PowerStepConfigPWM_DIV_1,					// 0
	PowerStepConfigPWM_DIV_2,					// 1
	PowerStepConfigPWM_DIV_3,					// 2
	PowerStepConfigPWM_DIV_4,					// 3
	PowerStepConfigPWM_DIV_5,					// 4
	PowerStepConfigPWM_DIV_6,					// 5
	PowerStepConfigPWM_DIV_7 };				// 6

union PowerStepStatusReg {
	struct {
		uint hiZ						: 1;
		uint busy						: 1;
		uint switchClosed				: 1;
		uint switchFallingEdgeEvent		: 1;
		uint forwardDirection			: 1;
		uint motorStatus				: 2;
		uint commandError				: 1;
		uint stepClockMode				: 1;
		uint underVoltageLockout		: 1;	// set on POR
		uint adcUnderVoltageLockout		: 1;
		uint thermalStatus				: 2;
		uint overcurrentDetect			: 1;
		uint stallB						: 1;
		uint stallA						: 1; } st;
	uint16_t	statusReg; };

enum PowerStepMotorStatus {
	PowerStepMotorStopped,				// 0
	PowerStepMotorAccelerating,			// 1
	PowerStepMotorDecelerating,			// 2
	PowerStepMotorConstantSpeed };		// 3

enum PowerStepThermalStatus {
	PowerStepThermalNormal,				// 0
	PowerStepThermalWarning,			// 1
	PowerStepThermalBridgeShutdown,		// 2
	PowerStepThermalDeviceShutdown };	// 3

enum PowerStepRegisters {
//  See the PowerStep_Param_Handler() function for more info about these.
	PowerStep_NO_OP,		//	0x00
  // ABS_POS is the current absolute offset from home. It is a 22 bit number expressed
  //  in two's complement. At power up, this value is 0. It cannot be written when
  //  the motor is running, but at any other time, it can be updated to change the
  //  interpreted position of the motor.
	PowerStep_ABS_POS,		//	0x01
  // EL_POS is the current electrical position in the step generation cycle. It can
  //  be set when the motor is not in motion. Value is 0 on power up.
	PowerStep_EL_POS,		//	0x02
  // MARK is a second position other than 0 that the motor can be told to go to. As
  //  with ABS_POS, it is 22-bit two's complement. Value is 0 on power up.
	PowerStep_MARK,			//	0x03
  // SPEED contains information about the current speed. It is read-only. It does 
  //  NOT provide direction information.
	PowerStep_SPEED,		//	0x04
  // ACC and DEC set the acceleration and deceleration rates. Set ACC to 0xFFF 
  //  to get infinite acceleration/deceleration - there is no way to get infinite
  //  deceleration w/o infinite acceleration (except the HARD STOP command).
  //  Cannot be written while motor is running. Both default to 0x08A on power up.
  // AccCalc() and DecCalc() functions exist to convert steps/s/s values into
  //  12-bit values for these two registers.
	PowerStep_ACC,			//	0x05
	PowerStep_DEC,			//	0x06
  // MAX_SPEED is just what it says- any command which attempts to set the speed
  //  of the motor above this value will simply cause the motor to turn at this
  //  speed. Value is 0x041 on power up.
  // MaxSpdCalc() function exists to convert steps/s value into a 10-bit value
  //  for this register.
	PowerStep_MAX_SPEED,	//	0x07
  // MIN_SPEED controls two things- the activation of the low-speed optimization
  //  feature and the lowest speed the motor will be allowed to operate at. LSPD_OPT
  //  is the 13th bit, and when it is set, the minimum allowed speed is automatically
  //  set to zero. This value is 0 on startup.
  // MinSpdCalc() function exists to convert steps/s value into a 12-bit value for this
  //  register. setLSPDOpt() function exists to enable/disable the optimization feature.
	PowerStep_MIN_SPEED,	//	0x08
  // KVAL is the maximum voltage of the PWM outputs. These 8-bit values are ratiometric
  //  representations: 255 for full output voltage, 128 for half, etc. Default is 0x29.
  // The implications of different KVAL settings is too complex to dig into here, but
  //  it will usually work to max the value for RUN, ACC, and DEC. Maxing the value for
  //  HOLD may result in excessive power dissipation when the motor is not running.
	PowerStep_KVAL_HOLD,	//	0x09
	PowerStep_KVAL_RUN,		//	0x0A
	PowerStep_KVAL_ACC,		//	0x0B
	PowerStep_KVAL_DEC,		//	0x0C
  // INT_SPD, ST_SLP, FN_SLP_ACC and FN_SLP_DEC are all related to the back EMF
  //  compensation functionality. Please see the data sheet for details of this
  //  function- it is too complex to discuss here. Default values seem to work
  //  well enough.
	PowerStep_INT_SPD,		//	0x0D
	PowerStep_ST_SLP,		//	0x0E
	PowerStep_FN_SLP_ACC,	//	0x0F
	PowerStep_FN_SLP_DEC,	//	0x10
  // K_THERM is motor winding thermal drift compensation. Please see the data sheet
  //  for full details on operation- the default value should be OK for most users.
	PowerStep_K_THERM,		//	0x11
  // ADC_OUT is a read-only register containing the result of the ADC measurements.
  //  This is less useful than it sounds; see the datasheet for more information.
	PowerStep_ADC_OUT,		//	0x12
  // Set the over current threshold. Ranges from 375mA to 6A in steps of 375mA.
  //  A set of defined constants is provided for the user's convenience. Default
  //  value is 3.375A- 0x08. This is a 4-bit value.
	PowerStep_OCD_TH,		//	0x13
  // Stall current threshold. Defaults to 0x40, or 2.03A. Value is from 31.25mA to
  //  4A in 31.25mA steps. This is a 7-bit value.
	PowerStep_STALL_TH,		//	0x14
  // FS_SPD register contains a threshold value above which micro-stepping is disabled
  //  and the PowerStep operates in full-step mode. Defaults to 0x027 on power up.
  // FSCalc() function exists to convert steps/s value into 10-bit integer for this
  //  register.
	PowerStep_FS_SPD,		//	0x15
  // STEP_MODE controls the micro stepping settings, as well as the generation of an
  //  output signal from the PowerStep. Bits 2:0 control the number of micro steps per
  //  step the part will generate. Bit 7 controls whether the BUSY/SYNC pin outputs
  //  a BUSY signal or a step synchronization signal. Bits 6:4 control the frequency
  //  of the output signal relative to the full-step frequency; see datasheet for
  //  that relationship as it is too complex to reproduce here.
  // Most likely, only the microsteps per step value will be needed; there is a set
  //  of constants provided for ease of use of these values.
	PowerStep_STEP_MODE,	//	0x16
  // ALARM_EN controls which alarms will cause the FLAG pin to fall. A set of constants
  //  is provided to make this easy to interpret. By default, ALL alarms will trigger the
  //  FLAG pin.
	PowerStep_ALARM_EN,		//	0x17
  // CONFIG contains some assorted configuration bits and fields. A fairly comprehensive
  //  set of reasonably self-explanatory constants is provided, but users should refer
  //  to the data sheet before modifying the contents of this register to be certain they
  //  understand the implications of their modifications. Value on boot is 0x2E88; this
  //  can be a useful way to verify proper start up and operation of the PowerStep chip.
	PowerStepGateConfig1Param,	//	0x18
	PowerStepGateConfig2Param,	//	0x19
	PowerStep_CONFIG,		//	0x1A
  // STATUS contains read-only information about the current condition of the chip. A
  //  comprehensive set of constants for masking and testing this register is provided, but
  //  users should refer to the data sheet to ensure that they fully understand each one of
  //  the bits in the register.
	PowerStep_STATUS };		//	0x1B

#define Bits4      0x00000F
#define Bits5      0x00001F
#define Bits7      0x00007F
#define Bits8      0x0000FF
#define Bits9      0x0001FF
#define Bits10     0x0003FF
#define Bits11     0x0007FF
#define Bits12     0x000FFF
#define Bits13     0x001FFF
#define Bits14     0x003FFF
#define Bits16     0x00FFFF
#define Bits20     0x0FFFFF
#define Bits22     0x3FFFFF

static const uint32_t registerMask[] = {
  0,
  Bits22,	// ABS_POS
  Bits9,	// EL_POS
  Bits22,	// MARK
  Bits20,	// SPEED
  Bits12,	// ACC
  Bits12,	// DEC
  Bits10,	// MAX_SPEED
  Bits13,	// MIN_SPEED
  Bits11,	// FS_SPD
  Bits8,	// KVAL_HOLD
  Bits8,	// KVAL_RUN
  Bits8,	// KVAL_ACC
  Bits8,	// KVAL_DEC
  Bits14,	// INT_SPEED
  Bits8,	// ST_SLP
  Bits8,	// FN_SLP_ACC
  Bits8,	// FN_SLP_DEC
  Bits4,	// K_THERM
  Bits5,	// ADC_OUT
  Bits4,	// OCD_TH
  Bits7,	// STALL_TH
  Bits8,	// STEP_MODE
  Bits8,	// ALARM_EN
  Bits11,	// GateConfig1
  Bits8,	// GateConfig2
  Bits16,	// CONFIG
  Bits16 }; // STATUS
  
//PowerStep commands
#define PowerStep_NOP             0x00
#define PowerStep_SET_PARAM       0x00
#define PowerStep_GET_PARAM       0x20
#define PowerStep_RUN             0x50
#define PowerStep_STEP_CLOCK      0x58
#define PowerStep_MOVE            0x40
#define PowerStep_GOTO            0x60
#define PowerStep_GOTO_DIR        0x68
#define PowerStep_GO_UNTIL        0x82
#define PowerStep_RELEASE_SW      0x92
#define PowerStep_GO_HOME         0x70
#define PowerStep_GO_MARK         0x78
#define PowerStep_RESET_POS       0xD8
#define PowerStep_RESET_DEVICE    0xC0
#define PowerStep_SOFT_STOP       0xB0
#define PowerStep_HARD_STOP       0xB8
#define PowerStep_SOFT_HIZ        0xA0
#define PowerStep_HARD_HIZ        0xA8
#define PowerStep_GET_STATUS      0xD0

/* PowerStep direction options */
enum PowerStepDirections {
	PowerStepReverse,	//	0x00
	PowerStepForward };	//	0x01

/* PowerStep action options */
enum PowerStepActions {
	PowerStepActionReset,	// 0x00
	PowerStepActionCopy };	// 0x01

// The value in the MIN_SPD register is [(steps/s)*(tick)]/(2^-24) where tick is 
//  250ns (datasheet value)- 0x000 on boot.
#define MinSpeedConversionFactor      4.1943

// The value in the MAX_SPD register is [(steps/s)*(tick)]/(2^-18) where tick is 
//  250ns (datasheet value)- 0x041 on boot.
#define MaxSpeedSetConversionFactor   0.065536

//  The 20-bit speed is (steps/s)*(tick)/(2^-28) where tick is 
//  250ns (datasheet value). resolution 0.0149
#define SpeedConversionFactor         0.01490116
#define SpeedSetConversionFactor      67.108864

// The value in the INT_SPD register is [(steps/s)*(tick)]/(2^-24) where tick is 
//  250ns (datasheet value)- 0x408 on boot.

// The value in the ACC register is [(steps/s/s)*(tick^2)]/(2^-40) where tick is 
//  250ns (datasheet value)- 0x08A on boot.
#define AccelerationConversionFactor  0.137438

#endif
