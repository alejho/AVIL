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
#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "compopt.h"
#include <stdint.h>
#include <string.h> //strlen
#include <stdio.h>

typedef struct boolVariableStruct{

    char name[MAX_VARNAMELEN + 1];
    bool value;
}prgBool_t;

typedef struct intVariableStruct{

    char name[MAX_VARNAMELEN + 1];
    int value;
}prgInt_t;

typedef struct stringVariableStruct{

    char name[MAX_VARNAMELEN + 1];
    char value[MAX_STRINGLEN + 1];
}prgString_t;

typedef struct labelVariableStruct{
    //the name of a label is also its value!
    char name[MAX_VARNAMELEN + 1];
}prgLabel_t;

typedef struct usrPrgDataStruct{

    uint8_t boolCounter;
    uint8_t intCounter;
    uint8_t strCounter;
    uint8_t labelCounter;

    prgBool_t booleans[MAX_BOOL_NUM];
    prgInt_t integers[MAX_INT_NUM];
    prgString_t strings[MAX_STRING_NUM];
    prgLabel_t labels[MAX_LABEL_NUM];

}prgData_t;

extern prgData_t prgData;

class UsrPrgData
{
public:

    bool static add(char* variableName, bool value);
    bool static get(char* variableName, bool &value);
    bool static set(char* variableName, bool value);

    bool static add(char* variableName, int value);
    bool static get(char* variableName, int &value);
    bool static set(char* variableName, int value);

    bool static add(char* variableName, char* value);
    bool static get(char* variableName, char* &value);
    bool static set(char* variableName, char* value);

    //for labels the name itself is the variable
    bool static add(char* labelName);

    bool static isBoolean(char* variableName);
    bool static isInteger(char* variableName);
    bool static isString(char* variableName);
    bool static isLabel(char* variableName);

    void static clearAll();
    void static clear(char* variableName);

private:

    UsrPrgData();
    uint8_t static isArgument(char* variableName);
    uint8_t static isOutVal(char* variableName);
};

union inDataCont_t{

    bool boolean;
    int integer;
    char string[MAX_STRINGLEN + 1];
};

union outDataCont_t{

    bool boolean;
    int integer;
    char string[MAX_FILE_LINE_LENGTH + 1];
};

typedef struct inDataStruct{

    bool isBoolean;
    bool isInteger;
    bool isString;
    inDataCont_t value;

}inData_t;

typedef struct outDataStruct{

    bool isBoolean;
    bool isInteger;
    bool isString;
    outDataCont_t value;

}outData_t;


extern inData_t inArgs[MAX_INPUT_ARGS_NUM];
extern outData_t outVals[MAX_OUT_VALS_NUM];

class IOData
{
public:

    bool static getArg(uint8_t index, bool &value);
    bool static getArg(uint8_t index, int &value);
    bool static getArg(uint8_t index, char* &value);

    bool static setArg(uint8_t index, bool value);
    bool static setArg(uint8_t index, int value);
    bool static setArg(uint8_t index, char* value);

    bool static getOutVal(uint8_t index, bool &value);
    bool static getOutVal(uint8_t index, int &value);
    bool static getOutVal(uint8_t index, char* &value);

    bool static setOutVal(uint8_t index, bool value);
    bool static setOutVal(uint8_t index, int value);
    bool static setOutVal(uint8_t index, char* value);

    void static clearArg(uint8_t index);
    void static clearOutVal(uint8_t index);

private:

    IOData();

};

#endif // DATAHANDLER_H
