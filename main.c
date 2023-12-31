//
//  main.c
//  DZ8x
//
//  Created by LogicalJoe on 8/10/18.
//  Copyright © 2018-2021 LogicalJoe. All rights reserved.
//

/*Value DZ8X_MACRONAME // default | desc*/
#undef  DZ8X_TESTING   // 0 | Fake input
#undef  DZ8X_DEBUG     // 0 | Linenumbers in errors
#define DZ8X_LODEPNG   // 1 | Compile in LodePNG
#undef  DZ8X_UPDATELOG // 0 | Compile in updatelog
#undef  DZ8X_F         // 0 | Compile in functionality (-F) argument
#undef  DZ8X_TI85FONT  // 0 | Compile in (incomplete) 85 font
#define DZ8X_UNSQSHDASM// 1 | Automatically squish unsquished assembly
#define Stack_Max 200  //200| Maximum stack size

#define DZ8X_V "0.7.0"
#define TIBD_V "1.2.7"

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#include <stdio.h>    // printf() getc() FILE fopen() EOF fclose() fseek() putc() rewind() ftell() ...
#include <stdlib.h>   // uint8_t uint16_t uint32_t exit() atoi() srandomdev() random() ...
//nclude <math.h>     // floor()
//nclude <ctype.h>    // isdigit() tolower() toupper() isxdigit()
#include <time.h>     // clock_t clock() CLOCKS_PER_SEC
#include <stdbool.h>  // bool
#include "DZ8.h"      // MEM[] TYPE[] colors_t oscolors[] HEX[] DZ80[] CB[] ED[] ...
#include "DZB.h"      // DZBA[] BASIC(5E|6X|7E|EF)[] SmallNumb() ICETokens[]
#include "replace.h"  // repl_str()
#include "Build.h"    // DZ8X_BUILD
#include "updatelog.h"// Updates
#if defined (_WIN32) || defined (_WIN64) // Newlines are "\r\n"?
#include <windows.h>
#endif /* defined (_WIN32) || defined (_WIN64) */

#ifdef _CTYPE_H_
#define dz_isdigit(a)      isdigit(a) // Used once in detecting -P first chardigit name
#define dz_isxdigit(a)     isxdigit(a)// Used once to detect hex files and once to read hex files (/commandline)
#define dz_tolower(a)      tolower(a) // Used once to detect the extension and once to print the extension
#define dz_toupper(a)      toupper(a) // Used once to read the contents of hex files and commandline
#else /*_CTYPE_H_*/
// Who needs ctype.h?  just use these awesome ascii (utf-7)-only macros
//  that I totally got off of overflow and did not make on the spot! :P
// dz- is removed from comments for ease of searching code
//efine dz_isalnum(a)     (('0'<=(a) && '9'>=(a)) || ('A'<=(a) && 'Z'>=(a)) || ('a'<=(a) && 'z'>=(a)))
//efine dz_isalpha(a)     (('A'<=(a) && 'Z'>=(a)) || ('a'<=(a) && 'z'>=(a)))
//efine dz_isblank(a)      ('\t'==(a) || ' '==(a))
//efine dz_iscntrl(a)      (0x1F<=(a) || 0x7F==(a))
#define dz_isdigit(a)      ('0'<=(a) && '9'>=(a))
//efine dz_isgraph(a)      ('!'<=(a) && '~'>=(a))
//efine dz_islower(a)      ('a'<=(a) && 'z'>=(a))
//efine dz_isprint(a)      ('!'<=(a) && '~'>=(a))
//efine dz_ispunct(a)     (('!'<=(a) && '/'>=(a)) || (':'<=(a) && '@'>=(a)) || ('['<=(a) && '`'>=(a)) || ('{'<=(a) && '~'>=(a)))
//efine dz_isspace(a)    (('\t'<=(a) && '\r'>=(a)) || ' '==(a))
//efine dz_isupper(a)      ('A'<=(a) && 'Z'>=(a))
#define dz_isxdigit(a)    (('0'<=(a) && '9'>=(a)) || ('A'<=(a) && 'F'>=(a)) || ('a'<=(a) && 'f'>=(a)))
#define dz_tolower(a)      ((a)+32*('A'<=(a) && 'Z'>=(a)))
#define dz_toupper(a)      ((a)-32*('a'<=(a) && 'z'>=(a)))
//efine dz_isascii(a)      (!(a&~0x7F))
//efine dz_toascii(a)      (a&0xFF)
#endif /*_CTYPE_H_*/
#define dz_isupnum(a)     (('0'<=(a) && '9'>=(a)) || ('A'<=(a) && 'Z'>=(a)))
#define dz_isxupper(a)    (('0'<=(a) && '9'>=(a)) || ('A'<=(a) && 'F'>=(a)))

#define LODEPNG_NO_COMPILE_DECODER
#define LODEPNG_NO_COMPILE_ANCILLARY_CHUNKS // Not compiling the decoder anyway
#ifndef DZ8X_LODEPNG // Haha, nope :P
#define LODEPNG_NO_COMPILE_ZLIB
#define LODEPNG_NO_COMPILE_PNG
#define LODEPNG_NO_COMPILE_ENCODER
#define LODEPNG_NO_COMPILE_DISK
#define LODEPNG_NO_COMPILE_ERROR_TEXT
#define LODEPNG_NO_COMPILE_ALLOCATORS
#else /*DZ8X_LODEPNG*/
#include "lodepng.h" // For a difference with Xcode, Dead Code Stripping must be on.
#endif /*DZ8X_LODEPNG*/

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


#ifdef _WIN32
#define DZ8X_RESET   0
#define DZ8X_HONEY   FOREGROUND_GREEN | FOREGROUND_RED // Disassembly
#define DZ8X_BLUE    FOREGROUND_BLUE | FOREGROUND_GREEN // Hexadecimal
#define DZ8X_WHITE   FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY // Debug
#define DZ8X_RED     FOREGROUND_RED | FOREGROUND_INTENSITY // Errors
#define DZ8X_GREEN   FOREGROUND_GREEN | FOREGROUND_INTENSITY // PASSES
#define DZ8X_YELLOW  FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY // Warnings
#define DZ8X_GRAY    FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY // Information
// https://www.mailsend-online.com/blog/setting-windows-console-text-colors-in-c.html
// This should work? (/spasm-ng-0.5-beta.3/console.cpp)
#define colorf(c) {\
if (c)\
  SetConsoleTextAttribute(GetStdHandle (STD_OUTPUT_HANDLE), c);\
else\
  SetConsoleTextAttribute(GetStdHandle (STD_OUTPUT_HANDLE), saved_attributes);\
}
uint32_t saved_attributes;

#define errorNL(c,...)  {printf("\n");colorf(DZ8X_RED);   printf("ERROR: "c,##__VA_ARGS__);  colorf(DZ8X_RESET);printf("\n");exit(1);}
#define errorf(c,...)   {             colorf(DZ8X_RED);   printf("ERROR: "c,##__VA_ARGS__);  colorf(DZ8X_RESET);printf("\n");exit(1);}
#define warningf(c,...) {             colorf(DZ8X_YELLOW);printf("WARNING: "c,##__VA_ARGS__);colorf(DZ8X_RESET);printf("\n");}
#define notef(c,...)    {             colorf(DZ8X_YELLOW);printf("WARNING: "c,##__VA_ARGS__);colorf(DZ8X_RESET);printf("\n");}

#define copyright "\xa9"

#else

/* Using defines will break STR() :/ */
#define DZ8X_RESET   0
#define DZ8X_HONEY   33 // Disassembly
#define DZ8X_BLUE    36 // Hexadecimal
#define DZ8X_GRAY    37 // Information
#define DZ8X_RED     91 // Errors
#define DZ8X_GREEN   92 // PASSES
#define DZ8X_YELLOW  93 // Warnings
#define DZ8X_WHITE   97 // Debug
#define scolor(c)        "\e["STR(c)"m"
#define colorf(c)        printf(scolor(c))

#ifdef DZ8X_DEBUG
#define errorNL(c,...)  {printf("\n"scolor(91)"ERROR ("STR(__LINE__)"): "c scolor(0)"\n",##__VA_ARGS__);exit(1);}
#define errorf(c,...)   {printf(    scolor(91)"ERROR ("STR(__LINE__)"): "c scolor(0)"\n",##__VA_ARGS__);exit(1);}
#else
#define errorNL(c,...)  {printf("\n"scolor(91)"ERROR: "c scolor(0)"\n",##__VA_ARGS__);exit(1);}
#define errorf(c,...)   {printf(    scolor(91)"ERROR: "c scolor(0)"\n",##__VA_ARGS__);exit(1);}
#endif
#define warningf(c,...) {printf(    scolor(93)"WARNING: "c scolor(0)"\n",##__VA_ARGS__);}
#define notef(c,...)    {printf(    scolor(93)"NOTE: "c scolor(0)"\n",##__VA_ARGS__);}

#define copyright "©"

#endif

#define squareroot "√"
#define theta "θ"


void info(void);
void info(void) { // 80x24 window default
    printf("DZ8x - eZ80 Disassembler and 8x (un)packager by LogicalJoe\n"
           "DZ8x    v"DZ8X_V" - "copyright"LogicalJoe - Aug 10 2018 - Mar 11 2022\n"
           "TI-BSD  v"TIBD_V" - "copyright"LogicalJoe - Aug 27 2018 - Oct  5 2018\n"
#ifdef DZ8X_LODEPNG
           "LodePNG v%s - "copyright"Lode Vandevenne\n", LODEPNG_VERSION_STRING // Is there a way around this?
#endif /*DZ8X_LODEPNG*/
           );
    printf("DZ8x Build "STR(DZ8X_BUILD)" - "__DATE__" @ "__TIME__
#ifdef __STDC_VERSION__
           " | C"STR(__STDC_VERSION__)
#endif /*__STDC_VERSION__*/
#if defined (_WIN32) || defined (_WIN64)
           "" // custom build info here
#endif
#ifdef __VERSION__ // Please see `gcc -dM -E - < /dev/null`
           "\nCompiled with %s\n", __VERSION__ // using STR includes quotes
#endif /*__VERSION__*/
           );
    /*#ifdef __GNUC__
     printf("Compiled with gcc "STR(__GNUC__)"."STR(__GNUC_MINOR__)
     #ifdef __GNUC_PATCHLEVEL__
     "."STR(__GNUC_PATCHLEVEL__)
     #endif //__GNUC_PATCHLEVEL__//
     "\n");
     #endif //__GNUC__//
     #ifdef __clang__
     printf("Compiled with clang "STR(__clang_major__)"."STR(__clang_minor__)"."STR(__clang_patchlevel__)"\n");
     #endif //__clang__*/
#if !defined (DZ8X_LODEPNG) || defined (DZ8X_UPDATELOG) || defined (DZ8X_UNSQSHDASM) || defined (_WIN32) || defined (_WIN64) || !defined (_CTYPE_H_)
    printf("Build Configuration:"
#ifndef DZ8X_LODEPNG
           " -LodePNG"
#endif /* !DZ8X_LODEPNG */
#ifdef DZ8X_UPDATELOG
           " +UpdateLog"
#endif /*DZ8X_UPDATELOG*/
#ifdef DZ8X_UNSQSHDASM
           " +UnsquishedAsm"
#endif /* !DZ8X_UNSQSHDASM */
#ifndef _CTYPE_H_
           " -ctype"
#endif /* !_CTYPE_H_ */
#if defined (_WIN32) || defined (_WIN64)
           " 4Windows"
#endif /* defined (_WIN32) || defined (_WIN64) */
           "\n");
#endif /* !defined (DZ8X_LODEPNG) || defined (DZ8X_UPDATELOG) || !defined (DZ8X_UNSQSHDASM) || defined (_WIN32) || defined (_WIN64) || !defined (_CTYPE_H_) */
}

uint32_t PLACE2 = 0; // I define PLACE2 out here so that is a different color, even though it should be local

/* unsigned DEC->HEX converter, (c) LogicalJoe */
char *HexConv(unsigned int c) {
    return repl_str(repl_str(repl_str("-!@","-",c>>8?HexConv(c>>8):""),"!",HEX[c>>4&15]),"@",HEX[c&15]);
}
// For printing?: printf("%X",a);

/* 8-bit hex converter (copyleft) LogicalJoe */
//char*Conv8bHex(int c){return repl_str(repl_str("!@","!",HEX[c>>4]),"@",HEX[c&15]);}
/* Old hex converer */
//char*HexConv(int c){return repl_str(repl_str("-+","-",c>>8?HexConv(c>>8):""),"+",repl_str(repl_str("!@","!",HEX[c>>4&15]),"@",HEX[c&15]))};

/* This Routine was shamelessly ripped from calc84maniac's TI-Boy CE Rom converter */
bool has_extension(const char *filename, const char *extension) {
    size_t i, name_len = strlen(filename), ext_len = strlen(extension);
    if (name_len < ext_len)  return 0;
    for (i = name_len - ext_len; i < name_len; i++)
        if (dz_tolower(filename[i]) != *extension++)// && '?' != *extension++)
            return 0;
    return 1;
}

bool FileExt(char* filename,char* Ext) {
    char *E[] =
    {".8xc",".8xd","8xg",".8xi",".8xl",".8xm",".8xn",".8xo",".8xp",".8xs",".8xt",".8xw",".8xy",".8xz"};
    uint8_t a=14;
    for (;a--;)
        if (has_extension(filename,repl_str(E[a], "8x", Ext)))
            return 1;
    return 0;
}

bool EightXFile(char *filename) {
    return has_extension(filename,".8xp") // Program
    || has_extension(filename,".8xl") // List
    || has_extension(filename,".8xm") // Matrix
    || has_extension(filename,".8xn") // Number
    || has_extension(filename,".8xv") // appVar
    || has_extension(filename,".8xs") // String
    || has_extension(filename,".8xg") // Uncompressed Group
    || has_extension(filename,".8cg") // Self-extracting Group
    || has_extension(filename,".8ca") // Color bAckground
    || has_extension(filename,".8ci") // Color pIcture
    || has_extension(filename,".8xi") // pIcture (This is meant for "84" files, not "83" files?)
    || has_extension(filename,".8xt") // Table set
    || has_extension(filename,".8xy") // Y-var
    || has_extension(filename,".8xz") // Zoom settings
    || has_extension(filename,".8xw") // Window
    || has_extension(filename,".8xc") // Complex
    || has_extension(filename,".8xo") // tilp group (identical to .8cg)
    || has_extension(filename,".8xd");// graph Data base
}
bool PackagingFormat(char *filename) { // This is really bad, I need to remove it at some point
    return EightXFile(filename) ||
    FileExt(filename,"73") ||
    FileExt(filename,"82") ||
    FileExt(filename,"83") ||
    FileExt(filename,"85") ||
    FileExt(filename,"86");
}
#define isapp(a) (has_extension(a,".8xk") || has_extension(a,".8ck") || has_extension(a,".8ek"))
#define isos(a)  (has_extension(a,".8xu") || has_extension(a,".8cu") || has_extension(a,".8eu") || has_extension(a,".8pu") || has_extension(a,".8yu") || has_extension(a,".82u"))
// x : mono
// c : CSE
// e : 84CE
// p : 83CE [only OS]
// y : 82AEP [only OS]
// 2 : 82A

// With colorf : 0 33 36 37 91 92 93 97 (200 results)
// With scolor : 0          91    93    ( 12 results)
// With \e     :                        (  1 result )


/*void EndTime(struct timeb time_start) {
 struct timeb time_end; ftime(&time_end);
 int s_diff = (int) (time_end.time - time_start.time), ms_diff = time_end.millitm - time_start.millitm;
 if      (ms_diff < 0   ) {ms_diff+=1000; s_diff-=1;}
 else if (ms_diff > 1000) {ms_diff-=1000; s_diff+=1;}
 printf("Time spent: %0.3f seconds\n", (float) s_diff + ((float) ms_diff / 1000.0f));
 }*/

#define EndTime(time_start) printf("Elapsed time: %f\n",((double)clock()-(time_start))/CLOCKS_PER_SEC)

/* Here for historic reasons only.
 void TIOS_Number(FILE *in, bool cmplx, bool disp) {
 // from merthsoft.com: If bits 2 and 3 == 1 && bit 1 == 0, it is half of a complex variable.
 uint8_t c,f=getc(in);printf("%s",(f&0b10000000)?"-":"+");printf("(10^%i)*",getc(in)-0x80);c=getc(in);
 printf("%s.%s",HEX[c>>4],HEX[c&15]);printf("%s%s%s%s%s%s%s\n",
 HexConv(getc(in)),HexConv(getc(in)),HexConv(getc(in)),HexConv(getc(in)),HexConv(getc(in)),HexConv(getc(in)),disp?"i":"");
 if (f&0b01110001) warningf("Unknown bits used");if (f&0b00000010) warningf("Value undefined");
 else {if (!cmplx && ((f&0b00001100)==0b00001100)) errorf("Complex number in a real var");
 if ( cmplx && ((f&0b00001100)!=0b00001100)) errorf("Real number in a complex var");}}*/

/**
 TI-OS number decoder directly from FILE
 
 @param in    file in
 @param imag  Display imaginary portion of number (system only, input should always be 0)
 
 TODO: Make a special fraction decoder
 * int G=100000000000000/gcd(100000000000000*F,100000000000000);
 * printf("%i/%i\n",F*G,G);
 */
void TIOS_Number(FILE *in, bool imag) {
    uint8_t c,f=getc(in),KIND2;
    colorf(DZ8X_GRAY); // because the color is not set before displaying the number :/
    KIND2 = f&0x7F;
    if (!(KIND2&0b01110000))
        KIND2&=0xC;
    //printf("  %s  ",HexConv(KIND2));
    // What the H-E-double hockysticks TI?
    if ((KIND2==0x1C || KIND2==0x1D) && KIND2==f) { // Real/Complex Radicals
        // https://docs.google.com/document/d/1P_OUbnZMZFg8zuOPJHAx34EnwxcQZ8HER9hPeOQ_dtI
        uint8_t Array[8];
        for (c=0;c<8;c++)
            Array[c] = getc(in); // 0 eee ccc aaa ddd bbb  |  ( +- a * √(b) +- c * √(d) ) / e
        printf("(%s %s * "squareroot"(%s) %s %s * "squareroot"(%s) ) / %s",
               16&Array[0]?" -":"",
               HexConv(((Array[3]&15)<<8)|Array[4]),
               HexConv(((Array[6]&15)<<8)|Array[7]),
               32&Array[0]?"-":"+",
               HexConv((Array[2]<<4)|(Array[3]>>4)),
               HexConv((Array[5]<<4)|(Array[6]>>4)),
               HexConv(((Array[0]&15)<<8)|Array[1]));
    } else {
        printf("%s(10^%i)*",(f&128)?"-":"+",getc(in)-128);
        c=getc(in);
        printf("%s.%s%s%s%s%s%s%s", HEX[c>>4],HEX[c&15], // Using HEX because values A-F are, in fact, "valid"
               HexConv(getc(in)),HexConv(getc(in)),HexConv(getc(in)),
               HexConv(getc(in)),HexConv(getc(in)),HexConv(getc(in)));
    }
    /* !! NOTE: DOESN'T OUTPUT π ON REAL PARTS OF THE COMPLEX PI FRACTION !! *
     *       Which apparently have an internal kind of 0xC? :thinking:       */
    if ((imag && KIND2==0x1E) || KIND2==0x1F || KIND2==0x20 || KIND2==0x21)
        printf("pi");
    if ((KIND2==0xC && !(f&2)) || KIND2==0x1B || KIND2==0x1D || KIND2==0x1E || KIND2==0x1F) {
        // from merthsoft.com: If bits 2 and 3 == 1 && bit 1 == 0, it is half of a complex variable.
        if (imag) printf("%si\n",KIND2==0x1D?" * ":"");
        else {
            if (KIND2==0x1D) printf(" + ");
            TIOS_Number(in, 1);
        }
    } else printf("\n");
    if (!KIND2 || KIND2==0xC) {
        if (f&2)
            warningf("Value undefined");
        if (f&1) // (tivars_lib_cpp/src/TypeHandlers/STH_FP.cpp)
            warningf("Initial sequence value");
    }
    colorf(DZ8X_RESET);
}
/* For this to be of any use, KIND itself must be fed in and I am not willing to do that right now.
 else {
 if (KIND2==0xC && !(f&0xC)) warningn("Complex number in a real var");
 if (KIND2==0x0 && (f&0xC)) warningn("Real number in a complex var");
 }*/
/* KIND2  KIND
 * 0x 0   0x 0 // Real
 * 0x 0   0x 1 // Real list
 * 0x 0   0x 2 // Matrix
 * 0x C   0x C // Complex
 * 0x C   0x D // Complex list
 * 0x 0   0x F // Window
 * 0x 0   0x10 // Zoom
 * 0x 0   0x11 // TblSet
 * 0x18   0x18 // Fraction
 * 0x1B   0x1B // Complex fraction
 * 0x1C   0x1C // Real radical
 * 0x1D   0x1D // Complex radical
 * 0x1E   0x1E // Complex pi
 * 0x1F   0x1F // Complex pi fraction
 * 0x20   0x20 // Real pi
 * 0x21   0x21 // Real pi fraction
 */

int main(int argc, char** argv) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
#endif
#ifdef DZ8X_TESTING
    // This is in general a bad idea.
    char *arg2[]={"DZ8x","-i","/Users/logicaljoe/Desktop/a.8xp"}; // how can I avoid this?
    argc=3;
    argv=arg2;
    colorf(DZ8X_WHITE);
    printf("TESTING MODE\n");
#endif /*DZ8X_TESTING*/

    colorf(DZ8X_RESET); // Correct color before anything can go wrong

    if (argc > 1) {
        if (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")) {
            info();
            //intf("--------------------------------------------------------------------------------");
            printf("\nUsage: %s [option] <arguments>\n",argv[0]);
            printf("\nOptions:\n"
                   "  -D <input> (<output>)\t\tSimple Disassemble <input> to <output>\n"
#ifndef _WIN32
                   "  -R [number] <output>\t\t[number] Random nybbles in <output.bin/hex>\n"
#endif
                   "  -X <input> <output>\t\tEXtract <input.8**> to <output.bin/hex>\n"
                   "  -P <input> <output> [status] name\tPackage bin/hex <input> to <output.8xp>\n"
                   "  -Z <input> (<output>)\t\tFull Disassemble <input> to <output>\n"
                   "  -Zbug <input> (<output>)\tDebug version of -Z\n"
                   "  -C [hex]\t\t\tDisassemble [hex] from Command line\n"
                   "  -Q [number]\t\t\tConvert dec [number] to hex\n"
                   "  -h, --help\t\t\tDisplay this Helpful usage message!\n"
                   "\nNote: Options are CASE-sensitive (for now)\n"
                   "Supported read  formats: bin hex boot 8ca 8cg 8ci\n"
                   "  8xc 8xd 8xg 8xi 8xl 8xm 8xn 8xo 8xp 8xs 8xt 8xv 8xw 8xy 8xz\n"
                   "  83c 83d 83g 83i 83l 83m 83n 83p 83s 83t 83w 83y 83z\n"
                   "  85c 85d 85i 85l 85m 85n 85p 85s 85w 85y 85z\n"
                   "  86c 86d 86i 86l 86m 86n 86p 86s 86w 86y 86z\n"
                   "Supported write formats: -P ->8xp | -X 8**->bin/hex"
#ifdef DZ8X_LODEPNG
                   " | -D 8ca/8ci/8xi->png"
#endif /*DZ8X_LODEPNG*/
                   "\nSupported platforms: TI-73/82/83/84/85/86\n"
                   "Output colors:\n");
            colorf(DZ8X_GRAY);  printf("\tInformation\n");
            colorf(DZ8X_HONEY); printf("\tDisassembly\n");
            colorf(DZ8X_BLUE);  printf("\tHexadecimal\n");
            colorf(DZ8X_YELLOW);printf("\tWarnings ");
            colorf(DZ8X_WHITE); printf("Debug\n");
            colorf(DZ8X_RED);   printf("\tErrors   FAILS\n");
            colorf(DZ8X_RESET); printf("\tDefault  ");
            colorf(DZ8X_GREEN); printf("PASSES\n");
            colorf(DZ8X_RESET);
            printf("To suggest new functionality, email my handle initials at my handle dot com\n");
            return 0;
        }
        if (!strcmp(argv[1],"-i") || !strcmp(argv[1],"--info") || !strcmp(argv[1],"-v") || !strcmp(argv[1],"--version")) {
            info();
            return 0;
        }
#ifdef DZ8X_UPDATELOG
        if (!strcmp(argv[1],"-u") || !strcmp(argv[1],"--updatelog")) {
            info();
            printf("%s", Updates);
            return 0;
        }
#endif /*DZ8X_UPDATELOG*/
#ifdef DZ8X_F
        if (!strcmp(argv[1],"-F")) {
            for (uint8_t a=0;a<=0x3E;a++)
                if (strcmp(TYPE[a].Ext,"")||strcmp(TYPE[a].Desc,""))
                    printf("%s | %s | %s | %s\n",TYPESUP[a]?squareroot:" ",HexConv(a),strcmp(TYPE[a].Ext,"")?TYPE[a].Ext:"    ",TYPE[a].Desc);
            return 0;
        }
#endif /*DZ8X_F*/
        /*
         if(!strcmp(argv[1],"-T")){clock_t start_time=clock();
         printf("Timer started, press enter to stop...");
         getchar();EndTime(start_time);return 0;}
         */

        if(!strcmp(argv[1],"-Q")){if(argc<3)errorf("Need number");
            printf("%i == 0x%s\n",atoi(argv[2]),HexConv(atoi(argv[2])));
            return 0;}

#ifndef _WIN32
        if (!strcmp(argv[1],"-R")) {
            /* This has two different modes:
             * Binary (.bin)
             * Ascii  (.hex)
             */
            printf("Scanning arguments...\n");
            if (argc<3)
                errorf("No number");
            if (argc<4)
                errorf("No output file");
            if (atoi(argv[2])<0)
                errorf("Cannot handle your input");
            if (atoi(argv[2])>1000000000)
                errorf("> 1 gigabyte of data requested");
            bool ascii=1;
            if (has_extension(argv[3],".bin"))
                ascii=0;
            else if (!has_extension(argv[3],".hex"))
                errorf("-R only outputs to .bin and .hex files");
            int l = atoi(argv[2]);
            if (!ascii && l&1)
                errorf("Even number required in binary mode");
            FILE *out = fopen(argv[3], "w");
            if (!out)
                errorf("Output could not be created");

            srandomdev();
            uint32_t a; // ~4,294,967,295
            /*-for 1 gigabyte (1,000,000,000 bytes),
             *  [not to be confused with 1 gibibyte (1,073,741,824 bytes) of hex]
             * -fprintf took 2:12,
             * -fputs took 1:18, &
             * -putc took 1:00
             *-for 10,000,000,000 (10 gigabytes) of hex,
             * -putc took 488.585s
             */

            clock_t start_time=clock();
            printf("Outputting to %s file...\n",has_extension(argv[3],".bin")?"bin":"hex");
            if (ascii) // write to hex
                for (a=l;a--;)
                    putc((int)*HEX[random()&15],out); // eliminated hexprob[] :D
            else // write to bin
                for (a=l+2;a-=2;)
                    putc(random()&255,out);
            fclose(out);
            printf("Output Complete.\n");
            printf("%i nybble%s stored to %s\n",l,l==1?"":"s", argv[3]);
            EndTime(start_time);

            exit(0);
        }
#endif
        if (!strcmp(argv[1],"-P")) {
            // dz8x -P <input> <output> [archive] name
            uint8_t a;
            printf("Scanning arguments...\n");
            if (argc < 6)
                errorf("-P must have 4 arguments");
            //if (!has_extension(argv[2],".bin") && !has_extension(argv[2],".hex"))
            //	errorf("-P only excepts bin and hex input files");
            if (!has_extension(argv[3],".8xp"))
                errorf("-P only outputs to 8xp files");
            if (atoi(argv[4])!=1 && atoi(argv[4])!=0)
                errorf("Archive status must be boolean");
            if (strlen(argv[5])>8)
                errorf("Program name must be <= 8 chars");
            for (a=(int)strlen(argv[5]);a--;)
                if (!dz_isupnum(argv[5][a]) || argv[5][a]==0x5B)
                    errorf("Program name must be (capital) alphanumeric");
            //if (!isupper(argv[5][0]))
            if (dz_isdigit(argv[5][0]))
                errorf("Program name cannot start with a number");
        }
        if (!strcmp(argv[1],"-X")) {
            printf("Scanning arguments...\n");
            if (argc<4)
                errorf("-X must have 2 file arguments");
            if (!PackagingFormat(argv[2]))
                errorf("-X only excepts packaged input files");
            if (!has_extension(argv[3],".bin") && !has_extension(argv[3],".hex"))
                errorf("-X only outputs to .bin and .hex files");
        }
        if (!strcmp(argv[1],"-C") || !strcmp(argv[1],"-D") ||
            !strcmp(argv[1],"-Z") || !strcmp(argv[1],"-Zbug") ||
            !strcmp(argv[1],"-P") || !strcmp(argv[1],"-X")) {
            bool DEBG=!strcmp(argv[1],"-Zbug"),
            Cline=    !strcmp(argv[1],"-C"),
            Tz80=     !strcmp(argv[1],"-Z") || DEBG,
            Extract=  !strcmp(argv[1],"-X"),
            Package=  !strcmp(argv[1],"-P"),
            op=0,Group=0,ascii=1;
            // Group can probably go out of the window soon
            uint8_t KIND=5,FILETYPE=84; // 84 Program by default
            uint16_t Size02=0;
            int c,d=0,FILESIZE=0; // d can be > 0xFFFF whenever apps become supported
            // c was originally intended to be an uint8_t, but has expanded into uint16_t, maybe have c8 and c16?
            FILE *in=NULL, *out=NULL;
            if (!Extract && !Package)
                printf("Scanning arguments...\n"); // just here because I like it
            /* Start clock */
            clock_t start_time=clock();

            if (Cline) {
                if (argc<3)
                    errorf("-C requires a hex string");
                d=(int)strlen(argv[2]);
                if (d>512) // Trying to make responsible users
                    errorf("Please put that data in a file");
            } else {
                uint16_t siz;
                if (argc<3)
                    errorf("No input file");
                if (!(in=fopen(argv[2],"rb")))
                    errorf("Input does not exist");
                // workaround to using a bunch of extra headers,
                // should not work on anything other than mac
                fseek(in,0,SEEK_END);
                siz=(int)ftell(in);
                rewind(in);
                if (!fopen(argv[2],"a") && siz && getc(in)==EOF)
                    errorf("Input is a directory");
                printf("Input: ");
                colorf(DZ8X_GRAY);
                printf("%s\n",argv[2]);
                colorf(DZ8X_RESET);
                if (argc>3) {
                    // This is so confusing TvT
                    if (fopen(argv[3],"ax")) {
                        op=out = fopen(argv[3],"w");
                        printf("Output: ");
                        colorf(DZ8X_GRAY);
                        printf("%s\n",argv[3]);
                    } else if (!strcmp(argv[2],argv[3]))
                        warningf("Input and Output are identical")
                    else if (fopen(argv[3],"a"))
                        warningf("Output already exists")
                    else if (fopen(argv[3],"rb")) {
                        out = fopen(argv[3],"rb");
                        fseek(out,0,SEEK_END);
                        siz=(int)ftell(out);
                        rewind(out);
                        if (siz && getc(out)==EOF)
                            warningf("Output is a directory")
                        else
                            warningf("Output already exists (Also check permissions)")
                        fclose(out);
                    } else
                        warningf("Output creation failed (Check permissions)")
                } else if (Tz80) notef("-Z recommends an output");

                colorf(DZ8X_RESET);
                if (PackagingFormat(argv[2])) { // Can I do this in the big if statment?
                    fseek(in,ascii=0,SEEK_END);
                    FILESIZE=(int)ftell(in)<<1;
                    printf("Full file size: ");
                    colorf(DZ8X_GRAY);
                    printf("%i bytes\n",FILESIZE>>1);
                    if (FILESIZE <= 0x4B)
                        errorf("Input file is too small");
                } else if (isapp(argv[2]) || isos(argv[2])) {
                    // Nothing here
                } else {
                    fseek(in,0,SEEK_END);
                    if (0xFFFFFF < ftell(in)) // 16 MiB (~16 MB)
                        errorf("Take that file elsewhere, I don't want to read it.");
                    fseek(in,0,SEEK_SET);
                    /* Now, what if a bin file is nothing but hex? :thinking: */
                    while ((c=getc(in)) != EOF && ascii)
                        if (dz_isxdigit(c)) d++;
                        else if (c!='\n' && c!=' ' && c!='\t') ascii=0;
                    if (d&1||!ascii) { // Can't have an odd number of ascii chars
                        fseek(in,ascii=0,SEEK_END);
                        d=(int)ftell(in)<<1;
                    }
                    printf("Detected (%i): ",d);
                    colorf(DZ8X_GRAY);
                    printf("%s\n",d?(ascii?"hexadecimal":"binary"):"None");
                }
                if (FILESIZE>131070) // Should I remove this?
                    errorf("Input file is WAAAAAAAAY too big");
                // hex files should be double, but who in their right mind would but that much data in a hex file?
                rewind(in);
            }
            colorf(DZ8X_RESET);

            if (isapp(argv[2]) || isos(argv[2])) {
                /* the complicated packaging of an 8?k/8?u file *
                 * Some copied out of rabbitsign's man page *
                 * 8?k/8?u file packaging:
                 * 2A 2A 54 49 46 4C 2A 2A ; (8 bytes) header [**TIFL**]
                 * ## ; Major version (App ID)
                 * ## ; Minor version (App Build)
                 * ## ; Flags: 0-binary | 1-INTEL Hex
                 * 88 ; Object Type (0=TI-89/TI-92+, 88=TI-73/TI-83+/TI-82AEP)
                 * MM DD YY YY ; Date
                 * ## ; Length of name
                 * ## ## ## ## ## ## ## ## ; Name (NULL padded)
                 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; (23 bytes) NULLs
                 * ## ; Calculator type (73=TI-83 Plus, 74=TI-73, 88=TI-92 Plus, 98=TI-89)
                 * ## ; var TYPE[] (0x23 = OS upgrade, 0x24 = application, 0x25 = certificate, 0x3E = License [89-2.01-2.04])
                 * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; (23 bytes) NULLs
                 * ## ; Calculator type (0=Legacy, DevelopmentKeys)
                 * 00 00 00 00 ; size of data section (reverse order)
                 *
                 * Variable: INTEL Intellec 8/MDS
                 */
                warningf("8%s files are not supported yet.",
                         has_extension(argv[2],"ek")?"ek":
                         has_extension(argv[2],"ck")?"ck":
                         has_extension(argv[2],"xk")?"xk":
                         has_extension(argv[2],"eu")?"eu":
                         has_extension(argv[2],"cu")?"cu":
                         has_extension(argv[2],"xu")?"xu":
                         has_extension(argv[2],"yu")?"yu":
                         " Waaaaaat");
                uint8_t q2=0;
                printf("Signature:");
                /* Why do I display the sig like this? */
                for(c=0;c<8;c++) {
                    uint8_t q=getc(in);
                    if (q2<32 || q<32 || q2>126 || q>126) printf(" ");
                    if (q>31 && q<127) {
                        colorf(DZ8X_GRAY);
                        printf("%c",q);
                    } else {
                        colorf(DZ8X_BLUE);
                        printf("%s",HexConv(q));
                    }
                    q2=q;
                }
                rewind(in);
                bool not=0;
                for(c=0;c<8;c++)
                    if (getc(in)!=SIGN[Nek].v[c])
                        not=1;
                if (not) {
                    colorf(DZ8X_RED);
                    printf(" FAIL\n");
                    errorf("Invalid signature");
                }
                colorf(DZ8X_GREEN);
                printf(" PASS");
                colorf(DZ8X_RESET);
                printf(" [app/os]\n");

                // "Export" bytes

                printf("Version:");
                colorf(DZ8X_GRAY);
                printf(" %X.%X\n",getc(in),getc(in));
                colorf(DZ8X_RESET);
                printf("Format: ");
                colorf(DZ8X_GRAY);
                printf("%X",ascii=getc(in));
                printf(" (%s)\n",ascii?"Intel Hex":"binary");

                colorf(DZ8X_RESET);
                printf("Object: ");
                colorf(DZ8X_GRAY);
                printf("%X",c=getc(in));
                printf(" (%s)\n",c?"TI-73/TI-83+":"TI-89/TI-92+");

                colorf(DZ8X_RESET);
                printf("Date: ");
                colorf(DZ8X_GRAY); // 4 bytes
                uint64_t Date=0;
                printf("%s",HexConv((unsigned int)(Date=0x1000000*getc(in)+0x10000*getc(in)+0x100*getc(in)+getc(in))));
                if (Date) { // TI doesn't do this in 8ck's for some reason
                    if ((((Date&0xFF000000)>>24)-3*((Date&0xF0000000)>>27)) > 11) // DDMMYYYY
                        printf(" (%s %s, %s)",
                               Month[((Date&0xFF0000)>>16)-3*((Date&0xF00000)>>19)],
                               HexConv((Date&0xFF000000)>>24),
                               HexConv(Date&0xFFFF));
                    else // MMDDYYYY
                        printf(" (%s %s, %s)",
                               Month[((Date&0xFF000000)>>24)-3*((Date&0xF0000000)>>27)],
                               HexConv((Date&0xFF0000)>>16),
                               HexConv(Date&0xFFFF));
                }

                colorf(DZ8X_RESET);
                printf("\nName (%i): ",getc(in));
                colorf(DZ8X_GRAY);
                for (q2=8;q2--;)
                    printf("%c",getc(in));

                fseek(in,48,SEEK_SET);
                colorf(DZ8X_RESET);
                printf("\nCalcID (%s): ",HexConv(q2=getc(in)));
                colorf(DZ8X_GRAY);
                printf("%s\n",q2==0x73?"TI-83 Plus":q2==0x74?"TI-73":q2==0x88?"TI-92 Plus":q2==0x98?"TI-98":"Unknown");
                colorf(DZ8X_RESET);
                printf("DataType: ");
                colorf(DZ8X_GRAY);
                printf("%s\n",TYPE[KIND=getc(in)].Desc);
                if (KIND!=0x23 && KIND!=0x24 && KIND!=0x25)
                    warningf("Non-FLASH format in a flash variable?");


                fseek(in,73,SEEK_SET);
                colorf(DZ8X_RESET);
                printf("Developer Key (%s): ",HexConv(q2=getc(in)));
                colorf(DZ8X_GRAY);
                printf("%s\n",DevelopmentKeys[q2]);

                colorf(DZ8X_RESET);
                printf("Size: ");
                colorf(DZ8X_GRAY);
                printf("%s bytes\n",HexConv(d=(unsigned int)(getc(in)+0x100*getc(in)+0x10000*getc(in)+0x1000000*getc(in))));

            } else if (PackagingFormat(argv[2])) {
                /* the complicated packaging of an 8x file *
                 * 8x# file packaging: (55 bytes)
                 * 2A 2A 54 49 38 33 46 2A ; (8 bytes) header [**TI83F*]
                 * 1A 0A ## ; Version bytes, 2nd byte: 0C for 85, 0A for 83/84/86 | last byte is an OSversion?
                 *   i.e. 1A0A0F for 8ca&8ci |
                 *  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
                 *  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ; (42 bytes) file comment
                 * 00 00 ; size of data section <= 0x35
                 *
                 * Variable header: (17 bytes)
                 * 0D 00 ; header length
                 * 00 00 ; size of var (little-endian)
                 * 00 ; var TYPE[] (ie 5=Program)
                 * 00 00 00 00 00 00 00 00 ; (8 bytes) file name (In tokens, not Ascii), NULL padded
                 * 00 ; Product ID <= 0x44
                 * 00 ; archive status:  00:Rammed,80:archived <= 0x45
                 * 00 00 ; size of var (little-endian)
                 *
                 * Variable:
                 *[00 00]; GDB, Picture, Window, TblSet |
                 *       ;  Program, Y-var, Group       | The size of the remaining contents
                 *       ; Real, Complex                | A continuation of the contents
                 *       ; List, Complex List           | The # of elements
                 *       ; Matrix                       | The w/h
                 * ... ; file contents
                 *
                 * File packaging:
                 * 00 00 ; checksum (little endian) of all contents after packaging
                 */

                // Header

                uint8_t q2=0;

                // Doesn't check extension
                char Sig[9]; fread(Sig, 1, 8, in);
                Sig[8]='\0'; // idk, but bugs
                printf("Signature:");
                // printf("%s",Sig);
                for(c=0;c<8;c++) {
                    if (q2<32 || Sig[c]<32 || q2>126 || Sig[c]>126) printf(" ");
                    if (Sig[c]>31 && Sig[c]<127) {
                        colorf(DZ8X_GRAY);
                        printf("%c",Sig[c]);
                    } else {
                        colorf(DZ8X_BLUE);
                        printf("%s",HexConv(Sig[c]));
                    };
                    q2=Sig[c];
                }
                for (c=0;c<Nend;c++)
                    if (!strcmp(Sig, SIGN[c].v))
                        q2=FILETYPE = SIGN[c].n;
                if (!q2) {
                    colorf(DZ8X_RED);
                    printf(" FAIL\n");
                    errorf("Invalid signature");
                }
                colorf(DZ8X_GREEN);
                printf(" PASS ");
                colorf(DZ8X_RESET);
                printf("[%i]\n",FILETYPE);


                /* !!! CHECKSUM START !!! */

                fseek(in,53,SEEK_SET); // skip comment
                uint16_t Size01=getc(in)+256*getc(in),CheckSum=0;
                printf("Data size: ");
                colorf(DZ8X_GRAY);
                printf("%i bytes\n",Size01);
                //fseek(in,55,SEEK_SET);
                for (c=Size01;c--;) {
                    //fseek(in,(int)ftell(in),SEEK_SET);
                    //printf("%i ",(int)ftell(in));
                    uint8_t peoe=getc(in);
                    //printf("%s\n",HexConv(peoe));
                    CheckSum+=peoe; // CheckSum
                }
                uint16_t CheckSum1=getc(in)+256*getc(in);
                colorf(DZ8X_RESET);
                printf("Checksum: ");
                colorf(DZ8X_GRAY);
                printf("%s %s",HexConv(CheckSum),HexConv(CheckSum1));
                if (CheckSum != CheckSum1) {
                    colorf(DZ8X_RED);
                    printf(" FAIL\n");
                    errorf("Checksum is invalid");
                }
                colorf(DZ8X_GREEN);
                printf(" PASS\n");

                /* !!! CHECKSUM  END  !!! */

                // skip header.
                fseek(in,8,SEEK_SET);

                // Export bytes

                colorf(DZ8X_RESET);
                printf("Version:");
                colorf(DZ8X_GRAY);
                printf(" %s %s %s",HexConv(getc(in)),HexConv(c=getc(in)),HexConv(q2=getc(in)));
                printf(" (%s | %s)\n",c==0xC?"TI-85":c==0xA?"TI-83/84":"Unknown",DevelopmentKeys[q2]);

                // Comment

                colorf(DZ8X_RESET);
                printf("Comment: ");
                colorf(DZ8X_GRAY);
                for(c=42;c--;) // print the comment (42 bytes)
                    printf("%c",getc(in)); // putchar( == printf("%c",

                getc(in);getc(in);

            VarPack:; // Too lazy to do it right

                /* Variable packaging */

                uint16_t head=getc(in)+256*getc(in); // header length
                Size02=getc(in)+256*getc(in);

                colorf(DZ8X_RESET);
                printf("\nHeader size: ");
                colorf(DZ8X_GRAY);
                printf("%s\n",HexConv(head));
                if (FILETYPE==84 && head!=0xD)
                    errorf("Incorrect header size");

                // Type

                colorf(DZ8X_RESET);
                printf("Type: ");
                colorf(DZ8X_GRAY);
                KIND=getc(in);
                if (FILETYPE!=85 && FILETYPE!=86)
                    Group=(KIND==0x17);
                printf("%s (%i %s)\n",HexConv(KIND),FILETYPE,
                       (FILETYPE==85 || FILETYPE==86)?TYPE85[KIND].Desc:
                       (has_extension(argv[2], ".8xo") && Group)?"Tilp Group":TYPE[KIND].Desc);

                // Extension

                colorf(DZ8X_RESET);
                printf("Extension: ");
                colorf(DZ8X_GRAY);

                for (c = (uint8_t)strlen(argv[2]) - 4; (unsigned)c < strlen(argv[2]); c++)
                    printf("%c",dz_tolower(argv[2][c])); // print actual extension

                if (!has_extension(argv[2], ".8xg") && !Group) {
                    char *T=TYPE[KIND].Ext;
                    T=repl_str(T,"8x",
                               (FILETYPE==73)?"73":
                               (FILETYPE==82)?"82":
                               (FILETYPE==83)?"83":"8x");
                    if (FILETYPE==85) T=TYPE85[KIND].Ext;
                    if (FILETYPE==86) T=repl_str(TYPE85[KIND].Ext,"85","86");

                    if (KIND==7 && Size02==758) { // if image and it is the size of a B/W, then it is a B/W
                        printf(" (.8xi)\n");
                        if (!has_extension(argv[2], ".8xi"))
                            warningf("Extension contradicts file contents");
                    }// else if (has_extension(argv[2], ".8xg") && (FILETYPE==83 || FILETYPE==84))
                    //	printf(" (.8xg)\n"); // ignore 8xg extension because it is independant of the file contents
                    else if (Group && has_extension(argv[2], ".8xo") && FILETYPE==84) // ignore 8xo extension?
                        printf(" (.8xo)\n");
                    else {
                        printf(" (%s)\n",T); // print "should be" extension
                        if (!has_extension(argv[2], T))
                            warningf("Extension contradicts file contents");
                    }

                    if ((has_extension(argv[2], ".85g")||has_extension(argv[2], ".86g")/*||has_extension(argv[2], ".8xg")*/) && !Group)
                        notef("Uncompressed groups are being experimented with.");
                } else
                    printf("\n");
                // Name

                colorf(DZ8X_RESET);
                printf("Name: ");
                colorf(DZ8X_GRAY);

                uint8_t nameleng=1,nameskip=1;

                switch (FILETYPE) {
                    case (83):
                    case (84):;
                        uint8_t v; // C89 compatible
                        /* Supported names: 0 1 2 3 4 5 6 7 8 C D F 10 11 15 17 18 1A 1B 1C 1D 1E 1F 20 21 */
                        switch (KIND) {
                            case (0x00): // Real
                            case (0x0C): // Complex
                            case (0x18): // Fraction
                            case (0x1B): // Complex fraction
                            case (0x1C): // Real radical
                            case (0x1D): // Complex radical
                            case (0x1E): // Complex pi
                            case (0x1F): // Complex pi fraction
                            case (0x20): // Real pi
                            case (0x21): // Real pi fraction
                                c=getc(in);
                                // Finance vars are at fixed addresses and cannot be exported
                                if (c==0x62) { // sequential n only?
                                    printf("%s",BASIC6X[getc(in)].B62);
                                    nameskip=nameleng=2;
                                } else
                                    printf("%s",DZBA[c].Base);
                                break;
                            case (0x01): // Real List
                            case (0x0D): // Complex List
                                switch (getc(in)) {
                                    case (0x3A): // I mean really?
                                        printf("Equation Solver Bounds");
                                    case (0x72):
                                        // This should never be a problem, but since CEmu is dumb:
                                        printf("Ans");
                                        break;
                                    case (0x5D):;
                                        c=getc(in);
                                        nameleng=nameskip=2;
                                        if (c<6) {
                                            printf("L%s",SmallNumb(c)); // 1-6
                                        } else {
                                            printf("|L%s",DZBA[c].Base); // print the list name
                                            for(v=6;v--;) {
                                                printf("%s",DZBA[c=getc(in)].Base);
                                                if (c) nameleng++;
                                                nameskip++;
                                            }
                                        }
                                        break;
                                    default:
                                        errorNL("Invalid list name");
                                }
                                break;
                            case (0x02): // Matrix
                                switch (getc(in)) {
                                    case (0x72):
                                        printf("Ans");
                                        break;
                                    case (0x5C):
                                        printf("[%c]",'A'+getc(in));
                                        nameleng=nameskip=2;
                                        break;
                                    default:
                                        errorNL("Invalid matrix name");
                                }
                                break;
                            case (0x03): // Equation
                                switch (getc(in)) {
                                    case (0x17):
                                        printf("MathPrint Equation Solver Left");
                                        break;
                                    case (0x18):
                                        printf("MathPrint Equation Solver Right");
                                        break;
                                    case (0x2E): // '.'
                                        printf(".");
                                        break;
                                    case (0x3E): // '>'
                                        printf("Classic Equation Solver");
                                        break;
                                    case (0x40): // '@'
                                        printf("@ (Variable Working Copy)"); // [Includes whitespace?]
                                        break;
                                    case (0x5E):
                                        c=getc(in);
                                        if (c>=0x10 && c<=0x19)
                                            printf("%s",BASIC5E[c-0x10]);
                                        else if (c>=0x1A && c<=0x1F)
                                            printf("Z%s",SmallNumb(c-0x1a));
                                        // Hopefully these don't have an entire format of their own
                                        else if (c>=0x20 && c<=0x2B)
                                            printf("%s",BASIC5E[c-0x20]); //-0x2A+10
                                        else if (c>=0x40 && c<=0x45)
                                            printf("%s",BASIC5E[c-0x2A]); //-0x40+22
                                        else if (c>=0x80 && c<=0x82)
                                            printf("%s",BASIC5E[c-0x64]); //-0x80+28
                                        else errorNL("Invalid Equation name");
                                        nameleng=nameskip=2;
                                        break;
                                    default:
                                        errorNL("Invalid Equation name");
                                }
                                break;
                            case (0x04): // String
                                switch (getc(in)) {
                                    case (0x72):
                                        printf("Ans");
                                        break;
                                    case (0xAA):
                                        c=getc(in);
                                        printf("Str%i",c==9?0:c+1); // print the string name
                                        nameleng=nameskip=2;
                                        break;
                                    default:
                                        errorNL("Invalid string name");
                                }
                                break;
                            case (0x05): // unprotected
                            case (0x06): // protected
                            case (0x15): // appvar
                            case (0x17): // group
                                nameleng=nameskip=0;
                                for(v=8;v--;) {
                                    printf("%s",((c=getc(in))==0x5B)?theta:(char*)&c); // Finally one that works!
                                    if (c) nameleng++;
                                    nameskip++;
                                }
                                break;
                            case (0x08): // GDB
                                if (getc(in) != 0x61)
                                    errorNL("Invalid GDB name");
                                c=getc(in);
                                printf("GDB%i",c==9?0:c+1); // (c+1)%10  |  c+1-10*(c==9)  |  (c+1)(c==9)
                                nameleng=nameskip=2;
                                break;
                            case (0x0F): // Window
                            case (0x10): // Zoom
                            case (0x11): // TblSet
                                nameleng=nameskip=0;
                                for(v=8;v--;) {
                                    printf("%c",c=getc(in));
                                    if (c) nameleng++;
                                    nameskip++;
                                }
                                break;
                            case (0x1A): // Image
                                if (getc(in) != 0x3C)
                                    errorNL("Invalid Image name");
                                if ((c=getc(in))>9)
                                    errorNL("Invalid Image number");
                                printf("Image%i",c==9?0:c+1);
                                nameleng=nameskip=2;
                                break;
                            case (0x07): // Picture
                                if (getc(in) != 0x60)
                                    errorNL("Invalid Pic name");
                                if ((c=getc(in))>9)
                                    errorNL("Invalid Pic number");
                                printf("Pic%i",c==9?0:c+1);
                                nameleng=nameskip=2;
                                break;
                            default:
                                errorNL("Unsupported format, Please report.");
                        }
                        fseek(in,8-nameskip,SEEK_CUR);
                        break;
                    case (85):
                        nameleng=getc(in);
                        for(c=nameleng;c--;) // name is not padded in 85 files
                            printf("%c",getc(in));
                        break;
                    case (86):
                        // some headers have a namelength and a padded name and some ust have a padded name.  Weird.
                        if (head!=0xB) {
                            nameleng=getc(in);
                            printf("(%i,%i%s) ",nameleng,head-5,nameleng!=head-5?" HELP":"");
                            for(c=8;c--;) // name is padded in some 86 files
                                printf("%c",getc(in));
                        } else {
                            for(c=8;c--;) // name is padded in 86 files
                                printf("%c",getc(in));
                        }
                        break;
                    case (73):
                    case (82):
                        nameleng=8;
                        for(c=8;c--;)
                            printf("%c",getc(in));
                        break;
                    default:
                        errorNL("Issue with FILETYPE, Please report.");
                }
                printf("\n");

                // Product ID byte & Archive Status

                if (FILETYPE==84) {
                    c=getc(in);
                    colorf(DZ8X_RESET);
                    printf("Owner ID: ");
                    colorf(DZ8X_GRAY);
                    if (c<=18)
                        printf("%s (%s)\n",HexConv(c),ExportVersion[c]);// Might be +0x20
                    else
                        printf("%s\n",HexConv(c));
                    colorf(DZ8X_RESET);
                    printf("Status: ");
                    colorf(DZ8X_GRAY);
                    printf("%srchived\n",(c=getc(in))?"A":"Una");
                    if (c&127)
                        warningf("Archive status problem");
                }

                // Size Stuff

                uint16_t Size03=getc(in)+256*getc(in);

                colorf(DZ8X_RESET);
                printf("Variable size: ");
                colorf(DZ8X_GRAY);

                printf("%i %i",Size02, Size03); // print sizes
                if (Size02 != Size03) { // Should also check this with FILESIZE if it is not a group
                    colorf(DZ8X_RED);
                    printf(" FAIL\n");
                    errorf("Invalid file size");
                }
                colorf(DZ8X_GREEN);
                printf(" PASS\n");

                colorf(DZ8X_RESET);
                printf("On-calc size: ");
                colorf(DZ8X_GRAY);
                printf("%i bytes\n",nameleng+Size02+9);

                // bin & hex exporter

                if (Extract) {
                    uint16_t a=Size02; // C89 compatible
                    notef("To veiw the contents, view the output file");
                    printf("Outputting to %s file...",has_extension(argv[3],".bin")?"bin":"hex");
                    if (has_extension(argv[3],".bin")) // write to bin
                        for (;a--;) // easy
                            putc(getc(in),out);
                    else if (has_extension(argv[3],".hex")) // write to hex
                        for (;a--;) { // easy-ish
                            c=getc(in);
                            putc((int)*HEX[c>>4],out);
                            putc((int)*HEX[c&15],out);
                            //fputs(HexConv(getc(in)),out);
                        }
                    fclose(in);
                    fclose(out);
                    printf(" Complete.\n");
                    EndTime(start_time);
                    exit(0);
                }
            }


            /*
             * THESE ARE THE VARIABLE READERS.
             */


            if (Group) {
                //notef("Group support is still in beta");
                colorf(DZ8X_RESET);
                printf("Group size: ");
                colorf(DZ8X_GRAY);
                printf("%i bytes\n", getc(in)+256*getc(in));
            }

            do {
                if (Group) { // what should I do with this?

                    uint8_t nameleng;
                    colorf(DZ8X_RESET);
                    printf("=========================\n");
                    // https://wikiti.brandonw.net/index.php?title=83Plus:OS:System_Table

                    printf("Type: "); // T
                    colorf(DZ8X_GRAY);
                    printf("%s\n",TYPE[KIND=getc(in)&0x1F].Desc);
                    //0-4 | Object Type
                    // 5  | Graph Equation selected
                    // 6  | Variable used during graphing
                    // 7  | Link Transfer flag
                    colorf(DZ8X_RESET);
                    printf("Reserved: "); // T2
                    colorf(DZ8X_GRAY);
                    printf("%s\n",HexConv(getc(in)));
                    colorf(DZ8X_RESET);
                    printf("Version: ");
                    colorf(DZ8X_GRAY);
                    printf("%s\n",HexConv(getc(in)));
                    colorf(DZ8X_RESET);
                    printf("Structure Pointer: "); // does this actually have useful data?
                    colorf(DZ8X_GRAY);
                    printf("%s%s\n",HexConv(getc(in)),HexConv(getc(in)));
                    colorf(DZ8X_RESET);
                    printf("Archive status?: "); // Zero (always unarchived)
                    colorf(DZ8X_GRAY);
                    printf("%srchived\n",getc(in)?"A":"Una");
                    colorf(DZ8X_RESET);
                    //Why are names so complicated?
                    nameleng=getc(in);
                    if (!nameleng) {
                        printf("Name (0): %c%c",
                               getc(in), // idk
                               getc(in));
                    } else if (nameleng<9) { // TODO: this is actually based off of font equates
                        printf("Name (%i): ",nameleng);
                        colorf(DZ8X_GRAY);
                        c=getc(in);
                        if (c==0x5D) {
                            c=getc(in);
                            if (c>5) {
                                printf("|L%s",DZBA[c].Base);
                                for(c=nameleng-2;c--;)
                                    printf("%s",DZBA[getc(in)].Base);
                            } else {
                                printf("L%i",c==9?0:c+1);
                                getc(in);
                            }
                        } else {
                            printf("%s",DZBA[c].Base);
                            for(c=nameleng-1;c--;)
                                printf("%s",DZBA[getc(in)].Base);
                        }
                    } else {
                        printf("Name: ");
                        colorf(DZ8X_GRAY);
                        if (nameleng==0x5E) {
                            c=getc(in);
                            // Technikly bugged if invalid name :/
                            printf("%s",BASIC5E[c-0x10*(c>=0x10 && c<=0x19)
                                                -0x16*(c>=0x20 && c<=0x2B)
                                                -0x2A*(c>=0x40 && c<=0x45)
                                                -0x64*(c>=0x80 && c<=0x82)]);
                            getc(in); // For some reason the name is 3 bytes?
                        } else if (nameleng==0xAA) {
                            c=getc(in);
                            printf("Str%i",c==9?0:c+1); // print the string name
                            getc(in);
                        } else if (nameleng==0x5C) {
                            printf("[%c]",'A'+getc(in));
                            getc(in);
                        } else {
                            printf("%c",nameleng);
                            getc(in);
                            getc(in);
                        }
                    }
                    printf("\n");
                }

                colorf(DZ8X_RESET);
                if ((FILETYPE==84 && KIND==26) || // only the 84 has Image (.8ca) files
                    ((FILETYPE==73 || FILETYPE==82 || FILETYPE==83 || FILETYPE==84) && KIND==7) ||
                    ((FILETYPE==85 || FILETYPE==86) && (KIND==0x11)) ) { // PIC/IMAGE
#ifdef DZ8X_LODEPNG
                    uint8_t error=0;
                    if (argc<4) errorf("8ca and 8?i files require an output");
                    if (!has_extension(argv[3], ".png")) errorf("8ca and 8?i files require a PNG output");
                    uint16_t DataSize = getc(in)+256*getc(in);
                    if (((FILETYPE==73 || FILETYPE==82 || FILETYPE==83 || FILETYPE==84) && KIND==7) ||
                        ((FILETYPE==85 || FILETYPE==86) && KIND==0x11)) { // PIC
                        if (DataSize==756 || DataSize==1008) { // B/W pic (.8xi)
                            printf("Writing Pic...");
                            uint8_t *image = malloc(DataSize);
                            for (d=0;d<DataSize;d++)
                                image[d]=255-getc(in);
                            error = lodepng_encode_file(argv[3], image, 96+32*(FILETYPE==85 || FILETYPE==86), 63, 0, 1);
                            // straight to 1-bit Grayscale :D
                            free(image);
                        } else if (DataSize==21945) { // Color pic (.8ci)
                            printf("Writing Pic...");
                            colors_t *image = malloc(131670); // 266*165*3 | DataSize*6
                            for (d=0;d<43890;d+=2) {
                                c=getc(in);
                                image[d  ].r=oscolors[c>>4].r;
                                image[d  ].g=oscolors[c>>4].g;
                                image[d  ].b=oscolors[c>>4].b;
                                image[d+1].r=oscolors[c&15].r;
                                image[d+1].g=oscolors[c&15].g;
                                image[d+1].b=oscolors[c&15].b;
                            }
                            error=lodepng_encode_file(argv[3], (uint8_t*)image, 266, 165, 2, 8);
                            free(image);
                        } else errorf("Invalid dimensions");
                    } else if(FILETYPE==84 && KIND==26){ // IMAGE (.8ca)
                        printf("Other:");
                        colorf(DZ8X_GRAY);
                        printf(" %s\n",HexConv(c=getc(in))); // I forget what this byte is
                        if (c!=129) errorf("Invalid image byte");
                        if (DataSize==22245) {
                            // https://www.irccloud.com/pastebin/WfcoM4AM/
                            colorf(DZ8X_RESET);
                            printf("Writing Image...");
                            colors_t *image = malloc(33366); // 134*83*3 | (DataSize-1)*3/2
                            uint16_t m;
                            for (d=0;d<11122;d++) {
                                //m=134*(82-floor(d/134))+fmod(d,134);
                                m=d+268*(41-d/134); // Flip the image upside-right
                                // 10988-d ?
                                c=getc(in)+256*getc(in);
                                // 565 - rrrr rggg gggb bbbb
                                image[m].r=(c>>11   )*255/31;
                                image[m].g=(c>>5 &63)*255/63;
                                image[m].b=(c    &31)*255/31;
                            }
                            error=lodepng_encode_file(argv[3], (uint8_t*)image, 134, 83, 2, 8);
                            free(image);
                        } else errorf("Invalid dimensions");
                    } else
                        errorf("Internal error: Please report.");
                    if (error) {
                        colorf(DZ8X_RED);
                        printf("\nLODEPNG ERROR %u: %s", error, lodepng_error_text(error));
                        colorf(DZ8X_RESET);
                        printf("\nIf the problem persists, please contact the developer.\n");
                    } else printf(" Complete\n");
                    // LodePNG (opens &) closes out
                    // LodePNG automaticity attempts to choose the smallest format
                    // (Lode20170917-3765 / Lode20181230-3546 / Lode20190914-3802)
#else /*DZ8X_LODEPNG*/
                    errorf("LodePNG was not included in this build of DZ8x");
#endif /*DZ8X_LODEPNG*/
                } else if (((FILETYPE==73 || FILETYPE==82 || FILETYPE==83 || FILETYPE==84) &&
                            (!KIND    || KIND== 1 || // Real    / R List
                             KIND== 2 ||             // Matrix
                             KIND==12 || KIND==13 || // Complex / C List
                             KIND==15 || KIND==16 || // Window  / Zoom
                             KIND==17 ||             // TblSet
                             KIND==24 ||             // Fraction
                             (KIND>=27 && KIND<=33)) // C Fract - R pi fract
                            ) ||
                           ((FILETYPE==85 || FILETYPE==86) &&
                            (!KIND    || KIND==1  ||  // Real    / Complex
                             (KIND>=4 && KIND<=7) ))  // R List  - C Matrix
                           ) {
                    /* New Size code */
                    uint16_t Elem=1,b=0,e=0;
                    colorf(DZ8X_RESET);
                    if (KIND==2) { // Matrix
                        b=getc(in);
                        e=getc(in);
                        printf("W/H: ");
                        colorf(DZ8X_GRAY);
                        printf("%i %i (%i)\n",b,e,Elem=b*e);
                    } else if (KIND==1 || KIND==13) { // R List / C List
                        printf("Elements: ");
                        colorf(DZ8X_GRAY);
                        printf("%i\n",Elem=getc(in)+256*getc(in));
                    } else if (KIND==15 || KIND==16 || KIND==17) { // Window / Zoom / TblSet
                        printf("Internal size: ");
                        colorf(DZ8X_GRAY);
                        printf("%i\n",getc(in)+256*getc(in));
                    }
                    if (!Group)
                        d=Size02;
                    // complex/real?
                    uint8_t l=1+(KIND==0xC||KIND==0xD||KIND==0x1B||KIND==0x1D||KIND==0x1E||KIND==0x1F);
                    switch (KIND) { // Sudo-spaghetti
                        case (0x0F): // (REAL) Window
                            d--;
                        case (0x10): // (REAL) Zoom
                            Elem=22;
                        case (0x11): // (REAL) TblSet
                            Elem+=1;
                        case (0x01): // real list
                        case (0x02): // (REAL) Matrix
                        case (0x0D): // complex list
                            d-=2;
                    }

                    if (d != 9*Elem*l && !Group) errorf("Invalid Length");
                    if (KIND==15) { // Dummy byte
                        colorf(DZ8X_RESET);
                        printf("Unknown: ");
                        colorf(DZ8X_GRAY);
                        printf("%s\n",HexConv(getc(in)));
                    }
                    colorf(DZ8X_RESET);
                    for (c=1; c<=Elem;c++) {
                        switch (KIND) {
                            case (0):
                            case (12): break;
                            case (15):
                            case (16): printf("%s\t",WINDOWTEXT[c-1]); break;
                            case (17): printf("%s\t",c==1?"TblStrt":c==2?"∆Tbl":"Error"); break;
                            case (2): printf("%ix%i\t",1+(c-1)/b,c-b*((c-1)/b)); break; // 1+(c-1)%b
                            default: printf("%i\t",c);
                        }
                        TIOS_Number(in, 0);
                    }
                } else if (((FILETYPE==73 || FILETYPE==82 || FILETYPE==83 || FILETYPE==84) && KIND==8) ||
                           ((FILETYPE==85 || FILETYPE==86) && (KIND==0xD || KIND==0xE || KIND==0xF || KIND==0x10))) { // GDBs are awesome
                    //notef("GDBs are being looked into,\nAnd will require the code to be COMPLETELY reformated,\nSo please be patient.");
                    notef("GDBs do not currently print functions");
                    colorf(DZ8X_RESET);
                    printf("GDB size: ");
                    colorf(DZ8X_GRAY);
                    printf("%i bytes\n", getc(in)+256*getc(in));
                    colorf(DZ8X_RESET);
                    printf("Unused: ");
                    colorf(DZ8X_GRAY);
                    printf("%s\n",HexConv(getc(in)));
                    colorf(DZ8X_RESET);
                    printf("Graph Mode: ");
                    colorf(DZ8X_GRAY);
                    int Mode=getc(in);
                    switch (Mode) {
                        case (0x10): printf("Function"); break;
                        case (0x20): printf("Polar"); break;
                        case (0x40): printf("Parametric"); break;
                        case (0x80): printf("Sequence"); break;
                        default: errorNL("Invalid graph mode");
                    }
                    colorf(DZ8X_RESET);
                    printf("\nMode settings:");
                    colorf(DZ8X_GRAY);
                    int a=getc(in);
                    printf(" %s | S%sl | GridO%s | %sGC | CoordO%s | AxesO%s | LabelO%s | Grid%s\n",
                           a&1?"Dot":"Connected", a&2?"imu":"equentia", a&4?"n":"ff",
                           a&8?"Polar":"Rect", a&16?"ff":"n", a&32?"ff":"n",
                           a&64?"n":"ff", a&128?"Line":"Dot");
                    a=getc(in);
                    colorf(DZ8X_RESET);
                    printf("Sequence settings: ");
                    colorf(DZ8X_GRAY);
                    printf("(%s) %s | %i | %i | %i | %i\n",HexConv(a),
                           a&1?"Web":a&4?"uv":a&8?"vw":a&16?"uw":"Time",
                           (a>>1)&1,(a>>5)&1,(a>>6)&1,(a>>7)&1);
                    if (a&0b11100010)
                        warningf("Unknown bits used");
                    colorf(DZ8X_RESET);
                    printf("Extended settings: ");
                    a=getc(in);
                    colorf(DZ8X_GRAY);
                    printf("(%s) %s | SEQ(n",HexConv(a),a&1?"ExprOff":"ExprOn");
                    if ((a>>1)&3)
                        printf("+%i",(a>>1)&3);
                    printf(")");
                    for (c=3;c<8;c++) printf(" | %i",(a>>c)&1);
                    printf("\n");
                    if (a&0b11111000)
                        warningf("Unknown bits used");
                    colorf(DZ8X_RESET);

                    for (c=0;c<(7+2*(Mode>0x10)+7*(Mode==0x80));c++) {
                        printf("%s\t",WINDOWTEXT[c + 3*(c>5)*(1 + (Mode>>7)) + 12*(Mode==0x10 && c==6) + (c==15)]);
                        TIOS_Number(in, 0);
                    }
                    printf("Style information:\n");
                    for (c=0;c<(10-4*(Mode==0x20 || Mode==0x40)-7*(Mode==0x80));c++) {
                        a=getc(in);
                        colorf(DZ8X_RESET);
                        if (Mode==0x80) printf("%c(n) - ",'u'+c);
                        else printf("%s - ",BASIC5E[c+(Mode==0x10?0:Mode==0x20?22:(c+10))]);
                        //   printf("%s - ",BASIC5E[c+(Mode==0x20)*22+(Mode==0x40)*(c+10)]); // more math, portable
                        if (a>7)
                            errorNL("Invalid style byte");
                        colorf(DZ8X_GRAY);
                        printf("%s\n",Styles[a].Graph);
                    }

                    colorf(DZ8X_RESET);
                    printf("Functions:\n");
                    uint16_t b;
                    for (b=0;b<(10-4*(Mode==0x20)+2*(Mode==0x40)-7*(Mode==0x80));b++) {
                        colorf(DZ8X_RESET);
                        if (Mode==0x80) printf("%c(n)",'u'+c);
                        else printf("%s",BASIC5E[b+(Mode==0x20)*22+(Mode==0x40)*10]);
                        printf(" - (%s) ",HexConv(a=getc(in)));
                        colorf(DZ8X_GRAY);
                        printf("%selected | %i",a&0b00100000?"S":"Uns",(a>>6)&1);
                        c=getc(in)+256*getc(in);
                        printf(" | (%i byte%s)",c,c==1?"":"s");
                        if (a&0b00001000) // thought this would look better here
                            printf(" | Yet to be graphed");
                        printf("\n");
                        if (a&0b10010100)
                            warningf("Unknown bits used");
                        colorf(DZ8X_HONEY);
                        /* FUNCTION GOES HERE */
                        printf("Function goes here");
                        for (;c--;)
                            getc(in); // Function
                        printf("\n");
                    }

                    colorf(DZ8X_RESET);
                    if (FILETYPE==84) {

                        printf("Color indicator: ");
                        char n[3]={getc(in),getc(in),getc(in)};

                        colorf(DZ8X_GRAY);
                        printf("%s",n);
                        if (strcmp(n,"84C")) {
                            colorf(DZ8X_RED);
                            printf(" FAIL\n");
                            errorf("Invalid color indicator");
                        }
                        colorf(DZ8X_GREEN);
                        printf(" PASS\n");

                        colorf(DZ8X_RESET);
                        printf("Function colors:\n");
                        for (c=0;c<(10-4*(Mode==0x20 || Mode==0x40)-7*(Mode==0x80));c++) {
                            a=getc(in);
                            colorf(DZ8X_RESET);
                            if (Mode==0x80) printf("%c(n) - ",'u'+c);
                            else printf("%s - ",BASIC5E[c+(Mode==0x10?0:Mode==0x20?22:(c+10))]);
                            if (a>15)
                                warningf("Invalid color");
                            colorf(DZ8X_GRAY);
                            printf("%s\n",a>15?HexConv(a):oscolors[a].name);
                        }
                        colorf(DZ8X_RESET);
                        printf("GridColor: ");
                        colorf(DZ8X_GRAY);
                        printf("%s\n",oscolors[getc(in)].name);
                        colorf(DZ8X_RESET);
                        printf("AxesColor: ");
                        colorf(DZ8X_GRAY);
                        printf("%s\n",oscolors[getc(in)].name);
                        colorf(DZ8X_RESET);
                        printf("LineStyle: ");
                        a=getc(in);
                        colorf(DZ8X_GRAY);
                        printf("%s (%s)\n",HexConv(a),Styles[a&15].Line);
                        // I have seen 0x42 and it regesters the "2" but not the "4"
                        colorf(DZ8X_RESET);
                        printf("BorderColor: ");
                        colorf(DZ8X_GRAY);
                        printf("%s\n",HexConv(a=getc(in)));
                        if (!a || a>4) warningf("BorderColor is out of range");
                        // Calc is not capable of handling anything other than 1-4
                        colorf(DZ8X_RESET);
                        printf("Extended extended settings: ");
                        a=getc(in);
                        colorf(DZ8X_GRAY);
                        printf("Detect Asymptotes: O%s",a&1?"ff":"n");
                        for (c=1;c<8;c++)
                            printf(" | %i",(a>>c)&1);
                        printf("\n");
                        // What does the last bit mean?
                        if ((a&0b01111110)||!(a>>7))
                            warningf("Unknown bits used");
                        colorf(DZ8X_RESET);
                    }
                } else if (KIND==0x15) { // AppVar
                    colorf(DZ8X_RESET);
                    printf("Data size: ");
                    colorf(DZ8X_GRAY);
                    printf("%i bytes\n", d=getc(in)+256*getc(in));
                    colorf(DZ8X_RESET);
                    printf("Code Type: ");
                    colorf(DZ8X_GRAY);
                    if (d>4) {
                        uint8_t a[]={getc(in),getc(in),getc(in),getc(in)};
                        if (( a[0]=='P' && a[1]=='Y') && // PYthon
                            ((a[2]=='S' && a[3]=='C') || // Scource Code
                             (a[2]=='C' && a[3]=='D'))) {// Compiled Document
                            printf("Python\n");
                            colorf(DZ8X_HONEY);
                            for (c=d-4;c--;)
                                printf("%c",getc(in));
                        } else {
                            printf("Data\n");
                            colorf(DZ8X_RESET);
                            printf("Data: ");
                            colorf(DZ8X_BLUE);
                            for (c=0;c<4;c++)
                                printf("%s",HexConv(a[c]));
                            for (c=d-4;c--;)
                                printf("%s",HexConv(getc(in)));
                            printf("\n");
                        }
                    } else {
                        printf("Data\n");
                        colorf(DZ8X_RESET);
                        printf("Data: ");
                        colorf(DZ8X_BLUE);
                        for (c=d;c--;)
                            printf("%s",HexConv(getc(in)));
                        printf("\n");
                    }
                    colorf(DZ8X_RESET);
                    if (op) notef("Use -X to output to a file");
                } else {

                    /* Nothing gets down to this point unless it is a program or a related filetype */

                    if (PackagingFormat(argv[2]))
                        d = getc(in)+256*getc(in);
                    else if (KIND!=0x23 && KIND!=0x24 && KIND!=0x25) {
                        // I can move this to be up with the initial file reader
                        if (!d)
                            errorf("Input file is empty");
                        if (d&1)
                            errorf("Odd number of characters");
                        d>>=1;
                    }

                    colorf(DZ8X_RESET);
                    printf("Code size: ");
                    colorf(DZ8X_GRAY);
                    printf("%i bytes\n", d);


                    uint8_t disasm[d]; // should be malloc'ing the right amount of mem?


                    colorf(DZ8X_RESET);
                    printf("Code: ");
                    colorf(DZ8X_BLUE);
                    if (Cline || (ascii && KIND==0x5)) { // for now KIND is assumed to be 5 if it is not a packaged file
                        /* for unpackaging that annoying command line and hexadecimal files */
                        uint32_t n=0; // this can be up to 17 bits, but shouldn't be.
                        if (Cline) op=0; // never output
                        for (; n<(unsigned)(d*2); n++) {
                            c=Cline?(char)argv[2][n]:getc(in);
                            if (c!='\n' && c!=' ' && c!='\t') {
                                if (!dz_isxdigit(c)) {
                                    colorf(DZ8X_RED);
                                    printf("%c\n",c);
                                    errorf("%s",Cline?"Invalid character":"Internal error: Please report.");
                                }
                                printf("%c",c=dz_toupper(c));
                                if (n&1)
                                    disasm[n>>1]+=dz_toxvalue(c);
                                else
                                    disasm[n>>1]=16*dz_toxvalue(c);
                            } else n--;
                        }
                    } else if (KIND==0x23 || KIND==0x24 || KIND==0x25) { // AMS||Application||Certificate
                        // ascii?Hex:binary
                        /* INTEL Intellec 8/MDS [Original from /linkguide/ti83+/fformat.html]
                         *  This is in ascii...
                         *   +--Colon (3A)
                         *   | +--Number of data bytes                     Line feed (0D0A)--+
                         *   | |  +--Address                        Checksum (256-C&255)--+  |
                         *   | |  |    +--Block type (00:data, 01:end, 02:page number)    |  |
                         *   | |  |    |  +--                  Data                   --+ |  |
                         *   | |  |    |  |                                             | |  |
                         *   : 02 0000 02 00 00 <--Page number                            FC CR/LF
                         *   : 10 4000 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 CR/LF
                         *   : 10 4010 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF F0 CR/LF
                         *   : 10 4020 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF E0 CR/LF
                         *   : 00 0000 01                                                 FF
                         */
                        uint32_t y=0,Base=0,g=0,n=0,l=0,add,b=0,che,meml=0;
                        /* Decode hex */
                        if (ascii) {
                            do {
                                getc(in); // skip ':'
                                l=16*dz_toxvalue(getc(in))+dz_toxvalue(getc(in));
                                add=4096*dz_toxvalue(getc(in))+256*dz_toxvalue(getc(in))+16*dz_toxvalue(getc(in))+dz_toxvalue(getc(in));
                                b=16*dz_toxvalue(getc(in))+dz_toxvalue(getc(in));
                                che=l+(add&0xFF)+((add&0xFF00)>>8)+b;
                                if (b==2) {
                                    colorf(DZ8X_RESET);
                                    printf("\nPage ");
                                    for (g=l;g--;) {
                                        printf("%s", HexConv(meml=16*dz_toxvalue(getc(in))+dz_toxvalue(getc(in))));
                                        che+=meml;
                                    }
                                    printf(":");
                                    colorf(DZ8X_BLUE);
                                } else {
                                    for (g=l;g--;n++) {
                                        printf("%s", HexConv(disasm[n]=16*dz_toxvalue(getc(in))+dz_toxvalue(getc(in))));
                                        che+=disasm[n];
                                    }
                                }
                                if ((256-che&255) != 16*dz_toxvalue(getc(in))+dz_toxvalue(getc(in)))
                                    errorNL("Invalid checksum (%i)",256-che&255);
                                getc(in);getc(in);
                                printf(" ");
                            } while (b!=1);
                        } else {
                            /* Binary */
                            for (n=0;n<(unsigned)d;n++)
                                printf("%s", HexConv(disasm[n]=getc(in)));
                        }
                        d=n;
                        /* Read headers */
                        g=0;n=0;
                        while (g!=0x807 && g!=0x817) { // Ending field
                            // Field number
                            g=((256*disasm[n]+disasm[n+1])&0xFFF0)>>4;
                            // Field size
                            l=disasm[n+1]&0x0F;
                            n+=2;
                            colorf(DZ8X_RESET);
                            printf("\nField #%s (%s): ",HexConv(g),HexConv(l));
                            colorf(DZ8X_BLUE);
                            if ((g==0x800 || g==0x810) && l==0xF) // length of code? | Cesium 3.0.6 has two 810 fields .-.
                                meml=256*256*256*disasm[n]+256*256*disasm[n+1]+256*disasm[n+2]+disasm[n+3];
                            if (l==0xD) l=disasm[n++];
                            else if (l==0xE) {
                                l=disasm[n+1]+256*disasm[n];
                                n+=2;
                            } else if (l==0xF) l=4; // not actually the size, it's the size of the size, but the contents include well everything
                            for (c=0;(unsigned)c<l;c++)
                                printf("%s",HexConv(disasm[n++]));
                        }
                        printf("\n");
                        y=PLACE2=n; // #tiisdumb
                        if (has_extension(argv[2], ".8ek")) {
                            char*ROR[]={"Main","Initialized location","Initialized size","Entry","Language","ExecLib","Copyright","Reserved"};
                            /* the extended packaging of an 8ek file *
                             * https://wikiti.brandonw.net/index.php?title=84PCE:OS:Applications#Additional_App_Structure
                             * /Excuse me?/ *
                             * 00 - 65 5A 38 ; "eZ8"
                             * 03 - ## ## ## ## ## ## ## ## 00 ; Name
                             * 0C - ## ; Flags: 0-DisableSplash, 1-OpenLib
                             * 0D - ## ; Unknown
                             * 0E - ## ; Flags: 0-LanguageApp, 1-IntegratedApp
                             * 0F - ## ## ## ; Unknown
                             * 12 - ## ## ## ; Main address
                             * 15 - ## ## ## ; Initialized data section
                             * 18 - ## ## ## ; Initialized data section length
                             * 1B - ## ## ## ; Execution entry (usually same as 012h)
                             * 1E - ## ## ## ; Language app offset
                             * 21 - ## ## ## ; Offset to ExecLib table
                             * 24 - ## ## ## ; Offset to memory screen Copyright
                             * 27 - ## ## ## ; Reserved
                             * 2A - ... ; relocation table
                             * Main address - ... ; start of code
                             */
                            colorf(DZ8X_RESET);
                            printf("EZ80: %c%c%c\n",disasm[PLACE2],disasm[PLACE2+1],disasm[PLACE2+2]);
                            PLACE2+=3;
                            printf("Name: ");
                            for (c=9;c--;)
                                printf("%c",disasm[PLACE2++]);
                            c=disasm[PLACE2++]; // 0C
                            printf("\nFlag1: %s | %s %s\n",HexConv(c),
                                   c&1?"DisableSplash":"0",
                                   c&2?"OpenLib":"0");

                            printf("Unknown: %s\n",HexConv(disasm[PLACE2++])); // 0D

                            c=disasm[PLACE2++]; // 0E
                            printf("Flag2: %s | %s %s %s\n", HexConv(c),
                                   c&1?"language":"0",c&2?"Integrated":"0",c&4?"Unknown":"0");

                            printf("Unknown: %s\n",HexConv(disasm[PLACE2]+256*disasm[PLACE2+1]+65536*disasm[PLACE2+2])); // 0F-11
                            PLACE2+=3;

                            Base=disasm[PLACE2]+256*disasm[PLACE2+1]+65536*disasm[PLACE2+2]; // Hax :P
                            for (c=0;c<8;c++) {
                                printf("%s: %s\n",ROR[c],HexConv(disasm[PLACE2]+256*disasm[PLACE2+1]+65536*disasm[PLACE2+2]));
                                PLACE2+=3;
                            }
                            c=PLACE2;
                            printf("Hole | Base:\n");
                            for (;c-y<Base;c+=6) {
                                printf("%s | %s\n",
                                       HexConv(disasm[c  ]+256*disasm[c+1]+65536*disasm[c+2]),
                                       HexConv(disasm[c+3]+256*disasm[c+4]+65536*disasm[c+5]));
                            }
                            Base=c;
                        }
                        /* App Signature */
                        n=6+meml;
                        g=((256*disasm[n]+disasm[n+1])&0xFFF0)>>4;
                        // Field size
                        l=disasm[n+1]&0x0F;
                        n+=2;
                        colorf(DZ8X_RESET);
                        printf("Field #%s(%s): ",HexConv(g),HexConv(l));
                        switch (l) {
                            case (0xF):
                                l=disasm[n+3]+256*disasm[n+2]+256*256*disasm[n+1]+256*256*256*disasm[n];
                                n+=4;
                                break;
                            case (0xE):
                                l=disasm[n+1]+256*disasm[n];
                                n+=2;
                                break;
                            case (0xD):
                                l=disasm[n];
                                n+=1;
                                break;
                        }
                        if (l && l!=0x100)
                            warningf("Strange number for sig size");
                        /*
                        if (l==0xE) {
                            l=disasm[n+1]+256*disasm[n];
                            n+=2;
                        } else {
                            warningf("Strange number for sig size");
                            l=disasm[n++];
                        }*/

                        colorf(DZ8X_BLUE);
                        for (c=0;(unsigned)c<l;c++,n++)
                            printf("%s",HexConv(disasm[n]));

                        /* Relocate code */
                        if (Base) { // #tisux
                            for (c=0;(unsigned)c<(meml-Base);c++)
                                disasm[c+2]=disasm[c+Base];
                            d=meml-Base+2;
                        } else {
                            for (c=0;(unsigned)c<(meml-PLACE2);c++)
                                disasm[c+2]=disasm[c+PLACE2];
                            d=meml-PLACE2+2;
                        }

                        // App packing end

                    } else { // packaged and bin files
                        for (c=0;c<d;c++)
                            printf("%s",HexConv(disasm[c]=getc(in)));
                        colorf(DZ8X_RESET);
                    }
                    PLACE2=2;

                    printf("\n");
                    uint8_t BASIC=15; // BASIC/All
                    if (has_extension(argv[2], ".8ek") || has_extension(argv[2], ".8eu"))
                        BASIC=0x20;
                    if (has_extension(argv[2], ".8ck"))
                        BASIC=0x21;
                    if (has_extension(argv[2], ".8xk"))
                        BASIC=0x22;
                    /* BASIC is for stuff that will affect execution
                     *  The first three bits of BASIC is the platform: MEM[BASIC&7].USERMEM;
                     *   0 - eZ80
                     *   1 - CSE Z80
                     *   2 - 83/84 plus Z80
                     *   3 - 85/86 Z80
                     *   4 - 82
                     *   7 - Any?
                     *  bit 3 = basic
                     *  bit 4 = ice source (ion source too?)
                     *  bit 5 = app/os
                     */
                    /* TODO: This is incredibly messy, how could it be improved? */
                    if (!(BASIC&32)) {
                        if (has_extension(argv[2], ".boot") || has_extension(argv[2], ".rom")) {
                            BASIC=0;
                        } else {
                            switch (disasm[0]) { // Should I preform FILETYPE checks on these too?  Probably not because that would be filist
                                case (0xEF):
                                    if (disasm[1]==0x7B) BASIC=0; // eZ80
                                    if (disasm[1]==0x69) BASIC=1; break; // CSE Z80
                                case (0xBB): if (disasm[1]==0x6D) BASIC=2; break; // 83 Asm
                                case (0x8E): if (disasm[1]==0x28) BASIC=3; break; // 86 asm
                                case (0xEC): if (FILETYPE==85 || FILETYPE==86) BASIC=3; break; // 85 asm (Yes, EC is a token, it's stupid, I know)
                                case (0xD5): if (FILETYPE==82) BASIC=4; break;
                            }
                            if (BASIC&8) { // if BASIC, check...
                                if (disasm[0]==',') BASIC=24; // ICE S (Basic|TI84CE|ICES) ',' == [i]
                                if (disasm[0]==':') BASIC=10; // ION S (Basic|TI83)        ':' == .
                            }
                        }

                        if (((FILETYPE==85 || FILETYPE==86) && (KIND==0x12)) ||
                            ((FILETYPE==73 || FILETYPE==82 || FILETYPE==83 || FILETYPE==84) && (KIND==5 || KIND==6))) {
                            // programs have code type displayed :P
                            colorf(DZ8X_RESET);
                            printf("Platform: ");
                            colorf(DZ8X_GRAY);
                            switch (BASIC&7) {
                                case (0): printf("TI-84 Plus CE\n"); break;
                                case (1): printf("TI-84 Plus C Silver Edition\n"); break;
                                case (2): printf("TI-83/84 Plus / TI-83/84 Plus Silver Edition\n"); break;
                                case (3): printf("TI-85 / TI-86 (?)\n"); break;
                                case (4): printf("TI-82 (?)\n"); break;
                                case (7): printf("All TI-83/84\n"); break;
                                default: errorNL("Platform problem, Please report.");
                            }
                            colorf(DZ8X_RESET);
                            printf("Code Type: ");
                            colorf(DZ8X_GRAY);
                            if (BASIC&8) {
                                if (BASIC==10) printf("Ion Source%s\n",disasm[1]==':'?" Subprogram":"");
                                else if (BASIC==24) printf("ICE Source%s\n",disasm[1]==','?" Subprogram":""); // ICE Source; You can thank me later, P_T
                                else printf("TI-BASIC\n");
                            } else
                                switch (BASIC&7) {
                                    case (0): // ICE; Yup, definitely thank me later.
                                        printf("%seZ80\n",!disasm[2]?"C ":disasm[2]==0x7F?"ICE ":""); break;
                                    case (2): if (disasm[2]==0xC9) printf("Ion/Mirage ");
                                    case (1): // CSE
                                        printf("Z80\n");
                                        warningf("Z80 isn't officially supported."); break;
                                    case (3): printf("85/86 Z80 (?)\n"); warningf("TI-85 / TI-86 Z80 (?) is not implemented"); break;
                                    case (4): printf("82 Z80? What am I supposed to do with this?"); break;
                                    default:
                                        errorNL("Program type problem, Please report.");
                                }
                        }
                    }
                    colorf(DZ8X_RESET);

                    if (Package) { // I can likely do this before creating disasm...
                        /* Packaging shown somewhere above */
                        printf("Name: ");
                        colorf(DZ8X_GRAY);
                        printf("%s\n",repl_str(argv[5],"[",theta));
                        colorf(DZ8X_RESET);
                        printf("Status: ");
                        colorf(DZ8X_GRAY);
                        printf("%srchived\n",atoi(argv[4])?"A":"Una");
                        colorf(DZ8X_RESET);
                        printf("Packaging %s to 8xp file...",ascii?"hexadecimal":"binary");


                        /* Use a struct for this? */
                        uint8_t fileO[d+76];
                        for(c=11;c--;) // print header
                            fileO[c]=SIGN[NXP].v[c];
                        char *comment="< Packaged by DZ8x (C) LogicalJoe >";
                        //  *SComment="< Created by jsTIfied - www.cemetech.net >";
                        //  *CEmument="Exported via CEmu ";
                        //  *TICnct  ="Created by TI Connect CE 5.4.0.1181";
                        //  *Spasm0  ="File generated by WabbitSign";
                        //  *Spasm1  ="File generated by spasm";
                        //  *fasmg   ="";
                        for(c=0;c<(int)strlen(comment);c++) // comment
                            fileO[c+11]=comment[c];
                        for(;c<42;c++) // comment whitespace (might not be white in allocated space)
                            fileO[c+11]=0;
                        fileO[53]=(d+19)&255; // size
                        fileO[54]=(d+19)>>8;
                        fileO[55]=0x0D; // header size
                        fileO[56]=0x00;
                        fileO[57]=(d+2)&255; // size
                        fileO[58]=(d+2)>>8;
                        fileO[59]=(BASIC&8)?5:6; // if !basic protect the code :P
                        for(c=0;c<(int)strlen(argv[5]);c++) // name
                            fileO[c+60]=argv[5][c];
                        for(;c<9;c++) // name whitespace
                            fileO[c+60]=0;
                        fileO[69]=atoi(argv[4])<<7; // archive
                        fileO[70]=(d+2)&255; // size
                        fileO[71]=(d+2)>>8;
                        fileO[72]=d&255; // size
                        fileO[73]=d>>8;
                        for(c=0;c<d;c++) // code
                            fileO[c+74]=disasm[c];
                        uint16_t checks=0;
                        for(c=55;c<(d+74);c++) // calculate checksum
                            checks+=fileO[c];
                        fileO[d+74]=checks&255; // checksum
                        fileO[d+75]=checks>>8;
                        out=fopen(argv[3],"w"); // Something broke :/
                        fwrite(fileO,1,d+76,out); // output to final 8xp
                        fclose(out);
                        printf(" Complete.\n");
                        EndTime(start_time);
                        exit(0);
                    }

                    if (BASIC&8 && (!PackagingFormat(argv[2]) || Cline))
                        warningf("BASIC detected, the following code might not make sense\n"
                                 "For eZ80 and Z80 code, prefix with EF7B and EF69 respectively.");
                REDO_EVERYTHING:;
                    int hx=0, fx=0, off, com=0; // HeX, OFFset, eZ80=!(BASIC&7)
                    /*Universal
                     * d      is the total length of the file
                     * hx     is the command being disassembled
                     * fx     is the occasional subcommand being disassembled
                     * off    is the offset in the hex
                     * com    is the number of opcodes/tokens disassembled (for display at the end)
                     * op     is the output flag
                     * PLACE2 is the location in disasm[]
                     * BASIC  is the code type flag -
                     *   0b00000111
                     *     0 - eZ80, 1 - CSE Z80, 2 - 83/84 plus Z80,
                     *     3 - 85/86 Z80, 4 - 82 Z80
                     *   0b00001000 - 0-asm/1-basic
                     *   0b00010000 - 0-standard/1-ice source
                     *   0b00100000 - 0-standard/1-Application
                     *TI-BASIC
                     * sfe    is the command char
                     *[e]Z80
                     * adl    is the internal s/l flag
                     * sis    is the display s/l flag
                     * fDD    is the DD flag
                     */
                    if (BASIC&8) { // BASIC
                        PLACE2=0;
                        /* name 1 - B&ISI  - BASIC & ICE Source Interpreter  *
                         * name 2 - IS&BD  - ICE Source & BASIC Detokenizer  *
                         * name 3 - TI-BSD - TIce-BASIC (Source) Detokenizer */
                        off=0;

                        colorf(DZ8X_HONEY);
                        if (FILETYPE==85 || FILETYPE==86) {
                            for(;off<d;com++,off++) {
#ifdef DZ8X_TI85FONT
                                printf("%s",TI85Font[disasm[off]]);
                                if (op) fprintf(out,"%s",TI85Font[disasm[off]]);
#else /*DZ8X_TI85FONT*/
                                c=disasm[off];
                                char *sfe=
                                c==0x17?"≤":c==0x19?"≥":c==0x1C?"→":c==0x1D?"₁₀":c==0xD6?"\n":(char*)&c;
                                printf("%s",sfe);
                                if (op) fprintf(out,"%s",sfe);
#endif /*DZ8X_TI85FONT*/
                            }
                        } else {
                            if (d>1 && (disasm[0]==0xEF && (disasm[1]==0x7A || disasm[1]==0x68))) {
                                notef("Unsquished %sZ80 Prgm token found\n"
#ifdef DZ8X_UNSQSHDASM
                                      "DZ8x will attempt to read the unsquished asm."
#else /*DZ8X_UNSQSHDASM*/
                                      "Disassembly must be done separately in a hex file."
#endif /*DZ8X_UNSQSHDASM*/
                                      ,disasm[1]==0x7A?"e":"");
                                colorf(DZ8X_HONEY);
                            }
                            for(;PLACE2<d;com++,PLACE2+=off) {
                                hx=disasm[PLACE2];
                                off=1;
                                char *sfe=DZBA[hx].Base;
                                if (PLACE2+1 != d) { // prevent the reading of bytes that don't exist
                                    fx=disasm[PLACE2+off++];
                                    switch (hx) {
                                        case (0x5D): // lists
                                            if (fx<=5)
                                                sfe=repl_str("L!","!",SmallNumb(fx)); // 1-6
                                            break;
                                        case (0x5E): // equation vars
                                            if (fx>=0x10 && fx<=0x19)
                                                sfe=BASIC5E[fx-0x10];
                                            else if (fx>=0x20 && fx<=0x2B)
                                                sfe=BASIC5E[fx-0x20]; //-0x2A+10
                                            else if (fx>=0x40 && fx<=0x45)
                                                sfe=BASIC5E[fx-0x2A]; //-0x40+22
                                            else if (fx>=0x80 && fx<=0x82)
                                                sfe=BASIC5E[fx-0x64]; //-0x80+28
                                            break;
                                        case (0x5C): // Matrix
                                        case (0x60): // Pic vars
                                        case (0x61): // GDB vars
                                        case (0xAA): // Str vars
                                            if (fx<=9)
                                                sfe=repl_str(hx==0x5C?"[!]":hx==0x60?"Pic!":hx==0x61?"GDB!":"Str!",
                                                             "!", hx==0x5C?HEX[fx+10]:HEX[fx==9?0:fx+1]);
                                            break;
                                        case (0x62): // graph vars & ICE tokens
                                            if (fx<=0x3C)
                                                sfe=BASIC6X[fx].B62;
                                            /* 10 ICE Tokens */
                                            if (BASIC&16 && fx>9 && fx<20) sfe=ICETokens[fx-10];
                                            break;
                                        case (0x63): // graph vars cont.
                                            if (fx<=0x38)
                                                sfe=BASIC6X[fx].B63;
                                            break;
                                        case (0x7E): // format tokens
                                            if (fx<=0x12)
                                                sfe=BASIC7E[fx];
                                            break;
                                        case (0xBB): // random tokens
                                            sfe=DZBA[fx].BB;
                                            break;
                                        case (0xEF): // extra tokens
                                            if (fx<=0xA6)
                                                sfe=BASICEF[fx];
                                            break;
                                        default: // do nothing
                                            off--; /* Cheating a little here :3 */
                                            break;
                                    }
                                }
                                if (!strcmp(sfe,""))
                                    errorNL("Unknown command encountered, make sure the %s.",
                                            Cline?"hex was typed correctly":"file is not corrupted");
                                printf("%s",sfe);
                                if (op) fprintf(out,"%s",sfe);

                                /* unsquished (e)Z80 */
#ifdef DZ8X_UNSQSHDASM
                                if (((hx==0xEF && (fx==0x7A || fx==0x68)) || (hx==0xBB && fx==0x6C)) && (!PLACE2 && off==2)) {
                                    // this can only occur if it is the first char.
                                    uint16_t a=2,bly=2;
                                    bool e=1;
                                    for (c=0; a<d; c+=(disasm[a++]!=0x3F))
                                        if (!dz_isxupper(disasm[a]) && disasm[a]!=0x3F)
                                            goto NOTSQUISHED;
                                    if (c&1 || !c) goto NOTSQUISHED;
                                    printf("\n");
                                    if (op) putc('\n',out);
                                    BASIC=(fx==0x68)+2*(fx==0x6C);
                                    for (a=2; a<d; a++)
                                        if ((c=disasm[a])!=0x3F) {
                                            if ((e=!e))
                                                // Xcode is weird (should be using the a&1, but a could be skipped with 3F)
                                                disasm[bly++]+=dz_toxvalue(c);
                                            else
                                                disasm[bly]=16*dz_toxvalue(c);
                                        }
                                    PLACE2=2;
                                    if (DEBG) {
                                        colorf(DZ8X_WHITE);
                                        printf("d:%i ch:%i r:%i bly:%i\n",d,off,c,bly); // d=1+d/2 | d=c-1
                                    }
                                    d=bly;
                                    goto REDO_EVERYTHING;
                                }
                            NOTSQUISHED: // Should I do this?
#endif /*DZ8X_UNSQSHDASM*/
                                /* What if... It is really just the Asm token without any doors? */
                                /* This is for DoorsCSE Z80 */
                                if (hx==0xEF && fx==0x68 && !Cline && com) {
                                    // more checks?
                                    printf("\n");
                                    if (op) fputc('\n',out); // fix slight newline problem
                                    warningf("Asm84CPrgm token encountered, Doors Z80 assumed"); // Sets color 0
                                    BASIC=1;
                                    PLACE2+=off+1;
                                    int pl=PLACE2+2; // store the old loco
                                    PLACE2+=disasm[PLACE2]+256*disasm[PLACE2+1]; // doors formatting.
                                    if (DEBG) {
                                        colorf(DZ8X_WHITE);
                                        printf("d:%i PL:%i da:%i ch:%i\n",d,PLACE2, disasm[0],off);
                                    }
                                    /* This is just for data between BASIC and Z80 */
                                    uint16_t b;
                                    for (c=pl; c<PLACE2;) {
                                        colorf(DZ8X_RESET);
                                        uint8_t q2=0,g=disasm[c]+256*disasm[c+1]; // size of section
                                        printf("%s (%i):",disasm[c+2]==0xff?"End":DOORHEAD[disasm[c+2]],g);
                                        for (b=c;b<(c+g);b++) { // output the contents nicely :P
                                            uint8_t q=disasm[b+3];
                                            if (q2<32 || q2>126 || q<32 || q>126) printf(" ");
                                            if (q>31 && q<127) {
                                                colorf(DZ8X_GRAY);
                                                printf("%c",q);
                                            } else {
                                                colorf(DZ8X_BLUE);
                                                printf("%s",HexConv(q));
                                            }
                                            q2=q;
                                        }
                                        printf("\n");
                                        c+=g+3;
                                    };
                                    colorf(DZ8X_RESET);
                                    for (c=0; c<=(d-PLACE2); c++)
                                        disasm[c+2]=disasm[c+PLACE2];
                                    d-=PLACE2-1;
                                    PLACE2=2;
                                    goto REDO_EVERYTHING;
                                }
                                /* Doors Asm header, see ResistorCalcCse, clipsprt, and the Doors Manual...
                                 Disp "Needs Doors CSE"
                                 OpenLib(DoorsCSE)
                                 ExecLib
                                 Return
                                 Asm84CPrgm ; this is the giveaway.
                                 <newline>
                                 ; word: offset to code (2 byte)
                                 ;
                                 ; section format:
                                 ; word: section size
                                 ; byte: Section type (0=description,1=icon,2=author,3=library,4=halfres,FF=end)
                                 ; stuff (text is NULL terminated)
                                 ; i.e. 0A0003 DoorsCSE 0800
                                 ;      0000FF
                                 ;
                                 ; program code
                                 */
                            }
                        }
                        printf("\n");
                        colorf(DZ8X_RESET);
                    } // Offset so Xcode can collapse code
                    else {
                        bool *CommOp=malloc(sizeof(bool)*d), // command operators, sizeof(bool)==1.
                        adl=!(BASIC&7),
                        adl2=adl, // for non-sticky (nonpersistent) adl changes (preprocessing only)
                        predone=!Tz80;
                        uint32_t *JumpOp=malloc(sizeof(uint32_t)*d), // jump stack.  I think a uint16_t should do.
                        // also needs as adl vector to complement the stack and to accommodate adl jp returns
                        Jump=1, // Label number
                        Stack=1, // size of stack
                        StackOp[Stack_Max], // Stack
                        UserMem=(KIND==0x23)?0x020100:(BASIC&32)?MEM[BASIC&7].APP:MEM[BASIC&7].USERMEM;
                        // 0x020100 is the magic 8eu OS number
                        uint16_t codez=0, // number of entrypoints (not first)
                        Entries[0]; // in PLACE2 format so it is consistant through memory modes
                        //Entries[]={3617,4692,5241,9357,10070,4038,5280,4396}; // for Graph3DC
                        uint8_t *ADL2=malloc(sizeof(uint8_t)*d), // adl ops, sizeof(int)==4
                        oldsis=0, // purely for preprocessing debug display
                        sis=0,fDD;
                        // JumpOp and CommOp are probably filled with trash
                        for (c=d; c--;) {
                            JumpOp[c]=CommOp[c]=0;
                            ADL2[c]=adl; // adl is always active (only is eZ80), unless it encounters a postcom
                        }
                        colorf(DZ8X_RESET);


                        if (has_extension(argv[2], ".boot") || has_extension(argv[2], ".rom")) {
                            UserMem=2;
                            PLACE2=2;
                        } else if (PLACE2 && BASIC^32) {
                            // if PLACE2==0 : then we can look at the beginning symbol
                            // if PLACE2==2 : we are coming from basic...
                            if (op) fprintf(out,".org $%s\n",HexConv(UserMem)); // set dot org for spasm
                            if (Tz80) printf("- ");
                            printf("0\t%s\t",HexConv(UserMem-2)); // easy-peasy!

                            colorf(DZ8X_BLUE);
                            printf("%s%s\t\t",HexConv(disasm[0]),HexConv(disasm[1]));
                            colorf(DZ8X_HONEY);
                            printf(".db $%s,$%s\n",HexConv(disasm[0]),HexConv(disasm[1]));
                            if (op) fprintf(out,".db $%s,$%s\n",HexConv(disasm[0]),HexConv(disasm[1]));

                            if ((BASIC&7)==7) // What about 4 and 5?
                                errorf("Program doesn't start with EF7B EF69 BB6D or 8E28, Please report.");

                            PLACE2=2;
                        }
                        if (Tz80) {
                            colorf(DZ8X_WHITE);
                            printf("Preprocessing, Please wait...%s\n",DEBG?" [Debug mode]":"");
                        }
                        /* On ADL mode -
                         *    The CPU can only make persistent mode switches between ADL mode and Z80 mode as
                         * part of a special control transfer instruction (CALL, JP, RST, RET, RETI, or RETN), or
                         * as part of an interrupt or trap operation. The Program Counter (PC) is thus prevented from
                         * making an uncontrolled jump. When the memory mode is changed in any of these ways, it
                         * remains in its new state until another of these operations changes the mode back. Persis-
                         * tent mode changes are ideal for calling and executing a block of Z80-style code from
                         * within a higher-level ADL mode program.
                         *    The memory mode can be changed by adding a suffix to a CALL, JP, RST, or RET,
                         * RETI, or, RETN instruction. Tables 14 through 20 describe how each of these 4 instruc-
                         * tions function. The individual instructions may perform additional operations that are not
                         * described here. These tables are focused only on the memory mode switching. For more
                         * detailed information, see eZ80® CPU Instruction Set Description on page 77
                         * - UM0077.pdf pages 34-35
                         */

                        colorf(DZ8X_RESET);
                        for (;PLACE2<d;com++,PLACE2+=off) {
                            off=1;
                            fDD=0;
                            hx=disasm[PLACE2];
                            char *sfe=DZ80[hx];
                            if (predone) {
                                sis=0;
                                adl=ADL2[PLACE2];
                            }
                            if ((!predone) || (Tz80 && predone && CommOp[PLACE2]) || (!Tz80)) {
                                //printf(" %s %i ",sfe, hx);
                                if (isZ80ModePrefix(hx) || isixiyPrefix(hx)) {
                                sisLbl:
                                    if (d-PLACE2-off<1) errorf("Incomplete hex code");
                                    switch (hx) {
                                        case (0x40):
                                        case (0x49):
                                        case (0x52):
                                        case (0x5B):
                                            sis=(hx-0x37)/9; // display
                                            adl=(sis>2); // code
                                            break;
                                        case (0xFD):
                                        case (0xDD):
                                            if (fDD) warningf("Stack of DD FD");
                                            fDD=1+(hx==0xFD);
                                            break;
                                        default: errorf("Internal error: Please report.");
                                    }
                                    fx=hx;
                                    sfe=DZ80[hx=disasm[PLACE2+off++]];

                                    // fxhx
                                    // DD40   | opcodetrap=
                                    // DDDD   | opcodetrap=
                                    // 40DD## | opcode=
                                    // 4040   | noni=

                                    if (fDD && (isZ80ModePrefix(hx) || isixiyPrefix(hx)))
                                        sfe="OPCODETRAP=";
                                    else if (isZ80ModePrefix(fx) && isZ80ModePrefix(hx))
                                        sfe="noni=";
                                    else if (isZ80ModePrefix(fx) && isixiyPrefix(hx))
                                        goto sisLbl;
                                    // if (isZ80ModePrefix(hx) || isixiyPrefix(hx)) goto sisLbl;
                                }
                                if (hx==0xCB) {
                                    if (fDD) off++; // The order of opcodes is ddcbnnmm -> mm, (ix+nn), here we read mm.
                                    if (d-PLACE2-off<1) errorf("Incomplete hex code");
                                    fx=disasm[PLACE2+off];
                                    sfe=repl_str(CB[fx>>3],"!",REG[fx&7]);
                                    if (!fDD) off++; // it will get incremented one way or another
                                    if (fDD && (fx+2)&7) { // proper check
                                        sfe="OPCODETRAP=";
                                        off++; // This is here because the ix/iy code won't add it.
                                    }
                                } else if (hx==0xED) {
                                    if (fDD) sfe="OPCODETRAP="; // Hm...
                                    else {
                                        if (d-PLACE2-off<1) errorf("Incomplete hex code");
                                        sfe = ED[fx=disasm[PLACE2+off++]];
                                    }
                                } else if (fDD) {
                                    switch (hx) {
                                            // these codes are drastically different to their base counterparts
                                        case (0x07): sfe="ld= bc, (hl)"; break;
                                        case (0x0F): sfe="ld= (hl), bc"; break;
                                        case (0x17): sfe="ld= de, (hl)"; break;
                                        case (0x1F): sfe="ld= (hl), de"; break;
                                        case (0x27): sfe="ld= hl, (hl)"; break;
                                        case (0x2F): sfe="ld= (hl), hl"; break;
                                        case (0x31): sfe="ld= iy, (hl)"; break;
                                        case (0x37): sfe="ld= ix, (hl)"; break;
                                        case (0x3E): sfe="ld= (hl), iy"; break;
                                        case (0x3F): sfe="ld= (hl), ix"; break;
                                        case (0xE9): sfe="jp= (ix)"    ; break; // this is odd
                                            /* 40DDCB2A(D3) [OPCODETRAP=] */
                                    }
                                }
                                // moved this here for the order of hex-codez
                                if (fDD) { // There must be a better way of doing this...
                                    if (fDD-1) { // iy
                                        sfe=repl_str(sfe," iy"," ix"); // switch iy/ix if necessary
                                        if (strstr(sfe," (hl)")) {
                                            // can I merge this with the flag code?
                                            if (d-PLACE2-off<1)
                                                errorf("Incomplete flag (need 1 byte)");
                                            char* addr = " (iy+$@)";
                                            uint8_t k=disasm[PLACE2+off-(hx==0xCB)];
                                            if (k >= 0x80) {
                                                addr=repl_str(addr,"+","-");
                                                addr=repl_str(addr,"@",HexConv(256-k));
                                            } else addr=repl_str(addr,"@",HexConv(k));
                                            sfe=repl_str(sfe," (hl)",k?addr:" (iy)");
                                            off++;
                                        } else
                                            if (strstr(sfe," hl"))
                                                sfe=repl_str(sfe," hl"," iy");
                                            else {
                                                sfe=repl_str(sfe," h"," iyh");
                                                sfe=repl_str(sfe," l"," iyl");
                                            }
                                    } else { // ix
                                        if (strstr(sfe," (hl)")) {
                                            if (d-PLACE2-off<1)
                                                errorf("Incomplete flag (need 1 byte)");
                                            char* addr = " (ix+$@)";
                                            uint8_t k=disasm[PLACE2+off-(hx==0xCB)];
                                            if (k >= 0x80) {
                                                addr=repl_str(addr,"+","-");
                                                addr=repl_str(addr,"@",HexConv(256-k));
                                            } else addr=repl_str(addr,"@",HexConv(k));
                                            sfe=repl_str(sfe," (hl)",k?addr:" (ix)");
                                            off++;
                                        } else
                                            if (strstr(sfe," hl"))
                                                sfe=repl_str(sfe," hl"," ix");
                                            else {
                                                sfe=repl_str(sfe," h"," ixh");
                                                sfe=repl_str(sfe," l"," ixl");
                                            }
                                    }
                                }

                                /* c:strstr() == basic:inString( */

                                // technically requires an include, but it'll be fine
                                if ((BASIC&7) && hx==0xEF) // if Z80, make life easy.
                                    sfe="b_call($######)";

                                if (strstr(sfe,"######")) { // address

                                b_jump_lbl:
                                    /* .lis uses mb for the first byte of addresses which is by default D0
                                     * so of I said call.lis $A94C,
                                     * it would call D0A94C.
                                     * This is only really useful within the program so it can be assumed that the program changes it to D1.
                                     */
                                    if (!(BASIC&7))
                                        if ((d-PLACE2-off)<(adl+1+!(BASIC&7)))
                                            errorf("Incomplete address or %i bit number (need %i byte[s])",16+8*adl,1-d+PLACE2+off+!(BASIC&7)+adl);
                                    uint32_t t=0;
                                    if (!(BASIC&7) && adl) { // if eZ80, then 24 bits
                                        t+=disasm[PLACE2+off+2];
                                        t*=256;
                                    }
                                    t+=disasm[PLACE2+off+1];
                                    t*=256;
                                    t+=disasm[PLACE2+off];

                                    if (!adl) t=((UserMem+PLACE2) & 0xFF0000)+t; // this is technicly not correct but is the closest I can get for now
                                    if (Tz80 && predone && UserMem<=t && (d+UserMem)>=t && !strstr(sfe,"b_")) {
                                        if (!(BASIC&7)) {
                                            if (adl) // adl mode eZ80
                                                sfe=repl_str(sfe,"$######",repl_str("Add#","#",HexConv(JumpOp[t-UserMem+2])));
                                            else // Z80 mode eZ80
                                                sfe=repl_str(sfe,"$######",repl_str("Add# & $FFFF","#",HexConv(JumpOp[t-UserMem+2])));
                                        } else // Z80
                                            sfe=repl_str(sfe,"$######",repl_str("Add#","#",HexConv(JumpOp[t-UserMem+2])));
                                    }
                                    sfe=repl_str(sfe,"######",HexConv(t&(0x00FFFF+0xFF0000*adl)));

                                    off+=2+(!(BASIC&7) && adl);
                                    // I made an N byte dec to hex converter
                                    // beats my dec to byte converter
                                    // and my dec to nybble converter :P

                                    if (!strcmp(sfe,"call= $50") && BASIC&7) {
                                        sfe="b_jump($######)";
                                        goto b_jump_lbl;
                                    }

                                    /* In call and jp...
                                     * The CE will return a 24-bit address, and
                                     * The CSE & SE only return 16 bit addresses
                                     */
                                }
                                if (strstr(sfe,"@@")) { // normal 8 bit number
                                    if (d-PLACE2-off<1)
                                        errorf("Incomplete 8 bit number (need 1 byte)");
                                    sfe=repl_str(sfe,"@@",HexConv(disasm[PLACE2+off++]));
                                }
                                if (strstr(sfe,"%%")) { // jump relative
                                    if (d-PLACE2-off<1)
                                        errorf("Incomplete jump (need 1 byte)");
                                    char* addr = "+$@";
                                    uint8_t k=disasm[PLACE2+off]; // 8-bit is required for the looping to work properly
                                    k+=++off;
                                    // error correction with sis because the address is measured at the beginning of the func
                                    if (Tz80 && predone) {
                                        int t=PLACE2;
                                        if (k >= 0x82)
                                            t-=256-k;
                                        else
                                            t+=k;
                                        if (2<=t && t<=d)
                                            if (JumpOp[t])
                                                sfe=repl_str(sfe,"$%%",repl_str("Add#","#",HexConv(JumpOp[t])));
                                        /* $00 +$02 2
                                         * $01 +$03 3
                                         * ...
                                         * $7F +$81 129
                                         * $80 -$7E 126
                                         * $81 -$7D 125
                                         * ...
                                         * $FD -$01 1
                                         * $FE  $00 0
                                         * $FF +$01 1

                                         * (With sign): (k>=0x80)?254-k:k+2
                                         * (No   sign): (k>=0x80)?k-254:k+2
                                         */
                                        //printf("Add:%X off:%i k:%i\n",UserMem+t,off,k);
                                    }

                                    if (k >= 0x82)
                                        addr=repl_str(addr,"+","-");
                                    if (!k)
                                        addr=repl_str(addr,"+$@","");
                                    addr=repl_str(addr,"@",HexConv((k>=0x82)?256-k:k));

                                    sfe=repl_str(sfe,"%%",addr);
                                }
                                if (strstr(sfe,"**")) { // flag
                                    if (d-PLACE2-off<1)
                                        errorf("Incomplete flag (need 1 byte)");
                                    char* addr = "+$@";
                                    uint8_t k=disasm[PLACE2+off];
                                    if (k >= 0x80) {
                                        addr=repl_str(addr,"+","-");
                                        addr=repl_str(addr,"@",HexConv(256-k));
                                    } else addr=repl_str(addr,"@",HexConv(k));
                                    sfe=repl_str(sfe,"+$**",k?addr:""); /* $+$00 looks funny */
                                    off++;
                                }
                                // Z80 in/out ops use 8bit ports, while eZ80 uses 16bit ports
                                if ((strstr(sfe,"out=") || strstr(sfe,"in=")) && strstr(sfe,"(bc)") && (BASIC&7))
                                    sfe=repl_str(sfe,"(bc)","(c)");
                            }

                            if (!predone) {
                                /* Only commands that jump set off to zero */

                                colorf(DZ8X_WHITE); // fix warning colors
                                CommOp[PLACE2]=1;
                                ADL2[PLACE2]=adl;

                                //printf("%s\t",sfe);
                                if (DEBG) {
                                    printf("Hex#: %i\t\t| %s\n", PLACE2, sfe);
                                    if (sis!=oldsis)
                                        printf("Adl : %i %i (%s)\n",adl,sis,xtraF[sis]);
                                }
                                oldsis=sis;
                                /* NOTE: on conditional jumps adl should be restored
                                 * 1 2     D1A881  2002            jr.sis z, Add00 & 0xFFFF
                                 =>1 4     D1A883  FE01            cp a, $01
                                 *                 1       Add01:  ; D1A885
                                 *       stuff happens and sets the Z80 bit
                                 * 0 #     D1A885  3A              ret
                                 basically, on those kinds of commands, read the previous siscode (somehow) and set the next sis code accordingly?
                                 */

                                uint32_t Add=0;
                                /* Get Address */
                                //printf("%s\n",sfe);
                                if (strstr(sfe,  "call=") ||
                                    (strstr(sfe, "jp="  ) && strcmp(sfe, "jp= (hl)")) ||
                                    (strstr(sfe, "ld="  ) && strstr(sfe, "$"))) {
                                    // full address code
                                    if (!(BASIC&7) && adl) { // if eZ80 & adl, then 24 bits
                                        Add=256*disasm[PLACE2+off-1];
                                        off--;
                                    }
                                    Add+=disasm[PLACE2+off-1];
                                    Add*=256;
                                    Add+=disasm[PLACE2+off-2];
                                    if (!(BASIC&7) && adl)
                                        off++;
                                    if (!(BASIC&7) && !adl)
                                        Add+=((UserMem+PLACE2) & 0xFF0000);
                                } else if (strstr(sfe, "jr=") || strstr(sfe, "djnz=")) {
                                    // relative address code
                                    uint8_t k=disasm[PLACE2+off-1]+off-1; // How the heck does this work?
                                    Add=UserMem+PLACE2-1;
                                    if (k >= 0x82)
                                        Add-=256-k;
                                    else
                                        Add+=k;
                                }

                                if (strstr(sfe,"jp= $") && Add==MEM[BASIC&7].ZX7)
                                    warningf("ZX7 compression found.");

                                if (strstr(sfe,  "call=") ||
                                    strstr(sfe,  "jr="  ) || strstr(sfe, "djnz="   )  ||
                                    (strstr(sfe, "jp="  ) && strcmp(sfe, "jp= (hl)")) ||
                                    (strstr(sfe, "ld="  ) && strstr(sfe, "$"       ))) {

                                    if (DEBG) printf("%s",HexConv(Add));
                                    Add-=UserMem-2; // Add is an uint32_t so values will loop and no crazy negatives
                                    if (DEBG) printf(" - %i",Add);

                                    /* Valid address? */
                                    if (2<=Add && (unsigned)(d+2)>Add) {

                                        /* Place label */
                                        if (!JumpOp[Add]) {
                                            JumpOp[Add]=Jump++;
                                            if (DEBG) printf("\nLbl#: %i at %i\n",Jump-1, Add);
                                        } else if (DEBG) printf(" (Old: %i)\n",JumpOp[Add]);

                                        /* Increase stack */
                                        if ((strstr(sfe,  "call=") || strstr(sfe, "djnz=")  ||
                                             (strstr(sfe, "jp="  ) &&!strstr(sfe, "jp= $")) || // conditional jp
                                             (strstr(sfe, "jr="  ) &&!strstr(sfe, "jr= $")))&& // conditional jr
                                            !CommOp[Add]) { // unparsed
                                            // inc stack somehow...
                                            StackOp[++Stack]=PLACE2+off;
                                            if (DEBG) printf("Stak: %i (%i->Stak)\n",Stack,StackOp[Stack]);
                                        }

                                        /* Jump to label */
                                        if (!strstr(sfe, "ld=") && !CommOp[Add]) {
                                            PLACE2=Add;
                                            off=0;
                                        }
                                    } else if (DEBG) printf(" Out\n");
                                }

                                /* Decrease stack */
                                if (((strstr(sfe, "jp= $") || strstr(sfe, "jr= $")) && (!(2<=Add && (unsigned)(d+2)>Add) || CommOp[Add]))
                                    // unconditional jp/jr out of code or to code that has already been parsed
                                    || strstr(sfe, "retn=") || strstr(sfe, "reti=") || !strcmp(sfe, "ret=") // Don't forget strcmp is backward
                                    || CommOp[PLACE2+off]) {
                                    do {
                                        if (DEBG && CommOp[PLACE2+off]) printf("Preparsed code\n");
                                        off=0;
                                        PLACE2=StackOp[Stack--];
                                        if (DEBG) printf("Stak: %i\n",Stack);
                                    } while (CommOp[PLACE2] && Stack);
                                }

                                if (!(BASIC&7)) {
                                    if (strstr(sfe, "call=") || strstr(sfe, "rst=" ) ||
                                        strstr(sfe, "jr="  ) || strstr(sfe, "jp="  ) ||
                                        !strcmp(sfe,"ret=" ) || strstr(sfe, "retn=") || strstr(sfe, "reti=" ) ||
                                        strstr(sfe,"OPCODETRAP="))
                                        adl2=adl;
                                    else
                                        adl=adl2;
                                }
                                if (Stack==Stack_Max) errorf("Stack Overflow");
                                if (Stack<=0) {
                                    if (Stack)
                                        warningf("Stack is less then zero, this may be a problem, Please report.");
                                    if (codez) { // this will eventually be used for entrypoints.
                                        if (DEBG) {
                                            colorf(DZ8X_GRAY); // Different color so I can find it
                                            printf("New part.\n");
                                            colorf(DZ8X_RESET);
                                        }
                                        codez--;
                                        PLACE2=Entries[codez];
                                        Stack=1;
                                    } else {
                                        PLACE2=2; // technikly EF7B was already parsed XP
                                        predone=1;
                                        com=0;
                                    }
                                    off=0;
                                }
                                if ((PLACE2+off)==d) { // if the end is reached then predone,
                                    PLACE2=2; // this should only happen if the last command
                                    predone=1; // is a jp/jr and it had already been parsed.
                                    off=com=0; // call should never be the last command.
                                    warningf("End of code reached, this may be a problem");
                                }
                                if (predone==1) {
                                    colorf(DZ8X_WHITE); // accommodate any warning being displayed
                                    printf("Preprocessing complete.\n");
                                    colorf(DZ8X_RESET);
                                    sis=0; // just in case *.*
                                }

                            } else {
                                if (Tz80) { // This could be problematic as it tries to decode an actual opcode before this.
                                    /*if(!CommOp[PLACE2]){//not an opcode
                                     sfe=repl_str(".db $# ; !","#",HexConv(disasm[PLACE2]));
                                     c=disasm[PLACE2];if((c>31&&c<127)||c=='\n')
                                     sfe=repl_str(sfe,"!",c=='\n'?"\\n":(char*)&c);//(*)'n'&:3
                                     else sfe=repl_str(sfe," ; !","");fDD=0;off=1;}*/
                                    if (!CommOp[PLACE2]) {
                                        uint8_t q2=0;
                                        off=0; fDD=0;
                                        sfe=".db \"\"-";
                                        do {
                                            c=disasm[PLACE2+off];
                                            if ((q2<32 || q2>126) ^ (c<32 || c>126)) {
                                                if ((q2<32 || q2>126)&&off)
                                                    sfe=repl_str(sfe,"\"\"-",", \"\"-");
                                                sfe=repl_str(sfe,"\"\"-","\"\"\"-");
                                            }
                                            if (c>31 && c<127)
                                                sfe=repl_str(sfe,"\"\"-",repl_str("@\"\"-","@",c=='\"'?"\\\"":c=='\\'?"\\\\":(char*)&c));
                                            else {
                                                if (off) sfe=repl_str(sfe,"\"\"-",", \"\"-");
                                                sfe=repl_str(sfe,"\"\"-",repl_str("$@\"\"-","@",HexConv(c)));
                                            }
                                            off++;
                                            q2=c;
                                        } while (!CommOp[PLACE2+off] && (PLACE2+off)<d && disasm[PLACE2+off-1] && !JumpOp[PLACE2+off]);
                                        sfe=repl_str(sfe,"\"\"-",(q2>31 && q2<127)?"\"":"");
                                    }

                                    colorf(DZ8X_HONEY);
                                    if (JumpOp[PLACE2]) {
                                        printf("\t\t%i\tAdd%s:  ; (%s)\n",JumpOp[PLACE2],HexConv(JumpOp[PLACE2]),
                                               HexConv(PLACE2+UserMem-2)); // print label
                                        if (op) fprintf(out,"Add%s:  ; (%s)\n",HexConv(JumpOp[PLACE2]),
                                                        HexConv(PLACE2+UserMem-2)); // print label to file
                                    }
                                    for(c=PLACE2+1;c<PLACE2+off;c++)
                                        if (JumpOp[c]) { // accommodate internals
                                            printf("\t\t%i\tAdd%s = $+%i  ; (%s)\n",JumpOp[c],HexConv(JumpOp[c]), c-PLACE2, HexConv(c+UserMem-2));
                                            if (op) fprintf(out,"Add%s = $+%i  ; (%s)\n",HexConv(JumpOp[c]), c-PLACE2, HexConv(c+UserMem-2));
                                            //warningf("Internal label #%s, (Self-Modifying Code?)",HexConv(JumpOp[c]));
                                        }
                                    colorf(DZ8X_RESET);
                                }
                                if (fDD>2) errorf("Internal error: Please report.");

                                if (Tz80 && (adl!=ADL2[PLACE2]))
                                    printf("\t\t\t\tassume adl=%i\n",adl); // print the sis assume
                                // so .s/.l=16/24 bit registers ; is/il=16/24 bits of data must be fetched... What a pain.

                                if (Tz80) printf("%i ",ADL2[PLACE2]);
                                printf("%i\t%s\t",PLACE2,HexConv(PLACE2+UserMem-2));
                                // print PLACE2 and Address
                                colorf(DZ8X_BLUE);
                                for (c=PLACE2;c<=(PLACE2+off-1);c++)
                                    printf("%s",HexConv(disasm[c]));
                                if (off >=8) printf("\n\t\t\t\t"); // if there is way too much hex
                                if (off < 4) printf("\t");
                                if (off < 8) printf("\t");
                                if (CommOp[PLACE2] || !Tz80) // I want to keep the data "=" (only applicable in -Z mode) OR, if ".db"
                                    sfe=repl_str(sfe,"=",xtraF[sis]);
                                colorf(DZ8X_HONEY);
                                printf("%s\n",sfe);
                                if (op) fprintf(out,"\t%s\n",sfe);

                                if (strstr(sfe,"OPCODETRAP="))
                                    warningf("Command parser encountered a Trap.");
                                colorf(DZ8X_RESET);
                            }
                            /* Original PLAN: This would have been refined had I had tried to implement it :P
                             * 1- run to end of code (to a definite ret or jump)?
                             *    and save all addresses jumped to (referenced addresses (ld's) are likely only data...).
                             * 2- Go to the next address on the table and...
                             *    if any addresses left, go to step one,
                             *    otherwise, end.
                             *
                             * Final PLAN:
                             * 1- decode code up to call/jp/jr/ret/djnz
                             * 2- call/djnz - increase stack and jump
                             *    jp/jr     - jump (don't touch the stack)
                             *    ret       - decrease stack
                             * 3- If the new location doesn't have a label, add it;
                             *    If the code has already been parsed, decrease the stack.
                             * 4- Goto step 1 until either:
                             *    1- stack==0 **and** jp/jr out of code (jr is unlikely, but possible)
                             *    2- stack==0 (ret && stack==1)
                             *
                             * Refined Resulting PLAN:
                             * 1- Advance pc
                             *    If the code has already been parsed, decrease the stack and goto step 1.
                             * 2- disassemble opcode
                             *    call/djnz - increase stack and jump
                             *    jp/jr n/c - increase stack and jump    ; & store adl for ret'ing correctly
                             *    jp/jr     - jump (don't touch the stack)
                             *                unless it jumps out of code, then decrease stack
                             *    ret n/c   - do nothing
                             *    ret/reti/retn
                             *              - decrease stack
                             *    ld        - don't do anything
                             *    If the opcode is not above, goto step 1, otherwise continue.
                             * 3- If opcode is not ld or is a trap, store new adl bit, else restore from old adl bit.
                             * 4- If the address doesn't have a label, add it.
                             * 5- Goto step 1 until !stack || end of code is reached.
                             * TODO: Add adl info into PLAN
                             */
                        }
                        if (!Tz80) // Weird bug
                            com++; // ef7b counts?
                        free(CommOp); // free command operators
                        free(JumpOp); // free label   operators
                        free(ADL2);   // free sis/lil operators
                        if (Tz80) printf("%i label%s\n",Jump-1,Jump==2?"":"s");
                    }
                    printf("%i %s%s decoded\n",com,BASIC&8?"token":"opcode",com==1?"":"s");
                }
            } while (Group && in && (FILESIZE>>1)-2>ftell(in));
            if (in) {
                if ((FILESIZE>>1)-2>ftell(in)){
                    printf("=========================");
                    goto VarPack;
                }
                fclose(in);
            }
            if (op) fclose(out);

            // say how long it took
            EndTime(start_time);
            exit(0);
        }
    }
    printf("\nInvalid arguments\nType ");
    colorf(DZ8X_RED);
    printf("%s --help",argv[0]);
    colorf(DZ8X_RESET);
    printf(" for help\n\n");
    return 1;
}
