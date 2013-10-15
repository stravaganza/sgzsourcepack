/*
 Snips taken from maxsdk\samples\objects\particles\blizzard.cpp to
 give support to superspray. I tried to simplify things to need
 the minimum of non maxsdk API code (this isn't from the API itself,
 it's taken from the particles plugin source) to compile and link this project.
 If there's some problem in adding this code please contact me at ithaqua@maptel.es
*/

#ifndef __SUPERSPRAYSUPPORT_H
#define __SUPERSPRAYSUPPORT_H

static Class_ID SUPRSPRAY_CLASS_ID(0x74f811e3, 0x21fb7b57);

#define PB_OFFAXIS				0
#define PB_AXISSPREAD			1
#define PB_OFFPLANE				2
#define PB_PLANESPREAD			3
#define PB_SPEED				4
#define PB_SPEEDVAR				5

#define PB_BIRTHMETHOD			6
#define PB_PBIRTHRATE			7
#define PB_PTOTALNUMBER			8
#define PB_EMITSTART			9
#define PB_EMITSTOP				10
#define PB_DISPUNTIL			11
#define PB_LIFE					12
#define PB_LIFEVAR				13
#define PB_SUBFRAMEMOVE			14
#define PB_SUBFRAMETIME			15
#define PB_SIZE					16
#define PB_SIZEVAR				17
#define PB_GROWTIME				18
#define PB_FADETIME				19
#define PB_RNDSEED				20
#define PB_EMITRWID				21
#define PB_EMITRHID				22

#define PB_PARTICLECLASS		23
#define PB_PARTICLETYPE			24
#define PB_METATENSION			25
#define PB_METATENSIONVAR		26
#define PB_METACOURSE			27
#define PB_METAAUTOCOARSE		28
#define PB_USESUBTREE			29
#define PB_ANIMATIONOFFSET		30
#define PB_OFFSETAMOUNT			31
#define PB_VIEWPORTSHOWS		32
#define PB_DISPLAYPORTION		33
#define PB_MAPPINGTYPE			34
#define PB_MAPPINGTIME			35
#define PB_MAPPINGDIST			36
						
#define PB_SPINTIME				37
#define PB_SPINTIMEVAR			38
#define PB_SPINPHASE			39
#define PB_SPINPHASEVAR			40
#define PB_SPINAXISTYPE			41
#define PB_SPINAXISX			42
#define PB_SPINAXISY			43
#define PB_SPINAXISZ			44
#define PB_SPINAXISVAR			45

#define PB_EMITVINFL			46
#define PB_EMITVMULT			47
#define PB_EMITVMULTVAR			48

#define PB_SPAWNTYPE			49
#define PB_SPAWNGENS			50
#define PB_SPAWNCOUNT			51
#define PB_SPAWNDIRCHAOS		52
#define PB_SPAWNSPEEDCHAOS		53
#define PB_SPAWNSPEEDSIGN		54
#define PB_SPAWNINHERITV		55
#define PB_SPAWNSCALECHAOS		56
#define PB_SPAWNSCALESIGN		57
#define PB_SPAWNLIFEVLUE		58
#define PB_SPAWNSPEEDFIXED		59
#define PB_SPAWNSCALEFIXED		60	

#define PB_BUBLAMP				61
#define PB_BUBLAMPVAR			62
#define PB_BUBLPER				63
#define PB_BUBLPERVAR			64
#define PB_BUBLPHAS				65
#define PB_BUBLPHASVAR			66

#define PB_STRETCH				67
#define PB_CUSTOMMTL			68
#define PB_METACOURSEV			69
#define PB_SUBFRAMEROT			70
#define PB_SPAWNPERCENT			71
#define PB_SPAWNMULTVAR			72
#define PB_SSNOTDRAFT			73
#define PB_SSSPAWNDIEAFTER		74
#define PB_SSSPAWNDIEAFTERVAR	75

#define PB_IPCOLLIDE_ON			76
#define PB_IPCOLLIDE_STEPS		77
#define PB_IPCOLLIDE_BOUNCE		78
#define PB_IPCOLLIDE_BOUNCEVAR	79

#endif // __SUPERSPRAYSUPPORT_H