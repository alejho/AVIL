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
#ifndef TERMINAL_H
#define TERMINAL_H

#include "compopt.h"
#include "sys.h"
#include "tokenizer.h"
#ifdef AVIL_INTERPRETER
    #include "avil.h"
#endif

class terminal
{
public:

    terminal();
    void prompt();

    bool getPrgName(const char *program_name_and_args, char* program_name);
    bool getPrgArguments(const char *program_name_and_args);

private:

    void cmdErr();
};

#endif // TERMINAL_H
