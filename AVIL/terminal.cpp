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
#include "terminal.h"

terminal::terminal()
{

}

void terminal::prompt(){

    char cmd[MAX_FILE_LINE_LENGTH+1];
    char l_programName[MAX_PRG_NAME_LENGTH+1];

    Sys::init();

    while(true){
        Sys::userOutput(F("\n\r>"));
        //get a cmd input
        cmd[0]='\0';
        //get a user input
        if(Sys::userInput(cmd, MAX_FILE_LINE_LENGTH+1)){
            Sys::userOutput(F("\n\r"));
            //get the program's name and arguments
            if(this->getPrgName(cmd, l_programName) && this->getPrgArguments(cmd)){
                if(Sys::sysProgramExists(l_programName)){
                    //is a system program
                    Sys::callPrg(l_programName);
                }
                #ifdef AVIL_INTERPRETER
                else if(Sys::fileExists(l_programName)){
                    avil interpreter;
                    interpreter.run(l_programName);
                }
                #endif
                else{
                    Sys::userOutput(cmd);
                    this->cmdErr();
                }
            }
            else{
                Sys::userOutput(F("can't parse input!\n\r"));
                Sys::userOutput(cmd);
                this->cmdErr();
            }
        }
        else{
            Sys::userOutput(F("bad input!\n"));
            this->cmdErr();
        }
    }

}

bool terminal::getPrgName(const char *program_name_and_args, char* program_name){

    //get the program name from the input cmd (made of program's name and arguments)
    const char* args = program_name_and_args;
    uint8_t len = 0;
    while(isalnum(*args)){
        ++args;
        len++;
        if(len > MAX_PRG_NAME_LENGTH){
            return false;
        }
    }
    memcpy(program_name, program_name_and_args, args - program_name_and_args);
    program_name[args - program_name_and_args] = 0;
    return true;
}

bool terminal::getPrgArguments(const char *program_name_and_args){

    const char* args = program_name_and_args;
    uint8_t len = 0;

    //move at the args beginning
    while(isalnum(*args)){
        ++args;
        len++;
        if(len > MAX_PRG_NAME_LENGTH){
            return false;
        }
    }
    //set up a tokenizer to look for the arguments
    Tokenizer l_tokenizer;
    l_tokenizer.setStatements(args);
    l_tokenizer.next();

    uint8_t i = 1;

    while(l_tokenizer.getToken() != TOKEN_ENDOFINPUT){

        if (l_tokenizer.getToken() == TOKEN_NUMBER){
            int l_nArg;
            l_tokenizer.getTokenIntValue(l_nArg);
            if(!IOData::setArg(i, l_nArg)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_STRING){
            char l_sArg[MAX_STRINGLEN + 1];
            if(!l_tokenizer.getTokenStringValue(l_sArg)){
                return false;
            }
            if(!IOData::setArg(i, l_sArg)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_INPUT){
            char l_sArg[MAX_STRINGLEN + 1];
            strcpy(l_sArg, "INPUT");
            if(!IOData::setArg(i, l_sArg)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_OUTPUT){
            char l_sArg[MAX_STRINGLEN + 1];
            strcpy(l_sArg, "OUTPUT");
            if(!IOData::setArg(i, l_sArg)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_TRUE){
            if(!IOData::setArg(i, true)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_FALSE){
            if(!IOData::setArg(i, false)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_VAR_NAME){

            char l_varName[MAX_VARNAMELEN + 1];
            if(!l_tokenizer.getTokenVariableName(l_varName)){
                return false;
            }

            if(UsrPrgData::isBoolean(l_varName)){
                //boolean
                bool l_bResult = false;
                UsrPrgData::get(l_varName, l_bResult);
                IOData::setArg(i, l_bResult);
            }
            else if(UsrPrgData::isInteger(l_varName)){
                //integer
                int l_nResult;
                UsrPrgData::get(l_varName, l_nResult);
                IOData::setArg(i, l_nResult);
            }
            else if(UsrPrgData::isString(l_varName)){
                //string
                char* l_sResult;
                UsrPrgData::get(l_varName, l_sResult);
                IOData::setArg(i, l_sResult);
            }
            else{
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_LEFTPAREN || l_tokenizer.getToken() == TOKEN_RIGHTPAREN || l_tokenizer.getToken() == TOKEN_COMMA){
            //do nothing...just skip to the next
            l_tokenizer.next();
        }
        else{
            //Sys::userOutput("argerr");
            return false;
        }
    }
    return true;
}

void terminal::cmdErr(){

    Sys::userOutput(F("???\n\r"));
    Sys::userOutput(F("*******AVIL TERMINAL*******"));
    Sys::userOutput(F("\n\r"));

}
