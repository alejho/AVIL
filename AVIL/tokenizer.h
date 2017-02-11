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
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "compopt.h"
#include <ctype.h> //isdigit, isalnum
#include <stdlib.h> //atoi
#include <string.h> //strlen
#include "keywords.h"

class Tokenizer
{
public:

    Tokenizer();
    //~Tokenizer();

    //void init(token const* keywords);
    eTokenId getToken();
    bool getTokenVariableName(char* dst);
    bool getTokenStringValue(char* dst);
    bool getTokenIntValue(int &dst);
    //bool getTokenBoolValue(bool &dst); //there is no need of this function...reading the current token when you need it's enough!
    void setStatements(const char* statements);
    const char* getStatements();
    const char* getCurrStatements();
    //token getKeyword(eTokenId keywordTokenId);
    void next();
    bool finished();

private:

    token const *m_keywords;
    eTokenId m_currentToken;
    char const* m_statements;
    char const* m_statementsPtrCurrent;
    char const* m_statementsPtrNext;

    eTokenId readToken();

};

#endif // TOKENIZER_H
