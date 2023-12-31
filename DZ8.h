//
//  DZ8.h
//  DZ8x
//
//  Created by LogicalJoe on 8/13/18.
//  Copyright © 2018-2020 LogicalJoe. All rights reserved.
//

#ifndef DZ8_h
#define DZ8_h

enum {
	CEMEM  = 0x0D1A881, // start of CE usermem
	CSEMEM = 0xA60B,    // start of CSE usermem
	SEMEM  = 0x9D95,    // start of SE usermem
	B6MEM  = 0xD748,    // start of 86(/85?) usermem
	//82 usermem?
	
	CEZX7  = 0xD09466,  // plotSScreen
	CSEZX7 = 0xFFFFFF, // not valid on 16 bit systems
	SEZX7  = 0xFFFFFF, // not valid on 16 bit systems
	B6ZX7  = 0x9000,
	
	CEAPP  = 0x3B0000, // 0x03A1CBC start of a random CE FlashApp
	CSEAPP = 0x4000, // 402b for Graph3DC because of header information (todo: fix)
	SEAPP  = 0x4070,
	B6APP  = 0x0,
};

struct {
	uint32_t USERMEM,ZX7,APP;
} MEM[] = {
	CEMEM , CEZX7 , CEAPP ,
	CSEMEM, CSEZX7, CSEAPP,
	SEMEM , SEZX7 , SEAPP ,
	B6MEM , B6ZX7 , B6APP
};

//const char *Theta = "θ";
//#define Theta DZBA[91].Base

struct {
	char *Ext, *Desc;
} TYPE85[] = { // The 85/86 is complicated :/
	".85n","Real",
	".85c","Complex",
	".85v","Real vector (list)",
	".85v","Complex vector",
	".85l","Real list",
	".85l","Complex list",
	".85m","Matrix",
	".85m","Complex matrix",
	".85k","Constant",
	".85k","Complex constant",
	".85e","Equation",
	""    ,"", // 0x0B
	".85s","String",
	".85d","Graph database",
	".85d","O Graph database",
	".85d","A Graph database",
	".85d","D Graph database", // 0x10
	".85i","Picture",
	".85p","Program",
	".85r","Range",
	""    ,"Screen",
	""    ,"Directory",
	""    ,"",
	".86w","Function",
	".86w","Polar",
	".86w","Parametric",
	".86w","Differential",
	".86w","Zoom Recall",
	""    ,"",
	""    ,"Backup",
	""    ,"Unknown"
};

struct {
	char *Ext, *Desc;
} TYPE[] = { // Updated using tilp/types84p.h , CEmu/gui/qt/mainwindow.cpp , linkguide/ti83+/packet.html , and tilp/lpg/tilp/tilibs/libtifiles/trunk/src/types83p.c
	".8xn", "Real", // 0x0
	".8xl", "Real list",
	".8xm", "Matrix",
	".8xy", "Equation",
	".8xs", "String",
	".8xp", "Program",
	".8xp", "Protected program",
	".8ci", "Picture", // or 8xi
	".8xd", "Graph database",
	"","", "","",
	".8xw", "New equation", // Window Settings in packet.html
	".8xc", "Complex", // 0xC
	".8xl", "Complex list",
	""    , "",
	".8xw", "Window setup",
	".8xz", "Zoom", // 0x10
	".8xt", "TableSet",
	""    , "LCD setup",
	""    , "Backup",
	""    , "", // Used to delete a FLASH appliction
	".8xv", "Appvar",
	".8xp", "Temporary program", // 0x16 (83+ Group)
	".8cg", "TI84+C Group", //  compressed
	//8xg", "TI84+ Group",  //uncompressed
	//8xo", "Tilp Group",   //  compressed
	".8xn", "Fraction", // 83PCE
	""    , "Directory", // What the heck is this?
	".8ca", "Image",
	// 83PCE
	".8xc", "Complex fraction", // 0x1B
	".8xn", "Real radical",
	".8xc", "Complex radical",
	".8xc", "Complex pi",
	".8xc", "Complex pi fraction",
	".8xn", "Real pi", // 0x20
	".8xn", "Real pi fraction",
	/* FLASH start */
	""    , "",
	".8eu", "FLASH OS", // AMS - Advanced Mathmatic Software
	".8ek", "FLASH Application",
	""    , "FLASH Certificate",
	""    , "ID-List", // 0x26
	""    , "Get certificate",
	""    , "",
	/* FLASH end */
	""    , "Clock", // 0x29
	"","", "","", "","", "","", "","", "","",
	"","", // 0x30
	"","", "","", "","", "","", "","",
	"","", "","", "","", "","", "","",
	"","", "","", "","",
	""    , "License" // 0x3E
};

/*
 82    Files are 2
 83    Files are 3
 84+   Files are x
 84+C  Files are c
 83PCE Files are p
 84+CE Files are e
 85    Files are 5
 86    Files are 6
 
 ".8xk", // 84+ App
 ".8ck", // 84+C App
 ".8ek", // 84+CE App
 k=app
 
 ".82u", // 82 Advanced OS
 ".8xu", // 84+ OS
 ".8cu", // 84+C OS
 ".8eu", // 84+CE OS
 u=OS
 
 ".b84", // 84+CE Bundle
 ".b83", // 83PCE Bundle
 
 ".92p", // 92 Program
 ".tns", // nSpire "Program"
 ".8xq", // Flash certificate
 ".8cidl",//ID-List
 */

/*
// 82+/83+/84+ are grouped since only the clock is the difference, and it dosen't have an actual varType.
// For number vartypes, Real and Complex are the genaric handlers we use, they'll dispatch to specific ones.
struct {
	char* Name;
	uint8_t Number;
	char* Extensions[7];
} TIVarTypes[] = {
	// order:                        82     83     82A  82+/83+ 84+C   84+CE  83+PCE
	//                                            84+T    84+         84+CE-T
	// "Unknown"          ,  -1 , { ""   , ""   , ""   , ""   , ""   , ""   , ""   },
	* Standard types *
	"Real"                , 0x00, { "82n", "83n", "8xn", "8xn", "8xn", "8xn", "8xn"},
	"RealList"            , 0x01, { "82l", "83l", "8xl", "8xl", "8xl", "8xl", "8xl"},
	"Matrix"              , 0x02, { "82m", "83m", "8xm", "8xm", "8xm", "8xm", "8xm"},
	"Equation"            , 0x03, { "82y", "83y", "8xy", "8xy", "8xy", "8xy", "8xy"},
	"String"              , 0x04, { "82s", "83s", "8xs", "8xs", "8xs", "8xs", "8xs"},
	"Program"             , 0x05, { "82p", "83p", "8xp", "8xp", "8xp", "8xp", "8xp"},
	"Protected Program"   , 0x06, { "82p", "83p", "8xp", "8xp", "8xp", "8xp", "8xp"},
	"Picture"             , 0x07, { ""   , ""   , "8xi", "8xi", "8ci", "8ci", "8ci"},
	"GraphDataBase"       , 0x08, { "82d", "83d", "8xd", "8xd", "8xd", "8xd", "8xd"},
	"Complex"             , 0x0C, { "82m", "83m", "8xm", "8xm", "8xm", "8xm", "8xm"},
	"ComplexList"         , 0x0D, { "82l", "83l", "8xl", "8xl", "8xl", "8xl", "8xl"},
	"WindowSettings"      , 0x0F, { "82w", "83w", "8xw", "8xw", "8xw", "8xw", "8xw"},
	"RecallWndow"         , 0x10, { "82z", "83z", "8xz", "8xz", "8xz", "8xz", "8xz"},
	"TableRange"          , 0x11, { "82t", "83t", "8xt", "8xt", "8xt", "8xt", "8xt"},
	"Backup"              , 0x13, { "82b", "83b", ""   , "8xb", "8cb", ""   , ""   },
	"AppVar"              , 0x15, { ""   , ""   , ""   , "8xv", "8xv", "8xv", "8xv"},
	"Group"               , 0x17, { "82g", "83g", "8xg", "8xg", "8xg", "8cg", "8cg"},
	"RealFraction"        , 0x18, { ""   , ""   , ""   , "8xn", "8xn", "8xn", "8xn"},
	"Image"               , 0x1A, { ""   , ""   , ""   , ""   , ""   , "8ca", "8ca"},
	* Exact values (TI-83 Premium CE) *
	"ExactComplexFrac"    , 0x1B, { ""   , ""   , ""   , ""   , ""   , ""   , "8xc"},
	"ExactRealRadical"    , 0x1C, { ""   , ""   , ""   , ""   , ""   , ""   , "8xn"},
	"ExactComplexRadical" , 0x1D, { ""   , ""   , ""   , ""   , ""   , ""   , "8xc"},
	"ExactComplexPi"      , 0x1E, { ""   , ""   , ""   , ""   , ""   , ""   , "8xc"},
	"ExactComplexPiFrac"  , 0x1F, { ""   , ""   , ""   , ""   , ""   , ""   , "8xc"},
	"ExactRealPi"         , 0x20, { ""   , ""   , ""   , ""   , ""   , ""   , "8xn"},
	"ExactRealPiFrac"     , 0x21, { ""   , ""   , ""   , ""   , ""   , ""   , "8xn"},
	* System/Flash-related things *
	"Operating System"    , 0x23, { "82u", "83u", "82u", "8xu", "8cu", "8eu", "8pu"},
	"FlashApp"            , 0x24, { ""   , ""   , ""   , "8xk", "8ck", "8ek", "8ek"},
	"Certificate"         , 0x25, { ""   , ""   , ""   , "8xq", "8cq", ""   , ""   },
};
*/

#ifdef DZ8X_F
bool TYPESUP[] = { // For -F only.
	1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,
	1,1,0,0,0,1,0,0,1,0,1,1,1,1,1,1,
	1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
#endif /* DZ8X_F */

/* https://github.com/debrouxl/tilibs/blob/63ba77de8f5ace2e4e6d0e07a11096ce4e2a55d5/libticalcs/trunk/src/ticalcs.h#L161-L191
 * https://www.brandonw.net/calculators/keys/
 * https://wiki.tiplanet.org/Versions_Nspire#V.C3.A9rification_en_NAND
 */
char *DevelopmentKeys[] = { // from ticalcs.h (tilp)
	"None",
	"TI-92 Plus",
	"TI-73/Explorer",
	"TI-89",
	"TI-83 Plus/SE",
	"Community",
	"","",
	"TI-V200",
	"TI-89 Titanium",
	"TI-84 Plus/SE", // 0xA
	"TI-82 Advanced",
	"TI-Nspire CAS",
	"TI-Nspire Lab Cradle, TI-Nspire ViewScreen",
	"TI-Nspire NonCAS",
	"TI-84 Plus CSE / TI-Nspire CX CAS",
	"TI-Nspire CX NonCAS", // 0x10
	"TI-Nspire CM-C CAS",
	"TI-Nspire CM-C NonCAS",
	"TI-(84 Plus|83 Premium) CE", // TI-82 Adv Edition Python? (Partial OS ID)
	"",
	"TI-82 Advanced Edition Python", // ? (OS field 73)
	"","","","","",
	"TI-84 Plus-T" // 0x1B
	"TI-Nspire CX II CAS / TI-Nspire CX II-T CAS / TI-Nspire CX II-C CAS",
	"TI-Nspire CX II",
	"TI-Nspire CX II-T"
};

/* https://wikiti.brandonw.net/index.php?title=83Plus:OS:Variable_Versions */
char *ExportVersion[] = {
	"TI-83/TI-84 Plus",
	"TI-83 Plus All versions",            // Archive-Garbagecollect
	"TI-83 Plus v1.15",                   // ~-%
	"TI-83 Plus v1.16",                   // …-inverse =
	"TI-84 Plus All versions",            // setDate(-ExecLib
	"TI-84 Plus v2.30 and above",         // invT(-Manual-Fit(
	"TI-84 Plus v2.53 and above",         // ZQuadrant-FRAC
	"","","",
	"TI-84 Plus C SE v4.0 and above",     // BLUE-BorderColor
	"TI-84 Plus CE v5.2.0 and above",     // tinydot-eval
	"TI-84 Plus CE v5.3.0 and above",     // ExecuteProgram-piecewise
	"","","","",
	"Unknown" // TI's software likes to use this in the 2nd version byte in the 8x format?
};

char *WINDOWTEXT[] = { // Also used in GDB display
	"Xmin\t", // 630A 00
	"Xmax\t", // 630B
	"Xscl\t", // 6302
	"Ymin\t", // 630C
	"Ymax\t", // 630D
	"Yscl\t", // 6303
	"θmin\t", // 6310
	"θmax\t", // 6311
	"θstep\t",// 6323
	"Tmin\t", // 630E
	"Tmax\t", // 630F 10
	"Tstep\t",// 6322
	"PlotStart",//631B
	"nMax\t", // 631D
	"u(1)\t", //  u(nMin),1st
	"v(1)\t", //  v(nMin),1st
	"nMin\t", // 631F
	"u(2)\t", //  u(nMax),2nd
	"v(2)\t", //  v(nMax),2nd
	"w(2)\t", //  w(nMax),1st
	"PlotStep",//6334 20
	"Xres\t", // 6336
	"w(1)\t", //  w(nMin),2nd
}; // 22 (0x16)

char *DOORHEAD[] = {
	"Description",
	"Icon",
	"Author",
	"Library",
	"Half_Resolution"
};

struct {
	char *Graph, *Line;
} Styles[] = {
	"Thin line",         "Thick",
	"Thick line",        "Dot-Think",
	"Shade above",       "Thin",
	"Shade below",       "Dot-Thin"
	"Trace",             "",
	"Animate",           "",
	"Thick dotted line", "",
	"Thin dotted line",  ""
};

typedef struct {
	uint8_t r,g,b;
} colors_t;

#define colorname(a) ((a)==16?"Off":(a)>16|!(a)?"":BASICEF[0x40+(a)])
struct {
	uint8_t r,g,b;
	char *name; // color names are EF41-EF4F (not Off)
} oscolors[] = {           //   CEmu   TIConnect  R5 G6 B5 (http://ceme.tech/t4925)
	255,255,255,0, // Transparent (8ci files)
	0  ,0  ,255,"BLUE"    ,// 0x0000FF  0x0000FF  00 00 31 | 00 00 FF R*255/31
	255,0  ,0  ,"RED"     ,// 0xFF0000  0xFF0000  31 00 00 | FF 00 00 G*255/63
	0  ,0  ,0  ,"BLACK"   ,// 0x000000  0x000000  00 00 00 | 00 00 00 B*255/31
	255,0  ,255,"MAGENTA" ,// 0xFF00FF  0xFF00FF  31 00 31 | FF 00 FF
	0  ,158,0  ,"GREEN"   ,// 0x009E00  0x009E00  00 39 00 | 00 9E 00
	255,142,33 ,"ORANGE"  ,// 0xFF8E20  0xFF8E21  31 35 04 | FF 8E 21
	181,32 ,0  ,"BROWN"   ,// 0xB62000  0xB52000  22 08 00 | B5 20 00
	0  ,0  ,132,"NAVY"    ,// 0x000086  0x000084  00 00 15 | 00 00 7B
	0  ,146,255,"LTBLUE"  ,// 0x0092FF  0x0092FF  00 36 31 | 00 92 FF
	255,255,0  ,"YELLOW"  ,// 0xFFFF00  0xFFFF00  31 63 00 | FF FF 00
	255,255,255,"WHITE"   ,// 0xFFFFFF  0xFFFFFF  31 63 31 | FF FF FF
	231,227,231,"LTGRAY"  ,// 0xE7E3E7  0xE7E3E7  28 56 28 | E7 ...
	198,195,198,"MEDGRAY" ,// 0xC7C3C7  0xC6C3C6  24 48 24 | C6
	140,138,140,"GRAY"    ,// 0x8E8A8E  0x8C8A8C  17 34 17 | 8C
	82 ,85 ,82 ,"DARKGRAY",// 0x515551  0x525552  10 21 10 | 52
	0  ,0  ,0  ,"Off" // for GDB colors
};

// numarical to hex, even Mateo's Convhex uses it :)
char *HEX[] = { // (char*)&hexprob[]
	"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F",
	"G","H","I","J" // Extra values for BASIC matrices | use `(char*)'A'+n` for matrices instead?
}; // Also used the first few for Image# | now using '0'+(c==9?0:c+1)
/* Use (int)*HEX[] instead
 * (uint4_t)numarical to (int)hex, this is the list to use when HEX[] fails (which it never will)
 * int hexprob[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
 */

#define dz_toxvalue(a) tohex[(a)-0x30]
int tohex[] = { // ascii-0x30 -> hex value
	0,1,2,3,4,5,6,7,8,9,
	0,0,0,0,0,0,0,
	10,11,12,13,14,15
};
// can this be done without a lookup table?
// (((a)>='0'&&(a)<='9')?(a)-'0':((a)>='A'&&(a)<='F')?(a)-'7':0)
// (((a)<='9')?(a)-'0':((a)>='A')?(a)-'A'+10:0)

// I need to find a better way of storing (struct?) and excecuting the check on these (loop through struct).
enum {
	N73, N82, N83,
	NXP, N85, N86,
	N89, N92, Nek,
	Nend,
};

/*struct { // This is ridiculous, but crazy awesome.
	uint8_t v[10],n;
} SIGN[] = {
	{'*','*','T','I','7','3','*','*',' ','\n',0},73,
	{'*','*','T','I','8','2','*','*',' ','\n',0},82,
	{'*','*','T','I','8','3','*','*',' ','\n',0},83,
	{'*','*','T','I','8','3','F','*',' ','\n',0},84,
	{'*','*','T','I','8','5','*','*',' ','\n',0},85,
	{'*','*','T','I','8','6','*','*',' ','\n',0},86,
	{'*','*','T','I','9','2','P','*',' ','\n',0},89,
	{'*','*','T','I','9','2','*','*',' ','\n',0},92,
	{'*','*','T','I','F','L','*','*',' ','\n',0},84,
};*/

struct {
	char v[11];
	uint8_t n;
} SIGN[] = {
	"**TI73**\x1A\n",73, // " \n\00"
	"**TI82**\x1A\n",82, // " \n\00"
	"**TI83**\x1A\n",83, // " \n\00"
	"**TI83F*\x1A\n",84, // " \n\00"
	"**TI85**\x1A\n",85, // " \n\00"
	"**TI86**\x1A\n",86, // " \n\00"
	"**TI92P*\x1A\n",89, // " \n\00"
	"**TI92**\x1A\n",92, // " \n\00"
	"**TIFL**\x1A\n",00, // " \n\00"
};

char *Month[] = {
	"NaM","January","Feburary","March","April","May","June","July","August","September","October","November","December"
};
/*
typedef struct {
	char *Header;
	char Comment[42];
	uint16_t Size, HSize, Size2;
	uint8_t Type;
	char Name[8];
	uint8_t Archive;
	uint16_t Size3, Size4;
	uint8_t *Contents;
	uint16_t Checksum;
} program_t;*/

#ifdef DZ8X_TI85FONT
/* Pulled from /lib/tilp/lpg/tilp/tilibs/libticonv/trunk/src/charset.c */
/*const unsigned long ti85_charset[256] = {
	'\0',   'b',   'o',     'd',    'h',   0x25b6, 0x2191, 0x2193,
	0x222b, 'x',   'A',     'B',    'C',   'D',    'E',    'F',

	0x221a, 180,    178,    0x2220, 176,    0x2b3,  0x22ba, 0x2264,
	0x2260, 0x2265, 0x2212, 0xd875dda4,0x2192,'?',  0x2191, 0x2193,

	' ',    '!',    '\"',   '#',    0x2074, '%',    '&',    '\'',
	'(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',

	'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',
	'8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',

	'@',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
	'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',

	'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
	'X',    'Y',    'Z',    '[',    '\\',   ']',    '^',    '_',

	'`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
	'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',

	'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
	'x',    'y',    'z',    '{',    '|',    '}',    '~',    '=',

	0x2080,	0x2081, 0x2082, 0x2083, 0x2084,	0x2085,	0x2086,	0x2087,
	0x2088,	0x2089,	192+1,  192+0,  192+2,  192+4,  224+1,  224+0,

	224+2,  224+4,  200+1,  200+0,  200+2,  200+4,  231+1,  231+0,
	231+2,  231+4,  204+1,  204+0,  204+2,  204+3,  236+1,  236+0,

	236+2,  236+3,  210+1,  210+0,  210+2,  210+4,  242+1,  242+0,
	242+2,  242+4,  217+1,  217+0,  217+2,  217+3,  249+1,  249+0,

	249+2,  249+3,  199,    231,    209,    204,    '\'',   '`',
	0x0a8,  0x0bf,  0x0a1,  0x3b1,  0x3b2,  0x3b3,  0x394,  0x3b4,

	0x3b5,  0x3b8,  0x3bb,  0x3bc,  0x3c0,  0x3c1,  0x3a3,  0x3c3,
	0x3c4,  0x3d5,  0x3a9,  'x',    'y',    '?',    0x2026, 0x25c0,

	0x25fe, '?',    0x2212, 178,    176,    179,    '\n',   0x26b6,
	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',

	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',
	'_',	'_',    '_',    '_',    '_',    '_',    '_',    '_',
};*/

char *TI85Font[] = {
	"\0","𝐛","𝐨","𝐝","𝐡","►","🡅","🡇","∫","×","𝐀","𝐁","𝐂","𝐃","𝐄","𝐅",
	"√","ˉ¹","²","∠","°","ʳ","ᵀ","≤","≠","≥","⁻","ᴇ","→","₁₀","↑","↓",
	" ","!","\"","#","$","%","&","'","(",")","*","+",",","-",".","/",
	"0","1","2","3","4","5","6","7","8","9",":",";","<","=",">","?",
	"@","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O",
	"P","Q","R","S","T","U","V","W","X","Y","Z","[","\\","]","^","_",
	"`","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o",
	"p","q","r","s","t","u","v","w","x","y","z","{","|","}","~","⌸",
	"₀","₁","₂","₃","₄","₅","₆","₇","₈","₉","Á","À","Â","Ä","á","à",
	"â","ä","É","È","Ê","Ë","é","è","ê","ë","Í","Ì","Î","Ï","í","ì",
	"î","ï","Ó","Ò","Ô","Ö","ó","ò","ô","ö","Ú","Ù","Û","Ü","ú","ù",
	"û","ü","Ç","ç","Ñ","ñ","´","`","¨","¿","¡","α","β","γ","Δ","δ",
	"ε","θ","λ","μ","π","ρ","Σ","σ","τ","Φ","Ω","x̄","ȳ","ˣ","…","◄",
	"▦","" ,"-","²","°","³","\n","🠺","█","","" ,"" ,"_","↥","" ,"",
	"░","" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,
	"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,"" ,""
};
#endif /*DZ8X_TI85FONT*/

#define isZ80ModePrefix(a) ((a)==0x40 || (a)==0x49 || (a)==0x52 || (a)==0x5B)
#define isixiyPrefix(a) (((a)|32)==0xFD) // ((a)==0xDD || (a)==0xFD)


/* I need to make a better way of doing this, ie, a subroutine */
char *DZ80[] = { // Base opcodes are the same in Z80/eZ80
	"nop=",             // 00
	"ld= bc, $######",
	"ld= (bc), a",
	"inc= bc",
	"inc= b",
	"dec= b",
	"ld= b, $@@",
	"rlca=",
	"ex= af, af'",
	"add= hl, bc",
	"ld= a, (bc)",
	"dec= bc",
	"inc= c",
	"dec= c",
	"ld= c, $@@",
	"rrca=",
	"djnz= $%%",        // 10
	"ld= de, $######",
	"ld= (de), a",
	"inc= de",
	"inc= d",
	"dec= d",
	"ld= d, $@@",
	"rla=",
	"jr= $%%",
	"add= hl, de",
	"ld= a, (de)",
	"dec= de",
	"inc= e",
	"dec= e",
	"ld= e, $@@",
	"rra=",
	"jr= nz, $%%",      // 20
	"ld= hl, $######",
	"ld= ($######), hl",// corrected h to hl Jan 1, 2019
	"inc= hl",
	"inc= h",
	"dec= h",
	"ld= h, $@@",
	"daa=",
	"jr= z, $%%",
	"add= hl, hl",
	"ld= hl, ($######)",
	"dec= hl",
	"inc= l",
	"dec= l",
	"ld= l, $@@",
	"cpl=",
	"jr= nc, $%%",      // 30
	"ld= sp, $######",
	"ld= ($######), a",
	"inc= sp",
	"inc= (hl)",
	"dec= (hl)",
	"ld= (hl), $@@",
	"scf=",
	"jr= c, $%%",
	"add= hl, sp",
	"ld= a, ($######)",
	"dec= sp",
	"inc= a",
	"dec= a",
	"ld= a, $@@",
	"ccf=",
	"ld= b, b", // 40 - 4040: noni.sis
	"ld= b, c",
	"ld= b, d",
	"ld= b, e",
	"ld= b, h",
	"ld= b, l",
	"ld= b, (hl)",
	"ld= b, a",
	"ld= c, b",
	"ld= c, c", // 49
	"ld= c, d",
	"ld= c, e",
	"ld= c, h",
	"ld= c, l",
	"ld= c, (hl)",
	"ld= c, a",
	"ld= d, b",         // 50
	"ld= d, c",
	"ld= d, d", // 52
	"ld= d, e",
	"ld= d, h",
	"ld= d, l",
	"ld= d, (hl)",
	"ld= d, a",
	"ld= e, b",
	"ld= e, c",
	"ld= e, d",
	"ld= e, e", // 5B
	"ld= e, h",
	"ld= e, l",
	"ld= e, (hl)",
	"ld= e, a",
	"ld= h, b",         // 60
	"ld= h, c",
	"ld= h, d",
	"ld= h, e",
	"ld= h, h",
	"ld= h, l",
	"ld= h, (hl)",
	"ld= h, a",
	"ld= l, b",
	"ld= l, c",
	"ld= l, d",
	"ld= l, e",
	"ld= l, h",
	"ld= l, l",
	"ld= l, (hl)",
	"ld= l, a",
	"ld= (hl), b",      // 70
	"ld= (hl), c",
	"ld= (hl), d",
	"ld= (hl), e",
	"ld= (hl), h",
	"ld= (hl), l",
	"halt=",
	"ld= (hl), a",
	"ld= a, b",
	"ld= a, c",
	"ld= a, d",
	"ld= a, e",
	"ld= a, h",
	"ld= a, l",
	"ld= a, (hl)",
	"ld= a, a",
	"add= a, b",        // 80
	"add= a, c",
	"add= a, d",
	"add= a, e",
	"add= a, h",
	"add= a, l",
	"add= a, (hl)",
	"add= a, a",
	"adc= a, b",
	"adc= a, c",
	"adc= a, d",
	"adc= a, e",
	"adc= a, h",
	"adc= a, l",
	"adc= a, (hl)",
	"adc= a, a",
	"sub= a, b",        // 90
	"sub= a, c",
	"sub= a, d",
	"sub= a, e",
	"sub= a, h",
	"sub= a, l",
	"sub= a, (hl)",
	"sub= a, a",
	"sbc= a, b",
	"sbc= a, c",
	"sbc= a, d",
	"sbc= a, e",
	"sbc= a, h",
	"sbc= a, l",
	"sbc= a, (hl)",
	"sbc= a, a",
	"and= a, b",        // A0
	"and= a, c",
	"and= a, d",
	"and= a, e",
	"and= a, h",
	"and= a, l",
	"and= a, (hl)",
	"and= a, a",
	"xor= a, b",
	"xor= a, c",
	"xor= a, d",
	"xor= a, e",
	"xor= a, h",
	"xor= a, l",
	"xor= a, (hl)",
	"xor= a, a",
	"or= a, b",         // B0
	"or= a, c",
	"or= a, d",
	"or= a, e",
	"or= a, h",
	"or= a, l",
	"or= a, (hl)",
	"or= a, a",
	"cp= a, b",
	"cp= a, c",
	"cp= a, d",
	"cp= a, e",
	"cp= a, h",
	"cp= a, l",
	"cp= a, (hl)",
	"cp= a, a",
	"ret= nz",          // C0
	"pop= bc",
	"jp= nz, $######",
	"jp= $######",
	"call= nz, $######",
	"push= bc",
	"add= a, $@@",
	"rst= $0",
	"ret= z",
	"ret=",
	"jp= z, $######",
	"", // CB
	"call= z, $######",
	"call= $######",
	"adc= a, $@@",
	"rst= $8",
	"ret= nc",          // D0
	"pop= de",
	"jp= nc, $######",
	"out= ($@@), a",
	"call= nc, $######",
	"push= de",
	"sub= a, $@@",
	"rst= $10",
	"ret= c",
	"exx=",
	"jp= c, $######",
	"in= a, ($@@)",
	"call= c, $######",
	"", // DD
	"sbc= a, $@@",
	"rst= $18",
	"ret= po",          // E0
	"pop= hl",
	"jp= po, $######",
	"ex= (sp), hl",
	"call= po, $######",
	"push= hl",
	"and= a, $@@",
	"rst= $20",
	"ret= pe",
	"jp= (hl)",
	"jp= pe, $######",
	"ex= de, hl",
	"call= pe, $######",
	"", // ED
	"xor= a, $@@",
	"rst= $28",
	"ret= p",           // F0
	"pop= af",
	"jp= p, $######",
	"di=",
	"call= p, $######",
	"push= af",
	"or= a, $@@",
	"rst= $30",
	"ret= m",
	"ld= sp, hl",
	"jp= m, $######",
	"ei=",
	"call= m, $######",
	"", // FD
	"cp= a, $@@",
	"rst= $38"          // FF
};

char *REG[] = { // "!" in next section. Same in Z80
	"b",
	"c",
	"d",
	"e",
	"h",
	"l",
	"(hl)",
	"a"
};

char *CB[] = { // Same in Z80
	"rlc= !", // rotate left circular
	"rrc= !", //   ``   right   ``
	"rl= !",  //   ``   left (logical)
	"rr= !",  //   ``   right   ``
	"sla= !", // shift  left arithmetic
	"sra= !", //   ``   right   ``
	"OPCODETRAP=", // sls in Z80
	"srl= !",
	"bit= 0, !",
	"bit= 1, !",
	"bit= 2, !",
	"bit= 3, !",
	"bit= 4, !",
	"bit= 5, !",
	"bit= 6, !",
	"bit= 7, !",
	"res= 0, !",
	"res= 1, !",
	"res= 2, !",
	"res= 3, !",
	"res= 4, !",
	"res= 5, !",
	"res= 6, !",
	"res= 7, !",
	"set= 0, !",
	"set= 1, !",
	"set= 2, !",
	"set= 3, !",
	"set= 4, !",
	"set= 5, !",
	"set= 6, !",
	"set= 7, !"
};

// s=short l=long : .l__ = long registers ._il = long data

char *xtraF[] = {
	"", // only here for display :/
	// <3 : 2 byte addresses
	".sis", // .s  40 Z80 control block (16-bit reg, 2 bytes of data), adl=0
	".lis", // .is 49 Z80 control block (24-bit reg, 2 bytes of data), adl=0
	// >2 : 3 byte addresses
	".sil", // .il 52 adl control block (16-bit reg, 3 bytes of data), adl=1
	".lil"  // .l  5B adl control block (24-bit reg, 3 bytes of data), adl=1 (default, I think...)
	// i.e. lis: `ld de.l, (0.s)`
};

char *ED[] = {
	"in0= b, ($@@)",        // 00
	"in0= ($@@), b",
	"lea= bc, ix+$**",
	"lea= bc, iy+$**",
	"tst= a, b",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ld= bc, (hl)",
	"in0= c, ($@@)",
	"out0= ($@@), c",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"tst= a, c",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ld= (hl), bc",
	"in0= d, ($@@)",        // 10
	"out0= ($@@), d",
	"lea= de, ix+$**",
	"lea= de, iy+$**",
	"tst= a, d",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ld= de, (hl)",
	"in0= e, ($@@)",
	"out0= ($@@), e",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"tst= a, e",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ld= (hl), de",
	"in0= h, ($@@)",        // 20
	"out0= ($@@), h",
	"lea= hl, ix+$**",
	"lea= hl, iy+$**",
	"tst= a, h",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ld= hl, (hl)",
	"in0= l, ($@@)",
	"out0= l, ($@@)",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"tst= a, l",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ld= (hl), hl",
	"OPCODETRAP=",           // 30
	"ld= iy, (hl)",
	"lea= ix, ix+$**",
	"lea= iy, iy+$**",
	"tst= a, (hl)",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ld= ix, (hl)",
	"in0= a, ($@@)",
	"out0= ($@@), a",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"tst= a, a",
	"OPCODETRAP=",
	"ld= (hl), iy",
	"ld= (hl), ix", // changed iy to ix on Jan 1, 2019
	"in= b, (bc)",          // 40 This is the first Z80 opcode seen in ED
	"out= (bc), b",
	"sbc= hl, bc",
	"ld= ($######), bc",
	"neg=",
	"retn=",
	"im= 0",
	"ld= i, a",
	"in= c, (bc)", // in/out (bc)s are only (c)s in Z80
	"out= (bc), c",
	"adc= hl, bc",
	"ld= bc, ($######)",
	"mlt= bc", // All mlt opcodes are neg in Z80
	"reti=",
	"OPCODETRAP=", // im 0
	"ld= r, a",
	"in= d, (bc)",          // 50
	"out= (bc), d",
	"sbc= hl, de",
	"ld= ($######), de",
	"lea= ix, iy+$**",
	"lea= iy, ix+$**",
	"im= 1",
	"ld= a, i",
	"in= e, (bc)",
	"out= (bc), e",
	"adc= hl, de",
	"ld= de, ($######)",
	"mlt= de",
	"OPCODETRAP=", // retn
	"im= 2",
	"ld= a, r",
	"in= h, (bc)",          // 60
	"out= (bc), h",
	"sbc= hl, hl",
	"ld= ($######), hl",
	"tst= a, $@@", // neg
	"pea= ix+$**",
	"pea= iy+$**",
	"rrd=",
	"in= l, (bc)",
	"out= (bc), l",
	"adc= hl, hl",
	"ld= hl, ($######)",
	"mlt= hl",
	"ld= mb, a", // retn  // mb is y default D0
	"ld= a, mb", // im 0
	"rld=",
	"OPCODETRAP=", // in f, (c) 70
	"OPCODETRAP=", // out= (c), f
	"sbc= hl, sp",
	"ld= ($######), sp",
	"tstio= $@@", // tsr? [neg in Z80]
	"OPCODETRAP=", // retn
	"slp=", // im 1
	"OPCODETRAP=", // ld i, i
	"in= a, (bc)",
	"out= (bc), a",
	"adc= hl, sp",
	"ld= sp, ($######)",
	"mlt= sp",
	"stmix=", // retn
	"rsmix=", // im 2
	"OPCODETRAP=", // ld r,r
	"OPCODETRAP=",           // 80
	"OPCODETRAP=",
	"inim=", // These don't exist in Z80...
	"otim=",
	"ini2=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"indm=",
	"otdm=",
	"ind2=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",           // 90
	"OPCODETRAP=",
	"inimr=",
	"otimr=",
	"ini2r=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"indmr=",
	"otdmr=",
	"ind2r=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ldi=", // and these do    A0
	"cpi=",
	"ini=",
	"outi=",
	"outi2=", // trap
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ldd=",
	"cpd=",
	"ind=",
	"outd=",
	"outd2=", // trap
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ldir=",                // B0
	"cpir=",
	"inir=",
	"otir=",
	"oti2r=", // trap
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"lddr=",
	"cpdr=",
	"indr=",
	"otdr=", // nothing below this point exists in Z80
	"otd2r=", // corrected r2->2r Sep 16, 2019
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",           // C0
	"OPCODETRAP=",
	"inirx=",
	"otirx=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ld= i, hl",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"indrx=",
	"otdrx=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",           // D0
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"ld= hl, i",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",           // E0
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=", // "FLASH_ERASE" for debugging in some emulators
	"OPCODETRAP=",
	"OPCODETRAP=",           // F0
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP=",
	"OPCODETRAP="            // FF
};

#endif /* !DZ8_h */
