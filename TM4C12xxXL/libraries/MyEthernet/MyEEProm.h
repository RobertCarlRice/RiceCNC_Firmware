
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

#ifndef MyEEProm_h
#define MyEEProm_h

#include "Energia.h"
#include "IPAddress.h"
#include "netif/etharp.h"


#define UseKey 0xA5  // something not likely to match uninitialized eeProm

class MyEEProm {
public:	
	enum SoftResetReason {
		ResetRequestFromHost,
		HostMessageTimeout,
		MaxPacketRetransmission };

	typedef union {
		uint32_t firstEEPromWord; // force word alignment
		struct {
			byte	  useKey;
//			bool  useDHCP; // DHCP hangs
			byte      macAddress      [6];
			byte      myIpAddress     [4];
			byte      routerAddress   [4];
			byte      subnetMask      [4];
			uint16_t  port;
   
			byte      remoteIpAddress [4];
			uint16_t  remote_Port;
			
			SoftResetReason	softResetReason;

			// pad to allow for misaligned word boundary
			byte      padding         [3]; } S; } Un;

	static Un U;
	
	static void
		setUseKey( bool use );

	static void
		readEEProm();

	static void
		writeEEProm();

	static void
		debugEePromCopyDump( String* msgPtr );

	static void
		dumpBytes(
			byte*		buffer,
			int			count,
			String*		msgPtr );

	static SoftResetReason
		softResetReason();

	static void
		recordResetReason(
			SoftResetReason	softResetReason );

	static void
		readMacAddress( byte* mac );

	static void
		setEthernetAddresses(
			char*		data,
			String*		msgPtr );

	static void
		recordConnectionFrom(
			IPAddress	remote_IP,
			uint16_t	remote_port );

	static bool
		remoteIsRecorded (
			IPAddress	remote_IP,
			uint16_t	remote_port );

private:
	static void
		MyEEPROMRead(
			uint32_t	*pui32Data,
			uint32_t	ui32PromAddress,
			uint32_t	ui32Count);

	static uint32_t
		MyEEPROMProgram(
			uint32_t	*pui32Data,
			uint32_t	ui32Address,
			uint32_t	ui32Count);

	static void
		parseIpAddress(
			char*		stringPtr,
			byte*		readByte,
			bool*		change );

	static void
		compare(
			byte*		readByte,
			byte*		writeByte,
			int			length,
			bool*		change );
};

#endif
