
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

#include <BLDC3.h>

//#define DebugDelete 1

/* To understand this eight-step commutation scheme think of a triangle rolling
slowly on a flat surface. One corner (sometimes two) is referenced to zero for
efficiency, while the level of the other two verticies is calculated.
Starting from flat, the top vertex will first climb from 60° to 90°, then
continue down to 0°
*/

#define Scaling 4096	// scaled to 12 bits 1/64 microstep

const float
	BLDC3::sineTable50[] = {	// for 600 ppr encoder
		0,					// 0
		0.020942419883357,	// 1.2
		0.0418756537292,	// 2.4
		0.062790519529313,	// 3.6
		0.083677843332315,	// 4.8
		0.104528463267654,	// 6
		0.125333233564304,	// 7.2
		0.146083028562412,	// 8.4
		0.166768746716102,	// 9.6
		0.187381314585725,	// 10.8
		0.207911690817759,	// 12
		0.228350870110656,	// 13.2
		0.248689887164855,	// 14.4
		0.268919820615266,	// 15.6
		0.289031796944472,	// 16.8
		0.309016994374947,	// 18
		0.328866646738583,	// 19.2
		0.348572047321815,	// 20.4
		0.368124552684678,	// 21.6
		0.387515586452103,	// 22.8
		0.4067366430758,	// 24
		0.425779291565073,	// 25.2
		0.444635179184928,	// 26.4
		0.463296035119862,	// 27.6
		0.481753674101715,	// 28.8
		0.5,				// 30
		0.51802700937313,	// 31.2
		0.535826794978997,	// 32.4
		0.553391549243344,	// 33.6
		0.570713567684432,	// 34.8
		0.587785252292473,	// 36
		0.604599114862375,	// 37.2
		0.62114778027831,	// 38.4
		0.63742398974869,	// 39.6
		0.653420603990105,	// 40.8
		0.669130606358858,	// 42
		0.684547105928689,	// 43.2
		0.699663340513365,	// 44.4
		0.714472679632803,	// 45.6
		0.728968627421411,	// 46.8
		0.743144825477394,	// 48
		0.756995055651756,	// 49.2
		0.770513242775789,	// 50.4
		0.78369345732584,	// 51.6
		0.796529918024196,	// 52.8
		0.809016994374947,	// 54
		0.821149209133704,	// 55.2
		0.832921240710099,	// 56.4
		0.844327925502015,	// 57.6
		0.855364260160507,	// 58.8
		0.866025403784439,	// 60
		0.876306680043864,	// 61.2
		0.886203579231215,	// 62.4
		0.895711760239413,	// 63.6
		0.90482705246602,	// 64.8
		0.913545457642601,	// 66
		0.921863151588501,	// 67.2
		0.929776485888252,	// 68.4
		0.937281989491892,	// 69.6
		0.944376370237481,	// 70.8
		0.951056516295154,	// 72
		0.957319497532067,	// 73.2
		0.963162566797658,	// 74.4
		0.968583161128631,	// 75.6
		0.97357890287316,	// 76.8
		0.978147600733806,	// 78
		0.982287250728689,	// 79.2
		0.985996037070505,	// 80.4
		0.989272332962988,	// 81.6
		0.992114701314478,	// 82.8
		0.994521895368273,	// 84
		0.996492859249504,	// 85.2
		0.998026728428272,	// 86.4
		0.999122830098858,	// 87.6
		0.999780683474846,	// 88.8
		1.0	};				// 90 };

enum Deg50 {
	Deg0,
	Deg1_2,
	Deg2_4,
	Deg3_6,
	Deg4_8,
	Deg6,
	Deg7_2,
	Deg8_4,
	Deg9_6,
	Deg10_8,
	Deg12,
	Deg13_2,
	Deg14_4,
	Deg15_6,
	Deg16_8,
	Deg18,
	Deg19_2,
	Deg20_4,
	Deg21_6,
	Deg22_8,
	Deg24,
	Deg25_2,
	Deg26_4,
	Deg27_6,
	Deg28_8,
	Deg30,
	Deg31_2,
	Deg32_4,
	Deg33_6,
	Deg34_8,
	Deg36,
	Deg37_2,
	Deg38_4,
	Deg39_6,
	Deg40_8,
	Deg42,
	Deg43_2,
	Deg44_4,
	Deg45_6,
	Deg46_8,
	Deg48,
	Deg49_2,
	Deg50_4,
	Deg51_6,
	Deg52_8,
	Deg54,
	Deg55_2,
	Deg56_4,
	Deg57_6,
	Deg58_8,
	Deg60,
	Deg61_2,
	Deg62_4,
	Deg63_6,
	Deg64_8,
	Deg66,
	Deg67_2,
	Deg68_4,
	Deg69_6,
	Deg70_8,
	Deg72,
	Deg73_2,
	Deg74_4,
	Deg75_6,
	Deg76_8,
	Deg78,
	Deg79_2,
	Deg80_4,
	Deg81_6,
	Deg82_8,
	Deg84,
	Deg85_2,
	Deg86_4,
	Deg87_6,
	Deg88_8,
	Deg90
	};

const uint16_t
	BLDC3::commutationVector50[] = {
		Scaling * sineTable50[ Deg60 ],
		Scaling * sineTable50[ Deg61_2 ],
		Scaling * sineTable50[ Deg62_4 ],
		Scaling * sineTable50[ Deg63_6 ],
		Scaling * sineTable50[ Deg64_8 ],
		Scaling * sineTable50[ Deg66 ],
		Scaling * sineTable50[ Deg67_2 ],
		Scaling * sineTable50[ Deg68_4 ],
		Scaling * sineTable50[ Deg69_6 ],
		Scaling * sineTable50[ Deg70_8 ],
		Scaling * sineTable50[ Deg72 ],
		Scaling * sineTable50[ Deg73_2 ],
		Scaling * sineTable50[ Deg74_4 ],
		Scaling * sineTable50[ Deg75_6 ],
		Scaling * sineTable50[ Deg76_8 ],
		Scaling * sineTable50[ Deg78 ],
		Scaling * sineTable50[ Deg79_2 ],
		Scaling * sineTable50[ Deg80_4 ],
		Scaling * sineTable50[ Deg81_6 ],
		Scaling * sineTable50[ Deg82_8 ],
		Scaling * sineTable50[ Deg84 ],
		Scaling * sineTable50[ Deg85_2 ],
		Scaling * sineTable50[ Deg86_4 ],
		Scaling * sineTable50[ Deg87_6 ],
		Scaling * sineTable50[ Deg88_8 ],
		Scaling * sineTable50[ Deg90 ],
		Scaling * sineTable50[ Deg88_8 ],
		Scaling * sineTable50[ Deg87_6 ],
		Scaling * sineTable50[ Deg86_4 ],
		Scaling * sineTable50[ Deg85_2 ],
		Scaling * sineTable50[ Deg84 ],
		Scaling * sineTable50[ Deg82_8 ],
		Scaling * sineTable50[ Deg81_6 ],
		Scaling * sineTable50[ Deg80_4 ],
		Scaling * sineTable50[ Deg79_2 ],
		Scaling * sineTable50[ Deg78 ],
		Scaling * sineTable50[ Deg76_8 ],
		Scaling * sineTable50[ Deg75_6 ],
		Scaling * sineTable50[ Deg74_4 ],
		Scaling * sineTable50[ Deg73_2 ],
		Scaling * sineTable50[ Deg72 ],
		Scaling * sineTable50[ Deg70_8 ],
		Scaling * sineTable50[ Deg69_6 ],
		Scaling * sineTable50[ Deg68_4 ],
		Scaling * sineTable50[ Deg67_2 ],
		Scaling * sineTable50[ Deg66 ],
		Scaling * sineTable50[ Deg64_8 ],
		Scaling * sineTable50[ Deg63_6 ],
		Scaling * sineTable50[ Deg62_4 ],
		Scaling * sineTable50[ Deg61_2 ],
		Scaling * sineTable50[ Deg60 ],
		Scaling * sineTable50[ Deg58_8 ],
		Scaling * sineTable50[ Deg57_6 ],
		Scaling * sineTable50[ Deg56_4 ],
		Scaling * sineTable50[ Deg55_2 ],
		Scaling * sineTable50[ Deg54 ],
		Scaling * sineTable50[ Deg52_8 ],
		Scaling * sineTable50[ Deg51_6 ],
		Scaling * sineTable50[ Deg50_4 ],
		Scaling * sineTable50[ Deg49_2 ],
		Scaling * sineTable50[ Deg48 ],
		Scaling * sineTable50[ Deg46_8 ],
		Scaling * sineTable50[ Deg45_6 ],
		Scaling * sineTable50[ Deg44_4 ],
		Scaling * sineTable50[ Deg43_2 ],
		Scaling * sineTable50[ Deg42 ],
		Scaling * sineTable50[ Deg40_8 ],
		Scaling * sineTable50[ Deg39_6 ],
		Scaling * sineTable50[ Deg38_4 ],
		Scaling * sineTable50[ Deg37_2 ],
		Scaling * sineTable50[ Deg36 ],
		Scaling * sineTable50[ Deg34_8 ],
		Scaling * sineTable50[ Deg33_6 ],
		Scaling * sineTable50[ Deg32_4 ],
		Scaling * sineTable50[ Deg31_2 ],
		Scaling * sineTable50[ Deg30 ],
		Scaling * sineTable50[ Deg28_8 ],
		Scaling * sineTable50[ Deg27_6 ],
		Scaling * sineTable50[ Deg26_4 ],
		Scaling * sineTable50[ Deg25_2 ],
		Scaling * sineTable50[ Deg24 ],
		Scaling * sineTable50[ Deg22_8 ],
		Scaling * sineTable50[ Deg21_6 ],
		Scaling * sineTable50[ Deg20_4 ],
		Scaling * sineTable50[ Deg19_2 ],
		Scaling * sineTable50[ Deg18 ],
		Scaling * sineTable50[ Deg16_8 ],
		Scaling * sineTable50[ Deg15_6 ],
		Scaling * sineTable50[ Deg14_4 ],
		Scaling * sineTable50[ Deg13_2 ],
		Scaling * sineTable50[ Deg12 ],
		Scaling * sineTable50[ Deg10_8 ],
		Scaling * sineTable50[ Deg9_6 ],
		Scaling * sineTable50[ Deg8_4 ],
		Scaling * sineTable50[ Deg7_2 ],
		Scaling * sineTable50[ Deg6 ],
		Scaling * sineTable50[ Deg4_8 ],
		Scaling * sineTable50[ Deg3_6 ],
		Scaling * sineTable50[ Deg2_4 ],
		Scaling * sineTable50[ Deg1_2 ],
		Scaling * sineTable50[ Deg0 ]	};

const float
	BLDC3::tanVector50[] = {
		0,					// 0
		0.02094701390966,	// 1.2
		0.041912418040789,	// 2.4
		0.06291466725365,	// 3.6
		0.083972346169481,	// 4.8
		0.105104235265677,	// 6
		0.126329378446108,	// 7.2
		0.147667152607893,	// 8.4
		0.169137339751982,	// 9.6
		0.190760202218567,	// 10.8
		0.212556561670022,	// 12
		0.234547882494949,	// 13.2
		0.256756360367727,	// 14.4
		0.2792050167703,	// 15.6
		0.301917800368199,	// 16.8
		0.324919696232906,	// 18
		0.348236844020021,	// 19.2
		0.371896666349877,	// 20.4
		0.395928008797721,	// 21.6
		0.420361293088197,	// 22.8
		0.445228685308536,	// 24
		0.470564281212251,	// 25.2
		0.496404310987367,	// 26.4
		0.522787366218689,	// 27.6
		0.54975465219277,	// 28.8
		0.577350269189626,	// 30
		0.6056215269924,	// 31.2
		0.634619297544148,	// 32.4
		0.66439841151314,	// 33.6
		0.695018105523749,	// 34.8
		0.726542528005361,	// 36
		0.759041313052109,	// 37.2
		0.792590233428544,	// 38.4
		0.827271945972476,	// 39.6
		0.863176845227244,	// 40.8
		0.90040404429784,	// 42
		0.939062505817492,	// 43.2
		0.979272350725784,	// 44.4
		1.021166378545104,	// 45.6
		1.064891840324792,	// 46.8
		1.110612514829193,	// 48
		1.15851115044303,	// 49.2
		1.208792350409609,	// 50.4
		1.261685998418443,	// 51.6
		1.31745134659271,	// 52.8
		1.376381920471173,	// 54
		1.438811438223504,	// 55.2
		1.505120997689535,	// 56.4
		1.575747859968651,	// 57.6
		1.651196259429776,	// 58.8
		1.732050807568877,	// 60
		1.818993247281066,	// 61.2
		1.912823577266186,	// 62.4
		2.014486937091585,	// 63.6
		2.125108173157202,	// 64.8
		2.246036773904216,	// 66
		2.378905994539766,	// 67.2
		2.525711689447306,	// 68.4
		2.688918967235936,	// 69.6
		2.871608840856907,	// 70.8
		3.077683537175253,	// 72
		3.312159795747269,	// 73.2
		3.581597535629855,	// 74.4
		3.894742854929857,	// 75.6
		4.263521756678117,	// 76.8
		4.704630109478451,	// 78
		5.242183581113181,	// 79.2
		5.912355021465794,	// 80.4
		6.77198674410242,	// 81.6
		7.915815088305826,	// 82.8
		9.514364454222587,	// 84
		11.90868238910108,	// 85.2
		15.89454484386527,	// 86.4
		23.85927719624275,	// 87.6
		47.73950140639533,	// 88.8
		1000000				// 90
		};
