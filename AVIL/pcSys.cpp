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
#include "sys.h"
#if SYSTEM == PC

const sysPrograms_t systemProgams[] = {

    {"ls", &ls},
    {"free", &free},
    {"setPM", &setPinMode},
    {"getPM", &getPinMode},
    {"dioW", &setPinStatus},
    {"dioR", &getPinStatus},
    {"delay", &idle},
    {"cat", &cat},
    {"touch", &touch},
    {"cp",&copy},
    {"rm", &rm},
    {"append", &append},
    {"fgetl", &fgetl},
    {"flen", &flen},
    {"fex", &fex},
    #ifdef DEBUG
    {"memDump", &memDump},
    #endif

    //add custom function's reference here
    /*{"fooAlias", &foo},*/

    {NULL, NULL}
};

fakePin_t ioPin[] = {
    {false, false},
    {false, false},
    {false, false},
    {false, false},
    {false, false},
    {false, false},
    {false, false},
    {false, false},
    {false, false},
    {false, false},
    {false, false},
    {false, false},
    {false, false},
    {false, false}
};


Sys::Sys()
{
}

bool Sys::init(){

    return true;
}


bool Sys::fileExists(const char* fileName){

    struct stat buf;
    int Result = stat(fileName, &buf);
    return Result == 0;

}

bool Sys::getFileLine(char* fileName, unsigned int lineNumber, char* line){

    std::ifstream l_programFile(fileName);
    std::string l_lineStatements;
    unsigned int i = 0;
    while(i<lineNumber && std::getline(l_programFile, l_lineStatements)){
        i++;
    }
    if(i == lineNumber){
        line[0]='\0';
        //so be careful...sizeof(line) must be greater than MAX_FILE_LINE_LENGTH
        if (l_lineStatements.length()==MAX_FILE_LINE_LENGTH){
            strncat(line, l_lineStatements.c_str(), MAX_FILE_LINE_LENGTH);
            line[MAX_FILE_LINE_LENGTH-1]='\n';
            line[MAX_FILE_LINE_LENGTH]='\0';
            l_programFile.close();
            return true;
        }
        else{
            strncat(line, l_lineStatements.c_str(), MAX_FILE_LINE_LENGTH);
            if(line[MAX_FILE_LINE_LENGTH-1] != '\n'){
                strncat(line, "\n", MAX_FILE_LINE_LENGTH);
            }
            return true;
        }

        return true;
    }
    else{
        l_programFile.close();
        return false;
    }

}

bool Sys::userInput(char* dst, size_t size){

    char* c;

    fgets(dst, size, stdin);
    c=strchr(dst, '\n');
    if(c!=NULL){
        char *pr = dst, *pw = dst;
        while (*pr){
            *pw = *pr++;
            pw += (*pw != '\n');
        }
        *pw = '\0';
        return true;
    }
    return false;
}

void Sys::userOutput(const char* msg){
    printf("%s", msg);
}

void Sys::userOutput(char c){
    printf("%c", c);
}


void Sys::userOutput(int num){
    printf("%d", num);
}

bool Sys::killSignalReceived(){

    return false;
}

bool Sys::callPrg(const char* sysProgramName){

    sysPrograms_t SysProgram;

    for(uint8_t i = 0; i<ARRAYSIZE(systemProgams)-1; i++){

        PROGMEM_get(&systemProgams[i], SysProgram);
        bool l_bSysProgramFound = strncmp(sysProgramName, SysProgram.name, strlen(SysProgram.name)) == 0;
        if (l_bSysProgramFound){
            //execute the sys program
            bool l_bRes = false;
            l_bRes = SysProgram.pFunc();
            //clear the args
            for (uint8_t j = 1; j <= MAX_INPUT_ARGS_NUM; j++) {
                IOData::clearArg(j);
            }
            if(!l_bRes){
                Sys::userOutput(F("call error: "));
                Sys::userOutput(sysProgramName);
                Sys::userOutput(F("\n\r"));
            }
            return l_bRes;
        }
    }
    Sys::userOutput(F("the program "));
    Sys::userOutput(sysProgramName);
    Sys::userOutput(F("is not implemented on this hardware \n"));
    return false;
}

bool Sys::sysProgramExists(const char* programName){

    sysPrograms_t SysProgram;
    for(uint8_t i = 0; i<ARRAYSIZE(systemProgams)-1; i++){
        PROGMEM_get(&systemProgams[i], SysProgram);
        bool l_bSysProgramFound = strncmp(programName, SysProgram.name, strlen(SysProgram.name)) == 0;
        l_bSysProgramFound = l_bSysProgramFound && strncmp(programName, SysProgram.name, strlen(programName)) == 0;
        if (l_bSysProgramFound){
            return true;
        }
    }
    return false;
}
void Sys::runtimeError(unsigned int errorCode, unsigned int programLine){
    //print data about a runtime error:
    Sys::userOutput(F("\n\r"));
    //current program line number
    Sys::userOutput(F("program line: "));
    Sys::userOutput((int)programLine);
    Sys::userOutput(F("\n\r"));

    if(Sys::fileExists("errdesc")){
        //error code's description
        char line[MAX_FILE_LINE_LENGTH+1];
        if(Sys::getFileLine("errdesc", errorCode, line)){
            //interpreter error code
            Sys::userOutput(line);
            Sys::userOutput(F("\n\r"));
        }
        //interpreter error code
        Sys::userOutput(F("error code: "));
        Sys::userOutput((int)errorCode);
        Sys::userOutput(F("\n\r"));

    }
    else{
        //interpreter error code
        Sys::userOutput(F("error code: "));
        Sys::userOutput((int)errorCode);
        Sys::userOutput(F("\n\r"));
    }
}

bool ls(){

#ifdef __WIN32
    system("dir");
    return true;
#else
    system("ls -l");
    return true;
#endif

}

bool free(){

#ifdef __WIN32
    system("wmic OS get FreePhysicalMemory /Value");
    return true;
#else
    system("free");
    return true;
#endif
}

bool setPinMode(){


    //set pin mode according to the arguments 1(int-pin number) and 2(str-pin mode)

    int l_nArg1;
    char* l_sArg2;

    if(IOData::getArg(1, l_nArg1) && IOData::getArg(2, l_sArg2)){
        if(strncmp(l_sArg2, "INPUT", strlen(l_sArg2)) == 0){
            Sys::userOutput(F("sym-->pinMode("));
            Sys::userOutput(l_nArg1);
            Sys::userOutput(", INPUT)\n\r");
            //pinMode(l_nArg1, INPUT);
            ioPin[l_nArg1-1].isInput=true;
            return true;
        }
        else if(strncmp(l_sArg2, "OUTPUT", strlen(l_sArg2)) == 0){
            Sys::userOutput("sym-->pinMode(");
            Sys::userOutput(l_nArg1);
            Sys::userOutput(", OUTPUT)\n\r");
            //pinMode(l_nArg1, OUTPUT);
            ioPin[l_nArg1-1].isInput=false;
            return true;
        }
        else{
            Sys::userOutput(F("wrong argument (must be INPUT or OUTPUT)\n\r"));
            return false;
        }
    }
    else{
        Sys::userOutput(F("usage: setPM pin_number INPUT/OUTPUT \n\r"));
        return false;
    }
}

bool setPinStatus(){


    int l_nArg1;
    //set pin status according to the arguments 1(int-pin number) and 2(str-pin value)
    if(IOData::getArg(1, l_nArg1)){
        if(isInput(l_nArg1)){
            Sys::userOutput(F("dioW: can't set the value of an input!"));
            return false;
        }
    }
    else{
        Sys::userOutput(F("usage: dioW pin_number true/false \n\r"));
        return false;
    }

    bool l_bArg2;

    if(IOData::getArg(1, l_nArg1) && IOData::getArg(2, l_bArg2)){

        if(l_bArg2){
            Sys::userOutput(F("sym-->digitalWrite("));
            Sys::userOutput(l_nArg1);
            Sys::userOutput(", HIGH)\n\r");
            ioPin[l_nArg1-1].status=true;
            //digitalWrite(l_nArg1, HIGH);
        }
        else{
            Sys::userOutput(F("sym-->digitalWrite("));
            Sys::userOutput(l_nArg1);
            Sys::userOutput(", LOW)\n\r");
            ioPin[l_nArg1-1].status=false;
            //digitalWrite(l_nArg1, LOW);
        }
        return true;
    }
    else{
        Sys::userOutput(F("usage: dioW pin_number true/false \n\r"));
        return false;
    }
}

bool getPinStatus(){

    //return the current pinStatus of the arg1 pin
    int l_nArg1;

    if(IOData::getArg(1, l_nArg1)){
        if(ioPin[l_nArg1-1].status){
            Sys::userOutput(F("true\n\r"));
        }
        else{
            Sys::userOutput(F("false\n\r"));
        }
        IOData::setOutVal(1, ioPin[l_nArg1-1].status);
        return true;
    }
    else{
        IOData::setOutVal(1, "ERR");
        Sys::userOutput(F("usage: dioR pin_number \n\r"));
        return false;
    }
}

bool isInput(unsigned int pin){
    return ioPin[pin-1].isInput;
}


bool getPinMode(){

    //return the current pinMode of the arg1 pin

    int l_nArg1;

    if(IOData::getArg(1, l_nArg1)){
        if(isInput(l_nArg1)){
            IOData::setOutVal(1, "INPUT");
            Sys::userOutput(F("INPUT"));
        }
        else{
            IOData::setOutVal(1, "OUTPUT");
            Sys::userOutput(F("OUTPUT"));
        }
        return true;
    }
    else{
        IOData::setOutVal(1, "ERR");
        Sys::userOutput(F("usage: getPM pin_number\n\r"));
        return false;
    }
}


bool idle(){

   //do nothing for arg1 milliseconds
   int l_nArg1;

   if(IOData::getArg(1, l_nArg1)){
       usleep(l_nArg1 * 1000);
       return true;
   }
   else{
       Sys::userOutput("malformed delay() declaration\n\r");
       return false;
   }
}

bool cat(){

    //print the content of arg1 file
    char* l_sArg1;
    char cmd[MAX_FILE_LINE_LENGTH+1];
    cmd[0]='\0';

    if(IOData::getArg(1, l_sArg1)){
        //Sys::userOutput("printing the file: ");
        //Sys::userOutput(l_sArg1);
        //Sys::userOutput("\n\r");
        #ifdef __WIN32
            strcat(cmd, "type ");
            strcat(cmd, l_sArg1);
        #else
            strcat(cmd, "cat ");
            strcat(cmd, l_sArg1);
        #endif
        system(cmd);
        return true;
    }
    else{
        Sys::userOutput("usage: cat \"filename\"\n\r");
        return false;
    }


}

bool touch(){

    //create a file named arg1
    char* l_sArg1;
    char cmd[MAX_FILE_LINE_LENGTH+1];
    cmd[0]='\0';

    if(IOData::getArg(1, l_sArg1)){
    #ifdef __WIN32
        strcat(cmd, "copy /b NUL ");
    #else
        strcat(cmd, "touch ");
    #endif
        strcat(cmd, l_sArg1);
        system(cmd);
        return true;
    }
    else{
        Sys::userOutput("usage: touch \"filename\"\n\r");
        return false;
    }
}

bool copy(){

    char* l_sArg1;
    char* l_sArg2;

    if(IOData::getArg(1, l_sArg1) && IOData::getArg(2, l_sArg2)){
        std::ifstream  src(l_sArg1, std::ios::binary);
        std::ofstream  dst(l_sArg2,   std::ios::binary);
        dst << src.rdbuf();
        return true;
    }
    else{
        Sys::userOutput("usage: cp \"srcFile\" \"dstFile\"\n\r");
        return false;
    }
}


bool rm(){

    //remove a file named arg1
    char* l_sArg1;
    char cmd[MAX_FILE_LINE_LENGTH+1];
    cmd[0]='\0';

    if(IOData::getArg(1, l_sArg1)){
        #ifdef __WIN32
            strcat(cmd, "del ");
        #else
            strcat(cmd, "rm ");
        #endif
        strcat(cmd, l_sArg1);
        system(cmd);
        return true;
    }
    else{
        Sys::userOutput("usage: rm \"filename\"\n\r");
        return false;
    }
}

bool append(){

    //append at the end of arg1 the string arg2
    char* l_sArg1;
    char* l_sArg2;
    char cmd[2*MAX_FILE_LINE_LENGTH+1];
    cmd[0]='\0';

    if(IOData::getArg(1, l_sArg1) && IOData::getArg(2, l_sArg2)){

        char *pos;
        if ((pos=strchr(l_sArg2, '\n')) != NULL){
            *pos = '\0';
        }
        strcat(cmd, "echo ");
        strcat(cmd, l_sArg2);
        strcat(cmd, ">>");
        strcat(cmd, l_sArg1);
        system(cmd);
        return true;
    }
    else{
        Sys::userOutput("usage: append \"filename\" \"inputString\"\n\r");
        return false;
    }
}

bool fgetl(){

    char* l_sArg1;
    int l_nArg2;

    if(IOData::getArg(1, l_sArg1) && IOData::getArg(2, l_nArg2)){
        char line[MAX_FILE_LINE_LENGTH+1];
        line[0]='\0';
        if(!Sys::getFileLine(l_sArg1, l_nArg2, line)) return false;
        if(!IOData::setOutVal(1, line)) return false;
        return true;
    }
    return false;
}

bool flen(){

    char* l_sArg1;

    if(IOData::getArg(1, l_sArg1)){
        int number_of_lines = 0;
        std::string line;
        std::ifstream file(l_sArg1);
        while(std::getline(file, line)){
            ++number_of_lines;
        }
        if(!IOData::setOutVal(1, number_of_lines)){
            return false;
        }
        return true;
    }
    return false;
}

bool fex(){

    char* l_sArg1;
    //write true in the output buffer if a file exists
    if(IOData::getArg(1, l_sArg1)){
        //this function rely on the standard system interface function
        if(Sys::fileExists(l_sArg1)){
            IOData::setOutVal(1, true);
        }
        else{
            IOData::setOutVal(1, false);
        }
        return true;
    }
    return false;
}


#ifdef DEBUG
bool memDump(){


    Sys::userOutput(F("\n-------MEMDUMP------\n"));

    //booleans
    Sys::userOutput(F("booleans:\n"));
    for (uint8_t i = 0; i < MAX_BOOL_NUM; ++i){
        Sys::userOutput(prgData.booleans[i].name);
        Sys::userOutput(F(": "));
        if(prgData.booleans[i].value){
            Sys::userOutput("true");
        }
        else{
            Sys::userOutput("false");
        }
        Sys::userOutput(F("\n"));
    }
    //integers
    Sys::userOutput(F("integers:\n"));
    for (uint8_t i = 0; i < MAX_INT_NUM; ++i){
       Sys::userOutput(prgData.integers[i].name);
       Sys::userOutput(F(": "));
       Sys::userOutput(prgData.integers[i].value);
       Sys::userOutput(F("\n"));
    }
    //strings
    Sys::userOutput(F("strings:\n"));
    for (uint8_t i = 0; i < MAX_STRING_NUM; ++i){
        Sys::userOutput(prgData.strings[i].name);
        Sys::userOutput(F(": "));
        Sys::userOutput(prgData.strings[i].value);
        Sys::userOutput(F("\n"));
    }
    //labels
    Sys::userOutput(F("labels:\n"));
    for (uint8_t i = 0; i < MAX_LABEL_NUM; ++i){
        Sys::userOutput(prgData.labels[i].name);
        Sys::userOutput(F("\n"));
    }
    //inArgs
    Sys::userOutput(F("arguments:\n"));
    for (uint8_t i = 0; i < MAX_INPUT_ARGS_NUM; ++i){
        if(inArgs[i].isBoolean){
            Sys::userOutput(F("arg"));
            Sys::userOutput(i+1);
            if(inArgs[i].value.boolean){
                Sys::userOutput(F(": true"));
            }
            else{
                Sys::userOutput(F(": false"));
            }
            Sys::userOutput(F("\n"));
        }
        else if(inArgs[i].isInteger){
            Sys::userOutput(F("arg"));
            Sys::userOutput(i+1);
            Sys::userOutput(F(": "));
            Sys::userOutput(inArgs[i].value.integer);
            Sys::userOutput(F("\n"));
        }
        else if(inArgs[i].isString){
            Sys::userOutput(F("arg"));
            Sys::userOutput(i+1);
            Sys::userOutput(F(": "));
            Sys::userOutput(inArgs[i].value.string);
            Sys::userOutput(F("\n"));
        }
    }

    //outData
    for (uint8_t i = 0; i < MAX_OUT_VALS_NUM; ++i){

        if(outVals[i].isBoolean){
            Sys::userOutput(F("outval"));
            Sys::userOutput(i+1);
            if(outVals[i].value.boolean){
                Sys::userOutput(F(": true"));
            }
            else{
                Sys::userOutput(F(": false"));
            }
            Sys::userOutput(F("\n"));
        }
        else if(outVals[i].isInteger){
            Sys::userOutput(F("outval"));
            Sys::userOutput(i+1);
            Sys::userOutput(F(": "));
            Sys::userOutput(outVals[i].value.integer);
            Sys::userOutput(F("\n"));
        }
        else if(outVals[i].isString){
            Sys::userOutput(F("outval"));
            Sys::userOutput(i+1);
            Sys::userOutput(F(": "));
            Sys::userOutput(outVals[i].value.string);
            Sys::userOutput(F("\n"));
        }
    }

    Sys::userOutput(F("\n----------------------\n"));
    return true;

}
#endif

//add custom function's implementation here
/*bool foo(){

    //foo!
    int l_nArg1;
    char* l_sArg2;

    //get first argument
    if(IOData::getArg(1, l_nArg1)){
        //print first argument
        Sys::userOutput(l_nArg1);
        Sys::userOutput("\n\r");
    }
    else{
        Sys::userOutput(F("error retrieving first argument!\n"));
        return false;
    }

    //get second argument
    if(IOData::getArg(2, l_sArg2)){
        //print second argument
        Sys::userOutput(l_sArg2);
        Sys::userOutput("\n\r");
    }
    else{
        Sys::userOutput(F("error retrieving second argument!\n"));
        return false;
    }

    //write "ciao!" in the first output buffer
    IOData::setOutVal(1, "ciao!");

    //write 33 in the first output buffer
    //IOData::setOutVal(1, 33);

    //write false in the first output buffer
    //IOData::setOutVal(1, false);

    return true;
}
*/
#endif
