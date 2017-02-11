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


const uint8_t KILL_KEY = 27;

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
#ifdef DEBUG
bool memDump();
#endif

#endif
#endif
