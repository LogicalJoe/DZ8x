//
//  DZB.h
//  DZ8x
//
//  Created by LogicalJoe on 8/27/18.
//  Copyright © 2018-2020 LogicalJoe. All rights reserved.
//

#ifndef DZB_h
#define DZB_h

// Eventually I will stuff this into DZ8.h and make a function for the eZ80.

struct {
	char *Base, *BB;
} DZBA[] = {
	// Base         BB
	"", /*NULL*/    "npv(", // 00
	"►DMS",         "irr(",
	"►Dec",         "bal(",
	"►Frac",        "ΣPrn(",
	"→",            "ΣInt(",
	"Boxplot",      "►Nom(",
	"[",            "►Eff(",
	"]",            "dbd(",
	"{",            "lcm(",
	"}",            "gcd(",
	"ʳ",            "randInt(",
	"°",            "randBin(",
	"ˉ¹",           "sub(",
	"²",            "stdDev(",
	"ᵀ",            "variance(",
	"³",            "inString(",
	"(",            "normalcdf(", // 10
	")",            "invNorm(",
	"round(",       "tcdf(",
	"pxl-Test(",    "χ²cdf(",
	"augment(",     "Fcdf(",
	"rowSwap(",     "binompdf(",
	"row+(",        "binomcdf(",
	"*row(",        "poissonpdf(",
	"*row+(",       "poissoncdf(",
	"max(",         "geometpdf(",
	"min(",         "geometcdf(",
	"R►Pr(",        "normalpdf(",
	"R►Pθ(",        "tpdf(",
	"P►Rx(",        "χ²pdf(",
	"P►Ry(",        "Fpdf(",
	"median(",      "randNorm(",
	"randM(",       "tvm_Pmt", // 20
	"mean(",        "tvm_I%",
	"solve(",       "tvm_PV",
	"seq(",         "tvm_N",
	"fnInt(",       "tvm_FV",
	"nDeriv(",      "conj(",
	"", /*26*/      "real(", // What is this?
	"fMin(",        "imag(",
	"fMax(",        "angle(",
	" ",            "cumSum(",
	"\"",           "expr(",
	",",            "length(",
	"i",            "ΔList(",
	"!",            "ref(",
	"CubicReg",     "rref(",
	"QuartReg",     "►Rect",
	"0",            "►Polar", // 30
	"1",            "e",
	"2",            "SinReg ",
	"3",            "Logistic ",
	"4",            "LinRegTTest ",
	"5",            "ShadeNorm(",
	"6",            "Shade_t(",
	"7",            "Shadeχ²(",
	"8",            "ShadeF(",
	"9",            "Matr►list(",
	".",            "List►matr(",
	"ᴇ",            "Z-Test(",
	" or ",         "T-Test ",
	" xor ",        "2-SampZTest(",
	":",            "1-PropZTest(",
	"\n",           "2-PropZTest(",
	" and ",        "χ²-Test(", // 40
	"A",            "ZInterval ",
	"B",            "2-SampZInt(",
	"C",            "1-PropZInt(",
	"D",            "2-PropZInt(",
	"E",            "GraphStyle(",
	"F",            "2-SampTTest",
	"G",            "2-SampFTest",
	"H",            "TInterval",
	"I",            "2-SampTInt",
	"J",            "SetUpEditor ",
	"K",            "Pmt_End",
	"L",            "Pmt_Bgn",
	"M",            "Real",
	"N",            "re^θi",
	"O",            "a+bi",
	"P",            "ExprOn", // 50
	"Q",            "ExprOff",
	"R",            "ClrAllLists",
	"S",            "GetCalc(",
	"T",            "DelVar ",
	"U",            "Equ►String(",
	"V",            "String►Equ(",
	"W",            "Clear Entries",
	"X",            "Select(",
	"Y",            "ANOVA(",
	"Z",            "ModBoxplot",
	"θ",            "NormProbPlot",
	"", /*Matri*/   "",
	"", /*Lists*/   "",
	"", /*Equ*/     "",
	"prgm",         "",
	"", /*Pic*/     "", // 60
	"", /*GDB*/     "",
	"", /*grf&ICE*/ "",
	"", /*contgrf*/ "",
	"Radian",       "G-T",
	"Degree",       "ZoomFit",
	"Normal",       "DiagnosticOn",
	"Sci",          "DiagnosticOff",
	"Eng",          "Archive ",
	"Float",        "UnArchive ",
	"=",            "Asm(",
	"<",            "AsmComp(",
	">",            "AsmPrgm",
	"≤",            "AsmCmp", // BB6D (compiled asm)
	"≥",            "Á",
	"≠",            "À",
	"+",            "Â",  // 70
	"-",            "Ä",
	"Ans",          "á",
	"Fix ",         "à",
	"Horiz",        "â",
	"Full",         "ä",
	"Func",         "É",
	"Param",        "È",
	"Polar",        "Ê",
	"Seq",          "Ë",
	"IndpntAuto",   "é",
	"IndpntAsk",    "è",
	"DependAuto",   "ê",
	"DependAsk",    "ë",
	"Graph Format", "", /*7E*/
	"□",            "Ì",
	"﹢",           "Î", // 80
	"·",            "Ï",
	"*",            "í",
	"/",            "ì",
	"Trace",        "î",
	"ClrDraw",      "ï",
	"ZStandard",    "Ó",
	"ZTrig",        "Ò",
	"ZBox",         "Ô",
	"Zoom In",      "Ö",
	"Zoom Out",     "ó",
	"ZSquare",      "ò",
	"ZInteger",     "ô",
	"ZPrevious",    "ö",
	"ZDecimal",     "Ú",
	"ZoomStat",     "Ù",
	"ZoomRcl",      "Û", // 90
	"PrintScreen",  "Ü",
	"ZoomSto",      "ú",
	"Text(",        "ù",
	" nPr ",        "û",
	" nCr ",        "ü",
	"FnOn ",        "Ç",
	"FnOff ",       "ç",
	"StorePic ",    "Ñ",
	"RecallPic ",   "ñ",
	"StoreGDB ",    "´",
	"RecallGDB ",   "`",
	"Line(",        "¨",
	"Vertical ",    "¿",
	"Pt-On(",       "¡",
	"Pt-Off(",      "α",
	"Pt-Change(",   "β", // A0
	"Pxl-On(",      "γ",
	"Pxl-Off(",     "Δ",
	"Pxl-Change(",  "δ",
	"Shade(",       "ε",
	"Circle(",      "λ",
	"Horizontal ",  "μ",
	"Tangent(",     "|π",
	"DrawInv ",     "ρ",
	"DrawF ",       "Σ",
	"", /*Str*/     "",
	"rand",         "Φ",
	"π",            "Ω",
	"getKey",       "ṗ",
	"'",            "χ",
	"?",            "𝐅",
	"⁻",            "a", // B0
	"int(",         "b",
	"abs(",         "c",
	"det(",         "d",
	"identity(",    "e",
	"dim(",         "f",
	"sum(",         "g",
	"prod(",        "h",
	"not(",         "i",
	"iPart(",       "j",
	"fPart(",       "k",
	"", /*BB*/      "",
	"√(",           "l",
	"³√(",          "m",
	"ln(",          "n",
	"e^(",          "o",
	"log(",         "p", // C0
	"₁₀^(",         "q",
	"sin(",         "r",
	"sin⁻¹(",       "s",
	"cos(",         "t",
	"cos⁻¹(",       "u",
	"tan(",         "v",
	"tan⁻¹(",       "w",
	"sinh(",        "x",
	"sinh⁻¹(",      "y",
	"cosh(",        "z",
	"cosh⁻¹(",      "σ",
	"tanh(",        "τ",
	"tanh⁻¹(",      "Í",
	"If ",          "GarbageCollect",
	"Then",         "~",
	"Else",         "'", // D0 // Lol, Adriweb got this backwards :3
	"While ",       "@",
	"Repeat ",      "#",
	"For(",         "$",
	"End",          "&",
	"Return",       "‛",
	"Lbl ",         ";",
	"Goto ",        "\\",
	"Pause ",       "|",
	"Stop",         "_",
	"IS>(",         "%",
	"DS<(",         "…",
	"Input ",       "∠",
	"Prompt ",      "ß",
	"Disp ",        "ˣ",
	"DispGraph",    "ᴛ",
	"Output(",      "₀", // E0
	"ClrHome",      "₁",
	"Fill(",        "₂",
	"SortA(",       "₃",
	"SortD(",       "₄",
	"DispTable",    "₅",
	"Menu(",        "₆",
	"Send(",        "₇",
	"Get(",         "₈",
	"PlotsOn ",     "₉",
	"PlotsOff ",    "₁₀",
	"⌊",            "◄",
	"Plot1(",       "►",
	"Plot2(",       "↑",
	"Plot3(",       "↓",
	"", /*EF*/      "",
	"^",            "×", // F0
	"ˣ√",           "∫",
	"1-Var Stats ", "🡅",
	"2-Var Stats ", "🡇",
	"LinReg(a+bx) ","√",
	"ExpReg ",      "⌸", //  [inverted =]
	"LnReg ",       "", // F6 (F6-FF are all `setDate(` tokens)
	"PwrReg ",      "",
	"Med-Med ",     "",
	"QuadReg ",     "",
	"ClrList ",     "",
	"ClrTable",     "",
	"Histogram",    "",
	"xyLine",       "",
	"Scatter",      "",
	"LinReg(ax+b) ","", // FF
};

char *BASIC5E[] = {
	"Y₁",  // 10
	"Y₂",
	"Y₃",
	"Y₄",
	"Y₅",
	"Y₆",
	"Y₇",
	"Y₈",
	"Y₉",
	"Y₀",  // 19
	"X₁ᴛ", // 20
	"Y₁ᴛ",
	"X₂ᴛ",
	"Y₂ᴛ",
	"X₃ᴛ",
	"Y₃ᴛ",
	"X₄ᴛ",
	"Y₄ᴛ",
	"X₅ᴛ",
	"Y₅ᴛ",
	"X₆ᴛ",
	"Y₆ᴛ", // 2B
	"r₁", // 40
	"r₂",
	"r₃",
	"r₄",
	"r₅",
	"r₆", // 45
	"|u", // 80 use putchar(c-11) instead?
	"|v",
	"|w", // 83 (0x1E terms)
}; // 31 (0x1F)

struct {
	char *B62,*B63;
} BASIC6X[] = {
	"",            "ZXscl", // 00
	"RegEQ",       "ZYscl",
	"n",           "Xscl",
	"x̄",           "Yscl",
	"Σx",          "u(nMin)",
	"Σx²",         "v(nMin)",
	"[Sx]",        "Un-₁",
	"σx",          "Vn-₁",
	"[minX]",      "Zu(nMin)",
	"[maxX]",      "Zv(nMin)",
	"[minY]",      "Xmin", // ICE Start (62)
	"[maxY]",      "Xmax",
	"ȳ",           "Ymin",
	"Σy",          "Ymax",
	"Σy²",         "Tmin",
	"[Sy]",        "Tmax",
	"σy",          "θMin", // 10
	"Σxy",         "θMax",
	"[r]",         "ZXmin",
	"[Med]",       "ZXmax", // ICE End (62)
	"Q₁",          "ZYmin",
	"Q₃",          "ZYmax",
	"[|a]",        "Zθmin",
	"[|b]",        "Zθmax",
	"[|c]",        "ZTmin",
	"[|d]",        "ZTmax",
	"[|e]",        "TblStart",
	"x₁",          "PlotStart",
	"x₂",          "ZPlotStart",
	"x₃",          "nMax",
	"y₁",          "ZnMax",
	"y₂",          "nMin",
	"y₃",          "ZnMin", // 20
	"𝒏",           "∆Tbl",
	"[p]",         "Tstep",
	"[z]",         "θstep",
	"[t]",         "ZTstep",
	"χ²",          "Zθstep",
	"[|F]",        "∆X",
	"[df]",        "∆Y",
	"p̂",           "XFact",
	"p̂₁",          "YFact",
	"p̂₂",          "TblInput",
	"x̄₁",          "𝗡",
	"Sx₁",         "I%",
	"n₁",          "PV",
	"x̄₂",          "PMT",
	"Sx₂",         "FV",
	"n₂",          "|P/Y", // 30
	"Sxp",         "|C/Y",
	"lower",       "w(nMin)",
	"upper",       "Zw(nMin)",
	"[s]",         "PlotStep",
	"r²",          "ZPlotStep",
	"R²",          "Xres",
	"[factordf]",  "ZXres",
	"[factorSS]",  "TraceStep", // 38
	"[factorMS]",  "", // BASIC7E[] Picks up here
	"[errordf]",   "",
	"[errorSS]",   "",
	"[errorMS]",   ""  // 3C
};

char *BASIC7E[] = {
	"Sequential", // 0
	"Simul",
	"PolarGC",
	"RectGC",
	"CoordOn",
	"CoordOff",
	"Thick", // "Connected",
	"Dot-Thick", // "Dot",
	"AxesOn ",
	"AxesOff",
	"GridDot ", // "GridOn",
	"GridOff",
	"LabelOn",
	"LabelOff",
	"Web",
	"Time",
	"uvAxes", // 10
	"vwAxes",
	"uwAxes"
}; // next is BB

char *BASICEF[] = {
	"setDate(", // 00
	"setTime(",
	"checkTmr(",
	"setDtFmt(",
	"setTmFmt(",
	"timeCnv(",
	"dayOfWk(",
	"getDtStr(",
	"getTmStr(",
	"getDate",
	"getTime",
	"startTmr",
	"getDtFmt",
	"getTmFmt",
	"isClockOn",
	"ClockOff",
	"ClockOn", // 10
	"OpenLib(",
	"ExecLib",
	"invT(",
	"χ²GOF-Test(",
	"LinRegTInt ",
	"Manual-Fit ",
	"ZQuadrant1",
	"ZFrac1⁄2",
	"ZFrac1⁄3",
	"ZFrac1⁄4",
	"ZFrac1⁄5",
	"ZFrac1⁄8",
	"ZFrac1⁄10",
	"⬚",
	"", // 1F
	"", // 20
	"",
	"",
	"",
	"",
	"", // Why all of these blanks?
	"",
	"", // 27
	"",
	"",
	"",
	"",
	"",
	"", //2D
	"⁄",
	"ᵤ",
	"►n⁄d◄►Un⁄d", // 30
	"►F◄►D",
	"remainder(",
	"Σ(",
	"logBASE(",
	"randIntNoRep(",
	"",
	"MATHPRINT",
	"CLASSIC",
	"n⁄d",
	"Un⁄d",
	"AUTO",
	"DEC",
	"FRAC-APPROX",
	"", // Settings stuff I guess
	"STATWIZARD ON",
	"STATWIZARD OFF", // 40
	"BLUE",
	"RED",
	"BLACK",
	"MAGENTA",
	"GREEN",
	"ORANGE",
	"BROWN",
	"NAVY",
	"LTBLUE",
	"YELLOW",
	"WHITE",
	"LTGRAY",
	"MEDGRAY",
	"GRAY",
	"DARKGRAY",
	"Image1", // 50
	"Image2",
	"Image3",
	"Image4",
	"Image5",
	"Image6",
	"Image7",
	"Image8",
	"Image9",
	"Image0",
	"GridLine ",
	"BackgroundOn ",
	"", /*Matri*/ // 5C
	"", /*Lists*/
	"", /*Equ*/
	"", // prgm
	"", /*Pic*/ // 60
	"", /*GDB*/
	"", /*grf&ICE*/
	"", /*contgrf*/
	"BackgroundOff", // 64
	"GraphColor(",
	"QuickPlot&Fit-EQ",
	"TextColor(",
	"Asm84CPrgm",
	"Asm84CCmp", // 69
	"DetectAsymOn",
	"DetectAsymOff",
	"BorderColor ",
	"",
	"",
	"",
	"", // 70
	"",
	"",
	"·", // 73
	"Thin",
	"Dot-Thin",
	"", // 76
	"",
	"",
	"PlySmlt2", // 79
	// On-calc only, can be used to prevent TI-connect from opening programs
	// Exists in the earliest versions of TICE-OS (5.0.0.0089)
	"Asm84CEPrgm", // 7A
	"Asm84CeCmp", // if this gets displayed, there is a problem
	"", // 7C
	"",
	"", // Graph format
	"",
	"", // 80
	"Quartiles Setting…", // on-calc only
	"u(𝒏-2)",
	"v(𝒏-2)",
	"w(𝒏-2)",
	"u(𝒏-1)",
	"v(𝒏-1)",
	"w(𝒏-1)",
	"u(𝒏)",
	"v(𝒏)",
	"w(𝒏)",
	"u(𝒏+1)",
	"v(𝒏+1)",
	"w(𝒏+1)",
	"", // 8E
	"SEQ(𝒏)",
	"SEQ(𝒏+1)", // 90
	"SEQ(𝒏+2)",
	"LEFT",
	"CENTER",
	"RIGHT",
	"invBinom(",
	"Wait ",
	"toString(",
	"eval(", // 98
	"",
	"",
	"",
	"",
	"",
	"Execute Program", // I blame TI :3
	"Undo Clear", // 9F
	"Insert Line Above", // A0
	"Cut Line",
	"Copy Line",
	"Paste Line Below",
	"Insert Comment Above",
	"Quit Editor",
	"piecewise(" // A6
};

// BBE1 - BBE9, BBE0
#define SmallNumb0(a) DZBA[a==9?224:225+a].BB // I think this is right (With 0)
#define SmallNumb(a) DZBA[225+a].BB // (without 0)

char *ICETokens[] = {
	"DefineSprite(", // 620A
	"Call ",
	"Data(",
	"Copy(",
	"Alloc(",
	"DefineTilemap(",
	"CopyData(",
	"LoadData(",
	"SetBrightness(",
	"Compare(" // 6213
};

#endif /* !DZB_h */
