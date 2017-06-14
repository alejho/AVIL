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

#if SYSTEM == ARDU_UNO


const sysPrograms_t systemProgams[] PROGMEM = {

    {"ls", &ls},
    {"free", &free},
    {"setPM", &setPinMode},
    {"getPM", &getPinMode},
    {"dioW", &setPinStatus},
    {"dioR", &getPinStatus},
    {"delay", &idle},
    {"cat", &cat},
    {"touch", &touch},
    {"cp", &copy},
    {"rm", &rm},
    {"append", &append},
    {"fgetl", &fgetl},
    {"flen", &flen},
    {"fex", &fex},
    {"frec", &frec},
    {"waitDio", &waitDio},
    #ifdef MEARM
    {"getKey", &getKey},
    {"manCmd", &manCmd},
    {"teachPos", &teachPos},
    {"open", &openGripper},
    {"close", &closeGripper},
    {"movej", &movej},
    #endif
    //add here your custom functions!








    #ifdef DEBUG
    {"memDump", &memDump},
    #endif

    {NULL, NULL}
};

#ifdef ETH_IO
    EthernetServer server(PORT);
#endif
#ifdef MEARM
    Servo leftServo;
    Servo rightServo;
    Servo baseServo;
    Servo gripperServo;
    unsigned int leftServoLastPos = 0;
    unsigned int rightServoLastPos = 0;
    unsigned int baseServoLastPos = 0;
    bool gripperIsOpen = false;
#endif

SdCard SD;
Fat16 File;
//this two values are the "file bookmark"
unsigned int nLastLine = 0;
uint32_t nLastFilePos = 0;

Sys::Sys()
{
}

bool Sys::init(){


#ifdef ETH_IO
    byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    IPAddress ip(IP_www, IP_xxx, IP_yyy, IP_zzz);
    Ethernet.begin(mac, ip);
    #ifdef DEBUG
        Serial.begin(9600);
    #endif
#else
    Serial.begin(9600);
#endif

#ifdef MEARM
    leftServo.attach(LEFT_SERVO_PIN, 500, 2400);
    rightServo.attach(RIGHT_SERVO_PIN, 500, 2400);
    baseServo.attach(BASE_SERVO_PIN, 500, 2400);
    gripperServo.attach(GRIPPER_SERVO_PIN, 500, 2400);

    leftServo.write(90);
    leftServoLastPos=90;

    rightServo.write(100);
    rightServoLastPos=100;

    baseServo.write(90);
    baseServoLastPos=90;

    gripperServo.write(GRIPPER_CLOSE_POS);
    gripperIsOpen=false;

#endif

    if (!SD.begin(CHIP_SELECT)){
        //sd card error
        return false;
    }
    if (!Fat16::init(&SD)){
        //sd card error
        return false;
    }
    return true;
}


bool Sys::fileExists(const char* fileName){

    //Fat16 File;
    if(File.isOpen()){
        File.close();
    }

    if (!File.open(fileName, O_READ)) {
        //Sys::userOutput(F("fnex"));
        return false;
    }
    File.close();
    return true;
}

bool Sys::getFileLine(char* fileName, unsigned int lineNumber, char* line){

    char l_lineStatements[MAX_FILE_LINE_LENGTH + 1];
    Fat16 File;

    if(!File.isOpen()){
        if (!File.open(fileName, O_READ)) {
            return false;
        }
    }

    unsigned int l_currentLine = 0;
    int len=0;

    //uhm...where were we last time?
    if(lineNumber > nLastLine){
        //if we need a line bigger than the last
        //we can just start from the last file's byte position and go ahead
        //this makes the interpreter up to 80% faster!!!

        //move to file bookmark!
        l_currentLine = nLastLine;
        File.seekSet(nLastFilePos);
    }

    while(l_currentLine<lineNumber){
        l_lineStatements[0]='\0';
        len=File.fgets(l_lineStatements, MAX_FILE_LINE_LENGTH+1);
        if(len <= 0){
            //end of file or error
            File.close();
            return false;
        }
        //if i didn't read the whole line skip until i get the delimiter "\n"
        if(l_lineStatements[MAX_FILE_LINE_LENGTH-1] != '\n' && len==MAX_FILE_LINE_LENGTH){
            l_lineStatements[MAX_FILE_LINE_LENGTH-1]='\n';
            while(File.read() != '\n');
        }
        l_currentLine++;
    }
    if(l_currentLine == lineNumber){
        line[0]='\0';
        //so be careful...sizeof(line) must be greater than MAX_FILE_LINE_LENGTH
        strncat(line, l_lineStatements, MAX_FILE_LINE_LENGTH);
        File.close();
        setFileBookmark(lineNumber, File.curPosition());
        return true;
    }
    else{
        File.close();
        return false;
    }
    File.close();
    return false;

}

bool Sys::userInput(char* dst, size_t size){

    #ifdef ETH_IO

        char rc='\0';
        uint8_t i = 0;
        char l_input[MAX_FILE_LINE_LENGTH + 1];

        while(true){

            EthernetClient client = server.available();
              if (client) {
                if (client.available() > 0) {
                  // read the bytes incoming from the client:
                  rc = client.read();
                  if(rc>0){
                      if(i>=size){
                          Sys::userOutput(F("command exceeding buffer size\n\r"));
                          return false;
                      }
                      if (rc != '\r' && rc != '\n'){
                          l_input[i] = rc;
                          i++;
                      }
                      else if ((rc == '\r' || rc == '\n') && i>0){
                          l_input[i] = '\0'; // terminate the string
                          strcpy(dst, l_input);
                          return true;
                      }
                  }
                }
              }
        }

    #else
        char rc='\0';
        uint8_t i = 0;
        char l_input[MAX_FILE_LINE_LENGTH + 1];

        while(true){

            if(Serial.available() > 0){
                rc = Serial.read();
                if(rc>0){
                    if (rc != '\r' && rc != '\n'){
                        //echo
                        Sys::userOutput(rc);
                        l_input[i] = rc;
                        i++;
                    }
                    else if(rc=='\r' && i>0){
                        l_input[i] = '\0'; // terminate the string
                        strcpy(dst, l_input);
                        return true;
                    }
                    if(i>=size){
                        Sys::userOutput(F("command exceeding buffer size\n\r"));
                        return false;
                    }
                }
            }
            else{
                rc=0;
            }

        }
    #endif
}

void Sys::userOutput(const char* msg){

    #ifdef ETH_IO
        server.print(msg);
    #else
        Serial.print(msg);
    #endif
}

void Sys::userOutput(const __FlashStringHelper* msg){

    #ifdef ETH_IO
        server.print(msg);
    #else
        Serial.print(msg);
    #endif

}

void Sys::userOutput(char c){
    #ifdef ETH_IO
        server.print(c);
    #else
        Serial.print(c);
    #endif
}


void Sys::userOutput(int num){
    #ifdef ETH_IO
        server.print(num);
    #else
        Serial.print(num);
    #endif
}

bool Sys::killSignalReceived(){

    char rc = 0;

    #ifdef ETH_IO
        EthernetClient client = server.available();
        if (client){
            if (client.available() > 0) {
            // read the bytes incoming from the client:
            rc = client.read();
            if(rc == KILL_KEY){
                return true;
            }
            else{
                return false;
            }
         }
        }
        else{
            return false;
        }
    #else
        rc = Serial.read();
        if(rc == KILL_KEY){
            return true;
        }
        else{
            return false;
        }
    #endif

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
            /*if(!l_bRes){
                Sys::userOutput(F("call error: "));
                Sys::userOutput(sysProgramName);
                Sys::userOutput(F("\n\r"));
            }*/
            return l_bRes;
        }
    }
    /*
    Sys::userOutput(F("the syscall "));
    Sys::userOutput(sysProgramName);
    Sys::userOutput(F("is not implemented on this hardware \n"));
    */
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

    resetFileBookmark();

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
    }
    //interpreter error code
    Sys::userOutput(F("error code: "));
    Sys::userOutput((int)errorCode);
    Sys::userOutput(F("\n\r"));

}

bool ls(){

    #ifdef ETH_IO
        //Fat16 File;
        dir_t dir;
        for (uint16_t index = 0; File.readDir(&dir, &index, DIR_ATT_VOLUME_ID); index++) {
            for (uint8_t j = 0; j < 11; j++) {
                Sys::userOutput(char(dir.name[j]));
            }
            Sys::userOutput(F("\t"));
            Sys::userOutput(int(dir.fileSize));
            Sys::userOutput(F(" bytes\n\r"));
        }
    #else
        //File.close();
        Fat16::ls(LS_SIZE);
    #endif

    return true;
}

bool free(){

    extern int __heap_start, *__brkval;
    int v;
    Sys::userOutput(F("free memory: "));
    Sys::userOutput((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
    Sys::userOutput(F(" bytes\n\r"));
    return true;
}

bool setPinMode(){


    //set pin mode according to the arguments 1(int-pin number) and 2(str-pin mode)

    int l_nArg1;
    char* l_sArg2;

    if(IOData::getArg(1, l_nArg1) && IOData::getArg(2, l_sArg2)){


        if(l_nArg1 == CHIP_SELECT){
            Sys::userOutput(F("setPM: this pin is reserved for sd card!\n\r"));
            return false;
        }
        #ifdef ETH_IO
        if(l_nArg1 == CHIP_SELECT_ETH){
            Sys::userOutput(F("setPM: this pin is reserved for ethernet module!\n\r"));
            return false;
        }
        #endif

        if(strncmp(l_sArg2, "INPUT", strlen(l_sArg2)) == 0){
            pinMode(l_nArg1, INPUT);
            return true;
        }
        else if(strncmp(l_sArg2, "OUTPUT", strlen(l_sArg2)) == 0){
            pinMode(l_nArg1, OUTPUT);
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
        if(l_nArg1 == CHIP_SELECT){
            Sys::userOutput(F("dioW: this pin is reserved for sd card!\n\r"));
            return false;
        }
        #ifdef ETH_IO
        if(l_nArg1 == CHIP_SELECT_ETH){
            Sys::userOutput(F("dioW: this pin is reserved for ethernet module!\n\r"));
            return false;
        }
        #endif
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
            digitalWrite(l_nArg1, HIGH);
        }
        else{
            digitalWrite(l_nArg1, LOW);
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

        if(digitalRead(l_nArg1) == HIGH){
            IOData::setOutVal(1, true);
            Sys::userOutput(F("true\n\r"));
        }
        else{
            IOData::setOutVal(1, false);
            Sys::userOutput(F("false\n\r"));
        }
        return true;
    }
    else{
        //IOData::setOutVal(1, "err");
        Sys::userOutput(F("usage: dioR pin_number \n\r"));
        return false;
    }
}

bool isInput(int pin){

    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    volatile uint8_t *reg = portModeRegister(port);

    if (*reg & bit) {
        // It's an output
        return false;
    }
    else {
        // It's an input
        return true;
    }
}


bool getPinMode(){

    //return the current pinMode of the arg1 pin

    int l_nArg1;

    if(IOData::getArg(1, l_nArg1)){
        if(isInput(l_nArg1)){
            IOData::setOutVal(1, "INPUT");
            Sys::userOutput(F("INPUT\n\r"));
        }
        else{
            IOData::setOutVal(1, "OUTPUT");
            Sys::userOutput(F("OUTPUT\n\r"));
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
        delay(l_nArg1);
        return true;
    }
    else{
        Sys::userOutput(F("usage: delay millisOfDelay\n\r"));
        return false;
    }
}

bool cat(){

    //print the content of arg1 file to system output
    char* l_sArg1;
    //Fat16 File;

    if(File.isOpen()){
        File.close();
    }

    if(IOData::getArg(1, l_sArg1)){
        if(Sys::fileExists(l_sArg1)){
            File.open(l_sArg1, O_READ);
            int16_t c;
            do {
                c = File.read();
                #ifdef ETH_IO
                    Sys::userOutput((char)c);
                #else
                    if(c=='\n'){
                        Sys::userOutput(F("\n\r"));
                    }
                    else{
                        Sys::userOutput((char)c);
                    }
                #endif

            } while (c>0);
            File.close();
            return true;
        }
        else{
            Sys::userOutput(F("cat: this file doesn't exist: "));
            Sys::userOutput(l_sArg1);
            Sys::userOutput(F("\n"));
            return false;
        }
    }
    else{
        Sys::userOutput(F("usage: cat \"filename\"\n\r"));
    }
    File.close();
}

bool touch(){

    //create a file named arg1
    char* l_sArg1;

    //Fat16 File;
    if(File.isOpen()){
        File.close();
    }

    if(IOData::getArg(1, l_sArg1)){
        if (!File.open(l_sArg1, O_CREAT | O_EXCL | O_WRITE)){
            Sys::userOutput(F("touch: the file already exists!\n"));
            return false;
        }
        return true;
    }
    else{
        Sys::userOutput(F("usage: touch \"filename\"\n\r"));
        return false;
    }
}

bool copy(){

    //src
    char* l_sArg1;
    //dst
    char* l_sArg2;

    if(File.isOpen()){
        File.close();
    }
    Fat16 dstFile;

    if(IOData::getArg(1, l_sArg1) && IOData::getArg(2, l_sArg2)){
        if (!File.open(l_sArg1, O_READ)){
            Sys::userOutput(F("cp: this file doesn't exist: "));
            Sys::userOutput(l_sArg1);
            Sys::userOutput(F("\n\r"));
            return false;
        }
        if (!dstFile.open(l_sArg2, O_CREAT | O_TRUNC | O_WRITE)){
            return false;
        }
        char buf[10];
        int16_t n;

        while ((n = File.read(buf, sizeof(buf))) > 0) {
            if (dstFile.write(buf, n) != n){
                Sys::userOutput(F("cp: error\n\r"));
                return false;
            }
        }
        File.close();
        dstFile.close();
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

    //Fat16 File;
    if(File.isOpen()){
        File.close();
    }

    if(IOData::getArg(1, l_sArg1)){
        if (File.open(l_sArg1, O_WRITE)){
            if(!File.remove()){
                Sys::userOutput(F("error removing the file!"));
                return false;
            }
            return true;
        }
        else{
            //Sys::userOutput(F("rm: this file doesn't exist: "));
            //Sys::userOutput(l_sArg1);
            //Sys::userOutput(F("\n"));
            return true;
        }
    }
    else{
            Sys::userOutput(F("usage: rm \"filename\"\n\r"));
            return false;
    }
}

bool append(){

    //append at the end of arg1 the string arg2
    char* l_sArg1;
    char* l_sArg2;

    //Fat16 File;
    if(File.isOpen()){
        File.close();
    }

    if(IOData::getArg(1, l_sArg1) && IOData::getArg(2, l_sArg2)){
        if (File.open(l_sArg1, O_APPEND | O_WRITE)){
            File.print(l_sArg2);
            if(l_sArg2[strlen(l_sArg2)-1]!='\n'){
                File.print("\n");
            }
            File.close();
            return true;
        }
        else{
            Sys::userOutput(F("append: this file doesn't exist: "));
            Sys::userOutput(l_sArg1);
            Sys::userOutput(F("\n"));
            File.close();
            return false;
        }
    }
    else{
            Sys::userOutput(F("usage: append filename inputString\n\r"));
            File.close();
            return false;
    }
    File.close();
}

bool fgetl(){

    char* l_sArg1;
    int l_nArg2;

    resetFileBookmark();

    if(File.isOpen()){
        File.close();
    }

    if(IOData::getArg(1, l_sArg1) && IOData::getArg(2, l_nArg2)){
        char line[MAX_FILE_LINE_LENGTH+1];
        line[0]='\0';
        if(!Sys::getFileLine(l_sArg1, l_nArg2, line)){
            //Sys::userOutput(F("line: "));
            //Sys::userOutput(l_nArg2);
            //Sys::userOutput(F(" \n"));
            return false;
        }
        if(!IOData::setOutVal(1, line)){
            return false;
        }
        return true;
    }
    return false;
}

bool flen(){

    char* l_sArg1;

    resetFileBookmark();

    if(File.isOpen()){
        File.close();
    }

    if(IOData::getArg(1, l_sArg1)){
        char line[MAX_FILE_LINE_LENGTH+1];
        line[0]='\0';
        int len = 1;
        while(Sys::getFileLine(l_sArg1, len, line)){
           len++;
        }
        if(!IOData::setOutVal(1, len-1)) return false;
        return true;
    }
    return false;
}

bool fex(){

    //write true in the output buffer if a file exists

    char* l_sArg1;

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
    else{
        Sys::userOutput(F("usage: fex filename\n\r"));
        return false;
    }
}

bool frec(){

    //receive a file char by char from the input and redirect it to l_sArg1
    //Fat16 File;
    if(File.isOpen()){
        File.close();
    }

    char* l_sArg1;
    if(IOData::getArg(1, l_sArg1)){
        char rc=0;
        if(!File.open(l_sArg1, O_WRITE | O_CREAT)){
            Sys::userOutput(F("error opening "));
            Sys::userOutput(l_sArg1);
            Sys::userOutput(F("\n\r"));
            File.close();
            return false;
        }
        while(rc!=KILL_KEY){

            #ifdef ETH_IO
                //read input from ethernet
                EthernetClient client = server.available();
                if (client) {
                    if (client.available() > 0) {
                        // read the byte incoming from the client:
                        rc = client.read();
                    }
                }
            #else
                //read input from serial
                if(Serial.available() > 0){
                    rc = Serial.read();
                    if(rc>0 && rc!=255 && rc!=KILL_KEY){
                        //echo to bash
                        if(rc=='\r'){
                            Serial.print(F("\n\r"));
                        }
                        else{
                            Serial.print(rc);
                        }
                    }
                }
                else{
                    rc=0;
                }
            #endif
                //write it to the file
                if(rc>0 && rc!=255 && rc!=KILL_KEY){
                    #ifdef ETH_IO
                        File.write(rc);
                    #else
                        if(rc=='\r'){
                            File.write('\n');
                            File.write('\r');
                        }
                        else{
                            File.write(rc);
                        }
                    #endif
                    rc=0;
                }
        }
        File.close();
        return true;
    }
    else{
        Sys::userOutput(F("usage: frec filename\n\r"));
        File.close();
        return false;
    }

}

bool waitDio(){

    int l_nArg1;

    if(IOData::getArg(1, l_nArg1)){

        bool l_bRes = false;
        do {
            getPinStatus();
            IOData::getOutVal(1, l_bRes);
        }while (!l_bRes);
        return true;
    }
    return false;
}

void setFileBookmark(unsigned int lineNumber, uint32_t filePos){

    nLastLine = lineNumber;
    nLastFilePos = filePos;
}

void resetFileBookmark(){

    nLastLine = 0;
    nLastFilePos = 0;
}

#ifdef MEARM

int jointControl(Servo joint, int currentPos, int setPoint){

    //move the given joint one step in the direction of the set point
    //return the new position of the servo

    if(currentPos == setPoint){
        //don't move...you're arrived!
        return currentPos;
    }
    else if(currentPos<setPoint){
        joint.write(currentPos+1);
        return ++currentPos;
    }
    else if(currentPos>setPoint){
        joint.write(currentPos-1);
        return --currentPos;
    }
}

bool jointStep(uint8_t servoId, int8_t step){


    switch (servoId) {
    case BASE_SERVO_ID:
        baseServoLastPos = jointControl(baseServo, baseServoLastPos, baseServoLastPos+step);
        Sys::userOutput(F("Base servo step "));
        break;
    case RIGHT_SERVO_ID:
        rightServoLastPos = jointControl(rightServo, rightServoLastPos, rightServoLastPos+step);
        Sys::userOutput(F("Right servo step "));
        break;
    case LEFT_SERVO_ID:
        leftServoLastPos = jointControl(leftServo, leftServoLastPos, leftServoLastPos+step);
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

    char key = 0;

    do{
       key = Serial.read();
    }while(key<=0);

    char l_sKey[2];
    l_sKey[0]=key;
    l_sKey[1]='\0';

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
        Sys::userOutput(F("\n\rdo you wan to overwrite this position?(y/n): "));
        getKey();
        if(!IOData::getOutVal(1, l_sKey)) return false;
        if(l_sKey[0]=='y'){
            Sys::userOutput(F("\n\r sorry...I still have to do this...\n\r"));
        }
        Sys::userOutput(F("\n\r saved!\n\r"));
        return true;
        }
    }

bool openGripper(){
    //open gripper
    gripperServo.write(GRIPPER_OPEN_POS);
    gripperIsOpen = true;
    return true;
}

bool closeGripper(){
    //close gripper
    gripperServo.write(GRIPPER_CLOSE_POS);
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
        if(!l_bSkipStep) baseServoLastPos = jointControl(baseServo, baseServoLastPos, l_nBaseServoPosSP);
        rightServoLastPos = jointControl(rightServo, rightServoLastPos, l_nRightServoPosSP);
        leftServoLastPos = jointControl(leftServo, leftServoLastPos, l_nLeftServoPosSP);
        delay((100 - l_nArg2) + 20);
    }
    //movement finished!
    return true;
}

#endif

#ifdef DEBUG
bool memDump(){


    Sys::userOutput(F("\n\r-------MEMDUMP------\n\r"));

    //booleans
    Sys::userOutput(F("booleans:\n\r"));
    for (uint8_t i = 0; i < MAX_BOOL_NUM; ++i){
        Sys::userOutput(prgData.booleans[i].name);
        Sys::userOutput(F(": "));
        if(prgData.booleans[i].value){
            Sys::userOutput("true");
        }
        else{
            Sys::userOutput("false");
        }
        Sys::userOutput(F("\n\r"));
    }
    //integers
    Sys::userOutput(F("integers:\n\r"));
    for (uint8_t i = 0; i < MAX_INT_NUM; ++i){
       Sys::userOutput(prgData.integers[i].name);
       Sys::userOutput(F(": "));
       Sys::userOutput(prgData.integers[i].value);
       Sys::userOutput(F("\n\r"));
    }
    //strings
    Sys::userOutput(F("strings:\n\r"));
    for (uint8_t i = 0; i < MAX_STRING_NUM; ++i){
        Sys::userOutput(prgData.strings[i].name);
        Sys::userOutput(F(": "));
        Sys::userOutput(prgData.strings[i].value);
        Sys::userOutput(F("\n\r"));
    }
    //labels
    Sys::userOutput(F("labels:\n\r"));
    for (uint8_t i = 0; i < MAX_LABEL_NUM; ++i){
        Sys::userOutput(prgData.labels[i].name);
        Sys::userOutput(F("\n\r"));
    }
    //inArgs
    Sys::userOutput(F("arguments:\n\r"));
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
            Sys::userOutput(F("\n\r"));
        }
        else if(inArgs[i].isInteger){
            Sys::userOutput(F("arg"));
            Sys::userOutput(i+1);
            Sys::userOutput(F(": "));
            Sys::userOutput(inArgs[i].value.integer);
            Sys::userOutput(F("\n\r"));
        }
        else if(inArgs[i].isString){
            Sys::userOutput(F("arg"));
            Sys::userOutput(i+1);
            Sys::userOutput(F(": "));
            Sys::userOutput(inArgs[i].value.string);
            Sys::userOutput(F("\n\r"));
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
#endif
