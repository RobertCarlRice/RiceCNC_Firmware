
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

#include <ADC.h>
#include <AnalogPin.h>

#include "inc/hw_adc.h"

// Allocate and initialize static variables
bool		ADC::adcRunning			= false;
AnalogPin*	ADC::sequencerReg[8][8]	= { NULL };

void
	adc0Seq0InterruptHandler(void) {

	ADC::intHandler(
		ADC0_BASE,
		0,
		ADC0_BASE + ADC_O_SSFIFO0 ); };

void
	adc0Seq1InterruptHandler(void) {

	ADC::intHandler(
		ADC0_BASE,
		1,
		ADC0_BASE + ADC_O_SSFIFO1 ); };

void
	adc0Seq2InterruptHandler(void) {

	ADC::intHandler(
		ADC0_BASE,
		2,
		ADC0_BASE + ADC_O_SSFIFO2 ); };

void
	adc0Seq3InterruptHandler(void) {

	ADC::intHandler(
		ADC0_BASE,
		3,
		ADC0_BASE + ADC_O_SSFIFO3 ); };


void
	adc1Seq0InterruptHandler(void) {

	ADC::intHandler(
		ADC1_BASE,
		4,
		ADC1_BASE + ADC_O_SSFIFO0 ); };

void
	adc1Seq1InterruptHandler(void) {

	ADC::intHandler(
		ADC1_BASE,
		5,
		ADC1_BASE + ADC_O_SSFIFO1 ); };

void
	adc1Seq2InterruptHandler(void) {

	ADC::intHandler(
		ADC1_BASE,
		6,
		ADC1_BASE + ADC_O_SSFIFO2 ); };

void
	adc1Seq3InterruptHandler(void) {

	ADC::intHandler(
		ADC1_BASE,
		7,
		ADC1_BASE + ADC_O_SSFIFO3 ); };

void
	ADC::configure() {

	for ( int i = 0; i < 8; i++ )
		for ( int j = 0; j < 8; j++ )
			sequencerReg[ i ][ j ]	= NULL;

    ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_ADC0 );
    ROM_SysCtlPeripheralEnable( SYSCTL_PERIPH_ADC1 );
	
	configureBase( ADC0_BASE );
	configureBase( ADC1_BASE );

	ADCIntRegister(
		ADC0_BASE,
		0,
		adc0Seq0InterruptHandler );

	ADCIntRegister(
		ADC0_BASE,
		1,
		adc0Seq1InterruptHandler );

	ADCIntRegister(
		ADC0_BASE,
		2,
		adc0Seq2InterruptHandler );

	ADCIntRegister(
		ADC0_BASE,
		3,
		adc0Seq3InterruptHandler );

	ADCIntRegister(
		ADC1_BASE,
		0,
		adc1Seq0InterruptHandler );

	ADCIntRegister(
		ADC1_BASE,
		1,
		adc1Seq1InterruptHandler );

	ADCIntRegister(
		ADC1_BASE,
		2,
		adc1Seq2InterruptHandler );

	ADCIntRegister(
		ADC1_BASE,
		3,
		adc1Seq3InterruptHandler );

    // clear interrupts
    HWREG( ADC0_BASE + ADC_O_ISC )	= 0xF;
	};

void
	ADC::configureBase( uint32_t adcBase ) {

	// Disable all sequences
	HWREG( adcBase + ADC_O_ACTSS )	= 0;

	// Set all sequencers to default ADC_TRIGGER_PROCESSOR
	HWREG( adcBase + ADC_O_EMUX )	= 0;

	// Set hardware oversampling
	HWREG( adcBase + ADC_O_SAC )	= 6;

    // Set the priority for sample sequences.
	HWREG( adcBase + ADC_O_SSPRI )	= 3;
	
	ADCReferenceSet(
		adcBase,
		ADC_REF_INT);
	
	// Enable interrupts
	HWREG( adcBase + ADC_O_IM )		= 0xF; };

void
	ADC::addAnalogPin( AnalogPin* analogPin ) {
	
	for (	uint32_t sequenceNumber = 0;
			sequenceNumber < 8;
			sequenceNumber++ ) {

		uint32_t adcSeq		= sequenceNumber & 3;

		uint32_t depth	= 8;
		if ( adcSeq )
			depth	= 4;
		if ( adcSeq == 3 )
			depth	= 1;

		for (	uint32_t step = 0;
				step < depth;
				step++ ) {

			if ( sequencerReg[ sequenceNumber ][ step ] == NULL ) {
				sequencerReg[ sequenceNumber ][ step ] = analogPin;

				uint32_t adcBase	= sequenceNumber < 4 ?
					ADC0_BASE
				:	ADC1_BASE;

				uint32_t seqOffset	= 
					adcBase +
					( ADC_O_SSMUX1 - ADC_O_SSMUX0 ) * adcSeq;

				uint32_t nibble		= step << 2;	// * 4

				// Disable the sequencer
				HWREG( adcBase	+ ADC_O_ACTSS )		&= ~( 1 << adcSeq );

				// Set the analog mux value for this step.
				HWREG( seqOffset + ADC_O_SSMUX0 )	&= ~( 0xF << nibble );
				HWREG( seqOffset + ADC_O_SSMUX0 )	|=
					( analogPin->aid->channelSelect & 0xF ) << nibble;

				// Set the analog mux extended bit for this step.
				HWREG( seqOffset + ADC_O_SSEMUX0 )	&= ~( 0xF << nibble );
				HWREG( seqOffset + ADC_O_SSEMUX0 )	|=
					( analogPin->aid->channelSelect >> 8 ) << nibble;

				// Set the last nibble to IE, END, single end, not temp
				HWREG( seqOffset + ADC_O_SSCTL0 )	= 0x6 << nibble;

				// Disable comparator output
				HWREG( seqOffset + ADC_O_SSOP0 )	&= ~( 1 << nibble );

				// Enable the sequencer
				HWREG( adcBase	+ ADC_O_ACTSS )		|= 1 << adcSeq;

				if ( ! adcRunning )
					triggerNextSequence(
						adcBase,
						sequenceNumber );
				
				return; }; }; }; };

void
	ADC::removeAnalogPin( AnalogPin* analogPin ) {

	for (	uint32_t sequenceNumber = 0;
			sequenceNumber < 8;
			sequenceNumber++ ) {

		for (	uint32_t step = 0;
				step < 8;
				step++ ) {

			if ( sequencerReg[ sequenceNumber ][ step ] == analogPin ) {

				while ( step < 8 ) {
					sequencerReg[ sequenceNumber ][ step ] = step == 7 ?
						NULL
					:	sequencerReg[ sequenceNumber ][ step + 1 ];
					step++; };

				uint32_t adcBase	= sequenceNumber < 4 ?
					ADC0_BASE
				:	ADC1_BASE;

				//	Safely stop the ADC converter
				// Set all sequencers to never trigger
				HWREG( adcBase + ADC_O_EMUX )	= 0xEEEE;
				// Wait until not busy
				while ( HWREG( adcBase + ADC_O_ACTSS ) & ADC_ACTSS_BUSY );

				uint32_t adcSeq		= sequenceNumber & 3;

				uint32_t seqOffset	= 
					adcBase +
					( ADC_O_SSMUX1 - ADC_O_SSMUX0 ) * adcSeq;

				uint32_t nibble		= step << 2;
				uint32_t mask		= 0xFFFFFFFF << nibble;

				// Disable the sequencer
				HWREG( adcBase	+ ADC_O_ACTSS )		&= ~( 1 << adcSeq );

				// shift the nibble out of the registers
				uint32_t cValue		= HWREG( seqOffset + ADC_O_SSMUX0 );
				HWREG( seqOffset + ADC_O_SSMUX0 )	&= ~mask;
				HWREG( seqOffset + ADC_O_SSMUX0 )	|= ( cValue >> 4 ) & mask;

				cValue				= HWREG( seqOffset + ADC_O_SSEMUX0 );
				HWREG( seqOffset + ADC_O_SSEMUX0 )	&= ~mask;
				HWREG( seqOffset + ADC_O_SSEMUX0 )	|= ( cValue >> 4 ) & mask;

				// Set the control for all steps - only one end interrupt
				HWREG( seqOffset + ADC_O_SSCTL0 )	>>= 4;
				if ( HWREG( seqOffset + ADC_O_SSCTL0 ) )
					// Reenable the sequencer
					HWREG( adcBase	+ ADC_O_ACTSS )		|= 1 << adcSeq;

				// Set all sequencers to default ADC_TRIGGER_PROCESSOR
				HWREG( adcBase + ADC_O_EMUX )	= 0;

				triggerNextSequence(
					adcBase,
					sequenceNumber );

				return; }; }; }; };

void
	ADC::intHandler(
		uint32_t	adcBase,
		uint32_t	sequenceNumber,
		uint32_t	fifo ) {
	
	uint32_t		adcSeq	= sequenceNumber & 3;

	//	Clear the interrupt
	HWREG( adcBase + ADC_O_ISC )	= 1 << adcSeq;

	for (	uint32_t step = 0;
			step < 8;
			step++ ) {
		AnalogPin* pin	= sequencerReg[ sequenceNumber ][ step ];
		if ( ! pin )
			break;
		pin->averageSample( HWREG( fifo ) ); };
	
	HWREG( fifo ); // extraneous read ensures synchronization

	// Loop by trigerring the next non-empty sequence
	triggerNextSequence(
		adcBase,
		sequenceNumber ); };

void
	ADC::triggerNextSequence(
		uint32_t	adcBase,
		uint32_t	sequenceNumber ) {

	for (	uint32_t i = 0;
			i < 8;
			i++ ) {
		sequenceNumber++;
		sequenceNumber	&= 7;
		if ( sequencerReg[ sequenceNumber ][ 0 ] ) {
			adcBase		= sequenceNumber < 4 ?
				ADC0_BASE :
				ADC1_BASE;
			ADCProcessorTrigger(
				adcBase,
				sequenceNumber & 3 );
			adcRunning		= true;
			return; }; };

	adcRunning		= false; };
