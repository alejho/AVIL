/* 
 * Copyright 2017 Alessio Villa
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#ifndef COMPOPT
#define COMPOPT
#define ARDU_UNO 0
#define PC 1


/************************************************************************************************/
/*COMPILE OPTIONS (customize your system here!) */
/************************************************************************************************/


//uncomment only the hw platform you'r using currently
//#define SYSTEM PC
#define SYSTEM ARDU_UNO

#if SYSTEM == ARDU_UNO

    //userInput/Output over ethernet (comment for use serial)
    //#define ETH_IO

    #ifdef ETH_IO
        //ip address www.xxx.yyy.zzz
        #define IP_www  192
        #define IP_xxx  168
        #define IP_yyy  1
        #define IP_zzz  177
        //telnet\webServer port
        #define PORT 23
    #endif

    //meArm controller extension
    //#define MEARM
    #ifdef MEARM
        //when using avil as meArm controller ehternet will be disabled!
        #undef ETH_IO
    #endif

    //debug additional functions
    //#define DEBUG

#endif


/////////////////////////////LOOK///////////////////////////////////////
//max number of input arguments allowed for AVIL's programs
#define MAX_INPUT_ARGS_NUM 2
//max number of output values allowed by AVIL's programs
#define MAX_OUT_VALS_NUM 1
//max chars number of an AVIL's string
#define MAX_STRINGLEN  16
//max chars number of a program's name (more than 8+3 it's useless with this filesystem!)
#define MAX_PRG_NAME_LENGTH 11
//max chars number of a program's (and file's) line
#define MAX_FILE_LINE_LENGTH  32
//max number of digit of a numeric value
#define MAX_NUMLEN  5
//max chars number of a variable's name (2 is the minimum)
#define MAX_VARNAMELEN  3
/////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////EXECUTION///////////////////////////////////////////////
//max number of nested if's in one program
#define MAX_NESTED_IF 2
//max number of nested for's loop in one program
#define MAX_NESTED_FOR 2
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////MEMORY/////////////////////////////////////////////////
//boolean variables number
#define MAX_BOOL_NUM 5
//integer variables number
#define MAX_INT_NUM 5
//string variables number
#define MAX_STRING_NUM 2
//labels number
#define MAX_LABEL_NUM 3
/////////////////////////////////////////////////////////////////////////////////////////













#if SYSTEM == ARDU_UNO
    #include <avr/pgmspace.h>
    #include <Arduino.h>
    template <typename T> T PROGMEM_get (const T * sce, T& dest){
        memcpy_P (&dest, sce, sizeof (T));
    }
#elif SYSTEM == PC
    #define PROGMEM
    template <typename T> T PROGMEM_get (const T * sce, T& dest){
        memcpy (&dest, sce, sizeof (T));
    }
#endif

#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))


const char IN_ARGS_IDENTIFIER[]="$";
const char OUT_VALS_IDENTIFIER[]="@";


#endif // COMPOPT

