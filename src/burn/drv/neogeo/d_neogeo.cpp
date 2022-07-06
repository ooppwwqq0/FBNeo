// FBNeo Neo Geo driver module

#include "neogeo.h"
#include "bitswap.h"

#if !defined ROM_VERIFY
	// enable to match Neo Geo BIOS to MAME's (also affects dipswitches -- leaves only the MVS BIOS option)
	// #define ROM_VERIFY
#endif

static struct BurnRomInfo emptyRomDesc[] = {
	{ "",                    0,          0, 0 },
};

// -----------------------------------------------------------------------------
// Common data

static struct BurnInputInfo neogeoInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	NeoButton2 + 0,	"p1 coin"},			//  0
//	{"P1 Coin 2",	BIT_DIGITAL,	NeoButton2 + 3,	"p1 coin 2"},
	{"P1 Start",	BIT_DIGITAL,	NeoButton1 + 0,	"p1 start"},		//	1
	{"P1 Select",	BIT_DIGITAL,	NeoButton1 + 1,	"p1 select"},		//  2

	{"P1 Up",		BIT_DIGITAL,	NeoJoy1 + 0,	"p1 up"},			//  3
	{"P1 Down",		BIT_DIGITAL,	NeoJoy1 + 1,	"p1 down"},			//  4
	{"P1 Left",		BIT_DIGITAL,	NeoJoy1 + 2,	"p1 left"},			//	5
	{"P1 Right",	BIT_DIGITAL,	NeoJoy1 + 3,	"p1 right"},		//	6
	{"P1 Button A",	BIT_DIGITAL,	NeoJoy1 + 4,	"p1 fire 1"},		//	7
	{"P1 Button B",	BIT_DIGITAL,	NeoJoy1 + 5,	"p1 fire 2"},		//	8
	{"P1 Button C",	BIT_DIGITAL,	NeoJoy1 + 6,	"p1 fire 3"},		//	9
	{"P1 Button D",	BIT_DIGITAL,	NeoJoy1 + 7,	"p1 fire 4"},		//	A

	{"P2 Coin",		BIT_DIGITAL,	NeoButton2 + 1,	"p2 coin"},			//	B
//	{"P2 Coin 2",	BIT_DIGITAL,	NeoButton2 + 4,	"p2 coin 2"},
	{"P2 Start",	BIT_DIGITAL,	NeoButton1 + 2,	"p2 start"},		//	C
	{"P2 Select",	BIT_DIGITAL,	NeoButton1 + 3,	"p2 select"},		//	D

	{"P2 Up",		BIT_DIGITAL,	NeoJoy2 + 0,	"p2 up"},			//	E
	{"P2 Down",		BIT_DIGITAL,	NeoJoy2 + 1,	"p2 down"},			//  F
	{"P2 Left",		BIT_DIGITAL,	NeoJoy2 + 2,	"p2 left"},			// 10
	{"P2 Right",	BIT_DIGITAL,	NeoJoy2 + 3,	"p2 right"},		// 11
	{"P2 Button A",	BIT_DIGITAL,	NeoJoy2 + 4,	"p2 fire 1"},		// 12
	{"P2 Button B",	BIT_DIGITAL,	NeoJoy2 + 5,	"p2 fire 2"},		// 13
	{"P2 Button C",	BIT_DIGITAL,	NeoJoy2 + 6,	"p2 fire 3"},		// 14
	{"P2 Button D",	BIT_DIGITAL,	NeoJoy2 + 7,	"p2 fire 4"},		// 15

	{"Reset",		BIT_DIGITAL,	&NeoReset,		"reset"},			// 16
	{"Test",		BIT_DIGITAL,	NeoDiag + 0,	"diag"},			// 17
	{"Service",		BIT_DIGITAL,	NeoButton2 + 2,	"service"},			// 18
	{"Dip 1",		BIT_DIPSWITCH,	NeoInput + 4,	"dip"},				// 19
	{"Dip 2",		BIT_DIPSWITCH,	NeoInput + 5,	"dip"},				// 1A

	{"System",		BIT_DIPSWITCH,	&NeoSystem,		"dip"},				// 1B
	{"Slots",		BIT_DIPSWITCH,	&nNeoNumSlots,	"dip"},				// 1C

	{"Debug Dip 1",	BIT_DIPSWITCH,	NeoDebugDip + 0,"dip"},				// 1D
	{"Debug Dip 2",	BIT_DIPSWITCH,	NeoDebugDip + 1,"dip"},				// 1E
};

STDINPUTINFO(neogeo)

static struct BurnInputInfo neoMVSInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	NeoButton2 + 0,	"p1 coin"},			//  0
//	{"P1 Coin 2",	BIT_DIGITAL,	NeoButton2 + 3,	"p1 coin 2"},
	{"P1 Start",	BIT_DIGITAL,	NeoButton1 + 0,	"p1 start"},		//	1
	{"P1 Select",	BIT_DIGITAL,	NeoButton1 + 1,	"p1 select"},		//  2

	{"P1 Up",		BIT_DIGITAL,	NeoJoy1 + 0,	"p1 up"},			//  3
	{"P1 Down",		BIT_DIGITAL,	NeoJoy1 + 1,	"p1 down"},			//  4
	{"P1 Left",		BIT_DIGITAL,	NeoJoy1 + 2,	"p1 left"},			//	5
	{"P1 Right",	BIT_DIGITAL,	NeoJoy1 + 3,	"p1 right"},		//	6
	{"P1 Button A",	BIT_DIGITAL,	NeoJoy1 + 4,	"p1 fire 1"},		//	7
	{"P1 Button B",	BIT_DIGITAL,	NeoJoy1 + 5,	"p1 fire 2"},		//	8
	{"P1 Button C",	BIT_DIGITAL,	NeoJoy1 + 6,	"p1 fire 3"},		//	9
	{"P1 Button D",	BIT_DIGITAL,	NeoJoy1 + 7,	"p1 fire 4"},		//	A

	{"P2 Coin",		BIT_DIGITAL,	NeoButton2 + 1,	"p2 coin"},			//	B
//	{"P2 Coin 2",	BIT_DIGITAL,	NeoButton2 + 4,	"p2 coin 2"},
	{"P2 Start",	BIT_DIGITAL,	NeoButton1 + 2,	"p2 start"},		//	C
	{"P2 Select",	BIT_DIGITAL,	NeoButton1 + 3,	"p2 select"},		//	D

	{"P2 Up",		BIT_DIGITAL,	NeoJoy2 + 0,	"p2 up"},			//	E
	{"P2 Down",		BIT_DIGITAL,	NeoJoy2 + 1,	"p2 down"},			//  F
	{"P2 Left",		BIT_DIGITAL,	NeoJoy2 + 2,	"p2 left"},			// 10
	{"P2 Right",	BIT_DIGITAL,	NeoJoy2 + 3,	"p2 right"},		// 11
	{"P2 Button A",	BIT_DIGITAL,	NeoJoy2 + 4,	"p2 fire 1"},		// 12
	{"P2 Button B",	BIT_DIGITAL,	NeoJoy2 + 5,	"p2 fire 2"},		// 13
	{"P2 Button C",	BIT_DIGITAL,	NeoJoy2 + 6,	"p2 fire 3"},		// 14
	{"P2 Button D",	BIT_DIGITAL,	NeoJoy2 + 7,	"p2 fire 4"},		// 15

	{"Reset",		BIT_DIGITAL,	&NeoReset,		"reset"},			// 16
	{"Test",		BIT_DIGITAL,	NeoDiag + 0,	"diag"},			// 17
	{"Service",		BIT_DIGITAL,	NeoButton2 + 2,	"service"},			// 18
	{"Dip 1",		BIT_DIPSWITCH,	NeoInput + 4,	"dip"},				// 19
	{"Dip 2",		BIT_DIPSWITCH,	NeoInput + 5,	"dip"},				// 1A

	{"System",		BIT_DIPSWITCH,	&NeoSystem,		"dip"},				// 1B

	{"Debug Dip 1",	BIT_DIPSWITCH,	NeoDebugDip + 0,"dip"},				// 1C
	{"Debug Dip 2",	BIT_DIPSWITCH,	NeoDebugDip + 1,"dip"},				// 1D
};

STDINPUTINFO(neoMVS)

static struct BurnInputInfo neoForceAESInputList[] = {
	{"P1 Start",	BIT_DIGITAL,	NeoButton1 + 0,	"p1 start"},		//	0
	{"P1 Select",	BIT_DIGITAL,	NeoButton1 + 1,	"p1 select"},		//  1

	{"P1 Up",		BIT_DIGITAL,	NeoJoy1 + 0,	"p1 up"},			//  2
	{"P1 Down",		BIT_DIGITAL,	NeoJoy1 + 1,	"p1 down"},			//  3
	{"P1 Left",		BIT_DIGITAL,	NeoJoy1 + 2,	"p1 left"},			//	4
	{"P1 Right",	BIT_DIGITAL,	NeoJoy1 + 3,	"p1 right"},		//	5
	{"P1 Button A",	BIT_DIGITAL,	NeoJoy1 + 4,	"p1 fire 1"},		//	6
	{"P1 Button B",	BIT_DIGITAL,	NeoJoy1 + 5,	"p1 fire 2"},		//	7
	{"P1 Button C",	BIT_DIGITAL,	NeoJoy1 + 6,	"p1 fire 3"},		//	8
	{"P1 Button D",	BIT_DIGITAL,	NeoJoy1 + 7,	"p1 fire 4"},		//	9

	{"P2 Start",	BIT_DIGITAL,	NeoButton1 + 2,	"p2 start"},		//	A
	{"P2 Select",	BIT_DIGITAL,	NeoButton1 + 3,	"p2 select"},		//	B

	{"P2 Up",		BIT_DIGITAL,	NeoJoy2 + 0,	"p2 up"},			//	C
	{"P2 Down",		BIT_DIGITAL,	NeoJoy2 + 1,	"p2 down"},			//  D
	{"P2 Left",		BIT_DIGITAL,	NeoJoy2 + 2,	"p2 left"},			//  E
	{"P2 Right",	BIT_DIGITAL,	NeoJoy2 + 3,	"p2 right"},		//  F
	{"P2 Button A",	BIT_DIGITAL,	NeoJoy2 + 4,	"p2 fire 1"},		// 10
	{"P2 Button B",	BIT_DIGITAL,	NeoJoy2 + 5,	"p2 fire 2"},		// 11
	{"P2 Button C",	BIT_DIGITAL,	NeoJoy2 + 6,	"p2 fire 3"},		// 12
	{"P2 Button D",	BIT_DIGITAL,	NeoJoy2 + 7,	"p2 fire 4"},		// 13

	{"Reset",		BIT_DIGITAL,	&NeoReset,		"reset"},			// 14
	{"Dip 1",		BIT_DIPSWITCH,	NeoInput + 4,	"dip"},				// 15
	{"Dip 2",		BIT_DIPSWITCH,	NeoInput + 5,	"dip"},				// 16

	{"System",		BIT_DIPSWITCH,	&NeoSystem,		"dip"},				// 17
	{"Slots",		BIT_DIPSWITCH,	&nNeoNumSlots,	"dip"},				// 18
};

STDINPUTINFO(neoForceAES)

static struct BurnDIPInfo ms5pcbDIPList[] = {
	// Offset
	{0x19,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x02,	0xFF, 0xFF,	0x86, NULL},

	// DIP 1
	{0,		0xFE, 0,	2,	  "Setting mode"},
	{0x00,	0x01, 0x01,	0x00, "Off"},
	{0x00,	0x01, 0x01,	0x01, "On"},

	// 1 or 2 coin slots
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x00,	0x02, 0x02,	0x00, "1"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	{0x00,	0x02, 0x02,	0x02, "2"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	// 2 or 4 coin slots
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x00,	0x82, 0x02,	0x00, "2"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	{0x00,	0x82, 0x02,	0x02, "4"},
	{0x02,	0x00, 0x3F,	0x06, NULL},

	{0,		0xFE, 0,	5,	  "Commmunicaton"},
	{0x00,	0x01, 0x38,	0x00, "Disabled"},
	{0x00,	0x01, 0x38,	0x08, "Setting 1"},
	{0x00,	0x01, 0x38,	0x10, "Setting 2"},
	{0x00,	0x01, 0x38,	0x18, "Setting 3"},
	{0x00,	0x01, 0x38,	0x20, "Setting 4"},
	{0x00,	0x01, 0x38,	0x28, "Setting 5"},
	{0x00,	0x01, 0x38,	0x30, "Setting 6"},
	{0x00,	0x01, 0x38,	0x38, "Setting 7"},
	{0,		0xFE, 0,	2,	  "Free play"},
	{0x00,	0x01, 0x40,	0x00, "Off"},
	{0x00,	0x01, 0x40,	0x40, "On"},
	{0,		0xFE, 0,	2,	  "Stop mode"},
	{0x00,	0x01, 0x80,	0x00, "Off"},
	{0x00,	0x01, 0x80,	0x80, "On"},

	// Region
	{0,		0xFD, 0,	2,	  "Region"},
	{0x02,	0x02, 0x03,	0x02, "Asia / Europe"},
	{0x02,	0x00, 0x30,	0x00, NULL},
	{0x02,	0x02, 0x03,	0x03, "Japan"},
	{0x02,	0x00, 0x30,	0x00, NULL},
};

STDDIPINFO(ms5pcb)

static struct BurnDIPInfo svcpcbDIPList[] = {
	// Offset
	{0x19,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x02,	0xFF, 0xFF,	0x86, NULL},

	// DIP 1
	{0,		0xFE, 0,	2,	  "Setting mode"},
	{0x00,	0x01, 0x01,	0x00, "Off"},
	{0x00,	0x01, 0x01,	0x01, "On"},

	// 1 or 2 coin slots
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x00,	0x02, 0x02,	0x00, "1"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	{0x00,	0x02, 0x02,	0x02, "2"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	// 2 or 4 coin slots
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x00,	0x82, 0x02,	0x00, "2"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	{0x00,	0x82, 0x02,	0x02, "4"},
	{0x02,	0x00, 0x3F,	0x06, NULL},

	{0,		0xFE, 0,	5,	  "Commmunicaton"},
	{0x00,	0x01, 0x38,	0x00, "Disabled"},
	{0x00,	0x01, 0x38,	0x08, "Setting 1"},
	{0x00,	0x01, 0x38,	0x10, "Setting 2"},
	{0x00,	0x01, 0x38,	0x18, "Setting 3"},
	{0x00,	0x01, 0x38,	0x20, "Setting 4"},
	{0x00,	0x01, 0x38,	0x28, "Setting 5"},
	{0x00,	0x01, 0x38,	0x30, "Setting 6"},
	{0x00,	0x01, 0x38,	0x38, "Setting 7"},
	{0,		0xFE, 0,	2,	  "Free play"},
	{0x00,	0x01, 0x40,	0x00, "Off"},
	{0x00,	0x01, 0x40,	0x40, "On"},
	{0,		0xFE, 0,	2,	  "Stop mode"},
	{0x00,	0x01, 0x80,	0x00, "Off"},
	{0x00,	0x01, 0x80,	0x80, "On"},

	// Region
	{0,		0xFD, 0,	2,	  "Region"},
	{0x02,	0x02, 0x03,	0x02, "Asia / Europe"},
	{0x02,	0x00, 0x30,	0x00, NULL},
	{0x02,	0x02, 0x03,	0x03, "Japan"},
	{0x02,	0x00, 0x30,	0x00, NULL},
};

STDDIPINFO(svcpcb)

static struct BurnDIPInfo kf2k3pcbDIPList[] = {
	// Offset
	{0x19,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x02,	0xFF, 0xFF,	0x86, NULL},

	// DIP 1
	{0,		0xFE, 0,	2,	  "Setting mode"},
	{0x00,	0x01, 0x01,	0x00, "Off"},
	{0x00,	0x01, 0x01,	0x01, "On"},

	// 1 or 2 coin slots
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x00,	0x02, 0x02,	0x00, "1"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	{0x00,	0x02, 0x02,	0x02, "2"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	// 2 or 4 coin slots
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x00,	0x82, 0x02,	0x00, "2"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	{0x00,	0x82, 0x02,	0x02, "4"},
	{0x02,	0x00, 0x3F,	0x06, NULL},

	{0,		0xFE, 0,	8,	  "Commmunicaton"},
	{0x00,	0x01, 0x38,	0x00, "Disabled"},
	{0x00,	0x01, 0x38,	0x08, "Setting 1"},
	{0x00,	0x01, 0x38,	0x10, "Setting 2"},
	{0x00,	0x01, 0x38,	0x18, "Setting 3"},
	{0x00,	0x01, 0x38,	0x20, "Setting 4"},
	{0x00,	0x01, 0x38,	0x28, "Setting 5"},
	{0x00,	0x01, 0x38,	0x30, "Setting 6"},
	{0x00,	0x01, 0x38,	0x38, "Setting 7"},
	{0,		0xFE, 0,	2,	  "Free play"},
	{0x00,	0x01, 0x40,	0x00, "Off"},
	{0x00,	0x01, 0x40,	0x40, "On"},
	{0,		0xFE, 0,	2,	  "Stop mode"},
	{0x00,	0x01, 0x80,	0x00, "Off"},
	{0x00,	0x01, 0x80,	0x80, "On"},
};

STDDIPINFO(kf2k3pcb)

static struct BurnInputInfo neoCDInputList[] = {
	{"P1 Start",	BIT_DIGITAL,	NeoButton1 + 0,	"p1 start"},		//	0
	{"P1 Select",	BIT_DIGITAL,	NeoButton1 + 1,	"p1 select"},		//  1

	{"P1 Up",		BIT_DIGITAL,	NeoJoy1 + 0,	"p1 up"},			//  2
	{"P1 Down",		BIT_DIGITAL,	NeoJoy1 + 1,	"p1 down"},			//  3
	{"P1 Left",		BIT_DIGITAL,	NeoJoy1 + 2,	"p1 left"},			//	4
	{"P1 Right",	BIT_DIGITAL,	NeoJoy1 + 3,	"p1 right"},		//	5
	{"P1 Button A",	BIT_DIGITAL,	NeoJoy1 + 4,	"p1 fire 1"},		//	6
	{"P1 Button B",	BIT_DIGITAL,	NeoJoy1 + 5,	"p1 fire 2"},		//	7
	{"P1 Button C",	BIT_DIGITAL,	NeoJoy1 + 6,	"p1 fire 3"},		//	8
	{"P1 Button D",	BIT_DIGITAL,	NeoJoy1 + 7,	"p1 fire 4"},		//	9

	{"P2 Start",	BIT_DIGITAL,	NeoButton1 + 2,	"p2 start"},		//	A
	{"P2 Select",	BIT_DIGITAL,	NeoButton1 + 3,	"p2 select"},		//	B

	{"P2 Up",		BIT_DIGITAL,	NeoJoy2 + 0,	"p2 up"},			//	C
	{"P2 Down",		BIT_DIGITAL,	NeoJoy2 + 1,	"p2 down"},			//  D
	{"P2 Left",		BIT_DIGITAL,	NeoJoy2 + 2,	"p2 left"},			//  E
	{"P2 Right",	BIT_DIGITAL,	NeoJoy2 + 3,	"p2 right"},		//  F
	{"P2 Button A",	BIT_DIGITAL,	NeoJoy2 + 4,	"p2 fire 1"},		// 10
	{"P2 Button B",	BIT_DIGITAL,	NeoJoy2 + 5,	"p2 fire 2"},		// 11
	{"P2 Button C",	BIT_DIGITAL,	NeoJoy2 + 6,	"p2 fire 3"},		// 12
	{"P2 Button D",	BIT_DIGITAL,	NeoJoy2 + 7,	"p2 fire 4"},		// 13

	{"Reset",		BIT_DIGITAL,	&NeoReset,		"reset"},			// 14
	{"Test",		BIT_DIGITAL,	NeoDiag + 0,	"diag"},			// 15

	{"System",		BIT_DIPSWITCH,	&NeoSystem,		"dip"},				// 16
};

STDINPUTINFO(neoCD)

#define A(a, b, c, d) { a, b, (UINT8*)(c), d }

static struct BurnInputInfo neopaddleInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	NeoButton2 + 0,	"p1 coin"},			//  0
	{"P1 Start",	BIT_DIGITAL,	NeoButton1 + 0,	"p1 start"},		//	1
	{"P1 Select",	BIT_DIGITAL,	NeoButton1 + 1,	"p1 select"},		//  2

	{"P1 Up",		BIT_DIGITAL,	NeoJoy1 + 0,	"p1 up"},			//  3
	{"P1 Down",		BIT_DIGITAL,	NeoJoy1 + 1,	"p1 down"},			//  4
	{"P1 Left",		BIT_DIGITAL,	NeoJoy1 + 2,	"p1 left"},			//	5
	{"P1 Right",	BIT_DIGITAL,	NeoJoy1 + 3,	"p1 right"},		//	6
	{"P1 Button A",	BIT_DIGITAL,	NeoJoy1 + 4,	"p1 fire 1"},		//	7
	{"P1 Button B",	BIT_DIGITAL,	NeoJoy1 + 5,	"p1 fire 2"},		//	8
	{"P1 Button C",	BIT_DIGITAL,	NeoJoy1 + 6,	"p1 fire 3"},		//	9
	{"P1 Button D",	BIT_DIGITAL,	NeoJoy1 + 7,	"mouse button 1"},	//	A
   A("P1 Paddle",	BIT_ANALOG_REL, NeoAxis + 0,	"mouse x-axis"),	//	B

	{"P2 Coin",		BIT_DIGITAL,	NeoButton2 + 1,	"p2 coin"},			//	C
	{"P2 Start",	BIT_DIGITAL,	NeoButton1 + 2,	"p2 start"},		//	D
	{"P2 Select",	BIT_DIGITAL,	NeoButton1 + 3,	"p2 select"},		//	E

	{"P2 Up",		BIT_DIGITAL,	NeoJoy2 + 0,	"p2 up"},			//	F
	{"P2 Down",		BIT_DIGITAL,	NeoJoy2 + 1,	"p2 down"},			// 10
	{"P2 Left",		BIT_DIGITAL,	NeoJoy2 + 2,	"p2 left"},			// 11
	{"P2 Right",	BIT_DIGITAL,	NeoJoy2 + 3,	"p2 right"},		// 12
	{"P2 Button A",	BIT_DIGITAL,	NeoJoy2 + 4,	"p2 fire 1"},		// 13
	{"P2 Button B",	BIT_DIGITAL,	NeoJoy2 + 5,	"p2 fire 2"},		// 14
	{"P2 Button C",	BIT_DIGITAL,	NeoJoy2 + 6,	"p2 fire 3"},		// 15
	{"P2 Button D",	BIT_DIGITAL,	NeoJoy2 + 7,	"p2 fire 4"},		// 16
   A("P2 Paddle",	BIT_ANALOG_REL, NeoAxis + 1,	"p2 x-axis"),		// 17

	{"Reset",		BIT_DIGITAL,	&NeoReset,		"reset"},			// 18
	{"Test",		BIT_DIGITAL,	NeoDiag + 0,	"diag"},			// 19
	{"Service",		BIT_DIGITAL,	NeoButton2 + 2,	"service"},			// 1A
	{"Dip 1",		BIT_DIPSWITCH,	NeoInput + 4,	"dip"},				// 1B
	{"Dip 2",		BIT_DIPSWITCH,	NeoInput + 5,	"dip"},				// 1C

	{"System",		BIT_DIPSWITCH,	&NeoSystem,		"dip"},				// 1D
	{"Slots",		BIT_DIPSWITCH,	&nNeoNumSlots,	"dip"},				// 1E

	{"Debug Dip 1",	BIT_DIPSWITCH,	NeoDebugDip + 0,"dip"},				// 1F
	{"Debug Dip 2",	BIT_DIPSWITCH,	NeoDebugDip + 1,"dip"},				// 1G
};

STDINPUTINFO(neopaddle)

static struct BurnInputInfo neotrackballInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	NeoButton2 + 0,	"p1 coin"},			//  0
	{"P1 Start",	BIT_DIGITAL,	NeoButton1 + 0,	"p1 start"},		//	1

   A("P1 X Axis",	BIT_ANALOG_REL,	NeoAxis + 0,	"mouse x-axis"),	//	2
   A("P1 Y Axis",	BIT_ANALOG_REL,	NeoAxis + 1,	"mouse y-axis"),	//	3

	{"P1 Button A",	BIT_DIGITAL,	NeoJoy2 + 4,	"mouse button 1"},	//	4
	{"P1 Button B",	BIT_DIGITAL,	NeoJoy2 + 5,	"mouse button 2"},	//	5

	{"P2 Coin",		BIT_DIGITAL,	NeoButton2 + 1,	"p2 coin"},			//	6
	{"P2 Start",	BIT_DIGITAL,	NeoButton1 + 2,	"p2 start"},		//	7

	{"P2 Button A",	BIT_DIGITAL,	NeoJoy2 + 6,	"p2 fire 1"},		//  8
	{"P2 Button B",	BIT_DIGITAL,	NeoJoy2 + 7,	"p2 fire 2"},		//  9

	{"Reset",		BIT_DIGITAL,	&NeoReset,		"reset"},			//  A
	{"Test",		BIT_DIGITAL,	NeoDiag + 0,	"diag"},			//  B
	{"Service",		BIT_DIGITAL,	NeoButton2 + 2,	"service"},			//  C
	{"Dip 1",		BIT_DIPSWITCH,	NeoInput + 4,	"dip"},				//  D
	{"Dip 2",		BIT_DIPSWITCH,	NeoInput + 5,	"dip"},				//  E

	{"System",		BIT_DIPSWITCH,	&NeoSystem,		"dip"},				//  F
	{"Slots",		BIT_DIPSWITCH,	&nNeoNumSlots,	"dip"},				// 10

	{"Debug Dip 1",	BIT_DIPSWITCH,	NeoDebugDip + 0,"dip"},				// 11
	{"Debug Dip 2",	BIT_DIPSWITCH,	NeoDebugDip + 1,"dip"},				// 12
};

STDINPUTINFO(neotrackball)

#undef A

static struct BurnInputInfo neodualInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	NeoButton2 + 0,	"p1 coin"},			//  0
	{"P1 Start",	BIT_DIGITAL,	NeoButton1 + 0,	"p1 start"},		//	1
	{"P1 Select",	BIT_DIGITAL,	NeoButton1 + 1,	"p1 select"},		//  2

	{"P1 Up",		BIT_DIGITAL,	NeoJoy1 + 0,	"p1 up"},			//  3
	{"P1 Down",		BIT_DIGITAL,	NeoJoy1 + 1,	"p1 down"},			//  4
	{"P1 Left",		BIT_DIGITAL,	NeoJoy1 + 2,	"p1 left"},			//	5
	{"P1 Right",	BIT_DIGITAL,	NeoJoy1 + 3,	"p1 right"},		//	6
	{"P1 Button A",	BIT_DIGITAL,	NeoJoy1 + 4,	"p1 fire 1"},		//	7
	{"P1 Button B",	BIT_DIGITAL,	NeoJoy1 + 5,	"p1 fire 2"},		//	8
	{"P1 Button C",	BIT_DIGITAL,	NeoJoy1 + 6,	"p1 fire 3"},		//	9
	{"P1 Button D",	BIT_DIGITAL,	NeoJoy1 + 7,	"p1 fire 4"},		//	A

	{"P2 Coin",		BIT_DIGITAL,	NeoButton2 + 1,	"p2 coin"},			//	B
	{"P2 Start",	BIT_DIGITAL,	NeoButton1 + 2,	"p2 start"},		//	C
	{"P2 Select",	BIT_DIGITAL,	NeoButton1 + 3,	"p2 select"},		//	D

	{"P2 Up",		BIT_DIGITAL,	NeoJoy2 + 0,	"p2 up"},			//	E
	{"P2 Down",		BIT_DIGITAL,	NeoJoy2 + 1,	"p2 down"},			//  F
	{"P2 Left",		BIT_DIGITAL,	NeoJoy2 + 2,	"p2 left"},			// 10
	{"P2 Right",	BIT_DIGITAL,	NeoJoy2 + 3,	"p2 right"},		// 11
	{"P2 Button A",	BIT_DIGITAL,	NeoJoy2 + 4,	"p2 fire 1"},		// 12
	{"P2 Button B",	BIT_DIGITAL,	NeoJoy2 + 5,	"p2 fire 2"},		// 13
	{"P2 Button C",	BIT_DIGITAL,	NeoJoy2 + 6,	"p2 fire 3"},		// 14
	{"P2 Button D",	BIT_DIGITAL,	NeoJoy2 + 7,	"p2 fire 4"},		// 15

	{"P3 Coin",		BIT_DIGITAL,	NeoButton4 + 0,	"p3 coin"},			// 16
	{"P3 Start",	BIT_DIGITAL,	NeoButton3 + 0,	"p3 start"},		// 17
	{"P3 Select",	BIT_DIGITAL,	NeoButton3 + 1,	"p3 select"},		// 18

	{"P3 Up",		BIT_DIGITAL,	NeoJoy3 + 0,	"p3 up"},			// 19
	{"P3 Down",		BIT_DIGITAL,	NeoJoy3 + 1,	"p3 down"},			// 1A
	{"P3 Left",		BIT_DIGITAL,	NeoJoy3 + 2,	"p3 left"},			// 1B
	{"P3 Right",	BIT_DIGITAL,	NeoJoy3 + 3,	"p3 right"},		// 1C
	{"P3 Button A",	BIT_DIGITAL,	NeoJoy3 + 4,	"p3 fire 1"},		// 1D
	{"P3 Button B",	BIT_DIGITAL,	NeoJoy3 + 5,	"p3 fire 2"},		// 1E
	{"P3 Button C",	BIT_DIGITAL,	NeoJoy3 + 6,	"p3 fire 3"},		// 1F
	{"P3 Button D",	BIT_DIGITAL,	NeoJoy3 + 7,	"p3 fire 4"},		// 20

	{"P4 Coin",		BIT_DIGITAL,	NeoButton4 + 1,	"p4 coin"},			// 21
	{"P4 Start",	BIT_DIGITAL,	NeoButton3 + 2,	"p4 start"},		// 22
	{"P4 Select",	BIT_DIGITAL,	NeoButton3 + 3,	"p4 select"},		// 23

	{"P4 Up",		BIT_DIGITAL,	NeoJoy4 + 0,	"p4 up"},			// 24
	{"P4 Down",		BIT_DIGITAL,	NeoJoy4 + 1,	"p4 down"},			// 25
	{"P4 Left",		BIT_DIGITAL,	NeoJoy4 + 2,	"p4 left"},			// 26
	{"P4 Right",	BIT_DIGITAL,	NeoJoy4 + 3,	"p4 right"},		// 27
	{"P4 Button A",	BIT_DIGITAL,	NeoJoy4 + 4,	"p4 fire 1"},		// 28
	{"P4 Button B",	BIT_DIGITAL,	NeoJoy4 + 5,	"p4 fire 2"},		// 29
	{"P4 Button C",	BIT_DIGITAL,	NeoJoy4 + 6,	"p4 fire 3"},		// 2A
	{"P4 Button D",	BIT_DIGITAL,	NeoJoy4 + 7,	"p4 fire 4"},		// 2B

	{"Reset",		BIT_DIGITAL,	&NeoReset,		"reset"},			// 2C

	{"Test 1",		BIT_DIGITAL,	NeoDiag + 0,	"diag"},			// 2D
	{"Service 1",	BIT_DIGITAL,	NeoButton2 + 2,	"service"},			// 2E
	{"Test 2",		BIT_DIGITAL,	NeoDiag + 1,	""},				// 2F
	{"Service 2",	BIT_DIGITAL,	NeoButton4 + 2,	""},				// 30

	{"Dip 1",		BIT_DIPSWITCH,	NeoInput + 4,	"dip"},				// 31
	{"Dip 2",		BIT_DIPSWITCH,	NeoInput + 5,	"dip"},				// 32

	{"System",		BIT_DIPSWITCH,	&NeoSystem,		"dip"},				// 33
	{"Slots",		BIT_DIPSWITCH,	&nNeoNumSlots,	"dip"},				// 34

	{"Debug Dip 1",	BIT_DIPSWITCH,	NeoDebugDip + 0,"dip"},				// 35
	{"Debug Dip 2",	BIT_DIPSWITCH,	NeoDebugDip + 1,"dip"},				// 36
};

STDINPUTINFO(neodual)

static struct BurnInputInfo neomahjongInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	NeoButton2 + 0,	"p1 coin"},			//  0
	{"P1 Start",	BIT_DIGITAL,	NeoButton1 + 0,	"p1 start"},		//	1
	{"P1 Select",	BIT_DIGITAL,	NeoButton1 + 1,	"p1 select"},		//  2

	{"P1 Up",		BIT_DIGITAL,	NeoJoy1 + 0,	"p1 up"},			//  3
	{"P1 Down",		BIT_DIGITAL,	NeoJoy1 + 1,	"p1 down"},			//  4
	{"P1 Left",		BIT_DIGITAL,	NeoJoy1 + 2,	"p1 left"},			//	5
	{"P1 Right",	BIT_DIGITAL,	NeoJoy1 + 3,	"p1 right"},		//	6
	{"P1 Button A",	BIT_DIGITAL,	NeoJoy1 + 4,	"p1 fire 1"},		//	7
	{"P1 Button B",	BIT_DIGITAL,	NeoJoy1 + 5,	"p1 fire 2"},		//	8
	{"P1 Button C",	BIT_DIGITAL,	NeoJoy1 + 6,	"p1 fire 3"},		//	9
	{"P1 Button D",	BIT_DIGITAL,	NeoJoy1 + 7,	"p1 fire 4"},		//	A

	{"A",			BIT_DIGITAL,	NeoButton1 +  8,	"key tab"},		//  B
	{"B",			BIT_DIGITAL,	NeoButton1 +  9,	"key q"},		//  C
	{"C",			BIT_DIGITAL,	NeoButton1 + 10,	"key w"},		//  D
	{"D",			BIT_DIGITAL,	NeoButton1 + 11,	"key e"},		//  E
	{"E",			BIT_DIGITAL,	NeoButton1 + 12,	"key r"},		//  F
	{"F",			BIT_DIGITAL,	NeoButton1 + 13,	"key t"},		// 10
	{"G",			BIT_DIGITAL,	NeoButton1 + 14,	"key y"},		// 11
	{"H",			BIT_DIGITAL,	NeoButton1 + 16,	"key u"},		// 12
	{"I",			BIT_DIGITAL,	NeoButton1 + 17,	"key i"},		// 13
	{"J",			BIT_DIGITAL,	NeoButton1 + 18,	"key o"},		// 14
	{"K",			BIT_DIGITAL,	NeoButton1 + 19,	"key p"},		// 15
	{"L",			BIT_DIGITAL,	NeoButton1 + 20,	"key ["},		// 16
	{"M",			BIT_DIGITAL,	NeoButton1 + 21,	"key ]"},		// 17
	{"N",			BIT_DIGITAL,	NeoButton1 + 22,	"key backslash"},		// 18
	{"Pon",			BIT_DIGITAL,	NeoButton1 + 24,	"key a"},		// 19
	{"Chi",			BIT_DIGITAL,	NeoButton1 + 25,	"key s"},		// 1A
	{"Kan",			BIT_DIGITAL,	NeoButton1 + 26,	"key d"},		// 1B
	{"Ron",			BIT_DIGITAL,	NeoButton1 + 27,	"key f"},		// 1C
	{"Reach",		BIT_DIGITAL,	NeoButton1 + 28,	"key g"},		// 1D

	{"P2 Coin",		BIT_DIGITAL,	NeoButton2 + 1,	"p2 coin"},			// 1E
	{"P2 Start",	BIT_DIGITAL,	NeoButton1 + 2,	"p2 start"},		// 1F
	{"P2 Select",	BIT_DIGITAL,	NeoButton1 + 3,	"p2 select"},		// 20

	{"P2 Up",		BIT_DIGITAL,	NeoJoy2 + 0,	"p2 up"},			// 21
	{"P2 Down",		BIT_DIGITAL,	NeoJoy2 + 1,	"p2 down"},			// 22
	{"P2 Left",		BIT_DIGITAL,	NeoJoy2 + 2,	"p2 left"},			// 23
	{"P2 Right",	BIT_DIGITAL,	NeoJoy2 + 3,	"p2 right"},		// 24
	{"P2 Button A",	BIT_DIGITAL,	NeoJoy2 + 4,	"p2 fire 1"},		// 25
	{"P2 Button B",	BIT_DIGITAL,	NeoJoy2 + 5,	"p2 fire 2"},		// 26
	{"P2 Button C",	BIT_DIGITAL,	NeoJoy2 + 6,	"p2 fire 3"},		// 27
	{"P2 Button D",	BIT_DIGITAL,	NeoJoy2 + 7,	"p2 fire 4"},		// 28

	{"Reset",		BIT_DIGITAL,	&NeoReset,		"reset"},			// 29
	{"Test",		BIT_DIGITAL,	NeoDiag + 0,	"diag"},			// 2A
	{"Service",		BIT_DIGITAL,	NeoButton2 + 2,	"service"},			// 2B
	{"Dip 1",		BIT_DIPSWITCH,	NeoInput + 4,	"dip"},				// 2C
	{"Dip 2",		BIT_DIPSWITCH,	NeoInput + 5,	"dip"},				// 2D

	{"System",		BIT_DIPSWITCH,	&NeoSystem,		"dip"},				// 2E
	{"Slots",		BIT_DIPSWITCH,	&nNeoNumSlots,	"dip"},				// 2F

	{"Debug Dip 1",	BIT_DIPSWITCH,	NeoDebugDip + 0,"dip"},				// 30
	{"Debug Dip 2",	BIT_DIPSWITCH,	NeoDebugDip + 1,"dip"},				// 31
};

STDINPUTINFO(neomahjong)

static struct BurnInputInfo vlinerInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	NeoButton2 + 0,	"p1 coin"},
	{"P1 Payout",	BIT_DIGITAL,	NeoButton1 + 0,	"p1 start"},

	{"P1 Up",		BIT_DIGITAL,	NeoJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	NeoJoy1 + 1,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	NeoJoy1 + 2,	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	NeoJoy1 + 3,	"p1 right"},
	{"P1 Big",	BIT_DIGITAL,	NeoJoy1 + 4,	"p1 fire 1"},
	{"P1 Small",	BIT_DIGITAL,	NeoJoy1 + 5,	"p1 fire 2"},
	{"P1 Double Up",	BIT_DIGITAL,	NeoJoy1 + 6,	"p1 fire 3"},
	{"P1 Start/Collect",	BIT_DIGITAL,	NeoJoy1 + 7,	"p1 fire 4"},

	{"P2 Coin",		BIT_DIGITAL,	NeoButton2 + 1,	"p2 coin"},
	{"P2 Start",	BIT_DIGITAL,	NeoButton1 + 2,	"p2 start"},

	{"P2 Up",		BIT_DIGITAL,	NeoJoy2 + 0,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	NeoJoy2 + 1,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	NeoJoy2 + 2,	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	NeoJoy2 + 3,	"p2 right"},
	{"P2 Button A",	BIT_DIGITAL,	NeoJoy2 + 4,	"p2 fire 1"},
	{"P2 Button B",	BIT_DIGITAL,	NeoJoy2 + 5,	"p2 fire 2"},
	{"P2 Button C",	BIT_DIGITAL,	NeoJoy2 + 6,	"p2 fire 3"},
	{"P2 Button D",	BIT_DIGITAL,	NeoJoy2 + 7,	"p2 fire 4"},

	{"Operator Menu",	BIT_DIGITAL,	NeoButton2 + 4,	"op menu"},
	{"Clear Credit",	BIT_DIGITAL,	NeoButton2 + 5,	"clear credit"},
	{"Hopper Out",		BIT_DIGITAL,	NeoButton2 + 7,	"hopper"},

	{"Reset",		BIT_DIGITAL,	&NeoReset,		"reset"},
	{"Test",		BIT_DIGITAL,	NeoDiag + 0,	"diag"},
	{"Dip 1",		BIT_DIPSWITCH,	NeoInput + 4,	"dip"},
	{"Dip 2",		BIT_DIPSWITCH,	NeoInput + 5,	"dip"},

	{"System",		BIT_DIPSWITCH,	&NeoSystem,		"dip"},
	{"Slots",		BIT_DIPSWITCH,	&nNeoNumSlots,	"dip"},

	{"Debug Dip 1",	BIT_DIPSWITCH,	NeoDebugDip + 0,"dip"},
	{"Debug Dip 2",	BIT_DIPSWITCH,	NeoDebugDip + 1,"dip"},
};

STDINPUTINFO(vliner)

static struct BurnDIPInfo neodefaultDIPList[] = {
	// Offset
	{0x19,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x02,	0xFF, 0xFF,	0x80, NULL},
	{0x03,	0xFF, 0xFF,	0x01, NULL},

	{0,		0xFE, 0,	2,	  "Autofire"},
	{0x00,	0x01, 0x04,	0x00, "Off"},
	{0x00,	0x01, 0x04,	0x04, "On"},
};

static struct BurnDIPInfo neoaesdefaultDIPList[] = {
	// Offset
	{0x19,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x02,	0xFF, 0xFF,	0x8f, NULL},  // aes_jpn
	{0x03,	0xFF, 0xFF,	0x02, NULL},

	{0,		0xFE, 0,	2,	  "Autofire"},
	{0x00,	0x01, 0x04,	0x00, "Off"},
	{0x00,	0x01, 0x04,	0x04, "On"},
};


static struct BurnDIPInfo neousadefaultDIPList[] = {
	// Offset
	{0x19,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x02,	0xFF, 0xFF,	0x83, NULL},
	{0x03,	0xFF, 0xFF,	0x02, NULL},

	{0,		0xFE, 0,	2,	  "Autofire"},
	{0x00,	0x01, 0x04,	0x00, "Off"},
	{0x00,	0x01, 0x04,	0x04, "On"},
};

static struct BurnDIPInfo neounidefaultDIPList[] = {
	// Offset
	{0x19,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x02,	0xFF, 0xFF,	0x8f, NULL},
	{0x03,	0xFF, 0xFF,	0x02, NULL},

	{0,		0xFE, 0,	2,	  "Autofire"},
	{0x00,	0x01, 0x04,	0x00, "Off"},
	{0x00,	0x01, 0x04,	0x04, "On"},
};

static struct BurnDIPInfo neopaddleDIPList[] = {
	// Offset
	{0x1B,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x02,	0xFF, 0xFF,	0x80, NULL},
	{0x03,	0xFF, 0xFF,	0x01, NULL},
};

static struct BurnDIPInfo neodualDIPList[] = {
	// Offset
	{0x31,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x02,	0xFF, 0xFF,	0x80, NULL},
	{0x03,	0xFF, 0xFF,	0x01, NULL},

	{0,		0xFE, 0,	2,	  "Autofire"},
	{0x00,	0x01, 0x04,	0x00, "Off"},
	{0x00,	0x01, 0x04,	0x04, "On"},
};

static struct BurnDIPInfo neomahjongDIPList[] = {
	// Offset
	{0x2C,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x02,	0xFF, 0xFF,	0x80, NULL},
	{0x03,	0xFF, 0xFF,	0x01, NULL},

	{0,		0xFE, 0,	2,	  "Mahjong control panel"},
	{0x00,	0x01, 0x04,	0x00, "Off"},
	{0x00,	0x01, 0x04,	0x04, "On"},
};

static struct BurnDIPInfo neogeoDIPList[] = {
	// DIP 1
	{0,		0xFE, 0,	2,	  "Setting mode"},
	{0x00,	0x01, 0x01,	0x00, "Off"},
	{0x00,	0x01, 0x01,	0x01, "On"},

	// 1 or 2 coin slots
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x00,	0x02, 0x02,	0x00, "1"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	{0x00,	0x02, 0x02,	0x02, "2"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	// 2 or 4 coin slots
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x00,	0x82, 0x02,	0x00, "2"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	{0x00,	0x82, 0x02,	0x02, "4"},
	{0x02,	0x00, 0x3F,	0x06, NULL},

	{0,		0xFE, 0,	5,	  "Commmunicaton"},
	{0x00,	0x01, 0x38,	0x00, "Disabled"},
	{0x00,	0x01, 0x38,	0x08, "Setting 1"},
	{0x00,	0x01, 0x38,	0x10, "Setting 2"},
	{0x00,	0x01, 0x38,	0x18, "Setting 3"},
	{0x00,	0x01, 0x38,	0x20, "Setting 4"},
	{0x00,	0x01, 0x38,	0x28, "Setting 5"},
	{0x00,	0x01, 0x38,	0x30, "Setting 6"},
	{0x00,	0x01, 0x38,	0x38, "Setting 7"},
	{0,		0xFE, 0,	2,	  "Free play"},
	{0x00,	0x01, 0x40,	0x00, "Off"},
	{0x00,	0x01, 0x40,	0x40, "On"},
	{0,		0xFE, 0,	2,	  "Stop mode"},
	{0x00,	0x01, 0x80,	0x00, "Off"},
	{0x00,	0x01, 0x80,	0x80, "On"},

	// Fake DIPs
	// BIOS
	{0,	0xFD, 0,	33,   "BIOS"                     	         },
	{0x02,	0x01, 0x3f,	0x00, "MVS Asia/Europe ver. 6 (1 slot)"  },
	{0x02,	0x01, 0x3f,	0x01, "MVS Asia/Europe ver. 5 (1 slot)"  },
	{0x02,	0x01, 0x3f,	0x02, "MVS Asia/Europe ver. 3 (4 slot)"  },
	{0x02,	0x01, 0x3f,	0x03, "MVS USA ver. 5 (2 slot)"          },
	{0x02,	0x01, 0x3f,	0x04, "MVS USA ver. 5 (4 slot)"          },
	{0x02,	0x01, 0x3f,	0x05, "MVS USA ver. 5 (6 slot)"          },
	{0x02,	0x01, 0x3f,	0x06, "MVS USA (U4)"                     },
	{0x02,	0x01, 0x3f,	0x07, "MVS USA (U3)"                     },
	{0x02,	0x01, 0x3f,	0x08, "MVS Japan ver. 6 (? slot)"        },
	{0x02,	0x01, 0x3f,	0x09, "MVS Japan ver. 5 (? slot)"        },
	{0x02,	0x01, 0x3f,	0x0a, "MVS Japan ver. 3 (4 slot)"        },
	{0x02,	0x01, 0x3f,	0x0b, "NEO-MVH MV1C (Asia)"              },
	{0x02,	0x01, 0x3f,	0x0c, "NEO-MVH MV1C (Japan)"             },
	{0x02,	0x01, 0x3f,	0x0d, "MVS Japan (J3)"                   },
	{0x02,	0x01, 0x3f,	0x0e, "MVS Japan (J3, alt)"              },
	{0x02,	0x01, 0x3f,	0x0f, "AES Japan"                        },
	{0x02,	0x01, 0x3f,	0x10, "AES Asia"                         },
	{0x02,	0x01, 0x3f,	0x11, "Development Kit"                  },
	{0x02,	0x01, 0x3f,	0x12, "Deck ver. 6 (Git Ver 1.3)"        },
	{0x02,	0x01, 0x3f,	0x13, "Universe BIOS ver. 4.0"           },
	{0x02,	0x01, 0x3f,	0x14, "Universe BIOS ver. 3.3"           },
	{0x02,	0x01, 0x3f,	0x15, "Universe BIOS ver. 3.2"           },
	{0x02,	0x01, 0x3f,	0x16, "Universe BIOS ver. 3.1"           },
	{0x02,	0x01, 0x3f,	0x17, "Universe BIOS ver. 3.0"           },
	{0x02,	0x01, 0x3f,	0x18, "Universe BIOS ver. 2.3"           },
	{0x02,	0x01, 0x3f,	0x19, "Universe BIOS ver. 2.3 (alt)"     },
	{0x02,	0x01, 0x3f,	0x1a, "Universe BIOS ver. 2.2"           },
	{0x02,	0x01, 0x3f,	0x1b, "Universe BIOS ver. 2.1"           },
	{0x02,	0x01, 0x3f,	0x1c, "Universe BIOS ver. 2.0"           },
	{0x02,	0x01, 0x3f,	0x1d, "Universe BIOS ver. 1.3"           },
	{0x02,	0x01, 0x3f,	0x1e, "Universe BIOS ver. 1.2"           },
	{0x02,	0x01, 0x3f,	0x1f, "Universe BIOS ver. 1.2 (alt)"     },
	{0x02,	0x01, 0x3f,	0x20, "Universe BIOS ver. 1.1"           },
	{0x02,	0x01, 0x3f,	0x21, "Universe BIOS ver. 1.0"           },
	{0x02,	0x01, 0x3f,	0x22, "NeoOpen BIOS v0.1 beta"           },

	// Memory card
	{0,		0xFD, 0,	2,	  "Memory card"},
	{0x02,	0x01, 0x80,	0x80, "Writable"},
	{0x02,	0x01, 0x80,	0x00, "Write-protected"},
	{0,		0xFD, 0,	2,	  "New card type"},
	{0x02,	0x01, 0x40,	0x40, "1 Megabit"},
	{0x02,	0x01, 0x40,	0x00, "Normal"},
};

STDDIPINFOEXT(neogeo, neodefault, neogeo)
STDDIPINFOEXT(neopaddle, neopaddle, neogeo)
STDDIPINFOEXT(neodual, neodual, neogeo)
STDDIPINFOEXT(neomahjong, neomahjong, neogeo)
STDDIPINFOEXT(neogeousa, neousadefault, neogeo)
STDDIPINFOEXT(neoaes, neoaesdefault, neogeo)
STDDIPINFOEXT(neogeouni, neounidefault, neogeo)

static struct BurnDIPInfo neoKOFDIPList[] = {
	// Offset
	{0x19,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x00, NULL},
	{0x01,	0xFF, 0x7F,	0x00, NULL},

	{0,		0xFE, 0,	2,	  "Autofire"},
	{0x00,	0x01, 0x04,	0x00, "Off"},
	{0x00,	0x01, 0x04,	0x04, "On"},

	// DIP 1
	{0,		0xFE, 0,	2,	  "Setting mode"},
	{0x00,	0x01, 0x01,	0x00, "Off"},
	{0x00,	0x01, 0x01,	0x01, "On"},

	// 1 or 2 coin slots
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x00,	0x02, 0x02,	0x00, "1"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	{0x00,	0x02, 0x02,	0x02, "2"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	// 2 or 4 coin slots
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x00,	0x82, 0x02,	0x00, "2"},
	{0x02,	0x00, 0x3F,	0x06, NULL},
	{0x00,	0x82, 0x02,	0x02, "4"},
	{0x02,	0x00, 0x3F,	0x06, NULL},

	{0,		0xFE, 0,	5,	  "Commmunicaton"},
	{0x00,	0x01, 0x38,	0x00, "Disabled"},
	{0x00,	0x01, 0x38,	0x08, "Setting 1"},
	{0x00,	0x01, 0x38,	0x10, "Setting 2"},
	{0x00,	0x01, 0x38,	0x18, "Setting 3"},
	{0x00,	0x01, 0x38,	0x20, "Setting 4"},
	{0x00,	0x01, 0x38,	0x28, "Setting 5"},
	{0x00,	0x01, 0x38,	0x30, "Setting 6"},
	{0x00,	0x01, 0x38,	0x38, "Setting 7"},
	{0,		0xFE, 0,	2,	  "Free play"},
	{0x00,	0x01, 0x40,	0x00, "Off"},
	{0x00,	0x01, 0x40,	0x40, "On"},
	{0,		0xFE, 0,	2,	  "Stop mode"},
	{0x00,	0x01, 0x80,	0x00, "Off"},
	{0x00,	0x01, 0x80,	0x80, "On"},
};

static struct BurnDIPInfo neoFakeDIPList[] = {
	// default
	{0x02,	0xFF, 0xFF,	0x80, NULL},

	// Fake DIPs
	// BIOS
	{0,		0xFD, 0,	33,   "BIOS"                     	     },
	{0x02,	0x01, 0x3f,	0x00, "MVS Asia/Europe ver. 6 (1 slot)"  },
	{0x02,	0x01, 0x3f,	0x01, "MVS Asia/Europe ver. 5 (1 slot)"  },
	{0x02,	0x01, 0x3f,	0x02, "MVS Asia/Europe ver. 3 (4 slot)"  },
	{0x02,	0x01, 0x3f,	0x03, "MVS USA ver. 5 (2 slot)"          },
	{0x02,	0x01, 0x3f,	0x04, "MVS USA ver. 5 (4 slot)"          },
	{0x02,	0x01, 0x3f,	0x05, "MVS USA ver. 5 (6 slot)"          },
	{0x02,	0x01, 0x3f,	0x06, "MVS USA (U4)"                     },
	{0x02,	0x01, 0x3f,	0x07, "MVS USA (U3)"                     },
	{0x02,	0x01, 0x3f,	0x08, "MVS Japan ver. 6 (? slot)"        },
	{0x02,	0x01, 0x3f,	0x09, "MVS Japan ver. 5 (? slot)"        },
	{0x02,	0x01, 0x3f,	0x0a, "MVS Japan ver. 3 (4 slot)"        },
	{0x02,	0x01, 0x3f,	0x0b, "NEO-MVH MV1C (Asia)"              },
	{0x02,	0x01, 0x3f,	0x0c, "NEO-MVH MV1C (Japan)"             },
	{0x02,	0x01, 0x3f,	0x0d, "MVS Japan (J3)"                   },
	{0x02,	0x01, 0x3f,	0x0e, "MVS Japan (J3, alt)"              },
	{0x02,	0x01, 0x3f,	0x0f, "AES Japan"                        },
	{0x02,	0x01, 0x3f,	0x10, "AES Asia"                         },
	{0x02,	0x01, 0x3f,	0x11, "Development Kit"                  },
	{0x02,	0x01, 0x3f,	0x12, "Deck ver. 6 (Git Ver 1.3)"        },
	{0x02,	0x01, 0x3f,	0x13, "Universe BIOS ver. 4.0"           },
	{0x02,	0x01, 0x3f,	0x14, "Universe BIOS ver. 3.3"           },
	{0x02,	0x01, 0x3f,	0x15, "Universe BIOS ver. 3.2"           },
	{0x02,	0x01, 0x3f,	0x16, "Universe BIOS ver. 3.1"           },
	{0x02,	0x01, 0x3f,	0x17, "Universe BIOS ver. 3.0"           },
	{0x02,	0x01, 0x3f,	0x18, "Universe BIOS ver. 2.3"           },
	{0x02,	0x01, 0x3f,	0x19, "Universe BIOS ver. 2.3 (alt)"     },
	{0x02,	0x01, 0x3f,	0x1a, "Universe BIOS ver. 2.2"           },
	{0x02,	0x01, 0x3f,	0x1b, "Universe BIOS ver. 2.1"           },
	{0x02,	0x01, 0x3f,	0x1c, "Universe BIOS ver. 2.0"           },
	{0x02,	0x01, 0x3f,	0x1d, "Universe BIOS ver. 1.3"           },
	{0x02,	0x01, 0x3f,	0x1e, "Universe BIOS ver. 1.2"           },
	{0x02,	0x01, 0x3f,	0x1f, "Universe BIOS ver. 1.2 (alt)"     },
	{0x02,	0x01, 0x3f,	0x20, "Universe BIOS ver. 1.1"           },
	{0x02,	0x01, 0x3f,	0x21, "Universe BIOS ver. 1.0"           },
	{0x02,	0x01, 0x3f,	0x22, "NeoOpen BIOS v0.1 beta"           },

	// Memory card
	{0,		0xFD, 0,	2,	  "Memory card"},
	{0x02,	0x01, 0x80,	0x80, "Writable"},
	{0x02,	0x01, 0x80,	0x00, "Write-protected"},
	{0,		0xFD, 0,	2,	  "New card type"},
	{0x02,	0x01, 0x40,	0x40, "1 Megabit"},
	{0x02,	0x01, 0x40,	0x00, "Normal"},
};

STDDIPINFOEXT(neoMVS, neoKOF, neoFake)

static struct BurnDIPInfo neoCDDIPList[] = {
	// Offset
	{0x16,	0xF0, 0x00,	0x00, NULL},

	// Defaults
	{0x00,	0xFF, 0xFF,	0x02, NULL},

	// Region
	{0,		0xFE, 0,	4,	  "Region"},
	{0x00,	0x01, 0x03,	0x00, "Japan"},
	{0x00,	0x01, 0x03,	0x01, "USA"},
	{0x00,	0x01, 0x03,	0x02, "Europe"},
	{0x00,	0x01, 0x03,	0x03, "Portugese"},

#if 0

	// Memory card
	{0,		0xFD, 0,	2,	  "Memory card"},
	{0x00,	0x01, 0x80,	0x80, "Writable"},
	{0x00,	0x01, 0x80,	0x00, "Write-protected"},
	{0,		0xFD, 0,	2,	  "New card type"},
	{0x00,	0x01, 0x40,	0x40, "1 Megabit"},
	{0x00,	0x01, 0x40,	0x00, "Normal"},

#endif

};

STDDIPINFO(neoCD)

static struct BurnDIPInfo neotrackballDIPList[] = {
	// Defaults
	{0x0D,	0xFF, 0xFF,	0x00, NULL},
	{0x0E,	0xFF, 0x7F,	0x00, NULL},
	// Fake DIPs
	{0x0F,	0xFF, 0xFF,	0x86, NULL},

	// DIP 1
	{0,		0xFE, 0,	2,	  "Setting mode"},
	{0x0D,	0x01, 0x01,	0x00, "Off"},
	{0x0D,	0x01, 0x01,	0x01, "On"},
	{0,		0xFE, 0,	2,	  "Coin chutes"},
	{0x0D,	0x01, 0x02,	0x00, "1"},
	{0x0D,	0x01, 0x02,	0x02, "2"},
	{0,		0xFE, 0,	2,	  "Autofire"},
	{0x0D,	0x01, 0x04,	0x00, "Off"},
	{0x0D,	0x01, 0x04,	0x04, "On"},
	{0,		0xFE, 0,	5,	  "Commmunicaton"},
	{0x0D,	0x01, 0x38,	0x00, "Disabled"},
	{0x0D,	0x01, 0x38,	0x08, "Setting 1"},
	{0x0D,	0x01, 0x38,	0x10, "Setting 2"},
	{0x0D,	0x01, 0x38,	0x18, "Setting 3"},
	{0x0D,	0x01, 0x38,	0x20, "Setting 4"},
	{0x0D,	0x01, 0x38,	0x28, "Setting 5"},
	{0x0D,	0x01, 0x38,	0x30, "Setting 6"},
	{0x0D,	0x01, 0x38,	0x38, "Setting 7"},
	{0,		0xFE, 0,	2,	  "Free play"},
	{0x0D,	0x01, 0x40,	0x00, "Off"},
	{0x0D,	0x01, 0x40,	0x40, "On"},
	{0,		0xFE, 0,	2,	  "Stop mode"},
	{0x0D,	0x01, 0x80,	0x00, "Off"},
	{0x0D,	0x01, 0x80,	0x80, "On"},

	// MVS/AES system
	{0,		0xFD, 0,	2,	  NULL},
	{0x0F,	0x02, 0x04,	0x04, "MVS system"},
	{0x0F,	0x00, 0x30,	0x00, NULL},
	{0x0F,	0x02, 0x04,	0x00, "AES system"},
	{0x0F,	0x00, 0x30,	0x00, NULL},
	// Region
	{0,		0xFD, 0,	3,	  "Region"},
	{0x0F,	0x01, 0x03,	0x00, "Japan"},
	{0x0F,	0x01, 0x03,	0x01, "USA"},
	{0x0F,	0x01, 0x03,	0x02, "Asia / Europe"},
	{0x0F,	0x01, 0x03,	0x03, "Japan"},

	// Memory card
	{0,		0xFD, 0,	2,	  "Memory card"},
	{0x0F,	0x01, 0x80,	0x80, "Writable"},
	{0x0F,	0x01, 0x80,	0x00, "Write-protected"},
	{0,		0xFD, 0,	2,	  "Default card type"},
	{0x0F,	0x01, 0x40,	0x40, "1 Megabit"},
	{0x0F,	0x01, 0x40,	0x00, "Normal"},
};

STDDIPINFO(neotrackball)

// Some games can only be played in AES.
static struct BurnDIPInfo neoForceAESDIPList[] = {
	// Offset
	{0x15,	0xF0, 0x00,	0x00, NULL              }, // neoForceAESInputList

	// Defaults
	{0x00,	0xFF, 0xFF,	0x40, NULL              }, // DIP 1 [Autofire = Off] [Free play = On]
	{0x01,	0xFF, 0x7F,	0x00, NULL              }, // DIP 2
	{0x02,	0xFF, 0xFF,	0x8f, NULL              }, // System [AES Japan] [Memory card = Writable]
	{0x03,	0xFF, 0xFF,	0x03, NULL              }, // Slots [3]

	// DIP 1
	{0,		0xFE, 0,	2,	  "Autofire"        },
	{0x00,	0x01, 0x04,	0x00, "Off"             },
	{0x00,	0x01, 0x04,	0x04, "On"              },

	{0,		0xFE, 0,	2,	  "Free play"       },
	{0x00,	0x01, 0x40,	0x00, "Off"             },
	{0x00,	0x01, 0x40,	0x40, "On"              },

	// System - BIOS
	{0,		0xFD, 0,	2,    "BIOS"            },
	{0x02,	0x01, 0x3f,	0x0f, "AES Japan"       },
	{0x02,	0x01, 0x3f,	0x10, "AES Asia"        },

	// System - Memory card
	{0,		0xFD, 0,	2,	  "Memory card"     },
	{0x02,	0x01, 0x80,	0x80, "Writable"        },
	{0x02,	0x01, 0x80,	0x00, "Write-protected" },

	{0,		0xFD, 0,	2,	  "New card type"   },
	{0x02,	0x01, 0x40,	0x40, "1 Megabit"       },
	{0x02,	0x01, 0x40,	0x00, "Normal"          },
};

STDDIPINFO(neoForceAES)

// Rom information
static struct BurnRomInfo neogeoRomDesc[] = {
	{ "sp-s3.sp1",         0x20000, 0x91b64be3, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT           }, //  0 MVS Asia/Europe ver. 6 (1 slot)
	{ "sp-s2.sp1",         0x20000, 0x9036d879, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  1 MVS Asia/Europe ver. 5 (1 slot)
	{ "sp-s.sp1",          0x20000, 0xc7f2fa45, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  2 MVS Asia/Europe ver. 3 (4 slot)
	{ "sp-u2.sp1",         0x20000, 0xe72943de, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  3 MVS USA ver. 5 (2 slot)
	{ "sp1-u2",            0x20000, 0x62f021f4, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  4 MVS USA ver. 5 (4 slot)
	{ "sp-e.sp1",          0x20000, 0x2723a5b5, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  5 MVS USA ver. 5 (6 slot)
	{ "sp1-u4.bin",        0x20000, 0x1179a30f, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  6 MVS USA (U4)
	{ "sp1-u3.bin",        0x20000, 0x2025b7a2, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  7 MVS USA (U3)
	{ "vs-bios.rom",       0x20000, 0xf0e8f27d, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  8 MVS Japan ver. 6 (? slot)
	{ "sp-j2.sp1",         0x20000, 0xacede59C, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  9 MVS Japan ver. 5 (? slot)
	{ "sp1.jipan.1024",    0x20000, 0x9fb0abe4, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 10 MVS Japan ver. 3 (4 slot)
	{ "sp-45.sp1",         0x80000, 0x03cc9f6a, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 11 NEO-MVH MV1C (Asia)
	{ "sp-j3.sp1",         0x80000, 0x486cb450, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 12 NEO-MVH MV1C (Japan)
	{ "japan-j3.bin",      0x20000, 0xdff6d41f, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 13 MVS Japan (J3)
	{ "sp1-j3.bin",        0x20000, 0xfbc6d469, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 14 MVS Japan (J3, alt)
#if !defined (ROM_VERIFY)
	{ "neo-po.bin",        0x20000, 0x16d0c132, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 15 AES Japan
	{ "neo-epo.bin",       0x20000, 0xd27a71f1, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 16 AES Asia
	{ "neodebug.bin",      0x20000, 0x698ebb7d, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 17 Development Kit
#else
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 15
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 16
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 17
#endif
	{ "sp-1v1_3db8c.bin",  0x20000, 0x162f0ebe, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 18 Deck ver. 6 (Git Ver 1.3)
	{ "uni-bios_4_0.rom",  0x20000, 0xa7aab458, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 19 Universe BIOS ver. 4.0 (free)
	{ "uni-bios_3_3.rom",  0x20000, 0x24858466, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 20 Universe BIOS ver. 3.3 (free)
	{ "uni-bios_3_2.rom",  0x20000, 0xa4e8b9b3, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 21 Universe BIOS ver. 3.2 (free)
	{ "uni-bios_3_1.rom",  0x20000, 0x0c58093f, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 22 Universe BIOS ver. 3.1 (free)
	{ "uni-bios_3_0.rom",  0x20000, 0xa97c89a9, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 23 Universe BIOS ver. 3.0 (free)
	{ "uni-bios_2_3.rom",  0x20000, 0x27664eb5, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 24 Universe BIOS ver. 2.3
	{ "uni-bios_2_3o.rom", 0x20000, 0x601720ae, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 25 Universe BIOS ver. 2.3 (alt)
	{ "uni-bios_2_2.rom",  0x20000, 0x2d50996a, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 26 Universe BIOS ver. 2.2
	{ "uni-bios_2_1.rom",  0x20000, 0x8dabf76b, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 27 Universe BIOS ver. 2.1
	{ "uni-bios_2_0.rom",  0x20000, 0x0c12c2ad, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 28 Universe BIOS ver. 2.0
	{ "uni-bios_1_3.rom",  0x20000, 0xb24b44a0, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 29 Universe BIOS ver. 1.3
	{ "uni-bios_1_2.rom",  0x20000, 0x4fa698e9, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 30 Universe BIOS ver. 1.2
	{ "uni-bios_1_2o.rom", 0x20000, 0xe19d3ce9, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 31 Universe BIOS ver. 1.2 (alt)
	{ "uni-bios_1_1.rom",  0x20000, 0x5dda0d84, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 32 Universe BIOS ver. 1.1
	{ "uni-bios_1_0.rom",  0x20000, 0x0ce453a0, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 33 Universe BIOS ver. 1.0
#if !defined (ROM_VERIFY)
	{ "neopen.sp1",        0x20000, 0xcb915e76, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 34 NeoOpen BIOS v0.1 beta
#else
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 34
#endif
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 35 Trackball BIOS loaded here
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 36 PCB BIOS loaded here

	{ "sm1.sm1",           0x20000, 0x94416d67, BRF_ESS | BRF_PRG | BRF_BIOS                        }, // 37 Z80 BIOS
	{ "sfix.sfix",         0x20000, 0xc2ea0cfd, BRF_GRA | BRF_BIOS                                  }, // 38 Text layer tiles
	{ "000-lo.lo",         0x20000, 0x5a86cff2, BRF_ESS | BRF_BIOS                                  }, // 39 Zoom table
};

STD_ROM_PICK(neogeo)
STD_ROM_FN(neogeo)

// Rom information
static struct BurnRomInfo neoMVSRomDesc[] = {
	{ "sp-s3.sp1",         0x20000, 0x91b64be3, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT           }, //  0 MVS Asia/Europe ver. 6 (1 slot)
	{ "sp-s2.sp1",         0x20000, 0x9036d879, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  1 MVS Asia/Europe ver. 5 (1 slot)
	{ "sp-s.sp1",          0x20000, 0xc7f2fa45, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  2 MVS Asia/Europe ver. 3 (4 slot)
	{ "usa_2slt.bin",      0x20000, 0xe72943de, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  3 MVS USA ver. 5 (2 slot)
	{ "sp1-u2",            0x20000, 0x62f021f4, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  4 MVS USA ver. 5 (4 slot)
	{ "sp-e.sp1",          0x20000, 0x2723a5b5, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  5 MVS USA ver. 5 (6 slot)
	{ "sp1-u4.bin",        0x20000, 0x1179a30f, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  6 MVS USA (U4)
	{ "sp1-u3.bin",        0x20000, 0x2025b7a2, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  7 MVS USA (U3)
	{ "vs-bios.rom",       0x20000, 0xf0e8f27d, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  8 MVS Japan ver. 6 (? slot)
	{ "sp-j2.sp1",         0x20000, 0xacede59C, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, //  9 MVS Japan ver. 5 (? slot)
	{ "sp1.jipan.1024",    0x20000, 0x9fb0abe4, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 10 MVS Japan ver. 3 (4 slot)
	{ "sp-45.sp1",         0x80000, 0x03cc9f6a, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 11 NEO-MVH MV1C (Asia)
	{ "sp-j3.sp1",         0x80000, 0x486cb450, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 12 NEO-MVH MV1C (Japan)
	{ "japan-j3.bin",      0x20000, 0xdff6d41f, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 13 MVS Japan (J3)
	{ "sp1-j3.bin",        0x20000, 0xfbc6d469, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 14 MVS Japan (J3, alt)
	{ "neo-po.bin",        0x20000, 0x16d0c132, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 15 AES Japan
	{ "neo-epo.bin",       0x20000, 0xd27a71f1, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 16 AES Asia
	{ "neodebug.bin",      0x20000, 0x698ebb7d, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 17 Development Kit
	{ "sp-1v1_3db8c.bin",  0x20000, 0x162f0ebe, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 18 Deck ver. 6 (Git Ver 1.3)
	{ "uni-bios_4_0.rom",  0x20000, 0xa7aab458, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 19 Universe BIOS ver. 4.0 (free)
	{ "uni-bios_3_3.rom",  0x20000, 0x24858466, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 20 Universe BIOS ver. 3.3 (free)
	{ "uni-bios_3_2.rom",  0x20000, 0xa4e8b9b3, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 21 Universe BIOS ver. 3.2 (free)
	{ "uni-bios_3_1.rom",  0x20000, 0x0c58093f, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 22 Universe BIOS ver. 3.1 (free)
	{ "uni-bios_3_0.rom",  0x20000, 0xa97c89a9, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 23 Universe BIOS ver. 3.0 (free)
	{ "uni-bios_2_3.rom",  0x20000, 0x27664eb5, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 24 Universe BIOS ver. 2.3
	{ "uni-bios_2_3o.rom", 0x20000, 0x601720ae, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 25 Universe BIOS ver. 2.3 (alt)
	{ "uni-bios_2_2.rom",  0x20000, 0x2d50996a, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 26 Universe BIOS ver. 2.2
	{ "uni-bios_2_1.rom",  0x20000, 0x8dabf76b, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 27 Universe BIOS ver. 2.1
	{ "uni-bios_2_0.rom",  0x20000, 0x0c12c2ad, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 28 Universe BIOS ver. 2.0
	{ "uni-bios_1_3.rom",  0x20000, 0xb24b44a0, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 29 Universe BIOS ver. 1.3
	{ "uni-bios_1_2.rom",  0x20000, 0x4fa698e9, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 30 Universe BIOS ver. 1.2
	{ "uni-bios_1_2o.rom", 0x20000, 0xe19d3ce9, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 31 Universe BIOS ver. 1.2 (alt)
	{ "uni-bios_1_1.rom",  0x20000, 0x5dda0d84, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 32 Universe BIOS ver. 1.1
	{ "uni-bios_1_0.rom",  0x20000, 0x0ce453a0, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 33 Universe BIOS ver. 1.0
	{ "neopen.sp1",        0x20000, 0xcb915e76, BRF_ESS | BRF_PRG | BRF_BIOS | BRF_SELECT | BRF_OPT }, // 34 NeoOpen BIOS v0.1 beta
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 35 Trackball BIOS loaded here
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 36 PCB BIOS loaded here

	{ "sm1.sm1",           0x20000, 0x94416d67, BRF_ESS | BRF_PRG | BRF_BIOS                        }, // 37 Z80 BIOS
	{ "sfix.sfix",         0x20000, 0xc2ea0cfd, BRF_GRA | BRF_BIOS                                  }, // 38 Text layer tiles
	{ "000-lo.lo",         0x20000, 0x5a86cff2, BRF_ESS | BRF_BIOS                                  }, // 39 Zoom table
};

STD_ROM_PICK(neoMVS)
STD_ROM_FN(neoMVS)

static struct BurnRomInfo neotrackballRomDesc[] = {
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  0
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  1
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  2
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  3
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  4
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  5
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  6
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  7
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  8
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  9
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 10
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 11
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 12
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 13
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 14
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 15
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 16
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 17
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 18
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 19
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 20
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 21
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 22
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 23
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 24
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 25
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 26
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 27
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 28
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 29
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 30
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 31
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 32
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 33
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 34
	{ "236-bios.sp1",      0x20000, 0x853e6b96, BRF_ESS | BRF_PRG | BRF_BIOS                        }, // 35 Trackball BIOS
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 36


//	{ "sm1.sm1",           0x20000, 0x94416d67, BRF_ESS | BRF_PRG | BRF_BIOS                        }, // 37 Z80 BIOS
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 37 Z80 BIOS
	{ "sfix.sfix",         0x20000, 0xc2ea0cfd, BRF_GRA | BRF_BIOS                                  }, // 38 Text layer tiles
	{ "000-lo.lo",         0x20000, 0x5a86cff2, BRF_ESS | BRF_BIOS                                  }, // 39 Zoom table
};

static struct BurnRomInfo ms5pcbBIOSRomDesc[] = {
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  0
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  1
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  2
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  3
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  4
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  5
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  6
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  7
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  8
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  9
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 10
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 11
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 12
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 13
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 14
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 15
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 16
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 17
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 18
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 19
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 20
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 21
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 22
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 23
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 24
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 25
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 26
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 27
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 28
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 29
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 30
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 31
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 32
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 33
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 34
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 35
	{ "sp-4x.sp1",         0x80000, 0xb4590283, BRF_ESS | BRF_PRG | BRF_BIOS                        }, // 36 PCB BIOS

	{ "",                  0x00000, 0x00000000, 0                                                   }, // 37 Z80 BIOS
	{ "sfix.sfix",         0x20000, 0xc2ea0cfd, BRF_GRA | BRF_BIOS                                  }, // 38 Text layer tiles
	{ "000-lo.lo",         0x20000, 0x5a86cff2, BRF_ESS | BRF_BIOS                                  }, // 39 Zoom table
};

static struct BurnRomInfo svcpcbBIOSRomDesc[] = {
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  0
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  1
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  2
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  3
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  4
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  5
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  6
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  7
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  8
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  9
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 10
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 11
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 12
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 13
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 14
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 15
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 16
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 17
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 18
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 19
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 20
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 21
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 22
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 23
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 24
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 25
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 26
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 27
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 28
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 29
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 30
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 31
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 32
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 33
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 34
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 35
	{ "sp-4x.sp1",         0x80000, 0xb4590283, BRF_ESS | BRF_PRG | BRF_BIOS                        }, // 36 PCB BIOS

	{ "",                  0x00000, 0x00000000, 0                                                   }, // 37 Z80 BIOS
	{ "sfix.sfix",         0x20000, 0xc2ea0cfd, BRF_GRA | BRF_BIOS                                  }, // 38 Text layer tiles
	{ "000-lo.lo",         0x20000, 0x5a86cff2, BRF_ESS | BRF_BIOS                                  }, // 39 Zoom table
};

static struct BurnRomInfo kf2k3pcbBIOSRomDesc[] = {
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  0
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  1
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  2
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  3
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  4
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  5
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  6
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  7
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  8
	{ "",                  0x00000, 0x00000000, 0                                                   }, //  9
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 10
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 11
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 12
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 13
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 14
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 15
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 16
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 17
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 18
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 19
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 20
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 21
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 22
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 23
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 24
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 25
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 26
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 27
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 28
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 29
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 30
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 31
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 32
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 33
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 34
	{ "",                  0x00000, 0x00000000, 0                                                   }, // 35
	{ "spj.sp1",           0x80000, 0x148dd727, BRF_ESS | BRF_PRG | BRF_BIOS                        }, // 36 PCB BIOS

	{ "",                  0x00000, 0x00000000, 0                                                   }, // 37 Z80 BIOS
	{ "sfix.sfix",         0x20000, 0xc2ea0cfd, BRF_GRA | BRF_BIOS                                  }, // 38 Text layer tiles
	{ "000-lo.lo",         0x20000, 0x5a86cff2, BRF_ESS | BRF_BIOS                                  }, // 39 Zoom table
};

struct BurnDriver BurnDrvNeoGeo = {
	"neogeo", NULL, NULL, NULL, "1990",
	"Neo Geo\0", "BIOS only", "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_BOARDROM, 0, HARDWARE_SNK_NEOGEO, 0, 0,
	NULL, neogeoRomInfo, neogeoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

struct BurnDriver BurnDrvNeoGeoMVS = {
	"neogeo", NULL, NULL, NULL, "1990",
	"Neo Geo MVS system\0", "System - cartridges selected seperately", "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING /*| BDF_SYSTEM_ONLY*/, 2, HARDWARE_SNK_MVS, GBF_MISC, 0,
	NULL, neoMVSRomInfo, neoMVSRomName, NULL, NULL, NULL, NULL, neoMVSInputInfo, neoMVSDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

#if defined ROM_VERIFY
/*
struct BurnDriver BurnDrvNeoGeoMV1 = {
	"ng_mv1", NULL, "neogeo", NULL, "1990",
	"Neo Geo (MV-1)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 0, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neogeoRomInfo, neogeoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

struct BurnDriver BurnDrvNeoGeoMV1F = {
	"ng_mv1f", "ng_mv1", NULL, NULL, "1990",
	"Neo Geo (MV-1F)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 0, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neogeoRomInfo, neogeoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

struct BurnDriver BurnDrvNeoGeoMV1FZ = {
	"ng_mv1fz", "ng_mv1", NULL, NULL, "1990",
	"Neo Geo (MV-1FZ)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 0, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neogeoRomInfo, neogeoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

struct BurnDriver BurnDrvNeoGeoMV2F = {
	"ng_mv2f", NULL, "neogeo", NULL, "1990",
	"Neo Geo (MV-2F)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 0, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neogeoRomInfo, neogeoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

struct BurnDriver BurnDrvNeoGeoMV4F = {
	"ng_mv4f", NULL, "neogeo", NULL, "1990",
	"Neo Geo (MV-4F)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 0, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neogeoRomInfo, neogeoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};*/
#endif

// ----------------------------------------------------------------------------
// Neo Geo CDZ system

static struct BurnRomInfo neocdzRomDesc[] = {
	{ "neocd.bin",      0x080000, 0xDF9DE490, BRF_ESS | BRF_PRG | BRF_BIOS },
	{ "000-lo.lo",      0x020000, 0x5a86cff2, BRF_ESS | BRF_PRG | BRF_BIOS },
};

STD_ROM_PICK(neocdz)
STD_ROM_FN(neocdz)

struct BurnDriver BurnDrvneocdz = {
	"neocdz", NULL, "neogeo", NULL, "1996",
	"Neo Geo CDZ system\0", "System - media selected seperately", "SNK", "Neo Geo CDZ",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING /*| BDF_SYSTEM_ONLY*/, 2, HARDWARE_SNK_NEOCD, GBF_MISC, 0,
	NULL, neocdzRomInfo, neocdzRomName, NULL, NULL, NULL, NULL, neoCDInputInfo, neoCDDIPInfo,
	NeoCDInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// ----------------------------------------------------------------------------
// Extra code for SMA protection

INT32 nSMARandomNumber[MAX_SLOT];

static UINT32 nNeoSMARNGAddress[MAX_SLOT][2] = { 0, 0 };

static pSekWriteWordHandler pSMABankswitchHandler[MAX_SLOT] = { NULL, };

// Bank in an area of memory
void NeoSMABankswitch()
{
	SekMapMemory(Neo68KROMActive + nNeo68KROMBank, 0x200000, 0x2FE3FF, MAP_ROM);
	if (nNeoSMARNGAddress[nNeoActiveSlot][0] > 0 || nNeoSMARNGAddress[nNeoActiveSlot][1] > 0) {
		SekMapMemory(Neo68KROMActive + nNeo68KROMBank + 0x0FE800, 0x2FE800, 0x2FFBFF, MAP_ROM);
	} else {
		SekMapMemory(Neo68KROMActive + nNeo68KROMBank + 0x0FE800, 0x2FE800, 0x2FFFFF, MAP_ROM);
	}
}

// Extra protection
UINT16 __fastcall neogeoReadWordSMA9A37(UINT32 sekAddress)
{
//	bprintf(PRINT_NORMAL, " -- SMA9A37 0x%08X (word)\n", sekAddress);

	if (sekAddress == 0x2FE446) {
		return 0x9A37;
	}

	return *((UINT16*)(Neo68KROMActive + nNeo68KROMBank + sekAddress - 0x200000));
}

UINT8 __fastcall neogeoReadByteSMA9A37(UINT32 sekAddress)
{
//	bprintf(PRINT_NORMAL, " -- SMA9A37 0x%08X (byte)\n", sekAddress);

	if (sekAddress == 0x2FE446) {
		return 0x9A;
	}
	if (sekAddress == 0x2FE447) {
		return 0x37;
	}

	return Neo68KROMActive[(nNeo68KROMBank + sekAddress - 0x200000) ^ 1];
}

// Random number generator
UINT16 __fastcall neogeoReadWordSMARNG(UINT32 sekAddress)
{
	if (sekAddress == nNeoSMARNGAddress[nNeoActiveSlot][0] || sekAddress == nNeoSMARNGAddress[nNeoActiveSlot][1]) {
		INT32 nRandomNumber = nSMARandomNumber[nNeoActiveSlot];

		INT32 nNewbit = (
				(nSMARandomNumber[nNeoActiveSlot] >>  2) ^
				(nSMARandomNumber[nNeoActiveSlot] >>  3) ^
				(nSMARandomNumber[nNeoActiveSlot] >>  5) ^
				(nSMARandomNumber[nNeoActiveSlot] >>  6) ^
				(nSMARandomNumber[nNeoActiveSlot] >>  7) ^
				(nSMARandomNumber[nNeoActiveSlot] >> 11) ^
				(nSMARandomNumber[nNeoActiveSlot] >> 12) ^
				(nSMARandomNumber[nNeoActiveSlot] >> 15)) & 1;

		nSMARandomNumber[nNeoActiveSlot] = ((nSMARandomNumber[nNeoActiveSlot] << 1) | nNewbit) & 0x0FFFFF;

		return nRandomNumber;
	}

	return *((UINT16*)(Neo68KROMActive + nNeo68KROMBank + sekAddress - 0x200000));
}

UINT8 __fastcall neogeoReadByteSMARNG(UINT32 sekAddress)
{
	if ((sekAddress & ~1) == nNeoSMARNGAddress[nNeoActiveSlot][0] || (sekAddress & ~1) == nNeoSMARNGAddress[nNeoActiveSlot][1]) {
		INT32 nRandomNumber = nSMARandomNumber[nNeoActiveSlot];

		INT32 nNewbit = (
				(nSMARandomNumber[nNeoActiveSlot] >>  2) ^
				(nSMARandomNumber[nNeoActiveSlot] >>  3) ^
				(nSMARandomNumber[nNeoActiveSlot] >>  5) ^
				(nSMARandomNumber[nNeoActiveSlot] >>  6) ^
				(nSMARandomNumber[nNeoActiveSlot] >>  7) ^
				(nSMARandomNumber[nNeoActiveSlot] >> 11) ^
				(nSMARandomNumber[nNeoActiveSlot] >> 12) ^
				(nSMARandomNumber[nNeoActiveSlot] >> 15)) & 1;

		nSMARandomNumber[nNeoActiveSlot] = ((nSMARandomNumber[nNeoActiveSlot] << 1) | nNewbit) & 0x0FFFFF;

		if (sekAddress & 1) {
			return nRandomNumber & 0xFF;
		} else {
			return nRandomNumber >> 8;
		}
	}

	return Neo68KROMActive[(nNeo68KROMBank + sekAddress - 0x200000) ^ 1];
}

INT32 NeoSMAScan(INT32 nAction, INT32* /*pnMin*/)
{
	if (nAction & ACB_DRIVER_DATA) {
		SCAN_VAR(nSMARandomNumber);
	}

	return 0;
}

static void NeoSMAInstallHanders()
{
//	bprintf(PRINT_NORMAL, _T("    Installing handlers for slot %i\n"), nNeoActiveSlot);

	SekMapHandler(8,		0x2FFC00, 0x2FFFFF, MAP_WRITE);	// Bankswitch

	SekSetWriteWordHandler(8, pSMABankswitchHandler[nNeoActiveSlot]);

	SekMapHandler(6,		0x2FE400, 0x2FE7FF, MAP_ROM);	// Protection

	SekSetReadWordHandler(6, neogeoReadWordSMA9A37);
	SekSetReadByteHandler(6, neogeoReadByteSMA9A37);

	if (nNeoSMARNGAddress[nNeoActiveSlot][0] > 0 || nNeoSMARNGAddress[nNeoActiveSlot][1] > 0) {
		SekMapHandler(7,	0x2FFC00, 0x2FFFFF, MAP_ROM);	// Randum number generator

		SekSetReadWordHandler(7, neogeoReadWordSMARNG);
		SekSetReadByteHandler(7, neogeoReadByteSMARNG);
	}
}

// Install handlers
static INT32 NeoSMAInit(void (*pInitCallback)(), pSekWriteWordHandler pBankswitchHandler, UINT32 nRNGAddress0, UINT32 nRNGAddress1)
{
	nNeoSMARNGAddress[nNeoActiveSlot][0] = nRNGAddress0; nNeoSMARNGAddress[nNeoActiveSlot][1] = nRNGAddress1;
	pSMABankswitchHandler[nNeoActiveSlot] = pBankswitchHandler;

	NeoCallbackActive->pInitialise = pInitCallback;

	// Control SMA protection in ips environment.
	if (!bDoIpsPatch || (GetIpsDrvDefine() & IPS_USE_PROTECT)) {
		NeoCallbackActive->pInstallHandlers = NeoSMAInstallHanders;
		NeoCallbackActive->pBankswitch = NeoSMABankswitch;
		NeoCallbackActive->pScan = NeoSMAScan;
	}

	nSMARandomNumber[nNeoActiveSlot] = 0x2345;

	return NeoInit();
}

INT32 NeoSMAExit()
{
	NeoExit();

	nNeoSMARNGAddress[nNeoActiveSlot][0] = nNeoSMARNGAddress[nNeoActiveSlot][1] = 0;

	return 0;
}

// ----------------------------------------------------------------------------
// Extra code for PCM2 protection

struct PCM2DecryptP2Info { UINT32 nAddressOffset[16]; };

struct PCM2DecryptV2Info { INT32 nAddressOffset; INT32 nAddressXor; UINT8 nDataXor[8]; };

static void PCM2DecryptP()
{
	// Descamble P-ROMs

	UINT8* pTemp = (UINT8*)BurnMalloc(0x400000);

	if (pTemp) {
		memcpy(pTemp, Neo68KROMActive + 0x100000, 0x400000);

		for (INT32 i = 0; i < 4; i++) {
			memcpy(Neo68KROMActive + 0x100000 + i * 0x100000, pTemp + 0x000000 + (((((i + 2) & 1) << 2) | ((i + 2) & 2)) << 19), 0x80000);
			memcpy(Neo68KROMActive + 0x180000 + i * 0x100000, pTemp + 0x080000 + (((((i + 1) & 1) << 2) | ((i + 1) & 2)) << 19), 0x80000);
		}

		BurnFree(pTemp);
	}
}

static void PCM2DecryptV(INT32 size, INT32 bit)
{
	for (INT32 i = 0; i < (size / 2) - (2 << bit); i += (2 << bit)) {
		UINT16 buffer[8];
		memcpy(buffer, ((UINT16*)(YM2610ADPCMAROM[nNeoActiveSlot])) + i, (2 << bit) * 2);
		for (INT32 j = (2 << bit) - 1; j >= 0; j--) {
			((UINT16*)(YM2610ADPCMAROM[nNeoActiveSlot]))[i + j] = buffer[j ^ (1 << bit)];
		}
	}
}

static void PCM2DecryptV2(const PCM2DecryptV2Info* const pInfo)
{
	// Decrypt V-ROMs

	UINT8* pTemp = (UINT8*)BurnMalloc(0x01000000);

	if (pTemp) {
		memcpy(pTemp, YM2610ADPCMAROM[nNeoActiveSlot], 0x01000000);

		for (INT32 i = 0; i < 0x01000000; i++) {
			INT32 nAddress = ((i & 0x00FEFFFE) | ((i & 0x00010000) >> 16) | ((i & 0x00000001) << 16)) ^ pInfo->nAddressOffset;

			YM2610ADPCMAROM[nNeoActiveSlot][nAddress] = pTemp[(i + pInfo->nAddressXor) & 0xffffff] ^ pInfo->nDataXor[nAddress & 0x07];
		}

		BurnFree(pTemp);
	}
}

static void PCM2DecryptP2(const PCM2DecryptP2Info* const pInfo)
{
	// Descamble P-ROMs

	UINT8* pTemp = (UINT8*)BurnMalloc(0x800000);

	if (pTemp) {
		memcpy(pTemp, Neo68KROMActive, 0x800000);

		for (INT32 i = 0; i < 16; i++) {
			memcpy(Neo68KROMActive + i * 0x80000, pTemp + pInfo->nAddressOffset[i], 0x80000);
		}

		BurnFree(pTemp);
	}
}

// ----------------------------------------------------------------------------
// PVC protection

static UINT8 *PVCRAM = NULL;

void NeoPVCPallette01() // unpack palette word to RGB dword
{
	UINT8 b1, b2;
	b1 = PVCRAM[0x1fe1];
	b2 = PVCRAM[0x1fe0];

	PVCRAM[0x1fe2] = ((b2 & 0xf) << 1) | ((b1 >> 4) & 1);
	PVCRAM[0x1fe3] = (((b2 >> 4) & 0xf) << 1) | ((b1 >> 5) & 1);
	PVCRAM[0x1fe4] = ((b1 & 0xf) << 1) | ((b1 >> 6) & 1);
	PVCRAM[0x1fe5] = b1 >> 7;
}

void NeoPVCPallette02() // pack RGB dword to palette word
{
	UINT8 b1, b2, b3, b4;
	b1 = PVCRAM[0x1fe9];
	b2 = PVCRAM[0x1fe8];
	b3 = PVCRAM[0x1feb];
	b4 = PVCRAM[0x1fea];

	PVCRAM[0x1fec] = (b2 >> 1) | ((b1 >> 1) << 4);
	PVCRAM[0x1fed] = (b4 >> 1) | ((b2 & 1) << 4) | ((b1 & 1) << 5) | ((b4 & 1) << 6) | ((b3 & 1) << 7);
}

void NeoPVCBankswitch()
{
	UINT32 nBank  = (PVCRAM[0x1ff3] << 16) | (PVCRAM[0x1ff2] << 8) | PVCRAM[0x1ff1];
		     nBank += (Neo68KROMActive[0x108] & 0x10) << 16;	// for kof2003 (bank 0 is $100000)

	if (nNeo68KROMBank != nBank)
	{
		nNeo68KROMBank = nBank;
		SekMapMemory(Neo68KROMActive + nNeo68KROMBank, 0x200000, 0x2fdfff, MAP_ROM);
	}

	PVCRAM[0x1ff0]  = 0xa0;
	PVCRAM[0x1ff1] &= 0xfe;
	PVCRAM[0x1ff3] &= 0x7f;
}

void __fastcall PVCWriteByteBankSwitch(UINT32 sekAddress, UINT8 byteValue)
{
	PVCRAM[(sekAddress & 0x1fff) ^ 1] = byteValue;
	if (sekAddress >= 0x2fffe0 && sekAddress <= 0x2fffe1) NeoPVCPallette01();
	else if(sekAddress >= 0x2fffe8 && sekAddress <= 0x2fffeb) NeoPVCPallette02();
	else if  (sekAddress >= 0x2ffff0 && sekAddress <= 0x2ffff3) NeoPVCBankswitch();
}

void __fastcall PVCWriteWordBankSwitch(UINT32 sekAddress, UINT16 wordValue)
{
	*((UINT16 *)(PVCRAM + (sekAddress & 0x1ffe))) = BURN_ENDIAN_SWAP_INT16(wordValue);
	if (sekAddress >= 0x2fffe0 && sekAddress <= 0x2fffe1) NeoPVCPallette01();
	else if (sekAddress >= 0x2fffe8 && sekAddress <= 0x2fffeb) NeoPVCPallette02();
	else if (sekAddress >= 0x2ffff0 && sekAddress <= 0x2ffff3) NeoPVCBankswitch();
}

static void NeoPVCMapBank()
{
	SekMapMemory(Neo68KROMActive + nNeo68KROMBank, 0x200000, 0x2fdfff, MAP_ROM);
}

static INT32 NeoPVCScan(INT32 nAction, INT32*)
{
	struct BurnArea ba;

	if (nAction & ACB_MEMORY_RAM) {
		ba.Data		= PVCRAM;
		ba.nLen		= 0x00002000;
		ba.nAddress	= 0;
		ba.szName	= "Neo-PVC 68K RAM";
		BurnAcb(&ba);
	}

	return 0;
}

static void NeoPVCInstallHandlers()
{
	// Install cart PVC protections
	SekMapMemory(PVCRAM, 0x2fe000,  0x2fffff,  MAP_READ);

	SekMapHandler(6,      0x2fe000,  0x2fffff, MAP_WRITE);
	SekSetWriteWordHandler(6,    PVCWriteWordBankSwitch);
	SekSetWriteByteHandler(6,    PVCWriteByteBankSwitch);
}

static INT32 NeoPVCInit()
{
	// Control PVC protection in ips environment.
	if (!bDoIpsPatch || (GetIpsDrvDefine() & IPS_USE_PROTECT)) {
		PVCRAM = (UINT8*)BurnMalloc(0x2000);
		if (!PVCRAM) return 1;

		memset(PVCRAM, 0, 0x2000);

		NeoCallbackActive->pInstallHandlers = NeoPVCInstallHandlers;
		NeoCallbackActive->pBankswitch = NeoPVCMapBank;
		NeoCallbackActive->pScan = NeoPVCScan;
	}


	return NeoInit();
}

static INT32 NeoPVCExit()
{
	if (NULL != PVCRAM) BurnFree(PVCRAM);

	return NeoExit();
}


// ----------------------------------------------------------------------------
// Helper functions

static void lans2004_sx_decode()
{
	INT32 i, j, n;
	for (i = 0; i < 0x20000; i+= 0x10) {
		for (j = 0; j < 0x08; j++) {
			n = NeoTextROM[nNeoActiveSlot][i + j + 8];
			NeoTextROM[nNeoActiveSlot][i + j + 8] = NeoTextROM[nNeoActiveSlot][i + j];
			NeoTextROM[nNeoActiveSlot][i + j + 0] = n;
		}
	}
}

static void lans2004_cx_decode(INT32 nLen)
{
	INT32 i, j, n;
	for (i = 0; i < nLen; i+= 0x80) {
		for (j = 0; j < 0x40; j++) {
			n = NeoSpriteROM[nNeoActiveSlot][i + j + 0x40];
			NeoSpriteROM[nNeoActiveSlot][i + j + 0x40] = NeoSpriteROM[nNeoActiveSlot][i + j];
			NeoSpriteROM[nNeoActiveSlot][i + j] = n;
		}
	}
}

static void DoPerm(INT32 g) // 0 - cthd2003, 1 - svcboot
{
	static INT32 idx[ 2 ][ 16 ] = {
		{ 0, 1, 2, 3, 3, 4, 4, 5, 0, 1, 2, 3, 3, 4, 4, 5 }, // 0
		{ 0, 1, 0, 1, 2, 3, 2, 3, 3, 4, 3, 4, 4, 5, 4, 5 }, // 1
	};

	static INT32 tbl[ 6 ][ 4 ] = {
		{ 3, 0, 1, 2 },
		{ 2, 3, 0, 1 },
		{ 1, 2, 3, 0 },
		{ 0, 1, 2, 3 },
		{ 3, 2, 1, 0 },
		{ 3, 0, 2, 1 },
	};

	INT32 i, j, k, *b;
	UINT8 dst[0x800];

	for (i = 0; i < 0x4000000 >> 11; i++)
	{
		for(j = 0; j < 0x800 >> 7; j++)
		{
			b = tbl[idx[g][(i >> (5 ^ g)) & 0x0f]];
			k = BITSWAP08(j, 7, 6, 5, 4, b[3], b[2], b[1], b[0]);

			memcpy (dst + (j << 7), NeoSpriteROM[nNeoActiveSlot] + (i << 11) + (k << 7), 0x80);
		}

		memcpy (NeoSpriteROM[nNeoActiveSlot] + (i << 11), dst, 0x800);
	}
}


// ----------------------------------------------------------------------------
// Mapping extra rom

static void NeoMapExtraRom(INT32 rom_len, INT32 exrom_len)
{
	SekOpen(0);
	SekMapMemory(Neo68KROMActive + rom_len, 0x900000, 0x900000 + exrom_len - 1, MAP_ROM);
	SekClose();
}

// ----------------------------------------------------------------------------
// Game drivers (games in MAME)

/* ID range from 100 - 199 is used for Korean (some) and Neo Print carts */

/* The following Korean releases use the same ID's as the Japanese and/or US releases

	ID-0080 - Quiz King of Fighters (Korean release)
	ID-088  - Stakes Winner (Korean release)
	ID-0214 - The King of Fighters '96 (Korean release)
	ID-0217 - Ninja Masters (Korean release)
	ID-0225 - Waku Waku 7 (Korean release)
	ID-0230 - Breakers (Korean release)
	ID-0232 - The King of Fighters '97 (Korean release)
	ID-0238 - Shock Troopers (Korean release)
	ID-0242 - The King of Fighters '98 (Korean release) */

/* The following ID's are used by Korean releases:

    ID-0122 - Pae Wang Jeon Seol / Legend of a Warrior (Korean censored Samurai Shodown IV)
    ID-0123 - Quiz Daisousa Sen - The Last Count Down (Korean release)
    ID-0124 - Real Bout Fatal Fury Special / Real Bout Garou Densetsu Special (Korean release)
    ID-0134 - The Last Soldier (Korean release of The Last Blade)
    ID-0140 - Real Bout Fatal Fury 2 - The Newcomers (Korean release)
    ID-0152 - The King of Fighters '99 - Millennium Battle (Korean release)
    ID-0163 - Saulabi Spirits / Jin Saulabi Tu Hon (Korean release of Samurai Shodown II)
    ID-0187 - Fighters Swords (Korean release of Samurai Shodown III)
    ID-0196 - Art of Fighting 3 - The Path of the Warrior (Korean release)
*/


// NAM-1975 (NGM-001)(NGH-001)
/* MVS AND AES VERSION */

static struct BurnRomInfo nam1975RomDesc[] = {
	{ "001-p1.p1",    0x080000, 0xcc9fc951, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ MB834200

	{ "001-s1.s1",    0x020000, 0x7988ba51, 2 | BRF_GRA },           //  1 Text layer tiles / MB831000

	{ "001-c1.c1",    0x080000, 0x32ea98e1, 3 | BRF_GRA },           //  2 Sprite data 		/ MB834200
	{ "001-c2.c2",    0x080000, 0xcbc4064c, 3 | BRF_GRA },           //  3 					/ MB834200
	{ "001-c3.c3",    0x080000, 0x0151054c, 3 | BRF_GRA },           //  4 					/ MB834200
	{ "001-c4.c4",    0x080000, 0x0a32570d, 3 | BRF_GRA },           //  5 					/ MB834200
	{ "001-c5.c5",    0x080000, 0x90b74cc2, 3 | BRF_GRA },           //  6 					/ MB834200
	{ "001-c6.c6",    0x080000, 0xe62bed58, 3 | BRF_GRA },           //  7 					/ MB834200

	{ "001-m1.m1",    0x040000, 0xba874463, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code 		/ MB832000

	{ "001-v11.v11",  0x080000, 0xa7c3d5e5, 5 | BRF_SND },           //  9 Sound data 		/ MB834200
	// AES has different label, data is the same: 001-v1.v1
	{ "001-v21.v21",  0x080000, 0x55e670b3, 6 | BRF_SND },           // 10 					/ MB834200
	// AES has different label, data is the same: 001-v2.v21
	{ "001-v22.v22",  0x080000, 0xab0d8368, 6 | BRF_SND },           // 11 					/ MB834000
	{ "001-v23.v23",  0x080000, 0xdf468e28, 6 | BRF_SND },           // 12 					/ MB834000
};

STDROMPICKEXT(nam1975, nam1975, neogeo)
STD_ROM_FN(nam1975)

struct BurnDriver BurnDrvNam1975 = {
	"nam1975", NULL, "neogeo", NULL, "1990",
	"NAM-1975 (NGM-001)(NGH-001)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SHOOT, 0,
	NULL, nam1975RomInfo, nam1975RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Baseball Stars Professional (NGM-002)
/* MVS VERSION */

static struct BurnRomInfo bstarsRomDesc[] = {
	{ "002-pg.p1",    0x080000, 0xc100b5f5, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ MB834200

	{ "002-s1.s1",    0x020000, 0x1a7fd0c6, 2 | BRF_GRA },           //  1 Text layer tiles / MB831000

	{ "002-c1.c1",    0x080000, 0xaaff2a45, 3 | BRF_GRA },           //  2 Sprite data 		/ MB834200
	{ "002-c2.c2",    0x080000, 0x3ba0f7e4, 3 | BRF_GRA },           //  3 					/ MB834200
	{ "002-c3.c3",    0x080000, 0x96f0fdfa, 3 | BRF_GRA },           //  4 					/ MB834200
	{ "002-c4.c4",    0x080000, 0x5fd87f2f, 3 | BRF_GRA },           //  5 					/ MB834200
	{ "002-c5.c5",    0x080000, 0x807ed83b, 3 | BRF_GRA },           //  6 					/ MB834200
	{ "002-c6.c6",    0x080000, 0x5a3cad41, 3 | BRF_GRA },           //  7 					/ MB834200

	{ "002-m1.m1",    0x040000, 0x4ecaa4ee, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code 		/ MB832000

	{ "002-v11.v11",  0x080000, 0xb7b925bd, 5 | BRF_SND },           //  9 Sound data 		/ MB834000
	{ "002-v12.v12",  0x080000, 0x329f26fc, 5 | BRF_SND },           // 10 					/ MB834000
	{ "002-v13.v13",  0x080000, 0x0c39f3c8, 5 | BRF_SND },           // 11 					/ MB834000
	{ "002-v14.v14",  0x080000, 0xc7e11c38, 5 | BRF_SND },           // 12 					/ MB834000
	{ "002-v21.v21",  0x080000, 0x04a733d1, 6 | BRF_SND },           // 13 					/ MB834000
};

STDROMPICKEXT(bstars, bstars, neogeo)
STD_ROM_FN(bstars)

struct BurnDriver BurnDrvBstars = {
	"bstars", NULL, "neogeo", NULL, "1990",
	"Baseball Stars Professional (NGM-002)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, bstarsRomInfo, bstarsRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Baseball Stars Professional (NGH-002)
/* AES VERSION */

static struct BurnRomInfo bstarshRomDesc[] = {
	{ "002-p1.p1",    0x080000, 0x3bc7790e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ MB834200

	{ "002-s1.s1",    0x020000, 0x1a7fd0c6, 2 | BRF_GRA },           //  1 Text layer tiles / MB831000

	{ "002-c1.c1",    0x080000, 0xaaff2a45, 3 | BRF_GRA },           //  2 Sprite data 		/ MB834200
	{ "002-c2.c2",    0x080000, 0x3ba0f7e4, 3 | BRF_GRA },           //  3 					/ MB834200
	{ "002-c3.c3",    0x080000, 0x96f0fdfa, 3 | BRF_GRA },           //  4 					/ MB834200
	{ "002-c4.c4",    0x080000, 0x5fd87f2f, 3 | BRF_GRA },           //  5 					/ MB834200
	{ "002-c5.c5",    0x080000, 0x807ed83b, 3 | BRF_GRA },           //  6 					/ MB834200
	{ "002-c6.c6",    0x080000, 0x5a3cad41, 3 | BRF_GRA },           //  7 					/ MB834200

	{ "002-m1.m1",    0x040000, 0x4ecaa4ee, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code 		/ MB832000

	{ "002-v11.v11",  0x080000, 0xb7b925bd, 5 | BRF_SND },           //  9 Sound data 		/ MB834000
	{ "002-v12.v12",  0x080000, 0x329f26fc, 5 | BRF_SND },           // 10 					/ MB834000
	{ "002-v13.v13",  0x080000, 0x0c39f3c8, 5 | BRF_SND },           // 11 					/ MB834000
	{ "002-v14.v14",  0x080000, 0xc7e11c38, 5 | BRF_SND },           // 12 					/ MB834000
	{ "002-v21.v21",  0x080000, 0x04a733d1, 6 | BRF_SND },           // 13 					/ MB834000
};

STDROMPICKEXT(bstarsh, bstarsh, neogeo)
STD_ROM_FN(bstarsh)

struct BurnDriver BurnDrvBstarsh = {
	"bstarsh", "bstars", "neogeo", NULL, "1990",
	"Baseball Stars Professional (NGH-002)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, bstarshRomInfo, bstarshRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Top Player's Golf (NGM-003)(NGH-003)
/* MVS AND AES VERSION */

static struct BurnRomInfo tpgolfRomDesc[] = {
	{ "003-p1.p1",    0x080000, 0xf75549ba, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "003-p2.p2",    0x080000, 0xb7809a8f, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC534200

	{ "003-s1.s1",    0x020000, 0x7b3eb9b1, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "003-c1.c1",    0x080000, 0x0315fbaf, 3 | BRF_GRA },           //  3 Sprite data 		/ TC534200
	{ "003-c2.c2",    0x080000, 0xb4c15d59, 3 | BRF_GRA },           //  4 					/ TC534200
	{ "003-c3.c3",    0x080000, 0x8ce3e8da, 3 | BRF_GRA },           //  5 					/ TC534200
	{ "003-c4.c4",    0x080000, 0x29725969, 3 | BRF_GRA },           //  6 					/ TC534200
	{ "003-c5.c5",    0x080000, 0x9a7146da, 3 | BRF_GRA },           //  7 					/ TC534200
	{ "003-c6.c6",    0x080000, 0x1e63411a, 3 | BRF_GRA },           //  8 					/ TC534200
	{ "003-c7.c7",    0x080000, 0x2886710c, 3 | BRF_GRA },           //  9 					/ TC534200
	{ "003-c8.c8",    0x080000, 0x422af22d, 3 | BRF_GRA },           // 10 					/ TC534200

	{ "003-m1.m1",    0x020000, 0x4cc545e6, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code 		/ TC531001

	{ "003-v11.v11",  0x080000, 0xff97f1cb, 5 | BRF_SND },           // 12 Sound data 		/ TC534000
	// AES has different label, data is the same (also found on MVS): 003-v1.v11
	{ "003-v21.v21",  0x080000, 0xd34960c6, 6 | BRF_SND },           // 13 					/ TC534000
	// AES has different label, data is the same (also found on MVS): 003-v2.v21
	{ "003-v22.v22",  0x080000, 0x9a5f58d4, 6 | BRF_SND },           // 14 					/ TC534000
	{ "003-v23.v23",  0x080000, 0x30f53e54, 6 | BRF_SND },           // 15 					/ TC534000
	{ "003-v24.v24",  0x080000, 0x5ba0f501, 6 | BRF_SND },           // 16 					/ TC534000
};

STDROMPICKEXT(tpgolf, tpgolf, neogeo)
STD_ROM_FN(tpgolf)

struct BurnDriver BurnDrvTpgolf = {
	"tpgolf", NULL, "neogeo", NULL, "1990",
	"Top Player's Golf (NGM-003)(NGH-003)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, tpgolfRomInfo, tpgolfRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Mahjong Kyo Retsuden (NGM-004)(NGH-004)
/* MVS AND AES VERSION */

static struct BurnRomInfo mahretsuRomDesc[] = {
	{ "004-p1.p1",    0x080000, 0xfc6f53db, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "004-s1.s1",    0x020000, 0x2bd05a06, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "004-c1.c1",    0x080000, 0xf1ae16bc, 3 | BRF_GRA },           //  2 Sprite data 		/ MB834200
	{ "004-c2.c2",    0x080000, 0xbdc13520, 3 | BRF_GRA },           //  3 					/ MB834200
	{ "004-c3.c3",    0x080000, 0x9c571a37, 3 | BRF_GRA },           //  4 					/ MB834200
	{ "004-c4.c4",    0x080000, 0x7e81cb29, 3 | BRF_GRA },           //  5 					/ MB834200

	{ "004-m1.m1",    0x020000, 0xc71fbb3b, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "004-v11.v11",  0x080000, 0xb2fb2153, 5 | BRF_SND },           //  7 Sound data 		/ CXK384001
	{ "004-v12.v12",  0x080000, 0x8503317b, 5 | BRF_SND },           //  8 					/ CXK384001
	{ "004-v21.v21",  0x080000, 0x4999fb27, 6 | BRF_SND },           //  9 					/ CXK384001
	{ "004-v22.v22",  0x080000, 0x776fa2a2, 6 | BRF_SND },           // 10 					/ CXK384001
	{ "004-v23.v23",  0x080000, 0xb3e7eeea, 6 | BRF_SND },           // 11 					/ CXK384001
};

STDROMPICKEXT(mahretsu, mahretsu, neogeo)
STD_ROM_FN(mahretsu)

struct BurnDriver BurnDrvMahretsu = {
	"mahretsu", NULL, "neogeo", NULL, "1990",
	"Mahjong Kyo Retsuden (NGM-004)(NGH-004)\0", NULL, "SNK", "Neo Geo MVS",
	L"\u9EBB\u96C0\u72C2\u5217\u4F1D\0Mahjong Kyo Retsuden (NGM-004)(NGH-004)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_MAHJONG, GBF_MAHJONG, 0,
	NULL, mahretsuRomInfo, mahretsuRomName, NULL, NULL, NULL, NULL, neomahjongInputInfo, neomahjongDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Riding Hero (NGM-006)(NGH-006)
/* MVS AND AES VERSION */

static struct BurnRomInfo ridheroRomDesc[] = {
	{ "006-p1.p1",    0x080000, 0xd4aaf597, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ MB834200

	{ "006-s1.s1",    0x020000, 0xeb5189f0, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "006-c1.c1",    0x080000, 0x4a5c7f78, 3 | BRF_GRA },           //  2 Sprite data 		/ CXK384000
	{ "006-c2.c2",    0x080000, 0xe0b70ece, 3 | BRF_GRA },           //  3 					/ CXK384000
	{ "006-c3.c3",    0x080000, 0x8acff765, 3 | BRF_GRA },           //  4 					/ CXK384000
	{ "006-c4.c4",    0x080000, 0x205e3208, 3 | BRF_GRA },           //  5 					/ CXK384000

	{ "006-m1.m1",    0x040000, 0x92e7b4fe, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ MB832000

	{ "006-v11.v11",  0x080000, 0xcdf74a42, 5 | BRF_SND },           //  7 Sound data 		/ MB834000
	{ "006-v12.v12",  0x080000, 0xe2fd2371, 5 | BRF_SND },           //  8 					/ MB834000
	{ "006-v21.v21",  0x080000, 0x94092bce, 6 | BRF_SND },           //  9 					/ MB834000
	{ "006-v22.v22",  0x080000, 0x4e2cd7c3, 6 | BRF_SND },           // 10 					/ MB834000
	{ "006-v23.v23",  0x080000, 0x069c71ed, 6 | BRF_SND },           // 11 					/ MB834000
	{ "006-v24.v24",  0x080000, 0x89fbb825, 6 | BRF_SND },           // 12 					/ MB834000

	{ "rhcom.bin", 	  0x002000, 0xe5cd6306, 0 | BRF_OPT }, 			 // 13 mcu				/ Hitachi HD6301V1 MCU
	/* dumped from a prototype with external ROM, not 100% confirmed as being the same on a final, or other games (lbowling, trally) */
};

STDROMPICKEXT(ridhero, ridhero, neogeo)
STD_ROM_FN(ridhero)

struct BurnDriver BurnDrvRidhero = {
	"ridhero", NULL, "neogeo", NULL, "1990",
	"Riding Hero (NGM-006)(NGH-006)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RACING, 0,
	NULL, ridheroRomInfo, ridheroRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Riding Hero (set 2)

static struct BurnRomInfo ridherohRomDesc[] = {
	{ "006-pg1.p1",   0x080000, 0x52445646, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ MB834200
	/* Chip label p1h does not exist, renamed temporarly to pg1, marked BAD_DUMP.
	This needs to be verified. */
	{ "006-s1.s1",    0x020000, 0xeb5189f0, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "006-c1.c1",    0x080000, 0x4a5c7f78, 3 | BRF_GRA },           //  2 Sprite data 		/ CXK384000
	{ "006-c2.c2",    0x080000, 0xe0b70ece, 3 | BRF_GRA },           //  3 					/ CXK384000
	{ "006-c3.c3",    0x080000, 0x8acff765, 3 | BRF_GRA },           //  4 					/ CXK384000
	{ "006-c4.c4",    0x080000, 0x205e3208, 3 | BRF_GRA },           //  5 					/ CXK384000

	{ "006-m1.m1",    0x040000, 0x92e7b4fe, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ MB832000

	{ "006-v11.v11",  0x080000, 0xcdf74a42, 5 | BRF_SND },           //  7 Sound data 		/ MB834000
	{ "006-v12.v12",  0x080000, 0xe2fd2371, 5 | BRF_SND },           //  8 					/ MB834000
	{ "006-v21.v21",  0x080000, 0x94092bce, 6 | BRF_SND },           //  9 					/ MB834000
	{ "006-v22.v22",  0x080000, 0x4e2cd7c3, 6 | BRF_SND },           // 10 					/ MB834000
	{ "006-v23.v23",  0x080000, 0x069c71ed, 6 | BRF_SND },           // 11 					/ MB834000
	{ "006-v24.v24",  0x080000, 0x89fbb825, 6 | BRF_SND },           // 12 					/ MB834000

	{ "rhcom.bin", 	  0x002000, 0xe5cd6306, 0 | BRF_OPT }, 			 // 13 mcu				/ Hitachi HD6301V1 MCU
	/* dumped from a prototype with external ROM, not 100% confirmed as being the same on a final, or other games (lbowling, trally) */
};

STDROMPICKEXT(ridheroh, ridheroh, neogeo)
STD_ROM_FN(ridheroh)

struct BurnDriver BurnDrvRidheroh = {
	"ridheroh", "ridhero", "neogeo", NULL, "1990",
	"Riding Hero (set 2)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RACING, 0,
	NULL, ridherohRomInfo, ridherohRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Alpha Mission II / ASO II - Last Guardian (NGM-007)(NGH-007)
/* MVS AND AES VERSION */

static struct BurnRomInfo alpham2RomDesc[] = {
	{ "007-p1.p1",    0x080000, 0x5b266f47, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "007-p2.p2",    0x020000, 0xeb9c1044, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC531024

	{ "007-s1.s1",    0x020000, 0x85ec9acf, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "007-c1.c1",    0x100000, 0x8fba8ff3, 3 | BRF_GRA },           //  3 Sprite data 		/ TC538200
	{ "007-c2.c2",    0x100000, 0x4dad2945, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "007-c3.c3",    0x080000, 0x68c2994e, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "007-c4.c4",    0x080000, 0x7d588349, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "007-m1.m1",    0x020000, 0x28dfe2cd, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "007-v1.v1",    0x100000, 0xcd5db931, 5 | BRF_SND },           //  8 Sound data 		/ TC538200
	{ "007-v2.v2",    0x100000, 0x63e9b574, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(alpham2, alpham2, neogeo)
STD_ROM_FN(alpham2)

struct BurnDriver BurnDrvAlpham2 = {
	"alpham2", NULL, "neogeo", NULL, "1991",
	"Alpha Mission II / ASO II - Last Guardian (NGM-007)(NGH-007)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VERSHOOT, 0,
	NULL, alpham2RomInfo, alpham2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Alpha Mission II / ASO II - Last Guardian (prototype)
/* early prototype - all roms were hand labeled with CRCs, dumps verified against them */
/* AES VERSION */

static struct BurnRomInfo alpham2pRomDesc[] = {
	{ "007_p1_faac.p1",    0x080000, 0xc763e52a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "007_p2_1813.p2",    0x080000, 0x7a0b435c, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "007_s1_36f1.s1",    0x020000, 0xefc9ae2e, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "007_c1_210e.c11",   0x080000, 0x24841639, 3 | BRF_GRA },           //  3 Sprite data
	{ "007_c2_29ae.c12",   0x080000, 0x912763ab, 3 | BRF_GRA },           //  4
	{ "007_c3_c873.c13",   0x080000, 0x0743bde2, 3 | BRF_GRA },           //  5
	{ "007_c4_2462.c14",   0x080000, 0x61240212, 3 | BRF_GRA },           //  6
	{ "007_c5_5f16.c15",   0x080000, 0xcf9f4c53, 3 | BRF_GRA },           //  7
	{ "007_c6_8abc.c16",   0x080000, 0x3d903b19, 3 | BRF_GRA },           //  8
	{ "007_c7_c983.c17",   0x080000, 0xe41e3875, 3 | BRF_GRA },           //  9
	{ "007_c8_b599.c18",   0x080000, 0x4483e2cf, 3 | BRF_GRA },           //  10

	{ "007_m1_9384.m1",    0x020000, 0x5976b464, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	{ "007_v11_1bb4.v11",  0x080000, 0x18eaa9e1, 5 | BRF_SND },           //  12 Sound data
	{ "007_v12_c8e8.v12",  0x080000, 0x2487d495, 5 | BRF_SND },           //  13
	{ "007_v13_09fa.v13",  0x080000, 0x25e60f25, 5 | BRF_SND },           //  14
	{ "007_v21_28c2.v21",  0x080000, 0xac44b75a, 5 | BRF_SND },           //  15
};

STDROMPICKEXT(alpham2p, alpham2p, neogeo)
STD_ROM_FN(alpham2p)

static void Alpham2pCallback()
{
	BurnLoadRom(Neo68KROMActive + 0x000000, 0, 2);
	BurnLoadRom(Neo68KROMActive + 0x000001, 1, 2);
}

static INT32 Alpham2pInit()
{
	NeoCallbackActive->pInitialise = Alpham2pCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvAlpham2p = {
	"alpham2p", "alpham2", "neogeo", NULL, "1991",
	"Alpha Mission II / ASO II - Last Guardian (prototype)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SPRITE32, GBF_VERSHOOT, 0,
	NULL, alpham2pRomInfo, alpham2pRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	Alpham2pInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Cyber-Lip (NGM-010)

static struct BurnRomInfo cyberlipRomDesc[] = {
	/* also found AES set on PROG board NEO-AEG PROG-B and CHA board NEO-AEG CHA-32;
	same chip labels */
	{ "010-p1.p1",    0x080000, 0x69a6b42d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "010-s1.s1",    0x020000, 0x79a35264, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "010-c1.c1",    0x080000, 0x8bba5113, 3 | BRF_GRA },           //  2 Sprite data 		/ TC534200
	{ "010-c2.c2",    0x080000, 0xcbf66432, 3 | BRF_GRA },           //  3 					/ TC534200
	{ "010-c3.c3",    0x080000, 0xe4f86efc, 3 | BRF_GRA },           //  4 					/ TC534200
	{ "010-c4.c4",    0x080000, 0xf7be4674, 3 | BRF_GRA },           //  5 					/ TC534200
	{ "010-c5.c5",    0x080000, 0xe8076da0, 3 | BRF_GRA },           //  6 					/ TC534200
	{ "010-c6.c6",    0x080000, 0xc495c567, 3 | BRF_GRA },           //  7 					/ TC534200

	{ "010-m1.m1",    0x020000, 0x8be3a078, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code 		/ TC531001

	{ "010-v11.v11",  0x080000, 0x90224d22, 5 | BRF_SND },           //  9 Sound data 		/ TC534000
	{ "010-v12.v12",  0x080000, 0xa0cf1834, 5 | BRF_SND },           // 10 					/ TC534000
	{ "010-v13.v13",  0x080000, 0xae38bc84, 5 | BRF_SND },           // 11 					/ TC534000
	{ "010-v14.v14",  0x080000, 0x70899bd2, 5 | BRF_SND },           // 12 					/ TC534000
	{ "010-v21.v21",  0x080000, 0x586f4cb2, 6 | BRF_SND },           // 13 					/ TC534000
};

STDROMPICKEXT(cyberlip, cyberlip, neogeo)
STD_ROM_FN(cyberlip)

struct BurnDriver BurnDrvCyberlip = {
	"cyberlip", NULL, "neogeo", NULL, "1990",
	"Cyber-Lip (NGM-010)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, 0,
	NULL, cyberlipRomInfo, cyberlipRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The Super Spy (NGM-011)(NGH-011)
/* MVS AND AES VERSION */

static struct BurnRomInfo superspyRomDesc[] = {
	{ "011-p1.p1",    0x080000, 0xc7f944b5, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ MB834200
	{ "sp2.p2",       0x020000, 0x811a4faf, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC531024

	{ "011-s1.s1",    0x020000, 0xec5fdb96, 2 | BRF_GRA },           //  2 Text layer tiles / MB831000

	{ "011-c1.c1",    0x100000, 0xcae7be57, 3 | BRF_GRA },           //  3 Sprite data 		/ MB838200
	{ "011-c2.c2",    0x100000, 0x9e29d986, 3 | BRF_GRA },           //  4 					/ MB838200
	{ "011-c3.c3",    0x100000, 0x14832ff2, 3 | BRF_GRA },           //  5 					/ MB838200
	{ "011-c4.c4",    0x100000, 0xb7f63162, 3 | BRF_GRA },           //  6 					/ MB838200

	{ "011-m1.m1",    0x040000, 0xca661f1b, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ MB832000

	{ "011-v11.v11",  0x100000, 0x5c674d5c, 5 | BRF_SND },           //  8 Sound data 		/ MB838000
	{ "011-v12.v12",  0x080000, 0x9f513d5a, 5 | BRF_SND },           //  9 					/ MB834000
	{ "011-v21.v21",  0x080000, 0x426cd040, 6 | BRF_SND },           // 10 					/ MB834000
};

STDROMPICKEXT(superspy, superspy, neogeo)
STD_ROM_FN(superspy)

struct BurnDriver BurnDrvSuperspy = {
	"superspy", NULL, "neogeo", NULL, "1990",
	"The Super Spy (NGM-011)(NGH-011)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, superspyRomInfo, superspyRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Mutation Nation (NGM-014)(NGH-014)
/* MVS AND AES VERSION */

static struct BurnRomInfo mutnatRomDesc[] = {
	{ "014-p1.p1",    0x080000, 0x6f1699c8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ CXK384500

	{ "014-s1.s1",    0x020000, 0x99419733, 2 | BRF_GRA },           //  1 Text layer tiles / CXK381000

	{ "014-c1.c1",    0x100000, 0x5e4381bf, 3 | BRF_GRA },           //  2 Sprite data 		/ CXK388000
	{ "014-c2.c2",    0x100000, 0x69ba4e18, 3 | BRF_GRA },           //  3 					/ CXK388000
	{ "014-c3.c3",    0x100000, 0x890327d5, 3 | BRF_GRA },           //  4 					/ CXK388000
	{ "014-c4.c4",    0x100000, 0xe4002651, 3 | BRF_GRA },           //  5 					/ CXK388000

	{ "014-m1.m1",    0x020000, 0xb6683092, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ CXK381003A

	{ "014-v1.v1",    0x100000, 0x25419296, 5 | BRF_SND },           //  7 Sound data 		/ CXK388000
	{ "014-v2.v2",    0x100000, 0x0de53d5e, 5 | BRF_SND },           //  8 					/ CXK388000
};

STDROMPICKEXT(mutnat, mutnat, neogeo)
STD_ROM_FN(mutnat)

struct BurnDriver BurnDrvMutnat = {
	"mutnat", NULL, "neogeo", NULL, "1992",
	"Mutation Nation (NGM-014)(NGH-014)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, mutnatRomInfo, mutnatRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// King of the Monsters (set 1)
/* MVS VERSION */

static struct BurnRomInfo kotmRomDesc[] = {
	{ "016-p1.p1",    0x080000, 0x1b818731, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "016-p2.p2",    0x020000, 0x12afdc2b, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC531024

	{ "016-s1.s1",    0x020000, 0x1a2eeeb3, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "016-c1.c1",    0x100000, 0x71471c25, 3 | BRF_GRA },           //  3 Sprite data 		/ TC538200
	{ "016-c2.c2",    0x100000, 0x320db048, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "016-c3.c3",    0x100000, 0x98de7995, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "016-c4.c4",    0x100000, 0x070506e2, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "016-m1.m1",    0x020000, 0x9da9ca10, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "016-v1.v1",    0x100000, 0x86c0a502, 5 | BRF_SND },           //  8 Sound data 		/ TC538200
	{ "016-v2.v2",    0x100000, 0x5bc23ec5, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(kotm, kotm, neogeo)
STD_ROM_FN(kotm)

struct BurnDriver BurnDrvKotm = {
	"kotm", NULL, "neogeo", NULL, "1991",
	"King of the Monsters (set 1)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, kotmRomInfo, kotmRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// King of the Monsters (set 2)
/* AES VERSION */

static struct BurnRomInfo kotmhRomDesc[] = {
	{ "016-hp1.p1",   0x080000, 0xb774621e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "016-p2.p2",    0x020000, 0x12afdc2b, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC531024

	{ "016-s1.s1",    0x020000, 0x1a2eeeb3, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "016-c1.c1",    0x100000, 0x71471c25, 3 | BRF_GRA },           //  3 Sprite data 		/ TC538200
	{ "016-c2.c2",    0x100000, 0x320db048, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "016-c3.c3",    0x100000, 0x98de7995, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "016-c4.c4",    0x100000, 0x070506e2, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "016-m1.m1",    0x020000, 0x9da9ca10, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "016-v1.v1",    0x100000, 0x86c0a502, 5 | BRF_SND },           //  8 Sound data 		/ TC538200
	{ "016-v2.v2",    0x100000, 0x5bc23ec5, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(kotmh, kotmh, neogeo)
STD_ROM_FN(kotmh)

struct BurnDriver BurnDrvKotmh = {
	"kotmh", "kotm", "neogeo", NULL, "1991",
	"King of the Monsters (set 2)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, kotmhRomInfo, kotmhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Sengoku / Sengoku Denshou (NGM-017)(NGH-017)
/* MVS AND AES VERSION */

static struct BurnRomInfo sengokuRomDesc[] = {
	{ "017-p1.p1",    0x080000, 0xf8a63983, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "017-p2.p2",    0x020000, 0x3024bbb3, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC531024

	{ "017-s1.s1",    0x020000, 0xb246204d, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "017-c1.c1",    0x100000, 0xb4eb82a1, 3 | BRF_GRA },           //  3 Sprite data 		/ TC538200
	{ "017-c2.c2",    0x100000, 0xd55c550d, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "017-c3.c3",    0x100000, 0xed51ef65, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "017-c4.c4",    0x100000, 0xf4f3c9cb, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "017-m1.m1",    0x020000, 0x9b4f34c6, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "017-v1.v1",    0x100000, 0x23663295, 5 | BRF_SND },           //  8 Sound data 		/ TC538200
	{ "017-v2.v2",    0x100000, 0xf61e6765, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(sengoku, sengoku, neogeo)
STD_ROM_FN(sengoku)

struct BurnDriver BurnDrvSengoku = {
	"sengoku", NULL, "neogeo", NULL, "1991",
	"Sengoku / Sengoku Denshou (NGM-017)(NGH-017)\0", NULL, "SNK", "Neo Geo MVS",
	L"Sengoku / Sengoku Denshou (NGM-017)(NGH-017)\0\u6226\u56FD\u4F1D\u627F\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, sengokuRomInfo, sengokuRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Sengoku / Sengoku Denshou (NGH-017)(US)
/* AES VERSION (US) */

static struct BurnRomInfo sengokuhRomDesc[] = {
	{ "017-hp1.p1",   0x080000, 0x33eccae0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "017-p2.p2",    0x020000, 0x3024bbb3, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC531024

	{ "017-s1.s1",    0x020000, 0xb246204d, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "017-c1.c1",    0x100000, 0xb4eb82a1, 3 | BRF_GRA },           //  3 Sprite data 		/ TC538200
	{ "017-c2.c2",    0x100000, 0xd55c550d, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "017-c3.c3",    0x100000, 0xed51ef65, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "017-c4.c4",    0x100000, 0xf4f3c9cb, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "017-m1.m1",    0x020000, 0x9b4f34c6, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "017-v1.v1",    0x100000, 0x23663295, 5 | BRF_SND },           //  8 Sound data 		/ TC538200
	{ "017-v2.v2",    0x100000, 0xf61e6765, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(sengokuh, sengokuh, neogeo)
STD_ROM_FN(sengokuh)

struct BurnDriver BurnDrvSengokuh = {
	"sengokuh", "sengoku", "neogeo", NULL, "1991",
	"Sengoku / Sengoku Denshou (NGH-017)(US)\0", NULL, "SNK", "Neo Geo MVS",
	L"Sengoku / Sengoku Denshou (NGH-017)(US)\0\u6226\u56FD\u4F1D\u627F\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, sengokuhRomInfo, sengokuhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Burning Fight (NGM-018)(NGH-018)
/* MVS AND AES VERSION */

static struct BurnRomInfo burningfRomDesc[] = {
	{ "018-p1.p1",    0x080000, 0x4092c8db, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ HN62434

	{ "018-s1.s1",    0x020000, 0x6799ea0d, 2 | BRF_GRA },           //  1 Text layer tiles / HN62321

	{ "018-c1.c1",    0x100000, 0x25a25e9b, 3 | BRF_GRA },           //  2 Sprite data 		/ HN62408
	{ "018-c2.c2",    0x100000, 0xd4378876, 3 | BRF_GRA },           //  3 					/ HN62408
	{ "018-c3.c3",    0x100000, 0x862b60da, 3 | BRF_GRA },           //  4 					/ HN62408
	{ "018-c4.c4",    0x100000, 0xe2e0aff7, 3 | BRF_GRA },           //  5 					/ HN62408

	{ "018-m1.m1",    0x020000, 0x0c939ee2, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ HN62321A

	{ "018-v1.v1",    0x100000, 0x508c9ffc, 5 | BRF_SND },           //  7 Sound data 		/ HN62408
	{ "018-v2.v2",    0x100000, 0x854ef277, 5 | BRF_SND },           //  8 					/ HN62408
};

STDROMPICKEXT(burningf, burningf, neogeo)
STD_ROM_FN(burningf)

struct BurnDriver BurnDrvBurningf = {
	"burningf", NULL, "neogeo", NULL, "1991",
	"Burning Fight (NGM-018)(NGH-018)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, burningfRomInfo, burningfRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Burning Fight (NGH-018)(US)
/* AES VERSION (US) */

static struct BurnRomInfo burningfhRomDesc[] = {
	{ "018-hp1.p1",   0x080000, 0xddffcbf4, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ HN62434

	{ "018-s1.s1",    0x020000, 0x6799ea0d, 2 | BRF_GRA },           //  1 Text layer tiles / HN62321

	{ "018-c1.c1",    0x100000, 0x25a25e9b, 3 | BRF_GRA },           //  2 Sprite data 		/ HN62408
	{ "018-c2.c2",    0x100000, 0xd4378876, 3 | BRF_GRA },           //  3 					/ HN62408
	{ "018-c3.c3",    0x100000, 0x862b60da, 3 | BRF_GRA },           //  4 					/ HN62408
	{ "018-c4.c4",    0x100000, 0xe2e0aff7, 3 | BRF_GRA },           //  5 					/ HN62408

	{ "018-m1.m1",    0x020000, 0x0c939ee2, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ HN62321A

	{ "018-v1.v1",    0x100000, 0x508c9ffc, 5 | BRF_SND },           //  7 Sound data 		/ HN62408
	{ "018-v2.v2",    0x100000, 0x854ef277, 5 | BRF_SND },           //  8 					/ HN62408
};

STDROMPICKEXT(burningfh, burningfh, neogeo)
STD_ROM_FN(burningfh)

struct BurnDriver BurnDrvBurningfh = {
	"burningfh", "burningf", "neogeo", NULL, "1991",
	"Burning Fight (NGH-018)(US)\0", NULL, "SNK", "Neo Geo AES",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, burningfhRomInfo, burningfhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Burning Fight (prototype, older)
/* early prototype - all roms were hand labeled with CRCs, dumps verified against them */

static struct BurnRomInfo burningfpRomDesc[] = {
	{ "018_p1_9397.p1",    0x080000, 0x5b4032e7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "018_p2_e335.p2",    0x080000, 0x78762f68, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "018_s1_9899.s1",    0x020000, 0xf3d130e8, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "018_c1_ba06.c11",   0x080000, 0x6569018b, 3 | BRF_GRA },           //  3 Sprite data
	{ "018_c2_d534.c12",   0x080000, 0x6949b501, 3 | BRF_GRA },           //  4
	{ "018_c3_9d70.c13",   0x080000, 0x410f653b, 3 | BRF_GRA },           //  5
	{ "018_c4_bf9d.c14",   0x080000, 0xd43bf2a5, 3 | BRF_GRA },           //  6
	{ "018_c5_ac9d.c15",   0x080000, 0x837d09d3, 3 | BRF_GRA },           //  7
	{ "018_c6_d2a5.c16",   0x080000, 0x5fee51e7, 3 | BRF_GRA },           //  8
	{ "018_c7_d568.c17",   0x080000, 0x0f3f0823, 3 | BRF_GRA },           //  9
	{ "018_c8_d256.c18",   0x080000, 0x67cc9e34, 3 | BRF_GRA },           //  10

	{ "018_m1_4586.m1",    0x020000, 0x470dd5d4, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	{ "018_v11_56ac.v11",  0x080000, 0xdc07ea3c, 5 | BRF_SND },           //  12 Sound data
	{ "018_v12_db95.v12",  0x080000, 0xf1ae637c, 5 | BRF_SND },           //  13
	{ "018_v21_98dd.v21",  0x080000, 0x9f3b4eda, 5 | BRF_SND },           //  14
};

STDROMPICKEXT(burningfp, burningfp, neogeo)
STD_ROM_FN(burningfp)

static void BurningfpCallback()
{
	BurnLoadRom(Neo68KROMActive + 0x000000, 0, 2);
	BurnLoadRom(Neo68KROMActive + 0x000001, 1, 2);
}

static INT32 BurningfpInit()
{
	NeoCallbackActive->pInitialise = BurningfpCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvBurningfp = {
	"burningfp", "burningf", "neogeo", NULL, "1991",
	"Burning Fight (prototype, older)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SPRITE32, GBF_SCRFIGHT, 0,
	NULL, burningfpRomInfo, burningfpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	BurningfpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Burning Fight (prototype, near final, ver 23.3, 910326)
/* later prototype - Sx, Vx and Cx data all matches final game, but with different rom arranagement, Px & Mx unique */

static struct BurnRomInfo burningfpaRomDesc[] = {
	{ "018_p1_1f28.podd",  0x080000, 0xf7d15752, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "018_p2_3217.peven", 0x080000, 0xffae22fb, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "018_s1_4491.s1",    0x020000, 0x6799ea0d, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "018_c1_2505.c1",    0x080000, 0x3a441c6a, 3 | BRF_GRA },           //  3 Sprite data
	{ "018_c2_3f55.c2",    0x080000, 0xbb72404b, 3 | BRF_GRA },           //  4
	{ "018_c3_a20d.c3",    0x080000, 0x87bffd2f, 3 | BRF_GRA },           //  5
	{ "018_c4_c6e1.c4",    0x080000, 0x33803163, 3 | BRF_GRA },           //  6
	{ "018_c5_2cd1.c5",    0x080000, 0x74391952, 3 | BRF_GRA },           //  7
	{ "018_c6_8135.c6",    0x080000, 0x95e220e6, 3 | BRF_GRA },           //  8
	{ "018_c7_d009.c7",    0x080000, 0x41326c0d, 3 | BRF_GRA },           //  9
	{ "018_c8_dc63.c8",    0x080000, 0xed3b1f04, 3 | BRF_GRA },           //  10

	{ "018_m1_d13e.m1",    0x020000, 0x2b0c0415, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	{ "018_v11_6c00.v11",  0x080000, 0xb55b9670, 5 | BRF_SND },           //  12 Sound data
	{ "018_v12_8146.v12",  0x080000, 0xa0bcf260, 5 | BRF_SND },           //  13
	{ "018_v13_b813.v13",  0x080000, 0x270f4707, 5 | BRF_SND },           //  14
	{ "018_v24_22ee.v24",  0x080000, 0x924e3d69, 5 | BRF_SND },           //  15
};

STDROMPICKEXT(burningfpa, burningfpa, neogeo)
STD_ROM_FN(burningfpa)

struct BurnDriver BurnDrvBurningfpa = {
	"burningfpa", "burningf", "neogeo", NULL, "1991",
	"Burning Fight (prototype, near final, ver 23.3, 910326)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SPRITE32, GBF_SCRFIGHT, 0,
	NULL, burningfpaRomInfo, burningfpaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	BurningfpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Burning Fight (prototype, newer, V07)
/* later prototype, but Sx, Cx, Vx etc. not yet final, Eproms are labeled Ver07 from Feb 1991 */

static struct BurnRomInfo burningfpbRomDesc[] = {
	{ "p1.bin",  		   0x080000, 0xf05ce157, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "p2.bin", 		   0x080000, 0x768ddc90, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "s1.bin",    		   0x020000, 0x3effc183, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "c1.bin",    		   0x080000, 0x8fd6a9bb, 3 | BRF_GRA },           //  3 Sprite data
	{ "c2.bin",    		   0x080000, 0x7c3464b8, 3 | BRF_GRA },           //  4
	{ "c3.bin",    		   0x080000, 0x0ac8f1be, 3 | BRF_GRA },           //  5
	{ "c4.bin",    		   0x080000, 0x3af6b235, 3 | BRF_GRA },           //  6
	{ "c5.bin",    		   0x080000, 0x707205ab, 3 | BRF_GRA },           //  7
	{ "c6.bin",    		   0x080000, 0x10d0d2cf, 3 | BRF_GRA },           //  8
	{ "c7.bin",    		   0x080000, 0xdef06900, 3 | BRF_GRA },           //  9
	{ "c8.bin",    		   0x080000, 0x6fdc1691, 3 | BRF_GRA },           //  10

	{ "m1.bin",    		   0x020000, 0x3031af09, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	{ "v11.bin",  		   0x080000, 0x20b40b70, 5 | BRF_SND },           //  12 Sound data
	{ "v12.bin",  		   0x080000, 0xa0bcf260, 5 | BRF_SND },           //  13
	{ "v13.bin",  		   0x080000, 0xa4e0e58a, 5 | BRF_SND },           //  14
	{ "v24-bad.bin",  	   0x080000, 0x4389e65a, 0 | BRF_SND },           //  15 chip was physically damaged
	// load over it with the final data, could be different however
	{ "018_v24_22ee.v24",  0x080000, 0x924e3d69, 5 | BRF_SND },           //  15
};

STDROMPICKEXT(burningfpb, burningfpb, neogeo)
STD_ROM_FN(burningfpb)

struct BurnDriver BurnDrvBurningfpb = {
	"burningfpb", "burningf", "neogeo", NULL, "1991",
	"Burning Fight (prototype, newer, V07)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SPRITE32, GBF_SCRFIGHT, 0,
	NULL, burningfpbRomInfo, burningfpbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	BurningfpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// League Bowling (NGM-019)(NGH-019)
/* MVS AND AES VERSION */

static struct BurnRomInfo lbowlingRomDesc[] = {
	{ "019-p1.p1",    0x080000, 0xa2de8445, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "019-s1.s1",    0x020000, 0x5fcdc0ed, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "019-c1.c1",    0x080000, 0x4ccdef18, 3 | BRF_GRA },           //  2 Sprite data 		/ TC534200
	{ "019-c2.c2",    0x080000, 0xd4dd0802, 3 | BRF_GRA },           //  3 					/ TC534200

	{ "019-m1.m1",    0x020000, 0xd568c17d, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code 		/ TC531001

	{ "019-v11.v11",  0x080000, 0x0fb74872, 5 | BRF_SND },           //  5 Sound data 		/ TC534000
	{ "019-v12.v12",  0x080000, 0x029faa57, 5 | BRF_SND },           //  6 					/ TC534000
	{ "019-v21.v21",  0x080000, 0x2efd5ada, 6 | BRF_SND },           //  7 					/ TC534000

	{ "hd6301v1p.com", 0x001000, 0x00000000, BRF_OPT | BRF_NODUMP }, // Hitachi HD6301V1 MCU
};

STDROMPICKEXT(lbowling, lbowling, neogeo)
STD_ROM_FN(lbowling)

struct BurnDriver BurnDrvLbowling = {
	"lbowling", NULL, "neogeo", NULL, "1990",
	"League Bowling (NGM-019)(NGH-019)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, lbowlingRomInfo, lbowlingRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Ghost Pilots (NGM-020)(NGH-020)
/* MVS AND AES VERSION */

static struct BurnRomInfo gpilotsRomDesc[] = {
	{ "020-p1.p1",    0x080000, 0xe6f2fe64, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "020-p2.p2",    0x020000, 0xedcb22ac, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC531024

	{ "020-s1.s1",    0x020000, 0xa6d83d53, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "020-c1.c1",    0x100000, 0xbd6fe78e, 3 | BRF_GRA },           //  3 Sprite data 		/ TC538200
	{ "020-c2.c2",    0x100000, 0x5f4a925c, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "020-c3.c3",    0x100000, 0xd1e42fd0, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "020-c4.c4",    0x100000, 0xedde439b, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "020-m1.m1",    0x020000, 0x48409377, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "020-v11.v11",  0x100000, 0x1b526c8b, 5 | BRF_SND },           //  8 Sound data 		/ TC538200
	{ "020-v12.v12",  0x080000, 0x4a9e6f03, 5 | BRF_SND },           //  9 					/ TC534200
	{ "020-v21.v21",  0x080000, 0x7abf113d, 6 | BRF_SND },           // 10 					/ TC534200
};

STDROMPICKEXT(gpilots, gpilots, neogeo)
STD_ROM_FN(gpilots)

struct BurnDriver BurnDrvGpilots = {
	"gpilots", NULL, "neogeo", NULL, "1991",
	"Ghost Pilots (NGM-020)(NGH-020)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VERSHOOT, 0,
	NULL, gpilotsRomInfo, gpilotsRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Ghost Pilots (NGH-020)(US)
/* AES VERSION (US) */

static struct BurnRomInfo gpilotshRomDesc[] = {
	{ "020-hp1.p1",   0x080000, 0x7cdb01ce, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "020-p2.p2",    0x020000, 0xedcb22ac, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC531024

	{ "020-s1.s1",    0x020000, 0xa6d83d53, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "020-c1.c1",    0x100000, 0xbd6fe78e, 3 | BRF_GRA },           //  3 Sprite data 		/ TC538200
	{ "020-c2.c2",    0x100000, 0x5f4a925c, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "020-c3.c3",    0x100000, 0xd1e42fd0, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "020-c4.c4",    0x100000, 0xedde439b, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "020-m1.m1",    0x020000, 0x48409377, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "020-v11.v11",  0x100000, 0x1b526c8b, 5 | BRF_SND },           //  8 Sound data 		/ TC538200
	{ "020-v12.v12",  0x080000, 0x4a9e6f03, 5 | BRF_SND },           //  9 					/ TC534200
	{ "020-v21.v21",  0x080000, 0x7abf113d, 6 | BRF_SND },           // 10 					/ TC534200
};

STDROMPICKEXT(gpilotsh, gpilotsh, neogeo)
STD_ROM_FN(gpilotsh)

struct BurnDriver BurnDrvGpilotsh = {
	"gpilotsh", "gpilots", "neogeo", NULL, "1991",
	"Ghost Pilots (NGH-020)(US)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VERSHOOT, 0,
	NULL, gpilotshRomInfo, gpilotshRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Ghost Pilots (prototype)

static struct BurnRomInfo gpilotspRomDesc[] = {
	{ "p1",           0x080000, 0xc42125b4, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "p2",           0x080000, 0x5a4db1f3, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "s1",           0x020000, 0xa6d83d53, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "c11",          0x080000, 0x7a5158b3, 3 | BRF_GRA },           //  3 Sprite data
	{ "c12",          0x080000, 0x41e67a5a, 3 | BRF_GRA },           //  4
	{ "c13",          0x080000, 0x5f80c0d0, 3 | BRF_GRA },           //  5
	{ "c14",          0x080000, 0x61227b93, 3 | BRF_GRA },           //  6
	{ "c15",          0x080000, 0x96de9bb5, 3 | BRF_GRA },           //  7
	{ "c16",          0x080000, 0x2b904c85, 3 | BRF_GRA },           //  8
	{ "c17",          0x080000, 0xc0d7517d, 3 | BRF_GRA },           //  9
	{ "c18",          0x080000, 0x75569e35, 3 | BRF_GRA },           //  10

	{ "m1",           0x020000, 0x32a108cf, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	{ "v11",          0x080000, 0x8cc44140, 5 | BRF_SND },           //  12 Sound data
	{ "v12",          0x080000, 0x415c61cd, 5 | BRF_SND },           //  13
	{ "v13",          0x080000, 0x4a9e6f03, 5 | BRF_SND },           //  14
	{ "v21",          0x080000, 0x7abf113d, 6 | BRF_SND },           //  15
};

STDROMPICKEXT(gpilotsp, gpilotsp, neogeo)
STD_ROM_FN(gpilotsp)

static void GpilotspCallback()
{
	BurnLoadRom(Neo68KROMActive + 0x000000, 0, 2);
	BurnLoadRom(Neo68KROMActive + 0x000001, 1, 2);
}

static INT32 GpilotspInit()
{
	NeoCallbackActive->pInitialise = GpilotspCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvGpilotsp = {
	"gpilotsp", "gpilots", "neogeo", NULL, "1991",
	"Ghost Pilots (prototype)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SPRITE32, GBF_VERSHOOT, 0,
	NULL, gpilotspRomInfo, gpilotspRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	GpilotspInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Puzzled / Joy Joy Kid (NGM-021)(NGH-021)
/* MVS AND AES VERSION */

static struct BurnRomInfo joyjoyRomDesc[] = {
	{ "021-p1.p1",    0x080000, 0x39c3478f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ MB834200

	{ "021-s1.s1",    0x020000, 0x6956d778, 2 | BRF_GRA },           //  1 Text layer tiles / MB831000

	{ "021-c1.c1",    0x080000, 0x509250ec, 3 | BRF_GRA },           //  2 Sprite data 		/ MB834200
	{ "021-c2.c2",    0x080000, 0x09ed5258, 3 | BRF_GRA },           //  3 					/ MB834200

	{ "021-m1.m1",    0x040000, 0x5a4be5e8, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code 		/ MB832000

	{ "021-v11.v11",  0x080000, 0x66c1e5c4, 5 | BRF_SND },           //  5 Sound data 		/ MB834200
	{ "021-v21.v21",  0x080000, 0x8ed20a86, 6 | BRF_SND },           //  6 					/ MB834200
};

STDROMPICKEXT(joyjoy, joyjoy, neogeo)
STD_ROM_FN(joyjoy)

struct BurnDriver BurnDrvJoyjoy = {
	"joyjoy", NULL, "neogeo", NULL, "1990",
	"Puzzled / Joy Joy Kid (NGM-021)(NGH-021)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, joyjoyRomInfo, joyjoyRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Quiz Daisousa Sen - The Last Count Down (NGM-023)(NGH-023)
/* MVS AND AES VERSION */

static struct BurnRomInfo quizdaisRomDesc[] = {
	{ "023-p1.p1",    0x100000, 0xc488fda3, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200

	{ "023-s1.s1",    0x020000, 0xac31818a, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "023-c1.c1",    0x100000, 0x2999535a, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "023-c2.c2",    0x100000, 0x876a99e6, 3 | BRF_GRA },           //  3 					/ TC538200

	{ "023-m1.m1",    0x020000, 0x2a2105e0, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code 		/ TC531001

	{ "023-v1.v1",    0x100000, 0xa53e5bd3, 5 | BRF_SND },           //  5 Sound data 		/ TC538200
};

STDROMPICKEXT(quizdais, quizdais, neogeo)
STD_ROM_FN(quizdais)

struct BurnDriver BurnDrvQuizdais = {
	"quizdais", NULL, "neogeo", NULL, "1991",
	"Quiz Daisousa Sen - The Last Count Down (NGM-023)(NGH-023)\0", NULL, "SNK", "Neo Geo MVS",
	L"\u30AF\u30A4\u30BA\u5927\u635C\u67FB\u7DDA - the last count down\0Quiz Daisousa Sen - The Last Count Down (NGM-023)(NGH-023)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_QUIZ, 0,
	NULL, quizdaisRomInfo, quizdaisRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Quiz Daisousa Sen - The Last Count Down (Korean release)
/* KOREAN VERSION */

static struct BurnRomInfo quizdaiskRomDesc[] = {
	{ "123-p1.p1",    0x100000, 0xa6f35eae, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200

	{ "123-s1.s1",    0x020000, 0x53de938a, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "123-c1.c1",    0x100000, 0xe579a606, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "123-c2.c2",    0x100000, 0xe182c837, 3 | BRF_GRA },           //  3 					/ TC538200

	{ "123-m1.m1",    0x020000, 0xd67f53f9, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code 		/ TC531001

	{ "123-v1.v1",    0x200000, 0x986f4af9, 5 | BRF_SND },           //  5 Sound data 		/ TC5316200
};

STDROMPICKEXT(quizdaisk, quizdaisk, neogeo)
STD_ROM_FN(quizdaisk)

struct BurnDriver BurnDrvQuizdaisk = {
	"quizdaisk", "quizdais", "neogeo", NULL, "1991",
	"Quiz Daisousa Sen - The Last Count Down (Korean release)\0", NULL, "SNK", "Neo Geo MVS",
	L"\u30AF\u30A4\u30BA\u5927\u635C\u67FB\u7DDA - The Last Count Down\0Quiz Daisousa Sen - The Last Count Down (Korean release)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_QUIZ, 0,
	NULL, quizdaiskRomInfo, quizdaiskRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Last Resort

static struct BurnRomInfo lresortRomDesc[] = {
	{ "024-p1.p1",    0x080000, 0x89c4ab97, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "024-s1.s1",    0x020000, 0x5cef5cc6, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "024-c1.c1",    0x100000, 0x3617c2dc, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "024-c2.c2",    0x100000, 0x3f0a7fd8, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "024-c3.c3",    0x080000, 0xe9f745f8, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "024-c4.c4",    0x080000, 0x7382fefb, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "024-m1.m1",    0x020000, 0xcec19742, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "024-v1.v1",    0x100000, 0xefdfa063, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "024-v2.v2",    0x100000, 0x3c7997c0, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(lresort, lresort, neogeo)
STD_ROM_FN(lresort)

struct BurnDriver BurnDrvLresort = {
	"lresort", NULL, "neogeo", NULL, "1992",
	"Last Resort\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_HORSHOOT, 0,
	NULL, lresortRomInfo, lresortRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Last Resort (prototype)
/* early prototype - all roms were hand labeled with CRCs, dumps verified against them */
/* MVS VERSION */

static struct BurnRomInfo lresortpRomDesc[] = {
	{ "024_p1_5937.podd",  0x080000, 0x8e6008ee, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "024_p2_8d37.peven", 0x080000, 0x6d9ee90f, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "024_s1_22fe.s1",    0x020000, 0x5cef5cc6, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "024_c1_b764.c1",    0x080000, 0x677749ec, 3 | BRF_GRA },           //  3 Sprite data
	{ "024_c2_1951.c2",    0x080000, 0x104d7b59, 3 | BRF_GRA },           //  4
	{ "024_c3_0f63.c3",    0x080000, 0xb0965a74, 3 | BRF_GRA },           //  5
	{ "024_c4_c5b5.c4",    0x080000, 0xdcfc3860, 3 | BRF_GRA },           //  6
	{ "024_c5_dd03.c5",    0x080000, 0x50322397, 3 | BRF_GRA },           //  7
	{ "024_c6_c81a.c6",    0x080000, 0xc3c93894, 3 | BRF_GRA },           //  8
	{ "024_c7_5edc.c7",    0x080000, 0x21faf72b, 3 | BRF_GRA },           //  9
	{ "024_c8_0850.c8",    0x080000, 0xfb57217b, 3 | BRF_GRA },           //  10

	{ "024_m1_fc7a.m1",    0x020000, 0x22122875, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	{ "024_v11_b085.v11",  0x080000, 0x0722da38, 5 | BRF_SND },           //  12 Sound data
	{ "024_v12_d3b7.v12",  0x080000, 0x670ce3ec, 5 | BRF_SND },           //  13
	{ "024_v13_a31e.v13",  0x080000, 0x2e39462b, 5 | BRF_SND },           //  14
	{ "024_v24_2f0f.v24",  0x080000, 0x7944754f, 5 | BRF_SND },           //  15
};

STDROMPICKEXT(lresortp, lresortp, neogeo)
STD_ROM_FN(lresortp)

static void LresortpCallback()
{
	BurnLoadRom(Neo68KROMActive + 0x000000, 0, 2);
	BurnLoadRom(Neo68KROMActive + 0x000001, 1, 2);
}

static INT32 LresortpInit()
{
	NeoCallbackActive->pInitialise = LresortpCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvLresortp = {
	"lresortp", "lresort", "neogeo", NULL, "1992",
	"Last Resort (prototype)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SPRITE32, GBF_SCRFIGHT, 0,
	NULL, lresortpRomInfo, lresortpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	LresortpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Eight Man (NGM-025)(NGH-025)
/* MVS AND AES VERSION */

static struct BurnRomInfo eightmanRomDesc[] = {
	{ "025-p1.p1",    0x080000, 0x43344cb0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC574200

	{ "025-s1.s1",    0x020000, 0xa402202b, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "025-c1.c1",    0x100000, 0x555e16a4, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "025-c2.c2",    0x100000, 0xe1ee51c3, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "025-c3.c3",    0x080000, 0x0923d5b0, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "025-c4.c4",    0x080000, 0xe3eca67b, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "025-m1.m1",    0x020000, 0x9927034c, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC541000

	{ "025-v1.v1",    0x100000, 0x4558558a, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "025-v2.v2",    0x100000, 0xc5e052e9, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(eightman, eightman, neogeo)
STD_ROM_FN(eightman)

struct BurnDriver BurnDrvEightman = {
	"eightman", NULL, "neogeo", NULL, "1991",
	"Eight Man (NGM-025)(NGH-025)\0", NULL, "SNK / Pallas", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, eightmanRomInfo, eightmanRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Legend of Success Joe / Ashitano Joe Densetsu

static struct BurnRomInfo legendosRomDesc[] = {
	{ "029-p1.p1",    0x080000, 0x9d563f19, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "029-s1.s1",    0x020000, 0xbcd502f0, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "029-c1.c1",    0x100000, 0x2f5ab875, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "029-c2.c2",    0x100000, 0x318b2711, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "029-c3.c3",    0x100000, 0x6bc52cb2, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "029-c4.c4",    0x100000, 0x37ef298c, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "029-m1.m1",    0x020000, 0x6f2843f0, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "029-v1.v1",    0x100000, 0x85065452, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
};

STDROMPICKEXT(legendos, legendos, neogeo)
STD_ROM_FN(legendos)

struct BurnDriver BurnDrvLegendos = {
	"legendos", NULL, "neogeo", NULL, "1991",
	"Legend of Success Joe / Ashitano Joe Densetsu\0", NULL, "SNK / WAVE", "Neo Geo MVS",
	L"Legend of Success Joe / Ashitano Joe Densetsu\0\u3042\u3057\u305F\u306E\u30B8\u30E7\u30FC\u4F1D\u8AAC\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, legendosRomInfo, legendosRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// 2020 Super Baseball (set 1)

static struct BurnRomInfo bb2020RomDesc[] = {
	{ "030-p1.p1",    0x080000, 0xd396c9cb, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	/* also found AES set with p1 on TC534200 on board NEO-AEG PROG42G; chip label is 030-p1
	and m1 on TC531001 on board NEO-AEG CHA42G; chip label is 030-M1. Other chip labels are the same */

	{ "030-s1.s1",    0x020000, 0x7015b8fc, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "030-c1.c1",    0x100000, 0x4f5e19bd, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "030-c2.c2",    0x100000, 0xd6314bf0, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "030-c3.c3",    0x100000, 0x47fddfee, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "030-c4.c4",    0x100000, 0x780d1c4e, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "030-m1.m1",    0x020000, 0x4cf466ec, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC54H1000

	{ "030-v1.v1",    0x100000, 0xd4ca364e, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "030-v2.v2",    0x100000, 0x54994455, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(bb2020, bb2020, neogeo)
STD_ROM_FN(bb2020)

struct BurnDriver BurnDrvbb2020 = {
	"2020bb", NULL, "neogeo", NULL, "1991",
	"2020 Super Baseball (set 1)\0", NULL, "SNK / Pallas", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, bb2020RomInfo, bb2020RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// 2020 Super Baseball (set 2)
/* MVS AND AES VERSION */

static struct BurnRomInfo bba2020RomDesc[] = {
	{ "030-epr.p1",   0x080000, 0xc59be3dd, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC754200
	/* P1 on eprom, correct chip label unknown */

	{ "030-s1.s1",    0x020000, 0x7015b8fc, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "030-c1.c1",    0x100000, 0x4f5e19bd, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "030-c2.c2",    0x100000, 0xd6314bf0, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "030-c3.c3",    0x100000, 0x47fddfee, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "030-c4.c4",    0x100000, 0x780d1c4e, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "030-m1.m1",    0x020000, 0x4cf466ec, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC54H1000

	{ "030-v1.v1",    0x100000, 0xd4ca364e, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "030-v2.v2",    0x100000, 0x54994455, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(bba2020, bba2020, neogeo)
STD_ROM_FN(bba2020)

struct BurnDriver BurnDrvbba2020 = {
	"2020bba", "2020bb", "neogeo", NULL, "1991",
	"2020 Super Baseball (set 2)\0", NULL, "SNK / Pallas", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, bba2020RomInfo, bba2020RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// 2020 Super Baseball (set 3)

static struct BurnRomInfo bbh2020RomDesc[] = {
	{ "030-pg1.p1",   0x080000, 0x12d048d7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	/* Chip label p1h does not exist, renamed temporarly to pg1, marked BAD_DUMP. This needs to be verified. */

	{ "030-s1.s1",    0x020000, 0x7015b8fc, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "030-c1.c1",    0x100000, 0x4f5e19bd, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "030-c2.c2",    0x100000, 0xd6314bf0, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "030-c3.c3",    0x100000, 0x47fddfee, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "030-c4.c4",    0x100000, 0x780d1c4e, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "030-m1.m1",    0x020000, 0x4cf466ec, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC54H1000

	{ "030-v1.v1",    0x100000, 0xd4ca364e, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "030-v2.v2",    0x100000, 0x54994455, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(bbh2020, bbh2020, neogeo)
STD_ROM_FN(bbh2020)

struct BurnDriver BurnDrvbbh2020 = {
	"2020bbh", "2020bb", "neogeo", NULL, "1991",
	"2020 Super Baseball (set 3)\0", NULL, "SNK / Pallas", "Neo Geo AES",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, bbh2020RomInfo, bbh2020RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Soccer Brawl (NGM-031)
/* MVS VERSION */

static struct BurnRomInfo socbrawlRomDesc[] = {
	{ "031-pg1.p1",   0x080000, 0x17f034a7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "031-s1.s1",    0x020000, 0x4c117174, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "031-c1.c1",    0x100000, 0xbd0a4eb8, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "031-c2.c2",    0x100000, 0xefde5382, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "031-c3.c3",    0x080000, 0x580f7f33, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "031-c4.c4",    0x080000, 0xed297de8, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "031-m1.m1",    0x020000, 0xcb37427c, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "031-v1.v1",    0x100000, 0xcc78497e, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "031-v2.v2",    0x100000, 0xdda043c6, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(socbrawl, socbrawl, neogeo)
STD_ROM_FN(socbrawl)

struct BurnDriver BurnDrvSocbrawl = {
	"socbrawl", NULL, "neogeo", NULL, "1991",
	"Soccer Brawl (NGM-031)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSFOOTBALL, 0,
	NULL, socbrawlRomInfo, socbrawlRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Soccer Brawl (NGH-031)
/* AES VERSION */

static struct BurnRomInfo socbrawlhRomDesc[] = {
	{ "031-p1.p1",    0x080000, 0xa2801c24, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "031-s1.s1",    0x020000, 0x4c117174, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "031-c1.c1",    0x100000, 0xbd0a4eb8, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "031-c2.c2",    0x100000, 0xefde5382, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "031-c3.c3",    0x080000, 0x580f7f33, 3 | BRF_GRA },           //  4 					/ TC534200
	{ "031-c4.c4",    0x080000, 0xed297de8, 3 | BRF_GRA },           //  5 					/ TC534200

	{ "031-m1.m1",    0x020000, 0xcb37427c, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "031-v1.v1",    0x100000, 0xcc78497e, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "031-v2.v2",    0x100000, 0xdda043c6, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(socbrawlh, socbrawlh, neogeo)
STD_ROM_FN(socbrawlh)

struct BurnDriver BurnDrvSocbrawlh = {
	"socbrawlh", "socbrawl", "neogeo", NULL, "1991",
	"Soccer Brawl (NGH-031)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSFOOTBALL, 0,
	NULL, socbrawlhRomInfo, socbrawlhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Robo Army (set 1)

static struct BurnRomInfo roboarmyRomDesc[] = {
	{ "032-p1.p1",    0x080000, 0xcd11cbd4, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "032-s1.s1",    0x020000, 0xac0daa1b, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "032-c1.c1",    0x100000, 0x97984c6c, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "032-c2.c2",    0x100000, 0x65773122, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "032-c3.c3",    0x080000, 0x40adfccd, 3 | BRF_GRA },           //  4 					/ TC534200
	{ "032-c4.c4",    0x080000, 0x462571de, 3 | BRF_GRA },           //  5 					/ TC534200

	{ "032-m1.m1",    0x020000, 0x35ec952d, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "032-v1.v1",    0x100000, 0x63791533, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "032-v2.v2",    0x100000, 0xeb95de70, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(roboarmy, roboarmy, neogeo)
STD_ROM_FN(roboarmy)

struct BurnDriver BurnDrvRoboarmy = {
	"roboarmy", NULL, "neogeo", NULL, "1991",
	"Robo Army (set 1)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, roboarmyRomInfo, roboarmyRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Robo Army (NGM-032) (NGH-032)
/* MVS AND AES VERSION*/

static struct BurnRomInfo roboarmyaRomDesc[] = {
	/* Found on legitimate Robo Army MVS and AES Carts. Debug code button which check control a sound test has been patched out
	because in a multislot situation that code could have been enabled 	that would stop roboarmy from working, 	sending it into an infinite loop ;
	correct chip label unknown */
	{ "032-epr.p1",   0x080000, 0x27c773cb, 1 | BRF_ESS | BRF_PRG }, //  0 68K code / D27C4000

	{ "032-s1.s1",    0x020000, 0xac0daa1b, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "032-c1.c1",    0x100000, 0x97984c6c, 3 | BRF_GRA },           //  2 Sprite data / TC538200
	{ "032-c2.c2",    0x100000, 0x65773122, 3 | BRF_GRA },           //  3 / TC538200
	{ "032-c3.c3",    0x080000, 0x40adfccd, 3 | BRF_GRA },           //  4 / TC534200
	{ "032-c4.c4",    0x080000, 0x462571de, 3 | BRF_GRA },           //  5 / TC534200

	{ "032-m1.m1",    0x020000, 0x35ec952d, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code / TC531001

	{ "032-v1.v1",    0x100000, 0x63791533, 5 | BRF_SND },           //  7 Sound data / TC538200
	{ "032-v2.v2",    0x100000, 0xeb95de70, 5 | BRF_SND },           //  8 / TC538200
};

STDROMPICKEXT(roboarmya, roboarmya, neogeo)
STD_ROM_FN(roboarmya)

struct BurnDriver BurnDrvRoboarmya = {
	"roboarmya", "roboarmy", "neogeo", NULL, "1991",
	"Robo Army (set 2)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, roboarmyaRomInfo, roboarmyaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Fatal Fury - King of Fighters / Garou Densetsu - shukumei no tatakai (NGM-033)(NGH-033)
/* MVS AND AES VERSION */

static struct BurnRomInfo fatfury1RomDesc[] = {
	{ "033-p1.p1",    0x080000, 0x47ebdc2f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "033-p2.p2",    0x020000, 0xc473af1c, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC531024

	{ "033-s1.s1",    0x020000, 0x3c3bdf8c, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "033-c1.c1",    0x100000, 0x74317e54, 3 | BRF_GRA },           //  3 Sprite data 		/ TC538200
	{ "033-c2.c2",    0x100000, 0x5bb952f3, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "033-c3.c3",    0x100000, 0x9b714a7c, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "033-c4.c4",    0x100000, 0x9397476a, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "033-m1.m1",    0x020000, 0x5be10ffd, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "033-v1.v1",    0x100000, 0x212fd20d, 5 | BRF_SND },           //  8 Sound data 		/ TC538200
	{ "033-v2.v2",    0x100000, 0xfa2ae47f, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(fatfury1, fatfury1, neogeo)
STD_ROM_FN(fatfury1)

struct BurnDriver BurnDrvFatFury1 = {
	"fatfury1", NULL, "neogeo", NULL, "1992",
	"Fatal Fury - King of Fighters / Garou Densetsu - shukumei no tatakai (NGM-033)(NGH-033)\0", NULL, "SNK", "Neo Geo MVS",
	L"Fatal Fury - King of Fighters\0\u9913\u72FC\u4F1D\u8AAC - \u5BBF\u547D\u306E\u95D8\u3044 (NGM-033)(NGH-033)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, fatfury1RomInfo, fatfury1RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Football Frenzy (NGM-034)(NGH-034)
/* MVS AND AES VERSION */

static struct BurnRomInfo fbfrenzyRomDesc[] = {
	{ "034-p1.p1",    0x080000, 0xcdef6b19, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "034-s1.s1",    0x020000, 0x8472ed44, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "034-c1.c1",    0x100000, 0x91c56e78, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "034-c2.c2",    0x100000, 0x9743ea2f, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "034-c3.c3",    0x080000, 0xe5aa65f5, 3 | BRF_GRA },           //  4 					/ TC534200
	{ "034-c4.c4",    0x080000, 0x0eb138cc, 3 | BRF_GRA },           //  5 					/ TC534200

	{ "034-m1.m1",    0x020000, 0xf41b16b8, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "034-v1.v1",    0x100000, 0x50c9d0dd, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "034-v2.v2",    0x100000, 0x5aa15686, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(fbfrenzy, fbfrenzy, neogeo)
STD_ROM_FN(fbfrenzy)

struct BurnDriver BurnDrvFbfrenzy = {
	"fbfrenzy", NULL, "neogeo", NULL, "1992",
	"Football Frenzy (NGM-034)(NGH-034)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, fbfrenzyRomInfo, fbfrenzyRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// King of the Monsters 2 - The Next Thing(NGM-039)(NGH-039)
/* MVS AND AES VERSION */

static struct BurnRomInfo kotm2RomDesc[] = {
	{ "039-p1.p1",    0x080000, 0xb372d54c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "039-p2.p2",    0x080000, 0x28661afe, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC534200

	{ "039-s1.s1",    0x020000, 0x63ee053a, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "039-c1.c1",    0x200000, 0x6d1c4aa9, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5316200
	{ "039-c2.c2",    0x200000, 0xf7b75337, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "039-c3.c3",    0x080000, 0xbfc4f0b2, 3 | BRF_GRA },           //  5 					/ TC534200
	{ "039-c4.c4",    0x080000, 0x81c9c250, 3 | BRF_GRA },           //  6 					/ TC534200

	{ "039-m1.m1",    0x020000, 0x0c5b2ad5, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "039-v2.v2",    0x200000, 0x86d34b25, 5 | BRF_SND },           //  8 Sound data 		/ TC5316200
	{ "039-v4.v4",    0x100000, 0x8fa62a0b, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(kotm2, kotm2, neogeo)
STD_ROM_FN(kotm2)

struct BurnDriver BurnDrvKotm2 = {
	"kotm2", NULL, "neogeo", NULL, "1992",
	"King of the Monsters 2 - The Next Thing (NGM-039)(NGH-039)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_VSFIGHT, 0,
	NULL, kotm2RomInfo, kotm2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// King of the Monsters 2 - The Next Thing (older)
/* MVS VERSION */

static struct BurnRomInfo kotm2aRomDesc[] = {
	{ "039_p1.p1",    0x080000, 0x8d186638, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "039-p2.p2",    0x080000, 0x28661afe, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC534200

	{ "039-s1.s1",    0x020000, 0x63ee053a, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "039-c1.c1",    0x200000, 0x6d1c4aa9, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5316200
	{ "039-c2.c2",    0x200000, 0xf7b75337, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "039-c3.c3",    0x080000, 0xbfc4f0b2, 3 | BRF_GRA },           //  5 					/ TC534200
	{ "039-c4.c4",    0x080000, 0x81c9c250, 3 | BRF_GRA },           //  6 					/ TC534200

	{ "039-m1.m1",    0x020000, 0x0c5b2ad5, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "039-v2.v2",    0x200000, 0x86d34b25, 5 | BRF_SND },           //  8 Sound data 		/ TC5316200
	{ "039-v4.v4",    0x100000, 0x8fa62a0b, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(kotm2a, kotm2a, neogeo)
STD_ROM_FN(kotm2a)

struct BurnDriver BurnDrvKotm2a = {
	"kotm2a", "kotm2", "neogeo", NULL, "1992",
	"King of the Monsters 2 - The Next Thing (older)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_VSFIGHT, 0,
	NULL, kotm2aRomInfo, kotm2aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// King of the Monsters 2 - The Next Thing (prototype)
/* fairly late prototype release, only the code differs from the main set */

static struct BurnRomInfo kotm2pRomDesc[] = {
	{ "proto_039-p1.p1", 0x080000, 0x3c1d17e7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "proto_039-p2.p2", 0x080000, 0xbc9691f0, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "039-s1.s1",       0x020000, 0x63ee053a, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	// same data as main set, but prototype board layout
	{ "proto_039-c1.c1", 0x100000, 0x7192a787, 3 | BRF_GRA },           //  3 Sprite data
	{ "proto_039-c2.c2", 0x100000, 0x7157eca1, 3 | BRF_GRA },           //  4
	{ "proto_039-c3.c3", 0x100000, 0x11d75727, 3 | BRF_GRA },           //  5
	{ "proto_039-c4.c4", 0x100000, 0x7ad48b28, 3 | BRF_GRA },           //  6
	{ "proto_039-c5.c5", 0x080000, 0x5bdaf9ca, 3 | BRF_GRA },           //  7
	{ "proto_039-c6.c6", 0x080000, 0x21d4be8c, 3 | BRF_GRA },           //  8
	{ "proto_039-c7.c7", 0x080000, 0xda55fd00, 3 | BRF_GRA },           //  9
	{ "proto_039-c8.c8", 0x080000, 0x592e9267, 3 | BRF_GRA },           //  10

	{ "039-m1.m1",       0x020000, 0x0c5b2ad5, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code 		/ TC531001

	// same data as main set, but prototype board layout
	{ "proto_039-v1.v1", 0x080000, 0xdd3566f1, 5 | BRF_SND },           //  12 Sound data
	{ "proto_039-v2.v2", 0x080000, 0x57f60274, 5 | BRF_SND },           //  13
	{ "proto_039-v3.v3", 0x080000, 0x0f008a07, 5 | BRF_SND },           //  14
	{ "proto_039-v4.v4", 0x080000, 0x1943d0fe, 5 | BRF_SND },           //  15
	{ "proto_039-v5.v5", 0x080000, 0x13be045b, 5 | BRF_SND },           //  16
	{ "proto_039-v6.v6", 0x080000, 0xd1dd3fd6, 5 | BRF_SND },           //  17
};

STDROMPICKEXT(kotm2p, kotm2p, neogeo)
STD_ROM_FN(kotm2p)

static void Kotm2pCallback()
{
	BurnLoadRom(Neo68KROMActive + 0x000000, 0, 2);
	BurnLoadRom(Neo68KROMActive + 0x000001, 1, 2);
}

static INT32 Kotm2pInit()
{
	NeoCallbackActive->pInitialise = Kotm2pCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvKotm2p = {
	"kotm2p", "kotm2", "neogeo", NULL, "1992",
	"King of the Monsters 2 - The Next Thing (prototype)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SPRITE32, GBF_VSFIGHT, 0,
	NULL, kotm2pRomInfo, kotm2pRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	Kotm2pInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Sengoku 2 / Sengoku Denshou 2

static struct BurnRomInfo sengoku2RomDesc[] = {
	/* also found AES set with PROG board NEO-AEG PROG16 and CHA board NEO-AEG CHA42G-1;
	same chip labels */
	{ "040-p1.p1",    0x100000, 0x6dde02c2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200

	{ "040-s1.s1",    0x020000, 0xcd9802a3, 2 | BRF_GRA },           //  1 Text layer tiles / TC531001

	{ "040-c1.c1",    0x200000, 0xfaa8ea99, 3 | BRF_GRA },           //  2 Sprite data 		/ TC5316200
	{ "040-c2.c2",    0x200000, 0x87d0ec65, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "040-c3.c3",    0x080000, 0x24b5ba80, 3 | BRF_GRA },           //  4 					/ TC534200
	{ "040-c4.c4",    0x080000, 0x1c9e9930, 3 | BRF_GRA },           //  5 					/ TC534200

	{ "040-m1.m1",    0x020000, 0xd4de4bca, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "040-v1.v1",    0x200000, 0x71cb4b5d, 5 | BRF_SND },           //  7 Sound data 		/ TC5316200
	// AES has different chip location, data is the same: 040-v1.v2  						/ TC5316200
	{ "040-v2.v2",    0x100000, 0xc5cece01, 5 | BRF_SND },           //  8 					/ TC538200
	// AES has different chip location, data is the same: 040-v2.v4  						/ TC538200
};

STDROMPICKEXT(sengoku2, sengoku2, neogeo)
STD_ROM_FN(sengoku2)

struct BurnDriver BurnDrvSengoku2 = {
	"sengoku2", NULL, "neogeo", NULL, "1993",
	"Sengoku 2 / Sengoku Denshou 2\0", NULL, "SNK", "Neo Geo MVS",
	L"Sengoku 2\0\u6226\u56FD\u4F1D\u627F\uFF12\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_SCRFIGHT, 0,
	NULL, sengoku2RomInfo, sengoku2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Baseball Stars 2

static struct BurnRomInfo bstars2RomDesc[] = {
	/* also found AES set with PROG board NEO-AEG PROG42G-1 and CHA board NEO-AEG CHA42G-1;
	same chip labels  */
	{ "041-p1.p1",    0x080000, 0x523567fd, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "041-s1.s1",    0x020000, 0x015c5c94, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "041-c1.c1",    0x100000, 0xb39a12e1, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "041-c2.c2",    0x100000, 0x766cfc2f, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "041-c3.c3",    0x100000, 0xfb31339d, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "041-c4.c4",    0x100000, 0x70457a0c, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "041-m1.m1",    0x020000, 0x15c177a6, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "041-v1.v1",    0x100000, 0xcb1da093, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "041-v2.v2",    0x100000, 0x1c954a9d, 5 | BRF_SND },           //  8 					/ TC538200
	{ "041-v3.v3",    0x080000, 0xafaa0180, 5 | BRF_SND },           //  9 					/ TC534200
};

STDROMPICKEXT(bstars2, bstars2, neogeo)
STD_ROM_FN(bstars2)

struct BurnDriver BurnDrvBstars2 = {
	"bstars2", NULL, "neogeo", NULL, "1992",
	"Baseball Stars 2\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, bstars2RomInfo, bstars2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Quiz Meitantei Neo & Geo - Quiz Daisousa Sen part 2 (NGM-042)(NGH-042)
/* MVS AND AES VERSION */

static struct BurnRomInfo quizdai2RomDesc[] = {
	{ "042-p1.p1",    0x100000, 0xed719dcf, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200

	{ "042-s1.s1",    0x020000, 0x164fd6e6, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "042-c1.c1",    0x100000, 0xcb5809a1, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "042-c2.c2",    0x100000, 0x1436dfeb, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "042-c3.c3",    0x080000, 0xbcd4a518, 3 | BRF_GRA },           //  4 					/ TC534200
	{ "042-c4.c4",    0x080000, 0xd602219b, 3 | BRF_GRA },           //  5 					/ TC534200

	{ "042-m1.m1",    0x020000, 0xbb19995d, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "042-v1.v1",    0x100000, 0xaf7f8247, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "042-v2.v2",    0x100000, 0xc6474b59, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(quizdai2, quizdai2, neogeo)
STD_ROM_FN(quizdai2)

struct BurnDriver BurnDrvQuizdai2 = {
	"quizdai2", NULL, "neogeo", NULL, "1992",
	"Quiz Meitantei Neo & Geo - Quiz Daisousa Sen part 2 (NGM-042)(NGH-042)\0", NULL, "SNK", "Neo Geo MVS",
	L"\u30AF\u30A4\u30BA\u8FF7\u63A2\u5075\u30CD\u30AA\uFF06\u30B8\u30AA - Quiz Daisousa Sen part 2 (NGM-042)(NGH-042)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_QUIZ, 0,
	NULL, quizdai2RomInfo, quizdai2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// 3 Count Bout / Fire Suplex (NGM-043)(NGH-043)
/* MVS AND AES VERSION */

static struct BurnRomInfo countb3RomDesc[] = {
	{ "043-p1.p1",    0x100000, 0xffbdd928, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200
	/* The original p1 is 8mbit; also found MVS sets with p1 / p2 4mbit on eprom.
	{ "043-epr.ep1",  0x080000, 0xeb2714c4, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ M27C4002
	{ "043-epr.ep2",  0x080000, 0x5e764567, 1 | BRF_ESS | BRF_PRG }, //  1 					/ M27C4002 */

	{ "043-s1.s1",    0x020000, 0xc362d484, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "043-c1.c1",    0x200000, 0xbad2d67f, 3 | BRF_GRA },           //  2 Sprite data 		/ TC5316200
	{ "043-c2.c2",    0x200000, 0xa7fbda95, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "043-c3.c3",    0x200000, 0xf00be011, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "043-c4.c4",    0x200000, 0x1887e5c0, 3 | BRF_GRA },           //  5 					/ TC5316200

	{ "043-m1.m1",    0x020000, 0x7eab59cb, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "043-v1.v1",    0x200000, 0x63688ce8, 5 | BRF_SND },           //  7 Sound data 		/ TC5316200
	{ "043-v2.v2",    0x200000, 0xc69a827b, 5 | BRF_SND },           //  8 					/ TC5316200
	/* also found AES set with p1 on TC538200 on PROG board NEO-AEG PROG16; chip label is 043-P1
	and v1 / v2 on different chip location on CHA board NEO-AEG CHA42G-1;
	{ "043-v1.v2",    0x200000, 0x63688ce8, 5 | BRF_SND },           //  7 Sound data 		/ TC5316200
	{ "043-v2.v4",    0x200000, 0xc69a827b, 5 | BRF_SND },           //  8 					/ TC5316200 */
};

STDROMPICKEXT(countb3, countb3, neogeo)
STD_ROM_FN(countb3)

struct BurnDriver BurnDrv3Countb = {
	"3countb", NULL, "neogeo", NULL, "1993",
	"3 Count Bout / Fire Suplex (NGM-043)(NGH-043)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_VSFIGHT, 0,
	NULL, countb3RomInfo, countb3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Art of Fighting / Ryuuko no Ken (NGM-044)(NGH-044)
/* MVS AND AES VERSION */

static struct BurnRomInfo aofRomDesc[] = {
	{ "044-p1.p1",    0x080000, 0xca9f7a6d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "044-s1.s1",    0x020000, 0x89903f39, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "044-c1.c1",    0x200000, 0xddab98a7, 3 | BRF_GRA },           //  2 Sprite data 		/ TC5316200
	{ "044-c2.c2",    0x200000, 0xd8ccd575, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "044-c3.c3",    0x200000, 0x403e898a, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "044-c4.c4",    0x200000, 0x6235fbaa, 3 | BRF_GRA },           //  5 					/ TC5316200

	{ "044-m1.m1",    0x020000, 0x0987e4bb, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "044-v2.v2",    0x200000, 0x3ec632ea, 5 | BRF_SND },           //  7 Sound data 		/ TC5316200
	{ "044-v4.v4",    0x200000, 0x4b0f8e23, 5 | BRF_SND },           //  8 					/ TC5316200
	/* also found MVS set with EP1 on M27C4002 eprom on PROG board NEO-MVS PROG4096; correct chip label unknown
	and v1 / v2 with different chip location on CHA board NEO-MVS CHA42G-1.
	{ "044-v2.v1",    0x200000, 0x3ec632ea, 5 | BRF_SND },           //  7 Sound data 		/ TC5316200
	{ "044-v4.v2",    0x200000, 0x4b0f8e23, 5 | BRF_SND },           //  8 					/ TC5316200 */
};

STDROMPICKEXT(aof, aof, neogeo)
STD_ROM_FN(aof)

struct BurnDriver BurnDrvAof = {
	"aof", NULL, "neogeo", NULL, "1992",
	"Art of Fighting / Ryuuko no Ken (NGM-044)(NGH-044)\0", NULL, "SNK", "Neo Geo MVS",
	L"Art of Fighting\0\u9F8D\u864E\u306E\u62F3 (NGM-044)(NGH-044)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_VSFIGHT, 0,
	NULL, aofRomInfo, aofRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Samurai Shodown / Samurai Spirits (NGM-045)
/* MVS VERSION */

static struct BurnRomInfo samshoRomDesc[] = {
	{ "045-p1.p1",    0x100000, 0xdfe51bf0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200
	{ "045-pg2.sp2",  0x100000, 0x46745b94, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC538200
	/* also found MVS set with p1 / p2 on board NEO-MVS PROGGSC; same rom data as samshoh
	{ "045-p1.p1",    0x100000, 0xdfe51bf0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200
	{ "045-p2.sp2",   0x080000, 0x38ee9ba9, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC534200 */

	{ "045-s1.s1",    0x020000, 0x9142a4d3, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	/* It also exists MVS set with different 6xC layout on board NEO-MVS CHA42G-3;
	naming scheme is 045-Cx.
	{ "045-c1.c1",    0x200000, 0x2e5873a4, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5316200
	{ "045-c2.c2",    0x200000, 0x04febb10, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "045-c3.c3",    0x200000, 0xf3dabd1e, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "045-c4.c4",    0x200000, 0x935c62f0, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "045-c5.c5",    0x080000, 0xa2bb8284, 3 | BRF_GRA },           //  7 					/ TC534200
	{ "045-c6.c6",    0x080000, 0x4fa71252, 3 | BRF_GRA },           //  8 					/ TC534200 */
	{ "045-c1.c1",    0x200000, 0x2e5873a4, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5316200
	{ "045-c2.c2",    0x200000, 0x04febb10, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "045-c3.c3",    0x200000, 0xf3dabd1e, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "045-c4.c4",    0x200000, 0x935c62f0, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "045-c51.c5",   0x100000, 0x81932894, 3 | BRF_GRA },           //  7 					/ TC538200
	{ "045-c61.c6",   0x100000, 0xbe30612e, 3 | BRF_GRA },           //  8 					/ TC538200

	{ "045-m1.m1",    0x020000, 0x95170640, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code 		/ TC531001

	{ "045-v1.v1",    0x200000, 0x37f78a9b, 5 | BRF_SND },           // 10 Sound data 		/ TC5316200
	{ "045-v2.v2",    0x200000, 0x568b20cf, 5 | BRF_SND },           // 11 					/ TC5316200
};

STDROMPICKEXT(samsho, samsho, neogeo)
STD_ROM_FN(samsho)

struct BurnDriver BurnDrvSamsho = {
	"samsho", NULL, "neogeo", NULL, "1993",
	"Samurai Shodown / Samurai Spirits (NGM-045)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samshoRomInfo, samshoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Samurai Shodown / Samurai Spirits (NGH-045)
/* AES VERSION */

static struct BurnRomInfo samshohRomDesc[] = {
	{ "045-p1.p1",    0x100000, 0xdfe51bf0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200
	{ "045-p2.sp2",   0x080000, 0x38ee9ba9, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC534200

	{ "045-s1.s1",    0x020000, 0x9142a4d3, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	/* It also exists AES set with different 6xC layout on board NEO-AEG CHA42G-4;
	chip labels are 045-C1; 045-C2; 045-C3; 045-C4; 045-C5 and 045-C61
	{ "045-c1.c1",    0x200000, 0x2e5873a4, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5316200
	{ "045-c2.c2",    0x200000, 0x04febb10, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "045-c3.c3",    0x200000, 0xf3dabd1e, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "045-c4.c4",    0x200000, 0x935c62f0, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "045-c5.c5",    0x080000, 0xa2bb8284, 3 | BRF_GRA },           //  7 					/ TC534200
	{ "045-c61.c6",   0x100000, 0xbe30612e, 3 | BRF_GRA },           //  8 					/ TC538200 */
	{ "045-c1.c1",    0x200000, 0x2e5873a4, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5316200
	{ "045-c2.c2",    0x200000, 0x04febb10, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "045-c3.c3",    0x200000, 0xf3dabd1e, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "045-c4.c4",    0x200000, 0x935c62f0, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "045-c5.c5",    0x080000, 0xa2bb8284, 3 | BRF_GRA },           //  7 					/ TC534200
	{ "045-c6.c6",    0x080000, 0x4fa71252, 3 | BRF_GRA },           //  8 					/ TC534200

	{ "045-m1.m1",    0x020000, 0x95170640, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code 		/ TC531001

	{ "045-v1.v1",    0x200000, 0x37f78a9b, 5 | BRF_SND },           // 10 Sound data 		/ TC5316200
	{ "045-v2.v2",    0x200000, 0x568b20cf, 5 | BRF_SND },           // 11 					/ TC5316200
};

STDROMPICKEXT(samshoh, samshoh, neogeo)
STD_ROM_FN(samshoh)

struct BurnDriver BurnDrvSamShoh = {
	"samshoh", "samsho", "neogeo", NULL, "1993",
	"Samurai Shodown / Samurai Spirits (NGH-045)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samshohRomInfo, samshohRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Top Hunter - Roddy & Cathy (NGM-046)
/* MVS VERSION */

static struct BurnRomInfo tophuntrRomDesc[] = {
	{ "046-p1.p1",    0x100000, 0x69fa9e29, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC538200
	{ "046-p2.sp2",   0x100000, 0xf182cb3e, 1 | BRF_ESS | BRF_PRG }, //  1 					/ mask rom TC538200

	{ "046-s1.s1",    0x020000, 0x14b01d7b, 2 | BRF_GRA },           //  2 Text layer tiles / mask rom TC531000

	{ "046-c1.c1",    0x100000, 0xfa720a4a, 3 | BRF_GRA },           //  3 Sprite data 		/ mask rom TC538200
	{ "046-c2.c2",    0x100000, 0xc900c205, 3 | BRF_GRA },           //  4 					/ mask rom TC538200
	{ "046-c3.c3",    0x100000, 0x880e3c25, 3 | BRF_GRA },           //  5 					/ mask rom TC538200
	{ "046-c4.c4",    0x100000, 0x7a2248aa, 3 | BRF_GRA },           //  6 					/ mask rom TC538200
	{ "046-c5.c5",    0x100000, 0x4b735e45, 3 | BRF_GRA },           //  7 					/ mask rom TC538200
	{ "046-c6.c6",    0x100000, 0x273171df, 3 | BRF_GRA },           //  8 					/ mask rom TC538200
	{ "046-c7.c7",    0x100000, 0x12829c4c, 3 | BRF_GRA },           //  9 					/ mask rom TC538200
	{ "046-c8.c8",    0x100000, 0xc944e03d, 3 | BRF_GRA },           // 10 					/ mask rom TC538200

	{ "046-m1.m1",    0x020000, 0x3f84bb9f, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code 		/ mask rom TC531001

	{ "046-v1.v1",    0x100000, 0xc1f9c2db, 5 | BRF_SND },           // 12 Sound data 		/ mask rom TC538200
	{ "046-v2.v2",    0x100000, 0x56254a64, 5 | BRF_SND },           // 13 					/ mask rom TC538200
	{ "046-v3.v3",    0x100000, 0x58113fb1, 5 | BRF_SND },           // 14 					/ mask rom TC538200
	{ "046-v4.v4",    0x100000, 0x4f54c187, 5 | BRF_SND },           // 15 					/ mask rom TC538200
};

STDROMPICKEXT(tophuntr, tophuntr, neogeo)
STD_ROM_FN(tophuntr)

struct BurnDriver BurnDrvtophntr = {
	"tophuntr", NULL, "neogeo", NULL, "1994",
	"Top Hunter - Roddy & Cathy (NGM-046)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, tophuntrRomInfo, tophuntrRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Top Hunter - Roddy & Cathy (NGH-046)
/* AES VERSION */

static struct BurnRomInfo tophuntrhRomDesc[] = {
	{ "046-pg1.p1",   0x100000, 0x771e39bc, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC538200
	{ "046-p2.sp2",   0x100000, 0xf182cb3e, 1 | BRF_ESS | BRF_PRG }, //  1 					/ mask rom TC538200

	{ "046-sg1.s1",   0x020000, 0x92e9ac8c, 2 | BRF_GRA },           //  2 Text layer tiles / mask rom TC531000

	{ "046-c1.c1",    0x100000, 0xfa720a4a, 3 | BRF_GRA },           //  3 Sprite data 		/ mask rom TC538200
	{ "046-c2.c2",    0x100000, 0xc900c205, 3 | BRF_GRA },           //  4 					/ mask rom TC538200
	{ "046-c3.c3",    0x100000, 0x880e3c25, 3 | BRF_GRA },           //  5 					/ mask rom TC538200
	{ "046-c4.c4",    0x100000, 0x7a2248aa, 3 | BRF_GRA },           //  6 					/ mask rom TC538200
	{ "046-c5.c5",    0x100000, 0x4b735e45, 3 | BRF_GRA },           //  7 					/ mask rom TC538200
	{ "046-c6.c6",    0x100000, 0x273171df, 3 | BRF_GRA },           //  8 					/ mask rom TC538200
	{ "046-c7.c7",    0x100000, 0x12829c4c, 3 | BRF_GRA },           //  9 					/ mask rom TC538200
	{ "046-c8.c8",    0x100000, 0xc944e03d, 3 | BRF_GRA },           // 10 					/ mask rom TC538200

	{ "046-m1.m1",    0x020000, 0x3f84bb9f, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code 		/ mask rom TC531001

	{ "046-v1.v1",    0x100000, 0xc1f9c2db, 5 | BRF_SND },           // 12 Sound data 		/ mask rom TC538200
	{ "046-v2.v2",    0x100000, 0x56254a64, 5 | BRF_SND },           // 13 					/ mask rom TC538200
	{ "046-v3.v3",    0x100000, 0x58113fb1, 5 | BRF_SND },           // 14 					/ mask rom TC538200
	{ "046-v4.v4",    0x100000, 0x4f54c187, 5 | BRF_SND },           // 15 					/ mask rom TC538200
};

STDROMPICKEXT(tophuntrh, tophuntrh, neogeo)
STD_ROM_FN(tophuntrh)

struct BurnDriver BurnDrvtophntrh = {
	"tophuntrh", "tophuntr", "neogeo", NULL, "1994",
	"Top Hunter - Roddy & Cathy (NGH-046)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, tophuntrhRomInfo, tophuntrhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Fatal Fury 2 / Garou Densetsu 2 - arata-naru tatakai (NGM-047)(NGH-047)
/* MVS AND AES VERSION */

static struct BurnRomInfo fatfury2RomDesc[] = {
	{ "047-p1.p1",    0x100000, 0xecfdbb69, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200
	/* The original p1 is 8mbit; also found MVS set with p1 / p2 4mbit on eprom
	on PROG board NEO-MVS PROG-G2 (SNK-9201)
	{ "047-epr.ep1",  0x080000, 0xbe40ea92, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ M27C4002
	{ "047-epr.ep2",  0x080000, 0x2a9beac5, 1 | BRF_ESS | BRF_PRG }, //  1 					/ M27C4002 */
	/* also found AES set with P1 / P3 on emprom on PROG board NEO-AEG PROG-G2 (SNK-9201);
	{ "047-epr.p1",   0x080000, 0x00000000, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC574200
	{ "047-epr.p3",   0x080000, 0x00000000, 1 | BRF_ESS | BRF_PRG }, //  1 					/ M27C4002 */

	{ "047-s1.s1",    0x020000, 0xd7dbbf39, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "047-c1.c1",    0x200000, 0xf72a939e, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5316200
	{ "047-c2.c2",    0x200000, 0x05119a0d, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "047-c3.c3",    0x200000, 0x01e00738, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "047-c4.c4",    0x200000, 0x9fe27432, 3 | BRF_GRA },           //  6 					/ TC5316200

	{ "047-m1.m1",    0x020000, 0x820b0ba7, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "047-v1.v1",    0x200000, 0xd9d00784, 5 | BRF_SND },           //  8 Sound data 		/ TC5316200
	{ "047-v2.v2",    0x200000, 0x2c9a4b33, 5 | BRF_SND },           //  9 					/ TC5316200
};

STDROMPICKEXT(fatfury2, fatfury2, neogeo)
STD_ROM_FN(fatfury2)

static INT32 prot_data;

UINT8 __fastcall fatfury2ReadByteProtection(UINT32 sekAddress)
{
	UINT16 res = (prot_data >> 24) & 0xFF;

//	bprintf(PRINT_NORMAL, _T("  - prot 0x%06X read byte (PC: 0x%06X)\n"), sekAddress, SekGetPC(-1));

	switch (sekAddress) {
		case 0x255551:
		case 0x2FFFF1:
		case 0x200001:
		case 0x2FF001:

		case 0x236001:
		case 0x236009:
//			bprintf(PRINT_NORMAL, _T("  - prot read byte: 0x%02X (PC: 0x%06X)\n"), res, SekGetPC(-1));
			return res;

		case 0x236005:
		case 0x23600D:
//			bprintf(PRINT_NORMAL, _T("  - prot read byte: 0x%02X (PC: 0x%06X)\n"), ((res & 0xF0) >> 4) | ((res & 0x0F) << 4), SekGetPC(-1));
			return ((res & 0xF0) >> 4) | ((res & 0x0F) << 4);

//		default:
//			bprintf(PRINT_NORMAL, _T("  - prot 0x%06X read byte (PC: 0x%06X)\n"), sekAddress, SekGetPC(-1));
	}

	return 0;
}

UINT16 __fastcall fatfury2ReadWordProtection(UINT32 sekAddress)
{
	UINT16 res = (prot_data >> 24) & 0xFF;

//	bprintf(PRINT_NORMAL, _T("  - prot 0x%06X read word (PC: 0x%06X)\n"), sekAddress, SekGetPC(-1));

	switch (sekAddress) {
		case 0x255550:
		case 0x2FFFF0:
		case 0x200000:
		case 0x2FF000:

		case 0x236000:
		case 0x236008:
//			bprintf(PRINT_NORMAL, _T("  - prot read word: 0x%02X (PC: 0x%06X)\n"), res, SekGetPC(-1));
			return res;

		case 0x236004:
		case 0x23600C:
//			bprintf(PRINT_NORMAL, _T("  - prot read word: 0x%02X (PC: 0x%06X)\n"), ((res & 0xF0) >> 4) | ((res & 0x0F) << 4), SekGetPC(-1));
			return ((res & 0xF0) >> 4) | ((res & 0x0F) << 4);

//		default:
//			bprintf(PRINT_NORMAL, _T("  - prot 0x%06X read word (PC: 0x%06X)\n"), sekAddress, SekGetPC(-1));
	}

	return 0;
}

void __fastcall fatfury2WriteByteProtection(UINT32 sekAddress, UINT8 /*byteValue*/)
{
//	bprintf(PRINT_NORMAL, _T("  - prot 0x%06X -> 0x%02X (PC: 0x%06X)\n"), sekAddress, byteValue, SekGetPC(-1));

	switch (sekAddress) {
		case 0x255551:
		case 0x2FFFF1:
		case 0x2FF001:

		case 0x236001:
		case 0x236005:
		case 0x236009:
		case 0x23600D:
			prot_data <<= 8;
			break;

//		default:
//			bprintf(PRINT_NORMAL, _T("  - prot 0x%06X -> 0x%02X\n"), sekAddress, byteValue);
	}
}

void __fastcall fatfury2WriteWordProtection(UINT32 sekAddress, UINT16 /*wordValue*/)
{
//	bprintf(PRINT_NORMAL, _T("  - prot 0x%06X -> 0x%04X (PC: 0x%06X)\n"), sekAddress, wordValue, SekGetPC(-1));

	switch (sekAddress) {
		case 0x211112:				// data = 0x1111, expects 0xFF000000 back
			prot_data = 0xFF000000;
/*
			prot_data  = (wordValue & 0x1000) << 19;
			prot_data |= (wordValue & 0x0100) << 22;
			prot_data |= (wordValue & 0x0010) << 25;
			prot_data |= (wordValue & 0x0001) << 28;

			prot_data |= prot_data >> 4;
*/
//			bprintf(PRINT_NORMAL, _T("    prot data -> 0x%08X\n"), prot_data);
			break;

		case 0x233332:				// data = 0x3333, expects 0x0000FFFF back
			prot_data = 0x0000FFFF;
/*
			prot_data  = wordValue << 0;
			prot_data |= wordValue << 2;
*/
//			bprintf(PRINT_NORMAL, _T("    prot data -> 0x%08X\n"), prot_data);
			break;

		case 0x244442:				// data = 0x4444, expects 0x00FF0000 back
			prot_data = 0x00FF0000;
/*
			prot_data  = (wordValue & 0x4000) <<  9;
			prot_data |= (wordValue & 0x0400) << 12;
			prot_data |= (wordValue & 0x0040) << 15;
			prot_data |= (wordValue & 0x0004) << 18;

			prot_data |= prot_data >> 4;
*/
//			bprintf(PRINT_NORMAL, _T("    prot data -> 0x%08X\n"), prot_data);
			break;

		case 0x255552:				// data == 0x5555; read back from 55550, ffff0, 00000, ff000
			prot_data = 0xFF00FF00;
			break;

		case 0x256782:				// data == 0x1234; read back from 36000 *or* 36004
			prot_data = 0xF05A3601;
			break;

		case 0x242812:				// data == 0x1824; read back from 36008 *or* 3600c
			prot_data = 0x81422418;
			break;

//		default:
//			bprintf(PRINT_NORMAL, _T("  - prot 0x%06X -> 0x%02X\n"), sekAddress, wordValue);
	}
}

INT32 fatfury2Scan(INT32 nAction, INT32* /*pnMin*/)
{
	if (nAction & ACB_DRIVER_DATA) {
		SCAN_VAR(prot_data);
	}

	return 0;
}

static void fatfury2InstallHanders()
{
	// Install protection handler
	SekMapHandler(6,	0x200000, 0x2FFFFF, MAP_WRITE);
	SekMapHandler(6,	0x200000, 0x2FFFFF, MAP_READ);
	SekSetWriteWordHandler(6, fatfury2WriteWordProtection);
	SekSetWriteByteHandler(6, fatfury2WriteByteProtection);
	SekSetReadWordHandler(6, fatfury2ReadWordProtection);
	SekSetReadByteHandler(6, fatfury2ReadByteProtection);
}

static INT32 fatfury2Init()
{
	NeoCallbackActive->pInstallHandlers = fatfury2InstallHanders;
	NeoCallbackActive->pScan = fatfury2Scan;

	fatfury2mode = 1;

	return NeoInit();
}

struct BurnDriver BurnDrvFatfury2 = {
	"fatfury2", NULL, "neogeo", NULL, "1992",
	"Fatal Fury 2 / Garou Densetsu 2 - arata-naru tatakai (NGM-047)(NGH-047)\0", NULL, "SNK", "Neo Geo MVS",
	L"Fatal Fury 2\0\u9913\u72FC\u4F1D\u8AAC\uFF12 - \u65B0\u305F\u306A\u308B\u95D8 (NGM-047)(NGH-047)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_VSFIGHT, FBF_FATFURY,
	NULL, fatfury2RomInfo, fatfury2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	fatfury2Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Super Sidekicks / Tokuten Ou

static struct BurnRomInfo ssidekiRomDesc[] = {
	/* also found AES set on PROG board NEO-AEG PROG-G2 (SNK-9201) and CHA board NEO-AEG CHA42G-2;
	same chip labels */
	{ "052-p1.p1",    0x080000, 0x9cd97256, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "052-s1.s1",    0x020000, 0x97689804, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "052-c1.c1",    0x200000, 0x53e1c002, 3 | BRF_GRA },           //  2 Sprite data 		/ TC5316200
	{ "052-c2.c2",    0x200000, 0x776a2d1f, 3 | BRF_GRA },           //  3 					/ TC5316200

	{ "052-m1.m1",    0x020000, 0x49f17d2d, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code 		/ TC531001

	{ "052-v1.v1",    0x200000, 0x22c097a5, 5 | BRF_SND },           //  5 Sound data 		/ TC5316200
};

STDROMPICKEXT(ssideki, ssideki, neogeo)
STD_ROM_FN(ssideki)

// This game uses protection similar to Fatal Fury 2

struct BurnDriver BurnDrvSsideki = {
	"ssideki", NULL, "neogeo", NULL, "1992",
	"Super Sidekicks / Tokuten Ou\0", NULL, "SNK", "Neo Geo MVS",
	L"Super Sidekicks\0\u5F97\u70B9\u738B\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_SPORTSFOOTBALL, 0,
	NULL, ssidekiRomInfo, ssidekiRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	fatfury2Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '94 (NGM-055)(NGH-055)
/* MVS AND AES VERSION */

static struct BurnRomInfo kof94RomDesc[] = {
	{ "055-p1.p1",    0x200000, 0xf10a2042, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC5316200

	{ "055-s1.s1",    0x020000, 0x825976c1, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "055-c1.c1",    0x200000, 0xb96ef460, 3 | BRF_GRA },           //  2 Sprite data 		/ mask rom TC5316200
	{ "055-c2.c2",    0x200000, 0x15e096a7, 3 | BRF_GRA },           //  3 					/ mask rom TC5316200
	{ "055-c3.c3",    0x200000, 0x54f66254, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "055-c4.c4",    0x200000, 0x0b01765f, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200
	{ "055-c5.c5",    0x200000, 0xee759363, 3 | BRF_GRA },           //  6 					/ mask rom TC5316200
	{ "055-c6.c6",    0x200000, 0x498da52c, 3 | BRF_GRA },           //  7 					/ mask rom TC5316200
	{ "055-c7.c7",    0x200000, 0x62f66888, 3 | BRF_GRA },           //  8 					/ mask rom TC5316200
	{ "055-c8.c8",    0x200000, 0xfe0a235d, 3 | BRF_GRA },           //  9 					/ mask rom TC5316200

	{ "055-m1.m1",    0x020000, 0xf6e77cf5, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code 		/ mask rom TC531001

	{ "055-v1.v1",    0x200000, 0x8889596d, 5 | BRF_SND },           // 11 Sound data 		/ mask rom TC5316200
	{ "055-v2.v2",    0x200000, 0x25022b27, 5 | BRF_SND },           // 12 					/ mask rom TC5316200
	{ "055-v3.v3",    0x200000, 0x83cf32c0, 5 | BRF_SND },           // 13 					/ mask rom TC5316200
};

STDROMPICKEXT(kof94, kof94, neogeo)
STD_ROM_FN(kof94)

struct BurnDriver BurnDrvKof94 = {
	"kof94", NULL, "neogeo", NULL, "1994",
	"The King of Fighters '94 (NGM-055)(NGH-055)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, FBF_KOF,
	NULL, kof94RomInfo, kof94RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Art of Fighting 2 / Ryuuko no Ken 2 (NGM-056)
/* MVS VERSION */

static struct BurnRomInfo aof2RomDesc[] = {
	{ "056-p1.p1",    0x100000, 0xa3b1d021, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200
	/* also found MVS set with EP1 / EP2 on eprom on PROG board NEO-MVS PROGGSC; correct chip labels unknown
	and CHA board NEO-MVS CHA256 with 8xC; chip labels are the same
	{ "056-epr.ep1",  0x080000, 0x00000000, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ 27C240-12
	{ "056-epr.ep2",  0x080000, 0x00000000, 1 | BRF_ESS | BRF_PRG }, //  1 					/ M27C4002 */

	{ "056-s1.s1",    0x020000, 0x8b02638e, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	/* Different layout with 4xC (32mbit) also exists on board NEO-MVS CHA256;
	chip labels are 056-C13, 056-C24, 056-C57 and 056-C68
	{ "056-c13.c1",   0x400000, 0xbd3aa959, 3 | BRF_GRA },           //  2 Sprite data
	{ "056-c24.c2",   0x400000, 0xe58297c2, 3 | BRF_GRA },           //  3
	{ "056-c57.c3",   0x400000, 0xb4ad87e5, 3 | BRF_GRA },           //  4
	{ "056-c68.c4",   0x400000, 0x9d3982c8, 3 | BRF_GRA },           //  5 */
	{ "056-c1.c1",    0x200000, 0x17b9cbd2, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "056-c2.c2",    0x200000, 0x5fd76b67, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "056-c3.c3",    0x200000, 0xd2c88768, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "056-c4.c4",    0x200000, 0xdb39b883, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "056-c5.c5",    0x200000, 0xc3074137, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "056-c6.c6",    0x200000, 0x31de68d3, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "056-c7.c7",    0x200000, 0x3f36df57, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "056-c8.c8",    0x200000, 0xe546d7a8, 3 | BRF_GRA },           //  9 					/ TC5316200

	{ "056-m1.m1",    0x020000, 0xf27e9d52, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "056-v1.v1",    0x200000, 0x4628fde0, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "056-v2.v2",    0x200000, 0xb710e2f2, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "056-v3.v3",    0x100000, 0xd168c301, 5 | BRF_SND },           // 13 					/ TC538200
};

STDROMPICKEXT(aof2, aof2, neogeo)
STD_ROM_FN(aof2)

struct BurnDriver BurnDrvAof2 = {
	"aof2", NULL, "neogeo", NULL, "1994",
	"Art of Fighting 2 / Ryuuko no Ken 2 (NGM-056)\0", NULL, "SNK", "Neo Geo MVS",
	L"Art of Fighting 2\0\u9F8D\u864E\u306E\u62F3\uFF12 (NGM-056)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, aof2RomInfo, aof2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Art of Fighting 2 / Ryuuko no Ken 2 (NGH-056)
/* AES VERSION */

static struct BurnRomInfo aof2aRomDesc[] = {
	{ "056-p1.p1",    0x100000, 0xa3b1d021, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200

	{ "056-s1.s1",    0x020000, 0x8b02638e, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "056-c1.c1",    0x200000, 0x17b9cbd2, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "056-c2.c2",    0x200000, 0x5fd76b67, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "056-c3.c3",    0x200000, 0xd2c88768, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "056-c4.c4",    0x200000, 0xdb39b883, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "056-c5.c5",    0x200000, 0xc3074137, 3 | BRF_GRA },           //  6					/ TC5316200
	{ "056-c6.c6",    0x200000, 0x31de68d3, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "056-c7.c7",    0x200000, 0x3f36df57, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "056-c8.c8",    0x200000, 0xe546d7a8, 3 | BRF_GRA },           //  9 					/ TC5316200

	{ "056-m1.m1",    0x020000, 0xf27e9d52, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "056-v1.v1",    0x200000, 0x4628fde0, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "056-v2.v2",    0x200000, 0xb710e2f2, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "056-v3.v3",    0x100000, 0xd168c301, 5 | BRF_SND },           // 13 					/ TC538200

	{ "056-epr.ep1",  0x080000, 0x75d6301c, 0 | BRF_ESS | BRF_PRG }, // 14					/ M27C4002
};

STDROMPICKEXT(aof2a, aof2a, neogeo)
STD_ROM_FN(aof2a)

static void aof2aCallback()
{
	BurnLoadRom(Neo68KROMActive, 14, 1);
}

static INT32 aof2aInit()
{
	NeoCallbackActive->pInitialise = aof2aCallback;

 	return NeoInit();
}

struct BurnDriver BurnDrvAof2a = {
	"aof2a", "aof2", "neogeo", NULL, "1994",
	"Art of Fighting 2 / Ryuuko no Ken 2 (NGH-056)\0", NULL, "SNK", "Neo Geo MVS",
	L"Art of Fighting 2\0\u9F8D\u864E\u306E\u62F3\uFF12 (NGH-056)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, aof2aRomInfo, aof2aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	aof2aInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Fatal Fury Special / Garou Densetsu Special (set 1)(NGM-058)(NGH-058)
/* MVS AND AES VERSION */

static struct BurnRomInfo fatfurspRomDesc[] = {
	{ "058-p1.p1",    0x100000, 0x2f585ba2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC538200
	{ "058-p2.sp2",   0x080000, 0xd7c71a6b, 1 | BRF_ESS | BRF_PRG }, //  1 					/ mask rom TC534200

	{ "058-s1.s1",    0x020000, 0x2df03197, 2 | BRF_GRA },           //  2 Text layer tiles / mask rom TC531000

	{ "058-c1.c1",    0x200000, 0x044ab13c, 3 | BRF_GRA },           //  3 Sprite data		/ mask rom TC5316200
	{ "058-c2.c2",    0x200000, 0x11e6bf96, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "058-c3.c3",    0x200000, 0x6f7938d5, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200
	{ "058-c4.c4",    0x200000, 0x4ad066ff, 3 | BRF_GRA },           //  6 					/ mask rom TC5316200
	{ "058-c5.c5",    0x200000, 0x49c5e0bf, 3 | BRF_GRA },           //  7 					/ mask rom TC5316200
	{ "058-c6.c6",    0x200000, 0x8ff1f43d, 3 | BRF_GRA },           //  8 					/ mask rom TC5316200

	{ "058-m1.m1",    0x020000, 0xccc5186e, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code			/ mask rom TC531001

	{ "058-v1.v1",    0x200000, 0x55d7ce84, 5 | BRF_SND },           // 10 Sound data		/ mask rom TC5316200
	{ "058-v2.v2",    0x200000, 0xee080b10, 5 | BRF_SND },           // 11 					/ mask rom TC5316200
	{ "058-v3.v3",    0x100000, 0xf9eb3d4a, 5 | BRF_SND },           // 12 					/ mask rom TC538200
};

STDROMPICKEXT(fatfursp, fatfursp, neogeo)
STD_ROM_FN(fatfursp)

struct BurnDriver BurnDrvFatfursp = {
	"fatfursp", NULL, "neogeo", NULL, "1993",
	"Fatal Fury Special / Garou Densetsu Special (set 1)(NGM-058)(NGH-058)\0", NULL, "SNK", "Neo Geo MVS",
	L"Fatal Fury Special\0\u9913\u72FC\u4F1D\u8AAC Special (set 1)(NGM-058)(NGH-058)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, fatfurspRomInfo, fatfurspRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Fatal Fury Special / Garou Densetsu Special (set 2)(NGM-058)(NGH-058)
/* MVS AND AES VERSION */

static struct BurnRomInfo fatfurspaRomDesc[] = {
	{ "058-p1.p1",    0x100000, 0x2f585ba2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC538200
	{ "058-p2.sp2",   0x080000, 0xd7c71a6b, 1 | BRF_ESS | BRF_PRG }, //  1					/ mask rom TC534200

	{ "058-s1.s1",    0x020000, 0x2df03197, 2 | BRF_GRA },           //  2 Text layer tiles / mask rom TC531000

	{ "058-c1.c1",    0x200000, 0x044ab13c, 3 | BRF_GRA },           //  3 Sprite data		/ mask rom TC5316200
	{ "058-c2.c2",    0x200000, 0x11e6bf96, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "058-c3.c3",    0x200000, 0x6f7938d5, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200
	{ "058-c4.c4",    0x200000, 0x4ad066ff, 3 | BRF_GRA },           //  6 					/ mask rom TC5316200
	{ "058-c5.c5",    0x200000, 0x49c5e0bf, 3 | BRF_GRA },           //  7 					/ mask rom TC5316200
	{ "058-c6.c6",    0x200000, 0x8ff1f43d, 3 | BRF_GRA },           //  8 					/ mask rom TC5316200

	{ "058-m1.m1",    0x020000, 0xccc5186e, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code			/ mask rom TC531001

	{ "058-v1.v1",    0x200000, 0x55d7ce84, 5 | BRF_SND },           // 10 Sound data		/ mask rom TC5316200
	{ "058-v2.v2",    0x200000, 0xee080b10, 5 | BRF_SND },           // 11 					/ mask rom TC5316200
	{ "058-v3.v3",    0x100000, 0xf9eb3d4a, 5 | BRF_SND },           // 12 					/ mask rom TC538200

	/* the rom below acts as a patch to the program rom in the cart, replacing the first 512kb */
	{ "058-epr.ep1",  0x080000, 0x9f0c1e1a, 0 | BRF_ESS | BRF_PRG }, // 13					/ M27C4002
	/* P is on eprom, correct chip label unknown */
};

STDROMPICKEXT(fatfurspa, fatfurspa, neogeo)
STD_ROM_FN(fatfurspa)

static void fatfurspaCallback()
{
	BurnLoadRom(Neo68KROMActive, 13, 1);
}

static INT32 fatfurspaInit()
{
	NeoCallbackActive->pInitialise = fatfurspaCallback;

 	return NeoInit();
}

struct BurnDriver BurnDrvFatfurspa = {
	"fatfurspa", "fatfursp", "neogeo", NULL, "1993",
	"Fatal Fury Special / Garou Densetsu Special (set 2)(NGM-058)(NGH-058)\0", NULL, "SNK", "Neo Geo MVS",
	L"Fatal Fury Special\0\u9913\u72FC\u4F1D\u8AAC Special (set 2)(NGM-058)(NGH-058)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, fatfurspaRomInfo, fatfurspaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	fatfurspaInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Savage Reign / Fu'un Mokushiroku - kakutou sousei
/* MVS AND AES VERSION? */

static struct BurnRomInfo savagereRomDesc[] = {
	/* also found MVS set with PROG board NEO-MVS PROG 4096 B and CHA board NEO-MVS CHA 42G-3;
	same layouts and chip labels */
	{ "059-p1.p1",    0x200000, 0x01d4e9c0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "059-s1.s1",    0x020000, 0xe08978ca, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "059-c1.c1",    0x200000, 0x763ba611, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "059-c2.c2",    0x200000, 0xe05e8ca6, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "059-c3.c3",    0x200000, 0x3e4eba4b, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "059-c4.c4",    0x200000, 0x3c2a3808, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "059-c5.c5",    0x200000, 0x59013f9e, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "059-c6.c6",    0x200000, 0x1c8d5def, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "059-c7.c7",    0x200000, 0xc88f7035, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "059-c8.c8",    0x200000, 0x484ce3ba, 3 | BRF_GRA },           //  9 					/ TC5316200

	{ "059-m1.m1",    0x020000, 0x29992eba, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "059-v1.v1",    0x200000, 0x530c50fd, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "059-v2.v2",    0x200000, 0xeb6f1cdb, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "059-v3.v3",    0x200000, 0x7038c2f9, 5 | BRF_SND },           // 13 					/ TC5316200
};

STDROMPICKEXT(savagere, savagere, neogeo)
STD_ROM_FN(savagere)

struct BurnDriver BurnDrvSavagere = {
	"savagere", NULL, "neogeo", NULL, "1995",
	"Savage Reign / Fu'un Mokushiroku - kakutou sousei\0", NULL, "SNK", "Neo Geo MVS",
	L"Savage Reign\0\u98A8\u96F2\u9ED9\u793A\u9332 - \u683C\u95D8\u5275\u4E16\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, savagereRomInfo, savagereRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Super Sidekicks 2 - The World Championship / Tokuten Ou 2 - real fight football (NGM-061)(NGH-061)
/* MVS AND AES VERSION */

static struct BurnRomInfo ssideki2RomDesc[] = {
	{ "061-p1.p1",      0x100000, 0x5969e0dc, 1 | BRF_ESS | BRF_PRG }, //  0 68K code		  / mask rom TC538200
	/* also found MVS set with EP1 / EP2 on eprom on PROG board NEO-MVS PROGTOP; correct chip labels unknown. */

	{ "061-s1.s1",      0x020000, 0x226d1b68, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	/* Different layout with 8xC (8 mbit) also exists on board NEO-MVS CHA256 ; naming scheme 061-Cx */
	{ "061-c1-16.c1",   0x200000, 0xa626474f, 3 | BRF_GRA },           //  2 Sprite data	  / mask rom TC5316200
	{ "061-c2-16.c2",   0x200000, 0xc3be42ae, 3 | BRF_GRA },           //  3 				  / mask rom TC5316200
	{ "061-c3-16.c3",   0x200000, 0x2a7b98b9, 3 | BRF_GRA },           //  4 				  / mask rom TC5316200
	{ "061-c4-16.c4",   0x200000, 0xc0be9a1f, 3 | BRF_GRA },           //  5 				  / mask rom TC5316200

	{ "061-m1.m1",      0x020000, 0x156f6951, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code		  / mask rom TC531001

	{ "061-v1.v1",      0x200000, 0xf081c8d3, 5 | BRF_SND },           //  7 Sound data		  / mask rom TC5316200
	{ "061-v2.v2",      0x200000, 0x7cd63302, 5 | BRF_SND },           //  8 				  / mask rom TC5316200
};

STDROMPICKEXT(ssideki2, ssideki2, neogeo)
STD_ROM_FN(ssideki2)

struct BurnDriver BurnDrvSsideki2 = {
	"ssideki2", NULL, "neogeo", NULL, "1994",
	"Super Sidekicks 2 - The World Championship / Tokuten Ou 2 - real fight football (NGM-061)(NGH-061)\0", NULL, "SNK", "Neo Geo MVS",
	L"Super Sidekicks 2 - the world championship\0\u5F97\u70B9\u738B\uFF12 - \u30EA\u30A2\u30EB\u30D5\u30A1\u30A4\u30C8\u30D5\u30C3\u30C8\u30DC\u30FC\u30EB (NGM-061)(NGH-061)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSFOOTBALL, 0,
	NULL, ssideki2RomInfo, ssideki2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Samurai Shodown II / Shin Samurai Spirits - Haohmaru jigokuhen (NGM-063)(NGH-063)
/* MVS AND AES VERSION */

static struct BurnRomInfo samsho2RomDesc[] = {
	{ "063-p1.p1",    0x200000, 0x22368892, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200
	/* also found MVS set with EP1 / EP2 / P1 / SP2 on eprom on PROG board NEO-MVS PROGTOP;
	and CHA board NEO-MVS CHA256 (same layout and chip labels).
	{ "063-epr.p1",   0x080000, 0x00000000, 1 | BRF_ESS | BRF_PRG }, //  0 68k code 		/ D27C4000 NEC JAPAN
	{ "063-epr.sp2",  0x080000, 0x00000000, 1 | BRF_ESS | BRF_PRG }, //  1 					/ D27C4000 NEC JAPAN
	{ "063-epr.ep1",  0x080000, 0x00000000, 1 | BRF_ESS | BRF_PRG }, //  2 					/ M27C4002
	{ "063-epr.ep2",  0x080000, 0x00000000, 1 | BRF_ESS | BRF_PRG }, //  3 					/ M27C4002 */

	{ "063-s1.s1",    0x020000, 0x64a5cd66, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "063-c1.c1",    0x200000, 0x86cd307c, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "063-c2.c2",    0x200000, 0xcdfcc4ca, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "063-c3.c3",    0x200000, 0x7a63ccc7, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "063-c4.c4",    0x200000, 0x751025ce, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "063-c5.c5",    0x200000, 0x20d3a475, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "063-c6.c6",    0x200000, 0xae4c0a88, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "063-c7.c7",    0x200000, 0x2df3cbcf, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "063-c8.c8",    0x200000, 0x1ffc6dfa, 3 | BRF_GRA },           //  9 					/ TC5316200

	{ "063-m1.m1",    0x020000, 0x56675098, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "063-v1.v1",    0x200000, 0x37703f91, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "063-v2.v2",    0x200000, 0x0142bde8, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "063-v3.v3",    0x200000, 0xd07fa5ca, 5 | BRF_SND },           // 13 					/ TC5316200
	{ "063-v4.v4",    0x100000, 0x24aab4bb, 5 | BRF_SND },           // 14 					/ TC538200
};

STDROMPICKEXT(samsho2, samsho2, neogeo)
STD_ROM_FN(samsho2)

struct BurnDriver BurnDrvSamsho2 = {
	"samsho2", NULL, "neogeo", NULL, "1994",
	"Samurai Shodown II / Shin Samurai Spirits - Haohmaru jigokuhen (NGM-063)(NGH-063)\0", NULL, "SNK", "Neo Geo MVS",
	L"Samurai Shodown II\0\u771F Samurai Spirits - \u8987\u738B\u4E38\u5730\u7344\u5909 (NGM-063)(NGH-063)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho2RomInfo, samsho2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Saulabi Spirits / Jin Saulabi Tu Hon (Korean release of Samurai Shodown II)
/* KOREAN VERSION */

static struct BurnRomInfo samsho2kRomDesc[] = {
	// This has corrupt text if used with the Japan bios due to the replacement of the s1 rom to contain the new logo
	{ "063-p1-kan.p1",0x200000, 0x147cc6d7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "063-s1-kan.s1",0x020000, 0xff08f80b, 2 | BRF_GRA },           //  1 Text layer tiles /

	{ "063-c1.c1",    0x200000, 0x86cd307c, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "063-c2.c2",    0x200000, 0xcdfcc4ca, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "063-c3.c3",    0x200000, 0x7a63ccc7, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "063-c4.c4",    0x200000, 0x751025ce, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "063-c5.c5",    0x200000, 0x20d3a475, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "063-c6.c6",    0x200000, 0xae4c0a88, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "063-c7.c7",    0x200000, 0x2df3cbcf, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "063-c8.c8",    0x200000, 0x1ffc6dfa, 3 | BRF_GRA },           //  9 					/ TC5316200

	{ "063-m1.m1",    0x020000, 0x56675098, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "063-v1.v1",    0x200000, 0x37703f91, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "063-v2.v2",    0x200000, 0x0142bde8, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "063-v3.v3",    0x200000, 0xd07fa5ca, 5 | BRF_SND },           // 13 					/ TC5316200
	{ "063-v4.v4",    0x100000, 0x24aab4bb, 5 | BRF_SND },           // 14 					/ TC538200

	// the roms below apply as patch over the main program
	{ "063-ep1-kan.ep1", 0x080000, 0xfa32e2d8, 0 | BRF_ESS | BRF_PRG }, // 15
	{ "063-ep2-kan.ep2", 0x080000, 0x70b1a4d9, 0 | BRF_ESS | BRF_PRG }, // 16
};

STDROMPICKEXT(samsho2k, samsho2k, neogeo)
STD_ROM_FN(samsho2k)

static void samsho2kCallback()
{
	BurnLoadRom(Neo68KROMActive + 0x000000, 15, 1);
	BurnLoadRom(Neo68KROMActive + 0x080000, 16, 1);
}

static INT32 samsho2kInit()
{
	NeoCallbackActive->pInitialise = samsho2kCallback;

 	return NeoInit();
}

struct BurnDriver BurnDrvsamsho2k = {
	"samsho2k", "samsho2", "neogeo", NULL, "1994",
	"Saulabi Spirits / Jin Saulabi Tu Hon (Korean release of Samurai Shodown II)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho2kRomInfo, samsho2kRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	samsho2kInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Saulabi Spirits / Jin Saulabi Tu Hon (Korean release of Samurai Shodown II, set 2)
/* KOREAN VERSION */

static struct BurnRomInfo samsho2kaRomDesc[] = {
	{ "063-p1-kan.p1",0x200000, 0x147cc6d7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	// Basically samsho2k without loading ep1 and ep2 over p1-kan ; The game has been confirmed to exist in this state

	{ "063-s1-kan.s1",0x020000, 0xff08f80b, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "063-c1.c1",    0x200000, 0x86cd307c, 3 | BRF_GRA },           //  2 Sprite data
	{ "063-c2.c2",    0x200000, 0xcdfcc4ca, 3 | BRF_GRA },           //  3
	{ "063-c3.c3",    0x200000, 0x7a63ccc7, 3 | BRF_GRA },           //  4
	{ "063-c4.c4",    0x200000, 0x751025ce, 3 | BRF_GRA },           //  5
	{ "063-c5.c5",    0x200000, 0x20d3a475, 3 | BRF_GRA },           //  6
	{ "063-c6.c6",    0x200000, 0xae4c0a88, 3 | BRF_GRA },           //  7
	{ "063-c7.c7",    0x200000, 0x2df3cbcf, 3 | BRF_GRA },           //  8
	{ "063-c8.c8",    0x200000, 0x1ffc6dfa, 3 | BRF_GRA },           //  9

	{ "063-m1.m1",    0x020000, 0x56675098, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "063-v1.v1",    0x200000, 0x37703f91, 5 | BRF_SND },           // 11 Sound data
	{ "063-v2.v2",    0x200000, 0x0142bde8, 5 | BRF_SND },           // 12
	{ "063-v3.v3",    0x200000, 0xd07fa5ca, 5 | BRF_SND },           // 13
	{ "063-v4.v4",    0x100000, 0x24aab4bb, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(samsho2ka, samsho2ka, neogeo)
STD_ROM_FN(samsho2ka)

struct BurnDriver BurnDrvsamsho2ka = {
	"samsho2ka", "samsho2", "neogeo", NULL, "1994",
	"Saulabi Spirits / Jin Saulabi Tu Hon (Korean release of Samurai Shodown II, set 2)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho2kaRomInfo, samsho2kaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Fatal Fury 3 - Road to the Final Victory / Garou Densetsu 3 - haruka-naru tatakai (NGM-069)(NGH-069)
/* MVS AND AES VERSION (95-02-28 13:39) */

static struct BurnRomInfo fatfury3RomDesc[] = {
	{ "069-p1.p1",    0x100000, 0xa8bcfbbc, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200
	{ "069-sp2.sp2",  0x200000, 0xdbe963ed, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "069-s1.s1",    0x020000, 0x0b33a800, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "069-c1.c1",    0x400000, 0xe302f93c, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "069-c2.c2",    0x400000, 0x1053a455, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "069-c3.c3",    0x400000, 0x1c0fde2f, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "069-c4.c4",    0x400000, 0xa25fc3d0, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "069-c5.c5",    0x200000, 0xb3ec6fa6, 3 | BRF_GRA },           //  7 					/ TC5332205
	{ "069-c6.c6",    0x200000, 0x69210441, 3 | BRF_GRA },           //  8 					/ TC5332205

	{ "069-m1.m1",    0x020000, 0xfce72926, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code			/ TC531001

	{ "069-v1.v1",    0x400000, 0x2bdbd4db, 5 | BRF_SND },           // 10 Sound data		/ TC5332204
	{ "069-v2.v2",    0x400000, 0xa698a487, 5 | BRF_SND },           // 11 					/ TC5332204
	{ "069-v3.v3",    0x200000, 0x581c5304, 5 | BRF_SND },           // 12 					/ TC5316200
};

STDROMPICKEXT(fatfury3, fatfury3, neogeo)
STD_ROM_FN(fatfury3)

struct BurnDriver BurnDrvFatfury3 = {
	"fatfury3", NULL, "neogeo", NULL, "1995",
	"Fatal Fury 3 - Road to the Final Victory / Garou Densetsu 3 - haruka-naru tatakai (NGM-069)(NGH-069)\0", NULL, "SNK", "Neo Geo MVS",
	L"Fatal Fury 3 - Road to the Final Victory\0\u9913\u72FC\u4F1D\u8AAC\uFF13 (NGM-069)(NGH-069)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, fatfury3RomInfo, fatfury3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Super Sidekicks 3 - The Next Glory / Tokuten Ou 3 - eikou e no michi
/* MVS ANS AES VERSION? */

static struct BurnRomInfo ssideki3RomDesc[] = {
	/* It also exists MVS Sets with PROG board NEO-MVS PROG 4096 B and CHA board NEO-MVS CHA 42G-3,
	and with PROG board NEO-MVS PROGBK1 and CHA board NEO-MVS CHA256B; same chip labels. */
	/* also found AES set with PROG board NEO-AEG PROGTOP2 and CHA board NEO-AEG CHA256B; same chip labels */
	{ "081-p1.p1",    0x200000, 0x6bc27a3d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "081-s1.s1",    0x020000, 0x7626da34, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "081-c1.c1",    0x200000, 0x1fb68ebe, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "081-c2.c2",    0x200000, 0xb28d928f, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "081-c3.c3",    0x200000, 0x3b2572e8, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "081-c4.c4",    0x200000, 0x47d26a7c, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "081-c5.c5",    0x200000, 0x17d42f0d, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "081-c6.c6",    0x200000, 0x6b53fb75, 3 | BRF_GRA },           //  7 					/ TC5316200

	{ "081-m1.m1",    0x020000, 0x82fcd863, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code			/ TC531001

	{ "081-v1.v1",    0x200000, 0x201fa1e1, 5 | BRF_SND },           //  9 Sound data 		/ TC5316200
	{ "081-v2.v2",    0x200000, 0xacf29d96, 5 | BRF_SND },           // 10 					/ TC5316200
	{ "081-v3.v3",    0x200000, 0xe524e415, 5 | BRF_SND },           // 11 					/ TC5316200
};

STDROMPICKEXT(ssideki3, ssideki3, neogeo)
STD_ROM_FN(ssideki3)

struct BurnDriver BurnDrvSsideki3 = {
	"ssideki3", NULL, "neogeo", NULL, "1995",
	"Super Sidekicks 3 - The Next Glory / Tokuten Ou 3 - eikou e no michi\0", NULL, "SNK", "Neo Geo MVS",
	L"Super Sidekicks 3 - The Next Glory\0\u5F97\u70B9\u738B\uFF13 - \u6804\u5149\u3078\u306E\u6311\u6226\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_SPORTSFOOTBALL, 0,
	NULL, ssideki3RomInfo, ssideki3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// The King of Fighters '95 (NGM-084)
/* MVS VERSION */

static struct BurnRomInfo kof95RomDesc[] = {
	{ "084-p1.p1",    0x200000, 0x2cba2716, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "084-s1.s1",    0x020000, 0xde716f8a, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "084-c1.c1",    0x400000, 0xfe087e32, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332202
	{ "084-c2.c2",    0x400000, 0x07864e09, 3 | BRF_GRA },           //  3 					/ TC5332202
	{ "084-c3.c3",    0x400000, 0xa4e65d1b, 3 | BRF_GRA },           //  4 					/ TC5332202
	{ "084-c4.c4",    0x400000, 0xc1ace468, 3 | BRF_GRA },           //  5 					/ TC5332202
	{ "084-c5.c5",    0x200000, 0x8a2c1edc, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "084-c6.c6",    0x200000, 0xf593ac35, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "084-c7.c7",    0x100000, 0x9904025f, 3 | BRF_GRA },           //  8 					/ TC538200
	{ "084-c8.c8",    0x100000, 0x78eb0f9b, 3 | BRF_GRA },           //  9 					/ TC538200

	{ "084-m1.m1",    0x020000, 0x6f2d7429, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "084-v1.v1",    0x400000, 0x84861b56, 5 | BRF_SND },           // 11 Sound data		/ TC5332201
	{ "084-v2.v2",    0x200000, 0xb38a2803, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "084-v3.v3",    0x100000, 0xd683a338, 5 | BRF_SND },           // 13 					/ TC538200
};

STDROMPICKEXT(kof95, kof95, neogeo)
STD_ROM_FN(kof95)

struct BurnDriver BurnDrvKof95 = {
	"kof95", NULL, "neogeo", NULL, "1995",
	"The King of Fighters '95 (NGM-084)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, FBF_KOF,
	NULL, kof95RomInfo, kof95RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '95 (NGH-084, alternate board)
/* MVS VERSION */

static struct BurnRomInfo kof95aRomDesc[] = {
	/* This set uses NEO-MVS PROGSM board; same rom data as in kof95h is used */
	{ "084-epr.ep1",  0x080000, 0x577ca1bb, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ M27C4002
	{ "084-epr.ep2",  0x080000, 0x30802a5d, 1 | BRF_ESS | BRF_PRG }, //  1					/ M27C4002
	{ "084-epr.ep3",  0x080000, 0x21ae248a, 1 | BRF_ESS | BRF_PRG }, //  2					/ M27C4002
	{ "084-epr.ep4",  0x080000, 0x19d3fbee, 1 | BRF_ESS | BRF_PRG }, //  3					/ M27C4002
	/* P's on eprom, correct chip label unknown */

	{ "084-s1.s1",    0x020000, 0xde716f8a, 2 | BRF_GRA },           //  4 Text layer tiles / TC531000

	{ "084-c1.c1",    0x400000, 0xfe087e32, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332202
	{ "084-c2.c2",    0x400000, 0x07864e09, 3 | BRF_GRA },           //  3 					/ TC5332202
	{ "084-c3.c3",    0x400000, 0xa4e65d1b, 3 | BRF_GRA },           //  4 					/ TC5332202
	{ "084-c4.c4",    0x400000, 0xc1ace468, 3 | BRF_GRA },           //  5 					/ TC5332202
	{ "084-c5.c5",    0x200000, 0x8a2c1edc, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "084-c6.c6",    0x200000, 0xf593ac35, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "084-c7.c7",    0x100000, 0x9904025f, 3 | BRF_GRA },           //  8 					/ TC538200
	{ "084-c8.c8",    0x100000, 0x78eb0f9b, 3 | BRF_GRA },           //  9 					/ TC538200

	{ "084-m1.m1",    0x020000, 0x6f2d7429, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "084-v1.v1",    0x400000, 0x84861b56, 5 | BRF_SND },           // 11 Sound data		/ TC5332201
	{ "084-v2.v2",    0x200000, 0xb38a2803, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "084-v3.v3",    0x100000, 0xd683a338, 5 | BRF_SND },           // 13 					/ TC538200
};

STDROMPICKEXT(kof95a, kof95a, neogeo)
STD_ROM_FN(kof95a)

struct BurnDriver BurnDrvKof95a = {
	"kof95a", "kof95", "neogeo", NULL, "1995",
	"The King of Fighters '95 (NGH-084, alternate board)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof95aRomInfo, kof95aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '95 (NGH-084)
/* MVS AND AES VERSION */

static struct BurnRomInfo kof95hRomDesc[] = {
	{ "084-pg1.p1",   0x200000, 0x5e54cf95, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "084-s1.s1",    0x020000, 0xde716f8a, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "084-c1.c1",    0x400000, 0xfe087e32, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332202
	{ "084-c2.c2",    0x400000, 0x07864e09, 3 | BRF_GRA },           //  3 					/ TC5332202
	{ "084-c3.c3",    0x400000, 0xa4e65d1b, 3 | BRF_GRA },           //  4					/ TC5332202
	{ "084-c4.c4",    0x400000, 0xc1ace468, 3 | BRF_GRA },           //  5 					/ TC5332202
	{ "084-c5.c5",    0x200000, 0x8a2c1edc, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "084-c6.c6",    0x200000, 0xf593ac35, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "084-c7.c7",    0x100000, 0x9904025f, 3 | BRF_GRA },           //  8 					/ TC538200
	{ "084-c8.c8",    0x100000, 0x78eb0f9b, 3 | BRF_GRA },           //  9 					/ TC538200

	{ "084-m1.m1",    0x020000, 0x6f2d7429, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "084-v1.v1",    0x400000, 0x84861b56, 5 | BRF_SND },           // 11 Sound data		/ TC5332201
	{ "084-v2.v2",    0x200000, 0xb38a2803, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "084-v3.v3",    0x100000, 0xd683a338, 5 | BRF_SND },           // 13 					/ TC538200
};

STDROMPICKEXT(kof95h, kof95h, neogeo)
STD_ROM_FN(kof95h)

struct BurnDriver BurnDrvKof95h = {
	"kof95h", "kof95", "neogeo", NULL, "1995",
	"The King of Fighters '95 (NGH-084)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, FBF_KOF,
	NULL, kof95hRomInfo, kof95hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Samurai Shodown III / Samurai Spirits - Zankurou Musouken (NGM-087)
/* MVS VERSION */

static struct BurnRomInfo samsho3RomDesc[] = {
	{ "087-epr.ep1",  0x080000, 0x23e09bb8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ M27C4002
	{ "087-epr.ep2",  0x080000, 0x256f5302, 1 | BRF_ESS | BRF_PRG }, //  1 					/ M27C4002
	{ "087-epr.ep3",  0x080000, 0xbf2db5dd, 1 | BRF_ESS | BRF_PRG }, //  2 					/ M27C4002
	{ "087-epr.ep4",  0x080000, 0x53e60c58, 1 | BRF_ESS | BRF_PRG }, //  3 					/ M27C4002
	/* P's on eprom, correct chip label unknown */
	{ "087-p5.p5",    0x100000, 0xe86ca4af, 1 | BRF_ESS | BRF_PRG }, //  4 					/ TC538200
	/* also found MVS set with p1 / sp2 on maskrom on PROG board NEO-MVS PROGBK1;
	chip label are 087-PG1 and 087-P2 */

	{ "087-s1.s1",    0x020000, 0x74ec7d9f, 2 | BRF_GRA },           //  5 Text layer tiles / TC531000

	{ "087-c1.c1",    0x400000, 0x07a233bc, 3 | BRF_GRA },           //  6 Sprite data		/ TC5332202
	{ "087-c2.c2",    0x400000, 0x7a413592, 3 | BRF_GRA },           //  7 					/ TC5332202
	{ "087-c3.c3",    0x400000, 0x8b793796, 3 | BRF_GRA },           //  8 					/ TC533202
	{ "087-c4.c4",    0x400000, 0x728fbf11, 3 | BRF_GRA },           //  9 					/ TC533202
	{ "087-c5.c5",    0x400000, 0x172ab180, 3 | BRF_GRA },           // 10 					/ TC5332202
	{ "087-c6.c6",    0x400000, 0x002ff8f3, 3 | BRF_GRA },           // 11 					/ TC5332202
	{ "087-c7.c7",    0x100000, 0xae450e3d, 3 | BRF_GRA },           // 12 					/ TC538200
	{ "087-c8.c8",    0x100000, 0xa9e82717, 3 | BRF_GRA },           // 13 					/ TC538200

	{ "087-m1.m1",    0x020000, 0x8e6440eb, 4 | BRF_ESS | BRF_PRG }, // 14 Z80 code			/ TC531001

	{ "087-v1.v1",    0x400000, 0x84bdd9a0, 5 | BRF_SND },           // 15 Sound data		/ TC5332201
	{ "087-v2.v2",    0x200000, 0xac0f261a, 5 | BRF_SND },           // 16					/ TC5316200
};

STDROMPICKEXT(samsho3, samsho3, neogeo)
STD_ROM_FN(samsho3)

struct BurnDriver BurnDrvSamSho3 = {
	"samsho3", NULL, "neogeo", NULL, "1995",
	"Samurai Shodown III / Samurai Spirits - Zankurou Musouken (NGM-087)\0", NULL, "SNK", "Neo Geo MVS",
	L"Samurai Shodown III\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4 - \u65AC\u7D05\u90CE\u7121\u53CC\u5263 (NGM-087)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho3RomInfo, samsho3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Samurai Shodown III / Samurai Spirits - Zankurou Musouken (NGH-087)
/* AES VERSION */

static struct BurnRomInfo samsho3hRomDesc[] = {
	{ "087-pg1.p1",   0x100000, 0x282a336e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC538200
	{ "087-p2.sp2",   0x200000, 0x9bbe27e0, 1 | BRF_ESS | BRF_PRG }, //  1 					/ mask rom TC5316200

	{ "087-s1.s1",    0x020000, 0x74ec7d9f, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "087-c1.c1",    0x400000, 0x07a233bc, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332202
	{ "087-c2.c2",    0x400000, 0x7a413592, 3 | BRF_GRA },           //  4 					/ TC5332202
	{ "087-c3.c3",    0x400000, 0x8b793796, 3 | BRF_GRA },           //  5 					/ TC5332202
	{ "087-c4.c4",    0x400000, 0x728fbf11, 3 | BRF_GRA },           //  6 					/ TC5332202
	{ "087-c5.c5",    0x400000, 0x172ab180, 3 | BRF_GRA },           //  7 					/ TC5332202
	{ "087-c6.c6",    0x400000, 0x002ff8f3, 3 | BRF_GRA },           //  8 					/ TC5332202
	{ "087-c7.c7",    0x100000, 0xae450e3d, 3 | BRF_GRA },           //  9 					/ TC538200
	{ "087-c8.c8",    0x100000, 0xa9e82717, 3 | BRF_GRA },           // 10 					/ TC538200

	{ "087-m1.m1",    0x020000, 0x8e6440eb, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "087-v1.v1",    0x400000, 0x84bdd9a0, 5 | BRF_SND },           // 12 Sound data		/ TC5332201
	{ "087-v2.v2",    0x200000, 0xac0f261a, 5 | BRF_SND },           // 13 					/ TC5316200
};

STDROMPICKEXT(samsho3h, samsho3h, neogeo)
STD_ROM_FN(samsho3h)

struct BurnDriver BurnDrvSamSho3h = {
	"samsho3h", "samsho3", "neogeo", NULL, "1995",
	"Samurai Shodown III / Samurai Spirits - Zankurou Musouken (NGH-087)\0", NULL, "SNK", "Neo Geo MVS",
	L"Samurai Shodown III\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4 - \u65AC\u7D05\u90CE\u7121\u53CC\u5263 (NGH-087)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO,  GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho3hRomInfo, samsho3hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Fighters Swords (Korean release of Samurai Shodown III)
/* KOREAN VERSION */

static struct BurnRomInfo fswordsRomDesc[] = {
	{ "187-p1k.p1",   0x100000, 0xc8e7c075, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC5316200
	{ "087-p2.sp2",   0x200000, 0x9bbe27e0, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "087-s1.s1",    0x020000, 0x74ec7d9f, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "087-c1.c1",    0x400000, 0x07a233bc, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332202
	{ "087-c2.c2",    0x400000, 0x7a413592, 3 | BRF_GRA },           //  4 					/ TC5332202
	{ "087-c3.c3",    0x400000, 0x8b793796, 3 | BRF_GRA },           //  5 					/ TC5332202
	{ "087-c4.c4",    0x400000, 0x728fbf11, 3 | BRF_GRA },           //  6 					/ TC5332202
	{ "087-c5.c5",    0x400000, 0x172ab180, 3 | BRF_GRA },           //  7 					/ TC5332202
	{ "087-c6.c6",    0x400000, 0x002ff8f3, 3 | BRF_GRA },           //  8 					/ TC5332202
	{ "087-c7.c7",    0x100000, 0xae450e3d, 3 | BRF_GRA },           //  9 					/ TC538200
	{ "087-c8.c8",    0x100000, 0xa9e82717, 3 | BRF_GRA },           // 10 					/ TC538200

	{ "087-m1.m1",    0x020000, 0x8e6440eb, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "087-v1.v1",    0x400000, 0x84bdd9a0, 5 | BRF_SND },           // 12 Sound data		/ TC5332201
	{ "087-v2.v2",    0x200000, 0xac0f261a, 5 | BRF_SND },           // 13 					/ TC5316200
};

STDROMPICKEXT(fswords, fswords, neogeo)
STD_ROM_FN(fswords)

struct BurnDriver BurnDrvfswords = {
	"fswords", "samsho3", "neogeo", NULL, "1995",
	"Fighters Swords (Korean release of Samurai Shodown III)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO,  GBF_VSFIGHT, FBF_SAMSHO,
	NULL, fswordsRomInfo, fswordsRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Real Bout Fatal Fury / Real Bout Garou Densetsu (NGM-095)(NGH-095)
/* MVS AND AES VERSION */

static struct BurnRomInfo rbff1RomDesc[] = {
	{ "095-p1.p1",    0x100000, 0x63b4d8ae, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200
	{ "095-p2.sp2",   0x200000, 0xcc15826e, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "095-s1.s1",    0x020000, 0xb6bf5e08, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "069-c1.c1",    0x400000, 0xe302f93c, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "069-c2.c2",    0x400000, 0x1053a455, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "069-c3.c3",    0x400000, 0x1c0fde2f, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "069-c4.c4",    0x400000, 0xa25fc3d0, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "095-c5.c5",    0x400000, 0x8b9b65df, 3 | BRF_GRA },           //  7 					/ TC5332202
	{ "095-c6.c6",    0x400000, 0x3e164718, 3 | BRF_GRA },           //  8 					/ TC5332202
	{ "095-c7.c7",    0x200000, 0xca605e12, 3 | BRF_GRA },           //  9 					/ TC5316200
	{ "095-c8.c8",    0x200000, 0x4e6beb6c, 3 | BRF_GRA },           // 10 					/ TC5316200

	{ "095-m1.m1",    0x020000, 0x653492a7, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "069-v1.v1",    0x400000, 0x2bdbd4db, 5 | BRF_SND },           // 12 Sound data		/ TC5332204
	{ "069-v2.v2",    0x400000, 0xa698a487, 5 | BRF_SND },           // 13 					/ TC5332204
	{ "095-v3.v3",    0x400000, 0x189d1c6c, 5 | BRF_SND },           // 14 					/ TC5332201
};

STDROMPICKEXT(rbff1, rbff1, neogeo)
STD_ROM_FN(rbff1)

struct BurnDriver BurnDrvrbff1 = {
	"rbff1", NULL, "neogeo", NULL, "1995",
	"Real Bout Fatal Fury / Real Bout Garou Densetsu (NGM-095)(NGH-095)\0", NULL, "SNK", "Neo Geo MVS",
	L"Real Bout Fatal Fury\0Real Bout \u9913\u72FC\u4F1D\u8AAC (NGM-095)(NGH-095)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbff1RomInfo, rbff1RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Real Bout Fatal Fury / Real Bout Garou Densetsu (bug fix revision)
/* MVS VERSION */

static struct BurnRomInfo rbff1aRomDesc[] = {
	/* This is a bug fixed revision applied over the original cart. The original P1 and P2 stayed in the cart and this */
	/* 512k ROM was added to replace the first 512k of P1. */
	{ "095-p1.p1",    0x100000, 0x63b4d8ae, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200
	{ "095-p2.sp2",   0x200000, 0xcc15826e, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "095-s1.s1",    0x020000, 0xb6bf5e08, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "069-c1.c1",    0x400000, 0xe302f93c, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "069-c2.c2",    0x400000, 0x1053a455, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "069-c3.c3",    0x400000, 0x1c0fde2f, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "069-c4.c4",    0x400000, 0xa25fc3d0, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "095-c5.c5",    0x400000, 0x8b9b65df, 3 | BRF_GRA },           //  7 					/ TC5332202
	{ "095-c6.c6",    0x400000, 0x3e164718, 3 | BRF_GRA },           //  8 					/ TC5332202
	{ "095-c7.c7",    0x200000, 0xca605e12, 3 | BRF_GRA },           //  9 					/ TC5316200
	{ "095-c8.c8",    0x200000, 0x4e6beb6c, 3 | BRF_GRA },           // 10 					/ TC5316200

	{ "095-m1.m1",    0x020000, 0x653492a7, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "069-v1.v1",    0x400000, 0x2bdbd4db, 5 | BRF_SND },           // 12 Sound data		/ TC5332204
	{ "069-v2.v2",    0x400000, 0xa698a487, 5 | BRF_SND },           // 13 					/ TC5332204
	{ "095-v3.v3",    0x400000, 0x189d1c6c, 5 | BRF_SND },           // 14 					/ TC5332201

	/* the rom below acts as a patch to the program rom in the cart, replacing the first 512kb */
	{ "095-epr.ep1",  0x080000, 0xbe0060a3, 0 | BRF_ESS | BRF_PRG }, // 15 68k code 		/ M27C4002
	/* P is on eprom, correct chip label unknown */
};

STDROMPICKEXT(rbff1a, rbff1a, neogeo)
STD_ROM_FN(rbff1a)

static void rbff1aCallback()
{
	BurnLoadRom(Neo68KROMActive, 15, 1);
}

static INT32 rbff1aInit()
{
	NeoCallbackActive->pInitialise = rbff1aCallback;

 	return NeoInit();
}

struct BurnDriver BurnDrvrbff1a = {
	"rbff1a", "rbff1", "neogeo", NULL, "1995",
	"Real Bout Fatal Fury / Real Bout Garou Densetsu (bug fix revision)\0", NULL, "SNK", "Neo Geo MVS",
	L"Real Bout Fatal Fury\0Real Bout \u9913\u72FC\u4F1D\u8AAC (bug fix revision)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbff1aRomInfo, rbff1aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	rbff1aInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Real Bout Fatal Fury / Real Bout Garou Densetsu (Korean release)
/* KOREAN VERSION */

static struct BurnRomInfo rbff1kRomDesc[] = {
	{ "095-p1k.p1",   0x100000, 0xf705364b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200
	{ "095-p2.sp2",   0x200000, 0xcc15826e, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "095-s1.s1",    0x020000, 0xb6bf5e08, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "069-c1.c1",    0x400000, 0xe302f93c, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "069-c2.c2",    0x400000, 0x1053a455, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "069-c3.c3",    0x400000, 0x1c0fde2f, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "069-c4.c4",    0x400000, 0xa25fc3d0, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "095-c5.c5",    0x400000, 0x8b9b65df, 3 | BRF_GRA },           //  7 					/ TC5332202
	{ "095-c6.c6",    0x400000, 0x3e164718, 3 | BRF_GRA },           //  8 					/ TC5332202
	{ "095-c7.c7",    0x200000, 0xca605e12, 3 | BRF_GRA },           //  9 					/ TC5316200
	{ "095-c8.c8",    0x200000, 0x4e6beb6c, 3 | BRF_GRA },           // 10 					/ TC5316200

	{ "095-m1.m1",    0x020000, 0x653492a7, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "069-v1.v1",    0x400000, 0x2bdbd4db, 5 | BRF_SND },           // 12 Sound data		/ TC5332204
	{ "069-v2.v2",    0x400000, 0xa698a487, 5 | BRF_SND },           // 13 					/ TC5332204
	{ "095-v3.v3",    0x400000, 0x189d1c6c, 5 | BRF_SND },           // 14 					/ TC5332201
};

STDROMPICKEXT(rbff1k, rbff1k, neogeo)
STD_ROM_FN(rbff1k)

struct BurnDriver BurnDrvrbff1k = {
	"rbff1k", "rbff1", "neogeo", NULL, "1995",
	"Real Bout Fatal Fury / Real Bout Garou Densetsu (Korean release)\0", NULL, "SNK", "Neo Geo MVS",
	L"Real Bout Fatal Fury\0Real Bout \u9913\u72FC\u4F1D\u8AAC (Korean release)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbff1kRomInfo, rbff1kRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Real Bout Fatal Fury / Real Bout Garou Densetsu (Korean release, bug fix revision)
/* KOREAN VERSION */

static struct BurnRomInfo rbff1kaRomDesc[] = {
	{ "095-p1k.p1",   0x100000, 0xf705364b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200
	{ "095-p2.sp2",   0x200000, 0xcc15826e, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "095-s1.s1",    0x020000, 0xb6bf5e08, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "069-c1.c1",    0x400000, 0xe302f93c, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "069-c2.c2",    0x400000, 0x1053a455, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "069-c3.c3",    0x400000, 0x1c0fde2f, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "069-c4.c4",    0x400000, 0xa25fc3d0, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "095-c5.c5",    0x400000, 0x8b9b65df, 3 | BRF_GRA },           //  7 					/ TC5332202
	{ "095-c6.c6",    0x400000, 0x3e164718, 3 | BRF_GRA },           //  8 					/ TC5332202
	{ "095-c7.c7",    0x200000, 0xca605e12, 3 | BRF_GRA },           //  9 					/ TC5316200
	{ "095-c8.c8",    0x200000, 0x4e6beb6c, 3 | BRF_GRA },           // 10 					/ TC5316200

	{ "095-m1.m1",    0x020000, 0x653492a7, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "069-v1.v1",    0x400000, 0x2bdbd4db, 5 | BRF_SND },           // 12 Sound data		/ TC5332204
	{ "069-v2.v2",    0x400000, 0xa698a487, 5 | BRF_SND },           // 13 					/ TC5332204
	{ "095-v3.v3",    0x400000, 0x189d1c6c, 5 | BRF_SND },           // 14 					/ TC5332201

	/* the rom below acts as a patch to the program rom in the cart, replacing the first 512kb */
	{ "095-eprk.ep1", 0x080000, 0x422093ed, 1 | BRF_ESS | BRF_PRG }, // 15 					/ M27C4002
	/* no label on eprom, correct chip label unknown */
};

STDROMPICKEXT(rbff1ka, rbff1ka, neogeo)
STD_ROM_FN(rbff1ka)

struct BurnDriver BurnDrvrbff1ka = {
	"rbff1ka", "rbff1", "neogeo", NULL, "1995",
	"Real Bout Fatal Fury / Real Bout Garou Densetsu (Korean release, bug fix revision)\0", NULL, "SNK", "Neo Geo MVS",
	L"Real Bout Fatal Fury\0Real Bout \u9913\u72FC\u4F1D\u8AAC (Korean release, bug fix revision)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbff1kaRomInfo, rbff1kaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	rbff1aInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Art of Fighting 3 - The Path of the Warrior / Art of Fighting - Ryuuko no Ken Gaiden

static struct BurnRomInfo aof3RomDesc[] = {
	{ "096-p1.p1",    0x100000, 0x9edb420d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200
	{ "096-p2.sp2",   0x200000, 0x4d5a2602, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "096-s1.s1",    0x020000, 0xcc7fd344, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "096-c1.c1",    0x400000, 0xf17b8d89, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "096-c2.c2",    0x400000, 0x3840c508, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "096-c3.c3",    0x400000, 0x55f9ee1e, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "096-c4.c4",    0x400000, 0x585b7e47, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "096-c5.c5",    0x400000, 0xc75a753c, 3 | BRF_GRA },           //  7 					/ TC5332205
	{ "096-c6.c6",    0x400000, 0x9a9d2f7a, 3 | BRF_GRA },           //  8 					/ TC5332205
	{ "096-c7.c7",    0x200000, 0x51bd8ab2, 3 | BRF_GRA },           //  9 					/ TC5316200
	{ "096-c8.c8",    0x200000, 0x9a34f99c, 3 | BRF_GRA },           // 10 					/ TC5316200

	{ "096-m1.m1",    0x020000, 0xcb07b659, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "096-v1.v1",    0x200000, 0xe2c32074, 5 | BRF_SND },           // 12 Sound data		/ TC5316200
	{ "096-v2.v2",    0x200000, 0xa290eee7, 5 | BRF_SND },           // 13 					/ TC5316200
	{ "096-v3.v3",    0x200000, 0x199d12ea, 5 | BRF_SND },           // 14 					/ TC5316200
};

STDROMPICKEXT(aof3, aof3, neogeo)
STD_ROM_FN(aof3)

struct BurnDriver BurnDrvAof3 = {
	"aof3", NULL, "neogeo", NULL, "1996",
	"Art of Fighting 3 - The Path of the Warrior / Art of Fighting - Ryuuko no Ken Gaiden\0", NULL, "SNK", "Neo Geo MVS",
	L"Art of Fighting 3 - The Path of the Warrior\0Art of Fighting - \u9F8D\u864E\u306E\u62F3\u5916\u4F1D\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, aof3RomInfo, aof3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Art of Fighting 3 - The Path of the Warrior (Korean release)
/* KOREAN VERSION */

static struct BurnRomInfo aof3kRomDesc[] = {
	{ "196-p1k.p1",   0x100000, 0xa0780789, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "096-p2.sp2",   0x200000, 0x4d5a2602, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "096-s1.s1",    0x020000, 0xcc7fd344, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "096-c1.c1",    0x400000, 0xf17b8d89, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "096-c2.c2",    0x400000, 0x3840c508, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "096-c3.c3",    0x400000, 0x55f9ee1e, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "096-c4.c4",    0x400000, 0x585b7e47, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "096-c5.c5",    0x400000, 0xc75a753c, 3 | BRF_GRA },           //  7 					/ TC5332205
	{ "096-c6.c6",    0x400000, 0x9a9d2f7a, 3 | BRF_GRA },           //  8 					/ TC5332205
	{ "096-c7.c7",    0x200000, 0x51bd8ab2, 3 | BRF_GRA },           //  9 					/ TC5316200
	{ "096-c8.c8",    0x200000, 0x9a34f99c, 3 | BRF_GRA },           // 10 					/ TC5316200

	{ "096-m1.m1",    0x020000, 0xcb07b659, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "096-v1.v1",    0x200000, 0xe2c32074, 5 | BRF_SND },           // 12 Sound data		/ TC5316200
	{ "096-v2.v2",    0x200000, 0xa290eee7, 5 | BRF_SND },           // 13 					/ TC5316200
	{ "096-v3.v3",    0x200000, 0x199d12ea, 5 | BRF_SND },           // 14 					/ TC5316200
};

STDROMPICKEXT(aof3k, aof3k, neogeo)
STD_ROM_FN(aof3k)

struct BurnDriver BurnDrvAof3k = {
	"aof3k", "aof3", "neogeo", NULL, "1996",
	"Art of Fighting 3 - The Path of the Warrior (Korean release)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, aof3kRomInfo, aof3kRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '96 (NGM-214)
/* MVS VERSION */

static struct BurnRomInfo kof96RomDesc[] = {
	{ "214-p1.p1",    0x100000, 0x52755d74, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200
	{ "214-p2.sp2",   0x200000, 0x002ccb73, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "214-s1.s1",    0x020000, 0x1254cbdb, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "214-c1.c1",    0x400000, 0x7ecf4aa2, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "214-c2.c2",    0x400000, 0x05b54f37, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "214-c3.c3",    0x400000, 0x64989a65, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "214-c4.c4",    0x400000, 0xafbea515, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "214-c5.c5",    0x400000, 0x2a3bbd26, 3 | BRF_GRA },           //  7 					/ TC5332205
	{ "214-c6.c6",    0x400000, 0x44d30dc7, 3 | BRF_GRA },           //  8 					/ TC5332205
	{ "214-c7.c7",    0x400000, 0x3687331b, 3 | BRF_GRA },           //  9 					/ TC5332205
	{ "214-c8.c8",    0x400000, 0xfa1461ad, 3 | BRF_GRA },           // 10 					/ TC5332205

	{ "214-m1.m1",    0x020000, 0xdabc427c, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "214-v1.v1",    0x400000, 0x63f7b045, 5 | BRF_SND },           // 12 Sound data		/ TC5332204
	{ "214-v2.v2",    0x400000, 0x25929059, 5 | BRF_SND },           // 13 					/ TC5332204
	{ "214-v3.v3",    0x200000, 0x92a2257d, 5 | BRF_SND },           // 14 					/ TC5316200
};

STDROMPICKEXT(kof96, kof96, neogeo)
STD_ROM_FN(kof96)

struct BurnDriver BurnDrvKof96 = {
	"kof96", NULL, "neogeo", NULL, "1996",
	"The King of Fighters '96 (NGM-214)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO,  GBF_VSFIGHT, FBF_KOF,
	NULL, kof96RomInfo, kof96RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '96 (NGH-214)
/* AES VERSION */

static struct BurnRomInfo kof96hRomDesc[] = {
	{ "214-pg1.p1",   0x100000, 0xbd3757c9, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "214-p2.sp2",   0x200000, 0x002ccb73, 1 | BRF_ESS | BRF_PRG }, //  1 					/ mask rom TC5316200

	{ "214-s1.s1",    0x020000, 0x1254cbdb, 2 | BRF_GRA },           //  2 Text layer tiles / mask rom TC531000

	{ "214-c1.c1",    0x400000, 0x7ecf4aa2, 3 | BRF_GRA },           //  3 Sprite data		/ mask rom TC5332205
	{ "214-c2.c2",    0x400000, 0x05b54f37, 3 | BRF_GRA },           //  4 					/ mask rom TC5332205
	{ "214-c3.c3",    0x400000, 0x64989a65, 3 | BRF_GRA },           //  5 					/ mask rom TC5332205
	{ "214-c4.c4",    0x400000, 0xafbea515, 3 | BRF_GRA },           //  6 					/ mask rom TC5332205
	{ "214-c5.c5",    0x400000, 0x2a3bbd26, 3 | BRF_GRA },           //  7 					/ mask rom TC5332205
	{ "214-c6.c6",    0x400000, 0x44d30dc7, 3 | BRF_GRA },           //  8 					/ mask rom TC5332205
	{ "214-c7.c7",    0x400000, 0x3687331b, 3 | BRF_GRA },           //  9 					/ mask rom TC5332205
	{ "214-c8.c8",    0x400000, 0xfa1461ad, 3 | BRF_GRA },           // 10 					/ mask rom TC5332205

	{ "214-m1.m1",    0x020000, 0xdabc427c, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ mask rom TC531001

	{ "214-v1.v1",    0x400000, 0x63f7b045, 5 | BRF_SND },           // 12 Sound data		/ mask rom TC5332204
	{ "214-v2.v2",    0x400000, 0x25929059, 5 | BRF_SND },           // 13 					/ mask rom TC5332204
	{ "214-v3.v3",    0x200000, 0x92a2257d, 5 | BRF_SND },           // 14 					/ mask rom TC5316200
};

STDROMPICKEXT(kof96h, kof96h, neogeo)
STD_ROM_FN(kof96h)

struct BurnDriver BurnDrvKof96h = {
	"kof96h", "kof96", "neogeo", NULL, "1996",
	"The King of Fighters '96 (NGH-214)\0", NULL, "SNK", "Neo Geo AES",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO,  GBF_VSFIGHT, FBF_KOF,
	NULL, kof96hRomInfo, kof96hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The Ultimate 11 - The SNK Football Championship / Tokuten Ou - Honoo no Libero

static struct BurnRomInfo ssideki4RomDesc[] = {
	{ "215-p1.p1",    0x200000, 0x519b4ba3, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/* TC5316200 */

	{ "215-s1.s1",    0x020000, 0xf0fe5c36, 2 | BRF_GRA },           //  1 Text layer tiles /* TC531000 */

	{ "215-c1.c1",    0x400000, 0x8ff444f5, 3 | BRF_GRA },           //  2 Sprite data		/* TC5332205 */
	{ "215-c2.c2",    0x400000, 0x5b155037, 3 | BRF_GRA },           //  3 					/* TC5332205 */
	{ "215-c3.c3",    0x400000, 0x456a073a, 3 | BRF_GRA },           //  4 					/* TC5332205 */
	{ "215-c4.c4",    0x400000, 0x43c182e1, 3 | BRF_GRA },           //  5 					/* TC5332205 */
	{ "215-c5.c5",    0x200000, 0x0c6f97ec, 3 | BRF_GRA },           //  6 					/* TC5332205 */
	{ "215-c6.c6",    0x200000, 0x329c5e1b, 3 | BRF_GRA },           //  7 					/* TC5332205 */

	{ "215-m1.m1",    0x020000, 0xa932081d, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code			/* TC531001 */

	{ "215-v1.v1",    0x400000, 0x877d1409, 5 | BRF_SND },           //  9 Sound data		/* TC5332204 */
	{ "215-v2.v2",    0x200000, 0x1bfa218b, 5 | BRF_SND },           // 10 					/* TC5316200 */
};

STDROMPICKEXT(ssideki4, ssideki4, neogeo)
STD_ROM_FN(ssideki4)

struct BurnDriver BurnDrvSsideki4 = {
	"ssideki4", NULL, "neogeo", NULL, "1996",
	"The Ultimate 11 - The SNK Football Championship / Tokuten Ou - Honoo no Libero\0", NULL, "SNK", "Neo Geo MVS",
	L"The Ultimate 11 - SNK Football Championship\0\u5F97\u70B9\u738B - \u708E\u306E\u30EA\u30D9\u30ED\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_SPORTSFOOTBALL, 0,
	NULL, ssideki4RomInfo, ssideki4RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Kizuna Encounter - Super Tag Battle / Fu'un Super Tag Battle

static struct BurnRomInfo kizunaRomDesc[] = {
	{ "216-p1.p1",    0x200000, 0x75d2b3de, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/* mask rom TC5316200 */

	{ "216-s1.s1",    0x020000, 0xefdc72d7, 2 | BRF_GRA },           //  1 Text layer tiles /* mask rom TC531000 */

	{ "059-c1.c1",    0x200000, 0x763ba611, 3 | BRF_GRA },           //  2 Sprite data		/* TC5316200 */
	{ "059-c2.c2",    0x200000, 0xe05e8ca6, 3 | BRF_GRA },           //  3 					/* TC5316200 */
	{ "216-c3.c3",    0x400000, 0x665c9f16, 3 | BRF_GRA },           //  4 					/* mask rom TC5332205 */
	{ "216-c4.c4",    0x400000, 0x7f5d03db, 3 | BRF_GRA },           //  5 					/* mask rom TC5332205 */
	{ "059-c5.c5",    0x200000, 0x59013f9e, 3 | BRF_GRA },           //  6 					/* TC5316200 */
	{ "059-c6.c6",    0x200000, 0x1c8d5def, 3 | BRF_GRA },           //  7 					/* TC5316200 */
	{ "059-c7.c7",    0x200000, 0xc88f7035, 3 | BRF_GRA },           //  8 					/* TC538200 */
	{ "059-c8.c8",    0x200000, 0x484ce3ba, 3 | BRF_GRA },           //  9 					/* TC538200 */

	{ "216-m1.m1",    0x020000, 0x1b096820, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "059-v1.v1",    0x200000, 0x530c50fd, 5 | BRF_SND },           // 11 Sound data		/* TC5316200 */
	{ "216-v2.v2",    0x200000, 0x03667a8d, 5 | BRF_SND },           // 12 					/* mask rom TC5316200 */
	{ "059-v3.v3",    0x200000, 0x7038c2f9, 5 | BRF_SND },           // 13 					/* TC5316200 */
	{ "216-v4.v4",    0x200000, 0x31b99bd6, 5 | BRF_SND },           // 14 					/* mask rom TC5316200 */
};

STDROMPICKEXT(kizuna, kizuna, neogeo)
STD_ROM_FN(kizuna)

struct BurnDriver BurnDrvKizuna = {
	"kizuna", NULL, "neogeo", NULL, "1996",
	"Kizuna Encounter - Super Tag Battle / Fu'un Super Tag Battle\0", NULL, "SNK", "Neo Geo MVS",
	L"Kizuna Encounter - Super Tag Battle\0\u98A8\u96F2 Super Tag Battle\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, kizunaRomInfo, kizunaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Samurai Shodown IV - Amakusa's Revenge / Samurai Spirits - Amakusa Kourin (NGM-222)(NGH-222)
/* MVS AND AES VERSION */

static struct BurnRomInfo samsho4RomDesc[] = {
	{ "222-p1.p1",    0x100000, 0x1a5cb56d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/* TC538200 */
	{ "222-p2.sp2",   0x400000, 0xb023cd8b, 1 | BRF_ESS | BRF_PRG }, //  1 					/* TC5332205 */

	{ "222-s1.s1",    0x020000, 0x8d3d3bf9, 2 | BRF_GRA },           //  2 Text layer tiles /* TC531000 */

	{ "222-c1.c1",    0x400000, 0x68f2ed95, 3 | BRF_GRA },           //  3 Sprite data		/* TC5332205 */
	{ "222-c2.c2",    0x400000, 0xa6e9aff0, 3 | BRF_GRA },           //  4 					/* TC5332205 */
	{ "222-c3.c3",    0x400000, 0xc91b40f4, 3 | BRF_GRA },           //  5 					/* TC5332205 */
	{ "222-c4.c4",    0x400000, 0x359510a4, 3 | BRF_GRA },           //  6 					/* TC5332205 */
	{ "222-c5.c5",    0x400000, 0x9cfbb22d, 3 | BRF_GRA },           //  7 					/* TC5332205 */
	{ "222-c6.c6",    0x400000, 0x685efc32, 3 | BRF_GRA },           //  8 					/* TC5332205 */
	{ "222-c7.c7",    0x400000, 0xd0f86f0d, 3 | BRF_GRA },           //  9 					/* TC5332205 */
	{ "222-c8.c8",    0x400000, 0xadfc50e3, 3 | BRF_GRA },           // 10 					/* TC5332205 */

	{ "222-m1.m1",    0x020000, 0x7615bc1b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/* TC531001 */

	{ "222-v1.v1",    0x400000, 0x7d6ba95f, 5 | BRF_SND },           // 12 Sound data		/* TC5332204 */
	{ "222-v2.v2",    0x400000, 0x6c33bb5d, 5 | BRF_SND },           // 13 					/* TC5332204 */
	{ "222-v3.v3",    0x200000, 0x831ea8c0, 5 | BRF_SND },           // 14 					/* TC5316200 */
};

STDROMPICKEXT(samsho4, samsho4, neogeo)
STD_ROM_FN(samsho4)

struct BurnDriver BurnDrvSamSho4 = {
	"samsho4", NULL, "neogeo", NULL, "1996",
	"Samurai Shodown IV - Amakusa's Revenge / Samurai Spirits - Amakusa Kourin (NGM-222)(NGH-222)\0", NULL, "SNK", "Neo Geo MVS",
	L"Samurai Shodown IV - Amakusa's Revenge\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4 - \u5929\u8349\u964D\u81E8 (NGM-222)(NGH-222)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho4RomInfo, samsho4RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Pae Wang Jeon Seol / Legend of a Warrior (Korean censored Samurai Shodown IV)
/* KOREAN VERSION */

static struct BurnRomInfo samsho4kRomDesc[] = {
	{ "222-p1k.p1",   0x100000, 0x06e0a25d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "222-p2.sp2",   0x400000, 0xb023cd8b, 1 | BRF_ESS | BRF_PRG }, //  1 					/* TC5332205 */

	{ "222-s1k.s1",   0x020000, 0xd313687d, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "222-c1.c1",    0x400000, 0x68f2ed95, 3 | BRF_GRA },           //  3 Sprite data		/* TC5332205 */
	{ "222-c2.c2",    0x400000, 0xa6e9aff0, 3 | BRF_GRA },           //  4 					/* TC5332205 */
	{ "222-c3.c3",    0x400000, 0xc91b40f4, 3 | BRF_GRA },           //  5 					/* TC5332205 */
	{ "222-c4.c4",    0x400000, 0x359510a4, 3 | BRF_GRA },           //  6 					/* TC5332205 */
	{ "222-c5.c5",    0x400000, 0x9cfbb22d, 3 | BRF_GRA },           //  7 					/* TC5332205 */
	{ "222-c6.c6",    0x400000, 0x685efc32, 3 | BRF_GRA },           //  8 					/* TC5332205 */
	{ "222-c7.c7",    0x400000, 0xd0f86f0d, 3 | BRF_GRA },           //  9 					/* TC5332205 */
	{ "222-c8.c8",    0x400000, 0xadfc50e3, 3 | BRF_GRA },           // 10 					/* TC5332205 */

	{ "222-m1.m1",    0x020000, 0x7615bc1b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/* TC531001 */

	{ "222-v1.v1",    0x400000, 0x7d6ba95f, 5 | BRF_SND },           // 12 Sound data		/* TC5332204 */
	{ "222-v2.v2",    0x400000, 0x6c33bb5d, 5 | BRF_SND },           // 13 					/* TC5332204 */
	{ "222-v3.v3",    0x200000, 0x831ea8c0, 5 | BRF_SND },           // 14 					/* TC5316200 */
};

STDROMPICKEXT(samsho4k, samsho4k, neogeo)
STD_ROM_FN(samsho4k)

struct BurnDriver BurnDrvSamSho4k = {
	"samsho4k", "samsho4", "neogeo", NULL, "1996",
	"Pae Wang Jeon Seol / Legend of a Warrior (Korean censored Samurai Shodown IV)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho4kRomInfo, samsho4kRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Real Bout Fatal Fury Special / Real Bout Garou Densetsu Special

static struct BurnRomInfo rbffspecRomDesc[] = {
	{ "223-p1.p1",    0x100000, 0xf84a2d1d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "223-p2.sp2",   0x400000, 0xaddd8f08, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "223-s1.s1",    0x020000, 0x7ecd6e8c, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "223-c1.c1",    0x400000, 0xebab05e2, 3 | BRF_GRA },           //  3 Sprite data
	{ "223-c2.c2",    0x400000, 0x641868c3, 3 | BRF_GRA },           //  4
	{ "223-c3.c3",    0x400000, 0xca00191f, 3 | BRF_GRA },           //  5
	{ "223-c4.c4",    0x400000, 0x1f23d860, 3 | BRF_GRA },           //  6
	{ "223-c5.c5",    0x400000, 0x321e362c, 3 | BRF_GRA },           //  7
	{ "223-c6.c6",    0x400000, 0xd8fcef90, 3 | BRF_GRA },           //  8
	{ "223-c7.c7",    0x400000, 0xbc80dd2d, 3 | BRF_GRA },           //  9
	{ "223-c8.c8",    0x400000, 0x5ad62102, 3 | BRF_GRA },           // 10

	{ "223-m1.m1",    0x020000, 0x3fee46bf, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "223-v1.v1",    0x400000, 0x76673869, 5 | BRF_SND },           // 12 Sound data
	{ "223-v2.v2",    0x400000, 0x7a275acd, 5 | BRF_SND },           // 13
	{ "223-v3.v3",    0x400000, 0x5a797fd2, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(rbffspec, rbffspec, neogeo)
STD_ROM_FN(rbffspec)

struct BurnDriver BurnDrvrbffspec = {
	"rbffspec", NULL, "neogeo", NULL, "1996",
	"Real Bout Fatal Fury Special / Real Bout Garou Densetsu Special\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbffspecRomInfo, rbffspecRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Real Bout Fatal Fury Special / Real Bout Garou Densetsu Special (Korean release)
/* KOREAN VERSION */

static struct BurnRomInfo rbffspeckRomDesc[] = {
	{ "223-p1k.p1",   0x100000, 0xb78c8391, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	/* Chip label is correct. They used Cart ID 0124 as 0123 was already used by quizdaisk */
	{ "223-p2.sp2",   0x400000, 0xaddd8f08, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "223-s1.s1",    0x020000, 0x7ecd6e8c, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "223-c1.c1",    0x400000, 0xebab05e2, 3 | BRF_GRA },           //  3 Sprite data
	{ "223-c2.c2",    0x400000, 0x641868c3, 3 | BRF_GRA },           //  4
	{ "223-c3.c3",    0x400000, 0xca00191f, 3 | BRF_GRA },           //  5
	{ "223-c4.c4",    0x400000, 0x1f23d860, 3 | BRF_GRA },           //  6
	{ "223-c5.c5",    0x400000, 0x321e362c, 3 | BRF_GRA },           //  7
	{ "223-c6.c6",    0x400000, 0xd8fcef90, 3 | BRF_GRA },           //  8
	{ "223-c7.c7",    0x400000, 0xbc80dd2d, 3 | BRF_GRA },           //  9
	{ "223-c8.c8",    0x400000, 0x5ad62102, 3 | BRF_GRA },           // 10

	{ "223-m1.m1",    0x020000, 0x3fee46bf, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "223-v1.v1",    0x400000, 0x76673869, 5 | BRF_SND },           // 12 Sound data
	{ "223-v2.v2",    0x400000, 0x7a275acd, 5 | BRF_SND },           // 13
	{ "223-v3.v3",    0x400000, 0x5a797fd2, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(rbffspeck, rbffspeck, neogeo)
STD_ROM_FN(rbffspeck)

struct BurnDriver BurnDrvrbffspeck = {
	"rbffspeck", "rbffspec", "neogeo", NULL, "1996",
	"Real Bout Fatal Fury Special / Real Bout Garou Densetsu Special (Korean release)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbffspeckRomInfo, rbffspeckRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// The King of Fighters '97 (NGM-2320)
/* MVS VERSION */

static struct BurnRomInfo kof97RomDesc[] = {
	{ "232-p1.p1",    0x100000, 0x7db81ad9, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "232-p2.sp2",   0x400000, 0x158b23f6, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "232-s1.s1",    0x020000, 0x8514ecf5, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "232-c1.c1",    0x800000, 0x5f8bf0a1, 3 | BRF_GRA },           //  3 Sprite data
	{ "232-c2.c2",    0x800000, 0xe4d45c81, 3 | BRF_GRA },           //  4
	{ "232-c3.c3",    0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.c4",    0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
	{ "232-c5.c5",    0x400000, 0x34fc4e51, 3 | BRF_GRA },           //  7
	{ "232-c6.c6",    0x400000, 0x4ff4d47b, 3 | BRF_GRA },           //  8

	{ "232-m1.m1",    0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.v1",    0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.v2",    0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.v3",    0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97, kof97, neogeo)
STD_ROM_FN(kof97)

struct BurnDriver BurnDrvKof97 = {
	"kof97", NULL, "neogeo", NULL, "1997",
	"The King of Fighters '97 (NGM-2320)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97RomInfo, kof97RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '97 (NGH-2320)
/* AES VERSION */

static struct BurnRomInfo kof97hRomDesc[] = {
	{ "232-pg1.p1",   0x100000, 0x5c2400b7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "232-p2.sp2",   0x400000, 0x158b23f6, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "232-s1.s1",    0x020000, 0x8514ecf5, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "232-c1.c1",    0x800000, 0x5f8bf0a1, 3 | BRF_GRA },           //  3 Sprite data
	{ "232-c2.c2",    0x800000, 0xe4d45c81, 3 | BRF_GRA },           //  4
	{ "232-c3.c3",    0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.c4",    0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
	{ "232-c5.c5",    0x400000, 0x34fc4e51, 3 | BRF_GRA },           //  7
	{ "232-c6.c6",    0x400000, 0x4ff4d47b, 3 | BRF_GRA },           //  8

	{ "232-m1.m1",    0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.v1",    0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.v2",    0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.v3",    0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97h, kof97h, neogeo)
STD_ROM_FN(kof97h)

struct BurnDriver BurnDrvKof97h = {
	"kof97h", "kof97", "neogeo", NULL, "1997",
	"The King of Fighters '97 (NGH-2320)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97hRomInfo, kof97hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '97 (Korean release)
/* KOREAN VERSION */

static struct BurnRomInfo kof97kRomDesc[] = {
	{ "232-p1.p1",    0x100000, 0x7db81ad9, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "232-p2.sp2",   0x400000, 0x158b23f6, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "232-s1.s1",    0x020000, 0x8514ecf5, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "232-c1.c1",    0x800000, 0x5f8bf0a1, 3 | BRF_GRA },           //  3 Sprite data
	{ "232-c2.c2",    0x800000, 0xe4d45c81, 3 | BRF_GRA },           //  4
	{ "232-c3.c3",    0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.c4",    0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
	{ "232-c5.c5",    0x400000, 0x34fc4e51, 3 | BRF_GRA },           //  7
	{ "232-c6.c6",    0x400000, 0x4ff4d47b, 3 | BRF_GRA },           //  8

	{ "232-m1k.m1",   0x020000, 0xbbea9070, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.v1",    0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.v2",    0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.v3",    0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97k, kof97k, neogeo)
STD_ROM_FN(kof97k)

struct BurnDriver BurnDrvKof97k = {
	"kof97k", "kof97", "neogeo", NULL, "1997",
	"The King of Fighters '97 (Korean release)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97kRomInfo, kof97kRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '97 Plus (bootleg)

static struct BurnRomInfo kof97plsRomDesc[] = {
	{ "kf97-p1p.bin", 0x100000, 0xc01fda46, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "kf97-p2p.bin", 0x400000, 0x5502b020, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "kf97-s1p.bin", 0x020000, 0x73254270, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "232-c1.c1",    0x800000, 0x5f8bf0a1, 3 | BRF_GRA },           //  3 Sprite data
	{ "232-c2.c2",    0x800000, 0xe4d45c81, 3 | BRF_GRA },           //  4
	{ "232-c3.c3",    0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.c4",    0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
	{ "232-c5.c5",    0x400000, 0x34fc4e51, 3 | BRF_GRA },           //  7
	{ "232-c6.c6",    0x400000, 0x4ff4d47b, 3 | BRF_GRA },           //  8

	{ "232-m1.m1",    0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.v1",    0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.v2",    0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.v3",    0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97pls, kof97pls, neogeo)
STD_ROM_FN(kof97pls)

struct BurnDriver BurnDrvKof97pls = {
	"kof97pls", "kof97", "neogeo", NULL, "1997",
	"The King of Fighters '97 Plus (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97plsRomInfo, kof97plsRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// King of Gladiator (The King of Fighters '97 bootleg)

static struct BurnRomInfo kogRomDesc[] = {
	{ "5232-p1.bin",  0x200000, 0xd2413ec6, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "232-p2.sp2",   0x400000, 0x158b23f6, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "5232-s1.bin",  0x020000, 0x0bef69da, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "5232-c1a.bin", 0x800000, 0x4eab9b0a, 3 | BRF_GRA },           //  3 Sprite data
	{ "5232-c2a.bin", 0x800000, 0x697f8fd0, 3 | BRF_GRA },           //  4
	{ "5232-c1b.bin", 0x800000, 0x1143fdf3, 3 | BRF_GRA },           //  5
	{ "5232-c2b.bin", 0x800000, 0xea82cf8f, 3 | BRF_GRA },           //  6
	{ "5232-c3.bin",  0x400000, 0xabd1be07, 3 | BRF_GRA },           //  7
	{ "5232-c4.bin",  0x400000, 0xd2bd967b, 3 | BRF_GRA },           //  8

	{ "232-m1.m1",    0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.v1",    0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.v2",    0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.v3",    0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kog, kog, neogeo)
STD_ROM_FN(kog)

// This is actually set by a jumper on the PCB
UINT16 __fastcall KogReadWord(UINT32)
{
	extern INT32 nBIOS;
	if (nBIOS == 5 || nBIOS == 8 || nBIOS == 9 || nBIOS == 10 || nBIOS == 13 || nBIOS == 15) {
		return 0xff00;
	} else {
		return 0xff01;
	}
}

static void kogCallback()
{
	INT32 i;
	UINT8 *dst = (UINT8 *)BurnMalloc( 0x100000 );

	if (dst)
	{
		static const INT32 sec[] = { 0x3, 0x8, 0x7, 0xc, 0x1, 0xa, 0x6, 0xd };

		for (i = 0; i < 0x100000 / 0x020000; i++)
			memcpy (dst + i * 0x020000, Neo68KROMActive + sec[i] * 0x020000, 0x020000);

		memcpy (dst + 0x090000, Neo68KROMActive + 0x040000, 0x004000);
		memcpy (Neo68KROMActive, dst, 0x100000);
		BurnFree (dst);
	}

	for (i = 0x90000; i < 0x94000; i+=2) {
		if ((*((UINT16 *)(Neo68KROMActive + i + 0)) & BURN_ENDIAN_SWAP_INT16(0xf2bf)) == BURN_ENDIAN_SWAP_INT16(0x42b9) && *((UINT16 *)(Neo68KROMActive + i + 2)) == 0)
			*((UINT16 *)(Neo68KROMActive + i + 2)) = BURN_ENDIAN_SWAP_INT16(0x0009);

		if (*((UINT16 *)(Neo68KROMActive + i + 0)) == BURN_ENDIAN_SWAP_INT16(0x4eb8)) {
			*((UINT16 *)(Neo68KROMActive + i + 0))  = BURN_ENDIAN_SWAP_INT16(0x6100);
			*((UINT16 *)(Neo68KROMActive + i + 2)) += BURN_ENDIAN_SWAP_INT16(0xfffe - (i & 0xfffe));
		}
	}

	memcpy (Neo68KROMActive + 0x0007a6, Neo68KROMActive + 0x0907a6, 0x000006);
	memcpy (Neo68KROMActive + 0x0007c6, Neo68KROMActive + 0x0907c6, 0x000006);
	memcpy (Neo68KROMActive + 0x0007e6, Neo68KROMActive + 0x0907e6, 0x000006);
	memmove (Neo68KROMActive + 0x100000, Neo68KROMActive + 0x200000, 0x400000);

	*((UINT16 *)(Neo68KROMActive + 0x924ac)) = BURN_ENDIAN_SWAP_INT16(0x0009);
	*((UINT16 *)(Neo68KROMActive + 0x9251c)) = BURN_ENDIAN_SWAP_INT16(0x0009);

	lans2004_sx_decode();
	lans2004_cx_decode(0x800000 * 5);
}

static void kogInstallHandlers()
{
	// Install jumper that controls title screen language
	SekMapHandler(6, 0x0FFFFE, 0x0FFFFF, MAP_READ);
	SekSetReadWordHandler(6, KogReadWord);
}

static INT32 kogInit()
{
 	INT32 nRet;

	nBurnCPUSpeedAdjust = 0x010a;	// fix garbage on intro
	NeoCallbackActive->pInitialise = kogCallback;
	NeoCallbackActive->pInstallHandlers = kogInstallHandlers;

 	nRet = NeoInit();

	return nRet;
}

struct BurnDriver BurnDrvKog = {
	"kog", "kof97", "neogeo", NULL, "1997",
	"King of Gladiator (The King of Fighters '97 bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kogRomInfo, kogRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kogInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// King of Gladiator Plus (The King of Fighters '97 bootleg)

static struct BurnRomInfo kogplusRomDesc[] = {
	{ "5232-p1p.bin", 0x200000, 0x0338f453, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "5232-p2p.bin", 0x400000, 0x5502b020, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "5232-s1p.bin", 0x020000, 0x6271e9ba, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "5232-c1a.bin", 0x800000, 0x4eab9b0a, 3 | BRF_GRA },           //  3 Sprite data
	{ "5232-c2a.bin", 0x800000, 0x697f8fd0, 3 | BRF_GRA },           //  4
	{ "5232-c1b.bin", 0x800000, 0x1143fdf3, 3 | BRF_GRA },           //  5
	{ "5232-c2b.bin", 0x800000, 0xea82cf8f, 3 | BRF_GRA },           //  6
	{ "5232-c3.bin",  0x400000, 0xabd1be07, 3 | BRF_GRA },           //  7
	{ "5232-c4.bin",  0x400000, 0xd2bd967b, 3 | BRF_GRA },           //  8

	{ "232-m1.m1",    0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.v1",    0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.v2",    0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.v3",    0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kogplus, kogplus, neogeo)
STD_ROM_FN(kogplus)

struct BurnDriver BurnDrvKogplus = {
	"kogplus", "kof97", "neogeo", NULL, "1997",
	"King of Gladiator Plus (The King of Fighters '97 bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kogplusRomInfo, kogplusRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kogInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The Last Blade / Bakumatsu Roman - Gekka no Kenshi (NGM-2340)
/* MVS VERSION */

static struct BurnRomInfo lastbladRomDesc[] = {
	{ "234-p1.p1",    0x100000, 0xe123a5a3, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "234-p2.sp2",   0x400000, 0x0fdc289e, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "234-s1.s1",    0x020000, 0x95561412, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "234-c1.c1",    0x800000, 0x9f7e2bd3, 3 | BRF_GRA },           //  3 Sprite data
	{ "234-c2.c2",    0x800000, 0x80623d3c, 3 | BRF_GRA },           //  4
	{ "234-c3.c3",    0x800000, 0x91ab1a30, 3 | BRF_GRA },           //  5
	{ "234-c4.c4",    0x800000, 0x3d60b037, 3 | BRF_GRA },           //  6
	{ "234-c5.c5",    0x400000, 0x1ba80cee, 3 | BRF_GRA },           //  7
	{ "234-c6.c6",    0x400000, 0xbeafd091, 3 | BRF_GRA },           //  8

	{ "234-m1.m1",    0x020000, 0x087628ea, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "234-v1.v1",    0x400000, 0xed66b76f, 5 | BRF_SND },           // 10 Sound data
	{ "234-v2.v2",    0x400000, 0xa0e7f6e2, 5 | BRF_SND },           // 11
	{ "234-v3.v3",    0x400000, 0xa506e1e2, 5 | BRF_SND },           // 12
	{ "234-v4.v4",    0x400000, 0x0e34157f, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(lastblad, lastblad, neogeo)
STD_ROM_FN(lastblad)

struct BurnDriver BurnDrvlastblad = {
	"lastblad", NULL, "neogeo", NULL, "1997",
	"The Last Blade / Bakumatsu Roman - Gekka no Kenshi (NGM-2340)\0", NULL, "SNK", "Neo Geo MVS",
	L"The Last Blade\0\u5E55\u672B\u6D6A\u6F2B \u6708\u83EF\u306E\u5263\u58EB (NGM-2340)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, lastbladRomInfo, lastbladRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// The Last Blade / Bakumatsu Roman - Gekka no Kenshi (NGH-2340)
/* AES VERSION */

static struct BurnRomInfo lastbladhRomDesc[] = {
	{ "234-pg1.p1",   0x100000, 0xcd01c06d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "234-p2.sp2",   0x400000, 0x0fdc289e, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "234-s1.s1",    0x020000, 0x95561412, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "234-c1.c1",    0x800000, 0x9f7e2bd3, 3 | BRF_GRA },           //  3 Sprite data
	{ "234-c2.c2",    0x800000, 0x80623d3c, 3 | BRF_GRA },           //  4
	{ "234-c3.c3",    0x800000, 0x91ab1a30, 3 | BRF_GRA },           //  5
	{ "234-c4.c4",    0x800000, 0x3d60b037, 3 | BRF_GRA },           //  6
	{ "234-c5.c5",    0x400000, 0x1ba80cee, 3 | BRF_GRA },           //  7
	{ "234-c6.c6",    0x400000, 0xbeafd091, 3 | BRF_GRA },           //  8

	{ "234-m1.m1",    0x020000, 0x087628ea, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "234-v1.v1",    0x400000, 0xed66b76f, 5 | BRF_SND },           // 10 Sound data
	{ "234-v2.v2",    0x400000, 0xa0e7f6e2, 5 | BRF_SND },           // 11
	{ "234-v3.v3",    0x400000, 0xa506e1e2, 5 | BRF_SND },           // 12
	{ "234-v4.v4",    0x400000, 0x0e34157f, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(lastbladh, lastbladh, neogeo)
STD_ROM_FN(lastbladh)

struct BurnDriver BurnDrvlastbladh = {
	"lastbladh", "lastblad", "neogeo", NULL, "1997",
	"The Last Blade / Bakumatsu Roman - Gekka no Kenshi (NGH-2340)\0", NULL, "SNK", "Neo Geo AES",
	L"The Last Blade\0\u5E55\u672B\u6D6A\u6F2B \u6708\u83EF\u306E\u5263\u58EB (NGH-2340)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, lastbladhRomInfo, lastbladhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// The Last Soldier (Korean release of The Last Blade)
/* KOREAN VERSION */

static struct BurnRomInfo lastsoldRomDesc[] = {
	{ "134-p1k.p1",   0x100000, 0x906f3065, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "234-p2.sp2",   0x400000, 0x0fdc289e, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "234-s1.s1",    0x020000, 0x95561412, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "234-c1.c1",    0x800000, 0x9f7e2bd3, 3 | BRF_GRA },           //  3 Sprite data
	{ "234-c2.c2",    0x800000, 0x80623d3c, 3 | BRF_GRA },           //  4
	{ "234-c3.c3",    0x800000, 0x91ab1a30, 3 | BRF_GRA },           //  5
	{ "234-c4.c4",    0x800000, 0x3d60b037, 3 | BRF_GRA },           //  6
	{ "234-c5.c5",    0x400000, 0x1ba80cee, 3 | BRF_GRA },           //  7
	{ "234-c6.c6",    0x400000, 0xbeafd091, 3 | BRF_GRA },           //  8

	{ "234-m1.m1",    0x020000, 0x087628ea, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "234-v1.v1",    0x400000, 0xed66b76f, 5 | BRF_SND },           // 10 Sound data
	{ "234-v2.v2",    0x400000, 0xa0e7f6e2, 5 | BRF_SND },           // 11
	{ "234-v3.v3",    0x400000, 0xa506e1e2, 5 | BRF_SND },           // 12
	{ "234-v4.v4",    0x400000, 0x0e34157f, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(lastsold, lastsold, neogeo)
STD_ROM_FN(lastsold)

struct BurnDriver BurnDrvlastsold = {
	"lastsold", "lastblad", "neogeo", NULL, "1997",
	"The Last Soldier (Korean release of The Last Blade)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, lastsoldRomInfo, lastsoldRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// The Irritating Maze / Ultra Denryu Iraira Bou
/* MVS ONLY RELEASE */

static struct BurnRomInfo irrmazeRomDesc[] = {
	{ "236-p1.p1",    0x200000, 0x4c2ff660, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "236-s1.s1",    0x020000, 0x5d1ca640, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "236-c1.c1",    0x400000, 0xc1d47902, 3 | BRF_GRA },           //  2 Sprite data
	{ "236-c2.c2",    0x400000, 0xe15f972e, 3 | BRF_GRA },           //  3

	{ "236-m1.m1",    0x020000, 0x880a1abd, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "236-v1.v1",    0x200000, 0x5f89c3b4, 5 | BRF_SND },           //  5 Sound data
	{ "236-v2.v2",    0x100000, 0x72e3add7, 6 | BRF_SND },           //  6
};

STDROMPICKEXT(irrmaze, irrmaze, neotrackball)
STD_ROM_FN(irrmaze)

struct BurnDriver BurnDrvIrrmaze = {
	"irrmaze", NULL, "neogeo", NULL, "1997",
	"The Irritating Maze / Ultra Denryu Iraira Bou\0", NULL, "SNK / Saurus", "Neo Geo MVS",
	L"The Irritating Maze\0\u30A6\u30EB\u30C8\u30E9\u96FB\u6D41\u30A4\u30E9\u30A4\u30E9\u68D2\0", NULL, NULL, NULL,
	BDF_GAME_WORKING /*| BDF_TRACKBALL*/, 1, HARDWARE_SNK_NEOGEO | HARDWARE_SNK_TRACKBALL | HARDWARE_SNK_SWAPP, GBF_MAZE, 0,
	NULL, irrmazeRomInfo, irrmazeRomName, NULL, NULL, NULL, NULL, neotrackballInputInfo, neotrackballDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Real Bout Fatal Fury 2 - The Newcomers / Real Bout Garou Densetsu 2 - the newcomers (NGM-2400)
/* MVS VERSION */

static struct BurnRomInfo rbff2RomDesc[] = {
	{ "240-p1.p1",    0x100000, 0x80e41205, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "240-p2.sp2",   0x400000, 0x960aa88d, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "240-s1.s1",    0x020000, 0xda3b40de, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "240-c1.c1",    0x800000, 0xeffac504, 3 | BRF_GRA },           //  3 Sprite data
	{ "240-c2.c2",    0x800000, 0xed182d44, 3 | BRF_GRA },           //  4
	{ "240-c3.c3",    0x800000, 0x22e0330a, 3 | BRF_GRA },           //  5
	{ "240-c4.c4",    0x800000, 0xc19a07eb, 3 | BRF_GRA },           //  6
	{ "240-c5.c5",    0x800000, 0x244dff5a, 3 | BRF_GRA },           //  7
	{ "240-c6.c6",    0x800000, 0x4609e507, 3 | BRF_GRA },           //  8

	{ "240-m1.m1",    0x040000, 0xed482791, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "240-v1.v1",    0x400000, 0xf796265a, 5 | BRF_SND },           // 10 Sound data
	{ "240-v2.v2",    0x400000, 0x2cb3f3bb, 5 | BRF_SND },           // 11
	{ "240-v3.v3",    0x400000, 0x8fe1367a, 5 | BRF_SND },           // 12
	{ "240-v4.v4",    0x200000, 0x996704d8, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(rbff2, rbff2, neogeo)
STD_ROM_FN(rbff2)

struct BurnDriver BurnDrvrbff2 = {
	"rbff2", NULL, "neogeo", NULL, "1998",
	"Real Bout Fatal Fury 2 - The Newcomers / Real Bout Garou Densetsu 2 - the newcomers (NGM-2400)\0", NULL, "SNK", "Neo Geo MVS",
	L"Real Bout Fatal Fury 2 - The Newcomers\0Real Bout \u9913\u72FC\u4F1D\u8AAC\uFF12 (NGM-2400)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbff2RomInfo, rbff2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Real Bout Fatal Fury 2 - The Newcomers / Real Bout Garou Densetsu 2 - the newcomers (NGH-2400)
/* AES VERSION */

static struct BurnRomInfo rbff2hRomDesc[] = {
	{ "240-pg1.p1",   0x100000, 0xb6969780, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "240-p2.sp2",   0x400000, 0x960aa88d, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "240-s1.s1",    0x020000, 0xda3b40de, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "240-c1.c1",    0x800000, 0xeffac504, 3 | BRF_GRA },           //  3 Sprite data
	{ "240-c2.c2",    0x800000, 0xed182d44, 3 | BRF_GRA },           //  4
	{ "240-c3.c3",    0x800000, 0x22e0330a, 3 | BRF_GRA },           //  5
	{ "240-c4.c4",    0x800000, 0xc19a07eb, 3 | BRF_GRA },           //  6
	{ "240-c5.c5",    0x800000, 0x244dff5a, 3 | BRF_GRA },           //  7
	{ "240-c6.c6",    0x800000, 0x4609e507, 3 | BRF_GRA },           //  8

	{ "240-m1.m1",    0x040000, 0xed482791, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "240-v1.v1",    0x400000, 0xf796265a, 5 | BRF_SND },           // 10 Sound data
	{ "240-v2.v2",    0x400000, 0x2cb3f3bb, 5 | BRF_SND },           // 11
	{ "240-v3.v3",    0x400000, 0x8fe1367a, 5 | BRF_SND },           // 12
	{ "240-v4.v4",    0x200000, 0x996704d8, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(rbff2h, rbff2h, neogeo)
STD_ROM_FN(rbff2h)

struct BurnDriver BurnDrvrbff2h = {
	"rbff2h", "rbff2", "neogeo", NULL, "1998",
	"Real Bout Fatal Fury 2 - The Newcomers / Real Bout Garou Densetsu 2 - the newcomers (NGH-2400)\0", NULL, "SNK", "Neo Geo MVS",
	L"Real Bout Fatal Fury 2 - The Newcomers\0Real Bout \u9913\u72FC\u4F1D\u8AAC\uFF12 (NGH-2400)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbff2hRomInfo, rbff2hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Real Bout Fatal Fury 2 - The Newcomers (Korean release)
/* KOREAN VERSION */

static struct BurnRomInfo rbff2kRomDesc[] = {
	{ "140-p1k.p1",   0x100000, 0x965edee1, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "240-p2.sp2",   0x400000, 0x960aa88d, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "240-s1.s1",    0x020000, 0xda3b40de, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "240-c1.c1",    0x800000, 0xeffac504, 3 | BRF_GRA },           //  3 Sprite data
	{ "240-c2.c2",    0x800000, 0xed182d44, 3 | BRF_GRA },           //  4
	{ "240-c3.c3",    0x800000, 0x22e0330a, 3 | BRF_GRA },           //  5
	{ "240-c4.c4",    0x800000, 0xc19a07eb, 3 | BRF_GRA },           //  6
	{ "240-c5.c5",    0x800000, 0x244dff5a, 3 | BRF_GRA },           //  7
	{ "240-c6.c6",    0x800000, 0x4609e507, 3 | BRF_GRA },           //  8

	{ "240-m1.m1",    0x040000, 0xed482791, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "240-v1.v1",    0x400000, 0xf796265a, 5 | BRF_SND },           // 10 Sound data
	{ "240-v2.v2",    0x400000, 0x2cb3f3bb, 5 | BRF_SND },           // 11
	{ "240-v3.v3",    0x400000, 0x8fe1367a, 5 | BRF_SND },           // 12
	{ "240-v4.v4",    0x200000, 0x996704d8, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(rbff2k, rbff2k, neogeo)
STD_ROM_FN(rbff2k)

struct BurnDriver BurnDrvrbff2k = {
	"rbff2k", "rbff2", "neogeo", NULL, "1998",
	"Real Bout Fatal Fury 2 - The Newcomers (Korean release)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbff2kRomInfo, rbff2kRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Metal Slug 2 - Super Vehicle-001/II (NGM-2410) (NGH-2410)
/* MVS AND AES VERSION */

static struct BurnRomInfo mslug2RomDesc[] = {
	{ "241-p1.p1",    0x100000, 0x2a53c5da, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "241-p2.sp2",   0x200000, 0x38883f44, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "241-s1.s1",    0x020000, 0xf3d32f0f, 2 | BRF_GRA },           //  2 Text layer tiles

	/* Different layout with 8xC (mask rom) also exists on NEO-MVS CHA256; naming scheme is 241-Cx
	{ "241-c1.c1",    0x400000, 0x00000000, 3 | BRF_GRA | BRF_NODUMP },  //  3 Sprite data 	/ mask rom
	{ "241-c2.c2",    0x400000, 0x00000000, 3 | BRF_GRA | BRF_NODUMP },  //  4 				/ mask rom
	{ "241-c3.c3",    0x400000, 0x00000000, 3 | BRF_GRA | BRF_NODUMP },  //  5 				/ mask rom
	{ "241-c4.c4",    0x400000, 0x00000000, 3 | BRF_GRA | BRF_NODUMP },  //  6 				/ mask rom
	{ "241-c5.c5",    0x400000, 0x00000000, 3 | BRF_GRA | BRF_NODUMP },  //  7 				/ mask rom
	{ "241-c6.c6",    0x400000, 0x00000000, 3 | BRF_GRA | BRF_NODUMP },  //  8 				/ mask rom
	{ "241-c7.c7",    0x400000, 0x00000000, 3 | BRF_GRA | BRF_NODUMP },  //  9 				/ mask rom
	{ "241-c8.c8",    0x400000, 0x00000000, 3 | BRF_GRA | BRF_NODUMP },  // 10 				/ mask rom	*/
	{ "241-c1.c1",    0x800000, 0x394b5e0d, 3 | BRF_GRA },           //  3 Sprite data
	{ "241-c2.c2",    0x800000, 0xe5806221, 3 | BRF_GRA },           //  4
	{ "241-c3.c3",    0x800000, 0x9f6bfa6f, 3 | BRF_GRA },           //  5
	{ "241-c4.c4",    0x800000, 0x7d3e306f, 3 | BRF_GRA },           //  6

	{ "241-m1.m1",    0x020000, 0x94520ebd, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code

	{ "241-v1.v1",    0x400000, 0x99ec20e8, 5 | BRF_SND },           //  8 Sound data
	{ "241-v2.v2",    0x400000, 0xecb16799, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(mslug2, mslug2, neogeo)
STD_ROM_FN(mslug2)

struct BurnDriver BurnDrvMSlug2 = {
	"mslug2", NULL, "neogeo", NULL, "1998",
	"Metal Slug 2 - Super Vehicle-001/II (NGM-2410) (NGH-2410)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug2RomInfo, mslug2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (NGM-2420)
/* encrypted code + protection */ /* MVS VERSION */

static struct BurnRomInfo kof98RomDesc[] = {
	/* This set uses NEO-MVS PROGSF1 board */
	{ "242-p1.p1",    0x200000, 0x8893df89, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p2.sp2",   0x400000, 0x980aba4c, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "242-s1.s1",    0x020000, 0x7f7b4805, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242-c1.c1",    0x800000, 0xe564ecd6, 3 | BRF_GRA },           //  3 Sprite data
	{ "242-c2.c2",    0x800000, 0xbd959b60, 3 | BRF_GRA },           //  4
	{ "242-c3.c3",    0x800000, 0x22127b4f, 3 | BRF_GRA },           //  5
	{ "242-c4.c4",    0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  6
	{ "242-c5.c5",    0x800000, 0x9d10bed3, 3 | BRF_GRA },           //  7
	{ "242-c6.c6",    0x800000, 0xda07b6a2, 3 | BRF_GRA },           //  8
	{ "242-c7.c7",    0x800000, 0xf6d7a38a, 3 | BRF_GRA },           //  9
	{ "242-c8.c8",    0x800000, 0xc823e045, 3 | BRF_GRA },           // 10

	{ "242-m1.m1",    0x040000, 0x4ef7016b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "242-v1.v1",    0x400000, 0xb9ea8051, 5 | BRF_SND },           // 12 Sound data
	{ "242-v2.v2",    0x400000, 0xcc11106e, 5 | BRF_SND },           // 13
	{ "242-v3.v3",    0x400000, 0x044ea4e1, 5 | BRF_SND },           // 14
	{ "242-v4.v4",    0x400000, 0x7985ea30, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof98, kof98, neogeo)
STD_ROM_FN(kof98)

static void kof98Decrypt()
{
	INT32 sec[] = {0x000000, 0x100000, 0x000004, 0x100004, 0x10000a, 0x00000a, 0x10000e, 0x00000e};
	INT32 pos[] = {0x000, 0x004, 0x00a, 0x00e};

	UINT8* pTemp = (UINT8*)BurnMalloc(0x200000);

	if (pTemp == NULL) {
		return;
	}

	memcpy(pTemp, Neo68KROMActive, 0x200000);

	for (INT32 i = 0x0800; i < 0x100000; i += 0x0200) {
		for (INT32 j = 0; j < 0x0100; j += 0x10) {
			for (INT32 k = 0; k < 8; k++) {
				memcpy(&Neo68KROMActive[i + j + k * 2 +      0], &pTemp[i + j + sec[k] + 0x0100], 2);
				memcpy(&Neo68KROMActive[i + j + k * 2 + 0x0100], &pTemp[i + j + sec[k] +      0], 2);
			}
			if (i >= 0x080000 && i < 0x0c0000) {
				for (INT32 k = 0; k < 4; k++) {
					memcpy(&Neo68KROMActive[i + j + pos[k] +      0], &pTemp[i + j + pos[k] +      0], 2);
					memcpy(&Neo68KROMActive[i + j + pos[k] + 0x0100], &pTemp[i + j + pos[k] + 0x0100], 2);
				}
			}
			if (i >= 0x0c0000) {
				for (INT32 k = 0; k < 4; k++) {
					memcpy(&Neo68KROMActive[i + j + pos[k] +      0], &pTemp[i + j + pos[k] + 0x0100], 2);
					memcpy(&Neo68KROMActive[i + j + pos[k] + 0x0100], &pTemp[i + j + pos[k] +      0], 2);
				}
			}
		}

		memcpy(&Neo68KROMActive[i + 0x000000], &pTemp[i + 0x000000], 2);
		memcpy(&Neo68KROMActive[i + 0x000002], &pTemp[i + 0x100000], 2);
		memcpy(&Neo68KROMActive[i + 0x000100], &pTemp[i + 0x000100], 2);
		memcpy(&Neo68KROMActive[i + 0x000102], &pTemp[i + 0x100100], 2);
	}

	UINT32 nSp2Size = 0x400000;

	if (bDoIpsPatch) {
		UINT32 nRet = GetIpsDrvDefine(), nProgSize = IPS_PROG_VALUE(nRet);

		// kof98 P2 = ProgSize - P1Size.
		// Same as [neo_run.cpp].
		// Extra Rom not moved in ips.
		nSp2Size = (nProgSize >= 0x600000) ? nProgSize - 0x200000 : nSp2Size + (0x200000 << 1);
	}

	memmove(&Neo68KROMActive[0x100000], &Neo68KROMActive[0x200000], nSp2Size);

	BurnFree(pTemp);
}

static UINT16 nkof98Protection;

static void kof98Protection()
{
	// We need two writes because the BIOS vector block is actually 1024 bytes large
	switch (nkof98Protection) {
		case 0x0090:
			*((UINT32*)Neo68KROMActive + 0x0100) = 0x00C200FD;
			SekWriteLongROM(0x000100, 0x00C200FD);
			break;
		case 0x00F0:
			*((UINT32*)Neo68KROMActive + 0x0100) = 0x4E454F2D;
			SekWriteLongROM(0x000100, 0x4E454F2D);
			break;
	}
}

void __fastcall kof98WriteByteProtection(UINT32 sekAddress, UINT8 byteValue)
{
	switch (sekAddress) {
		case 0x20AAAA: {
			nkof98Protection &= 0x00FF;
			nkof98Protection |= byteValue << 8;
			kof98Protection();
		}
		case 0x20AAAB: {
			nkof98Protection &= 0xFF00;
			nkof98Protection |= byteValue;
			kof98Protection();
		}
	}
}

void __fastcall kof98WriteWordProtection(UINT32 sekAddress, UINT16 wordValue)
{
	switch (sekAddress) {
		case 0x20AAAA: {
			nkof98Protection = wordValue;
			kof98Protection();
		}
	}
}

static void kof98InstallHandler()
{
	SekMapHandler(6,	0x200000, 0x2FFBFF, MAP_WRITE);
	SekSetWriteWordHandler(6, kof98WriteWordProtection);
	SekSetWriteByteHandler(6, kof98WriteByteProtection);

	nkof98Protection = 0;
}

static INT32 kof98Init()
{
	NeoCallbackActive->pInitialise = kof98Decrypt;
	NeoCallbackActive->pInstallHandlers = kof98InstallHandler;
//	NeoCallbackActive->pScan = kof98Scan;

	return NeoInit();

}

struct BurnDriver BurnDrvKof98 = {
	"kof98", NULL, "neogeo", NULL, "1998",
	"The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (NGM-2420)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98RomInfo, kof98RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof98Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (NGM-2420, alternate board)
/* encrypted code + protection */ /* MVS VERSION */

static struct BurnRomInfo kof98aRomDesc[] = {
	/* This set uses NEO-MVS PROGSF1E board; same rom data as in kof98 is used */
	{ "242-p1.p1",    0x200000, 0x8893df89, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-ep1.ep1",  0x200000, 0x3f74a000, 1 | BRF_ESS | BRF_PRG }, //  1
    { "242-ep2.ep2",  0x200000, 0x6e474841, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "242-s1.s1",    0x020000, 0x7f7b4805, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "242-c1.c1",    0x800000, 0xe564ecd6, 3 | BRF_GRA },           //  4 Sprite data
	{ "242-c2.c2",    0x800000, 0xbd959b60, 3 | BRF_GRA },           //  5
	{ "242-c3.c3",    0x800000, 0x22127b4f, 3 | BRF_GRA },           //  6
	{ "242-c4.c4",    0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  7
	{ "242-c5.c5",    0x800000, 0x9d10bed3, 3 | BRF_GRA },           //  8
	{ "242-c6.c6",    0x800000, 0xda07b6a2, 3 | BRF_GRA },           //  9
	{ "242-c7.c7",    0x800000, 0xf6d7a38a, 3 | BRF_GRA },           // 10
	{ "242-c8.c8",    0x800000, 0xc823e045, 3 | BRF_GRA },           // 11

	{ "242-m1.m1",    0x040000, 0x4ef7016b, 4 | BRF_ESS | BRF_PRG }, // 12 Z80 code

	{ "242-v1.v1",    0x400000, 0xb9ea8051, 5 | BRF_SND },           // 13 Sound data
	{ "242-v2.v2",    0x400000, 0xcc11106e, 5 | BRF_SND },           // 14
	{ "242-v3.v3",    0x400000, 0x044ea4e1, 5 | BRF_SND },           // 15
	{ "242-v4.v4",    0x400000, 0x7985ea30, 5 | BRF_SND },           // 16
};

STDROMPICKEXT(kof98a, kof98a, neogeo)
STD_ROM_FN(kof98a)


struct BurnDriver BurnDrvKof98a = {
	"kof98a", "kof98", "neogeo", NULL, "1998",
	"The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (NGM-2420, alternate board)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98aRomInfo, kof98aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof98Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (Korean board, set 1)
/* encrypted code + protection, only z80 rom is different to kof98 */ /* KOREAN VERSION */

static struct BurnRomInfo kof98kRomDesc[] = {
	{ "242-p1.p1",    0x200000, 0x8893df89, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p2.sp2",   0x400000, 0x980aba4c, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "242-s1.s1",    0x020000, 0x7f7b4805, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242-c1.c1",    0x800000, 0xe564ecd6, 3 | BRF_GRA },           //  3 Sprite data
	{ "242-c2.c2",    0x800000, 0xbd959b60, 3 | BRF_GRA },           //  4
	{ "242-c3.c3",    0x800000, 0x22127b4f, 3 | BRF_GRA },           //  5
	{ "242-c4.c4",    0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  6
	{ "242-c5.c5",    0x800000, 0x9d10bed3, 3 | BRF_GRA },           //  7
	{ "242-c6.c6",    0x800000, 0xda07b6a2, 3 | BRF_GRA },           //  8
	{ "242-c7.c7",    0x800000, 0xf6d7a38a, 3 | BRF_GRA },           //  9
	{ "242-c8.c8",    0x800000, 0xc823e045, 3 | BRF_GRA },           // 10

	/* Correct chip label */
	{ "242-m1k.m1",   0x040000, 0xce12da0c, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "242-v1.v1",    0x400000, 0xb9ea8051, 5 | BRF_SND },           // 12 Sound data
	{ "242-v2.v2",    0x400000, 0xcc11106e, 5 | BRF_SND },           // 13
	{ "242-v3.v3",    0x400000, 0x044ea4e1, 5 | BRF_SND },           // 14
	{ "242-v4.v4",    0x400000, 0x7985ea30, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof98k, kof98k, neogeo)
STD_ROM_FN(kof98k)

struct BurnDriver BurnDrvKof98k = {
	"kof98k", "kof98", "neogeo", NULL, "1998",
	"The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (Korean board, set 1)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98kRomInfo, kof98kRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof98Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (Korean board, set 2)
/* encrypted code + protection, only z80 rom is different to kof98 */ /* KOREAN VERSION */

static struct BurnRomInfo kof98kaRomDesc[] = {
	{ "242-p1.p1",    0x200000, 0x8893df89, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p2.sp2",   0x400000, 0x980aba4c, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "242-s1.s1",    0x020000, 0x7f7b4805, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242-c1.c1",    0x800000, 0xe564ecd6, 3 | BRF_GRA },           //  3 Sprite data
	{ "242-c2.c2",    0x800000, 0xbd959b60, 3 | BRF_GRA },           //  4
	{ "242-c3.c3",    0x800000, 0x22127b4f, 3 | BRF_GRA },           //  5
	{ "242-c4.c4",    0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  6
	{ "242-c5.c5",    0x800000, 0x9d10bed3, 3 | BRF_GRA },           //  7
	{ "242-c6.c6",    0x800000, 0xda07b6a2, 3 | BRF_GRA },           //  8
	{ "242-c7.c7",    0x800000, 0xf6d7a38a, 3 | BRF_GRA },           //  9
	{ "242-c8.c8",    0x800000, 0xc823e045, 3 | BRF_GRA },           // 10

	/* Correct chip label */
	{ "242-mg1k.m1",  0x040000, 0xce9fb07c, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "242-v1.v1",    0x400000, 0xb9ea8051, 5 | BRF_SND },           // 12 Sound data
	{ "242-v2.v2",    0x400000, 0xcc11106e, 5 | BRF_SND },           // 13
	{ "242-v3.v3",    0x400000, 0x044ea4e1, 5 | BRF_SND },           // 14
	{ "242-v4.v4",    0x400000, 0x7985ea30, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof98ka, kof98ka, neogeo)
STD_ROM_FN(kof98ka)

struct BurnDriver BurnDrvKof98ka = {
	"kof98ka", "kof98", "neogeo", NULL, "1998",
	"The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (Korean board, set 2)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98kaRomInfo, kof98kaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof98Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (NGH-2420)
/* MVS AND AES VERSION */

static struct BurnRomInfo kof98hRomDesc[] = {
	{ "242-pn1.p1",   0x100000, 0x61ac868a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p2.sp2",   0x400000, 0x980aba4c, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "242-s1.s1",    0x020000, 0x7f7b4805, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242-c1.c1",    0x800000, 0xe564ecd6, 3 | BRF_GRA },           //  3 Sprite data
	{ "242-c2.c2",    0x800000, 0xbd959b60, 3 | BRF_GRA },           //  4
	{ "242-c3.c3",    0x800000, 0x22127b4f, 3 | BRF_GRA },           //  5
	{ "242-c4.c4",    0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  6
	{ "242-c5.c5",    0x800000, 0x9d10bed3, 3 | BRF_GRA },           //  7
	{ "242-c6.c6",    0x800000, 0xda07b6a2, 3 | BRF_GRA },           //  8
	{ "242-c7.c7",    0x800000, 0xf6d7a38a, 3 | BRF_GRA },           //  9
	{ "242-c8.c8",    0x800000, 0xc823e045, 3 | BRF_GRA },           // 10

	{ "242-mg1.m1",   0x040000, 0x4e7a6b1b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "242-v1.v1",    0x400000, 0xb9ea8051, 5 | BRF_SND },           // 12 Sound data
	{ "242-v2.v2",    0x400000, 0xcc11106e, 5 | BRF_SND },           // 13
	{ "242-v3.v3",    0x400000, 0x044ea4e1, 5 | BRF_SND },           // 14
	{ "242-v4.v4",    0x400000, 0x7985ea30, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof98h, kof98h, neogeo)
STD_ROM_FN(kof98h)

struct BurnDriver BurnDrvKof98h = {
	"kof98h", "kof98", "neogeo", NULL, "1998",
	"The King of Fighters '98 - The Slugfest / King of Fighters '98 - dream match never ends (NGH-2420)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98hRomInfo, kof98hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The Last Blade 2 / Bakumatsu Roman - Dai Ni Maku Gekka no Kenshi (NGM-2430)(NGH-2430)
/* MVS AND AES VERSION */ /* later revision */

static struct BurnRomInfo lastbld2RomDesc[] = {
	{ "243-pg1.p1",   0x100000, 0xaf1e6554, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "243-pg2.sp2",  0x400000, 0xadd4a30b, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "243-s1.s1",    0x020000, 0xc9cd2298, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "243-c1.c1",    0x800000, 0x5839444d, 3 | BRF_GRA },           //  3 Sprite data
	{ "243-c2.c2",    0x800000, 0xdd087428, 3 | BRF_GRA },           //  4
	{ "243-c3.c3",    0x800000, 0x6054cbe0, 3 | BRF_GRA },           //  5
	{ "243-c4.c4",    0x800000, 0x8bd2a9d2, 3 | BRF_GRA },           //  6
	{ "243-c5.c5",    0x800000, 0x6a503dcf, 3 | BRF_GRA },           //  7
	{ "243-c6.c6",    0x800000, 0xec9c36d0, 3 | BRF_GRA },           //  8

	{ "243-m1.m1",    0x020000, 0xacf12d10, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "243-v1.v1",    0x400000, 0xf7ee6fbb, 5 | BRF_SND },           // 10 Sound data
	{ "243-v2.v2",    0x400000, 0xaa9e4df6, 5 | BRF_SND },           // 11
	{ "243-v3.v3",    0x400000, 0x4ac750b2, 5 | BRF_SND },           // 12
	{ "243-v4.v4",    0x400000, 0xf5c64ba6, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(lastbld2, lastbld2, neogeo)
STD_ROM_FN(lastbld2)

struct BurnDriver BurnDrvlastbld2 = {
	"lastbld2", NULL, "neogeo", NULL, "1998",
	"The Last Blade 2 / Bakumatsu Roman - Dai Ni Maku Gekka no Kenshi (NGM-2430)(NGH-2430)\0", NULL, "SNK", "Neo Geo MVS",
	L"The Last Blade 2\0\u5E55\u672B\u6D6A\u6F2B\u7B2C\u4E8C\u5E55 - \u6708\u83EF\u306E\u5263\u58EB - \u6708\u306B\u54B2\u304F\u83EF\u3001\u6563\u308A\u3086\u304F\u82B1 (NGM-2430)(NGH-2430)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, lastbld2RomInfo, lastbld2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Neo-Geo Cup '98 - The Road to the Victory

static struct BurnRomInfo neocup98RomDesc[] = {
	{ "244-p1.p1",    0x200000, 0xf8fdb7a5, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "244-s1.s1",    0x020000, 0x9bddb697, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "244-c1.c1",    0x800000, 0xc7a62b23, 3 | BRF_GRA },           //  2 Sprite data
	{ "244-c2.c2",    0x800000, 0x33aa0f35, 3 | BRF_GRA },           //  3

	{ "244-m1.m1",    0x020000, 0xa701b276, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "244-v1.v1",    0x400000, 0x79def46d, 5 | BRF_SND },           //  5 Sound data
	{ "244-v2.v2",    0x200000, 0xb231902f, 5 | BRF_SND },           //  6
};

STDROMPICKEXT(neocup98, neocup98, neogeo)
STD_ROM_FN(neocup98)

struct BurnDriver BurnDrvNeocup98 = {
	"neocup98", NULL, "neogeo", NULL, "1998",
	"Neo-Geo Cup '98 - The Road to the Victory\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_SPORTSFOOTBALL, 0,
	NULL, neocup98RomInfo, neocup98RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Metal Slug X - Super Vehicle-001 (NGM-2500)(NGH-2500)
/* MVS AND AES VERSION */

static struct BurnRomInfo mslugxRomDesc[] = {
	{ "250-p1.p1",    0x100000, 0x81f1f60b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "250-p2.ep1",   0x400000, 0x1fda2e12, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "250-s1.s1",    0x020000, 0xfb6f441d, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "250-c1.c1",    0x800000, 0x09a52c6f, 3 | BRF_GRA },           //  3 Sprite data
	{ "250-c2.c2",    0x800000, 0x31679821, 3 | BRF_GRA },           //  4
	{ "250-c3.c3",    0x800000, 0xfd602019, 3 | BRF_GRA },           //  5
	{ "250-c4.c4",    0x800000, 0x31354513, 3 | BRF_GRA },           //  6
	{ "250-c5.c5",    0x800000, 0xa4b56124, 3 | BRF_GRA },           //  7
	{ "250-c6.c6",    0x800000, 0x83e3e69d, 3 | BRF_GRA },           //  8

	{ "250-m1.m1",    0x020000, 0xfd42a842, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "250-v1.v1",    0x400000, 0xc79ede73, 5 | BRF_SND },           // 10 Sound data
	{ "250-v2.v2",    0x400000, 0xea9aabe1, 5 | BRF_SND },           // 11
	{ "250-v3.v3",    0x200000, 0x2ca65102, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(mslugx, mslugx, neogeo)
STD_ROM_FN(mslugx)

static UINT16 mslugx_command;
static UINT16 mslugx_counter;

static void mslugx_protection_write(UINT32 SekAddress, UINT16 wordValue)
{
	switch (SekAddress)
	{
		case 0x2fffe0: // start new read?
			mslugx_command = 0;
		return;

		case 0x2fffe2: // command? These are pulsed with data and then 0
		case 0x2fffe4:
			mslugx_command |= wordValue;
		return;

		case 0x2fffe6: // finished?
		return;

		case 0x2fffea: // init?
			mslugx_counter = 0;
		return;
	}
}

static UINT16 __fastcall mslugx_protection_read()
{
	UINT16 ret = 0;

	switch (mslugx_command)
	{
		case 0x0001: // $3bdc(?) and $3c30 (Register D7)
		{
			ret = (SekReadByte(0xdedd2 + ((mslugx_counter >> 3) & 0xfff)) >> (~mslugx_counter & 0x07)) & 1;
			mslugx_counter++;
		}
		break;

		case 0x0fff: // All other accesses (Register D2)
		{
			INT32 select = SekReadWord(0x10f00a) - 1; // Cheat and read calculated offset
			ret = (SekReadByte(0xdedd2 + ((select >> 3) & 0x0fff)) >> (~select & 0x07)) & 1;
		}
		break;
	}

	return ret;
}

static void mslugxMapBank()
{
	SekMapMemory(Neo68KROMActive + nNeo68KROMBank, 0x200000, 0x2ffbff, MAP_ROM);
}

static void mslugxBankswitch(UINT32 nBank)
{
	nBank = ((nBank & 7) + 1) * 0x100000;

	if (nBank != nNeo68KROMBank) {
		nNeo68KROMBank = nBank;
		mslugxMapBank();
	}
}

static UINT16 __fastcall mslugx_read_protection_word(UINT32 SekAddress)
{
	if (SekAddress == 0x2fffe8) {
		return mslugx_protection_read();
	}

	return *((UINT16*)(Neo68KROMActive + nNeo68KROMBank + (SekAddress & 0xffffe)));
}

static UINT8 __fastcall mslugx_read_protection_byte(UINT32 SekAddress)
{
	return Neo68KROMActive[(nNeo68KROMBank + (SekAddress & 0xfffff)) ^ 1];
}

static void __fastcall mslugx_write_protection_word(UINT32 SekAddress, UINT16 wordValue)
{
	if ((SekAddress & 0xfffff0) == 0x2fffe0) {
		mslugx_protection_write(SekAddress, wordValue);
	}

	if (SekAddress == 0x2ffff0) {
		mslugxBankswitch(wordValue);
	}
}

static void __fastcall mslugx_write_protection_byte(UINT32 SekAddress, UINT8 byteValue)
{
	if (SekAddress == 0x2ffff0) {
		mslugxBankswitch(byteValue);
	}
}

static void mslugxInstallBankSwitchHandler()
{
	SekMapHandler(6,	0x2ffc00, 0x2fffff, MAP_WRITE | MAP_READ | MAP_FETCH);
	SekSetReadWordHandler(6, mslugx_read_protection_word);
	SekSetReadByteHandler(6, mslugx_read_protection_byte);
	SekSetWriteWordHandler(6, mslugx_write_protection_word);
	SekSetWriteByteHandler(6, mslugx_write_protection_byte);
}

static INT32 mslugxInit()
{
	NeoCallbackActive->pInstallHandlers = mslugxInstallBankSwitchHandler;
	NeoCallbackActive->pBankswitch = mslugxMapBank;

	return NeoInit();
}

static INT32 mslugxScan(INT32 nAction, INT32 *pnMin)
{
	if (pnMin) {
		*pnMin =  0x029727;
	}

	if (nAction & ACB_DRIVER_DATA)
	{
		SCAN_VAR(mslugx_command);
		SCAN_VAR(mslugx_counter);
	}

	return NeoScan(nAction,pnMin);
}

struct BurnDriver BurnDrvMSlugx = {
	"mslugx", NULL, "neogeo", NULL, "1999",
	"Metal Slug X - Super Vehicle-001 (NGM-2500)(NGH-2500)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslugxRomInfo, mslugxRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslugxInit, NeoExit, NeoFrame, NeoRender, mslugxScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 - Millennium Battle (NGM-2510)
/* Original Version - Encrypted Code & GFX */ /* MVS VERSION */

static struct BurnRomInfo kof99RomDesc[] = {
	{ "ka.neo-sma",   0x040000, 0x7766d09e, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "251-p1.p1",    0x400000, 0x006e4532, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "251-p2.p2",    0x400000, 0x90175f15, 1 | BRF_ESS | BRF_PRG }, //  2

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "251-c1.c1",    0x800000, 0x0f9e93fe, 3 | BRF_GRA },           //  3 Sprite data
	{ "251-c2.c2",    0x800000, 0xe71e2ea3, 3 | BRF_GRA },           //  4
	{ "251-c3.c3",    0x800000, 0x238755d2, 3 | BRF_GRA },           //  5
	{ "251-c4.c4",    0x800000, 0x438c8b22, 3 | BRF_GRA },           //  6
	{ "251-c5.c5",    0x800000, 0x0b0abd0a, 3 | BRF_GRA },           //  7
	{ "251-c6.c6",    0x800000, 0x65bbf281, 3 | BRF_GRA },           //  8
	{ "251-c7.c7",    0x800000, 0xff65f62e, 3 | BRF_GRA },           //  9
	{ "251-c8.c8",    0x800000, 0x8d921c68, 3 | BRF_GRA },           // 10

	{ "251-m1.m1",    0x020000, 0x5e74539c, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "251-v1.v1",    0x400000, 0xef2eecc8, 5 | BRF_SND },           // 12 Sound data
	{ "251-v2.v2",    0x400000, 0x73e211ca, 5 | BRF_SND },           // 13
	{ "251-v3.v3",    0x400000, 0x821901da, 5 | BRF_SND },           // 14
	{ "251-v4.v4",    0x200000, 0xb49e6178, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof99, kof99, neogeo)
STD_ROM_FN(kof99)

static void kof99SMADecrypt()
{
	for (INT32 i = 0; i < 0x800000 / 2; i++) {
		((UINT16*)(Neo68KROMActive + 0x100000))[i] = BITSWAP16(BURN_ENDIAN_SWAP_INT16(((UINT16*)(Neo68KROMActive + 0x100000))[i]), 13, 7, 3, 0, 9, 4, 5, 6, 1, 12, 8, 14, 10, 11, 2, 15);
	}

	for (INT32 i = 0; i < 0x0C0000 / 2; i++) {
		((UINT16*)Neo68KROMActive)[i] = BURN_ENDIAN_SWAP_INT16(((UINT16*)Neo68KROMActive)[0x700000 / 2 + BITSWAP24(i, 23, 22, 21, 20, 19, 18, 11, 6, 14, 17, 16, 5, 8, 10, 12, 0, 4, 3, 2, 7, 9, 15, 13, 1)]);
	}

	for (INT32 i = 0; i < 0x600000 / 2; i += 0x0800 / 2) {
		UINT16 nBuffer[0x0800 / 2];
		memcpy(nBuffer, &((UINT16*)(Neo68KROMActive + 0x100000))[i], 0x0800);
		for (INT32 j = 0; j < 0x0800 / 2; j++) {
			((UINT16*)(Neo68KROMActive + 0x100000))[i + j] = BURN_ENDIAN_SWAP_INT16(nBuffer[BITSWAP24(j, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 6, 2, 4, 9, 8, 3, 1, 7, 0, 5)]);
		}
	}
}

void __fastcall kof99WriteWordBankswitch(UINT32 sekAddress, UINT16 wordValue)
{
	if (sekAddress == 0x2FFFF0) {
		static UINT32 bankoffset[64] = {
			0x100000, 0x200000, 0x300000, 0x400000,
			0x4cc000, 0x5cc000, 0x4f2000, 0x5f2000,
			0x507800, 0x607800, 0x50d000, 0x60d000,
			0x517800, 0x617800, 0x520800, 0x620800,
			0x524800, 0x624800, 0x529000, 0x629000,
			0x52e800, 0x62e800, 0x531800, 0x631800,
			0x64d000, 0x651000, 0x667000, 0x692800,
			0x688800, 0x681800, 0x699800, 0x694800,
			0x698000,	/* rest not used? */
		};

		// Unscramble bank number
		INT32 nBank =
			(((wordValue >> 14) & 1) << 0) +
			(((wordValue >>  6) & 1) << 1) +
			(((wordValue >>  8) & 1) << 2) +
			(((wordValue >> 10) & 1) << 3) +
			(((wordValue >> 12) & 1) << 4) +
			(((wordValue >>  5) & 1) << 5);

		if (bankoffset[nBank] != nNeo68KROMBank) {
			nNeo68KROMBank = bankoffset[nBank];
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank,			  0x200000, 0x2FE3FF, MAP_ROM);
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank + 0x0FE800, 0x2FE800, 0x2FFBFF, MAP_ROM);
		}
	}
}

static INT32 kof99Init()
{
	nNeoProtectionXor = 0x00;

	return NeoSMAInit(kof99SMADecrypt, kof99WriteWordBankswitch, 0x2FFFF8, 0x2FFFFA);
}

struct BurnDriver BurnDrvkof99 = {
	"kof99", NULL, "neogeo", NULL, "1999",
	"The King of Fighters '99 - Millennium Battle (NGM-2510)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99RomInfo, kof99RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof99Init, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 - Millennium Battle (NGH-2510)
/* Original Version - Encrypted Code & GFX */ /* AES VERSION */

static struct BurnRomInfo kof99hRomDesc[] = {
	{ "kc.neo-sma",   0x040000, 0x6c9d0647, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "251-p1.p1",    0x400000, 0x006e4532, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "251-pg2.p2",   0x400000, 0xd9057f51, 1 | BRF_ESS | BRF_PRG }, //  2

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "251-c1.c1",    0x800000, 0x0f9e93fe, 3 | BRF_GRA },           //  3 Sprite data
	{ "251-c2.c2",    0x800000, 0xe71e2ea3, 3 | BRF_GRA },           //  4
	{ "251-c3.c3",    0x800000, 0x238755d2, 3 | BRF_GRA },           //  5
	{ "251-c4.c4",    0x800000, 0x438c8b22, 3 | BRF_GRA },           //  6
	{ "251-c5.c5",    0x800000, 0x0b0abd0a, 3 | BRF_GRA },           //  7
	{ "251-c6.c6",    0x800000, 0x65bbf281, 3 | BRF_GRA },           //  8
	{ "251-c7.c7",    0x800000, 0xff65f62e, 3 | BRF_GRA },           //  9
	{ "251-c8.c8",    0x800000, 0x8d921c68, 3 | BRF_GRA },           // 10

	{ "251-m1.m1",    0x020000, 0x5e74539c, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "251-v1.v1",    0x400000, 0xef2eecc8, 5 | BRF_SND },           // 12 Sound data
	{ "251-v2.v2",    0x400000, 0x73e211ca, 5 | BRF_SND },           // 13
	{ "251-v3.v3",    0x400000, 0x821901da, 5 | BRF_SND },           // 14
	{ "251-v4.v4",    0x200000, 0xb49e6178, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof99h, kof99h, neogeo)
STD_ROM_FN(kof99h)

struct BurnDriver BurnDrvkof99h = {
	"kof99h", "kof99", "neogeo", NULL, "1999",
	"The King of Fighters '99 - Millennium Battle (NGH-2510)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99hRomInfo, kof99hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof99Init, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 - Millennium Battle (earlier)
/* Original Version - Encrypted Code & GFX */

static struct BurnRomInfo kof99eRomDesc[] = {
	{ "ka.neo-sma",   0x040000, 0x7766d09e, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	/* Is the SMA for this set correct? A set with this layout and a SMA.KB is known */
	{ "251-ep1.p1",   0x200000, 0x1e8d692d, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "251-ep2.p2",   0x200000, 0xd6206e5a, 1 | BRF_ESS | BRF_PRG }, //  2
	{ "251-ep3.p3",   0x200000, 0xd58c3ef8, 1 | BRF_ESS | BRF_PRG }, //  3
	{ "251-ep4.p4",   0x200000, 0x52de02ae, 1 | BRF_ESS | BRF_PRG }, //  4

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "251-c1.c1",    0x800000, 0x0f9e93fe, 3 | BRF_GRA },           //  5 Sprite data
	{ "251-c2.c2",    0x800000, 0xe71e2ea3, 3 | BRF_GRA },           //  6
	{ "251-c3.c3",    0x800000, 0x238755d2, 3 | BRF_GRA },           //  7
	{ "251-c4.c4",    0x800000, 0x438c8b22, 3 | BRF_GRA },           //  8
	{ "251-c5.c5",    0x800000, 0x0b0abd0a, 3 | BRF_GRA },           //  9
	{ "251-c6.c6",    0x800000, 0x65bbf281, 3 | BRF_GRA },           // 10
	{ "251-c7.c7",    0x800000, 0xff65f62e, 3 | BRF_GRA },           // 11
	{ "251-c8.c8",    0x800000, 0x8d921c68, 3 | BRF_GRA },           // 12

	{ "251-m1.m1",    0x020000, 0x5e74539c, 4 | BRF_ESS | BRF_PRG }, // 13 Z80 code

	{ "251-v1.v1",    0x400000, 0xef2eecc8, 5 | BRF_SND },           // 14 Sound data
	{ "251-v2.v2",    0x400000, 0x73e211ca, 5 | BRF_SND },           // 15
	{ "251-v3.v3",    0x400000, 0x821901da, 5 | BRF_SND },           // 16
	{ "251-v4.v4",    0x200000, 0xb49e6178, 5 | BRF_SND },           // 17
};

STDROMPICKEXT(kof99e, kof99e, neogeo)
STD_ROM_FN(kof99e)

struct BurnDriver BurnDrvkof99e = {
	"kof99e", "kof99", "neogeo", NULL, "1999",
	"The King of Fighters '99 - Millennium Battle (earlier)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99eRomInfo, kof99eRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof99Init, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 - Millennium Battle (Korean release)
/* Original Version - Encrypted code & GFX */ /* KOREAN VERSION */

static struct BurnRomInfo kof99kRomDesc[] = {
	{ "kb.neo-sma",   0x040000, 0x9fccc688, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "151-pg1k.p1",  0x400000, 0xef259292, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "151-pg2k.p2",  0x400000, 0xf3898ec2, 1 | BRF_ESS | BRF_PRG }, //  2

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "251-c1.c1",    0x800000, 0x0f9e93fe, 3 | BRF_GRA },           //  3 Sprite data
	{ "251-c2.c2",    0x800000, 0xe71e2ea3, 3 | BRF_GRA },           //  4
	{ "251-c3.c3",    0x800000, 0x238755d2, 3 | BRF_GRA },           //  5
	{ "251-c4.c4",    0x800000, 0x438c8b22, 3 | BRF_GRA },           //  6
	{ "251-c5.c5",    0x800000, 0x0b0abd0a, 3 | BRF_GRA },           //  7
	{ "251-c6.c6",    0x800000, 0x65bbf281, 3 | BRF_GRA },           //  8
	{ "251-c7.c7",    0x800000, 0xff65f62e, 3 | BRF_GRA },           //  9
	{ "251-c8.c8",    0x800000, 0x8d921c68, 3 | BRF_GRA },           // 10

	{ "251-m1.m1",    0x020000, 0x5e74539c, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "251-v1.v1",    0x400000, 0xef2eecc8, 5 | BRF_SND },           // 12 Sound data
	{ "251-v2.v2",    0x400000, 0x73e211ca, 5 | BRF_SND },           // 13
	{ "251-v3.v3",    0x400000, 0x821901da, 5 | BRF_SND },           // 14
	{ "251-v4.v4",    0x200000, 0xb49e6178, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof99k, kof99k, neogeo)
STD_ROM_FN(kof99k)

struct BurnDriver BurnDrvkof99k = {
	"kof99k", "kof99", "neogeo", NULL, "1999",
	"The King of Fighters '99 - Millennium Battle (Korean release)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99kRomInfo, kof99kRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof99Init, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 - Millennium Battle (Korean release)
/* Original Version - Encrypted GFX */ /* KOREAN VERSION */

static struct BurnRomInfo kof99kaRomDesc[] = {
	{ "152-p1.p1",    0x100000, 0xf2c7ddfa, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "152-p2.sp2",   0x400000, 0x274ef47a, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "251-c1.c1",    0x800000, 0x0f9e93fe, 3 | BRF_GRA },           //  2 Sprite data
	{ "251-c2.c2",    0x800000, 0xe71e2ea3, 3 | BRF_GRA },           //  3
	{ "251-c3.c3",    0x800000, 0x238755d2, 3 | BRF_GRA },           //  4
	{ "251-c4.c4",    0x800000, 0x438c8b22, 3 | BRF_GRA },           //  5
	{ "251-c5.c5",    0x800000, 0x0b0abd0a, 3 | BRF_GRA },           //  6
	{ "251-c6.c6",    0x800000, 0x65bbf281, 3 | BRF_GRA },           //  7
	{ "251-c7.c7",    0x800000, 0xff65f62e, 3 | BRF_GRA },           //  8
	{ "251-c8.c8",    0x800000, 0x8d921c68, 3 | BRF_GRA },           //  9

	{ "251-m1.m1",    0x020000, 0x5e74539c, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "251-v1.v1",    0x400000, 0xef2eecc8, 5 | BRF_SND },           // 11 Sound data
	{ "251-v2.v2",    0x400000, 0x73e211ca, 5 | BRF_SND },           // 12
	{ "251-v3.v3",    0x400000, 0x821901da, 5 | BRF_SND },           // 13
	{ "251-v4.v4",    0x200000, 0xb49e6178, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(kof99ka, kof99ka, neogeo)
STD_ROM_FN(kof99ka)

INT32 kof99kaInit()
{
	nNeoProtectionXor = 0x00;
	return NeoInit();
}

struct BurnDriver BurnDrvkof99ka = {
	"kof99ka", "kof99", "neogeo", NULL, "1999",
	"The King of Fighters '99 - Millennium Battle (Korean release, non-encrypted program)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99kaRomInfo, kof99kaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof99kaInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 - Millennium Battle (prototype)
/* Prototype Version - Possibly Hacked */

static struct BurnRomInfo kof99pRomDesc[] = {
	{ "proto_251-p1.p1",   0x100000, 0xf37929c4, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "proto_251-p2.p2",   0x400000, 0x739742ad, 1 | BRF_ESS | BRF_PRG }, //  1

	/* This is the S1 from the prototype, the final is different */
	{ "proto_251-s1.s1",   0x020000, 0xfb1498ed, 2 | BRF_GRA },           //  2 Text layer tiles

	/* these are probably decrypted versions of the roms found in the final */
	{ "proto_251-c1.c1",   0x800000, 0xe5d8ffa4, 3 | BRF_GRA },           //  3 Sprite data
	{ "proto_251-c2.c2",   0x800000, 0xd822778f, 3 | BRF_GRA },           //  4
	{ "proto_251-c3.c3",   0x800000, 0xf20959e8, 3 | BRF_GRA },           //  5
	{ "proto_251-c4.c4",   0x800000, 0x54ffbe9f, 3 | BRF_GRA },           //  6
	{ "proto_251-c5.c5",   0x800000, 0xd87a3bbc, 3 | BRF_GRA },           //  7
	{ "proto_251-c6.c6",   0x800000, 0x4d40a691, 3 | BRF_GRA },           //  8
	{ "proto_251-c7.c7",   0x800000, 0xa4479a58, 3 | BRF_GRA },           //  9
	{ "proto_251-c8.c8",   0x800000, 0xead513ce, 3 | BRF_GRA },           // 10

	/* Did the Prototype really use the same sound program / voice roms, sound isn't great .. */
	{ "251-m1.m1",         0x020000, 0x5e74539c, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "251-v1.v1",         0x400000, 0xef2eecc8, 5 | BRF_SND },           // 12 Sound data
	{ "251-v2.v2",         0x400000, 0x73e211ca, 5 | BRF_SND },           // 13
	{ "251-v3.v3",         0x400000, 0x821901da, 5 | BRF_SND },           // 14
	{ "251-v4.v4",         0x200000, 0xb49e6178, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof99p, kof99p, neogeo)
STD_ROM_FN(kof99p)

struct BurnDriver BurnDrvkof99p = {
	"kof99p", "kof99", "neogeo", NULL, "1999",
	"The King of Fighters '99 - Millennium Battle (prototype)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99pRomInfo, kof99pRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Garou - Mark of the Wolves (NGM-2530)
/* Original Version - Encrypted GFX */ /* MVS VERSION - later revision */

static struct BurnRomInfo garouRomDesc[] = {
	{ "kf.neo-sma",   0x040000, 0x98bc93dc, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "253-ep1.p1",   0x200000, 0xea3171a4, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "253-ep2.p2",   0x200000, 0x382f704b, 1 | BRF_ESS | BRF_PRG }, //  2
	{ "253-ep3.p3",   0x200000, 0xe395bfdd, 1 | BRF_ESS | BRF_PRG }, //  3
	{ "253-ep4.p4",   0x200000, 0xda92c08e, 1 | BRF_ESS | BRF_PRG }, //  4

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "253-c1.c1",    0x800000, 0x0603e046, 3 | BRF_GRA },           //  5 Sprite data
	{ "253-c2.c2",    0x800000, 0x0917d2a4, 3 | BRF_GRA },           //  6
	{ "253-c3.c3",    0x800000, 0x6737c92d, 3 | BRF_GRA },           //  7
	{ "253-c4.c4",    0x800000, 0x5ba92ec6, 3 | BRF_GRA },           //  8
	{ "253-c5.c5",    0x800000, 0x3eab5557, 3 | BRF_GRA },           //  9
	{ "253-c6.c6",    0x800000, 0x308d098b, 3 | BRF_GRA },           // 10
	{ "253-c7.c7",    0x800000, 0xc0e995ae, 3 | BRF_GRA },           // 11
	{ "253-c8.c8",    0x800000, 0x21a11303, 3 | BRF_GRA },           // 12

	{ "253-m1.m1",    0x040000, 0x36a806be, 4 | BRF_ESS | BRF_PRG }, // 13 Z80 code

	{ "253-v1.v1",    0x400000, 0x263e388c, 5 | BRF_SND },           // 14 Sound data
	{ "253-v2.v2",    0x400000, 0x2c6bc7be, 5 | BRF_SND },           // 15
	{ "253-v3.v3",    0x400000, 0x0425b27d, 5 | BRF_SND },           // 16
	{ "253-v4.v4",    0x400000, 0xa54be8a9, 5 | BRF_SND },           // 17
};

STDROMPICKEXT(garou, garou, neogeo)
STD_ROM_FN(garou)

static void garouSMADecrypt()
{
	for (INT32 i = 0; i < 0x800000 / 2; i++) {
		((UINT16*)(Neo68KROMActive + 0x100000))[i] = BITSWAP16(BURN_ENDIAN_SWAP_INT16(((UINT16*)(Neo68KROMActive + 0x100000))[i]), 13, 12, 14, 10, 8, 2, 3, 1, 5, 9, 11, 4, 15, 0, 6, 7);
	}

	for (INT32 i = 0; i < 0x0C0000 / 2; i++) {
		((UINT16*)Neo68KROMActive)[i] = BURN_ENDIAN_SWAP_INT16(((UINT16*)Neo68KROMActive)[0x710000 / 2 + BITSWAP24(i, 23, 22, 21, 20, 19, 18, 4, 5, 16, 14, 7, 9, 6, 13, 17, 15, 3, 1, 2, 12, 11, 8, 10, 0)]);
	}

	for (INT32 i = 0; i < 0x800000 / 2; i += 0x8000 / 2) {
		UINT16 nBuffer[0x8000 / 2];
		memcpy(nBuffer, &((UINT16*)(Neo68KROMActive + 0x100000))[i], 0x8000);
		for (INT32 j = 0; j < 0x8000 / 2; j++) {
			((UINT16*)(Neo68KROMActive + 0x100000))[i + j] = BURN_ENDIAN_SWAP_INT16(nBuffer[BITSWAP24(j, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 9, 4, 8, 3, 13, 6, 2, 7, 0, 12, 1, 11, 10, 5)]);
		}
	}
}

void __fastcall garouWriteWordBankswitch(UINT32 sekAddress, UINT16 wordValue)
{
	if (sekAddress == 0x2FFFC0) {
		static UINT32 bankoffset[64] = {
			0x100000, 0x200000, 0x300000, 0x400000, // 00
			0x380000, 0x480000, 0x3d0000, 0x4d0000, // 04
			0x3f0000, 0x4f0000, 0x500000, 0x600000, // 08
			0x520000, 0x620000, 0x540000, 0x640000, // 12
			0x598000, 0x698000, 0x5a0000, 0x6a0000, // 16
			0x5a8000, 0x6a8000, 0x5b0000, 0x6b0000, // 20
			0x5b8000, 0x6b8000, 0x5c0000, 0x6c0000, // 24
			0x5c8000, 0x6c8000, 0x5d0000, 0x6d0000, // 28
			0x558000, 0x658000, 0x560000, 0x660000, // 32
			0x568000, 0x668000, 0x570000, 0x670000, // 36
			0x578000, 0x678000, 0x580000, 0x680000, // 40
			0x588000, 0x688000, 0x590000, 0x690000, // 44
			0x6d0000, 0x6d8000, 0x6e0000, 0x6e8000, // 48
			0x6f0000, 0x6f8000, 0x700000, /* rest not used? */
		};

		// Unscramble bank number
		INT32 nBank =
			(((wordValue >>  5) & 1) << 0) +
			(((wordValue >>  9) & 1) << 1) +
			(((wordValue >>  7) & 1) << 2) +
			(((wordValue >>  6) & 1) << 3) +
			(((wordValue >> 14) & 1) << 4) +
			(((wordValue >> 12) & 1) << 5);

		if (bankoffset[nBank] != nNeo68KROMBank) {
			nNeo68KROMBank = bankoffset[nBank];
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank,			  0x200000, 0x2FE3FF, MAP_ROM);
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank + 0x0FE800, 0x2FE800, 0x2FFBFF, MAP_ROM);
		}
	}
}

static INT32 garouInit()
{
	nNeoProtectionXor = 0x06;

	return NeoSMAInit(garouSMADecrypt, garouWriteWordBankswitch, 0x2FFFCC, 0x2FFFF0);
}

struct BurnDriver BurnDrvGarou = {
	"garou", NULL, "neogeo", NULL, "1999",
	"Garou - Mark of the Wolves (NGM-2530)\0", NULL, "SNK", "Neo Geo MVS",
	L"Garou\0\u9913\u72FC - mark of the wolves (NGM-2530)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_VSFIGHT, FBF_FATFURY,
	NULL, garouRomInfo, garouRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	garouInit, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Garou - Mark of the Wolves (NGM-2530) (NGH-2530)
/* Original Version - Encrypted GFX */ /* MVS AND AES VERSION - earlier revision */

static struct BurnRomInfo garouhRomDesc[] = {
	{ "ke.neo-sma",   0x040000, 0x96c72233, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "253-p1.p1",    0x400000, 0x18ae5d7e, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "253-p2.p2",    0x400000, 0xafffa779, 1 | BRF_ESS | BRF_PRG }, //  2

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "253-c1.c1",    0x800000, 0x0603e046, 3 | BRF_GRA },           //  3 Sprite data
	{ "253-c2.c2",    0x800000, 0x0917d2a4, 3 | BRF_GRA },           //  4
	{ "253-c3.c3",    0x800000, 0x6737c92d, 3 | BRF_GRA },           //  5
	{ "253-c4.c4",    0x800000, 0x5ba92ec6, 3 | BRF_GRA },           //  6
	{ "253-c5.c5",    0x800000, 0x3eab5557, 3 | BRF_GRA },           //  7
	{ "253-c6.c6",    0x800000, 0x308d098b, 3 | BRF_GRA },           //  8
	{ "253-c7.c7",    0x800000, 0xc0e995ae, 3 | BRF_GRA },           //  9
	{ "253-c8.c8",    0x800000, 0x21a11303, 3 | BRF_GRA },           // 10

	{ "253-m1.m1",    0x040000, 0x36a806be, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "253-v1.v1",    0x400000, 0x263e388c, 5 | BRF_SND },           // 12 Sound data
	{ "253-v2.v2",    0x400000, 0x2c6bc7be, 5 | BRF_SND },           // 13
	{ "253-v3.v3",    0x400000, 0x0425b27d, 5 | BRF_SND },           // 14
	{ "253-v4.v4",    0x400000, 0xa54be8a9, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(garouh, garouh, neogeo)
STD_ROM_FN(garouh)

static void garouhSMADecrypt()
{
	for (INT32 i = 0; i < 0x800000 / 2; i++) {
		((UINT16*)(Neo68KROMActive + 0x100000))[i] = BITSWAP16(BURN_ENDIAN_SWAP_INT16(((UINT16*)(Neo68KROMActive + 0x100000))[i]), 14, 5, 1, 11, 7, 4, 10, 15, 3, 12, 8, 13, 0, 2, 9, 6);
	}

	for (INT32 i = 0; i < 0x0C0000 / 2; i++) {
		((UINT16*)Neo68KROMActive)[i] = BURN_ENDIAN_SWAP_INT16(((UINT16*)Neo68KROMActive)[0x7F8000 / 2 + BITSWAP24(i, 23, 22, 21, 20, 19, 18, 5, 16, 11, 2, 6, 7, 17, 3, 12, 8, 14, 4, 0, 9, 1, 10, 15, 13)]);
	}

	for (INT32 i = 0; i < 0x800000 / 2; i += 0x8000 / 2) {
		UINT16 nBuffer[0x8000 / 2];
		memcpy(nBuffer, &((UINT16*)(Neo68KROMActive + 0x100000))[i], 0x8000);
		for (INT32 j = 0; j < 0x8000 / 2; j++) {
			((UINT16*)(Neo68KROMActive + 0x100000))[i + j] = BURN_ENDIAN_SWAP_INT16(nBuffer[BITSWAP24(j, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 12, 8, 1, 7, 11, 3, 13, 10, 6, 9, 5, 4, 0, 2)]);
		}
	}
}

void __fastcall garouhWriteWordBankswitch(UINT32 sekAddress, UINT16 wordValue)
{
	if (sekAddress == 0x2FFFC0) {
		static UINT32 bankoffset[64] = {
			0x100000, 0x200000, 0x300000, 0x400000, // 00
			0x380000, 0x480000, 0x3d0000, 0x4d0000, // 04
			0x3c8000, 0x4c8000, 0x500000, 0x600000, // 08
			0x520000, 0x620000, 0x540000, 0x640000, // 12
			0x698000, 0x798000, 0x6a0000, 0x7a0000, // 16
			0x6a8000, 0x7a8000, 0x6b0000, 0x7b0000, // 20
			0x6b8000, 0x7b8000, 0x6c0000, 0x7c0000, // 24
			0x6c8000, 0x7c8000, 0x6d0000, 0x7d0000, // 28
			0x558000, 0x658000, 0x560000, 0x660000, // 32
			0x568000, 0x668000, 0x570000, 0x670000, // 36
			0x578000, 0x678000, 0x580000, 0x680000, // 40
			0x588000, 0x688000, 0x590000, 0x690000, // 44
			0x6d8000, 0x7d8000, 0x6e0000, 0x7e0000, // 48
			0x6e8000, 0x7e8000, 0x7e8000, 0x100000, // 52
			0x100000, 0x100000, 0x100000, 0x100000, // 56
			0x100000, 0x100000, 0x100000, 0x100000, // 60
		};

		// Unscramble bank number
		INT32 nBank =
			(((wordValue >>  4) & 1) << 0) +
			(((wordValue >>  8) & 1) << 1) +
			(((wordValue >> 14) & 1) << 2) +
			(((wordValue >>  2) & 1) << 3) +
			(((wordValue >> 11) & 1) << 4) +
			(((wordValue >> 13) & 1) << 5);

		if (bankoffset[nBank] != nNeo68KROMBank) {
			nNeo68KROMBank = bankoffset[nBank];
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank,			  0x200000, 0x2FE3FF, MAP_ROM);
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank + 0x0FE800, 0x2FE800, 0x2FFBFF, MAP_ROM);
		}
	}
}

static INT32 garouhInit()
{
	nNeoProtectionXor = 0x06;

	return NeoSMAInit(garouhSMADecrypt, garouhWriteWordBankswitch, 0x2FFFCC, 0x2FFFF0);
}

struct BurnDriver BurnDrvgarouh = {
	"garouh", "garou", "neogeo", NULL, "1999",
	"Garou - Mark of the Wolves (NGM-2530) (NGH-2530)\0", NULL, "SNK", "Neo Geo MVS",
	L"Garou\0\u9913\u72FC - mark of the wolves (NGM-2530) (NGH-2530)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_VSFIGHT, FBF_FATFURY,
	NULL, garouhRomInfo, garouhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	garouhInit, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Garou - Mark of the Wolves (NGH-2530)
/* Original Version - Encrypted GFX */ /* AES VERSION */

static struct BurnRomInfo garouhaRomDesc[] = {
	{ "neo-sma",      0x040000, 0x9aa450e6, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	/* standard program roms are identical to 'garou' set, only content of custom chip above differs */
	{ "253-ep1.p1",   0x200000, 0xea3171a4, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "253-ep2.p2",   0x200000, 0x382f704b, 1 | BRF_ESS | BRF_PRG }, //  2
	{ "253-ep3.p3",   0x200000, 0xe395bfdd, 1 | BRF_ESS | BRF_PRG }, //  3
	{ "253-ep4.p4",   0x200000, 0xda92c08e, 1 | BRF_ESS | BRF_PRG }, //  4

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "253-c1.c1",    0x800000, 0x0603e046, 3 | BRF_GRA },           //  5 Sprite data
	{ "253-c2.c2",    0x800000, 0x0917d2a4, 3 | BRF_GRA },           //  6
	{ "253-c3.c3",    0x800000, 0x6737c92d, 3 | BRF_GRA },           //  7
	{ "253-c4.c4",    0x800000, 0x5ba92ec6, 3 | BRF_GRA },           //  8
	{ "253-c5.c5",    0x800000, 0x3eab5557, 3 | BRF_GRA },           //  9
	{ "253-c6.c6",    0x800000, 0x308d098b, 3 | BRF_GRA },           // 10
	{ "253-c7.c7",    0x800000, 0xc0e995ae, 3 | BRF_GRA },           // 11
	{ "253-c8.c8",    0x800000, 0x21a11303, 3 | BRF_GRA },           // 12

	{ "253-m1.m1",    0x040000, 0x36a806be, 4 | BRF_ESS | BRF_PRG }, // 13 Z80 code

	{ "253-v1.v1",    0x400000, 0x263e388c, 5 | BRF_SND },           // 14 Sound data
	{ "253-v2.v2",    0x400000, 0x2c6bc7be, 5 | BRF_SND },           // 15
	{ "253-v3.v3",    0x400000, 0x0425b27d, 5 | BRF_SND },           // 16
	{ "253-v4.v4",    0x400000, 0xa54be8a9, 5 | BRF_SND },           // 17
};

STDROMPICKEXT(garouha, garouha, neogeo)
STD_ROM_FN(garouha)

struct BurnDriver BurnDrvgarouha = {
	"garouha", "garou", "neogeo", NULL, "1999",
	"Garou - Mark of the Wolves (NGH-2530)\0", NULL, "SNK", "Neo Geo MVS",
	L"Garou\0\u9913\u72FC - mark of the wolves (NGH-2530)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_VSFIGHT, FBF_FATFURY,
	NULL, garouhaRomInfo, garouhaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	garouInit, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Garou - Mark of the Wolves (prototype)
/* Prototype Version, seems genuine */

static struct BurnRomInfo garoupRomDesc[] = {
	{ "proto_253-p1.p1",   0x100000, 0xc72f0c16, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "proto_253-p2.p2",   0x400000, 0xbf8de565, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "proto_253-s1.s1",   0x020000, 0x779989de, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "proto_253-c1.c1",   0x800000, 0x5bb5d137, 3 | BRF_GRA },           //  3 Sprite data
	{ "proto_253-c2.c2",   0x800000, 0x5c8d2960, 3 | BRF_GRA },           //  4
	{ "proto_253-c3.c3",   0x800000, 0x234d16fc, 3 | BRF_GRA },           //  5
	{ "proto_253-c4.c4",   0x800000, 0xb9b5b993, 3 | BRF_GRA },           //  6
	{ "proto_253-c5.c5",   0x800000, 0x722615d2, 3 | BRF_GRA },           //  7
	{ "proto_253-c6.c6",   0x800000, 0x0a6fab38, 3 | BRF_GRA },           //  8
	{ "proto_253-c7.c7",   0x800000, 0xd68e806f, 3 | BRF_GRA },           //  9
	{ "proto_253-c8.c8",   0x800000, 0xf778fe99, 3 | BRF_GRA },           // 10

	{ "proto_253-m1.m1",   0x040000, 0xbbe464f7, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "proto_253-v1.v1",   0x400000, 0x274f3923, 5 | BRF_SND },           // 12 Sound data
	{ "proto_253-v2.v2",   0x400000, 0x8f86dabe, 5 | BRF_SND },           // 13
	{ "proto_253-v3.v3",   0x400000, 0x05fd06cd, 5 | BRF_SND },           // 14
	{ "proto_253-v4.v4",   0x400000, 0x14984063, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(garoup, garoup, neogeo)
STD_ROM_FN(garoup)

struct BurnDriver BurnDrvGaroup = {
	"garoup", "garou", "neogeo", NULL, "1999",
	"Garou - Mark of the Wolves (prototype)\0", NULL, "SNK", "Neo Geo MVS",
	L"Garou\0\u9913\u72FC - mark of the wolves (prototype)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, garoupRomInfo, garoupRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Garou - Mark of the Wolves (bootleg)

static struct BurnRomInfo garoublRomDesc[] = {
	{ "garou-p1.bin", 0x0100000, 0xfd446d59, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "garou-p2.bin", 0x0400000, 0x3fb10a84, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "garou-s1.bin", 0x0020000, 0xdf720e33, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "garou-c1.bin", 0x1000000, 0xe2ac83fa, 3 | BRF_GRA },           //  3 Sprite data
	{ "garou-c2.bin", 0x1000000, 0x7c344b24, 3 | BRF_GRA },           //  4
	{ "garou-c3.bin", 0x1000000, 0xd3aec5a6, 3 | BRF_GRA },           //  5
	{ "garou-c4.bin", 0x1000000, 0xe02a242d, 3 | BRF_GRA },           //  6

	{ "garou-m1.bin", 0x0080000, 0x7c51d002, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code

	{ "garou-v1.bin", 0x0400000, 0x98d736e4, 5 | BRF_SND },           //  8 Sound data
	{ "garou-v2.bin", 0x0400000, 0xeb43c03f, 5 | BRF_SND },           //  9
	{ "garou-v3.bin", 0x0400000, 0x10a0f814, 5 | BRF_SND },           // 10
	{ "garou-v4.bin", 0x0400000, 0x8918fdd3, 5 | BRF_SND },           // 11
};

STDROMPICKEXT(garoubl, garoubl, neogeo)
STD_ROM_FN(garoubl)

static void garoubl_sx_decode()
{
	INT32 i;
	for (i = 0; i < 0x020000; i++)
		NeoTextROM[nNeoActiveSlot][i] = BITSWAP08(NeoTextROM[nNeoActiveSlot][i], 7, 6, 0, 4, 3, 2, 1, 5);
}

static void garoublCallback()
{
	garoubl_sx_decode();
	lans2004_cx_decode(0x800000 * 8);
}

static INT32 garoublInit()
{
	NeoCallbackActive->pInitialise = garoublCallback;

 	return NeoInit();
}

struct BurnDriver BurnDrvgaroubl = {
	"garoubl", "garou", "neogeo", NULL, "1999",
	"Garou - Mark of the Wolves (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	L"Garou\0\u9913\u72FC - mark of the wolves (bootleg)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, garoublRomInfo, garoublRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	garoublInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Metal Slug 3 (NGM-2560)
/* Original Version - Encrypted Code & GFX */ /* revision 2000.4.1 */ /* MVS VERSION */

static struct BurnRomInfo mslug3RomDesc[] = {
	{ "green.neo-sma",  0x040000, 0x9cd55736, 9 | BRF_ESS | BRF_PRG }, //  0 68K code		/ stored in the custom chip
	/* The SMA for this release has a green colour marking; the older revision has a white colour marking */
	{ "256-pg1.p1",   	0x400000, 0xb07edfd5, 1 | BRF_ESS | BRF_PRG }, //  1 				/ TC5332202
	{ "256-pg2.p2",   	0x400000, 0x6097c26b, 1 | BRF_ESS | BRF_PRG }, //  2 				/ TC5332202

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "256-c1.c1",    	0x800000, 0x5a79c34e, 3 | BRF_GRA },           //  3 Sprite data	/ TC5364202
	{ "256-c2.c2",    	0x800000, 0x944c362c, 3 | BRF_GRA },           //  4 				/ TC5364202
	{ "256-c3.c3",    	0x800000, 0x6e69d36f, 3 | BRF_GRA },           //  5 				/ TC5364202
	{ "256-c4.c4",    	0x800000, 0xb755b4eb, 3 | BRF_GRA },           //  6 				/ TC5364202
	{ "256-c5.c5",    	0x800000, 0x7aacab47, 3 | BRF_GRA },           //  7 				/ TC5364202
	{ "256-c6.c6",    	0x800000, 0xc698fd5d, 3 | BRF_GRA },           //  8 				/ TC5364202
	{ "256-c7.c7",    	0x800000, 0xcfceddd2, 3 | BRF_GRA },           //  9 				/ TC5364202
	{ "256-c8.c8",    	0x800000, 0x4d9be34c, 3 | BRF_GRA },           // 10 				/ TC5364202

	{ "256-m1.m1",    	0x080000, 0xeaeec116, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code		/ mask rom TC534000

	{ "256-v1.v1",    	0x400000, 0xf2690241, 5 | BRF_SND },           // 12 Sound data 	/ TC5332204
	{ "256-v2.v2",    	0x400000, 0x7e2a10bd, 5 | BRF_SND },           // 13 				/ TC5332204
	{ "256-v3.v3",    	0x400000, 0x0eaec17c, 5 | BRF_SND },           // 14 				/ TC5332204
	{ "256-v4.v4",    	0x400000, 0x9b4b22d4, 5 | BRF_SND },           // 15 				/ TC5332204
};

STDROMPICKEXT(mslug3, mslug3, neogeo)
STD_ROM_FN(mslug3)

static void mslug3SMADecrypt()
{
	for (INT32 i = 0; i < 0x800000 / 2; i++) {
		((UINT16*)(Neo68KROMActive + 0x100000))[i] = BITSWAP16(BURN_ENDIAN_SWAP_INT16(((UINT16*)(Neo68KROMActive + 0x100000))[i]), 4, 11, 14, 3, 1, 13, 0, 7, 2, 8, 12, 15, 10, 9, 5, 6);
	}

	for (INT32 i = 0; i < 0x0C0000 / 2; i++) {
		((UINT16*)Neo68KROMActive)[i] = BURN_ENDIAN_SWAP_INT16(((UINT16*)Neo68KROMActive)[0x5D0000 / 2 + BITSWAP24(i, 23, 22, 21, 20, 19, 18, 15, 2, 1, 13, 3, 0, 9, 6, 16, 4, 11, 5, 7, 12, 17, 14, 10, 8)]);
	}

	for (INT32 i = 0; i < 0x800000 / 2; i += 0x010000 / 2) {
		UINT16 nBuffer[0x010000 / 2];
		memcpy(nBuffer, &((UINT16*)(Neo68KROMActive + 0x100000))[i], 0x010000);
		for (INT32 j = 0; j < 0x010000 / 2; j++) {
			((UINT16*)(Neo68KROMActive + 0x100000))[i + j] = BURN_ENDIAN_SWAP_INT16(nBuffer[BITSWAP24(j, 23, 22, 21, 20, 19, 18, 17, 16, 15, 2, 11, 0, 14, 6, 4, 13, 8, 9, 3, 10, 7, 5, 12, 1)]);
		}
	}
}

void __fastcall mslug3WriteWordBankswitch(UINT32 sekAddress, UINT16 wordValue)
{

//	bprintf(PRINT_NORMAL, " -- bankswitch: 0x%08X -> 0x%04X\n", sekAddress, wordValue);

	if (sekAddress == 0x2FFFE4) {
		static UINT32 bankoffset[64] = {
			0x100000, 0x120000, 0x140000, 0x160000, // 00
			0x170000, 0x190000, 0x1b0000, 0x1d0000, // 04
			0x1e0000, 0x1f0000, 0x220000, 0x230000, // 08
			0x240000, 0x250000, 0x280000, 0x290000, // 12
			0x2a0000, 0x2b0000, 0x2e0000, 0x2f0000, // 16
			0x300000, 0x310000, 0x340000, 0x350000, // 20
			0x360000, 0x370000, 0x3a0000, 0x3b0000, // 24
			0x3c0000, 0x3d0000, 0x400000, 0x410000, // 28
			0x420000, 0x430000, 0x460000, 0x470000, // 32
			0x480000, 0x490000, 0x4c0000, 0x4d0000, // 36
			0x500000, 0x510000, 0x540000, 0x550000, // 40
			0x560000, 0x570000, 0x5a0000, 0x5b0000, // 44
			0x5c0000, /* rest not used? */
		};

		// Unscramble bank number
		INT32 nBank =
			(((wordValue >> 14) & 1) << 0) +
			(((wordValue >> 12) & 1) << 1) +
			(((wordValue >> 15) & 1) << 2) +
			(((wordValue >>  6) & 1) << 3) +
			(((wordValue >>  3) & 1) << 4) +
			(((wordValue >>  9) & 1) << 5);

		if (bankoffset[nBank] != nNeo68KROMBank) {
			nNeo68KROMBank = bankoffset[nBank];
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank,			  0x200000, 0x2FE3FF, MAP_ROM);
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank + 0x0FE800, 0x2FE800, 0x2FFFFF, MAP_ROM);
		}
	}
}

static INT32 mslug3Init()
{
	nNeoProtectionXor = 0xAD;

	return NeoSMAInit(mslug3SMADecrypt, mslug3WriteWordBankswitch, 0, 0);
}

struct BurnDriver BurnDrvmslug3 = {
	"mslug3", NULL, "neogeo", NULL, "2000",
	"Metal Slug 3 (NGM-2560)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug3RomInfo, mslug3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug3Init, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};


// Metal Slug 3 (NGM-2560, earlier)
/* Original Version - Encrypted Code & GFX */ /* MVS VERSION */

static struct BurnRomInfo mslug3aRomDesc[] = {
	{ "white.neo-sma",	0x040000, 0xc60d29b2, 9 | BRF_ESS | BRF_PRG }, //  0 68K code		/ stored in the custom chip
	{ "256.p1",	  		0x400000, 0xa1177628, 1 | BRF_ESS | BRF_PRG }, //  1 				/ TC5332202
	{ "256.p2",	  		0x400000, 0x9b659826, 1 | BRF_ESS | BRF_PRG }, //  2				/ TC5332202

	{ "256-c1.c1",    	0x800000, 0x5a79c34e, 3 | BRF_GRA },           //  3 Sprite data	/ TC5364202
	{ "256-c2.c2",    	0x800000, 0x944c362c, 3 | BRF_GRA },           //  4 				/ TC5364202
	{ "256-c3.c3",    	0x800000, 0x6e69d36f, 3 | BRF_GRA },           //  5 				/ TC5364202
	{ "256-c4.c4",    	0x800000, 0xb755b4eb, 3 | BRF_GRA },           //  6 				/ TC5364202
	{ "256-c5.c5",    	0x800000, 0x7aacab47, 3 | BRF_GRA },           //  7 				/ TC5364202
	{ "256-c6.c6",    	0x800000, 0xc698fd5d, 3 | BRF_GRA },           //  8 				/ TC5364202
	{ "256-c7.c7",    	0x800000, 0xcfceddd2, 3 | BRF_GRA },           //  9 				/ TC5364202
	{ "256-c8.c8",    	0x800000, 0x4d9be34c, 3 | BRF_GRA },           // 10 				/ TC5364202

	{ "256-m1.m1",    	0x080000, 0xeaeec116, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code		/ mask rom TC534000

	{ "256-v1.v1",    	0x400000, 0xf2690241, 5 | BRF_SND },           // 12 Sound data 	/ TC5332204
	{ "256-v2.v2",    	0x400000, 0x7e2a10bd, 5 | BRF_SND },           // 13 				/ TC5332204
	{ "256-v3.v3",    	0x400000, 0x0eaec17c, 5 | BRF_SND },           // 14 				/ TC5332204
	{ "256-v4.v4",    	0x400000, 0x9b4b22d4, 5 | BRF_SND },           // 15 				/ TC5332204
};

STDROMPICKEXT(mslug3a, mslug3a, neogeo)
STD_ROM_FN(mslug3a)

static void mslug3aSMADecrypt()
{
	for (INT32 i = 0; i < 0x800000 / 2; i++) {
		((UINT16*)(Neo68KROMActive + 0x100000))[i] = BITSWAP16(BURN_ENDIAN_SWAP_INT16(((UINT16*)(Neo68KROMActive + 0x100000))[i]), 2,11,12,14,9,3,1,4,13,7,6,8,10,15,0,5);
	}

	for (INT32 i = 0; i < 0x0C0000 / 2; i++) {
		((UINT16*)Neo68KROMActive)[i] = BURN_ENDIAN_SWAP_INT16(((UINT16*)Neo68KROMActive)[0x5D0000 / 2 + BITSWAP24(i, 23,22,21,20,19,18,1,16,14,7,17,5,8,4,15,6,3,2,0,13,10,12,9,11)]);
	}

	for (INT32 i = 0; i < 0x800000 / 2; i += 0x010000 / 2) {
		UINT16 nBuffer[0x010000 / 2];
		memcpy(nBuffer, &((UINT16*)(Neo68KROMActive + 0x100000))[i], 0x010000);
		for (INT32 j = 0; j < 0x010000 / 2; j++) {
			((UINT16*)(Neo68KROMActive + 0x100000))[i + j] = BURN_ENDIAN_SWAP_INT16(nBuffer[BITSWAP24(j, 23,22,21,20,19,18,17,16,15,12,0,11,3,4,13,6,8,14,7,5,2,10,9,1)]);
		}
	}
}

void __fastcall mslug3aWriteWordBankswitch(UINT32 sekAddress, UINT16 wordValue)
{
//	bprintf(PRINT_NORMAL, " -- bankswitch: 0x%08X -> 0x%04X\n", sekAddress, wordValue);

	if (sekAddress == 0x2FFFE4) {
		static const UINT32 bankoffset[64] = {
			0x100000, 0x130000, 0x140000, 0x170000, // 00
			0x180000, 0x1a0000, 0x1c0000, 0x1e0000, // 04
			0x1f0000, 0x200000, 0x230000, 0x240000, // 08
			0x250000, 0x260000, 0x290000, 0x2a0000, // 12
			0x2B0000, 0x2C0000, 0x2F0000, 0x300000, // 16
			0x310000, 0x320000, 0x350000, 0x360000, // 20
			0x370000, 0x380000, 0x3B0000, 0x3C0000, // 24
			0x3D0000, 0x3E0000, 0x410000, 0x420000, // 28
			0x430000, 0x440000, 0x470000, 0x480000, // 32
			0x490000, 0x4A0000, 0x4D0000, 0x4E0000, // 36
			0x500000, 0x510000, 0x540000, 0x550000, // 40
			0x560000, 0x570000, 0x5A0000, 0x5B0000, // 44
			0x5C0000, /* rest not used? */
		};

		// Unscramble bank number
		INT32 nBank =
			(((wordValue >> 15) & 1) << 0) +
			(((wordValue >>  3) & 1) << 1) +
			(((wordValue >>  1) & 1) << 2) +
			(((wordValue >>  6) & 1) << 3) +
			(((wordValue >> 12) & 1) << 4) +
			(((wordValue >> 11) & 1) << 5);

		if (bankoffset[nBank] != nNeo68KROMBank) {
			nNeo68KROMBank = bankoffset[nBank];
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank,		      0x200000, 0x2FE3FF, MAP_ROM);
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank + 0x0FE800, 0x2FE800, 0x2FFFFF, MAP_ROM);
		}
	}
}

static INT32 mslug3aInit()
{
	nNeoProtectionXor = 0xAD;

	return NeoSMAInit(mslug3aSMADecrypt, mslug3aWriteWordBankswitch, 0, 0);
}

struct BurnDriver BurnDrvmslug3a = {
	"mslug3a", "mslug3", "neogeo", NULL, "2000",
	"Metal Slug 3 (NGM-2560, earlier)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug3aRomInfo, mslug3aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug3aInit, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};


// Metal Slug 3 (NGH-2560)
/* Original Version - Encrypted GFX */ /* revision 2000.3.17 */ /* AES VERSION */

static struct BurnRomInfo mslug3hRomDesc[] = {
	{ "256-ph1.p1",   0x100000, 0x9c42ca85, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "256-ph2.sp2",  0x400000, 0x1f3d8ce8, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "256-c1.c1",    0x800000, 0x5a79c34e, 3 | BRF_GRA },           //  2 Sprite data 	/ TC5364202
	{ "256-c2.c2",    0x800000, 0x944c362c, 3 | BRF_GRA },           //  3 				/ TC5364202
	{ "256-c3.c3",    0x800000, 0x6e69d36f, 3 | BRF_GRA },           //  4 				/ TC5364202
	{ "256-c4.c4",    0x800000, 0xb755b4eb, 3 | BRF_GRA },           //  5 				/ TC5364202
	{ "256-c5.c5",    0x800000, 0x7aacab47, 3 | BRF_GRA },           //  6 				/ TC5364202
	{ "256-c6.c6",    0x800000, 0xc698fd5d, 3 | BRF_GRA },           //  7 				/ TC5364202
	{ "256-c7.c7",    0x800000, 0xcfceddd2, 3 | BRF_GRA },           //  8 				/ TC5364202
	{ "256-c8.c8",    0x800000, 0x4d9be34c, 3 | BRF_GRA },           //  9 				/ TC5364202

	{ "256-m1.m1",    0x080000, 0xeaeec116, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code		/ mask rom TC534000

	{ "256-v1.v1",    0x400000, 0xf2690241, 5 | BRF_SND },           // 11 Sound data	/ TC5332204
	{ "256-v2.v2",    0x400000, 0x7e2a10bd, 5 | BRF_SND },           // 12 				/ TC5332204
	{ "256-v3.v3",    0x400000, 0x0eaec17c, 5 | BRF_SND },           // 13 				/ TC5332204
	{ "256-v4.v4",    0x400000, 0x9b4b22d4, 5 | BRF_SND },           // 14 				/ TC5332204
};

STDROMPICKEXT(mslug3h, mslug3h, neogeo)
STD_ROM_FN(mslug3h)

static INT32 mslug3hInit()
{
	nNeoProtectionXor = 0xAD;

	return NeoInit();
}

struct BurnDriver BurnDrvmslug3h = {
	"mslug3h", "mslug3", "neogeo", NULL, "2000",
	"Metal Slug 3 (NGH-2560)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug3hRomInfo, mslug3hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug3hInit, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 6 (Metal Slug 3 bootleg)

static struct BurnRomInfo mslug3b6RomDesc[] = {
	{ "299-p1.bin",   0x200000, 0x5f2fe228, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "299-p2.bin",   0x400000, 0x193fa835, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "299-s1.bin",   0x020000, 0x6f8b9635, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "256-c1.c1",    0x800000, 0x5a79c34e, 3 | BRF_GRA },           //  3 Sprite data
	{ "256-c2.c2",    0x800000, 0x944c362c, 3 | BRF_GRA },           //  4
	{ "256-c3.c3",    0x800000, 0x6e69d36f, 3 | BRF_GRA },           //  5
	{ "256-c4.c4",    0x800000, 0xb755b4eb, 3 | BRF_GRA },           //  6
	{ "256-c5.c5",    0x800000, 0x7aacab47, 3 | BRF_GRA },           //  7
	{ "256-c6.c6",    0x800000, 0xc698fd5d, 3 | BRF_GRA },           //  8
	{ "256-c7.c7",    0x800000, 0xcfceddd2, 3 | BRF_GRA },           //  9
	{ "256-c8.c8",    0x800000, 0x4d9be34c, 3 | BRF_GRA },           // 10

	{ "256-m1.m1",    0x080000, 0xeaeec116, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "256-v1.v1",    0x400000, 0xf2690241, 5 | BRF_SND },           // 12 Sound data
	{ "256-v2.v2",    0x400000, 0x7e2a10bd, 5 | BRF_SND },           // 13
	{ "256-v3.v3",    0x400000, 0x0eaec17c, 5 | BRF_SND },           // 14
	{ "256-v4.v4",    0x400000, 0x9b4b22d4, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(mslug3b6, mslug3b6, neogeo)
STD_ROM_FN(mslug3b6)

static void mslug3b6Callback()
{
	memmove(Neo68KROMActive, Neo68KROMActive + 0x100000, 0x500000);

	garoubl_sx_decode();
}

static INT32 mslug3b6Init()
{
	NeoCallbackActive->pInitialise = mslug3b6Callback;
	nNeoProtectionXor = 0xAD;

 	return NeoInit();
}

struct BurnDriver BurnDrvmslug3b6 = {
	"mslug3b6", "mslug3", "neogeo", NULL, "2000",
	"Metal Slug 6 (Metal Slug 3 bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug3b6RomInfo, mslug3b6RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug3b6Init, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters 2000 (NGM-2570) (NGH-2570)
/* Original Version, Encrypted Code + Sound + GFX Roms */ /* MVS AND AES VERSION */

static struct BurnRomInfo kof2000RomDesc[] = {
	{ "neo-sma",      0x040000, 0x71c6e6bb, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "257-p1.p1",    0x400000, 0x60947b4c, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "257-p2.p2",    0x400000, 0x1b7ec415, 1 | BRF_ESS | BRF_PRG }, //  2

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "257-c1.c1",    0x800000, 0xcef1cdfa, 3 | BRF_GRA },           //  3 Sprite data
	{ "257-c2.c2",    0x800000, 0xf7bf0003, 3 | BRF_GRA },           //  4
	{ "257-c3.c3",    0x800000, 0x101e6560, 3 | BRF_GRA },           //  5
	{ "257-c4.c4",    0x800000, 0xbd2fc1b1, 3 | BRF_GRA },           //  6
	{ "257-c5.c5",    0x800000, 0x89775412, 3 | BRF_GRA },           //  7
	{ "257-c6.c6",    0x800000, 0xfa7200d5, 3 | BRF_GRA },           //  8
	{ "257-c7.c7",    0x800000, 0x7da11fe4, 3 | BRF_GRA },           //  9
	{ "257-c8.c8",    0x800000, 0xb1afa60b, 3 | BRF_GRA },           // 10

	{ "257-m1.m1",    0x040000, 0x4b749113, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "257-v1.v1",    0x400000, 0x17cde847, 5 | BRF_SND },           // 12 Sound data
	{ "257-v2.v2",    0x400000, 0x1afb20ff, 5 | BRF_SND },           // 13
	{ "257-v3.v3",    0x400000, 0x4605036a, 5 | BRF_SND },           // 14
	{ "257-v4.v4",    0x400000, 0x764bbd6b, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof2000, kof2000, neogeo)
STD_ROM_FN(kof2000)

static void kof2000SMADecrypt()
{
	for (INT32 i = 0; i < 0x800000 / 2; i++) {
		((UINT16*)(Neo68KROMActive + 0x100000))[i] = BITSWAP16(BURN_ENDIAN_SWAP_INT16(((UINT16*)(Neo68KROMActive + 0x100000))[i]), 12, 8, 11, 3, 15, 14, 7, 0, 10, 13, 6, 5, 9, 2, 1, 4);
	}

	for (INT32 i = 0; i < 0x0C0000 / 2; i++) {
		((UINT16*)Neo68KROMActive)[i] = BURN_ENDIAN_SWAP_INT16(((UINT16*)Neo68KROMActive)[0x73A000 / 2 + BITSWAP24(i, 23, 22, 21, 20, 19, 18, 8, 4, 15, 13, 3, 14, 16, 2, 6, 17, 7, 12, 10, 0, 5, 11, 1, 9)]);
	}

	for (INT32 i = 0; i < 0x63A000 / 2; i += 0x0800 / 2) {
		UINT16 nBuffer[0x0800 / 2];
		memcpy(nBuffer, &((UINT16*)(Neo68KROMActive + 0x100000))[i], 0x0800);
		for (INT32 j = 0; j < 0x0800 / 2; j++) {
			((UINT16*)(Neo68KROMActive + 0x100000))[i + j] = BURN_ENDIAN_SWAP_INT16(nBuffer[BITSWAP24(j, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 4, 1, 3, 8, 6, 2, 7, 0, 9, 5)]);
		}
	}
}

void __fastcall kof2000WriteWordBankswitch(UINT32 sekAddress, UINT16 wordValue)
{
	if (sekAddress == 0x2FFFEC) {
		static UINT32 bankoffset[64] = {
			0x100000, 0x200000, 0x300000, 0x400000, // 00
			0x4f7800, 0x5f7800, 0x4ff800, 0x5ff800, // 04
			0x507800, 0x607800, 0x50f800, 0x60f800, // 08
			0x516800, 0x616800, 0x51d800, 0x61d800, // 12
			0x524000, 0x624000, 0x623800, 0x723800, // 16
			0x626000, 0x726000, 0x628000, 0x728000, // 20
			0x62a000, 0x72a000, 0x62b800, 0x72b800, // 24
			0x62d000, 0x72d000, 0x62e800, 0x72e800, // 28
			0x718000, 0x719000, 0x71a000, 0x71a800, // 32
		};

		// Unscramble bank number
		INT32 nBank =
			(((wordValue >> 15) & 1) << 0) +
			(((wordValue >> 14) & 1) << 1) +
			(((wordValue >>  7) & 1) << 2) +
			(((wordValue >>  3) & 1) << 3) +
			(((wordValue >> 10) & 1) << 4) +
			(((wordValue >>  5) & 1) << 5);

		if (bankoffset[nBank] != nNeo68KROMBank) {
			nNeo68KROMBank = bankoffset[nBank];
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank,			  0x200000, 0x2FE3FF, MAP_ROM);
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank + 0x0FE800, 0x2FE800, 0x2FFBFF, MAP_ROM);
		}
	}
}

static INT32 kof2000Init()
{
	nNeoProtectionXor = 0x00;

	return NeoSMAInit(kof2000SMADecrypt, kof2000WriteWordBankswitch, 0x2FFFD8, 0x2FFFDA);
}

struct BurnDriver BurnDrvkof2000 = {
	"kof2000", NULL, "neogeo", NULL, "2000",
	"The King of Fighters 2000 (NGM-2570) (NGH-2570)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_SMA_PROTECTION | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2000RomInfo, kof2000RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2000Init, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters 2000 (not encrypted)
/* Original Version, Encrypted Sound + GFX Roms */

static struct BurnRomInfo kof2000nRomDesc[] = {
	{ "257-pg1.p1",   0x100000, 0x5f809dbe, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "257-pg2.sp2",  0x400000, 0x693c2c5e, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "257-c1.c1",    0x800000, 0xcef1cdfa, 3 | BRF_GRA },           //  2 Sprite data
	{ "257-c2.c2",    0x800000, 0xf7bf0003, 3 | BRF_GRA },           //  3
	{ "257-c3.c3",    0x800000, 0x101e6560, 3 | BRF_GRA },           //  4
	{ "257-c4.c4",    0x800000, 0xbd2fc1b1, 3 | BRF_GRA },           //  5
	{ "257-c5.c5",    0x800000, 0x89775412, 3 | BRF_GRA },           //  6
	{ "257-c6.c6",    0x800000, 0xfa7200d5, 3 | BRF_GRA },           //  7
	{ "257-c7.c7",    0x800000, 0x7da11fe4, 3 | BRF_GRA },           //  8
	{ "257-c8.c8",    0x800000, 0xb1afa60b, 3 | BRF_GRA },           //  9

	{ "257-m1.m1",    0x040000, 0x4b749113, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "257-v1.v1",    0x400000, 0x17cde847, 5 | BRF_SND },           // 11 Sound data
	{ "257-v2.v2",    0x400000, 0x1afb20ff, 5 | BRF_SND },           // 12
	{ "257-v3.v3",    0x400000, 0x4605036a, 5 | BRF_SND },           // 13
	{ "257-v4.v4",    0x400000, 0x764bbd6b, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(kof2000n, kof2000n, neogeo)
STD_ROM_FN(kof2000n)

static INT32 kof2000nInit()
{
	nNeoProtectionXor = 0x00;
	return NeoInit();
}

struct BurnDriver BurnDrvkof2000n = {
	"kof2000n", "kof2000", "neogeo", NULL, "2000",
	"The King of Fighters 2000 (not encrypted)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2000nRomInfo, kof2000nRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2000nInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Zupapa!
/* Original Version - Encrypted GFX */ /* MVS ONLY RELEASE */

static struct BurnRomInfo zupapaRomDesc[] = {
	{ "070-p1.p1",    0x100000, 0x5a96203e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 	/ mask rom TC538200

	/* The Encrypted Boards do _not_ have an s1 rom, data for it comes from the Cx ROMs */
	{ "070-c1.c1",    0x800000, 0xf8ad02d8, 3 | BRF_GRA },           //  1 Sprite data 	/ mask rom TC5364205
	{ "070-c2.c2",    0x800000, 0x70156dde, 3 | BRF_GRA },           //  2 				/ mask rom TC5364205

	{ "070-epr.m1",   0x020000, 0x5a3b3191, 4 | BRF_ESS | BRF_PRG }, //  3 Z80 code		/ M27C1001
	/* M1 on eprom, correct chip label unknown */

	{ "070-v1.v1",    0x200000, 0xd3a7e1ff, 5 | BRF_SND },           //  4 Sound data	/ mask rom TC5316200
};

STDROMPICKEXT(zupapa, zupapa, neogeo)
STD_ROM_FN(zupapa)

static INT32 zupapaInit()
{
	nNeoProtectionXor = 0xBD;
	return NeoInit();
}

struct BurnDriver BurnDrvZupapa = {
	"zupapa", NULL, "neogeo", NULL, "2001",
	"Zupapa!\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42, GBF_PLATFORM, 0,
	NULL, zupapaRomInfo, zupapaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	zupapaInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Sengoku 3 / Sengoku Densho 2001
/* Original Version - Encrypted GFX */

static struct BurnRomInfo sengoku3RomDesc[] = {
	/* also found AES set with P1 on maskrom on PROG board NEO-AEG PROGBK1F; chip label is 261-PH1
	and CHA board NEO-AEG CHAFIO (1999.8.10) (NEO-CMC 7042)	*/
	{ "261-ph1.p1",   0x200000, 0xe0d4bc0a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "261-c1.c1",    0x800000, 0xded84d9c, 3 | BRF_GRA },           //  1 Sprite data
	{ "261-c2.c2",    0x800000, 0xb8eb4348, 3 | BRF_GRA },           //  2
	{ "261-c3.c3",    0x800000, 0x84e2034a, 3 | BRF_GRA },           //  3
	{ "261-c4.c4",    0x800000, 0x0b45ae53, 3 | BRF_GRA },           //  4

	{ "261-m1.m1",    0x080000, 0x7d501c39, 4 | BRF_ESS | BRF_PRG }, //  5 Z80 code

	{ "261-v1.v1",    0x400000, 0x64c30081, 5 | BRF_SND },           //  6 Sound data
	{ "261-v2.v2",    0x400000, 0x392a9c47, 5 | BRF_SND },           //  7
	{ "261-v3.v3",    0x400000, 0xc1a7ebe3, 5 | BRF_SND },           //  8
	{ "261-v4.v4",    0x200000, 0x9000d085, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(sengoku3, sengoku3, neogeo)
STD_ROM_FN(sengoku3)

static INT32 sengoku3Init()
{
	nNeoProtectionXor = 0xFE;
	return NeoInit();
}

struct BurnDriver BurnDrvSengoku3 = {
	"sengoku3", NULL, "neogeo", NULL, "2001",
	"Sengoku 3 / Sengoku Densho 2001 (set 1)\0", NULL, "SNK / Noise Factory", "Neo Geo MVS",
	L"Sengoku 3\0\u6226\u56FD\u4F1D\u627F\uFF12\uFF10\uFF10\uFF11 (Set 1)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SWAPP, GBF_SCRFIGHT, 0,
	NULL, sengoku3RomInfo, sengoku3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	sengoku3Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Sengoku 3 / Sengoku Densho 2001
/* Original Version - Encrypted GFX */

static struct BurnRomInfo sengoku3aRomDesc[] = {
	{ "261-pg1.p1",   0x200000, 0x5b557201, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "261-c1.c1",    0x800000, 0xded84d9c, 3 | BRF_GRA },           //  1 Sprite data
	{ "261-c2.c2",    0x800000, 0xb8eb4348, 3 | BRF_GRA },           //  2
	{ "261-c3.c3",    0x800000, 0x84e2034a, 3 | BRF_GRA },           //  3
	{ "261-c4.c4",    0x800000, 0x0b45ae53, 3 | BRF_GRA },           //  4

	{ "261-m1.m1",    0x080000, 0x7d501c39, 4 | BRF_ESS | BRF_PRG }, //  5 Z80 code

	{ "261-v1.v1",    0x400000, 0x64c30081, 5 | BRF_SND },           //  6 Sound data
	{ "261-v2.v2",    0x400000, 0x392a9c47, 5 | BRF_SND },           //  7
	{ "261-v3.v3",    0x400000, 0xc1a7ebe3, 5 | BRF_SND },           //  8
	{ "261-v4.v4",    0x200000, 0x9000d085, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(sengoku3a, sengoku3a, neogeo)
STD_ROM_FN(sengoku3a)

struct BurnDriver BurnDrvSengoku3a = {
	"sengoku3a", "sengoku3", "neogeo", NULL, "2001",
	"Sengoku 3 / Sengoku Densho 2001 (Set 2)\0", NULL, "SNK / Noise Factory", "Neo Geo MVS",
	L"Sengoku 3\0\u6226\u56FD\u4F1D\u627F\uFF12\uFF10\uFF10\uFF11 (Set 2)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SWAPP, GBF_SCRFIGHT, 0,
	NULL, sengoku3aRomInfo, sengoku3aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	sengoku3Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// The King of Fighters 2001 (NGM-262?)
/* MVS VERSION */

static struct BurnRomInfo kof2001RomDesc[] = {
	{ "262-p1-08-e0.p1",       0x100000, 0x9381750d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "262-p2-08-e0.sp2",      0x400000, 0x8e0d8329, 1 | BRF_ESS | BRF_PRG }, //  1
	/* The first/early production run sets have proms with above labels. Some later? sets found have eproms instead of proms */

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "262-c1-08-e0.c1",       0x800000, 0x99cc785a, 3 | BRF_GRA },     //  2 Sprite data
	{ "262-c2-08-e0.c2",       0x800000, 0x50368cbf, 3 | BRF_GRA },     //  3
	{ "262-c3-08-e0.c3",       0x800000, 0xfb14ff87, 3 | BRF_GRA },     //  4
	{ "262-c4-08-e0.c4",       0x800000, 0x4397faf8, 3 | BRF_GRA },     //  5
	{ "262-c5-08-e0.c5",       0x800000, 0x91f24be4, 3 | BRF_GRA },     //  6
	{ "262-c6-08-e0.c6",       0x800000, 0xa31e4403, 3 | BRF_GRA },     //  7
	{ "262-c7-08-e0.c7",       0x800000, 0x54d9d1ec, 3 | BRF_GRA },     //  8
	{ "262-c8-08-e0.c8",       0x800000, 0x59289a6b, 3 | BRF_GRA },     //  9

	{ "265-262-m1.m1",         0x040000, 0xa7f8119f, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "262-v1-08-e0.v1",       0x400000, 0x83d49ecf, 5 | BRF_SND },     // 11 Sound data
	{ "262-v2-08-e0.v2",       0x400000, 0x003f1843, 5 | BRF_SND },     // 12
	{ "262-v3-08-e0.v3",       0x400000, 0x2ae38dbe, 5 | BRF_SND },     // 13
	{ "262-v4-08-e0.v4",       0x400000, 0x26ec4dd9, 5 | BRF_SND },     // 14
};

STDROMPICKEXT(kof2001, kof2001, neogeo)
STD_ROM_FN(kof2001)

static INT32 kof2001Init()
{
	nNeoProtectionXor = 0x1E;
	return NeoInit();
}

struct BurnDriver BurnDrvkof2001 = {
	"kof2001", NULL, "neogeo", NULL, "2001",
	"The King of Fighters 2001 (NGM-262?)\0", NULL, "SNK / Eolith", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2001RomInfo, kof2001RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2001Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters 2001 (NGH-2621)
/* AES VERSION */

static struct BurnRomInfo kof2001hRomDesc[] = {
	{ "262-pg1.p1",            0x100000, 0x2af7e741, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "262-pg2.sp2",           0x400000, 0x91eea062, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "262-c1-08-e0.c1",       0x800000, 0x99cc785a, 3 | BRF_GRA },     //  2 Sprite data
	{ "262-c2-08-e0.c2",       0x800000, 0x50368cbf, 3 | BRF_GRA },     //  3
	{ "262-c3-08-e0.c3",       0x800000, 0xfb14ff87, 3 | BRF_GRA },     //  4
	{ "262-c4-08-e0.c4",       0x800000, 0x4397faf8, 3 | BRF_GRA },     //  5
	{ "262-c5-08-e0.c5",       0x800000, 0x91f24be4, 3 | BRF_GRA },     //  6
	{ "262-c6-08-e0.c6",       0x800000, 0xa31e4403, 3 | BRF_GRA },     //  7
	{ "262-c7-08-e0.c7",       0x800000, 0x54d9d1ec, 3 | BRF_GRA },     //  8
	{ "262-c8-08-e0.c8",       0x800000, 0x59289a6b, 3 | BRF_GRA },     //  9

	{ "265-262-m1.m1",         0x040000, 0xa7f8119f, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "262-v1-08-e0.v1",       0x400000, 0x83d49ecf, 5 | BRF_SND },     // 11 Sound data
	{ "262-v2-08-e0.v2",       0x400000, 0x003f1843, 5 | BRF_SND },     // 12
	{ "262-v3-08-e0.v3",       0x400000, 0x2ae38dbe, 5 | BRF_SND },     // 13
	{ "262-v4-08-e0.v4",       0x400000, 0x26ec4dd9, 5 | BRF_SND },     // 14
};

STDROMPICKEXT(kof2001h, kof2001h, neogeo)
STD_ROM_FN(kof2001h)

struct BurnDriver BurnDrvkof2001h = {
	"kof2001h", "kof2001", "neogeo", NULL, "2001",
	"The King of Fighters 2001 (NGH-2621)\0", NULL, "SNK / Eolith", "Neo Geo AES",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2001hRomInfo, kof2001hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2001Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Crouching Tiger Hidden Dragon 2003 (The King of Fighters 2001 bootleg)

static struct BurnRomInfo cthd2003RomDesc[] = {
	{ "5003-p1.bin",  0x100000, 0xbb7602c1, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "5003-p2.bin",  0x400000, 0xadc1c22b, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "5003-s1.bin",  0x020000, 0x5ba29aab, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "5003-c1.bin",  0x800000, 0x68f54b67, 3 | BRF_GRA },           //  3 Sprite data
	{ "5003-c2.bin",  0x800000, 0x2f8849d5, 3 | BRF_GRA },           //  4
	{ "5003-c3.bin",  0x800000, 0xac4aff71, 3 | BRF_GRA },           //  5
	{ "5003-c4.bin",  0x800000, 0xafef5d66, 3 | BRF_GRA },           //  6
	{ "5003-c5.bin",  0x800000, 0xc7c1ae50, 3 | BRF_GRA },           //  7
	{ "5003-c6.bin",  0x800000, 0x613197f9, 3 | BRF_GRA },           //  8
	{ "5003-c7.bin",  0x800000, 0x64ddfe0f, 3 | BRF_GRA },           //  9
	{ "5003-c8.bin",  0x800000, 0x917a1439, 3 | BRF_GRA },           // 10

	{ "5003-m1.bin",  0x020000, 0x1a8c274b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "262-v1-08-e0.v1",    0x400000, 0x83d49ecf, 5 | BRF_SND },     // 12 Sound data
	{ "262-v2-08-e0.v2",    0x400000, 0x003f1843, 5 | BRF_SND },     // 13
	{ "262-v3-08-e0.v3",    0x400000, 0x2ae38dbe, 5 | BRF_SND },     // 14
	{ "262-v4-08-e0.v4",    0x400000, 0x26ec4dd9, 5 | BRF_SND },     // 15
};

STDROMPICKEXT(cthd2003, cthd2003, neogeo)
STD_ROM_FN(cthd2003)

static void cthd2003Bankswitch(UINT32 nBank)
{
	static UINT32 cthd2003_banks[8] =	{
		0x200000, 0x100000, 0x200000, 0x100000, 0x200000, 0x100000, 0x400000, 0x300000,
	};

	nBank = cthd2003_banks[nBank & 7];

	if (nBank != nNeo68KROMBank) {
		nNeo68KROMBank = nBank;
		SekMapMemory(Neo68KROMActive + nNeo68KROMBank, 0x200000, 0x2fffff, MAP_ROM);
	}
}

void __fastcall cthd2003WriteByteBankswitch(UINT32 sekAddress, UINT8 byteValue)
{
	if (sekAddress == 0x2ffff0) {
		cthd2003Bankswitch(byteValue);
	}
}

void __fastcall cthd2003WriteWordBankswitch(UINT32 sekAddress, UINT16 wordValue)
{
	if (sekAddress == 0x2ffff0) {
		cthd2003Bankswitch(wordValue);
	}
}
static void cthd2003_decode()
{
	INT32 i, n;

	// fix garbage on s1 layer over everything
	*((UINT16*)(Neo68KROMActive + 0xf415a)) = BURN_ENDIAN_SWAP_INT16(0x4ef9);
	*((UINT16*)(Neo68KROMActive + 0xf415c)) = BURN_ENDIAN_SWAP_INT16(0x000f);
	*((UINT16*)(Neo68KROMActive + 0xf415e)) = BURN_ENDIAN_SWAP_INT16(0x4cf2);

	// Fix corruption in attract mode before title screen
	for (i = 0x1ae290; i < 0x1ae8d0; i+=2) {
		*((UINT16*)(Neo68KROMActive + i)) = BURN_ENDIAN_SWAP_INT16(0x0000);
	}

	// Fix for title page
	for (i = 0x1f8ef0; i < 0x1fa1f0; i += 4) {
		*((UINT16*)(Neo68KROMActive + i + 0)) -= BURN_ENDIAN_SWAP_INT16(0x7000);
		*((UINT16*)(Neo68KROMActive + i + 2)) -= BURN_ENDIAN_SWAP_INT16(0x0010);
	}

	// Fix for green dots on title page
	for (i = 0xac500; i < 0xac520; i+=2) {
		*((UINT16*)(Neo68KROMActive + i)) = BURN_ENDIAN_SWAP_INT16(0xffff);
	}

	// Fix for blanks as screen change level end clear
	*((UINT16*)(Neo68KROMActive + 0x991d0)) = BURN_ENDIAN_SWAP_INT16(0xdd03);
	*((UINT16*)(Neo68KROMActive + 0x99306)) = BURN_ENDIAN_SWAP_INT16(0xdd03);
	*((UINT16*)(Neo68KROMActive + 0x99354)) = BURN_ENDIAN_SWAP_INT16(0xdd03);
	*((UINT16*)(Neo68KROMActive + 0x9943e)) = BURN_ENDIAN_SWAP_INT16(0xdd03);

	// Swap bits 15 & 16 in the address of the Z80 ROM
	for (i = 0; i < 0x10000 / 2; i++) {
		n = NeoZ80ROMActive[0x08000 + i];
		NeoZ80ROMActive[0x08000 + i] = NeoZ80ROMActive[0x10000 + i];
		NeoZ80ROMActive[0x10000 + i] = n;
	}

	DoPerm(0);
}

static void cthd2003Callback()
{
	INT32 i, n;
	for (i = 0; i < 0x8000; i++)
	{
		n = NeoTextROM[nNeoActiveSlot][0x08000 + i];
		NeoTextROM[nNeoActiveSlot][0x08000 + i] = NeoTextROM[nNeoActiveSlot][0x10000 + i];
		NeoTextROM[nNeoActiveSlot][0x10000 + i] = n;
	}

	cthd2003_decode();
}

static void cthd2003InstallBankSwitchHandler()
{
	SekMapHandler(6,	0x200000, 0x2fffff, MAP_WRITE);
	SekSetWriteWordHandler(6, cthd2003WriteWordBankswitch);
	SekSetWriteByteHandler(6, cthd2003WriteByteBankswitch);

	cthd2003Bankswitch(0);
}

static INT32 cthd2003Init()
{
	nBurnCPUSpeedAdjust = 0x010a;

	NeoCallbackActive->pInitialise = cthd2003Callback;
	NeoCallbackActive->pInstallHandlers = cthd2003InstallBankSwitchHandler;
	NeoCallbackActive->pBankswitch = NeoMapBank;

 	return NeoInit();
}

struct BurnDriver BurnDrvcthd2003 = {
	"cthd2003", "kof2001", "neogeo", NULL, "2003",
	"Crouching Tiger Hidden Dragon 2003 (set 1)\0", "Hack of \"The King of Fighters 2001\"", "Phenixsoft", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO,  GBF_VSFIGHT, FBF_KOF,
	NULL, cthd2003RomInfo, cthd2003RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	cthd2003Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Crouching Tiger Hidden Dragon 2003 Super Plus (The King of Fighters 2001 bootleg)

static struct BurnRomInfo ct2k3spRomDesc[] = {
	{ "5003-p1sp.bin",  0x100000, 0xab5c4de0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "5003-p2.bin",    0x400000, 0xadc1c22b, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "5003-s1sp.bin",  0x040000, 0x6c355ab4, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "5003-c1.bin",    0x800000, 0x68f54b67, 3 | BRF_GRA },           //  3 Sprite data
	{ "5003-c2.bin",    0x800000, 0x2f8849d5, 3 | BRF_GRA },           //  4
	{ "5003-c3.bin",    0x800000, 0xac4aff71, 3 | BRF_GRA },           //  5
	{ "5003-c4.bin",    0x800000, 0xafef5d66, 3 | BRF_GRA },           //  6
	{ "5003-c5.bin",    0x800000, 0xc7c1ae50, 3 | BRF_GRA },           //  7
	{ "5003-c6.bin",    0x800000, 0x613197f9, 3 | BRF_GRA },           //  8
	{ "5003-c7.bin",    0x800000, 0x64ddfe0f, 3 | BRF_GRA },           //  9
	{ "5003-c8.bin",    0x800000, 0x917a1439, 3 | BRF_GRA },           // 10

	{ "5003-m1.bin",    0x020000, 0x1a8c274b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "262-v1-08-e0.v1",      0x400000, 0x83d49ecf, 5 | BRF_SND },     // 12 Sound data
	{ "262-v2-08-e0.v2",      0x400000, 0x003f1843, 5 | BRF_SND },     // 13
	{ "262-v3-08-e0.v3",      0x400000, 0x2ae38dbe, 5 | BRF_SND },     // 14
	{ "262-v4-08-e0.v4",      0x400000, 0x26ec4dd9, 5 | BRF_SND },     // 15
};

STDROMPICKEXT(ct2k3sp, ct2k3sp, neogeo)
STD_ROM_FN(ct2k3sp)

static void ct2k3spCallback()
{
	INT32 i, j;
	UINT8 *dst = (UINT8 *)BurnMalloc( 0x040000 );
	if (dst)
	{
		for (i = 0; i < 0x040000; i++)
		{
			j = BITSWAP24(i, 23,22,21,20,19,18,17,3,0,1,4,2,13,14,15,16,5,6,11,10,9,8,7,12);
			dst[i] = NeoTextROM[nNeoActiveSlot][j];
		}

		memcpy (NeoTextROM[nNeoActiveSlot], dst, 0x040000 );

		BurnFree (dst);
	}

	cthd2003_decode();
}

static void ct2kspInstallBankSwitchHandler()
{
	SekMapHandler(6,	0x200000, 0x2fffff, MAP_WRITE);
	SekSetWriteWordHandler(6, cthd2003WriteWordBankswitch);
	SekSetWriteByteHandler(6, cthd2003WriteByteBankswitch);

	cthd2003Bankswitch(0);
}

static INT32 ct2k3spInit()
{
	nBurnCPUSpeedAdjust = 0x010a;

	NeoCallbackActive->pInitialise = ct2k3spCallback;
	NeoCallbackActive->pInstallHandlers = ct2kspInstallBankSwitchHandler;
	NeoCallbackActive->pBankswitch = NeoMapBank;

 	return NeoInit();
}

struct BurnDriver BurnDrvct2k3sp = {
	"ct2k3sp", "kof2001", "neogeo", NULL, "2003",
	"Crouching Tiger Hidden Dragon 2003 Super Plus\0", NULL, "bootleg", "Neo Geo MVS",
	L"Crouching Tiger Hidden Dragon 2003 Super Plus\0\u81E5\u864E\u85CF\u9F8D\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, ct2k3spRomInfo, ct2k3spRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	ct2k3spInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Crouching Tiger Hidden Dragon 2003 Super Plus alternate (The King of Fighters 2001 bootleg)

static struct BurnRomInfo ct2k3saRomDesc[] = {
	{ "5003-p1sa.bin",  0x100000, 0x013a509d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "5003-p2.bin",    0x400000, 0xadc1c22b, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "5003-s1sa.bin",  0x020000, 0x4e1f7eae, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "5003-c1.bin",    0x800000, 0x68f54b67, 3 | BRF_GRA },           //  3 Sprite data
	{ "5003-c2.bin",    0x800000, 0x2f8849d5, 3 | BRF_GRA },           //  4
	{ "5003-c3.bin",    0x800000, 0xac4aff71, 3 | BRF_GRA },           //  5
	{ "5003-c4.bin",    0x800000, 0xafef5d66, 3 | BRF_GRA },           //  6
	{ "5003-c5.bin",    0x800000, 0xc7c1ae50, 3 | BRF_GRA },           //  7
	{ "5003-c6.bin",    0x800000, 0x613197f9, 3 | BRF_GRA },           //  8
	{ "5003-c7.bin",    0x800000, 0x64ddfe0f, 3 | BRF_GRA },           //  9
	{ "5003-c8.bin",    0x800000, 0x917a1439, 3 | BRF_GRA },           // 10

	{ "5003-m1.bin",    0x020000, 0x1a8c274b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "262-v1-08-e0.v1",      0x400000, 0x83d49ecf, 5 | BRF_SND },     // 12 Sound data
	{ "262-v2-08-e0.v2",      0x400000, 0x003f1843, 5 | BRF_SND },     // 13
	{ "262-v3-08-e0.v3",      0x400000, 0x2ae38dbe, 5 | BRF_SND },     // 14
	{ "262-v4-08-e0.v4",      0x400000, 0x26ec4dd9, 5 | BRF_SND },     // 15
};

STDROMPICKEXT(ct2k3sa, ct2k3sa, neogeo)
STD_ROM_FN(ct2k3sa)

static INT32 ct2k3saInit()
{
	nBurnCPUSpeedAdjust = 0x010d;
	NeoCallbackActive->pInitialise = cthd2003_decode;
 	return NeoInit();
}

struct BurnDriver BurnDrvct2k3sa = {
	"ct2k3sa", "kof2001", "neogeo", NULL, "2003",
	"Crouching Tiger Hidden Dragon 2003 Super Plus alternate\0", NULL, "bootleg", "Neo Geo MVS",
	L"Crouching Tiger Hidden Dragon 2003 Super Plus (alternate set)\0\u81E5\u864E\u85CF\u9F8D\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, ct2k3saRomInfo, ct2k3saRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	ct2k3saInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 (NGM-2650)(NGH-2650)
/* Encrypted Set */ /* MVS AND AES VERSION */

static struct BurnRomInfo kof2002RomDesc[] = {
	{ "265-p1.p1",    0x100000, 0x9ede7323, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265-p2.sp2",   0x400000, 0x327266b8, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "265-c1.c1",    0x800000, 0x2b65a656, 3 | BRF_GRA },           //  2 Sprite data
	{ "265-c2.c2",    0x800000, 0xadf18983, 3 | BRF_GRA },           //  3
	{ "265-c3.c3",    0x800000, 0x875e9fd7, 3 | BRF_GRA },           //  4
	{ "265-c4.c4",    0x800000, 0x2da13947, 3 | BRF_GRA },           //  5
	{ "265-c5.c5",    0x800000, 0x61bd165d, 3 | BRF_GRA },           //  6
	{ "265-c6.c6",    0x800000, 0x03fdd1eb, 3 | BRF_GRA },           //  7
	{ "265-c7.c7",    0x800000, 0x1a2749d8, 3 | BRF_GRA },           //  8
	{ "265-c8.c8",    0x800000, 0xab0bb549, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "265-m1.m1",    0x020000, 0x85aaa632, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "265-v1.v1",    0x800000, 0x15e8f3f5, 5 | BRF_SND },           // 11 Sound data
	{ "265-v2.v2",    0x800000, 0xda41d6f9, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof2002, kof2002, neogeo)
STD_ROM_FN(kof2002)

static INT32 kof2002Init()
{
	INT32 nRet;

	nNeoProtectionXor = 0xEC;
	NeoCallbackActive->pInitialise = PCM2DecryptP;

	nRet = NeoInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0xa5000, 0x000000, { 0xf9, 0xe0, 0x5d, 0xf3, 0xea, 0x92, 0xbe, 0xef } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkof2002 = {
	"kof2002", NULL, "neogeo", NULL, "2002",
	"The King of Fighters 2002 (NGM-2650)(NGH-2650)\0", NULL, "Eolith / Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2002RomInfo, kof2002RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2002Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 (bootleg)

static struct BurnRomInfo kof2002bRomDesc[] = {
	{ "265-p1.p1",    0x100000, 0x9ede7323, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "2k2-p2.bin",   0x400000, 0x6dbee4df, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "2k2-s1.bin",   0x020000, 0x2255f5bf, 2 | BRF_GRA },		 //  2 Text data

	{ "2k2-c1.bin",   0x800000, 0xf25d3d66, 3 | BRF_GRA },		 //  3 Sprite data
	{ "2k2-c2.bin",   0x800000, 0xe3e66f1d, 3 | BRF_GRA },		 //  4
	{ "2k2-c3.bin",   0x800000, 0x8732fa30, 3 | BRF_GRA },		 //  5
	{ "2k2-c4.bin",   0x800000, 0x0989fd40, 3 | BRF_GRA },		 //  6
	{ "2k2-c5.bin",   0x800000, 0x60635cd2, 3 | BRF_GRA },		 //  7
	{ "2k2-c6.bin",   0x800000, 0xbd736824, 3 | BRF_GRA },		 //  8
	{ "2k2-c7.bin",   0x800000, 0x2da8d8cf, 3 | BRF_GRA },		 //  9
	{ "2k2-c8.bin",   0x800000, 0x2048404a, 3 | BRF_GRA },		 // 10

	{ "265-m1.m1",    0x020000, 0x85aaa632, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "265-v1.v1",    0x800000, 0x15e8f3f5, 5 | BRF_SND },		 // 12 Sound data
	{ "265-v2.v2",    0x800000, 0xda41d6f9, 5 | BRF_SND },		 // 13
};

STDROMPICKEXT(kof2002b, kof2002b, neogeo)
STD_ROM_FN(kof2002b)

void kof2002b_gfx_decrypt(UINT8 *src, INT32 nLen)
{
	INT32 i, j, k, *m;
	INT32 tbl[8][6] = {
		{ 1, 2, 6, 7, 8, 0 },
		{ 2, 6, 7, 8, 0, 1 },
		{ 6, 7, 8, 0, 1, 2 },
		{ 7, 8, 0, 1, 2, 6 },
		{ 8, 0, 1, 2, 6, 7 },
		{ 8, 7, 6, 2, 1, 0 },
		{ 8, 7, 6, 0, 1, 2 },
		{ 1, 2, 6, 7, 0, 8 }
	};

	UINT8 dst[0x10000];

	for (i = 0; i < nLen; i+= 0x10000)
	{
		memcpy (dst, src + i, 0x10000);

		for (j = 0; j < 0x10000 / 0x80; j++)
		{
			m = tbl[(j >> 3) & 7];
			k = BITSWAP16(j, 15, 14, 13, 12, 11, 10, 9, m[5], m[4], m[3], 5, 4, 3, m[2], m[1], m[0]);
			memcpy (src + i + k * 0x80, dst + j * 0x80, 0x80);
		}
	}
}

static void kof2002bCallback()
{
	PCM2DecryptP();
	kof2002b_gfx_decrypt(NeoSpriteROM[nNeoActiveSlot], 0x4000000);
	kof2002b_gfx_decrypt(NeoTextROM[nNeoActiveSlot], 0x020000);
}

static INT32 kof2002bInit()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = kof2002bCallback;

	nRet = NeoInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0xa5000, 0x000000, { 0xf9, 0xe0, 0x5d, 0xf3, 0xea, 0x92, 0xbe, 0xef } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkof2002b = {
	"kof2002b", "kof2002", "neogeo", NULL, "2002",
	"The King of Fighters 2002 (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2002bRomInfo, kof2002bRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2002bInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 Plus (bootleg set 1)

static struct BurnRomInfo kf2k2plsRomDesc[] = {
	{ "2k2-p1p.bin",  0x100000, 0x3ab03781, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265-p2.sp2",   0x400000, 0x327266b8, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "2k2-s1p.bin",  0x020000, 0x595e0006, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265-c1.c1",    0x800000, 0x2b65a656, 3 | BRF_GRA },           //  3 Sprite data
	{ "265-c2.c2",    0x800000, 0xadf18983, 3 | BRF_GRA },           //  4
	{ "265-c3.c3",    0x800000, 0x875e9fd7, 3 | BRF_GRA },           //  5
	{ "265-c4.c4",    0x800000, 0x2da13947, 3 | BRF_GRA },           //  6
	{ "265-c5.c5",    0x800000, 0x61bd165d, 3 | BRF_GRA },           //  7
	{ "265-c6.c6",    0x800000, 0x03fdd1eb, 3 | BRF_GRA },           //  8
	{ "265-c7.c7",    0x800000, 0x1a2749d8, 3 | BRF_GRA },           //  9
	{ "265-c8.c8",    0x800000, 0xab0bb549, 3 | BRF_GRA },           // 10

	{ "265-m1.m1",    0x020000, 0x85aaa632, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "265-v1.v1",    0x800000, 0x15e8f3f5, 5 | BRF_SND },           // 12 Sound data
	{ "265-v2.v2",    0x800000, 0xda41d6f9, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k2pls, kf2k2pls, neogeo)
STD_ROM_FN(kf2k2pls)

struct BurnDriver BurnDrvkf2k2pls = {
	"kf2k2pls", "kof2002", "neogeo", NULL, "2002",
	"The King of Fighters 2002 Plus (bootleg set 1)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k2plsRomInfo, kf2k2plsRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2002Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// The King of Fighters 2002 Plus (bootleg set 2)

static struct BurnRomInfo kf2k2plaRomDesc[] = {
	{ "2k2-p1pa.bin", 0x100000, 0x6a3a02f3, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265-p2.sp2",   0x400000, 0x327266b8, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "2k2-s1pa.bin", 0x020000, 0x1a3ed064, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265-c1.c1",    0x800000, 0x2b65a656, 3 | BRF_GRA },           //  3 Sprite data
	{ "265-c2.c2",    0x800000, 0xadf18983, 3 | BRF_GRA },           //  4
	{ "265-c3.c3",    0x800000, 0x875e9fd7, 3 | BRF_GRA },           //  5
	{ "265-c4.c4",    0x800000, 0x2da13947, 3 | BRF_GRA },           //  6
	{ "265-c5.c5",    0x800000, 0x61bd165d, 3 | BRF_GRA },           //  7
	{ "265-c6.c6",    0x800000, 0x03fdd1eb, 3 | BRF_GRA },           //  8
	{ "265-c7.c7",    0x800000, 0x1a2749d8, 3 | BRF_GRA },           //  9
	{ "265-c8.c8",    0x800000, 0xab0bb549, 3 | BRF_GRA },           // 10

	{ "265-m1.m1",    0x020000, 0x85aaa632, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "265-v1.v1",    0x800000, 0x15e8f3f5, 5 | BRF_SND },           // 12 Sound data
	{ "265-v2.v2",    0x800000, 0xda41d6f9, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k2pla, kf2k2pla, neogeo)
STD_ROM_FN(kf2k2pla)

struct BurnDriver BurnDrvkf2k2pla = {
	"kf2k2pla", "kof2002", "neogeo", NULL, "2002",
	"The King of Fighters 2002 Plus (bootleg set 2)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k2plaRomInfo, kf2k2plaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2002Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 Magic Plus (bootleg)

static struct BurnRomInfo kf2k2mpRomDesc[] = {
	{ "kf02m-p1.bin", 0x400000, 0xff7c6ec0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "kf02m-p2.bin", 0x400000, 0x91584716, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "kf02m-s1.bin", 0x020000, 0x348d6f2c, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265-c1.c1",    0x800000, 0x2b65a656, 3 | BRF_GRA },           //  3 Sprite data
	{ "265-c2.c2",    0x800000, 0xadf18983, 3 | BRF_GRA },           //  4
	{ "265-c3.c3",    0x800000, 0x875e9fd7, 3 | BRF_GRA },           //  5
	{ "265-c4.c4",    0x800000, 0x2da13947, 3 | BRF_GRA },           //  6
	{ "265-c5.c5",    0x800000, 0x61bd165d, 3 | BRF_GRA },           //  7
	{ "265-c6.c6",    0x800000, 0x03fdd1eb, 3 | BRF_GRA },           //  8
	{ "265-c7.c7",    0x800000, 0x1a2749d8, 3 | BRF_GRA },           //  9
	{ "265-c8.c8",    0x800000, 0xab0bb549, 3 | BRF_GRA },           // 10

	{ "265-m1.m1",    0x020000, 0x85aaa632, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "265-v1.v1",    0x800000, 0x15e8f3f5, 5 | BRF_SND },           // 12 Sound data
	{ "265-v2.v2",    0x800000, 0xda41d6f9, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k2mp, kf2k2mp, neogeo)
STD_ROM_FN(kf2k2mp)

static void kf2k2mpCallback()
{
	INT32 i, j;
	for (i = 0; i < 0x500000; i++)
	{
		j = (i & 0xffff00) + BITSWAP08(i, 7, 3, 4, 5, 6, 1, 2, 0);
		Neo68KROMActive[i] = Neo68KROMActive[0x300000 + j];
	}

	garoubl_sx_decode();
}

static INT32 kf2k2mpInit()
{
	INT32 nRet;

	nNeoProtectionXor = 0xEC;
	NeoCallbackActive->pInitialise = kf2k2mpCallback;

	nRet = NeoInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0xa5000, 0x000000, { 0xf9, 0xe0, 0x5d, 0xf3, 0xea, 0x92, 0xbe, 0xef } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkf2k2mp = {
	"kf2k2mp", "kof2002", "neogeo", NULL, "2002",
	"The King of Fighters 2002 Magic Plus (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k2mpRomInfo, kf2k2mpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf2k2mpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// The King of Fighters 2002 Magic Plus II (bootleg)

static struct BurnRomInfo kf2k2mp2RomDesc[] = {
	{ "k2k2m2p1.bin", 0x200000, 0x1016806c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "k2k2m2p2.bin", 0x400000, 0x432fdf53, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "k2k2m2s1.bin", 0x020000, 0x446e74c5, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265-c1.c1",    0x800000, 0x2b65a656, 3 | BRF_GRA },           //  3 Sprite data
	{ "265-c2.c2",    0x800000, 0xadf18983, 3 | BRF_GRA },           //  4
	{ "265-c3.c3",    0x800000, 0x875e9fd7, 3 | BRF_GRA },           //  5
	{ "265-c4.c4",    0x800000, 0x2da13947, 3 | BRF_GRA },           //  6
	{ "265-c5.c5",    0x800000, 0x61bd165d, 3 | BRF_GRA },           //  7
	{ "265-c6.c6",    0x800000, 0x03fdd1eb, 3 | BRF_GRA },           //  8
	{ "265-c7.c7",    0x800000, 0x1a2749d8, 3 | BRF_GRA },           //  9
	{ "265-c8.c8",    0x800000, 0xab0bb549, 3 | BRF_GRA },           // 10

	{ "265-m1.m1",    0x020000, 0x85aaa632, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "265-v1.v1",    0x800000, 0x15e8f3f5, 5 | BRF_SND },           // 12 Sound data
	{ "265-v2.v2",    0x800000, 0xda41d6f9, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k2mp2, kf2k2mp2, neogeo)
STD_ROM_FN(kf2k2mp2)

static void kf2k2mp2Callback()
{
	memcpy (Neo68KROMActive + 0x000000, Neo68KROMActive + 0x1c0000, 0x040000);
	memcpy (Neo68KROMActive + 0x0c0000, Neo68KROMActive + 0x100000, 0x040000);
	memmove (Neo68KROMActive + 0x100000, Neo68KROMActive + 0x200000, 0x400000);

	lans2004_sx_decode();
}

static INT32 kof2km2Init()
{
	INT32 nRet;

	nNeoProtectionXor = 0xEC;
	NeoCallbackActive->pInitialise = kf2k2mp2Callback;

	nRet = NeoInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0xa5000, 0x000000, { 0xf9, 0xe0, 0x5d, 0xf3, 0xea, 0x92, 0xbe, 0xef } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkf2k2mp2 = {
	"kf2k2mp2", "kof2002", "neogeo", NULL, "2002",
	"The King of Fighters 2002 Magic Plus II (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k2mp2RomInfo, kf2k2mp2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2km2Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 10th Anniversary (bootleg)

static struct BurnRomInfo kof10thRomDesc[] = {
	{ "kf10-p1.bin",  0x800000, 0xb1fd0c43, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "kf10-c1a.bin", 0x400000, 0x3bbc0364, 3 | BRF_GRA },           //  1 Sprite data
	{ "kf10-c2a.bin", 0x400000, 0x91230075, 3 | BRF_GRA },           //  2
	{ "kf10-c1b.bin", 0x400000, 0xb5abfc28, 3 | BRF_GRA },           //  3
	{ "kf10-c2b.bin", 0x400000, 0x6cc4c6e1, 3 | BRF_GRA },           //  4
	{ "kf10-c3a.bin", 0x400000, 0x5b3d4a16, 3 | BRF_GRA },           //  5
	{ "kf10-c4a.bin", 0x400000, 0xc6f3419b, 3 | BRF_GRA },           //  6
	{ "kf10-c3b.bin", 0x400000, 0x9d2bba19, 3 | BRF_GRA },           //  7
	{ "kf10-c4b.bin", 0x400000, 0x5a4050cb, 3 | BRF_GRA },           //  8
	{ "kf10-c5a.bin", 0x400000, 0xa289d1e1, 3 | BRF_GRA },           //  9
	{ "kf10-c6a.bin", 0x400000, 0xe6494b5d, 3 | BRF_GRA },           // 10
	{ "kf10-c5b.bin", 0x400000, 0x404fff02, 3 | BRF_GRA },           // 11
	{ "kf10-c6b.bin", 0x400000, 0xf2ccfc9e, 3 | BRF_GRA },           // 12
	{ "kf10-c7a.bin", 0x400000, 0xbe79c5a8, 3 | BRF_GRA },           // 13
	{ "kf10-c8a.bin", 0x400000, 0xa5952ca4, 3 | BRF_GRA },           // 14
	{ "kf10-c7b.bin", 0x400000, 0x3fdb3542, 3 | BRF_GRA },           // 15
	{ "kf10-c8b.bin", 0x400000, 0x661b7a52, 3 | BRF_GRA },           // 16

	{ "kf10-m1.bin",  0x020000, 0xf6fab859, 4 | BRF_ESS | BRF_PRG }, // 17 Z80 code

	{ "kf10-v1.bin",  0x800000, 0x0fc9a58d, 5 | BRF_SND },           // 18 Sound data
	{ "kf10-v2.bin",  0x800000, 0xb8c475a4, 5 | BRF_SND },           // 19
};

STDROMPICKEXT(kof10th, kof10th, neogeo)
STD_ROM_FN(kof10th)

static UINT8* kof10thExtraRAMA,  *kof10thExtraRAMB;

static void kof10thBankswitch(UINT32 nBank)
{
	nBank = 0x100000 + ((nBank & 7) << 20);
	if (nBank >= 0x700000) {
		nBank = 0x100000;
	}

	if (nBank != nNeo68KROMBank) {
		nNeo68KROMBank = nBank;
		SekMapMemory(Neo68KROMActive + nNeo68KROMBank, 0x200000, 0x2fdfff, MAP_ROM);
	}
}

// Text data extraction, game does this on the fly!
void __fastcall kof10thWriteWordCustom(UINT32 sekAddress, UINT16 wordValue)
{
	if (kof10thExtraRAMB[0x1ffc]) {
		NeoUpdateTextOne(((sekAddress >> 1) & 0x1ffff), BITSWAP08(BURN_ENDIAN_SWAP_INT16(wordValue), 3, 2, 1, 5, 7, 6, 0, 4));
	} else {
		*(UINT16*)(kof10thExtraRAMA + (sekAddress & 0x1fffe)) = BURN_ENDIAN_SWAP_INT16(wordValue);
	}
}

// 68K Bankswitch and RAM Handlers
void __fastcall kof10thWriteByteBankswitch(UINT32 sekAddress, UINT8 byteValue)
{
	sekAddress ^= 1;
	if (sekAddress == 0x2ffff0)
		kof10thBankswitch(byteValue);

	kof10thExtraRAMB[sekAddress & 0x1fff] = byteValue;
}

void __fastcall kof10thWriteWordBankswitch(UINT32 sekAddress, UINT16 wordValue)
{
	if (sekAddress == 0x2ffff0)
		kof10thBankswitch(wordValue);
	else if (sekAddress == 0x2ffff8 && *(UINT16*)(kof10thExtraRAMB + 0x1ff8) != BURN_ENDIAN_SWAP_INT16(wordValue))
		SekMapMemory(Neo68KROMActive + ((wordValue & 1) ? 0x710000 : 0x010000) , 0x010000, 0x0dffff, MAP_ROM);

	*(UINT16*)(kof10thExtraRAMB + (sekAddress & 0x01ffe)) = wordValue;
}

static void kof10thCallback()
{
	INT32 i, j, k;
	UINT8 *dst = (UINT8*)BurnMalloc(0x100000);
	if (dst)
	{
		for (i = 0; i < 0x800000; i+=0x100000)
		{
			for (j = 0; j < 0x100000; j++)
			{
				k = BITSWAP24(j,23,22,21,20,19,18,17,16,15,14,13,12,11,2,9,8,7,1,5,4,3,10,6,0);
				dst[k] = Neo68KROMActive[i + j];
			}
			memcpy (Neo68KROMActive + i, dst, 0x100000);
		}

		memmove (Neo68KROMActive + 0x100000, Neo68KROMActive, 0x700000);
		memcpy (Neo68KROMActive, dst, 0x100000);
		BurnFree (dst);
	}

	// Altera protection chip patches these over P ROM
	*((UINT16 *)(Neo68KROMActive + 0x0124)) = BURN_ENDIAN_SWAP_INT16(0x000d); // Enables XOR for RAM moves, forces SoftDIPs, and USA region
	*((UINT16 *)(Neo68KROMActive + 0x0126)) = BURN_ENDIAN_SWAP_INT16(0xf7a8);

	*((UINT16 *)(Neo68KROMActive + 0x8bf4)) = BURN_ENDIAN_SWAP_INT16(0x4ef9); // Run code to change "S" data
	*((UINT16 *)(Neo68KROMActive + 0x8bf6)) = BURN_ENDIAN_SWAP_INT16(0x000d);
	*((UINT16 *)(Neo68KROMActive + 0x8bf8)) = BURN_ENDIAN_SWAP_INT16(0xf980);
}

static void kof10thMapBank()
{
	SekMapMemory(Neo68KROMActive + nNeo68KROMBank, 0x200000, 0x2fdfff, MAP_ROM);
}

static INT32 kof10thScan(INT32 nAction, INT32* pnMin)
{
	struct BurnArea ba;

	if (pnMin) {												// Return minimum compatible version
		*pnMin =  0x029713;
	}

	if (nAction & ACB_MEMORY_RAM) {
		ba.Data		= kof10thExtraRAMA;
		ba.nLen		= 0x00020000;
		ba.nAddress	= 0;
		ba.szName	= "Extra RAM A";
		BurnAcb(&ba);

		ba.Data		= kof10thExtraRAMB;
		ba.nLen		= 0x00002000;
		ba.nAddress	= 0;
		ba.szName	= "Extra RAM B";
		BurnAcb(&ba);
	}
	return 0;
}

static void kof10thInstallHandlers()
{
	// Install RAM handlers
	SekMapMemory(kof10thExtraRAMA, 0x0e0000, 0x0fffff, MAP_ROM); // Text RAM on range 0x0e0000 - 0x0fffff
	SekMapMemory(kof10thExtraRAMB, 0x2fe000, 0x2fffff, MAP_ROM); // Extra 68K RAM

	// Install bankswitch and text handlers
	SekMapHandler(6,	0x2fe000, 0x2fffff, MAP_WRITE);
	SekSetWriteWordHandler(6, kof10thWriteWordBankswitch);
	SekSetWriteByteHandler(6, kof10thWriteByteBankswitch);

	SekMapHandler(7,	0x200000, 0x23ffff, MAP_WRITE);
	SekSetWriteWordHandler(7,     kof10thWriteWordCustom);

	// Set bank
	nNeo68KROMBank = 1;
	kof10thBankswitch(0);
}

static INT32 kof10thInit()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = kof10thCallback;
	NeoCallbackActive->pInstallHandlers = kof10thInstallHandlers;
	NeoCallbackActive->pBankswitch = kof10thMapBank;
	NeoCallbackActive->pScan = kof10thScan;

	nNeoTextROMSize[nNeoActiveSlot] = 0x20000;

	nRet = NeoInit();

	if (nRet == 0) {
		kof10thExtraRAMA = Neo68KROMActive + 0x7e0000;
		kof10thExtraRAMB = Neo68KROMActive + 0x1fe000;
		memset(kof10thExtraRAMA, 0, 0x20000);
		memset(kof10thExtraRAMB, 0, 0x02000);
	}

	return nRet;
}

struct BurnDriver BurnDrvkof10th = {
	"kof10th", "kof2002", "neogeo", NULL, "2002",
	"The King of Fighters 10th Anniversary (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof10thRomInfo, kof10thRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof10thInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 10th Anniversary Extra Plus (bootleg)

static struct BurnRomInfo kf10thepRomDesc[] = {
	{ "5008-p1.bin",  0x200000, 0xbf5469ba, 1 | BRF_PRG | BRF_OPT }, //  0 68K code
	{ "5008-p2.bin",  0x400000, 0xa649ec38, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "5008-p3.bin",  0x200000, 0xe629e13c, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "5008-s1.bin",  0x020000, 0x92410064, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "kf10-c1a.bin", 0x400000, 0x3bbc0364, 3 | BRF_GRA },           //  4 Sprite data
	{ "kf10-c2a.bin", 0x400000, 0x91230075, 3 | BRF_GRA },           //  5
	{ "kf10-c1b.bin", 0x400000, 0xb5abfc28, 3 | BRF_GRA },           //  6
	{ "kf10-c2b.bin", 0x400000, 0x6cc4c6e1, 3 | BRF_GRA },           //  7
	{ "kf10-c3a.bin", 0x400000, 0x5b3d4a16, 3 | BRF_GRA },           //  8
	{ "kf10-c4a.bin", 0x400000, 0xc6f3419b, 3 | BRF_GRA },           //  9
	{ "kf10-c3b.bin", 0x400000, 0x9d2bba19, 3 | BRF_GRA },           // 10
	{ "kf10-c4b.bin", 0x400000, 0x5a4050cb, 3 | BRF_GRA },           // 11
	{ "kf10-c5a.bin", 0x400000, 0xa289d1e1, 3 | BRF_GRA },           // 12
	{ "kf10-c6a.bin", 0x400000, 0xe6494b5d, 3 | BRF_GRA },           // 13
	{ "kf10-c5b.bin", 0x400000, 0x404fff02, 3 | BRF_GRA },           // 14
	{ "kf10-c6b.bin", 0x400000, 0xf2ccfc9e, 3 | BRF_GRA },           // 15
	{ "kf10-c7a.bin", 0x400000, 0xbe79c5a8, 3 | BRF_GRA },           // 16
	{ "kf10-c8a.bin", 0x400000, 0xa5952ca4, 3 | BRF_GRA },           // 17
	{ "5008-c7b.bin", 0x400000, 0x33604ef0, 3 | BRF_GRA },           // 18
	{ "5008-c8b.bin", 0x400000, 0x51f6a8f8, 3 | BRF_GRA },           // 19

	{ "5008-m1.bin",  0x020000, 0x5a47d9ad, 4 | BRF_ESS | BRF_PRG }, // 20 Z80 code

	{ "kf10-v1.bin",  0x800000, 0x0fc9a58d, 5 | BRF_SND },           // 21 Sound data
	{ "kf10-v2.bin",  0x800000, 0xb8c475a4, 5 | BRF_SND },           // 22
};

STDROMPICKEXT(kf10thep, kf10thep, neogeo)
STD_ROM_FN(kf10thep)

static void kf10thepCallback()
{
	INT32 i;
	UINT8 *dst = (UINT8*)BurnMalloc(0x100000);
	if (dst)
	{
		UINT32 sec[8] = { 0x3, 0x8, 0x7, 0xC, 0x1, 0xA, 0x6, 0xD };

		for (i = 0; i < 8; i++)
			memcpy (dst + i * 0x20000, Neo68KROMActive + sec[i] * 0x20000, 0x20000);

		memcpy (dst + 0x0002e0, Neo68KROMActive + 0x0402e0, 0x06a);
		memcpy (dst + 0x0f92bc, Neo68KROMActive + 0x0492bc, 0xb9e);
		memcpy (Neo68KROMActive, dst, 0x100000);
		BurnFree (dst);
	}

	memmove (Neo68KROMActive + 0x100000, Neo68KROMActive + 0x200000, 0x600000);

	for (i = 0xf92bc; i < 0xf9e58; i += 2)
	{
		if ((*((UINT16*)(Neo68KROMActive + i + 0)) & 0xffbf) == BURN_ENDIAN_SWAP_INT16(0x4eb9) && *((UINT16*)(Neo68KROMActive + i + 2)) == BURN_ENDIAN_SWAP_INT16(0x0000))
		{
			*((UINT16*)(Neo68KROMActive + i + 2)) = BURN_ENDIAN_SWAP_INT16(0x000f);
 		}
	}

	*((UINT16 *)(Neo68KROMActive + 0x00342)) = BURN_ENDIAN_SWAP_INT16(0x000f);

	lans2004_sx_decode();
}

static INT32 kf10thepInit()
{
	NeoCallbackActive->pInitialise = kf10thepCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvkf10thep = {
	"kf10thep", "kof2002", "neogeo", NULL, "2005",
	"The King of Fighters 10th Anniversary Extra Plus (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kf10thepRomInfo, kf10thepRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf10thepInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 10th Anniversary 2005 Unique (bootleg)

static struct BurnRomInfo kf2k5uniRomDesc[] = {
	{ "5006-p2a.bin", 0x400000, 0xced883a2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "5006-p1.bin",  0x400000, 0x72c39c46, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "5006-s1.bin",  0x020000, 0x91f8c544, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "kf10-c1a.bin", 0x400000, 0x3bbc0364, 3 | BRF_GRA },           //  3 Sprite data
	{ "kf10-c2a.bin", 0x400000, 0x91230075, 3 | BRF_GRA },           //  4
	{ "kf10-c1b.bin", 0x400000, 0xb5abfc28, 3 | BRF_GRA },           //  5
	{ "kf10-c2b.bin", 0x400000, 0x6cc4c6e1, 3 | BRF_GRA },           //  6
	{ "kf10-c3a.bin", 0x400000, 0x5b3d4a16, 3 | BRF_GRA },           //  7
	{ "kf10-c4a.bin", 0x400000, 0xc6f3419b, 3 | BRF_GRA },           //  8
	{ "kf10-c3b.bin", 0x400000, 0x9d2bba19, 3 | BRF_GRA },           //  9
	{ "kf10-c4b.bin", 0x400000, 0x5a4050cb, 3 | BRF_GRA },           // 10
	{ "kf10-c5a.bin", 0x400000, 0xa289d1e1, 3 | BRF_GRA },           // 11
	{ "kf10-c6a.bin", 0x400000, 0xe6494b5d, 3 | BRF_GRA },           // 12
	{ "kf10-c5b.bin", 0x400000, 0x404fff02, 3 | BRF_GRA },           // 13
	{ "kf10-c6b.bin", 0x400000, 0xf2ccfc9e, 3 | BRF_GRA },           // 14
	{ "kf10-c7a.bin", 0x400000, 0xbe79c5a8, 3 | BRF_GRA },           // 15
	{ "kf10-c8a.bin", 0x400000, 0xa5952ca4, 3 | BRF_GRA },           // 16
	{ "kf10-c7b.bin", 0x400000, 0x3fdb3542, 3 | BRF_GRA },           // 17
	{ "kf10-c8b.bin", 0x400000, 0x661b7a52, 3 | BRF_GRA },           // 18

	{ "5006-m1.bin",  0x020000, 0x9050bfe7, 4 | BRF_ESS | BRF_PRG }, // 19 Z80 code

	{ "kf10-v1.bin",  0x800000, 0x0fc9a58d, 5 | BRF_SND },           // 20 Sound data
	{ "kf10-v2.bin",  0x800000, 0xb8c475a4, 5 | BRF_SND },           // 21
};

STDROMPICKEXT(kf2k5uni, kf2k5uni, neogeo)
STD_ROM_FN(kf2k5uni)

static void kf2k5uniCallback()
{
	INT32 i, j, k;
	UINT8 dst[0x80];

	for (i = 0; i < 0x800000; i+=0x80)
	{
		for (j = 0; j < 0x80; j++)
		{
			k = BITSWAP08(j, 7, 3, 4, 5, 6, 1, 2, 0);
			dst[j] = Neo68KROMActive[i + k];
		}
		memcpy (Neo68KROMActive + i, dst, 0x80);
	}

	memcpy(Neo68KROMActive, Neo68KROMActive + 0x600000, 0x100000);

	for (i = 0; i < 0x30000; i++)
		NeoZ80ROMActive[i] = BITSWAP08(NeoZ80ROMActive[i], 4, 5, 6, 7, 0, 1, 2, 3);

	for (i = 0; i < 0x20000; i++)
		NeoTextROM[nNeoActiveSlot][i] = BITSWAP08(NeoTextROM[nNeoActiveSlot][i], 4, 5, 6, 7, 0, 1, 2, 3);
}

static INT32 kf2k5uniInit()
{
	NeoCallbackActive->pInitialise = kf2k5uniCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvkf2k5uni = {
	"kf2k5uni", "kof2002", "neogeo", NULL, "2004",
	"The King of Fighters 10th Anniversary 2005 Unique (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k5uniRomInfo, kf2k5uniRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf2k5uniInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters Special Edition 2004 (bootleg)

static struct BurnRomInfo kof2k4seRomDesc[] = {
	{ "k2k4s-p1.bin", 0x400000, 0xe6c50566, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "k2k4s-p2.bin", 0x080000, 0x21a84084, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "k2k4s-p3.bin", 0x080000, 0xfebb484e, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "k2k4s-s1.bin", 0x020000, 0xa3c9b2d8, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "k2k4s-c4.bin", 0x800000, 0x7a050288, 3 | BRF_GRA },           //  4 Sprite data
	{ "k2k4s-c8.bin", 0x800000, 0xe924afcf, 3 | BRF_GRA },           //  5
	{ "k2k4s-c3.bin", 0x800000, 0x959fad0b, 3 | BRF_GRA },           //  6
	{ "k2k4s-c7.bin", 0x800000, 0xefe6a468, 3 | BRF_GRA },           //  7
	{ "k2k4s-c2.bin", 0x800000, 0x74bba7c6, 3 | BRF_GRA },           //  8
	{ "k2k4s-c6.bin", 0x800000, 0xe20d2216, 3 | BRF_GRA },           //  9
	{ "k2k4s-c1.bin", 0x800000, 0xfa705b2b, 3 | BRF_GRA },           // 10
	{ "k2k4s-c5.bin", 0x800000, 0x2c912ff9, 3 | BRF_GRA },           // 11

	{ "k2k4s-m1.bin", 0x020000, 0x5a47d9ad, 4 | BRF_ESS | BRF_PRG }, // 12 Z80 code

	{ "k2k4s-v2.bin", 0x800000, 0xe4ddfb3f, 5 | BRF_SND },           // 13 Sound data
	{ "k2k4s-v1.bin", 0x800000, 0xb887d287, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(kof2k4se, kof2k4se, neogeo)
STD_ROM_FN(kof2k4se)

static void kof2k4seCallback()
{
	UINT8 *dst = (UINT8*)BurnMalloc(0x100000);

	if (dst)
	{
		memcpy (dst,                        Neo68KROMActive + 0x000000, 0x100000);
		memcpy (Neo68KROMActive + 0x000000, Neo68KROMActive + 0x400000, 0x100000);
		memcpy (Neo68KROMActive + 0x400000, dst,                        0x100000);
		memcpy (dst,                        Neo68KROMActive + 0x100000, 0x100000);
		memcpy (Neo68KROMActive + 0x100000, Neo68KROMActive + 0x300000, 0x100000);
		memcpy (Neo68KROMActive + 0x300000, dst,                        0x100000);

		BurnFree (dst);
	}
}

static INT32 kof2k4seInit()
{
	NeoCallbackActive->pInitialise = kof2k4seCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvkof2k4se = {
	"kof2k4se", "kof2002", "neogeo", NULL, "2004",
	"The King of Fighters Special Edition 2004 (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2k4seRomInfo, kof2k4seRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2k4seInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 (NGM-2680)
/* Encrypted Set */ /* MVS VERSION */

static struct BurnRomInfo mslug5RomDesc[] = {
	{ "268-p1cr.p1",   0x400000, 0xd0466792, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "268-p2cr.p2",   0x400000, 0xfbf6b61e, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "268-c1c.c1",    0x800000, 0xab7c389a, 3 | BRF_GRA },           //  2 Sprite data
	{ "268-c2c.c2",    0x800000, 0x3560881b, 3 | BRF_GRA },           //  3
	{ "268-c3c.c3",    0x800000, 0x3af955ea, 3 | BRF_GRA },           //  4
	{ "268-c4c.c4",    0x800000, 0xc329c373, 3 | BRF_GRA },           //  5
	{ "268-c5c.c5",    0x800000, 0x959c8177, 3 | BRF_GRA },           //  6
	{ "268-c6c.c6",    0x800000, 0x010a831b, 3 | BRF_GRA },           //  7
	{ "268-c7c.c7",    0x800000, 0x6d72a969, 3 | BRF_GRA },           //  8
	{ "268-c8c.c8",    0x800000, 0x551d720e, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "268-m1.m1",     0x080000, 0x4a5a6e0e, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "268-v1c.v1",    0x800000, 0xae31d60c, 5 | BRF_SND },           // 11 Sound data
	{ "268-v2c.v2",    0x800000, 0xc40613ed, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(mslug5, mslug5, neogeo)
STD_ROM_FN(mslug5)

static void mslug5Callback()
{
	INT32 i, j, k;
	for (i = 0; i < 0x100000; i++)
		Neo68KROMActive[i] ^= Neo68KROMActive[0x0fffe0 + (i & 0x1f)];

	for (i = 0x100000; i < 0x700000; i++)
		Neo68KROMActive[i] ^= ~Neo68KROMActive[0x7fffe0 + (i & 0x1f)];

	// Allow byte interleaving to extend to [0x500000 - 0x700000]
	for (i = 0x100000; i < 0x0700000; i += 4)
	{
		UINT16 rom16 = BURN_UNALIGNED_READ16(Neo68KROMActive + i + 1);
		rom16 = BITSWAP16(rom16, 15, 14, 13, 12, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0);
		BURN_UNALIGNED_WRITE16(Neo68KROMActive + i + 1, rom16);
	}

	memcpy (Neo68KROMActive + 0x700000, Neo68KROMActive, 0x100000);

	for (i = 0; i < 0x100000 / 0x010000; i++)
	{
		j = BITSWAP08(i, 7, 6, 5, 4, 1, 0, 3, 2) * 0x010000;
		memcpy (Neo68KROMActive + i * 0x010000, Neo68KROMActive + 0x700000 + j, 0x010000);
	}

	for (i = 0x100000; i < 0x700000; i += 0x100000)
	{
		for (j = 0; j < 0x100000; j+=0x100)
		{
			k = ((j & 0xf00) ^ 0x700) + (BITSWAP08((j >> 12), 5, 4, 7, 6, 1, 0, 3, 2 ) << 12);
			memcpy (Neo68KROMActive + 0x700000 + j, Neo68KROMActive + i + k, 0x100);
		}

		memcpy (Neo68KROMActive + i, Neo68KROMActive + 0x700000, 0x100000);
	}
}

static INT32 mslug5Init()
{
	INT32 nRet;

	nNeoProtectionXor = 0x19;
	NeoCallbackActive->pInitialise = mslug5Callback;

	nRet = NeoPVCInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0x4e001, 0xfe2cf6, { 0xc3, 0xfd, 0x81, 0xac, 0x6d, 0xe7, 0xbf, 0x9e } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvmslug5 = {
	"mslug5", NULL, "neogeo", NULL, "2003",
	"Metal Slug 5 (NGM-2680)\0", NULL, "SNK Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5RomInfo, mslug5RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug5Init, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Metal Slug 5 (NGH-2680)
/* Encrypted Set */ /* AES release of the game but is also found in later MVS carts */

static struct BurnRomInfo mslug5hRomDesc[] = {
	{ "268-p1c.p1",    0x400000, 0x3636690a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "268-p2c.p2",    0x400000, 0x8dfc47a2, 1 | BRF_ESS | BRF_PRG }, //  1
	/* also found AES set with P1 / P2 on mask rom on PROG board NEO-AEG PROGBK3S (2003.6.10);
	chip labels are 268-P1CR2 and 268-P2CR2
	and M1 on eprom M27C4001 with label sticker 268-M1 on CHA board NEO-MVS CHAFIO (2003.7.24) (NEO-CMC 7050) */

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "268-c1c.c1",    0x800000, 0xab7c389a, 3 | BRF_GRA },           //  2 Sprite data
	{ "268-c2c.c2",    0x800000, 0x3560881b, 3 | BRF_GRA },           //  3
	{ "268-c3c.c3",    0x800000, 0x3af955ea, 3 | BRF_GRA },           //  4
	{ "268-c4c.c4",    0x800000, 0xc329c373, 3 | BRF_GRA },           //  5
	{ "268-c5c.c5",    0x800000, 0x959c8177, 3 | BRF_GRA },           //  6
	{ "268-c6c.c6",    0x800000, 0x010a831b, 3 | BRF_GRA },           //  7
	{ "268-c7c.c7",    0x800000, 0x6d72a969, 3 | BRF_GRA },           //  8
	{ "268-c8c.c8",    0x800000, 0x551d720e, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "268-m1.m1",     0x080000, 0x4a5a6e0e, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "268-v1c.v1",    0x800000, 0xae31d60c, 5 | BRF_SND },           // 11 Sound data
	{ "268-v2c.v2",    0x800000, 0xc40613ed, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(mslug5h, mslug5h, neogeo)
STD_ROM_FN(mslug5h)

struct BurnDriver BurnDrvmslug5h = {
	"mslug5h", "mslug5", "neogeo", NULL, "2003",
	"Metal Slug 5 (NGH-2680)\0", NULL, "SNK Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5hRomInfo, mslug5hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug5Init, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 (JAMMA PCB)

static struct BurnRomInfo ms5pcbRomDesc[] = {
	{ "268-p1r.p1",   0x400000,  0xd0466792, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "268-p2r.p2",   0x400000,  0xfbf6b61e, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "268-c1.c1",    0x1000000, 0x802042e8, 3 | BRF_GRA },           //  2 Sprite data
	{ "268-c2.c2",    0x1000000, 0x3b89fb9f, 3 | BRF_GRA },           //  3
	{ "268-c3.c3",    0x1000000, 0x0f3f59e3, 3 | BRF_GRA },           //  4
	{ "268-c4.c4",    0x1000000, 0x3ad8435a, 3 | BRF_GRA },           //  5

	{ "268-m1.m1",    0x080000,  0x4a5a6e0e, 4 | BRF_ESS | BRF_PRG }, // 6 Z80 code

	{ "268-v1.v1",    0x1000000, 0x8458afe5, 5 | BRF_SND },           //  7 Sound data
};

STDROMPICKEXT(ms5pcb, ms5pcb, ms5pcbBIOS)
STD_ROM_FN(ms5pcb)

struct BurnDriver BurnDrvms5pcb = {
	"ms5pcb", NULL, NULL, NULL, "2003",
	"Metal Slug 5 (JAMMA PCB)\0", NULL, "SNK Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SNK_DEDICATED_PCB | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, ms5pcbRomInfo, ms5pcbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, ms5pcbDIPInfo,
	mslug5Init, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 Plus (bootleg)

static struct BurnRomInfo ms5plusRomDesc[] = {
	{ "ms5-p1p.bin",  0x100000, 0x106b276f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "ms5-p2p.bin",  0x200000, 0xd6a458e8, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "ms5-p3p.bin",  0x200000, 0x439ec031, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "ms5-s1p.bin",  0x020000, 0x21e04432, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "268-c1c.c1",   0x800000, 0xab7c389a, 3 | BRF_GRA },           //  4 Sprite data
	{ "268-c2c.c2",   0x800000, 0x3560881b, 3 | BRF_GRA },           //  5
	{ "268-c3c.c3",   0x800000, 0x3af955ea, 3 | BRF_GRA },           //  6
	{ "268-c4c.c4",   0x800000, 0xc329c373, 3 | BRF_GRA },           //  7
	{ "268-c5c.c5",   0x800000, 0x959c8177, 3 | BRF_GRA },           //  8
	{ "268-c6c.c6",   0x800000, 0x010a831b, 3 | BRF_GRA },           //  9
	{ "268-c7c.c7",   0x800000, 0x6d72a969, 3 | BRF_GRA },           // 10
	{ "268-c8c.c8",   0x800000, 0x551d720e, 3 | BRF_GRA },           // 11

	{ "268-m1.m1",    0x080000, 0x4a5a6e0e, 4 | BRF_ESS | BRF_PRG }, // 12 Z80 code

	{ "268-v1c.v1",   0x800000, 0xae31d60c, 5 | BRF_SND },           // 13 Sound data
	{ "268-v2c.v2",   0x800000, 0xc40613ed, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(ms5plus, ms5plus, neogeo)
STD_ROM_FN(ms5plus)

void __fastcall ms5plusWriteWordBankSwitch(UINT32 sekAddress, UINT16 wordValue)
{
	if (sekAddress==0x2ffff4)
	{
		UINT32 nBank = wordValue << 16;
		if (nNeo68KROMBank != nBank)
		{
			nNeo68KROMBank = nBank;
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank, 0x200000, 0x2fdfff, MAP_ROM);
		}
	};
}

static void ms5plusInstallHandlers()
{
	SekMapHandler(6,	0x2fe000, 0x2fffff, MAP_WRITE);
	SekSetWriteWordHandler(6, ms5plusWriteWordBankSwitch);
}

static INT32 ms5plusInit()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = lans2004_sx_decode;
	NeoCallbackActive->pInstallHandlers = ms5plusInstallHandlers;
	NeoCallbackActive->pBankswitch = NeoPVCMapBank;
	nNeoProtectionXor = 0x19;

	nRet = NeoInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0x4e001, 0xfe2cf6, { 0xc3, 0xfd, 0x81, 0xac, 0x6d, 0xe7, 0xbf, 0x9e } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvms5plus = {
	"ms5plus", "mslug5", "neogeo", NULL, "2003",
	"Metal Slug 5 Plus (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, ms5plusRomInfo, ms5plusRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	ms5plusInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// SNK vs. Capcom - SVC Chaos (JAMMA PCB, set 1)

static struct BurnRomInfo svcpcbRomDesc[] = {
	{ "269-p1.p1",    0x2000000, 0x432cfdfc, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "269-c1.c1",    0x2000000, 0x1b608f9c, 3 | BRF_GRA },           //  1 Sprite data
	{ "269-c2.c2",    0x2000000, 0x5a95f294, 3 | BRF_GRA },           //  2

	{ "269-m1.m1",    0x080000,  0xf6819d00, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "269-v1.v1",    0x800000,  0xc659b34c, 5 | BRF_SND },           //  5 Sound data
	{ "269-v2.v2",    0x800000,  0xdd903835, 5 | BRF_SND },           //  6
};

STDROMPICKEXT(svcpcb, svcpcb, svcpcbBIOS)
STD_ROM_FN(svcpcb)

static void svcCallback()
{
	INT32 i, j, k;
	for (i = 0; i < 0x100000; i++)
		Neo68KROMActive[i] ^= ~Neo68KROMActive[0x0fffe0 + (i & 0x1f)];

	for (i = 0x100000; i < 0x800000; i++)
		Neo68KROMActive[i] ^= ~Neo68KROMActive[0x7fffe0 + (i & 0x1f)];

	for (i = 0x100000; i < 0x0600000; i+=4)
	{
		UINT16 rom16 = BURN_UNALIGNED_READ16(Neo68KROMActive + i + 1);
		rom16 = BITSWAP16(rom16, 15, 14, 13, 12, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0);
		BURN_UNALIGNED_WRITE16(Neo68KROMActive + i + 1, rom16);
	}

	memcpy (Neo68KROMActive + 0x700000, Neo68KROMActive, 0x100000);

	for (i = 0; i < 0x0100000 / 0x10000; i++)
	{
		j = BITSWAP08(i, 7, 6, 5, 4, 2, 3, 0, 1);
		memcpy (Neo68KROMActive + i * 0x010000, Neo68KROMActive + 0x700000 + j * 0x010000, 0x010000);
	}

	for (i = 0x100000; i < 0x700000; i += 0x100000)
	{
		for (j = 0; j < 0x100000; j+= 0x100)
		{
			k  = BITSWAP08(j >> 12, 4, 5, 6, 7, 1, 0, 3, 2 ) << 12;
			k |= (j & 0x00f00) ^ 0x00a00;

			memcpy (Neo68KROMActive + 0x700000 + j, Neo68KROMActive + i + k, 0x100);
		}

		memcpy (Neo68KROMActive + i, Neo68KROMActive + 0x700000, 0x100000);
	}
}

static INT32 svcpcbInit()
{
	INT32 nRet;

	nNeoProtectionXor = 0x57;
	NeoCallbackActive->pInitialise = svcCallback;

	nRet = NeoPVCInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0xC2000, 0xFFAC28, { 0xC3, 0xFD, 0x81, 0xAC, 0x6D, 0xE7, 0xBF, 0x9E } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvsvcpcb = {
	"svcpcb", NULL, NULL, NULL, "2003",
	"SNK vs. Capcom - SVC Chaos (JAMMA PCB, set 1)\0", NULL, "SNK Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SNK_DEDICATED_PCB | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF | FBF_SF,
	NULL, svcpcbRomInfo, svcpcbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, svcpcbDIPInfo,
	svcpcbInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// SNK vs. Capcom - SVC Chaos (JAMMA PCB, set 2)

static struct BurnRomInfo svcpcbaRomDesc[] = {
	/* alt PCB version, this one has the same program roms as the MVS set, and different GFX / Sound rom arrangements */
	{ "269-p1a.p1",   0x400000,  0x38e2005e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "269-p2a.p2",   0x400000,  0x6d13797c, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "269-c1a.c1",   0x1000000, 0xe64d2b0c, 3 | BRF_GRA },           //  2 Sprite data
	{ "269-c2a.c2",   0x1000000, 0x249089c2, 3 | BRF_GRA },           //  3
	{ "269-c3a.c3",   0x1000000, 0xd32f2fab, 3 | BRF_GRA },           //  4
	{ "269-c4a.c4",   0x1000000, 0xbf77e878, 3 | BRF_GRA },           //  5

	{ "269-m1.m1",    0x080000,  0xf6819d00, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "269-v1a.v1",   0x1000000, 0xa6af4753, 5 | BRF_SND },           //  7 Sound data
};

STDROMPICKEXT(svcpcba, svcpcba, svcpcbBIOS)
STD_ROM_FN(svcpcba)

struct BurnDriver BurnDrvsvcpcba = {
	"svcpcba", "svcpcb", NULL, NULL, "2003",
	"SNK vs. Capcom - SVC Chaos (JAMMA PCB, set 2)\0", NULL, "SNK Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SNK_DEDICATED_PCB | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF | FBF_SF,
	NULL, svcpcbaRomInfo, svcpcbaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, svcpcbDIPInfo,
	svcpcbInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// SNK vs. Capcom - SVC Chaos (NGM-2690)(NGH-2690)
/* Encrypted Set */ /* MVS AND AES VERSION */

static struct BurnRomInfo svcRomDesc[] = {
	{ "269-p1.p1",    0x400000, 0x38e2005e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "269-p2.p2",    0x400000, 0x6d13797c, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "269-c1r.c1",   0x800000, 0x887b4068, 3 | BRF_GRA },           //  2 Sprite data
	{ "269-c2r.c2",   0x800000, 0x4e8903e4, 3 | BRF_GRA },           //  3
	{ "269-c3r.c3",   0x800000, 0x7d9c55b0, 3 | BRF_GRA },           //  4
	{ "269-c4r.c4",   0x800000, 0x8acb5bb6, 3 | BRF_GRA },           //  5
	{ "269-c5r.c5",   0x800000, 0x097a4157, 3 | BRF_GRA },           //  6
	{ "269-c6r.c6",   0x800000, 0xe19df344, 3 | BRF_GRA },           //  7
	{ "269-c7r.c7",   0x800000, 0xd8f0340b, 3 | BRF_GRA },           //  8
	{ "269-c8r.c8",   0x800000, 0x2570b71b, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "269-m1.m1",    0x080000,  0xf6819d00, 4 | BRF_ESS | BRF_PRG }, //  10 Z80 code

	/* Encrypted */
	{ "269-v1.v1",    0x800000, 0xc659b34c, 5 | BRF_SND },           // 11 Sound data
	{ "269-v2.v2",    0x800000, 0xdd903835, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(svc, svc, neogeo)
STD_ROM_FN(svc)

struct BurnDriver BurnDrvsvc = {
	"svc", NULL, "neogeo", NULL, "2003",
	"SNK vs. Capcom - SVC Chaos (NGM-2690)(NGH-2690)\0", NULL, "SNK Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF | FBF_SF,
	NULL, svcRomInfo, svcRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	svcpcbInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// SNK vs. Capcom - SVC Chaos (bootleg)

static struct BurnRomInfo svcbootRomDesc[] = {
	{ "svc-p1.bin",   0x800000, 0x0348f162, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "svc-s1.bin",   0x020000, 0x70b44df1, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "svc-c1.bin",   0x800000, 0xa7826b89, 3 | BRF_GRA },           //  2 Sprite data
	{ "svc-c2.bin",   0x800000, 0xed3c2089, 3 | BRF_GRA },           //  3
	{ "svc-c3.bin",   0x800000, 0x71ed8063, 3 | BRF_GRA },           //  4
	{ "svc-c4.bin",   0x800000, 0x250bde2d, 3 | BRF_GRA },           //  5
	{ "svc-c5.bin",   0x800000, 0x9817c082, 3 | BRF_GRA },           //  6
	{ "svc-c6.bin",   0x800000, 0x2bc0307f, 3 | BRF_GRA },           //  7
	{ "svc-c7.bin",   0x800000, 0x4358d7b9, 3 | BRF_GRA },           //  8
	{ "svc-c8.bin",   0x800000, 0x366deee5, 3 | BRF_GRA },           //  9

	{ "svc-m1.bin",   0x020000, 0x804328c3, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "svc-v2.bin",   0x400000, 0xb5097287, 5 | BRF_SND },           // 11 Sound data
	{ "svc-v1.bin",   0x400000, 0xbd3a391f, 5 | BRF_SND },           // 12
	{ "svc-v4.bin",   0x400000, 0x33fc0b37, 5 | BRF_SND },           // 13
	{ "svc-v3.bin",   0x400000, 0xaa9849a0, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(svcboot, svcboot, neogeo)
STD_ROM_FN(svcboot)

static void svcboot_sx_decode()
{
	for (INT32 i = 0; i < 0x20000 / 2; i++) {
		INT32 n = NeoTextROM[nNeoActiveSlot][i];
		NeoTextROM[nNeoActiveSlot][i] = NeoTextROM[nNeoActiveSlot][0x10000 + i];
		NeoTextROM[nNeoActiveSlot][0x10000 + i] = n;
	}
}

static void svcboot_decode()
{
	DoPerm(1);

	for (INT32 i = 0; i < 0x20000 / 2; i++) {
		INT32 n = NeoZ80ROMActive[i];
		NeoZ80ROMActive[i] = NeoZ80ROMActive[0x10000 + i];
		NeoZ80ROMActive[0x10000 + i] = n;
	}
}

static void svcbootCallback()
{
	INT32 i, j, k;
	for (i = 0x100000; i < 0x800000; i+=0x100000)
	{
		memcpy (Neo68KROMActive, Neo68KROMActive + i, 0x100000);

		for (j = 0; j < 0x100000; j++)
		{
			k  = (j & 0xfff00) + BITSWAP08(j, 7, 2, 1, 4, 3, 6, 5, 0);
			Neo68KROMActive[i + j] = Neo68KROMActive[k];
		}
	}

	memcpy (Neo68KROMActive, Neo68KROMActive + 0x700000, 0x100000);

	svcboot_sx_decode();
	svcboot_decode();
}


static INT32 svcbootInit()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = svcbootCallback;

	nRet = NeoPVCInit();

	if (nRet == 0) {
		BurnByteswap(YM2610ADPCMAROM[nNeoActiveSlot], 0x1000000);
	}

	return nRet;
}

struct BurnDriver BurnDrvsvcboot = {
	"svcboot", "svc", "neogeo", NULL, "2003",
	"SNK vs. Capcom - SVC Chaos (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF | FBF_SF,
	NULL, svcbootRomInfo, svcbootRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	svcbootInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// SNK vs. Capcom - SVC Chaos Plus (bootleg set 1)

static struct BurnRomInfo svcplusRomDesc[] = {
	{ "svc-p2p.bin",  0x200000, 0x50c0e2b7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "svc-p3p.bin",  0x200000, 0x58cdc293, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "svc-p1p.bin",  0x200000, 0xa194d842, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "svc-s1p.bin",  0x020000, 0x73344711, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "svc-c1.bin",   0x800000, 0xa7826b89, 3 | BRF_GRA },           //  4 Sprite data
	{ "svc-c2.bin",   0x800000, 0xed3c2089, 3 | BRF_GRA },           //  5
	{ "svc-c3.bin",   0x800000, 0x71ed8063, 3 | BRF_GRA },           //  6
	{ "svc-c4.bin",   0x800000, 0x250bde2d, 3 | BRF_GRA },           //  7
	{ "svc-c5.bin",   0x800000, 0x9817c082, 3 | BRF_GRA },           //  8
	{ "svc-c6.bin",   0x800000, 0x2bc0307f, 3 | BRF_GRA },           //  9
	{ "svc-c7.bin",   0x800000, 0x4358d7b9, 3 | BRF_GRA },           // 10
	{ "svc-c8.bin",   0x800000, 0x366deee5, 3 | BRF_GRA },           // 11

	{ "svc-m1.bin",   0x020000, 0x804328c3, 4 | BRF_ESS | BRF_PRG }, // 12 Z80 code

	{ "svc-v2.bin",   0x400000, 0xb5097287, 5 | BRF_SND },           // 13 Sound data
	{ "svc-v1.bin",   0x400000, 0xbd3a391f, 5 | BRF_SND },           // 14
	{ "svc-v4.bin",   0x400000, 0x33fc0b37, 5 | BRF_SND },           // 15
	{ "svc-v3.bin",   0x400000, 0xaa9849a0, 5 | BRF_SND },           // 16
};

STDROMPICKEXT(svcplus, svcplus, neogeo)
STD_ROM_FN(svcplus)

static void svcplusCallback()
{
	INT32 i, j, k;
	UINT8 *dst = (UINT8*)BurnMalloc(0x100000);

	if (dst)
	{
		for (i = 0; i < 0x600000; i+= 0x100000)
		{
			for (j = 0; j < 0x100000; j++)
			{
				k = BITSWAP24(j, 23,22,21,20,1,2,3,16,15,14,13,12,11,10,9,8,7,6,5,4,17,18,19,0);
				k ^= 0x0e000e;

				dst[j] = Neo68KROMActive[i + k];
			}

			memcpy (Neo68KROMActive + i, dst, 0x100000);
		}

		memmove (Neo68KROMActive + 0x100000, Neo68KROMActive, 0x500000);
		memcpy (Neo68KROMActive, dst, 0x100000);

		BurnFree (dst);
	}

	*((UINT16*)(Neo68KROMActive + 0xf8016)) = BURN_ENDIAN_SWAP_INT16(0x33c1); // Patch protected address

	lans2004_sx_decode();
	svcboot_decode();
}

static INT32 svcplusInit()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = svcplusCallback;

	nRet = NeoInit();

	if (nRet == 0) {
		BurnByteswap(YM2610ADPCMAROM[nNeoActiveSlot], 0x1000000);
	}

	return nRet;
}

struct BurnDriver BurnDrvsvcplus = {
	"svcplus", "svc", "neogeo", NULL, "2003",
	"SNK vs. Capcom - SVC Chaos Plus (bootleg set 1)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF | FBF_SF,
	NULL, svcplusRomInfo, svcplusRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	svcplusInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// SNK vs. Capcom - SVC Chaos Plus (bootleg set 2)

static struct BurnRomInfo svcplusaRomDesc[] = {
	{ "svc-p2pl.bin", 0x400000, 0x7231ace2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "svc-p1pl.bin", 0x200000, 0x16b44144, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "svc-s1pl.bin", 0x020000, 0xca3c735e, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "svc-c1.bin",   0x800000, 0xa7826b89, 3 | BRF_GRA },           //  3 Sprite data
	{ "svc-c2.bin",   0x800000, 0xed3c2089, 3 | BRF_GRA },           //  4
	{ "svc-c3.bin",   0x800000, 0x71ed8063, 3 | BRF_GRA },           //  5
	{ "svc-c4.bin",   0x800000, 0x250bde2d, 3 | BRF_GRA },           //  6
	{ "svc-c5.bin",   0x800000, 0x9817c082, 3 | BRF_GRA },           //  7
	{ "svc-c6.bin",   0x800000, 0x2bc0307f, 3 | BRF_GRA },           //  8
	{ "svc-c7.bin",   0x800000, 0x4358d7b9, 3 | BRF_GRA },           //  9
	{ "svc-c8.bin",   0x800000, 0x366deee5, 3 | BRF_GRA },           // 10

	{ "svc-m1.bin",   0x020000, 0x804328c3, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "svc-v2.bin",   0x400000, 0xb5097287, 5 | BRF_SND },           // 12 Sound data
	{ "svc-v1.bin",   0x400000, 0xbd3a391f, 5 | BRF_SND },           // 13
	{ "svc-v4.bin",   0x400000, 0x33fc0b37, 5 | BRF_SND },           // 14
	{ "svc-v3.bin",   0x400000, 0xaa9849a0, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(svcplusa, svcplusa, neogeo)
STD_ROM_FN(svcplusa)

static void svcplusaCallback()
{
	UINT8 *dst = (UINT8*)BurnMalloc(0x100000);
	if (dst)
	{
		memcpy (dst, Neo68KROMActive + 0x500000, 0x100000);
		memmove (Neo68KROMActive + 0x100000, Neo68KROMActive, 0x500000);
		memcpy (Neo68KROMActive, dst, 0x100000);
		BurnFree (dst);
		dst = NULL;
	}

	*((UINT16*)(Neo68KROMActive + 0xf8016)) = BURN_ENDIAN_SWAP_INT16(0x33c1); // Patch protected address

	svcboot_sx_decode();
	svcboot_decode();
}

static INT32 svcplusaInit()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = svcplusaCallback;

	nRet = NeoInit();

	if (nRet == 0) {
		BurnByteswap(YM2610ADPCMAROM[nNeoActiveSlot], 0x1000000);
	}

	return nRet;
}

struct BurnDriver BurnDrvsvcplusa = {
	"svcplusa", "svc", "neogeo", NULL, "2003",
	"SNK vs. Capcom - SVC Chaos Plus (bootleg set 2)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF | FBF_SF,
	NULL, svcplusaRomInfo, svcplusaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	svcplusaInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// SNK vs. Capcom - SVC Chaos Super Plus (bootleg)

static struct BurnRomInfo svcsplusRomDesc[] = {
	{ "svc-p1sp.bin", 0x400000, 0x2601902f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "svc-p2sp.bin", 0x400000, 0x0ca13305, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "svc-s1sp.bin", 0x020000, 0x233d6439, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "svc-c1.bin",   0x800000, 0xa7826b89, 3 | BRF_GRA },           //  3 Sprite data
	{ "svc-c2.bin",   0x800000, 0xed3c2089, 3 | BRF_GRA },           //  4
	{ "svc-c3.bin",   0x800000, 0x71ed8063, 3 | BRF_GRA },           //  5
	{ "svc-c4.bin",   0x800000, 0x250bde2d, 3 | BRF_GRA },           //  6
	{ "svc-c5.bin",   0x800000, 0x9817c082, 3 | BRF_GRA },           //  7
	{ "svc-c6.bin",   0x800000, 0x2bc0307f, 3 | BRF_GRA },           //  8
	{ "svc-c7.bin",   0x800000, 0x4358d7b9, 3 | BRF_GRA },           //  9
	{ "svc-c8.bin",   0x800000, 0x366deee5, 3 | BRF_GRA },           // 10

	{ "svc-m1.bin",   0x020000, 0x804328c3, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "svc-v2.bin",   0x400000, 0xb5097287, 5 | BRF_SND },           // 12 Sound data
	{ "svc-v1.bin",   0x400000, 0xbd3a391f, 5 | BRF_SND },           // 13
	{ "svc-v4.bin",   0x400000, 0x33fc0b37, 5 | BRF_SND },           // 14
	{ "svc-v3.bin",   0x400000, 0xaa9849a0, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(svcsplus, svcsplus, neogeo)
STD_ROM_FN(svcsplus)

static void svcsplus_sx_decode()
{
	for (INT32 i = 0; i < 0x20000; i++)
		NeoTextROM[nNeoActiveSlot][i] = BITSWAP08(NeoTextROM[nNeoActiveSlot][i], 7, 6, 0, 4, 3, 2, 1, 5);
}

static void svcsplusCallback()
{
	INT32 i, j, k;
	for (i = 0x100000; i < 0x800000; i+=0x10000)
	{
		memcpy (Neo68KROMActive, Neo68KROMActive + i, 0x10000);

		for (j = 0; j < 0x10000; j++)
		{
			k = BITSWAP16(j, 1, 9, 10, 12, 11, 13, 14, 5, 4, 2, 8, 7, 3, 6, 15, 0);
        		Neo68KROMActive[i + j] = Neo68KROMActive[k];
		}
	}

	memcpy (Neo68KROMActive, Neo68KROMActive + 0x600000, 0x100000);

	*((UINT16*)(Neo68KROMActive + 0x9e90)) = BURN_ENDIAN_SWAP_INT16(0x000f); // Enable S. Plus
	*((UINT16*)(Neo68KROMActive + 0x9e92)) = BURN_ENDIAN_SWAP_INT16(0xc9c0);
	*((UINT16*)(Neo68KROMActive + 0xa10c)) = BURN_ENDIAN_SWAP_INT16(0x4eb9); // Enable boss icons
	*((UINT16*)(Neo68KROMActive + 0xa10e)) = BURN_ENDIAN_SWAP_INT16(0x000e);
	*((UINT16*)(Neo68KROMActive + 0xa110)) = BURN_ENDIAN_SWAP_INT16(0x9750);

	svcsplus_sx_decode();
	svcboot_decode();
}

static INT32 svcsplusInit()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = svcsplusCallback;

	nRet = NeoPVCInit();

	if (nRet == 0) {
		BurnByteswap(YM2610ADPCMAROM[nNeoActiveSlot], 0x1000000);
	}

	return nRet;
}

struct BurnDriver BurnDrvsvcsplus = {
	"svcsplus", "svc", "neogeo", NULL, "2003",
	"SNK vs. Capcom - SVC Chaos Super Plus (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF | FBF_SF,
	NULL, svcsplusRomInfo, svcsplusRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	svcsplusInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Samurai Shodown V / Samurai Spirits Zero (NGM-2700, set 1)
/* Encrypted Set */ /* MVS VERSION */

static struct BurnRomInfo samsho5RomDesc[] = {
	{ "270-p1.p1",    0x400000, 0x4a2a09e6, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "270-p2.sp2",   0x400000, 0xe0c74c85, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "270-c1.c1",    0x800000, 0x14ffffac, 3 | BRF_GRA },           //  2 Sprite data
	{ "270-c2.c2",    0x800000, 0x401f7299, 3 | BRF_GRA },           //  3
	{ "270-c3.c3",    0x800000, 0x838f0260, 3 | BRF_GRA },           //  4
	{ "270-c4.c4",    0x800000, 0x041560a5, 3 | BRF_GRA },           //  5
	{ "270-c5.c5",    0x800000, 0xbd30b52d, 3 | BRF_GRA },           //  6
	{ "270-c6.c6",    0x800000, 0x86a69c70, 3 | BRF_GRA },           //  7
	{ "270-c7.c7",    0x800000, 0xd28fbc3c, 3 | BRF_GRA },           //  8
	{ "270-c8.c8",    0x800000, 0x02c530a6, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "270-m1.m1",    0x080000, 0x49c9901a, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "270-v1.v1",    0x800000, 0x62e434eb, 5 | BRF_SND },           // 11 Sound data
	{ "270-v2.v2",    0x800000, 0x180f3c9a, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(samsho5, samsho5, neogeo)
STD_ROM_FN(samsho5)

static void samsho5Callback()
{
	PCM2DecryptP2Info Info = { 0x000000, 0x080000, 0x700000, 0x680000, 0x500000, 0x180000, 0x200000, 0x480000, 0x300000, 0x780000, 0x600000, 0x280000, 0x100000, 0x580000, 0x400000, 0x380000 };

	PCM2DecryptP2(&Info);
}

static INT32 samsho5Init()
{
	nNeoProtectionXor = 0x0F;

	NeoCallbackActive->pInitialise = samsho5Callback;

	INT32 nRet = NeoInit();

	if (nRet == 0) {
		PCM2DecryptV2Info Info = { 0x0A000, 0xFEB2C0, { 0xCB, 0x29, 0x7D, 0x43, 0xD2, 0x3A, 0xC2, 0xB4 } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvsamsho5 = {
	"samsho5", NULL, "neogeo", NULL, "2003",
	"Samurai Shodown V / Samurai Spirits Zero (NGM-2700, set 1)\0", NULL, "Yuki Enterprise / SNK Playmore", "Neo Geo MVS",
	L"Samurai Shodown V\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4\u96F6 (NGM-2700, set 1)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho5RomInfo, samsho5RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	samsho5Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Samurai Shodown V / Samurai Spirits Zero (NGM-2700, set 2)
/* Encrypted Set */ /* MVS VERSION */
/* handwritten labels, possible hack of samsho5h?
   p1.bin                  270-p1c.p1              90.607002%
   p2.bin                  270-p2c.sp2             99.999763% (all bytes that differ do so by only bits 0x20 or 0x02, which is suspicious)
*/

static struct BurnRomInfo samsho5aRomDesc[] = {
	{ "p1.bin",    	  0x400000, 0x7795fffe, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "p2.bin",       0x400000, 0x2b844fe9, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "270-c1.c1",    0x800000, 0x14ffffac, 3 | BRF_GRA },           //  2 Sprite data
	{ "270-c2.c2",    0x800000, 0x401f7299, 3 | BRF_GRA },           //  3
	{ "270-c3.c3",    0x800000, 0x838f0260, 3 | BRF_GRA },           //  4
	{ "270-c4.c4",    0x800000, 0x041560a5, 3 | BRF_GRA },           //  5
	{ "270-c5.c5",    0x800000, 0xbd30b52d, 3 | BRF_GRA },           //  6
	{ "270-c6.c6",    0x800000, 0x86a69c70, 3 | BRF_GRA },           //  7
	{ "270-c7.c7",    0x800000, 0xd28fbc3c, 3 | BRF_GRA },           //  8
	{ "270-c8.c8",    0x800000, 0x02c530a6, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "270-m1.m1",    0x080000, 0x49c9901a, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "270-v1.v1",    0x800000, 0x62e434eb, 5 | BRF_SND },           // 11 Sound data
	{ "270-v2.v2",    0x800000, 0x180f3c9a, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(samsho5a, samsho5a, neogeo)
STD_ROM_FN(samsho5a)

struct BurnDriver BurnDrvsamsho5a = {
	"samsho5a", "samsho5", "neogeo", NULL, "2003",
	"Samurai Shodown V / Samurai Spirits Zero (NGM-2700, set 2)\0", NULL, "Yuki Enterprise / SNK Playmore", "Neo Geo MVS",
	L"Samurai Shodown V\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4\u96F6 (NGM-2700, set 2)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho5aRomInfo, samsho5aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	samsho5Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Samurai Shodown V / Samurai Spirits Zero (NGH-2700)
/* Encrypted Set, Alternate Set */ /* AES VERSION */

static struct BurnRomInfo samsho5hRomDesc[] = {
	{ "270-p1c.p1",   0x400000, 0xbf956089, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "270-p2c.sp2",  0x400000, 0x943a6b1d, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "270-c1.c1",    0x800000, 0x14ffffac, 3 | BRF_GRA },           //  2 Sprite data
	{ "270-c2.c2",    0x800000, 0x401f7299, 3 | BRF_GRA },           //  3
	{ "270-c3.c3",    0x800000, 0x838f0260, 3 | BRF_GRA },           //  4
	{ "270-c4.c4",    0x800000, 0x041560a5, 3 | BRF_GRA },           //  5
	{ "270-c5.c5",    0x800000, 0xbd30b52d, 3 | BRF_GRA },           //  6
	{ "270-c6.c6",    0x800000, 0x86a69c70, 3 | BRF_GRA },           //  7
	{ "270-c7.c7",    0x800000, 0xd28fbc3c, 3 | BRF_GRA },           //  8
	{ "270-c8.c8",    0x800000, 0x02c530a6, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "270-m1.m1",    0x080000, 0x49c9901a, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "270-v1.v1",    0x800000, 0x62e434eb, 5 | BRF_SND },           // 11 Sound data
	{ "270-v2.v2",    0x800000, 0x180f3c9a, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(samsho5h, samsho5h, neogeo)
STD_ROM_FN(samsho5h)

struct BurnDriver BurnDrvsamsho5h = {
	"samsho5h", "samsho5", "neogeo", NULL, "2003",
	"Samurai Shodown V / Samurai Spirits Zero (NGH-2700)\0", NULL, "Yuki Enterprise / SNK Playmore", "Neo Geo MVS",
	L"Samurai Shodown V\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4\u96F6 (NGH-2700)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho5hRomInfo, samsho5hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	samsho5Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Samurai Shodown V / Samurai Spirits Zero (bootleg)

static struct BurnRomInfo samsho5bRomDesc[] = {
	{ "ssv-p2.bin",   0x400000, 0x5023067f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "ssv-p1.bin",   0x400000, 0xb6cbe386, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "ssv-s1.bin",   0x020000, 0x70f667d0, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "ssv-c1.bin",   0x1000000, 0x9c564a01, 3 | BRF_GRA },           //  3 Sprite data
	{ "ssv-c2.bin",   0x1000000, 0x4b73b8da, 3 | BRF_GRA },           //  4
	{ "ssv-c3.bin",   0x1000000, 0x029f9bb5, 3 | BRF_GRA },           //  5
	{ "ssv-c4.bin",   0x1000000, 0x75722430, 3 | BRF_GRA },           //  6

	{ "ssv-m1.bin",   0x020000, 0x18114fb1, 4 | BRF_ESS | BRF_PRG }, // 7 Z80 code (decrypted)

	{ "ssv-v1.bin",   0x400000, 0xa3609761, 5 | BRF_SND },           // 8 Sound data
	{ "ssv-v2.bin",   0x400000, 0xcbd6ebd5, 5 | BRF_SND },           // 9
	{ "ssv-v3.bin",   0x400000, 0x6f1c2703, 5 | BRF_SND },           // 10
	{ "ssv-v4.bin",   0x400000, 0x5020c055, 5 | BRF_SND },           // 11
};

STDROMPICKEXT(samsho5b, samsho5b, neogeo)
STD_ROM_FN(samsho5b)

static void samsho5b_sx_decode()
{
	UINT8 *Buf = (UINT8*)BurnMalloc(0x20000);
	if (Buf) {
		memcpy(Buf, NeoTextROM[nNeoActiveSlot], 0x20000);

		for (INT32 i = 0; i < 0x20000; i += 0x10) {
			memcpy(&NeoTextROM[nNeoActiveSlot][i + 0], &Buf[i + 8], 8);
			memcpy(&NeoTextROM[nNeoActiveSlot][i + 8], &Buf[i + 0], 8);
		}

		BurnFree(Buf);
	}
}

static void samsho5b_vx_decode()
{
	for (INT32 i = 0; i < 0x400000 * 4; i++)
		YM2610ADPCMAROM[nNeoActiveSlot][i] = BITSWAP08(YM2610ADPCMAROM[nNeoActiveSlot][i], 0, 1, 5, 4, 3, 2, 6, 7);
}

static void samsho5bCallback()
{
	INT32 i, j, k;
	UINT8 *dst = (UINT8 *)BurnMalloc(0x100000);

	if (dst)
	{
		for (i = 0; i < 0x800000; i+=0x100000)
		{
			for (j = 0; j < 0x100000; j++)
			{
				k = BITSWAP08((j & 0xff), 7, 6, 5, 4, 1, 2, 3, 0);
				k += (j & 0xfffff00);
				k ^= 0x0c000a;

				dst[j] = Neo68KROMActive[i + k];
			}

			memcpy (Neo68KROMActive + i, dst, 0x100000);
		}

		memmove (Neo68KROMActive + 0x100000, Neo68KROMActive, 0x700000);
		memcpy (Neo68KROMActive, dst, 0x100000);

		BurnFree (dst);
	}

	samsho5b_sx_decode();
	lans2004_cx_decode(0x1000000 * 4);

}

static INT32 samsho5bInit()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = samsho5bCallback;

	nRet = NeoInit();

	if (nRet == 0) {
		samsho5b_vx_decode();
	}

	return nRet;
}

struct BurnDriver BurnDrvsamsho5b = {
	"samsho5b", "samsho5", "neogeo", NULL, "2003",
	"Samurai Shodown V / Samurai Spirits Zero (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	L"Samurai Shodown V\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4\u96F6 (bootleg)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho5bRomInfo, samsho5bRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	samsho5bInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2003 (Japan, JAMMA PCB)

static struct BurnRomInfo kf2k3pcbRomDesc[] = {
	{ "271-p1.p1",    0x0400000, 0xb9da070c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "271-p2.p2",    0x0400000, 0xda3118c4, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "271-p3.p3",    0x0100000, 0x5cefd0d2, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "271-c1.c1",    0x1000000, 0xf5ebb327, 3 | BRF_GRA },           //  3 Sprite data
	{ "271-c2.c2",    0x1000000, 0x2be21620, 3 | BRF_GRA },           //  4
	{ "271-c3.c3",    0x1000000, 0xddded4ff, 3 | BRF_GRA },           //  5
	{ "271-c4.c4",    0x1000000, 0xd85521e6, 3 | BRF_GRA },           //  6
	{ "271-c5.c5",    0x1000000, 0x18aa3540, 3 | BRF_GRA },           //  7
	{ "271-c6.c6",    0x1000000, 0x1c40de87, 3 | BRF_GRA },           //  8

	{ "271-m1.m1",    0x0080000, 0xd6bcf2bc, 4 | BRF_ESS | BRF_ESS }, //  9 Z80 code

	{ "271-v1.v1",    0x1000000, 0x1d96154b, 5 | BRF_SND },           // 10 Sound data
};

STDROMPICKEXT(kf2k3pcb, kf2k3pcb, kf2k3pcbBIOS)
STD_ROM_FN(kf2k3pcb)

void kf2k3pcb_bios_decode()
{
	INT32 i, j;
	static const INT32 address[0x40] = {
		0x04,0x0a,0x04,0x0a,0x04,0x0a,0x04,0x0a,
		0x0a,0x04,0x0a,0x04,0x0a,0x04,0x0a,0x04,
		0x09,0x07,0x09,0x07,0x09,0x07,0x09,0x07,
		0x09,0x09,0x04,0x04,0x09,0x09,0x04,0x04,
		0x0b,0x0d,0x0b,0x0d,0x03,0x05,0x03,0x05,
		0x0e,0x0e,0x03,0x03,0x0e,0x0e,0x03,0x03,
		0x03,0x05,0x0b,0x0d,0x03,0x05,0x0b,0x0d,
		0x04,0x00,0x04,0x00,0x0e,0x0a,0x0e,0x0a
	};

	extern UINT8 *Neo68KBIOS;
	UINT16 *src = (UINT16*)Neo68KBIOS;
	UINT16 *dst = (UINT16*)BurnMalloc( 0x80000 );

	for (i = 0; i < 0x80000 / 2; i++) {
				  j  = i;
		if ( i & 0x00020) j ^= 0x0010;
		if (~i & 0x00010) j ^= 0x0040;
		j ^= 0x00a0;
		if ( i & 0x00004) j ^= 0x0080;
		if ( i & 0x00200) j ^= 0x0100;
		if (~i & 0x02000) j ^= 0x0400;
		if (~i & 0x10000) j ^= 0x1000;
		if ( i & 0x02000) j ^= 0x8000;
		j ^= address[((i >> 1) & 0x38) | (i & 7)];

				     dst[i]  = src[j];
		if (BURN_ENDIAN_SWAP_INT16(dst[i]) & 0x0004) dst[i] ^= BURN_ENDIAN_SWAP_INT16(0x0001);
		if (BURN_ENDIAN_SWAP_INT16(dst[i]) & 0x0010) dst[i] ^= BURN_ENDIAN_SWAP_INT16(0x0002);
		if (BURN_ENDIAN_SWAP_INT16(dst[i]) & 0x0020) dst[i] ^= BURN_ENDIAN_SWAP_INT16(0x0008);
	}

	memcpy (src, dst, 0x80000);

	BurnFree (dst);
}

static void kf2k3pcbCallback()
{
	INT32 i, j, k;
	for (i = 0; i < 0x100000; i++)
	        Neo68KROMActive[0x800000 + i] ^= Neo68KROMActive[0x100002 | i];

	for (i = 0x100000; i < 0x800000; i++)
		Neo68KROMActive[i] ^= ~Neo68KROMActive[0x7fffe0 + (i & 0x1f)];

	for (i = 0x100000; i < 0x700000; i += 4) {
		UINT16 rom16 = BURN_UNALIGNED_READ16(Neo68KROMActive + i + 1);
		rom16 = BITSWAP16(rom16, 15, 14, 13, 12, 4, 5, 6, 7, 8, 9, 10, 11, 3, 2, 1, 0);
		BURN_UNALIGNED_WRITE16(Neo68KROMActive + i + 1, rom16);
	}

	memcpy (Neo68KROMActive + 0x700000, Neo68KROMActive, 0x100000);

	for (i = 0; i < 0x0100000 / 0x10000; i++) {
		j = BITSWAP08(i, 7, 6, 5, 4, 1, 0, 3, 2);
		memcpy (Neo68KROMActive + i * 0x010000, Neo68KROMActive + 0x700000 + j * 0x010000, 0x010000);
	}

	memmove (Neo68KROMActive + 0x200000, Neo68KROMActive + 0x100000, 0x600000);

	for (i = 0x200000; i < 0x900000; i += 0x100000)
	{
		for (j = 0; j < 0x100000; j += 0x100)
		{
			k  = (j & 0xf00) ^ 0x00300;
			k |= BITSWAP08(j >> 12, 4, 5, 6, 7, 1, 0, 3, 2 ) << 12;

			memcpy (Neo68KROMActive + 0x100000 + j, Neo68KROMActive + i + k, 0x100);
		}

		memcpy (Neo68KROMActive + i, Neo68KROMActive + 0x100000, 0x100000);
	}
}

static INT32 kf2k3pcbInit()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = kf2k3pcbCallback;

	nNeoTextROMSize[nNeoActiveSlot] = 0x100000;
	nNeoProtectionXor = 0x9D;

	nRet = NeoPVCInit();

	if (nRet == 0) {
		PCM2DecryptV2Info Info = { 0xa7001, 0xff14ea, { 0x4b, 0xa4, 0x63, 0x46, 0xf0, 0x91, 0xea, 0x62 } };

		PCM2DecryptV2(&Info);

		// M1 has additional swap
		for (INT32 i = 0; i < 0x80000; i++) {
			NeoZ80ROMActive[i] = BITSWAP08(NeoZ80ROMActive[i], 5, 6, 1, 4, 3, 0, 7, 2);
		}
	}

	return nRet;
}

struct BurnDriver BurnDrvkf2k3pcb = {
	"kf2k3pcb", NULL, NULL, NULL, "2003",
	"The King of Fighters 2003 (Japan, JAMMA PCB)\0", NULL, "Playmore / Capcom", "dedicated Neo Geo PCB",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SNK_DEDICATED_PCB | HARDWARE_SNK_CMC50 | HARDWARE_SNK_KOF2K3 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k3pcbRomInfo, kf2k3pcbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, kf2k3pcbDIPInfo,
	kf2k3pcbInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters 2003 (NGM-2710)
/* Encrypted Code + Sound + GFX Roms */ /* MVS VERSION */

static struct BurnRomInfo kof2003RomDesc[] = {
	{ "271-p1c.p1",   0x400000, 0x530ecc14, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "271-p2c.p2",   0x400000, 0xfd568da9, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "271-p3c.p3",   0x100000, 0xaec5b4a9, 1 | BRF_ESS | BRF_PRG }, //  2

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "271-c1c.c1",   0x800000, 0xb1dc25d0, 3 | BRF_GRA },           //  3 Sprite data
	{ "271-c2c.c2",   0x800000, 0xd5362437, 3 | BRF_GRA },           //  4
	{ "271-c3c.c3",   0x800000, 0x0a1fbeab, 3 | BRF_GRA },           //  5
	{ "271-c4c.c4",   0x800000, 0x87b19a0c, 3 | BRF_GRA },           //  6
	{ "271-c5c.c5",   0x800000, 0x704ea371, 3 | BRF_GRA },           //  7
	{ "271-c6c.c6",   0x800000, 0x20a1164c, 3 | BRF_GRA },           //  8
	{ "271-c7c.c7",   0x800000, 0x189aba7f, 3 | BRF_GRA },           //  9
	{ "271-c8c.c8",   0x800000, 0x20ec4fdc, 3 | BRF_GRA },           // 10

	/* Encrypted */
	{ "271-m1c.m1",   0x080000, 0xf5515629, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	/* Encrypted */
	{ "271-v1c.v1",   0x800000, 0xffa3f8c7, 5 | BRF_SND },           // 12 Sound data
	{ "271-v2c.v2",   0x800000, 0x5382c7d1, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kof2003, kof2003, neogeo)
STD_ROM_FN(kof2003)

static void kof2003Callback()
{
	INT32 i, j, k;
	for (i = 0; i < 0x100000; i++)
		Neo68KROMActive[i] ^= ~Neo68KROMActive[0x0fffe0 + (i & 0x1f)];

	for (i = 0; i < 0x100000; i++)
	        Neo68KROMActive[0x800000 + i] ^= Neo68KROMActive[0x100002 | i];

	for (i = 0x100000; i < 0x800000; i++)
		Neo68KROMActive[i] ^= ~Neo68KROMActive[0x7fffe0 + (i & 0x1f)];

	for (i = 0x100000; i < 0x800000; i += 4) {
		UINT16 rom16 = BURN_UNALIGNED_READ16(Neo68KROMActive + i + 1);
		rom16 = BITSWAP16(rom16, 15, 14, 13, 12, 5, 4, 7, 6, 9, 8, 11, 10, 3, 2, 1, 0);
		BURN_UNALIGNED_WRITE16(Neo68KROMActive + i + 1, rom16);
	}

	memcpy (Neo68KROMActive + 0x700000, Neo68KROMActive, 0x100000);

	for (i = 0; i < 0x0100000 / 0x10000; i++) {
		j = BITSWAP08(i, 7, 6, 5, 4, 0, 1, 2, 3);
		memcpy (Neo68KROMActive + i * 0x010000, Neo68KROMActive + 0x700000 + j * 0x010000, 0x010000);
	}

	memmove (Neo68KROMActive + 0x200000, Neo68KROMActive + 0x100000, 0x600000);

	for (i = 0x200000; i < 0x900000; i += 0x100000)
	{
		for (j = 0; j < 0x100000; j += 0x100)
		{
			k  = (j & 0xf00) ^ 0x00800;
			k |= BITSWAP08(j >> 12, 4, 5, 6, 7, 1, 0, 3, 2 ) << 12;

			memcpy (Neo68KROMActive + 0x100000 + j, Neo68KROMActive + i + k, 0x100);
		}

		memcpy (Neo68KROMActive + i, Neo68KROMActive + 0x100000, 0x100000);
	}
}

static INT32 kof2003Init()
{
	INT32 nRet;

	nNeoProtectionXor = 0x9D;
	NeoCallbackActive->pInitialise = kof2003Callback;

	nRet = NeoPVCInit();

	if (nRet == 0) {
		PCM2DecryptV2Info Info = { 0xa7001, 0xff14ea, { 0x4b, 0xa4, 0x63, 0x46, 0xf0, 0x91, 0xea, 0x62 } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkof2003 = {
	"kof2003", NULL, "neogeo", NULL, "2003",
	"The King of Fighters 2003 (NGM-2710)\0", NULL, "SNK Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2003RomInfo, kof2003RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2003Init, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2003 (NGH-2710)
/* Encrypted Code + Sound + GFX Roms */ /* AES VERSION */

static struct BurnRomInfo kof2003hRomDesc[] = {
	/* All chip labels for this set are correct */
	{ "271-p1k.p1",           0x400000, 0xd0d0ae3e, 1 | BRF_ESS | BRF_PRG }, //  0 68K Code
	{ "271-p2k.p2",           0x400000, 0xfb3f79d9, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "271-p3k.p3",           0x100000, 0x232702ad, 1 | BRF_ESS | BRF_PRG }, //  2

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "271-c1k.c1",           0x800000, 0xefb9dd24, 3 | BRF_GRA },		 //  3 Sprite data
	{ "271-c2k.c2",           0x800000, 0x3fb90447, 3 | BRF_GRA },		 //  4
	{ "271-c3k.c3",           0x800000, 0x27950f28, 3 | BRF_GRA },		 //  5
	{ "271-c4k.c4",           0x800000, 0x735177f8, 3 | BRF_GRA },		 //  6
	{ "271-c5k.c5",           0x800000, 0xa51b7c0f, 3 | BRF_GRA },		 //  7
	{ "271-c6k.c6",           0x800000, 0xd5cae4e0, 3 | BRF_GRA },		 //  8
	{ "271-c7k.c7",           0x800000, 0xe65ae2d0, 3 | BRF_GRA },		 //  9
	{ "271-c8k.c8",           0x800000, 0x312f528c, 3 | BRF_GRA },		 // 10

	/* Encrypted */
	{ "271-m1k.m1",           0x080000, 0x48d9affe, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	/* Encrypted */
	{ "271-v1c.v1",           0x800000, 0xffa3f8c7, 5 | BRF_SND },		 // 12 Sound data
	{ "271-v2c.v2",           0x800000, 0x5382c7d1, 5 | BRF_SND },		 // 13
};

STDROMPICKEXT(kof2003h, kof2003h, neogeo)
STD_ROM_FN(kof2003h)

static void kof2003hCallback()
{
	INT32 i, j, k;
	for (i = 0; i < 0x100000; i++)
		Neo68KROMActive[i] ^= ~Neo68KROMActive[0x0fffe0 + (i & 0x1f)];

	for (i = 0; i < 0x100000; i++)
	        Neo68KROMActive[0x800000 + i] ^= Neo68KROMActive[0x100002 | i];

	for (i = 0x100000; i < 0x800000; i++)
		Neo68KROMActive[i] ^= ~Neo68KROMActive[0x7fffe0 + (i & 0x1f)];

	for (i = 0x100000; i < 0x800000; i += 4) {
		UINT16 rom16 = BURN_UNALIGNED_READ16(Neo68KROMActive + i + 1);
		rom16 = BITSWAP16(rom16, 15, 14, 13, 12, 10, 11, 8, 9, 6, 7, 4, 5, 3, 2, 1, 0);
		BURN_UNALIGNED_WRITE16(Neo68KROMActive + i + 1, rom16);
	}

	memcpy (Neo68KROMActive + 0x700000, Neo68KROMActive, 0x100000);

	for (i = 0; i < 0x0100000 / 0x10000; i++) {
		j = BITSWAP08(i, 7, 6, 5, 4, 1, 0, 3, 2);
		memcpy (Neo68KROMActive + i * 0x010000, Neo68KROMActive + 0x700000 + j * 0x010000, 0x010000);
	}

	memmove (Neo68KROMActive + 0x200000, Neo68KROMActive + 0x100000, 0x600000);

	for (i = 0x200000; i < 0x900000; i += 0x100000)
	{
		for (j = 0; j < 0x100000; j += 0x100)
		{
			k  = (j & 0xf00) ^ 0x00400;
			k |= BITSWAP08(j >> 12, 6, 7, 4, 5, 0, 1, 2, 3) << 12;

			memcpy (Neo68KROMActive + 0x100000 + j, Neo68KROMActive + i + k, 0x100);
		}

		memcpy (Neo68KROMActive + i, Neo68KROMActive + 0x100000, 0x100000);
	}
}

static INT32 kof2003hInit()
{
	INT32 nRet;

	nNeoProtectionXor = 0x9D;
	NeoCallbackActive->pInitialise = kof2003hCallback;

	nRet = NeoPVCInit();

	if (nRet == 0) {
		PCM2DecryptV2Info Info = { 0xa7001, 0xff14ea, { 0x4b, 0xa4, 0x63, 0x46, 0xf0, 0x91, 0xea, 0x62 } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkof2003h = {
	"kof2003h", "kof2003", "neogeo", NULL, "2003",
	"The King of Fighters 2003 (NGH-2710)\0", NULL, "SNK Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2003hRomInfo, kof2003hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2003hInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2003 (bootleg set 1)

static struct BurnRomInfo kf2k3blRomDesc[] = {
	{ "2k3-p1.bin",   0x400000, 0x92ed6ee3, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "2k3-p2.bin",   0x400000, 0x5d3d8bb3, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "2k3-s1.bin",   0x020000, 0x482c48a5, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "271-c1c.c1",   0x800000, 0xb1dc25d0, 3 | BRF_GRA },           //  3 Sprite data
	{ "271-c2c.c2",   0x800000, 0xd5362437, 3 | BRF_GRA },           //  4
	{ "271-c3c.c3",   0x800000, 0x0a1fbeab, 3 | BRF_GRA },           //  5
	{ "271-c4c.c4",   0x800000, 0x87b19a0c, 3 | BRF_GRA },           //  6
	{ "271-c5c.c5",   0x800000, 0x704ea371, 3 | BRF_GRA },           //  7
	{ "271-c6c.c6",   0x800000, 0x20a1164c, 3 | BRF_GRA },           //  8
	{ "271-c7c.c7",   0x800000, 0x189aba7f, 3 | BRF_GRA },           //  9
	{ "271-c8c.c8",   0x800000, 0x20ec4fdc, 3 | BRF_GRA },           // 10

	{ "2k3-m1.bin",   0x020000, 0x3a4969ff, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "271-v1c.v1",   0x800000, 0xffa3f8c7, 5 | BRF_SND },           // 12 Sound data
	{ "271-v2c.v2",   0x800000, 0x5382c7d1, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k3bl, kf2k3bl, neogeo)
STD_ROM_FN(kf2k3bl)

UINT8 __fastcall kf2k3blReadByteProtection(UINT32)
{
	return PVCRAM[0x1ff2];
}

UINT16 __fastcall kf2k3blReadWordProtection(UINT32)
{
	return PVCRAM[0x1ff2];
}

static void kf2k3blCallback()
{
	memmove (Neo68KROMActive + 0x100000, Neo68KROMActive, 0x700000);
	memcpy (Neo68KROMActive, Neo68KROMActive + 0x700000, 0x100000);

	lans2004_sx_decode();
}

static void kf2k3blInstallHandlers()
{
	NeoPVCInstallHandlers();

	SekMapHandler(7,    0x058196,    0x058197,  MAP_READ);
	SekSetReadByteHandler(7,  kf2k3blReadByteProtection);
}

static INT32 kf2k3blInit()
{
	INT32 nRet;

	nNeoProtectionXor = 0x9D;
	NeoCallbackActive->pInitialise = kf2k3blCallback;

	nRet = NeoPVCInit();

	NeoCallbackActive->pInstallHandlers = kf2k3blInstallHandlers;

	if (nRet == 0) {
		PCM2DecryptV2Info Info = { 0xa7001, 0xff14ea, { 0x4b, 0xa4, 0x63, 0x46, 0xf0, 0x91, 0xea, 0x62 } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkf2k3bl = {
	"kf2k3bl", "kof2003", "neogeo", NULL, "2003",
	"The King of Fighters 2003 (bootleg set 1)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k3blRomInfo, kf2k3blRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf2k3blInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2003 (bootleg set 2)

static struct BurnRomInfo kf2k3blaRomDesc[] = {
	{ "2k3-p1bl.bin", 0x100000, 0x4ea414dd, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "2k3-p3bl.bin", 0x400000, 0x370acbff, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "2k3-p2bl.bin", 0x200000, 0x9c04fc52, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "2k3-s1.bin",   0x020000, 0x482c48a5, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "271-c1c.c1",   0x800000, 0xb1dc25d0, 3 | BRF_GRA },           //  3 Sprite data
	{ "271-c2c.c2",   0x800000, 0xd5362437, 3 | BRF_GRA },           //  4
	{ "271-c3c.c3",   0x800000, 0x0a1fbeab, 3 | BRF_GRA },           //  5
	{ "271-c4c.c4",   0x800000, 0x87b19a0c, 3 | BRF_GRA },           //  6
	{ "271-c5c.c5",   0x800000, 0x704ea371, 3 | BRF_GRA },           //  7
	{ "271-c6c.c6",   0x800000, 0x20a1164c, 3 | BRF_GRA },           //  8
	{ "271-c7c.c7",   0x800000, 0x189aba7f, 3 | BRF_GRA },           //  9
	{ "271-c8c.c8",   0x800000, 0x20ec4fdc, 3 | BRF_GRA },           // 10

	{ "2k3-m1.bin",   0x020000, 0x3a4969ff, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "271-v1c.v1",   0x800000, 0xffa3f8c7, 5 | BRF_SND },           // 12 Sound data
	{ "271-v2c.v2",   0x800000, 0x5382c7d1, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k3bla, kf2k3bla, neogeo)
STD_ROM_FN(kf2k3bla)

static void kf2k3blaCallback()
{
	INT32 i, j, k;
	UINT8 *dst = (UINT8 *)BurnMalloc(0x100000);
	if (dst)
	{
		for (i = 0; i < 0x700000; i += 0x100000)
		{
			memcpy(dst, Neo68KROMActive + i, 0x100000);

			for (j = 0; j < 0x100000; j++)
			{
				k = BITSWAP24(j, 23,22,21,20,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,0);
				Neo68KROMActive[i + j] = dst[k];
			}
		}

		BurnFree (dst);
	}

	// patched by Altera protection chip
	*((UINT16*)(Neo68KROMActive + 0x0f38ac)) = BURN_ENDIAN_SWAP_INT16(0x4e75);

	lans2004_sx_decode();
}

void __fastcall kf2k3blaWriteWordBankswitch(UINT32 sekAddress, UINT16 wordValue)
{
	*((UINT16*)(PVCRAM + (sekAddress & 0x1ffe))) = BURN_ENDIAN_SWAP_INT16(wordValue);

	if (sekAddress == 0x2ffff2)
	{
		UINT32 nBank = ((PVCRAM[0x1ff3] << 16) | (PVCRAM[0x1ff2] << 8) | PVCRAM[0x1ff0]) + 0x100000;

		if (nBank != nNeo68KROMBank)
		{
			nNeo68KROMBank = nBank;
			SekMapMemory(Neo68KROMActive + nNeo68KROMBank, 0x200000, 0x2fdfff, MAP_ROM);
		}
	}
}

static void kf2k3blaInstallHandlers()
{
	NeoPVCInstallHandlers();

	SekMapHandler(7,         0x2ffff0, 0x2fffff, MAP_WRITE);
	SekSetWriteWordHandler(7, kf2k3blaWriteWordBankswitch);
}

static INT32 kf2k3blaInit()
{
	INT32 nRet;

	nNeoProtectionXor = 0x9D;

	NeoCallbackActive->pInitialise = kf2k3blaCallback;

	nRet = NeoPVCInit();

	NeoCallbackActive->pInstallHandlers = kf2k3blaInstallHandlers;

	if (nRet == 0) {
		PCM2DecryptV2Info Info = { 0xa7001, 0xff14ea, { 0x4b, 0xa4, 0x63, 0x46, 0xf0, 0x91, 0xea, 0x62 } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkf2k3bla = {
	"kf2k3bla", "kof2003", "neogeo", NULL, "2003",
	"The King of Fighters 2003 (bootleg set 2)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k3blaRomInfo, kf2k3blaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf2k3blaInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2004 Plus / Hero (bootleg)

static struct BurnRomInfo kf2k3plRomDesc[] = {
	{ "2k3-p1pl.bin", 0x100000, 0x07b84112, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "2k3-p3bl.bin", 0x400000, 0x370acbff, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "2k3-p2bl.bin", 0x200000, 0x9c04fc52, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "2k3-s1pl.bin", 0x020000, 0xad548a36, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "271-c1c.c1",   0x800000, 0xb1dc25d0, 3 | BRF_GRA },           //  3 Sprite data
	{ "271-c2c.c2",   0x800000, 0xd5362437, 3 | BRF_GRA },           //  4
	{ "271-c3c.c3",   0x800000, 0x0a1fbeab, 3 | BRF_GRA },           //  5
	{ "271-c4c.c4",   0x800000, 0x87b19a0c, 3 | BRF_GRA },           //  6
	{ "271-c5c.c5",   0x800000, 0x704ea371, 3 | BRF_GRA },           //  7
	{ "271-c6c.c6",   0x800000, 0x20a1164c, 3 | BRF_GRA },           //  8
	{ "271-c7c.c7",   0x800000, 0x189aba7f, 3 | BRF_GRA },           //  9
	{ "271-c8c.c8",   0x800000, 0x20ec4fdc, 3 | BRF_GRA },           // 10

	{ "2k3-m1.bin",   0x020000, 0x3a4969ff, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "271-v1c.v1",   0x800000, 0xffa3f8c7, 5 | BRF_SND },           // 12 Sound data
	{ "271-v2c.v2",   0x800000, 0x5382c7d1, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k3pl, kf2k3pl, neogeo)
STD_ROM_FN(kf2k3pl)

struct BurnDriver BurnDrvkf2k3pl = {
	"kf2k3pl", "kof2003", "neogeo", NULL, "2003",
	"The King of Fighters 2004 Plus / Hero (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k3plRomInfo, kf2k3plRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf2k3blaInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// The King of Fighters 2004 Ultra Plus (bootleg)

static struct BurnRomInfo kf2k3uplRomDesc[] = {
	{ "2k3-p1up.bin", 0x800000, 0x87294c01, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "2k3-s1up.bin", 0x020000, 0xe5708c0c, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "271-c1c.c1",   0x800000, 0xb1dc25d0, 3 | BRF_GRA },           //  2 Sprite data
	{ "271-c2c.c2",   0x800000, 0xd5362437, 3 | BRF_GRA },           //  3
	{ "271-c3c.c3",   0x800000, 0x0a1fbeab, 3 | BRF_GRA },           //  4
	{ "271-c4c.c4",   0x800000, 0x87b19a0c, 3 | BRF_GRA },           //  5
	{ "271-c5c.c5",   0x800000, 0x704ea371, 3 | BRF_GRA },           //  6
	{ "271-c6c.c6",   0x800000, 0x20a1164c, 3 | BRF_GRA },           //  7
	{ "271-c7c.c7",   0x800000, 0x189aba7f, 3 | BRF_GRA },           //  8
	{ "271-c8c.c8",   0x800000, 0x20ec4fdc, 3 | BRF_GRA },           //  9

	{ "2k3-m1.bin",   0x020000, 0x3a4969ff, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "271-v1c.v1",   0x800000, 0xffa3f8c7, 5 | BRF_SND },           // 11 Sound data
	{ "271-v2c.v2",   0x800000, 0x5382c7d1, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kf2k3upl, kf2k3upl, neogeo)
STD_ROM_FN(kf2k3upl)

static void kf2k3uplCallback()
{
	INT32 i, j;
	memmove (Neo68KROMActive + 0x100000, Neo68KROMActive, 0x600000);
	memcpy (Neo68KROMActive, Neo68KROMActive + 0x700000, 0x100000);

	for(i = 0; i < 0x2000; i++) {
		j = (i & 0x1f00) | BITSWAP08(i & 0xff, 7, 1, 5, 4, 3, 2, 6, 0) ;
		Neo68KROMActive[0xfe000 + i] = Neo68KROMActive[0xd0610 + j];
	}

	garoubl_sx_decode();
}

static INT32 kof2k3uplInit()
{
	INT32 nRet;

	nNeoProtectionXor = 0x9D;
	NeoCallbackActive->pInitialise = kf2k3uplCallback;

	nRet = NeoPVCInit();

	NeoCallbackActive->pInstallHandlers = kf2k3blInstallHandlers;

	if (nRet == 0) {
		PCM2DecryptV2Info Info = { 0xa7001, 0xff14ea, { 0x4b, 0xa4, 0x63, 0x46, 0xf0, 0x91, 0xea, 0x62 } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkf2k3upl = {
	"kf2k3upl", "kof2003", "neogeo", NULL, "2003",
	"The King of Fighters 2004 Ultra Plus (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k3uplRomInfo, kf2k3uplRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2k3uplInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2003 - PS2 Style Portraits (Hack)
// Hack By 0 Day-S,Eddids,Hiker

static struct BurnRomInfo kf2k3ps2spRomDesc[] = {
	{ "271ps2sp.p1",     0x400000, 0xa2b1cd0b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "271ps2sp.p2",     0x400000, 0xc2321f79, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "271ps2sp.p3",     0x100000, 0x48af7f48, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "271ps2sp.c1",     0x800000, 0x3c43a170, 3 | BRF_GRA },           //  3 Sprite data
	{ "271ps2sp.c2",     0x800000, 0xd8a64d42, 3 | BRF_GRA },           //  4
	{ "271ps2sp.c3",     0x800000, 0xd818be4e, 3 | BRF_GRA },           //  5
	{ "271ps2sp.c4",     0x800000, 0xeb8bd6d6, 3 | BRF_GRA },           //  6
	{ "271ps2sp.c5",     0x800000, 0x53bb6706, 3 | BRF_GRA },           //  7
	{ "271ps2sp.c6",     0x800000, 0x993254d5, 3 | BRF_GRA },           //  8
	{ "271ps2sp.c7",     0x800000, 0x51db8ffe, 3 | BRF_GRA },           //  9
	{ "271ps2sp.c8",     0x800000, 0x7dad01ab, 3 | BRF_GRA },           // 10

	{ "271-m1c.m1",      0x080000, 0xf5515629, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "271-v1c.v1",      0x800000, 0xffa3f8c7, 5 | BRF_SND },           // 12 Sound data
	{ "271-v2c.v2",      0x800000, 0x5382c7d1, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k3ps2sp, kf2k3ps2sp, neogeo)
STD_ROM_FN(kf2k3ps2sp)

struct BurnDriver BurnDrvkf2k3ps2sp = {
	"kf2k3ps2sp", "kof2003", "neogeo", NULL, "20??",
	"The King of Fighters 2003 - PS2 Style Portraits (Hack By 0 Day-S, Eddids, Hiker)\0", "Secret Characters available in MVS", "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k3ps2spRomInfo, kf2k3ps2spRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2003Init, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Samurai Shodown V Special / Samurai Spirits Zero Special (NGM-2720)
/* Encrypted Set */ /* MVS VERSION */

static struct BurnRomInfo samsh5spRomDesc[] = {
	/* Uncensored */
	{ "272-p1.p1",    0x400000, 0xfb7a6bba, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "272-p2.sp2",   0x400000, 0x63492ea6, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "272-c1.c1",    0x800000, 0x4f97661a, 3 | BRF_GRA },           //  2 Sprite data
	{ "272-c2.c2",    0x800000, 0xa3afda4f, 3 | BRF_GRA },           //  3
	{ "272-c3.c3",    0x800000, 0x8c3c7502, 3 | BRF_GRA },           //  4
	{ "272-c4.c4",    0x800000, 0x32d5e2e2, 3 | BRF_GRA },           //  5
	{ "272-c5.c5",    0x800000, 0x6ce085bc, 3 | BRF_GRA },           //  6
	{ "272-c6.c6",    0x800000, 0x05c8dc8e, 3 | BRF_GRA },           //  7
	{ "272-c7.c7",    0x800000, 0x1417b742, 3 | BRF_GRA },           //  8
	{ "272-c8.c8",    0x800000, 0xd49773cd, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "272-m1.m1",    0x080000, 0xadeebf40, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "272-v1.v1",    0x800000, 0x76a94127, 5 | BRF_SND },           // 11 Sound data
	{ "272-v2.v2",    0x800000, 0x4ba507f1, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(samsh5sp, samsh5sp, neogeo)
STD_ROM_FN(samsh5sp)

static void samsh5spCallback()
{
	PCM2DecryptP2Info Info = { 0x000000, 0x080000, 0x500000, 0x480000, 0x600000, 0x580000, 0x700000, 0x280000, 0x100000, 0x680000, 0x400000, 0x780000, 0x200000, 0x380000, 0x300000, 0x180000 };

	PCM2DecryptP2(&Info);
}

static INT32 samsh5spInit()
{
	INT32 nRet;

//	nNeoTextROMSize = 0x20000;
	nNeoProtectionXor = 0x0D;
	NeoCallbackActive->pInitialise = samsh5spCallback;

	nRet = NeoInit();

	if (nRet == 0) {
		PCM2DecryptV2Info Info = { 0x02000, 0xffb440, { 0x4b, 0xa4, 0x63, 0x46, 0xf0, 0x91, 0xea, 0x62 } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvsamsh5sp = {
	"samsh5sp", NULL, "neogeo", NULL, "2004",
	"Samurai Shodown V Special / Samurai Spirits Zero Special (NGM-2720)\0", NULL, "Yuki Enterprise / SNK Playmore", "Neo Geo MVS",
	L"Samurai Shodown V Special\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4\u96F6 Special  (NGM-2720)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsh5spRomInfo, samsh5spRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	samsh5spInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

/*  Some info about the 2nd AES release of Samurai Shodown 5 Special (samsh5sph):

    The fixed carts have a small round neogeo sticker applied to the front side of the cart (top right near cart sticker).
    SNK Playmore had authorized a recall of all Samurai Shodown V Special (Samurai Spirits 0 Special) home cartridges. This recall involved bug fixes
    and the addition of fatalities. (The fatalities were originally removed at the last minute due to the Nagasaki incident, a murder caused by a child killing her classmate by knife.)
    Bug fixes: Improvements on Voice, Back Ground Music, and Practice mode.
    Fatalities: SNK PLAYMORE modified the game program by including the removed "Zetumei Ougi" in a modified version.
    This new version does not show the complete fatalities, they are instead replaced by what SNK PLAYMORE refers to as "lessened fatalities".
*/

// Samurai Shodown V Special / Samurai Spirits Zero Special (NGH-2720) (2nd release, less censored)
/* Encrypted Set */ /* AES VERSION, 2nd bugfix release */

static struct BurnRomInfo samsh5sphRomDesc[] = {
	/* Less censored */
	{ "272-p1ca.p1",  0x400000, 0xc30a08dd, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "272-p2ca.sp2", 0x400000, 0xbd64a518, 1 | BRF_ESS | BRF_PRG }, //  1
	/* Correct chip labels unknown */

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "272-c1.c1",    0x800000, 0x4f97661a, 3 | BRF_GRA },           //  2 Sprite data
	{ "272-c2.c2",    0x800000, 0xa3afda4f, 3 | BRF_GRA },           //  3
	{ "272-c3.c3",    0x800000, 0x8c3c7502, 3 | BRF_GRA },           //  4
	{ "272-c4.c4",    0x800000, 0x32d5e2e2, 3 | BRF_GRA },           //  5
	{ "272-c5.c5",    0x800000, 0x6ce085bc, 3 | BRF_GRA },           //  6
	{ "272-c6.c6",    0x800000, 0x05c8dc8e, 3 | BRF_GRA },           //  7
	{ "272-c7.c7",    0x800000, 0x1417b742, 3 | BRF_GRA },           //  8
	{ "272-c8.c8",    0x800000, 0xd49773cd, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "272-m1.m1",    0x080000, 0xadeebf40, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "272-v1.v1",    0x800000, 0x76a94127, 5 | BRF_SND },           // 11 Sound data
	{ "272-v2.v2",    0x800000, 0x4ba507f1, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(samsh5sph, samsh5sph, neogeo)
STD_ROM_FN(samsh5sph)

struct BurnDriver BurnDrvsamsh5sph = {
	"samsh5sph", "samsh5sp", "neogeo", NULL, "2004",
	"Samurai Shodown V Special / Samurai Spirits Zero Special (NGH-2720) (2nd release, less censored)\0", NULL, "Yuki Enterprise / SNK Playmore", "Neo Geo MVS",
	L"Samurai Shodown V Special\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4\u96F6 Special (NGH-2720) (2nd release, less censored)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsh5sphRomInfo, samsh5sphRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	samsh5spInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Samurai Shodown V Special / Samurai Spirits Zero Special (NGH-2720) (1st release, censored)
/* Encrypted Set */ /* AES VERSION, 1st release */

static struct BurnRomInfo samsh5sphoRomDesc[] = {
	/* Censored */
	{ "272-p1c.p1",   0x400000, 0x9291794d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "272-p2c.sp2",  0x400000, 0xfa1a7dd8, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "272-c1.c1",    0x800000, 0x4f97661a, 3 | BRF_GRA },           //  2 Sprite data
	{ "272-c2.c2",    0x800000, 0xa3afda4f, 3 | BRF_GRA },           //  3
	{ "272-c3.c3",    0x800000, 0x8c3c7502, 3 | BRF_GRA },           //  4
	{ "272-c4.c4",    0x800000, 0x32d5e2e2, 3 | BRF_GRA },           //  5
	{ "272-c5.c5",    0x800000, 0x6ce085bc, 3 | BRF_GRA },           //  6
	{ "272-c6.c6",    0x800000, 0x05c8dc8e, 3 | BRF_GRA },           //  7
	{ "272-c7.c7",    0x800000, 0x1417b742, 3 | BRF_GRA },           //  8
	{ "272-c8.c8",    0x800000, 0xd49773cd, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "272-m1.m1",    0x080000, 0xadeebf40, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "272-v1.v1",    0x800000, 0x76a94127, 5 | BRF_SND },           // 11 Sound data
	{ "272-v2.v2",    0x800000, 0x4ba507f1, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(samsh5spho, samsh5spho, neogeo)
STD_ROM_FN(samsh5spho)

struct BurnDriver BurnDrvsamsh5spho = {
	"samsh5spho", "samsh5sp", "neogeo", NULL, "2004",
	"Samurai Shodown V Special / Samurai Spirits Zero Special (NGH-2720) (1st release, censored)\0", NULL, "Yuki Enterprise / SNK Playmore", "Neo Geo MVS",
	L"Samurai Shodown V Special\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4\u96F6 Special (NGH-2720) (1st release, censored)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsh5sphoRomInfo, samsh5sphoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	samsh5spInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Magician Lord (NGM-005)
/* MVS VERSION */

static struct BurnRomInfo maglordRomDesc[] = {
	{ "005-pg1.p1",   0x080000, 0xbd0a492d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ MB834200

	{ "005-s1.s1",    0x020000, 0x1c5369a2, 2 | BRF_GRA },           //  1 Text layer tiles / MB831000

	{ "005-c1.c1",    0x080000, 0x806aee34, 3 | BRF_GRA },           //  2 Sprite data 		/ MB834200
	{ "005-c2.c2",    0x080000, 0x34aa9a86, 3 | BRF_GRA },           //  3 					/ MB834200
	{ "005-c3.c3",    0x080000, 0xc4c2b926, 3 | BRF_GRA },           //  4 					/ MB834200
	{ "005-c4.c4",    0x080000, 0x9c46dcf4, 3 | BRF_GRA },           //  5 					/ MB834200
	{ "005-c5.c5",    0x080000, 0x69086dec, 3 | BRF_GRA },           //  6 					/ MB834200
	{ "005-c6.c6",    0x080000, 0xab7ac142, 3 | BRF_GRA },           //  7 					/ MB834200

	{ "005-m1.m1",    0x040000, 0x26259f0f, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code 		/ MB832000

	{ "005-v11.v11",  0x080000, 0xcc0455fd, 5 | BRF_SND },           //  9 Sound data 		/ MB834000
	{ "005-v21.v21",  0x080000, 0xf94ab5b7, 6 | BRF_SND },           // 10 					/ MB834000
	{ "005-v22.v22",  0x080000, 0x232cfd04, 6 | BRF_SND },           // 11 					/ MB834000
};

STDROMPICKEXT(maglord, maglord, neogeo)
STD_ROM_FN(maglord)

struct BurnDriver BurnDrvmaglord = {
	"maglord", NULL, "neogeo", NULL, "1990",
	"Magician Lord (NGM-005)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, maglordRomInfo, maglordRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Magician Lord (NGH-005)
/* AES VERSION */

static struct BurnRomInfo maglordhRomDesc[] = {
	{ "005-p1.p1",    0x080000, 0x599043c5, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ MB834200

	{ "005-s1.s1",    0x020000, 0x1c5369a2, 2 | BRF_GRA },           //  1 Text layer tiles / MB831000

	{ "005-c1.c1",    0x080000, 0x806aee34, 3 | BRF_GRA },           //  2 Sprite data 		/ MB834200
	{ "005-c2.c2",    0x080000, 0x34aa9a86, 3 | BRF_GRA },           //  3 					/ MB834200
	{ "005-c3.c3",    0x080000, 0xc4c2b926, 3 | BRF_GRA },           //  4 					/ MB834200
	{ "005-c4.c4",    0x080000, 0x9c46dcf4, 3 | BRF_GRA },           //  5 					/ MB834200
	{ "005-c5.c5",    0x080000, 0x69086dec, 3 | BRF_GRA },           //  6 					/ MB834200
	{ "005-c6.c6",    0x080000, 0xab7ac142, 3 | BRF_GRA },           //  7 					/ MB834200

	{ "005-m1.m1",    0x040000, 0x26259f0f, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code 		/ MB832000

	{ "005-v11.v11",  0x080000, 0xcc0455fd, 5 | BRF_SND },           //  9 Sound data 		/ MB834000
	{ "005-v21.v21",  0x080000, 0xf94ab5b7, 6 | BRF_SND },           // 10 					/ MB834000
	{ "005-v22.v22",  0x080000, 0x232cfd04, 6 | BRF_SND },           // 11 					/ MB834000
};

STDROMPICKEXT(maglordh, maglordh, neogeo)
STD_ROM_FN(maglordh)

struct BurnDriver BurnDrvmaglordh = {
	"maglordh", "maglord", "neogeo", NULL, "1990",
	"Magician Lord (NGH-005)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, maglordhRomInfo, maglordhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Ninja Combat (NGM-009)
/* MVS VERSION */

static struct BurnRomInfo ncombatRomDesc[] = {
	{ "009-p1.p1",    0x080000, 0xb45fcfbf, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "009-s1.s1",    0x020000, 0xd49afee8, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "009-c1.c1",    0x080000, 0x33cc838e, 3 | BRF_GRA },           //  2 Sprite data 		/ TC534200
	{ "009-c2.c2",    0x080000, 0x26877feb, 3 | BRF_GRA },           //  3 					/ TC534200
	{ "009-c3.c3",    0x080000, 0x3b60a05d, 3 | BRF_GRA },           //  4 					/ TC534200
	{ "009-c4.c4",    0x080000, 0x39c2d039, 3 | BRF_GRA },           //  5 					/ TC534200
	{ "009-c5.c5",    0x080000, 0x67a4344e, 3 | BRF_GRA },           //  6 					/ TC534200
	{ "009-c6.c6",    0x080000, 0x2eca8b19, 3 | BRF_GRA },           //  7 					/ TC534200

	{ "009-m1.m1",    0x020000, 0xb5819863, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code 		/ TC531001

	{ "009-v11.v11",  0x080000, 0xcf32a59c, 5 | BRF_SND },           //  9 Sound data 		/ TC534000
	{ "009-v12.v12",  0x080000, 0x7b3588b7, 5 | BRF_SND },           // 10 					/ TC534000
	{ "009-v13.v13",  0x080000, 0x505a01b5, 5 | BRF_SND },           // 11 					/ TC534000
	{ "009-v21.v21",  0x080000, 0x365f9011, 6 | BRF_SND },           // 12 					/ TC534000
};

STDROMPICKEXT(ncombat, ncombat, neogeo)
STD_ROM_FN(ncombat)

struct BurnDriver BurnDrvncombat = {
	"ncombat", NULL, "neogeo", NULL, "1990",
	"Ninja Combat (NGM-009)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, ncombatRomInfo, ncombatRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Ninja Combat (NGH-009)
/* MVS AND AES VERSION */

static struct BurnRomInfo ncombathRomDesc[] = {
	{ "009-pg1.p1",   0x080000, 0x8e9f0add, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "009-s1.s1",    0x020000, 0xd49afee8, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "009-c1.c1",    0x080000, 0x33cc838e, 3 | BRF_GRA },           //  2 Sprite data 		/ TC534200
	{ "009-c2.c2",    0x080000, 0x26877feb, 3 | BRF_GRA },           //  3 					/ TC534200
	{ "009-c3.c3",    0x080000, 0x3b60a05d, 3 | BRF_GRA },           //  4 					/ TC534200
	{ "009-c4.c4",    0x080000, 0x39c2d039, 3 | BRF_GRA },           //  5 					/ TC534200
	{ "009-c5.c5",    0x080000, 0x67a4344e, 3 | BRF_GRA },           //  6 					/ TC534200
	{ "009-c6.c6",    0x080000, 0x2eca8b19, 3 | BRF_GRA },           //  7 					/ TC534200

	{ "009-m1.m1",    0x020000, 0xb5819863, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code 		/ TC531001

	{ "009-v11.v11",  0x080000, 0xcf32a59c, 5 | BRF_SND },           //  9 Sound data 		/ TC534000
	{ "009-v12.v12",  0x080000, 0x7b3588b7, 5 | BRF_SND },           // 10 					/ TC534000
	{ "009-v13.v13",  0x080000, 0x505a01b5, 5 | BRF_SND },           // 11 					/ TC534000
	{ "009-v21.v21",  0x080000, 0x365f9011, 6 | BRF_SND },           // 12 					/ TC534000
};

STDROMPICKEXT(ncombath, ncombath, neogeo)
STD_ROM_FN(ncombath)

struct BurnDriver BurnDrvncombath = {
	"ncombath", "ncombat", "neogeo", NULL, "1990",
	"Ninja Combat (NGH-009)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, ncombathRomInfo, ncombathRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Blue's Journey / Raguy (ALM-001)(ALH-001)
/* MVS AND AES VERSION */

static struct BurnRomInfo bjourneyRomDesc[] = {
	{ "022-p1.p1",    0x100000, 0x6a2f6d4a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200

	{ "022-s1.s1",    0x020000, 0x843c3624, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "022-c1.c1",    0x100000, 0x4d47a48c, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "022-c2.c2",    0x100000, 0xe8c1491a, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "022-c3.c3",    0x080000, 0x66e69753, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "022-c4.c4",    0x080000, 0x71bfd48a, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "022-m1.m1",    0x020000, 0x8e1d4ab6, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "022-v11.v11",  0x100000, 0x2cb4ad91, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "022-v22.v22",  0x100000, 0x65a54d13, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(bjourney, bjourney, neogeo)
STD_ROM_FN(bjourney)

struct BurnDriver BurnDrvbjourney = {
	"bjourney", NULL, "neogeo", NULL, "1990",
	"Blue's Journey / Raguy (ALM-001)(ALH-001)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, bjourneyRomInfo, bjourneyRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette, 0x1000,
	320, 224, 4, 3
};

// Blue's Journey / Raguy (ALH-001)
/* AES VERSION */

static struct BurnRomInfo bjourneyhRomDesc[] = {
	{ "022-hp1.p1",   0x100000, 0x62cbe7b2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200

	{ "022-s1.s1",    0x020000, 0x843c3624, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "022-c1.c1",    0x100000, 0x4d47a48c, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "022-c2.c2",    0x100000, 0xe8c1491a, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "022-c3.c3",    0x080000, 0x66e69753, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "022-c4.c4",    0x080000, 0x71bfd48a, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "022-m1.m1",    0x020000, 0x8e1d4ab6, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "022-v11.v11",  0x100000, 0x2cb4ad91, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
	{ "022-v22.v22",  0x100000, 0x65a54d13, 5 | BRF_SND },           //  8 					/ TC538200
};

STDROMPICKEXT(bjourneyh, bjourneyh, neogeo)
STD_ROM_FN(bjourneyh)

struct BurnDriver BurnDrvbjourneyh = {
	"bjourneyh", "bjourney", "neogeo", NULL, "1990",
	"Blue's Journey / Raguy (ALH-001)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, bjourneyhRomInfo, bjourneyhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette, 0x1000,
	320, 224, 4, 3
};

// Crossed Swords (ALM-002)(ALH-002)
/* MVS AND AES VERSION */

static struct BurnRomInfo crswordRomDesc[] = {
	{ "037-p1.p1",    0x080000, 0xe7f2553c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200

	{ "037-s1.s1",    0x020000, 0x74651f27, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "037-c1.c1",    0x100000, 0x09df6892, 3 | BRF_GRA },           //  2 Sprite data 		/ TC538200
	{ "037-c2.c2",    0x100000, 0xac122a78, 3 | BRF_GRA },           //  3 					/ TC538200
	{ "037-c3.c3",    0x100000, 0x9d7ed1ca, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "037-c4.c4",    0x100000, 0x4a24395d, 3 | BRF_GRA },           //  5 					/ TC538200

	{ "037-m1.m1",    0x020000, 0x9504b2c6, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "037-v1.v1",    0x100000, 0x61fedf65, 5 | BRF_SND },           //  7 Sound data 		/ TC538200
};

STDROMPICKEXT(crsword, crsword, neogeo)
STD_ROM_FN(crsword)

struct BurnDriver BurnDrvcrsword = {
	"crsword", NULL, "neogeo", NULL, "1991",
	"Crossed Swords (ALM-002)(ALH-002)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, crswordRomInfo, crswordRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Thrash Rally (ALM-003)(ALH-003)
/* MVS AND AES VERSION */

static struct BurnRomInfo trallyRomDesc[] = {
	{ "038-p1.p1",    0x080000, 0x1e52a576, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "038-p2.p2",    0x080000, 0xa5193e2f, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC534200

	{ "038-s1.s1",    0x020000, 0xfff62ae3, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "038-c1.c1",    0x100000, 0xc58323d4, 3 | BRF_GRA },           //  3 Sprite data 		/ TC538200
	{ "038-c2.c2",    0x100000, 0xbba9c29e, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "038-c3.c3",    0x080000, 0x3bb7b9d6, 3 | BRF_GRA },           //  5 					/ TC534200
	{ "038-c4.c4",    0x080000, 0xa4513ecf, 3 | BRF_GRA },           //  6 					/ TC534200

	{ "038-m1.m1",    0x020000, 0x0908707e, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "038-v1.v1",    0x100000, 0x5ccd9fd5, 5 | BRF_SND },           //  8 Sound data 		/ TC538200
	{ "038-v2.v2",    0x080000, 0xddd8d1e6, 5 | BRF_SND },           //  9 					/ TC534200

	{ "hd6301v1p.hd6301v1", 0x001000, 0x00000000, BRF_OPT | BRF_NODUMP }, // Hitachi HD6301V1 MCU
};

STDROMPICKEXT(trally, trally, neogeo)
STD_ROM_FN(trally)

struct BurnDriver BurnDrvtrally = {
	"trally", NULL, "neogeo", NULL, "1991",
	"Thrash Rally (ALM-003)(ALH-003)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RACING, 0,
	NULL, trallyRomInfo, trallyRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Ninja Commando

static struct BurnRomInfo ncommandRomDesc[] = {
	{ "050-p1.p1",    0x100000, 0x4e097c40, 1 | BRF_ESS | BRF_PRG }, //  0 68K code / CXK388000

	{ "050-s1.s1",    0x020000, 0xdb8f9c8e, 2 | BRF_GRA },           //  1 Text layer tiles / CXK 381000

	{ "050-c1.c1",    0x100000, 0x87421a0a, 3 | BRF_GRA },           //  2 Sprite data / CXK388000
	{ "050-c2.c2",    0x100000, 0xc4cf5548, 3 | BRF_GRA },           //  3 / CXK388000
	{ "050-c3.c3",    0x100000, 0x03422c1e, 3 | BRF_GRA },           //  4 / CXK388000
	{ "050-c4.c4",    0x100000, 0x0845eadb, 3 | BRF_GRA },           //  5 / CXK388000

	{ "050-m1.m1",    0x020000, 0x6fcf07d3, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code / CXK381003

	{ "050-v1.v1",    0x100000, 0x23c3ab42, 5 | BRF_SND },           //  7 Sound data / CXK388000
	{ "050-v2.v2",    0x080000, 0x80b8a984, 5 | BRF_SND },           //  8 / CXK384000
};

STDROMPICKEXT(ncommand, ncommand, neogeo)
STD_ROM_FN(ncommand)

struct BurnDriver BurnDrvncommand = {
	"ncommand", NULL, "neogeo", NULL, "1992",
	"Ninja Commando\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VERSHOOT, 0,
	NULL, ncommandRomInfo, ncommandRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// World Heroes (ALM-005)
/* MVS VERSION */

static struct BurnRomInfo wh1RomDesc[] = {
	{ "053-epr.p1",   0x080000, 0xd42e1e9a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code / D27C400
	{ "053-epr.p2",   0x080000, 0x0e33e8a3, 1 | BRF_ESS | BRF_PRG }, //  1 			/ D27C400
	/* P's on eprom, correct chip label unknown */
	/* also found MVS set with p1 / p2 on TC534200 on PROG board NEO-MVS PROG16; chip labels are 053-P1 and 053-P2
	and M1 on TC541000 on CHA board NEO-MVS CHA42G-1; 53 is hand written on the chip */

	{ "053-s1.s1",    0x020000, 0x8c2c2d6b, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "053-c1.c1",    0x200000, 0x85eb5bce, 3 | BRF_GRA },           //  3 Sprite data
	{ "053-c2.c2",    0x200000, 0xec93b048, 3 | BRF_GRA },           //  4
	{ "053-c3.c3",    0x100000, 0x0dd64965, 3 | BRF_GRA },           //  5
	{ "053-c4.c4",    0x100000, 0x9270d954, 3 | BRF_GRA },           //  6

	{ "053-m1.m1",    0x020000, 0x1bd9d04b, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code / TC54H1000

	{ "053-v2.v2",    0x200000, 0xa68df485, 5 | BRF_SND },           //  8 Sound data
	{ "053-v4.v4",    0x100000, 0x7bea8f66, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(wh1, wh1, neogeo)
STD_ROM_FN(wh1)

struct BurnDriver BurnDrvwh1 = {
	"wh1", NULL, "neogeo", NULL, "1992",
	"World Heroes (ALM-005)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_VSFIGHT, 0,
	NULL, wh1RomInfo, wh1RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};


// World Heroes (ALH-005)
/* AES VERSION */

static struct BurnRomInfo wh1hRomDesc[] = {
	/* also found AES set with p1 / p2 on eprom; correct chip label unknown
	and v1 / v2 on PROG board NEO-AEG PROG-G2; chip labels are 053-V2 and  053-V4
	also M1 on TC541000 on CHA board NEO-AEG CHA42G-2C; 53 is hand written on the chip */

	{ "053-p1.p1",    0x080000, 0x95b574cb, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC534200
	{ "053-p2.p2",    0x080000, 0xf198ed45, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC534200

	{ "053-s1.s1",    0x020000, 0x8c2c2d6b, 2 | BRF_GRA },           //  2 Text layer tiles	/ TC531000

	{ "053-c1.c1",    0x200000, 0x85eb5bce, 3 | BRF_GRA },           //  3 Sprite data		/ TC5316200
	{ "053-c2.c2",    0x200000, 0xec93b048, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "053-c3.c3",    0x100000, 0x0dd64965, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "053-c4.c4",    0x100000, 0x9270d954, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "053-m1.m1",    0x020000, 0x1bd9d04b, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code			/ TC531001

	{ "053-v2.v2",    0x200000, 0xa68df485, 5 | BRF_SND },           //  8 Sound data		/ TC5316200
	{ "053-v4.v4",    0x100000, 0x7bea8f66, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(wh1h, wh1h, neogeo)
STD_ROM_FN(wh1h)

struct BurnDriver BurnDrvwh1h = {
	"wh1h", "wh1", "neogeo", NULL, "1992",
	"World Heroes (ALH-005)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_VSFIGHT, 0,
	NULL, wh1hRomInfo, wh1hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};


// World Heroes (set 3)

static struct BurnRomInfo wh1haRomDesc[] = {
	{ "053-p1_a.p1",  0x080000, 0xed29fde2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "053-p2_a.p2",  0x080000, 0x98f2b158, 1 | BRF_ESS | BRF_PRG }, //  1
	/* Correct chip labels for p1 and p2 unknown */

	{ "053-s1.s1",    0x020000, 0x8c2c2d6b, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "053-c1.c1",    0x200000, 0x85eb5bce, 3 | BRF_GRA },           //  3 Sprite data		/ TC5316200
	{ "053-c2.c2",    0x200000, 0xec93b048, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "053-c3.c3",    0x100000, 0x0dd64965, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "053-c4.c4",    0x100000, 0x9270d954, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "053-m1.m1",    0x020000, 0x1bd9d04b, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code			/ TC531001

	{ "053-v2.v2",    0x200000, 0xa68df485, 5 | BRF_SND },           //  8 Sound data		/ TC5316200
	{ "053-v4.v4",    0x100000, 0x7bea8f66, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(wh1ha, wh1ha, neogeo)
STD_ROM_FN(wh1ha)

struct BurnDriver BurnDrvwh1ha = {
	"wh1ha", "wh1", "neogeo", NULL, "1992",
	"World Heroes (set 3)\0", NULL, "Alpha Denshi Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_VSFIGHT, 0,
	NULL, wh1haRomInfo, wh1haRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// World Heroes 2 (ALM-006)(ALH-006)
/* MVS AND AES VERSION */

static struct BurnRomInfo wh2RomDesc[] = {
	{ "057-p1.p1",    0x200000, 0x65a891d9, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC5316200

	{ "057-s1.s1",    0x020000, 0xfcaeb3a4, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "057-c1.c1",    0x200000, 0x21c6bb91, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "057-c2.c2",    0x200000, 0xa3999925, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "057-c3.c3",    0x200000, 0xb725a219, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "057-c4.c4",    0x200000, 0x8d96425e, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "057-c5.c5",    0x200000, 0xb20354af, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "057-c6.c6",    0x200000, 0xb13d1de3, 3 | BRF_GRA },           //  7 					/ TC5316200

	{ "057-m1.m1",    0x020000, 0x8fa3bc77, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code			/ TC531001

	{ "057-v1.v1",    0x200000, 0x8877e301, 5 | BRF_SND },           //  9 Sound data		/ TC5316200
	{ "057-v2.v2",    0x200000, 0xc1317ff4, 5 | BRF_SND },           // 10 					/ TC5316200
};

STDROMPICKEXT(wh2, wh2, neogeo)
STD_ROM_FN(wh2)

struct BurnDriver BurnDrvwh2 = {
	"wh2", NULL, "neogeo", NULL, "1993",
	"World Heroes 2 (ALM-006)(ALH-006)\0", NULL, "ADK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, wh2RomInfo, wh2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// World Heroes 2 (ALM-006)(ALH-006)
/* AES VERSION */

static struct BurnRomInfo wh2hRomDesc[] = {
	{ "057-pg1.p1",   0x200000, 0xcde9aff5, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC5316200

	{ "057-s1.s1",    0x020000, 0xfcaeb3a4, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "057-c1.c1",    0x200000, 0x21c6bb91, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "057-c2.c2",    0x200000, 0xa3999925, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "057-c3.c3",    0x200000, 0xb725a219, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "057-c4.c4",    0x200000, 0x8d96425e, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "057-c5.c5",    0x200000, 0xb20354af, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "057-c6.c6",    0x200000, 0xb13d1de3, 3 | BRF_GRA },           //  7 					/ TC5316200

	{ "057-m1.m1",    0x020000, 0x8fa3bc77, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code			/ TC531001

	{ "057-v1.v1",    0x200000, 0x8877e301, 5 | BRF_SND },           //  9 Sound data		/ TC5316200
	{ "057-v2.v2",    0x200000, 0xc1317ff4, 5 | BRF_SND },           // 10 					/ TC5316200
};

STDROMPICKEXT(wh2h, wh2h, neogeo)
STD_ROM_FN(wh2h)

struct BurnDriver BurnDrvwh2h = {
	"wh2h", "wh2", "neogeo", NULL, "1993",
	"World Heroes 2 (ALH-006)\0", NULL, "ADK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, wh2hRomInfo, wh2hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// World Heroes 2 Jet (ADM-007)(ADH-007)
/* MVS AND AES VERSION */

static struct BurnRomInfo wh2jRomDesc[] = {
	/* also found AES set with PROG board NEO-AEG PROGRKB and CHA board NEO-AEG CHA256[B];
	   same layouts and chip labels (on maskroms) */
	{ "064-p1.p1",    0x200000, 0x385a2e86, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "064-s1.s1",    0x020000, 0x2a03998a, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	/* Different layout with 4xC (32mbit) on mask rom also exists on board NEO-MVS CHA256;
	/ chip labels are 064-C13, 064-C24, 064-C57 and 064-C68 */
	{ "064-c1.c1",    0x200000, 0x2ec87cea, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "064-c2.c2",    0x200000, 0x526b81ab, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "064-c3.c3",    0x200000, 0x436d1b31, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "064-c4.c4",    0x200000, 0xf9c8dd26, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "064-c5.c5",    0x200000, 0x8e34a9f4, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "064-c6.c6",    0x200000, 0xa43e4766, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "064-c7.c7",    0x200000, 0x59d97215, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "064-c8.c8",    0x200000, 0xfc092367, 3 | BRF_GRA },           //  9 					/ TC5316200

	{ "064-m1.m1",    0x020000, 0xd2eec9d3, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "064-v1.v1",    0x200000, 0xaa277109, 5 | BRF_SND },           //  7 Sound data		/ TC5316200
	{ "064-v2.v2",    0x200000, 0xb6527edd, 5 | BRF_SND },           //  8 					/ TC5316200
};

STDROMPICKEXT(wh2j, wh2j, neogeo)
STD_ROM_FN(wh2j)

struct BurnDriver BurnDrvwh2j = {
	"wh2j", NULL, "neogeo", NULL, "1994",
	"World Heroes 2 Jet (ADM-007)(ADH-007)\0", NULL, "ADK / SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, wh2jRomInfo, wh2jRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Aggressors of Dark Kombat / Tsuukai GANGAN Koushinkyoku (ADM-008)(ADH-008)
/* MVS AND AES VERSION */

static struct BurnRomInfo aodkRomDesc[] = {
	{ "074-p1.p1",    0x200000, 0x62369553, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "074-s1.s1",    0x020000, 0x96148d2b, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "074-c1.c1",    0x200000, 0xa0b39344, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "074-c2.c2",    0x200000, 0x203f6074, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "074-c3.c3",    0x200000, 0x7fff4d41, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "074-c4.c4",    0x200000, 0x48db3e0a, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "074-c5.c5",    0x200000, 0xc74c5e51, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "074-c6.c6",    0x200000, 0x73e8e7e0, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "074-c7.c7",    0x200000, 0xac7daa01, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "074-c8.c8",    0x200000, 0x14e7ad71, 3 | BRF_GRA },           //  9 					/ TC5316200

	{ "074-m1.m1",    0x020000, 0x5a52a9d1, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "074-v1.v1",    0x200000, 0x7675b8fa, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "074-v2.v2",    0x200000, 0xa9da86e9, 5 | BRF_SND },           // 12 					/ TC5316200
};

STDROMPICKEXT(aodk, aodk, neogeo)
STD_ROM_FN(aodk)

struct BurnDriver BurnDrvaodk = {
	"aodk", NULL, "neogeo", NULL, "1994",
	"Aggressors of Dark Kombat / Tsuukai GANGAN Koushinkyoku (ADM-008)(ADH-008)\0", NULL, "ADK / SNK", "Neo Geo MVS",
	L"Aggressors of Dark Kombat\0\u75DB\u5FEB\uFF27\uFF41\uFF4E\uFF47\uFF41\uFF4E\u884C\u9032\u66F2 (ADM-008)(ADH-008)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, aodkRomInfo, aodkRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// World Heroes Perfect

static struct BurnRomInfo whpRomDesc[] = {
	/* It also exists MVS sets with PROG board NEO-MVS PROGGSC and CHA board NEO-MVS CHA256,
	PROG board NEO-MVS PROGGSC and CHA board NEO-MVS CHA256B, PROG board NEO-MVS PROGBK1
	and NEO-MVS CHA256B; same chip labels */
	{ "090-p1.p1",    0x200000, 0xafaa4702, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC5316200

	{ "090-s1.s1",    0x020000, 0x174a880f, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "090-c1.c1",    0x400000, 0xcd30ed9b, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5332205
	{ "090-c2.c2",    0x400000, 0x10eed5ee, 3 | BRF_GRA },           //  3 					/ mask rom TC5332205
	{ "064-c3.c3",    0x200000, 0x436d1b31, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "064-c4.c4",    0x200000, 0xf9c8dd26, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200
	{ "064-c5.c5",    0x200000, 0x8e34a9f4, 3 | BRF_GRA },           //  6 					/ mask rom TC5316200
	{ "064-c6.c6",    0x200000, 0xa43e4766, 3 | BRF_GRA },           //  7 					/ mask rom TC5316200
	{ "064-c7.c7",    0x200000, 0x59d97215, 3 | BRF_GRA },           //  8 					/ mask rom TC5316200
	{ "064-c8.c8",    0x200000, 0xfc092367, 3 | BRF_GRA },           //  9 					/ mask rom TC5316200

	{ "090-m1.m1",    0x020000, 0x28065668, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ mask rom TC531001

	{ "090-v1.v1",    0x200000, 0x30cf2709, 5 | BRF_SND },           // 11 Sound data		/ mask rom TC5316200
	{ "064-v2.v2",    0x200000, 0xb6527edd, 5 | BRF_SND },           // 12 					/ mask rom TC5316200
	{ "090-v3.v3",    0x200000, 0x1908a7ce, 5 | BRF_SND },           // 13 					/ mask rom TC5316200
};

STDROMPICKEXT(whp, whp, neogeo)
STD_ROM_FN(whp)

struct BurnDriver BurnDrvwhp = {
	"whp", NULL, "neogeo", NULL, "1995",
	"World Heroes Perfect\0", NULL, "ADK / SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, whpRomInfo, whpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Syougi No Tatsujin - Master of Syougi

static struct BurnRomInfo moshougiRomDesc[] = {
	{ "203-p1.p1",    0x100000, 0x7ba70e2d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200

	{ "203-s1.s1",    0x020000, 0xbfdc8309, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "203-c1.c1",    0x200000, 0xbba9e8c0, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "203-c2.c2",    0x200000, 0x2574be03, 3 | BRF_GRA },           //  3 					/ TC5316200

	{ "203-m1.m1",    0x020000, 0xa602c2c2, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code			/ TC531001

	{ "203-v1.v1",    0x200000, 0xbaa2b9a5, 5 | BRF_SND },           //  5 Sound data		/ TC5316200
};

STDROMPICKEXT(moshougi, moshougi, neogeo)
STD_ROM_FN(moshougi)

struct BurnDriver BurnDrvmoshougi = {
	"moshougi", NULL, "neogeo", NULL, "1995",
	"Syougi No Tatsujin - Master of Shougi\0", NULL, "ADK / SNK", "Neo Geo MVS",
	L"\u5C06\u68CB\u306E\u9054\u4EBA\0Master of Shougi\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, moshougiRomInfo, moshougiRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Over Top

static struct BurnRomInfo overtopRomDesc[] = {
	{ "212-p1.p1",    0x200000, 0x16c063a9, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC5316200

	{ "212-s1.s1",    0x020000, 0x481d3ddc, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "212-c1.c1",    0x400000, 0x50f43087, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5332205
	{ "212-c2.c2",    0x400000, 0xa5b39807, 3 | BRF_GRA },           //  3 					/ mask rom TC5332205
	{ "212-c3.c3",    0x400000, 0x9252ea02, 3 | BRF_GRA },           //  4 					/ mask rom TC5332205
	{ "212-c4.c4",    0x400000, 0x5f41a699, 3 | BRF_GRA },           //  5 					/ mask rom TC5332205
	{ "212-c5.c5",    0x200000, 0xfc858bef, 3 | BRF_GRA },           //  6 					/ mask rom TC5316200
	{ "212-c6.c6",    0x200000, 0x0589c15e, 3 | BRF_GRA },           //  7 					/ mask rom TC5316200

	{ "212-m1.m1",    0x020000, 0xfcab6191, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code			/ mask rom TC531001

	{ "212-v1.v1",    0x400000, 0x013d4ef9, 5 | BRF_SND },           //  9 Sound data		/ mask rom TC5332204
};

STDROMPICKEXT(overtop, overtop, neogeo)
STD_ROM_FN(overtop)

struct BurnDriver BurnDrvovertop = {
	"overtop", NULL, "neogeo", NULL, "1996",
	"Over Top\0", NULL, "ADK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_RACING, 0,
	NULL, overtopRomInfo, overtopRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Ninja Master's - haoh-ninpo-cho
/* MVS AND AES VERSION? */

static struct BurnRomInfo ninjamasRomDesc[] = {
	/* also found AES set with PROG board NEO-AEG PROGBK1Y and CHA board NEO-AEG CHA256RY
	   same layouts and chip labels (on mask roms) */
	{ "217-p1.p1",    0x100000, 0x3e97ed69, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200
	{ "217-p2.sp2",   0x200000, 0x191fca88, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "217-s1.s1",    0x020000, 0x8ff782f0, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "217-c1.c1",    0x400000, 0x5fe97bc4, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5332205
	{ "217-c2.c2",    0x400000, 0x886e0d66, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "217-c3.c3",    0x400000, 0x59e8525f, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "217-c4.c4",    0x400000, 0x8521add2, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "217-c5.c5",    0x400000, 0xfb1896e5, 3 | BRF_GRA },           //  7 					/ TC5332205
	{ "217-c6.c6",    0x400000, 0x1c98c54b, 3 | BRF_GRA },           //  8 					/ TC5332205
	{ "217-c7.c7",    0x400000, 0x8b0ede2e, 3 | BRF_GRA },           //  9 					/ TC5332205
	{ "217-c8.c8",    0x400000, 0xa085bb61, 3 | BRF_GRA },           // 10 					/ TC5332205

	{ "217-m1.m1",    0x020000, 0xd00fb2af, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "217-v1.v1",    0x400000, 0x1c34e013, 5 | BRF_SND },           // 12 Sound data 		/ TC5332204
	{ "217-v2.v2",    0x200000, 0x22f1c681, 5 | BRF_SND },           // 13 					/ TC5316200
};

STDROMPICKEXT(ninjamas, ninjamas, neogeo)
STD_ROM_FN(ninjamas)

struct BurnDriver BurnDrvninjamas = {
	"ninjamas", NULL, "neogeo", NULL, "1996",
	"Ninja Master's - haoh-ninpo-cho\0", NULL, "ADK / SNK", "Neo Geo MVS",
	L"Ninja master's \u8987\u738B\u5FCD\u6CD5\u5E16\0Ninja Master's haoh ninpo cho\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, ninjamasRomInfo, ninjamasRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Twinkle Star Sprites

static struct BurnRomInfo twinspriRomDesc[] = {
	{ "224-p1.p1",    0x200000, 0x7697e445, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "224-s1.s1",    0x020000, 0xeeed5758, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "224-c1.c1",    0x400000, 0xf7da64ab, 3 | BRF_GRA },           //  2 Sprite data
	{ "224-c2.c2",    0x400000, 0x4c09bbfb, 3 | BRF_GRA },           //  3
	{ "224-c3.c3",    0x100000, 0xc59e4129, 3 | BRF_GRA },           //  4
	{ "224-c4.c4",    0x100000, 0xb5532e53, 3 | BRF_GRA },           //  5

	{ "224-m1.m1",    0x020000, 0x364d6f96, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "224-v1.v1",    0x400000, 0xff57f088, 5 | BRF_SND },           //  7 Sound data
	{ "224-v2.v2",    0x200000, 0x7ad26599, 5 | BRF_SND },           //  8
};

STDROMPICKEXT(twinspri, twinspri, neogeo)
STD_ROM_FN(twinspri)

struct BurnDriver BurnDrvtwinspri = {
	"twinspri", NULL, "neogeo", NULL, "1996",
	"Twinkle Star Sprites\0", NULL, "ADK", "Neo Geo MVS",
	L"TwinkleStar Sprites\0\u30C6\u30A3\u30F3\u30AF\u30EB\u30B9\u30BF\u30FC\u30B9\u30D7\u30E9\u30A4\u30C4\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VERSHOOT, 0,
	NULL, twinspriRomInfo, twinspriRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Zintrick / Oshidashi Zentrix (hack / bootleg)

static struct BurnRomInfo zintrckbRomDesc[] = {
	{ "zin-p1.bin",   0x100000, 0x06c8fca7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "zin-s1.bin",   0x020000, 0xa7ab0e81, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "zin-c1.bin",   0x200000, 0x76aee189, 3 | BRF_GRA },           //  2 Sprite data
	{ "zin-c2.bin",   0x200000, 0x844ed4b3, 3 | BRF_GRA },           //  3

	{ "zin-m1.bin",   0x020000, 0xfd9627ca, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "zin-v1.bin",   0x200000, 0xc09f74f1, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(zintrckb, zintrckb, neogeo)
STD_ROM_FN(zintrckb)

struct BurnDriver BurnDrvzintrckb = {
	"zintrckb", NULL, "neogeo", NULL, "1996",
	"Zintrick / Oshidashi Zentrix (hack / bootleg)\0", NULL, "hack / bootleg", "Neo Geo MVS",
	L"Zintrick\0\u62BC\u3057\u51FA\u3057\u30B8\u30F3\u30C8\u30EA\u30C3\u30AF (hack / bootleg)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, zintrckbRomInfo, zintrckbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Viewpoint

static struct BurnRomInfo viewpoinRomDesc[] = {
	{ "051-p1.p1",    0x100000, 0x17aa899d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ CXK388002

	{ "051-s1.s1",    0x020000, 0x9fea5758, 2 | BRF_GRA },           //  1 Text layer tiles / CXK381000

	{ "051-c1.c1",    0x200000, 0xd624c132, 3 | BRF_GRA },           //  2 Sprite data 		/ CXK381600
	{ "051-c2.c2",    0x200000, 0x40d69f1e, 3 | BRF_GRA },           //  3 					/ CXK381600

	{ "051-m1.m1",    0x020000, 0x8e69f29a, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code 		/ CXK381003

	/* What board was originally used (labels 051-V2 and 051-V4)? MVS is twice confirmed on NEO-MVS PROG-G2 */
	{ "051-v2.v1",    0x200000, 0x019978b6, 5 | BRF_SND },           //  5 Sound data 		/ CXK381600
	{ "051-v4.v2",    0x200000, 0x5758f38c, 5 | BRF_SND },           //  6 					/ CXK381600
};

STDROMPICKEXT(viewpoin, viewpoin, neogeo)
STD_ROM_FN(viewpoin)

struct BurnDriver BurnDrvviewpoin = {
	"viewpoin", NULL, "neogeo", NULL, "1992",
	"Viewpoint\0", NULL, "Sammy / Aicom", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_SHOOT, 0,
	NULL, viewpoinRomInfo, viewpoinRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Viewpoint (prototype)
/* crashes during stage 4 on real hardware (but stage 5 can be loaded with memcard) */
/* AES AND MVS VERSION */

static struct BurnRomInfo viewpoinpRomDesc[] = {
	{ "p1",    			0x080000, 0x0adb5eee, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "p2",    			0x080000, 0x31129194, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "s1",    			0x020000, 0x9fea5758, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "c1",   			0x080000, 0x97a3183b, 3 | BRF_GRA },           //  3 Sprite data
	{ "c2",   			0x080000, 0xf637667a, 3 | BRF_GRA },           //  4
	{ "c3",   			0x080000, 0xb017d2d1, 3 | BRF_GRA },           //  5
	{ "c4",   			0x080000, 0x10f1624a, 3 | BRF_GRA },           //  6
	{ "c5",   			0x080000, 0x714a8bc4, 3 | BRF_GRA },           //  7
	{ "c6",   			0x080000, 0x5bc32495, 3 | BRF_GRA },           //  8
	{ "c7",   			0x080000, 0x28a0a127, 3 | BRF_GRA },           //  9
	{ "c8",   			0x080000, 0x439c9036, 3 | BRF_GRA },           //  10

	{ "m1",    			0x020000, 0x8e69f29a, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	{ "v1",  			0x100000, 0x7e9af736, 5 | BRF_SND },           //  12 Sound data
	{ "v2",  			0x100000, 0x503a9c51, 5 | BRF_SND },           //  13
	{ "v3",  			0x100000, 0xdfa8d079, 5 | BRF_SND },           //  14
	{ "v4",  			0x100000, 0x612b9292, 5 | BRF_SND },           //  15
};

STDROMPICKEXT(viewpoinp, viewpoinp, neogeo)
STD_ROM_FN(viewpoinp)

static void ViewpoinpCallback()
{
	BurnLoadRom(Neo68KROMActive + 0x000000, 0, 2);
	BurnLoadRom(Neo68KROMActive + 0x000001, 1, 2);
}

static INT32 ViewpoinpInit()
{
	NeoCallbackActive->pInitialise = ViewpoinpCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvViewpoinp = {
	"viewpoinp", "viewpoin", "neogeo", NULL, "1992",
	"Viewpoint (prototype)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SPRITE32, GBF_SHOOT, 0,
	NULL, viewpoinpRomInfo, viewpoinpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	ViewpoinpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Jyanshin Densetsu - Quest of Jongmaster
/* MVS ONLY RELEASE */

static struct BurnRomInfo janshinRomDesc[] = {
	{ "048-p1.p1",    0x100000, 0xfa818cbb, 1 | BRF_ESS | BRF_PRG }, //  0 68K code / mask rom TC538200

	{ "048-s1.s1",    0x020000, 0x8285b25a, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "048-c1.c1",    0x200000, 0x3fa890e9, 3 | BRF_GRA },           //  2 Sprite data / mask rom TC5316200
	{ "048-c2.c2",    0x200000, 0x59c48ad8, 3 | BRF_GRA },           //  3 / mask rom TC5316200

	{ "048-m1.m1",    0x020000, 0x310467c7, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code / mask rom TC531001

	{ "048-v1.v1",    0x200000, 0xf1947d2b, 5 | BRF_SND },           //  5 Sound data / mask rom TC5316200
};

STDROMPICKEXT(janshin, janshin, neogeo)
STD_ROM_FN(janshin)

struct BurnDriver BurnDrvjanshin = {
	"janshin", NULL, "neogeo", NULL, "1994",
	"Jyanshin Densetsu - Quest of Jongmaster\0", NULL, "Aicom", "Neo Geo MVS",
	L"\u96C0\u795E\u4F1D\u8AAC - quest of jongmaster\0Jyanshin Densetsu\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_MAHJONG, GBF_MAHJONG, 0,
	NULL, janshinRomInfo, janshinRomName, NULL, NULL, NULL, NULL, neomahjongInputInfo, neomahjongDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Pulstar

static struct BurnRomInfo pulstarRomDesc[] = {
	{ "089-p1.p1",    0x100000, 0x5e5847a2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC538200
	{ "089-p2.sp2",   0x200000, 0x028b774c, 1 | BRF_ESS | BRF_PRG }, //  1 					/ mask rom TC5316200

	{ "089-s1.s1",    0x020000, 0xc79fc2c8, 2 | BRF_GRA },           //  2 Text layer tiles / mask rom TC531000

	{ "089-c1.c1",    0x400000, 0xf4e97332, 3 | BRF_GRA },           //  3 Sprite data		/ mask rom TC5332205
	{ "089-c2.c2",    0x400000, 0x836d14da, 3 | BRF_GRA },           //  4 					/ mask rom TC5332205
	{ "089-c3.c3",    0x400000, 0x913611c4, 3 | BRF_GRA },           //  5 					/ mask rom TC5332205
	{ "089-c4.c4",    0x400000, 0x44cef0e3, 3 | BRF_GRA },           //  6 					/ mask rom TC5332205
	{ "089-c5.c5",    0x400000, 0x89baa1d7, 3 | BRF_GRA },           //  7 					/ mask rom TC5332205
	{ "089-c6.c6",    0x400000, 0xb2594d56, 3 | BRF_GRA },           //  8 					/ mask rom TC5332205
	{ "089-c7.c7",    0x200000, 0x6a5618ca, 3 | BRF_GRA },           //  9 					/ mask rom TC5316200
	{ "089-c8.c8",    0x200000, 0xa223572d, 3 | BRF_GRA },           // 10 					/ mask rom TC5316200

	{ "089-m1.m1",    0x020000, 0xff3df7c7, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ mask rom TC531001

	{ "089-v1.v1",    0x400000, 0x6f726ecb, 5 | BRF_SND },           // 12 Sound data		/ mask rom TC5332204
	{ "089-v2.v2",    0x400000, 0x9d2db551, 5 | BRF_SND },           // 13 					/ mask rom TC5332204
};

STDROMPICKEXT(pulstar, pulstar, neogeo)
STD_ROM_FN(pulstar)

struct BurnDriver BurnDrvpulstar = {
	"pulstar", NULL, "neogeo", NULL, "1995",
	"Pulstar\0", NULL, "Aicom", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_HORSHOOT, 0,
	NULL, pulstarRomInfo, pulstarRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Blazing Star

static struct BurnRomInfo blazstarRomDesc[] = {
	{ "239-p1.p1",    0x100000, 0x183682f8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "239-p2.sp2",   0x200000, 0x9a9f4154, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "239-s1.s1",    0x020000, 0xd56cb498, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "239-c1.c1",    0x400000, 0x84f6d584, 3 | BRF_GRA },           //  3 Sprite data
	{ "239-c2.c2",    0x400000, 0x05a0cb22, 3 | BRF_GRA },           //  4
	{ "239-c3.c3",    0x400000, 0x5fb69c9e, 3 | BRF_GRA },           //  5
	{ "239-c4.c4",    0x400000, 0x0be028c4, 3 | BRF_GRA },           //  6
	{ "239-c5.c5",    0x400000, 0x74bae5f8, 3 | BRF_GRA },           //  7
	{ "239-c6.c6",    0x400000, 0x4e0700d2, 3 | BRF_GRA },           //  8
	{ "239-c7.c7",    0x400000, 0x010ff4fd, 3 | BRF_GRA },           //  9
	{ "239-c8.c8",    0x400000, 0xdb60460e, 3 | BRF_GRA },           // 10

	{ "239-m1.m1",    0x020000, 0xd31a3aea, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "239-v1.v1",    0x400000, 0x1b8d5bf7, 5 | BRF_SND },           // 12 Sound data
	{ "239-v2.v2",    0x400000, 0x74cf0a70, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(blazstar, blazstar, neogeo)
STD_ROM_FN(blazstar)

struct BurnDriver BurnDrvblazstar = {
	"blazstar", NULL, "neogeo", NULL, "1998",
	"Blazing Star\0", NULL, "Yumekobo", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_HORSHOOT, 0,
	NULL, blazstarRomInfo, blazstarRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Prehistoric Isle 2
/* Original Version, Encrypted GFX */ /* MVS ONLY RELEASE */

static struct BurnRomInfo preisle2RomDesc[] = {
	{ "255-p1.p1",    0x100000, 0xdfa3c0f3, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "255-p2.sp2",   0x400000, 0x42050b80, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "255-c1.c1",    0x800000, 0xea06000b, 3 | BRF_GRA },           //  2 Sprite data
	{ "255-c2.c2",    0x800000, 0x04e67d79, 3 | BRF_GRA },           //  3
	{ "255-c3.c3",    0x800000, 0x60e31e08, 3 | BRF_GRA },           //  4
	{ "255-c4.c4",    0x800000, 0x40371d69, 3 | BRF_GRA },           //  5
	{ "255-c5.c5",    0x800000, 0x0b2e6adf, 3 | BRF_GRA },           //  6
	{ "255-c6.c6",    0x800000, 0xb001bdd3, 3 | BRF_GRA },           //  7

	{ "255-m1.m1",    0x020000, 0x8efd4014, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	{ "255-v1.v1",    0x400000, 0x5a14543d, 5 | BRF_SND },           //  9 Sound data
	{ "255-v2.v2",    0x200000, 0x6610d91a, 5 | BRF_SND },           // 10
};

STDROMPICKEXT(preisle2, preisle2, neogeo)
STD_ROM_FN(preisle2)

INT32 preisle2Init()
{
	nNeoProtectionXor = 0x9F;
	return NeoInit();
}

struct BurnDriver BurnDrvpreisle2 = {
	"preisle2", NULL, "neogeo", NULL, "1999",
	"Prehistoric Isle 2\0", NULL, "Yumekobo", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42, GBF_HORSHOOT, 0,
	NULL, preisle2RomInfo, preisle2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	preisle2Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Spin Master / Miracle Adventure

static struct BurnRomInfo spinmastRomDesc[] = {
	{ "062-p1.p1",    0x100000, 0x37aba1aa, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC538200
	{ "062-p2.sp2",   0x100000, 0xf025ab77, 1 | BRF_ESS | BRF_PRG }, //  1 					/ mask rom TC538200

	{ "062-s1.s1",    0x020000, 0x289e2bbe, 2 | BRF_GRA },           //  2 Text layer tiles / mask rom TC531000

	{ "062-c1.c1",    0x100000, 0xa9375aa2, 3 | BRF_GRA },           //  3 Sprite data		/ mask rom TC538200
	{ "062-c2.c2",    0x100000, 0x0e73b758, 3 | BRF_GRA },           //  4 					/ mask rom TC538200
	{ "062-c3.c3",    0x100000, 0xdf51e465, 3 | BRF_GRA },           //  5 					/ mask rom TC538200
	{ "062-c4.c4",    0x100000, 0x38517e90, 3 | BRF_GRA },           //  6 					/ mask rom TC538200
	{ "062-c5.c5",    0x100000, 0x7babd692, 3 | BRF_GRA },           //  7 					/ mask rom TC538200
	{ "062-c6.c6",    0x100000, 0xcde5ade5, 3 | BRF_GRA },           //  8 					/ mask rom TC538200
	{ "062-c7.c7",    0x100000, 0xbb2fd7c0, 3 | BRF_GRA },           //  9 					/ mask rom TC538200
	{ "062-c8.c8",    0x100000, 0x8d7be933, 3 | BRF_GRA },           // 10 					/ mask rom TC538200

	{ "062-m1.m1",    0x020000, 0x76108b2f, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ mask rom TC531001

	{ "062-v1.v1",    0x100000, 0xcc281aef, 5 | BRF_SND },           // 12 Sound data		/ mask rom TC538200
};

STDROMPICKEXT(spinmast, spinmast, neogeo)
STD_ROM_FN(spinmast)

struct BurnDriver BurnDrvspinmast = {
	"spinmast", NULL, "neogeo", NULL, "1993",
	"Spin Master / Miracle Adventure\0", NULL, "Data East Corporation", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, 0,
	NULL, spinmastRomInfo, spinmastRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Windjammers / Flying Power Disc
/* MVS AND AES VERSION? */

static struct BurnRomInfo wjammersRomDesc[] = {
	/* also found AES set with PROG board NEO-AEG PROG42G-1 and CHA board NEO-AEG CHA42G-1
	   same layouts and chip labels */
	{ "065-p1.p1",    0x100000, 0x6692c140, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC538200

	{ "065-s1.s1",    0x020000, 0x074b5723, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "065-c1.c1",    0x100000, 0xc7650204, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC538200
	{ "065-c2.c2",    0x100000, 0xd9f3e71d, 3 | BRF_GRA },           //  3 					/ mask rom TC538200
	{ "065-c3.c3",    0x100000, 0x40986386, 3 | BRF_GRA },           //  4 					/ mask rom TC538200
	{ "065-c4.c4",    0x100000, 0x715e15ff, 3 | BRF_GRA },           //  5 					/ mask rom TC538200

	{ "065-m1.m1",    0x020000, 0x52c23cfc, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ mask rom TC531001

	{ "065-v1.v1",    0x100000, 0xce8b3698, 5 | BRF_SND },           //  7 Sound data		/ mask rom TC538200
	{ "065-v2.v2",    0x100000, 0x659f9b96, 5 | BRF_SND },           //  8 					/ mask rom TC538200
	{ "065-v3.v3",    0x100000, 0x39f73061, 5 | BRF_SND },           //  9 					/ mask rom TC538200
	{ "065-v4.v4",    0x100000, 0x5dee7963, 5 | BRF_SND },           // 10 					/ mask rom TC538200
};

STDROMPICKEXT(wjammers, wjammers, neogeo)
STD_ROM_FN(wjammers)

struct BurnDriver BurnDrvwjammers = {
	"wjammers", NULL, "neogeo", NULL, "1994",
	"Windjammers / Flying Power Disc\0", NULL, "Data East Corporation", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, wjammersRomInfo, wjammersRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Karnov's Revenge / Fighter's History Dynamite

static struct BurnRomInfo karnovrRomDesc[] = {
	{ "066-p1.p1",    0x100000, 0x8c86fd22, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200

	{ "066-s1.s1",    0x020000, 0xbae5d5e5, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "066-c1.c1",    0x200000, 0x09dfe061, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "066-c2.c2",    0x200000, 0xe0f6682a, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "066-c3.c3",    0x200000, 0xa673b4f7, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "066-c4.c4",    0x200000, 0xcb3dc5f4, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "066-c5.c5",    0x200000, 0x9a28785d, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "066-c6.c6",    0x200000, 0xc15c01ed, 3 | BRF_GRA },           //  7 					/ TC5316200

	{ "066-m1.m1",    0x020000, 0x030beae4, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code			/ TC531001

	{ "066-v1.v1",    0x200000, 0x0b7ea37a, 5 | BRF_SND },           //  9 Sound data		/ TC5316200
};

STDROMPICKEXT(karnovr, karnovr, neogeo)
STD_ROM_FN(karnovr)

struct BurnDriver BurnDrvkarnovr = {
	"karnovr", NULL, "neogeo", NULL, "1994",
	"Karnov's Revenge / Fighter's History Dynamite\0", NULL, "Data East Corporation", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, karnovrRomInfo, karnovrRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Street Hoop / Street Slam / Dunk Dream (DEM-004) (DEH-004)
/* MVS AND AES VERSION */

static struct BurnRomInfo strhoopRomDesc[] = {
	{ "079-p1.p1",    0x100000, 0x5e78328e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200

	{ "079-s1.s1",    0x020000, 0x3ac06665, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "079-c1.c1",    0x200000, 0x0581c72a, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "079-c2.c2",    0x200000, 0x5b9b8fb6, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "079-c3.c3",    0x200000, 0xcd65bb62, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "079-c4.c4",    0x200000, 0xa4c90213, 3 | BRF_GRA },           //  5 					/ TC5316200

	{ "079-m1.m1",    0x020000, 0xbee3455a, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "079-v1.v1",    0x200000, 0x718a2400, 5 | BRF_SND },           //  7 Sound data		/ TC5316200
	{ "079-v2.v2",    0x100000, 0x720774eb, 5 | BRF_SND },           //  8 					/ TC538200
	/* AES 079-v2 is only 4 mbit (TC534200), data is the same
	{ "079-v2.v2",    0x080000, 0xb19884f8, 5 | BRF_SND },           //  8 					/ TC534200 */
};

STDROMPICKEXT(strhoop, strhoop, neogeo)
STD_ROM_FN(strhoop)

struct BurnDriver BurnDrvstrhoop = {
	"strhoop", NULL, "neogeo", NULL, "1994",
	"Street Hoop / Street Slam / Dunk Dream (DEM-004) (DEH-004)\0", NULL, "Data East Corporation", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, strhoopRomInfo, strhoopRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Ghostlop (prototype)

static struct BurnRomInfo ghostlopRomDesc[] = {
	{ "proto_228-p1.p1",   0x100000, 0x6033172e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "proto_228-s1.s1",   0x020000, 0x83c24e81, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "proto_228-c1.c1",   0x400000, 0xbfc99efe, 3 | BRF_GRA },           //  2 Sprite data
	{ "proto_228-c2.c2",   0x400000, 0x69788082, 3 | BRF_GRA },           //  3

	{ "proto_228-m1.m1",   0x020000, 0xfd833b33, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "proto_228-v1.v1",   0x200000, 0xc603fce6, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(ghostlop, ghostlop, neogeo)
STD_ROM_FN(ghostlop)

struct BurnDriver BurnDrvghostlop = {
	"ghostlop", NULL, "neogeo", NULL, "1996",
	"Ghostlop (prototype)\0", NULL, "Data East Corporation", "Neo Geo MVS",
	L"Ghostlop \u30B4\u30FC\u30B9\u30C8\u30ED\u30C3\u30D7 (prototype)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, ghostlopRomInfo, ghostlopRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Magical Drop II

static struct BurnRomInfo magdrop2RomDesc[] = {
	{ "221-p1.p1",    0x080000, 0x7be82353, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "221-s1.s1",    0x020000, 0x2a4063a3, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "221-c1.c1",    0x400000, 0x1f862a14, 3 | BRF_GRA },           //  2 Sprite data
	{ "221-c2.c2",    0x400000, 0x14b90536, 3 | BRF_GRA },           //  3

	{ "221-m1.m1",    0x020000, 0xbddae628, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "221-v1.v1",    0x200000, 0x7e5e53e4, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(magdrop2, magdrop2, neogeo)
STD_ROM_FN(magdrop2)

struct BurnDriver BurnDrvmagdrop2 = {
	"magdrop2", NULL, "neogeo", NULL, "1996",
	"Magical Drop II\0", NULL, "Data East Corporation", "Neo Geo MVS",
	L"Magical Drop II\0\u30DE\u30B8\u30AB\u30EB\u30C9\u30ED\u30C3\u30D7\uFF12\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, magdrop2RomInfo, magdrop2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Magical Drop III

static struct BurnRomInfo magdrop3RomDesc[] = {
	{ "233-p1.p1",    0x100000, 0x931e17fa, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "233-s1.s1",    0x020000, 0x7399e68a, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "233-c1.c1",    0x400000, 0x65e3f4c4, 3 | BRF_GRA },           //  2 Sprite data
	{ "233-c2.c2",    0x400000, 0x35dea6c9, 3 | BRF_GRA },           //  3
	{ "233-c3.c3",    0x400000, 0x0ba2c502, 3 | BRF_GRA },           //  4
	{ "233-c4.c4",    0x400000, 0x70dbbd6d, 3 | BRF_GRA },           //  5

	{ "233-m1.m1",    0x020000, 0x5beaf34e, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "233-v1.v1",    0x400000, 0x58839298, 5 | BRF_SND },           //  7 Sound data
	{ "233-v2.v2",    0x080000, 0xd5e30df4, 5 | BRF_SND },           //  8
};

STDROMPICKEXT(magdrop3, magdrop3, neogeo)
STD_ROM_FN(magdrop3)

struct BurnDriver BurnDrvmagdrop3 = {
	"magdrop3", NULL, "neogeo", NULL, "1997",
	"Magical Drop III\0", NULL, "Data East Corporation", "Neo Geo MVS",
	L"Magical Drop III\0\u30DE\u30B8\u30AB\u30EB\u30C9\u30ED\u30C3\u30D7III\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, magdrop3RomInfo, magdrop3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Nightmare in the Dark
/* Original Version - Encrypted GFX */ /* MVS ONLY RELEASE */

static struct BurnRomInfo nitdRomDesc[] = {
	{ "260-p1.p1",    0x080000, 0x61361082, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "260-c1.c1",    0x800000, 0x147b0c7f, 3 | BRF_GRA },           //  1 Sprite data
	{ "260-c2.c2",    0x800000, 0xd2b04b0d, 3 | BRF_GRA },           //  2

	{ "260-m1.m1",    0x080000, 0x6407c5e5, 4 | BRF_ESS | BRF_PRG }, //  3 Z80 code

	{ "260-v1.v1",    0x400000, 0x24b0480c, 5 | BRF_SND },           //  4 Sound data
};

STDROMPICKEXT(nitd, nitd, neogeo)
STD_ROM_FN(nitd)

INT32 nitdInit()
{
	nNeoProtectionXor = 0xFF;
	return NeoInit();
}

struct BurnDriver BurnDrvnitd = {
	"nitd", NULL, "neogeo", NULL, "2000",
	"Nightmare in the Dark\0", NULL, "Eleven / Gavaking", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42, GBF_PLATFORM, 0,
	NULL, nitdRomInfo, nitdRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	nitdInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Nightmare in the Dark (bootleg)

static struct BurnRomInfo nitdblRomDesc[] = {
	{ "nitd-p1.bin", 0x080000, 0x1a05bd1b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "nitd-s1.bin", 0x020000, 0xdd3bf47c, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "nitd-c1.bin", 0x200000, 0xb4353190, 3 | BRF_GRA },           //  2 Sprite data
	{ "nitd-c2.bin", 0x200000, 0x6e27511f, 3 | BRF_GRA },           //  3
	{ "nitd-c3.bin", 0x200000, 0x472cf075, 3 | BRF_GRA },           //  4
	{ "nitd-c4.bin", 0x200000, 0x4c3926e6, 3 | BRF_GRA },           //  5

	{ "260-m1.m1",   0x080000, 0x6407c5e5, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "nitd-v1.bin", 0x200000, 0x79008868, 5 | BRF_SND },           //  7 Sound data
	{ "nitd-v2.bin", 0x200000, 0x728558f9, 5 | BRF_SND },           //  8
};

STDROMPICKEXT(nitdbl, nitdbl, neogeo)
STD_ROM_FN(nitdbl)

struct BurnDriver BurnDrvnitdbl = {
	"nitdbl", "nitd", "neogeo", NULL, "2001",
	"Nightmare in the Dark (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, nitdblRomInfo, nitdblRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Gururin
/* MVS ONLY RELEASE */

static struct BurnRomInfo gururinRomDesc[] = {
	/* It also exist MVS set with PROG board NEO-MVS PROG16 and CHA board NEO-MVS CHA256; same chip labels */
	{ "067-p1.p1",    0x080000, 0x4cea8a49, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ UM8303B

	{ "067-s1.s1",    0x020000, 0xb119e1eb, 2 | BRF_GRA },           //  1 Text layer tiles / UMK300

	{ "067-c1.c1",    0x200000, 0x35866126, 3 | BRF_GRA },           //  2 Sprite data		/ UMT301B
	{ "067-c2.c2",    0x200000, 0x9db64084, 3 | BRF_GRA },           //  3 					/ UMT302B

	{ "067-m1.m1",    0x020000, 0x9e3c6328, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code			/ UML359

	{ "067-v1.v1",    0x080000, 0xcf23afd0, 5 | BRF_SND },           //  5 Sound data		/ UM8302
};

STDROMPICKEXT(gururin, gururin, neogeo)
STD_ROM_FN(gururin)

struct BurnDriver BurnDrvgururin = {
	"gururin", NULL, "neogeo", NULL, "1994",
	"Gururin\0", NULL, "Face", "Neo Geo MVS",
	L"\u3050\u308B\u308A\u3093\0Gururin\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, gururinRomInfo, gururinRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Money Puzzle Exchanger / Money Idol Exchanger
/* MVS ONLY RELEASE */

static struct BurnRomInfo miexchngRomDesc[] = {
	{ "231-p1.p1",    0x080000, 0x61be1810, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "231-s1.s1",    0x020000, 0xfe0c0c53, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "231-c1.c1",    0x200000, 0x6c403ba3, 3 | BRF_GRA },           //  2 Sprite data
	{ "231-c2.c2",    0x200000, 0x554bcd9b, 3 | BRF_GRA },           //  3
	{ "231-c3.c3",    0x100000, 0x4f6f7a63, 3 | BRF_GRA },           //  4
	{ "231-c4.c4",    0x100000, 0x2e35e71b, 3 | BRF_GRA },           //  5

	{ "231-m1.m1",    0x020000, 0xde41301b, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "231-v1.v1",    0x400000, 0x113fb898, 5 | BRF_SND },           //  7 Sound data
};

STDROMPICKEXT(miexchng, miexchng, neogeo)
STD_ROM_FN(miexchng)

struct BurnDriver BurnDrvmiexchng = {
	"miexchng", NULL, "neogeo", NULL, "1997",
	"Money Puzzle Exchanger / Money Idol Exchanger\0", NULL, "Face", "Neo Geo MVS",
	L"Money Puzzle Exchanger\0\u30DE\u30CD\u30FC\u30A2\u30A4\u30C9\u30EB\u30A8\u30AF\u30B9\u30C1\u30A7\u30F3\u30B8\u30E3\u30FC\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, miexchngRomInfo, miexchngRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Panic Bomber
/* MVS ONLY RELEASE */

static struct BurnRomInfo panicbomRomDesc[] = {
	{ "073-p1.p1",    0x080000, 0xadc356ad, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC534200

	{ "073-s1.s1",    0x020000, 0xb876de7e, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "073-c1.c1",    0x100000, 0x8582e1b5, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC538200
	{ "073-c2.c2",    0x100000, 0xe15a093b, 3 | BRF_GRA },           //  3 					/ mask rom TC538200

	{ "073-m1.m1",    0x020000, 0x3cdf5d88, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code			/ mask rom TC531001

	{ "073-v1.v1",    0x200000, 0x7fc86d2f, 5 | BRF_SND },           //  5 Sound data		/ mask rom TC5316200
	{ "073-v2.v2",    0x100000, 0x082adfc7, 5 | BRF_SND },           //  6 					/ mask rom TC538200
};

STDROMPICKEXT(panicbom, panicbom, neogeo)
STD_ROM_FN(panicbom)

struct BurnDriver BurnDrvpanicbom = {
	"panicbom", NULL, "neogeo", NULL, "1994",
	"Panic Bomber\0", NULL, "Eighting / Hudson", "Neo Geo MVS",
	L"Panic Bomber\0\u3071\u306B\u3063\u304F\u30DC\u30F3\u30D0\u30FC \u30DC\u30F3\u30D0\u30FC\u30DE\u30F3\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, panicbomRomInfo, panicbomRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Far East of Eden - Kabuki Klash / Tengai Makyou - Shin Den

static struct BurnRomInfo kabukiklRomDesc[] = {
	/* also find MVS set with PROG board NEO-MVS PROGTOP and CHA board NOE-MVS CHA256; same chip labels */
	{ "092-p1.p1",    0x200000, 0x28ec9b77, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC5316200

	{ "092-s1.s1",    0x020000, 0xa3d68ee2, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "092-c1.c1",    0x400000, 0x2a9fab01, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5332205
	{ "092-c2.c2",    0x400000, 0x6d2bac02, 3 | BRF_GRA },           //  3 					/ mask rom TC5332205
	{ "092-c3.c3",    0x400000, 0x5da735d6, 3 | BRF_GRA },           //  4 					/ mask rom TC5332205
	{ "092-c4.c4",    0x400000, 0xde07f997, 3 | BRF_GRA },           //  5 					/ mask rom TC5332205

	{ "092-m1.m1",    0x020000, 0x91957ef6, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ mask rom TC531001

	{ "092-v1.v1",    0x200000, 0x69e90596, 5 | BRF_SND },           //  7 Sound data		/ mask rom TC5316200
	{ "092-v2.v2",    0x200000, 0x7abdb75d, 5 | BRF_SND },           //  8 					/ mask rom TC5316200
	{ "092-v3.v3",    0x200000, 0xeccc98d3, 5 | BRF_SND },           //  9 					/ mask rom TC5316200
	{ "092-v4.v4",    0x100000, 0xa7c9c949, 5 | BRF_SND },           // 10 					/ mask rom TC538200
};

STDROMPICKEXT(kabukikl, kabukikl, neogeo)
STD_ROM_FN(kabukikl)

struct BurnDriver BurnDrvkabukikl = {
	"kabukikl", NULL, "neogeo", NULL, "1995",
	"Far East of Eden - Kabuki Klash / Tengai Makyou - Shin Den\0", NULL, "Hudson", "Neo Geo MVS",
	L"Kabuki Klash - far east of eden\0\u5929\u5916\u9B54\u5883 - \u771F\u4F1D\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, kabukiklRomInfo, kabukiklRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Neo Bomberman
/* MVS ONLY RELEASE */

static struct BurnRomInfo neobombeRomDesc[] = {
	{ "093-p1.p1",    0x100000, 0xa1a71d0d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200

	{ "093-s1.s1",    0x020000, 0x4b3fa119, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "093-c1.c1",    0x400000, 0xd1f328f8, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332205
	{ "093-c2.c2",    0x400000, 0x82c49540, 3 | BRF_GRA },           //  3 					/ TC5332205
	{ "093-c3.c3",    0x080000, 0xe37578c5, 3 | BRF_GRA },           //  4 					/ TC534200
	{ "093-c4.c4",    0x080000, 0x59826783, 3 | BRF_GRA },           //  5 					/ TC534200

	{ "093-m1.m1",    0x020000, 0xe81e780b, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "093-v1.v1",    0x400000, 0x02abd4b0, 5 | BRF_SND },           //  7 Sound data		/ TC5332204
	{ "093-v2.v2",    0x200000, 0xa92b8b3d, 5 | BRF_SND },           //  8 					/ TC5316200
};

STDROMPICKEXT(neobombe, neobombe, neogeo)
STD_ROM_FN(neobombe)

struct BurnDriver BurnDrvneobombe = {
	"neobombe", NULL, "neogeo", NULL, "1997",
	"Neo Bomberman\0", NULL, "Hudson", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_MAZE, 0,
	NULL, neobombeRomInfo, neobombeRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Minasanno Okagesamadesu! Daisugorokutaikai (MOM-001)(MOH-001)
/* MVS AND AES VERSION */

static struct BurnRomInfo minasanRomDesc[] = {
	{ "027-p1.p1",    0x080000, 0xc8381327, 1 | BRF_ESS | BRF_PRG }, //  0 68K code / HN62422PC

	{ "027-s1.s1",    0x020000, 0xe5824baa, 2 | BRF_GRA },           //  1 Text layer tiles / HN62321BP

	{ "027-c1.c1",    0x100000, 0xd0086f94, 3 | BRF_GRA },           //  2 Sprite data / HN62408PD
	{ "027-c2.c2",    0x100000, 0xda61f5a6, 3 | BRF_GRA },           //  3 / HN62408PD
	{ "027-c3.c3",    0x100000, 0x08df1228, 3 | BRF_GRA },           //  4 / HN62408PD
	{ "027-c4.c4",    0x100000, 0x54e87696, 3 | BRF_GRA },           //  5 / HN62408PD

	{ "027-m1.m1",    0x020000, 0xadd5a226, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code / HN62321AP

	{ "027-v11.v11",  0x100000, 0x59ad4459, 5 | BRF_SND },           //  7 Sound data / HN62308BPC
	{ "027-v21.v21",  0x100000, 0xdf5b4eeb, 6 | BRF_SND },           //  8 / HN62308BPC
};

STDROMPICKEXT(minasan, minasan, neogeo)
STD_ROM_FN(minasan)

struct BurnDriver BurnDrvminasan = {
	"minasan", NULL, "neogeo", NULL, "1990",
	"Minasanno Okagesamadesu! Daisugorokutaikai (MOM-001)(MOH-001)\0", NULL, "Monolith Corp.", "Neo Geo MVS",
	L"\u307F\u306A\u3055\u3093\u306E\u304A\u304B\u3052\u3055\u307E\u3067\u3059\uFF01\0Minnasanno Okagesamadesu (MOM-001)(MOH-001)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_MAHJONG, GBF_MAHJONG, 0,
	NULL, minasanRomInfo, minasanRomName, NULL, NULL, NULL, NULL, neomahjongInputInfo, neomahjongDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Bakatonosama Mahjong Manyuuki (MOM-002)(MOH-002)
/* MVS AND AES VERSION */

static struct BurnRomInfo bakatonoRomDesc[] = {
	{ "036-p1.p1",    0x080000, 0x1c66b6fa, 1 | BRF_ESS | BRF_PRG }, //  0 68K code / CXK384500

	{ "036-s1.s1",    0x020000, 0xf3ef4485, 2 | BRF_GRA },           //  1 Text layer tiles / CXK381000

	{ "036-c1.c1",    0x100000, 0xfe7f1010, 3 | BRF_GRA },           //  2 Sprite data / CXK388000
	{ "036-c2.c2",    0x100000, 0xbbf003f5, 3 | BRF_GRA },           //  3 / CXK388000
	{ "036-c3.c3",    0x100000, 0x9ac0708e, 3 | BRF_GRA },           //  4 / CXK388000
	{ "036-c4.c4",    0x100000, 0xf2577d22, 3 | BRF_GRA },           //  5 / CXK388000

	{ "036-m1.m1",    0x020000, 0xf1385b96, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code / CXK381003

	{ "036-v1.v1",    0x100000, 0x1c335dce, 5 | BRF_SND },           //  7 Sound data / CXK388000
	{ "036-v2.v2",    0x100000, 0xbbf79342, 5 | BRF_SND },           //  8 / CXK388000
};

STDROMPICKEXT(bakatono, bakatono, neogeo)
STD_ROM_FN(bakatono)

struct BurnDriver BurnDrvbakatono = {
	"bakatono", NULL, "neogeo", NULL, "1991",
	"Bakatonosama Mahjong Manyuuki (MOM-002)(MOH-002)\0", NULL, "Monolith Corp.", "Neo Geo MVS",
	L"\u30D0\u30AB\u6BBF\u69D8 \u9EBB\u96C0\u6F2B\u904A\u8A18\0Bakatonosama Mahjong Manyuki (MOM-002)(MOH-002)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_MAHJONG, GBF_MAHJONG, 0,
	NULL, bakatonoRomInfo, bakatonoRomName, NULL, NULL, NULL, NULL, neomahjongInputInfo, neomahjongDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo Turf Masters / Big Tournament Golf

static struct BurnRomInfo turfmastRomDesc[] = {
	{ "200-p1.p1",    0x200000, 0x28c83048, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC5316200

	{ "200-s1.s1",    0x020000, 0x9a5402b2, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "200-c1.c1",    0x400000, 0x8e7bf41a, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5332205
	{ "200-c2.c2",    0x400000, 0x5a65a8ce, 3 | BRF_GRA },           //  3 					/ mask rom TC5332205

	{ "200-m1.m1",    0x020000, 0x9994ac00, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code			/ mask rom TC531001

	{ "200-v1.v1",    0x200000, 0x00fd48d2, 5 | BRF_SND },           //  5 Sound data		/ mask rom TC5316200
	{ "200-v2.v2",    0x200000, 0x082acb31, 5 | BRF_SND },           //  6 					/ mask rom TC5316200
	{ "200-v3.v3",    0x200000, 0x7abca053, 5 | BRF_SND },           //  7 					/ mask rom TC5316200
	{ "200-v4.v4",    0x200000, 0x6c7b4902, 5 | BRF_SND },           //  8 					/ mask rom TC5316200
};

STDROMPICKEXT(turfmast, turfmast, neogeo)
STD_ROM_FN(turfmast)

struct BurnDriver BurnDrvturfmast = {
	"turfmast", NULL, "neogeo", NULL, "1996",
	"Neo Turf Masters / Big Tournament Golf\0", NULL, "Nazca", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_SPORTSMISC, 0,
	NULL, turfmastRomInfo, turfmastRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug - Super Vehicle-001

static struct BurnRomInfo mslugRomDesc[] = {
	{ "201-p1.p1",    0x200000, 0x08d8daa5, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "201-s1.s1",    0x020000, 0x2f55958d, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "201-c1.c1",    0x400000, 0x72813676, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332205
	{ "201-c2.c2",    0x400000, 0x96f62574, 3 | BRF_GRA },           //  3 					/ TC5332205
	{ "201-c3.c3",    0x400000, 0x5121456a, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "201-c4.c4",    0x400000, 0xf4ad59a3, 3 | BRF_GRA },           //  5 					/ TC5332205

	{ "201-m1.m1",    0x020000, 0xc28b3253, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "201-v1.v1",    0x400000, 0x23d22ed1, 5 | BRF_SND },           //  7 Sound data		/ TC5332204
	{ "201-v2.v2",    0x400000, 0x472cf9db, 5 | BRF_SND },           //  8 					/ TC5332204
};

STDROMPICKEXT(mslug, mslug, neogeo)
STD_ROM_FN(mslug)

struct BurnDriver BurnDrvmslug = {
	"mslug", NULL, "neogeo", NULL, "1996",
	"Metal Slug - Super Vehicle-001\0", NULL, "Nazca", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslugRomInfo, mslugRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Zed Blade / Operation Ragnarok
/* MVS ONLY RELEASE */

static struct BurnRomInfo zedbladeRomDesc[] = {
	{ "076-p1.p1",    0x080000, 0xd7c1effd, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC538200

	{ "076-s1.s1",    0x020000, 0xf4c25dd5, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "076-c1.c1",    0x200000, 0x4d9cb038, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5316200
	{ "076-c2.c2",    0x200000, 0x09233884, 3 | BRF_GRA },           //  3 					/ mask rom TC5316200
	{ "076-c3.c3",    0x200000, 0xd06431e3, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "076-c4.c4",    0x200000, 0x4b1c089b, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200

	{ "076-m1.m1",    0x020000, 0x7b5f3d0a, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ mask rom TC531001

	{ "076-v1.v1",    0x200000, 0x1a21d90c, 5 | BRF_SND },           //  7 Sound data		/ mask rom TC5316200
	{ "076-v2.v2",    0x200000, 0xb61686c3, 5 | BRF_SND },           //  8 					/ mask rom TC5316200
	{ "076-v3.v3",    0x100000, 0xb90658fa, 5 | BRF_SND },           //  9 					/ mask rom TC538200
};

STDROMPICKEXT(zedblade, zedblade, neogeo)
STD_ROM_FN(zedblade)

struct BurnDriver BurnDrvzedblade = {
	"zedblade", NULL, "neogeo", NULL, "1994",
	"Zed Blade / Operation Ragnarok\0", NULL, "NMK", "Neo Geo MVS",
	L"Zed Blade\0Operation Ragnarok\0\u4F5C\u6226\u540D\uFF02\u30E9\u30B0\u30CA\u30ED\u30AF\uFF02\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_HORSHOOT, 0,
	NULL, zedbladeRomInfo, zedbladeRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan,
	&NeoRecalcPalette, 0x1000, 304, 224, 4, 3
};

// Strikers 1945 Plus
/* Original Version, Encrypted GFX Roms */ /* MVS ONLY RELEASE */

static struct BurnRomInfo s1945pRomDesc[] = {
	{ "254-p1.p1",    0x100000, 0xff8efcff, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "254-p2.sp2",   0x400000, 0xefdfd4dd, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	{ "254-c1.c1",    0x800000, 0xae6fc8ef, 3 | BRF_GRA },           //  2 Sprite data
	{ "254-c2.c2",    0x800000, 0x436fa176, 3 | BRF_GRA },           //  3
	{ "254-c3.c3",    0x800000, 0xe53ff2dc, 3 | BRF_GRA },           //  4
	{ "254-c4.c4",    0x800000, 0x818672f0, 3 | BRF_GRA },           //  5
	{ "254-c5.c5",    0x800000, 0x4580eacd, 3 | BRF_GRA },           //  6
	{ "254-c6.c6",    0x800000, 0xe34970fc, 3 | BRF_GRA },           //  7
	{ "254-c7.c7",    0x800000, 0xf2323239, 3 | BRF_GRA },           //  8
	{ "254-c8.c8",    0x800000, 0x66848c7d, 3 | BRF_GRA },           //  9

	{ "254-m1.m1",    0x020000, 0x994b4487, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "254-v1.v1",    0x400000, 0x844f58fb, 5 | BRF_SND },           // 11 Sound data
	{ "254-v2.v2",    0x400000, 0xd9a248f0, 5 | BRF_SND },           // 12
	{ "254-v3.v3",    0x400000, 0x0b0d2d33, 5 | BRF_SND },           // 13
	{ "254-v4.v4",    0x400000, 0x6d13dc91, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(s1945p, s1945p, neogeo)
STD_ROM_FN(s1945p)

static INT32 s1945pInit()
{
	nNeoProtectionXor = 0x05;
	s1945pmode = 1;

	return NeoInit();
}

struct BurnDriver BurnDrvs1945p = {
	"s1945p", NULL, "neogeo", NULL, "1999",
	"Strikers 1945 Plus\0", NULL, "Psikyo", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42, GBF_VERSHOOT, 0,
	NULL, s1945pRomInfo, s1945pRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	s1945pInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Quiz King of Fighters (SAM-080)(SAH-080)
/* MVS AND AES VERSION */

static struct BurnRomInfo quizkofRomDesc[] = {
	{ "080-p1.p1",    0x100000, 0x4440315e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC538200

	{ "080-s1.s1",    0x020000, 0xd7b86102, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "080-c1.c1",    0x200000, 0xea1d764a, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5316200
	{ "080-c2.c2",    0x200000, 0xd331d4a4, 3 | BRF_GRA },           //  3 					/ mask rom TC5316200
	{ "080-c3.c3",    0x200000, 0xb4851bfe, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "080-c4.c4",    0x200000, 0xca6f5460, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200

	{ "080-m1.m1",    0x020000, 0xf5f44172, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ mask rom TC531001

	{ "080-v1.v1",    0x200000, 0x0be18f60, 5 | BRF_SND },           //  7 Sound data		/ mask rom TC5316200
	{ "080-v2.v2",    0x200000, 0x4abde3ff, 5 | BRF_SND },           //  8 					/ mask rom TC5316200
	{ "080-v3.v3",    0x200000, 0xf02844e2, 5 | BRF_SND },           //  9 					/ mask rom TC5316200
};

STDROMPICKEXT(quizkof, quizkof, neogeo)
STD_ROM_FN(quizkof)

struct BurnDriver BurnDrvquizkof = {
	"quizkof", NULL, "neogeo", NULL, "1995",
	"Quiz King of Fighters (SAM-080)(SAH-080)\0", NULL, "Saurus", "Neo Geo MVS",
	L"\u30AF\u30A4\u30BA\u30AD\u30F3\u30B0\u30AA\u30D6\u30D5\u30A1\u30A4\u30BF\u30FC\u30BA\0Quiz King of Fighters (SAM-080)(SAH-080)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_QUIZ, 0,
	NULL, quizkofRomInfo, quizkofRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Quiz King of Fighters (Korean release)
/* KOREAN VERSION */

static struct BurnRomInfo quizkofkRomDesc[] = {
	/* Made by Viccom Corp.; proms have manufacturer stamp VICxxxxxx-xxx, chip labels same as quizkof; Cart ID 0080 */
	/* Due to parent set naming limitations, roms have been named vic-xxx */
	{ "vic-080-p1.p1", 0x100000, 0x2589488e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "vic-080-s1.s1", 0x020000, 0xaf72c30f, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "vic-080-c1.c1", 0x200000, 0x94d90170, 3 | BRF_GRA },           //  2 Sprite data
	{ "vic-080-c2.c2", 0x200000, 0x297f25a1, 3 | BRF_GRA },           //  3
	{ "vic-080-c3.c3", 0x200000, 0xcf484c4f, 3 | BRF_GRA },           //  4
	{ "vic-080-c4.c4", 0x200000, 0x36e5d997, 3 | BRF_GRA },           //  5

	{ "vic-080-m1.m1", 0x020000, 0x4f157e9a, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "080-v1.v1",     0x200000, 0x0be18f60, 5 | BRF_SND },           //  7 Sound data			/ mask rom TC5316200
	{ "vic-080-v2.v2", 0x200000, 0x719fee5b, 5 | BRF_SND },           //  8
	{ "vic-080-v3.v3", 0x200000, 0x64b7efde, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(quizkofk, quizkofk, neogeo)
STD_ROM_FN(quizkofk)

struct BurnDriver BurnDrvquizkofk = {
	"quizkofk", "quizkof", "neogeo", NULL, "1995",
	"Quiz King of Fighters (Korean release)\0", NULL, "Saurus", "Neo Geo MVS",
	L"\u30AF\u30A4\u30BA\u30AD\u30F3\u30B0\u30AA\u30D6\u30D5\u30A1\u30A4\u30BF\u30FC\u30BA\0Quiz King of Fighters (Korean release)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_QUIZ, 0,
	NULL, quizkofkRomInfo, quizkofkRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Stakes Winner / Stakes Winner - GI kinzen seiha e no michi

static struct BurnRomInfo stakwinRomDesc[] = {
	{ "088-p1.p1",    0x200000, 0xbd5814f6, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "088-s1.s1",    0x020000, 0x073cb208, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "088-c1.c1",    0x200000, 0x6e733421, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "088-c2.c2",    0x200000, 0x4d865347, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "088-c3.c3",    0x200000, 0x8fa5a9eb, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "088-c4.c4",    0x200000, 0x4604f0dc, 3 | BRF_GRA },           //  5 					/ TC5316200

	{ "088-m1.m1",    0x020000, 0x2fe1f499, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "088-v1.v1",    0x200000, 0xb7785023, 5 | BRF_SND },           //  7 Sound data		/ TC5316200
};

STDROMPICKEXT(stakwin, stakwin, neogeo)
STD_ROM_FN(stakwin)

struct BurnDriver BurnDrvstakwin = {
	"stakwin", NULL, "neogeo", NULL, "1995",
	"Stakes Winner / Stakes Winner - GI kinzen seiha e no michi\0", NULL, "Saurus", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_RACING, 0,
	NULL, stakwinRomInfo, stakwinRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Ragnagard / Shin-Oh-Ken

static struct BurnRomInfo ragnagrdRomDesc[] = {
	{ "218-p1.p1",    0x200000, 0xca372303, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "218-s1.s1",    0x020000, 0x7d402f9a, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "218-c1.c1",    0x400000, 0xc31500a4, 3 | BRF_GRA },           //  2 Sprite data
	{ "218-c2.c2",    0x400000, 0x98aba1f9, 3 | BRF_GRA },           //  3
	{ "218-c3.c3",    0x400000, 0x833c163a, 3 | BRF_GRA },           //  4
	{ "218-c4.c4",    0x400000, 0xc1a30f69, 3 | BRF_GRA },           //  5
	{ "218-c5.c5",    0x400000, 0x6b6de0ff, 3 | BRF_GRA },           //  6
	{ "218-c6.c6",    0x400000, 0x94beefcf, 3 | BRF_GRA },           //  7
	{ "218-c7.c7",    0x400000, 0xde6f9b28, 3 | BRF_GRA },           //  8
	{ "218-c8.c8",    0x400000, 0xd9b311f6, 3 | BRF_GRA },           //  9

	{ "218-m1.m1",    0x020000, 0x17028bcf, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "218-v1.v1",    0x400000, 0x61eee7f4, 5 | BRF_SND },           // 11 Sound data
	{ "218-v2.v2",    0x400000, 0x6104e20b, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(ragnagrd, ragnagrd, neogeo)
STD_ROM_FN(ragnagrd)

struct BurnDriver BurnDrvragnagrd = {
	"ragnagrd", NULL, "neogeo", NULL, "1996",
	"Ragnagard / Shin-Oh-Ken\0", NULL, "Saurus", "Neo Geo MVS",
	L"Operation Ragnagard\0\u795E\u51F0\u62F3\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, ragnagrdRomInfo, ragnagrdRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Pleasure Goal / Futsal - 5 on 5 Mini Soccer (NGM-219)
/* MVS ONLY RELEASE */

static struct BurnRomInfo pgoalRomDesc[] = {
	{ "219-p1.p1",    0x200000, 0x6af0e574, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "219-s1.s1",    0x020000, 0x002f3c88, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "219-c1.c1",    0x400000, 0x67fec4dc, 3 | BRF_GRA },           //  2 Sprite data
	{ "219-c2.c2",    0x400000, 0x86ed01f2, 3 | BRF_GRA },           //  3
	{ "219-c3.c3",    0x200000, 0x5fdad0a5, 3 | BRF_GRA },           //  4
	{ "219-c4.c4",    0x200000, 0xf57b4a1c, 3 | BRF_GRA },           //  5

	{ "219-m1.m1",    0x020000, 0x958efdc8, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "219-v1.v1",    0x400000, 0xd0ae33d9, 5 | BRF_SND },           //  7 Sound data
};

STDROMPICKEXT(pgoal, pgoal, neogeo)
STD_ROM_FN(pgoal)

struct BurnDriver BurnDrvpgoal = {
	"pgoal", NULL, "neogeo", NULL, "1996",
	"Pleasure Goal / Futsal - 5 on 5 Mini Soccer (NGM-219)\0", NULL, "Saurus", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_SPORTSFOOTBALL, 0,
	NULL, pgoalRomInfo, pgoalRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Choutetsu Brikin'ger - Iron clad (Prototype)
/* Prototype - crcs should match the ones of the unreleased dump. */

static struct BurnRomInfo ironcladRomDesc[] = {
	{ "proto_220-p1.p1",   0x200000, 0x62a942c6, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "proto_220-s1.s1",   0x020000, 0x372fe217, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "proto_220-c1.c1",   0x400000, 0x9aa2b7dc, 3 | BRF_GRA },           //  2 Sprite data
	{ "proto_220-c2.c2",   0x400000, 0x8a2ad708, 3 | BRF_GRA },           //  3
	{ "proto_220-c3.c3",   0x400000, 0xd67fb15a, 3 | BRF_GRA },           //  4
	{ "proto_220-c4.c4",   0x400000, 0xe73ea38b, 3 | BRF_GRA },           //  5

	{ "proto_220-m1.m1",   0x020000, 0x3a08bb63, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "proto_220-v1.v1",   0x400000, 0x8f30a215, 5 | BRF_SND },           //  7 Sound data
};

STDROMPICKEXT(ironclad, ironclad, neogeo)
STD_ROM_FN(ironclad)

struct BurnDriver BurnDrvironclad = {
	"ironclad", NULL, "neogeo", NULL, "1996",
	"Choutetsu Brikin'ger - Iron clad (Prototype)\0", NULL, "Saurus", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_HORSHOOT, 0,
	NULL, ironcladRomInfo, ironcladRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeousaDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Choutetsu Brikin'ger - Iron clad (Prototype, older)
/* Prototype - bootleg/hack based on later release. */

static struct BurnRomInfo ironcladoRomDesc[] = {
	{ "proto_220-p1o.p1",  0x200000, 0xce37e3a0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "proto_220-s1.s1",   0x020000, 0x372fe217, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "proto_220-c1.c1",   0x400000, 0x9aa2b7dc, 3 | BRF_GRA },           //  2 Sprite data
	{ "proto_220-c2.c2",   0x400000, 0x8a2ad708, 3 | BRF_GRA },           //  3
	{ "proto_220-c3.c3",   0x400000, 0xd67fb15a, 3 | BRF_GRA },           //  4
	{ "proto_220-c4.c4",   0x400000, 0xe73ea38b, 3 | BRF_GRA },           //  5

	{ "proto_220-m1.m1",   0x020000, 0x3a08bb63, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "proto_220-v1.v1",   0x400000, 0x8f30a215, 5 | BRF_SND },           //  7 Sound data
};

STDROMPICKEXT(ironclado, ironclado, neogeo)
STD_ROM_FN(ironclado)

struct BurnDriver BurnDrvironclado = {
	"ironclado", "ironclad", "neogeo", NULL, "1996",
	"Choutetsu Brikin'ger - Iron clad (Prototype, older)\0", NULL, "Saurus", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_HORSHOOT, 0,
	NULL, ironcladoRomInfo, ironcladoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeousaDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Stakes Winner 2

static struct BurnRomInfo stakwin2RomDesc[] = {
	{ "227-p1.p1",    0x200000, 0xdaf101d2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "227-s1.s1",    0x020000, 0x2a8c4462, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "227-c1.c1",    0x400000, 0x7d6c2af4, 3 | BRF_GRA },           //  2 Sprite data
	{ "227-c2.c2",    0x400000, 0x7e402d39, 3 | BRF_GRA },           //  3
	{ "227-c3.c3",    0x200000, 0x93dfd660, 3 | BRF_GRA },           //  4
	{ "227-c4.c4",    0x200000, 0x7efea43a, 3 | BRF_GRA },           //  5

	{ "227-m1.m1",    0x020000, 0xc8e5e0f9, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "227-v1.v1",    0x400000, 0xb8f24181, 5 | BRF_SND },           //  7 Sound data
	{ "227-v2.v2",    0x400000, 0xee39e260, 5 | BRF_SND },           //  8
};

STDROMPICKEXT(stakwin2, stakwin2, neogeo)
STD_ROM_FN(stakwin2)

struct BurnDriver BurnDrvstakwin2 = {
	"stakwin2", NULL, "neogeo", NULL, "1996",
	"Stakes Winner 2\0", NULL, "Saurus", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_RACING, 0,
	NULL, stakwin2RomInfo, stakwin2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Shock Troopers (set 1)
/* MVS ONLY RELEASE */

static struct BurnRomInfo shocktroRomDesc[] = {
	{ "238-pg1.p1",   0x100000, 0xefedf8dc, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "238-p2.sp2",   0x400000, 0x5b4a09c5, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "238-s1.s1",    0x020000, 0x1f95cedb, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "238-c1.c1",    0x400000, 0x90c6a181, 3 | BRF_GRA },           //  3 Sprite data
	{ "238-c2.c2",    0x400000, 0x888720f0, 3 | BRF_GRA },           //  4
	{ "238-c3.c3",    0x400000, 0x2c393aa3, 3 | BRF_GRA },           //  5
	{ "238-c4.c4",    0x400000, 0xb9e909eb, 3 | BRF_GRA },           //  6
	{ "238-c5.c5",    0x400000, 0xc22c68eb, 3 | BRF_GRA },           //  7
	{ "238-c6.c6",    0x400000, 0x119323cd, 3 | BRF_GRA },           //  8
	{ "238-c7.c7",    0x400000, 0xa72ce7ed, 3 | BRF_GRA },           //  9
	{ "238-c8.c8",    0x400000, 0x1c7c2efb, 3 | BRF_GRA },           // 10

	{ "238-m1.m1",    0x020000, 0x075b9518, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "238-v1.v1",    0x400000, 0x260c0bef, 5 | BRF_SND },           // 12 Sound data
	{ "238-v2.v2",    0x200000, 0x4ad7d59e, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(shocktro, shocktro, neogeo)
STD_ROM_FN(shocktro)

struct BurnDriver BurnDrvshocktro = {
	"shocktro", NULL, "neogeo", NULL, "1997",
	"Shock Troopers (set 1)\0", NULL, "Saurus", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, 0,
	NULL, shocktroRomInfo, shocktroRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Shock Troopers (set 2)
/* MVS ONLY RELEASE */

static struct BurnRomInfo shocktroaRomDesc[] = {
	{ "238-p1.p1",    0x100000, 0x5677456f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "238-p2.sp2",   0x400000, 0x5b4a09c5, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "238-s1.s1",    0x020000, 0x1f95cedb, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "238-c1.c1",    0x400000, 0x90c6a181, 3 | BRF_GRA },           //  3 Sprite data
	{ "238-c2.c2",    0x400000, 0x888720f0, 3 | BRF_GRA },           //  4
	{ "238-c3.c3",    0x400000, 0x2c393aa3, 3 | BRF_GRA },           //  5
	{ "238-c4.c4",    0x400000, 0xb9e909eb, 3 | BRF_GRA },           //  6
	{ "238-c5.c5",    0x400000, 0xc22c68eb, 3 | BRF_GRA },           //  7
	{ "238-c6.c6",    0x400000, 0x119323cd, 3 | BRF_GRA },           //  8
	{ "238-c7.c7",    0x400000, 0xa72ce7ed, 3 | BRF_GRA },           //  9
	{ "238-c8.c8",    0x400000, 0x1c7c2efb, 3 | BRF_GRA },           // 10

	{ "238-m1.m1",    0x020000, 0x075b9518, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "238-v1.v1",    0x400000, 0x260c0bef, 5 | BRF_SND },           // 12 Sound data
	{ "238-v2.v2",    0x200000, 0x4ad7d59e, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(shocktroa, shocktroa, neogeo)
STD_ROM_FN(shocktroa)

struct BurnDriver BurnDrvshocktroa = {
	"shocktroa", "shocktro", "neogeo", NULL, "1997",
	"Shock Troopers (set 2)\0", NULL, "Saurus", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, 0,
	NULL, shocktroaRomInfo, shocktroaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Shock Troopers - 2nd Squad

static struct BurnRomInfo shocktr2RomDesc[] = {
	{ "246-p1.p1",    0x100000, 0x6d4b7781, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "246-p2.sp2",   0x400000, 0x72ea04c3, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "246-s1.s1",    0x020000, 0x2a360637, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "246-c1.c1",    0x800000, 0x47ac9ec5, 3 | BRF_GRA },           //  3 Sprite data
	{ "246-c2.c2",    0x800000, 0x7bcab64f, 3 | BRF_GRA },           //  4
	{ "246-c3.c3",    0x800000, 0xdb2f73e8, 3 | BRF_GRA },           //  5
	{ "246-c4.c4",    0x800000, 0x5503854e, 3 | BRF_GRA },           //  6
	{ "246-c5.c5",    0x800000, 0x055b3701, 3 | BRF_GRA },           //  7
	{ "246-c6.c6",    0x800000, 0x7e2caae1, 3 | BRF_GRA },           //  8

	{ "246-m1.m1",    0x020000, 0xd0604ad1, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "246-v1.v1",    0x400000, 0x16986fc6, 5 | BRF_SND },           // 10 Sound data
	{ "246-v2.v2",    0x400000, 0xada41e83, 5 | BRF_SND },           // 11
	{ "246-v3.v3",    0x200000, 0xa05ba5db, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(shocktr2, shocktr2, neogeo)
STD_ROM_FN(shocktr2)

struct BurnDriver BurnDrvshocktr2 = {
	"shocktr2", NULL, "neogeo", NULL, "1998",
	"Shock Troopers - 2nd Squad\0", NULL, "Saurus", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, 0,
	NULL, shocktr2RomInfo, shocktr2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Lansquenet 2004 (Shock Troopers - 2nd Squad bootleg)

static struct BurnRomInfo lans2004RomDesc[] = {
	{ "lnsq-p1.bin",  0x200000, 0xb40a879a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "lnsq-p21.bin", 0x200000, 0xecdb2d42, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "lnsq-p22.bin", 0x200000, 0xfac5e2e7, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "lnsq-s1.bin",  0x020000, 0x39e82897, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "lnsq-c1.bin",  0x800000, 0xb83de59f, 3 | BRF_GRA },           //  4 Sprite data
	{ "lnsq-c2.bin",  0x800000, 0xe08969fd, 3 | BRF_GRA },           //  5
	{ "lnsq-c3.bin",  0x800000, 0x013f2cda, 3 | BRF_GRA },           //  6
	{ "lnsq-c4.bin",  0x800000, 0xd8c3a758, 3 | BRF_GRA },           //  7
	{ "lnsq-c5.bin",  0x800000, 0x75500b82, 3 | BRF_GRA },           //  8
	{ "lnsq-c6.bin",  0x800000, 0x670ac13b, 3 | BRF_GRA },           //  9

	{ "246-m1.bin",   0x020000, 0xd0604ad1, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "lnsq-v1.bin",  0x400000, 0x4408ebc3, 5 | BRF_SND },           // 11 Sound data
	{ "lnsq-v2.bin",  0x400000, 0x3d953975, 5 | BRF_SND },           // 12
	{ "lnsq-v3.bin",  0x200000, 0x437d1d8e, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(lans2004, lans2004, neogeo)
STD_ROM_FN(lans2004)

static void lans2004Callback()
{
	INT32 i;
	UINT8 *dst = (UINT8*)BurnMalloc(0x100000);
	if (dst)
	{
		static const INT32 sec[] = { 0x3, 0x8, 0x7, 0xc, 0x1, 0xa, 0x6, 0xd };

		for (i = 0; i < 8; i++)
			memcpy (dst + i * 0x20000, Neo68KROMActive + sec[i] * 0x20000, 0x20000);

		memcpy (dst + 0x0bbb00, Neo68KROMActive + 0x045b00, 0x001710);
		memcpy (dst + 0x02fff0, Neo68KROMActive + 0x1a92be, 0x000010);
		memcpy (Neo68KROMActive, dst, 0x100000);
		BurnFree (dst);
	}

	memmove (Neo68KROMActive + 0x100000, Neo68KROMActive + 0x200000, 0x400000);

	for (i = 0xbbb00; i < 0xbe000; i+=2) {
		if ((BURN_ENDIAN_SWAP_INT16(*((UINT16 *)(Neo68KROMActive + i + 0))) & 0xf2bf) == BURN_ENDIAN_SWAP_INT16(0x42b9) &&
		     *((UINT16 *)(Neo68KROMActive + i + 2)) == 0x0000)
		{
			*((UINT16 *)(Neo68KROMActive + i + 2)) = BURN_ENDIAN_SWAP_INT16(0x000b);
			*((UINT16 *)(Neo68KROMActive + i + 4)) += BURN_ENDIAN_SWAP_INT16(0x6000);
		}
	}

	*((UINT16 *)(Neo68KROMActive + 0x2d15c)) = BURN_ENDIAN_SWAP_INT16(0x000b);
	*((UINT16 *)(Neo68KROMActive + 0x2d15e)) = BURN_ENDIAN_SWAP_INT16(0xbb00);
	*((UINT16 *)(Neo68KROMActive + 0x2d1e4)) = BURN_ENDIAN_SWAP_INT16(0x6002);
	*((UINT16 *)(Neo68KROMActive + 0x2ea7e)) = BURN_ENDIAN_SWAP_INT16(0x6002);
	*((UINT16 *)(Neo68KROMActive + 0xbbcd0)) = BURN_ENDIAN_SWAP_INT16(0x6002);
	*((UINT16 *)(Neo68KROMActive + 0xbbdf2)) = BURN_ENDIAN_SWAP_INT16(0x6002);
	*((UINT16 *)(Neo68KROMActive + 0xbbe42)) = BURN_ENDIAN_SWAP_INT16(0x6002);

	lans2004_sx_decode();
	lans2004_cx_decode(0x800000 * 6);
}

static INT32 lans2004Init()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = lans2004Callback;

 	nRet = NeoInit();

	if (nRet == 0) {
		for (INT32 i = 0; i < 0xa00000; i++)
			YM2610ADPCMAROM[nNeoActiveSlot][i] = BITSWAP08(YM2610ADPCMAROM[nNeoActiveSlot][i], 0, 1, 5, 4, 3, 2, 6, 7);
	}

	return nRet;
}

struct BurnDriver BurnDrvlans2004 = {
	"lans2004", "shocktr2", "neogeo", NULL, "1998",
	"Lansquenet 2004 (Shock Troopers - 2nd Squad bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, 0,
	NULL, lans2004RomInfo, lans2004RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	lans2004Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Galaxy Fight - Universal Warriors

static struct BurnRomInfo galaxyfgRomDesc[] = {
	{ "078-p1.p1",    0x200000, 0x45906309, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC5316200

	{ "078-s1.s1",    0x020000, 0x72f8923e, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "078-c1.c1",    0x200000, 0xc890c7c0, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5316200
	{ "078-c2.c2",    0x200000, 0xb6d25419, 3 | BRF_GRA },           //  3 					/ mask rom TC5316200
	{ "078-c3.c3",    0x200000, 0x9d87e761, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "078-c4.c4",    0x200000, 0x765d7cb8, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200
	{ "078-c5.c5",    0x200000, 0xe6b77e6a, 3 | BRF_GRA },           //  6 					/ mask rom TC5316200
	{ "078-c6.c6",    0x200000, 0xd779a181, 3 | BRF_GRA },           //  7 					/ mask rom TC5316200
	{ "078-c7.c7",    0x100000, 0x4f27d580, 3 | BRF_GRA },           //  8 					/ mask rom TC5316200
	{ "078-c8.c8",    0x100000, 0x0a7cc0d8, 3 | BRF_GRA },           //  9 					/ mask rom TC5316200

	{ "078-m1.m1",    0x020000, 0x8e9e3b10, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ mask rom TC531001

	{ "078-v1.v1",    0x200000, 0xe3b735ac, 5 | BRF_SND },           // 11 Sound data		/ mask rom TC5316200
	{ "078-v2.v2",    0x200000, 0x6a8e78c2, 5 | BRF_SND },           // 12 					/ mask rom TC5316200
	{ "078-v3.v3",    0x100000, 0x70bca656, 5 | BRF_SND },           // 13 					/ mask rom TC538200
};

STDROMPICKEXT(galaxyfg, galaxyfg, neogeo)
STD_ROM_FN(galaxyfg)

struct BurnDriver BurnDrvgalaxyfg = {
	"galaxyfg", NULL, "neogeo", NULL, "1995",
	"Galaxy Fight - Universal Warriors\0", NULL, "Sunsoft", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, galaxyfgRomInfo, galaxyfgRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Waku Waku 7

static struct BurnRomInfo wakuwak7RomDesc[] = {
	{ "225-p1.p1",    0x100000, 0xb14da766, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "225-p2.sp2",   0x200000, 0xfe190665, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "225-s1.s1",    0x020000, 0x71c4b4b5, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "225-c1.c1",    0x400000, 0xee4fea54, 3 | BRF_GRA },           //  3 Sprite data
	{ "225-c2.c2",    0x400000, 0x0c549e2d, 3 | BRF_GRA },           //  4
	{ "225-c3.c3",    0x400000, 0xaf0897c0, 3 | BRF_GRA },           //  5
	{ "225-c4.c4",    0x400000, 0x4c66527a, 3 | BRF_GRA },           //  6
	{ "225-c5.c5",    0x400000, 0x8ecea2b5, 3 | BRF_GRA },           //  7
	{ "225-c6.c6",    0x400000, 0x0eb11a6d, 3 | BRF_GRA },           //  8

	{ "225-m1.m1",    0x020000, 0x0634bba6, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "225-v1.v1",    0x400000, 0x6195c6b4, 5 | BRF_SND },           // 10 Sound data
	{ "225-v2.v2",    0x400000, 0x6159c5fe, 5 | BRF_SND },           // 11
};

STDROMPICKEXT(wakuwak7, wakuwak7, neogeo)
STD_ROM_FN(wakuwak7)

struct BurnDriver BurnDrvwakuwak7 = {
	"wakuwak7", NULL, "neogeo", NULL, "1996",
	"Waku Waku 7\0", NULL, "Sunsoft", "Neo Geo MVS",
	L"Waku Waku 7\0\u308F\u304F\u308F\u304F\uFF17\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, wakuwak7RomInfo, wakuwak7RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Puzzle Bobble / Bust-A-Move (Neo-Geo) (NGM-083)
/* MVS ONLY RELEASE */

static struct BurnRomInfo pbobblenRomDesc[] = {
	/* This set uses CHA and PROG board from Power Spikes II. Six Power Spikes II prom's are replaced with
	Puzzle Bobble prom's. Confirmed on several original carts. Do other layouts also exist? */
	{ "d96-07.ep1",   0x080000, 0x6102ca14, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ 27C240

	{ "d96-04.s1",    0x020000, 0x9caae538, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "068-c1.c1",    0x100000, 0x7f250f76, 3 | BRF_GRA },           //  2 Sprite data		/ unused / mask rom TC538200
	{ "068-c2.c2",    0x100000, 0x20912873, 3 | BRF_GRA },           //  3 					/ unused / mask rom TC538200
	{ "068-c3.c3",    0x100000, 0x4b641ba1, 3 | BRF_GRA },           //  4 					/ unused / mask rom TC538200
	{ "068-c4.c4",    0x100000, 0x35072596, 3 | BRF_GRA },           //  5 					/ unused / mask rom TC538200
	{ "d96-02.c5",    0x080000, 0xe89ad494, 3 | BRF_GRA },           //  6 Sprite data		/ mask rom TC534200
	{ "d96-03.c6",    0x080000, 0x4b42d7eb, 3 | BRF_GRA },           //  7 					/ mask rom TC534200

	{ "d96-06.m1",    0x020000, 0xf424368a, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code			/ mask rom TC531001

	{ "068-v1.v1",    0x100000, 0x2ced86df, 5 | BRF_SND },           //  9 Sound data		/ unused / mask rom TC538200
	{ "068-v2.v2",    0x100000, 0x970851ab, 5 | BRF_SND },           // 10 					/ unused / mask rom TC538200
	{ "d96-01.v3",    0x100000, 0x0840cbc4, 5 | BRF_SND },           // 11					/ mask rom TC538200
	{ "d96-05.v4",    0x080000, 0x0a548948, 5 | BRF_SND },           // 12 					/ mask rom TC534200
};

STDROMPICKEXT(pbobblen, pbobblen, neogeo)
STD_ROM_FN(pbobblen)

struct BurnDriver BurnDrvpbobblen = {
	"pbobblen", NULL, "neogeo", NULL, "1994",
	"Puzzle Bobble / Bust-A-Move (Neo-Geo) (NGM-083)\0", NULL, "Taito", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, pbobblenRomInfo, pbobblenRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};


// Puzzle Bobble / Bust-A-Move (Neo-Geo) (bootleg)

static struct BurnRomInfo pbobblenbRomDesc[] = {
	{ "u7",           0x080000, 0xac1e9ef3, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "us1",          0x020000, 0x9caae538, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "uc5",          0x080000, 0xe89ad494, 3 | BRF_GRA },           //  2 Sprite data
	{ "uc6",          0x080000, 0x4b42d7eb, 3 | BRF_GRA },           //  3

	{ "um1",          0x020000, 0xf424368a, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "u8",           0x100000, 0x0840cbc4, 5 | BRF_SND },           //  5 Sound data
	{ "u9",           0x080000, 0x0a548948, 5 | BRF_SND },           //  6
};

STDROMPICKEXT(pbobblenb, pbobblenb, neogeo)
STD_ROM_FN(pbobblenb)

struct BurnDriver BurnDrvpbobblenb = {
	"pbobblenb", "pbobblen", "neogeo", NULL, "1994",
	"Puzzle Bobble / Bust-A-Move (Neo-Geo) (bootleg)\0", NULL, "Taito", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, pbobblenbRomInfo, pbobblenbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Puzzle Bobble 2 / Bust-A-Move Again (Neo-Geo)
/* MVS ONLY RELEASE */
// ported to Neo-Geo by SNK, original version is on Taito F3

static struct BurnRomInfo pbobbl2nRomDesc[] = {
	{ "248-p1.p1",    0x100000, 0x9d6c0754, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "248-s1.s1",    0x020000, 0x0a3fee41, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "248-c1.c1",    0x400000, 0xd9115327, 3 | BRF_GRA },           //  2 Sprite data
	{ "248-c2.c2",    0x400000, 0x77f9fdac, 3 | BRF_GRA },           //  3
	{ "248-c3.c3",    0x100000, 0x8890bf7c, 3 | BRF_GRA },           //  4
	{ "248-c4.c4",    0x100000, 0x8efead3f, 3 | BRF_GRA },           //  5

	{ "248-m1.m1",    0x020000, 0x883097a9, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "248-v1.v1",    0x400000, 0x57fde1fa, 5 | BRF_SND },           //  7 Sound data
	{ "248-v2.v2",    0x400000, 0x4b966ef3, 5 | BRF_SND },           //  8
};

STDROMPICKEXT(pbobbl2n, pbobbl2n, neogeo)
STD_ROM_FN(pbobbl2n)

struct BurnDriver BurnDrvpbobbl2n = {
	"pbobbl2n", NULL, "neogeo", NULL, "1999",
	"Puzzle Bobble 2 / Bust-A-Move Again (Neo-Geo)\0", NULL, "Taito (SNK license)", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, pbobbl2nRomInfo, pbobbl2nRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Pochi and Nyaa (Ver 2.02)
/* Encrypted Set */ /* MVS ONLY RELEASE */

static struct BurnRomInfo pnyaaRomDesc[] = {
	{ "pn202.p1",     0x100000, 0xbf34e71c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 	/ M27C800

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	{ "267-c1.c1",    0x800000, 0x5eebee65, 3 | BRF_GRA },           //  1 Sprite data 	/ mask rom TC534000
	{ "267-c2.c2",    0x800000, 0x2b67187b, 3 | BRF_GRA },           //  2 				/ mask rom TC534000

	{ "267-m1.m1",    0x080000, 0xc7853ccd, 4 | BRF_ESS | BRF_PRG }, //  3 Z80 code 	/ mask rom TC534000

	{ "267-v1.v1",    0x400000, 0xe2e8e917, 5 | BRF_SND },           //  4 Sound data 	/ mask rom TC5332204
};

STDROMPICKEXT(pnyaa, pnyaa, neogeo)
STD_ROM_FN(pnyaa)

INT32 pnyaaInit()
{
	INT32 nRet;

	nNeoProtectionXor = 0x2E;

	nRet = NeoInit();

	if (nRet == 0) {
		PCM2DecryptV(0x400000, 0);
	}

	return nRet;
}

struct BurnDriver BurnDrvpnyaa = {
	"pnyaa", NULL, "neogeo", NULL, "2003",
	"Pochi and Nyaa (Ver 2.02)\0", NULL, "Aiky / Taito", "Neo Geo MVS",
	L"Pochi and Nyaa\0\u30DD\u30C1\u30C3\u3068\u306B\u3083\uFF5E (Ver 2.02)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_PUZZLE, 0,
	NULL, pnyaaRomInfo, pnyaaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	pnyaaInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Pochi and Nyaa (Ver 2.00)
/* Encrypted Set */ /* MVS ONLY RELEASE */
// this version does not show the web address on the title screen

static struct BurnRomInfo pnyaaaRomDesc[] = {
	{ "267-p1.p1",    0x100000, 0x112fe2c0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 	/ mask rom TC538200

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	{ "267-c1.c1",    0x800000, 0x5eebee65, 3 | BRF_GRA },           //  1 Sprite data 	/ mask rom TC534000
	{ "267-c2.c2",    0x800000, 0x2b67187b, 3 | BRF_GRA },           //  2 				/ mask rom TC534000

	{ "267-m1.m1",    0x080000, 0xc7853ccd, 4 | BRF_ESS | BRF_PRG }, //  3 Z80 code 	/ mask rom TC534000

	{ "267-v1.v1",    0x400000, 0xe2e8e917, 5 | BRF_SND },           //  4 Sound data 	/ mask rom TC5332204
};

STDROMPICKEXT(pnyaaa, pnyaaa, neogeo)
STD_ROM_FN(pnyaaa)

struct BurnDriver BurnDrvpnyaaa = {
	"pnyaaa", "pnyaa", "neogeo", NULL, "2003",
	"Pochi and Nyaa (Ver 2.00)\0", NULL, "Aiky / Taito", "Neo Geo MVS",
	L"Pochi and Nyaa\0\u30DD\u30C1\u30C3\u3068\u306B\u3083\uFF5E (Ver 2.00)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_PUZZLE, 0,
	NULL, pnyaaaRomInfo, pnyaaaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	pnyaaInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Chibi Marukochan Deluxe Quiz

static struct BurnRomInfo marukodqRomDesc[] = {
	{ "206-p1.p1",    0x100000, 0xc33ed21e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC538200

	{ "206-s1.s1",    0x020000, 0xf0b68780, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "206-c1.c1",    0x400000, 0x846e4e8e, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5332205
	{ "206-c2.c2",    0x400000, 0x1cba876d, 3 | BRF_GRA },           //  3 					/ mask rom TC5332205
	{ "206-c3.c3",    0x100000, 0x79aa2b48, 3 | BRF_GRA },           //  4 					/ mask rom TC538200
	{ "206-c4.c4",    0x100000, 0x55e1314d, 3 | BRF_GRA },           //  5 					/ mask rom TC538200

	{ "206-m1.m1",    0x020000, 0x0e22902e, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ mask rom TC531001

	{ "206-v1.v1",    0x200000, 0x5385eca8, 5 | BRF_SND },           //  7 Sound data		/ mask rom TC5316200
	{ "206-v2.v2",    0x200000, 0xf8c55404, 5 | BRF_SND },           //  8 					/ mask rom TC5316200
};

STDROMPICKEXT(marukodq, marukodq, neogeo)
STD_ROM_FN(marukodq)

struct BurnDriver BurnDrvmarukodq = {
	"marukodq", NULL, "neogeo", NULL, "1995",
	"Chibi Marukochan Deluxe Quiz\0", NULL, "Takara", "Neo Geo MVS",
	L"\u3061\u3073\u307E\u308B\u5B50\u3061\u3083\u3093 \u307E\u308B\u5B50\u30C7\u30E9\u30C3\u30AF\u30B9\u30AF\u30A4\u30BA\0Chibi Marukochan Deluxe Quiz\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_QUIZ, 0,
	NULL, marukodqRomInfo, marukodqRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Double Dragon (Neo-Geo)

static struct BurnRomInfo doubledrRomDesc[] = {
	/* also found MVS set with PROG board NEO-MVS PROG 4096 B and CHA board NEO-MVS CHA 42G-3; same chip labels */
	{ "082-p1.p1",    0x200000, 0x34ab832a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "082-s1.s1",    0x020000, 0xbef995c5, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "082-c1.c1",    0x200000, 0xb478c725, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "082-c2.c2",    0x200000, 0x2857da32, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "082-c3.c3",    0x200000, 0x8b0d378e, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "082-c4.c4",    0x200000, 0xc7d2f596, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "082-c5.c5",    0x200000, 0xec87bff6, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "082-c6.c6",    0x200000, 0x844a8a11, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "082-c7.c7",    0x100000, 0x727c4d02, 3 | BRF_GRA },           //  8 					/ TC538200
	{ "082-c8.c8",    0x100000, 0x69a5fa37, 3 | BRF_GRA },           //  9 					/ TC538200

	{ "082-m1.m1",    0x020000, 0x10b144de, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "082-v1.v1",    0x200000, 0xcc1128e4, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "082-v2.v2",    0x200000, 0xc3ff5554, 5 | BRF_SND },           // 12 					/ TC5316200
};

STDROMPICKEXT(doubledr, doubledr, neogeo)
STD_ROM_FN(doubledr)

struct BurnDriver BurnDrvdoubledr = {
	"doubledr", NULL, "neogeo", NULL, "1995",
	"Double Dragon (Neo-Geo)\0", NULL, "Technos", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, doubledrRomInfo, doubledrRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Voltage Fighter - Gowcaizer / Choujin Gakuen Gowcaizer

static struct BurnRomInfo gowcaizrRomDesc[] = {
	{ "094-p1.p1",    0x200000, 0x33019545, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "094-s1.s1",    0x020000, 0x2f8748a2, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "094-c1.c1",    0x200000, 0x042f6af5, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "094-c2.c2",    0x200000, 0x0fbcd046, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "094-c3.c3",    0x200000, 0x58bfbaa1, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "094-c4.c4",    0x200000, 0x9451ee73, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "094-c5.c5",    0x200000, 0xff9cf48c, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "094-c6.c6",    0x200000, 0x31bbd918, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "094-c7.c7",    0x200000, 0x2091ec04, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "094-c8.c8",    0x200000, 0xd80dd241, 3 | BRF_GRA },           //  9 					/ TC5316200

	{ "094-m1.m1",    0x020000, 0x78c851cb, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "094-v1.v1",    0x200000, 0x6c31223c, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "094-v2.v2",    0x200000, 0x8edb776c, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "094-v3.v3",    0x100000, 0xc63b9285, 5 | BRF_SND },           // 13 					/ TC538200
};

STDROMPICKEXT(gowcaizr, gowcaizr, neogeo)
STD_ROM_FN(gowcaizr)

struct BurnDriver BurnDrvgowcaizr = {
	"gowcaizr", NULL, "neogeo", NULL, "1995",
	"Voltage Fighter - Gowcaizer / Choujin Gakuen Gowcaizer\0", NULL, "Technos", "Neo Geo MVS",
	L"Voltage Fighter - Gowcaizer\0\u8D85\u4EBA\u5B66\u5712\u30B4\u30A6\u30AB\u30A4\u30B6\u30FC\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, gowcaizrRomInfo, gowcaizrRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Super Dodge Ball / Kunio no Nekketsu Toukyuu Densetsu
/* MVS ONLY RELEASE */

static struct BurnRomInfo sdodgebRomDesc[] = {
	{ "208-p1.p1",    0x200000, 0x127f3d32, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "208-s1.s1",    0x020000, 0x64abd6b3, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "208-c1.c1",    0x400000, 0x93d8619b, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332205
	{ "208-c2.c2",    0x400000, 0x1c737bb6, 3 | BRF_GRA },           //  3 					/ TC5332205
	{ "208-c3.c3",    0x200000, 0x14cb1703, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "208-c4.c4",    0x200000, 0xc7165f19, 3 | BRF_GRA },           //  5 					/ TC5332205

	{ "208-m1.m1",    0x020000, 0x0a5f3325, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "208-v1.v1",    0x400000, 0xe7899a24, 5 | BRF_SND },           //  7 Sound data		/ TC5332204
};

STDROMPICKEXT(sdodgeb, sdodgeb, neogeo)
STD_ROM_FN(sdodgeb)

struct BurnDriver BurnDrvsdodgeb = {
	"sdodgeb", NULL, "neogeo", NULL, "1996",
	"Super Dodge Ball / Kunio no Nekketsu Toukyuu Densetsu\0", NULL, "Technos", "Neo Geo MVS",
	L"Super Dodge Ball\0\u304F\u306B\u304A\u306E\u71B1\u8840\u95D8\u7403\u4F1D\u8AAC\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_SPORTSMISC, 0,
	NULL, sdodgebRomInfo, sdodgebRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Dragon's Heaven (development board)
// same ID code as Voltage Fighter Gowkaizer, developed by ex-Technos staff

static struct BurnRomInfo dragonshRomDesc[] = {
	{ "ep1.bin",      0x080000,  0xf353448c, 1 | BRF_ESS | BRF_PRG }, 	 //  0 68K code
	{ "ep2.bin",      0x080000,  0xf25c71ad, 1 | BRF_ESS | BRF_PRG }, 	 //  1

	{ "s1.s1",    	  0x020000,  0x706477a7, 2 | BRF_GRA },           	 //  2 Text layer tiles

	{ "no3.bin",      0x1000000, 0x81821826, 3 | BRF_GRA },           	 //  3 Sprite data
	{ "no4.bin",      0x1000000, 0x3601d568, 3 | BRF_GRA },           	 //  4

	{ "sm1.sm1",      0x020000,  0x94416d67, 4 | BRF_ESS | BRF_PRG }, 	 //  5 Z80 code

	{ "sram.v1",      0x200000,  0x00000000, 5 | BRF_SND | BRF_NODUMP }, //  6 Sound data
};

STDROMPICKEXT(dragonsh, dragonsh, neogeo)
STD_ROM_FN(dragonsh)

static void DragonshCallback()
{
	BurnLoadRom(Neo68KROMActive + 0x000000, 0, 2);
	BurnLoadRom(Neo68KROMActive + 0x000001, 1, 2);
}

static INT32 DragonshInit()
{
	NeoCallbackActive->pInitialise = DragonshCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvdragonsh = {
	"dragonsh", NULL, "neogeo", NULL, "1996",
	"Dragon's Heaven (development board)\0", NULL, "Face", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_SPORTSMISC, 0,
	NULL, dragonshRomInfo, dragonshRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	DragonshInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Tecmo World Soccer '96
/* MVS ONLY RELEASE */

static struct BurnRomInfo twsoc96RomDesc[] = {
	{ "086-p1.p1",    0x100000, 0x03e20ab6, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC538200

	{ "086-s1.s1",    0x020000, 0x6f5e2b3a, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "086-c1.c1",    0x400000, 0x2611bc2a, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5332205
	{ "086-c2.c2",    0x400000, 0x6b0d6827, 3 | BRF_GRA },           //  3 					/ mask rom TC5332205
	{ "086-c3.c3",    0x100000, 0x750ddc0c, 3 | BRF_GRA },           //  4 					/ mask rom TC538200
	{ "086-c4.c4",    0x100000, 0x7a6e7d82, 3 | BRF_GRA },           //  5 					/ mask rom TC538200

	{ "086-m1.m1",    0x020000, 0xcb82bc5d, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ mask rom TC531001

	{ "086-v1.v1",    0x200000, 0x97bf1986, 5 | BRF_SND },           //  7 Sound data		/ mask rom TC5316200
	{ "086-v2.v2",    0x200000, 0xb7eb05df, 5 | BRF_SND },           //  8 					/ mask rom TC5316200
};

STDROMPICKEXT(twsoc96, twsoc96, neogeo)
STD_ROM_FN(twsoc96)

struct BurnDriver BurnDrvtwsoc96 = {
	"twsoc96", NULL, "neogeo", NULL, "1996",
	"Tecmo World Soccer '96\0", NULL, "Tecmo", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSFOOTBALL, 0,
	NULL, twsoc96RomInfo, twsoc96RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Fight Fever (set 1)
/* MVS ONLY RELEASE */

static struct BurnRomInfo fightfevRomDesc[] = {
	{ "060-p1.p1",    0x100000, 0x2a104b50, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "060-s1.s1",    0x020000, 0xd62a72e9, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "060-c1.c1",    0x200000, 0x8908fff9, 3 | BRF_GRA },           //  2 Sprite data
	{ "060-c2.c2",    0x200000, 0xc6649492, 3 | BRF_GRA },           //  3
	{ "060-c3.c3",    0x200000, 0x0956b437, 3 | BRF_GRA },           //  4
	{ "060-c4.c4",    0x200000, 0x026f3b62, 3 | BRF_GRA },           //  5

	{ "060-m1.m1",    0x020000, 0x0b7c4e65, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "060-v1.v1",    0x200000, 0xf417c215, 5 | BRF_SND },           //  7 Sound data
	{ "060-v2.v2",    0x100000, 0xefcff7cf, 5 | BRF_SND },           //  8
};

STDROMPICKEXT(fightfev, fightfev, neogeo)
STD_ROM_FN(fightfev)

struct BurnDriver BurnDrvfightfev = {
	"fightfev", NULL, "neogeo", NULL, "1994",
	"Fight Fever (set 1)\0", NULL, "Viccom", "Neo Geo MVS",
	L"Fight Fever\0\uC655\uC911\uC655 (set 1)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, fightfevRomInfo, fightfevRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Fight Fever (set 2)
/* MVS ONLY RELEASE */

static struct BurnRomInfo fightfevaRomDesc[] = {
	{ "060-p1.p1",    0x100000, 0x2a104b50, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ VIC940800-101
	/* also found sets with p1/ p2 4mbit on eprom with Viccom sticker labels on board NEO-MVS PROGGSC
	{ "viccom_1.ep1",    0x080000, 0x00000000, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ M27C4002
	{ "viccom_2.ep2",    0x080000, 0x00000000, 1 | BRF_ESS | BRF_PRG }, //  1 				/ M27C4002 */

	{ "060-s1.s1",    0x020000, 0xd62a72e9, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "060-c1.c1",    0x200000, 0x8908fff9, 3 | BRF_GRA },           //  2 Sprite data 		/ VIC931600
	{ "060-c2.c2",    0x200000, 0xc6649492, 3 | BRF_GRA },           //  3 					/ VIC931600
	{ "060-c3.c3",    0x200000, 0x0956b437, 3 | BRF_GRA },           //  4 					/ VIC931600
	{ "060-c4.c4",    0x200000, 0x026f3b62, 3 | BRF_GRA },           //  5 					/ VIC931600

	{ "060-m1.m1",    0x020000, 0x0b7c4e65, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ VIC930100

	{ "060-v1.v1",    0x200000, 0xf417c215, 5 | BRF_SND },           //  7 Sound data 		/ VIC930800
	{ "060-v2.v2",    0x100000, 0xefcff7cf, 5 | BRF_SND },           //  8 					/ VIC931600

	{ "1.sp2",  	  0x080000, 0x3032041b, 0 | BRF_ESS | BRF_PRG }, //  9 68K code 		/ TC574200
	/* P is on eprom, has a Viccom logo at the top of the label with a circled '1' in the center */
};

STDROMPICKEXT(fightfeva, fightfeva, neogeo)
STD_ROM_FN(fightfeva)

static void fightfevaCallback()
{
	BurnLoadRom(Neo68KROMActive, 9, 1);
}

static INT32 fightfevaInit()
{
	NeoCallbackActive->pInitialise = fightfevaCallback;

 	return NeoInit();
}

struct BurnDriver BurnDrvfightfeva = {
	"fightfeva", "fightfev", "neogeo", NULL, "1994",
	"Fight Fever (set 2)\0", NULL, "Viccom", "Neo Geo MVS",
	L"Fight Fever\0\uC655\uC911\uC655 (set 2)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, fightfevaRomInfo, fightfevaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	fightfevaInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Power Spikes II (NGM-068)
/* MVS ONLY RELEASE */

static struct BurnRomInfo pspikes2RomDesc[] = {
	{ "068-pg1.p1",   0x100000, 0x105a408f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC538200

	{ "068-sg1.s1",   0x020000, 0x18082299, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "068-c1.c1",    0x100000, 0x7f250f76, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC538200
	{ "068-c2.c2",    0x100000, 0x20912873, 3 | BRF_GRA },           //  3 					/ mask rom TC538200
	{ "068-c3.c3",    0x100000, 0x4b641ba1, 3 | BRF_GRA },           //  4 					/ mask rom TC538200
	{ "068-c4.c4",    0x100000, 0x35072596, 3 | BRF_GRA },           //  5 					/ mask rom TC538200
	{ "068-c5.c5",    0x100000, 0x151dd624, 3 | BRF_GRA },           //  6 					/ mask rom TC538200
	{ "068-c6.c6",    0x100000, 0xa6722604, 3 | BRF_GRA },           //  7 					/ mask rom TC538200

	{ "068-mg1.m1",   0x020000, 0xb1c7911e, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code			/ mask rom TC531001

	{ "068-v1.v1",    0x100000, 0x2ced86df, 5 | BRF_SND },           //  9 Sound data		/ mask rom TC538200
	{ "068-v2.v2",    0x100000, 0x970851ab, 5 | BRF_SND },           // 10 					/ mask rom TC538200
	{ "068-v3.v3",    0x100000, 0x81ff05aa, 5 | BRF_SND },           // 11 					/ mask rom TC538200
};

STDROMPICKEXT(pspikes2, pspikes2, neogeo)
STD_ROM_FN(pspikes2)

struct BurnDriver BurnDrvpspikes2 = {
	"pspikes2", NULL, "neogeo", NULL, "1994",
	"Power Spikes II (NGM-068)\0", NULL, "Video System Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, pspikes2RomInfo, pspikes2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Aero Fighters 2 / Sonic Wings 2

static struct BurnRomInfo sonicwi2RomDesc[] = {
	{ "075-p1.p1",    0x200000, 0x92871738, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC5316200

	{ "075-s1.s1",    0x020000, 0xc9eec367, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "075-c1.c1",    0x200000, 0x3278e73e, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5316200
	{ "075-c2.c2",    0x200000, 0xfe6355d6, 3 | BRF_GRA },           //  3 					/ mask rom TC5316200
	{ "075-c3.c3",    0x200000, 0xc1b438f1, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "075-c4.c4",    0x200000, 0x1f777206, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200

	{ "075-m1.m1",    0x020000, 0xbb828df1, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ mask rom TC531001

	{ "075-v1.v1",    0x200000, 0x7577e949, 5 | BRF_SND },           //  7 Sound data		/ mask rom TC5316200
	{ "075-v2.v2",    0x100000, 0x021760cd, 5 | BRF_SND },           //  8 					/ mask rom TC538200
};

STDROMPICKEXT(sonicwi2, sonicwi2, neogeo)
STD_ROM_FN(sonicwi2)

struct BurnDriver BurnDrvsonicwi2 = {
	"sonicwi2", NULL, "neogeo", NULL, "1994",
	"Aero Fighters 2 / Sonic Wings 2\0", NULL, "Video System Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VERSHOOT, FBF_SONICWI,
	NULL, sonicwi2RomInfo, sonicwi2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Aero Fighters 3 / Sonic Wings 3

static struct BurnRomInfo sonicwi3RomDesc[] = {
	{ "097-p1.p1",    0x200000, 0x0547121d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "097-s1.s1",    0x020000, 0x8dd66743, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "097-c1.c1",    0x400000, 0x33d0d589, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332205
	{ "097-c2.c2",    0x400000, 0x186f8b43, 3 | BRF_GRA },           //  3 					/ TC5332205
	{ "097-c3.c3",    0x200000, 0xc339fff5, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "097-c4.c4",    0x200000, 0x84a40c6e, 3 | BRF_GRA },           //  5 					/ TC5316200

	{ "097-m1.m1",    0x020000, 0xb20e4291, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "097-v1.v1",    0x400000, 0x6f885152, 5 | BRF_SND },           //  7 Sound data		/ TC5332201
	{ "097-v2.v2",    0x200000, 0x3359e868, 5 | BRF_SND },           //  8 					/ TC5316200
};

STDROMPICKEXT(sonicwi3, sonicwi3, neogeo)
STD_ROM_FN(sonicwi3)

struct BurnDriver BurnDrvsonicwi3 = {
	"sonicwi3", NULL, "neogeo", NULL, "1995",
	"Aero Fighters 3 / Sonic Wings 3\0", NULL, "Video System Co.", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VERSHOOT, FBF_SONICWI,
	NULL, sonicwi3RomInfo, sonicwi3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Pop 'n Bounce / Gapporin
/* MVS ONLY RELEASE */

static struct BurnRomInfo popbouncRomDesc[] = {
	{ "237-p1.p1",    0x100000, 0xbe96e44f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "237-s1.s1",    0x020000, 0xb61cf595, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "237-c1.c1",    0x200000, 0xeda42d66, 3 | BRF_GRA },           //  2 Sprite data
	{ "237-c2.c2",    0x200000, 0x5e633c65, 3 | BRF_GRA },           //  3

	{ "237-m1.m1",    0x020000, 0xd4c946dd, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "237-v1.v1",    0x200000, 0xedcb1beb, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(popbounc, popbounc, neogeo)
STD_ROM_FN(popbounc)

struct BurnDriver BurnDrvpopbounc = {
	"popbounc", NULL, "neogeo", NULL, "1997",
	"Pop 'n Bounce / Gapporin\0", NULL, "Video System Co.", "Neo Geo MVS",
	L"Pop 'n Bounce\0\u30AC\u30C3\u30DD\u30EA\u30F3\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_PADDLE, GBF_PUZZLE, 0,
	NULL, popbouncRomInfo, popbouncRomName, NULL, NULL, NULL, NULL, neopaddleInputInfo, neopaddleDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Andro Dunos (NGM-049)(NGH-049)
/* MVS AND AES VERSION */

static struct BurnRomInfo androdunRomDesc[] = {
	{ "049-p1.p1",    0x080000, 0x3b857da2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ CXK384500
	{ "049-p2.p2",    0x080000, 0x2f062209, 1 | BRF_ESS | BRF_PRG }, //  1 					/ CXK384500

	{ "049-s1.s1",    0x020000, 0x6349de5d, 2 | BRF_GRA },           //  2 Text layer tiles / CXK381000

	{ "049-c1.c1",    0x100000, 0x7ace6db3, 3 | BRF_GRA },           //  3 Sprite data 		/ CXK388000
	{ "049-c2.c2",    0x100000, 0xb17024f7, 3 | BRF_GRA },           //  4 					/ CXK388000

	{ "049-m1.m1",    0x020000, 0xedd2acf4, 4 | BRF_ESS | BRF_PRG }, //  5 Z80 code 		/ CXK381003

	{ "049-v1.v1",    0x100000, 0xce43cb89, 5 | BRF_SND },           //  6 Sound data 		/ CXK388000
};

STDROMPICKEXT(androdun, androdun, neogeo)
STD_ROM_FN(androdun)

struct BurnDriver BurnDrvandrodun = {
	"androdun", NULL, "neogeo", NULL, "1992",
	"Andro Dunos (NGM-049)(NGH-049)\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_HORSHOOT, 0,
	NULL, androdunRomInfo, androdunRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Bang Bang Busters (2010 NCI release)

static struct BurnRomInfo b2bRomDesc[] = {
	{ "071.p1",    	   0x080000, 0x7687197d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "071.s1",    	   0x020000, 0x44e5f154, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "071.c1",    	   0x200000, 0x23d84a7a, 3 | BRF_GRA },           //  2 Sprite data
	{ "071.c2",        0x200000, 0xce7b6248, 3 | BRF_GRA },           //  3

	{ "071.m1",    	   0x020000, 0x6da739ad, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "071.v1",    	   0x100000, 0x50feffb0, 5 | BRF_SND },           //  7 Sound data

};

STDROMPICKEXT(b2b, b2b, neogeo)
STD_ROM_FN(b2b)

struct BurnDriver BurnDrvb2b = {
	"b2b", NULL, "neogeo", NULL, "2000",
	"Bang Bang Busters (2010 NCI release)\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, b2bRomInfo, b2bRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Puzzle De Pon!
/* MVS ONLY RELEASE */
// game concept licensed from Taito

static struct BurnRomInfo puzzledpRomDesc[] = {
	{ "202-p1.p1",    0x080000, 0x2b61415b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC534200

	{ "202-s1.s1",    0x020000, 0xcd19264f, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "202-c1.c1",    0x100000, 0xcc0095ef, 3 | BRF_GRA },           //  2 Sprite data		/ TC538200
	{ "202-c2.c2",    0x100000, 0x42371307, 3 | BRF_GRA },           //  3 					/ TC538200

	{ "202-m1.m1",    0x020000, 0x9c0291ea, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code			/ TC531001

	{ "202-v1.v1",    0x080000, 0xdebeb8fb, 5 | BRF_SND },           //  5 Sound data		/ TC534200
};

STDROMPICKEXT(puzzledp, puzzledp, neogeo)
STD_ROM_FN(puzzledp)

struct BurnDriver BurnDrvpuzzledp = {
	"puzzledp", NULL, "neogeo", NULL, "1995",
	"Puzzle De Pon!\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, puzzledpRomInfo, puzzledpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo Mr. Do!
/* MVS ONLY RELEASE */
// game concept licensed from Universal

static struct BurnRomInfo neomrdoRomDesc[] = {
	/* also found MVS set with PROG Board NEO-MVS PROG 4096 and CHA board NEO-MVS CHA42G-3B; same chip labels */
	{ "207-p1.p1",    0x100000, 0x334ea51e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200

	{ "207-s1.s1",    0x020000, 0x6aebafce, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "207-c1.c1",    0x200000, 0xc7541b9d, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "207-c2.c2",    0x200000, 0xf57166d2, 3 | BRF_GRA },           //  3 					/ TC5316200

	{ "207-m1.m1",    0x020000, 0xb5b74a95, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code			/ TC531001

	{ "207-v1.v1",    0x200000, 0x4143c052, 5 | BRF_SND },           //  5 Sound data		/ TC5316200
};

STDROMPICKEXT(neomrdo, neomrdo, neogeo)
STD_ROM_FN(neomrdo)

struct BurnDriver BurnDrvneomrdo = {
	"neomrdo", NULL, "neogeo", NULL, "1996",
	"Neo Mr. Do!\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_MAZE, 0,
	NULL, neomrdoRomInfo, neomrdoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Goal! Goal! Goal!
/* MVS ONLY RELEASE */

static struct BurnRomInfo goalx3RomDesc[] = {
	{ "209-p1.p1",    0x200000, 0x2a019a79, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC5316200

	{ "209-s1.s1",    0x020000, 0xc0eaad86, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "209-c1.c1",    0x400000, 0xb49d980e, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5332205
	{ "209-c2.c2",    0x400000, 0x5649b015, 3 | BRF_GRA },           //  3 					/ mask rom TC5332205
	{ "209-c3.c3",    0x100000, 0x5f91bace, 3 | BRF_GRA },           //  4 					/ mask rom TC538200
	{ "209-c4.c4",    0x100000, 0x1e9f76f2, 3 | BRF_GRA },           //  5 					/ mask rom TC538200

	{ "209-m1.m1",    0x020000, 0xcd758325, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ mask rom TC531001

	{ "209-v1.v1",    0x200000, 0xef214212, 5 | BRF_SND },           //  7 Sound data		/ mask rom TC5316200
};

STDROMPICKEXT(goalx3, goalx3, neogeo)
STD_ROM_FN(goalx3)

struct BurnDriver BurnDrvgoalx3 = {
	"goalx3", NULL, "neogeo", NULL, "1995",
	"Goal! Goal! Goal!\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_SPORTSFOOTBALL, 0,
	NULL, goalx3RomInfo, goalx3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo Drift Out - New Technology
/* MVS ONLY RELEASE */

static struct BurnRomInfo neodriftRomDesc[] = {
	{ "213-p1.p1",    0x200000, 0xe397d798, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC5316200

	{ "213-s1.s1",    0x020000, 0xb76b61bc, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "213-c1.c1",    0x400000, 0x3edc8bd3, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5316200
	{ "213-c2.c2",    0x400000, 0x46ae5f16, 3 | BRF_GRA },           //  3 					/ mask rom TC5316200

	{ "213-m1.m1",    0x020000, 0x200045f1, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code			/ mask rom TC531001

	{ "213-v1.v1",    0x200000, 0xa421c076, 5 | BRF_SND },           //  5 Sound data		/ mask rom TC5332205
	{ "213-v2.v2",    0x200000, 0x233c7dd9, 5 | BRF_SND },           //  6 					/ mask rom TC5332205
};

STDROMPICKEXT(neodrift, neodrift, neogeo)
STD_ROM_FN(neodrift)

struct BurnDriver BurnDrvneodrift = {
	"neodrift", NULL, "neogeo", NULL, "1996",
	"Neo Drift Out - New Technology\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_RACING, 0,
	NULL, neodriftRomInfo, neodriftRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Breakers

static struct BurnRomInfo breakersRomDesc[] = {
	{ "230-p1.p1",    0x200000, 0xed24a6e6, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "230-s1.s1",    0x020000, 0x076fb64c, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "230-c1.c1",    0x400000, 0x68d4ae76, 3 | BRF_GRA },           //  2 Sprite data
	{ "230-c2.c2",    0x400000, 0xfdee05cd, 3 | BRF_GRA },           //  3
	{ "230-c3.c3",    0x400000, 0x645077f3, 3 | BRF_GRA },           //  4
	{ "230-c4.c4",    0x400000, 0x63aeb74c, 3 | BRF_GRA },           //  5

	{ "230-m1.m1",    0x020000, 0x3951a1c1, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "230-v1.v1",    0x400000, 0x7f9ed279, 5 | BRF_SND },           //  7 Sound data
	{ "230-v2.v2",    0x400000, 0x1d43e420, 5 | BRF_SND },           //  8
};

STDROMPICKEXT(breakers, breakers, neogeo)
STD_ROM_FN(breakers)

struct BurnDriver BurnDrvbreakers = {
	"breakers", NULL, "neogeo", NULL, "1996",
	"Breakers\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, breakersRomInfo, breakersRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Puzzle De Pon! R!
/* MVS ONLY RELEASE */
// game concept licensed from Taito

static struct BurnRomInfo puzzldprRomDesc[] = {
	{ "235-p1.p1",    0x080000, 0xafed5de2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "235-s1.s1",    0x020000, 0x3b13a22f, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "202-c1.c1",    0x100000, 0xcc0095ef, 3 | BRF_GRA },           //  2 Sprite data
	{ "202-c2.c2",    0x100000, 0x42371307, 3 | BRF_GRA },           //  3

	{ "202-m1.m1",    0x020000, 0x9c0291ea, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "202-v1.v1",    0x080000, 0xdebeb8fb, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(puzzldpr, puzzldpr, neogeo)
STD_ROM_FN(puzzldpr)

struct BurnDriver BurnDrvpuzzldpr = {
	"puzzldpr", "puzzledp", "neogeo", NULL, "1997",
	"Puzzle De Pon! R!\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, puzzldprRomInfo, puzzldprRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Breakers Revenge
/* MVS ONLY RELEASE */

static struct BurnRomInfo breakrevRomDesc[] = {
	{ "245-p1.p1",    0x200000, 0xc828876d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "245-s1.s1",    0x020000, 0xe7660a5d, 2 | BRF_GRA },           //  1 Text layer tiles

	/* The chip labels and sizes are correct */
	{ "245-c1.c1",    0x400000, 0x68d4ae76, 3 | BRF_GRA },           //  2 Sprite data
	{ "245-c2.c2",    0x400000, 0xfdee05cd, 3 | BRF_GRA },           //  3
	{ "245-c3.c3",    0x400000, 0x645077f3, 3 | BRF_GRA },           //  4
	{ "245-c4.c4",    0x400000, 0x63aeb74c, 3 | BRF_GRA },           //  5
	{ "245-c5.c5",    0x400000, 0xb5f40e7f, 3 | BRF_GRA },           //  6
	{ "245-c6.c6",    0x400000, 0xd0337328, 3 | BRF_GRA },           //  7

	{ "245-m1.m1",    0x020000, 0x00f31c66, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	{ "245-v1.v1",    0x400000, 0xe255446c, 5 | BRF_SND },           //  9 Sound data
	{ "245-v2.v2",    0x400000, 0x9068198a, 5 | BRF_SND },           // 10
};

STDROMPICKEXT(breakrev, breakrev, neogeo)
STD_ROM_FN(breakrev)

struct BurnDriver BurnDrvbreakrev = {
	"breakrev", NULL, "neogeo", NULL, "1998",
	"Breakers Revenge\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, breakrevRomInfo, breakrevRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Battle Flip Shot
/* MVS ONLY RELEASE */

static struct BurnRomInfo flipshotRomDesc[] = {
	{ "247-p1.p1",    0x100000, 0x95779094, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "247-s1.s1",    0x020000, 0x6300185c, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "247-c1.c1",    0x200000, 0xc9eedcb2, 3 | BRF_GRA },           //  2 Sprite data
	{ "247-c2.c2",    0x200000, 0x7d6d6e87, 3 | BRF_GRA },           //  3

	{ "247-m1.m1",    0x020000, 0xa9fe0144, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "247-v1.v1",    0x200000, 0x42ec743d, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(flipshot, flipshot, neogeo)
STD_ROM_FN(flipshot)

struct BurnDriver BurnDrvflipshot = {
	"flipshot", NULL, "neogeo", NULL, "1998",
	"Battle Flip Shot\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_BALLPADDLE, 0,
	NULL, flipshotRomInfo, flipshotRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Captain Tomaday
/* MVS ONLY RELEASE */

static struct BurnRomInfo ctomadayRomDesc[] = {
	{ "249-p1.p1",    0x200000, 0xc9386118, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "249-s1.s1",    0x020000, 0xdc9eb372, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "249-c1.c1",    0x400000, 0x041fb8ee, 3 | BRF_GRA },           //  2 Sprite data
	{ "249-c2.c2",    0x400000, 0x74f3cdf4, 3 | BRF_GRA },           //  3

	{ "249-m1.m1",    0x020000, 0x80328a47, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "249-v1.v1",    0x400000, 0xde7c8f27, 5 | BRF_SND },           //  5 Sound data
	{ "249-v2.v2",    0x100000, 0xc8e40119, 5 | BRF_SND },           //  6
};

STDROMPICKEXT(ctomaday, ctomaday, neogeo)
STD_ROM_FN(ctomaday)

struct BurnDriver BurnDrvctomaday = {
	"ctomaday", NULL, "neogeo", NULL, "1999",
	"Captain Tomaday\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VERSHOOT, 0,
	NULL, ctomadayRomInfo, ctomadayRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Ganryu / Musashi Ganryuki
/* Original Version - Encrypted GFX */ /* MVS ONLY RELEASE */

static struct BurnRomInfo ganryuRomDesc[] = {
	{ "252-p1.p1",    0x200000, 0x4b8ac4fb, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	{ "252-c1.c1",    0x800000, 0x50ee7882, 3 | BRF_GRA },           //  1 Sprite data
	{ "252-c2.c2",    0x800000, 0x62585474, 3 | BRF_GRA },           //  2

	{ "252-m1.m1",    0x020000, 0x30cc4099, 4 | BRF_ESS | BRF_PRG }, //  3 Z80 code

	{ "252-v1.v1",    0x400000, 0xe5946733, 5 | BRF_SND },           //  4 Sound data
};

STDROMPICKEXT(ganryu, ganryu, neogeo)
STD_ROM_FN(ganryu)

static INT32 ganryuInit()
{
	nNeoProtectionXor = 0x07;
	return NeoInit();
}

struct BurnDriver BurnDrvganryu = {
	"ganryu", NULL, "neogeo", NULL, "1999",
	"Ganryu / Musashi Ganryuki\0", NULL, "Visco", "Neo Geo MVS",
	L"Ganryu\0\u6B66\u8535\u5DCC\u6D41\u8A18\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP | HARDWARE_SNK_CMC42, GBF_PLATFORM, 0,
	NULL, ganryuRomInfo, ganryuRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	ganryuInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Bang Bead
/* Original Version - Encrypted GFX */ /* MVS ONLY RELEASE */

static struct BurnRomInfo bangbeadRomDesc[] = {
	{ "259-p1.p1",    0x200000, 0x88a37f8b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	{ "259-c1.c1",    0x800000, 0x1f537f74, 3 | BRF_GRA },           //  1 Sprite data
	{ "259-c2.c2",    0x800000, 0x0efd98ff, 3 | BRF_GRA },           //  2

	{ "259-m1.m1",    0x020000, 0x85668ee9, 4 | BRF_ESS | BRF_PRG }, //  3 Z80 code

	{ "259-v1.v1",    0x400000, 0x088eb8ab, 5 | BRF_SND },           //  4 Sound data
	{ "259-v2.v2",    0x100000, 0x97528fe9, 5 | BRF_SND },           //  5
};

STDROMPICKEXT(bangbead, bangbead, neogeo)
STD_ROM_FN(bangbead)

static INT32 bangbeadInit()
{
	nNeoProtectionXor = 0xF8;
	return NeoInit();
}

struct BurnDriver BurnDrvbangbead = {
	"bangbead", NULL, "neogeo", NULL, "2000",
	"Bang Bead\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP | HARDWARE_SNK_CMC42, GBF_BALLPADDLE, 0,
	NULL, bangbeadRomInfo, bangbeadRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	bangbeadInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 4 (NGM-2630)
/* Original Version - Encrypted GFX */ /* MVS VERSION */

static struct BurnRomInfo mslug4RomDesc[] = {
	/* There also exist carts where p1 label is pg1; the PG1 revision has a Japanese cart label, SN 02Jxxxxx
	The P1 revision has a US/EUR cart label, SN 02Txxxxx ; Rom data on both is identical.
	These carts were manufactured by Mega Enterprise, not SNK. */
	{ "263-p1.p1",    0x100000, 0x27e4def3, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "263-p2.sp2",   0x400000, 0xfdb7aed8, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "263-c1.c1",    0x800000, 0x84865f8a, 3 | BRF_GRA },           //  2 Sprite data
	{ "263-c2.c2",    0x800000, 0x81df97f2, 3 | BRF_GRA },           //  3
	{ "263-c3.c3",    0x800000, 0x1a343323, 3 | BRF_GRA },           //  4
	{ "263-c4.c4",    0x800000, 0x942cfb44, 3 | BRF_GRA },           //  5
	{ "263-c5.c5",    0x800000, 0xa748854f, 3 | BRF_GRA },           //  6
	{ "263-c6.c6",    0x800000, 0x5c8ba116, 3 | BRF_GRA },           //  7

	/* Encrypted */
	{ "263-m1.m1",    0x020000, 0x46ac8228, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	/* Encrypted */
	{ "263-v1.v1",    0x800000, 0x01e9b9cd, 5 | BRF_SND },           //  9 Sound data
	{ "263-v2.v2",    0x800000, 0x4ab2bf81, 5 | BRF_SND },           // 10
};

STDROMPICKEXT(mslug4, mslug4, neogeo)
STD_ROM_FN(mslug4)

static INT32 mslug4Init()
{
	INT32 nRet;

//	nNeoTextROMSize = 0x080000;
	nNeoProtectionXor = 0x31;

	nRet = NeoInit();

	if (nRet == 0) {
		PCM2DecryptV(0x1000000, 1);
	}

	return nRet;
}

struct BurnDriver BurnDrvmslug4 = {
	"mslug4", NULL, "neogeo", NULL, "2002",
	"Metal Slug 4 (NGM-2630)\0", NULL, "Mega / Noise Factory / Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug4RomInfo, mslug4RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug4Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Metal Slug 4 (NGH-2630)
/* Original Version - Encrypted GFX */ /* AES VERSION */

static struct BurnRomInfo mslug4hRomDesc[] = {
	{ "263-ph1.p1",   0x100000, 0xc67f5c8d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "263-ph2.sp2",  0x400000, 0xbc3ec89e, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "263-c1.c1",    0x800000, 0x84865f8a, 3 | BRF_GRA },           //  2 Sprite data
	{ "263-c2.c2",    0x800000, 0x81df97f2, 3 | BRF_GRA },           //  3
	{ "263-c3.c3",    0x800000, 0x1a343323, 3 | BRF_GRA },           //  4
	{ "263-c4.c4",    0x800000, 0x942cfb44, 3 | BRF_GRA },           //  5
	{ "263-c5.c5",    0x800000, 0xa748854f, 3 | BRF_GRA },           //  6
	{ "263-c6.c6",    0x800000, 0x5c8ba116, 3 | BRF_GRA },           //  7

	/* Encrypted */
	{ "263-m1.m1",    0x020000, 0x46ac8228, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	/* Encrypted */
	{ "263-v1.v1",    0x800000, 0x01e9b9cd, 5 | BRF_SND },           //  9 Sound data
	{ "263-v2.v2",    0x800000, 0x4ab2bf81, 5 | BRF_SND },           // 10
};

STDROMPICKEXT(mslug4h, mslug4h, neogeo)
STD_ROM_FN(mslug4h)

struct BurnDriver BurnDrvmslug4h = {
	"mslug4h", "mslug4", "neogeo", NULL, "2002",
	"Metal Slug 4 (NGH-2630)\0", NULL, "Mega / Noise Factory / Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug4hRomInfo, mslug4hRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug4Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Metal Slug 4 Plus (bootleg)

static struct BurnRomInfo ms4plusRomDesc[] = {
	{ "ms4-p1p.bin",  0x100000, 0x806a6e04, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "263-p2.sp2",   0x400000, 0xfdb7aed8, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "ms4-s1p.bin",  0x020000, 0x07ff87ce, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "263-c1.c1",    0x800000, 0x84865f8a, 3 | BRF_GRA },           //  3 Sprite data
	{ "263-c2.c2",    0x800000, 0x81df97f2, 3 | BRF_GRA },           //  4
	{ "263-c3.c3",    0x800000, 0x1a343323, 3 | BRF_GRA },           //  5
	{ "263-c4.c4",    0x800000, 0x942cfb44, 3 | BRF_GRA },           //  6
	{ "263-c5.c5",    0x800000, 0xa748854f, 3 | BRF_GRA },           //  7
	{ "263-c6.c6",    0x800000, 0x5c8ba116, 3 | BRF_GRA },           //  8

	{ "263-m1.m1",    0x020000, 0x46ac8228, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	{ "263-v1.v1",    0x800000, 0x01e9b9cd, 5 | BRF_SND },           //  9 Sound data
	{ "263-v2.v2",    0x800000, 0x4ab2bf81, 5 | BRF_SND },           // 10
};

STDROMPICKEXT(ms4plus, ms4plus, neogeo)
STD_ROM_FN(ms4plus)

struct BurnDriver BurnDrvms4plus = {
	"ms4plus", "mslug4", "neogeo", NULL, "2002",
	"Metal Slug 4 Plus (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, ms4plusRomInfo, ms4plusRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug4Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Rage of the Dragons (NGM-2640)
/* Encrypted Set */ /* MVS VERSION */

static struct BurnRomInfo rotdRomDesc[] = {
	{ "264-p1.p1",    0x800000, 0xb8cc969d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "264-c1.c1",    0x800000, 0x4f148fee, 3 | BRF_GRA },           //  1 Sprite data
	{ "264-c2.c2",    0x800000, 0x7cf5ff72, 3 | BRF_GRA },           //  2
	{ "264-c3.c3",    0x800000, 0x64d84c98, 3 | BRF_GRA },           //  3
	{ "264-c4.c4",    0x800000, 0x2f394a95, 3 | BRF_GRA },           //  4
	{ "264-c5.c5",    0x800000, 0x6b99b978, 3 | BRF_GRA },           //  5
	{ "264-c6.c6",    0x800000, 0x847d5c7d, 3 | BRF_GRA },           //  6
	{ "264-c7.c7",    0x800000, 0x231d681e, 3 | BRF_GRA },           //  7
	{ "264-c8.c8",    0x800000, 0xc5edb5c4, 3 | BRF_GRA },           //  8

	{ "264-m1.m1",    0x020000, 0x4dbd7b43, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "264-v1.v1",    0x800000, 0xfa005812, 5 | BRF_SND },           // 10 Sound data
	{ "264-v2.v2",    0x800000, 0xc3dc8bf0, 5 | BRF_SND },           // 11
};

STDROMPICKEXT(rotd, rotd, neogeo)
STD_ROM_FN(rotd)

static INT32 rotdInit()
{
	INT32 nRet;

	nNeoProtectionXor = 0x3F;

	nRet = NeoInit();

	if (nRet == 0) {
		PCM2DecryptV(0x1000000, 2);
	}

	return nRet;
}

struct BurnDriver BurnDrvrotd = {
	"rotd", NULL, "neogeo", NULL, "2002",
	"Rage of the Dragons (NGM-264?)\0", NULL, "Evoga / Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, 0,
	NULL, rotdRomInfo, rotdRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	rotdInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Rage of the Dragons (NGH-2640)
/* Encrypted Set */ /* AES VERSION */

 static struct BurnRomInfo rotdhRomDesc[] = {
	{ "264-pk1.p1",   0x100000, 0xff2fa719, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 	/ mask rom TC538200
	{ "264-pk2.sp2",  0x200000, 0x0df2e112, 1 | BRF_ESS | BRF_PRG }, //  1 				/ mask rom TC5316200

	{ "264-c1.c1",    0x800000, 0x4f148fee, 3 | BRF_GRA },           //  1 Sprite data 	/ mask rom TC5364205
	{ "264-c2.c2",    0x800000, 0x7cf5ff72, 3 | BRF_GRA },           //  2 				/ mask rom TC5364205
	{ "264-c3.c3",    0x800000, 0x64d84c98, 3 | BRF_GRA },           //  3 				/ mask rom TC5364205
	{ "264-c4.c4",    0x800000, 0x2f394a95, 3 | BRF_GRA },           //  4 				/ mask rom TC5364205
	{ "264-c5.c5",    0x800000, 0x6b99b978, 3 | BRF_GRA },           //  5 				/ mask rom TC5364205
	{ "264-c6.c6",    0x800000, 0x847d5c7d, 3 | BRF_GRA },           //  6 				/ mask rom TC5364205
	{ "264-c7.c7",    0x800000, 0x231d681e, 3 | BRF_GRA },           //  7 				/ mask rom TC5364205
	{ "264-c8.c8",    0x800000, 0xc5edb5c4, 3 | BRF_GRA },           //  8 				/ mask rom TC5364205

	{ "264-m1.m1",    0x020000, 0x4dbd7b43, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code 	/ mask rom 27c010

	{ "264-v1.v1",    0x800000, 0xfa005812, 5 | BRF_SND },           // 10 Sound data 	/ mask rom TC5364205
	{ "264-v2.v2",    0x800000, 0xc3dc8bf0, 5 | BRF_SND },           // 11 				/ mask rom TC5364205
};

STDROMPICKEXT(rotdh, rotdh, neogeo)
STD_ROM_FN(rotdh)

struct BurnDriver BurnDrvrotdh = {
	"rotdh", "rotd", "neogeo", NULL, "2002",
	"Rage of the Dragons (NGH-2640)\0", NULL, "Evoga / Playmore", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, 0,
	NULL, rotdhRomInfo, rotdhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	rotdInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};


// Matrimelee / Shin Gouketsuji Ichizoku Toukon (NGM-2660) (NGH-2660)
/* Encrypted Set */ /* MVS AND AES VERSION */

static struct BurnRomInfo matrimRomDesc[] = {
	{ "266-p1.p1",    0x100000, 0x5d4c2dc7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "266-p2.sp2",   0x400000, 0xa14b1906, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "266-c1.c1",    0x800000, 0x505f4e30, 3 | BRF_GRA },           //  2 Sprite data
	{ "266-c2.c2",    0x800000, 0x3cb57482, 3 | BRF_GRA },           //  3
	{ "266-c3.c3",    0x800000, 0xf1cc6ad0, 3 | BRF_GRA },           //  4
	{ "266-c4.c4",    0x800000, 0x45b806b7, 3 | BRF_GRA },           //  5
	{ "266-c5.c5",    0x800000, 0x9a15dd6b, 3 | BRF_GRA },           //  6
	{ "266-c6.c6",    0x800000, 0x281cb939, 3 | BRF_GRA },           //  7
	{ "266-c7.c7",    0x800000, 0x4b71f780, 3 | BRF_GRA },           //  8
	{ "266-c8.c8",    0x800000, 0x29873d33, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "266-m1.m1",    0x020000, 0x456c3e6c, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "266-v1.v1",    0x800000, 0xa4f83690, 5 | BRF_SND },           // 11 Sound data
	{ "266-v2.v2",    0x800000, 0xd0f69eda, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(matrim, matrim, neogeo)
STD_ROM_FN(matrim)

static INT32 matrimInit()
{
	INT32 nRet;

//	nNeoTextROMSize = 0x080000;
	nNeoProtectionXor = 0x6A;
	NeoCallbackActive->pInitialise = PCM2DecryptP;

	nRet = NeoInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0x01000, 0xffce20, { 0xc4, 0x83, 0xa8, 0x5f, 0x21, 0x27, 0x64, 0xaf } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvmatrim = {
	"matrim", NULL, "neogeo", NULL, "2002",
	"Matrimelee / Shin Gouketsuji Ichizoku Toukon (NGM-2660) (NGH-2660)\0", NULL, "Noise Factory / Atlus", "Neo Geo MVS",
	L"\u65B0\u8C6A\u8840\u5BFA\u4E00\u65CF - \u95D8\u5A5A\0Matrimelee (NGM-2660) (NGH-2660)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_PWRINST,
	NULL, matrimRomInfo, matrimRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	matrimInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Matrimelee / Shin Gouketsuji Ichizoku Toukon (bootleg)

static struct BurnRomInfo matrimblRomDesc[] = {
	{ "266-p1.p1",    0x100000, 0x5d4c2dc7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "266-p2.sp2",   0x400000, 0xa14b1906, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "mart-c1.bin",  0x800000, 0xa5595656, 3 | BRF_GRA },		 	 //  2 Sprite data
	{ "mart-c2.bin",  0x800000, 0xc5f7c300, 3 | BRF_GRA },		 	 //  3
	{ "mart-c3.bin",  0x800000, 0x574efd7d, 3 | BRF_GRA },		 	 //  4
	{ "mart-c4.bin",  0x800000, 0x109d54d9, 3 | BRF_GRA },		 	 //  5
	{ "mart-c5.bin",  0x800000, 0x15c9e882, 3 | BRF_GRA },		 	 //  6
	{ "mart-c6.bin",  0x800000, 0x77497b97, 3 | BRF_GRA },		 	 //  7
	{ "mart-c7.bin",  0x800000, 0xab481bb6, 3 | BRF_GRA },		 	 //  8
	{ "mart-c8.bin",  0x800000, 0x906cf267, 3 | BRF_GRA },		 	 //  9

	{ "mart-m1.bin",  0x020000, 0x3ea96ab1, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "mart-v1.bin",  0x400000, 0x352b0a07, 5 | BRF_SND },		 	 // 11 Sound data
	{ "mart-v2.bin",  0x400000, 0x1e9bd59e, 5 | BRF_SND },		 	 // 12
	{ "mart-v3.bin",  0x400000, 0xe8362fcc, 5 | BRF_SND },		 	 // 13
	{ "mart-v4.bin",  0x400000, 0xc8c79b19, 5 | BRF_SND },		 	 // 14
};

STDROMPICKEXT(matrimbl, matrimbl, neogeo)
STD_ROM_FN(matrimbl)

static void matrimblCallback()
{
	INT32 i, j;
	UINT8 *dst = (UINT8 *)BurnMalloc( 0x020000 );

	if (dst)
	{
		memcpy(dst, NeoZ80ROMActive, 0x020000);

		for(i = 0; i < 0x020000; i++) {
					 j  = i;
			if (i & 0x00001) j ^= 0x00800;
			if (i & 0x00002) j ^= 0x02000;
			if (i & 0x00800) j ^= 0x10800;
			if (i & 0x10000) j ^= 0x00800;
			NeoZ80ROMActive[j] = dst[i];
		}
		BurnFree (dst);
	}

	PCM2DecryptP();
	DoPerm(0);
	NeoCMCExtractSData(NeoSpriteROM[nNeoActiveSlot], NeoTextROM[nNeoActiveSlot], 0x4000000, 0x080000);
}

static INT32 matrimblInit()
{
	INT32 nRet;

//	nNeoTextROMSize = 0x080000;
	NeoCallbackActive->pInitialise = matrimblCallback;

	nRet = NeoInit();

	if (nRet == 0) {
		BurnByteswap(YM2610ADPCMAROM[nNeoActiveSlot] + 0x400000, 0x400000);
		BurnByteswap(YM2610ADPCMAROM[nNeoActiveSlot] + 0xc00000, 0x400000);
	}

	return nRet;
}

struct BurnDriver BurnDrvmatrimbl = {
	"matrimbl", "matrim", "neogeo", NULL, "2002",
	"Matrimelee / Shin Gouketsuji Ichizoku Toukon (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	L"\u65B0\u8C6A\u8840\u5BFA\u4E00\u65CF - \u95D8\u5A5A\0Matrimelee (bootleg)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_ALTERNATE_TEXT, GBF_VSFIGHT, FBF_PWRINST,
	NULL, matrimblRomInfo, matrimblRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	matrimblInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Jockey Grand Prix (set 1)
/* MVS ONLY RELEASE */

static struct BurnRomInfo jockeygpRomDesc[] = {
	/* Officially licensed? Cart has a holographic 'SNK' sticker applied */
	{ "008-epr.p1",   0x100000, 0x2fb7f388, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	/* P on eprom, correct chip label unknown */

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "008-c1.c1",    0x800000, 0xa9acbf18, 3 | BRF_GRA },           //  1 Sprite data
	{ "008-c2.c2",    0x800000, 0x6289eef9, 3 | BRF_GRA },           //  2

	/* Encrypted */
	{ "008-mg1.m1",   0x080000, 0xd163c690, 4 | BRF_ESS | BRF_PRG }, //  3 Z80 code

	{ "008-v1.v1",    0x200000, 0x443eadba, 5 | BRF_SND },           //  4 Sound data
};

STDROMPICKEXT(jockeygp, jockeygp, neogeo)
STD_ROM_FN(jockeygp)

INT32 jockeygpInit()
{
	nNeoProtectionXor = 0xAC;
	return NeoInit();
}

struct BurnDriver BurnDrvjockeygp = {
	"jockeygp", NULL, "neogeo", NULL, "2001",
	"Jockey Grand Prix (set 1)\0", NULL, "Sun Amusement / BrezzaSoft", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_GAMBLING | HARDWARE_SNK_ENCRYPTED_M1, GBF_RACING, 0,
	NULL, jockeygpRomInfo, jockeygpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	jockeygpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Jockey Grand Prix (set 2)
/* MVS ONLY RELEASE */

static struct BurnRomInfo jockeygpaRomDesc[] = {
	/* Officially licensed? Cart has a holographic 'SNK' sticker applied */
	{ "008-epr_a.p1", 0x100000, 0xb8f35532, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	/* P on eprom, correct chip label unknown */

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "008-c1.c1",    0x800000, 0xa9acbf18, 3 | BRF_GRA },           //  1 Sprite data
	{ "008-c2.c2",    0x800000, 0x6289eef9, 3 | BRF_GRA },           //  2

	/* Encrypted */
	{ "008-mg1.m1",   0x080000, 0xd163c690, 4 | BRF_ESS | BRF_PRG }, //  3 Z80 code

	{ "008-v1.v1",    0x200000, 0x443eadba, 5 | BRF_SND },           //  4 Sound data
};

STDROMPICKEXT(jockeygpa, jockeygpa, neogeo)
STD_ROM_FN(jockeygpa)

struct BurnDriver BurnDrvjockeygpa = {
	"jockeygpa", "jockeygp", "neogeo", NULL, "2001",
	"Jockey Grand Prix (set 2)\0", NULL, "Sun Amusement / BrezzaSoft", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_GAMBLING | HARDWARE_SNK_ENCRYPTED_M1, GBF_RACING, 0,
	NULL, jockeygpaRomInfo, jockeygpaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	jockeygpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// V-Liner (v0.7a)
/* MVS ONLY RELEASE */

static struct BurnRomInfo vlinerRomDesc[] = {
	/* Officially licensed? Cart has a holographic 'SNK' sticker applied */
	{ "epr_7a.p1",    0x080000, 0x052f93ed, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "s-1.s1",       0x020000, 0x972d8c31, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "c-1.c1",       0x080000, 0x5118f7c0, 3 | BRF_GRA },           //  2 Sprite data
	{ "c-2.c2",       0x080000, 0xefe9b33e, 3 | BRF_GRA },           //  3

	{ "m-1.m1",       0x010000, 0x9b92b7d1, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code
};

STDROMPICKEXT(vliner, vliner, neogeo)
STD_ROM_FN(vliner)

static INT32 vlinerInit()
{
	vlinermode = 1;

	return NeoInit();
}

struct BurnDriver BurnDrvvliner = {
	"vliner", NULL, "neogeo", NULL, "2001",
	"V-Liner (v0.7a)\0", NULL, "Dyna / BreezaSoft", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SNK_NEOGEO | HARDWARE_SNK_GAMBLING, GBF_CASINO, 0,
	NULL, vlinerRomInfo, vlinerRomName, NULL, NULL, NULL, NULL, vlinerInputInfo, neogeoDIPInfo,
	vlinerInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// V-Liner (v0.7e)
/* MVS ONLY RELEASE */

static struct BurnRomInfo vliner7eRomDesc[] = {
	/* Officially licensed? Cart has a holographic 'SNK' sticker applied */
	{ "epr_7e.p1",    0x080000, 0x49a94db5, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "s-1.s1",       0x020000, 0x972d8c31, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "c-1.c1",       0x080000, 0x5118f7c0, 3 | BRF_GRA },           //  2 Sprite data
	{ "c-2.c2",       0x080000, 0xefe9b33e, 3 | BRF_GRA },           //  3

	{ "m-1.m1",       0x010000, 0x9b92b7d1, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code
};

STDROMPICKEXT(vliner7e, vliner7e, neogeo)
STD_ROM_FN(vliner7e)

struct BurnDriver BurnDrvvliner7e = {
	"vliner7e", "vliner", "neogeo", NULL, "2001",
	"V-Liner (v0.7e)\0", NULL, "Dyna / BreezaSoft", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SNK_NEOGEO | HARDWARE_SNK_GAMBLING, GBF_CASINO, 0,
	NULL, vliner7eRomInfo, vliner7eRomName, NULL, NULL, NULL, NULL, vlinerInputInfo, neogeoDIPInfo,
	vlinerInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// V-Liner (v0.6e)
/* MVS ONLY RELEASE */

static struct BurnRomInfo vliner6eRomDesc[] = {
	/* Officially licensed? Cart has a holographic 'SNK' sticker applied */
	{ "epr_6e.p1",    0x080000, 0x72a2c043, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "s-1.s1",       0x020000, 0x972d8c31, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "c-1.c1",       0x080000, 0x5118f7c0, 3 | BRF_GRA },           //  2 Sprite data
	{ "c-2.c2",       0x080000, 0xefe9b33e, 3 | BRF_GRA },           //  3

	{ "m-1.m1",       0x010000, 0x9b92b7d1, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code
};

STDROMPICKEXT(vliner6e, vliner6e, neogeo)
STD_ROM_FN(vliner6e)

struct BurnDriver BurnDrvvliner6e = {
	"vliner6e", "vliner", "neogeo", NULL, "2001",
	"V-Liner (v0.6e)\0", NULL, "Dyna / BreezaSoft", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SNK_NEOGEO | HARDWARE_SNK_GAMBLING, GBF_CASINO, 0,
	NULL, vliner6eRomInfo, vliner6eRomName, NULL, NULL, NULL, NULL, vlinerInputInfo, neogeoDIPInfo,
	vlinerInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// V-Liner (v0.54)
/* MVS ONLY RELEASE */

static struct BurnRomInfo vliner54RomDesc[] = {
	/* Officially licensed? Cart has a holographic 'SNK' sticker applied */
	{ "epr_54.p1",    0x080000, 0x172efc18, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "s-1.s1",       0x020000, 0x972d8c31, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "c-1.c1",       0x080000, 0x5118f7c0, 3 | BRF_GRA },           //  2 Sprite data
	{ "c-2.c2",       0x080000, 0xefe9b33e, 3 | BRF_GRA },           //  3

	{ "m-1.m1",       0x010000, 0x9b92b7d1, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code
};

STDROMPICKEXT(vliner54, vliner54, neogeo)
STD_ROM_FN(vliner54)

struct BurnDriver BurnDrvvliner54 = {
	"vliner54", "vliner", "neogeo", NULL, "2001",
	"V-Liner (v0.54)\0", NULL, "Dyna / BreezaSoft", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SNK_NEOGEO | HARDWARE_SNK_GAMBLING, GBF_CASINO, 0,
	NULL, vliner54RomInfo, vliner54RomName, NULL, NULL, NULL, NULL, vlinerInputInfo, neogeoDIPInfo,
	vlinerInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// V-Liner (v0.53)
/* MVS ONLY RELEASE */

static struct BurnRomInfo vliner53RomDesc[] = {
	/* Officially licensed? Cart has a holographic 'SNK' sticker applied */
	{ "epr_53.p1",    0x080000, 0xe263dce8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "s-1.s1",       0x020000, 0x972d8c31, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "c-1.c1",       0x080000, 0x5118f7c0, 3 | BRF_GRA },           //  2 Sprite data
	{ "c-2.c2",       0x080000, 0xefe9b33e, 3 | BRF_GRA },           //  3

	{ "m-1.m1",       0x010000, 0x9b92b7d1, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code
};

STDROMPICKEXT(vliner53, vliner53, neogeo)
STD_ROM_FN(vliner53)

struct BurnDriver BurnDrvvliner53 = {
	"vliner53", "vliner", "neogeo", NULL, "2001",
	"V-Liner (v0.53)\0", NULL, "Dyna / BreezaSoft", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SNK_NEOGEO | HARDWARE_SNK_GAMBLING, GBF_CASINO, 0,
	NULL, vliner53RomInfo, vliner53RomName, NULL, NULL, NULL, NULL, vlinerInputInfo, neogeoDIPInfo,
	vlinerInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Digger Man

static struct BurnRomInfo diggermaRomDesc[] = {
	{ "dig-p1.bin",   0x080000, 0xeda433d7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "dig-s1.bin",   0x020000, 0x9b3168f0, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "dig-c1.bin",   0x080000, 0x3db0a4ed, 3 | BRF_GRA },           //  2 Sprite data
	{ "dig-c2.bin",   0x080000, 0x3e632161, 3 | BRF_GRA },           //  3

	{ "dig-m1.bin",   0x020000, 0xe777a234, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "dig-v1.bin",   0x080000, 0xee15bda4, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(diggerma, diggerma, neogeo)
STD_ROM_FN(diggerma)

struct BurnDriver BurnDrvdiggerma = {
	"diggerma", NULL, "neogeo", NULL, "2000",
	"Digger Man\0", NULL, "Kyle Hodgetts", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_MAZE, 0,
	NULL, diggermaRomInfo, diggermaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Super Bubble Pop
/* Unlicensed, no official game ID # */ /* MVS ONLY VERSION */

static struct BurnRomInfo sbpRomDesc[] = {
	{ "001-003-02a.u2",   0x080000, 0xd054d264, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "001-003-02b.u2",   0x080000, 0x2fd04b2a, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "001-003-03b.u3",   0x200000, 0x44791317, 3 | BRF_GRA },           //  2 Sprite data
	{ "001-003-04b.u4",   0x200000, 0xa3a1c0df, 3 | BRF_GRA },           //  3

	{ "001-003-01b.u1",   0x080000, 0x7b1f86f7, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "001-003-12a.u12",  0x400000, 0xc96723b9, 5 | BRF_SND }, 		 	 //  5 Sound data
	{ "001-003-13a.u13",  0x400000, 0x08c339a5, 5 | BRF_SND },		 	 //  6
};

STDROMPICKEXT(sbp, sbp, neogeo)
STD_ROM_FN(sbp)

static void sbpCallback()
{
	UINT16* ROM = (UINT16*)Neo68KROMActive;

	for (INT32 i = 0x200 / 2; i < 0x2000 / 2; i++) {
		UINT16 OrigData = ROM[i];
		UINT16 Data =  BITSWAP16(OrigData, 11, 10, 9, 8, 15, 14, 13, 12, 3, 2, 1, 0, 7, 6, 5, 4);

		if (i == 0xf5e) {
			ROM[i] = OrigData;
		} else {
			ROM[i] = Data;
		}
	}

	// stop the game overwriting the text layer data
	ROM[0x2a6f8 / 2] = 0x4e71;
	ROM[0x2a6fa / 2] = 0x4e71;
	ROM[0x2a6fc / 2] = 0x4e71;

	nNeoTextROMSize[nNeoActiveSlot] = 0x20000;
}

static INT32 sbpInit()
{
	NeoCallbackActive->pInitialise = sbpCallback;

	return NeoInit();
}

struct BurnDriverD BurnDrvsbp = {
	"sbp", NULL, "neogeo", NULL, "2004",
	"Super Bubble Pop\0", NULL, "Vektorlogic", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_HOMEBREW, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SPORTSMISC, 0,
	NULL, sbpRomInfo, sbpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	sbpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// -----------------------------------------------------------------------------
// Games not in MAME

// 3 Count Bout / Fire Suplex (NGM-043)
/* MVS VERSION */

static struct BurnRomInfo countba3RomDesc[] = {
	{ "043-epr.ep1",  0x080000, 0xeb2714c4, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ M27C4002
	{ "043-epr.ep2",  0x080000, 0x5e764567, 1 | BRF_ESS | BRF_PRG }, //  1 					/ M27C4002 */

	{ "043-s1.s1",    0x020000, 0xc362d484, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "043-c1.c1",    0x200000, 0xbad2d67f, 3 | BRF_GRA },           //  2 Sprite data 		/ TC5316200
	{ "043-c2.c2",    0x200000, 0xa7fbda95, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "043-c3.c3",    0x200000, 0xf00be011, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "043-c4.c4",    0x200000, 0x1887e5c0, 3 | BRF_GRA },           //  5 					/ TC5316200

	{ "043-m1.m1",    0x020000, 0x7eab59cb, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code 		/ TC531001

	{ "043-v1.v1",    0x200000, 0x63688ce8, 5 | BRF_SND },           //  7 Sound data 		/ TC5316200
	{ "043-v2.v2",    0x200000, 0xc69a827b, 5 | BRF_SND },           //  8 					/ TC5316200
};

STDROMPICKEXT(countba3, countba3, neogeo)
STD_ROM_FN(countba3)

struct BurnDriver BurnDrv3Countba = {
	"3countba", "3countb", "neogeo", NULL, "1993",
	"3 Count Bout / Fire Suplex (NGM-043)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_VSFIGHT, 0,
	NULL, countba3RomInfo, countba3RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Bang Bead (Prototype?)

static struct BurnRomInfo bangbedpRomDesc[] = {
	{ "259-p1.bin",   0x200000, 0x88a37f8b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "259-s1p.bin",  0x020000, 0xbb50fb2d, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "259-c1p.bin",  0x200000, 0xe3919e44, 3 | BRF_GRA },           //  2 Sprite data
	{ "259-c2p.bin",  0x200000, 0xbaf5a320, 3 | BRF_GRA },           //  3
	{ "259-c3p.bin",  0x100000, 0xc8e52157, 3 | BRF_GRA },           //  4
	{ "259-c4p.bin",  0x100000, 0x69fa8e60, 3 | BRF_GRA },           //  5

	{ "259-m1.bin",   0x020000, 0x85668ee9, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "259-v1p.bin",  0x200000, 0xe97b9385, 5 | BRF_SND },           //  7 Sound data
	{ "259-v2p.bin",  0x200000, 0xb0cbd70a, 5 | BRF_SND },           //  8
	{ "259-v3p.bin",  0x100000, 0x97528fe9, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(bangbedp, bangbedp, neogeo)
STD_ROM_FN(bangbedp)

struct BurnDriver BurnDrvbangbedp = {
	"bangbedp", "bangbead", "neogeo", NULL, "2000",
	"Bang Bead (Prototype?)\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_PROTOTYPE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_BALLPADDLE, 0,
	NULL, bangbedpRomInfo, bangbedpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Fatal Fury 2 / Garou Densetsu 2 - arata-naru tatakai (NGM-047)
/* MVS VERSION */

static struct BurnRomInfo fatfury2aRomDesc[] = {
	{ "047-epr.ep1",  0x080000, 0xbe40ea92, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ M27C4002
	{ "047-epr.ep2",  0x080000, 0x2a9beac5, 1 | BRF_ESS | BRF_PRG }, //  1 					/ M27C4002 */

	{ "047-s1.s1",    0x020000, 0xd7dbbf39, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "047-c1.c1",    0x200000, 0xf72a939e, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5316200
	{ "047-c2.c2",    0x200000, 0x05119a0d, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "047-c3.c3",    0x200000, 0x01e00738, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "047-c4.c4",    0x200000, 0x9fe27432, 3 | BRF_GRA },           //  6 					/ TC5316200

	{ "047-m1.m1",    0x020000, 0x820b0ba7, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "047-v1.v1",    0x200000, 0xd9d00784, 5 | BRF_SND },           //  8 Sound data 		/ TC5316200
	{ "047-v2.v2",    0x200000, 0x2c9a4b33, 5 | BRF_SND },           //  9 					/ TC5316200
};

STDROMPICKEXT(fatfury2a, fatfury2a, neogeo)
STD_ROM_FN(fatfury2a)

struct BurnDriver BurnDrvFatfury2a = {
	"fatfury2a", "fatfury2", "neogeo", NULL, "1992",
	"Fatal Fury 2 / Garou Densetsu 2 - arata-naru tatakai (NGM-047)\0", NULL, "SNK", "Neo Geo MVS",
	L"Fatal Fury 2\0\u9913\u72FC\u4F1D\u8AAC\uFF12 - \u65B0\u305F\u306A\u308B\u95D8 (NGM-047)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPC, GBF_VSFIGHT, FBF_FATFURY,
	NULL, fatfury2aRomInfo, fatfury2aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	fatfury2Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Fatal Fury 3 - Road to the Final Victory / Garou Densetsu 3 - haruka-naru tatakai (NGM-069)(NGH-069) (alternate set)
/* MVS AND AES VERSION (95-05-17 16:08) */

static struct BurnRomInfo fatfury3aRomDesc[] = {
	{ "069-p1.bin",   0x100000, 0x4e27bd16, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200
	{ "069-sp2.sp2",  0x200000, 0xdbe963ed, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "069-s1.s1",    0x020000, 0x0b33a800, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "069-c1.c1",    0x400000, 0xe302f93c, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "069-c2.c2",    0x400000, 0x1053a455, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "069-c3.c3",    0x400000, 0x1c0fde2f, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "069-c4.c4",    0x400000, 0xa25fc3d0, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "069-c5.c5",    0x200000, 0xb3ec6fa6, 3 | BRF_GRA },           //  7 					/ TC5332205
	{ "069-c6.c6",    0x200000, 0x69210441, 3 | BRF_GRA },           //  8 					/ TC5332205

	{ "069-m1.m1",    0x020000, 0xfce72926, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code			/ TC531001

	{ "069-v1.v1",    0x400000, 0x2bdbd4db, 5 | BRF_SND },           // 10 Sound data		/ TC5332204
	{ "069-v2.v2",    0x400000, 0xa698a487, 5 | BRF_SND },           // 11 					/ TC5332204
	{ "069-v3.v3",    0x200000, 0x581c5304, 5 | BRF_SND },           // 12 					/ TC5316200
};

STDROMPICKEXT(fatfury3a, fatfury3a, neogeo)
STD_ROM_FN(fatfury3a)

struct BurnDriver BurnDrvFatfury3a = {
	"fatfury3a", "fatfury3", "neogeo", NULL, "1995",
	"Fatal Fury 3 - Road to the Final Victory / Garou Densetsu 3 - haruka-naru tatakai (NGM-069)(NGH-069) (alternate set)\0", NULL, "SNK", "Neo Geo MVS",
	L"Fatal Fury 3 - Road to the Final Victory\0\u9913\u72FC\u4F1D\u8AAC\uFF13 (NGM-069)(NGH-069) (alternate set)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, fatfury3aRomInfo, fatfury3aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Samurai Shodown / Samurai Spirits (NGM-045, alternate board)
/* MVS VERSION */

static struct BurnRomInfo samshoaRomDesc[] = {
	/* This set uses NEO-MVS PROGSC board and NEO-MVS CHA 42G-3 board; same rom data as in samshoh is used. */
	{ "045-epr.ep1",  0x080000, 0x80aa6c97, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ M27C4002
	{ "045-epr.ep2",  0x080000, 0x71768728, 1 | BRF_ESS | BRF_PRG }, //  1 					/ M27C4002
	{ "045-p2.sp2",   0x080000, 0x38ee9ba9, 1 | BRF_ESS | BRF_PRG }, //  2 					/ TC534200

	{ "045-s1.s1",    0x020000, 0x9142a4d3, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "045-c1.c1",    0x200000, 0x2e5873a4, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5316200
	{ "045-c2.c2",    0x200000, 0x04febb10, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "045-c3.c3",    0x200000, 0xf3dabd1e, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "045-c4.c4",    0x200000, 0x935c62f0, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "045-c5.c5",    0x080000, 0xa2bb8284, 3 | BRF_GRA },           //  7 					/ TC534200
	{ "045-c6.c6",    0x080000, 0x4fa71252, 3 | BRF_GRA },           //  8 					/ TC534200

	{ "045-m1.m1",    0x020000, 0x95170640, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code 		/ TC5310001

	{ "045-v1.v1",    0x200000, 0x37f78a9b, 5 | BRF_SND },           // 10 Sound data 		/ TC5316200
	{ "045-v2.v2",    0x200000, 0x568b20cf, 5 | BRF_SND },           // 11 					/ TC5316200
};

STDROMPICKEXT(samshoa, samshoa, neogeo)
STD_ROM_FN(samshoa)

struct BurnDriver BurnDrvSamShoa = {
	"samshoa", "samsho", "neogeo", NULL, "1993",
	"Samurai Shodown / Samurai Spirits (NGM-045, alternate board)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samshoaRomInfo, samshoaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Samurai Shodown V Special / Samurai Spirits Zero Special (NGM-272) (NGH-272) (Final Edition, location test version)

static struct BurnRomInfo samsh5feRomDesc[] = {
	{ "272-p1.bin",  0x400000, 0x9e72cc83, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "272-p2.bin",  0x400000, 0x1f5469bc, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "272-s1.bin",  0x020000, 0xc297f973, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "272-c1d.bin", 0x800000, 0xec9fda8d, 3 | BRF_GRA },           //  3 Sprite data
	{ "272-c2d.bin", 0x800000, 0xd2fc888d, 3 | BRF_GRA },           //  4
	{ "272-c3d.bin", 0x800000, 0xb0ea781b, 3 | BRF_GRA },           //  5
	{ "272-c4d.bin", 0x800000, 0xd34ac591, 3 | BRF_GRA },           //  6
	{ "272-c5d.bin", 0x800000, 0x1b5c1ea2, 3 | BRF_GRA },           //  7
	{ "272-c6d.bin", 0x800000, 0xdeeaad58, 3 | BRF_GRA },           //  8
	{ "272-c7d.bin", 0x800000, 0xdce0119f, 3 | BRF_GRA },           //  9
	{ "272-c8d.bin", 0x800000, 0x3bf256c5, 3 | BRF_GRA },           // 10

	{ "272-m1d.bin", 0x080000, 0x203d744e, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "272-v1d.bin", 0x800000, 0x28d57d10, 5 | BRF_SND },           // 12 Sound data
	{ "272-v2d.bin", 0x800000, 0x95fe7646, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(samsh5fe, samsh5fe, neogeo)
STD_ROM_FN(samsh5fe)

struct BurnDriver BurnDrvsamsh5fe = {
	"samsh5fe", "samsh5sp", "neogeo", NULL, "2004",
	"Samurai Shodown V Special / Samurai Spirits Zero Special (NGM-272) (NGH-272) (Final Edition, location test version)\0", NULL, "Yuki Enterprise / SNK Playmore", "Neo Geo MVS",
	L"Samurai Shodown V Special\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4\u96F6 Special (NGM-272) (NGH-272) (Final Edition, location test version)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsh5feRomInfo, samsh5feRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '96 (NGM-214, alternate board)
/* MVS VERSION */

static struct BurnRomInfo kof96aRomDesc[] = {
	/* This set uses NEO-MVS PROGSS3 board; same rom data as in kof96h is used */
	{ "214-epr.ep1",  0x080000, 0xa6101486, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ 27C240  / 27C4096
	{ "214-epr.ep2",  0x080000, 0x6abc7ae5, 1 | BRF_ESS | BRF_PRG }, //  1 					/ 27C4096 / 27C4096
	{ "214-epr.ep3",  0x080000, 0xa588dff4, 1 | BRF_ESS | BRF_PRG }, //  2 					/ 27C240  / 27C4096
	{ "214-epr.ep4",  0x080000, 0x6d6f17eb, 1 | BRF_ESS | BRF_PRG }, //  3 					/ 27C240  / 27C4096
	{ "214-p5.p5",    0x200000, 0xd388f4da, 1 | BRF_ESS | BRF_PRG }, //  4 					/ TC5316200CP */

	{ "214-s1.s1",    0x020000, 0x1254cbdb, 2 | BRF_GRA },           //  5 Text layer tiles / TC531000

	{ "214-c1.c1",    0x400000, 0x7ecf4aa2, 3 | BRF_GRA },           //  6 Sprite data		/ TC5332205
	{ "214-c2.c2",    0x400000, 0x05b54f37, 3 | BRF_GRA },           //  7 					/ TC5332205
	{ "214-c3.c3",    0x400000, 0x64989a65, 3 | BRF_GRA },           //  8 					/ TC5332205
	{ "214-c4.c4",    0x400000, 0xafbea515, 3 | BRF_GRA },           //  9 					/ TC5332205
	{ "214-c5.c5",    0x400000, 0x2a3bbd26, 3 | BRF_GRA },           // 10 					/ TC5332205
	{ "214-c6.c6",    0x400000, 0x44d30dc7, 3 | BRF_GRA },           // 11 					/ TC5332205
	{ "214-c7.c7",    0x400000, 0x3687331b, 3 | BRF_GRA },           // 12 					/ TC5332205
	{ "214-c8.c8",    0x400000, 0xfa1461ad, 3 | BRF_GRA },           // 13 					/ TC5332205

	{ "214-m1.m1",    0x020000, 0xdabc427c, 4 | BRF_ESS | BRF_PRG }, // 14 Z80 code			/ TC531001

	{ "214-v1.v1",    0x400000, 0x63f7b045, 5 | BRF_SND },           // 15 Sound data		/ TC5332204
	{ "214-v2.v2",    0x400000, 0x25929059, 5 | BRF_SND },           // 16 					/ TC5332204
	{ "214-v3.v3",    0x200000, 0x92a2257d, 5 | BRF_SND },           // 17 					/ TC5316200
};

STDROMPICKEXT(kof96a, kof96a, neogeo)
STD_ROM_FN(kof96a)

struct BurnDriver BurnDrvKof96a = {
	"kof96a", "kof96", "neogeo", NULL, "1996",
	"The King of Fighters '96 (NGM-214, alternate board)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO,  GBF_VSFIGHT, FBF_KOF,
	NULL, kof96aRomInfo, kof96aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The Last Blade / Bakumatsu Roman - Gekka no Kenshi (NGM-2340, alternate board)
/* MVS VERSION */

static struct BurnRomInfo lastbladaRomDesc[] = {
	/* This set uses NEO-MVS PROGBK1 board; same rom data as in lastblad is used */
	{ "0234-ep1.ep1", 0x080000, 0x3893d38b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code / M27C4002
	{ "0234-ep2.ep2", 0x080000, 0x9470c788, 1 | BRF_ESS | BRF_PRG }, //  1 			/ M27C4002
	{ "0234-p1.p1",   0x200000, 0xb45e631c, 1 | BRF_ESS | BRF_PRG }, //  2 			/ M27C160
	{ "0234-sp2.sp2", 0x200000, 0xaa34e149, 1 | BRF_ESS | BRF_PRG }, //  3 			/ M27C160

	{ "234-s1.s1",    0x020000, 0x95561412, 2 | BRF_GRA },           //  4 Text layer tiles

	{ "234-c1.c1",    0x800000, 0x9f7e2bd3, 3 | BRF_GRA },           //  5 Sprite data
	{ "234-c2.c2",    0x800000, 0x80623d3c, 3 | BRF_GRA },           //  6
	{ "234-c3.c3",    0x800000, 0x91ab1a30, 3 | BRF_GRA },           //  7
	{ "234-c4.c4",    0x800000, 0x3d60b037, 3 | BRF_GRA },           //  8
	{ "234-c5.c5",    0x400000, 0x1ba80cee, 3 | BRF_GRA },           //  9
	{ "234-c6.c6",    0x400000, 0xbeafd091, 3 | BRF_GRA },           // 10

	{ "0234-m1.m1",   0x020000, 0x087628ea, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code / M27C1001

	{ "234-v1.v1",    0x400000, 0xed66b76f, 5 | BRF_SND },           // 12 Sound data
	{ "234-v2.v2",    0x400000, 0xa0e7f6e2, 5 | BRF_SND },           // 13
	{ "234-v3.v3",    0x400000, 0xa506e1e2, 5 | BRF_SND },           // 14
	{ "234-v4.v4",    0x400000, 0x0e34157f, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(lastblada, lastblada, neogeo)
STD_ROM_FN(lastblada)

struct BurnDriver BurnDrvlastblada = {
	"lastblada", "lastblad", "neogeo", NULL, "1997",
	"The Last Blade / Bakumatsu Roman - Gekka no Kenshi (NGM-2340, alternate board)\0", NULL, "SNK", "Neo Geo MVS",
	L"The Last Blade\0\u5E55\u672B\u6D6A\u6F2B \u6708\u83EF\u306E\u5263\u58EB (NGM-2340, alternate board)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, lastbladaRomInfo, lastbladaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// World Heroes 2 Jet (ADM-007)
/* MVS VERSION */

static struct BurnRomInfo wh2jaRomDesc[] = {
	{ "064-p1.p1",    0x200000, 0x385a2e86, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "064-s1.s1",    0x020000, 0x2a03998a, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "064-c13.c1",   0x400000, 0x771a6365, 3 | BRF_GRA },           //  2 Sprite data 		/ mask rom TC5332205
	{ "064-c24.c2",   0x400000, 0xe7863a05, 3 | BRF_GRA },           //  3 					/ mask rom TC5332205
	{ "064-c57.c3",   0x400000, 0x64594ed4, 3 | BRF_GRA },           //  4 					/ mask rom TC5332205
	{ "064-c68.c4",   0x400000, 0x6e385398, 3 | BRF_GRA },           //  5 					/ mask rom TC5332205 */

	{ "064-m1.m1",    0x020000, 0xd2eec9d3, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "064-v1.v1",    0x200000, 0xaa277109, 5 | BRF_SND },           //  7 Sound data		/ TC5316200
	{ "064-v2.v2",    0x200000, 0xb6527edd, 5 | BRF_SND },           //  8 					/ TC5316200
};

STDROMPICKEXT(wh2ja, wh2ja, neogeo)
STD_ROM_FN(wh2ja)

struct BurnDriver BurnDrvwh2ja = {
	"wh2ja", "wh2j", "neogeo", NULL, "1994",
	"World Heroes 2 Jet (ADM-007)\0", NULL, "ADK / SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, wh2jaRomInfo, wh2jaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Xeno Crisis (NGM-BB01) (NGH-BB01)
/* MVS AND AES VERSION */

static struct BurnRomInfo xenocrisisRomDesc[] = {
	{ "BB01-p1.p1",   	0x0100000, 0x637605a6, 1 | BRF_ESS | BRF_PRG }, 	//  0 68K code
	{ "BB01-p2.p2",   	0x0100000, 0x84838145, 1 | BRF_ESS | BRF_PRG }, 	//  1

	{ "BB01-s1.s1",		0x0020000, 0x7537ea79, 2 | BRF_GRA },           	//  2 Text layer tiles

	{ "BB01-c1.c1",		0x0200000, 0xae51ef89, 3 | BRF_GRA },           	//  3 Sprite data
	{ "BB01-c2.c2",		0x0200000, 0xa8610100, 3 | BRF_GRA },           	//  4

	{ "BB01-m1.m1",		0x0010000, 0x28c13ed9, 4 | BRF_ESS | BRF_PRG }, 	//  5 Z80 code

	{ "BB01-v1.v1",		0x1000000, 0x60d57867, 5 | BRF_SND },           	//  6 Sound data
};

STDROMPICKEXT(xenocrisis, xenocrisis, neogeo)
STD_ROM_FN(xenocrisis)

struct BurnDriver BurnDrvxenocrisis = {
	"xenocrisis", NULL, "neogeo", NULL, "2019",
	"Xeno Crisis (NGM-BB01) (NGH-BB01)\0", NULL, "Bitmap Bureau", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN | GBF_SHOOT, 0,
	NULL, xenocrisisRomInfo, xenocrisisRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// -----------------------------------------------------------------------------
// Bootlegs, hacks & decrypted sets

// Art of Fighting 2 / Ryuuko no Ken 2 (Enable hidden characters V2)

static struct BurnRomInfo aof2bhRomDesc[] = {
	{ "056-p1bh.p1",  0x100000, 0x3af1e484, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200

	{ "056-s1.s1",    0x020000, 0x8b02638e, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "056-c1.c1",    0x200000, 0x17b9cbd2, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "056-c2.c2",    0x200000, 0x5fd76b67, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "056-c3.c3",    0x200000, 0xd2c88768, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "056-c4.c4",    0x200000, 0xdb39b883, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "056-c5.c5",    0x200000, 0xc3074137, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "056-c6.c6",    0x200000, 0x31de68d3, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "056-c7.c7",    0x200000, 0x3f36df57, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "056-c8.c8",    0x200000, 0xe546d7a8, 3 | BRF_GRA },           //  9 					/ TC5316200

	{ "056-m1.m1",    0x020000, 0xf27e9d52, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "056-v1.v1",    0x200000, 0x4628fde0, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "056-v2.v2",    0x200000, 0xb710e2f2, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "056-v3.v3",    0x100000, 0xd168c301, 5 | BRF_SND },           // 13 					/ TC538200
};

STDROMPICKEXT(aof2bh, aof2bh, neogeo)
STD_ROM_FN(aof2bh)

struct BurnDriver BurnDrvAof2bh = {
	"aof2bh", "aof2", "neogeo", NULL, "1994",
	"Art of Fighting 2 / Ryuuko no Ken 2 (Enable hidden characters V2)\0", NULL, "Yumeji", "Neo Geo MVS",
	L"Art of Fighting 2\0\u9F8D\u864E\u306E\u62F3\uFF12 (Enable hidden characters V2)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, aof2bhRomInfo, aof2bhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Art of Fighting 3 - The Path of the Warrior / Art of Fighting - Ryuuko no Ken Gaiden (Enable Hidden Characters V2)

static struct BurnRomInfo aof3bhRomDesc[] = {
	{ "096-p1bh.p1",  0x100000, 0x70969ff1, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200
	{ "096-p2.sp2",   0x200000, 0x4d5a2602, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "096-s1.s1",    0x020000, 0xcc7fd344, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "096-c1.c1",    0x400000, 0xf17b8d89, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "096-c2.c2",    0x400000, 0x3840c508, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "096-c3.c3",    0x400000, 0x55f9ee1e, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "096-c4.c4",    0x400000, 0x585b7e47, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "096-c5.c5",    0x400000, 0xc75a753c, 3 | BRF_GRA },           //  7 					/ TC5332205
	{ "096-c6.c6",    0x400000, 0x9a9d2f7a, 3 | BRF_GRA },           //  8 					/ TC5332205
	{ "096-c7.c7",    0x200000, 0x51bd8ab2, 3 | BRF_GRA },           //  9 					/ TC5316200
	{ "096-c8.c8",    0x200000, 0x9a34f99c, 3 | BRF_GRA },           // 10 					/ TC5316200

	{ "096-m1.m1",    0x020000, 0xcb07b659, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "096-v1.v1",    0x200000, 0xe2c32074, 5 | BRF_SND },           // 12 Sound data		/ TC5316200
	{ "096-v2.v2",    0x200000, 0xa290eee7, 5 | BRF_SND },           // 13 					/ TC5316200
	{ "096-v3.v3",    0x200000, 0x199d12ea, 5 | BRF_SND },           // 14 					/ TC5316200
};

STDROMPICKEXT(aof3bh, aof3bh, neogeo)
STD_ROM_FN(aof3bh)

struct BurnDriver BurnDrvAof3bh = {
	"aof3bh", "aof3", "neogeo", NULL, "1996",
	"Art of Fighting 3 - The Path of the Warrior / Art of Fighting - Ryuuko no Ken Gaiden (Enable Hidden Characters V2)\0", NULL, "Yumeji", "Neo Geo MVS",
	L"Art of Fighting 3 - The Path of the Warrior\0Art of Fighting (Enable Hidden Characters V2)\u9F8D\u864E\u306E\u62F3\u5916\u4F1D\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, aof3bhRomInfo, aof3bhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Blazing Star Stage Select Mod by Clear Paper, NeoCverA, JustinG

static struct BurnRomInfo blazstarcmRomDesc[] = {
	{ "239-p1_cm.p1",    0x100000, 0x077ba687, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "239-p2.sp2",   0x200000, 0x9a9f4154, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "239-s1.s1",    0x020000, 0xd56cb498, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "239-c1.c1",    0x400000, 0x84f6d584, 3 | BRF_GRA },           //  3 Sprite data
	{ "239-c2.c2",    0x400000, 0x05a0cb22, 3 | BRF_GRA },           //  4
	{ "239-c3.c3",    0x400000, 0x5fb69c9e, 3 | BRF_GRA },           //  5
	{ "239-c4.c4",    0x400000, 0x0be028c4, 3 | BRF_GRA },           //  6
	{ "239-c5.c5",    0x400000, 0x74bae5f8, 3 | BRF_GRA },           //  7
	{ "239-c6.c6",    0x400000, 0x4e0700d2, 3 | BRF_GRA },           //  8
	{ "239-c7.c7",    0x400000, 0x010ff4fd, 3 | BRF_GRA },           //  9
	{ "239-c8.c8",    0x400000, 0xdb60460e, 3 | BRF_GRA },           // 10

	{ "239-m1.m1",    0x020000, 0xd31a3aea, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "239-v1.v1",    0x400000, 0x1b8d5bf7, 5 | BRF_SND },           // 12 Sound data
	{ "239-v2.v2",    0x400000, 0x74cf0a70, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(blazstarcm, blazstarcm, neogeo)
STD_ROM_FN(blazstarcm)

struct BurnDriver BurnDrvblazstarcm = {
	"blazstarcm", "blazstar", "neogeo", NULL, "2020",
	"Blazing Star (Stage Select Hack)\0", NULL, "Clear Paper, NeoCverA, JustinG", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_HORSHOOT, 0,
	NULL, blazstarcmRomInfo, blazstarcmRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Breakers Revenge (Boss Hack)
// Enable Bai-Hu, with portrait - Hack by yumeji1768 [2ch]

static struct BurnRomInfo breakrevbhRomDesc[] = {
	{ "245-p1bh.p1",  0x200000, 0x52c978b5, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "245-s1.s1",    0x020000, 0xe7660a5d, 2 | BRF_GRA },           //  1 Text layer tiles

	/* The chip labels and sizes are correct */
	{ "245-c1.c1",    0x400000, 0x68d4ae76, 3 | BRF_GRA },           //  2 Sprite data
	{ "245-c2.c2",    0x400000, 0xfdee05cd, 3 | BRF_GRA },           //  3
	{ "245-c3.c3",    0x400000, 0x645077f3, 3 | BRF_GRA },           //  4
	{ "245-c4.c4",    0x400000, 0x63aeb74c, 3 | BRF_GRA },           //  5
	{ "245-c5.c5",    0x400000, 0xb5f40e7f, 3 | BRF_GRA },           //  6
	{ "245-c6.c6",    0x400000, 0xd0337328, 3 | BRF_GRA },           //  7

	{ "245-m1.m1",    0x020000, 0x00f31c66, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	{ "245-v1.v1",    0x400000, 0xe255446c, 5 | BRF_SND },           //  9 Sound data
	{ "245-v2.v2",    0x400000, 0x9068198a, 5 | BRF_SND },           // 10
};

STDROMPICKEXT(breakrevbh, breakrevbh, neogeo)
STD_ROM_FN(breakrevbh)

struct BurnDriver BurnDrvbreakrevbh = {
	"breakrevbh", "breakrev", "neogeo", NULL, "1998",
	"Breakers Revenge (Boss Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, breakrevbhRomInfo, breakrevbhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Breakers Revenge - Extra Mode (Hack)

static struct BurnRomInfo brkrevextRomDesc[] = {
	{ "245-p1ext.p1", 0x200000, 0x603b47a4, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "245-s1.s1",    0x020000, 0xe7660a5d, 2 | BRF_GRA },           //  1 Text layer tiles

	/* The chip labels and sizes are correct */
	{ "245-c1.c1",    0x400000, 0x68d4ae76, 3 | BRF_GRA },           //  2 Sprite data
	{ "245-c2.c2",    0x400000, 0xfdee05cd, 3 | BRF_GRA },           //  3
	{ "245-c3.c3",    0x400000, 0x645077f3, 3 | BRF_GRA },           //  4
	{ "245-c4.c4",    0x400000, 0x63aeb74c, 3 | BRF_GRA },           //  5
	{ "245-c5.c5",    0x400000, 0xb5f40e7f, 3 | BRF_GRA },           //  6
	{ "245-c6.c6",    0x400000, 0xd0337328, 3 | BRF_GRA },           //  7

	{ "245-m1.m1",    0x020000, 0x00f31c66, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	{ "245-v1.v1",    0x400000, 0xe255446c, 5 | BRF_SND },           //  9 Sound data
	{ "245-v2.v2",    0x400000, 0x9068198a, 5 | BRF_SND },           // 10
};

STDROMPICKEXT(brkrevext, brkrevext, neogeo)
STD_ROM_FN(brkrevext)

struct BurnDriver BurnDrvbrkrevext = {
	"brkrevext", "breakrev", "neogeo", NULL, "2018",
	"Breakers Revenge - Extra Mode (Hack)\0", NULL, "Visco", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, brkrevextRomInfo, brkrevextRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Crossed Swords 2 (bootleg CD to cartridge conversion)

static struct BurnRomInfo crswd2blRomDesc[] = {
	{ "054-p1.p1",    0x200000, 0x64836147, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "054-s1.s1",    0x020000, 0x22e02ddd, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "054-c1.c1",    0x400000, 0x8221b712, 3 | BRF_GRA },           //  2 Sprite data
	{ "054-c2.c2",    0x400000, 0xd6c6183d, 3 | BRF_GRA },           //  3

	{ "054-m1.m1",    0x020000, 0x63e28343, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "054-v1.v1",    0x200000, 0x22d4b93b, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(crswd2bl, crswd2bl, neogeo)
STD_ROM_FN(crswd2bl)

struct BurnDriver BurnDrvcrswd2bl = {
	"crswd2bl", NULL, "neogeo", NULL, "1991",
	"Crossed Swords 2 (bootleg CD to cartridge conversion)\0", NULL, "bootleg (Razoola)", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_SCRFIGHT, 0,
	NULL, crswd2blRomInfo, crswd2blRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Crouching Tiger Hidden Dragon 2003 (The King of Fighters 2001 bootleg set 2)

static struct BurnRomInfo cthd2k3aRomDesc[] = {
	{ "5003-p1a.bin", 0x100000, 0x1185fe39, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "5003-p2a.bin", 0x400000, 0xea71faf7, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "5003-s1a.bin", 0x020000, 0x174ccffd, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "5003-c1a.bin", 0x800000, 0xdc90c563, 3 | BRF_GRA },           //  3 Sprite data
	{ "5003-c2a.bin", 0x800000, 0x7b08d331, 3 | BRF_GRA },           //  4
	{ "5003-c3.bin",  0x800000, 0xac4aff71, 3 | BRF_GRA },           //  5
	{ "5003-c4.bin",  0x800000, 0xafef5d66, 3 | BRF_GRA },           //  6
	{ "5003-c5.bin",  0x800000, 0xc7c1ae50, 3 | BRF_GRA },           //  7
	{ "5003-c6.bin",  0x800000, 0x613197f9, 3 | BRF_GRA },           //  8
	{ "5003-c7a.bin", 0x800000, 0xad2d72b3, 3 | BRF_GRA },           //  9
	{ "5003-c8a.bin", 0x800000, 0x8c3fc1b5, 3 | BRF_GRA },           // 10

	{ "5003-m1a.bin", 0x020000, 0xa37cc447, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "262-v1-08-e0.bin",   0x400000, 0x83d49ecf, 5 | BRF_SND },     // 12 Sound data
	{ "5003-v2a.bin",       0x400000, 0x2b498449, 5 | BRF_SND },     // 13
	{ "262-v3-08-e0.bin",   0x400000, 0x2ae38dbe, 5 | BRF_SND },     // 14
	{ "262-v4-08-e0.bin",   0x400000, 0x26ec4dd9, 5 | BRF_SND },     // 15
};

STDROMPICKEXT(cthd2k3a, cthd2k3a, neogeo)
STD_ROM_FN(cthd2k3a)

static void cthd2k3aCallback()
{
	INT32 i, n;

	// P-ROM Encryption
	UINT8 nBank[] = { 0x06, 0x02, 0x04, 0x05, 0x01, 0x03, 0x00, 0x07, 0x27, 0x0E, 0x1C, 0x15, 0x1B, 0x17, 0x0A, 0x0F, 0x16, 0x14, 0x23, 0x0B, 0x22, 0x26, 0x08, 0x24, 0x21, 0x13, 0x1A, 0x0C, 0x19, 0x1D, 0x25, 0x10, 0x09, 0x20, 0x18, 0x1F, 0x1E, 0x12, 0x0D, 0x11 };

	UINT8 *pTemp = (UINT8*)BurnMalloc(0x500000);
	if (pTemp) {
		for (i = 0; i < 0x500000 / 0x20000; i++) {
			memcpy(pTemp + (i * 0x20000), Neo68KROMActive + (nBank[i] * 0x20000), 0x20000);
		}
		memcpy(Neo68KROMActive, pTemp, 0x500000);
		BurnFree(pTemp);
	}

	UINT16 *Rom = (UINT16*)Neo68KROMActive;
	Rom[0xed00e / 2] = BURN_ENDIAN_SWAP_INT16(0x4e71);
	Rom[0xed394 / 2] = BURN_ENDIAN_SWAP_INT16(0x4e71);
	Rom[0xa2b7e / 2] = BURN_ENDIAN_SWAP_INT16(0x4e71);

	// Text ROM
	for (i = 0; i < 0x8000; i++) {
		n = NeoTextROM[nNeoActiveSlot][0x08000 + i];
		NeoTextROM[nNeoActiveSlot][0x08000 + i] = NeoTextROM[nNeoActiveSlot][0x10000 + i];
		NeoTextROM[nNeoActiveSlot][0x10000 + i] = n;
	}

	// Swap bits 15 & 16 in the address of the Z80 ROM
	for (i = 0; i < 0x10000 / 2; i++) {
		n = NeoZ80ROMActive[0x08000 + i];
		NeoZ80ROMActive[0x08000 + i] = NeoZ80ROMActive[0x10000 + i];
		NeoZ80ROMActive[0x10000 + i] = n;
	}

	DoPerm(0);
}

static INT32 cthd2k3aInit()
{
	NeoCallbackActive->pInitialise = cthd2k3aCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvcthd2k3a = {
	"cthd2k3a", "kof2001", "neogeo", NULL, "2003",
	"Crouching Tiger Hidden Dragon 2003 (set 2)\0", NULL, "bootleg", "Neo Geo MVS",
	L"Crouching Tiger Hidden Dragon 2003 (set 2)\0\u81E5\u864E\u85CF\u9F8D\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, cthd2k3aRomInfo, cthd2k3aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	cthd2k3aInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Double Dragon (Boss hack)
// Enable 2 bosses: Duke & Shuko - hack by yumeji1768

static struct BurnRomInfo doubledrbhRomDesc[] = {
	{ "082-p1bh.p1",  0x200000, 0x92826c06, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "082-s1.s1",    0x020000, 0xbef995c5, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "082-c1.c1",    0x200000, 0xb478c725, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "082-c2.c2",    0x200000, 0x2857da32, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "082-c3.c3",    0x200000, 0x8b0d378e, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "082-c4.c4",    0x200000, 0xc7d2f596, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "082-c5.c5",    0x200000, 0xec87bff6, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "082-c6.c6",    0x200000, 0x844a8a11, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "082-c7.c7",    0x100000, 0x727c4d02, 3 | BRF_GRA },           //  8 					/ TC538200
	{ "082-c8.c8",    0x100000, 0x69a5fa37, 3 | BRF_GRA },           //  9 					/ TC538200

	{ "082-m1.m1",    0x020000, 0x10b144de, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "082-v1.v1",    0x200000, 0xcc1128e4, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "082-v2.v2",    0x200000, 0xc3ff5554, 5 | BRF_SND },           // 12 					/ TC5316200
};

STDROMPICKEXT(doubledrbh, doubledrbh, neogeo)
STD_ROM_FN(doubledrbh)

struct BurnDriver BurnDrvdoubledrbh = {
	"doubledrbh", "doubledr", "neogeo", NULL, "1995",
	"Double Dragon (Boss hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, doubledrbhRomInfo, doubledrbhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Double Dragon (Special 2017, hack)
// Modified by: GSC2007
// Version number: Ver 1.0.0311

static struct BurnRomInfo doubledrspRomDesc[] = {
	{ "082-p1sp.p1",  0x100000, 0x8ea8ee3d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "082-p2sp.p2",  0x100000, 0x0e2616ab, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "082-p3sp.p3",  0x020000, 0x8b4839c4, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "082-s1.s1",    0x020000, 0xbef995c5, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "082-c1.c1",    0x200000, 0xb478c725, 3 | BRF_GRA },           //  4 Sprite data
	{ "082-c2.c2",    0x200000, 0x2857da32, 3 | BRF_GRA },           //  5
	{ "082-c3.c3",    0x200000, 0x8b0d378e, 3 | BRF_GRA },           //  6
	{ "082-c4.c4",    0x200000, 0xc7d2f596, 3 | BRF_GRA },           //  7
	{ "082-c5sp.c5",  0x200000, 0xb9c799fe, 3 | BRF_GRA },           //  8
	{ "082-c6sp.c6",  0x200000, 0x11569bc9, 3 | BRF_GRA },           //  9
	{ "082-c7.c7",    0x100000, 0x727c4d02, 3 | BRF_GRA },           // 10
	{ "082-c8.c8",    0x100000, 0x69a5fa37, 3 | BRF_GRA },           // 11

	{ "082-m1.m1",    0x020000, 0x10b144de, 4 | BRF_ESS | BRF_PRG }, // 12 Z80 code

	{ "082-v1.v1",    0x200000, 0xcc1128e4, 5 | BRF_SND },           // 13 Sound data
	{ "082-v2.v2",    0x200000, 0xc3ff5554, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(doubledrsp, doubledrsp, neogeo)
STD_ROM_FN(doubledrsp)

static void doubledrspPatchCallback()
{
	UINT16* rom = (UINT16*)Neo68KROMActive;

	for (INT32 i = 0; i < 0x100000 >> 1; i++) {
		if (rom[i] == 0x4e7d) rom[i] = 0x4e71;
		if (rom[i] == 0x4e7c) rom[i] = 0x4e75;
	}

	rom[0xbff2] = 0x2b7c; // 4ef9
	rom[0xbff3] = 0x0001; // 0091
	rom[0xbff4] = 0x7fee; // 0206
	rom[0xbff5] = 0xa26a; // 4e7d

	rom = (UINT16*)(Neo68KROMActive + 0x200000);

	for (INT32 i = 0; i < 0x20000 >> 1; i++) {
		if (rom[i] == 0x4e7d) rom[i] = 0x4e71;
		if (rom[i] == 0x4e7c) rom[i] = 0x4e75;
	}
}

static INT32 DoubledrspInit()
{
	NeoCallbackActive->pInitialise = doubledrspPatchCallback;

	INT32 nRet = NeoInit();
	if (0 == nRet) NeoMapExtraRom(0x200000, 0x20000);

	return nRet;
}

struct BurnDriver BurnDrvdoubledrsp = {
	"doubledrsp", "doubledr", "neogeo", NULL, "2017",
	"Double Dragon (Special 2017, hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, doubledrspRomInfo, doubledrspRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	DoubledrspInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Far East of Eden - Kabuki Klash / Tengai Makyou - Shin Den (Add hidden characters)

static struct BurnRomInfo kabukiklbRomDesc[] = {
	{ "092-p1b.p1",   0x200000, 0x9e814a43, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC5316200

	{ "092-s1.s1",    0x020000, 0xa3d68ee2, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "092-c1.c1",    0x400000, 0x2a9fab01, 3 | BRF_GRA },           //  2 Sprite data		/ mask rom TC5332205
	{ "092-c2.c2",    0x400000, 0x6d2bac02, 3 | BRF_GRA },           //  3 					/ mask rom TC5332205
	{ "092-c3.c3",    0x400000, 0x5da735d6, 3 | BRF_GRA },           //  4 					/ mask rom TC5332205
	{ "092-c4.c4",    0x400000, 0xde07f997, 3 | BRF_GRA },           //  5 					/ mask rom TC5332205

	{ "092-m1.m1",    0x020000, 0x91957ef6, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ mask rom TC531001

	{ "092-v1.v1",    0x200000, 0x69e90596, 5 | BRF_SND },           //  7 Sound data		/ mask rom TC5316200
	{ "092-v2.v2",    0x200000, 0x7abdb75d, 5 | BRF_SND },           //  8 					/ mask rom TC5316200
	{ "092-v3.v3",    0x200000, 0xeccc98d3, 5 | BRF_SND },           //  9 					/ mask rom TC5316200
	{ "092-v4.v4",    0x100000, 0xa7c9c949, 5 | BRF_SND },           // 10 					/ mask rom TC538200
};

STDROMPICKEXT(kabukiklb, kabukiklb, neogeo)
STD_ROM_FN(kabukiklb)

struct BurnDriver BurnDrvkabukiklb = {
	"kabukiklb", "kabukikl", "neogeo", NULL, "1995",
	"Far East of Eden - Kabuki Klash / Tengai Makyou - Shin Den (Add hidden characters)\0", NULL, "Ydmis / Creamymami[EGCG]", "Neo Geo MVS",
	L"Kabuki Klash - far east of eden\0\u5929\u5916\u9B54\u5883 - \u771F\u4F1D (Add hidden characters)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, kabukiklbRomInfo, kabukiklbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Fatal Fury - King of Fighters / Garou Densetsu - shukumei no tatakai (Boss Hack by Yumeji1768)
/* MVS AND AES VERSION */

static struct BurnRomInfo fatfury1bhRomDesc[] = {
	{ "033-p1bh.p1",  0x080000, 0xfc051e1d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC534200
	{ "033-p2.p2",    0x020000, 0xc473af1c, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC531024

	{ "033-s1.s1",    0x020000, 0x3c3bdf8c, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "033-c1.c1",    0x100000, 0x74317e54, 3 | BRF_GRA },           //  3 Sprite data 		/ TC538200
	{ "033-c2.c2",    0x100000, 0x5bb952f3, 3 | BRF_GRA },           //  4 					/ TC538200
	{ "033-c3.c3",    0x100000, 0x9b714a7c, 3 | BRF_GRA },           //  5 					/ TC538200
	{ "033-c4.c4",    0x100000, 0x9397476a, 3 | BRF_GRA },           //  6 					/ TC538200

	{ "033-m1.m1",    0x020000, 0x5be10ffd, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code 		/ TC531001

	{ "033-v1.v1",    0x100000, 0x212fd20d, 5 | BRF_SND },           //  8 Sound data 		/ TC538200
	{ "033-v2.v2",    0x100000, 0xfa2ae47f, 5 | BRF_SND },           //  9 					/ TC538200
};

STDROMPICKEXT(fatfury1bh, fatfury1bh, neogeo)
STD_ROM_FN(fatfury1bh)

struct BurnDriver BurnDrvFatfury1bh = {
	"fatfury1bh", "fatfury1", "neogeo", NULL, "1992",
	"Fatal Fury - King of Fighters / Garou Densetsu - shukumei no tatakai (Boss Hack by Yumeji)\0", NULL, "SNK", "Neo Geo MVS",
	L"Fatal Fury - King of Fighters (Boss Hack by Yumeji)\0\u9913\u72FC\u4F1D\u8AAC - \u5BBF\u547D\u306E\u95D8\u3044 (NGM-033)(NGH-033)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, fatfury1bhRomInfo, fatfury1bhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Fatal Fury Special / Garou Densetsu Special (Optional Hidden Character Third Edition)

static struct BurnRomInfo fatfurspbsRomDesc[] = {
	{ "058-p1bs.p1",  0x100000, 0x8cd18f7f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ mask rom TC538200
	{ "058-p2.sp2",   0x080000, 0xd7c71a6b, 1 | BRF_ESS | BRF_PRG }, //  1					/ mask rom TC534200

	{ "058-s1.s1",    0x020000, 0x2df03197, 2 | BRF_GRA },           //  2 Text layer tiles / mask rom TC531000

	{ "058-c1.c1",    0x200000, 0x044ab13c, 3 | BRF_GRA },           //  3 Sprite data		/ mask rom TC5316200
	{ "058-c2.c2",    0x200000, 0x11e6bf96, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "058-c3.c3",    0x200000, 0x6f7938d5, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200
	{ "058-c4.c4",    0x200000, 0x4ad066ff, 3 | BRF_GRA },           //  6 					/ mask rom TC5316200
	{ "058-c5.c5",    0x200000, 0x49c5e0bf, 3 | BRF_GRA },           //  7 					/ mask rom TC5316200
	{ "058-c6.c6",    0x200000, 0x8ff1f43d, 3 | BRF_GRA },           //  8					/ mask rom TC5316200

	{ "058-m1.m1",    0x020000, 0xccc5186e, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code			/ mask rom TC531001

	{ "058-v1.v1",    0x200000, 0x55d7ce84, 5 | BRF_SND },           // 10 Sound data		/ mask rom TC5316200
	{ "058-v2.v2",    0x200000, 0xee080b10, 5 | BRF_SND },           // 11 					/ mask rom TC5316200
	{ "058-v3.v3",    0x100000, 0xf9eb3d4a, 5 | BRF_SND },           // 12 					/ mask rom TC538200
};

STDROMPICKEXT(fatfurspbs, fatfurspbs, neogeo)
STD_ROM_FN(fatfurspbs)

struct BurnDriver BurnDrvFatfurspbs = {
	"fatfurspbs", "fatfursp", "neogeo", NULL, "1993",
	"Fatal Fury Special / Garou Densetsu Special (Optional Hidden Character Third Edition)\0", NULL, "Yumeji", "Neo Geo MVS",
	L"Fatal Fury Special\0\u9913\u72FC\u4F1D\u8AAC Special (Optional Hidden Character Third Edition)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, fatfurspbsRomInfo, fatfurspbsRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Fatal Fury 3 - Road to the Final Victory / Garou Densetsu 3 - haruka-naru tatakai (Ancient Battles Resurgence 2015-03-13)

static struct BurnRomInfo fatfury3bhRomDesc[] = {
	{ "069-p1bh.p1",  0x100000, 0xb8362f59, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC538200
	{ "069-sp2.sp2",  0x200000, 0xdbe963ed, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "069-s1.s1",    0x020000, 0x0b33a800, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "069-c1.c1",    0x400000, 0xe302f93c, 3 | BRF_GRA },           //  3 Sprite data		/ TC5332205
	{ "069-c2.c2",    0x400000, 0x1053a455, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "069-c3.c3",    0x400000, 0x1c0fde2f, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "069-c4.c4",    0x400000, 0xa25fc3d0, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "069-c5.c5",    0x200000, 0xb3ec6fa6, 3 | BRF_GRA },           //  7 					/ TC5332205
	{ "069-c6.c6",    0x200000, 0x69210441, 3 | BRF_GRA },           //  8 					/ TC5332205

	{ "069-m1.m1",    0x020000, 0xfce72926, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code			/ TC531001

	{ "069-v1.v1",    0x400000, 0x2bdbd4db, 5 | BRF_SND },           // 10 Sound data		/ TC5332204
	{ "069-v2.v2",    0x400000, 0xa698a487, 5 | BRF_SND },           // 11 					/ TC5332204
	{ "069-v3.v3",    0x200000, 0x581c5304, 5 | BRF_SND },           // 12 					/ TC5316200
};

STDROMPICKEXT(fatfury3bh, fatfury3bh, neogeo)
STD_ROM_FN(fatfury3bh)

struct BurnDriver BurnDrvFatfury3bh = {
	"fatfury3bh", "fatfury3", "neogeo", NULL, "1995",
	"Fatal Fury 3 - Road to the Final Victory / Garou Densetsu 3 - haruka-naru tatakai (Ancient Battles Resurgence 2015-03-13)\0", NULL, "Yumeji", "Neo Geo MVS",
	L"Fatal Fury 3 - Road to the Final Victory\0\u9913\u72FC\u4F1D\u8AAC\uFF13 (Ancient Battles Resurgence 2015-03-13)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, fatfury3bhRomInfo, fatfury3bhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Garou - Mark of the Wolves (Enable hidden characters)

static struct BurnRomInfo garoubRomDesc[] = {
	{ "kf.neo-sma",   0x040000, 0x98bc93dc, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "253-ep1.p1",   0x200000, 0xea3171a4, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "253-ep2.p2",   0x200000, 0x382f704b, 1 | BRF_ESS | BRF_PRG }, //  2
	{ "253-ep3.p3",   0x200000, 0xe395bfdd, 1 | BRF_ESS | BRF_PRG }, //  3
	{ "253-ep4b.p4",  0x200000, 0xc1fb6ff4, 1 | BRF_ESS | BRF_PRG }, //  4

	{ "253-c1.c1",    0x800000, 0x0603e046, 3 | BRF_GRA },           //  5 Sprite data
	{ "253-c2.c2",    0x800000, 0x0917d2a4, 3 | BRF_GRA },           //  6
	{ "253-c3.c3",    0x800000, 0x6737c92d, 3 | BRF_GRA },           //  7
	{ "253-c4.c4",    0x800000, 0x5ba92ec6, 3 | BRF_GRA },           //  8
	{ "253-c5.c5",    0x800000, 0x3eab5557, 3 | BRF_GRA },           //  9
	{ "253-c6.c6",    0x800000, 0x308d098b, 3 | BRF_GRA },           // 10
	{ "253-c7.c7",    0x800000, 0xc0e995ae, 3 | BRF_GRA },           // 11
	{ "253-c8.c8",    0x800000, 0x21a11303, 3 | BRF_GRA },           // 12

	{ "253-m1.m1",    0x040000, 0x36a806be, 4 | BRF_ESS | BRF_PRG }, // 13 Z80 code

	{ "253-v1.v1",    0x400000, 0x263e388c, 5 | BRF_SND },           // 14 Sound data
	{ "253-v2.v2",    0x400000, 0x2c6bc7be, 5 | BRF_SND },           // 15
	{ "253-v3.v3",    0x400000, 0x0425b27d, 5 | BRF_SND },           // 16
	{ "253-v4.v4",    0x400000, 0xa54be8a9, 5 | BRF_SND },           // 17
};

STDROMPICKEXT(garoub, garoub, neogeo)
STD_ROM_FN(garoub)

struct BurnDriver BurnDrvGaroub = {
	"garoub", "garou", "neogeo", NULL, "1999",
	"Garou - Mark of the Wolves (Enable hidden characters)\0", NULL, "Ydmis", "Neo Geo MVS",
	L"Garou\0\u9913\u72FC - mark of the wolves (Enable hidden characters)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_VSFIGHT, 0,
	NULL, garoubRomInfo, garoubRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	garouInit, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Idol Mahjong - final romance 2 (Neo CD Conversion)

static struct BurnRomInfo froman2bRomDesc[] = {
	{ "098.p1", 0x080000, 0x09675541, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "098.s1", 0x020000, 0x0e6a7c73, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "098.c1", 0x400000, 0x29148bf7, 3 | BRF_GRA },           //  2 Sprite data
	{ "098.c2", 0x400000, 0x226b1263, 3 | BRF_GRA },           //  3

	{ "098.m1", 0x020000, 0xda4878cf, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	{ "098.v1", 0x100000, 0x6f8ccddc, 5 | BRF_SND },           //  9 Sound data
};

STDROMPICKEXT(froman2b, froman2b, neogeo)
STD_ROM_FN(froman2b)

struct BurnDriver BurnDrvfroman2b = {
	"froman2b", NULL, "neogeo", NULL, "1995",
	"Idol Mahjong - final romance 2 (Neo CD Conversion)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HACK, 2, HARDWARE_SNK_NEOGEO, GBF_MAHJONG, 0,
	NULL, froman2bRomInfo, froman2bRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Last Hope (Neo CD conversion)

static struct BurnRomInfo lhcdbRomDesc[] = {
	{ "300-p1ch.bin", 0x100000, 0x0f7405d7, 1 | BRF_ESS | BRF_PRG }, //  0 68K Code

	{ "300-s1ch.bin", 0x020000, 0x298495d6, 2 | BRF_GRA },		 	 //  1 Text data

	{ "300-c1ch.bin", 0x400000, 0x554e6b73, 3 | BRF_GRA },		 	 //  2 Sprite data
	{ "300-c2ch.bin", 0x400000, 0x7c84b0fc, 3 | BRF_GRA },		 	 //  3
	{ "300-c3ch.bin", 0x400000, 0x28ec7555, 3 | BRF_GRA },		 	 //  4
	{ "300-c4ch.bin", 0x400000, 0x8b7c236b, 3 | BRF_GRA },		 	 //  5

	{ "300-m1ch.bin", 0x020000, 0xd9f6c153, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "300-v1ch.bin", 0x400000, 0xde563ec3, 5 | BRF_SND },		 	 //  7 Sound data
	{ "300-v2ch.bin", 0x400000, 0x93478033, 5 | BRF_SND },		 	 //  8
	{ "300-v3ch.bin", 0x400000, 0xe0fc99ca, 5 | BRF_SND },		 	 //  9
};

STDROMPICKEXT(lhcdb, lhcdb, neogeo)
STD_ROM_FN(lhcdb)

struct BurnDriver BurnDrvlhcdb = {
	"lhcdb", NULL, "neogeo", NULL, "2007",
	"Last Hope CD Beta (Neo CD conversion)\0", "Imperfect graphics", "NG:Dev.Team", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HACK, 2, HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, lhcdbRomInfo, lhcdbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Last Hope (bootleg AES to MVS conversion, no coin support)
// wasn't actually released on MVS but bootleg carts have been sold, this doesn't accept coins, runs like a console game

static struct BurnRomInfo lasthopeRomDesc[] = {
	{ "ngdt-300-p1.bin", 0x100000, 0x3776a88f, 1 | BRF_ESS | BRF_PRG }, //  0 68K Code

	{ "ngdt-300-s1.bin", 0x010000, 0x0c0ff9e6, 2 | BRF_GRA },		 	//  1 Text data

	{ "ngdt-300-c1.bin", 0x400000, 0x53ef41b5, 3 | BRF_GRA },		 	//  2 Sprite data
	{ "ngdt-300-c2.bin", 0x400000, 0xf9b15ab3, 3 | BRF_GRA },		 	//  3
	{ "ngdt-300-c3.bin", 0x400000, 0x50cc21cf, 3 | BRF_GRA },		 	//  4
	{ "ngdt-300-c4.bin", 0x400000, 0x8486ad9e, 3 | BRF_GRA },		 	//  5

	{ "ngdt-300-m1.bin", 0x020000, 0x113c870f, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "ngdt-300-v1.bin", 0x200000, 0xb765bafe, 5 | BRF_SND },		 	//  7 Sound data
	{ "ngdt-300-v2.bin", 0x200000, 0x9fd0d559, 5 | BRF_SND },		 	//  8
	{ "ngdt-300-v3.bin", 0x200000, 0x6d5107e2, 5 | BRF_SND },		 	//  9
};

STDROMPICKEXT(lasthope, lasthope, neogeo)
STD_ROM_FN(lasthope)

struct BurnDriver BurnDrvlasthope = {
	"lasthope", NULL, "neogeo", NULL, "2005",
	"Last Hope (bootleg AES to MVS conversion, no coin support)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_HORSHOOT, 0,
	NULL, lasthopeRomInfo, lasthopeRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Magical Drop III (Secret Character Hack)
// Enable all hidden characters - hack by GSC2007

static struct BurnRomInfo magdrop3bhRomDesc[] = {
	{ "233-p1bh.p1", 	0x100000, 0x80bfe2a9, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "233-s1.s1",    	0x020000, 0x7399e68a, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "233-c1.c1",    	0x400000, 0x65e3f4c4, 3 | BRF_GRA },           //  2 Sprite data
	{ "233-c2.c2",    	0x400000, 0x35dea6c9, 3 | BRF_GRA },           //  3
	{ "233-c3.c3",    	0x400000, 0x0ba2c502, 3 | BRF_GRA },           //  4
	{ "233-c4.c4",    	0x400000, 0x70dbbd6d, 3 | BRF_GRA },           //  5

	{ "233-m1.m1",    	0x020000, 0x5beaf34e, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "233-v1.v1",    	0x400000, 0x58839298, 5 | BRF_SND },           //  7 Sound data
	{ "233-v2.v2",    	0x080000, 0xd5e30df4, 5 | BRF_SND },           //  8
};

STDROMPICKEXT(magdrop3bh, magdrop3bh, neogeo)
STD_ROM_FN(magdrop3bh)

struct BurnDriver BurnDrvmagdrop3bh = {
	"magdrop3bh", "magdrop3", "neogeo", NULL, "1997",
	"Magical Drop III (Secret Character Hack)\0", NULL, "hack", "Neo Geo MVS",
	L"Magical Drop III\0\u30DE\u30B8\u30AB\u30EB\u30C9\u30ED\u30C3\u30D7III (Secret Character Hack)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, magdrop3bhRomInfo, magdrop3bhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Matrimelee / Shin Gouketsuji Ichizoku Toukon (Enable Hidden Characters V2)

static struct BurnRomInfo matrimbhRomDesc[] = {
	{ "266-p1bh.p1",  0x100000, 0x5f7b6942, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "266-p2.sp2",   0x400000, 0xa14b1906, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "266-c1.c1",    0x800000, 0x505f4e30, 3 | BRF_GRA },           //  2 Sprite data
	{ "266-c2.c2",    0x800000, 0x3cb57482, 3 | BRF_GRA },           //  3
	{ "266-c3.c3",    0x800000, 0xf1cc6ad0, 3 | BRF_GRA },           //  4
	{ "266-c4.c4",    0x800000, 0x45b806b7, 3 | BRF_GRA },           //  5
	{ "266-c5.c5",    0x800000, 0x9a15dd6b, 3 | BRF_GRA },           //  6
	{ "266-c6.c6",    0x800000, 0x281cb939, 3 | BRF_GRA },           //  7
	{ "266-c7.c7",    0x800000, 0x4b71f780, 3 | BRF_GRA },           //  8
	{ "266-c8.c8",    0x800000, 0x29873d33, 3 | BRF_GRA },           //  9

	/* Encrypted */
	{ "266-m1.m1",    0x020000, 0x456c3e6c, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	/* Encrypted */
	{ "266-v1.v1",    0x800000, 0xa4f83690, 5 | BRF_SND },           // 11 Sound data
	{ "266-v2.v2",    0x800000, 0xd0f69eda, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(matrimbh, matrimbh, neogeo)
STD_ROM_FN(matrimbh)

struct BurnDriver BurnDrvMatrimbh = {
	"matrimbh", "matrim", "neogeo", NULL, "2002",
	"Matrimelee / Shin Gouketsuji Ichizoku Toukon (Enable Hidden Characters V2)\0", NULL, "Creamymami[EGCG]", "Neo Geo MVS",
	L"\u65B0\u8C6A\u8840\u5BFA\u4E00\u65CF - \u95D8\u5A5A\0Matrimelee (Enable Hidden Characters V2)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_PWRINST,
	NULL, matrimbhRomInfo, matrimbhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	matrimInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// GOTVG  Metal Slug - Super Vehicle-001 (Multifunction Hack, 20180430)
// mslugs18 in HBMAME.

static struct BurnRomInfo mslugdgRomDesc[] = {
	{ "201-p1.bin",    0x200000, 0x0a739521, 1 | BRF_ESS | BRF_PRG }, //  0 68K code		/ TC5316200

	{ "201-s1.s1",    0x020000, 0x2f55958d, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "201-c1.c1",    0x400000, 0x72813676, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332205
	{ "201-c2.c2",    0x400000, 0x96f62574, 3 | BRF_GRA },           //  3 					/ TC5332205
	{ "201-c3.c3",    0x400000, 0x5121456a, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "201-c4.c4",    0x400000, 0xf4ad59a3, 3 | BRF_GRA },           //  5 					/ TC5332205

	{ "201-m1.m1",    0x020000, 0xc28b3253, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "201-v1.v1",    0x400000, 0x23d22ed1, 5 | BRF_SND },           //  7 Sound data		/ TC5332204
	{ "201-v2.v2",    0x400000, 0x472cf9db, 5 | BRF_SND },           //  8 					/ TC5332204
};

STDROMPICKEXT(mslugdg, mslugdg, neogeo)
STD_ROM_FN(mslugdg)

struct BurnDriver BurnDrvmslugdg = {
	"mslugdg", "mslug", "neogeo", NULL, "2018",
	"Metal Slug (Multifunction Hack, 20180430)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslugdgRomInfo, mslugdgRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// GOTVG Metal Slug 2 - Super Vehicle-001/II (Multifunction Hack, 20170509)
// mslug2s26 in HBMAME.

static struct BurnRomInfo mslug2dgRomDesc[] = {
	{ "241-p1.bin",   0x100000, 0x00c455e7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "241-p2.bin",   0x200000, 0x1bf6b12a, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "241-s1.s1",    0x020000, 0xf3d32f0f, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "241-c1.c1",    0x800000, 0x394b5e0d, 3 | BRF_GRA },           //  3 Sprite data
	{ "241-c2.c2",    0x800000, 0xe5806221, 3 | BRF_GRA },           //  4
	{ "241-c3.c3",    0x800000, 0x9f6bfa6f, 3 | BRF_GRA },           //  5
	{ "241-c4.c4",    0x800000, 0x7d3e306f, 3 | BRF_GRA },           //  6

	{ "241-m1.m1",    0x020000, 0x94520ebd, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code

	{ "241-v1.v1",    0x400000, 0x99ec20e8, 5 | BRF_SND },           //  8 Sound data
	{ "241-v2.v2",    0x400000, 0xecb16799, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(mslug2dg, mslug2dg, neogeo)
STD_ROM_FN(mslug2dg)

struct BurnDriver BurnDrvmslug2dg = {
	"mslug2dg", "mslug2", "neogeo", NULL, "2017",
	"Metal Slug 2 - Super Vehicle-001/II (Multifunction Hack, 20170509)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug2dgRomInfo, mslug2dgRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 2 - Super Vehicle-001/II (Friendly Fire FC2 Ver.2 20210415, hack by CZXINc)
// GOTVG

static struct BurnRomInfo mslug2fmRomDesc[] = {
	{ "241-p1fm.bin", 0x100000, 0x487173f6, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "241-p2fm.bin", 0x200000, 0x37a118fc, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "241-s1.s1",    0x020000, 0xf3d32f0f, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "241-c1.c1",    0x800000, 0x394b5e0d, 3 | BRF_GRA },           //  3 Sprite data
	{ "241-c2.c2",    0x800000, 0xe5806221, 3 | BRF_GRA },           //  4
	{ "241-c3.c3",    0x800000, 0x9f6bfa6f, 3 | BRF_GRA },           //  5
	{ "241-c4.c4",    0x800000, 0x7d3e306f, 3 | BRF_GRA },           //  6

	{ "241-m1.m1",    0x020000, 0x94520ebd, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code

	{ "241-v1.v1",    0x400000, 0x99ec20e8, 5 | BRF_SND },           //  8 Sound data
	{ "241-v2.v2",    0x400000, 0xecb16799, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(mslug2fm, mslug2fm, neogeo)
STD_ROM_FN(mslug2fm)

struct BurnDriver BurnDrvmslug2fm = {
	"mslug2fm", "mslug2", "neogeo", NULL, "2021",
	"Metal Slug 2 - Super Vehicle-001/II (Friendly Fire FC2 Ver.2 20210415, hack by CZXINc)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug2fmRomInfo, mslug2fmRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 2 - Super Vehicle-001/II (Turbo)
// ID-0941 - community produced hack

static struct BurnRomInfo mslug2tRomDesc[] = {
	{ "941-p1.p1",		0x100000, 0xdf5d6fbc, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "241-p2.sp2",		0x200000, 0x38883f44, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "241-s1.s1",		0x020000, 0xf3d32f0f, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "241-c1.c1",		0x800000, 0x394b5e0d, 3 | BRF_GRA },           //  3 Sprite data
	{ "241-c2.c2",		0x800000, 0xe5806221, 3 | BRF_GRA },           //  4
	{ "241-c3.c3",		0x800000, 0x9f6bfa6f, 3 | BRF_GRA },           //  5
	{ "241-c4.c4",		0x800000, 0x7d3e306f, 3 | BRF_GRA },           //  6

	{ "241-m1.m1",		0x020000, 0x94520ebd, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code

	{ "241-v1.v1",		0x400000, 0x99ec20e8, 5 | BRF_SND },           //  8 Sound data
	{ "241-v2.v2",		0x400000, 0xecb16799, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(mslug2t, mslug2t, neogeo)
STD_ROM_FN(mslug2t)

struct BurnDriver BurnDrvMSlug2t = {
	"mslug2t", "mslug2", "neogeo", NULL, "2016",
	"Metal Slug 2 - Super Vehicle-001/II (Turbo)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug2tRomInfo, mslug2tRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug X - Super Vehicle-001 (AzStar Soda Remix FC2 Version, hack)

static struct BurnRomInfo mslugxsrfRomDesc[] = {
	{ "250-p1srf.bin",	0x100000, 0xaed327fe, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "250-p2srf.bin",  0x400000, 0x4389f47d, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "250-s1.s1",    	0x020000, 0xfb6f441d, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "250-c1.c1",    	0x800000, 0x09a52c6f, 3 | BRF_GRA },           //  3 Sprite data
	{ "250-c2.c2",    	0x800000, 0x31679821, 3 | BRF_GRA },           //  4
	{ "250-c3.c3",    	0x800000, 0xfd602019, 3 | BRF_GRA },           //  5
	{ "250-c4.c4",    	0x800000, 0x31354513, 3 | BRF_GRA },           //  6
	{ "250-c5.c5",    	0x800000, 0xa4b56124, 3 | BRF_GRA },           //  7
	{ "250-c6.c6",    	0x800000, 0x83e3e69d, 3 | BRF_GRA },           //  8

	{ "250-m1.m1",    	0x020000, 0xfd42a842, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "250-v1.v1",    	0x400000, 0xc79ede73, 5 | BRF_SND },           // 10 Sound data
	{ "250-v2.v2",    	0x400000, 0xea9aabe1, 5 | BRF_SND },           // 11
	{ "250-v3.v3",    	0x200000, 0x2ca65102, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(mslugxsrf, mslugxsrf, neogeo)
STD_ROM_FN(mslugxsrf)

struct BurnDriver BurnDrvMSlugxsrf = {
	"mslugxsrf", "mslugx", "neogeo", NULL, "2020",
	"Metal Slug X - Super Vehicle-001 (AzStar Soda Remix FC2 Version, hack)\0", NULL, "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslugxsrfRomInfo, mslugxsrfRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslugxInit, NeoExit, NeoFrame, NeoRender, mslugxScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 3 (Enhanced Violence Version)
// hack by EEZEZY

static struct BurnRomInfo mslug3vRomDesc[] = {
	{ "256-pg1.p1",   0x100000, 0x47f9aeea, 1 | BRF_ESS | BRF_PRG }, //  1 				/ TC5332202
	{ "256-pg2.p2",   0x400000, 0x1f3d8ce8, 1 | BRF_ESS | BRF_PRG }, //  2 				/ TC5332202

	{ "256-c1.c1",    0x800000, 0x5a79c34e, 3 | BRF_GRA },           //  3 Sprite data	/ TC5364202
	{ "256-c2.c2",    0x800000, 0x944c362c, 3 | BRF_GRA },           //  4 				/ TC5364202
	{ "256-c3.c3",    0x800000, 0x6e69d36f, 3 | BRF_GRA },           //  5 				/ TC5364202
	{ "256-c4.c4",    0x800000, 0xb755b4eb, 3 | BRF_GRA },           //  6 				/ TC5364202
	{ "256-c5.c5",    0x800000, 0x7aacab47, 3 | BRF_GRA },           //  7 				/ TC5364202
	{ "256-c6.c6",    0x800000, 0xc698fd5d, 3 | BRF_GRA },           //  8 				/ TC5364202
	{ "256-c7.c7",    0x800000, 0xcfceddd2, 3 | BRF_GRA },           //  9 				/ TC5364202
	{ "256-c8.c8",    0x800000, 0x4d9be34c, 3 | BRF_GRA },           // 10 				/ TC5364202

	{ "256-m1.m1",    0x080000, 0xeaeec116, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code		/ mask rom TC534000

	{ "256-v1.v1",    0x400000, 0xf2690241, 5 | BRF_SND },           // 12 Sound data 	/ TC5332204
	{ "256-v2.v2",    0x400000, 0x7e2a10bd, 5 | BRF_SND },           // 13 				/ TC5332204
	{ "256-v3.v3",    0x400000, 0x0eaec17c, 5 | BRF_SND },           // 14 				/ TC5332204
	{ "256-v4.v4",    0x400000, 0x9b4b22d4, 5 | BRF_SND },           // 15 				/ TC5332204
};

STDROMPICKEXT(mslug3v, mslug3v, neogeo)
STD_ROM_FN(mslug3v)

struct BurnDriver BurnDrvmslug3v = {
	"mslug3v", "mslug3", "neogeo", NULL, "2012",
	"Metal Slug 3 (NGH-2560) (Enhanced Violence Version, hack by EEZEZY)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug3vRomInfo, mslug3vRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug3hInit, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 3 (Multifunction Hack, 20190119)
// FBA4DROID/GOTVG
// Fixed the max attack bars in mslug3g have not been visible (s1) by alicemsu (20220614)

static struct BurnRomInfo mslug3gRomDesc[] = {
	{ "256-ph1g.p1",	0x100000, 0xb23bd9b7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "256-ph2g.sp2",	0x400000, 0x8053a3fb, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "256-s1g.s1",		0x020000, 0xaf90d166, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "256-c1d.c1",		0x800000, 0x3540398c, 3 | BRF_GRA },           //  3 Sprite data
	{ "256-c2d.c2",		0x800000, 0xbdd220f0, 3 | BRF_GRA },           //  4
	{ "256-c3d.c3",		0x800000, 0xbfaade82, 3 | BRF_GRA },           //  5
	{ "256-c4d.c4",		0x800000, 0x1463add6, 3 | BRF_GRA },           //  6
	{ "256-c5d.c5",		0x800000, 0x48ca7f28, 3 | BRF_GRA },           //  7
	{ "256-c6d.c6",		0x800000, 0x806eb36f, 3 | BRF_GRA },           //  8
	{ "256-c7n.c7",		0x800000, 0xed559fac, 3 | BRF_GRA },           //  9
	{ "256-c8n.c8",		0x800000, 0x1c52378b, 3 | BRF_GRA },           // 10

	{ "256-m1.m1",		0x080000, 0xeaeec116, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "256-v1.v1",		0x400000, 0xf2690241, 5 | BRF_SND },           // 12 Sound data
	{ "256-v2.v2",		0x400000, 0x7e2a10bd, 5 | BRF_SND },           // 13
	{ "256-v3.v3",		0x400000, 0x0eaec17c, 5 | BRF_SND },           // 14
	{ "256-v4.v4",		0x400000, 0x9b4b22d4, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(mslug3g, mslug3g, neogeo)
STD_ROM_FN(mslug3g)

struct BurnDriver BurnDrvmslug3g = {
	"mslug3g", "mslug3", "neogeo", NULL, "2019",
	"Metal Slug 3 (Multifunction Hack, 20190119)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug3gRomInfo, mslug3gRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 4 (Multifunction Hack, 20171225)
// GOTVG

static struct BurnRomInfo mslug4dgRomDesc[] = {
	{ "263-p1.bin",  0x100000, 0x36dfa877, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "263-p2.bin",  0x400000, 0xcf6feb75, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "263.s1",    	 0x020000, 0xa9446774, 2 | BRF_GRA },			//  2 Text layer tiles

	{ "263nd.c1",	 0x800000, 0x361ddda6, 3 | BRF_GRA },			//  3 Sprite data
	{ "263nd.c2",	 0x800000, 0x15d192c4, 3 | BRF_GRA },			//  4
	{ "263-c3.bin",	 0x800000, 0xe296612c, 3 | BRF_GRA },			//  5
	{ "263-c4.bin",	 0x800000, 0xc22e3ad0, 3 | BRF_GRA },			//  6
	{ "263-c5.bin",	 0x400000, 0x288cd91f, 3 | BRF_GRA },			//  7
	{ "263-c6.bin",	 0x400000, 0xb015aeee, 3 | BRF_GRA },			//  8

	{ "263nd.m1",    0x020000, 0xef5db532, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "263-v1.bin",	 0x400000, 0x8cb5a9ef, 5 | BRF_SND }, 			// 10 Sound data
	{ "263-v2.bin",	 0x400000, 0x94217b1e, 5 | BRF_SND },			// 11
	{ "263-v3.bin",	 0x400000, 0x7616fcec, 5 | BRF_SND },			// 12
	{ "263-v4.bin",	 0x400000, 0xc5967f91, 5 | BRF_SND }, 			// 13
};

STDROMPICKEXT(mslug4dg, mslug4dg, neogeo)
STD_ROM_FN(mslug4dg)

struct BurnDriver BurnDrvmslug4dg = {
	"mslug4dg", "mslug4", "neogeo", NULL, "2017",
	"Metal Slug 4 (Multifunction Hack, 20171225)\0", NULL, "hack", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug4dgRomInfo, mslug4dgRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 4 (Last Bullet Remix Hack)
// GOTVG

static struct BurnRomInfo mslug4lwRomDesc[] = {
	{ "263-p1lw.bin",  0x100000, 0xad713169, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "263-p2lw.bin",  0x800000, 0x427eed0b, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "263-s1.bin",    0x020000, 0xa9446774, 2 | BRF_GRA },			  //  2 Text layer tiles

	{ "263-c1.bin",	   0x800000, 0x361ddda6, 3 | BRF_GRA },			  //  3 Sprite data
	{ "263-c2.bin",	   0x800000, 0x15d192c4, 3 | BRF_GRA },			  //  4
	{ "263-c3.bin",	   0x800000, 0xe296612c, 3 | BRF_GRA },			  //  5
	{ "263-c4.bin",	   0x800000, 0xc22e3ad0, 3 | BRF_GRA },			  //  6
	{ "263-c5.bin",	   0x400000, 0x288cd91f, 3 | BRF_GRA },			  //  7
	{ "263-c6.bin",	   0x400000, 0xb015aeee, 3 | BRF_GRA },			  //  8

	{ "263-m1.bin",    0x020000, 0x49b1453e, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "263-v1.bin",	   0x400000, 0x8cb5a9ef, 5 | BRF_SND }, 			// 10 Sound data
	{ "263-v2.bin",	   0x400000, 0xc9572c14, 5 | BRF_SND },			    // 11
	{ "263-v3.bin",	   0x400000, 0xe7c14624, 5 | BRF_SND },			    // 12
	{ "263-v4.bin",	   0x800000, 0xb1e5ac70, 5 | BRF_SND }, 			// 13
};

STDROMPICKEXT(mslug4lw, mslug4lw, neogeo)
STD_ROM_FN(mslug4lw)

struct BurnDriver BurnDrvmslug4lw = {
	"mslug4lw", "mslug4", "neogeo", NULL, "2017",
	"Metal Slug 4 (Last Bullet Remix Hack)\0", NULL, "hack", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug4lwRomInfo, mslug4lwRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 4 (20th Anniversary)
// GOTVG 2021-08-26

static struct BurnRomInfo mslug4aRomDesc[] = {
	{ "263-p1a.p1",		0x100000, 0x0f2b0fc2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "263-p2a.sp2",	0x400000, 0x87dc01b9, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "263-c1.c1",		0x800000, 0x84865f8a, 3 | BRF_GRA },           //  2 Sprite data
	{ "263-c2.c2",		0x800000, 0x81df97f2, 3 | BRF_GRA },           //  3
	{ "263-c3.c3",		0x800000, 0x1a343323, 3 | BRF_GRA },           //  4
	{ "263-c4.c4",		0x800000, 0x942cfb44, 3 | BRF_GRA },           //  5
	{ "263-c5.c5",		0x800000, 0xa748854f, 3 | BRF_GRA },           //  6
	{ "263-c6.c6",		0x800000, 0x5c8ba116, 3 | BRF_GRA },           //  7

	/* Encrypted */
	{ "263-m1.m1",		0x020000, 0x46ac8228, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	/* Encrypted */
	{ "263-v1.v1",		0x800000, 0x01e9b9cd, 5 | BRF_SND },           //  9 Sound data
	{ "263-v2.v2",		0x800000, 0x4ab2bf81, 5 | BRF_SND },           // 10
};

STDROMPICKEXT(mslug4a, mslug4a, neogeo)
STD_ROM_FN(mslug4a)

struct BurnDriver BurnDrvmslug4a = {
	"mslug4a", "mslug4", "neogeo", NULL, "2021",
	"Metal Slug 4 (20th Anniversary)\0", "Enter [Soft Dip] to turn on level display", "hack", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug4aRomInfo, mslug4aRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug4Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 (bootleg, set 1)

static struct BurnRomInfo mslug5bRomDesc[] = {
	{ "ms5b-p1.bin",   0x100000, 0x1376f43c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "ms5b-p2.bin",   0x400000, 0x4becfba0, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "ms5b-s1.bin",   0x020000, 0x3a427c9f, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "ms5b-c1.bin",   0x800000, 0x4b0e5998, 3 | BRF_GRA },           //  3 Sprite data
	{ "ms5b-c2.bin",   0x800000, 0x022fc30b, 3 | BRF_GRA },           //  4
	{ "ms5b-c3.bin",   0x800000, 0xead86d28, 3 | BRF_GRA },           //  5
	{ "ms5b-c4.bin",   0x800000, 0x0be6be35, 3 | BRF_GRA },           //  6
	{ "ms5b-c5.bin",   0x200000, 0x2a23e569, 3 | BRF_GRA },           //  7
	{ "ms5b-c6.bin",   0x200000, 0x6eb6bc9e, 3 | BRF_GRA },           //  8
	{ "ms5b-c7.bin",   0x800000, 0x57f4e53f, 3 | BRF_GRA },           //  9
	{ "ms5b-c8.bin",   0x800000, 0x9d59ff7c, 3 | BRF_GRA },           // 10

	{ "ms5b-m1.bin",   0x020000, 0xbf1601bc, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "ms5b-v1.bin",   0x400000, 0xe3f9fd75, 5 | BRF_SND },           // 12 Sound data
	{ "ms5b-v2.bin",   0x400000, 0xa53618f6, 5 | BRF_SND },           // 13
	{ "ms5b-v3.bin",   0x400000, 0x14f000ee, 5 | BRF_SND },           // 14
	{ "ms5b-v4.bin",   0x400000, 0x0ccee813, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(mslug5b, mslug5b, neogeo)
STD_ROM_FN(mslug5b)

static void mslug5b_cx_decode()
{
	UINT8 *tmp = NeoSpriteROM[nNeoActiveSlot] + 0x2400000; // use empty space as buffer

	// swap chunks
	for (INT32 i = 0; i < 0x4000000; i += 0x200000) {
		memcpy (tmp + 0x000000, NeoSpriteROM[nNeoActiveSlot] + i + 0x100000, 0x080000);
		memcpy (tmp + 0x080000, NeoSpriteROM[nNeoActiveSlot] + i + 0x080000, 0x080000);
		memmove (NeoSpriteROM[nNeoActiveSlot] + i + 0x80000, tmp, 0x100000);
	}
}

static void mslug5b_vx_decrypt()
{
	for (INT32 i = 0; i < 0x1000000; i += 2)
		YM2610ADPCMAROM[nNeoActiveSlot][i + 1] = BITSWAP08(YM2610ADPCMAROM[nNeoActiveSlot][i + 1], 3, 2, 4, 1, 5, 0, 6, 7);
}

static void mslug5bCallback()
{
	svcsplus_sx_decode();
	mslug5b_cx_decode();
}

static INT32 mslug5bInit()
{
	INT32 nRet;

	NeoCallbackActive->pInitialise = mslug5bCallback;

 	nRet = NeoInit();

	if (nRet == 0) {
		mslug5b_vx_decrypt();
	}

	return nRet;
}

struct BurnDriver BurnDrvmslug5b = {
	"mslug5b", "mslug5", "neogeo", NULL, "2003",
	"Metal Slug 5 (bootleg, set 1)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5bRomInfo, mslug5bRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug5bInit, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 (bootleg, set 2)

static struct BurnRomInfo mslug5b2RomDesc[] = {
	{ "ms5-p1b.bin",   0x500000, 0xd05853ea, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "268-c1c.bin",   0x800000, 0xab7c389a, 3 | BRF_GRA },           //  1 Sprite data
	{ "268-c2c.bin",   0x800000, 0x3560881b, 3 | BRF_GRA },           //  2
	{ "268-c3c.bin",   0x800000, 0x3af955ea, 3 | BRF_GRA },           //  3
	{ "268-c4c.bin",   0x800000, 0xc329c373, 3 | BRF_GRA },           //  4
	{ "268-c5c.bin",   0x800000, 0x959c8177, 3 | BRF_GRA },           //  5
	{ "268-c6c.bin",   0x800000, 0x010a831b, 3 | BRF_GRA },           //  6
	{ "268-c7c.bin",   0x800000, 0x6d72a969, 3 | BRF_GRA },           //  7
	{ "268-c8c.bin",   0x800000, 0x551d720e, 3 | BRF_GRA },           //  8

	{ "268-m1h.bin",   0x080000, 0x4a5a6e0e, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "268-v1c.bin",   0x800000, 0xae31d60c, 5 | BRF_SND },           // 10 Sound data
	{ "268-v2c.bin",   0x800000, 0xc40613ed, 5 | BRF_SND },           // 11
};

STDROMPICKEXT(mslug5b2, mslug5b2, neogeo)
STD_ROM_FN(mslug5b2)

static void mslug5b2Callback()
{
	UINT8 *pTemp = (UINT8*)BurnMalloc(0x500000);

	if (pTemp) {
		memcpy(pTemp, Neo68KROMActive, 0x500000);
		memset(Neo68KROMActive, 0, 0x500000);
		memcpy(Neo68KROMActive + 0x000000, pTemp + 0x000000, 0x100000);
		memcpy(Neo68KROMActive + 0x100000, pTemp + 0x100000, 0x100000);
		memcpy(Neo68KROMActive + 0x300000, pTemp + 0x200000, 0x100000);
		memcpy(Neo68KROMActive + 0x200000, pTemp + 0x300000, 0x100000);
		memcpy(Neo68KROMActive + 0x400000, pTemp + 0x400000, 0x100000);

		BurnFree(pTemp);
	}
}

static INT32 mslug5b2Init()
{
	INT32 nRet;

	nNeoProtectionXor = 0x19;
	NeoCallbackActive->pInitialise = mslug5b2Callback;

	nRet = NeoPVCInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0x4e001, 0xfe2cf6, { 0xc3, 0xfd, 0x81, 0xac, 0x6d, 0xe7, 0xbf, 0x9e } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvmslug5b2 = {
	"mslug5b2", "mslug5", "neogeo", NULL, "2003",
	"Metal Slug 5 (bootleg, set 2)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5b2RomInfo, mslug5b2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug5b2Init, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 (Multifunction Hack, 20170523)
// GOTVG
// mslug5hp00 in HBMAME.

static struct BurnRomInfo mslug5dRomDesc[] = {
	{ "268-p1d.bin",	0x600000, 0x3be747ab, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "268d-s1.rom",	0x020000, 0x64952683, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "268d-c1.rom",	0x800000, 0x969c0d62, 3 | BRF_GRA },           //  2 Sprite data
	{ "268d-c2.rom",	0x800000, 0xc69ae867, 3 | BRF_GRA },           //  3
	{ "268d-c3.rom",	0x800000, 0xd7beaeaf, 3 | BRF_GRA },           //  4
	{ "268d-c4.rom",	0x800000, 0xe1b1131b, 3 | BRF_GRA },           //  5
	{ "268d-c5.rom",	0x800000, 0x2fa1a5ad, 3 | BRF_GRA },           //  6
	{ "268d-c6.rom",	0x800000, 0x6de89589, 3 | BRF_GRA },           //  7
	{ "268d-c7.rom",	0x800000, 0x97bd0c0a, 3 | BRF_GRA },           //  8
	{ "268d-c8.rom",	0x800000, 0xc0d5bc20, 3 | BRF_GRA },           //  9

	{ "268d-m1.rom",	0x020000, 0x6fa01c9a, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "268d-v1.rom",	0x400000, 0xc3540e0d, 5 | BRF_SND },           // 11 Sound data
	{ "268d-v2.rom",	0x400000, 0x077bd2f4, 5 | BRF_SND },           // 12
	{ "268d-v3.rom",	0x400000, 0x39b14567, 5 | BRF_SND },           // 13
	{ "268d-v4.rom",	0x400000, 0x969ff3b2, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(mslug5d, mslug5d, neogeo)
STD_ROM_FN(mslug5d)

struct BurnDriver BurnDrvmslug5d = {
	"mslug5d", "mslug5", "neogeo", NULL, "2017",
	"Metal Slug 5 (Multifunction Hack, 20170523)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5dRomInfo, mslug5dRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 (The New War, Hack)
// Fixed the graphics to be visible by alicemsu (Alice愛麗絲)
// GOTVG

static struct BurnRomInfo mslug5wRomDesc[] = {
	{ "254-p1w.p1",		0x100000, 0xb0c126da, 1 | BRF_PRG | BRF_ESS }, //  0 68K code
	{ "254-p2w.sp2",	0x400000, 0xf06c589a, 1 | BRF_PRG | BRF_ESS }, //  1
	
	{ "268-s1d.s1",		0x020000, 0x64952683, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "268-c1d.c1",		0x800000, 0xe8239365, 3 | BRF_GRA },           //  3 Sprite data
	{ "268-c2d.c2",		0x800000, 0x89b21d4c, 3 | BRF_GRA },           //  4
	{ "268-c3d.c3",		0x800000, 0x3cda13a0, 3 | BRF_GRA },           //  5
	{ "268-c4d.c4",		0x800000, 0x9c00160d, 3 | BRF_GRA },           //  6
	{ "268-c5dw.c5",	0x800000, 0x483a986c, 3 | BRF_GRA },           //  7
	{ "268-c6dw.c6",	0x800000, 0xd918f796, 3 | BRF_GRA },           //  8
	{ "268-c7dw.c7",	0x800000, 0xbdb9a887, 3 | BRF_GRA },           //  9
	{ "268-c8dw.c8",	0x800000, 0x6f8ac6fb, 3 | BRF_GRA },           // 10

	{ "254-m1w.m1",     0x080000, 0x464c72ad, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "254-v1w.v1",		0x400000, 0xa8e12a92, 5 | BRF_SND },           // 12 Sound data
	{ "254-v2w.v2",		0x400000, 0x0608cba3, 5 | BRF_SND },           // 13
	{ "254-v3nd.v3",	0x400000, 0x02fd519e, 5 | BRF_SND },           // 14
	{ "254-v4w.v4",		0x4002f0, 0x179cbca3, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(mslug5w, mslug5w, neogeo)
STD_ROM_FN(mslug5w)

struct BurnDriver BurnDrvmslug5w = {
	"mslug5w", "mslug5", "neogeo", NULL, "2016",
	"Metal Slug 5 (The New War, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5wRomInfo, mslug5wRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Ninja Master's - haoh-ninpo-cho (Plus)

static struct BurnRomInfo ninjamashaRomDesc[] = {
	{ "217-p1ha.p1",  0x100000, 0x45332f39, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ TC538200
	{ "217-p2.sp2",   0x200000, 0x191fca88, 1 | BRF_ESS | BRF_PRG }, //  1 					/ TC5316200

	{ "217-s1.s1",    0x020000, 0x8ff782f0, 2 | BRF_GRA },           //  2 Text layer tiles / TC531000

	{ "217-c1.c1",    0x400000, 0x5fe97bc4, 3 | BRF_GRA },           //  3 Sprite data 		/ TC5332205
	{ "217-c2.c2",    0x400000, 0x886e0d66, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "217-c3.c3",    0x400000, 0x59e8525f, 3 | BRF_GRA },           //  5 					/ TC5332205
	{ "217-c4.c4",    0x400000, 0x8521add2, 3 | BRF_GRA },           //  6 					/ TC5332205
	{ "217-c5.c5",    0x400000, 0xfb1896e5, 3 | BRF_GRA },           //  7 					/ TC5332205
	{ "217-c6.c6",    0x400000, 0x1c98c54b, 3 | BRF_GRA },           //  8 					/ TC5332205
	{ "217-c7.c7",    0x400000, 0x8b0ede2e, 3 | BRF_GRA },           //  9 					/ TC5332205
	{ "217-c8.c8",    0x400000, 0xa085bb61, 3 | BRF_GRA },           // 10 					/ TC5332205

	{ "217-m1.m1",    0x020000, 0xd00fb2af, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/ TC531001

	{ "217-v1.v1",    0x400000, 0x1c34e013, 5 | BRF_SND },           // 12 Sound data 		/ TC5332204
	{ "217-v2.v2",    0x200000, 0x22f1c681, 5 | BRF_SND },           // 13 					/ TC5316200
};

STDROMPICKEXT(ninjamasha, ninjamasha, neogeo)
STD_ROM_FN(ninjamasha)

struct BurnDriver BurnDrvNinjamasha = {
	"ninjamasha", "ninjamas", "neogeo", NULL, "1996",
	"Ninja Master's - haoh-ninpo-cho (Plus)\0", NULL, "007325", "Neo Geo MVS",
	L"Ninja master's \u8987\u738B\u5FCD\u6CD5\u5E16\0Ninja Master's haoh ninpo cho (Plus)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, ninjamashaRomInfo, ninjamashaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// Real Bout Fatal Fury 2 - The Newcomers / Real Bout Garou Densetsu 2 - the newcomers (Secret Character Hack)
// Enable Alfred v3 and Fix Alfred's player 2 color - hack by Eddids[FCHT]

static struct BurnRomInfo rbff2bhRomDesc[] = {
	{ "240-p1fc.p1",  0x100000, 0xd01854fa, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "240-p2fc.sp2", 0x400000, 0xc063193d, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "240-s1.s1",    0x020000, 0xda3b40de, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "240-c1.c1",    0x800000, 0xeffac504, 3 | BRF_GRA },           //  3 Sprite data
	{ "240-c2.c2",    0x800000, 0xed182d44, 3 | BRF_GRA },           //  4
	{ "240-c3.c3",    0x800000, 0x22e0330a, 3 | BRF_GRA },           //  5
	{ "240-c4.c4",    0x800000, 0xc19a07eb, 3 | BRF_GRA },           //  6
	{ "240-c5fc.c5",  0x800000, 0x21831787, 3 | BRF_GRA },           //  7
	{ "240-c6fc.c6",  0x800000, 0x06b2d1da, 3 | BRF_GRA },           //  8

	{ "240-m1.m1",    0x040000, 0xed482791, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "240-v1.v1",    0x400000, 0xf796265a, 5 | BRF_SND },           // 10 Sound data
	{ "240-v2.v2",    0x400000, 0x2cb3f3bb, 5 | BRF_SND },           // 11
	{ "240-v3.v3",    0x400000, 0x8fe1367a, 5 | BRF_SND },           // 12
	{ "240-v4.v4",    0x200000, 0x996704d8, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(rbff2bh, rbff2bh, neogeo)
STD_ROM_FN(rbff2bh)

struct BurnDriver BurnDrvrbff2bh = {
	"rbff2bh", "rbff2", "neogeo", NULL, "1998",
	"Real Bout Fatal Fury 2 - The Newcomers / Real Bout Garou Densetsu 2 - the newcomers (Secret Character Hack)\0", NULL, "hack", "Neo Geo MVS",
	L"Real Bout Fatal Fury 2 - The Newcomers\0Real Bout \u9913\u72FC\u4F1D\u8AAC\uFF12 (Secret Character Hack)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbff2bhRomInfo, rbff2bhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Real Bout Fatal Fury Special / Real Bout Garou Densetsu Special (Boss Hack)
// Enable hidden characters - hack by yumeji1768 [2ch]

static struct BurnRomInfo rbffspbhRomDesc[] = {
	{ "223-p1bs.p1",  0x100000, 0xabf2a6e7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "223-p2.sp2",   0x400000, 0xaddd8f08, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "223-s1.s1",    0x020000, 0x7ecd6e8c, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "223-c1.c1",    0x400000, 0xebab05e2, 3 | BRF_GRA },           //  3 Sprite data
	{ "223-c2.c2",    0x400000, 0x641868c3, 3 | BRF_GRA },           //  4
	{ "223-c3.c3",    0x400000, 0xca00191f, 3 | BRF_GRA },           //  5
	{ "223-c4.c4",    0x400000, 0x1f23d860, 3 | BRF_GRA },           //  6
	{ "223-c5.c5",    0x400000, 0x321e362c, 3 | BRF_GRA },           //  7
	{ "223-c6.c6",    0x400000, 0xd8fcef90, 3 | BRF_GRA },           //  8
	{ "223-c7.c7",    0x400000, 0xbc80dd2d, 3 | BRF_GRA },           //  9
	{ "223-c8.c8",    0x400000, 0x5ad62102, 3 | BRF_GRA },           // 10

	{ "223-m1.m1",    0x020000, 0x3fee46bf, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "223-v1.v1",    0x400000, 0x76673869, 5 | BRF_SND },           // 12 Sound data
	{ "223-v2.v2",    0x400000, 0x7a275acd, 5 | BRF_SND },           // 13
	{ "223-v3.v3",    0x400000, 0x5a797fd2, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(rbffspbh, rbffspbh, neogeo)
STD_ROM_FN(rbffspbh)

struct BurnDriver BurnDrvrbffspbh = {
	"rbffspbh", "rbffspec", "neogeo", NULL, "1996",
	"Real Bout Fatal Fury Special / Real Bout Garou Densetsu Special (Boss Hack)\0", "Select Geese Howard with portrait, press 'B+C' to select EX characters", "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_FATFURY,
	NULL, rbffspbhRomInfo, rbffspbhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Samurai Shodown II / Shin Samurai Spirits - Haohmaru jigokuhen (Special 2017, hack)
// Modified by: GSC2007
// Version number: Ver 1.1

static struct BurnRomInfo samsho2spRomDesc[] = {
	{ "063-p1sp.p1",	0x100000, 0xed466b3d, 1 | BRF_ESS | BRF_PRG }, //  0 68k code
	{ "063-p2sp.sp2",	0x100000, 0x6e1aef70, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "063-p3sp.p3",	0x020000, 0xf9079ea3, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "063-s1sp.s1",	0x020000, 0x1951a907, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "063-c1.c1",		0x200000, 0x86cd307c, 3 | BRF_GRA },           //  4 Sprite data
	{ "063-c2.c2",		0x200000, 0xcdfcc4ca, 3 | BRF_GRA },           //  5
	{ "063-c3.c3",		0x200000, 0x7a63ccc7, 3 | BRF_GRA },           //  6
	{ "063-c4.c4",		0x200000, 0x751025ce, 3 | BRF_GRA },           //  7
	{ "063-c5.c5",		0x200000, 0x20d3a475, 3 | BRF_GRA },           //  8
	{ "063-c6.c6",		0x200000, 0xae4c0a88, 3 | BRF_GRA },           //  9
	{ "063-c7sp.c7",	0x200000, 0xdbebced2, 3 | BRF_GRA },           // 10
	{ "063-c8sp.c8",	0x200000, 0xbf70b93c, 3 | BRF_GRA },           // 11

	{ "063-m1.m1",		0x020000, 0x56675098, 4 | BRF_ESS | BRF_PRG }, // 12 Z80 code

	{ "063-v1.v1",		0x200000, 0x37703f91, 5 | BRF_SND },           // 13 Sound data
	{ "063-v2.v2",		0x200000, 0x0142bde8, 5 | BRF_SND },           // 14
	{ "063-v3.v3",		0x200000, 0xd07fa5ca, 5 | BRF_SND },           // 15
	{ "063-v4.v4",		0x100000, 0x24aab4bb, 5 | BRF_SND },           // 16
};

STDROMPICKEXT(samsho2sp, samsho2sp, neogeo)
STD_ROM_FN(samsho2sp)

static INT32 Samsho2spInit()
{
    INT32 nRet = NeoInit();
	if (0 == nRet) NeoMapExtraRom(0x200000, 0x20000);

    return nRet;
}

struct BurnDriver BurnDrvSamsho2sp = {
	"samsho2sp", "samsho2", "neogeo", NULL, "2017",
	"Samurai Shodown II / Shin Samurai Spirits - Haohmaru jigokuhen (Special 2017, hack)\0", NULL, "hack", "Neo Geo MVS",
	L"Samurai Shodown II\0\u771F Samurai Spirits - \u8987\u738B\u4E38\u5730\u7344\u5909 (Special 2017, hack)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho2spRomInfo, samsho2spRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	Samsho2spInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Samurai Shodown IV - Amakusa's Revenge / Samurai Spirits - Amakusa Kourin (Special 2017, hack)
// Modified by: GSC2007
// Version number: Ver 1.0.0501

static struct BurnRomInfo samsho4spRomDesc[] = {
	{ "222-p1sp.p1",  0x100000, 0x6e98579a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "222-p2sp.sp2", 0x400000, 0x2c9c64db, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "222-s1.s1",    0x020000, 0x8d3d3bf9, 2 | BRF_GRA },           //  2 Text layer tiles /* TC531000 */

	{ "222-c1.c1",    0x400000, 0x68f2ed95, 3 | BRF_GRA },           //  3 Sprite data		/* TC5332205 */
	{ "222-c2.c2",    0x400000, 0xa6e9aff0, 3 | BRF_GRA },           //  4 					/* TC5332205 */
	{ "222-c3.c3",    0x400000, 0xc91b40f4, 3 | BRF_GRA },           //  5 					/* TC5332205 */
	{ "222-c4.c4",    0x400000, 0x359510a4, 3 | BRF_GRA },           //  6 					/* TC5332205 */
	{ "222-c5.c5",    0x400000, 0x9cfbb22d, 3 | BRF_GRA },           //  7 					/* TC5332205 */
	{ "222-c6.c6",    0x400000, 0x685efc32, 3 | BRF_GRA },           //  8 					/* TC5332205 */
	{ "222-c7sp.c7",  0x400000, 0xecb13c24, 3 | BRF_GRA },           //  9
	{ "222-c8sp.c8",  0x400000, 0x0f9a0bda, 3 | BRF_GRA },           // 10

	{ "222-m1.m1",    0x020000, 0x7615bc1b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code			/* TC531001 */

	{ "222-v1.v1",    0x400000, 0x7d6ba95f, 5 | BRF_SND },           // 12 Sound data		/* TC5332204 */
	{ "222-v2.v2",    0x400000, 0x6c33bb5d, 5 | BRF_SND },           // 13 					/* TC5332204 */
	{ "222-v3.v3",    0x200000, 0x831ea8c0, 5 | BRF_SND },           // 14 					/* TC5316200 */
};

STDROMPICKEXT(samsho4sp, samsho4sp, neogeo)
STD_ROM_FN(samsho4sp)

struct BurnDriver BurnDrvSamSho4sp = {
	"samsho4sp", "samsho4", "neogeo", NULL, "1996",
	"Samurai Shodown IV - Amakusa's Revenge / Samurai Spirits - Amakusa Kourin (Special 2017, hack)\0", NULL, "hack", "Neo Geo MVS",
	L"Samurai Shodown IV - Amakusa's Revenge\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4 - \u5929\u8349\u964D\u81E8 (Special 2017, hack)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho4spRomInfo, samsho4spRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Samurai Shodown V / Samurai Spirits Zero (hack of XBOX version)

static struct BurnRomInfo samsho5xRomDesc[] = {
	{ "ssvx_p1.rom",  0x800000, 0x16983af9, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "ssv_s1.rom",   0x020000, 0x2ad6048b, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "ssvx_c1.rom",  0x800000, 0x25272e50, 3 | BRF_GRA },           //  2 Sprite data
	{ "ssvx_c2.rom",  0x800000, 0xba68f2e7, 3 | BRF_GRA },           //  3
	{ "ssvx_c3.rom",  0x800000, 0x75883cde, 3 | BRF_GRA },           //  4
	{ "ssvx_c4.rom",  0x800000, 0x348540e6, 3 | BRF_GRA },           //  5
	{ "ssvx_c5.rom",  0x800000, 0x1fee8dc8, 3 | BRF_GRA },           //  6
	{ "ssvx_c6.rom",  0x800000, 0xc300b50d, 3 | BRF_GRA },           //  7
	{ "ssvx_c7.rom",  0x800000, 0x5e722b0b, 3 | BRF_GRA },           //  8
	{ "ssvx_c8.rom",  0x800000, 0xe2a2c546, 3 | BRF_GRA },           //  9

	{ "ssvx_m1.rom",  0x080000, 0x5218a10a, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "ssv_v1.rom",   0x800000, 0x809c7617, 5 | BRF_SND },           // 11 Sound data
	{ "ssv_v2.rom",   0x800000, 0x42671607, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(samsho5x, samsho5x, neogeo)
STD_ROM_FN(samsho5x)

struct BurnDriver BurnDrvsamsho5x = {
	"samsho5x", "samsho5", "neogeo", NULL, "2003",
	"Samurai Shodown V / Samurai Spirits Zero (hack of XBOX version)\0", NULL, "hack", "Neo Geo MVS",
	L"Samurai Shodown V\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4\u96F6 (hack of XBOX version)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsho5xRomInfo, samsho5xRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Samurai Shodown V Perfect / Samurai Spirits Zero Perfect (bootleg, hack)

static struct BurnRomInfo samsh5pfRomDesc[] = {
	{ "273-p1.bin",  0x800000, 0xe3d18d3a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	
	{ "273-s1.bin",  0x020000, 0xabb3baf9, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "273-c1d.bin", 0x800000, 0xec9fda8d, 3 | BRF_GRA },           //  2 Sprite data
	{ "273-c2d.bin", 0x800000, 0xd2fc888d, 3 | BRF_GRA },           //  3
	{ "273-c3d.bin", 0x800000, 0xb0ea781b, 3 | BRF_GRA },           //  4
	{ "273-c4d.bin", 0x800000, 0xd34ac591, 3 | BRF_GRA },           //  5
	{ "273-c5d.bin", 0x800000, 0x1b5c1ea2, 3 | BRF_GRA },           //  6
	{ "273-c6d.bin", 0x800000, 0xdeeaad58, 3 | BRF_GRA },           //  7
	{ "273-c7d.bin", 0x800000, 0xaf90afc8, 3 | BRF_GRA },           //  8
	{ "273-c8d.bin", 0x800000, 0xb14872da, 3 | BRF_GRA },           //  9

	{ "273-m1.bin",  0x020000, 0x654e9236, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "273-v1d.bin", 0x800000, 0x3bf61586, 5 | BRF_SND },           // 11 Sound data
	{ "273-v2d.bin", 0x800000, 0x95fe7646, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(samsh5pf, samsh5pf, neogeo)
STD_ROM_FN(samsh5pf)

struct BurnDriver BurnDrvsamsh5pf = {
	"samsh5pf", "samsh5sp", "neogeo", NULL, "2020",
	"Samurai Shodown V Perfect / Samurai Spirits Zero Perfect (bootleg, hack)\0", NULL, "bootleg", "Neo Geo MVS",
	L"Samurai Shodown V Perfect\0\u30B5\u30E0\u30E9\u30A4\u30B9\u30D4\u30EA\u30C3\u30C4\u96F6 Perfect (bootleg, hack)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_SAMSHO,
	NULL, samsh5pfRomInfo, samsh5pfRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Savage Reign / Fu'un Mokushiroku - kakutou sousei (Boss Hack)

static struct BurnRomInfo savagerebRomDesc[] = {
	{ "059-p1b.p1",   0x200000, 0x3a7fbff0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "059-s1.s1",    0x020000, 0xe08978ca, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "059-c1.c1",    0x200000, 0x763ba611, 3 | BRF_GRA },           //  2 Sprite data		/ TC5316200
	{ "059-c2.c2",    0x200000, 0xe05e8ca6, 3 | BRF_GRA },           //  3 					/ TC5316200
	{ "059-c3.c3",    0x200000, 0x3e4eba4b, 3 | BRF_GRA },           //  4 					/ TC5316200
	{ "059-c4.c4",    0x200000, 0x3c2a3808, 3 | BRF_GRA },           //  5 					/ TC5316200
	{ "059-c5.c5",    0x200000, 0x59013f9e, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "059-c6.c6",    0x200000, 0x1c8d5def, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "059-c7.c7",    0x200000, 0xc88f7035, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "059-c8.c8",    0x200000, 0x484ce3ba, 3 | BRF_GRA },           //  9 					/ TC5316200

	{ "059-m1.m1",    0x020000, 0x29992eba, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "059-v1.v1",    0x200000, 0x530c50fd, 5 | BRF_SND },           // 11 Sound data		/ TC5316200
	{ "059-v2.v2",    0x200000, 0xeb6f1cdb, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "059-v3.v3",    0x200000, 0x7038c2f9, 5 | BRF_SND },           // 13 					/ TC5316200
};

STDROMPICKEXT(savagereb, savagereb, neogeo)
STD_ROM_FN(savagereb)

struct BurnDriver BurnDrvSavagereb = {
	"savagereb", "savagere", "neogeo", NULL, "1995",
	"Savage Reign / Fu'un Mokushiroku - kakutou sousei (Boss Hack)\0", NULL, "Yumeji, Dodowang[EGCG]", "Neo Geo MVS",
	L"Savage Reign (Boss Hack)\0\u98A8\u96F2\u9ED9\u793A\u9332 - \u683C\u95D8\u5275\u4E16\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, 0,
	NULL, savagerebRomInfo, savagerebRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Sengoku 3 / Sengoku Densho 2001 (Evolution 1.0, FCHT hack)

static struct BurnRomInfo sengoku3sRomDesc[] = {
	{ "261-p1s.bin",  			0x200000, 0x5a1b5e85, 1 | BRF_ESS | BRF_PRG }, 	//  0 68K code

	{ "261-s1.bin",   			0x020000, 0xc1e27cc7, 2 | BRF_GRA },           	//  1 Text layer tiles

	{ "261-c1_decrypted.bin",    0x800000, 0x9af7cbca, 3 | BRF_GRA },			//  2 Sprite data
	{ "261-c2_decrypted.bin",    0x800000, 0x2a1f874d, 3 | BRF_GRA },   		//  3
	{ "261-c3_decrypted.bin",    0x800000, 0x5403adb5, 3 | BRF_GRA },   		//  4
	{ "261-c4_decrypted.bin",    0x800000, 0x18926df6, 3 | BRF_GRA },   		//  5

	{ "261-m1.m1",    			0x080000, 0x7d501c39, 4 | BRF_ESS | BRF_PRG }, 	//  6 Z80 code

	{ "261-v1.v1",    			0x400000, 0x64c30081, 5 | BRF_SND },           	//  7 Sound data
	{ "261-v2.v2",    			0x400000, 0x392a9c47, 5 | BRF_SND },           	//  8
	{ "261-v3.v3",    			0x400000, 0xc1a7ebe3, 5 | BRF_SND },           	//  9
	{ "261-v4.v4",    			0x200000, 0x9000d085, 5 | BRF_SND },           	// 10
};

STDROMPICKEXT(sengoku3s, sengoku3s, neogeo)
STD_ROM_FN(sengoku3s)

struct BurnDriver BurnDrvSengoku3s = {
	"sengoku3s", "sengoku3", "neogeo", NULL, "2001",
	"Sengoku 3 / Sengoku Densho 2001 (Evolution 1.0, FCHT hack)\0", NULL, "hack", "Neo Geo MVS",
	L"Sengoku 3\0\u6226\u56FD\u4F1D\u627F\uFF12\uFF10\uFF10\uFF11 (Evolution 1.0, FCHT hack)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SCRFIGHT, 0,
	NULL, sengoku3sRomInfo, sengoku3sRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Sengoku 3 / Sengoku Densho 2001 (Feng Shen Edition)
// Hacked by BinDi
// GOTVG 2020/04/10

static struct BurnRomInfo sengoku3fsRomDesc[] = {
	{ "261-ph1fs.p1",	0x200000, 0x1242fa34, 1 | BRF_ESS | BRF_PRG },	//  0 68K code

	{ "261-c1.c1",		0x800000, 0xded84d9c, 3 | BRF_GRA },			//  1 Sprite data
	{ "261-c2.c2",		0x800000, 0xb8eb4348, 3 | BRF_GRA },			//  2
	{ "261-c3.c3",		0x800000, 0x84e2034a, 3 | BRF_GRA },			//  3
	{ "261-c4.c4",		0x800000, 0x0b45ae53, 3 | BRF_GRA },			//  4

	{ "261-m1.m1",		0x080000, 0x7d501c39, 4 | BRF_ESS | BRF_PRG },	//  5 Z80 code

	{ "261-v1.v1",		0x400000, 0x64c30081, 5 | BRF_SND },			//  6 Sound data
	{ "261-v2.v2",		0x400000, 0x392a9c47, 5 | BRF_SND },			//  7
	{ "261-v3.v3",		0x400000, 0xc1a7ebe3, 5 | BRF_SND },			//  8
	{ "261-v4.v4",		0x200000, 0x9000d085, 5 | BRF_SND },			//  9
};

STDROMPICKEXT(sengoku3fs, sengoku3fs, neogeo)
STD_ROM_FN(sengoku3fs)

struct BurnDriver BurnDrvsengoku3fs = {
	"sengoku3fs", "sengoku3", "neogeo", NULL, "2020",
	"Sengoku 3 / Sengoku Densho 2001 (Feng Shen Edition)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SWAPP, GBF_SCRFIGHT, 0,
	NULL, sengoku3fsRomInfo, sengoku3fsRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	sengoku3Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Sengoku 3 / Sengoku Densho 2001 (Qing Edition)
// Hacked by BinDi
// GOTVG 2013/02/27

static struct BurnRomInfo sengoku3ebRomDesc[] = {
	{ "261-ph1eb.p1",	0x200000, 0x821b7a95, 1 | BRF_ESS | BRF_PRG },	//  0 68K code

	{ "261-c1.c1",		0x800000, 0xded84d9c, 3 | BRF_GRA },			//  1 Sprite data
	{ "261-c2.c2",		0x800000, 0xb8eb4348, 3 | BRF_GRA },			//  2
	{ "261-c3.c3",		0x800000, 0x84e2034a, 3 | BRF_GRA },			//  3
	{ "261-c4.c4",		0x800000, 0x0b45ae53, 3 | BRF_GRA },			//  4

	{ "261-m1.m1",		0x080000, 0x7d501c39, 4 | BRF_ESS | BRF_PRG },	//  5 Z80 code

	{ "261-v1.v1",		0x400000, 0x64c30081, 5 | BRF_SND },			//  6 Sound data
	{ "261-v2.v2",		0x400000, 0x392a9c47, 5 | BRF_SND },			//  7
	{ "261-v3.v3",		0x400000, 0xc1a7ebe3, 5 | BRF_SND },			//  8
	{ "261-v4.v4",		0x200000, 0x9000d085, 5 | BRF_SND },			//  9
};

STDROMPICKEXT(sengoku3eb, sengoku3eb, neogeo)
STD_ROM_FN(sengoku3eb)

struct BurnDriver BurnDrvsengoku3eb = {
	"sengoku3eb", "sengoku3", "neogeo", NULL, "2013",
	"Sengoku 3 / Sengoku Densho 2001 (Qing Edition)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SWAPP, GBF_SCRFIGHT, 0,
	NULL, sengoku3ebRomInfo, sengoku3ebRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	sengoku3Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Super Dodge Ball / Kunio no Nekketsu Toukyuu Densetsu (Secret Character Hack)
// Unlock MAOU TEAM - hack by PlayerX

static struct BurnRomInfo sdodgebhRomDesc[] = {
	{ "208-p1bh.p1",  0x200000, 0xe6e58566, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "208-s1.s1",    0x020000, 0x64abd6b3, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "208-c1.c1",    0x400000, 0x93d8619b, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332205
	{ "208-c2.c2",    0x400000, 0x1c737bb6, 3 | BRF_GRA },           //  3 					/ TC5332205
	{ "208-c3.c3",    0x200000, 0x14cb1703, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "208-c4.c4",    0x200000, 0xc7165f19, 3 | BRF_GRA },           //  5 					/ TC5332205

	{ "208-m1.m1",    0x020000, 0x0a5f3325, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "208-v1.v1",    0x400000, 0xe7899a24, 5 | BRF_SND },           //  7 Sound data		/ TC5332204
};

STDROMPICKEXT(sdodgebh, sdodgebh, neogeo)
STD_ROM_FN(sdodgebh)

struct BurnDriver BurnDrvsdodgebh = {
	"sdodgebh", "sdodgeb", "neogeo", NULL, "1996",
	"Super Dodge Ball / Kunio no Nekketsu Toukyuu Densetsu (Secret Character Hack)\0", NULL, "hack", "Neo Geo MVS",
	L"Super Dodge Ball\0\u304F\u306B\u304A\u306E\u71B1\u8840\u95D8\u7403\u4F1D\u8AAC (Secret Character Hack)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_SPORTSMISC, 0,
	NULL, sdodgebhRomInfo, sdodgebhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '94 (portuguese edition) by BisonSAS and Mr.Fox
// https://romhackers.org/traducoes/arcade/neo-geo/the-king-of-fighters-94-neogeo-br-team/

static struct BurnRomInfo kof94brRomDesc[] = {
	{ "055-p1br.p1",  0x200000, 0x0138a992, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC5316200

	{ "055-s1.s1",    0x020000, 0x825976c1, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "055-c1br.c1",  0x200000, 0x6fb14154, 3 | BRF_GRA },           //  2 Sprite data 		/ mask rom TC5316200
	{ "055-c2.c2",    0x200000, 0x15e096a7, 3 | BRF_GRA },           //  3 					/ mask rom TC5316200
	{ "055-c3.c3",    0x200000, 0x54f66254, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "055-c4.c4",    0x200000, 0x0b01765f, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200
	{ "055-c5.c5",    0x200000, 0xee759363, 3 | BRF_GRA },           //  6 					/ mask rom TC5316200
	{ "055-c6.c6",    0x200000, 0x498da52c, 3 | BRF_GRA },           //  7 					/ mask rom TC5316200
	{ "055-c7.c7",    0x200000, 0x62f66888, 3 | BRF_GRA },           //  8 					/ mask rom TC5316200
	{ "055-c8.c8",    0x200000, 0xfe0a235d, 3 | BRF_GRA },           //  9 					/ mask rom TC5316200

	{ "055-m1.m1",    0x020000, 0xf6e77cf5, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code 		/ mask rom TC531001

	{ "055-v1.v1",    0x200000, 0x8889596d, 5 | BRF_SND },           // 11 Sound data 		/ mask rom TC5316200
	{ "055-v2.v2",    0x200000, 0x25022b27, 5 | BRF_SND },           // 12 					/ mask rom TC5316200
	{ "055-v3.v3",    0x200000, 0x83cf32c0, 5 | BRF_SND },           // 13 					/ mask rom TC5316200
};

STDROMPICKEXT(kof94br, kof94br, neogeo)
STD_ROM_FN(kof94br)

struct BurnDriver BurnDrvKof94br = {
	"kof94br", "kof94", "neogeo", NULL, "1994",
	"The King of Fighters '94 (Portuguese edition Hack)\0", "Translation ROM - Portuguese BRA", "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, FBF_KOF,
	NULL, kof94brRomInfo, kof94brRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '94 (Hack Boss Remixed)

static struct BurnRomInfo kof94rzRomDesc[] = {
	{ "055-p1rz.p1",  0x200000, 0x38410fdf, 1 | BRF_ESS | BRF_PRG }, //  0 68K code 		/ mask rom TC5316200

	{ "055-s1rz.s1",  0x020000, 0x286ab67d, 2 | BRF_GRA },           //  1 Text layer tiles / mask rom TC531000

	{ "055-c1.c1",    0x200000, 0xb96ef460, 3 | BRF_GRA },           //  2 Sprite data 		/ mask rom TC5316200
	{ "055-c2.c2",    0x200000, 0x15e096a7, 3 | BRF_GRA },           //  3 					/ mask rom TC5316200
	{ "055-c3.c3",    0x200000, 0x54f66254, 3 | BRF_GRA },           //  4 					/ mask rom TC5316200
	{ "055-c4.c4",    0x200000, 0x0b01765f, 3 | BRF_GRA },           //  5 					/ mask rom TC5316200
	{ "055-c5.c5",    0x200000, 0xee759363, 3 | BRF_GRA },           //  6 					/ mask rom TC5316200
	{ "055-c6.c6",    0x200000, 0x498da52c, 3 | BRF_GRA },           //  7 					/ mask rom TC5316200
	{ "055-c7.c7",    0x200000, 0x62f66888, 3 | BRF_GRA },           //  8 					/ mask rom TC5316200
	{ "055-c8.c8",    0x200000, 0xfe0a235d, 3 | BRF_GRA },           //  9 					/ mask rom TC5316200

	{ "055-m1.m1",    0x020000, 0xf6e77cf5, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code 		/ mask rom TC531001

	{ "055-v1.v1",    0x200000, 0x8889596d, 5 | BRF_SND },           // 11 Sound data 		/ mask rom TC5316200
	{ "055-v2.v2",    0x200000, 0x25022b27, 5 | BRF_SND },           // 12 					/ mask rom TC5316200
	{ "055-v3.v3",    0x200000, 0x83cf32c0, 5 | BRF_SND },           // 13 					/ mask rom TC5316200
};

STDROMPICKEXT(kof94rz, kof94rz, neogeo)
STD_ROM_FN(kof94rz)

struct BurnDriver BurnDrvKof94rz = {
	"kof94rz", "kof94", "neogeo", NULL, "1994",
	"The King of Fighters '94 (Hack Boss Remixed)\0", NULL, "ZKW", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, FBF_KOF,
	NULL, kof94rzRomInfo, kof94rzRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '95 (Special 2017)
// Modified by: GSC2007
// Version number: Ver 1.2.0222

// Rom information
	/*
	084-p1sp.p1 8bcca012 : P1 Ver 1.0.0627
	084-p3sp.p3 030e71cb : P3 Ver 1.0.0627
	*/
static struct BurnRomInfo kof95spRomDesc[] = {
//	{ "084-p1sp.p1",  0x100000, 0x8bcca012, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "084-p1sp.p1",  0x100000, 0xb3c26333, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "084-p2sp.p2",  0x100000, 0x5cb1af9e, 1 | BRF_ESS | BRF_PRG }, //  1
//	{ "084-p3sp.p3",  0x020000, 0x030e71cb, 1 | BRF_ESS | BRF_PRG }, //  2
	{ "084-p3sp.p3",  0x020000, 0x7eddc5d8, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "084-s1sp.s1",  0x020000, 0x83cbae60, 2 | BRF_GRA },           //  3 Text layer tiles / TC531000

	{ "084-c1.c1",    0x400000, 0xfe087e32, 3 | BRF_GRA },           //  4 Sprite data		/ TC5332202
	{ "084-c2.c2",    0x400000, 0x07864e09, 3 | BRF_GRA },           //  5 					/ TC5332202
	{ "084-c3.c3",    0x400000, 0xa4e65d1b, 3 | BRF_GRA },           //  6					/ TC5332202
	{ "084-c4.c4",    0x400000, 0xc1ace468, 3 | BRF_GRA },           //  7 					/ TC5332202
	{ "084-c5.c5",    0x200000, 0x8a2c1edc, 3 | BRF_GRA },           //  8 					/ TC5316200
	{ "084-c6.c6",    0x200000, 0xf593ac35, 3 | BRF_GRA },           //  9 					/ TC5316200
	{ "084-c7.c7",    0x100000, 0x9904025f, 3 | BRF_GRA },           // 10 					/ TC538200
	{ "084-c8.c8",    0x100000, 0x78eb0f9b, 3 | BRF_GRA },           // 11 					/ TC538200

	{ "084-m1.m1",    0x020000, 0x6f2d7429, 4 | BRF_ESS | BRF_PRG }, // 12 Z80 code			/ TC531001

	{ "084-v1.v1",    0x400000, 0x84861b56, 5 | BRF_SND },           // 13 Sound data		/ TC5332201
	{ "084-v2.v2",    0x200000, 0xb38a2803, 5 | BRF_SND },           // 14 					/ TC5316200
	{ "084-v3.v3",    0x100000, 0xd683a338, 5 | BRF_SND },           // 15 					/ TC538200
};

STDROMPICKEXT(kof95sp, kof95sp, neogeo)
STD_ROM_FN(kof95sp)

static void kof95PatchCallback()
{
	UINT16* rom = (UINT16*)Neo68KROMActive;

	for (INT32 i = 0; i < 0x100000 >> 1; i++) {
		if (rom[i] == 0x4e7d) rom[i] = 0x4e71;
		if (rom[i] == 0x4e7c) rom[i] = 0x4e75;
	}

	rom[0x1f3a8] = 0x2b7c; // 4ef9
	rom[0x1f3a9] = 0x0003; // 0090
	rom[0x1f3aa] = 0xe7fa; // 16be

	rom = (UINT16*)(Neo68KROMActive + 0x200000);

	for (INT32 i = 0; i < 0x20000 >> 1; i++) {
		if (rom[i] == 0x4e7d) rom[i] = 0x4e71;
		if (rom[i] == 0x4e7c) rom[i] = 0x4e75;
	}
}

static INT32 kof95spInit()
{
	NeoCallbackActive->pInitialise = kof95PatchCallback;

 	INT32 nRet = NeoInit();
	if (0 == nRet) NeoMapExtraRom(0x200000, 0x20000);

	return nRet;
}

struct BurnDriver BurnDrvKof95sp = {
	"kof95sp", "kof95", "neogeo", NULL, "2017",
	"The King of Fighters '95 (Special 2017, hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof95spRomInfo, kof95spRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof95spInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '95 (Enable Hidden Characters V.[?])

static struct BurnRomInfo kof95bRomDesc[] = {
	{ "084-p1b.p1",   0x200000, 0x64d9aa39, 1 | BRF_ESS | BRF_PRG }, //  0 68K code			/ TC5316200

	{ "084-s1.s1",    0x020000, 0xde716f8a, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "084-c1.c1",    0x400000, 0xfe087e32, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332202
	{ "084-c2.c2",    0x400000, 0x07864e09, 3 | BRF_GRA },           //  3 					/ TC5332202
	{ "084-c3.c3",    0x400000, 0xa4e65d1b, 3 | BRF_GRA },           //  4 					/ TC5332202
	{ "084-c4.c4",    0x400000, 0xc1ace468, 3 | BRF_GRA },           //  5 					/ TC5332202
	{ "084-c5.c5",    0x200000, 0x8a2c1edc, 3 | BRF_GRA },           //  6 					/ TC5316200
	{ "084-c6.c6",    0x200000, 0xf593ac35, 3 | BRF_GRA },           //  7 					/ TC5316200
	{ "084-c7.c7",    0x100000, 0x9904025f, 3 | BRF_GRA },           //  8 					/ TC538200
	{ "084-c8.c8",    0x100000, 0x78eb0f9b, 3 | BRF_GRA },           //  9 					/ TC538200

	{ "084-m1.m1",    0x020000, 0x6f2d7429, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code			/ TC531001

	{ "084-v1.v1",    0x400000, 0x84861b56, 5 | BRF_SND },           // 11 Sound data		/ TC5332201
	{ "084-v2.v2",    0x200000, 0xb38a2803, 5 | BRF_SND },           // 12 					/ TC5316200
	{ "084-v3.v3",    0x100000, 0xd683a338, 5 | BRF_SND },           // 13 					/ TC538200
};

STDROMPICKEXT(kof95b, kof95b, neogeo)
STD_ROM_FN(kof95b)

struct BurnDriver BurnDrvKof95b = {
	"kof95b", "kof95", "neogeo", NULL, "1995",
	"The King of Fighters '95 (Enable Hidden Characters V.[?])\0", NULL, "Ydmis & Creamymami[EGCG]", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE |  BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_VSFIGHT, FBF_KOF,
	NULL, kof95bRomInfo, kof95bRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '96 (Anniversary Edition, EGHT hack)

static struct BurnRomInfo kof96aeRomDesc[] = {
	{ "214-p1ae.bin",	0x100000, 0x47660e7c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "214-p2ae.bin",	0x400000, 0x824ff3eb, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "214-s1ae.bin",	0x020000, 0x203b3aaf, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "214-c1ae.bin",	0x400000, 0xb308c1f9, 3 | BRF_GRA },           //  3 Sprite data
	{ "214-c2ae.bin",	0x400000, 0x220a92b2, 3 | BRF_GRA },           //  4
	{ "214-c3.c3",		0x400000, 0x64989a65, 3 | BRF_GRA },           //  5
	{ "214-c4.c4",		0x400000, 0xafbea515, 3 | BRF_GRA },           //  6
	{ "214-c5ae.bin",	0x400000, 0x6e03cb0c, 3 | BRF_GRA },           //  7
	{ "214-c6ae.bin",	0x400000, 0xa90bc55d, 3 | BRF_GRA },           //  8
	{ "214-c7ae.bin",	0x400000, 0x08021fd6, 3 | BRF_GRA },           //  9
	{ "214-c8ae.bin",	0x400000, 0xbbd21b08, 3 | BRF_GRA },           // 10
	{ "214-c9ae.bin",	0x400000, 0x57e5bf80, 3 | BRF_GRA },           // 11
	{ "214-c10ae.bin",	0x400000, 0xcbe44c81, 3 | BRF_GRA },           // 12
	{ "214-c11ae.bin",	0x400000, 0x5c7ddc8b, 3 | BRF_GRA },           // 13
	{ "214-c12ae.bin",	0x400000, 0x48d8b574, 3 | BRF_GRA },           // 14

	{ "214-m1ae.bin",	0x020000, 0xc9dd0348, 4 | BRF_ESS | BRF_PRG }, // 15 Z80 code

	{ "214-v1.v1",		0x400000, 0x63f7b045, 5 | BRF_SND },           // 16 Sound data
	{ "214-v2.v2",		0x400000, 0x25929059, 5 | BRF_SND },           // 17
	{ "214-v3ae.bin",	0x400000, 0x143923a7, 5 | BRF_SND },           // 18
};

STDROMPICKEXT(kof96ae, kof96ae, neogeo)
STD_ROM_FN(kof96ae)

struct BurnDriver BurnDrvkof96ae = {
	"kof96ae", "kof96", "neogeo", NULL, "2007",
	"The King of Fighters '96 (Anniversary Edition, EGHT hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof96aeRomInfo, kof96aeRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '96 (The Anniversary Edition 2.0, Build 2.3.0320)

static struct BurnRomInfo kof96ae20RomDesc[] = {
	{ "214-p1ae2.p1",		0x100000, 0xc718ea76, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "214-p2ae2.p2",		0x400000, 0x2638be07, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "214-s1ae2.s1",		0x020000, 0xb9626494, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "214-c1ae2.c1",		0x800000, 0xa7466eea, 3 | BRF_GRA },           //  3 Sprite data
	{ "214-c2ae2.c2",		0x800000, 0x6ff22256, 3 | BRF_GRA },           //  4
	{ "214-c3ae2.c3",		0x800000, 0x48d81318, 3 | BRF_GRA },           //  5
	{ "214-c4ae2.c4",		0x800000, 0xd69836e6, 3 | BRF_GRA },           //  6
	{ "214-c5ae2.c5",		0x800000, 0xed71e6bb, 3 | BRF_GRA },           //  7
	{ "214-c6ae2.c6",		0x800000, 0x059fa364, 3 | BRF_GRA },           //  8

	{ "214-m1ae2.m1",		0x020000, 0x3a4a7c21, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "214-v1.v1",			0x400000, 0x63f7b045, 5 | BRF_SND },           // 10 Sound data
	{ "214-v2.v2",			0x400000, 0x25929059, 5 | BRF_SND },           // 11
	{ "214-v3ae2.v3",		0x400000, 0xf85673b0, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof96ae20, kof96ae20, neogeo)
STD_ROM_FN(kof96ae20)

struct BurnDriver BurnDrvkof96ae20 = {
	"kof96ae20", "kof96", "neogeo", NULL, "2020",
	"The King of Fighters '96 (The Anniversary Edition 2.0, Build 2.3.0320)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof96ae20RomInfo, kof96ae20RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '96 (Chinese Edition ver 1.0, hack)

static struct BurnRomInfo kof96cnRomDesc[] = {
	{ "214cn-p1.bin", 0x100000, 0xa8c25f0a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "214cn-p2.bin", 0x200000, 0x0edface5, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "214cn-p3.bin", 0x500000, 0x8724a441, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "214cn-s1.bin", 0x020000, 0xf917527a, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "214-c1.bin",   0x400000, 0x7ecf4aa2, 3 | BRF_GRA },           //  4 Sprite data
	{ "214-c2.bin",   0x400000, 0x05b54f37, 3 | BRF_GRA },           //  5
	{ "214-c3.bin",   0x400000, 0x64989a65, 3 | BRF_GRA },           //  6
	{ "214-c4.bin",   0x400000, 0xafbea515, 3 | BRF_GRA },           //  7
	{ "214-c5.bin",   0x400000, 0x2a3bbd26, 3 | BRF_GRA },           //  8
	{ "214-c6.bin",   0x400000, 0x44d30dc7, 3 | BRF_GRA },           //  9
	{ "214-c7.bin",   0x400000, 0x3687331b, 3 | BRF_GRA },           // 10
	{ "214-c8.bin",   0x400000, 0xfa1461ad, 3 | BRF_GRA },           // 11
	{ "214cn-c9.bin", 0x400000, 0xa9f811d2, 3 | BRF_GRA },           // 12
	{ "214cn-c10.bin",0x400000, 0x1147406a, 3 | BRF_GRA },           // 13

	{ "214-m1.bin",   0x020000, 0xdabc427c, 4 | BRF_ESS | BRF_PRG }, // 14 Z80 code

	{ "214-v1.bin",   0x400000, 0x63f7b045, 5 | BRF_SND },           // 15 Sound data
	{ "214-v2.bin",   0x400000, 0x25929059, 5 | BRF_SND },           // 16
	{ "214-v3.bin",   0x200000, 0x92a2257d, 5 | BRF_SND },           // 17
};

STDROMPICKEXT(kof96cn, kof96cn, neogeo)
STD_ROM_FN(kof96cn)

struct BurnDriver BurnDrvkof96cn = {
	"kof96cn", "kof96", "neogeo", NULL, "200?",
	"The King of Fighters '96 (Chinese Edition ver 1.0, hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof96cnRomInfo, kof96cnRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '96 (bootleg / hack)

static struct BurnRomInfo kof96epRomDesc[] = {
	{ "214-p1.bin",   0x100000, 0x52755d74, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "214-p2.bin",   0x200000, 0x002ccb73, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "214-s1.bin",   0x020000, 0x1254cbdb, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "214-c1.bin",   0x400000, 0x7ecf4aa2, 3 | BRF_GRA },           //  3 Sprite data
	{ "214-c2.bin",   0x400000, 0x05b54f37, 3 | BRF_GRA },           //  4
	{ "214-c3.bin",   0x400000, 0x64989a65, 3 | BRF_GRA },           //  5
	{ "214-c4.bin",   0x400000, 0xafbea515, 3 | BRF_GRA },           //  6
	{ "214-c5.bin",   0x400000, 0x2a3bbd26, 3 | BRF_GRA },           //  7
	{ "214-c6.bin",   0x400000, 0x44d30dc7, 3 | BRF_GRA },           //  8
	{ "214-c7.bin",   0x400000, 0x3687331b, 3 | BRF_GRA },           //  9
	{ "214-c8.bin",   0x400000, 0xfa1461ad, 3 | BRF_GRA },           // 10

	{ "214-m1.bin",   0x020000, 0xdabc427c, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "214-v1.bin",   0x400000, 0x63f7b045, 5 | BRF_SND },           // 12 Sound data
	{ "214-v2.bin",   0x400000, 0x25929059, 5 | BRF_SND },           // 13
	{ "214-v3.bin",   0x200000, 0x92a2257d, 5 | BRF_SND },           // 14

	{ "kof96m_ep1.rom",0x080000, 0xe5f2fb43, 0 | BRF_ESS | BRF_PRG }, // 15
};

STDROMPICKEXT(kof96ep, kof96ep, neogeo)
STD_ROM_FN(kof96ep)

static void kof96epCallback()
{
	UINT8 *pTemp = (UINT8*)BurnMalloc(0x80000);

	if (pTemp) {
		BurnLoadRom(pTemp, 15, 1);

		for (UINT32 i = 0; i < 0x80000; i++) {
			if (pTemp[i] - Neo68KROMActive[i] == 8) pTemp[i] = Neo68KROMActive[i];
		}

		memcpy(Neo68KROMActive, pTemp, 0x80000);

		BurnFree(pTemp);
	}
}

static INT32 kof96epInit()
{
	NeoCallbackActive->pInitialise = kof96epCallback;

 	return NeoInit();
}

struct BurnDriver BurnDrvkof96ep = {
	"kof96ep", "kof96", "neogeo", NULL, "1996",
	"The King of Fighters '96 (bootleg / hack)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof96epRomInfo, kof96epRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof96epInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '97 (10th Anniversary Chinese Edition, EGHT hack)

static struct BurnRomInfo kof97cnRomDesc[] = {
	{ "232cn-p1.bin", 0x100000, 0x082933c8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "232cn-p2.bin", 0x600000, 0xd22d3ca3, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "232cn-s1.bin", 0x020000, 0xde5ad278, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "232cn-c1.bin", 0x800000, 0x8c3d2c0c, 3 | BRF_GRA },           //  3 Sprite data
	{ "232cn-c2.bin", 0x800000, 0x43ce69ae, 3 | BRF_GRA },           //  4
	{ "232-c3.bin",   0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.bin",   0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
	{ "232-c5.bin",   0x400000, 0x34fc4e51, 3 | BRF_GRA },           //  7
	{ "232-c6.bin",   0x400000, 0x4ff4d47b, 3 | BRF_GRA },           //  8

	{ "232-m1.bin",   0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.bin",   0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.bin",   0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.bin",   0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97cn, kof97cn, neogeo)
STD_ROM_FN(kof97cn)

struct BurnDriver BurnDrvkof97cn = {
	"kof97cn", "kof97", "neogeo", NULL, "2007",
	"The King of Fighters '97 (10th Anniversary Chinese Edition, EGHT hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97cnRomInfo, kof97cnRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '97 Plus 2003 (bootleg / hack)

static struct BurnRomInfo kof97plaRomDesc[] = {
	{ "kf97-p1a.bin", 0x100000, 0x035911c5, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "232-p2.bin",   0x400000, 0x158b23f6, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "kf97-s1a.bin", 0x020000, 0x4ee2149a, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "232-c1.bin",   0x800000, 0x5f8bf0a1, 3 | BRF_GRA },           //  3 Sprite data
	{ "232-c2.bin",   0x800000, 0xe4d45c81, 3 | BRF_GRA },           //  4
	{ "232-c3.bin",   0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.bin",   0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
	{ "232-c5.bin",   0x400000, 0x34fc4e51, 3 | BRF_GRA },           //  7
	{ "232-c6.bin",   0x400000, 0x4ff4d47b, 3 | BRF_GRA },           //  8

	{ "232-m1.bin",   0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.bin",   0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.bin",   0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.bin",   0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97pla, kof97pla, neogeo)
STD_ROM_FN(kof97pla)

static void kof97plaCallback()
{
	lans2004_sx_decode();
}

static INT32 kof97plaInit()
{
	NeoCallbackActive->pInitialise = kof97plaCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvkof97pla = {
	"kof97pla", "kof97", "neogeo", NULL, "2003",
	"The King of Fighters '97 Plus 2003 (bootleg / hack)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97plaRomInfo, kof97plaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof97plaInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '97 oroshi plus 2003

static struct BurnRomInfo kof97oroRomDesc[] = {
	{ "orochi-p1.bin",  0x0100000, 0x6dcb2946, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "orochi-p21.bin", 0x0200000, 0x6e1c4d8c, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "orochi-p29.bin", 0x0200000, 0x4c7c0221, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "orochi-s1.bin",  0x0020000, 0x4ee2149a, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "orochi-c1.bin",  0x1000000, 0xf13e841c, 3 | BRF_GRA },           //  4 Sprite data
	{ "orochi-c2.bin",  0x1000000, 0x2db1f6d3, 3 | BRF_GRA },           //  5
	{ "orochi-c51.bin", 0x0200000, 0xa90340cb, 3 | BRF_GRA },           //  6
	{ "orochi-c61.bin", 0x0200000, 0x188e351a, 3 | BRF_GRA },           //  7
	{ "orochi-c52.bin", 0x0200000, 0xd4eec50a, 3 | BRF_GRA },           //  8
	{ "orochi-c62.bin", 0x0200000, 0x031b1ad5, 3 | BRF_GRA },           //  9

	{ "orochi-m1.bin",  0x0020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "orochi-v1.bin",  0x0400000, 0x22a2b5b5, 5 | BRF_SND },           // 11 Sound data
	{ "orochi-v2.bin",  0x0400000, 0x2304e744, 5 | BRF_SND },           // 12
	{ "orochi-v3.bin",  0x0400000, 0x759eb954, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kof97oro, kof97oro, neogeo)
STD_ROM_FN(kof97oro)

static void kof97oro_px_decode()
{
	int i;
	UINT16 *tmp = (UINT16*)BurnMalloc(0x500000);
	UINT16 *src = (UINT16*)Neo68KROMActive;

	for (i = 0; i < 0x100000/2; i++) {
		tmp[i] = src[i ^ 0x7ffef];
	}

	for (i = 0; i < 0x400000/2; i++) {
		tmp[i + (0x100000/2)] = src[(i ^ 0xfffef) + (0x100000/2)];
	}

	memcpy (src, tmp, 0x500000);

	BurnFree (tmp);
}

static void kof97oroCallback()
{
	kof97oro_px_decode();
	lans2004_cx_decode(0x2800000);
	lans2004_sx_decode();
}

static INT32 kof97oroInit()
{
 	INT32 nRet;

	NeoCallbackActive->pInitialise = kof97oroCallback;

 	nRet = NeoInit();

	return nRet;
}

struct BurnDriver BurnDrvKof97oro = {
	"kof97oro", "kof97", "neogeo", NULL, "1997",
	"The King of Fighters '97 oroshi plus 2003\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97oroRomInfo, kof97oroRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof97oroInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '97 - Final Battle (hack)

static struct BurnRomInfo kof97xtRomDesc[] = {
	{ "232xt-p1.bin", 0x100000, 0x2e4f4e3b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "232xt-p2.bin", 0x400000, 0x6d4503ce, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "232xt-s1.bin", 0x020000, 0xd6fe166f, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "232xt-c1.bin", 0x800000, 0xd504bf4a, 3 | BRF_GRA },           //  3 Sprite data
	{ "232xt-c2.bin", 0x800000, 0x942ea708, 3 | BRF_GRA },           //  4
	{ "232-c3.bin",   0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.bin",   0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
	{ "232xt-c5.bin", 0x800000, 0xc1bd2375, 3 | BRF_GRA },           //  7
	{ "232xt-c6.bin", 0x800000, 0xa7c8506a, 3 | BRF_GRA },           //  8

	{ "232-m1.bin",   0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.bin",   0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.bin",   0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.bin",   0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97xt, kof97xt, neogeo)
STD_ROM_FN(kof97xt)

struct BurnDriver BurnDrvkof97xt = {
	"kof97xt", "kof97", "neogeo", NULL, "2007",
	"The King of Fighters '97 - Final Battle (hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97xtRomInfo, kof97xtRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '97 - Random Combo Version 2010 (Hack)
// *IVEX KOF 97 COMBO HACK WITH RANDOM SELECT* //

static struct BurnRomInfo kof97rcRomDesc[] = {
	{ "232-rc.p1",    0x100000, 0x1d476235, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "232-rc.sp2",   0x400000, 0x406e4a4b, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "232-rc.s1",    0x020000, 0xd4957067, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "232-rc.c1",    0x800000, 0xe2fe7d7a, 3 | BRF_GRA },           //  3 Sprite data
	{ "232-rc.c2",    0x800000, 0xd0746893, 3 | BRF_GRA },           //  4
	{ "232-c3.c3",    0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.c4",    0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
	{ "232-c5.c5",    0x400000, 0x34fc4e51, 3 | BRF_GRA },           //  7
	{ "232-c6.c6",    0x400000, 0x4ff4d47b, 3 | BRF_GRA },           //  8

	{ "232-m1.m1",    0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.v1",    0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.v2",    0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.v3",    0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97rc, kof97rc, neogeo)
STD_ROM_FN(kof97rc)

struct BurnDriver BurnDrvkof97rc = {
	"kof97rc", "kof97", "neogeo", NULL, "2019",
	"The King of Fighters '97 - Random Combo (Hack, Ver. 2010)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97rcRomInfo, kof97rcRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '97 - Combo Training Version 2018 (Hack)

static struct BurnRomInfo kof97cbtRomDesc[] = {
	{ "97cbt-p1.bin",       0x100000, 0x323e4263, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "97cbt-p2.bin",       0x400000, 0x91c0cfdb, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "kof97cbt-s1.bin",    0x020000, 0xd4957067, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "kof97cbt-c1.bin",    0x800000, 0xea16b711, 3 | BRF_GRA },           //  3 Sprite data
	{ "kof97cbt-c2.bin",    0x800000, 0xb7bef674, 3 | BRF_GRA },           //  4
	{ "232-c3.c3",          0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.c4",          0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
	{ "kof97cbt-c5.bin",    0x400000, 0x6fe02054, 3 | BRF_GRA },           //  7
	{ "kof97cbt-c6.bin",    0x400000, 0x0f96c84a, 3 | BRF_GRA },           //  8

	{ "232-m1.m1",          0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.v1",          0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.v2",          0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.v3",          0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97cbt, kof97cbt, neogeo)
STD_ROM_FN(kof97cbt)

struct BurnDriver BurnDrvkof97cbt = {
	"kof97cbt", "kof97", "neogeo", NULL, "2019",
	"The King of Fighters '97 - Combo Training (Hack, Ver. 2018)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97cbtRomInfo, kof97cbtRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '97 (Optimized Edition)
// GOTVG (Patched version)
// kof97t in HBMAME.

static struct BurnRomInfo kof97tRomDesc[] = {
    { "232t.p1",    	0x100000, 0x45834e9b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
    { "232t.p2",    	0x400000, 0xd9e51750, 1 | BRF_ESS | BRF_PRG }, //  1

    { "232-s1.s1",    	0x020000, 0x8514ecf5, 2 | BRF_GRA },           //  2 Text layer tiles

    { "232t.c1",    	0x800000, 0xb7f5a3b9, 3 | BRF_GRA },           //  3 Sprite data
    { "232t.c2",    	0x800000, 0x959d6d78, 3 | BRF_GRA },           //  4
    { "232-c3.c3",   	0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.c4",    	0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
    { "232-c5.c5",    	0x400000, 0x34fc4e51, 3 | BRF_GRA },           //  7
	{ "232-c6.c6",    	0x400000, 0x4ff4d47b, 3 | BRF_GRA },           //  8

    { "232-m1.m1",    	0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.v1",    	0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.v2",    	0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.v3",    	0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97t, kof97t, neogeo)
STD_ROM_FN(kof97t)

static void kof97tPatchCallback()
{
	if (!bDoIpsPatch) {

		// Crash Patch
		UINT32 patch_fix[] = {
			0x0001a0, 0xe7, 0x0001a1, 0x48, 0x0001a2, 0x00, 0x0001a3, 0x80, 0x0001a4, 0x39, 0x0001a5, 0x20, 0x0001a6, 0x10, 0x0001a7, 0x00,
			0x0001a8, 0x00, 0x0001a9, 0x85, 0x0001aa, 0xbc, 0x0001ab, 0xb0, 0x0001ac, 0x00, 0x0001ad, 0x00, 0x0001ae, 0x18, 0x0001af, 0xea,
			0x0001b0, 0x00, 0x0001b1, 0x67, 0x0001b2, 0x10, 0x0001b3, 0x00, 0x0001b4, 0xbc, 0x0001b5, 0xb0, 0x0001b6, 0x01, 0x0001b7, 0x00,
			0x0001b8, 0xd0, 0x0001b9, 0xf9, 0x0001ba, 0x00, 0x0001bb, 0x67, 0x0001bc, 0x14, 0x0001bd, 0x00, 0x0001be, 0x00, 0x0001bf, 0x60,
			0x0001c0, 0x20, 0x0001c1, 0x00, 0x0001c2, 0x3c, 0x0001c3, 0x30, 0x0001c4, 0x8a, 0x0001c5, 0x59, 0x0001c6, 0xc0, 0x0001c7, 0x33,
			0x0001c8, 0x10, 0x0001c9, 0x00, 0x0001ca, 0x16, 0x0001cb, 0xa8, 0x0001cc, 0x00, 0x0001cd, 0x60, 0x0001ce, 0x12, 0x0001cf, 0x00,
			0x0001d0, 0x3c, 0x0001d1, 0x30, 0x0001d2, 0x8a, 0x0001d3, 0x89, 0x0001d4, 0xc0, 0x0001d5, 0x33, 0x0001d6, 0x10, 0x0001d7, 0x00,
			0x0001d8, 0x38, 0x0001d9, 0x82, 0x0001da, 0xc0, 0x0001db, 0x33, 0x0001dc, 0x10, 0x0001dd, 0x00, 0x0001de, 0x38, 0x0001df, 0x84,
			0x0001e0, 0xdf, 0x0001e1, 0x4c, 0x0001e2, 0x01, 0x0001e3, 0x00, 0x0001e4, 0xc0, 0x0001e5, 0x13, 0x0001e6, 0x30, 0x0001e7, 0x00,
			0x0001e8, 0x01, 0x0001e9, 0x00, 0x0001ea, 0x75, 0x0001eb, 0x4e, 0x009ebe, 0xb9, 0x009ebf, 0x4e, 0x009ec0, 0x00, 0x009ec2, 0xa0,
			0x009ec3, 0x01, 0x022458, 0xed, 0x02245a, 0x3f, 0x02245b, 0x28, 0x022464, 0xed, 0x022466, 0x50, 0x022467, 0x28, 0x02246c, 0x48,
			0x02246d, 0x50, 0x02246e, 0x4a, 0x02246f, 0x50, 0x02247a, 0xe0, 0x02247b, 0x00, 0x02247e, 0x04, 0x022482, 0x05, 0x022486, 0x06,
			0x0224b6, 0xc6, 0x0224b8, 0x2d, 0x0224b9, 0x09, 0x0224ba, 0xf2, 0x0224bb, 0x27, 0x0224c8, 0x98, 0x0224c9, 0x6d, 0x0224d2, 0x99,
			0x0224d3, 0x6d, 0x0224fc, 0xc2, 0x0224fe, 0xc2, 0x022502, 0xc6, 0x022504, 0xc6, 0x022508, 0xe0, 0x02250a, 0xe0, 0x02250e, 0xe4,
			0x022510, 0xe4, 0x022514, 0xe8, 0x022516, 0xe8, 0x02252a, 0xb8, 0x02252b, 0x4e, 0x02252c, 0x26, 0x02252d, 0x30, 0x02252e, 0x40,
			0x02252f, 0x02, 0x022530, 0x1f, 0x022531, 0x00, 0x022532, 0x40, 0x022533, 0x39, 0x022534, 0xd6, 0x022535, 0x00, 0x022536, 0xbc,
			0x022537, 0x28, 0x022538, 0x02, 0x022539, 0x00, 0x02253a, 0x3c, 0x02253b, 0x25, 0x02253c, 0x00, 0x02253d, 0x70, 0x02253e, 0x6c,
			0x02253f, 0x26, 0x022540, 0x84, 0x022541, 0x00, 0x022542, 0x6c, 0x022543, 0x20, 0x022544, 0xc6, 0x022545, 0x00, 0x022546, 0x28,
			0x022547, 0x1c, 0x022548, 0x03, 0x022549, 0x00, 0x02254a, 0x2c, 0x02254b, 0x4a, 0x02254c, 0xf8, 0x02254d, 0x00, 0x02254e, 0x74,
			0x02254f, 0x66, 0x022552, 0xfa, 0x022554, 0x14, 0x022556, 0x40, 0x022557, 0x11, 0x022558, 0x0a, 0x022559, 0x00, 0x02255a, 0x00,
			0x02255b, 0x52, 0x02255c, 0x40, 0x02255d, 0x11, 0x02255e, 0x0b, 0x02255f, 0x00, 0x022560, 0x00, 0x022561, 0x52, 0x022562, 0x40,
			0x022563, 0x11, 0x022564, 0x0c, 0x022565, 0x00, 0x022566, 0x2c, 0x022567, 0x52, 0x022568, 0xfa, 0x022569, 0x00, 0x02256a, 0x28,
			0x02256b, 0x10, 0x02256c, 0x01, 0x02256d, 0x00, 0x02256e, 0x00, 0x02256f, 0x0c, 0x022570, 0x0b, 0x022571, 0x00, 0x022572, 0x20,
			0x022573, 0x67, 0x022574, 0x00, 0x022575, 0x0c, 0x022576, 0x0c, 0x022577, 0x00, 0x022578, 0x1a, 0x022579, 0x67, 0x02257a, 0x00,
			0x02257b, 0x0c, 0x02257c, 0x0e, 0x02257d, 0x00, 0x02257e, 0x14, 0x02257f, 0x67, 0x022580, 0x00, 0x022581, 0x0c, 0x022582, 0x0d,
			0x022583, 0x00, 0x022584, 0x14, 0x022585, 0x66, 0x022586, 0x28, 0x022587, 0x10, 0x022588, 0x0a, 0x022589, 0x00, 0x02258a, 0x68,
			0x02258b, 0x11, 0x02258c, 0x0c, 0x02258e, 0x0a, 0x02258f, 0x00, 0x022590, 0x40, 0x022591, 0x11, 0x022592, 0x0c, 0x022593, 0x00,
			0x022594, 0x03, 0x022595, 0x7c, 0x022596, 0x6c, 0x022597, 0x53, 0x022598, 0xd6, 0x022599, 0x00, 0x02259a, 0x6c, 0x02259b, 0x20,
			0x02259c, 0xc2, 0x02259d, 0x00, 0x02259e, 0x00, 0x02259f, 0x70, 0x0225a0, 0x80, 0x0225a1, 0x10, 0x0225bc, 0x70, 0x0225c0, 0xc2,
			0x0225ca, 0x06, 0x0225cc, 0x02, 0x0225d2, 0x70, 0x0225d6, 0xc2, 0x0225dc, 0xc2, 0x0225e4, 0x40, 0x0225e5, 0x02, 0x0225e6, 0x70,
			0x0225e7, 0x00, 0x0225e9, 0x4a, 0x0225ea, 0x00, 0x0225eb, 0x67, 0x0225ec, 0x4a, 0x0225ed, 0x00, 0x0225ee, 0x04, 0x0225f0, 0x02,
			0x0225fa, 0xf4, 0x0225fe, 0x0a, 0x022620, 0xf4, 0x022626, 0xe7, 0x022627, 0x48, 0x022628, 0x80, 0x022629, 0xc0, 0x02262a, 0x3c,
			0x02262b, 0x30, 0x02262c, 0x1d, 0x02262d, 0x00, 0x02262e, 0xb8, 0x02262f, 0x4e, 0x022631, 0x77, 0x022632, 0xdf, 0x022633, 0x4c,
			0x022634, 0x03, 0x022635, 0x01, 0x022636, 0x46, 0x022637, 0x11, 0x022638, 0x03, 0x02263a, 0x06, 0x02263b, 0x0c, 0x02263c, 0x03,
			0x02263d, 0x00, 0x022648, 0x4a, 0x022664, 0xfa, 0x022665, 0x45, 0x022666, 0xe6, 0x022667, 0xff, 0x02266e, 0xfa, 0x02266f, 0x45,
			0x022670, 0xe8, 0x022671, 0xff, 0x022672, 0x4a, 0x022678, 0xc6, 0x0226fe, 0x0a, 0x022700, 0x98, 0x022701, 0xc6, 0x0227b6, 0x83,
			0x0227b7, 0xec, 0x0227b8, 0x24, 0x0227b9, 0x83, 0x0227ba, 0x6c, 0x0227bb, 0x24, 0x0227bc, 0x38, 0x0227bd, 0x01, 0x0227be, 0x8b,
			0x0227bf, 0x4c, 0x0227c0, 0x24, 0x0227c1, 0x28, 0x0227c2, 0x8b, 0x0227c3, 0x81, 0x0227c4, 0xa4, 0x0227c5, 0x01, 0x0227c6, 0x00,
			0x0227c7, 0x00, 0x0227c8, 0x8b, 0x0227c9, 0x91, 0x0227ca, 0x20, 0x0227cb, 0x01, 0x0227cc, 0x00, 0x0227cd, 0x00, 0x0227ce, 0x55,
			0x0227cf, 0x8b, 0x0227d0, 0x69, 0x0227d1, 0x5c, 0x0227d2, 0x8b, 0x0227d3, 0x48, 0x0227d4, 0x08, 0x0227d5, 0x89, 0x0227d6, 0x4c,
			0x0227d7, 0x24, 0x0227d8, 0x0c, 0x0227d9, 0x8b, 0x0227da, 0x48, 0x0227db, 0x0c, 0x0227dc, 0x89, 0x0227dd, 0x4c, 0x0227de, 0x24,
			0x0227df, 0x1c, 0x0227e0, 0x8b, 0x0227e1, 0x48, 0x0227e2, 0x10, 0x0227e3, 0x8b, 0x0227e4, 0x40, 0x0227e5, 0x14, 0x0227e6, 0x89,
			0x0227e7, 0x6c, 0x0227e8, 0x24, 0x0227e9, 0x20, 0x0227ea, 0x89, 0x0227eb, 0x54, 0x0227ec, 0x24, 0x0227ed, 0x18, 0x0227ee, 0x89,
			0x0227ef, 0x4c, 0x0227f0, 0x24, 0x0227f1, 0x14, 0x0227f2, 0x89, 0x0227f3, 0x44, 0x0227f4, 0x24, 0x0227f5, 0x10, 0x0227f6, 0xcc,
			0x0227f7, 0xcc, 0x0227f8, 0xcc, 0x0227f9, 0xcc, 0x0227fa, 0xcc, 0x0227fb, 0xcc, 0x0227fc, 0xcc, 0x0227fd, 0xcc, 0x0227fe, 0xac,
			0x0227ff, 0x11
		};

		for (INT32 i = 0; i < (sizeof(patch_fix) / sizeof(UINT32)) >> 1; i++)
			Neo68KROMActive[patch_fix[(i << 1) + 0]] = (UINT8)patch_fix[(i << 1) + 1];
	}
}

static INT32 kof97tInit()
{
	NeoCallbackActive->pInitialise = kof97tPatchCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvkof97t = {
	"kof97t", "kof97", "neogeo", NULL, "2014",
	"The King of Fighters '97 (Optimized Edition)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97tRomInfo, kof97tRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof97tInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '97 - Anniversary Edition (Build 2.1.0212)
// Hacked by AndyChan

static struct BurnRomInfo kof97aeRomDesc[] = {
	{ "232-p1ae.p1",	0x100000, 0xcecc4f64, 1 | BRF_ESS | BRF_PRG },	//  0 68K code
	{ "232-p2ae.sp2",	0x400000, 0x228aa8d1, 1 | BRF_ESS | BRF_PRG },	//  1

	{ "232-s1ae.s1",	0x020000, 0x30a6b3b8, 2 | BRF_GRA },			//  2 Text layer tiles

	{ "232-c1ae.c1",	0x800000, 0x5f0133eb, 3 | BRF_GRA },			//  3 Sprite data
	{ "232-c2ae.c2",	0x800000, 0x136b8449, 3 | BRF_GRA },			//  4
	{ "232-c3.c3",		0x800000, 0x581d6618, 3 | BRF_GRA },			//  5
	{ "232-c4.c4",		0x800000, 0x49bb1e68, 3 | BRF_GRA },			//  6
	{ "232-c5ae.c5",	0x800000, 0x8167b444, 3 | BRF_GRA },			//  7
	{ "232-c6ae.c6",	0x800000, 0xa410f3ca, 3 | BRF_GRA },			//  8
	{ "232-c7ae.c7",	0x800000, 0x50a48a56, 3 | BRF_GRA },			//  9
	{ "232-c8ae.c8",	0x800000, 0x80d7ec00, 3 | BRF_GRA },			//  10

	{ "232-m1ae.m1",	0x020000, 0xf7b7ee13, 4 | BRF_ESS | BRF_PRG },	//  11 Z80 code

	{ "232-v1.v1",		0x400000, 0x22a2b5b5, 5 | BRF_SND },			//  12 Sound data
	{ "232-v2.v2",		0x400000, 0x2304e744, 5 | BRF_SND },			//  13
	{ "232-v3.v3",		0x400000, 0x759eb954, 5 | BRF_SND },			//  14
	{ "232-v4ae.v4",	0x200000, 0x7d37caa0, 5 | BRF_SND },			//  15
};

STDROMPICKEXT(kof97ae, kof97ae, neogeo)
STD_ROM_FN(kof97ae)

struct BurnDriver BurnDrvkof97ae = {
	"kof97ae", "kof97", "neogeo", NULL, "2018",
	"The King of Fighters '97 - Anniversary Edition (Build 2.1.0212)\0", NULL, "EGHT", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97aeRomInfo, kof97aeRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '97 - Anniversary Edition (Build 2.1.1811)
// Second hack by Ice Flame Fantasy 2020/08/10
// Fork from Kof97ae (Original author - AndyChan)

static struct BurnRomInfo kof97aefRomDesc[] = {
	{ "232-p1aef.p1",	0x100000, 0xe453b221, 1 | BRF_ESS | BRF_PRG },	//  0 68K code
	{ "232-p2aef.sp2",	0x400000, 0xf00ba90d, 1 | BRF_ESS | BRF_PRG },	//  1

	{ "232-s1ae.s1",	0x020000, 0x30a6b3b8, 2 | BRF_GRA },			//  2 Text layer tiles

	{ "232-c1ae.c1",	0x800000, 0x5f0133eb, 3 | BRF_GRA },			//  3 Sprite data
	{ "232-c2ae.c2",	0x800000, 0x136b8449, 3 | BRF_GRA },			//  4
	{ "232-c3.c3",		0x800000, 0x581d6618, 3 | BRF_GRA },			//  5
	{ "232-c4.c4",		0x800000, 0x49bb1e68, 3 | BRF_GRA },			//  6
	{ "232-c5ae.c5",	0x800000, 0x8167b444, 3 | BRF_GRA },			//  7
	{ "232-c6ae.c6",	0x800000, 0xa410f3ca, 3 | BRF_GRA },			//  8
	{ "232-c7ae.c7",	0x800000, 0x50a48a56, 3 | BRF_GRA },			//  9
	{ "232-c8ae.c8",	0x800000, 0x80d7ec00, 3 | BRF_GRA },			//  10

	{ "232-m1ae.m1",	0x020000, 0xf7b7ee13, 4 | BRF_ESS | BRF_PRG },	//  11 Z80 code

	{ "232-v1.v1",		0x400000, 0x22a2b5b5, 5 | BRF_SND },			//  12 Sound data
	{ "232-v2.v2",		0x400000, 0x2304e744, 5 | BRF_SND },			//  13
	{ "232-v3.v3",		0x400000, 0x759eb954, 5 | BRF_SND },			//  14
	{ "232-v4ae.v4",	0x200000, 0x7d37caa0, 5 | BRF_SND },			//  15
};

STDROMPICKEXT(kof97aef, kof97aef, neogeo)
STD_ROM_FN(kof97aef)

static INT32 kof97aefInit()
{
	INT32 nRet = NeoInit();

	/* Make sure that the data is not modified again after the ips is loaded. */
	if ((nRet == 0) && (*((UINT16*)(Neo68KROMActive + 0x9b12a)) == 0x48e7)) *((UINT16*)(Neo68KROMActive + 0x9b12a)) = BURN_ENDIAN_SWAP_INT16(0x6038);

	return nRet;
}

struct BurnDriver BurnDrvkof97aef = {
	"kof97aef", "kof97", "neogeo", NULL, "2020-08-10",
	"The King of Fighters '97 - Anniversary Edition (Build 2.1.1811)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97aefRomInfo, kof97aefRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	kof97aefInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '97 (Optimised Edition 2020)
// Second hack by Unknown
// Fork from kof97t (GOTVG)

static struct BurnRomInfo kof97tpRomDesc[] = {
	{ "232-p1tp.p1",	0x100000, 0x53d9851d, 1 | BRF_ESS | BRF_PRG },	//  0 68K code
	{ "232-p2tp.sp2",	0x400000, 0x6d448462, 1 | BRF_ESS | BRF_PRG },	//  1

	{ "232-s1tp.s1",	0x020000, 0xe338d57f, 2 | BRF_GRA },			//  2 Text layer tiles

	{ "232-c1tp.c1",	0x800000, 0x848040cb, 3 | BRF_GRA },			//  3 Sprite data
	{ "232-c2tp.c2",	0x800000, 0xd01ad7c9, 3 | BRF_GRA },			//  4
	{ "232-c3.c3",		0x800000, 0x581d6618, 3 | BRF_GRA },			//  5
	{ "232-c4.c4",		0x800000, 0x49bb1e68, 3 | BRF_GRA },			//  6
	{ "232-c5tp.c5",	0x800000, 0x7bb13ff4, 3 | BRF_GRA },			//  7
	{ "232-c6tp.c6",	0x800000, 0x6846a8a5, 3 | BRF_GRA },			//  8

	{ "232-m1.m1",		0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG },	//  9 Z80 code

	{ "232-v1.v1",		0x400000, 0x22a2b5b5, 5 | BRF_SND },			//  10 Sound data
	{ "232-v2.v2",		0x400000, 0x2304e744, 5 | BRF_SND },			//  11
	{ "232-v3.v3",		0x400000, 0x759eb954, 5 | BRF_SND },			//  12
};

STDROMPICKEXT(kof97tp, kof97tp, neogeo)
STD_ROM_FN(kof97tp)

struct BurnDriver BurnDrvkof97tp = {
	"kof97tp", "kof97", "neogeo", NULL, "2020",
	"The King of Fighters '97 (Optimised Edition 2020)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97tpRomInfo, kof97tpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '97 (Invincible Plus!)
// Hacker: Ice Flame Fantasy [IFF] 2021/07/09
/* [IFF] He was very worried that his masterpiece might become a commodity
   Therefore, it is necessary to encrypt his masterpiece in multiple ways to
   prevent what he thinks is a disaster */

static struct BurnRomInfo kof97ipRomDesc[] = {
	/* Encrypted */
	{ "232-p1ip.p1",	0x200000, 0x9cd04061, 1 | BRF_ESS | BRF_PRG },	//  0 68K code
	{ "232-p2ip.sp2",	0x400000, 0x7ce0fc4d, 1 | BRF_ESS | BRF_PRG },	//  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "232-c1ip.c1",	0x800000, 0x8b6315fd, 3 | BRF_GRA },			//  2 Sprite data
	{ "232-c2ip.c2",	0x800000, 0x3fe2d05b, 3 | BRF_GRA },			//  3
	{ "232-c3ip.c3",	0x800000, 0x999955d0, 3 | BRF_GRA },			//  4
	{ "232-c4ip.c4",	0x800000, 0xb30ee22c, 3 | BRF_GRA },			//  5
	{ "232-c5ip.c5",	0x800000, 0xf91f949a, 3 | BRF_GRA },			//  6
	{ "232-c6ip.c6",	0x800000, 0x2ee0de5e, 3 | BRF_GRA },			//  7
	{ "232-c7ip.c7",	0x800000, 0x1a6a322f, 3 | BRF_GRA },			//  8
	{ "232-c8ip.c8",	0x800000, 0xe6b9b8ee, 3 | BRF_GRA },			//  9

	/* Encrypted */
	{ "232-m1ip.m1",	0x020000, 0xfcd044b5, 4 | BRF_ESS | BRF_PRG },	//  10 Z80 code

	/* Encrypted */
	{ "232-v1ip.v1",	0x800000, 0x3f7d07ec, 5 | BRF_SND },			//  11 Sound data
	{ "232-v2ip.v2",	0x800000, 0x517b9787, 5 | BRF_SND },			//  12
};

STDROMPICKEXT(kof97ip, kof97ip, neogeo)
STD_ROM_FN(kof97ip)

static void kof97ipCallback()
{
	kof98Decrypt();
	PCM2DecryptP();

	if (!bDoIpsPatch) {

		// Fixed Gfx
		UINT16 patch_fix[] = {
			0x9932, 0x79, 0x9933, 0x1b, 0x9934, 0x10, 0x9935, 0x00,
			0x9936, 0x90, 0x9937, 0xfd, 0x9938, 0x46, 0x9939, 0x28
		};

		for (INT32 i = 0; i < (sizeof(patch_fix) / sizeof(UINT16)) >> 1; i++)
			Neo68KROMActive[patch_fix[(i << 1) + 0]] = (UINT8)patch_fix[(i << 1) + 1];
	}
}

static INT32 kof97ipInit()
{
	nNeoProtectionXor = 0xEC;
	NeoCallbackActive->pInitialise = kof97ipCallback;

	INT32 nRet = NeoInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0xa5000, 0x000000, { 0xf9, 0xe0, 0x5d, 0xf3, 0xea, 0x92, 0xbe, 0xef } };

		PCM2DecryptV2(&Info);
	}
	return nRet;
}

struct BurnDriver BurnDrvkof97ip = {
	"kof97ip", "kof97", "neogeo", NULL, "2021",
	"The King of Fighters '97 (Invincible Plus! B2.1.2107)\0", NULL, "hack", "Neo Geo AES",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97ipRomInfo, kof97ipRomName, NULL, NULL, NULL, NULL, neoForceAESInputInfo, neoForceAESDIPInfo,
	kof97ipInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '97 (Practice Mode)
// Hacked by Kof1996

static struct BurnRomInfo kof97pmRomDesc[] = {
	{ "232pm-p1.p1",	0x100000, 0x34ccabae, 1 | BRF_ESS | BRF_PRG },	//  0 68K code
	{ "232-p2.sp2",		0x400000, 0x158b23f6, 1 | BRF_ESS | BRF_PRG },	//  1

	{ "232-s1.s1",		0x020000, 0x8514ecf5, 2 | BRF_GRA },			//  2 Text layer tiles

	{ "232-c1.c1",		0x800000, 0x5f8bf0a1, 3 | BRF_GRA },			//  3 Sprite data
	{ "232-c2.c2",		0x800000, 0xe4d45c81, 3 | BRF_GRA },			//  4
	{ "232-c3.c3",		0x800000, 0x581d6618, 3 | BRF_GRA },			//  5
	{ "232-c4.c4",		0x800000, 0x49bb1e68, 3 | BRF_GRA },			//  6
	{ "232-c5.c5",		0x400000, 0x34fc4e51, 3 | BRF_GRA },			//  7
	{ "232-c6.c6",		0x400000, 0x4ff4d47b, 3 | BRF_GRA },			//  8

	{ "232-m1.m1",		0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG },	//  9 Z80 code

	{ "232-v1.v1",		0x400000, 0x22a2b5b5, 5 | BRF_SND },			// 10 Sound data
	{ "232-v2.v2",		0x400000, 0x2304e744, 5 | BRF_SND },			// 11
	{ "232-v3.v3",		0x400000, 0x759eb954, 5 | BRF_SND },			// 12
};

STDROMPICKEXT(kof97pm, kof97pm, neogeo)
STD_ROM_FN(kof97pm)

struct BurnDriver BurnDrvKof97pm = {
	"kof97pm", "kof97", "neogeo", NULL, "1997",
	"The King of Fighters '97 (Practice Mode)\0", NULL, "hack", "Neo Geo AES",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97pmRomInfo, kof97pmRomName, NULL, NULL, NULL, NULL, neoForceAESInputInfo, neoForceAESDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '97 (Imitation Playstation final improved version 2016-10-29 by EGCG/EGGT)

static struct BurnRomInfo kof97psRomDesc[] = {
	{ "232-p1ps.p1",  0x100000, 0xfc25feec, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "232-p2ps.sp2", 0x400000, 0x90723643, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "232-s1.s1",    0x020000, 0x8514ecf5, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "232-c1ps.c1",  0x800000, 0x748ade86, 3 | BRF_GRA },           //  3 Sprite data
	{ "232-c2ps.c2",  0x800000, 0x3270fa6e, 3 | BRF_GRA },           //  4
	{ "232-c3.c3",    0x800000, 0x581d6618, 3 | BRF_GRA },           //  5
	{ "232-c4.c4",    0x800000, 0x49bb1e68, 3 | BRF_GRA },           //  6
	{ "232-c5ps.c5",  0x400000, 0x47711a8c, 3 | BRF_GRA },           //  7
	{ "232-c6ps.c6",  0x400000, 0x6703a48a, 3 | BRF_GRA },           //  8

	{ "232-m1.m1",    0x020000, 0x45348747, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "232-v1.v1",    0x400000, 0x22a2b5b5, 5 | BRF_SND },           // 10 Sound data
	{ "232-v2.v2",    0x400000, 0x2304e744, 5 | BRF_SND },           // 11
	{ "232-v3.v3",    0x400000, 0x759eb954, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof97ps, kof97ps, neogeo)
STD_ROM_FN(kof97ps)

struct BurnDriver BurnDrvKof97ps = {
	"kof97ps", "kof97", "neogeo", NULL, "1997",
	"The King of Fighters '97 (Imitation Playstation final improved version 2016-10-29)\0", NULL, "Eddids", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof97psRomInfo, kof97psRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '98 (Anniversary Edition, EGHT hack)

static struct BurnRomInfo kof98aeRomDesc[] = {
	// Rom information
	/*
	242-p1ae.bin c9188c66 : original P1
	242-p1ae.bin 23a80b3e : kensou 2624+BD bug[by mhloveke520]
	242-p2ae.bin 609fac6b : original P2
	242-p2ae.bin 99b3e5cc : squat error fixes[by Bing Yan Dream]
	*/
//	{ "242-p1ae.bin", 0x100000, 0xc9188c66, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p1ae.bin", 0x100000, 0x23a80b3e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
//	{ "242-p2ae.bin", 0x600000, 0x609fac6b, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "242-p2ae.bin", 0x600000, 0x99b3e5cc, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "242-s1ae.bin", 0x020000, 0xf1fee5c0, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242-c1ae.bin", 0x800000, 0x0319cfc9, 3 | BRF_GRA },           //  3 Sprite data
	{ "242-c2ae.bin", 0x800000, 0x553f6714, 3 | BRF_GRA },           //  4
	{ "242-c3.bin",   0x800000, 0x22127b4f, 3 | BRF_GRA },           //  5
	{ "242-c4.bin",   0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  6
	{ "242-c5ae.bin", 0x800000, 0x71641718, 3 | BRF_GRA },           //  7
	{ "242-c6ae.bin", 0x800000, 0x982ba2b3, 3 | BRF_GRA },           //  8
	{ "242-c7ae.bin", 0x800000, 0x8d495552, 3 | BRF_GRA },           //  9
	{ "242-c8ae.bin", 0x800000, 0x8bfc3417, 3 | BRF_GRA },           // 10
	{ "242-c9ae.bin", 0x800000, 0x128256d1, 3 | BRF_GRA },           // 11
	{ "242-c91ae.bin",0x800000, 0x4bd8412d, 3 | BRF_GRA },           // 12
	{ "242-c92ae.bin",0x800000, 0xe893277f, 3 | BRF_GRA },           // 13
	{ "242-c93ae.bin",0x800000, 0xf165589e, 3 | BRF_GRA },           // 14

	{ "242-m1ae.bin", 0x040000, 0x9ade0528, 4 | BRF_ESS | BRF_PRG }, // 15 Z80 code

	{ "242-v1.bin",   0x400000, 0xb9ea8051, 5 | BRF_SND },           // 16 Sound data
	{ "242-v2.bin",   0x400000, 0xcc11106e, 5 | BRF_SND },           // 17
	{ "242-v3.bin",   0x400000, 0x044ea4e1, 5 | BRF_SND },           // 18
	{ "242-v4.bin",   0x400000, 0x7985ea30, 5 | BRF_SND },           // 19
	{ "242-v5ae.bin", 0x400000, 0xafdd9660, 5 | BRF_SND },           // 20
};

STDROMPICKEXT(kof98ae, kof98ae, neogeo)
STD_ROM_FN(kof98ae)

struct BurnDriver BurnDrvkof98ae = {
	"kof98ae", "kof98", "neogeo", NULL, "2007",
	"The King of Fighters '98 (Anniversary Edition, EGHT hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98aeRomInfo, kof98aeRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '98 (Anniversary Edition build 1.2.0827, EGCG 2016)
// [2016.08.27] version
// Update team : GSC2007 [program], Dream [graphics], Bing Yan Dream [Color]

static struct BurnRomInfo kof98ae2016RomDesc[] = {
//	{ "242ae-p1.bin", 0x100000, 0xc7fddbd8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242ae-p1.p1",  0x100000, 0xd69ecaa9, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
//	{ "242ae-p2.bin", 0x600000, 0xd84dbe5b, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "242ae-p2.sp2", 0x600000, 0xac1e9e79, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "242ae-s1.s1",  0x040000, 0x345c20b0, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242ae-c1.c1",  0x800000, 0x086ec060, 3 | BRF_GRA },           //  3 Sprite data
	{ "242ae-c2.c2",  0x800000, 0x16b9a9a2, 3 | BRF_GRA },           //  4
	{ "242-c3.c3",    0x800000, 0x22127b4f, 3 | BRF_GRA },           //  5
	{ "242-c4.c4",    0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  6
	{ "242ae-c5.c5",  0x800000, 0x71641718, 3 | BRF_GRA },           //  7
	{ "242ae-c6.c6",  0x800000, 0x982ba2b3, 3 | BRF_GRA },           //  8
	{ "242ae-c7.c7",  0x800000, 0x8d495552, 3 | BRF_GRA },           //  9
	{ "242ae-c8.c8",  0x800000, 0x8bfc3417, 3 | BRF_GRA },           // 10
	{ "242ae-c9.c9",  0x800000, 0x128256d1, 3 | BRF_GRA },           // 11
	{ "242ae-c10.c10",0x800000, 0x4bd8412d, 3 | BRF_GRA },           // 12
	{ "242ae-c11.c11",0x800000, 0x56d361cb, 3 | BRF_GRA },           // 13
	{ "242ae-c12.c12",0x800000, 0xf8cb115b, 3 | BRF_GRA },           // 14

	{ "242ae-m1.m1",  0x040000, 0x9ade0528, 4 | BRF_ESS | BRF_PRG }, // 15 Z80 code

	{ "242-v1.v1",    0x400000, 0xb9ea8051, 5 | BRF_SND },           // 16 Sound data
	{ "242-v2.v2",    0x400000, 0xcc11106e, 5 | BRF_SND },           // 17
	{ "242-v3.v3",    0x400000, 0x044ea4e1, 5 | BRF_SND },           // 18
	{ "242-v4.v4",    0x400000, 0x7985ea30, 5 | BRF_SND },           // 19
	{ "242ae-v5.v5",  0x400000, 0xafdd9660, 5 | BRF_SND },           // 20
};

STDROMPICKEXT(kof98ae2016, kof98ae2016, neogeo)
STD_ROM_FN(kof98ae2016)

struct BurnDriver BurnDrvkof98ae2016 = {
	"kof98ae2016", "kof98", "neogeo", NULL, "2016",
	"The King of Fighters '98 (Anniversary Edition build 1.2.0827)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98ae2016RomInfo, kof98ae2016RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '98 (Ultimate Match)
// This is a hack based on KOF98AE/KOF98AE2016, not a real PGM2 title.
// Hacked by Elrayzeur.

static struct BurnRomInfo kof98ultRomDesc[] = {
	{ "242ult.p1",    0x100000, 0x9c450313, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242ult.p2",    0x600000, 0x6d109dc7, 1 | BRF_ESS | BRF_PRG }, //  1 
	
	{ "242ult.s1",    0x040000, 0x8fd2c84d, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242ult.c1",    0x800000, 0x8e297a35, 3 | BRF_GRA },           //  3 Sprite data
	{ "242ult.c2",    0x800000, 0xd8a4b216, 3 | BRF_GRA },           //  4 
	{ "242-c3.c3",    0x800000, 0x22127b4f, 3 | BRF_GRA },           //  5 
	{ "242-c4.c4",    0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  6 
	{ "242ae-c5.c5",  0x800000, 0x71641718, 3 | BRF_GRA },           //  7 
	{ "242ae-c6.c6",  0x800000, 0x982ba2b3, 3 | BRF_GRA },           //  8 
	{ "242ae-c7.c7",  0x800000, 0x8d495552, 3 | BRF_GRA },           //  9 
	{ "242ae-c8.c8",  0x800000, 0x8bfc3417, 3 | BRF_GRA },           // 10 
	{ "242ult.c9",    0x800000, 0xde7e8582, 3 | BRF_GRA },           // 11 
	{ "242ult.c10",   0x800000, 0x76c7b038, 3 | BRF_GRA },           // 12 
	{ "242ae-c11.c11",0x800000, 0x56d361cb, 3 | BRF_GRA },           // 13 
	{ "242ae-c12.c12",0x800000, 0xf8cb115b, 3 | BRF_GRA },           // 14 

	{ "242ae-m1.m1",  0x040000, 0x9ade0528, 4 | BRF_ESS | BRF_PRG }, // 15 Z80 code

	{ "242-v1.v1",    0x400000, 0xb9ea8051, 5 | BRF_SND },           // 16 Sound data
	{ "242-v2.v2",    0x400000, 0xcc11106e, 5 | BRF_SND },           // 17 
	{ "242-v3.v3",    0x400000, 0x044ea4e1, 5 | BRF_SND },           // 18 
	{ "242-v4.v4",    0x400000, 0x7985ea30, 5 | BRF_SND },           // 19 
	{ "242ae-v5.v5",  0x400000, 0xafdd9660, 5 | BRF_SND },           // 20 
};

STDROMPICKEXT(kof98ult, kof98ult, neogeo)
STD_ROM_FN(kof98ult)

struct BurnDriver BurnDrvkof98ult = {
	"kof98ult", "kof98", "neogeo", NULL, "2020",
	"The King of Fighters '98 (Ultimate Match)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98ultRomInfo, kof98ultRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '98 (Combo)
/* Ivex hack - 2018/05/05 version */

static struct BurnRomInfo kof98cbRomDesc[] = {
	{ "242cb-p1.p1", 	0x100000, 0x2565e431, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242cb-p2.sp2", 	0x400000, 0xd34a4d38, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "242cb-s1.s1", 	0x020000, 0x7333d8b0, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242cb-c1.c1", 	0x800000, 0x066db0a6, 3 | BRF_GRA },           //  3 Sprite data
	{ "242cb-c2.c2", 	0x800000, 0x99d0b0fa, 3 | BRF_GRA },           //  4
	{ "242cb-c3.c3", 	0x800000, 0xea84bdae, 3 | BRF_GRA },           //  5
	{ "242cb-c4.c4",    0x800000, 0x2c17ac8e, 3 | BRF_GRA },           //  6
	{ "242-c5.c5", 		0x800000, 0x9d10bed3, 3 | BRF_GRA },           //  7
	{ "242-c6.c6", 		0x800000, 0xda07b6a2, 3 | BRF_GRA },           //  8
	{ "242-c7.c7", 		0x800000, 0xf6d7a38a, 3 | BRF_GRA },           //  9
	{ "242-c8.c8", 		0x800000, 0xc823e045, 3 | BRF_GRA },           // 10

	{ "242cb-m1.m1", 	0x040000, 0xdb046fc4, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "242-v1.v1",   	0x400000, 0xb9ea8051, 5 | BRF_SND },           // 16 Sound data
	{ "242-v2.v2",   	0x400000, 0xcc11106e, 5 | BRF_SND },           // 17
	{ "242-v3.v3",   	0x400000, 0x044ea4e1, 5 | BRF_SND },           // 18
	{ "242-v4.v4",   	0x400000, 0x7985ea30, 5 | BRF_SND },           // 19
};

STDROMPICKEXT(kof98cb, kof98cb, neogeo)
STD_ROM_FN(kof98cb)

struct BurnDriver BurnDrvkof98cb = {
	"kof98cb", "kof98", "neogeo", NULL, "2018",
	"The King of Fighters '98 (Combo, hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98cbRomInfo, kof98cbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '98 BC 2nd Impact Edition (Hack)
// Version 2020-07-29 (Author Unknown)

static struct BurnRomInfo kof98bc2ndRomDesc[] = {
	{ "98bc2nd-p1.p1", 0x100000, 0xbd45dd5f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p2.sp2",    0x400000, 0x980aba4c, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "98bc2nd-s1.s1", 0x020000, 0xb4ed8fb0, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242-c1.c1",     0x800000, 0xe564ecd6, 3 | BRF_GRA },           //  3 Sprite data
	{ "242-c2.c2",     0x800000, 0xbd959b60, 3 | BRF_GRA },           //  4
	{ "242-c3.c3",     0x800000, 0x22127b4f, 3 | BRF_GRA },           //  5
	{ "242-c4.c4",     0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  6
	{ "242-c5.c5", 	   0x800000, 0x9d10bed3, 3 | BRF_GRA },           //  7
	{ "242-c6.c6", 	   0x800000, 0xda07b6a2, 3 | BRF_GRA },           //  8
	{ "242-c7.c7", 	   0x800000, 0xf6d7a38a, 3 | BRF_GRA },           //  9
	{ "242-c8.c8", 	   0x800000, 0xc823e045, 3 | BRF_GRA },           // 10

	{ "242-m1.m1",     0x040000, 0x4ef7016b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "242-v1.v1",     0x400000, 0xb9ea8051, 5 | BRF_SND },           // 16 Sound data
	{ "242-v2.v2",     0x400000, 0xcc11106e, 5 | BRF_SND },           // 17
	{ "242-v3.v3",     0x400000, 0x044ea4e1, 5 | BRF_SND },           // 18
	{ "242-v4.v4",     0x400000, 0x7985ea30, 5 | BRF_SND },           // 19
};

STDROMPICKEXT(kof98bc2nd, kof98bc2nd, neogeo)
STD_ROM_FN(kof98bc2nd)

struct BurnDriver BurnDrvkof98bc2nd = {
	"kof98bc2nd", "kof98", "neogeo", NULL, "2020",
	"The King of Fighters '98 BC 2nd Impact Edition (Hack, Ver.2020-07-29)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98bc2ndRomInfo, kof98bc2ndRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '98 Mix (2015-12-29)
// Uploaded to GOTVG by GSC2007
// Last update to ver. 20151229 from GOTVG (p1/p2/s1/c9/c10)

static struct BurnRomInfo kof98mixRomDesc[] = {
	{ "242mix.p1",	0x100000, 0x8f3da74e, 1 | BRF_PRG | BRF_ESS }, //  0 maincpu
	{ "242mix.p2",	0x400000, 0x73d3505d, 1 | BRF_PRG | BRF_ESS }, //  1

	{ "242mix.s1",  0x020000, 0x9acd571d, 2 | BRF_GRA },		   //  2 Text data

	{ "242-c1.c1",	0x800000, 0xe564ecd6, 3 | BRF_GRA },           //  3 sprites
	{ "242-c2.c2",	0x800000, 0xbd959b60, 3 | BRF_GRA },           //  4
	{ "242-c3.c3",	0x800000, 0x22127b4f, 3 | BRF_GRA },           //  5
	{ "242-c4.c4",	0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  6
	{ "242-c5.c5",	0x800000, 0x9d10bed3, 3 | BRF_GRA },           //  7
	{ "242-c6.c6",	0x800000, 0xda07b6a2, 3 | BRF_GRA },           //  8
	{ "242mix.c7",	0x800000, 0x374eb904, 3 | BRF_GRA },           //  9
	{ "242mix.c8",	0x800000, 0xeddd2789, 3 | BRF_GRA },           // 10
	{ "242mix.c9",	0x800000, 0x1823dfa3, 3 | BRF_GRA },           // 11
	{ "242mix.c10",	0x800000, 0x6a04ab8c, 3 | BRF_GRA },           // 12

	{ "242-mg1.m1", 0x040000, 0x4e7a6b1b, 4 | BRF_ESS | BRF_PRG }, // 13 Z80 code

	{ "242-v1.v1",	0x400000, 0xb9ea8051, 5 | BRF_SND },           // 14 ymsnd
	{ "242-v2.v2",	0x400000, 0xcc11106e, 5 | BRF_SND },           // 15
	{ "242-v3.v3",	0x400000, 0x044ea4e1, 5 | BRF_SND },           // 16
	{ "242-v4.v4",	0x400000, 0x7985ea30, 5 | BRF_SND },           // 17
};

STDROMPICKEXT(kof98mix, kof98mix, neogeo)
STD_ROM_FN(kof98mix)

static INT32 kof98mixInit()
{
	NeoCallbackActive->pInstallHandlers = kof98InstallHandler;
//	NeoCallbackActive->pScan = kof98Scan;

	return NeoInit();
}

struct BurnDriver BurnDrvKof98mix = {
	"kof98mix", "kof98", "neogeo", NULL, "2015",
	"The King of Fighters '98 Mix (2015-12-29)\0", NULL, "hack", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98mixRomInfo, kof98mixRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof98mixInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '98 (Plus Final Edition 2017-07-23)
// px,s1,m1,c1,2,7,8 confirmed

static struct BurnRomInfo kof98pfeRomDesc[] = {
	{ "242-pn1pfe.p1",	0x100000, 0x23876d95, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p2pfe.sp2",	0x400000, 0xdfa686e9, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "242-p3pfe.p3",	0x020000, 0x930ea34e, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "242-s1pfe.s1",	0x020000, 0x7f4dbf23, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "242-c1pfe.c1",	0x800000, 0x379654a5, 3 | BRF_GRA },           //  4 Sprite data
	{ "242-c2pfe.c2",	0x800000, 0x9c71fa3d, 3 | BRF_GRA },           //  5
	{ "242-c3.c3",		0x800000, 0x22127b4f, 3 | BRF_GRA },           //  6
	{ "242-c4.c4",		0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  7
	{ "242-c5.c5",		0x800000, 0x9d10bed3, 3 | BRF_GRA },           //  8
	{ "242-c6.c6",		0x800000, 0xda07b6a2, 3 | BRF_GRA },           //  9
	{ "242-c7pfe.c7",	0x800000, 0x02f09b2e, 3 | BRF_GRA },           // 10
	{ "242-c8pfe.c8",	0x800000, 0xd43ab3e6, 3 | BRF_GRA },           // 11

	{ "242-mg1.m1",		0x040000, 0x4e7a6b1b, 4 | BRF_ESS | BRF_PRG }, // 12 Z80 code

	{ "242-v1.v1",		0x400000, 0xb9ea8051, 5 | BRF_SND },           // 13 Sound data
	{ "242-v2.v2",		0x400000, 0xcc11106e, 5 | BRF_SND },           // 14
	{ "242-v3.v3",		0x400000, 0x044ea4e1, 5 | BRF_SND },           // 15
	{ "242-v4.v4",		0x400000, 0x7985ea30, 5 | BRF_SND },           // 16
};

STDROMPICKEXT(kof98pfe, kof98pfe, neogeo)
STD_ROM_FN(kof98pfe)

static void kof98pfePatchCallback()
{
	UINT16* rom = (UINT16*)Neo68KROMActive;

	for (INT32 i = 0; i < 0x100000 >> 1; i++) {
		if (rom[i] == 0x4e7d) rom[i] = 0x4e71;
		if (rom[i] == 0x4e7c) rom[i] = 0x4e75;
	}

	rom = (UINT16*)(Neo68KROMActive + 0x500000);

	for (INT32 i = 0; i < 0x20000 >> 1; i++) {
		if (rom[i] == 0x4e7d) rom[i] = 0x4e71;
		if (rom[i] == 0x4e7c) rom[i] = 0x4e75;
	}

	rom[0x1af4 >> 1] = 0x4e71;

	(Neo68KROMActive + 0x500000)[0x1b19] = 0x60;
	(Neo68KROMActive + 0x500000)[0x1ca3] = 0x60;
}

static INT32 kof98pfeInit()
{
	NeoCallbackActive->pInitialise = kof98pfePatchCallback;

	INT32 nRet = NeoInit();
	if (0 == nRet) NeoMapExtraRom(0x500000, 0x20000);

	return nRet;
}

struct BurnDriver BurnDrvkof98pfe = {
	"kof98pfe", "kof98", "neogeo", NULL, "2019",
	"The King of Fighters '98 (Plus Final Edition 2017-07-23)\0", NULL, "hack", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98pfeRomInfo, kof98pfeRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof98pfeInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	320, 224, 4, 3
};

// The King of Fighters '98 - Easy Combo King (GOTVG Version, hack)
/* Hacked by gunloc941 - Build 2019-02-17 */

static struct BurnRomInfo kof98eckgRomDesc[] = {
	/* P1 Encrypted */
	{ "242-p1eckg.p1",	0x200000, 0x9746a706, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p2eckg.sp2",	0x400000, 0x5d59e3ae, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "242-s1eckg.s1",	0x020000, 0xf91c29d2, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242-c1eckg.c1",	0x800000, 0xd3da4dc1, 3 | BRF_GRA },           //  3 Sprite data
	{ "242-c2eckg.c2",	0x800000, 0xf5a5d149, 3 | BRF_GRA },           //  4
	{ "242-c3eckg.c3",	0x800000, 0x0689e5b1, 3 | BRF_GRA },           //  5
	{ "242-c4eckg.c4",	0x800000, 0x5cc4d647, 3 | BRF_GRA },           //  6
	{ "242-c5eckg.c5",	0x800000, 0x1531b766, 3 | BRF_GRA },           //  7
	{ "242-c6eckg.c6",	0x800000, 0x04131307, 3 | BRF_GRA },           //  8
	{ "242-c7eckg.c7",	0x800000, 0x6f879c38, 3 | BRF_GRA },           //  9
	{ "242-c8eckg.c8",	0x800000, 0x89c797df, 3 | BRF_GRA },           // 10

	{ "242-m1.m1",		0x040000, 0x4e7a6b1b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "242-v1eckg.v1",	0x400000, 0x32da3c63, 5 | BRF_SND },           // 16 Sound data
	{ "242-v2eckg.v2",	0x400000, 0x246edf6d, 5 | BRF_SND },           // 17
	{ "242-v3eckg.v3",	0x400000, 0x308b0393, 5 | BRF_SND },           // 18
	{ "242-v4eckg.v4",	0x400000, 0x00276d91, 5 | BRF_SND },           // 19
};

STDROMPICKEXT(kof98eckg, kof98eckg, neogeo)
STD_ROM_FN(kof98eckg)

struct BurnDriver BurnDrvkof98eckg = {
	"kof98eckg", "kof98", "neogeo", NULL, "2019",
	"The King of Fighters '98 Easy Combo King (GOTVG Version, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98eckgRomInfo, kof98eckgRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof98Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '98 - Easy Combo King (YZKOF Version, hack)
/* Hacked by gunloc941 - YZKOF Build 2020-04-09 */

static struct BurnRomInfo kof98eckyRomDesc[] = {
	{ "242-pn1ecky.p1",	0x100000, 0x0ced4a93, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p2ecky.sp2",	0x400000, 0xef36876a, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "242-s1ecky.s1", 	0x020000, 0x5a498ed2, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242-c1ecky.c1", 	0x800000, 0xd528dab9, 3 | BRF_GRA },           //  3 Sprite data
	{ "242-c2ecky.c2", 	0x800000, 0x73556130, 3 | BRF_GRA },           //  4
	{ "242-c3ecky.c3", 	0x800000, 0x4091bca0, 3 | BRF_GRA },           //  5
	{ "242-c4ecky.c4",	0x800000, 0x56918bab, 3 | BRF_GRA },           //  6
	{ "242-c5ecky.c5",	0x800000, 0x76252bdc, 3 | BRF_GRA },           //  7
	{ "242-c6ecky.c6",	0x800000, 0x13d5b9b7, 3 | BRF_GRA },           //  8
	{ "242-c7ecky.c7",	0x800000, 0x1b3a6aa9, 3 | BRF_GRA },           //  9
	{ "242-c8ecky.c8",	0x800000, 0xef3390ca, 3 | BRF_GRA },           // 10

	{ "242-mg1.m1",		0x040000, 0x4e7a6b1b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "242-v1ecky.v1",	0x400000, 0xb5650f76, 5 | BRF_SND },           // 16 Sound data
	{ "242-v2ecky.v2",	0x400000, 0xb43b83ee, 5 | BRF_SND },           // 17
	{ "242-v3ecky.v3",	0x400000, 0x59c53922, 5 | BRF_SND },           // 18
	{ "242-v4ecky.v4",	0x400000, 0x46ca6f5e, 5 | BRF_SND },           // 19
};

STDROMPICKEXT(kof98ecky, kof98ecky, neogeo)
STD_ROM_FN(kof98ecky)

struct BurnDriver BurnDrvkof98ecky = {
	"kof98ecky", "kof98", "neogeo", NULL, "2020",
	"The King of Fighters '98 Easy Combo King (YZKOF Version, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98eckyRomInfo, kof98eckyRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '98 (Combo Plus, GSC2007 & DREAM)
// GOTVG
// Prototype - Ivex kof98cb

static struct BurnRomInfo kof98cpRomDesc[] = {
	{ "242-pn1cp.p1",	0x100000, 0x003d1128, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p2cp.sp2",	0x400000, 0xd4ecdf60, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "242-p3cp.p3",	0x400000, 0x4846dde0, 1 | BRF_ESS | BRF_PRG }, //  2
	{ "242-p4cp.p4",	0x040000, 0xc076b84e, 1 | BRF_ESS | BRF_PRG }, //  3 Extra ROM

	{ "242-s1cp.s1",	0x020000, 0xb4ed8fb0, 2 | BRF_GRA },           //  4 Text layer tiles

	{ "242-c1cp.c1",	0x800000, 0x9cb35021, 3 | BRF_GRA },           //  5 Sprite data
	{ "242-c2cp.c2",	0x800000, 0x432a83ac, 3 | BRF_GRA },           //  6
	{ "242-c3cp.c3",	0x800000, 0xea84bdae, 3 | BRF_GRA },           //  7
	{ "242-c4cp.c4",	0x800000, 0x2c17ac8e, 3 | BRF_GRA },           //  8
	{ "242-c5cp.c5",	0x800000, 0x71641718, 3 | BRF_GRA },           //  9
	{ "242-c6cp.c6",	0x800000, 0x982ba2b3, 3 | BRF_GRA },           //  10
	{ "242-c7cp.c7",	0x800000, 0x8d495552, 3 | BRF_GRA },           //  11
	{ "242-c8cp.c8",	0x800000, 0x8bfc3417, 3 | BRF_GRA },           //  12
	{ "242-c9cp.c9",	0x800000, 0x33a453ca, 3 | BRF_GRA },           //  13
	{ "242-c10cp.c10",	0x800000, 0x0c85069f, 3 | BRF_GRA },           //  14
	{ "242-c11cp.c11",	0x800000, 0x56d361cb, 3 | BRF_GRA },           //  15
	{ "242-c12cp.c12",	0x800000, 0xf8cb115b, 3 | BRF_GRA },           //  16

	{ "242-m1cp.m1",	0x040000, 0x564b53f8, 4 | BRF_ESS | BRF_PRG }, //  17 Z80 code

	{ "242-v1.v1",		0x400000, 0xb9ea8051, 5 | BRF_SND },           //  18 Sound data
	{ "242-v2.v2",		0x400000, 0xcc11106e, 5 | BRF_SND },           //  19
	{ "242-v3.v3",		0x400000, 0x044ea4e1, 5 | BRF_SND },           //  20
	{ "242-v4.v4",		0x400000, 0x7985ea30, 5 | BRF_SND },           //  21
	{ "242-v5cp.v5",	0x400000, 0xafdd9660, 5 | BRF_SND },           //  22
};

STDROMPICKEXT(kof98cp, kof98cp, neogeo)
STD_ROM_FN(kof98cp)

static INT32 kof98cpInit()
{
	INT32 nRet = NeoInit();
	if (0 == nRet) NeoMapExtraRom(0x900000, 0x40000);

	return nRet;
}

struct BurnDriver BurnDrvkof98cp = {
	"kof98cp", "kof98", "neogeo", NULL, "2020",
	"The King of Fighters '98 (Combo Plus)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98cpRomInfo, kof98cpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof98cpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters '98 (Ratio hack)

static struct BurnRomInfo kof98ratioRomDesc[] = {
	{ "242-ratio.p1", 0x100000, 0x9171669d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "242-p2.sp2",   0x400000, 0x980aba4c, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "242-s1.s1",    0x020000, 0x7f7b4805, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "242-c1.c1",    0x800000, 0xe564ecd6, 3 | BRF_GRA },           //  3 Sprite data
	{ "242-c2.c2",    0x800000, 0xbd959b60, 3 | BRF_GRA },           //  4
	{ "242-c3.c3",    0x800000, 0x22127b4f, 3 | BRF_GRA },           //  5
	{ "242-c4.c4",    0x800000, 0x0b4fa044, 3 | BRF_GRA },           //  6
	{ "242-c5.c5",    0x800000, 0x9d10bed3, 3 | BRF_GRA },           //  7
	{ "242-c6.c6",    0x800000, 0xda07b6a2, 3 | BRF_GRA },           //  8
	{ "242-c7.c7",    0x800000, 0xf6d7a38a, 3 | BRF_GRA },           //  9
	{ "242-c8.c8",    0x800000, 0xc823e045, 3 | BRF_GRA },           // 10

	{ "242-mg1.m1",   0x040000, 0x4e7a6b1b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "242-v1.v1",    0x400000, 0xb9ea8051, 5 | BRF_SND },           // 12 Sound data
	{ "242-v2.v2",    0x400000, 0xcc11106e, 5 | BRF_SND },           // 13
	{ "242-v3.v3",    0x400000, 0x044ea4e1, 5 | BRF_SND },           // 14
	{ "242-v4.v4",    0x400000, 0x7985ea30, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof98ratio, kof98ratio, neogeo)
STD_ROM_FN(kof98ratio)

struct BurnDriver BurnDrvKof98ratio = {
	"kof98ratio", "kof98", "neogeo", NULL, "2021",
	"The King of Fighters '98 (Ratio hack)\0", NULL, "bankbank", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof98ratioRomInfo, kof98ratioRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 Anniversary Edition
// hack by Yashional
// Last update to ver. 2020/04/07

static struct BurnRomInfo kof99aeRomDesc[] = {
	{ "kof99ae_p1.bin",   	   0x100000, 0x4c63e81c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "kof99ae_p2.bin",        0x400000, 0xf3f47f41, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "kof99ae_p3.bin",        0x400000, 0xbc85cf02, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "kof99ae_s1.bin",        0x020000, 0x3c31ee43, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "kof99ae_c1.bin",   	   0x800000, 0x7eabea6c, 3 | BRF_GRA },           //  4 Sprite data
	{ "kof99ae_c2.bin",   	   0x800000, 0xe5a5bc5c, 3 | BRF_GRA },           //  5
	{ "kof99ae_c3.bin",   	   0x800000, 0xb047c9d5, 3 | BRF_GRA },           //  6
	{ "kof99ae_c4.bin",   	   0x800000, 0x6bc8e4b1, 3 | BRF_GRA },           //  7
	{ "kof99ae_c5.bin",   	   0x800000, 0x9746268c, 3 | BRF_GRA },           //  8
	{ "kof99ae_c6.bin",   	   0x800000, 0x238b3e71, 3 | BRF_GRA },           //  9
	{ "kof99ae_c7.bin",   	   0x800000, 0x963c28ab, 3 | BRF_GRA },           // 10
	{ "kof99ae_c8.bin",   	   0x800000, 0x43f1e92e, 3 | BRF_GRA },           // 11
	{ "kof99ae_c9.bin",   	   0x800000, 0x86a3550d, 3 | BRF_GRA },           // 12
	{ "kof99ae_c10.bin",       0x800000, 0x986bb897, 3 | BRF_GRA },           // 13
	{ "kof99ae_c11.bin",       0x800000, 0x7fe785c2, 3 | BRF_GRA },           // 14
	{ "kof99ae_c12.bin",       0x800000, 0xa7541483, 3 | BRF_GRA },           // 15

	{ "kof99ae_m1.bin",        0x020000, 0xf847e188, 4 | BRF_ESS | BRF_PRG }, // 16 Z80 code

	{ "kof99ae_v1.bin",        0x400000, 0xceaa3bae, 5 | BRF_SND },           // 17 Sound data
	{ "kof99ae_v2.bin",        0x400000, 0x07d70650, 5 | BRF_SND },           // 18
	{ "kof99ae_v3.bin",        0x400000, 0x821901da, 5 | BRF_SND },           // 19
	{ "kof99ae_v4.bin",        0x200000, 0xb49e6178, 5 | BRF_SND },           // 20
};

STDROMPICKEXT(kof99ae, kof99ae, neogeo)
STD_ROM_FN(kof99ae)

struct BurnDriver BurnDrvkof99ae = {
	"kof99ae", "kof99", "neogeo", NULL, "2020",
	"The King of Fighters '99 Anniversary Edition (hack)\0", NULL, "Yashional", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99aeRomInfo, kof99aeRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 - Remix Pro V2.01 Final (Hack By FCHT - FightChina Hackteam)
// Hack By FCHT - FightChina Hackteam - 2006
// Bug fixed by Ice Flame Fantasy - 2022/02/15 (http://www.nesbbs.com/bbs/thread-53283-1-1.html)

static struct BurnRomInfo kof99rpRomDesc[] = {
	{ "rp.neo-sma",		0x040000, 0x173e7ac4, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "251-p1rp.p1",	0x400000, 0xeffd218b, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "251-p2rp.p2",	0x400000, 0x6b8f8478, 1 | BRF_ESS | BRF_PRG }, //  2

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "251-c1rp.c1",	0x800000, 0x39fbe868, 3 | BRF_GRA },           //  3 Sprite data
	{ "251-c2rp.c2",	0x800000, 0xef461a92, 3 | BRF_GRA },           //  4
	{ "251-c3rp.c3",	0x800000, 0xc79dfe9d, 3 | BRF_GRA },           //  5
	{ "251-c4rp.c4",	0x800000, 0x24907843, 3 | BRF_GRA },           //  6
	{ "251-c5rp.c5",	0x800000, 0x3192738e, 3 | BRF_GRA },           //  7
	{ "251-c6rp.c6",	0x800000, 0x0c9f4912, 3 | BRF_GRA },           //  8
	{ "251-c7rp.c7",	0x800000, 0x54bc5d03, 3 | BRF_GRA },           //  9
	{ "251-c8rp.c8",	0x800000, 0x4943a1c3, 3 | BRF_GRA },           // 10

	{ "251-m1rp.m1",	0x020000, 0x0cb24d46, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "251-v1.v1",		0x400000, 0xef2eecc8, 5 | BRF_SND },           // 12 Sound data
	{ "251-v2.v2",		0x400000, 0x73e211ca, 5 | BRF_SND },           // 13
	{ "251-v3rp.v3",	0x400000, 0x403628da, 5 | BRF_SND },           // 14
	{ "251-v4.v4",		0x200000, 0xb49e6178, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof99rp, kof99rp, neogeo)
STD_ROM_FN(kof99rp)

static INT32 kof99rpInit()
{
	nNeoProtectionXor = 0x00;
	NeoCallbackActive->pInitialise = kof99SMADecrypt;

	return NeoInit();
}

struct BurnDriver BurnDrvkof99rp = {
	"kof99rp", "kof99", "neogeo", NULL, "2022",
	"The King of Fighters '99 Remix Pro (v2.01 Final)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99rpRomInfo, kof99rpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof99rpInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 - Evolution Ultra Remix (Hack By Yashional)

static struct BurnRomInfo kof99eurRomDesc[] = {
	{ "kf99eur.p1",    0x100000, 0x66fa20fd, 1 | BRF_ESS | BRF_PRG }, //  0
	{ "kf99eur.p2",    0x400000, 0xaec93904, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "kf99eur.s1",    0x020000, 0xa6747969, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "kf99eur.c1",    0x800000, 0xc7e8bf32, 3 | BRF_GRA },           //  3 Sprite data
	{ "kf99eur.c2",    0x800000, 0x4e8f9f7d, 3 | BRF_GRA },           //  4
	{ "kf99eur.c3",    0x800000, 0xb047c9d5, 3 | BRF_GRA },           //  5
	{ "kf99eur.c4",    0x800000, 0x6bc8e4b1, 3 | BRF_GRA },           //  6
	{ "kf99eur.c5",    0x800000, 0x9746268c, 3 | BRF_GRA },           //  7
	{ "kf99eur.c6",    0x800000, 0x238b3e71, 3 | BRF_GRA },           //  8
	{ "kf99eur.c7",    0x800000, 0xf0b5ff02, 3 | BRF_GRA },           //  9
	{ "kf99eur.c8",    0x800000, 0x7107b8d5, 3 | BRF_GRA },           //  10

	{ "kf99eur.m1",    0x020000, 0xf847e188, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	{ "kf99eur.v1",    0x400000, 0x4ae46226, 5 | BRF_SND },           //  12 Sound data
	{ "kf99eur.v2",    0x400000, 0x07d70650, 5 | BRF_SND },           //  13
	{ "251-v3.v3",     0x400000, 0x821901da, 5 | BRF_SND },           //  14
	{ "251-v4.v4",     0x200000, 0xb49e6178, 5 | BRF_SND },           //  15
};

STDROMPICKEXT(kof99eur, kof99eur, neogeo)
STD_ROM_FN(kof99eur)

static INT32 Kof99TimePatchInit() // We need this patch to get timer working properly!
{
 	INT32 nRet = NeoInit();
	UINT8* ROM = (UINT8*)Neo68KROMActive;
	ROM[0x2ABE9] = 0x65;
	return nRet;
}

struct BurnDriver BurnDrvkof99eur = {
	"kof99eur", "kof99", "neogeo", NULL, "201?",
	"The King of Fighters '99 Evolution Ultra Remix (Hack By Yashional)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99eurRomInfo, kof99eurRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	Kof99TimePatchInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 - Evolution Ultra Remix 2015
// Original By Yashional (Version 2015-07-13)
// In HBMAME File is kof99s36

static struct BurnRomInfo kof99eur2RomDesc[] = {
	{ "251h36.p1",    0x100000, 0xbdb0aad2, 1 | BRF_ESS | BRF_PRG }, //  0
	{ "251h36.p2",    0x400000, 0x115a796a, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "251h36.s1",    0x020000, 0xb39bf6ed, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "251eur.c1",    0x800000, 0xc7e8bf32, 3 | BRF_GRA },           //  3 Sprite data
	{ "251eur.c2",    0x800000, 0x4e8f9f7d, 3 | BRF_GRA },           //  4
	{ "proto_251.c3", 0x800000, 0xf20959e8, 3 | BRF_GRA },           //  5
	{ "proto_251.c4", 0x800000, 0x54ffbe9f, 3 | BRF_GRA },           //  6
	{ "proto_251.c5", 0x800000, 0xd87a3bbc, 3 | BRF_GRA },           //  7
	{ "proto_251.c6", 0x800000, 0x4d40a691, 3 | BRF_GRA },           //  8
	{ "251eur.c7",    0x800000, 0xf0b5ff02, 3 | BRF_GRA },           //  9
	{ "251eur.c8",    0x800000, 0x7107b8d5, 3 | BRF_GRA },           //  10

	{ "251ae.m1",     0x020000, 0xf847e188, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	{ "251ae.v1",     0x400000, 0x4ae46226, 5 | BRF_SND },           //  12 Sound data
	{ "251ae.v2",     0x400000, 0x07d70650, 5 | BRF_SND },           //  13
	{ "251-v3.v3",    0x400000, 0x821901da, 5 | BRF_SND },           //  14
	{ "251-v4.v4",    0x200000, 0xb49e6178, 5 | BRF_SND },           //  15
};

STDROMPICKEXT(kof99eur2, kof99eur2, neogeo)
STD_ROM_FN(kof99eur2)

struct BurnDriver BurnDrvkof99eur2 = {
	"kof99eur2", "kof99", "neogeo", NULL, "2015",
	"The King of Fighters '99 Evolution Ultra Remix 2015 (Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99eur2RomInfo, kof99eur2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	Kof99TimePatchInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters '99 - Summer Revolution (Hack By FCHT - FightChina Hackteam)

static struct BurnRomInfo kof99srRomDesc[] = {
	{ "kf99sr-p1.bin",    0x100000, 0xb96da0bf, 1 | BRF_ESS | BRF_PRG }, //  0
	{ "kf99sr-p2.bin",    0x400000, 0x9f1d1660, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "kf99sr-s1.bin",    0x020000, 0x08055c7a, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "kf99sr-c1.bin",    0x800000, 0xb3d88546, 3 | BRF_GRA },           //  3 Sprite data
	{ "kf99sr-c2.bin",    0x800000, 0x915c8634, 3 | BRF_GRA },           //  4
	{ "kf99sr-c3.bin",    0x800000, 0xb047c9d5, 3 | BRF_GRA },           //  5
	{ "kf99sr-c4.bin",    0x800000, 0x6bc8e4b1, 3 | BRF_GRA },           //  6
	{ "kf99sr-c5.bin",    0x800000, 0x9746268c, 3 | BRF_GRA },           //  7
	{ "kf99sr-c6.bin",    0x800000, 0x238b3e71, 3 | BRF_GRA },           //  8
	{ "kf99sr-c7.bin",    0x800000, 0x65de862c, 3 | BRF_GRA },           //  9
	{ "kf99sr-c8.bin",    0x800000, 0x3ff457fe, 3 | BRF_GRA },           //  10

	{ "kf99sr-m1.bin",    0x020000, 0x5e74539c, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	{ "kf99sr-v1.bin",    0x400000, 0xef2eecc8, 5 | BRF_SND },           //  12 Sound data
	{ "kf99sr-v2.bin",    0x400000, 0x73e211ca, 5 | BRF_SND },           //  13
	{ "kf99sr-v3.bin",    0x400000, 0x821901da, 5 | BRF_SND },           //  14
	{ "kf99sr-v4.bin",    0x200000, 0xb49e6178, 5 | BRF_SND },           //  15
};

STDROMPICKEXT(kof99sr, kof99sr, neogeo)
STD_ROM_FN(kof99sr)

struct BurnDriver BurnDrvkof99sr = {
	"kof99sr", "kof99", "neogeo", NULL, "200?",
	"The King of Fighters '99 Summer Revolution (Hack By FCHT)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof99srRomInfo, kof99srRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	Kof99TimePatchInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters 2000 (Playstation 2 ver. , EGHT hack)

static struct BurnRomInfo kof2000ps2RomDesc[] = {
	{ "257-pg1ps2.p1",		0x100000, 0x56941018, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "257-pg2ps2.sp2",		0x400000, 0x1669a5ad, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "257-c1d.c1",			0x800000, 0xabcdd424, 3 | BRF_GRA },		   //  2 Sprite data
	{ "257-c2d.c2",			0x800000, 0xcda33778, 3 | BRF_GRA },		   //  3
	{ "257-c3d.c3",			0x800000, 0x087fb15b, 3 | BRF_GRA },		   //  4
	{ "257-c4d.c4",			0x800000, 0xfe9dfde4, 3 | BRF_GRA },		   //  5
	{ "257-c5d.c5",			0x800000, 0x03ee4bf4, 3 | BRF_GRA },		   //  6
	{ "257-c6d.c6",			0x800000, 0x8599cc5b, 3 | BRF_GRA },		   //  7
	{ "257-c7ps2.c7",		0x800000, 0x93c343ec, 3 | BRF_GRA },		   //  8
	{ "257-c8ps2.c8",		0x800000, 0xba92f698, 3 | BRF_GRA },		   //  9

	{ "257-m1.m1",			0x040000, 0x4b749113, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "257-v1.v1",			0x400000, 0x17cde847, 5 | BRF_SND },		   // 11 Sound data
	{ "257-v2.v2",			0x400000, 0x1afb20ff, 5 | BRF_SND },		   // 12
	{ "257-v3.v3",			0x400000, 0x4605036a, 5 | BRF_SND },		   // 13
	{ "257-v4.v4",			0x400000, 0x764bbd6b, 5 | BRF_SND },		   // 14
};

STDROMPICKEXT(kof2000ps2, kof2000ps2, neogeo)
STD_ROM_FN(kof2000ps2)

struct BurnDriver BurnDrvkof2000ps2 = {
	"kof2000ps2", "kof2000", "neogeo", NULL, "2000",
	"The King of Fighters 2000 (Playstation 2 ver. , EGHT hack)\0", "hack only enable in AES mode", "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2000ps2RomInfo, kof2000ps2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2000 (OTC, hack)
// YZKOF Last Update: 2020/06/15

static struct BurnRomInfo kof2000otcRomDesc[] = {
	{ "257-pg1otc.p1",		0x100000, 0x11dbf4dc, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "257-pg2otc.sp2",		0x400000, 0xe1095f50, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "257-c1d.c1",			0x800000, 0xabcdd424, 3 | BRF_GRA }, 		   //  2 Sprite data
	{ "257-c2d.c2",			0x800000, 0xcda33778, 3 | BRF_GRA }, 		   //  3
	{ "257-c3d.c3",			0x800000, 0x087fb15b, 3 | BRF_GRA }, 		   //  4
	{ "257-c4d.c4",			0x800000, 0xfe9dfde4, 3 | BRF_GRA }, 		   //  5
	{ "257-c5d.c5",			0x800000, 0x03ee4bf4, 3 | BRF_GRA }, 		   //  6
	{ "257-c6d.c6",			0x800000, 0x8599cc5b, 3 | BRF_GRA }, 		   //  7
	{ "257-c7otc.c7",		0x800000, 0xb2cd7102, 3 | BRF_GRA }, 		   //  8
	{ "257-c8otc.c8",		0x800000, 0xd11c3b66, 3 | BRF_GRA }, 		   //  9

	{ "257-m1otc.m1",		0x040000, 0x80ebc3aa, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "257-v1.v1",			0x400000, 0x17cde847, 5 | BRF_SND },           // 11 Sound data
	{ "257-v2.v2",			0x400000, 0x1afb20ff, 5 | BRF_SND },           // 12
	{ "257-v3.v3",			0x400000, 0x4605036a, 5 | BRF_SND },           // 13
	{ "257-v4.v4",			0x400000, 0x764bbd6b, 5 | BRF_SND },           // 14
	{ "257-v5otc.v5",		0x400000, 0x54df285f, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof2000otc, kof2000otc, neogeo)
STD_ROM_FN(kof2000otc)

struct BurnDriver BurnDrvkof2000otc = {
	"kof2000otc", "kof2000", "neogeo", NULL, "2020",
	"The King of Fighters 2000 (OTC, hack)\0", "ZERO only enabled in AES mode", "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_ALTERNATE_TEXT, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2000otcRomInfo, kof2000otcRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2000 - Special Edition (Final Version, Hack)
/* Hacked by GSC2007 & EGCG - 2021/03/04
	NEWS:
	1.调整BOSS性能，开启全人物
	2.随机屏蔽小头像，随机配色
	3.使用ABCD隐藏排序
	4.开启隐藏援助
	5.K'、八神、BOSS使用MAX超杀有血红背景特效
	6.投技计算HIT
	7.练习模式增加HUMAN
	8.增加晕厥STUN提示
	9.修复原版大量BUG */

static struct BurnRomInfo kof2000spRomDesc[] = {
	{ "257-pg1sp.p1",		0x100000, 0xd31861a2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "257-pg2sp.sp2", 		0x400000, 0x48a1a381, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "257-c1sp.c1",		0x800000, 0x11fdb7eb, 3 | BRF_GRA }, 		   //  2 Sprite data
	{ "257-c2sp.c2",		0x800000, 0x580bbd0b, 3 | BRF_GRA }, 		   //  3
	{ "257-c3sp.c3",		0x800000, 0xed1414d7, 3 | BRF_GRA }, 		   //  4
	{ "257-c4sp.c4",		0x800000, 0xe448fc89, 3 | BRF_GRA }, 		   //  5
	{ "257-c5sp.c5",		0x800000, 0x7510542e, 3 | BRF_GRA }, 		   //  6
	{ "257-c6sp.c6",		0x800000, 0xcca7f13e, 3 | BRF_GRA }, 		   //  7
	{ "257-c7sp.c7",		0x800000, 0x2c9b3c00, 3 | BRF_GRA }, 		   //  8
	{ "257-c8sp.c8",		0x800000, 0x568b3ba3, 3 | BRF_GRA }, 		   //  9

	/* Encrypted */
	{ "257-m1.m1",			0x040000, 0x4b749113, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "257-v1.v1",			0x400000, 0x17cde847, 5 | BRF_SND },           // 11 Sound data
	{ "257-v2.v2",			0x400000, 0x1afb20ff, 5 | BRF_SND },           // 12
	{ "257-v3.v3",			0x400000, 0x4605036a, 5 | BRF_SND },           // 13
	{ "257-v4.v4",			0x400000, 0x764bbd6b, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(kof2000sp, kof2000sp, neogeo)
STD_ROM_FN(kof2000sp)

struct BurnDriver BurnDrvkof2000sp = {
	"kof2000sp", "kof2000", "neogeo", NULL, "2021",
	"The King of Fighters 2000 - Special Edition (Final Version, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2000spRomInfo, kof2000spRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2000 SP XXX (Hack)

static struct BurnRomInfo kof2kxxxRomDesc[] = {
	{ "257xxx-p1.p1",    0x100000, 0xb88cd5eb, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "257xxx-p2.sp2",   0x400000, 0x1a10f2be, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "257xxx-c1.c1",    0x800000, 0x3006a5e4, 3 | BRF_GRA },           //  2 Sprite data
	{ "257xxx-c2.c2",    0x800000, 0x366fd118, 3 | BRF_GRA },           //  3
	{ "257xxx-c3.c3",    0x800000, 0x5b16d7e3, 3 | BRF_GRA },           //  4
	{ "257xxx-c4.c4",    0x800000, 0x7e57dfc1, 3 | BRF_GRA },           //  5
	{ "257xxx-c5.c5",    0x800000, 0x671904f4, 3 | BRF_GRA },           //  6
	{ "257xxx-c6.c6",    0x800000, 0x68adf255, 3 | BRF_GRA },           //  7
	{ "257xxx-c7.c7",    0x800000, 0x8c6b22b3, 3 | BRF_GRA },           //  8
	{ "257xxx-c8.c8",    0x800000, 0x6223bd1d, 3 | BRF_GRA },           //  9

	{ "257-m1.m1",       0x040000, 0x4b749113, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "257-v1.v1",       0x400000, 0x17cde847, 5 | BRF_SND },           // 11 Sound data
	{ "257-v2.v2",       0x400000, 0x1afb20ff, 5 | BRF_SND },           // 12
	{ "257-v3.v3",       0x400000, 0x4605036a, 5 | BRF_SND },           // 13
	{ "257-v4.v4",       0x400000, 0x764bbd6b, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(kof2kxxx, kof2kxxx, neogeo)
STD_ROM_FN(kof2kxxx)

struct BurnDriver BurnDrvkof2kxxx = {
	"kof2kxxx", "kof2000", "neogeo", NULL, "2016",
	"The King of Fighters 2000 SP XXX (Hack, Ver. 2016-01-04)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2kxxxRomInfo, kof2kxxxRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2000nInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2001 Plus (set 2, bootleg / hack)

static struct BurnRomInfo kf2k1paRomDesc[] = {
	{ "2k1-p1a.bin",      0x100000, 0xf8a71b6f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "262-pg2.bin",      0x400000, 0x91eea062, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "2k1-s1a.bin",      0x020000, 0x50986eeb, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "262-c1-08-e0.bin", 0x800000, 0x99cc785a, 3 | BRF_GRA },     		 //  3 Sprite data
	{ "262-c2-08-e0.bin", 0x800000, 0x50368cbf, 3 | BRF_GRA },     		 //  4
	{ "262-c3-08-e0.bin", 0x800000, 0xfb14ff87, 3 | BRF_GRA },     		 //  5
	{ "262-c4-08-e0.bin", 0x800000, 0x4397faf8, 3 | BRF_GRA },     		 //  6
	{ "262-c5-08-e0.bin", 0x800000, 0x91f24be4, 3 | BRF_GRA },     		 //  7
	{ "262-c6-08-e0.bin", 0x800000, 0xa31e4403, 3 | BRF_GRA },     		 //  8
	{ "262-c7-08-e0.bin", 0x800000, 0x54d9d1ec, 3 | BRF_GRA },     		 //  9
	{ "262-c8-08-e0.bin", 0x800000, 0x59289a6b, 3 | BRF_GRA },     		 // 10

	{ "265-262-m1.bin",   0x040000, 0xa7f8119f, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "262-v1-08-e0.bin", 0x400000, 0x83d49ecf, 5 | BRF_SND },     		 // 12 Sound data
	{ "262-v2-08-e0.bin", 0x400000, 0x003f1843, 5 | BRF_SND },     		 // 13
	{ "262-v3-08-e0.bin", 0x400000, 0x2ae38dbe, 5 | BRF_SND },     		 // 14
	{ "262-v4-08-e0.bin", 0x400000, 0x26ec4dd9, 5 | BRF_SND },     		 // 15
};

STDROMPICKEXT(kf2k1pa, kf2k1pa, neogeo)
STD_ROM_FN(kf2k1pa)

static void kf2k1paCallback()
{
	for (INT32 i = 0; i < 0x20000; i++) {
		NeoTextROM[nNeoActiveSlot][i] = BITSWAP08(NeoTextROM[nNeoActiveSlot][i], 3, 2, 4, 5, 1, 6, 0, 7);
	}
}

static INT32 kf2k1paInit()
{
	nNeoProtectionXor = 0x1e;
	NeoCallbackActive->pInitialise = kf2k1paCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvkf2k1pa = {
	"kf2k1pa", "kof2001", "neogeo", NULL, "2002",
	"The King of Fighters 2001 Plus (set 2, bootleg / hack)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k1paRomInfo, kf2k1paRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf2k1paInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2001 Plus (set 1, bootleg / hack)

static struct BurnRomInfo kf2k1plsRomDesc[] = {
	{ "2k1-p1p.bin",      0x100000, 0x758529a7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "262-pg2.bin",      0x400000, 0x91eea062, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "2k1-s1p.bin",      0x020000, 0x088657e6, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "262-c1-08-e0.bin", 0x800000, 0x99cc785a, 3 | BRF_GRA },     		 //  3 Sprite data
	{ "262-c2-08-e0.bin", 0x800000, 0x50368cbf, 3 | BRF_GRA },     		 //  4
	{ "262-c3-08-e0.bin", 0x800000, 0xfb14ff87, 3 | BRF_GRA },     		 //  5
	{ "262-c4-08-e0.bin", 0x800000, 0x4397faf8, 3 | BRF_GRA },     		 //  6
	{ "262-c5-08-e0.bin", 0x800000, 0x91f24be4, 3 | BRF_GRA },     		 //  7
	{ "262-c6-08-e0.bin", 0x800000, 0xa31e4403, 3 | BRF_GRA },     		 //  8
	{ "262-c7-08-e0.bin", 0x800000, 0x54d9d1ec, 3 | BRF_GRA },     		 //  9
	{ "262-c8-08-e0.bin", 0x800000, 0x59289a6b, 3 | BRF_GRA },     		 // 10

	{ "265-262-m1.bin",   0x040000, 0xa7f8119f, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "262-v1-08-e0.bin", 0x400000, 0x83d49ecf, 5 | BRF_SND },     		 // 12 Sound data
	{ "262-v2-08-e0.bin", 0x400000, 0x003f1843, 5 | BRF_SND },     		 // 13
	{ "262-v3-08-e0.bin", 0x400000, 0x2ae38dbe, 5 | BRF_SND },     		 // 14
	{ "262-v4-08-e0.bin", 0x400000, 0x26ec4dd9, 5 | BRF_SND },     		 // 15
};

STDROMPICKEXT(kf2k1pls, kf2k1pls, neogeo)
STD_ROM_FN(kf2k1pls)

static INT32 kf2k1plsInit()
{
	nNeoProtectionXor = 0x1e;

	return NeoInit();
}

struct BurnDriver BurnDrvkf2k1pls = {
	"kf2k1pls", "kof2001", "neogeo", NULL, "2002",
	"The King of Fighters 2001 Plus (set 1, bootleg / hack)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k1plsRomInfo, kf2k1plsRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf2k1plsInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2001 (PS2 Krizalid Edition 2019-12-12, Eddids)
// This hack is known as kof2001s46 in HBMAME

static struct BurnRomInfo kf2k1pkzRomDesc[] = {
	{ "kf2k1ps2-p1.bin",       0x100000, 0xc484272e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "kf2k1ps2-p2.bin",       0x400000, 0x91d4f9f7, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "kf2k1ps2-s1.bin",       0x020000, 0x73efb81d, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "kf2k1_c1.rom",          0x800000, 0x103225b1, 3 | BRF_GRA },     //  3 Sprite data
	{ "kf2k1_c2.rom",          0x800000, 0xf9d05d99, 3 | BRF_GRA },     //  4
	{ "kf2k1_c3.rom",          0x800000, 0x4c7ec427, 3 | BRF_GRA },     //  5
	{ "kf2k1_c4.rom",          0x800000, 0x1d237aa6, 3 | BRF_GRA },     //  6
	{ "kf2k1_c5.rom",          0x800000, 0xc2256db5, 3 | BRF_GRA },     //  7
	{ "kf2k1_c6.rom",          0x800000, 0x8d6565a9, 3 | BRF_GRA },     //  8
	{ "kf2k1ps2-c7.bin",       0x800000, 0xfd3f6dcb, 3 | BRF_GRA },     //  9
	{ "kf2k1ps2-c8.bin",       0x800000, 0x3de3f84f, 3 | BRF_GRA },     //  10
	{ "kf2k1ps2-c9.bin",       0x800000, 0x54e0b19a, 3 | BRF_GRA },     //  11
	{ "kf2k1ps2-c10.bin",      0x800000, 0x35109b7d, 3 | BRF_GRA },     //  12

	{ "262-m1d.bin",           0x040000, 0x4bcc537b, 4 | BRF_ESS | BRF_PRG }, // 13 Z80 code

	{ "262-v1-08-e0.v1",       0x400000, 0x83d49ecf, 5 | BRF_SND },     // 14 Sound data
	{ "262-v2-08-e0.v2",       0x400000, 0x003f1843, 5 | BRF_SND },     // 15
	{ "262-v3-08-e0.v3",       0x400000, 0x2ae38dbe, 5 | BRF_SND },     // 16
	{ "262-v4-08-e0.v4",       0x400000, 0x26ec4dd9, 5 | BRF_SND },     // 17
};

STDROMPICKEXT(kf2k1pkz, kf2k1pkz, neogeo)
STD_ROM_FN(kf2k1pkz)

struct BurnDriver BurnDrvkf2k1pkz = {
	"kf2k1pkz", "kof2001", "neogeo", NULL, "2019-12-12",
	"The King of Fighters 2001 (PS2 Krizalid Edition)\0", "hack only enable in AES mode", "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k1pkzRomInfo, kf2k1pkzRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	kf2k1plsInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters 2001 Remix Ultra 2.3
// Version 2004-05-07 Hack By Jason FGCH

static struct BurnRomInfo kof2001ruRomDesc[] = {
	{ "2k1ru-p1.p1",       0x100000, 0x3377ebee, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "2k1ru-p2.p2",       0x400000, 0x394bcc59, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "2k1ru-s1.s1",       0x020000, 0xa5d90035, 2 | BRF_GRA },     //  2 Text layer tiles

	{ "262-c1d.c1",        0x800000, 0x103225b1, 3 | BRF_GRA },     //  3 Sprite Data
	{ "262-c2d.c2",        0x800000, 0xf9d05d99, 3 | BRF_GRA },     //  4
	{ "262-c3d.c3",        0x800000, 0x4c7ec427, 3 | BRF_GRA },     //  5
	{ "262-c4d.c4",        0x800000, 0x1d237aa6, 3 | BRF_GRA },     //  6
	{ "262-c5d.c5",        0x800000, 0xc2256db5, 3 | BRF_GRA },     //  7
	{ "262-c6d.c6",        0x800000, 0x8d6565a9, 3 | BRF_GRA },     //  8
	{ "262-c7d.c7",        0x800000, 0xd1408776, 3 | BRF_GRA },     //  9
	{ "262-c8d.c8",        0x800000, 0x954d0e16, 3 | BRF_GRA },     //  10

	{ "2k1ru-m1d.m1",      0x020000, 0x2fb0a8a5, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "262-v1-08-e0.v1",   0x400000, 0x83d49ecf, 5 | BRF_SND },           // 12 Sound data
	{ "262-v2-08-e0.v2",   0x400000, 0x003f1843, 5 | BRF_SND },           // 13
	{ "262-v3-08-e0.v3",   0x400000, 0x2ae38dbe, 5 | BRF_SND },           // 14
	{ "262-v4-08-e0.v4",   0x400000, 0x26ec4dd9, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof2001ru, kof2001ru, neogeo)
STD_ROM_FN(kof2001ru)

struct BurnDriver BurnDrvkof2001ru = {
	"kof2001ru", "kof2001", "neogeo", NULL, "Version 2004-05-07",
	"The King of Fighters 2001 Remix Ultra 2.3 (Hack By Jason FGCH)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2001ruRomInfo, kof2001ruRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters 2001 Remix Pro v1.02 Final
// Version 2004-03-01 Hack By Jason,Kim & Raymonose

static struct BurnRomInfo kof2k1rpRomDesc[] = {
	{ "262rp-p1.p1",           0x100000, 0xe9e42e55, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "262rp-sp2.sp2",         0x400000, 0x91668d86, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "262rp-s1.s1",           0x020000, 0x492f5efe, 2 | BRF_GRA },     //  2 Text layer tiles

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "262-c1-08-e0.c1",       0x800000, 0x99cc785a, 3 | BRF_GRA },     //  2 Sprite data
	{ "262-c2-08-e0.c2",       0x800000, 0x50368cbf, 3 | BRF_GRA },     //  3
	{ "262-c3-08-e0.c3",       0x800000, 0xfb14ff87, 3 | BRF_GRA },     //  4
	{ "262-c4-08-e0.c4",       0x800000, 0x4397faf8, 3 | BRF_GRA },     //  5
	{ "262-c5-08-e0.c5",       0x800000, 0x91f24be4, 3 | BRF_GRA },     //  6
	{ "262-c6-08-e0.c6",       0x800000, 0xa31e4403, 3 | BRF_GRA },     //  7
	{ "262-c7-08-e0.c7",       0x800000, 0x54d9d1ec, 3 | BRF_GRA },     //  8
	{ "262-c8-08-e0.c8",       0x800000, 0x59289a6b, 3 | BRF_GRA },     //  9

	{ "262rp-m1d.m1",          0x020000, 0x2fb0a8a5, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "262-v1-08-e0.v1",       0x400000, 0x83d49ecf, 5 | BRF_SND },           // 12 Sound data
	{ "262-v2-08-e0.v2",       0x400000, 0x003f1843, 5 | BRF_SND },           // 13
	{ "262-v3-08-e0.v3",       0x400000, 0x2ae38dbe, 5 | BRF_SND },           // 14
	{ "262-v4-08-e0.v4",       0x400000, 0x26ec4dd9, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof2k1rp, kof2k1rp, neogeo)
STD_ROM_FN(kof2k1rp)

struct BurnDriver BurnDrvkof2k1rp = {
	"kof2k1rp", "kof2001", "neogeo", NULL, "Version 2004-03-01",
	"The King of Fighters 2001 Remix Pro v1.02 Final (Hack By Jason, Kim & Raymonose)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2k1rpRomInfo, kof2k1rpRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2001Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters 2002 Plus (bootleg set 3)

static struct BurnRomInfo kf2k2plbRomDesc[] = {
	{ "2k2-p1p.bin",  0x100000, 0x3ab03781, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265-p2.bin",   0x400000, 0x327266b8, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "2k2-s1pb.bin", 0x020000, 0x2072d5e9, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265-c1.bin",   0x800000, 0x2b65a656, 3 | BRF_GRA },           //  3 Sprite data
	{ "265-c2.bin",   0x800000, 0xadf18983, 3 | BRF_GRA },           //  4
	{ "265-c3.bin",   0x800000, 0x875e9fd7, 3 | BRF_GRA },           //  5
	{ "265-c4.bin",   0x800000, 0x2da13947, 3 | BRF_GRA },           //  6
	{ "265-c5.bin",   0x800000, 0x61bd165d, 3 | BRF_GRA },           //  7
	{ "265-c6.bin",   0x800000, 0x03fdd1eb, 3 | BRF_GRA },           //  8
	{ "265-c7.bin",   0x800000, 0x1a2749d8, 3 | BRF_GRA },           //  9
	{ "265-c8.bin",   0x800000, 0xab0bb549, 3 | BRF_GRA },           // 10

	{ "265-m1.bin",   0x020000, 0x85aaa632, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "265-v1.bin",   0x800000, 0x15e8f3f5, 5 | BRF_SND },           // 12 Sound data
	{ "265-v2.bin",   0x800000, 0xda41d6f9, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k2plb, kf2k2plb, neogeo)
STD_ROM_FN(kf2k2plb)

struct BurnDriver BurnDrvkf2k2plb = {
	"kf2k2plb", "kof2002", "neogeo", NULL, "2002",
	"The King of Fighters 2002 Plus (bootleg set 3)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k2plbRomInfo, kf2k2plbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2002Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 Super (bootleg)

static struct BurnRomInfo kf2k2plcRomDesc[] = {
	{ "2k2-p1pc.bin", 0x100000, 0xebedae17, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265-p2.bin",   0x400000, 0x327266b8, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "2k2-s1pc.bin", 0x020000, 0xfecbb589, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265-c1.bin",   0x800000, 0x2b65a656, 3 | BRF_GRA },           //  3 Sprite data
	{ "265-c2.bin",   0x800000, 0xadf18983, 3 | BRF_GRA },           //  4
	{ "265-c3.bin",   0x800000, 0x875e9fd7, 3 | BRF_GRA },           //  5
	{ "265-c4.bin",   0x800000, 0x2da13947, 3 | BRF_GRA },           //  6
	{ "265-c5.bin",   0x800000, 0x61bd165d, 3 | BRF_GRA },           //  7
	{ "265-c6.bin",   0x800000, 0x03fdd1eb, 3 | BRF_GRA },           //  8
	{ "265-c7.bin",   0x800000, 0x1a2749d8, 3 | BRF_GRA },           //  9
	{ "265-c8.bin",   0x800000, 0xab0bb549, 3 | BRF_GRA },           // 10

	{ "265-m1.bin",   0x020000, 0x85aaa632, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "265-v1.bin",   0x800000, 0x15e8f3f5, 5 | BRF_SND },           // 12 Sound data
	{ "265-v2.bin",   0x800000, 0xda41d6f9, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k2plc, kf2k2plc, neogeo)
STD_ROM_FN(kf2k2plc)

static void kf2k2plcCallback()
{
	PCM2DecryptP();

	garoubl_sx_decode();
}

static INT32 kf2k2plcInit()
{
	INT32 nRet;

	nNeoProtectionXor = 0xEC;
	NeoCallbackActive->pInitialise = kf2k2plcCallback;

	nRet = NeoInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0xa5000, 0x000000, { 0xf9, 0xe0, 0x5d, 0xf3, 0xea, 0x92, 0xbe, 0xef } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkf2k2plc = {
	"kf2k2plc", "kof2002", "neogeo", NULL, "2002",
	"The King of Fighters 2002 Super (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k2plcRomInfo, kf2k2plcRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf2k2plcInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 (PlayStation 2 ver 0.4, EGHT hack)

static struct BurnRomInfo kf2k2ps2RomDesc[] = {
	{ "265ps2-p1.bin",   		0x100000, 0x336c4ca8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265ps2-p2.bin",   		0x500000, 0xcb0032bf, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "265ps2-s1.bin",   		0x020000, 0x714ade47, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265-c1_decrypted.bin", 	0x800000, 0x7efa6ef7, 3 | BRF_GRA },           //  3 Sprite data
	{ "265-c2_decrypted.bin", 	0x800000, 0xaa82948b, 3 | BRF_GRA },           //  4
	{ "265-c3_decrypted.bin", 	0x800000, 0x959fad0b, 3 | BRF_GRA },           //  5
	{ "265-c4_decrypted.bin", 	0x800000, 0xefe6a468, 3 | BRF_GRA },           //  6
	{ "265-c5_decrypted.bin", 	0x800000, 0x74bba7c6, 3 | BRF_GRA },           //  7
	{ "265-c6_decrypted.bin", 	0x800000, 0xe20d2216, 3 | BRF_GRA },           //  8
	{ "265ps2-c7.bin",   		0x800000, 0x1b1d35fb, 3 | BRF_GRA },           //  9
	{ "265ps2-c8.bin",   		0x800000, 0xa5e35d11, 3 | BRF_GRA },           // 10
	{ "265ps2-c9.bin",   		0x800000, 0xaa8bbc97, 3 | BRF_GRA },           // 11
	{ "265ps2-c10.bin",  		0x800000, 0x9832713d, 3 | BRF_GRA },           // 12

	{ "265-m1.bin",      		0x020000, 0x85aaa632, 4 | BRF_ESS | BRF_PRG }, // 13 Z80 code

	{ "265-v1.bin",      		0x800000, 0x15e8f3f5, 5 | BRF_SND },           // 14 Sound data
	{ "265-v2.bin",      		0x800000, 0xda41d6f9, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kf2k2ps2, kf2k2ps2, neogeo)
STD_ROM_FN(kf2k2ps2)

static INT32 kf2k2ps2Init()
{
	INT32 nRet;

	nRet = NeoInit();

	if (nRet == 0) {
		const PCM2DecryptV2Info Info = { 0xa5000, 0x000000, { 0xf9, 0xe0, 0x5d, 0xf3, 0xea, 0x92, 0xbe, 0xef } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkf2k2ps2 = {
	"kf2k2ps2", "kof2002", "neogeo", NULL, "2007",
	"The King of Fighters 2002 (PlayStation 2 ver 0.4, EGHT hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k2ps2RomInfo, kf2k2ps2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf2k2ps2Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 (PlayStation 2, Hack)
// Hacked by Dream

static struct BurnRomInfo kof2002ps2RomDesc[] = {
	/* Encrypted */
	{ "265-p1ps2.p1",		0x100000, 0x9da95b36, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265-p2ps2.sp2",		0x500000, 0x11419517, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "265-c1ps2.c1",		0x800000, 0xa92b140e, 3 | BRF_GRA },           //  2 Sprite data
	{ "265-c2ps2.c2",		0x800000, 0x365e3945, 3 | BRF_GRA },           //  3
	{ "265-c3ps2.c3",		0x800000, 0xe88beb81, 3 | BRF_GRA },           //  4
	{ "265-c4ps2.c4",		0x800000, 0x06c374e8, 3 | BRF_GRA },           //  5
	{ "265-c5ps2.c5",		0x800000, 0x4ffb0fdf, 3 | BRF_GRA },           //  6
	{ "265-c6ps2.c6",		0x800000, 0xa33c38b9, 3 | BRF_GRA },           //  7
	{ "265-c7ps2.c7",		0x800000, 0x186cd736, 3 | BRF_GRA },           //  8
	{ "265-c8ps2.c8",		0x800000, 0xaf60bcf9, 3 | BRF_GRA },           //  9
	{ "265-c9ps2.c9",		0x800000, 0xbbbd99e5, 3 | BRF_GRA },           // 10
	{ "265-c10ps2.c10",		0x800000, 0x5d820e2e, 3 | BRF_GRA },           // 11

	/* Encrypted */
	{ "265-m1ps2.m1",		0x020000, 0x23961378, 4 | BRF_ESS | BRF_PRG }, // 12 Z80 code

	/* Encrypted */
	{ "265-v1.v1",			0x800000, 0x15e8f3f5, 5 | BRF_SND },           // 11 Sound data
	{ "265-v2.v2",			0x800000, 0xda41d6f9, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof2002ps2, kof2002ps2, neogeo)
STD_ROM_FN(kof2002ps2)

struct BurnDriver BurnDrvkof2002ps2 = {
	"kof2002ps2", "kof2002", "neogeo", NULL, "2018",
	"The King of Fighters 2002 (PlayStation 2, Hack)\0", "hack only enabled in AES mode", "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2002ps2RomInfo, kof2002ps2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	kof2002Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 - 3rd Strike of Orochi (Hack by EGCG/EGHT)
// YZKOF - The date of the last file: 2020-06-14 (kf2k23rd-p1.rom)

static struct BurnRomInfo kf2k23rdRomDesc[] = {
	{ "kf2k23rd-p1.rom",      0x100000, 0xde02918d, 1 | BRF_ESS | BRF_PRG }, // 0 68K code
	{ "kf2k23rd-p2.rom",      0x400000, 0x293bbf78, 1 | BRF_ESS | BRF_PRG }, // 1

	{ "kf2k23rd-s1.rom",      0x020000, 0x67e7cbe1, 2 | BRF_GRA },           // 2 Text layer tiles / TC531000

	{ "kf2k23rd-c1.rom",      0x800000, 0x7efa6ef7, 3 | BRF_GRA },           // 3 Sprite data
	{ "kf2k23rd-c2.rom",      0x800000, 0xaa82948b, 3 | BRF_GRA },           // 4
	{ "kf2k23rd-c3.rom",      0x800000, 0x959fad0b, 3 | BRF_GRA },           // 5
	{ "kf2k23rd-c4.rom",      0x800000, 0x2558972f, 3 | BRF_GRA },           // 6
	{ "kf2k23rd-c5.rom",      0x800000, 0x74bba7c6, 3 | BRF_GRA },           // 7
	{ "kf2k23rd-c6.rom",      0x800000, 0xe20d2216, 3 | BRF_GRA },           // 8
	{ "kf2k23rd-c7.rom",      0x800000, 0x201e75e0, 3 | BRF_GRA },           // 9
	{ "kf2k23rd-c8.rom",      0x800000, 0xff0fd53b, 3 | BRF_GRA },           // 10

	{ "kf2k23rd-m1.rom",      0x020000, 0x1c661a4b, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "kf2k23rd-v1.rom",      0x400000, 0x13d98607, 5 | BRF_SND },           // 12 Sound data
	{ "kf2k23rd-v2.rom",      0x400000, 0x9cf74677, 5 | BRF_SND },           // 13
	{ "kf2k23rd-v3.rom",      0x400000, 0x8e9448b5, 5 | BRF_SND },           // 14
	{ "kf2k23rd-v4.rom",      0x400000, 0x067271b5, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kf2k23rd, kf2k23rd, neogeo)
STD_ROM_FN(kf2k23rd)

struct BurnDriver BurnDrvkf2k23rd = {
	"kf2k23rd", "kof2002", "neogeo", NULL, "2020-06-14",
	"The King of Fighters 2002 - 3rd Strike of Orochi (Hack by EGCG/EGHT)\0", NULL, "hack", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k23rdRomInfo, kf2k23rdRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 Remix Ultra 3.5 - Hack By FCHT

static struct BurnRomInfo kf2k2ru35RomDesc[] = {
	{ "kf2k2ru35-p1.bin",   0x100000, 0x8da72d5c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "kf2k2ru35-p2.bin",   0x400000, 0x75185760, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "kf2k2ru35-s1.bin",   0x020000, 0xbd19c308, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "kf2k2ru35-c1.bin",   0x800000, 0x7efa6ef7, 3 | BRF_GRA },           //  3 Sprite data
	{ "kf2k2ru35-c2.bin",   0x800000, 0xaa82948b, 3 | BRF_GRA },           //  4
	{ "kf2k2ru35-c3.bin",   0x800000, 0xca656090, 3 | BRF_GRA },           //  5
	{ "kf2k2ru35-c4.bin",   0x800000, 0xeb898849, 3 | BRF_GRA },           //  6
	{ "kf2k2ru35-c5.bin",   0x800000, 0x74bba7c6, 3 | BRF_GRA },           //  7
	{ "kf2k2ru35-c6.bin",   0x800000, 0xe20d2216, 3 | BRF_GRA },           //  8
	{ "kf2k2ru35-c7.bin",   0x800000, 0x2a9d505c, 3 | BRF_GRA },           //  9
	{ "kf2k2ru35-c8.bin",   0x800000, 0x93d8501f, 3 | BRF_GRA },           // 10

	{ "kf2k2ru35-m1.bin",   0x020000, 0x9956ccd8, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "kf2k2ru35-v1.bin",   0x800000, 0x6e7e3508, 5 | BRF_SND },           // 12 Sound data
	{ "kf2k2ru35-v2.bin",   0x800000, 0x2157b90f, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k2ru35, kf2k2ru35, neogeo)
STD_ROM_FN(kf2k2ru35)

struct BurnDriver BurnDrvkf2k2ru35 = {
	"kf2k2ru35", "kof2002", "neogeo", NULL, "2006",
	"The King of Fighters 2002 Remix Ultra 3.5 (Hack By FCHT)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k2ru35RomInfo, kf2k2ru35RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters 2002 (Omega v.0?)

static struct BurnRomInfo kof2k2omgRomDesc[] = {
	{ "265omg.p1",    0x3bd4c1, 0x62174834, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265omg.p2",    0x400000, 0x6fadc5c3, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "265omg.s1",    0x020000, 0x6145daf4, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265xxx.c1",    0x800000, 0x745b343e, 3 | BRF_GRA },           //  3 Sprite data
	{ "265xxx.c2",    0x800000, 0x2aab7f98, 3 | BRF_GRA },           //  4
	{ "265ori.c3",    0x800000, 0xe5074eea, 3 | BRF_GRA },           //  5
	{ "265ori.c4",    0x800000, 0xf6eb1ff2, 3 | BRF_GRA },           //  6
	{ "265d.c5",      0x800000, 0x74bba7c6, 3 | BRF_GRA },           //  7
	{ "265d.c6",      0x800000, 0xe20d2216, 3 | BRF_GRA },           //  8
	{ "265ori.c7",    0x800000, 0x0e9f6adb, 3 | BRF_GRA },           //  9
	{ "265ori.c8",    0x800000, 0x9961799e, 3 | BRF_GRA },           // 10

	{ "265ori.m1",    0x020000, 0xab9d360e, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "265nu.v1",     0x400000, 0x13d98607, 5 | BRF_SND },           // 12 Sound data
	{ "265nu.v2",     0x400000, 0x9cf74677, 5 | BRF_SND },           // 13
	{ "265nu.v3",     0x400000, 0x8e9448b5, 5 | BRF_SND },           // 14
	{ "265nu.v4",     0x400000, 0x067271b5, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof2k2omg, kof2k2omg, neogeo)
STD_ROM_FN(kof2k2omg)

static void kof2k2omgCallback()
{
	BurnLoadRom(Neo68KROMActive + 0x000000, 0, 1);
	BurnLoadRom(Neo68KROMActive + 0x100000, 1, 1);
}

static INT32 kof2k2omgInit()
{
	NeoCallbackActive->pInitialise = kof2k2omgCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvkof2k2omg = {
	"kof2k2omg", "kof2002", "neogeo", NULL, "2002",
	"The King of Fighters 2002 (Omega v.0?)\0", NULL, "hack", "KOF-ON Team",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2k2omgRomInfo, kof2k2omgRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kof2k2omgInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 (Omega v0.8)

static struct BurnRomInfo kof2k2omg8RomDesc[] = {
	{ "265omg8.p1",   0x100000, 0x53086581, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265omg8.p2",   0x400000, 0x40228fe5, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "265omg8.s1",   0x020000, 0x65351d40, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265omg8.c1",   0x800000, 0xe144302d, 3 | BRF_GRA },           //  3 Sprite data
	{ "265omg8.c2",   0x800000, 0xb10c6958, 3 | BRF_GRA },           //  4
	{ "265omg8.c3",   0x800000, 0x2367927c, 3 | BRF_GRA },           //  5
	{ "265omg8.c4",   0x800000, 0x4ad48c29, 3 | BRF_GRA },           //  6
	{ "365ru.c5",     0x800000, 0x11126545, 3 | BRF_GRA },           //  7
	{ "365ru.c6",     0x800000, 0xd225bb9b, 3 | BRF_GRA },           //  8
	{ "265omg8.c7",   0x800000, 0x890c2a4a, 3 | BRF_GRA },           //  9
	{ "265omg8.c8",   0x800000, 0xb726009d, 3 | BRF_GRA },           // 10

	{ "265omg8.m1",   0x040000, 0x9a3d5c65, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "kf10-v1.bin",  0x800000, 0x0fc9a58d, 5 | BRF_SND },           // 12 Sound data
	{ "kf10-v2.bin",  0x800000, 0xb8c475a4, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kof2k2omg8, kof2k2omg8, neogeo)
STD_ROM_FN(kof2k2omg8)

struct BurnDriver BurnDrvkof2k2omg8 = {
	"kof2k2omg8", "kof2002", "neogeo", NULL, "2010",
	"The King of Fighters 2002 (Omega v0.8)\0", NULL, "hack", "KOF-ON Team",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2k2omg8RomInfo, kof2k2omg8RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 (Omega v0.9 beta)

static struct BurnRomInfo kof2k2omg9bRomDesc[] = {
	{ "265-p1.p1",    0x100000, 0x9ede7323, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265omg9b.p2",  0x400000, 0xa171b684, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "265omg9b.s1",  0x020000, 0x7e3f391a, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265omg9b.c1",  0x800000, 0xc8314ae2, 3 | BRF_GRA },           //  3 Sprite data
	{ "265omg9b.c2",  0x800000, 0xe0ddced5, 3 | BRF_GRA },           //  4
	{ "265omg9b.c3",  0x800000, 0x6898f6e2, 3 | BRF_GRA },           //  5
	{ "265omg9b.c4",  0x800000, 0x9232e491, 3 | BRF_GRA },           //  6
	{ "365ru.c5",     0x800000, 0x11126545, 3 | BRF_GRA },           //  7
	{ "365ru.c6",     0x800000, 0xd225bb9b, 3 | BRF_GRA },           //  8
	{ "265omg9b.c7",  0x800000, 0x6954ef57, 3 | BRF_GRA },           //  9
	{ "265omg9b.c8",  0x800000, 0x3e394883, 3 | BRF_GRA },           // 10

	{ "265omg8.m1",   0x040000, 0x9a3d5c65, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "265nu.v1",     0x400000, 0x13d98607, 5 | BRF_SND },           // 12 Sound data
	{ "265nu.v2",     0x400000, 0x9cf74677, 5 | BRF_SND },           // 13
	{ "265nu.v3",     0x400000, 0x8e9448b5, 5 | BRF_SND },           // 14
	{ "265nu.v4",     0x400000, 0x067271b5, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof2k2omg9b, kof2k2omg9b, neogeo)
STD_ROM_FN(kof2k2omg9b)

struct BurnDriver BurnDrvkof2k2omg9b = {
	"kof2k2omg9b", "kof2002", "neogeo", NULL, "2011",
	"The King of Fighters 2002 (Omega v0.9 beta)\0", NULL, "hack", "KOF-ON Team",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2k2omg9bRomInfo, kof2k2omg9bRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 (Omega v0.9)

static struct BurnRomInfo kof2k2omg9RomDesc[] = {
	{ "265-p1.p1",    0x100000, 0x9ede7323, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265omg9.p2",   0x400000, 0xcd503fcf, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "265omg9.s1",   0x020000, 0x4562fb20, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "265omg9b.c1",  0x800000, 0xc8314ae2, 3 | BRF_GRA },           //  3 Sprite data
	{ "265omg9b.c2",  0x800000, 0xe0ddced5, 3 | BRF_GRA },           //  4
	{ "265omg9b.c3",  0x800000, 0x6898f6e2, 3 | BRF_GRA },           //  5
	{ "265omg9b.c4",  0x800000, 0x9232e491, 3 | BRF_GRA },           //  6
	{ "365ru.c5",     0x800000, 0x11126545, 3 | BRF_GRA },           //  7
	{ "365ru.c6",     0x800000, 0xd225bb9b, 3 | BRF_GRA },           //  8
	{ "265omg9b.c7",  0x800000, 0x6954ef57, 3 | BRF_GRA },           //  9
	{ "265omg9b.c8",  0x800000, 0x3e394883, 3 | BRF_GRA },           // 10

	{ "265ori.m1",    0x020000, 0xab9d360e, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "265nu.v1",     0x400000, 0x13d98607, 5 | BRF_SND },           // 12 Sound data
	{ "265nu.v2",     0x400000, 0x9cf74677, 5 | BRF_SND },           // 13
	{ "265nu.v3",     0x400000, 0x8e9448b5, 5 | BRF_SND },           // 14
	{ "265nu.v4",     0x400000, 0x067271b5, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(kof2k2omg9, kof2k2omg9, neogeo)
STD_ROM_FN(kof2k2omg9)

struct BurnDriver BurnDrvkof2k2omg9 = {
	"kof2k2omg9", "kof2002", "neogeo", NULL, "2012",
	"The King of Fighters 2002 (Omega v0.9)\0", NULL, "hack", "KOF-ON Team",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2k2omg9RomInfo, kof2k2omg9RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 (PlayStation 2 v1.0 2018-12-17, EGCG & GSC2007)
// This game can select a different m1 and vx, depending on the character chosen.
// Due to lack of info, this is not emulated. It could happen that bad sounds might occur.

static struct BurnRomInfo k2k2ps2reRomDesc[] = {
	{ "265-p1re.p1",	0x100000, 0x77a0044c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265-p2dre.sp2",	0x500000, 0xf9e4456a, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "265-p3re.p3",	0x020000, 0x6e6beeba, 1 | BRF_ESS | BRF_PRG }, //  2 Extra ROM

	{ "265-s1dre.s1",	0x020000, 0x714ade47, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "265-c1d.c1",		0x800000, 0x7efa6ef7, 3 | BRF_GRA },           //  4 Sprite data
	{ "265-c2d.c2",		0x800000, 0xaa82948b, 3 | BRF_GRA },           //  5
	{ "265-c3d.c3",		0x800000, 0x959fad0b, 3 | BRF_GRA },           //  6
	{ "265-c4d.c4",		0x800000, 0xefe6a468, 3 | BRF_GRA },           //  7
	{ "265-c5d.c5",		0x800000, 0x74bba7c6, 3 | BRF_GRA },           //  8
	{ "265-c6d.c6",		0x800000, 0xe20d2216, 3 | BRF_GRA },           //  9
	{ "265-c7dre.c7",	0x800000, 0xf0897b93, 3 | BRF_GRA },           //  10
	{ "265-c8dre.c8",	0x800000, 0x8d27a4a6, 3 | BRF_GRA },           //  11
	{ "265-c9dre.c9",	0x800000, 0x9939c08a, 3 | BRF_GRA },           //  12
	{ "265-c10dre.c10",	0x800000, 0xc724c069, 3 | BRF_GRA },           //  13

	{ "265-m1d.m1",		0x020000, 0x1c661a4b, 4 | BRF_ESS | BRF_PRG }, //  14 Z80 code
	{ "214-m1.m1",		0x020000, 0xdabc427c, 4 | BRF_ESS | BRF_PRG }, //  15 m2 not emulated
	{ "262-m1d.m1",		0x040000, 0x4bcc537b, 4 | BRF_ESS | BRF_PRG }, //  16 m3 not emulated
	{ "269-m1d.m1",		0x080000, 0x7b7bf462, 4 | BRF_ESS | BRF_PRG }, //  17 m4 not emulated

	{ "265-v1d.v1",		0x800000, 0x0fc9a58d, 5 | BRF_SND },           //  18 Sound data
	{ "265-v2d.v2",		0x800000, 0xb8c475a4, 5 | BRF_SND },           //  19

	{ "214-v1.v1",		0x400000, 0x63f7b045, 5 | BRF_SND },           //  20 m2 snd data not emulated
	{ "214-v2.v2",		0x400000, 0x25929059, 5 | BRF_SND },           //  21
	{ "214-v3.v3",		0x200000, 0x92a2257d, 5 | BRF_SND },           //  22

	{ "262-v1d.v1",		0x800000, 0xac2913bf, 5 | BRF_SND },           //  23 m3 snd data not emulated
	{ "262-v2d.v2",		0x800000, 0x15042f30, 5 | BRF_SND },           //  24

	{ "269-v1d.v1",		0x800000, 0xff64cd56, 5 | BRF_SND },           //  25 m4 snd data not emulated
	{ "269-v2d.v2",		0x800000, 0xa8dd6446, 5 | BRF_SND },           //  26
};

STDROMPICKEXT(k2k2ps2re, k2k2ps2re, neogeo)
STD_ROM_FN(k2k2ps2re)

static INT32 k2k2ps2reInit()
{
	INT32 nRet = NeoInit();
	if (0 == nRet) NeoMapExtraRom(0x600000, 0x20000);

	return nRet;
}

struct BurnDriver BurnDrvk2k2ps2re = {
	"kf2k2ps2re", "kof2002", "neogeo", NULL, "2018",
	"The King of Fighters 2002 (PlayStation 2 v1.0 Public Beta)\0", "Imperfect sound", "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, k2k2ps2reRomInfo, k2k2ps2reRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	k2k2ps2reInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2002 (Plus 2017, Hack) ver. 20201022
// Hack by GSC2007
// GOTVG 2020/10/29

static struct BurnRomInfo kof2k2plusRomDesc[] = {
	/* Encrypted */
	{ "265-p1pls.p1",		0x100000, 0x059da6f9, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "265-p2pls.sp2",		0x500000, 0xb0ac3d06, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "265-p3pls.p3",		0x020000, 0x80aacc1f, 1 | BRF_ESS | BRF_PRG }, //  2 Extra ROM
	
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "265-c1pls.c1",		0x800000, 0x7edfc0bd, 3 | BRF_GRA },           //  3 Sprite data
	{ "265-c2pls.c2",		0x800000, 0xb4f0de84, 3 | BRF_GRA },           //  4
	{ "265-c3pls.c3",		0x800000, 0x009a05ec, 3 | BRF_GRA },           //  5
	{ "265-c4pls.c4",		0x800000, 0x68c38128, 3 | BRF_GRA },           //  6
	{ "265-c5pls.c5",		0x800000, 0x26b43328, 3 | BRF_GRA },           //  7
	{ "265-c6pls.c6",		0x800000, 0xb27c49b6, 3 | BRF_GRA },           //  8
	{ "265-c7pls.c7",		0x800000, 0x80533c3f, 3 | BRF_GRA },           //  9
	{ "265-c8pls.c8",		0x800000, 0x3d5c17ae, 3 | BRF_GRA },           //  10

	/* Encrypted */
	{ "265-m1.m1",			0x020000, 0x85aaa632, 4 | BRF_ESS | BRF_PRG }, //  11 Z80 code

	/* Encrypted */
	{ "265-v1.v1",			0x800000, 0x15e8f3f5, 5 | BRF_SND },           //  12 Sound data
	{ "265-v2.v2",			0x800000, 0xda41d6f9, 5 | BRF_SND },           //  13
};

STDROMPICKEXT(kof2k2plus, kof2k2plus, neogeo)
STD_ROM_FN(kof2k2plus)

static INT32 kof2k2plusInit()
{
	nNeoProtectionXor = 0xEC;
	NeoCallbackActive->pInitialise = PCM2DecryptP;
	INT32 nRet = NeoInit();

	if (nRet == 0) {
		SekOpen(0);
		SekMapMemory(Neo68KROMActive + 0x600000, 0x900000, 0x91ffff, MAP_ROM);
		SekClose();

		const PCM2DecryptV2Info Info = { 0xa5000, 0x000000, { 0xf9, 0xe0, 0x5d, 0xf3, 0xea, 0x92, 0xbe, 0xef } };

		PCM2DecryptV2(&Info);
	}

	return nRet;
}

struct BurnDriver BurnDrvkof2k2plus = {
	"kof2k2plus", "kof2002", "neogeo", NULL, "2020",
	"The King of Fighters 2002 (Plus 2017)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_VSFIGHT, FBF_KOF,
	NULL, kof2k2plusRomInfo, kof2k2plusRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	kof2k2plusInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 10th Anniversary (The King of Fighters 2002 bootleg / Fully Decrypted)

static struct BurnRomInfo kof10thdRomDesc[] = {
	{ "363d.p1",      0x800000, 0x30c82f4c, 1 | BRF_ESS | BRF_PRG }, // 1 68K code

	{ "363d.s1",      0x020000, 0x3c757cb1, 2 | BRF_GRA },           // 2 Text layer tiles / TC531000

	{ "kf10-c1a.bin", 0x400000, 0x3bbc0364, 3 | BRF_GRA },           // 3 Sprite data
	{ "kf10-c2a.bin", 0x400000, 0x91230075, 3 | BRF_GRA },           // 4
	{ "kf10-c1b.bin", 0x400000, 0xb5abfc28, 3 | BRF_GRA },           // 5
	{ "kf10-c2b.bin", 0x400000, 0x6cc4c6e1, 3 | BRF_GRA },           // 6
	{ "kf10-c3a.bin", 0x400000, 0x5b3d4a16, 3 | BRF_GRA },           // 7
	{ "kf10-c4a.bin", 0x400000, 0xc6f3419b, 3 | BRF_GRA },           // 8
	{ "kf10-c3b.bin", 0x400000, 0x9d2bba19, 3 | BRF_GRA },           // 9
	{ "kf10-c4b.bin", 0x400000, 0x5a4050cb, 3 | BRF_GRA },           // 10
	{ "kf10-c5a.bin", 0x400000, 0xa289d1e1, 3 | BRF_GRA },           // 11
	{ "kf10-c6a.bin", 0x400000, 0xe6494b5d, 3 | BRF_GRA },           // 12
	{ "kf10-c5b.bin", 0x400000, 0x404fff02, 3 | BRF_GRA },           // 13
	{ "kf10-c6b.bin", 0x400000, 0xf2ccfc9e, 3 | BRF_GRA },           // 14
	{ "kf10-c7a.bin", 0x400000, 0xbe79c5a8, 3 | BRF_GRA },           // 15
	{ "kf10-c8a.bin", 0x400000, 0xa5952ca4, 3 | BRF_GRA },           // 16
	{ "kf10-c7b.bin", 0x400000, 0x3fdb3542, 3 | BRF_GRA },           // 17
	{ "kf10-c8b.bin", 0x400000, 0x661b7a52, 3 | BRF_GRA },           // 18

	{ "kf10-m1.bin",  0x020000, 0xf6fab859, 4 | BRF_ESS | BRF_PRG }, // 19 Z80 code

	{ "kf10-v1.bin",  0x800000, 0x0fc9a58d, 5 | BRF_SND },           // 20 Sound data
	{ "kf10-v2.bin",  0x800000, 0xb8c475a4, 5 | BRF_SND },           // 21
};

STDROMPICKEXT(kof10thd, kof10thd, neogeo)
STD_ROM_FN(kof10thd)

struct BurnDriver BurnDrvkof10thd = {
	"kof10thd", "kof2002", "neogeo", NULL, "200?",
	"The King of Fighters 10th Anniversary (The King of Fighters 2002 bootleg / Fully Decrypted)\0", NULL, "hack", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof10thdRomInfo, kof10thdRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 10th Anniversary 2020 (Optimized version 2020)
// Hack by DOVE FLY
// GOTVG 2020/11/11

static struct BurnRomInfo kof10thuoRomDesc[] = {
	{ "kf10udo-p1.p1",   0x800000, 0x5c261d57, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "kf10udo-s1.s1",   0x020000, 0x312bf34b, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "kf10udo-c1.c1",   0x800000, 0x2c59db66, 3 | BRF_GRA },           //  2 Sprite data
	{ "kf10udo-c2.c2",   0x800000, 0x024ffd69, 3 | BRF_GRA },           //  3
	{ "kf10udo-c3.c3",   0x800000, 0x959fad0b, 3 | BRF_GRA },           //  4
	{ "kf10udo-c4.c4",   0x800000, 0xefe6a468, 3 | BRF_GRA },           //  5
	{ "kf10udo-c5.c5",   0x800000, 0x74bba7c6, 3 | BRF_GRA },           //  6
	{ "kf10udo-c6.c6",   0x800000, 0xe20d2216, 3 | BRF_GRA },           //  7
	{ "kf10udo-c7.c7",   0x800000, 0x8a5b561c, 3 | BRF_GRA },           //  8
	{ "kf10udo-c8.c8",   0x800000, 0xbef667a3, 3 | BRF_GRA },           //  9

	{ "kf10udo-m1.m1",   0x020000, 0xf6fab859, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "kf10udo-v1.v1",   0x800000, 0x0fc9a58d, 5 | BRF_SND },           // 11 Sound data
	{ "kf10udo-v2.v2",   0x800000, 0xb8c475a4, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(kof10thuo, kof10thuo, neogeo)
STD_ROM_FN(kof10thuo)

struct BurnDriver BurnDrvkof10thuo = {
	"kof10thuo", "kof2002", "neogeo", NULL, "2020",
	"The King of Fighters 10th Anniversary 2020 (Optimized version 2020)\0", NULL, "hack", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kof10thuoRomInfo, kof10thuoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The King of Fighters 2003 - PlayStation 2 (Hack By EGCG)

static struct BurnRomInfo kf2k3ps2RomDesc[] = {
	{ "kf2k3ps2-p1.bin",    0x800000, 0x8e66056d, 1 | BRF_ESS | BRF_PRG }, //  0

	{ "kf2k3ps2-s1.bin",    0x020000, 0xc2e48e98, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "kf2k3ps2-c1.bin",    0x800000, 0xe42fc226, 3 | BRF_GRA },           //  2 Sprite data
	{ "kf2k3ps2-c2.bin",    0x800000, 0x1b5e3b58, 3 | BRF_GRA },           //  3
	{ "kf2k3ps2-c3.bin",    0x800000, 0xd334fdd9, 3 | BRF_GRA },           //  4
	{ "kf2k3ps2-c4.bin",    0x800000, 0x0d457699, 3 | BRF_GRA },           //  5
	{ "kf2k3ps2-c5.bin",    0x800000, 0x8a91aae4, 3 | BRF_GRA },           //  6
	{ "kf2k3ps2-c6.bin",    0x800000, 0x9f8674b8, 3 | BRF_GRA },           //  7
	{ "kf2k3ps2-c7.bin",    0x800000, 0x0ff993d7, 3 | BRF_GRA },           //  8
	{ "kf2k3ps2-c8.bin",    0x800000, 0x22e36002, 3 | BRF_GRA },           //  9

	{ "kf2k3ps2-m1.bin",    0x080000, 0x4374cb5c, 4 | BRF_ESS | BRF_PRG }, //  10 Z80 code

	{ "kf2k3ps2-v1.bin",    0x800000, 0xdd6c6a85, 5 | BRF_SND },           //  11 Sound data
	{ "kf2k3ps2-v2.bin",    0x800000, 0x0e84f8c1, 5 | BRF_SND },           //  12
};

STDROMPICKEXT(kf2k3ps2, kf2k3ps2, neogeo)
STD_ROM_FN(kf2k3ps2)

struct BurnDriver BurnDrvkf2k3ps2 = {
	"kf2k3ps2", "kof2003", "neogeo", NULL, "2006",
	"The King of Fighters 2003 PlayStation 2 (Hack By EGCG)\0", "Use AES (Console) mode!", "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k3ps2RomInfo, kf2k3ps2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The King of Fighters Special Edition 2004 Plus (bootleg)

static struct BurnRomInfo kf2k4plsRomDesc[] = {
	{ "k2k4s-p1p.bin", 0x200000, 0xed97c684, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "k2k4s-p1.bin",  0x400000, 0xe6c50566, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "k2k4s-s1p.bin", 0x020000, 0xcc0ab564, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "k2k4s-c4.bin",  0x800000, 0x7a050288, 3 | BRF_GRA },           //  3 Sprite data
	{ "k2k4s-c8.bin",  0x800000, 0xe924afcf, 3 | BRF_GRA },           //  4
	{ "k2k4s-c3.bin",  0x800000, 0x959fad0b, 3 | BRF_GRA },           //  5
	{ "k2k4s-c7.bin",  0x800000, 0xefe6a468, 3 | BRF_GRA },           //  6
	{ "k2k4s-c2.bin",  0x800000, 0x74bba7c6, 3 | BRF_GRA },           //  7
	{ "k2k4s-c6.bin",  0x800000, 0xe20d2216, 3 | BRF_GRA },           //  8
	{ "k2k4s-c1.bin",  0x800000, 0xfa705b2b, 3 | BRF_GRA },           //  9
	{ "k2k4s-c5.bin",  0x800000, 0x2c912ff9, 3 | BRF_GRA },           // 10

	{ "k2k4s-m1.bin",  0x020000, 0x5a47d9ad, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "k2k4s-v2.bin",  0x800000, 0xe4ddfb3f, 5 | BRF_SND },           // 12 Sound data
	{ "k2k4s-v1.bin",  0x800000, 0xb887d287, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(kf2k4pls, kf2k4pls, neogeo)
STD_ROM_FN(kf2k4pls)

static void kf2k4plsCallback()
{
	UINT8 *pTemp = (UINT8*)BurnMalloc(0x600000);

	if (pTemp) {
		memcpy(pTemp, Neo68KROMActive, 0x600000);
		memcpy(Neo68KROMActive + 0x000000, pTemp + 0x000000, 0x100000);
		memcpy(Neo68KROMActive + 0x500000, pTemp + 0x100000, 0x100000);
		memcpy(Neo68KROMActive + 0x400000, pTemp + 0x200000, 0x100000);
		memcpy(Neo68KROMActive + 0x300000, pTemp + 0x300000, 0x100000);
		memcpy(Neo68KROMActive + 0x200000, pTemp + 0x400000, 0x100000);
		memcpy(Neo68KROMActive + 0x100000, pTemp + 0x500000, 0x100000);

		memset(pTemp, 0, 0x600000);
		memcpy(pTemp + 0x000000, Neo68KROMActive + 0x000000, 0x100000);
		memcpy(pTemp + 0x100000, Neo68KROMActive + 0x500000, 0x100000);
		memcpy(Neo68KROMActive + 0x000000, pTemp + 0x1a0000, 0x020000);
		memcpy(Neo68KROMActive + 0x020000, pTemp + 0x080000, 0x020000);
		memcpy(Neo68KROMActive + 0x040000, pTemp + 0x140000, 0x020000);
		memcpy(Neo68KROMActive + 0x060000, pTemp + 0x000000, 0x020000);
		memcpy(Neo68KROMActive + 0x080000, pTemp + 0x180000, 0x020000);
		memcpy(Neo68KROMActive + 0x0a0000, pTemp + 0x0a0000, 0x020000);
		memcpy(Neo68KROMActive + 0x0c0000, pTemp + 0x100000, 0x020000);
		memcpy(Neo68KROMActive + 0x0e0000, pTemp + 0x040000, 0x020000);

		BurnFree(pTemp);
	}

	lans2004_sx_decode();
}

static INT32 kf2k4plsInit()
{
	NeoCallbackActive->pInitialise = kf2k4plsCallback;

	return NeoInit();
}

struct BurnDriver BurnDrvkf2k4pls = {
	"kf2k4pls", "kof2002", "neogeo", NULL, "2004",
	"The King of Fighters Special Edition 2004 Plus (bootleg)\0", NULL, "bootleg", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, FBF_KOF,
	NULL, kf2k4plsRomInfo, kf2k4plsRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	kf2k4plsInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// The Last Blade / Bakumatsu Roman - Gekka no Kenshi (Special 2017, hack)
// Modified by: GSC2007
// Version number: Ver 1.1-FINAL

static struct BurnRomInfo lastbladspRomDesc[] = {
	{ "234-p1sp.p1",	0x100000, 0xf8adc621, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "234-p2sp.sp2",	0x600000, 0x8ff3fb6d, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "234-p3sp.p3",	0x020000, 0x3c1770df, 1 | BRF_ESS | BRF_PRG }, //  2

	{ "234-s1.s1",		0x020000, 0x95561412, 2 | BRF_GRA },           //  3 Text layer tiles

	{ "234-c1.c1",		0x800000, 0x9f7e2bd3, 3 | BRF_GRA },           //  4 Sprite data
	{ "234-c2.c2",		0x800000, 0x80623d3c, 3 | BRF_GRA },           //  5
	{ "234-c3.c3",		0x800000, 0x91ab1a30, 3 | BRF_GRA },           //  6
	{ "234-c4.c4",		0x800000, 0x3d60b037, 3 | BRF_GRA },           //  7
	{ "234-c5sp.c5",	0x400000, 0x4ea22fe0, 3 | BRF_GRA },           //  8
	{ "234-c6sp.c6",	0x400000, 0xa863c882, 3 | BRF_GRA },           //  9

	{ "234-m1.m1",		0x020000, 0x087628ea, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "234-v1.v1",		0x400000, 0xed66b76f, 5 | BRF_SND },           // 11 Sound data
	{ "234-v2.v2",		0x400000, 0xa0e7f6e2, 5 | BRF_SND },           // 12
	{ "234-v3.v3",		0x400000, 0xa506e1e2, 5 | BRF_SND },           // 13
	{ "234-v4.v4",		0x400000, 0x0e34157f, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(lastbladsp, lastbladsp, neogeo)
STD_ROM_FN(lastbladsp)

static void LastbladspPatchCallback()
{
	UINT16* rom = (UINT16*)Neo68KROMActive;

	for (INT32 i = 0; i < 0x100000 >> 1; i++) {
		if (rom[i] == 0x4e7d) rom[i] = 0x4e71;
		if (rom[i] == 0x4e7c) rom[i] = 0x4e75;
	}

	rom[0x69c10 >> 1] = 0x303c; // 4ef9
	rom[0x69c12 >> 1] = 0x0b37; // 0091
	rom[0x69c14 >> 1] = 0x323c; // 03c4
	rom[0x69c16 >> 1] = 0x0013; // 4e7d

	rom = (UINT16*)(Neo68KROMActive + 0x700000);

	for (INT32 i = 0; i < 0x20000 >> 1; i++) {
		if (rom[i] == 0x4e7d) rom[i] = 0x4e71;
		if (rom[i] == 0x4e7c) rom[i] = 0x4e75;
	}

	rom[0x11036 >> 1] = 0x4e75; // lbsp v1.1 fix, thanks HBMAME :)

}

static INT32 LastbladspInit()
{
	NeoCallbackActive->pInitialise = LastbladspPatchCallback;

	INT32 nRet = NeoInit();
	if (0 == nRet) NeoMapExtraRom(0x700000, 0x20000);

	return nRet;
}

struct BurnDriver BurnDrvlastbladsp = {
	"lastbladsp", "lastblad", "neogeo", NULL, "2017",
	"The Last Blade / Bakumatsu Roman - Gekka no Kenshi (Special 2017, hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, lastbladspRomInfo, lastbladspRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	LastbladspInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// The Last Blade 2 / Bakumatsu Roman - Dai Ni Maku Gekka no Kenshi (Enhanced Hack)
// Hackers: Dodowang, Eddids
// Unlock "EX" mode (highlight Speed and press C) & Enable hidden characters

static struct BurnRomInfo lastbld2ehRomDesc[] = {
	{ "243-p1eh.p1",  0x100000, 0x6c8867d2, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "243-p2eh.p2",  0x400000, 0xeef07572, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "243-s1.s1",    0x020000, 0xc9cd2298, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "243-c1.c1",    0x800000, 0x5839444d, 3 | BRF_GRA },           //  3 Sprite data
	{ "243-c2.c2",    0x800000, 0xdd087428, 3 | BRF_GRA },           //  4
	{ "243-c3.c3",    0x800000, 0x6054cbe0, 3 | BRF_GRA },           //  5
	{ "243-c4.c4",    0x800000, 0x8bd2a9d2, 3 | BRF_GRA },           //  6
	{ "243-c5.c5",    0x800000, 0x6a503dcf, 3 | BRF_GRA },           //  7
	{ "243-c6.c6",    0x800000, 0xec9c36d0, 3 | BRF_GRA },           //  8

	{ "243-m1.m1",    0x020000, 0xacf12d10, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "243-v1.v1",    0x400000, 0xf7ee6fbb, 5 | BRF_SND },           // 10 Sound data
	{ "243-v2.v2",    0x400000, 0xaa9e4df6, 5 | BRF_SND },           // 11
	{ "243-v3.v3",    0x400000, 0x4ac750b2, 5 | BRF_SND },           // 12
	{ "243-v4.v4",    0x400000, 0xf5c64ba6, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(lastbld2eh, lastbld2eh, neogeo)
STD_ROM_FN(lastbld2eh)

struct BurnDriver BurnDrvDlastbld2eh = {
	"lastbld2eh", "lastbld2", "neogeo", NULL, "1998",
	"The Last Blade 2 / Bakumatsu Roman - Dai Ni Maku Gekka no Kenshi (Enhanced Hack)\0", NULL, "hack", "Neo Geo MVS",
	L"The Last Blade 2\0\u5E55\u672B\u6D6A\u6F2B\u7B2C\u4E8C\u5E55 - \u6708\u83EF\u306E\u5263\u58EB - \u6708\u306B\u54B2\u304F\u83EF\u3001\u6563\u308A\u3086\u304F\u82B1 (Enhanced Hack)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, lastbld2ehRomInfo, lastbld2ehRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// The Last Blade 2 / Bakumatsu Roman - Dai Ni Maku Gekka no Kenshi (Team Edition Hack)
// Hackers: GSC2007
// Add Team Mod / Version 1.1

static struct BurnRomInfo lastbld2teRomDesc[] = {
	{ "243te-pg1.p1", 0x100000, 0x8f9a24bf, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "243te-pg2.p2", 0x400000, 0x37aaffa0, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "243-s1.s1",    0x020000, 0xc9cd2298, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "243-c1.c1",    0x800000, 0x5839444d, 3 | BRF_GRA },           //  3 Sprite data
	{ "243-c2.c2",    0x800000, 0xdd087428, 3 | BRF_GRA },           //  4
	{ "243-c3.c3",    0x800000, 0x6054cbe0, 3 | BRF_GRA },           //  5
	{ "243-c4.c4",    0x800000, 0x8bd2a9d2, 3 | BRF_GRA },           //  6
	{ "243-c5.c5",    0x800000, 0x6a503dcf, 3 | BRF_GRA },           //  7
	{ "243-c6.c6",    0x800000, 0xec9c36d0, 3 | BRF_GRA },           //  8

	{ "243-m1.m1",    0x020000, 0xacf12d10, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "243-v1.v1",    0x400000, 0xf7ee6fbb, 5 | BRF_SND },           // 10 Sound data
	{ "243-v2.v2",    0x400000, 0xaa9e4df6, 5 | BRF_SND },           // 11
	{ "243-v3.v3",    0x400000, 0x4ac750b2, 5 | BRF_SND },           // 12
	{ "243-v4.v4",    0x400000, 0xf5c64ba6, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(lastbld2te, lastbld2te, neogeo)
STD_ROM_FN(lastbld2te)

struct BurnDriver BurnDrvDlastbld2te = {
	"lastbld2te", "lastbld2", "neogeo", NULL, "1998",
	"The Last Blade 2 / Bakumatsu Roman - Dai Ni Maku Gekka no Kenshi (Team Edition Hack)\0", NULL, "hack", "Neo Geo MVS",
	L"The Last Blade 2\0\u5E55\u672B\u6D6A\u6F2B\u7B2C\u4E8C\u5E55 - \u6708\u83EF\u306E\u5263\u58EB - \u6708\u306B\u54B2\u304F\u83EF\u3001\u6563\u308A\u3086\u304F\u82B1 (Team Edition Hack)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, lastbld2teRomInfo, lastbld2teRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// The Last Blade 2 / Bakumatsu Roman - Dai Ni Maku Gekka no Kenshi (Enable Hidden Characters V4)

static struct BurnRomInfo lb2bRomDesc[] = {
	{ "243-pg1b.p1",  0x100000, 0x6e512568, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "243-pg2.sp2",  0x400000, 0xadd4a30b, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "243-s1.s1",    0x020000, 0xc9cd2298, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "243-c1.c1",    0x800000, 0x5839444d, 3 | BRF_GRA },           //  3 Sprite data
	{ "243-c2.c2",    0x800000, 0xdd087428, 3 | BRF_GRA },           //  4
	{ "243-c3.c3",    0x800000, 0x6054cbe0, 3 | BRF_GRA },           //  5
	{ "243-c4.c4",    0x800000, 0x8bd2a9d2, 3 | BRF_GRA },           //  6
	{ "243-c5.c5",    0x800000, 0x6a503dcf, 3 | BRF_GRA },           //  7
	{ "243-c6.c6",    0x800000, 0xec9c36d0, 3 | BRF_GRA },           //  8

	{ "243-m1.m1",    0x020000, 0xacf12d10, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "243-v1.v1",    0x400000, 0xf7ee6fbb, 5 | BRF_SND },           // 10 Sound data
	{ "243-v2.v2",    0x400000, 0xaa9e4df6, 5 | BRF_SND },           // 11
	{ "243-v3.v3",    0x400000, 0x4ac750b2, 5 | BRF_SND },           // 12
	{ "243-v4.v4",    0x400000, 0xf5c64ba6, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(lb2b, lb2b, neogeo)
STD_ROM_FN(lb2b)

struct BurnDriver BurnDrvLb2b = {
	"lb2b", "lastbld2", "neogeo", NULL, "1998",
	"The Last Blade 2 / Bakumatsu Roman - Dai Ni Maku Gekka no Kenshi (Enable Hidden Characters V4)\0", NULL, "Dodowang[EGCG]", "Neo Geo MVS",
	L"The Last Blade 2\0\u5E55\u672B\u6D6A\u6F2B\u7B2C\u4E8C\u5E55 - \u6708\u83EF\u306E\u5263\u58EB - \u6708\u306B\u54B2\u304F\u83EF\u3001\u6563\u308A\u3086\u304F\u82B1 (Enable Hidden Characters V4)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, lb2bRomInfo, lb2bRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 320, 224, 4, 3
};

// Treasure of the Caribbean

static struct BurnRomInfo totcRomDesc[] = {
	{ "316-p1.p1",    	0x100000, 0x99604539, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "316-s1.s1",    	0x020000, 0x0a3fee41, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "316-c1.c1",    	0x200000, 0xcdd6600f, 3 | BRF_GRA },           //  2 Sprite data
	{ "316-c1.c2",    	0x200000, 0xf362c271, 3 | BRF_GRA },           //  3

	{ "316-m1.m1",    	0x020000, 0x18b23ace, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "316-v1.v1",    	0x200000, 0x15c7f9e6, 5 | BRF_SND },           //  7 Sound data
	{ "316-v2.v2",    	0x200000, 0x1b264559, 5 | BRF_SND },           //  8
	{ "316-v3.v3",    	0x100000, 0x84b62c5d, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(totc, totc, neogeo)
STD_ROM_FN(totc)

struct BurnDriver BurnDrvtotc = {
	"totc", NULL, "neogeo", NULL, "2011",
	"Treasure of the Caribbean\0", NULL, "FACE Corporation / N.C.I - Le Cortex", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, totcRomInfo, totcRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Waku Waku 7 (Boss Hack)
// Enabled Fernandeath, Bonuskun - hack by yumeji1768 [2ch]

static struct BurnRomInfo wakuwak7bhRomDesc[] = {
	{ "225-p1bh.p1",  0x100000, 0x0b7a3776, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "225-p2.sp2",   0x200000, 0xfe190665, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "225-s1.s1",    0x020000, 0x71c4b4b5, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "225-c1.c1",    0x400000, 0xee4fea54, 3 | BRF_GRA },           //  3 Sprite data
	{ "225-c2.c2",    0x400000, 0x0c549e2d, 3 | BRF_GRA },           //  4
	{ "225-c3.c3",    0x400000, 0xaf0897c0, 3 | BRF_GRA },           //  5
	{ "225-c4.c4",    0x400000, 0x4c66527a, 3 | BRF_GRA },           //  6
	{ "225-c5.c5",    0x400000, 0x8ecea2b5, 3 | BRF_GRA },           //  7
	{ "225-c6.c6",    0x400000, 0x0eb11a6d, 3 | BRF_GRA },           //  8

	{ "225-m1.m1",    0x020000, 0x0634bba6, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "225-v1.v1",    0x400000, 0x6195c6b4, 5 | BRF_SND },           // 10 Sound data
	{ "225-v2.v2",    0x400000, 0x6159c5fe, 5 | BRF_SND },           // 11
};

STDROMPICKEXT(wakuwak7bh, wakuwak7bh, neogeo)
STD_ROM_FN(wakuwak7bh)

struct BurnDriver BurnDrvwakuwak7bh = {
	"wakuwak7bh", "wakuwak7", "neogeo", NULL, "1996",
	"Waku Waku 7 (Boss Hack)\0", NULL, "hack", "Neo Geo MVS",
	L"Waku Waku 7\0\u308F\u304F\u308F\u304F\uFF17 (Boss Hack)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, wakuwak7bhRomInfo, wakuwak7bhRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Zintrick / Oshidashi Zentrix (Neo CD conversion)

static struct BurnRomInfo zintrkcdRomDesc[] = {
	{ "211-p1.bin",   0x100000, 0x9a0bfe0a, 1 | BRF_ESS | BRF_PRG }, //  0 68K Code

	{ "211-s1.bin",   0x020000, 0x56d16afa, 2 | BRF_GRA },		 	 //  1 Text data

	{ "zin-c1.bin",   0x200000, 0x76aee189, 3 | BRF_GRA },		 	 //  2 Sprite data
	{ "zin-c2.bin",   0x200000, 0x844ed4b3, 3 | BRF_GRA },		 	 //  3

	{ "211-m1.bin",   0x010000, 0xfcae1407, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "211-v1.bin",   0x100000, 0x781439da, 5 | BRF_SND },		 	 //  5 Sound data
};

STDROMPICKEXT(zintrkcd, zintrkcd, neogeo)
STD_ROM_FN(zintrkcd)

struct BurnDriver BurnDrvzintrkcd = {
	"zintrkcd", "zintrckb", "neogeo", NULL, "1996",
	"Zintrick / Oshidashi Zentrix (Neo CD conversion)\0", NULL, "hack", "Neo Geo MVS",
	L"Zintrick\0\u62BC\u3057\u51FA\u3057\u30B8\u30F3\u30C8\u30EA\u30C3\u30AF (Neo CD conversion)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, zintrkcdRomInfo, zintrkcdRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// -----------------------------------------------------------------------------
// Diagnostic Cartridges

// Neo Geo 4 player test cartridge

static struct BurnRomInfo fourplayRomDesc[] = {
	{ "4play_p1.rom", 0x100000, 0x3C3D741B, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "4play_s1.rom", 0x020000, 0xDE828076, 2 | BRF_GRA },			 //  1 Text data

	{ "4play_c1.rom", 0x020000, 0x7EE8CDCD, 3 | BRF_GRA },			 //  2 Sprite data
	{ "4play_c2.rom", 0x020000, 0x7EE8CDCD, 3 | BRF_GRA },			 //  3

	{ "4play_m1.rom", 0x020000, 0x5132580E, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "4play_v1.rom", 0x020000, 0x7EE8CDCD, 5 | BRF_SND },			 //  5 Sound data
};

STDROMPICKEXT(fourplay, fourplay, neogeo)
STD_ROM_FN(fourplay)

struct BurnDriver BurnDrv4play = {
	"4play", NULL, "neogeo", NULL, "???",
	"4 player input test\0", "4 player  input test cartridge", "SNK", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 4, HARDWARE_SNK_NEOGEO | HARDWARE_SNK_4_JOYSTICKS, GBF_MISC, 0,
	NULL, fourplayRomInfo, fourplayRomName, NULL, NULL, NULL, NULL, neodualInputInfo, neodualDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// -----------------------------------------------------------------------------
// Demos and Homebrew

// Shadow of the Beast (Neo Geo demo)

static struct BurnRomInfo beastRomDesc[] = {
	{ "2002-p1.bin",  0x080000, 0x1f9e62a4, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "2002-s1.bin",  0x020000, 0x05a417f5, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "2002-c1.bin",  0x080000, 0xe1974741, 3 | BRF_GRA },           //  2 Sprite data
	{ "2002-c2.bin",  0x080000, 0x9f5b9cf6, 3 | BRF_GRA },           //  3

	{ "nnp-m1.rom",   0x010000, 0x7669de64, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "nnp-v1.rom",   0x080000, 0x504bf849, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(beast, beast, neogeo)
STD_ROM_FN(beast)

struct BurnDriver BurnDrvbeast = {
	"beast", NULL, "neogeo", NULL, "????",
	"Shadow of the Beast (Neo Geo demo)\0", NULL, "Jeff Kurtz", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 2, HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, beastRomInfo, beastRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Codename - Blut Engel (2006-01-19)

static struct BurnRomInfo cnbeRomDesc[] = {
	{ "cnbe-p1.bin",  0x080000, 0x7ece4186, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "cnbe-s1.bin",  0x010000, 0x40a4698f, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "cnbe-c1.bin",  0x100000, 0x0f4fea6f, 3 | BRF_GRA },           //  2 Sprite data
	{ "cnbe-c2.bin",  0x100000, 0x3d5fc36f, 3 | BRF_GRA },           //  3

	{ "cnbe-m1.bin",  0x020000, 0xa5821c9c, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "cnbe-v1.bin",  0x080000, 0x70191764, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(cnbe, cnbe, neogeo)
STD_ROM_FN(cnbe)

struct BurnDriver BurnDrvcnbe = {
	"cnbe", NULL, "neogeo", NULL, "2006",
	"Codename - Blut Engel (2006-01-19)\0", NULL, "blastar@gmx.net", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_VERSHOOT, 0,
	NULL, cnbeRomInfo, cnbeRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Codename - Blut Engel (2018-09-05)

static struct BurnRomInfo cnbe2018RomDesc[] = {
	{ "cnbe2018-p1.bin",  0x040000, 0x189fecd5, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "cnbe2018-s1.bin",  0x010000, 0xec227c62, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "cnbe2018-c1.bin",  0x040000, 0xdb0e9083, 3 | BRF_GRA },           //  2 Sprite data
	{ "cnbe2018-c2.bin",  0x040000, 0x6114b6fb, 3 | BRF_GRA },           //  3 Sprite data

	{ "cnbe2018-m1.bin",  0x010000, 0xb94f41b4, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "cnbe2018-v1.bin",  0x040000, 0x54947995, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(cnbe2018, cnbe2018, neogeo)
STD_ROM_FN(cnbe2018)

struct BurnDriver BurnDrvcnbe2018 = {
	"cnbe2018", "cnbe", "neogeo", NULL, "2018",
	"Codename - Blut Engel (2018-09-05)\0", NULL, "blastar@gmx.net", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_VERSHOOT, 0,
	NULL, cnbe2018RomInfo, cnbe2018RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Columns (Neo Geo)

static struct BurnRomInfo columnsnRomDesc[] = {
	{ "2000-p1.bin",  0x080000, 0xc6c9888d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "2000-s1.bin",  0x020000, 0x089a28a8, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "2000-c1.bin",  0x080000, 0x9a579cab, 3 | BRF_GRA },           //  2 Sprite data
	{ "2000-c2.bin",  0x080000, 0x91d14043, 3 | BRF_GRA },           //  3

	{ "nnp-m1.rom",   0x010000, 0x7669de64, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "nnp-v1.rom",   0x080000, 0x504bf849, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(columnsn, columnsn, neogeo)
STD_ROM_FN(columnsn)

struct BurnDriver BurnDrvcolumnsn = {
	"columnsn", NULL, "neogeo", NULL, "????",
	"Columns (Neo Geo)\0", NULL, "homebrew", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, columnsnRomInfo, columnsnRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Jonas Indiana and the Lost Temple of RA (20050717)

static struct BurnRomInfo ltorbRomDesc[] = {
	{ "nnp-p1.rom",   0x040000, 0x353f6b05, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "nnp-s1.rom",   0x010000, 0x8224d8b7, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "nnp-c1.rom",   0x080000, 0x8b607198, 3 | BRF_GRA },           //  2 Sprite data
	{ "nnp-c2.rom",   0x080000, 0x6b96b59c, 3 | BRF_GRA },           //  3

	{ "nnp-m1.rom",   0x010000, 0x6847fd52, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "nnp-v1.rom",   0x080000, 0x0329c2e7, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(ltorb, ltorb, neogeo)
STD_ROM_FN(ltorb)

struct BurnDriver BurnDrvltorb = {
	"ltorb", NULL, "neogeo", NULL, "2005",
	"Jonas Indiana and the Lost Temple of RA (20050717)\0", NULL, "blastar@gmx.net", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, ltorbRomInfo, ltorbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo 2500 Demo

static struct BurnRomInfo neo2500RomDesc[] = {
	{ "2500-p1.bin",  0x080000, 0x2321c0d8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "2500-s1.bin",  0x010000, 0x1a37925c, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "2500-c1.bin",  0x080000, 0x8c304b56, 3 | BRF_GRA },           //  2 Sprite data
	{ "2500-c2.bin",  0x080000, 0x9a0758d6, 3 | BRF_GRA },           //  3

	{ "202-m1.bin",   0x020000, 0x9c0291ea, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "202-v1.bin",   0x080000, 0xdebeb8fb, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(neo2500, neo2500, neogeo)
STD_ROM_FN(neo2500)

struct BurnDriver BurnDrvneo2500 = {
	"neo2500", NULL, "neogeo", NULL, "2004",
	"Neo 2500 Demo\0", NULL, "blastar@gmx.net", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 2, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neo2500RomInfo, neo2500RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo Demo
static struct BurnRomInfo neodemoRomDesc[] = {
	{ "demo-p1.bin",  0x080000, 0xfbfce2a4, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "demo-s1.bin",  0x020000, 0xcd19264f, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "demo-c1.bin",  0x100000, 0x870b28ea, 3 | BRF_GRA },           //  2 Sprite data
	{ "demo-c2.bin",  0x100000, 0x1ecf8128, 3 | BRF_GRA },           //  3

	{ "202-m1.bin",   0x020000, 0x9c0291ea, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "202-v1.bin",   0x080000, 0xdebeb8fb, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(neodemo, neodemo, neogeo)
STD_ROM_FN(neodemo)

struct BurnDriver BurnDrvneodemo = {
	"neodemo", NULL, "neogeo", NULL, "2002",
	"Chaos Demo (Neo Geo)\0", NULL, "Chaos", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 2, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neodemoRomInfo, neodemoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo No Panepon (beta)

static struct BurnRomInfo neonoponRomDesc[] = {
	{ "nnp-p1.rom",   0x080000, 0x8a792271, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "nnp-s1.rom",   0x010000, 0x93e6fe7d, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "nnp-c1.rom",   0x100000, 0x4a718ae3, 3 | BRF_GRA },           //  2 Sprite data
	{ "nnp-c2.rom",   0x100000, 0x0e2cbc25, 3 | BRF_GRA },           //  3

	{ "nnp-m1.rom",   0x010000, 0x7669de64, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "nnp-v1.rom",   0x080000, 0x504bf849, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(neonopon, neonopon, neogeo)
STD_ROM_FN(neonopon)

struct BurnDriver BurnDrvneonopon = {
	"neonopon", NULL, "neogeo", NULL, "????",
	"Neo No Panepon (beta)\0", NULL, "blastar@gmx.net", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, neonoponRomInfo, neonoponRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo Pang by CeL

static struct BurnRomInfo neopangRomDesc[] = {
	{ "pang_p1.rom",  0x080000, 0xdc5922ee, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "pang_s1.rom",  0x010000, 0xd78bd9a0, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "pang_c1.rom",  0x100000, 0x62f5405c, 3 | BRF_GRA },           //  2 Sprite data
	{ "pang_c2.rom",  0x100000, 0xe1183030, 3 | BRF_GRA },           //  3

	{ "pang_m1.rom",  0x020000, 0x9c0291ea, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "pang_v1.rom",  0x080000, 0xdebeb8fb, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(neopang, neopang, neogeo)
STD_ROM_FN(neopang)

struct BurnDriver BurnDrvneopang = {
	"neopang", NULL, "neogeo", NULL, "2010",
	"Neo Pang\0", NULL, "CeL", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_BALLPADDLE, 0,
	NULL, neopangRomInfo, neopangRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo Pong

static struct BurnRomInfo neopongRomDesc[] = {
	{ "pong_p1.rom",  0x020000, 0x9f35e29d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "pong_s1.rom",  0x020000, 0xcd19264f, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "pong_c1.rom",  0x100000, 0xf5a57d1a, 3 | BRF_GRA },           //  2 Sprite data
	{ "pong_c2.rom",  0x100000, 0xaffde64e, 3 | BRF_GRA },           //  3

	{ "pong_m1.rom",  0x020000, 0x78c851cb, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "pong_v1.rom",  0x200000, 0x6c31223c, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(neopong, neopong, neogeo)
STD_ROM_FN(neopong)

struct BurnDriver BurnDrvneopong = {
	"neopong", NULL, "neogeo", NULL, "2002",
	"Neo Pong (ver 1.1)\0", NULL, "Neo Dev Corporation", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_BALLPADDLE, 0,
	NULL, neopongRomInfo, neopongRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo Pong (v1.0)

static struct BurnRomInfo neopongaRomDesc[] = {
	{ "np10_p1.rom",  0x020000, 0x31b724d7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "pong_s1.rom",  0x020000, 0xcd19264f, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "np10_c1.rom",  0x100000, 0xd7587282, 3 | BRF_GRA },           //  2 Sprite data
	{ "np10_c2.rom",  0x100000, 0xfefc9d06, 3 | BRF_GRA },           //  3

	{ "np10_m1.rom",  0x020000, 0x9c0291ea, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "np10_v1.rom",  0x080000, 0xdebeb8fb, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(neoponga, neoponga, neogeo)
STD_ROM_FN(neoponga)

static INT32 neopongaInit()
{
	bDisableNeoWatchdog = true;

	return NeoInit();
}

struct BurnDriver BurnDrvneoponga = {
	"neoponga", "neopong", "neogeo", NULL, "2002",
	"Neo Pong (ver 1.0)\0", NULL, "Neo Dev Corporation", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_BALLPADDLE, 0,
	NULL, neopongaRomInfo, neopongaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	neopongaInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// NGEM2K (beta 2006-01-18)

static struct BurnRomInfo ngem2kRomDesc[] = {
	{ "ngem-p1.bin",  0x080000, 0xf2c42fe3, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "ngem-s1.bin",  0x010000, 0xfb3c02d2, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "ngem-c1.bin",  0x100000, 0xa8e2ab47, 3 | BRF_GRA },           //  2 Sprite data
	{ "ngem-c2.bin",  0x100000, 0xa738ea1c, 3 | BRF_GRA },           //  3

	{ "ngem-m1.bin",  0x020000, 0xb1f44a15, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "ngem-v1.bin",  0x080000, 0xc32199f0, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(ngem2k, ngem2k, neogeo)
STD_ROM_FN(ngem2k)

struct BurnDriver BurnDrvngem2k = {
	"ngem2k", NULL, "neogeo", NULL, "2006",
	"NGEM2K (beta 2006-01-18)\0", NULL, "homebrew", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, ngem2kRomInfo, ngem2kRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo Frog Feast

static struct BurnRomInfo ngfrogRomDesc[] = {
	{ "frg-p1.bin",   0x080000, 0xc26848da, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "frg-s1.bin",   0x020000, 0x88f4cd13, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "frg-c1.bin",   0x100000, 0x7b2f9fb3, 3 | BRF_GRA },           //  2 Sprite data
	{ "frg-c2.bin",   0x100000, 0xa7f18a6f, 3 | BRF_GRA },           //  3
	{ "frg-c3.bin",   0x100000, 0x956bac74, 3 | BRF_GRA },           //  4
	{ "frg-c4.bin",   0x100000, 0x956bac74, 3 | BRF_GRA },           //  5

	{ "frg-m1.bin",   0x020000, 0xc825b484, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code

	{ "frg-v1.bin",   0x100000, 0x2639da11, 5 | BRF_SND },           //  7 Sound data
	{ "frg-v2.bin",   0x100000, 0xe5eda58b, 5 | BRF_SND },           //  8
};

STDROMPICKEXT(ngfrog, ngfrog, neogeo)
STD_ROM_FN(ngfrog)

struct BurnDriver BurnDrvngfrog = {
	"ngfrog", NULL, "neogeo", NULL, "2006",
	"Frog Feast (Neo Geo)\0", NULL, "Rastersoft", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_ACTION, 0,
	NULL, ngfrogRomInfo, ngfrogRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Poker Night

static struct BurnRomInfo poknightRomDesc[] = {
	{ "2003-p1.bin",  0x080000, 0x750421ee, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "2003-s1.bin",  0x020000, 0xa26d2f09, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "2003-c1.bin",  0x080000, 0x5fb2b761, 3 | BRF_GRA },           //  2 Sprite data
	{ "2003-c2.bin",  0x080000, 0xf2b570e3, 3 | BRF_GRA },           //  3

	{ "nnp-m1.rom",   0x010000, 0x7669de64, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "nnp-v1.rom",   0x080000, 0x504bf849, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(poknight, poknight, neogeo)
STD_ROM_FN(poknight)

struct BurnDriver BurnDrvpoknight = {
	"poknight", NULL, "neogeo", NULL, "199?",
	"Poker Night\0", NULL, "Jeff Kurtz", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 1, HARDWARE_SNK_NEOGEO, GBF_CASINO, 0,
	NULL, poknightRomInfo, poknightRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Neo System Check (ver 1.0b)

static struct BurnRomInfo syscheckRomDesc[] = {
	{ "sys-p1.rom",   0x080000, 0xa4adbe1e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "sys-s1.rom",   0x010000, 0x4774f28e, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "sys-c1.rom",   0x080000, 0x75660aac, 3 | BRF_GRA },           //  2 Sprite data
	{ "sys-c2.rom",   0x080000, 0x75660aac, 3 | BRF_GRA },           //  3

	{ "sys-m1.rom",   0x010000, 0x7669de64, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "sys-v1.rom",   0x080000, 0x504bf849, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(syscheck, syscheck, neogeo)
STD_ROM_FN(syscheck)

struct BurnDriver BurnDrvsyscheck = {
	"syscheck", NULL, "neogeo", NULL, "????",
	"Neo System Check (ver 1.0b)\0", NULL, "blastar@gmx.net", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, syscheckRomInfo, syscheckRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Chip n Dale (FMV Demo)

static struct BurnRomInfo cndiRomDesc[] = {
	{ "cnd-p1.bin",   0x800000, 0x2df9a41d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "cnd-s1.bin",   0x010000, 0x63b8b25e, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "cnd-c1.bin",   0x400000, 0x55bd32fc, 3 | BRF_GRA },           //  2 Sprite data
	{ "cnd-c2.bin",   0x400000, 0xbbb43733, 3 | BRF_GRA },           //  3
	{ "cnd-c3.bin",   0x400000, 0x715e0318, 3 | BRF_GRA },           //  4
	{ "cnd-c4.bin",   0x400000, 0x016f3c7e, 3 | BRF_GRA },           //  5
	{ "cnd-c5.bin",   0x400000, 0x28c6cb8c, 3 | BRF_GRA },           //  6
	{ "cnd-c6.bin",   0x400000, 0x9c89503f, 3 | BRF_GRA },           //  7
	{ "cnd-c7.bin",   0x400000, 0x074dd407, 3 | BRF_GRA },           //  8
	{ "cnd-c8.bin",   0x400000, 0x5ddb7db5, 3 | BRF_GRA },           //  9

	{ "cnd-m1.bin",   0x002000, 0xae120a59, 4 | BRF_ESS | BRF_PRG }, //  10 Z80 code

	{ "cnd-v1.bin",   0x092000, 0x91d45537, 5 | BRF_SND },           //  11 Sound data
};

STDROMPICKEXT(cndi, cndi, neogeo)
STD_ROM_FN(cndi)

struct BurnDriver BurnDrvcndi = {
	"cndi", NULL, "neogeo", NULL, "2009",
	"Chip n Dale (FMV Demo)\0", "You must use the Universe BIOS and set region to Japan AES", "Sergi", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, cndiRomInfo, cndiRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeouniDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// DarkWing Duck (FMV Demo)

static struct BurnRomInfo dwiRomDesc[] = {
	{ "dwi-p1.bin",   0x800000, 0x8e3f2ea8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "dwi-s1.bin",   0x010000, 0x3dc5c1ef, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "dwi-c1.bin",   0x400000, 0x2b8bda94, 3 | BRF_GRA },           //  2 Sprite data
	{ "dwi-c2.bin",   0x400000, 0x5345e4de, 3 | BRF_GRA },           //  3
	{ "dwi-c3.bin",   0x400000, 0xc6185f14, 3 | BRF_GRA },           //  4
	{ "dwi-c4.bin",   0x400000, 0xc9e59701, 3 | BRF_GRA },           //  5
	{ "dwi-c5.bin",   0x400000, 0x42dd1c90, 3 | BRF_GRA },           //  6
	{ "dwi-c6.bin",   0x400000, 0x9fa6d651, 3 | BRF_GRA },           //  7
	{ "dwi-c7.bin",   0x400000, 0x5cfa6886, 3 | BRF_GRA },           //  8
	{ "dwi-c8.bin",   0x400000, 0x59c37871, 3 | BRF_GRA },           //  9

	{ "dwi-m1.bin",   0x002000, 0x2249e065, 4 | BRF_ESS | BRF_PRG }, //  10 Z80 code

	{ "dwi-v1.bin",   0x09b000, 0x13bb4ce4, 5 | BRF_SND },           //  11 Sound data
};

STDROMPICKEXT(dwi, dwi, neogeo)
STD_ROM_FN(dwi)

struct BurnDriver BurnDrvdwi = {
	"dwi", NULL, "neogeo", NULL, "2009",
	"DarkWing Duck (FMV Demo)\0", "You must use the Universe BIOS and set region to Japan AES", "Sergi", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, dwiRomInfo, dwiRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeouniDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// DarkWing Duck (FMV Demo, alt)

static struct BurnRomInfo dwiaRomDesc[] = {
	{ "dwi-p1.bin",   0x800000, 0x8e3f2ea8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "dwi-s1a.bin",  0x010000, 0x63b8b25e, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "dwi-c1.bin",   0x400000, 0x2b8bda94, 3 | BRF_GRA },           //  2 Sprite data
	{ "dwi-c2.bin",   0x400000, 0x5345e4de, 3 | BRF_GRA },           //  3
	{ "dwi-c3.bin",   0x400000, 0xc6185f14, 3 | BRF_GRA },           //  4
	{ "dwi-c4.bin",   0x400000, 0xc9e59701, 3 | BRF_GRA },           //  5
	{ "dwi-c5.bin",   0x400000, 0x42dd1c90, 3 | BRF_GRA },           //  6
	{ "dwi-c6.bin",   0x400000, 0x9fa6d651, 3 | BRF_GRA },           //  7
	{ "dwi-c7.bin",   0x400000, 0x5cfa6886, 3 | BRF_GRA },           //  8
	{ "dwi-c8.bin",   0x400000, 0x59c37871, 3 | BRF_GRA },           //  9

	{ "dwi-m1a.bin",  0x002000, 0xb9f8df96, 4 | BRF_ESS | BRF_PRG }, //  10 Z80 code

	{ "dwi-v1.bin",   0x09b000, 0x13bb4ce4, 5 | BRF_SND },           //  11 Sound data
};

STDROMPICKEXT(dwia, dwia, neogeo)
STD_ROM_FN(dwia)

struct BurnDriver BurnDrvdwia = {
	"dwia", "dwi", "neogeo", NULL, "2009",
	"DarkWing Duck (FMV Demo, alt)\0", "You must use the Universe BIOS and set region to Japan AES", "Sergi", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, dwiaRomInfo, dwiaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeouniDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Duck Tales (FMV Demo)

static struct BurnRomInfo duckiRomDesc[] = {
	{ "duck-p1.bin",  0x800000, 0xf975711c, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "duck-s1.bin",  0x010000, 0x63b8b25e, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "duck-c1.bin",  0x400000, 0x02adabba, 3 | BRF_GRA },           //  2 Sprite data
	{ "duck-c2.bin",  0x400000, 0x00bd87ac, 3 | BRF_GRA },           //  3
	{ "duck-c3.bin",  0x400000, 0x263307c8, 3 | BRF_GRA },           //  4
	{ "duck-c4.bin",  0x400000, 0xb8330c1e, 3 | BRF_GRA },           //  5
	{ "duck-c5.bin",  0x400000, 0xce99fcb8, 3 | BRF_GRA },           //  6
	{ "duck-c6.bin",  0x400000, 0xa6d30781, 3 | BRF_GRA },           //  7
	{ "duck-c7.bin",  0x400000, 0xeda15323, 3 | BRF_GRA },           //  8
	{ "duck-c8.bin",  0x400000, 0x400fd4cd, 3 | BRF_GRA },           //  9

	{ "duck-m1.bin",  0x002000, 0x7761d993, 4 | BRF_ESS | BRF_PRG }, //  10 Z80 code

	{ "duck-v1.bin",  0x090200, 0x24b716ee, 5 | BRF_SND },           //  11 Sound data
};

STDROMPICKEXT(ducki, ducki, neogeo)
STD_ROM_FN(ducki)

struct BurnDriver BurnDrvducki = {
	"ducki", NULL, "neogeo", NULL, "2009",
	"Duck Tales (FMV Demo)\0", "You must use the Universe BIOS and set region to Japan AES", "Sergi", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, duckiRomInfo, duckiRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeouniDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Ghost Busters (FMV Demo)

static struct BurnRomInfo ghostbiRomDesc[] = {
	{ "ghostb-p1.bin",  0x100000, 0xaf69ef5f, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "ghostb-p2.bin",  0x400000, 0xf57e393f, 1 | BRF_ESS | BRF_PRG }, //  1 68K code

	{ "ghostb-s1.bin",  0x010000, 0x63b8b25e, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "ghostb-c1.bin",  0x400000, 0x4281fecd, 3 | BRF_GRA },           //  3 Sprite data
	{ "ghostb-c2.bin",  0x400000, 0x0f07018c, 3 | BRF_GRA },           //  4
	{ "ghostb-c3.bin",  0x400000, 0xa4589b2a, 3 | BRF_GRA },           //  5
	{ "ghostb-c4.bin",  0x400000, 0x2a72bd04, 3 | BRF_GRA },           //  6
	{ "ghostb-c5.bin",  0x400000, 0xad3b72f9, 3 | BRF_GRA },           //  7
	{ "ghostb-c6.bin",  0x400000, 0x963e0941, 3 | BRF_GRA },           //  8

	{ "ghostb-m1.bin",  0x002000, 0x6624e642, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "ghostb-v1.bin",  0x094a00, 0x44864761, 5 | BRF_SND },           //  10 Sound data
};

STDROMPICKEXT(ghostbi, ghostbi, neogeo)
STD_ROM_FN(ghostbi)

struct BurnDriver BurnDrvghostbi = {
	"ghostbi", NULL, "neogeo", NULL, "2009",
	"Ghost Busters (FMV Demo)\0", "You must use the Universe BIOS and set region to Japan AES", "Sergi", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, ghostbiRomInfo, ghostbiRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeouniDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Robocop (FMV Demo)

static struct BurnRomInfo robocopiRomDesc[] = {
	{ "robo-p1.bin",  0x800000, 0xadcb4fe0, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "robo-s1.bin",  0x010000, 0x63b8b25e, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "robo-c1.bin",  0x400000, 0x8cf565a0, 3 | BRF_GRA },           //  2 Sprite data
	{ "robo-c2.bin",  0x400000, 0x76ef561d, 3 | BRF_GRA },           //  3
	{ "robo-c3.bin",  0x400000, 0x7166790c, 3 | BRF_GRA },           //  4
	{ "robo-c4.bin",  0x400000, 0xc0cdcbfc, 3 | BRF_GRA },           //  5
	{ "robo-c5.bin",  0x400000, 0xa5ddc363, 3 | BRF_GRA },           //  6
	{ "robo-c6.bin",  0x400000, 0x1fcc8fce, 3 | BRF_GRA },           //  7
	{ "robo-c7.bin",  0x400000, 0xbe9f9da2, 3 | BRF_GRA },           //  8
	{ "robo-c8.bin",  0x400000, 0x7cff633a, 3 | BRF_GRA },           //  9

	{ "robo-m1.bin",  0x002000, 0xc4d35e83, 4 | BRF_ESS | BRF_PRG }, //  10 Z80 code

	{ "robo-v1.bin",  0x096000, 0xecb284a9, 5 | BRF_SND },           //  11 Sound data
};

STDROMPICKEXT(robocopi, robocopi, neogeo)
STD_ROM_FN(robocopi)

struct BurnDriver BurnDrvrobocopi = {
	"robocopi", NULL, "neogeo", NULL, "2009",
	"Robocop (FMV Demo)\0", "You must use the Universe BIOS and set region to Japan AES", "Sergi", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, robocopiRomInfo, robocopiRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeouniDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Spiderman (FMV Demo)

static struct BurnRomInfo spidermiRomDesc[] = {
	{ "sman-p1.bin",  0x100000, 0x66b5cfe8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "sman-p2.bin",  0x400000, 0x72082aea, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "sman-s1.bin",  0x010000, 0x63b8b25e, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "sman-c1.bin",  0x400000, 0x8e5ccfdd, 3 | BRF_GRA },           //  3 Sprite data
	{ "sman-c2.bin",  0x400000, 0xa6406f5b, 3 | BRF_GRA },           //  4
	{ "sman-c3.bin",  0x400000, 0xadfff926, 3 | BRF_GRA },           //  5
	{ "sman-c4.bin",  0x400000, 0x068c307f, 3 | BRF_GRA },           //  6
	{ "sman-c5.bin",  0x400000, 0x9a5cf7a6, 3 | BRF_GRA },           //  7
	{ "sman-c6.bin",  0x400000, 0x934f03c0, 3 | BRF_GRA },           //  8

	{ "sman-m1.bin",  0x002000, 0x6a02cb1d, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "sman-v1.bin",  0x096100, 0x95c138da, 5 | BRF_SND },           //  10 Sound data
};

STDROMPICKEXT(spidermi, spidermi, neogeo)
STD_ROM_FN(spidermi)

struct BurnDriver BurnDrvspidermi = {
	"spidermi", NULL, "neogeo", NULL, "2009",
	"Spiderman (FMV Demo)\0", "You must use the Universe BIOS and set region to Japan AES", "Sergi", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, spidermiRomInfo, spidermiRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeouniDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Teenage Mutant Ninja Turtles (FMV Demo)

static struct BurnRomInfo tmntiRomDesc[] = {
	{ "tmnt-p1.bin",  0x800000, 0x2d5066ff, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "tmnt-s1.bin",  0x010000, 0x3dc5c1ef, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "tmnt-c1.bin",  0x400000, 0xae1735c1, 3 | BRF_GRA },           //  2 Sprite data
	{ "tmnt-c2.bin",  0x400000, 0x344281b5, 3 | BRF_GRA },           //  3
	{ "tmnt-c3.bin",  0x400000, 0x1058a648, 3 | BRF_GRA },           //  4
	{ "tmnt-c4.bin",  0x400000, 0x1c6ba1b0, 3 | BRF_GRA },           //  5
	{ "tmnt-c5.bin",  0x400000, 0x73a89fb0, 3 | BRF_GRA },           //  6
	{ "tmnt-c6.bin",  0x400000, 0xa139246a, 3 | BRF_GRA },           //  7
	{ "tmnt-c7.bin",  0x400000, 0xed957842, 3 | BRF_GRA },           //  8
	{ "tmnt-c8.bin",  0x400000, 0x8c15d91b, 3 | BRF_GRA },           //  9

	{ "tmnt-m1.bin",  0x002000, 0x953542ef, 4 | BRF_ESS | BRF_PRG }, //  10 Z80 code

	{ "tmnt-v1.bin",  0x094900, 0x273640c3, 5 | BRF_SND },           //  11 Sound data
};

STDROMPICKEXT(tmnti, tmnti, neogeo)
STD_ROM_FN(tmnti)

struct BurnDriver BurnDrvtmnti = {
	"tmnti", NULL, "neogeo", NULL, "2009",
	"Teenage Mutant Ninja Turtles (FMV Demo)\0", "You must use the Universe BIOS and set region to Japan AES", "Sergi", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, tmntiRomInfo, tmntiRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeouniDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Teenage Mutant Ninja Turtles (FMV Demo, alt)

static struct BurnRomInfo tmntiaRomDesc[] = {
	{ "tmnt-p1a.bin", 0x800000, 0xed4f5fec, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "tmnt-s1.bin",  0x010000, 0x3dc5c1ef, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "tmnt-c1.bin",  0x400000, 0xae1735c1, 3 | BRF_GRA },           //  2 Sprite data
	{ "tmnt-c2.bin",  0x400000, 0x344281b5, 3 | BRF_GRA },           //  3
	{ "tmnt-c3.bin",  0x400000, 0x1058a648, 3 | BRF_GRA },           //  4
	{ "tmnt-c4.bin",  0x400000, 0x1c6ba1b0, 3 | BRF_GRA },           //  5
	{ "tmnt-c5.bin",  0x400000, 0x73a89fb0, 3 | BRF_GRA },           //  6
	{ "tmnt-c6.bin",  0x400000, 0xa139246a, 3 | BRF_GRA },           //  7
	{ "tmnt-c7.bin",  0x400000, 0xed957842, 3 | BRF_GRA },           //  8
	{ "tmnt-c8.bin",  0x400000, 0x8c15d91b, 3 | BRF_GRA },           //  9

	{ "tmnt-m1.bin",  0x002000, 0x953542ef, 4 | BRF_ESS | BRF_PRG }, //  10 Z80 code

	{ "tmnt-v1.bin",  0x094900, 0x273640c3, 5 | BRF_SND },           //  11 Sound data
};

STDROMPICKEXT(tmntia, tmntia, neogeo)
STD_ROM_FN(tmntia)

struct BurnDriver BurnDrvtmntia = {
	"tmntia", "tmnti", "neogeo", NULL, "2009",
	"Teenage Mutant Ninja Turtles (FMV Demo, alt)\0", "You must use the Universe BIOS and set region to Japan AES", "Sergi", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, tmntiaRomInfo, tmntiaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeouniDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Neo Thunder

static struct BurnRomInfo neothndrRomDesc[] = {
	{ "thund_p1.rom", 0x100000, 0xF7050757, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "thund_s1.rom", 0x020000, 0xA545B593, 2 | BRF_GRA },				//  1 Text data

	{ "thund_c1.rom", 0x100000, 0x48B9126E, 3 | BRF_GRA },				//  2 Sprite data
	{ "thund_c2.rom", 0x100000, 0x214DF62E, 3 | BRF_GRA },				//  3

	{ "thund_m1.rom", 0x020000, 0x9C0291EA, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "thund_v1.rom", 0x080000, 0xDEBEB8FB, 5 | BRF_SND },				//  5 Sound data
};

STDROMPICKEXT(neothndr, neothndr, neogeo)
STD_ROM_FN(neothndr)

struct BurnDriver BurnDrvneothndr = {
	"neothndr", NULL, "neogeo", NULL, "2012",
	"Neo Thunder\0", "No Sound", "Sebastian Mihai", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 1, HARDWARE_SNK_NEOGEO, GBF_HORSHOOT, 0,
	NULL, neothndrRomInfo, neothndrRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// NGF Transparency Demo

static struct BurnRomInfo ngftdemoRomDesc[] = {
	{ "ngftd_p1.rom", 0x080000, 0x84d87190, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "ngftd_s1.rom", 0x020000, 0xa545b593, 2 | BRF_GRA },				//  1 Text data

	{ "ngftd_c1.rom", 0x200000, 0xb2fba994, 3 | BRF_GRA },				//  2 Sprite data
	{ "ngftd_c2.rom", 0x200000, 0x37495ab2, 3 | BRF_GRA },				//  3

	{ "ngftd_m1.rom", 0x020000, 0x5ea216be, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "ngftd_v1.rom", 0x200000, 0xd03c87eb, 5 | BRF_SND },				//  5 Sound data
};

STDROMPICKEXT(ngftdemo, ngftdemo, neogeo)
STD_ROM_FN(ngftdemo)

struct BurnDriver BurnDrvngftdemo = {
	"ngftdemo", NULL, "neogeo", NULL, "2012",
	"NGF Transparency Demo\0", "redarmor.net", "CeL", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, ngftdemoRomInfo, ngftdemoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Neo CastleVania Demo

static struct BurnRomInfo neocstlvRomDesc[] = {
	{ "cstlv_p1.rom", 0x080000, 0x8d6cd9f2, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "cstlv_s1.rom", 0x040000, 0xbe950195, 2 | BRF_GRA },				//  1 Text data

	{ "cstlv_c1.rom", 0x100000, 0x960a7414, 3 | BRF_GRA },				//  2 Sprite data
	{ "cstlv_c2.rom", 0x100000, 0x24a961d0, 3 | BRF_GRA },				//  3

	{ "cstlv_m1.rom", 0x020000, 0x89c71460, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "cstlv_v1.rom", 0x080000, 0xdebeb8fb, 5 | BRF_SND },				//  5 Sound data
};

STDROMPICKEXT(neocstlv, neocstlv, neogeo)
STD_ROM_FN(neocstlv)

struct BurnDriver BurnDrvneocstlv = {
	"neocstlv", NULL, "neogeo", NULL, "2004",
	"Neo CastleVania Demo\0", NULL, "Barf/BarfHappy", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neocstlvRomInfo, neocstlvRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// NeoGeo 3D! Demo

static struct BurnRomInfo neo3ddmoRomDesc[] = {
	{ "neo3d_p1.rom", 0x080000, 0x791f6042, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "neo3d_s1.rom", 0x020000, 0xcd19264f, 2 | BRF_GRA },				//  1 Text data

	{ "neo3d_c1.rom", 0x100000, 0xa7eaca76, 3 | BRF_GRA },				//  2 Sprite data
	{ "neo3d_c2.rom", 0x100000, 0x042f2cde, 3 | BRF_GRA },				//  3

	{ "neo3d_m1.rom", 0x020000, 0x7e74cc1f, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "neo3d_v1.rom", 0x080000, 0xdebeb8fb, 5 | BRF_SND },				//  5 Sound data
};

STDROMPICKEXT(neo3ddmo, neo3ddmo, neogeo)
STD_ROM_FN(neo3ddmo)

struct BurnDriver BurnDrvneo3ddmo = {
	"neo3ddmo", NULL, "neogeo", NULL, "2012",
	"NeoGeo 3D! Demo\0", NULL, "Oxygene", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neo3ddmoRomInfo, neo3ddmoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// WW2 (Demo) - Arcade Development Project

static struct BurnRomInfo neoww2RomDesc[] = {
	{ "ww2_p1.rom",   0x080000, 0xd53762ff, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "ww2_s1.rom",   0x010000, 0x3fda5d1a, 2 | BRF_GRA },				//  1 Text data

	{ "ww2_c1.rom",   0x100000, 0x955efd3e, 3 | BRF_GRA },				//  2 Sprite data
	{ "ww2_c2.rom",   0x100000, 0xc213940f, 3 | BRF_GRA },				//  3

	{ "ww2_m1.rom",   0x020000, 0x99c2354e, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "ww2_v1.rom",   0x080000, 0xdebeb8fb, 5 | BRF_SND },				//  5 Sound data
};

STDROMPICKEXT(neoww2, neoww2, neogeo)
STD_ROM_FN(neoww2)

struct BurnDriver BurnDrvneoww2 = {
	"neoww2", NULL, "neogeo", NULL, "2012",
	"WW2 (Demo) - Arcade Development Project\0", NULL, "Charles DOTY/RasterSoft (USA)", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neoww2RomInfo, neoww2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Time's UP! 

static struct BurnRomInfo timesupRomDesc[] = {
	{ "tup_p1.rom",   0x100000, 0xb4be3ede, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "tup_s1.rom",   0x020000, 0xa545b593, 2 | BRF_GRA },				//  1 Text data

	{ "tup_c1.rom",   0x200000, 0x1c83fc38, 3 | BRF_GRA },				//  2 Sprite data
	{ "tup_c2.rom",   0x200000, 0x2b3f48b4, 3 | BRF_GRA },				//  3

	{ "tup_m1.rom",   0x020000, 0x8b83308b, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "tup_v1.rom",   0x200000, 0x1b48708b, 5 | BRF_SND },				//  5 Sound data
	{ "tup_v2.rom",   0x200000, 0x6b17df84, 5 | BRF_SND },				//  6
	{ "tup_v3.rom",   0x200000, 0x6c798d46, 5 | BRF_SND },				//  7 
};

STDROMPICKEXT(timesup, timesup, neogeo)
STD_ROM_FN(timesup)

struct BurnDriver BurnDrvtimesup = {
	"timesup", NULL, "neogeo", NULL, "2012",
	"Time's UP!\0", NULL, "NGF Dev. Inc.", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_VERSHOOT, 0,
	NULL, timesupRomInfo, timesupRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Time's UP! (Demo)

static struct BurnRomInfo timesupdRomDesc[] = {
	{ "tup_p1.rom",   0x200000, 0xbe86adb1, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "tup_s1.rom",   0x020000, 0xa545b593, 2 | BRF_GRA },				//  1 Text data

	{ "tup_c1.rom",   0x200000, 0xc19a300a, 3 | BRF_GRA },				//  2 Sprite data
	{ "tup_c2.rom",   0x200000, 0xfdb3f7ed, 3 | BRF_GRA },				//  3

	{ "tup_m1.rom",   0x020000, 0xfe795d11, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "tup_v1.rom",   0x400000, 0x13b8f47b, 5 | BRF_SND },				//  5 Sound data
	{ "tup_v2.rom",   0x400000, 0x6fdd663d, 5 | BRF_SND },				//  6 Sound data
};

STDROMPICKEXT(timesupd, timesupd, neogeo)
STD_ROM_FN(timesupd)

struct BurnDriver BurnDrvtimesupd = {
	"timesupd", NULL, "neogeo", NULL, "2012",
	"Time's UP! (Demo)\0", NULL, "NGF Dev. Inc.", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_VERSHOOT, 0,
	NULL, timesupdRomInfo, timesupdRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Galaga Demo (set 1)

static struct BurnRomInfo neogalagRomDesc[] = {
	{ "gal_p1.rom",   0x100000, 0x7226db5c, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "gal_s1.rom",   0x002000, 0x90557449, 2 | BRF_GRA },				//  1 Text data

	{ "gal_c1.rom",   0x100000, 0x6de8c6f6, 3 | BRF_GRA },				//  2 Sprite data
	{ "gal_c2.rom",   0x100000, 0xfa5f0c1d, 3 | BRF_GRA },				//  3

	{ "gal_m1.rom",   0x020000, 0x9c0291ea, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "gal_v1.rom",   0x080000, 0xdebeb8fb, 5 | BRF_SND },				//  5 Sound data
};

STDROMPICKEXT(neogalag, neogalag, neogeo)
STD_ROM_FN(neogalag)

struct BurnDriver BurnDrvneogalag = {
	"neogalag", NULL, "neogeo", NULL, "2013",
	"Galaga Demo (set 1)\0", NULL, "Cristiano Bei/www.iocerom.com", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neogalagRomInfo, neogalagRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Galaga Demo (set 2)

static struct BurnRomInfo neogalagaRomDesc[] = {
	{ "gal_p1a.rom",  0x100000, 0x01dc85fd, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "gal_s1.rom",   0x002000, 0x90557449, 2 | BRF_GRA },				//  1 Text data

	{ "gal_c1a.rom",  0x100000, 0xf8221e34, 3 | BRF_GRA },				//  2 Sprite data
	{ "gal_c2a.rom",  0x100000, 0xc30cf1fc, 3 | BRF_GRA },				//  3

	{ "gal_m1.rom",   0x020000, 0x9c0291ea, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "gal_v1.rom",   0x080000, 0xdebeb8fb, 5 | BRF_SND },				//  5 Sound data
};

STDROMPICKEXT(neogalaga, neogalaga, neogeo)
STD_ROM_FN(neogalaga)

struct BurnDriver BurnDrvneogalaga = {
	"neogalaga", "neogalag", "neogeo", NULL, "2013",
	"Galaga Demo (set 2)\0", NULL, "Cristiano Bei/www.iocerom.com", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HOMEBREW | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neogalagaRomInfo, neogalagaRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// NeoGeo 2-Player Tetris

static struct BurnRomInfo neotetRomDesc[] = {
	{ "tet_p1.rom",   0x080000, 0x3465569a, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "tet_s1.rom",   0x020000, 0xa545b593, 2 | BRF_GRA },				//  1 Text data

	{ "tet_c1.rom",   0x080000, 0xacc6d1d4, 3 | BRF_GRA },				//  2 Sprite data
	{ "tet_c2.rom",   0x080000, 0x7ec06ab5, 3 | BRF_GRA },				//  3

	{ "tet_m1.rom",   0x020000, 0x6b3703c6, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "tet_v1.rom",   0x080000, 0x2be8e290, 5 | BRF_SND },				//  5 Sound data
};

STDROMPICKEXT(neotet, neotet, neogeo)
STD_ROM_FN(neotet)

struct BurnDriver BurnDrvneotet = {
	"neotet", NULL, "neogeo", NULL, "2013",
	"NeoGeo 2-Player Tetris\0", NULL, "Crim/Stephen", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, neotetRomInfo, neotetRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Primo Demo

static struct BurnRomInfo neoprimoRomDesc[] = {
	{ "pri_p1.rom",   0x100000, 0xa315fb81, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code

	{ "pri_s1.rom",   0x020000, 0xa545b593, 2 | BRF_GRA },				//  1 Text data

	{ "pri_c1.rom",   0x100000, 0xd68941e1, 3 | BRF_GRA },				//  2 Sprite data
	{ "pri_c2.rom",   0x100000, 0xdf7e359f, 3 | BRF_GRA },				//  3

	{ "pri_m1.rom",   0x020000, 0x9c0291ea, 4 | BRF_ESS | BRF_PRG },	//  4 Z80 code

	{ "pri_v1.rom",   0x080000, 0xdebeb8fb, 5 | BRF_SND },				//  5 Sound data
};

STDROMPICKEXT(neoprimo, neoprimo, neogeo)
STD_ROM_FN(neoprimo)

struct BurnDriver BurnDrvneoprimo = {
	"neoprimo", NULL, "neogeo", NULL, "2013",
	"Primo Demo\0", NULL, "iocerom.com", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, neoprimoRomInfo, neoprimoRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Crouching Poney Hidden Dragon (DEMO)

static struct BurnRomInfo cphdRomDesc[] = {
	{ "cphd.p1",  	  0x100000, 0xde032a95, 1 | BRF_ESS | BRF_PRG },	//  0 68K Code
	{ "cphd.p2",  	  0x800000, 0x7a3a2e41, 1 | BRF_ESS | BRF_PRG },	//  1

	{ "cphd.s1",   	  0x020000, 0x1736099a, 2 | BRF_GRA },				//  2 Text data

	{ "cphd.c1",  	 0x2000000, 0xf3a4b475, 3 | BRF_GRA },				//  3 Sprite data
	{ "cphd.c2",     0x2000000, 0x05ce93a9, 3 | BRF_GRA },				//  4
	{ "cphd.c3",     0x2000000, 0x59450445, 0 | BRF_GRA | BRF_OPT }, 	//  5
	{ "cphd.c4",     0x2000000, 0x59450445, 0 | BRF_GRA | BRF_OPT }, 	//  6

	{ "cphd.m1",   	  0x010000, 0x535a7397, 4 | BRF_ESS | BRF_PRG },	//  7 Z80 code

	{ "cphd.v1",     0x2000000, 0x04eb9c07, 5 | BRF_SND },				//  8 Sound data
	{ "cphd.v2",     0x2000000, 0x9c9aec7f, 5 | BRF_SND },				//  8 Sound data

};

STDROMPICKEXT(cphd, cphd, neogeo)
STD_ROM_FN(cphd)

static INT32 CphdInit()
{
	cphdmode = 1;

	return NeoInit();
}

struct BurnDriver BurnDrvcphd = {
	"cphd", NULL, "neogeo", NULL, "2013",
	"Crouching Poney Hidden Dragon (DEMO)\0", NULL, "Le Cortex", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_ACTION, 0,
	NULL, cphdRomInfo, cphdRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	CphdInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Bad Apple (Tech-Demo)

static struct BurnRomInfo badappleRomDesc[] = {
	{ "bada-p1.bin",   0x0400000, 0xf68ade5a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "bada-s1.bin",   0x0020000, 0xbc7ebee7, 2 | BRF_GRA },           //  1 Text data

	{ "bada-c1.bin",   0x3100000, 0x88ddc443, 3 | BRF_GRA },           //  2 Sprite data
	{ "bada-c2.bin",   0x3100000, 0xb64face4, 3 | BRF_GRA },           //  3

	{ "bada-m1.bin",   0x0010000, 0xc11e8874, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "bada-v1.bin",   0x0500000, 0x3b741894, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(badapple, badapple, neogeo)
STD_ROM_FN(badapple)

struct BurnDriver BurnDrvbadapple = {
	"badapple", NULL, "neogeo", NULL, "2017",
	"Bad Apple (Tech-Demo)\0", NULL, "Hpman", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW | BDF_DEMO, 1, HARDWARE_SNK_NEOGEO, GBF_MISC, 0,
	NULL, badappleRomInfo, badappleRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};


// Project Neon (Caravan Edition, prealpha 0.4.19)

static struct BurnRomInfo proneonRomDesc[] = {
	{ "neon-p1.bin",    0x0080000, 0xf45cac3a, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "neon-s1.bin",    0x0020000, 0xf32de862, 2 | BRF_GRA },           //  1 Text data

	{ "neon-c1.bin",    0x0080000, 0x87df580b, 3 | BRF_GRA },           //  2 Sprite data
	{ "neon-c2.bin",    0x0080000, 0x7b854cc1, 3 | BRF_GRA },           //  3

	{ "neon-m1.bin",    0x0010000, 0x6d24cf93, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "neon-v1.bin",    0x0100000, 0x2823ca49, 5 | BRF_SND },           //  5 Sound data
};

STDROMPICKEXT(proneon, proneon, neogeo)
STD_ROM_FN(proneon)

struct BurnDriver BurnDrvproneon = {
	"proneon", NULL, "neogeo", NULL, "2019",
	"Project Neon (Caravan Edition, prealpha v0.4.19)\0", NULL, "FULLSET", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_SNK_NEOGEO, GBF_VERSHOOT, 0,
	NULL, proneonRomInfo, proneonRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 224, 304, 3, 4
};


// NeoTRIS (Free beta 2 Ver. 202009)
// https://www.chipsonsteroids.com/
// 3 & 4 players not working (needs Multitap)

static struct BurnRomInfo neotrisRomDesc[] = {
	{ "neotris-p1.bin",    0x080000, 0x4cd619cf, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "neotris-s1.bin",    0x020000, 0x6809043a, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "neotris-c1.bin",    0x400000, 0xe1c8eca9, 3 | BRF_GRA },           //  2 Sprite data
	{ "neotris-c2.bin",    0x400000, 0x11ca6e64, 3 | BRF_GRA },           //  3
	{ "neotris-c3.bin",    0x400000, 0x1d9e2046, 3 | BRF_GRA },           //  4
	{ "neotris-c4.bin",    0x400000, 0x9091e795, 3 | BRF_GRA },           //  5
	{ "neotris-c5.bin",    0x400000, 0xbf278afe, 3 | BRF_GRA },           //  6
	{ "neotris-c6.bin",    0x400000, 0x8eb17e24, 3 | BRF_GRA },           //  7

	{ "neotris-m1.bin",    0x010000, 0x5a63bb9d, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	{ "neotris-v1.bin",    0x400000, 0xfef16eb4, 5 | BRF_SND },           //  9 Sound data
	{ "neotris-v2.bin",    0x400000, 0xf0d28e19, 5 | BRF_SND },           // 10
	{ "neotris-v3.bin",    0x400000, 0xfc652c8b, 5 | BRF_SND },           // 11
	{ "neotris-v4.bin",    0x400000, 0xc25764ca, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(neotris, neotris, neogeo)
STD_ROM_FN(neotris)

struct BurnDriver BurnDrvneotris = {
	"neotris", NULL, "neogeo", NULL, "2020",
	"NeoTRIS (Free Beta 2, Ver. 202009)\0", NULL, "Chipsonsteroids", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 4, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, neotrisRomInfo, neotrisRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};


// Neo Super Mario Bros Demo 0.01!
// https://ozzyouzo.itch.io/neosmb

static struct BurnRomInfo nsmbRomDesc[] = {
	{ "nsmb-p1.bin",    0x100000, 0x9e0fded1, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "nsmb-s1.bin",    0x020000, 0xa545b593, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "nsmb-c1.bin",    0x800000, 0xf187934c, 3 | BRF_GRA },           //  2 Sprite data
	{ "nsmb-c2.bin",    0x800000, 0x7ecfc8bb, 3 | BRF_GRA },           //  3
#if 0
	{ "nsmb-c3.bin",    0x800000, 0xf187934c, 3 | BRF_GRA },           //  Duplicate Sprite data files
	{ "nsmb-c4.bin",    0x800000, 0x7ecfc8bb, 3 | BRF_GRA },           //
	{ "nsmb-c5.bin",    0x800000, 0xf187934c, 3 | BRF_GRA },           //
	{ "nsmb-c6.bin",    0x800000, 0x7ecfc8bb, 3 | BRF_GRA },           //
	{ "nsmb-c7.bin",    0x800000, 0xf187934c, 3 | BRF_GRA },           //
	{ "nsmb-c8.bin",    0x800000, 0x7ecfc8bb, 3 | BRF_GRA },           //
#endif

	{ "nsmb-m1.bin",    0x010000, 0x3c99e181, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "nsmb-v1.bin",    0x100000, 0xd1e4fc94, 5 | BRF_SND },           //  5 Sound data
	{ "nsmb-v2.bin",    0x100000, 0x7d42fab3, 5 | BRF_SND },           //  6
	{ "nsmb-v3.bin",    0x100000, 0x806b797d, 5 | BRF_SND },           //  7
	{ "nsmb-v4.bin",    0x100000, 0xfbf00c96, 5 | BRF_SND },           //  8
	{ "nsmb-v5.bin",    0x100000, 0x9b2031d4, 5 | BRF_SND },           //  9
	{ "nsmb-v6.bin",    0x100000, 0x107cfc89, 5 | BRF_SND },           // 10
	{ "nsmb-v7.bin",    0x100000, 0x0703b761, 5 | BRF_SND },           // 11
	{ "nsmb-v8.bin",    0x100000, 0x8d525588, 5 | BRF_SND },           // 12
	{ "nsmb-v9.bin",    0x100000, 0x97f073b6, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(nsmb, nsmb, neogeo)
STD_ROM_FN(nsmb)

struct BurnDriver BurnDrvNsmb = {
	"nsmb", NULL, "neogeo", NULL, "2020",
	"Neo Super Mario Bros (Demo, v0.01)\0", NULL, "OzzyOuzo", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 1, HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, nsmbRomInfo, nsmbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};


// Neo Black Tiger (Demo)
// https://ozzyouzo.itch.io/neo-blktgr

static struct BurnRomInfo nblktigerRomDesc[] = {
	{ "nblktiegr-p1.bin",    0x100000, 0x18f34200, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "nblktiger-s1.bin",    0x020000, 0xa545b593, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "nblktiger-c1.bin",    0x800000, 0xaa469494, 3 | BRF_GRA },           //  2 Sprite data
	{ "nblktiger-c2.bin",    0x800000, 0xfa07ba1e, 3 | BRF_GRA },           //  3
#if 0
	{ "nblktiger-c3.bin",    0x800000, 0xaa469494, 3 | BRF_GRA },           //  Duplicate Sprite data files
	{ "nblktiger-c4.bin",    0x800000, 0xfa07ba1e, 3 | BRF_GRA },           //
	{ "nblktiger-c5.bin",    0x800000, 0xaa469494, 3 | BRF_GRA },           //  
	{ "nblktiger-c6.bin",    0x800000, 0xfa07ba1e, 3 | BRF_GRA },           //
	{ "nblktiger-c7.bin",    0x800000, 0xaa469494, 3 | BRF_GRA },           //  
	{ "nblktiger-c8.bin",    0x800000, 0xfa07ba1e, 3 | BRF_GRA },           //
#endif

	{ "nblktiger-m1.bin",    0x010000, 0x2037dc19, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	{ "nblktiger-v1.bin",    0x100000, 0x3ac066e1, 5 | BRF_SND },           //  5 Sound data
	{ "nblktiger-v2.bin",    0x100000, 0xc5a12987, 5 | BRF_SND },           //  6
	{ "nblktiger-v3.bin",    0x100000, 0x3c623679, 5 | BRF_SND },           //  7
	{ "nblktiger-v4.bin",    0x100000, 0xfbf00c96, 5 | BRF_SND },           //  8
	{ "nblktiger-v5.bin",    0x100000, 0x9b2031d4, 5 | BRF_SND },           //  9
	{ "nblktiger-v6.bin",    0x100000, 0x107cfc89, 5 | BRF_SND },           // 10
	{ "nblktiger-v7.bin",    0x100000, 0x0703b761, 5 | BRF_SND },           // 11
	{ "nblktiger-v8.bin",    0x100000, 0x8d525588, 5 | BRF_SND },           // 12
	{ "nblktiger-v9.bin",    0x100000, 0x97f073b6, 5 | BRF_SND },           // 13
};

STDROMPICKEXT(nblktiger, nblktiger, neogeo)
STD_ROM_FN(nblktiger)

struct BurnDriver BurnDrvNblktiger = {
	"nblktiger", NULL, "neogeo", NULL, "2020",
	"Neo Black Tiger (Demo)\0", NULL, "OzzyOuzo", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 1, HARDWARE_SNK_NEOGEO, GBF_PLATFORM, 0,
	NULL, nblktigerRomInfo, nblktigerRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// The Eye of Typhoon (Beta 5 Version)
// https://ozzyouzo.itch.io/teot

static struct BurnRomInfo teotRomDesc[] = {
	{ "teot-p1.bin",    0x0100000, 0x1cd69227, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "teot-p2.bin",    0x0800000, 0x68dc7463, 1 | BRF_ESS | BRF_PRG }, //  1 68K code

	{ "teot-s1.bin",    0x0020000, 0x6d05f74b, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "teot-c1.bin",    0x1000000, 0x2fdbfbef, 3 | BRF_GRA },           //  3 Sprite data
	{ "teot-c2.bin",    0x1000000, 0x4b953a79, 3 | BRF_GRA },           //  4
	
	{ "teot-m1.bin",    0x0010000, 0x16f81e41, 4 | BRF_ESS | BRF_PRG }, //  5 Z80 code

	{ "teot-v1.bin",    0x0800000, 0xd2911e9c, 5 | BRF_SND },           //  6 Sound data
	{ "teot-v2.bin",    0x0800000, 0x49e3afe6, 5 | BRF_SND },           //  7
};

STDROMPICKEXT(teot, teot, neogeo)
STD_ROM_FN(teot)

struct BurnDriver BurnDrvTeot = {
	"teot", NULL, "neogeo", NULL, "2022",
	"The Eye of Typhoon (Beta 5 Version)\0", NULL, "OzzyOuzo", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_SNK_NEOGEO, GBF_VSFIGHT, 0,
	NULL, teotRomInfo, teotRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neoaesDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Cabal (Neo-Geo Conversion)

static struct BurnRomInfo cabalngRomDesc[] = {
	{ "cabal-p1.bin",   0x100000, 0xbb193926, 1 | BRF_ESS | BRF_PRG }, 	//  0 68K code

	{ "cabal-s1.bin",	0x020000, 0x45f0bc5e, 2 | BRF_GRA },           	//  1 Text layer tiles

	{ "cabal-c1.bin",	0x100000, 0x9f5ea8bb, 3 | BRF_GRA },           	//  2 Sprite data
	{ "cabal-c2.bin",	0x100000, 0xedf277f2, 3 | BRF_GRA },           	//  3

	{ "cabal-m1.bin",	0x010000, 0xefd97334, 4 | BRF_ESS | BRF_PRG }, 	//  4 Z80 code

	{ "cabal-v1.bin",	0x100000, 0x1a5bbc12, 5 | BRF_SND },           	//  5 Sound data
	{ "cabal-v2.bin",	0x100000, 0xa2b9c011, 5 | BRF_SND },           	//  5 Sound data
};

STDROMPICKEXT(cabalng, cabalng, neogeo)
STD_ROM_FN(cabalng)

struct BurnDriver BurnDrvCabalng = {
	"cabalng", NULL, "neogeo", NULL, "1988-2021",
	"Cabal (Neo-Geo Conversion)\0", NULL, "TAD Corporation", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_SHOOT, 0,
	NULL, cabalngRomInfo, cabalngRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Looptris (v.2021-12-26)

static struct BurnRomInfo looptrisRomDesc[] = {
	{ "looptris.p1",	0x080000, 0x8fcb5104, 1 | BRF_ESS | BRF_PRG }, 	//  0 68K code

	{ "looptris.s1",	0x020000, 0x70e70448, 2 | BRF_GRA },           	//  1 Text layer tiles

	{ "looptris.c1",	0x080000, 0xb9413f13, 3 | BRF_GRA },           	//  2 Sprite data
	{ "looptris.c2",	0x080000, 0x9409dbe8, 3 | BRF_GRA },           	//  3

	{ "looptris.m1",	0x020000, 0xe7105df8, 4 | BRF_ESS | BRF_PRG }, 	//  4 Z80 code

	{ "looptris.v1",	0x080000, 0xdfa63cd2, 5 | BRF_SND },           	//  5 Sound data
};

STDROMPICKEXT(looptris, looptris, neogeo)
STD_ROM_FN(looptris)

struct BurnDriver BurnDrvLooptris = {
	"looptris", NULL, "neogeo", NULL, "2021",
	"Looptris (v.2021-12-26)\0", NULL, "Blastar", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_PUZZLE, 0,
	NULL, looptrisRomInfo, looptrisRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Hypernoid (Ver. 20211128)

static struct BurnRomInfo hypernoidRomDesc[] = {
	{ "hypernoid-p1.p1",	0x100000, 0xe024fa76, 1 | BRF_ESS | BRF_PRG }, 	//  0 68K code

	{ "hypernoid-s1.s1",	0x020000, 0xbb82ab71, 2 | BRF_GRA },           	//  1 Text layer tiles

	{ "hypernoid-c1.c1",	0x200000, 0x41d6140a, 3 | BRF_GRA },           	//  2 Sprite data
	{ "hypernoid-c2.c2",	0x200000, 0x36f35df2, 3 | BRF_GRA },           	//  3

	{ "hypernoid-m1.m1",	0x080000, 0x6c8eaacc, 4 | BRF_ESS | BRF_PRG }, 	//  4 Z80 code

	{ "hypernoid-v.v1",		0x400000, 0xdafa1bdd, 5 | BRF_SND },           	//  5 Sound data
	{ "hypernoid-v.v2",		0x400000, 0x85ad8283, 5 | BRF_SND },           	//  6
	{ "hypernoid-v.v3",		0x400000, 0x86c27f0c, 5 | BRF_SND },           	//  7
	{ "hypernoid-v.v4",		0x400000, 0xa3982244, 5 | BRF_SND },           	//  8
};

STDROMPICKEXT(hypernoid, hypernoid, neogeo)
STD_ROM_FN(hypernoid)

struct BurnDriver BurnDrvHypernoid = {
	"hypernoid", NULL, "neogeo", NULL, "2021",
	"Hypernoid (Ver. 20211128)\0", NULL, "NeoHomeBrew", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 1, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_BREAKOUT, 0,
	NULL, hypernoidRomInfo, hypernoidRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

static struct BurnRomInfo mslugunityRomDesc[] = {
	{ "201-p1.p1",    0x200000, 0xa3186dfd, 1 | BRF_ESS | BRF_PRG }, //  0 68K code		/ TC5316200

	{ "201-s1.s1",    0x020000, 0x2f55958d, 2 | BRF_GRA },           //  1 Text layer tiles / TC531000

	{ "201-c1.c1",    0x400000, 0x72813676, 3 | BRF_GRA },           //  2 Sprite data		/ TC5332205
	{ "201-c2.c2",    0x400000, 0x96f62574, 3 | BRF_GRA },           //  3 					/ TC5332205
	{ "201-c3.c3",    0x400000, 0x5121456a, 3 | BRF_GRA },           //  4 					/ TC5332205
	{ "201-c4.c4",    0x400000, 0xf4ad59a3, 3 | BRF_GRA },           //  5 					/ TC5332205

	{ "201-m1.m1",    0x020000, 0xc28b3253, 4 | BRF_ESS | BRF_PRG }, //  6 Z80 code			/ TC531001

	{ "201-v1.v1",    0x400000, 0x23d22ed1, 5 | BRF_SND },           //  7 Sound data		/ TC5332204
	{ "201-v2.v2",    0x400000, 0x472cf9db, 5 | BRF_SND },           //  8 					/ TC5332204
};

STDROMPICKEXT(mslugunity, mslugunity, neogeo)
STD_ROM_FN(mslugunity)

struct BurnDriver BurnDrvmslugunity = {
	"mslugunity", "mslug", "neogeo", NULL, "2021",
	"Metal Slug Unity (Added Timer Ver. 2021-02-05)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_SWAPP, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslugunityRomInfo, mslugunityRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

static struct BurnRomInfo mslug2unityRomDesc[] = {
	{ "241-p1.bin",   0x100000, 0x1562cf23, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "241-p2.sp2",   0x200000, 0x38883f44, 1 | BRF_ESS | BRF_PRG }, //  1 

	{ "241-s1.s1",    0x020000, 0xf3d32f0f, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "241-c1.c1",    0x800000, 0x394b5e0d, 3 | BRF_GRA },           //  3 Sprite data
	{ "241-c2.c2",    0x800000, 0xe5806221, 3 | BRF_GRA },           //  4 
	{ "241-c3.c3",    0x800000, 0x9f6bfa6f, 3 | BRF_GRA },           //  5 
	{ "241-c4.c4",    0x800000, 0x7d3e306f, 3 | BRF_GRA },           //  6 

	{ "241-m1.m1",    0x020000, 0x94520ebd, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code

	{ "241-v1.v1",    0x400000, 0x99ec20e8, 5 | BRF_SND },           //  8 Sound data
	{ "241-v2.v2",    0x400000, 0xecb16799, 5 | BRF_SND },           //  9 
};

STDROMPICKEXT(mslug2unity, mslug2unity, neogeo)
STD_ROM_FN(mslug2unity)

struct BurnDriver BurnDrvmslug2unity = {
	"mslug2unity", "mslug2", "neogeo", NULL, "2021",
	"Metal Slug 2 - Super Vehicle-001/II Unity (Added Timer Ver. 2021-01-22)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug2unityRomInfo, mslug2unityRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

static struct BurnRomInfo mslugxunityRomDesc[] = {
	{ "250-p1.p1",    0x100000, 0x36102d34, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "250-p2.ep1",   0x400000, 0x1fda2e12, 1 | BRF_ESS | BRF_PRG }, //  1 

	{ "250-s1.s1",    0x020000, 0xfb6f441d, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "250-c1.c1",    0x800000, 0x09a52c6f, 3 | BRF_GRA },           //  3 Sprite data
	{ "250-c2.c2",    0x800000, 0x31679821, 3 | BRF_GRA },           //  4 
	{ "250-c3.c3",    0x800000, 0xfd602019, 3 | BRF_GRA },           //  5 
	{ "250-c4.c4",    0x800000, 0x31354513, 3 | BRF_GRA },           //  6 
	{ "250-c5.c5",    0x800000, 0xa4b56124, 3 | BRF_GRA },           //  7 
	{ "250-c6.c6",    0x800000, 0x83e3e69d, 3 | BRF_GRA },           //  8 

	{ "250-m1.m1",    0x020000, 0xfd42a842, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "250-v1.v1",    0x400000, 0xc79ede73, 5 | BRF_SND },           // 10 Sound data
	{ "250-v2.v2",    0x400000, 0xea9aabe1, 5 | BRF_SND },           // 11 
	{ "250-v3.v3",    0x200000, 0x2ca65102, 5 | BRF_SND },           // 12 
};

STDROMPICKEXT(mslugxunity, mslugxunity, neogeo)
STD_ROM_FN(mslugxunity)

struct BurnDriver BurnDrvmslugxunity = {
	"mslugxunity", "mslugx", "neogeo", NULL, "2021",
	"Metal Slug X - Super Vehicle-001 Unity (Added Timer Ver. 2021-01-23)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslugxunityRomInfo, mslugxunityRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslugxInit, NeoExit, NeoFrame, NeoRender, mslugxScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

static struct BurnRomInfo mslug3unityRomDesc[] = {
	{ "green.neo-sma",  0x040000, 0x3c6c536b, 9 | BRF_ESS | BRF_PRG }, //  0 68K code		/ stored in the custom chip
																	   /* The SMA for this release has a green colour marking; the older revision has a white colour marking */
	{ "256-pg1.p1",   	0x400000, 0xb07edfd5, 1 | BRF_ESS | BRF_PRG }, //  1 				/ TC5332202
	{ "256-pg2.p2",   	0x400000, 0xea0c22b6, 1 | BRF_ESS | BRF_PRG }, //  2 				/ TC5332202

																	   /* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
																	   /* Encrypted */
	{ "256-c1.c1",    	0x800000, 0x5a79c34e, 3 | BRF_GRA },           //  3 Sprite data	/ TC5364202
	{ "256-c2.c2",    	0x800000, 0x944c362c, 3 | BRF_GRA },           //  4 				/ TC5364202
	{ "256-c3.c3",    	0x800000, 0x6e69d36f, 3 | BRF_GRA },           //  5 				/ TC5364202		
	{ "256-c4.c4",    	0x800000, 0xb755b4eb, 3 | BRF_GRA },           //  6 				/ TC5364202
	{ "256-c5.c5",    	0x800000, 0x7aacab47, 3 | BRF_GRA },           //  7 				/ TC5364202
	{ "256-c6.c6",    	0x800000, 0xc698fd5d, 3 | BRF_GRA },           //  8 				/ TC5364202	
	{ "256-c7.c7",    	0x800000, 0xcfceddd2, 3 | BRF_GRA },           //  9 				/ TC5364202
	{ "256-c8.c8",    	0x800000, 0x4d9be34c, 3 | BRF_GRA },           // 10 				/ TC5364202

	{ "256-m1.m1",    	0x080000, 0xeaeec116, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code		/ mask rom TC534000

	{ "256-v1.v1",    	0x400000, 0xf2690241, 5 | BRF_SND },           // 12 Sound data 	/ TC5332204
	{ "256-v2.v2",    	0x400000, 0x7e2a10bd, 5 | BRF_SND },           // 13 				/ TC5332204
	{ "256-v3.v3",    	0x400000, 0x0eaec17c, 5 | BRF_SND },           // 14 				/ TC5332204
	{ "256-v4.v4",    	0x400000, 0x9b4b22d4, 5 | BRF_SND },           // 15 				/ TC5332204
};

STDROMPICKEXT(mslug3unity, mslug3unity, neogeo)
STD_ROM_FN(mslug3unity)

struct BurnDriver BurnDrvmslug3unity = {
	"mslug3unity", "mslug3", "neogeo", NULL, "2021",
	"Metal Slug 3 Unity (Added Timer Ver. 2021-02-13)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug3unityRomInfo, mslug3unityRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug3Init, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

static struct BurnRomInfo mslug4unityRomDesc[] = {
	{ "263-p1.p1",    0x100000, 0xef3501b6, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "263-p2.sp2",   0x400000, 0xfdb7aed8, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "263-c1.c1",    0x800000, 0x84865f8a, 3 | BRF_GRA },           //  2 Sprite data
	{ "263-c2.c2",    0x800000, 0x81df97f2, 3 | BRF_GRA },           //  3
	{ "263-c3.c3",    0x800000, 0x1a343323, 3 | BRF_GRA },           //  4
	{ "263-c4.c4",    0x800000, 0x942cfb44, 3 | BRF_GRA },           //  5
	{ "263-c5.c5",    0x800000, 0xa748854f, 3 | BRF_GRA },           //  6
	{ "263-c6.c6",    0x800000, 0x5c8ba116, 3 | BRF_GRA },           //  7

	{ "263-m1.m1",    0x020000, 0x46ac8228, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	{ "263-v1.v1",    0x800000, 0x01e9b9cd, 5 | BRF_SND },           //  9 Sound data
	{ "263-v2.v2",    0x800000, 0x4ab2bf81, 5 | BRF_SND },           // 10
};

STDROMPICKEXT(mslug4unity, mslug4unity, neogeo)
STD_ROM_FN(mslug4unity)

struct BurnDriver BurnDrvmslug4unity = {
	"mslug4unity", "mslug4", "neogeo", NULL, "2021",
	"Metal Slug 4 Unity (Added Timer Ver. 2021-01-30)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug4unityRomInfo, mslug4unityRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug4Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

static struct BurnRomInfo mslug5unityRomDesc[] = {
	{ "268-p1cr.p1",   0x400000, 0xced9f5a8, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "268-p2cr.p2",   0x400000, 0x1c08dbb6, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "268-c1c.c1",    0x800000, 0xab7c389a, 3 | BRF_GRA },           //  2 Sprite data
	{ "268-c2c.c2",    0x800000, 0x3560881b, 3 | BRF_GRA },           //  3
	{ "268-c3c.c3",    0x800000, 0x3af955ea, 3 | BRF_GRA },           //  4
	{ "268-c4c.c4",    0x800000, 0xc329c373, 3 | BRF_GRA },           //  5
	{ "268-c5c.c5",    0x800000, 0x959c8177, 3 | BRF_GRA },           //  6
	{ "268-c6c.c6",    0x800000, 0x010a831b, 3 | BRF_GRA },           //  7
	{ "268-c7c.c7",    0x800000, 0x6d72a969, 3 | BRF_GRA },           //  8
	{ "268-c8c.c8",    0x800000, 0x551d720e, 3 | BRF_GRA },           //  9

	{ "268-m1.bin",   0x080000, 0x4a5a6e0e, 4 | BRF_ESS | BRF_PRG },

	{ "268-v1c.v1",    0x800000, 0xae31d60c, 5 | BRF_SND },           // 11 Sound data
	{ "268-v2c.v2",    0x800000, 0xc40613ed, 5 | BRF_SND },           // 12
};

STDROMPICKEXT(mslug5unity, mslug5unity, neogeo)
STD_ROM_FN(mslug5unity)

struct BurnDriver BurnDrvmslug5unity = {
	"mslug5unity", "mslug5", "neogeo", NULL, "2021",
	"Metal Slug 5 Unity (Added Timer Ver. 2021-04-23)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5unityRomInfo, mslug5unityRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug5Init, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 3 (X, Homebrew)
// 20220203
static struct BurnRomInfo mslug3xRomDesc[] = {
	{ "mslug3x-p1cr.p1",	0x400000, 0x840b5b8e, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "mslug3x-p2cr.p2",	0x400000, 0xc7635ac9, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "mslug3x-c1c.c1",		0x800000, 0xbbb8d9f6, 3 | BRF_GRA },           //  2 Sprite data
	{ "mslug3x-c2c.c2",		0x800000, 0x5fb3498a, 3 | BRF_GRA },           //  3

	/* Encrypted */
	{ "mslug3x-m1.m1",		0x020000, 0x876df5e1, 4 | BRF_ESS | BRF_PRG }, //  4 Z80 code

	/* Encrypted */
	{ "mslug3x-v1c.v1",		0x800000, 0x5d904213, 5 | BRF_SND },           //  5 Sound data
	{ "mslug3x-v2c.v2",		0x800000, 0x97cee550, 5 | BRF_SND },           //  6
};

STDROMPICKEXT(mslug3x, mslug3x, neogeo)
STD_ROM_FN(mslug3x)

struct BurnDriver BurnDrvmslug3x = {
	"mslug3x", NULL, "neogeo", NULL, "2022",
	"Metal Slug 3 (X, Homebrew, 20220203)\0", NULL, "Willnie", "Neo Geo AES",
	L"Metal Slug 3 (X, Homebrew, 20220203)\0\u5408\u91d1\u5f48\u982d\u5f69\u86cb\u6bcd\u8266\u5927\u6c7a\u6230 (\u81ea\u88fd\u7248)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HOMEBREW, 1, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ALTERNATE_TEXT | HARDWARE_SNK_P32 | HARDWARE_SNK_ENCRYPTED_M1, GBF_MISC, FBF_MSLUG,
	NULL, mslug3xRomInfo, mslug3xRomName, NULL, NULL, NULL, NULL, neoForceAESInputInfo, neoForceAESDIPInfo,
	mslug5Init, NeoPVCExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 2 - Super Vehicle-001/II (Survival, Hack)
// GOTVG 20180731
static struct BurnRomInfo mslug2ctRomDesc[] = {
	{ "241-p1ct.p1",	0x100000, 0x78ad6864, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "241-p2ct.sp2",	0x200000, 0xfe36f353, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "241-s1.s1",		0x020000, 0xf3d32f0f, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "241-c1.c1",		0x800000, 0x394b5e0d, 3 | BRF_GRA },           //  3 Sprite data
	{ "241-c2.c2",		0x800000, 0xe5806221, 3 | BRF_GRA },           //  4
	{ "241-c3.c3",		0x800000, 0x9f6bfa6f, 3 | BRF_GRA },           //  5
	{ "241-c4.c4",		0x800000, 0x7d3e306f, 3 | BRF_GRA },           //  6

	{ "241-m1.m1",		0x020000, 0x94520ebd, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code

	{ "241-v1.v1",		0x400000, 0x99ec20e8, 5 | BRF_SND },           //  8 Sound data
	{ "241-v2.v2",		0x400000, 0xecb16799, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(mslug2ct, mslug2ct, neogeo)
STD_ROM_FN(mslug2ct)

struct BurnDriver BurnDrvmslug2ct = {
	"mslug2ct", "mslug2", "neogeo", NULL, "2018",
	"Metal Slug 2 - Super Vehicle-001/II (Survival, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug2ctRomInfo, mslug2ctRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 2 - Super Vehicle-001/II (Enemies Reset, Hack)
// GOTVG 20140914
static struct BurnRomInfo mslug2rRomDesc[] = {
	{ "241-p1r.p1",		0x100000, 0xf882d50d, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "241-p2.sp2",		0x200000, 0x38883f44, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "241-s1.s1",		0x020000, 0xf3d32f0f, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "241-c1.c1",		0x800000, 0x394b5e0d, 3 | BRF_GRA },           //  3 Sprite data
	{ "241-c2.c2",		0x800000, 0xe5806221, 3 | BRF_GRA },           //  4
	{ "241-c3.c3",		0x800000, 0x9f6bfa6f, 3 | BRF_GRA },           //  5
	{ "241-c4.c4",		0x800000, 0x7d3e306f, 3 | BRF_GRA },           //  6

	{ "241-m1.m1",		0x020000, 0x94520ebd, 4 | BRF_ESS | BRF_PRG }, //  7 Z80 code

	{ "241-v1.v1",		0x400000, 0x99ec20e8, 5 | BRF_SND },           //  8 Sound data
	{ "241-v2.v2",		0x400000, 0xecb16799, 5 | BRF_SND },           //  9
};

STDROMPICKEXT(mslug2r, mslug2r, neogeo)
STD_ROM_FN(mslug2r)

struct BurnDriver BurnDrvmslug2r = {
	"mslug2r", "mslug2", "neogeo", NULL, "2014",
	"Metal Slug 2 - Super Vehicle-001/II (Enemies Reset, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug2rRomInfo, mslug2rRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug X - Super Vehicle-001 (Enemies Reset, Hack)
// GOTVG 20131028
static struct BurnRomInfo mslugxc2RomDesc[] = {
	{ "250-p1.p1",		0x100000, 0x81f1f60b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "250-p2c2.ep1",	0x400000, 0x5d1c52cd, 1 | BRF_ESS | BRF_PRG }, //  1 

	{ "250-s1.s1",		0x020000, 0xfb6f441d, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "250-c1.c1",		0x800000, 0x09a52c6f, 3 | BRF_GRA },           //  3 Sprite data
	{ "250-c2.c2",		0x800000, 0x31679821, 3 | BRF_GRA },           //  4 
	{ "250-c3.c3",		0x800000, 0xfd602019, 3 | BRF_GRA },           //  5 
	{ "250-c4.c4",		0x800000, 0x31354513, 3 | BRF_GRA },           //  6 
	{ "250-c5.c5",		0x800000, 0xa4b56124, 3 | BRF_GRA },           //  7 
	{ "250-c6.c6",		0x800000, 0x83e3e69d, 3 | BRF_GRA },           //  8 

	{ "250-m1.m1",		0x020000, 0xfd42a842, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "250-v1.v1",		0x400000, 0xc79ede73, 5 | BRF_SND },           // 10 Sound data
	{ "250-v2.v2",		0x400000, 0xea9aabe1, 5 | BRF_SND },           // 11 
	{ "250-v3.v3",		0x200000, 0x2ca65102, 5 | BRF_SND },           // 12 
};

STDROMPICKEXT(mslugxc2, mslugxc2, neogeo)
STD_ROM_FN(mslugxc2)

struct BurnDriver BurnDrvmslugxc2 = {
	"mslugxc2", "mslugx", "neogeo", NULL, "2013",
	"Metal Slug X - Super Vehicle-001 (Enemies Reset, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslugxc2RomInfo, mslugxc2RomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslugxInit, NeoExit, NeoFrame, NeoRender, mslugxScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug X - Super Vehicle-001 (Green Blue, Hack)
// GOTVG 20161016
static struct BurnRomInfo mslugxebRomDesc[] = {
	{ "250-p1eb.p1",	0x100000, 0x764d1bb1, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "250-p2eb.ep1",	0x400000, 0xa51363d1, 1 | BRF_ESS | BRF_PRG }, //  1 

	{ "250-s1.s1",		0x020000, 0xfb6f441d, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "250-c1.c1",		0x800000, 0x09a52c6f, 3 | BRF_GRA },           //  3 Sprite data
	{ "250-c2.c2",		0x800000, 0x31679821, 3 | BRF_GRA },           //  4 
	{ "250-c3.c3",		0x800000, 0xfd602019, 3 | BRF_GRA },           //  5 
	{ "250-c4.c4",		0x800000, 0x31354513, 3 | BRF_GRA },           //  6 
	{ "250-c5.c5",		0x800000, 0xa4b56124, 3 | BRF_GRA },           //  7 
	{ "250-c6.c6",		0x800000, 0x83e3e69d, 3 | BRF_GRA },           //  8 

	{ "250-m1.m1",		0x020000, 0xfd42a842, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "250-v1.v1",		0x400000, 0xc79ede73, 5 | BRF_SND },           // 10 Sound data
	{ "250-v2.v2",		0x400000, 0xea9aabe1, 5 | BRF_SND },           // 11 
	{ "250-v3.v3",		0x200000, 0x2ca65102, 5 | BRF_SND },           // 12 
};

STDROMPICKEXT(mslugxeb, mslugxeb, neogeo)
STD_ROM_FN(mslugxeb)

struct BurnDriver BurnDrvmslugxeb = {
	"mslugxeb", "mslugx", "neogeo", NULL, "2016",
	"Metal Slug X - Super Vehicle-001 (Green Blue, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslugxebRomInfo, mslugxebRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslugxInit, NeoExit, NeoFrame, NeoRender, mslugxScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug X - Super Vehicle-001 (Extreme Zone, Hack)
// GOTVG 20150421
static struct BurnRomInfo mslugxlbRomDesc[] = {
	{ "250-p1lb.p1",	0x100000, 0x41fc71cc, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "250-p2lb.ep1",	0x400000, 0x6a114e1a, 1 | BRF_ESS | BRF_PRG }, //  1 

	{ "250-s1.s1",		0x020000, 0xfb6f441d, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "250-c1lb.c1",	0x800000, 0xd52e8702, 3 | BRF_GRA },           //  3 Sprite data
	{ "250-c2lb.c2",	0x800000, 0xedec334c, 3 | BRF_GRA },           //  4 
	{ "250-c3.c3",		0x800000, 0xfd602019, 3 | BRF_GRA },           //  5 
	{ "250-c4.c4",		0x800000, 0x31354513, 3 | BRF_GRA },           //  6 
	{ "250-c5.c5",		0x800000, 0xa4b56124, 3 | BRF_GRA },           //  7 
	{ "250-c6.c6",		0x800000, 0x83e3e69d, 3 | BRF_GRA },           //  8 

	{ "250-m1.m1",		0x020000, 0xfd42a842, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "250-v1.v1",		0x400000, 0xc79ede73, 5 | BRF_SND },           // 10 Sound data
	{ "250-v2.v2",		0x400000, 0xea9aabe1, 5 | BRF_SND },           // 11 
	{ "250-v3.v3",		0x200000, 0x2ca65102, 5 | BRF_SND },           // 12 
};

STDROMPICKEXT(mslugxlb, mslugxlb, neogeo)
STD_ROM_FN(mslugxlb)

struct BurnDriver BurnDrvmslugxlb = {
	"mslugxlb", "mslugx", "neogeo", NULL, "2015",
	"Metal Slug X - Super Vehicle-001 (Extreme Zone, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK | BDF_HISCORE_SUPPORTED, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslugxlbRomInfo, mslugxlbRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslugxInit, NeoExit, NeoFrame, NeoRender, mslugxScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 3 (Enemies Reset, Hack)
// GOTVG 20190720
static struct BurnRomInfo mslug3cRomDesc[] = {
	{ "256-ph1c.p1",	0x100000, 0x05c99714, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "256-ph2c.sp2",	0x400000, 0xb948a472, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "256-s1d.s1",		0x020000, 0x8458fff9, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "256-c1d.c1",		0x800000, 0x3540398c, 3 | BRF_GRA },           //  3 Sprite data
	{ "256-c2d.c2",		0x800000, 0xbdd220f0, 3 | BRF_GRA },           //  4
	{ "256-c3d.c3",		0x800000, 0xbfaade82, 3 | BRF_GRA },           //  5
	{ "256-c4d.c4",		0x800000, 0x1463add6, 3 | BRF_GRA },           //  6
	{ "256-c5d.c5",		0x800000, 0x48ca7f28, 3 | BRF_GRA },           //  7
	{ "256-c6d.c6",		0x800000, 0x806eb36f, 3 | BRF_GRA },           //  8
	{ "256-c7d.c7",		0x800000, 0xed559fac, 3 | BRF_GRA },           //  9
	{ "256-c8d.c8",		0x800000, 0x1c52378b, 3 | BRF_GRA },           // 10

	{ "256-m1.m1",		0x080000, 0xeaeec116, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "256-v1.v1",		0x400000, 0xf2690241, 5 | BRF_SND },           // 12 Sound data
	{ "256-v2.v2",		0x400000, 0x7e2a10bd, 5 | BRF_SND },           // 13
	{ "256-v3.v3",		0x400000, 0x0eaec17c, 5 | BRF_SND },           // 14
	{ "256-v4.v4",		0x400000, 0x9b4b22d4, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(mslug3c, mslug3c, neogeo)
STD_ROM_FN(mslug3c)

struct BurnDriver BurnDrvmslug3c = {
	"mslug3c", "mslug3", "neogeo", NULL, "2019",
	"Metal Slug 3 (Enemies Reset, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug3cRomInfo, mslug3cRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 3 (Green Blue, Hack)
// GOTVG 20130227
static struct BurnRomInfo mslug3ebRomDesc[] = {
	{ "ms3eb.neo-sma",	0x040000, 0x2a9935c2, 9 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "256-pg1eb.p1",	0x400000, 0x6ec2e58d, 1 | BRF_ESS | BRF_PRG }, //  1
	{ "256-pg2eb.p2",	0x400000, 0xca4eb0d3, 1 | BRF_ESS | BRF_PRG }, //  2

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "256-c1.c1",		0x800000, 0x5a79c34e, 3 | BRF_GRA },           //  3 Sprite data
	{ "256-c2.c2",		0x800000, 0x944c362c, 3 | BRF_GRA },           //  4
	{ "256-c3.c3",		0x800000, 0x6e69d36f, 3 | BRF_GRA },           //  5
	{ "256-c4.c4",		0x800000, 0xb755b4eb, 3 | BRF_GRA },           //  6
	{ "256-c5.c5",		0x800000, 0x7aacab47, 3 | BRF_GRA },           //  7
	{ "256-c6.c6",		0x800000, 0xc698fd5d, 3 | BRF_GRA },           //  8
	{ "256-c7.c7",		0x800000, 0xcfceddd2, 3 | BRF_GRA },           //  9
	{ "256-c8.c8",		0x800000, 0x4d9be34c, 3 | BRF_GRA },           // 10

	{ "256-m1.m1",		0x080000, 0xeaeec116, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "256-v1.v1",		0x400000, 0xf2690241, 5 | BRF_SND },           // 12 Sound data
	{ "256-v2.v2",		0x400000, 0x7e2a10bd, 5 | BRF_SND },           // 13
	{ "256-v3.v3",		0x400000, 0x0eaec17c, 5 | BRF_SND },           // 14
	{ "256-v4.v4",		0x400000, 0x9b4b22d4, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(mslug3eb, mslug3eb, neogeo)
STD_ROM_FN(mslug3eb)

struct BurnDriver BurnDrvmslug3eb = {
	"mslug3eb", "mslug3", "neogeo", NULL, "2013",
	"Metal Slug 3 (Green Blue, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC42 | HARDWARE_SNK_SMA_PROTECTION, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug3ebRomInfo, mslug3ebRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug3Init, NeoSMAExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 3 (Last Bullet Remix, Hack)
// Enter the portal with the following commands : Start + Button D
// 20220616
static struct BurnRomInfo mslug3lwRomDesc[] = {
	{ "256-ph1lw.p1",	0x100000, 0xf804b927, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "256-ph2lw.sp2",	0x400000, 0xed558b74, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "256-s1d.s1",		0x020000, 0x8458fff9, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "256-c1d.c1",		0x800000, 0x3540398c, 3 | BRF_GRA },           //  3 Sprite data
	{ "256-c2d.c2",		0x800000, 0xbdd220f0, 3 | BRF_GRA },           //  4
	{ "256-c3d.c3",		0x800000, 0xbfaade82, 3 | BRF_GRA },           //  5
	{ "256-c4d.c4",		0x800000, 0x1463add6, 3 | BRF_GRA },           //  6
	{ "256-c5d.c5",		0x800000, 0x48ca7f28, 3 | BRF_GRA },           //  7
	{ "256-c6d.c6",		0x800000, 0x806eb36f, 3 | BRF_GRA },           //  8
	{ "256-c7d.c7",		0x800000, 0xed559fac, 3 | BRF_GRA },           //  9
	{ "256-c8d.c8",		0x800000, 0x1c52378b, 3 | BRF_GRA },           // 10

	{ "256-m1.m1",		0x080000, 0xeaeec116, 4 | BRF_ESS | BRF_PRG }, // 11 Z80 code

	{ "256-v1.v1",		0x400000, 0xf2690241, 5 | BRF_SND },           // 12 Sound data
	{ "256-v2.v2",		0x400000, 0x7e2a10bd, 5 | BRF_SND },           // 13
	{ "256-v3.v3",		0x400000, 0x0eaec17c, 5 | BRF_SND },           // 14
	{ "256-v4.v4",		0x400000, 0x9b4b22d4, 5 | BRF_SND },           // 15
};

STDROMPICKEXT(mslug3lw, mslug3lw, neogeo)
STD_ROM_FN(mslug3lw)

struct BurnDriver BurnDrvmslug3lw = {
	"mslug3lw", "mslug3", "neogeo", NULL, "2022",
	"Metal Slug 3 (Last Bullet Remix, Hack)\0", "Enter the portal : Start + Button D", "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug3lwRomInfo, mslug3lwRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000, 304, 224, 4, 3
};

// Metal Slug 4 (Enemies Reset Fusion 20th Anniversary, Hack)
// Not GOTVG, maybe mslug4a + 4c? 20210929
static struct BurnRomInfo mslug4arRomDesc[] = {
	{ "263-p1ar.p1",	0x100000, 0xf0c97843, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "263-p2ar.sp2",	0x400000, 0x1464718D, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "263-s1nd.bin",	0x080000, 0x1eaa05e0, 2 | BRF_GRA },           //  2 Text layer tiles

	{ "263-c1d.c1",		0x800000, 0xa75ffcde, 3 | BRF_GRA },           //  3 Sprite data
	{ "263-c2d.c2",		0x800000, 0x5ab0d12b, 3 | BRF_GRA },           //  4
	{ "263-c3d.c3",		0x800000, 0x61af560c, 3 | BRF_GRA },           //  5
	{ "263-c4d.c4",		0x800000, 0xf2c544fd, 3 | BRF_GRA },           //  6
	{ "263-c5d.c5",		0x800000, 0x84c66c44, 3 | BRF_GRA },           //  7
	{ "263-c6d.c6",		0x800000, 0x5ed018ab, 3 | BRF_GRA },           //  8

	{ "263-m1nd.m1",	0x020000, 0xef5db532, 4 | BRF_ESS | BRF_PRG }, //  9 Z80 code

	{ "263-v1nd.bin",	0x800000, 0xfd6b982e, 5 | BRF_SND },           // 10 Sound data
	{ "263-v2nd.bin",	0x800000, 0x20125227, 5 | BRF_SND },           // 11
};

STDROMPICKEXT(mslug4ar, mslug4ar, neogeo)
STD_ROM_FN(mslug4ar)

struct BurnDriver BurnDrvmslug4ar = {
	"mslug4ar", "mslug4", "neogeo", NULL, "2021",
	"Metal Slug 4 (Enemies Reset Fusion 20th Anniversary, Hack)\0", NULL, "hack", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug4arRomInfo, mslug4arRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 4 (Enemies Reset, Hack)
// GOTVG 20191117
static struct BurnRomInfo mslug4cRomDesc[] = {
	{ "263-p1c.p1",		0x100000, 0x81fd4ae9, 1 | BRF_ESS | BRF_PRG }, //  0 68K code
	{ "263-p2c.sp2",	0x400000, 0xfdb7aed8, 1 | BRF_ESS | BRF_PRG }, //  1

	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
	/* Encrypted */
	{ "263-c1.c1",		0x800000, 0x84865f8a, 3 | BRF_GRA },           //  2 Sprite data
	{ "263-c2.c2",		0x800000, 0x81df97f2, 3 | BRF_GRA },           //  3
	{ "263-c3.c3",		0x800000, 0x1a343323, 3 | BRF_GRA },           //  4
	{ "263-c4.c4",		0x800000, 0x942cfb44, 3 | BRF_GRA },           //  5
	{ "263-c5.c5",		0x800000, 0xa748854f, 3 | BRF_GRA },           //  6
	{ "263-c6.c6",		0x800000, 0x5c8ba116, 3 | BRF_GRA },           //  7

	/* Encrypted */
	{ "263-m1.m1",		0x020000, 0x46ac8228, 4 | BRF_ESS | BRF_PRG }, //  8 Z80 code

	/* Encrypted */
	{ "263-v1.v1",		0x800000, 0x01e9b9cd, 5 | BRF_SND },           //  9 Sound data
	{ "263-v2.v2",		0x800000, 0x4ab2bf81, 5 | BRF_SND },           // 10
};

STDROMPICKEXT(mslug4c, mslug4c, neogeo)
STD_ROM_FN(mslug4c)

struct BurnDriver BurnDrvmslug4c = {
	"mslug4c", "mslug4", "neogeo", NULL, "2021",
	"Metal Slug 4 (Enemies Reset, Hack)\0", NULL, "hack", "Neo Geo",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO | HARDWARE_SNK_CMC50 | HARDWARE_SNK_ENCRYPTED_M1, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug4cRomInfo, mslug4cRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	mslug4Init, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 (Enemies Reset, Hack)
// GOTVG 20190728
static struct BurnRomInfo mslug5cRomDesc[] = {
	{ "268-p1dc.bin",	0x600000, 0xe876d1e7, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "268d-s1.rom",	0x020000, 0x64952683, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "268d-c1.rom",	0x800000, 0x969c0d62, 3 | BRF_GRA },           //  2 Sprite data
	{ "268d-c2.rom",	0x800000, 0xc69ae867, 3 | BRF_GRA },           //  3
	{ "268d-c3.rom",	0x800000, 0xd7beaeaf, 3 | BRF_GRA },           //  4
	{ "268d-c4.rom",	0x800000, 0xe1b1131b, 3 | BRF_GRA },           //  5
	{ "268d-c5.rom",	0x800000, 0x2fa1a5ad, 3 | BRF_GRA },           //  6
	{ "268d-c6.rom",	0x800000, 0x6de89589, 3 | BRF_GRA },           //  7
	{ "268d-c7.rom",	0x800000, 0x97bd0c0a, 3 | BRF_GRA },           //  8
	{ "268d-c8.rom",	0x800000, 0xc0d5bc20, 3 | BRF_GRA },           //  9

	{ "268d-m1.rom",	0x020000, 0x6fa01c9a, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "268d-v1.rom",	0x400000, 0xc3540e0d, 5 | BRF_SND },           // 11 Sound data
	{ "268d-v2.rom",	0x400000, 0x077bd2f4, 5 | BRF_SND },           // 12
	{ "268d-v3.rom",	0x400000, 0x39b14567, 5 | BRF_SND },           // 13
	{ "268d-v4.rom",	0x400000, 0x969ff3b2, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(mslug5c, mslug5c, neogeo)
STD_ROM_FN(mslug5c)

struct BurnDriver BurnDrvmslug5c = {
	"mslug5c", "mslug5", "neogeo", NULL, "2019",
	"Metal Slug 5 (Enemies Reset, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5cRomInfo, mslug5cRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 (Enemy Demons, Hack)
// GOTVG 20191207
static struct BurnRomInfo mslug5mgRomDesc[] = {
	{ "268-p1dmg.bin",	0x600000, 0x1fc7de70, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "268d-s1.rom",	0x020000, 0x64952683, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "268d-c1.rom",	0x800000, 0x969c0d62, 3 | BRF_GRA },           //  2 Sprite data
	{ "268d-c2.rom",	0x800000, 0xc69ae867, 3 | BRF_GRA },           //  3
	{ "268d-c3.rom",	0x800000, 0xd7beaeaf, 3 | BRF_GRA },           //  4
	{ "268d-c4.rom",	0x800000, 0xe1b1131b, 3 | BRF_GRA },           //  5
	{ "268d-c5.rom",	0x800000, 0x2fa1a5ad, 3 | BRF_GRA },           //  6
	{ "268d-c6.rom",	0x800000, 0x6de89589, 3 | BRF_GRA },           //  7
	{ "268d-c7.rom",	0x800000, 0x97bd0c0a, 3 | BRF_GRA },           //  8
	{ "268d-c8.rom",	0x800000, 0xc0d5bc20, 3 | BRF_GRA },           //  9

	{ "268d-m1.rom",	0x020000, 0x6fa01c9a, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "268d-v1.rom",	0x400000, 0xc3540e0d, 5 | BRF_SND },           // 11 Sound data
	{ "268d-v2.rom",	0x400000, 0x077bd2f4, 5 | BRF_SND },           // 12
	{ "268d-v3.rom",	0x400000, 0x39b14567, 5 | BRF_SND },           // 13
	{ "268d-v4.rom",	0x400000, 0x969ff3b2, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(mslug5mg, mslug5mg, neogeo)
STD_ROM_FN(mslug5mg)

struct BurnDriver BurnDrvmslug5mg = {
	"mslug5mg", "mslug5", "neogeo", NULL, "2019",
	"Metal Slug 5 (Enemy Demons, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5mgRomInfo, mslug5mgRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 (Remake Final FC2 Version, Hack)
// 20211003
static struct BurnRomInfo mslug5sgfRomDesc[] = {
	{ "268-p1dsgf.bin",	0x600000, 0xb1fbb850, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "268d-s1.rom",	0x020000, 0x64952683, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "268d-c1.rom",	0x800000, 0x969c0d62, 3 | BRF_GRA },           //  2 Sprite data
	{ "268d-c2.rom",	0x800000, 0xc69ae867, 3 | BRF_GRA },           //  3
	{ "268d-c3.rom",	0x800000, 0xd7beaeaf, 3 | BRF_GRA },           //  4
	{ "268d-c4.rom",	0x800000, 0xe1b1131b, 3 | BRF_GRA },           //  5
	{ "268d-c5.rom",	0x800000, 0x2fa1a5ad, 3 | BRF_GRA },           //  6
	{ "268d-c6.rom",	0x800000, 0x6de89589, 3 | BRF_GRA },           //  7
	{ "268d-c7.rom",	0x800000, 0x97bd0c0a, 3 | BRF_GRA },           //  8
	{ "268d-c8.rom",	0x800000, 0xc0d5bc20, 3 | BRF_GRA },           //  9

	{ "268d-m1.rom",	0x020000, 0x6fa01c9a, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "268d-v1.rom",	0x400000, 0xc3540e0d, 5 | BRF_SND },           // 11 Sound data
	{ "268d-v2.rom",	0x400000, 0x077bd2f4, 5 | BRF_SND },           // 12
	{ "268d-v3.rom",	0x400000, 0x39b14567, 5 | BRF_SND },           // 13
	{ "268d-v4.rom",	0x400000, 0x969ff3b2, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(mslug5sgf, mslug5sgf, neogeo)
STD_ROM_FN(mslug5sgf)

struct BurnDriver BurnDrvmslug5sgf = {
	"mslug5sgf", "mslug5", "neogeo", NULL, "2021",
	"Metal Slug 5 (Remake Final FC2 Version, Hack)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5sgfRomInfo, mslug5sgfRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};

// Metal Slug 5 (X Final Ver, Hack, 20220606)
static struct BurnRomInfo mslug5xRomDesc[] = {
	{ "268-p1dx.bin",	0x600000, 0xafffcd5b, 1 | BRF_ESS | BRF_PRG }, //  0 68K code

	{ "268d-s1.rom",	0x020000, 0x64952683, 2 | BRF_GRA },           //  1 Text layer tiles

	{ "268d-c1.rom",	0x800000, 0x969c0d62, 3 | BRF_GRA },           //  2 Sprite data
	{ "268d-c2.rom",	0x800000, 0xc69ae867, 3 | BRF_GRA },           //  3
	{ "268d-c3.rom",	0x800000, 0xd7beaeaf, 3 | BRF_GRA },           //  4
	{ "268d-c4.rom",	0x800000, 0xe1b1131b, 3 | BRF_GRA },           //  5
	{ "268d-c5.rom",	0x800000, 0x2fa1a5ad, 3 | BRF_GRA },           //  6
	{ "268d-c6.rom",	0x800000, 0x6de89589, 3 | BRF_GRA },           //  7
	{ "268d-c7.rom",	0x800000, 0x97bd0c0a, 3 | BRF_GRA },           //  8
	{ "268d-c8.rom",	0x800000, 0xc0d5bc20, 3 | BRF_GRA },           //  9

	{ "268d-m1.rom",	0x020000, 0x6fa01c9a, 4 | BRF_ESS | BRF_PRG }, // 10 Z80 code

	{ "268d-v1.rom",	0x400000, 0xc3540e0d, 5 | BRF_SND },           // 11 Sound data
	{ "268d-v2.rom",	0x400000, 0x077bd2f4, 5 | BRF_SND },           // 12
	{ "268d-v3.rom",	0x400000, 0x39b14567, 5 | BRF_SND },           // 13
	{ "268d-v4.rom",	0x400000, 0x969ff3b2, 5 | BRF_SND },           // 14
};

STDROMPICKEXT(mslug5x, mslug5x, neogeo)
STD_ROM_FN(mslug5x)

struct BurnDriver BurnDrvmslug5x = {
	"mslug5x", "mslug5", "neogeo", NULL, "2022",
	"Metal Slug 5 (X Final Ver, Hack, 20220606)\0", NULL, "hack", "Neo Geo MVS",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HACK, 2, HARDWARE_PREFIX_CARTRIDGE | HARDWARE_SNK_NEOGEO, GBF_RUNGUN, FBF_MSLUG,
	NULL, mslug5xRomInfo, mslug5xRomName, NULL, NULL, NULL, NULL, neogeoInputInfo, neogeoDIPInfo,
	NeoInit, NeoExit, NeoFrame, NeoRender, NeoScan, &NeoRecalcPalette,
	0x1000,	304, 224, 4, 3
};
