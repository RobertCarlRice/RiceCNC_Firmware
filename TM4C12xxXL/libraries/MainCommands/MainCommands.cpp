
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

#include <MainCommands.h>
#include <Timers.h>
#include <Machine.h>
#include <InterpolationTimer.h>
#include <MySerial.h>
#include <SysTick.h>
#include <SoftPwmTimer.h>
#include <AxisTimer.h>

#include <SSI.h>
#include <SSI8305.h>
#include <SSI8711.h>
#include <SSIdSpin.h>

#include "inc/hw_adc.h"
#include <driverlib/sysctl.h>


#if defined( PART_TM4C1294NCPDT )         // Connected LaunchPad
	#include <MyEEProm.h>
#endif

// Reinitialized by setup()
uint32_t	MainCommands::interpolationInterruptRate	= 1500;	// Hz

uint32_t	MainCommands::hostTimeout					= 0;
uint32_t	MainCommands::hardwareOvesampling			= 6;
uint32_t	MainCommands::periodicReportingInterval		= 1000000;

float		MainCommands::stopFactor		= 90;
float		MainCommands::targetLead		= 0;

void
	MainCommands::signOnMsg( String* msg ) {

	uint32_t resetCause	= SysCtlResetCauseGet();
	
	char data[ 30 ];
	snprintf( data, 30,
		"\n\n%c%c Version %u",
		INTERPOLATOR_NAME,
		RESET_COMMAND,
		firmware_version );
	*msg += String( data );

	if ( resetCause & SYSCTL_CAUSE_HSRVREQ )
		*msg	+= " (Hw Sys Service Request)";

	if ( resetCause & SYSCTL_CAUSE_HIB )
		*msg	+= " (Hibernate)";

	if ( resetCause & SYSCTL_CAUSE_LDO )
		*msg	+= " (Low Voltage)";

	if ( resetCause & SYSCTL_CAUSE_WDOG1 )
		*msg	+= " (Watchdog 1)";

	if ( resetCause & SYSCTL_CAUSE_SW ) {
		*msg	+= " (Software)";

		#if defined( PART_TM4C1294NCPDT )  // Connected LaunchPad
		switch ( MyEEProm::softResetReason() ) {
			case MyEEProm::ResetRequestFromHost :
			*msg	+= " Reset Request From Host";
			break;
				
			case MyEEProm::HostMessageTimeout :
			*msg	+= " Host Message Timeout";
			break;
				
			case MyEEProm::MaxPacketRetransmission :
			*msg	+= " Max Packet Retransmissions"; };
		#endif
		}

	if ( resetCause & SYSCTL_CAUSE_WDOG0 )
		*msg	+= " (Watchdog 0)";

	if ( resetCause & SYSCTL_CAUSE_WDOG )
		*msg	+= " (Watchdog)";

	if ( resetCause & SYSCTL_CAUSE_BOR )
		*msg	+= " (Brown Out)";

	if ( resetCause & SYSCTL_CAUSE_POR )
		*msg	+= " (Power On)";

	if ( resetCause & SYSCTL_CAUSE_EXT )
		*msg	+= " (External)"; };


void
	MainCommands::processCommand( char* commandPtr, String* msgPtr ) {

	InterpolationTimer::watchDogTimer	= 0; // Reset the watchdog

	if ( *commandPtr == INTERPOLATOR_NAME ) {
		MainCommands::processInterpolatorCommand(
			commandPtr + 1,
			msgPtr );
		return; };

	if ( Machine::firstMachine )
		Machine::firstMachine->processCommand(
			commandPtr,
			msgPtr );

	else
		*msgPtr  += "\nE Machine not Configured"; };


void
	MainCommands::processInterpolatorCommand(
		char*		iCommandPtr,
		String*		msgPtr ) {

	char  command	= *iCommandPtr;
	char* data		= iCommandPtr + 1;

//  char header[4];
//  snprintf( header, 4, "%c%c", INTERPOLATOR_NAME, command );
	*msgPtr  +=
		String( INTERPOLATOR_NAME ) +
		String( command );

	switch ( command ) {
		case RESET_COMMAND:
		// Only get here with serial connection
		// ethernet uses packet version number 0 to signal reset
#if defined( PART_TM4C1294NCPDT )  // Connected LaunchPad
		MainCommands::softwareReset(
			MyEEProm::ResetRequestFromHost );
#else
		MainCommands::softwareReset();
#endif
		break;

		case CONFIGURE_INTERPOLATOR:
		MainCommands::configureInterpolator(
			data,
			msgPtr );
		HWREG( ADC0_BASE + ADC_O_SAC )	= MainCommands::hardwareOvesampling;
		HWREG( ADC1_BASE + ADC_O_SAC )	= MainCommands::hardwareOvesampling;
		break;

		case CONFIGURE_SSI:
		MainCommands::configureSSI(			data, msgPtr );
		break;

		case CREATE_MACHINE:
		MainCommands::createMachine(		data, msgPtr );
		break;

		case WATCHDOG:
		break;

#ifdef PART_TM4C1294NCPDT
		case SET_ETHERNET_ADDRESSES:
		MyEEProm::setEthernetAddresses(		data, msgPtr );
		break;
#endif

		default:
		*msgPtr += "E Invalid Interpolator Command - Ignored"; }; };

#if defined( PART_TM4C1294NCPDT )  // Connected LaunchPad
	void
		MainCommands::softwareReset( MyEEProm::SoftResetReason softResetReason ) {

		MyEEProm::recordResetReason( softResetReason );

		SysCtlResetCauseClear( ~0 ); // Clear prior reset cause
		SysCtlReset(); };

#else
	void
		MainCommands::softwareReset() {

		SysCtlResetCauseClear( ~0 ); // Clear prior reset cause
		SysCtlReset(); };
#endif

void
	MainCommands::configureInterpolator(
		char*		data,
		String*		msgPtr ) {

	uint32_t softPwmInterruptRate			= 0;	// kHz
	uint32_t motorStepInterruptRate			= 15;	// kHz
	uint32_t interpolationInterruptRate		= 1500;	// Hz
	uint32_t hostTimeout					= 0;
	uint32_t targetLeadPct					= 75;
	uint32_t stopFactorPct					= 100;
	uint32_t periodicReportingInterval		= 0;

	sscanf( data,
		"%lu %lu %lu %lu %lu %lu %lu %lu",
		&softPwmInterruptRate,
		&motorStepInterruptRate,
		&interpolationInterruptRate,
		&hostTimeout,
		&targetLeadPct,
		&stopFactorPct,
		&MainCommands::hardwareOvesampling,
		&periodicReportingInterval );
 
	if (	interpolationInterruptRate < 1000
		||	interpolationInterruptRate > 2000 )

		interpolationInterruptRate	= 1500;
    
	MainCommands::targetLead					= float( targetLeadPct ) / 100.0;
	MainCommands::stopFactor					= float( stopFactorPct ) / 100.0;

	MainCommands::interpolationInterruptRate	= interpolationInterruptRate;
	MainCommands::hostTimeout					= hostTimeout * 1000; // milliseconds

	MainCommands::periodicReportingInterval		=
		2000000
		/ periodicReportingInterval; // µS

	if ( softPwmInterruptRate < 100 )
		softPwmInterruptRate = 0;

	snprintf( data, 100,
		"\n -Tuning-\n SysClk %lu",
		ClockFrequency ); // SysCtlClockGet() / 120000000
	*msgPtr += String( data );

	if ( softPwmInterruptRate ) {
		snprintf( data, 100,
			"\n Soft PFM interrupt rate %lu kHz",
			softPwmInterruptRate );
		*msgPtr += String( data ); }

	else
		*msgPtr += "\n Soft PFM Off";

	snprintf( data, 100,
		"\n Motor step interrupt rate %lu kHz",
		motorStepInterruptRate );
	*msgPtr += String( data );

	snprintf( data, 100,
		"\n Interpolation interrupt rate %lu Hz",
		interpolationInterruptRate );
	*msgPtr += String( data );

	snprintf( data, 100,
		"\n WD Timeout %lu sec",
		hostTimeout );
	*msgPtr += String( data );

	snprintf( data, 100,
		"\n Reports %.1f/Sec",
		float( periodicReportingInterval ) * 0.5 );
	*msgPtr += String( data );

	// Pulse
	snprintf( data, 100,
		"\n -Pulse-\n Lead %lu %%",
		targetLeadPct );
	*msgPtr += String( data );

	snprintf( data, 100,
		"\n Stop factor %lu %%",
		stopFactorPct );
	*msgPtr += String( data );

	snprintf( data, 100,
		"\n -ADC Oversample- %lu",
		1 << MainCommands::hardwareOvesampling );
	*msgPtr += String( data );

	*msgPtr += "\n System Configured\n";

	// Soft PFM and Axis share the same timer 0
	AxisTimer::startAxisTimerInterrupts(
		motorStepInterruptRate * 1000 );	// kHz->Hz

	if ( softPwmInterruptRate )
		SoftPwmTimer::startSoftPwmTimerInterrupts(
			softPwmInterruptRate * 1000 );	// kHz->Hz
	};


void
	MainCommands::configureSSI(
		char*		data,
		String*		msgPtr ) {

	uint32_t   ssiNumber;
	sscanf( data,
		"%lu",
		&ssiNumber );
		
	// First delete an existing SSI with this number
	SSI* ssi		= SSI::ssiWithNumber( ssiNumber );
	if ( ssi )
		delete ssi;

	SSI*  newssi	= NULL;
	switch ( *data ) {
		case SSI_TYPE_ST :
		newssi		= new SSIdSpin (
			data,
			msgPtr );
		break;
		
		case SSI_TYPE_8711 :
		newssi		= new SSI8711 (
			data,
			msgPtr );
		break;
		
		case SSI_TYPE_8305 :
		newssi		= new SSI8305 (
			data,
			msgPtr );
		break;
		
		default:
		*msgPtr		+= " Invalid SSI type";
		return; };

	if ( newssi->configured ) {
		newssi->nextSSI    = SSI::firstSsi; // Add the new SSI - order not significant
		SSI::firstSsi      = newssi; }

	else
		delete newssi; };


void
	MainCommands::createMachine( char* data, String* msgPtr ) {

	MainCommands::deleteMachine( data, msgPtr ); // delete any machine with the same number
	Machine::firstMachine        = new Machine( data, msgPtr ); };


void
	MainCommands::deleteMachine( char* data, String* msgPtr ) {

	if ( Machine::firstMachine ) {
		Machine* machinePtr		= Machine::firstMachine;
		Machine::firstMachine	= NULL;
		delete machinePtr; }; };
