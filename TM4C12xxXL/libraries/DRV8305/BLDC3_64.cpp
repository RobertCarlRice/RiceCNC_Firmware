
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

#ifdef sineTable64

#define MicrostepIncrement 5	// 1/64 micro-step
enum  MagneticAngle64 {
	MS64_0			= 0,
		MS64_30		= 1		<< MicrostepIncrement,
	MS64_60			= 2		<< MicrostepIncrement,
		MS64_90		= 3		<< MicrostepIncrement,	// 96
	MS64_120		= 4		<< MicrostepIncrement,
		MS64_150	= 5		<< MicrostepIncrement,
	MS64_180		= 6		<< MicrostepIncrement,
		MS64_210	= 7		<< MicrostepIncrement,
	MS64_240		= 8		<< MicrostepIncrement,
		MS64_270	= 9		<< MicrostepIncrement,
	MS64_300		= 10	<< MicrostepIncrement,
		MS64_330	= 11	<< MicrostepIncrement,
	MS64_360		= 12	<< MicrostepIncrement };	// 384


static const float
	BLDC3::sineTable64[] = {
		0,					// 0
		0.016361731626487,	// 0_9375
		0.032719082821776,	// 1_875
		0.049067674327418,	// 2_8125
		0.065403129230143,	// 3_75
		0.081721074133668,	// 4_6875
		0.098017140329561,	// 5_625
		0.114286964966846,	// 6_5625
		0.130526192220052,	// 7_5
		0.146730474455362,	// 8_4375
		0.162895473394589,	// 9_375
		0.179016861276633,	// 10_3125
		0.195090322016128,	// 11_25
		0.211111552358965,	// 12_1875
		0.227076263034373,	// 13_125
		0.242980179903264,	// 14_0625
		0.258819045102521,	// 15
		0.274588618184932,	// 15_9375
		0.290284677254462,	// 16_875
		0.305903020096554,	// 17_8125
		0.321439465303162,	// 18_75
		0.33688985339222,	// 19_6875
		0.352250047921234,	// 20_625
		0.367515936594704,	// 21_5625
		0.38268343236509,	// 22_5
		0.397748474527011,	// 23_4375
		0.412707029804395,	// 24_375
		0.427555093430282,	// 25_3125
		0.442288690219001,	// 26_25
		0.453990499739547,	// 27_1875
		0.469471562785891,	// 28_125
		0.484809620246337,	// 29_0625
		0.5,				// 30
		0.514102744193222,	// 30_9375
		0.528067850650368,	// 31_875
		0.541891580574752,	// 32_8125
		0.555570233019602,	// 33_75
		0.569100145878898,	// 34_6875
		0.582477696867802,	// 35_625
		0.595699304492433,	// 36_5625
		0.608761429008721,	// 37_5
		0.621660573370077,	// 38_4375
		0.634393284163646,	// 39_375
		0.646956152534857,	// 40_3125
		0.659345815100069,	// 41_25
		0.671558954847018,	// 42_1875
		0.683592302022871,	// 43_125
		0.695442635009612,	// 44_0625
		0.707106781186548,	// 45
		0.718581617779698,	// 45_9375
		0.729864072697836,	// 46_875
		0.740951125354959,	// 47_8125
		0.751839807478977,	// 48_75
		0.762527203906388,	// 49_6875
		0.773010453362737,	// 50_625
		0.78328674922865,	// 51_5625
		0.793353340291235,	// 52_5
		0.803207531480645,	// 53_4375
		0.812846684591615,	// 54_375
		0.822268218989775,	// 55_3125
		0.831469612302545,	// 56_25
		0.840448401094438,	// 57_1875
		0.849202181526579,	// 58_125
		0.857728610000272,	// 59_0625
		0.866025403784439,	// 60
		0.874090341626759,	// 60_9375
		0.881921264348355,	// 61_875
		0.889516075421856,	// 62_8125
		0.896872741532688,	// 63_75
		0.903989293123443,	// 64_6875
		0.910863824921176,	// 65_625
		0.917494496447491,	// 66_5625
		0.923879532511287,	// 67_5
		0.930017223684012,	// 68_4375
		0.935905926757326,	// 69_375
		0.941544065183021,	// 70_3125
		0.946930129495106,	// 71_25
		0.952062677713924,	// 72_1875
		0.956940335732209,	// 73_125
		0.961561797682962,	// 74_0625
		0.965925826289068,	// 75
		0.970031253194544,	// 75_9375
		0.973876979277334,	// 76_875
		0.977461974943572,	// 77_8125
		0.98078528040323,	// 78_75
		0.983846005927077,	// 79_6875
		0.986643332084879,	// 80_625
		0.989176509964781,	// 81_5625
		0.99144486137381,	// 82_5
		0.993447779019444,	// 83_4375
		0.995184726672197,	// 84_375
		0.99665523930918,	// 85_3125
		0.997858923238604,	// 86_25
		0.998795456205172,	// 87_1875
		0.999464587476366,	// 88_125
		0.999866137909562,	// 89_0625
		1.0	};				// 90 };

#define Scaling 4096	// scaled to 12 bits 1/64 microstep

const uint16_t
	BLDC3::commutationVector64[] = {
		Scaling * sineTable64[ 64 ],		// A:0		B:120		C:240
		Scaling * sineTable64[ 65 ],		// A:0.9375
		Scaling * sineTable64[ 66 ],		// A: 1.875, 358.125
		Scaling * sineTable64[ 67 ],
		Scaling * sineTable64[ 68 ],		// A: 3.75,	 356.25
		Scaling * sineTable64[ 69 ],
		Scaling * sineTable64[ 70 ],		// A: 5.625, 354.375
		Scaling * sineTable64[ 71 ],
		Scaling * sineTable64[ 72 ],		// A:7.5
		Scaling * sineTable64[ 73 ],
		Scaling * sineTable64[ 74 ],		// A:9.375
		Scaling * sineTable64[ 75 ],
		Scaling * sineTable64[ 76 ],		// A:11.25
		Scaling * sineTable64[ 77 ],
		Scaling * sineTable64[ 78 ],		// A:13.125
		Scaling * sineTable64[ 79 ],
		Scaling * sineTable64[ 80 ],		// A:15
		Scaling * sineTable64[ 81 ],
		Scaling * sineTable64[ 82 ],		// A:16.875
		Scaling * sineTable64[ 83 ],
		Scaling * sineTable64[ 84 ],		// A:18.75
		Scaling * sineTable64[ 85 ],
		Scaling * sineTable64[ 86 ],		// A:20.625
		Scaling * sineTable64[ 87 ],
		Scaling * sineTable64[ 88 ],		// A:22.5
		Scaling * sineTable64[ 89 ],
		Scaling * sineTable64[ 90 ],		// A:24.375
		Scaling * sineTable64[ 91 ],
		Scaling * sineTable64[ 92 ],		// A:26.25
		Scaling * sineTable64[ 93 ],
		Scaling * sineTable64[ 94 ],		// A:28.125
		Scaling * sineTable64[ 95 ],
		Scaling * sineTable64[ 96 ],		// A: 30,330	B:150,210	C:270,90
		Scaling * sineTable64[ 95 ],
		Scaling * sineTable64[ 94 ],
		Scaling * sineTable64[ 93 ],
		Scaling * sineTable64[ 92 ],
		Scaling * sineTable64[ 91 ],
		Scaling * sineTable64[ 90 ],
		Scaling * sineTable64[ 89 ],
		Scaling * sineTable64[ 88 ],
		Scaling * sineTable64[ 87 ],
		Scaling * sineTable64[ 86 ],
		Scaling * sineTable64[ 85 ],
		Scaling * sineTable64[ 84 ],
		Scaling * sineTable64[ 83 ],
		Scaling * sineTable64[ 82 ],
		Scaling * sineTable64[ 81 ],
		Scaling * sineTable64[ 80 ],
		Scaling * sineTable64[ 79 ],
		Scaling * sineTable64[ 78 ],
		Scaling * sineTable64[ 77 ],
		Scaling * sineTable64[ 76 ],
		Scaling * sineTable64[ 75 ],
		Scaling * sineTable64[ 74 ],
		Scaling * sineTable64[ 73 ],
		Scaling * sineTable64[ 72 ],
		Scaling * sineTable64[ 71 ],
		Scaling * sineTable64[ 70 ],
		Scaling * sineTable64[ 69 ],
		Scaling * sineTable64[ 68 ],
		Scaling * sineTable64[ 67 ],
		Scaling * sineTable64[ 66 ],
		Scaling * sineTable64[ 65 ],
		Scaling * sineTable64[ 64 ],		// A: 60,	300
		Scaling * sineTable64[ 63 ],
		Scaling * sineTable64[ 62 ],
		Scaling * sineTable64[ 61 ],
		Scaling * sineTable64[ 60 ],
		Scaling * sineTable64[ 59 ],
		Scaling * sineTable64[ 58 ],
		Scaling * sineTable64[ 57 ],
		Scaling * sineTable64[ 56 ],
		Scaling * sineTable64[ 55 ],
		Scaling * sineTable64[ 54 ],
		Scaling * sineTable64[ 53 ],
		Scaling * sineTable64[ 52 ],
		Scaling * sineTable64[ 51 ],
		Scaling * sineTable64[ 50 ],
		Scaling * sineTable64[ 49 ],
		Scaling * sineTable64[ 48 ],
		Scaling * sineTable64[ 47 ],
		Scaling * sineTable64[ 46 ],
		Scaling * sineTable64[ 45 ],
		Scaling * sineTable64[ 44 ],
		Scaling * sineTable64[ 43 ],
		Scaling * sineTable64[ 42 ],
		Scaling * sineTable64[ 41 ],
		Scaling * sineTable64[ 40 ],
		Scaling * sineTable64[ 39 ],
		Scaling * sineTable64[ 38 ],
		Scaling * sineTable64[ 37 ],
		Scaling * sineTable64[ 36 ],
		Scaling * sineTable64[ 35 ],
		Scaling * sineTable64[ 34 ],
		Scaling * sineTable64[ 33 ],
		Scaling * sineTable64[ 32 ],		// A: 90,	270
		Scaling * sineTable64[ 31 ],
		Scaling * sineTable64[ 30 ],
		Scaling * sineTable64[ 29 ],
		Scaling * sineTable64[ 28 ],
		Scaling * sineTable64[ 27 ],
		Scaling * sineTable64[ 26 ],
		Scaling * sineTable64[ 25 ],
		Scaling * sineTable64[ 24 ],
		Scaling * sineTable64[ 23 ],
		Scaling * sineTable64[ 22 ],
		Scaling * sineTable64[ 21 ],
		Scaling * sineTable64[ 20 ],
		Scaling * sineTable64[ 19 ],
		Scaling * sineTable64[ 18 ],
		Scaling * sineTable64[ 17 ],
		Scaling * sineTable64[ 16 ],
		Scaling * sineTable64[ 15 ],
		Scaling * sineTable64[ 14 ],
		Scaling * sineTable64[ 13 ],
		Scaling * sineTable64[ 12 ],
		Scaling * sineTable64[ 11 ],
		Scaling * sineTable64[ 10 ],
		Scaling * sineTable64[ 9 ],
		Scaling * sineTable64[ 8 ],
		Scaling * sineTable64[ 7 ],
		Scaling * sineTable64[ 6 ],
		Scaling * sineTable64[ 5 ],
		Scaling * sineTable64[ 4 ],
		Scaling * sineTable64[ 3 ],
		Scaling * sineTable64[ 2 ],
		Scaling * sineTable64[ 1 ],
		Scaling * sineTable64[ 0 ]	};	// A: 120,	240

const float
	BLDC3::tanVector64[] = {
		0,
		0.016363922135312,	// Tan_0_9375
		0.032736610412973,	// Tan_1_875
		0.049126849769467,	// Tan_2_8125
		0.065543462815238,	// Tan_3_75
		0.06992681194351,	// Tan_4_6875
		0.081995328886559,	// Tan_5_625
		0.115040737299398,	// Tan_6_5625
		0.131652497587396,	// Tan_7_5
		0.148335987538347,	// Tan_8_4375
		0.165100668192197,	// Tan_9_375
		0.181956180335301,	// Tan_10_3125
		0.198912367379658,	// Tan_11_25
		0.215979299216374,	// Tan_12_1875
		0.233167297170198,	// Tan_13_125
		0.250486960191306,	// Tan_14_0625
		0.267949192431123,	// Tan_15
		0.285565232361142,	// Tan_15_9375
		0.303346683607342,	// Tan_16_875
		0.321305547688396,	// Tan_17_8125
		0.339454258863376,	// Tan_18_75
		0.357805721314524,	// Tan_19_6875
		0.376373348913058,	// Tan_20_625
		0.395171107841303,	// Tan_21_5625
		0.414213562373095,	// Tan_22_5
		0.433515924146772,	// Tan_23_4375
		0.453094105301755,	// Tan_24_375
		0.47296477589132,	// Tan_25_3125
		0.493145426031304,	// Tan_26_25
		0.513654433298164,	// Tan_27_1875
		0.534511135950792,	// Tan_28_125
		0.555735912620075,	// Tan_29_0625
		0.577350269189626,	// Tan_30
		0.599376933681924,	// Tan_30_9375
		0.621839960068261,	// Tan_31_875
		0.644764842040388,	// Tan_32_8125
		0.668178637919299,	// Tan_33_75
		0.692110108035168,	// Tan_34_6875
		0.716589866095654,	// Tan_35_625
		0.741650546272035,	// Tan_36_5625
		0.76732698797896,	// Tan_37_5
		0.793656440610369,	// Tan_38_4375
		0.82067879082866,	// Tan_39_375
		0.848436815395613,	// Tan_40_3125
		0.876976462992757,	// Tan_41_25
		0.906347169019147,	// Tan_42_1875
		0.936602207992061,	// Tan_43_125
		0.9677990889308,	// Tan_44_0625
		1,					// Tan_45
		1.033272309756744,	// Tan_45_9375
		1.067689133622538,	// Tan_46_875
		1.103329975733475,	// Tan_47_8125
		1.140281458167548,	// Tan_48_75
		1.178638151780006,	// Tan_49_6875
		1.218503525587976,	// Tan_50_625
		1.259991034950766,	// Tan_51_5625
		1.303225372841206,	// Tan_52_5
		1.34834391348672,	// Tan_53_4375
		1.395498383822406,	// Tan_54_375
		1.444856805861283,	// Tan_55_3125
		1.496605762665489,	// Tan_56_25
		1.550953052643897,	// Tan_57_1875
		1.608130812130869,	// Tan_58_125
		1.668399205583506,	// Tan_59_0625
		1.732050807568877,	// Tan_60
		1.799415832756597,	// Tan_60_9375
		1.87086841178939,	// Tan_61_875
		1.946834165489473,	// Tan_62_8125
		2.027799401989225,	// Tan_63_75
		2.11432235754864,	// Tan_64_6875
		2.207047031287269,	// Tan_65_625
		2.306720340130894,	// Tan_66_5625
		2.414213562373094,	// Tan_67_5
		2.530549375086377,	// Tan_68_4375
		2.656936265248156,	// Tan_69_375
		2.794812772490477,	// Tan_70_3125
		2.945905004545788,	// Tan_71_25
		3.112302315333208,	// Tan_72_1875
		3.296558208938322,	// Tan_73_125
		3.50182685662288,	// Tan_74_0625
		3.732050807568878,	// Tan_75
		3.992223783770083,	// Tan_75_9375
		4.288766101148648,	// Tan_76_875
		4.630073361790908,	// Tan_77_8125
		5.027339492125847,	// Tan_78_75
		5.495828710831612,	// Tan_79_6875
		6.056910677283768,	// Tan_80_625
		6.741452405414988,	// Tan_81_5625
		7.595754112725143,	// Tan_82_5
		8.692572939596692,	// Tan_83_4375
		10.15317038760887,	// Tan_84_375
		12.19581668345409,	// Tan_85_3125
		15.25705168826554,	// Tan_86_25
		20.35546762498714,	// Tan_87_1875
		30.54683998694403,	// Tan_88_125
		61.11004389602284	// Tan_89_0625
		};
#endif
