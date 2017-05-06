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
#include "avilData.h"

prgData_t prgData;

UsrPrgData::UsrPrgData()
{

}

bool UsrPrgData::add(char *variableName, bool value){

    //check if we have to create a program's output value
    if(UsrPrgData::isOutVal(variableName)){
        return IOData::setOutVal(UsrPrgData::isOutVal(variableName), value);
    }
    if(*variableName==*OUT_VALS_IDENTIFIER || *variableName==*IN_ARGS_IDENTIFIER){
        //invalid name...may be you're trying to declare an output val
        //but you already reached MAX_OUT_VALS_NUM or you' retrying to declare an input args!
        return false;
    }
    if (prgData.boolCounter>=MAX_BOOL_NUM){
        //max bool num reached
        return false;
    }
    if(strlen(variableName)>MAX_VARNAMELEN){
        //too long!
        return false;
    }
    strncpy(prgData.booleans[prgData.boolCounter].name, variableName, strlen(variableName));
    prgData.booleans[prgData.boolCounter].name[strlen(variableName)]='\0';
    prgData.booleans[prgData.boolCounter].value = value;
    prgData.boolCounter++;
    return true;
}

bool UsrPrgData::get(char* variableName, bool &value){

    //check if an argument or out val is requested
    if(UsrPrgData::isArgument(variableName)){
        return IOData::getArg(UsrPrgData::isArgument(variableName), value);
    }
    else if(UsrPrgData::isOutVal(variableName)){
        return IOData::getOutVal(UsrPrgData::isOutVal(variableName), value);
    }
    for (uint8_t i = 0; i < MAX_BOOL_NUM; ++i){
        if(strcmp(variableName, prgData.booleans[i].name)==0){
            value = prgData.booleans[i].value;
            return true;
        }
    }
    return false;
}

bool UsrPrgData::set(char* variableName, bool value){

    //check if we have to set a program's output value
    if(UsrPrgData::isOutVal(variableName)){
        return IOData::setOutVal(UsrPrgData::isOutVal(variableName), value);
    }

    for (uint8_t i = 0; i < MAX_BOOL_NUM; ++i){
        if(strcmp(variableName, prgData.booleans[i].name)==0){
            prgData.booleans[i].value = value;
            return true;
        }
    }
    return false;
}

bool UsrPrgData::add(char *variableName, int value){

    //check if we have to create a program's output value
    if(UsrPrgData::isOutVal(variableName)){
        return IOData::setOutVal(UsrPrgData::isOutVal(variableName), value);
    }

    if(*variableName==*OUT_VALS_IDENTIFIER || *variableName==*IN_ARGS_IDENTIFIER){
        //invalid name...may be you're trying to declare an output val
        //but you already reached MAX_OUT_VALS_NUM or you' retrying to declare an input args!
        return false;
    }
    if (prgData.intCounter>=MAX_INT_NUM){
        //max int num reached
        return false;
    }
    if(strlen(variableName)>MAX_VARNAMELEN){
        //too long!
        return false;
    }
    strcpy(prgData.integers[prgData.intCounter].name, variableName);
    prgData.integers[prgData.intCounter].value = value;
    prgData.intCounter++;

    return true;
}

bool UsrPrgData::get(char* variableName, int &value){

    //check if an argument or out val is requested
    if(UsrPrgData::isArgument(variableName)){
        return IOData::getArg(UsrPrgData::isArgument(variableName), value);
    }
    else if(UsrPrgData::isOutVal(variableName)){
        return IOData::getOutVal(UsrPrgData::isOutVal(variableName), value);
    }
    for (uint8_t i = 0; i < MAX_INT_NUM; ++i){
        if(strcmp(variableName, prgData.integers[i].name)==0){
            value  = prgData.integers[i].value;
            return true;
        }
    }
    return false;
}

bool UsrPrgData::set(char* variableName, int value){

    if(UsrPrgData::isOutVal(variableName)){
        return IOData::setOutVal(UsrPrgData::isOutVal(variableName), value);
    }
    for (uint8_t i = 0; i < MAX_INT_NUM; ++i){
        if(strcmp(variableName, prgData.integers[i].name)==0){
            prgData.integers[i].value = value;
            return true;
        }
    }
    return false;
}

bool UsrPrgData::add(char *variableName, char* value){

    //check if we have to create a program's output value
    if(UsrPrgData::isOutVal(variableName)){
        return IOData::setOutVal(UsrPrgData::isOutVal(variableName), value);
    }
    if(*variableName==*OUT_VALS_IDENTIFIER || *variableName==*IN_ARGS_IDENTIFIER){
        //invalid name...may be you're trying to declare an output val
        //but you already reached MAX_OUT_VALS_NUM or you' retrying to declare an input args!
        return false;
    }
    if (prgData.strCounter>=MAX_STRING_NUM){
        //max string num reached
        return false;
    }
    if(strlen(variableName)>MAX_VARNAMELEN){
        //too long!
        return false;
    }
    if(strlen(value)>MAX_STRINGLEN){
        //too long!
        return false;
    }
    strncpy(prgData.strings[prgData.strCounter].name, variableName, strlen(variableName));
    prgData.strings[prgData.strCounter].name[strlen(variableName)]='\0';
    strncpy(prgData.strings[prgData.strCounter].value, value, strlen(value));
    prgData.strings[prgData.strCounter].value[strlen(value)]='\0';
    prgData.strCounter++;
    return true;
}

bool UsrPrgData::get(char* variableName, char* &value){

    //check if an argument or out val is requested
    if(UsrPrgData::isArgument(variableName)){
        return IOData::getArg(UsrPrgData::isArgument(variableName), value);
    }
    else if(UsrPrgData::isOutVal(variableName)){
        return IOData::getOutVal(UsrPrgData::isOutVal(variableName), value);
    }
    for (uint8_t i = 0; i < MAX_STRING_NUM; ++i){
        if(strcmp(variableName, prgData.strings[i].name)==0){
            //strncpy(value, prgData.strings[i].value, strlen(prgData.strings[i].value));
            value = prgData.strings[i].value;
            return true;
        }
    }
    return false;
}

bool UsrPrgData::set(char *variableName, char *value){

    if(UsrPrgData::isOutVal(variableName)){
        return IOData::setOutVal(UsrPrgData::isOutVal(variableName), value);
    }
    for (uint8_t i = 0; i < MAX_STRING_NUM; ++i){
        if(strcmp(variableName, prgData.strings[i].name)==0){
            strcpy(prgData.strings[i].value, value);
            return true;
        }
    }
    return false;
}

bool UsrPrgData::add(char *labelName){

    if (prgData.labelCounter>=MAX_LABEL_NUM){
        //max label num reached
        return false;
    }
    strncpy(prgData.labels[prgData.labelCounter].name, labelName, strlen(labelName));
    prgData.labelCounter++;
    return true;
}

bool UsrPrgData::isBoolean(char* variableName){

    bool l_bValue;
    return UsrPrgData::get(variableName, l_bValue);
}

bool UsrPrgData::isInteger(char* variableName){

    int l_nValue;
    return UsrPrgData::get(variableName, l_nValue);
}

bool UsrPrgData::isString(char* variableName){

    char* l_sValue;
    return UsrPrgData::get(variableName, l_sValue);
}

bool UsrPrgData::isLabel(char* variableName){

    for (uint8_t i = 0; i < MAX_LABEL_NUM; ++i){
        if(strcmp(variableName, prgData.labels[i].name)==0){
            return true;
        }
    }
    return false;
}

void UsrPrgData::clearAll(){

    uint8_t i = 0;

    for (i = 0; i < MAX_BOOL_NUM; ++i){
        prgData.booleans[i].name[0] = '\0';
        prgData.booleans[i].value = false;
    }
    for (i = 0; i < MAX_INT_NUM; ++i){
        prgData.integers[i].name[0] = '\0';
        prgData.integers[i].value = 0;
    }
    for (i = 0; i < MAX_STRING_NUM; ++i){
        prgData.strings[i].name[0] = '\0';
        prgData.strings[i].value[0] = '\0';
    }
    for (i = 0; i < MAX_LABEL_NUM; ++i){
        prgData.labels[i].name[0] = '\0';
    }

    prgData.boolCounter = 0;
    prgData.intCounter = 0;
    prgData.strCounter = 0;
    prgData.labelCounter = 0;
}

void UsrPrgData::clear(char* variableName){

    for (uint8_t i = 0; i < MAX_BOOL_NUM; ++i){
        if(strcmp(variableName, prgData.booleans[i].name)==0){
            prgData.booleans[i].name[0] = '\0';
            prgData.booleans[i].value = false;
            if(prgData.boolCounter>0){
                prgData.boolCounter--;
            }
            return;
        }
    }

    for (uint8_t i = 0; i < MAX_INT_NUM; ++i){
        if(strcmp(variableName, prgData.integers[i].name)==0){
            prgData.integers[i].name[0] = '\0';
            prgData.integers[i].value = 0;
            if(prgData.intCounter>0){
                prgData.intCounter--;
            }
            return;
        }
    }

    for (uint8_t i = 0; i < MAX_STRING_NUM; ++i){
        if(strcmp(variableName, prgData.strings[i].name)==0){
            prgData.strings[i].name[0] = '\0';
            prgData.strings[i].value[0] = '\0';
            if(prgData.strCounter>0){
                prgData.strCounter--;
            }
            return;
        }
    }

    for (uint8_t i = 0; i < MAX_LABEL_NUM; ++i){
        if(strcmp(variableName, prgData.labels[i].name)==0){
            prgData.labels[i].name[0] = '\0';
            if(prgData.labelCounter>0){
                prgData.labelCounter--;
            }
            return;
        }
    }
}

uint8_t UsrPrgData::isArgument(char* variableName){

    //if the variable is an argument return the argument index, 0 otherwise
    char l_sArgName[3];
    char l_sArgIndex[2];

    for (uint8_t i = 1; i <= MAX_INPUT_ARGS_NUM; ++i) {
        l_sArgName[0] = '\0';
        l_sArgIndex[0] = '\0';
        sprintf(l_sArgIndex, "%u", i);
        strcat(l_sArgName, IN_ARGS_IDENTIFIER);
        strcat(l_sArgName, l_sArgIndex);
        if (strcmp(variableName, l_sArgName)==0){
            return i;
        }
    }
    return 0;
}

uint8_t UsrPrgData::isOutVal(char* variableName){

    //if the variable is an output value return the index, 0 otherwise
    char l_sOutValName[3];
    char l_sOutValIndex[2];

    for (uint8_t i = 1; i <= MAX_OUT_VALS_NUM; ++i) {
        l_sOutValIndex[0] = '\0';
        l_sOutValName[0] = '\0';
        sprintf(l_sOutValIndex, "%u", i);
        strcat(l_sOutValName, OUT_VALS_IDENTIFIER);
        strcat(l_sOutValName, l_sOutValIndex);
        if (strcmp(variableName, l_sOutValName)==0){
            return i;
        }
    }
    return 0;
}

inData_t inArgs[MAX_INPUT_ARGS_NUM];
outData_t outVals[MAX_OUT_VALS_NUM];

IOData::IOData()
{

}

bool IOData::getArg(uint8_t index, bool &value){

    if(index<=MAX_INPUT_ARGS_NUM && inArgs[index-1].isBoolean){
        value = inArgs[index-1].value.boolean;
        return true;
    }
    //out of range or not defined argument!
    return false;
}

bool IOData::getArg(uint8_t index, int &value){

    if(index<=MAX_INPUT_ARGS_NUM && inArgs[index-1].isInteger){
        value = inArgs[index-1].value.integer;
        return true;
    }
    //out of range or not defined argument!
    return false;
}

bool IOData::getArg(uint8_t index, char* &value){

   if(index<=MAX_INPUT_ARGS_NUM && inArgs[index-1].isString){
        value = inArgs[index-1].value.string;
        return true;
    }
    //out of range or not defined argument!
    return false;
}

bool IOData::setArg(uint8_t index, bool value){

    if(index<=MAX_INPUT_ARGS_NUM){
        inArgs[index-1].value.boolean = value;
        inArgs[index-1].isBoolean = true;
        inArgs[index-1].isInteger = false;
        inArgs[index-1].isString = false;
        return true;
    }
    //out of range!
    return false;
}

bool IOData::setArg(uint8_t index, int value){

    if(index<=MAX_INPUT_ARGS_NUM){
        inArgs[index-1].value.integer = value;
        inArgs[index-1].isBoolean = false;
        inArgs[index-1].isInteger = true;
        inArgs[index-1].isString = false;
        return true;
    }
    //out of range!
    return false;
}

bool IOData::setArg(uint8_t index, char* value){

    if(index<=MAX_INPUT_ARGS_NUM){

        inArgs[index-1].value.string[0]='\0';
        strncat(inArgs[index-1].value.string, value, MAX_STRINGLEN);
        inArgs[index-1].isBoolean = false;
        inArgs[index-1].isInteger = false;
        inArgs[index-1].isString = true;
        return true;
    }
    //out of range!
    return false;
}

bool IOData::getOutVal(uint8_t index, bool &value){

    if(index<=MAX_OUT_VALS_NUM && outVals[index-1].isBoolean){
        value = outVals[index-1].value.boolean;
        return true;
    }
    //out of range or not defined value!
    return false;
}

bool IOData::getOutVal(uint8_t index, int &value){

    if(index<=MAX_OUT_VALS_NUM && outVals[index-1].isInteger){
        value = outVals[index-1].value.integer;
        return true;
    }
    //out of range or not defined value!
    return false;
}

bool IOData::getOutVal(uint8_t index, char* &value){

    if(index<=MAX_OUT_VALS_NUM && outVals[index-1].isString){
        value = outVals[index-1].value.string;
        return true;
    }
    //out of range or not defined value!
    return false;
}

bool IOData::setOutVal(uint8_t index, bool value){

    if(index<=MAX_OUT_VALS_NUM){
        outVals[index-1].value.boolean = value;
        outVals[index-1].isBoolean = true;
        outVals[index-1].isInteger = false;
        outVals[index-1].isString = false;
        return true;
    }
    //out of range!
    return false;
}

bool IOData::setOutVal(uint8_t index, int value){

    if(index<=MAX_OUT_VALS_NUM){
        outVals[index-1].value.integer = value;
        outVals[index-1].isBoolean = false;
        outVals[index-1].isInteger = true;
        outVals[index-1].isString = false;
        return true;
    }
    //out of range!
    return false;
}

bool IOData::setOutVal(uint8_t index, char *value){

    if(index<=MAX_OUT_VALS_NUM){
        outVals[index-1].value.string[0]='\0';
        strncat(outVals[index-1].value.string, value, MAX_FILE_LINE_LENGTH);
        outVals[index-1].isBoolean = false;
        outVals[index-1].isInteger = false;
        outVals[index-1].isString = true;
        return true;
    }
    //out of range!
    return false;
}

void IOData::clearArg(uint8_t index){

    if(index<=MAX_INPUT_ARGS_NUM){
        inArgs[index-1].isBoolean = false;
        inArgs[index-1].isInteger = false;
        inArgs[index-1].isString = false;
    }
}

void IOData::clearOutVal(uint8_t index){

    if(index<=MAX_OUT_VALS_NUM){
        outVals[index-1].isBoolean = false;
        outVals[index-1].isInteger = false;
        outVals[index-1].isString = false;
    }
}
