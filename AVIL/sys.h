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
#ifndef SYSTEM_H
#define SYSTEM_H

#include "compopt.h"
#include "stdint.h"
#include "avilData.h"


#if SYSTEM == ARDU_UNO
       #define HW "sysArduinoUNO.h"
#elif SYSTEM == PC
       #define HW "sysPC.h"
       #define F(x) x
#else
       #define HW "default.h"
#endif
#include HW


typedef struct sysProgramsStruct{
    const char* name;
    bool (*pFunc)();
}sysPrograms_t;


class Sys
{
public:


    bool static init();
    bool static fileExists(const char* fileName);
    bool static getFileLine(char* fileName, unsigned int lineNumber, char *line);

    //input from user
    bool static userInput(char* dst, size_t size);
    //output to user
    void static userOutput(const char* msg);
    #if SYSTEM == ARDU_UNO
    void static userOutput(const __FlashStringHelper* msg);
    #endif
    void static userOutput(char c);
    void static userOutput(int num);
    bool static killSignalReceived();
    bool static callPrg(const char* sysProgramName);
    bool static sysProgramExists(const char* programName);
    //runtime error
    void static runtimeError(unsigned int errorCode, unsigned int programLine);

private:

    Sys();

};

#endif // SYSTEM_H

