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
#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H
#include "compopt.h"
#include "avilTok.h"
#include "avilData.h"

class Expressions
{
public:
    Expressions();

    bool evaluate(const char* expression, bool &result);
    bool evaluate(const char* expression, int &result);
    bool evaluate(const char* expression, char* result);

private:

    Tokenizer m_tokenizer;

    bool IntComparison(bool &result);
    bool boolOperation(bool &result);
    bool IntOperation(int &result);

    bool StringComparison(bool &result);
    bool StringOperation(char* result);

    bool getOperand(bool &op);
    bool getOperand(int &op);
    bool getOperand(char* op);
};

#endif // EXPRESSIONS_H
