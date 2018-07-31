// FB Alpha Williams Games driver module
// Based on MAME driver by Aaron Giles

// Works:
//  Defender
//  Stargate
//  Joust
//  Robotron 2084
//  Bubbles
//  Colony 7
//  Mayday
//  Jin
//  Splat
//  Alien Area - no sound (there is none)
//  Sinistar
//  Blaster

// todo / tofix:
//  Speed Ball - inputs
//  Lottofun - memory protect sw. error

#include "tiles_generic.h"
#include "m6809_intf.h"
#include "m6800_intf.h"
#include "dac.h"
#include "hc55516.h"
#include "watchdog.h"
#include "6821pia.h"

static UINT8 *AllMem;
static UINT8 *MemEnd;
static UINT8 *AllRam;
static UINT8 *RamEnd;
static UINT8 *DrvM6809ROM0;
static UINT8 *DrvM6800ROM0;
static UINT8 *DrvM6800ROM1;
static UINT8 *DrvGfxROM;
static UINT8 *DrvColPROM;
static UINT8 *DrvNVRAM;
static UINT8 *DrvM6809RAM0;
static UINT8 *DrvM6800RAM0;
static UINT8 *DrvM6800RAM1;
static UINT8 *DrvVidRAM;
static UINT8 *DrvPalRAM;

static UINT8 *DrvBlitRAM; // 8 bytes
static UINT8 *blitter_remap; // 256*256

static UINT32 *Palette;
static UINT32 *DrvPalette;
static UINT8 DrvRecalc;

static INT32 nExtraCycles[3];

static UINT8 blitter_xor = 0;
static INT32 blitter_window_enable = 0;
static INT32 blitter_clip_address = ~0;
static INT32 blitter_remap_index = 0;

static UINT8 blaster_video_control = 0;
static INT32 blaster_color0 = 0;

static UINT8 cocktail = 0;
static UINT8 bankselect = 0;
static UINT8 vram_select = 0;
static UINT8 port_select = 0;
static UINT8 rom_bank = 0;

static INT32 screen_x_adjust;
static INT32 scanline;

static UINT8 DrvJoy1[8];
static UINT8 DrvJoy2[8];
static UINT8 DrvJoy3[8];
static UINT8 DrvJoy4[8];
static UINT8 DrvJoy5[8];
static UINT8 DrvJoy6[8];
static UINT8 DrvJoy7[8];
static UINT8 DrvInputs[7];
static UINT8 DrvDips[3] = { 0, 0, 0 };
static UINT8 DrvReset;
static INT16 DrvAnalogPort0 = 0;
static INT16 DrvAnalogPort1 = 0;

static INT32 mayday = 0;
static INT32 splat = 0;
static INT32 blaster = 0;
static INT32 uses_hc55516 = 0;
static INT32 uses_colprom = 0;

// dc-blocking filter for DAC
static INT16 dac_lastin_r;
static INT16 dac_lastout_r;
static INT16 dac_lastin_l;
static INT16 dac_lastout_l;

// raster update helpers
static INT32 lastline;
static void (*pStartDraw)() = NULL;
static void (*pDrawScanline)() = NULL;

static INT32 nCyclesDone[3];

static struct BurnInputInfo DefenderInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 5,	"p1 start"	},
	{"P1 Up",					BIT_DIGITAL,	DrvJoy2 + 0,	"p1 up"		},
	{"P1 Down",					BIT_DIGITAL,	DrvJoy1 + 7,	"p1 down"	},
	{"Fire",					BIT_DIGITAL,	DrvJoy1 + 0,	"p1 fire 1"	},
	{"Thrust",					BIT_DIGITAL,	DrvJoy1 + 1,	"p1 fire 2"	},
	{"Smart Bomb",				BIT_DIGITAL,	DrvJoy1 + 2,	"p1 fire 3"	},
	{"Hyperspace",				BIT_DIGITAL,	DrvJoy1 + 3,	"p1 fire 4"	},
	{"Reverse",					BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 5"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score Reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"service3"	},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};

STDINPUTINFO(Defender)

static struct BurnInputInfo MaydayInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 5,	"p1 start"	},
	{"P1 Up",					BIT_DIGITAL,	DrvJoy2 + 0,	"p1 up"		},
	{"P1 Down",					BIT_DIGITAL,	DrvJoy1 + 7,	"p1 down"	},
	{"P1 Right",				BIT_DIGITAL,	DrvJoy1 + 1,	"p1 right"	},
	{"Fire",					BIT_DIGITAL,	DrvJoy1 + 0,	"p1 fire 1"	},
	{"Mayday",					BIT_DIGITAL,	DrvJoy1 + 2,	"p1 fire 2"	},
	{"Back Fire",				BIT_DIGITAL,	DrvJoy1 + 3,	"p1 fire 3"	},

	{"P2 Coin",					BIT_DIGITAL,	DrvJoy3 + 5,	"p2 coin"	},
	{"P2 Start",				BIT_DIGITAL,	DrvJoy1 + 4,	"p2 start"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Test Credit",				BIT_DIGITAL,	DrvJoy3 + 2,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service3"	},
};

STDINPUTINFO(Mayday)

static struct BurnInputInfo Colony7InputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 5,	"p1 start"	},
	{"P1 Up",					BIT_DIGITAL,	DrvJoy1 + 3,	"p1 up"		},
	{"P1 Down",					BIT_DIGITAL,	DrvJoy1 + 0,	"p1 down"	},
	{"P1 Left",					BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"	},
	{"P1 Right",				BIT_DIGITAL,	DrvJoy1 + 1,	"p1 right"	},
	{"P1 Button 1",				BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 1"	},
	{"P1 Button 2",				BIT_DIGITAL,	DrvJoy1 + 7,	"p1 fire 2"	},
	{"P1 Button 3",				BIT_DIGITAL,	DrvJoy2 + 0,	"p1 fire 3"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 5,	"tilt"		},
	{"Dip A",					BIT_DIPSWITCH,	DrvDips + 0,	"dip"		}, // b
	{"Dip B",					BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
	{"Dip C",					BIT_DIPSWITCH,	DrvDips + 2,	"dip"		},
};

STDINPUTINFO(Colony7)

static struct BurnInputInfo JinInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 2,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 5,	"p1 start"	},
	{"P1 Up",					BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"P1 Down",					BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"P1 Left",					BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"	},
	{"P1 Right",				BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"	},
	{"P1 Button 1",				BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 1"	},
	{"P1 Button 2",				BIT_DIGITAL,	DrvJoy1 + 7,	"p1 fire 2"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Dip A",					BIT_DIPSWITCH,	DrvDips + 0,	"dip"		}, // 9
	{"Dip B",					BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
	{"Dip C",					BIT_DIPSWITCH,	DrvDips + 2,	"dip"		},
};

STDINPUTINFO(Jin)

static struct BurnInputInfo StargateInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 5,	"p1 start"	},
	{"P1 Up",					BIT_DIGITAL,	DrvJoy2 + 0,	"p1 up"		},
	{"P1 Down",					BIT_DIGITAL,	DrvJoy1 + 7,	"p1 down"	},
	{"Fire",					BIT_DIGITAL,	DrvJoy1 + 0,	"p1 fire 1"	},
	{"Thrust",					BIT_DIGITAL,	DrvJoy1 + 1,	"p1 fire 2"	},
	{"Smart Bomb",				BIT_DIGITAL,	DrvJoy1 + 2,	"p1 fire 3"	},
	{"Reverse",					BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 4"	},
	{"Inviso",					BIT_DIGITAL,	DrvJoy2 + 1,	"p1 fire 5"	},
	{"Hyperspace",				BIT_DIGITAL,	DrvJoy1 + 3,	"p1 fire 6"	},

	{"P2 Coin",					BIT_DIGITAL,	DrvJoy3 + 5,	"p2 coin"	},
	{"P2 Start",				BIT_DIGITAL,	DrvJoy1 + 4,	"p2 start"	},
	{"P3 Coin",					BIT_DIGITAL,	DrvJoy3 + 2,	"p3 coin"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"diag"		},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};

STDINPUTINFO(Stargate)

static struct BurnInputInfo RobotronInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 4,	"p1 start"	},
	{"Move Up",					BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"Move Down",				BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"Move Left",				BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"	},
	{"Move Right",				BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"	},
	{"Fire Up",					BIT_DIGITAL,	DrvJoy1 + 6,	"p2 up"		},
	{"Fire Down",				BIT_DIGITAL,	DrvJoy1 + 7,	"p2 down"	},
	{"Fire Left",				BIT_DIGITAL,	DrvJoy2 + 0,	"p2 left"	},
	{"Fire Right",				BIT_DIGITAL,	DrvJoy2 + 1,	"p2 right"	},

	{"P2 Coin",					BIT_DIGITAL,	DrvJoy3 + 5,	"p2 coin"	},
	{"P2 Start",				BIT_DIGITAL,	DrvJoy1 + 5,	"p2 start"	},
	{"P3 Coin",					BIT_DIGITAL,	DrvJoy3 + 2,	"p3 coin"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score Reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"service3"		},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};

STDINPUTINFO(Robotron)

static struct BurnInputInfo BubblesInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 5,	"p1 start"	},
	{"Up",						BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"Down",					BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"Left",					BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"	},
	{"Right",					BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"	},

	{"P2 Coin",					BIT_DIGITAL,	DrvJoy3 + 5,	"p2 coin"	},
	{"P2 Start",				BIT_DIGITAL,	DrvJoy1 + 4,	"p2 start"	},
	{"P3 Coin",					BIT_DIGITAL,	DrvJoy3 + 2,	"p3 coin"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score Reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"service3"	},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};

STDINPUTINFO(Bubbles)

static struct BurnInputInfo SplatInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 4,	"p1 start"	},

	{"P1 Left Stick Up",		BIT_DIGITAL,	DrvJoy5 + 0,	"p1 up"		},
	{"P1 Left Stick Down",		BIT_DIGITAL,	DrvJoy5 + 1,	"p1 down"	},
	{"P1 Left Stick Left",		BIT_DIGITAL,	DrvJoy5 + 2,	"p1 left"	},
	{"P1 Left Stick Right",     BIT_DIGITAL,	DrvJoy5 + 3,	"p1 right"	},
	{"P1 Right Stick Up",		BIT_DIGITAL,	DrvJoy5 + 6,	"p3 up"		},
	{"P1 Right Stick Down",		BIT_DIGITAL,	DrvJoy5 + 7,	"p3 down"	},
	{"P1 Right Stick Left",		BIT_DIGITAL,	DrvJoy7 + 0,	"p3 left"	},
	{"P1 Right Stick Right",	BIT_DIGITAL,	DrvJoy7 + 1,	"p3 right"	},

	{"P2 Coin",					BIT_DIGITAL,	DrvJoy3 + 5,	"p2 coin"	},
	{"P2 Start",				BIT_DIGITAL,	DrvJoy1 + 5,	"p2 start"	},
	{"P2 Left Stick Up",		BIT_DIGITAL,	DrvJoy4 + 0,	"p2 up"		},
	{"P2 Left Stick Down",		BIT_DIGITAL,	DrvJoy4 + 1,	"p2 down"	},
	{"P2 Left Stick Left",		BIT_DIGITAL,	DrvJoy4 + 2,	"p2 left"	},
	{"P2 Left Stick Right",	    BIT_DIGITAL,	DrvJoy4 + 3,	"p2 right"	},
	{"P2 Right Stick Up",		BIT_DIGITAL,	DrvJoy4 + 6,	"p4 up"		},
	{"P2 Right Stick Down",		BIT_DIGITAL,	DrvJoy4 + 7,	"p4 down"	},
	{"P2 Right Stick Left",		BIT_DIGITAL,	DrvJoy6 + 0,	"p4 left"	},
	{"P2 Right Stick Right",	BIT_DIGITAL,	DrvJoy6 + 1,	"p4 right"	},

	{"P3 Coin",					BIT_DIGITAL,	DrvJoy3 + 2,	"p3 coin"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score Reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"service3"	},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};

STDINPUTINFO(Splat)

static struct BurnInputInfo JoustInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 5,	"p1 start"	},
	{"P1 Left",					BIT_DIGITAL,	DrvJoy5 + 0,	"p1 left"	},
	{"P1 Right",				BIT_DIGITAL,	DrvJoy5 + 1,	"p1 right"	},
	{"P1 Button 1",				BIT_DIGITAL,	DrvJoy5 + 2,	"p1 fire 1"	},

	{"P2 Coin",					BIT_DIGITAL,	DrvJoy3 + 5,	"p2 coin"	},
	{"P2 Start",				BIT_DIGITAL,	DrvJoy1 + 4,	"p2 start"	},
	{"P2 Left",					BIT_DIGITAL,	DrvJoy4 + 0,	"p2 left"	},
	{"P2 Right",				BIT_DIGITAL,	DrvJoy4 + 1,	"p2 right"	},
	{"P2 Button 1",				BIT_DIGITAL,	DrvJoy4 + 2,	"p2 fire 1"	},

	{"P3 Coin",					BIT_DIGITAL,	DrvJoy3 + 2,	"p3 coin"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score Reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"service3"	},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};

STDINPUTINFO(Joust)

static struct BurnInputInfo SpdballInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy5 + 6,	"p1 start"	},
	{"P1 Up",					BIT_DIGITAL,	DrvJoy4 + 0,	"p1 up"		},
	{"P1 Down",					BIT_DIGITAL,	DrvJoy4 + 1,	"p1 down"	},
	{"P1 Left",					BIT_DIGITAL,	DrvJoy5 + 0,	"p1 left"	},
	{"P1 Right",				BIT_DIGITAL,	DrvJoy5 + 1,	"p1 right"	},
	{"P1 Button 1",				BIT_DIGITAL,	DrvJoy4 + 4,	"p1 fire 1"	},

	// analog input placeholder
	{"P1 Button 2",				BIT_DIGITAL,	DrvJoy4 + 5,	"p1 fire 2"	},
	{"P1 Button 3",				BIT_DIGITAL,	DrvJoy4 + 5,	"p1 fire 3"	},

	{"P2 Coin",					BIT_DIGITAL,	DrvJoy3 + 5,	"p2 coin"	},
	{"P2 Start",				BIT_DIGITAL,	DrvJoy5 + 7,	"p2 start"	},
	{"P2 Up",					BIT_DIGITAL,	DrvJoy4 + 2,	"p2 up"		},
	{"P2 Down",					BIT_DIGITAL,	DrvJoy4 + 3,	"p2 down"	},
	{"P2 Left",					BIT_DIGITAL,	DrvJoy5 + 2,	"p2 left"	},
	{"P2 Right",				BIT_DIGITAL,	DrvJoy5 + 3,	"p2 right"	},
	{"P2 Button 1",				BIT_DIGITAL,	DrvJoy4 + 6,	"p2 fire 1"	},

	// analog input placeholder
	{"P2 Button 2",				BIT_DIGITAL,	DrvJoy4 + 7,	"p2 fire 2"	},
	{"P2 Button 3",				BIT_DIGITAL,	DrvJoy4 + 7,	"p2 fire 3"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score Reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"service3"	},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};

STDINPUTINFO(Spdball)

static struct BurnInputInfo AlienarInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 4,	"p1 start"	},
	{"P1 Up",					BIT_DIGITAL,	DrvJoy5 + 0,	"p1 up"		},
	{"P1 Down",					BIT_DIGITAL,	DrvJoy5 + 1,	"p1 down"	},
	{"P1 Left",					BIT_DIGITAL,	DrvJoy5 + 2,	"p1 left"	},
	{"P1 Right",				BIT_DIGITAL,	DrvJoy5 + 3,	"p1 right"	},
	{"P1 Button 1",				BIT_DIGITAL,	DrvJoy5 + 6,	"p1 fire 1"	},
	{"P1 Button 2",				BIT_DIGITAL,	DrvJoy5 + 7,	"p1 fire 2"	},

	{"P2 Coin",					BIT_DIGITAL,	DrvJoy3 + 5,	"p2 coin"	},
	{"P2 Start",				BIT_DIGITAL,	DrvJoy1 + 5,	"p2 start"	},
	{"P2 Up",					BIT_DIGITAL,	DrvJoy4 + 0,	"p2 up"		},
	{"P2 Down",					BIT_DIGITAL,	DrvJoy4 + 1,	"p2 down"	},
	{"P2 Left",					BIT_DIGITAL,	DrvJoy4 + 2,	"p2 left"	},
	{"P2 Right",				BIT_DIGITAL,	DrvJoy4 + 3,	"p2 right"	},
	{"P2 Button 1",				BIT_DIGITAL,	DrvJoy4 + 6,	"p2 fire 1"	},
	{"P2 Button 2",				BIT_DIGITAL,	DrvJoy4 + 7,	"p2 fire 2"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score Reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"service3"	},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};

STDINPUTINFO(Alienar)

#define A(a, b, c, d) {a, b, (UINT8*)(c), d}
static struct BurnInputInfo SinistarInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy2 + 4,	"p1 start"	},
	{"P1 Button 1",				BIT_DIGITAL,	DrvJoy2 + 0,	"p1 fire 1"	},
	{"P1 Button 2",				BIT_DIGITAL,	DrvJoy2 + 1,	"p1 fire 2"	},

	A("P1 Stick X",             BIT_ANALOG_REL, &DrvAnalogPort0,"p1 x-axis"),
	A("P1 Stick Y",             BIT_ANALOG_REL, &DrvAnalogPort1,"p1 y-axis"),

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score Reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"service3"	},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};
#undef A

STDINPUTINFO(Sinistar)

#define A(a, b, c, d) {a, b, (UINT8*)(c), d}
static struct BurnInputInfo BlasterInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy2 + 4,	"p1 start"	},
	{"P1 Button 1",				BIT_DIGITAL,	DrvJoy2 + 1,	"p1 fire 1"	},
	{"P1 Button 2",				BIT_DIGITAL,	DrvJoy2 + 0,	"p1 fire 2"	},
	{"P1 Button 3",				BIT_DIGITAL,	DrvJoy2 + 2,	"p1 fire 3"	},

	A("P1 Stick X",             BIT_ANALOG_REL, &DrvAnalogPort0,"p1 x-axis"),
	A("P1 Stick Y",             BIT_ANALOG_REL, &DrvAnalogPort1,"p1 y-axis"),

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score Reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"service3"	},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};
#undef A

STDINPUTINFO(Blaster)

static struct BurnInputInfo LottofunInputList[] = {
	{"P1 Coin",					BIT_DIGITAL,	DrvJoy3 + 4,	"p1 coin"	},
	{"P1 Start",				BIT_DIGITAL,	DrvJoy1 + 4,	"p1 start"	},
	{"Up",						BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"Down",					BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"Left",					BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"	},
	{"Right",					BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"	},

	{"P2 Coin",					BIT_DIGITAL,	DrvJoy3 + 2,	"p2 coin"	},

	{"Reset",					BIT_DIGITAL,	&DrvReset,		"reset"		},
	{"Auto Up / Manual Down",	BIT_DIGITAL,	DrvJoy3 + 0,	"service"	},
	{"Advance",					BIT_DIGITAL,	DrvJoy3 + 1,	"service2"	},
	{"High Score Reset",		BIT_DIGITAL,	DrvJoy3 + 3,	"service3"	},
	{"Tilt",					BIT_DIGITAL,	DrvJoy3 + 6,	"tilt"		},
};

STDINPUTINFO(Lottofun)

static struct BurnDIPInfo Colony7DIPList[]=
{
	{0x0b, 0xff, 0xff, 0x00, NULL					},
	{0x0c, 0xff, 0xff, 0x00, NULL					},
	{0x0d, 0xff, 0xff, 0x01, NULL					},

	{0   , 0xfe, 0   ,    2, "Lives"				},
	{0x0d, 0x01, 0x01, 0x00, "2"					},
	{0x0d, 0x01, 0x01, 0x01, "3"					},

	{0   , 0xfe, 0   ,    4, "Bonus At"				},
	{0x0d, 0x01, 0x02, 0x00, "20k/40k"				},
	{0x0d, 0x01, 0x02, 0x02, "30k/50k"				},
	{0x0d, 0x01, 0x02, 0x00, "30k/50k"				},
	{0x0d, 0x01, 0x02, 0x02, "40k/70k"				},
};

STDDIPINFO(Colony7)

static struct BurnDIPInfo JinDIPList[]=
{
	{0x09, 0xff, 0xff, 0x00, NULL					},
	{0x0a, 0xff, 0xff, 0x60, NULL					},
	{0x0b, 0xff, 0xff, 0x00, NULL					},

	{0   , 0xfe, 0   ,    8, "Coinage"				},
	{0x0a, 0x01, 0x07, 0x07, "A 2C/1C B 1C/3C"		},
	{0x0a, 0x01, 0x07, 0x06, "A 2C/1C B 1C/1C"		},
	{0x0a, 0x01, 0x07, 0x05, "A 1C/3C B 1C/3C"		},
	{0x0a, 0x01, 0x07, 0x04, "A 1C/3C B 1C/1C"		},
	{0x0a, 0x01, 0x07, 0x03, "A 1C/2C B 1C/3C"		},
	{0x0a, 0x01, 0x07, 0x02, "A 1C/2C B 1C/1C"		},
	{0x0a, 0x01, 0x07, 0x01, "A 1C/1C B 1C/3C"		},
	{0x0a, 0x01, 0x07, 0x00, "A 1C/1C B 1C/1C"		},

	{0   , 0xfe, 0   ,    2, "Lives"				},
	{0x0a, 0x01, 0x08, 0x08, "4"					},
	{0x0a, 0x01, 0x08, 0x00, "3"					},

	{0   , 0xfe, 0   ,    4, "Level completed"		},
	{0x0a, 0x01, 0x60, 0x60, "85%"					},
	{0x0a, 0x01, 0x60, 0x40, "75%"					},
	{0x0a, 0x01, 0x60, 0x20, "65%"					},
	{0x0a, 0x01, 0x60, 0x00, "55%"					},
};

STDDIPINFO(Jin)

enum
{
	//controlbyte (0xCA00) bit definitions
	WMS_BLITTER_CONTROLBYTE_NO_EVEN = 0x80,
	WMS_BLITTER_CONTROLBYTE_NO_ODD = 0x40,
	WMS_BLITTER_CONTROLBYTE_SHIFT = 0x20,
	WMS_BLITTER_CONTROLBYTE_SOLID = 0x10,
	WMS_BLITTER_CONTROLBYTE_FOREGROUND_ONLY = 0x08,
	WMS_BLITTER_CONTROLBYTE_SLOW = 0x04, //2us blits instead of 1us
	WMS_BLITTER_CONTROLBYTE_DST_STRIDE_256 = 0x02,
	WMS_BLITTER_CONTROLBYTE_SRC_STRIDE_256 = 0x01
};

static void blit_pixel(INT32 dstaddr, INT32 srcdata, INT32 controlbyte)
{
	INT32 curpix = (dstaddr < 0xc000) ? DrvVidRAM[dstaddr] : M6809CheatRead(dstaddr);   //current pixel values at dest

	INT32 solid = DrvBlitRAM[1];
	UINT8 keepmask = 0xff;

	if((controlbyte & WMS_BLITTER_CONTROLBYTE_FOREGROUND_ONLY) && !(srcdata & 0xf0))
	{
		if(controlbyte & WMS_BLITTER_CONTROLBYTE_NO_EVEN)
			keepmask &= 0x0f;
	}
	else
	{
		if(!(controlbyte & WMS_BLITTER_CONTROLBYTE_NO_EVEN))
			keepmask &= 0x0f;
	}

	if((controlbyte & WMS_BLITTER_CONTROLBYTE_FOREGROUND_ONLY) && !(srcdata & 0x0f))
	{
		if(controlbyte & WMS_BLITTER_CONTROLBYTE_NO_ODD)
			keepmask &= 0xf0;
	}
	else
	{
		if(!(controlbyte & WMS_BLITTER_CONTROLBYTE_NO_ODD))
			keepmask &= 0xf0;
	}

	curpix &= keepmask;
	if(controlbyte & WMS_BLITTER_CONTROLBYTE_SOLID)
		curpix |= (solid & ~keepmask);
	else
		curpix |= (srcdata & ~keepmask);

	if (!blitter_window_enable || dstaddr < blitter_clip_address || dstaddr >= 0xc000)
	{
		extern void M6809WriteByte(UINT16 Address, UINT8 Data);
		M6809WriteByte(dstaddr, curpix);
	}
}

static INT32 blitter_core(UINT16 sstart, UINT16 dstart, UINT8 w, UINT8 h, UINT8 controlbyte)
{
	INT32 source, sxadv, syadv;
	INT32 dest, dxadv, dyadv;
	INT32 x, y;
	INT32 accesses = 0;
	UINT8 *remap_ptr = blitter_remap + blitter_remap_index * 256;

	sxadv = (controlbyte & WMS_BLITTER_CONTROLBYTE_SRC_STRIDE_256) ? 0x100 : 1;
	syadv = (controlbyte & WMS_BLITTER_CONTROLBYTE_SRC_STRIDE_256) ? 1 : w;
	dxadv = (controlbyte & WMS_BLITTER_CONTROLBYTE_DST_STRIDE_256) ? 0x100 : 1;
	dyadv = (controlbyte & WMS_BLITTER_CONTROLBYTE_DST_STRIDE_256) ? 1 : w;

	INT32 pixdata=0;

	for (y = 0; y < h; y++)
	{
		source = sstart & 0xffff;
		dest = dstart & 0xffff;

		for (x = 0; x < w; x++)
		{
			if (!(controlbyte & WMS_BLITTER_CONTROLBYTE_SHIFT)) //no shift
			{
				blit_pixel(dest, remap_ptr[M6809CheatRead(source)], controlbyte);
			}
			else
			{   //shift one pixel right
				pixdata = (pixdata << 8) | remap_ptr[M6809CheatRead(source)];
				blit_pixel(dest, (pixdata >> 4) & 0xff, controlbyte);
			}
			accesses += 2;

			source = (source + sxadv) & 0xffff;
			dest   = (dest + dxadv) & 0xffff;
		}

		if (controlbyte & WMS_BLITTER_CONTROLBYTE_DST_STRIDE_256)
			dstart = (dstart & 0xff00) | ((dstart + dyadv) & 0xff);
		else
			dstart += dyadv;

		if (controlbyte & WMS_BLITTER_CONTROLBYTE_SRC_STRIDE_256)
			sstart = (sstart & 0xff00) | ((sstart + syadv) & 0xff);
		else
			sstart += syadv;
	}
	return accesses;
}

static void williams_blitter_write(INT32 offset, UINT8 data)
{
	offset &= 7;

	INT32 sstart, dstart, w, h, accesses;
	INT32 estimated_clocks_at_4MHz;

	DrvBlitRAM[offset] = data;

	if (offset != 0)
		return;

	sstart = (DrvBlitRAM[2] << 8) + DrvBlitRAM[3];
	dstart = (DrvBlitRAM[4] << 8) + DrvBlitRAM[5];

	w = DrvBlitRAM[6] ^ blitter_xor;
	h = DrvBlitRAM[7] ^ blitter_xor;

	if (w == 0) w = 1;
	if (h == 0) h = 1;

	//bprintf(0, _T("blit: sstart %X  dstart %X  w %X  h %X  data %X.\n"), sstart, dstart, w, h, data);
	accesses = blitter_core(sstart, dstart, w, h, data);

	if(data & WMS_BLITTER_CONTROLBYTE_SLOW)
	{
		estimated_clocks_at_4MHz = 4 + 4 * (accesses + 2);
	}
	else
	{
		estimated_clocks_at_4MHz = 4 + 2 * (accesses + 3);
	}
	
//	m6809_eat_cycles(-((estimated_clocks_at_4MHz + 3) / 4));
}

static void defender_bank_write(UINT16 address, UINT8 data)
{
	//bprintf (0, _T("BW: %4.4x %2.2x\n"), address, data);
	
	if (address == 0x03ff) {
		if (data == 0x39) {
			BurnWatchogWrite();
			bprintf(0, _T("Watchdog Write.    **\n"));
		}
		return;
	}
	
	if ((address & 0xfc10) == 0x0000) {
		DrvPalRAM[address & 0x0f] = data;
		return;
	}

	if ((address & 0xfc10) == 0x0010) {
		cocktail = data & 0x01;
		return;
	}

	if ((address & 0xfc00) == 0x0400) {
		DrvNVRAM[address & 0xff] = data | 0xf0;
		return;
	}

	if ((address & 0xfc1c) == 0x0c00) {
		pia_write(1, address & 0x3, data);
		return;
	}

	if ((address & 0xfc1c) == 0x0c04) {
		pia_write(0, address & 0x3, data);
		return;
	}

	bprintf (0, _T("BW: %4.4x %2.2x\n"), address, data);

	if (address >= 0xa000) return; // nop
}

static UINT8 defender_bank_read(UINT16 address)
{
//	bprintf (0, _T("BR: %4.4x\n"), address);
	
	if ((address & 0xfc00) == 0x0400) {
		return DrvNVRAM[address & 0xff];
	}

	if ((address & 0xfc00) == 0x0800) {
		return (scanline < 0x100) ? (scanline & 0xfc) : 0xfc;
	}

	if ((address & 0xfc1c) == 0x0c00) {
		return pia_read(1, address & 3);
	}
	
	if ((address & 0xfc1c) == 0x0c04) {
		return pia_read(0, address & 3);
	}

	if (address >= 0x1000 && address <= 0x9fff) {
		return DrvM6809ROM0[0xf000 + address];
	}

	if (address >= 0xa000) {
		return 0;
	}

	bprintf (0, _T("BR: %4.4x\n"), address);

	return 0;
}

static void bankswitch()
{
	if (bankselect >= 1 && bankselect <= 9) {
		M6809MapMemory(DrvM6809ROM0 + 0x10000 + (bankselect - 1) * 0x1000, 0xc000, 0xcfff, MAP_ROM);
	} else {
		M6809UnmapMemory(0xc000, 0xcfff, MAP_RAM);
	}
}

static void defender_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf000) == 0xc000) {
		defender_bank_write((address & 0xfff) + (bankselect * 0x1000), data);
		return;
	}

	if ((address & 0xf000) == 0xd000) {
		bankselect = data & 0xf;
		bankswitch();

		// mayday
		if (DrvM6809ROM0[0xf7b5] == 0xff && DrvM6809ROM0[0xf7b6] == 0xff) {
		   // bprintf (0, _T("Mayday prot: %3.3x, %2.2x\n"), address & 0xfff, data);
			//if (data == 0x01) {
			//	memset (DrvM6809ROM0 + 0xf7ad, 0, 6);
			//}
			// else {
			//	static const UINT8 test_data[8] = {
			//		0x31, 0x62, 0xE2, 0xA1, 0x91, 0x09, 0xC0, 0x00
			//	};
			//	memcpy (DrvM6809ROM0 + 0xf7ad, test_data, 2);
			//}
		}
		return;
	}
}

static UINT8 defender_main_read(UINT16 address)
{
	if (address >= 0x0000 && address <= 0xbfff) { // Mayday prot
		if (mayday && address >= 0xa190 && address <= 0xa191) { // Mayday prot
			bprintf(0, _T("read mayday prot: %X.\n"), address);
			return DrvVidRAM[address + 3];
		}
		return DrvVidRAM[address];
	}

	if ((address & 0xf000) == 0xc000) {
		return defender_bank_read((address & 0xfff) + (bankselect * 0x1000));
	}

	return 0;
}

static void williams_bank()
{
	if (vram_select == 0)
	{
		M6809MapMemory(DrvVidRAM,				0x0000, 0x8fff, MAP_RAM);
	}
	else
	{	
		M6809MapMemory(DrvM6809ROM0 + 0x10000,	0x0000, 0x8fff, MAP_ROM);
	}
}

static void williams_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xfc00) == 0xc000) {
		DrvPalRAM[address & 0xf] = data;
		return;
	}

	if ((address & 0xfc00) == 0xcc00) {
		DrvNVRAM[address & 0x3ff] = data | 0xf0;
		return;
	}

	if ((address & 0xff00) == 0xc900) {
		vram_select = data & 1;
		cocktail = data & 2;
		if (blitter_clip_address == 0x7400) blitter_window_enable = data & 4; // sinistar
		williams_bank();
		return;
	}

	if ((address & 0xff0c) == 0xc804) {
		pia_write(0, address & 0x3, data);
		return;
	}

	if ((address & 0xff0c) == 0xc808) { // spdball
		pia_write(3, address & 0x3, data);
		return;
	}

	if ((address & 0xff0c) == 0xc80c) {
		pia_write(1, address & 0x3, data);
		return;
	}

	if ((address & 0xff00) == 0xca00) {
		williams_blitter_write(address, data);
		return;
	}

	switch (address)
	{
		case 0xcbff:
			if (data == 0x39) {
				BurnWatchogWrite();
			}
		return;
	}

	// sinistar debug derp spew writes @ 0xe000 - 0xffff
	if ((address & 0xe000) != 0xe000) bprintf (0, _T("MW: %4.4x, %2.2x\n"), address, data);
}

static UINT8 williams_main_read(UINT16 address)
{
	if ((address & 0xfffc) == 0xc800) {
		return 0; // spdball analog input read
	}

	if ((address & 0xff0c) == 0xc804) {
		return pia_read(0, address & 3);
	}

	if ((address & 0xff0c) == 0xc808) { // spdball
		return pia_read(3, address & 3);
	}

	if ((address & 0xff0c) == 0xc80c) {
		return pia_read(1, address & 3);
	}

	if ((address & 0xff00) == 0xca00) {
		return DrvBlitRAM[address & 7];
	}

	if ((address & 0xff00) == 0xcb00) {
		return (scanline < 0x100) ? (scanline & 0xfc) : 0xfc;
	}

	if ((address & 0xfc00) == 0xc000) { // palette read, maybe? (sinistar)
		return DrvPalRAM[address & 0xf];
	}

	if (address == 0xc900) return 0; // NOP
	if (address == 0xc940) return 0; // NOP
	//if (address == 0xc000) return 0; // NOP (sinistar)?

	bprintf (0, _T("MR: %4.4x\n"), address);


	return 0;
}

static void blaster_bankswitch()
{
	if (vram_select == 0)
	{
		M6809MapMemory(DrvVidRAM,		0x0000, 0x8fff, MAP_RAM);
	}
	else
	{
		M6809MapMemory(DrvM6809ROM0 + 0x18000 + rom_bank * 0x4000, 0x0000, 0x3fff, MAP_ROM);
		M6809MapMemory(DrvM6809ROM0 + 0x10000, 0x4000, 0x8fff, MAP_ROM);
	}
}

static void blaster_main_write(UINT16 address, UINT8 data)
{
		if ((address & 0xffc0) == 0xc900) {
			vram_select = data & 0x01;
			cocktail = data & 0x02;
			blitter_window_enable = data & 0x04;
			blaster_bankswitch();
			return;
		}
		
		if ((address & 0xffc0) == 0xc940) {
			blitter_remap_index = data;
			return;
		}
		
		if ((address & 0xffc0) == 0xc980) {
			rom_bank = data & 0x0f;
			blaster_bankswitch();
			return;
		}
		
		if ((address & 0xffc0) == 0xc9c0) {
			blaster_video_control = data;
			return;
		}

		williams_main_write(address, data);
}

static void defender_sound_write(UINT16 address, UINT8 data)
{
	if ((address & 0x7ffc) == 0x0400) {
		pia_write(2, address & 0x3, data);
		return;
	}
}

static UINT8 defender_sound_read(UINT16 address)
{
	if ((address & 0x7ffc) == 0x0400) {
		return pia_read(2, address & 3);
	}

	return 0;
}

static void blaster_sound_write(UINT16 address, UINT8 data)
{
	if ((address & 0x7ffc) == 0x0400) {
		pia_write(4, address & 0x3, data);
		return;
	}
}

static UINT8 blaster_sound_read(UINT16 address)
{
	if ((address & 0x7ffc) == 0x0400) {
		return pia_read(4, address & 3);
	}

	return 0;
}

static UINT8 pia0_in_a(UINT16 )
{
	return DrvInputs[0];
}

static UINT8 pia0_in_b(UINT16 )
{
	return DrvInputs[1];
}

static UINT8 pia1_in_a(UINT16 )
{
	return DrvInputs[2];
}

static void pia0_muxed_out_b2(UINT16 , UINT8 data)
{
	port_select = data;
}

static UINT8 pia0_muxed_joust_in_a(UINT16 )
{
	return (DrvInputs[0] & 0x30) | ((port_select == 0) ? (DrvInputs[3] & ~0x30) : (DrvInputs[4] & ~0x30));
}

static UINT8 pia0_muxed_joust_in_b(UINT16 )
{
	return (DrvInputs[1] & 0xfc) | ((port_select == 0) ? (DrvInputs[5] & 0x3) : (DrvInputs[6] & 0x3));
}

static UINT8 pia0_49way_in_a(UINT16 )
{
	static const UINT8 translate49[7] = { 0x0, 0x4, 0x6, 0x7, 0xb, 0x9, 0x8 };
	INT16 x = ProcessAnalog(DrvAnalogPort0, 0, 1, 0x00, 0x6f);
	INT16 y = ProcessAnalog(DrvAnalogPort1, 1, 1, 0x00, 0x6f);
	return (translate49[x >> 4] << 4) | translate49[y >> 4];
}

static void sync_sound(INT32 num)
{
	INT32 cyc = (INT32)(((double)((double)M6809TotalCycles() * 894886) / 1000000)+0.5);
	INT32 todo = cyc - M6800TotalCycles();
	//bprintf(0, _T("m6809 cyc: %d  m6800 cyc: %d.    cyc: %d  todo %d.\n"), M6809TotalCycles(), M6800TotalCycles(), cyc, todo);
	// Adding in a couple cycles to prevent lost soundcommands.  This is OK since we lose a few cycles/frame due to division & rounding loss.
	if (todo < 1) todo = 15;
	nCyclesDone[num + 1] += M6800Run(todo + 10);
}

static void pia1_out_b(UINT16 , UINT8 data)
{
	if (!blaster) { // defender, williams HW
		M6800Open(0);
		sync_sound(0);
		data |= 0xc0;
		pia_set_input_b(2, data);
		pia_set_input_cb1(2, (data == 0xff) ? 0 : 1);
		M6800Close();
	} else {        // Blaster HW
		UINT8 l_data = data | 0x80;
		UINT8 r_data = (data >> 1 & 0x40) | (data & 0x3f) | 0x80;

		M6800Open(0);
		//bprintf(0, _T("0) "));
		sync_sound(0);
		pia_set_input_b(2, l_data);
		pia_set_input_cb1(2, (l_data == 0xff) ? 0 : 1);
		M6800Close();

		M6800Open(1);
		//bprintf(0, _T("1) "));
		sync_sound(1);
		pia_set_input_b(4, r_data);
		pia_set_input_cb1(4, (r_data == 0xff) ? 0 : 1);
		M6800Close();
	}
}

static void pia2_out_b(UINT16 , UINT8 data)
{
	DACWrite(0, data);
}

static void pia2_out_b2(UINT16 , UINT8 data)
{
	DACWrite(1, data);
}

static void hc55516_digit_out(UINT16 , UINT8 data)
{
	hc55516_digit_w(data);
}

static void hc55516_clock_out(UINT16 , UINT8 data)
{
	hc55516_clock_w(data);
}

static void pia1_main_irq(INT32 state)
{
	M6809SetIRQLine(M6809_IRQ_LINE, state ? CPU_IRQSTATUS_ACK : CPU_IRQSTATUS_NONE);
}

static void pia2_sound_irq(INT32 state)
{
	if (state)
		M6800SetIRQLine(M6800_IRQ_LINE, CPU_IRQSTATUS_HOLD);
	// blaster 2nd soundcpu doesn't like this @ boot
	//M6800SetIRQLine(M6800_IRQ_LINE, state ? CPU_IRQSTATUS_ACK : CPU_IRQSTATUS_NONE);
}

static pia6821_interface pia_0 = {
	pia0_in_a, pia0_in_b,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL
};

static pia6821_interface pia_muxed_joust_0 = {
	pia0_muxed_joust_in_a, pia0_muxed_joust_in_b,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, pia0_muxed_out_b2,
	NULL, NULL
};

static pia6821_interface pia_49way_0 = {
	pia0_49way_in_a, pia0_in_b,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL
};

static pia6821_interface pia_1 = {
	pia1_in_a, NULL,
	NULL, NULL, NULL, NULL,
	NULL, pia1_out_b, NULL, NULL,
	pia1_main_irq, pia1_main_irq
};

static pia6821_interface pia_2 = {
	NULL, NULL,
	NULL, NULL, NULL, NULL,
	pia2_out_b, NULL, NULL, NULL,
	pia2_sound_irq, pia2_sound_irq
};

static pia6821_interface pia_2_2 = { // blaster 2nd soundchip
	NULL, NULL,
	NULL, NULL, NULL, NULL,
	pia2_out_b2, NULL, NULL, NULL,
	pia2_sound_irq, pia2_sound_irq
};

static pia6821_interface pia_2_sinistar = {
	NULL, NULL,
	NULL, NULL, NULL, NULL,
	pia2_out_b, NULL, hc55516_digit_out, hc55516_clock_out,
	pia2_sound_irq, pia2_sound_irq
};

static UINT8 pia3_in_a(UINT16 )
{
	return DrvInputs[3];
}

static UINT8 pia3_in_b(UINT16 )
{
	return DrvInputs[4];
}

static pia6821_interface pia_3 = {
	pia3_in_a, pia3_in_b,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL
};

static INT32 DrvDoReset(INT32 clear_mem)
{
	if (clear_mem) {
		memset (AllRam, 0, RamEnd - AllRam);
	}

	M6809Open(0);
	M6809Reset();
	M6809Close();

	M6800Open(0);
	M6800Reset();
	M6800Close();

	if (blaster) {
		M6800Open(1);
		M6800Reset();
		M6800Close();
	}

	pia_reset();

	BurnWatchdogReset();

	DACReset();

	if (uses_hc55516)
		hc55516_reset();

	cocktail = 0;
	bankselect = 0;
	vram_select = 0;
	port_select = 0;
	rom_bank = 0;
	blaster_video_control = 0;
	blaster_color0 = 0;

	dac_lastin_r = 0;
	dac_lastout_r = 0;
	dac_lastin_l = 0;
	dac_lastout_l = 0;

	nExtraCycles[0] = nExtraCycles[1] = nExtraCycles[2] = 0;

	return 0;
}

static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	DrvM6809ROM0		= Next; Next += 0x050000;
	DrvM6800ROM0		= Next; Next += 0x010000;
	DrvM6800ROM1		= Next; Next += 0x010000;
	
	DrvGfxROM			= Next; Next += 0x018000;
	
	DrvColPROM			= Next; Next += 0x001000;

	Palette				= (UINT32*)Next; Next += 0x0100 * sizeof(UINT32);
	DrvPalette			= (UINT32*)Next; Next += (0x0010 + 0x0100) * sizeof(UINT32);

	DrvNVRAM			= Next; Next += 0x000400;

	blitter_remap		= Next; Next += 0x010000;
	
	AllRam				= Next;

	DrvM6809RAM0		= Next; Next += 0x004000;
	DrvM6800RAM0		= Next; Next += 0x000100;
	DrvM6800RAM1		= Next; Next += 0x000100;
	DrvVidRAM			= Next; Next += 0x00c000;
	DrvPalRAM			= Next; Next += 0x000010;
	DrvBlitRAM			= Next; Next += 0x000008;

	RamEnd				= Next;

	MemEnd				= Next;

	return 0;
}

static INT32 DrvRomLoad(INT32 type) // 1-defender, 2-mysticm, 3-tshoot, 4-sinistar, 0-blaster
{
	char* pRomName;
	struct BurnRomInfo ri;
	UINT8 *mLoad = DrvM6809ROM0 + 0xd000;
	UINT8 *cLoad = DrvColPROM;
	UINT8 *gLoad = DrvGfxROM;

	if (type == 4) mLoad += 0x1000;
	
	for (INT32 i = 0; !BurnDrvGetRomName(&pRomName, i, 0); i++)
	{
		BurnDrvGetRomInfo(&ri, i);

		if ((ri.nType & 7) == 1) {
			INT32 offset = mLoad - DrvM6809ROM0;

			// blaster
			if (ri.nLen == 0x4000 && offset == 0x15000) mLoad += 0x3000; // gap
			if (type == 2) {
				if (offset == 0x18000) mLoad += 0x8000;
				if (offset == 0x24000) mLoad += 0xc000;
				if (offset == 0x38000) mLoad += 0x8000;
			}
			if (type == 3) {
				if (offset == 0x18000) mLoad += 0x8000;
				if (offset == 0x26000) mLoad += 0xa000;
				if (offset == 0x38000) mLoad += 0x8000;
			}

			if (BurnLoadRom(mLoad, i, 1)) return 1;
			mLoad += ri.nLen;

			// defender
			if (type == 1 && (offset + ri.nLen) == 0x13000) mLoad += 0x3000; // gap
			continue;
		}

		if ((ri.nType & 7) == 2) {
			memmove (DrvM6800ROM0, DrvM6800ROM0 + ri.nLen, 0x10000 - ri.nLen); // move back
			if (BurnLoadRom(DrvM6800ROM0 + 0x10000 - ri.nLen, i, 1)) return 1;
			continue;
		}

		if ((ri.nType & 7) == 3) {
			memmove (DrvM6800ROM1, DrvM6800ROM1 + ri.nLen, 0x10000 - ri.nLen); // move back
			if (BurnLoadRom(DrvM6800ROM1 + 0x10000 - ri.nLen, i, 1)) return 1;
			continue;
		}

		if ((ri.nType & 7) == 4) {
			if (BurnLoadRom(cLoad, i, 1)) return 1;
			cLoad += ri.nLen;
			uses_colprom = 1;
			continue;
		}

		if ((ri.nType & 7) == 5) {
			if (BurnLoadRom(gLoad, i, 1)) return 1;
			gLoad += ri.nLen;
			continue;
		}
	}

	// colony
	if ((mLoad - DrvM6809ROM0) == 0x12800) memcpy (DrvM6809ROM0 + 0x12800, DrvM6809ROM0 + 0x12000, 0x0800);

	return 0;
}

static void blitter_init(INT32 blitter_config, UINT8 *prom)
{
	static UINT8 dummy_table[16] = { STEP16(0,1) };

	if (prom) bprintf(0, _T(" ** Using DrvColPROM.\n"));

	blitter_window_enable = 0;
	blitter_xor = (blitter_config == 1) ? 4 : 0;
	blitter_remap_index = 0;

	for (INT32 i = 0; i < 256; i++)
	{
		UINT8 *table = prom ? (prom + (i & 0x7f) * 16) : dummy_table;

		for (INT32 j = 0; j < 256; j++)
			blitter_remap[i * 256 + j] = (table[j >> 4] << 4) | table[j & 0x0f];
	}
}

static INT32 DrvInit(INT32 maptype, INT32 loadtype, INT32 x_adjust, INT32 blitter_config, INT32 blitter_clip_addr)
{
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	{
		if (DrvRomLoad(loadtype)) return 1;
	}

	// sound HW
	M6800Init(0);
	M6800Open(0);
	M6800MapMemory(DrvM6800RAM0,				0x0000, 0x00ff, MAP_RAM);
	M6800MapMemory(DrvM6800ROM0 + 0xb000,		0xb000, 0xffff, MAP_ROM);
	M6800SetWriteHandler(defender_sound_write);
	M6800SetReadHandler(defender_sound_read);
	M6800Close();

	if (maptype == 2) // blaster
	{
		M6809Init(0);
		M6809Open(0);
		M6809MapMemory(DrvVidRAM,				0x0000, 0xbfff, MAP_RAM); // banked
		M6809MapMemory(DrvNVRAM,				0xcc00, 0xcfff, MAP_ROM);
		M6809MapMemory(DrvM6809ROM0 + 0xd000,	0xd000, 0xffff, MAP_ROM);
		M6809SetWriteHandler(blaster_main_write);
		M6809SetReadHandler(williams_main_read);
		M6809Close();

		M6800Init(1);
		M6800Open(1);
		M6800MapMemory(DrvM6800RAM1,				0x0000, 0x00ff, MAP_RAM);
		M6800MapMemory(DrvM6800ROM1 + 0xb000,		0xb000, 0xffff, MAP_ROM);
		M6800SetWriteHandler(blaster_sound_write);
		M6800SetReadHandler(blaster_sound_read);
		M6800Close();
	}
	else if (maptype == 1) // williams
	{
		M6809Init(0);
		M6809Open(0);
		M6809MapMemory(DrvVidRAM,				0x0000, 0xbfff, MAP_RAM); // banked
		M6809MapMemory(DrvNVRAM,				0xcc00, 0xcfff, MAP_ROM); // handler
		M6809MapMemory(DrvM6809ROM0 + 0xd000,	0xd000, 0xffff, MAP_ROM);
		M6809SetWriteHandler(williams_main_write);
		M6809SetReadHandler(williams_main_read);
		M6809Close();
	}
	else if (maptype == 0) // defender
	{
		M6809Init(0);
		M6809Open(0);
		if (mayday) {
			M6809MapMemory(DrvVidRAM,				0x0000, 0xbfff, MAP_WRITE); // read -> handler
		} else {
			M6809MapMemory(DrvVidRAM,				0x0000, 0xbfff, MAP_RAM);
		}
		M6809MapMemory(DrvM6809ROM0 + 0xd000,	0xd000, 0xffff, MAP_ROM);
		M6809SetWriteHandler(defender_main_write);
		M6809SetReadHandler(defender_main_read);
		M6809Close();
	}

	pia_init();
	pia_config(0, 0, &pia_0);
	pia_config(1, 0, &pia_1);
	pia_config(2, 0, &pia_2);
	pia_config(3, 0, &pia_3);

	BurnWatchdogInit(DrvDoReset, 180);

	DACInit(0, 0, 0, M6800TotalCycles, 894886);
	DACSetRoute(0, 0.35, BURN_SND_ROUTE_BOTH);

	if (maptype == 2) // blaster, pia config & l+r dac
	{
		pia_init();
		pia_config(0, 0, &pia_49way_0);
		pia_config(1, 0, &pia_1);
		pia_config(2, 0, &pia_2);
		pia_config(3, 0, &pia_3);
		pia_config(4, 0, &pia_2_2); // 2nd soundboard

		DACSetRoute(0, 0.35, BURN_SND_ROUTE_LEFT);

		DACInit(1, 0, 0, M6800TotalCycles, 894886);
		DACSetRoute(1, 0.35, BURN_SND_ROUTE_RIGHT);
	}

	blitter_clip_address = blitter_clip_addr;
	blitter_init(blitter_config, (uses_colprom) ? DrvColPROM : NULL);
	
	GenericTilesInit();

	screen_x_adjust = x_adjust;

	DrvDoReset(1);

	return 0;
}

static INT32 DrvExit()
{
	GenericTilesExit();

	M6809Exit();
	M6800Exit();
	pia_exit();

	DACExit();

	if (uses_hc55516)
		hc55516_exit();

	BurnFree(AllMem);

	memset (DrvDips, 0, 3);

	mayday = 0;
	splat = 0;
	blaster = 0;

	uses_hc55516 = 0;
	uses_colprom = 0;

	pStartDraw = NULL;
	pDrawScanline = NULL;

	return 0;
}

static void DrvPaletteInit()
{
	for (INT32 i = 0; i < 0x100; i++)
	{
		INT32 bit0 = (i & 0x01) >> 0;
		INT32 bit1 = (i & 0x02) >> 1;
		INT32 bit2 = (i & 0x04) >> 2;

		UINT8 r = ((bit2 * 1200 + bit1 * 560 + bit0 * 330) * 255) / 2090;

		bit0 = (i & 0x08) >> 3;
		bit1 = (i & 0x10) >> 4;
		bit2 = (i & 0x20) >> 5;

		UINT8 g = ((bit2 * 1200 + bit1 * 560 + bit0 * 330) * 255) / 2090;

		bit0 = (i & 0x40) >> 6;
		bit1 = (i & 0x80) >> 7;

		UINT8 b = ((bit1 * 560 + bit0 * 330) * 255) / 890;

		Palette[i] = BurnHighCol(r,g,b,0);
		DrvPalette[i + 0x10] = Palette[i];
	}
}

static void DrvPaletteUpdate()
{
	for (INT32 i = 0; i < 16; i++) {
		DrvPalette[i] = Palette[DrvPalRAM[i]];
	}
}

static void draw_bitmap(INT32 starty, INT32 endy)
{
	for (INT32 y = starty; y < endy; y++)
	{
		UINT16 *dst = pTransDraw + (y * nScreenWidth);
		UINT8 *src = DrvVidRAM + y + 7;

		if (y >= 240) return;

		for (INT32 x = 0; x < nScreenWidth; x += 2)
		{
			UINT8 pix = src[((x + screen_x_adjust) / 2) * 256];

			dst[x + 0] = pix >> 4;
			dst[x + 1] = pix & 0x0f;
		}
	}
}

static void blaster_draw_bitmap(INT32 starty, INT32 endy)
{
	const UINT8 YOFFS = 7;
	UINT8 *palette_control = DrvVidRAM + 0xbb00 + YOFFS;
	UINT8 *scanline_control = DrvVidRAM + 0xbc00 + YOFFS;

	if (starty == 0 || !(blaster_video_control & 1))
		blaster_color0 = 0x10 + (palette_control[0 - YOFFS] ^ 0xff);

	for (INT32 y = starty; y < endy; y++)
	{
		INT32 erase_behind = blaster_video_control & scanline_control[y] & 2;
		UINT8 *source = DrvVidRAM + y + YOFFS;
		UINT16 *dest = pTransDraw + (y * nScreenWidth);

		if (y >= 240) return;

		if (blaster_video_control & scanline_control[y] & 1)
			blaster_color0 = 0x10 + (palette_control[y] ^ 0xff);

		for (INT32 x = 0; x < nScreenWidth; x += 2)
		{
			INT32 pix = source[(x/2) * 256];

			if (erase_behind)
				source[(x/2) * 256] = 0;

			dest[x+0] = (pix & 0xf0) ? (pix >> 4) : blaster_color0;
			dest[x+1] = (pix & 0x0f) ? (pix & 0x0f) : blaster_color0;
		}
	}
}

static void DrvDrawBegin()
{
	if (DrvRecalc) {
		DrvPaletteInit();
		DrvRecalc = 0;
	}

	lastline = 0;

	DrvPaletteUpdate();
}

static void DrvDrawLine()
{
	if (scanline > nScreenHeight || !pBurnDraw) return;

	DrvPaletteUpdate();

	draw_bitmap(lastline, scanline);

	lastline = scanline;
}

static void BlasterDrawLine()
{
	if (scanline > nScreenHeight || !pBurnDraw) return;

	DrvPaletteUpdate();

	blaster_draw_bitmap(lastline, scanline);

	lastline = scanline;
}

static void DrvDrawEnd()
{
	if (!pBurnDraw) return;
	BurnTransferCopy(DrvPalette);
}

static INT32 DrvDraw()
{
	if (DrvRecalc) {
		DrvPaletteInit();
		DrvRecalc = 0;
	}

	DrvPaletteUpdate();

	draw_bitmap(0, nScreenHeight);

	BurnTransferCopy(DrvPalette);

	return 0;
}

static INT32 BlasterDraw()
{
	if (DrvRecalc) {
		DrvPaletteInit();
		DrvRecalc = 0;
	}

	DrvPaletteUpdate();

	blaster_draw_bitmap(0, nScreenHeight);

	BurnTransferCopy(DrvPalette);

	return 0;
}

static void dcfilter_dac()
{
	for (INT32 i = 0; i < nBurnSoundLen; i++) {
		INT16 r = pBurnSoundOut[i*2+0];
		INT16 l = pBurnSoundOut[i*2+1];

		INT16 outr = r - dac_lastin_r + 0.995 * dac_lastout_r;
		INT16 outl = l - dac_lastin_l + 0.995 * dac_lastout_l;

		dac_lastin_r = r;
		dac_lastout_r = outr;
		dac_lastin_l = l;
		dac_lastout_l = outl;
		pBurnSoundOut[i*2+0] = outr;
		pBurnSoundOut[i*2+1] = outl;
	}
}

static INT32 DrvFrame()
{
	BurnWatchdogUpdate();
	
	if (DrvReset) {
		DrvDoReset(1);
	}

	M6809NewFrame();
	M6800NewFrame();

	{
		DrvInputs[0] = DrvDips[0];
		DrvInputs[1] = DrvDips[1];
		DrvInputs[2] = DrvDips[2];

		memset (DrvInputs + 3, 0, 7-3);

		for (INT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
			DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
			DrvInputs[3] ^= (DrvJoy4[i] & 1) << i;
			DrvInputs[4] ^= (DrvJoy5[i] & 1) << i;
			DrvInputs[5] ^= (DrvJoy6[i] & 1) << i;
			DrvInputs[6] ^= (DrvJoy7[i] & 1) << i;
		}
	}

	INT32 nInterleave = 256;
	INT32 nCyclesTotal[3] = { 1000000 / 60, 894886 / 60, 894886 / 60 };
	//INT32 nCyclesDone[3] = { nExtraCycles[0], nExtraCycles[1], nExtraCycles[2] };
	nCyclesDone[0] = nExtraCycles[0]; nCyclesDone[1] = nExtraCycles[1]; nCyclesDone[2] = nExtraCycles[2];

	M6809Open(0);

	if (pStartDraw) pStartDraw();

	for (INT32 i = 0; i < nInterleave; i++)
	{
		scanline = i;

		nCyclesDone[0] += M6809Run(((i + 1) * nCyclesTotal[0] / nInterleave) - nCyclesDone[0]);

		if (scanline % 8 == 0) {
			pia_set_input_cb1(1, scanline & 0x20);
			if (pDrawScanline) pDrawScanline();
		}

		if (scanline == 0 || scanline == 240)
			pia_set_input_ca1(1, scanline >= 240 ? 1 : 0);

		M6800Open(0);
		nCyclesDone[1] += M6800Run(((i + 1) * nCyclesTotal[1] / nInterleave) - nCyclesDone[1]);
		M6800Close();

		if (blaster) {
			M6800Open(1);
			nCyclesDone[2] += M6800Run(((i + 1) * nCyclesTotal[2] / nInterleave) - nCyclesDone[2]);
			M6800Close();
		}
	}

	if (pBurnSoundOut) {
		M6800Open(0);
		DACUpdate(pBurnSoundOut, nBurnSoundLen);
		dcfilter_dac();
		if (uses_hc55516)
			hc55516_update(pBurnSoundOut, nBurnSoundLen);
		M6800Close();
	}

	M6809Close();

	nExtraCycles[0] = nCyclesDone[0] - nCyclesTotal[0];
	nExtraCycles[1] = nCyclesDone[1] - nCyclesTotal[1];
	nExtraCycles[2] = nCyclesDone[2] - nCyclesTotal[2];
	
	if (pBurnDraw) {
		if (pStartDraw)
			DrvDrawEnd();
		else
			BurnDrvRedraw();
	}

	return 0;
}

static INT32 DrvScan(INT32 nAction, INT32 *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {
		*pnMin = 0x029702;
	}

	if (nAction & ACB_VOLATILE) {
		memset(&ba, 0, sizeof(ba));

		ba.Data	  = AllRam;
		ba.nLen	  = RamEnd - AllRam;
		ba.szName = "All Ram";
		BurnAcb(&ba);

		M6809Scan(nAction);
		M6800Scan(nAction);

		DACScan(nAction, pnMin);
		if (uses_hc55516)
			hc55516_scan(nAction, pnMin);

		SCAN_VAR(cocktail);
		SCAN_VAR(bankselect);
		SCAN_VAR(vram_select);
		SCAN_VAR(rom_bank);
		SCAN_VAR(blaster_video_control);
		SCAN_VAR(blaster_color0);

		SCAN_VAR(nExtraCycles);
	}

	if (nAction & ACB_NVRAM) {
		ba.Data		= DrvNVRAM;
		ba.nLen		= 0x00400;
		ba.nAddress	= 0;
		ba.szName	= "NVRAM";
		BurnAcb(&ba);
	}

	return 0;
}


// Defender (Red label)

static struct BurnRomInfo defenderRomDesc[] = {
	{ "defend.1",			0x0800, 0xc3e52d7e, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "defend.4",			0x0800, 0x9a72348b, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "defend.2",			0x1000, 0x89b75984, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "defend.3",			0x1000, 0x94f51e9b, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "defend.9",			0x0800, 0x6870e8a5, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "defend.12",			0x0800, 0xf1f88938, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "defend.8",			0x0800, 0xb649e306, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "defend.11",			0x0800, 0x9deaf6d9, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "defend.7",			0x0800, 0x339e092e, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "defend.10",			0x0800, 0xa543b167, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "defend.6",			0x0800, 0x65f4efd1, 1 | BRF_PRG | BRF_ESS }, // 10

	{ "defend.snd",			0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, // 11 M6808 Code

	{ "decoder.2",			0x0200, 0x8dd98da5, 0 | BRF_OPT },           // 12 Address Decoder
	{ "decoder.3",			0x0200, 0xc3f45f70, 0 | BRF_OPT },           // 13
};

STD_ROM_PICK(defender)
STD_ROM_FN(defender)

static INT32 DefenderInit()
{
	return DrvInit(0, 1, 12, -1, 0);
}

struct BurnDriver BurnDrvDefender = {
	"defender", NULL, NULL, NULL, "1980",
	"Defender (Red label)\0", NULL, "Williams", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_MISC, 0,
	NULL, defenderRomInfo, defenderRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Defender (Green label)

static struct BurnRomInfo defendergRomDesc[] = {
	{ "defeng01.bin",		0x0800, 0x6111d74d, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "defeng04.bin",		0x0800, 0x3cfc04ce, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "defeng02.bin",		0x1000, 0xd184ab6b, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "defeng03.bin",		0x1000, 0x788b76d7, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "defeng09.bin",		0x0800, 0xf57caa62, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "defeng12.bin",		0x0800, 0x33db686f, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "defeng08.bin",		0x0800, 0x9a9eb3d2, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "defeng11.bin",		0x0800, 0x5ca4e860, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "defeng07.bin",		0x0800, 0x545c3326, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "defeng10.bin",		0x0800, 0x941cf34e, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "defeng06.bin",		0x0800, 0x3af34c05, 1 | BRF_PRG | BRF_ESS }, // 10

	{ "defend.snd",			0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, // 10 M6808 Code

	{ "decoder.1",			0x0200, 0x8dd98da5, 0 | BRF_OPT },           // 11 Address Decoder
};

STD_ROM_PICK(defenderg)
STD_ROM_FN(defenderg)

struct BurnDriver BurnDrvDefenderg = {
	"defenderg", "defender", NULL, NULL, "1980",
	"Defender (Green label)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, defendergRomInfo, defendergRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Defender (Blue label)

static struct BurnRomInfo defenderbRomDesc[] = {
	{ "wb01.bin",			0x1000, 0x0ee1019d, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "defeng02.bin",		0x1000, 0xd184ab6b, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "wb03.bin",			0x1000, 0xa732d649, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "defeng09.bin",		0x0800, 0xf57caa62, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "defeng12.bin",		0x0800, 0x33db686f, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "defeng08.bin",		0x0800, 0x9a9eb3d2, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "defeng11.bin",		0x0800, 0x5ca4e860, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "defeng07.bin",		0x0800, 0x545c3326, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "defeng10.bin",		0x0800, 0x941cf34e, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "defeng06.bin",		0x0800, 0x3af34c05, 1 | BRF_PRG | BRF_ESS }, //  9

	{ "defend.snd",			0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, // 10 M6808 Code

	{ "decoder.1",			0x0200, 0x8dd98da5, 0 | BRF_OPT },           // 11 Address Decoder
};

STD_ROM_PICK(defenderb)
STD_ROM_FN(defenderb)

struct BurnDriver BurnDrvDefenderb = {
	"defenderb", "defender", NULL, NULL, "1980",
	"Defender (Blue label)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, defenderbRomInfo, defenderbRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Defender (White label)

static struct BurnRomInfo defenderwRomDesc[] = {
	{ "rom1.bin",			0x1000, 0x5af871e3, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "rom2.bin",			0x1000, 0x1126adc9, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "rom3.bin",			0x1000, 0x4097b46b, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "rom9.bin",			0x0800, 0x93012991, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "rom12.bin",			0x0800, 0x4bdd8dc4, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "rom8.bin",			0x0800, 0x5227fc0b, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "rom11.bin",			0x0800, 0xd068f0c5, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "rom7.bin",			0x0800, 0xfef4cb77, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "rom10.bin",			0x0800, 0x49b50b40, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "rom6.bin",			0x0800, 0x43d42a1b, 1 | BRF_PRG | BRF_ESS }, //  9

	{ "defend.snd",			0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, // 10 M6808 Code

	{ "decoder.1",			0x0200, 0x8dd98da5, 0 | BRF_OPT },           // 11 Address Decoder
};

STD_ROM_PICK(defenderw)
STD_ROM_FN(defenderw)

struct BurnDriver BurnDrvDefenderw = {
	"defenderw", "defender", NULL, NULL, "1980",
	"Defender (White label)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, defenderwRomInfo, defenderwRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// T.T Defender

static struct BurnRomInfo defenderjRomDesc[] = {
	{ "df1-1.e3",			0x1000, 0x8c04602b, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "df2-1.e2",			0x1000, 0x89b75984, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "df3-1.e1",			0x1000, 0x94f51e9b, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "df10-1.a1",			0x0800, 0x12e2bd1c, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "df7-1.b1",			0x0800, 0xf1f88938, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "df9-1.a2",			0x0800, 0xb649e306, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "df6-1.b2",			0x0800, 0x9deaf6d9, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "df8-1.a3",			0x0800, 0x339e092e, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "df5-1.b3",			0x0800, 0xa543b167, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "df4-1.c1",			0x0800, 0x65f4efd1, 1 | BRF_PRG | BRF_ESS }, //  9

	{ "df12.i3",			0x0800, 0xf122d9c9, 2 | BRF_PRG | BRF_ESS }, // 10 M6808 Code
};

STD_ROM_PICK(defenderj)
STD_ROM_FN(defenderj)

struct BurnDriver BurnDrvDefenderj = {
	"defenderj", "defender", NULL, NULL, "1980",
	"T.T Defender\0", NULL, "Williams (Taito Corporation license)", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, defenderjRomInfo, defenderjRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Defender (bootleg)

static struct BurnRomInfo defndjeuRomDesc[] = {
	{ "15",					0x1000, 0x706a24bd, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "16",					0x1000, 0x03201532, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "17",					0x1000, 0x25287eca, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "21",					0x1000, 0xbddb71a3, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "20",					0x1000, 0x12fa0788, 1 | BRF_PRG | BRF_ESS | BRF_NODUMP }, //  4
	{ "19",					0x1000, 0x769f5984, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "18",					0x1000, 0xe99d5679, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "s",					0x0800, 0xcb79ae42, 2 | BRF_PRG | BRF_ESS }, //  7 M6808 Code
};

STD_ROM_PICK(defndjeu)
STD_ROM_FN(defndjeu)

struct BurnDriverD BurnDrvDefndjeu = {
	"defndjeu", "defender", NULL, NULL, "1980",
	"Defender (bootleg)\0", NULL, "bootleg (Jeutel)", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, defndjeuRomInfo, defndjeuRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Tornado (set 1, Defender bootleg)

static struct BurnRomInfo tornado1RomDesc[] = {
	{ "torna1.bin",			0x1000, 0x706a24bd, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "torna3.bin",			0x1000, 0xa79213b2, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "torna4.bin",			0x1000, 0xf96d3d26, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "tornc4.bin",			0x1000, 0xe30f4c00, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "tornb3.bin",			0x1000, 0x0e3fef55, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "tornb1.bin",			0x1000, 0xf2bef850, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "tornb4.bin",			0x1000, 0xe99d5679, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "tornb6.bin",			0x1000, 0x3685e033, 2 | BRF_PRG | BRF_ESS }, //  7 M6808 Code
};

STD_ROM_PICK(tornado1)
STD_ROM_FN(tornado1)

struct BurnDriver BurnDrvTornado1 = {
	"tornado1", "defender", NULL, NULL, "1980",
	"Tornado (set 1, Defender bootleg)\0", NULL, "bootleg (Jeutel)", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, tornado1RomInfo, tornado1RomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Tornado (set 2, Defender bootleg)

static struct BurnRomInfo tornado2RomDesc[] = {
	{ "tto15.bin",			0x1000, 0x910ac603, 1 | BRF_PRG | BRF_ESS | BRF_NODUMP }, //  0 M6809 Code
	{ "to16.bin",			0x1000, 0x46ccd582, 1 | BRF_PRG | BRF_ESS | BRF_NODUMP }, //  1
	{ "tto17.bin",			0x1000, 0xfaa3613c, 1 | BRF_PRG | BRF_ESS | BRF_NODUMP }, //  2
	{ "to21.bin",			0x1000, 0xe30f4c00, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "to20.bin",			0x1000, 0xe90bdcb2, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "to19.bin",			0x1000, 0x42885b4f, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "to18.bin",			0x1000, 0xc15ffc03, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "to_s.bin",			0x0800, 0xcb79ae42, 2 | BRF_PRG | BRF_ESS }, //  7 M6808 Code
};

STD_ROM_PICK(tornado2)
STD_ROM_FN(tornado2)

struct BurnDriverD BurnDrvTornado2 = {
	"tornado2", "defender", NULL, NULL, "1980",
	"Tornado (set 2, Defender bootleg)\0", NULL, "bootleg (Jeutel)", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, tornado2RomInfo, tornado2RomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Zero (set 1, Defender bootleg)

static struct BurnRomInfo zeroRomDesc[] = {
	{ "zero-15",			0x1000, 0x706a24bd, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "zero-16",			0x1000, 0xa79213b2, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "zero-17",			0x1000, 0x25287eca, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "zero-21",			0x1000, 0x7ca35cfd, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "zero-20",			0x1000, 0x0757967f, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "zero-19",			0x1000, 0xf2bef850, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "zero-18",			0x1000, 0xe99d5679, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "defend.snd",			0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, //  7 M6808 Code
};

STD_ROM_PICK(zero)
STD_ROM_FN(zero)

struct BurnDriver BurnDrvZero = {
	"zero", "defender", NULL, NULL, "1980",
	"Zero (set 1, Defender bootleg)\0", NULL, "bootleg (Jeutel)", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, zeroRomInfo, zeroRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Zero (set 2, Defender bootleg)

static struct BurnRomInfo zero2RomDesc[] = {
	{ "15me.1a",			0x1000, 0x9323eee5, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "to16.3a",			0x1000, 0xa79213b2, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "17m5.4a",			0x1000, 0x16a3c0dd, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "21.4c",				0x1000, 0x7ca35cfd, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "20m5.3b",			0x1000, 0x7473955b, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "to19.1b",			0x1000, 0xf2bef850, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "18m5.4b",			0x1000, 0x7e4afe43, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "to4.6b",				0x0800, 0xcb79ae42, 2 | BRF_PRG | BRF_ESS }, //  7 M6808 Code
};

STD_ROM_PICK(zero2)
STD_ROM_FN(zero2)

struct BurnDriver BurnDrvZero2 = {
	"zero2", "defender", NULL, NULL, "1980",
	"Zero (set 2, Defender bootleg)\0", NULL, "bootleg (Amtec)", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, zero2RomInfo, zero2RomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Defense Command (Defender bootleg)

static struct BurnRomInfo defcmndRomDesc[] = {
	{ "defcmnda.1",			0x1000, 0x68effc1d, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "defcmnda.2",			0x1000, 0x1126adc9, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "defcmnda.3",			0x1000, 0x7340209d, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "defcmnda.10",		0x0800, 0x3dddae75, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "defcmnda.7",			0x0800, 0x3f1e7cf8, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "defcmnda.9",			0x0800, 0x8882e1ff, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "defcmnda.6",			0x0800, 0xd068f0c5, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "defcmnda.8",			0x0800, 0xfef4cb77, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "defcmnda.5",			0x0800, 0x49b50b40, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "defcmnda.4",			0x0800, 0x43d42a1b, 1 | BRF_PRG | BRF_ESS }, //  9

	{ "defcmnda.snd",		0x0800, 0xf122d9c9, 2 | BRF_PRG | BRF_ESS }, // 10 M6808 Code
};

STD_ROM_PICK(defcmnd)
STD_ROM_FN(defcmnd)

struct BurnDriver BurnDrvDefcmnd = {
	"defcmnd", "defender", NULL, NULL, "1980",
	"Defense Command (Defender bootleg)\0", NULL, "bootleg", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, defcmndRomInfo, defcmndRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Star Trek (Defender bootleg)

static struct BurnRomInfo startrkdRomDesc[] = {
	{ "st_rom8.bin",		0x1000, 0x5af871e3, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "st_rom9.bin",		0x1000, 0x1126adc9, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "st_rom10.bin",		0x1000, 0x4097b46b, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "st_rom6.bin",		0x0800, 0x93012991, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "st_rom5.bin",		0x0800, 0xc6f0c004, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "st_rom4.bin",		0x0800, 0xb48430bf, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "st_rom3.bin",		0x0800, 0xd068f0c5, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "st_rom2.bin",		0x0800, 0xfef4cb77, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "st_rom1.bin",		0x0800, 0xd23d6cdb, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "st_rom7.bin",		0x0800, 0x43d42a1b, 1 | BRF_PRG | BRF_ESS }, //  9

	{ "defend.snd",			0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, // 10 M6808 Code
};

STD_ROM_PICK(startrkd)
STD_ROM_FN(startrkd)

struct BurnDriver BurnDrvStartrkd = {
	"startrkd", "defender", NULL, NULL, "1981",
	"Star Trek (Defender bootleg)\0", NULL, "bootleg", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, startrkdRomInfo, startrkdRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Defence Command (Defender bootleg)

static struct BurnRomInfo defenceRomDesc[] = {
	{ "1",					0x1000, 0xebc93622, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "2",					0x1000, 0x2a4f4f44, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "3",					0x1000, 0xa4112f91, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "0",					0x0800, 0x7a1e5998, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "7",					0x0800, 0x4c2616a3, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "9",					0x0800, 0x7b146003, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "6",					0x0800, 0x6d748030, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "8",					0x0800, 0x52d5438b, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "5",					0x0800, 0x4a270340, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "4",					0x0800, 0xe13f457c, 1 | BRF_PRG | BRF_ESS }, //  9

	{ "defcmnda.snd",		0x0800, 0xf122d9c9, 2 | BRF_PRG | BRF_ESS }, // 10 M6808 Code
};

STD_ROM_PICK(defence)
STD_ROM_FN(defence)

struct BurnDriver BurnDrvDefence = {
	"defence", "defender", NULL, NULL, "1981",
	"Defence Command (Defender bootleg)\0", NULL, "bootleg (Outer Limits)", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, defenceRomInfo, defenceRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Attack (Defender bootleg)

static struct BurnRomInfo attackfRomDesc[] = {
	{ "1.bin",				0x1000, 0x0ee1019d, 1 | BRF_PRG | BRF_ESS | BRF_NODUMP }, //  0 M6809 Code
	{ "2.bin",				0x1000, 0xd184ab6b, 1 | BRF_PRG | BRF_ESS | BRF_NODUMP }, //  1
	{ "3ojo.bin",			0x1000, 0xa732d649, 1 | BRF_PRG | BRF_ESS | BRF_NODUMP }, //  2
	{ "9.bin",				0x0800, 0xf57caa62, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "12.bin",				0x0800, 0xeb73d8a1, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "8.bin",				0x0800, 0x17f7abde, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "11.bin",				0x0800, 0x5ca4e860, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "7.bin",				0x0800, 0x545c3326, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "10.bin",				0x0800, 0x3940d731, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "6.bin",				0x0800, 0x3af34c05, 1 | BRF_PRG | BRF_ESS }, //  9

	{ "13.bin",				0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, // 10 M6808 Code

	{ "decoder.1",			0x0200, 0x8dd98da5, 0 | BRF_OPT },           // 11 Address Decoder
};

STD_ROM_PICK(attackf)
STD_ROM_FN(attackf)

struct BurnDriver BurnDrvAttackf = {
	"attackf", "defender", NULL, NULL, "1980",
	"Attack (Defender bootleg)\0", NULL, "bootleg (Famare SA)", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_NOT_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, attackfRomInfo, attackfRomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Galaxy Wars II (Defender bootleg)

static struct BurnRomInfo galwars2RomDesc[] = {
	{ "9d-1-2532.bin",		0x1000, 0xebc93622, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "9c-2-2532.bin",		0x1000, 0x2a4f4f44, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "8d-3-2532.bin",		0x1000, 0xa4112f91, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "4c-10-2716.bin",		0x0800, 0x7a1e5998, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "5d-7-2716.bin",		0x0800, 0xa9bdacdc, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "4d-9-2716.bin",		0x0800, 0x906dca8f, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "6c-6-2716.bin",		0x0800, 0x6d748030, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "5c-8-2716.bin",		0x0800, 0x52d5438b, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "6d-5-2716.bin",		0x0800, 0x4a270340, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "7c-4-2716.bin",		0x0800, 0xe13f457c, 1 | BRF_PRG | BRF_ESS }, //  9

	{ "3f-11-2716.bin",		0x0800, 0xf122d9c9, 2 | BRF_PRG | BRF_ESS }, // 10 M6808 Code

	{ "1l-13-8516.bin",		0x0800, 0x7e113979, 0 | BRF_OPT },           // 11 Unknown
	{ "1a-12-8516.bin",		0x0800, 0xa562c506, 0 | BRF_OPT },           // 12
};

STD_ROM_PICK(galwars2)
STD_ROM_FN(galwars2)

struct BurnDriver BurnDrvGalwars2 = {
	"galwars2", "defender", NULL, NULL, "1981",
	"Galaxy Wars II (Defender bootleg)\0", NULL, "bootleg (Sonic)", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, galwars2RomInfo, galwars2RomName, NULL, NULL, DefenderInputInfo, NULL,
	DefenderInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Mayday (set 1)

static struct BurnRomInfo maydayRomDesc[] = {
	{ "ic03-3.bin",			0x1000, 0xa1ff6e62, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "ic02-2.bin",			0x1000, 0x62183aea, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "ic01-1.bin",			0x1000, 0x5dcb113f, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "ic04-4.bin",			0x1000, 0xea6a4ec8, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "ic05-5.bin",			0x1000, 0x0d797a3e, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "ic06-6.bin",			0x1000, 0xee8bfcd6, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "ic07-7d.bin",		0x1000, 0xd9c065e7, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "ic28-8.bin",			0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, //  7 M6808 Code
};

STD_ROM_PICK(mayday)
STD_ROM_FN(mayday)

static INT32 MaydayInit()
{
	mayday = 1;

	return DrvInit(0, 1, 12, -1, 0);
}

struct BurnDriver BurnDrvMayday = {
	"mayday", NULL, NULL, NULL, "1980",
	"Mayday (set 1)\0", NULL, "Hoei", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, maydayRomInfo, maydayRomName, NULL, NULL, MaydayInputInfo, NULL,
	MaydayInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Mayday (set 2)

static struct BurnRomInfo maydayaRomDesc[] = {
	{ "mayday.c",			0x1000, 0x872a2f2d, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "mayday.b",			0x1000, 0xc4ab5e22, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "mayday.a",			0x1000, 0x329a1318, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "mayday.d",			0x1000, 0xc2ae4716, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "mayday.e",			0x1000, 0x41225666, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "mayday.f",			0x1000, 0xc39be3c0, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "mayday.g",			0x1000, 0x2bd0f106, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "ic28-8.bin",			0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, //  7 M6808 Code
};

STD_ROM_PICK(maydaya)
STD_ROM_FN(maydaya)

struct BurnDriver BurnDrvMaydaya = {
	"maydaya", "mayday", NULL, NULL, "1980",
	"Mayday (set 2)\0", NULL, "Hoei", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, maydayaRomInfo, maydayaRomName, NULL, NULL, MaydayInputInfo, NULL,
	MaydayInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Mayday (set 3)

static struct BurnRomInfo maydaybRomDesc[] = {
	{ "ic03-3.bin",			0x1000, 0xa1ff6e62, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "ic02-2.bin",			0x1000, 0x62183aea, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "ic01-1.bin",			0x1000, 0x5dcb113f, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "rom7.bin",			0x1000, 0x0c3ca687, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "ic05-5.bin",			0x1000, 0x0d797a3e, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "ic06-6.bin",			0x1000, 0xee8bfcd6, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "ic07-7d.bin",		0x1000, 0xd9c065e7, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "ic28-8.bin",			0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, //  7 M6808 Code

	{ "rom11.bin",			0x0800, 0x7e113979, 0 | BRF_OPT },           //  8 user1
	{ "rom12.bin",			0x0800, 0xa562c506, 0 | BRF_OPT },           //  9
	{ "rom6a.bin",			0x0800, 0x8e4e981f, 0 | BRF_OPT },           // 10
	{ "rom8-sos.bin",		0x0800, 0x6a9b383f, 0 | BRF_OPT },           // 11
};

STD_ROM_PICK(maydayb)
STD_ROM_FN(maydayb)

struct BurnDriver BurnDrvMaydayb = {
	"maydayb", "mayday", NULL, NULL, "1980",
	"Mayday (set 3)\0", NULL, "Hoei", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, maydaybRomInfo, maydaybRomName, NULL, NULL, MaydayInputInfo, NULL,
	MaydayInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Battle Zone (bootleg of Mayday)

static struct BurnRomInfo batlzoneRomDesc[] = {
	{ "43-2732.rom.bin",	0x1000, 0x244334f8, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "42-2732.rom.bin",	0x1000, 0x62183aea, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "41-2732.rom.bin",	0x1000, 0xa7e9093e, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "44-8532.rom.bin",	0x1000, 0xbba3e626, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "45-8532.rom.bin",	0x1000, 0x43b3a0de, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "46-8532.rom.bin",	0x1000, 0x3df9b901, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "47-8532.rom.bin",	0x1000, 0x55a27e02, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "48-2716.rom.bin",	0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, //  7 M6808 Code
};

STD_ROM_PICK(batlzone)
STD_ROM_FN(batlzone)

struct BurnDriver BurnDrvBatlzone = {
	"batlzone", "mayday", NULL, NULL, "1980",
	"Battle Zone (bootleg of Mayday)\0", NULL, "bootleg (Video Game)", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, batlzoneRomInfo, batlzoneRomName, NULL, NULL, MaydayInputInfo, NULL,
	MaydayInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Colony 7 (set 1)

static struct BurnRomInfo colony7RomDesc[] = {
	{ "cs03.bin",			0x1000, 0x7ee75ae5, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "cs02.bin",			0x1000, 0xc60b08cb, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "cs01.bin",			0x1000, 0x1bc97436, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "cs06.bin",			0x0800, 0x318b95af, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "cs04.bin",			0x0800, 0xd740faee, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "cs07.bin",			0x0800, 0x0b23638b, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "cs05.bin",			0x0800, 0x59e406a8, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "cs08.bin",			0x0800, 0x3bfde87a, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "cs11.bin",			0x0800, 0x6032293c, 2 | BRF_PRG | BRF_ESS }, //  8 M6808 Code

	{ "cs10.bin",			0x0200, 0x25de5d85, 0 | BRF_OPT },           //  9 Address Decoder?
	{ "decoder.3",			0x0200, 0xc3f45f70, 0 | BRF_OPT },           // 10
};

STD_ROM_PICK(colony7)
STD_ROM_FN(colony7)

static INT32 Colony7Init()
{
	return DrvInit(0, 0, 14, -1, 0);
}

struct BurnDriver BurnDrvColony7 = {
	"colony7", NULL, NULL, NULL, "1981",
	"Colony 7 (set 1)\0", NULL, "Taito", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_SHOOT, 0,
	NULL, colony7RomInfo, colony7RomName, NULL, NULL, Colony7InputInfo, Colony7DIPInfo,
	Colony7Init, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	240, 292, 3, 4
};


// Colony 7 (set 2)

static struct BurnRomInfo colony7aRomDesc[] = {
	{ "cs03a.bin",			0x1000, 0xe0b0d23b, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "cs02a.bin",			0x1000, 0x370c6f41, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "cs01a.bin",			0x1000, 0xba299946, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "cs06.bin",			0x0800, 0x318b95af, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "cs04.bin",			0x0800, 0xd740faee, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "cs07.bin",			0x0800, 0x0b23638b, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "cs05.bin",			0x0800, 0x59e406a8, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "cs08.bin",			0x0800, 0x3bfde87a, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "cs11.bin",			0x0800, 0x6032293c, 2 | BRF_PRG | BRF_ESS }, //  8 M6808 Code

	{ "cs10.bin",			0x0200, 0x25de5d85, 0 | BRF_OPT },           //  9 Address Decoder?
	{ "decoder.3",			0x0200, 0xc3f45f70, 0 | BRF_OPT },           // 10
};

STD_ROM_PICK(colony7a)
STD_ROM_FN(colony7a)

struct BurnDriver BurnDrvColony7a = {
	"colony7a", "colony7", NULL, NULL, "1981",
	"Colony 7 (set 2)\0", NULL, "Taito", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_SHOOT, 0,
	NULL, colony7aRomInfo, colony7aRomName, NULL, NULL, Colony7InputInfo, Colony7DIPInfo,
	Colony7Init, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	240, 292, 3, 4
};


// Jin

static struct BurnRomInfo jinRomDesc[] = {
	{ "jin11.6c",			0x1000, 0xc4b9e93f, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "jin12.7c",			0x1000, 0xa8bc9fdd, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "jin13.6d",			0x1000, 0x79779b85, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "jin14.4c",			0x1000, 0x6a4df97e, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "jin15.3f",			0x0800, 0xfefd5b48, 2 | BRF_PRG | BRF_ESS }, //  4 M6808 Code

	{ "jin.1a",				0x0200, 0x8dd98da5, 0 | BRF_OPT },           //  5 Address Decoder?
	{ "jin.1l",				0x0200, 0xc3f45f70, 0 | BRF_OPT },           //  6
};

STD_ROM_PICK(jin)
STD_ROM_FN(jin)

static INT32 JinInit()
{
	return DrvInit(0, 0, 0, -1, 0);
}

struct BurnDriver BurnDrvJin = {
	"jin", NULL, NULL, NULL, "1982",
	"Jin\0", NULL, "Falcon", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL | BDF_ORIENTATION_FLIPPED, 2, HARDWARE_MISC_PRE90S, GBF_PUZZLE, 0,
	NULL, jinRomInfo, jinRomName, NULL, NULL, JinInputInfo, JinDIPInfo,
	JinInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	240, 316, 3, 4
};


// Stargate

static struct BurnRomInfo stargateRomDesc[] = {
	{ "10",					0x1000, 0x60b07ff7, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "11",					0x1000, 0x7d2c5daf, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "12",					0x1000, 0xa0396670, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "01",					0x1000, 0x88824d18, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "02",					0x1000, 0xafc614c5, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "03",					0x1000, 0x15077a9d, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "04",					0x1000, 0xa8b4bf0f, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "05",					0x1000, 0x2d306074, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "06",					0x1000, 0x53598dde, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "07",					0x1000, 0x23606060, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "08",					0x1000, 0x4ec490c7, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "09",					0x1000, 0x88187b64, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "sg.snd",				0x0800, 0x2fcf6c4d, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.5",			0x0200, 0xf921c5fe, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(stargate)
STD_ROM_FN(stargate)

static INT32 StargateInit()
{
	return DrvInit(1, 0, 6, -1, 0);
}

struct BurnDriver BurnDrvStargate = {
	"stargate", NULL, NULL, NULL, "1981",
	"Stargate\0", NULL, "Williams / Vid Kidz", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, stargateRomInfo, stargateRomName, NULL, NULL, StargateInputInfo, NULL,
	StargateInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Robotron: 2084 (Solid Blue label)

static struct BurnRomInfo robotronRomDesc[] = {
	{ "robotron.sba",		0x1000, 0x13797024, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "robotron.sbb",		0x1000, 0x7e3c1b87, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "robotron.sbc",		0x1000, 0x645d543e, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "robotron.sb1",		0x1000, 0x66c7d3ef, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "robotron.sb2",		0x1000, 0x5bc6c614, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "robotron.sb3",		0x1000, 0xe99a82be, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "robotron.sb4",		0x1000, 0xafb1c561, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "robotron.sb5",		0x1000, 0x62691e77, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "robotron.sb6",		0x1000, 0xbd2c853d, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "robotron.sb7",		0x1000, 0x49ac400c, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "robotron.sb8",		0x1000, 0x3a96e88c, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "robotron.sb9",		0x1000, 0xb124367b, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "robotron.snd",		0x1000, 0xc56c1d28, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(robotron)
STD_ROM_FN(robotron)

static INT32 RobotronInit()
{
	return DrvInit(1, 0, 6, 1, 0xc000);
}

struct BurnDriver BurnDrvRobotron = {
	"robotron", NULL, NULL, NULL, "1982",
	"Robotron: 2084 (Solid Blue label)\0", NULL, "Williams / Vid Kidz", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_SHOOT, 0,
	NULL, robotronRomInfo, robotronRomName, NULL, NULL, RobotronInputInfo, NULL,
	RobotronInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Robotron: 2084 (Yellow/Orange label)

static struct BurnRomInfo robotronyoRomDesc[] = {
	{ "robotron.yoa",		0x1000, 0x4a9d5f52, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "robotron.yob",		0x1000, 0x2afc5e7f, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "robotron.yoc",		0x1000, 0x45da9202, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "robotron.sb1",		0x1000, 0x66c7d3ef, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "robotron.sb2",		0x1000, 0x5bc6c614, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "robotron.yo3",		0x1000, 0x67a369bc, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "robotron.yo4",		0x1000, 0xb0de677a, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "robotron.yo5",		0x1000, 0x24726007, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "robotron.yo6",		0x1000, 0x028181a6, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "robotron.yo7",		0x1000, 0x4dfcceae, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "robotron.sb8",		0x1000, 0x3a96e88c, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "robotron.sb9",		0x1000, 0xb124367b, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "robotron.snd",		0x1000, 0xc56c1d28, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(robotronyo)
STD_ROM_FN(robotronyo)

struct BurnDriver BurnDrvRobotronyo = {
	"robotronyo", "robotron", NULL, NULL, "1982",
	"Robotron: 2084 (Yellow/Orange label)\0", NULL, "Williams / Vid Kidz", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_SHOOT, 0,
	NULL, robotronyoRomInfo, robotronyoRomName, NULL, NULL, RobotronInputInfo, NULL,
	RobotronInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Robotron: 2084 (1987 'shot-in-the-corner' bugfix)

static struct BurnRomInfo robotron87RomDesc[] = {
	{ "robotron.sba",		0x1000, 0x13797024, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "fixrobo.sbb",		0x1000, 0xe83a2eda, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "robotron.sbc",		0x1000, 0x645d543e, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "robotron.sb1",		0x1000, 0x66c7d3ef, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "robotron.sb2",		0x1000, 0x5bc6c614, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "robotron.sb3",		0x1000, 0xe99a82be, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "robotron.sb4",		0x1000, 0xafb1c561, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "fixrobo.sb5",		0x1000, 0x827cb5c9, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "robotron.sb6",		0x1000, 0xbd2c853d, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "robotron.sb7",		0x1000, 0x49ac400c, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "robotron.sb8",		0x1000, 0x3a96e88c, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "robotron.sb9",		0x1000, 0xb124367b, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "robotron.snd",		0x1000, 0xc56c1d28, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(robotron87)
STD_ROM_FN(robotron87)

struct BurnDriver BurnDrvRobotron87 = {
	"robotron87", "robotron", NULL, NULL, "1987",
	"Robotron: 2084 (1987 'shot-in-the-corner' bugfix)\0", NULL, "hack", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_SHOOT, 0,
	NULL, robotron87RomInfo, robotron87RomName, NULL, NULL, RobotronInputInfo, NULL,
	RobotronInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Robotron: 2084 (2012 'wave 201 start' hack)

static struct BurnRomInfo robotron12RomDesc[] = {
	{ "robotron.sba",		0x1000, 0x13797024, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "fixrobo.sbb",		0x1000, 0xe83a2eda, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "robotron.sbc",		0x1000, 0x645d543e, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "robotron.sb1",		0x1000, 0x66c7d3ef, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "robotron.sb2",		0x1000, 0x5bc6c614, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "wave201.sb3",		0x1000, 0x85eb583e, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "robotron.sb4",		0x1000, 0xafb1c561, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "fixrobo.sb5",		0x1000, 0x827cb5c9, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "robotron.sb6",		0x1000, 0xbd2c853d, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "robotron.sb7",		0x1000, 0x49ac400c, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "robotron.sb8",		0x1000, 0x3a96e88c, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "robotron.sb9",		0x1000, 0xb124367b, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "robotron.snd",		0x1000, 0xc56c1d28, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(robotron12)
STD_ROM_FN(robotron12)

struct BurnDriver BurnDrvRobotron12 = {
	"robotron12", "robotron", NULL, NULL, "2012",
	"Robotron: 2084 (2012 'wave 201 start' hack)\0", NULL, "hack", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_POST90S, GBF_SHOOT, 0,
	NULL, robotron12RomInfo, robotron12RomName, NULL, NULL, RobotronInputInfo, NULL,
	RobotronInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Robotron: 2084 (2015 'tie-die V2' hack)

static struct BurnRomInfo robotrontdRomDesc[] = {
	{ "tiedie.sba",			0x1000, 0x952bea55, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "tiedie.sbb",			0x1000, 0x4c05fd3c, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "robotron.sbc",		0x1000, 0x645d543e, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "robotron.sb1",		0x1000, 0x66c7d3ef, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "robotron.sb2",		0x1000, 0x5bc6c614, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "robotron.sb3",		0x1000, 0xe99a82be, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "tiedie.sb4",			0x1000, 0xe8238019, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "fixrobo.sb5",		0x1000, 0x827cb5c9, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "robotron.sb6",		0x1000, 0xbd2c853d, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "tiedie.sb7",			0x1000, 0x3ecf4620, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "tiedie.sb8",			0x1000, 0x752d7a46, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "robotron.sb9",		0x1000, 0xb124367b, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "robotron.snd",		0x1000, 0xc56c1d28, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(robotrontd)
STD_ROM_FN(robotrontd)

struct BurnDriver BurnDrvRobotrontd = {
	"robotrontd", "robotron", NULL, NULL, "2015",
	"Robotron: 2084 (2015 'tie-die V2' hack)\0", NULL, "hack", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_POST90S, GBF_SHOOT, 0,
	NULL, robotrontdRomInfo, robotrontdRomName, NULL, NULL, RobotronInputInfo, NULL,
	RobotronInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Joust (White/Green label)

static struct BurnRomInfo joustRomDesc[] = {
	{ "3006-22.10b",		0x1000, 0x3f1c4f89, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "3006-23.11b",		0x1000, 0xea48b359, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "3006-24.12b",		0x1000, 0xc710717b, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "3006-13.1b",			0x1000, 0xfe41b2af, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "3006-14.2b",			0x1000, 0x501c143c, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "3006-15.3b",			0x1000, 0x43f7161d, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "3006-16.4b",			0x1000, 0xdb5571b6, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "3006-17.5b",			0x1000, 0xc686bb6b, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "3006-18.6b",			0x1000, 0xfac5f2cf, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "3006-19.7b",			0x1000, 0x81418240, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "3006-20.8b",			0x1000, 0xba5359ba, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "3006-21.9b",			0x1000, 0x39643147, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "joust.snd",			0x1000, 0xf1835bdd, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(joust)
STD_ROM_FN(joust)

static INT32 JoustInit()
{
	INT32 nRet = DrvInit(1, 0, 6, 1, 0xc000);

	if (nRet == 0)
	{
		pia_config(0, 0, &pia_muxed_joust_0);
		pStartDraw = DrvDrawBegin;
		pDrawScanline = DrvDrawLine;
	}

	return nRet;
}

struct BurnDriver BurnDrvJoust = {
	"joust", NULL, NULL, NULL, "1982",
	"Joust (White/Green label)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_PLATFORM, 0,
	NULL, joustRomInfo, joustRomName, NULL, NULL, JoustInputInfo, NULL,
	JoustInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Joust (White/Red label)

static struct BurnRomInfo joustwrRomDesc[] = {
	{ "joust.wra",			0x1000, 0x2039014a, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "joust.wgb",			0x1000, 0xea48b359, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "joust.wgc",			0x1000, 0xc710717b, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "joust.wg1",			0x1000, 0xfe41b2af, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "joust.wg2",			0x1000, 0x501c143c, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "joust.wg3",			0x1000, 0x43f7161d, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "joust.wg4",			0x1000, 0xdb5571b6, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "joust.wg5",			0x1000, 0xc686bb6b, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "joust.wg6",			0x1000, 0xfac5f2cf, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "joust.wr7",			0x1000, 0xe6f439c4, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "joust.wg8",			0x1000, 0xba5359ba, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "joust.wg9",			0x1000, 0x39643147, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "joust.snd",			0x1000, 0xf1835bdd, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(joustwr)
STD_ROM_FN(joustwr)

struct BurnDriver BurnDrvJoustwr = {
	"joustwr", "joust", NULL, NULL, "1982",
	"Joust (White/Red label)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_PLATFORM, 0,
	NULL, joustwrRomInfo, joustwrRomName, NULL, NULL, JoustInputInfo, NULL,
	JoustInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Joust (Solid Red label)

static struct BurnRomInfo joustrRomDesc[] = {
	{ "joust.sra",			0x1000, 0xc0c6e52a, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "joust.srb",			0x1000, 0xab11bcf9, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "joust.src",			0x1000, 0xea14574b, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "joust.wg1",			0x1000, 0xfe41b2af, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "joust.wg2",			0x1000, 0x501c143c, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "joust.wg3",			0x1000, 0x43f7161d, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "joust.sr4",			0x1000, 0xab347170, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "joust.wg5",			0x1000, 0xc686bb6b, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "joust.sr6",			0x1000, 0x3d9a6fac, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "joust.sr7",			0x1000, 0x0a70b3d1, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "joust.sr8",			0x1000, 0xa7f01504, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "joust.sr9",			0x1000, 0x978687ad, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "joust.snd",			0x1000, 0xf1835bdd, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(joustr)
STD_ROM_FN(joustr)

struct BurnDriver BurnDrvJoustr = {
	"joustr", "joust", NULL, NULL, "1982",
	"Joust (Solid Red label)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_PLATFORM, 0,
	NULL, joustrRomInfo, joustrRomName, NULL, NULL, JoustInputInfo, NULL,
	JoustInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Bubbles

static struct BurnRomInfo bubblesRomDesc[] = {
	{ "bubbles.10b",		0x1000, 0x26e7869b, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "bubbles.11b",		0x1000, 0x5a5b572f, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "bubbles.12b",		0x1000, 0xce22d2e2, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "bubbles.1b",			0x1000, 0x8234f55c, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "bubbles.2b",			0x1000, 0x4a188d6a, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "bubbles.3b",			0x1000, 0x7728f07f, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "bubbles.4b",			0x1000, 0x040be7f9, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "bubbles.5b",			0x1000, 0x0b5f29e0, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "bubbles.6b",			0x1000, 0x4dd0450d, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "bubbles.7b",			0x1000, 0xe0a26ec0, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "bubbles.8b",			0x1000, 0x4fd23d8d, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "bubbles.9b",			0x1000, 0xb48559fb, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "bubbles.snd",		0x1000, 0x689ce2aa, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(bubbles)
STD_ROM_FN(bubbles)

static INT32 BubblesInit()
{
	INT32 nRet = DrvInit(1, 0, 6, 1, 0xc000);
	
	if (nRet == 0) {
		M6809Open(0);
		M6809MapMemory(DrvNVRAM,		0xcc00, 0xcfff, MAP_RAM); // 8 bit
		M6809Close();
	}
	
	return nRet;
}

struct BurnDriver BurnDrvBubbles = {
	"bubbles", NULL, NULL, NULL, "1982",
	"Bubbles\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_MAZE, 0,
	NULL, bubblesRomInfo, bubblesRomName, NULL, NULL, BubblesInputInfo, NULL,
	BubblesInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Bubbles (Solid Red label)

static struct BurnRomInfo bubblesrRomDesc[] = {
	{ "bubblesr.10b",		0x1000, 0x8b396db0, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "bubblesr.11b",		0x1000, 0x096af43e, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "bubblesr.12b",		0x1000, 0x5c1244ef, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "bubblesr.1b",		0x1000, 0xdda4e782, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "bubblesr.2b",		0x1000, 0x3c8fa7f5, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "bubblesr.3b",		0x1000, 0xf869bb9c, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "bubblesr.4b",		0x1000, 0x0c65eaab, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "bubblesr.5b",		0x1000, 0x7ece4e13, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "bubbles.6b",			0x1000, 0x4dd0450d, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "bubbles.7b",			0x1000, 0xe0a26ec0, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "bubblesr.8b",		0x1000, 0x598b9bd6, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "bubbles.9b",			0x1000, 0xb48559fb, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "bubbles.snd",		0x1000, 0x689ce2aa, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(bubblesr)
STD_ROM_FN(bubblesr)

struct BurnDriver BurnDrvBubblesr = {
	"bubblesr", "bubbles", NULL, NULL, "1982",
	"Bubbles (Solid Red label)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_MAZE, 0,
	NULL, bubblesrRomInfo, bubblesrRomName, NULL, NULL, BubblesInputInfo, NULL,
	BubblesInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Bubbles (prototype version)

static struct BurnRomInfo bubblespRomDesc[] = {
	{ "bub_prot.10b",		0x1000, 0x89a565df, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "bub_prot.11b",		0x1000, 0x5a0c36a7, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "bub_prot.12b",		0x1000, 0x2bfd3438, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "bub_prot.1b",		0x1000, 0x6466a746, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "bub_prot.2b",		0x1000, 0xcca04357, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "bub_prot.3b",		0x1000, 0x7aaff9e5, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "bub_prot.4b",		0x1000, 0x4e264f01, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "bub_prot.5b",		0x1000, 0x121b0be6, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "bub_prot.6b",		0x1000, 0x80e90b25, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "bubbles.7b",			0x1000, 0xe0a26ec0, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "bub_prot.8b",		0x1000, 0x96fb19c8, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "bub_prot.9b",		0x1000, 0xbe7e1028, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "bubbles.snd",		0x1000, 0x689ce2aa, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(bubblesp)
STD_ROM_FN(bubblesp)

struct BurnDriver BurnDrvBubblesp = {
	"bubblesp", "bubbles", NULL, NULL, "1982",
	"Bubbles (prototype version)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_MISC_PRE90S, GBF_MAZE, 0,
	NULL, bubblespRomInfo, bubblespRomName, NULL, NULL, BubblesInputInfo, NULL,
	BubblesInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Splat!

static struct BurnRomInfo splatRomDesc[] = {
	{ "splat.10",			0x1000, 0xd1a1f632, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "splat.11",			0x1000, 0xca8cde95, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "splat.12",			0x1000, 0x5bee3e60, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "splat.01",			0x1000, 0x1cf26e48, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "splat.02",			0x1000, 0xac0d4276, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "splat.03",			0x1000, 0x74873e59, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "splat.04",			0x1000, 0x70a7064e, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "splat.05",			0x1000, 0xc6895221, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "splat.06",			0x1000, 0xea4ab7fd, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "splat.07",			0x1000, 0x82fd8713, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "splat.08",			0x1000, 0x7dded1b4, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "splat.09",			0x1000, 0x71cbfe5a, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "splat.snd",			0x1000, 0xa878d5f3, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(splat)
STD_ROM_FN(splat)

static INT32 SplatInit()
{
	splat = 1;

	INT32 nRet = DrvInit(1, 0, 6, 2, 0xc000);

	if (nRet == 0)
	{
		pia_config(0, 0, &pia_muxed_joust_0);
		pStartDraw = DrvDrawBegin;
		pDrawScanline = DrvDrawLine;
	}

	return nRet;
}

struct BurnDriver BurnDrvSplat = {
	"splat", NULL, NULL, NULL, "1982",
	"Splat!\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_SHOOT, 0,
	NULL, splatRomInfo, splatRomName, NULL, NULL, SplatInputInfo, NULL,
	SplatInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Speed Ball - Contest at Neonworld (prototype)

static struct BurnRomInfo spdballRomDesc[] = {
	{ "speedbal.10",		0x1000, 0x4a3add93, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "speedbal.11",		0x1000, 0x1fbcfaa5, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "speedbal.12",		0x1000, 0xf3458f41, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "speedbal.01",		0x1000, 0x7f4801bb, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "speedbal.02",		0x1000, 0x5cd5e489, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "speedbal.03",		0x1000, 0x280e11a4, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "speedbal.04",		0x1000, 0x3469cbbf, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "speedbal.05",		0x1000, 0x87373c89, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "speedbal.06",		0x1000, 0x48779a0d, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "speedbal.07",		0x1000, 0x2e5d8db6, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "speedbal.08",		0x1000, 0xc173cedf, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "speedbal.09",		0x1000, 0x415f424b, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "speedbal.snd",		0x1000, 0x78de20e2, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "mystery.rom",		0x1000, 0xdcb6a070, 0 | BRF_OPT },           // 13 ??
};

STD_ROM_PICK(spdball)
STD_ROM_FN(spdball)

static INT32 SpdballInit()
{
	return DrvInit(1, 0, 6, 1, 0xc000);
}

struct BurnDriver BurnDrvSpdball = {
	"spdball", NULL, NULL, NULL, "1985",
	"Speed Ball - Contest at Neonworld (prototype)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_PROTOTYPE, 2, HARDWARE_MISC_PRE90S, GBF_SPORTSMISC, 0,
	NULL, spdballRomInfo, spdballRomName, NULL, NULL, SpdballInputInfo, NULL,
	SpdballInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};



// Alien Arena

static struct BurnRomInfo alienarRomDesc[] = {
	{ "aarom10",			0x1000, 0x6feb0314, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code 
	{ "aarom11",			0x1000, 0xae3a270e, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "aarom12",			0x1000, 0x6be9f09e, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "aarom01",			0x1000, 0xbb0c21be, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "aarom02",			0x1000, 0x165acd37, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "aarom03",			0x1000, 0xe5d51d92, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "aarom04",			0x1000, 0x24f6feb8, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "aarom05",			0x1000, 0x5b1ac59b, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "aarom06",			0x1000, 0xda7195a2, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "aarom07",			0x1000, 0xf9812be4, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "aarom08",			0x1000, 0xcd7f3a87, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "aarom09",			0x1000, 0xe6ce77b4, 1 | BRF_PRG | BRF_ESS }, // 11
};

STD_ROM_PICK(alienar)
STD_ROM_FN(alienar)

static INT32 AlienarInit()
{
	INT32 nRet = DrvInit(1, 0, 6, 2, 0xc000);

	if (nRet == 0)
	{
		pia_config(0, 0, &pia_muxed_joust_0);
	}

	return nRet;
}

struct BurnDriver BurnDrvAlienar = {
	"alienar", NULL, NULL, NULL, "1985",
	"Alien Arena\0", "Game has no sound", "Duncan Brown", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_MAZE, 0,
	NULL, alienarRomInfo, alienarRomName, NULL, NULL, AlienarInputInfo, NULL,
	AlienarInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Alien Arena (Stargate upgrade)

static struct BurnRomInfo alienaruRomDesc[] = {
	{ "aarom10",			0x1000, 0x6feb0314, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "aarom11",			0x1000, 0xae3a270e, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "aarom12",			0x1000, 0x6be9f09e, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "aarom01",			0x1000, 0xbb0c21be, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "aarom02",			0x1000, 0x165acd37, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "aarom03",			0x1000, 0xe5d51d92, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "aarom04",			0x1000, 0x24f6feb8, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "aarom05",			0x1000, 0x5b1ac59b, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "aarom06",			0x1000, 0xda7195a2, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "aarom07",			0x1000, 0xf9812be4, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "aarom08",			0x1000, 0xcd7f3a87, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "aarom09",			0x1000, 0xe6ce77b4, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "sg.snd",				0x0800, 0x2fcf6c4d, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 13 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 14
};

STD_ROM_PICK(alienaru)
STD_ROM_FN(alienaru)

struct BurnDriver BurnDrvAlienaru = {
	"alienaru", "alienar", NULL, NULL, "1985",
	"Alien Arena (Stargate upgrade)\0", NULL, "Duncan Brown", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_MAZE, 0,
	NULL, alienaruRomInfo, alienaruRomName, NULL, NULL, AlienarInputInfo, NULL,
	AlienarInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Sinistar (revision 3)

static struct BurnRomInfo sinistarRomDesc[] = {
	{ "sinistar.10",		0x1000, 0x3d670417, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "sinistar.11",		0x1000, 0x3162bc50, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "sinistar.01",		0x1000, 0xf6f3a22c, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "sinistar.02",		0x1000, 0xcab3185c, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "sinistar.03",		0x1000, 0x1ce1b3cc, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "sinistar.04",		0x1000, 0x6da632ba, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "sinistar.05",		0x1000, 0xb662e8fc, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "sinistar.06",		0x1000, 0x2306183d, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "sinistar.07",		0x1000, 0xe5dd918e, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "sinistar.08",		0x1000, 0x4785a787, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "sinistar.09",		0x1000, 0x50cb63ad, 1 | BRF_PRG | BRF_ESS }, // 10

	{ "speech.ic7",			0x1000, 0xe1019568, 2 | BRF_PRG | BRF_ESS }, // 11 M6800 Code
	{ "speech.ic5",			0x1000, 0xcf3b5ffd, 2 | BRF_PRG | BRF_ESS }, // 12
	{ "speech.ic6",			0x1000, 0xff8d2645, 2 | BRF_PRG | BRF_ESS }, // 13
	{ "speech.ic4",			0x1000, 0x4b56a626, 2 | BRF_PRG | BRF_ESS }, // 14
	{ "sinistar.snd",		0x1000, 0xb82f4ddb, 2 | BRF_PRG | BRF_ESS }, // 15

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 16 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 17
};

STD_ROM_PICK(sinistar)
STD_ROM_FN(sinistar)

static INT32 SinistarInit()
{
	INT32 nRet = DrvInit(1, 4, 6, 1, 0x7400);

	if (nRet == 0)
	{
		hc55516_init(M6800TotalCycles, 894886);
		uses_hc55516 = 1;

		pStartDraw = DrvDrawBegin;
		pDrawScanline = DrvDrawLine;

		pia_init();
		pia_config(0, 0, &pia_49way_0);
		pia_config(1, 0, &pia_1);
		pia_config(2, 0, &pia_2_sinistar);
		pia_config(3, 0, &pia_3);

		M6809Open(0);
		M6809MapMemory(DrvM6809RAM0,		0xd000, 0xdfff, MAP_RAM);
		M6809Close();
	}

	return nRet;
}

struct BurnDriver BurnDrvSinistar = {
	"sinistar", NULL, NULL, NULL, "1982",
	"Sinistar (revision 3)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_SHOOT, 0,
	NULL, sinistarRomInfo, sinistarRomName, NULL, NULL, SinistarInputInfo, NULL,
	SinistarInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	240, 292, 3, 4
};


// Sinistar (prototype version)

static struct BurnRomInfo sinistar1RomDesc[] = {
	{ "sinrev1.10",			0x1000, 0xea87a53f, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "sinrev1.11",			0x1000, 0x88d36e80, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "sinrev1.01",			0x1000, 0x3810d7b8, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "sinistar.02",		0x1000, 0xcab3185c, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "sinrev1.03",			0x1000, 0x7c984ca9, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "sinrev1.04",			0x1000, 0xcc6c4f24, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "sinrev1.05",			0x1000, 0x12285bfe, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "sinrev1.06",			0x1000, 0x7a675f35, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "sinrev1.07",			0x1000, 0xb0463243, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "sinrev1.08",			0x1000, 0x909040d4, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "sinrev1.09",			0x1000, 0xcc949810, 1 | BRF_PRG | BRF_ESS }, // 10

	{ "speech.ic7",			0x1000, 0xe1019568, 2 | BRF_PRG | BRF_ESS }, // 11 M6800 Code
	{ "speech.ic5",			0x1000, 0xcf3b5ffd, 2 | BRF_PRG | BRF_ESS }, // 12
	{ "speech.ic6",			0x1000, 0xff8d2645, 2 | BRF_PRG | BRF_ESS }, // 13
	{ "speech.ic4",			0x1000, 0x4b56a626, 2 | BRF_PRG | BRF_ESS }, // 14
	{ "sinistar.snd",		0x1000, 0xb82f4ddb, 2 | BRF_PRG | BRF_ESS }, // 15

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 16 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 17
};

STD_ROM_PICK(sinistar1)
STD_ROM_FN(sinistar1)

struct BurnDriver BurnDrvSinistar1 = {
	"sinistar1", "sinistar", NULL, NULL, "1982",
	"Sinistar (prototype version)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_SHOOT, 0,
	NULL, sinistar1RomInfo, sinistar1RomName, NULL, NULL, SinistarInputInfo, NULL,
	SinistarInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	240, 292, 3, 4
};


// Sinistar (revision 2)

static struct BurnRomInfo sinistar2RomDesc[] = {
	{ "sinistar.10",		0x1000, 0x3d670417, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "sinrev2.11",			0x1000, 0x792c8b00, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "sinistar.01",		0x1000, 0xf6f3a22c, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "sinistar.02",		0x1000, 0xcab3185c, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "sinistar.03",		0x1000, 0x1ce1b3cc, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "sinistar.04",		0x1000, 0x6da632ba, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "sinistar.05",		0x1000, 0xb662e8fc, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "sinistar.06",		0x1000, 0x2306183d, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "sinistar.07",		0x1000, 0xe5dd918e, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "sinrev2.08",			0x1000, 0xd7ecee45, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "sinistar.09",		0x1000, 0x50cb63ad, 1 | BRF_PRG | BRF_ESS }, // 10

	{ "speech.ic7",			0x1000, 0xe1019568, 2 | BRF_PRG | BRF_ESS }, // 11 M6800 Code
	{ "speech.ic5",			0x1000, 0xcf3b5ffd, 2 | BRF_PRG | BRF_ESS }, // 12
	{ "speech.ic6",			0x1000, 0xff8d2645, 2 | BRF_PRG | BRF_ESS }, // 13
	{ "speech.ic4",			0x1000, 0x4b56a626, 2 | BRF_PRG | BRF_ESS }, // 14
	{ "sinistar.snd",		0x1000, 0xb82f4ddb, 2 | BRF_PRG | BRF_ESS }, // 15

	{ "decoder.4",			0x0200, 0xe6631c23, 0 | BRF_OPT },           // 16 Address Decoder
	{ "decoder.6",			0x0200, 0x83faf25e, 0 | BRF_OPT },           // 17
};

STD_ROM_PICK(sinistar2)
STD_ROM_FN(sinistar2)

struct BurnDriver BurnDrvSinistar2 = {
	"sinistar2", "sinistar", NULL, NULL, "1982",
	"Sinistar (revision 2)\0", NULL, "Williams", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_SHOOT, 0,
	NULL, sinistar2RomInfo, sinistar2RomName, NULL, NULL, SinistarInputInfo, NULL,
	SinistarInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	240, 292, 3, 4
};


// Lotto Fun

static struct BurnRomInfo lottofunRomDesc[] = {
	{ "vl7a.dat",			0x1000, 0xfb2aec2c, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "vl7c.dat",			0x1000, 0x9a496519, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "vl7e.dat",			0x1000, 0x032cab4b, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "vl4e.dat",			0x1000, 0x5e9af236, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "vl4c.dat",			0x1000, 0x4b134ae2, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "vl4a.dat",			0x1000, 0xb2f1f95a, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "vl5e.dat",			0x1000, 0xc8681c55, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "vl5c.dat",			0x1000, 0xeb9351e0, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "vl5a.dat",			0x1000, 0x534f2fa1, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "vl6e.dat",			0x1000, 0xbefac592, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "vl6c.dat",			0x1000, 0xa73d7f13, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "vl6a.dat",			0x1000, 0x5730a43d, 1 | BRF_PRG | BRF_ESS }, // 11

	{ "vl2532.snd",			0x1000, 0x214b8a04, 2 | BRF_PRG | BRF_ESS }, // 12 M6800 Code
};

STD_ROM_PICK(lottofun)
STD_ROM_FN(lottofun)

static INT32 LottofunInit()
{
	return DrvInit(1, 0, 6, 1, 0xc000);
}

struct BurnDriver BurnDrvLottofun = {
	"lottofun", NULL, NULL, NULL, "1987",
	"Lotto Fun\0", NULL, "H.A.R. Management", "6809 System",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_MISC, 0,
	NULL, lottofunRomInfo, lottofunRomName, NULL, NULL, LottofunInputInfo, NULL,
	LottofunInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x10,
	292, 240, 4, 3
};


// Blaster

static struct BurnRomInfo blasterRomDesc[] = {
	{ "16.ic39",			0x1000, 0x54a40b21, 1 | BRF_PRG | BRF_ESS }, //  0 M6809 Code
	{ "13.ic27",			0x2000, 0xf4dae4c8, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "11.ic25",			0x2000, 0x6371e62f, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "12.ic26",			0x2000, 0x9804faac, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "17.ic41",			0x1000, 0xbf96182f, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "15.ic38",			0x4000, 0x1ad146a4, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "8.ic20",				0x4000, 0xf110bbb0, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "9.ic22",				0x4000, 0x5c5b0f8a, 1 | BRF_PRG | BRF_ESS }, //  7
	{ "10.ic24",			0x4000, 0xd47eb67f, 1 | BRF_PRG | BRF_ESS }, //  8
	{ "6.ic13",				0x4000, 0x47fc007e, 1 | BRF_PRG | BRF_ESS }, //  9
	{ "5.ic11",				0x4000, 0x15c1b94d, 1 | BRF_PRG | BRF_ESS }, // 10
	{ "14.ic35",			0x4000, 0xaea6b846, 1 | BRF_PRG | BRF_ESS }, // 11
	{ "7.ic15",				0x4000, 0x7a101181, 1 | BRF_PRG | BRF_ESS }, // 12
	{ "1.ic1",				0x4000, 0x8d0ea9e7, 1 | BRF_PRG | BRF_ESS }, // 13
	{ "2.ic3",				0x4000, 0x03c4012c, 1 | BRF_PRG | BRF_ESS }, // 14
	{ "4.ic7",				0x4000, 0xfc9d39fb, 1 | BRF_PRG | BRF_ESS }, // 15
	{ "3.ic6",				0x4000, 0x253690fb, 1 | BRF_PRG | BRF_ESS }, // 16

	{ "18.sb13",			0x1000, 0xc33a3145, 2 | BRF_PRG | BRF_ESS }, // 17 M6800 #0 Code

	{ "18.sb10",			0x1000, 0xc33a3145, 3 | BRF_PRG | BRF_ESS }, // 18 M6800 #1 Code

	{ "4.u42",				0x0200, 0xe6631c23, 0 | BRF_OPT },           // 19 proms
	{ "6.u23",				0x0200, 0x83faf25e, 0 | BRF_OPT },           // 20
	{ "blaster.col",		0x0800, 0xbac50bc4, 4 | BRF_GRA },           // 21
};

STD_ROM_PICK(blaster)
STD_ROM_FN(blaster)

static INT32 BlasterInit()
{
	blaster = 1;

	INT32 nRet = DrvInit(2, 0, 6, 2, 0x9700);

	if (nRet == 0)
	{
		pStartDraw = DrvDrawBegin;
		pDrawScanline = BlasterDrawLine;
	}

	return nRet;
}

struct BurnDriver BurnDrvBlaster = {
	"blaster", NULL, NULL, NULL, "1983",
	"Blaster\0", NULL, "Williams / Vid Kidz", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_MISC, 0,
	NULL, blasterRomInfo, blasterRomName, NULL, NULL, BlasterInputInfo, NULL,
	BlasterInit, DrvExit, DrvFrame, BlasterDraw, DrvScan, &DrvRecalc, 0x110,
	292, 240, 4, 3
};





#if 0

GAME( 1983, playball,   0,        playball,       playball, williams_state, init_playball, ROT270, "Williams", "PlayBall! (prototype)", MACHINE_SUPPORTS_SAVE )

GAME( 1983, blaster,    0,        blaster,        blaster,  blaster_state,  init_blaster,  ROT0,   "Williams / Vid Kidz", "Blaster", MACHINE_SUPPORTS_SAVE )
GAME( 1983, blastero,   blaster,  blaster,        blaster,  blaster_state,  init_blaster,  ROT0,   "Williams / Vid Kidz", "Blaster (location test)", MACHINE_SUPPORTS_SAVE )
GAME( 1983, blasterkit, blaster,  blastkit,       blastkit, blaster_state,  init_blaster,  ROT0,   "Williams / Vid Kidz", "Blaster (conversion kit)", MACHINE_SUPPORTS_SAVE ) // mono sound

GAME( 1983, mysticm,    0,        mysticm,        mysticm, williams2_state, init_mysticm,  ROT0,   "Williams", "Mystic Marathon", MACHINE_WRONG_COLORS | MACHINE_SUPPORTS_SAVE)
GAME( 1983, mysticmp,   mysticm,  mysticm,        mysticm, williams2_state, init_mysticm,  ROT0,   "Williams", "Mystic Marathon (prototype)", MACHINE_WRONG_COLORS | MACHINE_SUPPORTS_SAVE ) // newest roms are 'proto 6' ?

GAME( 1984, tshoot,     0,        tshoot,         tshoot,  tshoot_state,    init_tshoot,   ROT0,   "Williams", "Turkey Shoot (prototype)", MACHINE_SUPPORTS_SAVE )

GAME( 1984, inferno,    0,        inferno,        inferno, williams2_state, init_inferno,  ROT0,   "Williams", "Inferno (Williams)", MACHINE_SUPPORTS_SAVE )

GAME( 1986, joust2,     0,        joust2,         joust2,  joust2_state,    init_joust2,   ROT270, "Williams", "Joust 2 - Survival of the Fittest (revision 2)", MACHINE_SUPPORTS_SAVE )
GAME( 1986, joust2r1,   joust2,   joust2,         joust2,  joust2_state,    init_joust2,   ROT270, "Williams", "Joust 2 - Survival of the Fittest (revision 1)", MACHINE_SUPPORTS_SAVE )

#endif
