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
    {"waitDio", &waitDio},
    //meArm controller test
    {"getKey", &getKey},
    {"manCmd", &manCmd},
    {"teachPos", &teachPos},
    {"open", &openGripper},
    {"close", &closeGripper},
    {"movej", &movej},
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

int leftServoLastPos = 0;
int rightServoLastPos = 0;
int baseServoLastPos = 0;
bool gripperIsOpen = false;


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
        strncat(line, l_lineStatements.c_str(), MAX_FILE_LINE_LENGTH-1);
        if (l_lineStatements.length()<=MAX_FILE_LINE_LENGTH){
            //strncat(line, l_lineStatements.c_str(), MAX_FILE_LINE_LENGTH-1);
            //line[l_lineStatements.length()+1]='\n';
            //line[l_lineStatements.length()+2]='\0';
            l_programFile.close();
            return true;
        }
        else{
            //strncat(line, l_lineStatements.c_str(), MAX_FILE_LINE_LENGTH-1);
            line[MAX_FILE_LINE_LENGTH-1]='\n';
            line[MAX_FILE_LINE_LENGTH]='\0';
            l_programFile.close();
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

    //on pc this function has only debug purpose!

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

    //on pc this function has only debug purpose!

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

    //on pc this function has only debug purpose!

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

    //on pc this function has only debug purpose!

    return ioPin[pin-1].isInput;
}


bool getPinMode(){

    //on pc this function has only debug purpose!

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

bool waitDio(){

    Sys::userOutput("\n\rwaitDio make no sense on this hardware!\n\r");
    return true;
}


int jointControl(int currentPos, int setPoint){

    //on pc this function has only debug purpose!

    //move the given joint one step in the direction of the set point
    //return the new position of the servo

    if(currentPos == setPoint){
        //don't move...you're arrived!
        return currentPos;
    }
    else if(currentPos<setPoint){
        return ++currentPos;
    }
    else if(currentPos>setPoint){
        return --currentPos;
    }

    return 0;
}

bool jointStep(uint8_t servoId, int8_t step){

    //on pc this function has only debug purpose!

    switch (servoId) {
    case BASE_SERVO_ID:
        baseServoLastPos = jointControl(baseServoLastPos, baseServoLastPos+step);
        Sys::userOutput(F("Base servo step "));
        break;
    case RIGHT_SERVO_ID:
        rightServoLastPos = jointControl(rightServoLastPos, baseServoLastPos+step);
        Sys::userOutput(F("Right servo step "));
        break;
    case LEFT_SERVO_ID:
        leftServoLastPos = jointControl(leftServoLastPos, baseServoLastPos+step);
        Sys::userOutput(F("Left servo step "));
        break;
    default:
        return false;
        break;
    }
    Sys::userOutput(step);
    Sys::userOutput(F("\n\r"));

    return true;
}

bool getKey(){

    char l_sKey[3];

    Sys::userInput(l_sKey, 3);

    if(!IOData::setOutVal(1, l_sKey)){
            return false;
    }
    return true;
}



bool manCmd(){

    char* l_sArg1;

    if(IOData::getArg(1, l_sArg1)){

        switch(l_sArg1[0])
        {
        case ('q'):
            //left servo +1
            jointStep(LEFT_SERVO_ID, 1);
            break;
        case ('a'):
            //left servo -1
            jointStep(LEFT_SERVO_ID, -1);
            break;
        case ('r'):
            //right servo +1
            jointStep(RIGHT_SERVO_ID, 1);
            break;
        case ('d'):
            //right servo -1
            jointStep(RIGHT_SERVO_ID, -1);
            break;
        case ('x'):
            //base servo +1
            jointStep(BASE_SERVO_ID, 1);
            break;
        case ('c'):
            //base servo -1
            jointStep(BASE_SERVO_ID, -1);
            break;
        case ('g'):
            //toggle gripper
            gripperIsOpen? closeGripper():openGripper();
            break;
        default:
            //opzionale
            break;
        };
        return true;
    }
    else{
        Sys::userOutput(F("usage: movej posname\n\r"));
        return false;
    }
}

bool writePos(char* posName){

    char l_sPos[MAX_NUMLEN+1];

    if(!IOData::setArg(1, "posDB")) return false;

    //pos name
    if(!IOData::setArg(2, posName)) return false;
    append();
    //base
    l_sPos[0] = '\0';
    sprintf(l_sPos, "%u", baseServoLastPos);
    if(!IOData::setArg(2, l_sPos)) return false;
    append();
    //right
    l_sPos[0] = '\0';
    sprintf(l_sPos, "%u", rightServoLastPos);
    if(!IOData::setArg(2, l_sPos)) return false;
    append();
    //left
    l_sPos[0] = '\0';
    sprintf(l_sPos, "%u", leftServoLastPos);
    if(!IOData::setArg(2, l_sPos)) return false;
    append();

    return true;
}

bool teachPos(){

    char* l_sArg1;
    char l_sPosName[MAX_STRINGLEN+1];

    if(!IOData::getArg(1, l_sArg1)) return false;
    l_sPosName[0]='\0';
    strncat(l_sPosName, l_sArg1, MAX_STRINGLEN);

    //check if the pos file already exists...if not create it
    if(!IOData::setArg(1, "posDB")) return false;
    if (!Sys::fileExists("posDB")){
        if(!touch()) return false;
    }

    //check if the "pos" already exists!

    char* l_newLine;
    char* l_sCurrLine;
    char* l_sKey;
    bool l_isNewPosition = true;
    bool l_bReadOk = false;
    int lineNumber = 1;

    do {
        if(!IOData::setArg(2, lineNumber)) return false;
        l_bReadOk = fgetl() && IOData::getOutVal(1, l_sCurrLine);
        if(l_bReadOk){
            //remove trailing \n if exists
            if ((l_newLine=strchr(l_sCurrLine, '\n')) != NULL) *l_newLine = '\0';
            if(strncmp(l_sCurrLine, l_sPosName, strlen(l_sPosName))==0) l_isNewPosition = false;
        }
        lineNumber++;
    }while(l_bReadOk && l_isNewPosition);
    //File.close()

    if(l_isNewPosition){
        //new position...just write
        if(!writePos(l_sPosName)) return false;
        Sys::userOutput(F("\n\r saved!\n\r"));
        return true;
    }
    else{
        //already existing
        Sys::userOutput(F("do you wan to overwrite this position?(y/n): "));
        getKey();
        if(!IOData::getOutVal(1, l_sKey)) return false;
        if(l_sKey[0]=='y'){
            Sys::userOutput(F("\n\r sorry...still have to do this...\n\r"));
        }
        Sys::userOutput(F("\n\r saved!\n\r"));
        return true;
        }
    }


bool openGripper(){
    //open gripper

    gripperIsOpen = true;
    return true;
}

bool closeGripper(){
    //close gripper

    gripperIsOpen = false;
    return true;
}

bool movej(){

    char* l_sArg1;
    int l_nArg2;
    char l_sPosName[MAX_STRINGLEN+1];
    char* l_sCurrLine;

    if(!IOData::getArg(1, l_sArg1) || !IOData::getArg(2, l_nArg2)){
        Sys::userOutput(F("usage: movej pos_name speed_perc\n\r"));
        return false;
    }

    //trim speed value
    if(l_nArg2>100) l_nArg2=100;
    if(l_nArg2<1) l_nArg2=1;
    //copy locally the position's name
    strncpy(l_sPosName, l_sArg1, MAX_STRINGLEN);
    int lineNumber = 1;

    if(!IOData::setArg(1, "posDB")) return false;

    char* l_newLine;

    do{
        IOData::setArg(2, lineNumber);
        if(!fgetl() && IOData::getOutVal(1, l_sCurrLine)) return false;
        //remove trailing \n if exists
        if ((l_newLine=strchr(l_sCurrLine, '\n')) != NULL) *l_newLine = '\0';
        lineNumber++;

     } while(strcmp(l_sCurrLine, l_sPosName)!=0);

    //the next 3 rows contains the servo values of the wanted pos!

    //base servo set-point
    //set arguments for fgetl
    if(!IOData::setArg(1, "posDB") || !IOData::setArg(2, lineNumber)) return false;
    //execute and get fgetl's result
    if (!fgetl() || !IOData::getOutVal(1, l_sCurrLine)) return false;
    int l_nBaseServoPosSP = atoi(l_sCurrLine);

    //right servo set-point
    //set arguments for fgetl
    if (!IOData::setArg(1, "posDB") || !IOData::setArg(2, lineNumber+1)) return false;
    //execute and get fgetl's result
    if (!fgetl() || !IOData::getOutVal(1, l_sCurrLine)) return false;
    int l_nRightServoPosSP = atoi(l_sCurrLine);

    //left servo set-point
    //set arguments for fgetl
    if(!IOData::setArg(1, "posDB") || !IOData::setArg(2, lineNumber+2)) return false;
    //execute and get fgetl's result
    if (!fgetl() || !IOData::getOutVal(1, l_sCurrLine)) return false;
    int l_nLeftServoPosSP = atoi(l_sCurrLine);

    //now execute the movement one step at time
    bool l_bSkipStep=false;
    while(baseServoLastPos != l_nBaseServoPosSP || rightServoLastPos != l_nRightServoPosSP || leftServoLastPos != l_nLeftServoPosSP){
        //the base rotate half the speed of the other joints
        l_bSkipStep = !l_bSkipStep;
        if(!l_bSkipStep) baseServoLastPos = jointControl(baseServoLastPos, l_nBaseServoPosSP);
        rightServoLastPos = jointControl(rightServoLastPos, l_nRightServoPosSP);
        leftServoLastPos = jointControl(leftServoLastPos, l_nLeftServoPosSP);
        //delay((100 - l_nArg2) + 20);
    }
    //movement finished!
    return true;
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
