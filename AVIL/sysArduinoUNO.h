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

#ifndef ARDUINO_SYS_H
#define ARDUINO_SYS_H

#include "compopt.h"
#if SYSTEM == ARDU_UNO

#include <Fat16.h>
const uint8_t CHIP_SELECT = 4;


#ifdef ETH_IO
    #include <Ethernet.h>
    const uint8_t CHIP_SELECT_ETH = 10;
#endif

#ifdef MEARM
    //pins 4,10,11,12,13 are for ethernet shield
    #include <Servo.h>
    const uint8_t LEFT_SERVO_PIN = 9;
    const uint8_t RIGHT_SERVO_PIN = 8;
    const uint8_t BASE_SERVO_PIN = 7;
    const uint8_t GRIPPER_SERVO_PIN = 6;

    const uint8_t BASE_SERVO_ID = 1;
    const uint8_t RIGHT_SERVO_ID = 2;
    const uint8_t LEFT_SERVO_ID = 3;
    const uint8_t GRIPPER_SERVO_ID = 4;

    const uint8_t GRIPPER_CLOSE_POS = 170;
    const uint8_t GRIPPER_OPEN_POS = 90;

#endif


const uint8_t KILL_KEY = 27;
const uint8_t BACKSPACE_KEY = 127;

bool ls();
bool free();
bool setPinMode();
bool setPinStatus();
bool getPinStatus();
bool isInput(int pin);
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
bool frec();
bool waitDio();

void setFileBookmark(unsigned int lineNumber, uint32_t filePos);
void resetFileBookmark();

#ifdef MEARM
bool getKey();
bool jointStep(uint8_t servoId, int8_t step);
bool manCmd();
bool writePos();
bool teachPos();
bool openGripper();
bool closeGripper();
bool movej();
#endif

#ifdef DEBUG
bool memDump();
#endif

#endif
#endif
