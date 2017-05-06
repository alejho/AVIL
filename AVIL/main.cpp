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
#include "avil.h"

#if SYSTEM == PC

avil interpreter;
char cmd[30];

int main(){

    if(!interpreter.init()){
         //something wrong during initialization...want to do something?
    }

    //run the program blink
    //interpreter.run("blink.av");

    if(!interpreter.init()){
          Sys::userOutput(F("init error\n\r"));
          while(1);
    }
    cmd[0]='\0';

    while(1){
        Sys::userOutput(F("\n\r>"));
          //get a user input
          if(Sys::userInput(cmd, 30)){
              Sys::userOutput(F("\n\r"));
              interpreter.run(cmd);
              Sys::userOutput(F("program finished \n\r"));
          }
          else{
              Sys::userOutput(F("bad input!\n"));
          }
    }

    return 0;
}
#endif

