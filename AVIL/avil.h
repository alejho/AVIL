/* 
 * Copyright 2017 Alessio Villa
 * 
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
#ifndef AVIL_H
#define AVIL_H

#include "compopt.h"

#include "avilTok.h"
#include "avilData.h"
#include "avilExpr.h"
#include "sys.h"

#pragma GCC diagnostic ignored "-Wswitch"

typedef struct programLineStruct{

    char statements[MAX_FILE_LINE_LENGTH+1];
    unsigned int number;
}programLine_t;


typedef struct nestedIfStackStruct{

    unsigned int lineNumber;
    bool condition;
}ifStack_t ;

typedef struct nestedForStackStruct{

    unsigned int forLineNumber;
    unsigned int endForLineNumber;
    int toValue;
    int step;

}for_stack_t;


class avil
{
public:

    avil();

    bool init();
    bool run(char *program_name_and_args);
    bool step();

    bool finished();

private:

    static const unsigned int FIRST_PROGRAM_LINE = 1;

    Tokenizer m_tokenizer;

    char m_programName[MAX_PRG_NAME_LENGTH+1];
    //char* m_programName;

    programLine_t m_currentProgramLine;
    ifStack_t m_nestedIfStack[MAX_NESTED_IF];
    uint8_t m_nestedIfCounter;
    for_stack_t m_nestedForStack[MAX_NESTED_IF];
    uint8_t m_nestedForCounter;

    unsigned int m_subProgramLine;

    bool usrPrgInit(char *program_name_and_args);

    void clearAll();
    bool variablesDeclaration();
    bool booleanVariableDeclaration();
    bool integerVariableDeclaration();
    bool stringVariableDeclaration();
    bool labelVariableDeclaration();

    bool gotoStatement();
    bool printStatement();
    bool printlnStatement();
    bool delayStatement();
    bool getStrStatement();
    bool getIntStatement(bool &isNumber);
    bool assignmentStatement();
    bool ifStatement();
    bool skipToElse();
    bool skipToEndif();
    bool elseStatement();
    bool endIfStatement();
    bool forStatement();
    bool skipToEndFor();
    bool endForStatement();
    bool callStatement();
    bool subStatement();

    bool setCurrentProgramLine(unsigned int lineNumber);
    bool skipToToken(eTokenId token);
    bool skipToToken(eTokenId token, bool& bUnexpected);
    bool expectedTokenSequence(eTokenId tokenSequence[], size_t size);
    bool skipToSub(char* subName);

    bool getPrgName(const char *program_name_and_args, char* program_name);
    bool getPrgArguments(const char *program_name_and_args);

};

#endif // AVIL_H
