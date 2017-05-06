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
#if SYSTEM == PC

#ifndef PC_SYS_H
#define PC_SYS_H

#include "compopt.h"

#include "stdlib.h"
#include "iostream"
#include "fstream"
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <curses.h>

//fake pin for digital I/O simulation
typedef struct fakePinStruct{
    bool isInput;
    bool status;
}fakePin_t;

//meArm servo id's for simulation
const uint8_t  BASE_SERVO_ID = 1;
const uint8_t  RIGHT_SERVO_ID = 2;
const uint8_t  LEFT_SERVO_ID = 3;
const uint8_t  GRIPPER_SERVO_ID = 4;


bool ls();
bool free();
bool setPinMode();
bool setPinStatus();
bool getPinStatus();
bool isInput(unsigned int pin);
bool getPinMode();
bool idle();
bool cat();
bool touch();
bool copy();
bool rm();
bool append();
bool fgetl();
bool flen();
bool fex();
bool getKey();
bool waitDio();
bool manCmd();
bool writePos(char* posName);
bool teachPos();
bool jointStep(uint8_t servoId, int8_t step);
bool openGripper();
bool closeGripper();
bool movej();
#ifdef DEBUG
bool memDump();
#endif

//add custom function's prototypes here:
//bool foo();


#endif
#endif
