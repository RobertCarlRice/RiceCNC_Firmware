
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

#ifndef dSpinDefines_h
#define dSpinDefines_h


// If we have an error, return the status code from the stepper controller.  Return 0 on no error
// getStatus() return may be non-zero even if there are no errors

/*****************************************************************
Example code for the STMicro L6470 dSPIN stepper motor driver.
This code is public domain beerware/Sunny-D-ware. If you find it useful and
run into me someday, I'd appreciate a cold one.

12/12/2011- Mike Hord, SparkFun Electronics

The breakout board for the dSPIN chip has 7 data lines:
BSYN	- this line is LOW when the chip is busy; busy generally means things
   like executing a move command.
STBY	- drag low to reset the device to default conditions. Also should be
   performed after power up to ensure a known-good initial state.
FLGN	- when the dSPIN raises a flag it usually means an error has occurred
STCK	- used as a step clock input; the direction (and activation of this input
   is done by setting registers on the chip.
SDI		- SPI data FROM the µC TO the dSPIN
SDO		- SPI data TO the µC FROM the dSPIN
CSN		- active-low slave select for the SPI bus
CK		- data clock for the SPI bus


ERRATA: IMPORTANT!!! READ THIS!!!
There are several errors in the datasheet for the L6470:
- the internal oscillator is specified as 16MHz +/- 3%. Experimentally, it
   seems to be more like a 6% tolerance.

- when transitioning from one movement command to another, it may be necessary
   to include a dSPIN_softStop() between the two to ensure proper operation. For
   example, if dSPIN_Move(FWD, 800) is used to move 800 steps FWD, and
   immediately after that, with no soft stop between them, a dSPIN_Run(FWD, 200)
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
//  register dSPIN_OCD_TH to set the level at which an overcurrent even occurs.
enum dSPIN_OCD_TH_VALUES {
  dSPIN_OCD_TH_375mA,
  dSPIN_OCD_TH_750mA,
  dSPIN_OCD_TH_1125mA,
  dSPIN_OCD_TH_1500mA,
  dSPIN_OCD_TH_1875mA,
  dSPIN_OCD_TH_2250mA,
  dSPIN_OCD_TH_2625mA,
  dSPIN_OCD_TH_3000mA,
  dSPIN_OCD_TH_3375mA,
  dSPIN_OCD_TH_3750mA,
  dSPIN_OCD_TH_4125mA,
  dSPIN_OCD_TH_4500mA,
  dSPIN_OCD_TH_4875mA,
  dSPIN_OCD_TH_5250mA,
  dSPIN_OCD_TH_5625mA,
  dSPIN_OCD_TH_6000mA };
  
// STEP_MODE option values.
// First comes the "microsteps per step" options...
#define dSPIN_STEP_MODE_STEP_SEL        0x07  // Mask for these bits only.

enum dSPIN_STEP_SEL {
  dSPIN_STEP_SEL_1,
  dSPIN_STEP_SEL_1_2,
  dSPIN_STEP_SEL_1_4,
  dSPIN_STEP_SEL_1_8,
  dSPIN_STEP_SEL_1_16,
  dSPIN_STEP_SEL_1_32,
  dSPIN_STEP_SEL_1_64,
  dSPIN_STEP_SEL_1_128 };

// ...next, define the SYNC_EN bit. When set, the BUSYN pin will instead
//  output a clock related to the full-step frequency as defined by the
//  SYNC_SEL bits below.
#define dSPIN_STEP_MODE_SYNC_EN		0x80  // Mask for this bit
#define dSPIN_SYNC_EN				0x80

// ...last, define the SYNC_SEL modes. The clock output is defined by
//  the full-step frequency and the value in these bits- see the datasheet
//  for a matrix describing that relationship (page 46).
#define dSPIN_STEP_MODE_SYNC_SEL	0x70
#define dSPIN_SYNC_SEL_1_2			0x00
#define dSPIN_SYNC_SEL_1			0x10
#define dSPIN_SYNC_SEL_2			0x20
#define dSPIN_SYNC_SEL_4			0x30
#define dSPIN_SYNC_SEL_8			0x40
#define dSPIN_SYNC_SEL_16			0x50
#define dSPIN_SYNC_SEL_32			0x60
#define dSPIN_SYNC_SEL_64			0x70

// Bit names for the ALARM_EN register.
//  Each of these bits defines one potential alarm condition.
//  When one of these conditions occurs and the respective bit in ALARM_EN is set,
//  the FLAG pin will go low. The register must be queried to determine which event
//  caused the alarm.
#define dSPIN_ALARM_EN_OVERCURRENT       0x01
#define dSPIN_ALARM_EN_THERMAL_SHUTDOWN	 0x02
#define dSPIN_ALARM_EN_THERMAL_WARNING   0x04
#define dSPIN_ALARM_EN_UNDER_VOLTAGE     0x08
#define dSPIN_ALARM_EN_STALL_DET_A       0x10
#define dSPIN_ALARM_EN_STALL_DET_B       0x20
#define dSPIN_ALARM_EN_SW_TURN_ON        0x40
#define dSPIN_ALARM_EN_WRONG_NPERF_CMD   0x80

// CONFIG register renames.

// Oscillator options.
// The dSPIN needs to know what the clock frequency is because it uses that for some
//  calculations during operation.
#define dSPIN_CONFIG_OSC_SEL                 0x000F // Mask for this bit field.
#define dSPIN_CONFIG_INT_16MHZ               0x00	// Internal 16MHz, no output
#define dSPIN_CONFIG_INT_16MHZ_OSCOUT_2MHZ   0x08	// Default; internal 16MHz, 2MHz output
#define dSPIN_CONFIG_INT_16MHZ_OSCOUT_4MHZ   0x09	// Internal 16MHz, 4MHz output
#define dSPIN_CONFIG_INT_16MHZ_OSCOUT_8MHZ   0x0A	// Internal 16MHz, 8MHz output
#define dSPIN_CONFIG_INT_16MHZ_OSCOUT_16MHZ  0x0B	// Internal 16MHz, 16MHz output
#define dSPIN_CONFIG_EXT_8MHZ_XTAL_DRIVE     0x04	// External 8MHz crystal
#define dSPIN_CONFIG_EXT_16MHZ_XTAL_DRIVE    0x05	// External 16MHz crystal
#define dSPIN_CONFIG_EXT_24MHZ_XTAL_DRIVE    0x06	// External 24MHz crystal
#define dSPIN_CONFIG_EXT_32MHZ_XTAL_DRIVE    0x07	// External 32MHz crystal
#define dSPIN_CONFIG_EXT_8MHZ_OSCOUT_INVERT  0x0C	// External 8MHz crystal, output inverted
#define dSPIN_CONFIG_EXT_16MHZ_OSCOUT_INVERT 0x0D	// External 16MHz crystal, output inverted
#define dSPIN_CONFIG_EXT_24MHZ_OSCOUT_INVERT 0x0E	// External 24MHz crystal, output inverted
#define dSPIN_CONFIG_EXT_32MHZ_OSCOUT_INVERT 0x0F	// External 32MHz crystal, output inverted

// Configure the functionality of the external switch input
#define dSPIN_CONFIG_SW_MODE                 0x0010 // Mask for this bit.
#define dSPIN_CONFIG_SW_HARD_STOP            0x00	// Default; hard stop motor on switch.
#define dSPIN_CONFIG_SW_USER                 0x10	// Tie to the GoUntil and ReleaseSW
                                                    //  commands to provide jog function.
                                                    //  See page 25 of datasheet

// Configure the motor voltage compensation mode (see page 34 of datasheet)
#define dSPIN_CONFIG_EN_VSCOMP               0x0020 // Mask for this bit.
#define dSPIN_CONFIG_VS_COMP_DISABLE         0x00	// Disable motor voltage compensation.
#define dSPIN_CONFIG_VS_COMP_ENABLE          0x20	// Enable motor voltage compensation.

// Configure overcurrent detection event handling
#define dSPIN_CONFIG_OC_SD                   0x0080 // Mask for this bit.
#define dSPIN_CONFIG_OC_SD_DISABLE           0x00	// Bridges do NOT shutdown on OC detect
#define dSPIN_CONFIG_OC_SD_ENABLE            0x80	// Bridges shutdown on OC detect

// Configure the slew rate of the power bridge output
#define dSPIN_CONFIG_POW_SR                  0x0300  // Mask for this bit field.
#define dSPIN_CONFIG_SR_320V_us              0x0000  // 320V/us
#define dSPIN_CONFIG_SR_75V_us               0x0100  // 75V/us
#define dSPIN_CONFIG_SR_110V_us              0x0200  // 110V/us
#define dSPIN_CONFIG_SR_260V_us              0x0300  // 260V/us

// Integer divisors for PWM sinewave generation
//  See page 32 of the datasheet for more information on this.
#define dSPIN_CONFIG_F_PWM_DEC               7 << 10      // mask for this bit field
#define dSPIN_CONFIG_PWM_MUL_0_625           0 << 10
#define dSPIN_CONFIG_PWM_MUL_0_75            1 << 10
#define dSPIN_CONFIG_PWM_MUL_0_875           2 << 10
#define dSPIN_CONFIG_PWM_MUL_1               3 << 10
#define dSPIN_CONFIG_PWM_MUL_1_25            4 << 10
#define dSPIN_CONFIG_PWM_MUL_1_5             5 << 10
#define dSPIN_CONFIG_PWM_MUL_1_75            6 << 10
#define dSPIN_CONFIG_PWM_MUL_2               7 << 10

// Multiplier for the PWM sinewave frequency
#define dSPIN_CONFIG_F_PWM_INT               7 << 13     // mask for this bit field.
#define dSPIN_CONFIG_PWM_DIV_1               0 << 13
#define dSPIN_CONFIG_PWM_DIV_2               1 << 13
#define dSPIN_CONFIG_PWM_DIV_3               2 << 13
#define dSPIN_CONFIG_PWM_DIV_4               3 << 13
#define dSPIN_CONFIG_PWM_DIV_5               4 << 13
#define dSPIN_CONFIG_PWM_DIV_6               5 << 13
#define dSPIN_CONFIG_PWM_DIV_7               6 << 13

// Register address redefines.
//  See the dSPIN_Param_Handler() function for more info about these.
#define  dSPIN_NO_OP       0x00
  // ABS_POS is the current absolute offset from home. It is a 22 bit number expressed
  //  in two's complement. At power up, this value is 0. It cannot be written when
  //  the motor is running, but at any other time, it can be updated to change the
  //  interpreted position of the motor.
#define  dSPIN_ABS_POS     0x01
  // EL_POS is the current electrical position in the step generation cycle. It can
  //  be set when the motor is not in motion. Value is 0 on power up.
#define  dSPIN_EL_POS      0x02
  // MARK is a second position other than 0 that the motor can be told to go to. As
  //  with ABS_POS, it is 22-bit two's complement. Value is 0 on power up.
#define  dSPIN_MARK        0x03
  // SPEED contains information about the current speed. It is read-only. It does 
  //  NOT provide direction information.
#define  dSPIN_SPEED       0x04
  // ACC and DEC set the acceleration and deceleration rates. Set ACC to 0xFFF 
  //  to get infinite acceleration/deceleration - there is no way to get infinite
  //  deceleration w/o infinite acceleration (except the HARD STOP command).
  //  Cannot be written while motor is running. Both default to 0x08A on power up.
  // AccCalc() and DecCalc() functions exist to convert steps/s/s values into
  //  12-bit values for these two registers.
#define  dSPIN_ACC         0x05
#define  dSPIN_DEC         0x06
  // MAX_SPEED is just what it says- any command which attempts to set the speed
  //  of the motor above this value will simply cause the motor to turn at this
  //  speed. Value is 0x041 on power up.
  // MaxSpdCalc() function exists to convert steps/s value into a 10-bit value
  //  for this register.
#define  dSPIN_MAX_SPEED   0x07
  // MIN_SPEED controls two things- the activation of the low-speed optimization
  //  feature and the lowest speed the motor will be allowed to operate at. LSPD_OPT
  //  is the 13th bit, and when it is set, the minimum allowed speed is automatically
  //  set to zero. This value is 0 on startup.
  // MinSpdCalc() function exists to convert steps/s value into a 12-bit value for this
  //  register. setLSPDOpt() function exists to enable/disable the optimization feature.
#define  dSPIN_MIN_SPEED   0x08
  // KVAL is the maximum voltage of the PWM outputs. These 8-bit values are ratiometric
  //  representations: 255 for full output voltage, 128 for half, etc. Default is 0x29.
  // The implications of different KVAL settings is too complex to dig into here, but
  //  it will usually work to max the value for RUN, ACC, and DEC. Maxing the value for
  //  HOLD may result in excessive power dissipation when the motor is not running.
#define  dSPIN_KVAL_HOLD   0x09
#define  dSPIN_KVAL_RUN    0x0A
#define  dSPIN_KVAL_ACC    0x0B
#define  dSPIN_KVAL_DEC    0x0C
  // INT_SPD, ST_SLP, FN_SLP_ACC and FN_SLP_DEC are all related to the back EMF
  //  compensation functionality. Please see the data sheet for details of this
  //  function- it is too complex to discuss here. Default values seem to work
  //  well enough.
#define  dSPIN_INT_SPD     0x0D
#define  dSPIN_ST_SLP      0x0E
#define  dSPIN_FN_SLP_ACC  0x0F
#define  dSPIN_FN_SLP_DEC  0x10
  // K_THERM is motor winding thermal drift compensation. Please see the data sheet
  //  for full details on operation- the default value should be OK for most users.
#define  dSPIN_K_THERM     0x11
  // ADC_OUT is a read-only register containing the result of the ADC measurements.
  //  This is less useful than it sounds; see the datasheet for more information.
#define  dSPIN_ADC_OUT     0x12
  // Set the over current threshold. Ranges from 375mA to 6A in steps of 375mA.
  //  A set of defined constants is provided for the user's convenience. Default
  //  value is 3.375A- 0x08. This is a 4-bit value.
#define  dSPIN_OCD_TH      0x13
  // Stall current threshold. Defaults to 0x40, or 2.03A. Value is from 31.25mA to
  //  4A in 31.25mA steps. This is a 7-bit value.
#define  dSPIN_STALL_TH    0x14
  // FS_SPD register contains a threshold value above which micro-stepping is disabled
  //  and the dSPIN operates in full-step mode. Defaults to 0x027 on power up.
  // FSCalc() function exists to convert steps/s value into 10-bit integer for this
  //  register.
#define  dSPIN_FS_SPD      0x15
  // STEP_MODE controls the micro stepping settings, as well as the generation of an
  //  output signal from the dSPIN. Bits 2:0 control the number of micro steps per
  //  step the part will generate. Bit 7 controls whether the BUSY/SYNC pin outputs
  //  a BUSY signal or a step synchronization signal. Bits 6:4 control the frequency
  //  of the output signal relative to the full-step frequency; see datasheet for
  //  that relationship as it is too complex to reproduce here.
  // Most likely, only the microsteps per step value will be needed; there is a set
  //  of constants provided for ease of use of these values.
#define  dSPIN_STEP_MODE   0x16
  // ALARM_EN controls which alarms will cause the FLAG pin to fall. A set of constants
  //  is provided to make this easy to interpret. By default, ALL alarms will trigger the
  //  FLAG pin.
#define  dSPIN_ALARM_EN    0x17
  // CONFIG contains some assorted configuration bits and fields. A fairly comprehensive
  //  set of reasonably self-explanatory constants is provided, but users should refer
  //  to the data sheet before modifying the contents of this register to be certain they
  //  understand the implications of their modifications. Value on boot is 0x2E88; this
  //  can be a useful way to verify proper start up and operation of the dSPIN chip.
#define  dSPIN_CONFIG      0x18
  // STATUS contains read-only information about the current condition of the chip. A
  //  comprehensive set of constants for masking and testing this register is provided, but
  //  users should refer to the data sheet to ensure that they fully understand each one of
  //  the bits in the register.
#define  dSPIN_STATUS      0x19

#define Bits4      0x00000F
#define Bits5      0x00001F
#define Bits7      0x00007F
#define Bits8      0x0000FF
#define Bits9      0x0001FF
#define Bits10     0x0003FF
#define Bits12     0x000FFF
#define Bits13     0x001FFF
#define Bits14     0x003FFF
#define Bits16     0x00FFFF
#define Bits20     0x0FFFFF
#define Bits22     0x3FFFFF

static const uint32_t registerMask[] = {
  0,
  Bits22, // ABS_POS
  Bits9,  // EL_POS
  Bits22, // MARK
  Bits20, // SPEED
  Bits12, // ACC
  Bits12, // DEC
  Bits10, // MAX_SPEED
  Bits13, // MIN_SPEED
  Bits10, // FS_SPD
  Bits8,  // KVAL_HOLD
  Bits8,  // KVAL_RUN
  Bits8,  // KVAL_ACC
  Bits8,  // KVAL_DEC
  Bits14, // INT_SPEED
  Bits8,  // ST_SLP
  Bits8,  // FN_SLP_ACC
  Bits8,  // FN_SLP_DEC
  Bits4,  // K_THERM
  Bits5,  // ADC_OUT
  Bits4,  // OCD_TH
  Bits7,  // STALL_TH
  Bits8,  // STEP_MODE
  Bits8,  // ALARM_EN
  Bits16, // CONFIG
  Bits16 }; // STATUS
  
//dSPIN commands
#define dSPIN_NOP             0x00
#define dSPIN_SET_PARAM       0x00
#define dSPIN_GET_PARAM       0x20
#define dSPIN_RUN             0x50
#define dSPIN_STEP_CLOCK      0x58
#define dSPIN_MOVE            0x40
#define dSPIN_GOTO            0x60
#define dSPIN_GOTO_DIR        0x68
#define dSPIN_GO_UNTIL        0x82
#define dSPIN_RELEASE_SW      0x92
#define dSPIN_GO_HOME         0x70
#define dSPIN_GO_MARK         0x78
#define dSPIN_RESET_POS       0xD8
#define dSPIN_RESET_DEVICE    0xC0
#define dSPIN_SOFT_STOP       0xB0
#define dSPIN_HARD_STOP       0xB8
#define dSPIN_SOFT_HIZ        0xA0
#define dSPIN_HARD_HIZ        0xA8
#define dSPIN_GET_STATUS      0xD0

/* dSPIN direction options */
#define FWD	              0x01
#define REV		      0x00

/* dSPIN action options */
#define ACTION_RESET	      0x00
#define ACTION_COPY	      0x01

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
