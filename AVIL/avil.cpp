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

avil::avil()
{
}

bool avil::init(){
    return Sys::init();
}

bool avil::usrPrgInit(char* program_name_and_args){

    //reset some stuff
    this->clearAll();

    if(!this->getPrgName(program_name_and_args, m_programName)){
        Sys::runtimeError(75, m_currentProgramLine.number);
        return false;
    }
    //collect the program's arguments
    if(!this->getPrgArguments(program_name_and_args)){
        Sys::runtimeError(76, m_currentProgramLine.number);
        return false;
    }
    if(!Sys::fileExists(m_programName)){
        Sys::runtimeError(86, m_currentProgramLine.number);
        return false;
    }

    if (!this->setCurrentProgramLine(FIRST_PROGRAM_LINE)){
        Sys::runtimeError(3, m_currentProgramLine.number);
        return false;
    }
    //variables declaration
    if (!this->variablesDeclaration()){
        Sys::runtimeError(4, m_currentProgramLine.number);
        return false;
    }
    //reach the program beginning
    if(!this->skipToToken(TOKEN_BEGIN)){
        Sys::runtimeError(5, m_currentProgramLine.number);
        return false;
    }
    if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
        Sys::runtimeError(6, m_currentProgramLine.number);
        return false;
    }
    return true;
}

bool avil::run(char* program_name_and_args){

   if(this->usrPrgInit(program_name_and_args)){
       while(!this->finished()){
           if(!this->step()){
               return false;
           }
           if(Sys::killSignalReceived()){
               return false;
           }
       }
   }
   else{
       //init failed
       return false;
   }
   return true;
}

bool avil::step(){

    int i = m_currentProgramLine.number;

    //skip to the first meaningful token
    while(m_tokenizer.getToken() == TOKEN_ENDOFINPUT){
        if(!this->setCurrentProgramLine(i+1)){
            Sys::runtimeError(7, m_currentProgramLine.number);
            return false;
        }
        i++;
    }
    eTokenId l_currentToken = TOKEN_ERROR;
    bool l_bIsNumber = false;

    l_currentToken = m_tokenizer.getToken();
    switch(l_currentToken)
    {
    case (TOKEN_PRINT):
        if(!this->printStatement()){
            return false;
        }
        break;
    case (TOKEN_PRINTLN):
        if(!this->printlnStatement()){
            return false;
        }
        break;
    case(TOKEN_DELAY):
        if(!this->delayStatement()){
            return false;
        }
        break;
    case (TOKEN_GET_STR):
        if(!this->getStrStatement()){
            return false;
        }
        break;
    case (TOKEN_GET_INT):
        if(!this->getIntStatement(l_bIsNumber)){
            return false;
        }
        if(!l_bIsNumber) Sys::userOutput(F("NaN"));
        break;
    case (TOKEN_GOTO):
        if(!this->gotoStatement()){
            return false;
        }
        break;
    case (TOKEN_VAR_NAME):
        char l_sVarName[MAX_STRINGLEN + 1];
        if(m_tokenizer.getTokenVariableName(l_sVarName) && UsrPrgData::isLabel(l_sVarName)){
           //it's just a label...skip it
           if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
                return false;
           }
           break;
        }
        if(!this->assignmentStatement()){
            return false;
        }
        break;
    case (TOKEN_IF):
        if(!this->ifStatement()){
            return false;
        }
        break;
    case (TOKEN_ELSE):
        if(!this->elseStatement()){
            return false;
        }
        break;
    case (TOKEN_ENDIF):
        if(!this->endIfStatement()){
            return false;
        }
        break;
    case (TOKEN_FOR):
        if(!this->forStatement()){
            return false;
        }
        break;
    case (TOKEN_ENDFOR):
        if(!this->endForStatement()){
            return false;
        }
        break;
    case (TOKEN_CALL):
        if(!this->callStatement()){
            return false;
        }
        break;
    case (TOKEN_SUB):
        if(!this->subStatement()){
            return false;
        }
        break;
    case (TOKEN_ENDSUB):
        //this is trivial...no need of a statement, just roll back to the sub calling line
        if(m_subProgramLine!=0){
            if(!this->setCurrentProgramLine(m_subProgramLine + 1)){
                return false;
            }
        }
        m_subProgramLine = 0;
        break;
    case (TOKEN_COMMENT):
        //just a comment...skipt to the next line
        if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
            return false;
        }
        break;
    case (TOKEN_END):
        //this is the end my friend!
        break;
    default:
        Sys::runtimeError(8, m_currentProgramLine.number);
        return false;
        break;
    };
    return true;
}

bool avil::finished(){
    return m_tokenizer.getToken() == TOKEN_END;
}

void avil::clearAll(){

    UsrPrgData::clearAll();
    m_nestedIfCounter = 0;
    for (uint8_t i = 0; i < MAX_NESTED_IF; ++i){
        m_nestedIfStack[i].lineNumber = 0;
        m_nestedIfStack[i].condition = false;
    }
    m_nestedForCounter= 0;
    for (uint8_t i = 0; i < MAX_NESTED_FOR; ++i){
        m_nestedForStack[i].forLineNumber = 0;
        m_nestedForStack[i].endForLineNumber = 0;
        m_nestedForStack[i].toValue = 0;
        m_nestedForStack[i].step = 0;
    }
    m_currentProgramLine.number = 0;
    m_currentProgramLine.statements[0] = '\0';
}

bool avil::variablesDeclaration(){

    bool l_bUnexpected = false;

    if(!this->skipToToken(TOKEN_VARS_DECL_BEGIN, l_bUnexpected) || l_bUnexpected){
        Sys::runtimeError(12, m_currentProgramLine.number);
        return false;
    }
    unsigned int i = m_currentProgramLine.number;

    while(m_tokenizer.getToken() != TOKEN_VARS_DECL_END){

        i++;
        if(!this->setCurrentProgramLine(i)){
            Sys::runtimeError(13, m_currentProgramLine.number);
            return false;
        }
        switch(m_tokenizer.getToken())
        {
        case (TOKEN_BOOL_T):
            if (!this->booleanVariableDeclaration()) return false;
            break;
        case (TOKEN_INT_T):
            if (!this->integerVariableDeclaration()) return false;
            break;
        case (TOKEN_STRING_T):
            if (!this->stringVariableDeclaration()) return false;
            break;
        case (TOKEN_LABEL_T):
            if (!this->labelVariableDeclaration()) return false;
            break;
        case (TOKEN_ENDOFINPUT):
            //empty line ...just skip to the next
            break;
        case (TOKEN_VARS_DECL_END):
            //end of variables declaration reached...nothing to do here!(for the moment)
            break;
        case (TOKEN_COMMENT):
            //just a comment...skipt to the next line
            if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
                return false;
            }
            break;
        default:
            Sys::runtimeError(14, m_currentProgramLine.number);
            return false;
        }
    }
    return true;
}

bool avil::booleanVariableDeclaration(){

    eTokenId l_expectedTokensSequence[] = {TOKEN_BOOL_T, TOKEN_VAR_NAME, TOKEN_EQ};

    if (!this->expectedTokenSequence(l_expectedTokensSequence,ARRAYSIZE(l_expectedTokensSequence))){
        Sys::runtimeError(15, m_currentProgramLine.number);
        return false;
    }
    bool l_bValue;
    switch(m_tokenizer.getToken())
    {
    case (TOKEN_TRUE):
        l_bValue = true;
        break;
    case (TOKEN_FALSE):
        l_bValue = false;
        break;
    default:
        Sys::runtimeError(16, m_currentProgramLine.number);
        return false;
        break;
    };
    if(!this->setCurrentProgramLine(m_currentProgramLine.number)){
        return false;
    }
    this->skipToToken(TOKEN_VAR_NAME);
    char l_varName[MAX_VARNAMELEN + 1];
    if(!m_tokenizer.getTokenVariableName(l_varName)){
        Sys::runtimeError(17, m_currentProgramLine.number);
        return false;
    }
    if(!UsrPrgData::add(l_varName, l_bValue)){
        Sys::runtimeError(18, m_currentProgramLine.number);
        return false;
	}
    return true;
}

bool avil::integerVariableDeclaration(){

    eTokenId l_expectedTokensSequence[] = {TOKEN_INT_T, TOKEN_VAR_NAME, TOKEN_EQ, TOKEN_NUMBER};

    if (!this->expectedTokenSequence(l_expectedTokensSequence,ARRAYSIZE(l_expectedTokensSequence))){
        Sys::runtimeError(19, m_currentProgramLine.number);
        return false;
    }
    if(!this->setCurrentProgramLine(m_currentProgramLine.number)){
        return false;
    }
    this->skipToToken(TOKEN_VAR_NAME);
    char l_varName[MAX_VARNAMELEN + 1];
    if(!m_tokenizer.getTokenVariableName(l_varName)){
        Sys::runtimeError(20, m_currentProgramLine.number);
        return false;
    }
    this->skipToToken(TOKEN_NUMBER);
    int l_nValue;
    if(!m_tokenizer.getTokenIntValue(l_nValue)){
        Sys::runtimeError(21, m_currentProgramLine.number);
        return false;
    }
    if(!UsrPrgData::add(l_varName, l_nValue)){
        Sys::runtimeError(22, m_currentProgramLine.number);
        return false;
	}
    return true;
}

bool avil::labelVariableDeclaration(){

    eTokenId l_expectedTokensSequence[] = {TOKEN_LABEL_T, TOKEN_VAR_NAME};


    if (!this->expectedTokenSequence(l_expectedTokensSequence,ARRAYSIZE(l_expectedTokensSequence))){
        Sys::runtimeError(23, m_currentProgramLine.number);
        return false;
    }
    if(!this->setCurrentProgramLine(m_currentProgramLine.number)){
        return false;
    }
    this->skipToToken(TOKEN_VAR_NAME);
    char l_varName[MAX_VARNAMELEN + 1];
    if(!m_tokenizer.getTokenVariableName(l_varName)){
        Sys::runtimeError(24, m_currentProgramLine.number);
        return false;
    }
    if(!UsrPrgData::add(l_varName)){
        Sys::runtimeError(25, m_currentProgramLine.number);
        return false;
	}
    return true;
}

bool avil::stringVariableDeclaration(){

    eTokenId l_expectedTokensSequence[] = {TOKEN_STRING_T, TOKEN_VAR_NAME, TOKEN_EQ, TOKEN_STRING};


    if (!this->expectedTokenSequence(l_expectedTokensSequence,ARRAYSIZE(l_expectedTokensSequence))){
        Sys::runtimeError(26, m_currentProgramLine.number);
        return false;
    }

    if(!this->setCurrentProgramLine(m_currentProgramLine.number)){
        return false;
    }
    this->skipToToken(TOKEN_VAR_NAME);
    char l_varName[MAX_VARNAMELEN + 1];
    if(!m_tokenizer.getTokenVariableName(l_varName)){
        Sys::runtimeError(27, m_currentProgramLine.number);
        return false;
    }
    char l_sValue[MAX_STRINGLEN + 1];
    this->skipToToken(TOKEN_STRING);
    if(!m_tokenizer.getTokenStringValue(l_sValue)){
        Sys::runtimeError(28, m_currentProgramLine.number);
        return false;
    }
    if(!UsrPrgData::add(l_varName, l_sValue)){
        Sys::runtimeError(29, m_currentProgramLine.number);
        return false;
    }
    return true;
}

bool avil::gotoStatement(){

    //goto label_name
    //...
    //...
    //label_name:
    //...

    m_tokenizer.next();
    if(m_tokenizer.getToken() != TOKEN_VAR_NAME){
        Sys::runtimeError(30, m_currentProgramLine.number);
        return false;
    }
    char l_labelName[MAX_VARNAMELEN + 1];
    m_tokenizer.getTokenVariableName(l_labelName);
    if(!UsrPrgData::isLabel(l_labelName)){
        Sys::runtimeError(31, m_currentProgramLine.number);
        return false;
    }
    m_tokenizer.next();
    //if(m_tokenizer.getToken() != TOKEN_ENDOFINPUT && m_tokenizer.getToken() != TOKEN_CR){
    if(m_tokenizer.getToken() != TOKEN_ENDOFINPUT){
        Sys::runtimeError(32, m_currentProgramLine.number);
        return false;
    }
    //look for the label (it must be the first token of a program's line followed by "end of input")
    char l_varName[MAX_VARNAMELEN + 1];
    unsigned int i = m_currentProgramLine.number + 1;
    while(m_tokenizer.getToken() != TOKEN_VAR_NAME || strcmp(l_varName, l_labelName) != 0){
        if(!this->setCurrentProgramLine(i)){
            Sys::runtimeError(33, m_currentProgramLine.number);
            return false;
        }
        if(m_tokenizer.getToken() == TOKEN_VAR_NAME) m_tokenizer.getTokenVariableName(l_varName);
        i++;
    }
    m_tokenizer.next();
    if(m_tokenizer.getToken() != TOKEN_COLON){
        Sys::runtimeError(34, m_currentProgramLine.number);
        return false;
    }
    if(!this->setCurrentProgramLine(i)){
        Sys::runtimeError(35, m_currentProgramLine.number);
        return false;
    }
    return true;
}

bool avil::printStatement(){

    //print "ciao!!" || print sStringVariable || print nNum

    m_tokenizer.next();
    switch(m_tokenizer.getToken())
    {
    case (TOKEN_STRING):
        char l_sToPrint[MAX_STRINGLEN + 1];
        if(!m_tokenizer.getTokenStringValue(l_sToPrint)){
            Sys::runtimeError(89, m_currentProgramLine.number);
            return false;
        }
        Sys::userOutput(l_sToPrint);
        break;
    case (TOKEN_VAR_NAME):
        char l_varName[MAX_VARNAMELEN + 1];

        if(!m_tokenizer.getTokenVariableName(l_varName)){
            Sys::runtimeError(36, m_currentProgramLine.number);
            return false;
        }
        if(UsrPrgData::isInteger(l_varName)){
            int l_nValue;
            UsrPrgData::get(l_varName, l_nValue);
            Sys::userOutput(l_nValue);
        }
        else if(UsrPrgData::isString(l_varName)){
            //char l_sValue[MAX_STRINGLEN + 1];
            char* l_sValue;
            UsrPrgData::get(l_varName, l_sValue);
            Sys::userOutput(l_sValue);
        }
        else{
            Sys::runtimeError(37, m_currentProgramLine.number);
            return false;
        }
        break;
    default:
        Sys::runtimeError(38, m_currentProgramLine.number);
        return false;
        break;
    };
    if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
        return false;
    }
    return true;

}

bool avil::printlnStatement(){

    ;
    if(!this->printStatement()){
        return false;
    }
    Sys::userOutput(F("\n\r"));
    return true;
}

bool avil::delayStatement(){

    if(!this->callStatement()){
        return false;
    }
    return true;
}

bool avil::getStrStatement(){
    //getStr sVar

    m_tokenizer.next();
    if(m_tokenizer.getToken() != TOKEN_VAR_NAME){
        Sys::runtimeError(39, m_currentProgramLine.number);
        return false;
    }
    char l_varName[MAX_VARNAMELEN + 1];
    if(!m_tokenizer.getTokenVariableName(l_varName)){
        Sys::runtimeError(40, m_currentProgramLine.number);
        return false;
    }
    char l_sInput[MAX_STRINGLEN+1];
    if(!Sys::userInput(l_sInput, MAX_STRINGLEN+1)){
        Sys::runtimeError(41, m_currentProgramLine.number);
        return false;
    }
    if(UsrPrgData::isString(l_varName)){
        UsrPrgData::set(l_varName, l_sInput);
    }
    else{
        Sys::runtimeError(42, m_currentProgramLine.number);
        return false;
    }

    if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
        return false;
    }
    return true;
}

bool avil::getIntStatement(bool &isNumber){
    //getStr nVar

    isNumber = true;
    m_tokenizer.next();
    if(m_tokenizer.getToken() != TOKEN_VAR_NAME){
        Sys::runtimeError(43, m_currentProgramLine.number);
        return false;
    }
    char l_varName[MAX_VARNAMELEN + 1];
    if(!m_tokenizer.getTokenVariableName(l_varName)){
        Sys::runtimeError(44, m_currentProgramLine.number);
        return false;
    }
    char l_sInput[MAX_NUMLEN + 1];
    if(!Sys::userInput(l_sInput, MAX_NUMLEN+1)){
        Sys::runtimeError(45, m_currentProgramLine.number);
        return false;
    }
    for (uint8_t i=0;i<strlen(l_sInput); i++){
        if (!isdigit(l_sInput[i]) && l_sInput[i] != '\n')
        {
            //not a number!
            isNumber = false;
        }
    }
    int l_nInput = 0;
    if (isNumber) l_nInput = atoi(l_sInput);

    if (UsrPrgData::isInteger(l_varName)){
        UsrPrgData::set(l_varName, l_nInput);
    }
    else{
        Sys::runtimeError(46, m_currentProgramLine.number);
        return false;
    }

    if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
        return false;
    }
    return true;
}

bool avil::assignmentStatement(){

    char l_varName[MAX_VARNAMELEN + 1];

    if(!m_tokenizer.getTokenVariableName(l_varName)){
        Sys::runtimeError(47, m_currentProgramLine.number);
        return false;
    }
    m_tokenizer.next();
    if(m_tokenizer.getToken() != TOKEN_EQ){
        Sys::runtimeError(48, m_currentProgramLine.number);
        return false;
    }
    m_tokenizer.next();

    Expressions l_expression;

    if(UsrPrgData::isBoolean(l_varName)){
        //boolean assignment
        bool l_bResult = false;
        if(!l_expression.evaluate(m_tokenizer.getCurrStatements(), l_bResult)){
            Sys::runtimeError(49, m_currentProgramLine.number);
            return false;
        }
        else{
            UsrPrgData::set(l_varName, l_bResult);
        }
    }
    else if(UsrPrgData::isInteger(l_varName)){
        //integer assignment
        int l_nResult = 0;
        if(!l_expression.evaluate(m_tokenizer.getCurrStatements(), l_nResult)){
            Sys::runtimeError(50, m_currentProgramLine.number);
            return false;
        }
        else{
            UsrPrgData::set(l_varName, l_nResult);
        }
    } 
    else if(UsrPrgData::isString(l_varName)){
        //string assignment
        char l_sResult[MAX_STRINGLEN + 1];
        if(!l_expression.evaluate(this->m_tokenizer.getCurrStatements(), l_sResult)){
            Sys::runtimeError(51, m_currentProgramLine.number);
            return false;
        }
        else{
          UsrPrgData::set(l_varName, l_sResult);
        }
    }
    else{
        Sys::runtimeError(52, m_currentProgramLine.number);
        return false;
    }
    if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
        return false;
    }
    return true;
}

bool avil::ifStatement(){
    //check the condition
    Expressions l_expression;
    bool l_bIfCondition = false;

    m_tokenizer.next();
    if(!l_expression.evaluate(m_tokenizer.getCurrStatements(), l_bIfCondition)){
        Sys::runtimeError(53, m_currentProgramLine.number);
        return false;
    }
    //add an element at the top of the if's stack
    if(m_nestedIfCounter>=MAX_NESTED_IF){
        Sys::runtimeError(54, m_currentProgramLine.number);
        return false;
    }
    m_nestedIfStack[m_nestedIfCounter].condition = l_bIfCondition;
    m_nestedIfStack[m_nestedIfCounter].lineNumber = m_currentProgramLine.number;
    m_nestedIfCounter++;
    //continue the program or jump to else/endif
    if(l_bIfCondition){
        //continue
        if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
            return false;
        }
        return true;
    }
    else{
        //jump to else/endif
        int l_programLineCurrent = m_currentProgramLine.number;
        if(this->skipToElse()){
            if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
                return false;
            }
            return true;
        }
        if(!this->setCurrentProgramLine(l_programLineCurrent)){
            return false;
        }
        if(!this->skipToEndif()){
            Sys::runtimeError(55, m_currentProgramLine.number);
            return false;
        }
        return true;
    }
    return false;
}

bool avil::skipToElse(){

    unsigned int i = m_currentProgramLine.number;
    uint8_t l_nestedIf = 0;

    //look for the corresponding else of an if
    do {
        i++;
        if(!this->setCurrentProgramLine(i)){
            return false;
        }
        if(m_tokenizer.getToken() == TOKEN_IF) l_nestedIf++;
        if(m_tokenizer.getToken() == TOKEN_ENDIF && l_nestedIf == 0){
            return false;
        }
        if(m_tokenizer.getToken() == TOKEN_ENDIF && l_nestedIf > 0){
            l_nestedIf--;
        }
    } while (m_tokenizer.getToken() != TOKEN_ELSE || l_nestedIf != 0);

    return true;
}

bool avil::skipToEndif(){

    unsigned int i = m_currentProgramLine.number;
    uint8_t l_nestedIf = 1;
    ;

    //look for the corresponding endif of an if
    do{
        i++;
        if(!this->setCurrentProgramLine(i)){
            Sys::runtimeError(56, m_currentProgramLine.number);
            return false;
        }
        if(m_tokenizer.getToken() == TOKEN_IF) l_nestedIf++;
        if(m_tokenizer.getToken() == TOKEN_ENDIF && l_nestedIf > 0) l_nestedIf--;
    }while(m_tokenizer.getToken() != TOKEN_ENDIF || l_nestedIf != 0);

    return true;
}

bool avil::elseStatement(){

    ;

    if(m_nestedIfCounter>MAX_NESTED_IF){
        Sys::runtimeError(57, m_currentProgramLine.number);
        return false;
    }
    //check the condition of the if at the top of the if's stack
    if(m_nestedIfCounter > 0){
        if (m_nestedIfStack[m_nestedIfCounter - 1].condition){
            //jump to endif
            if(!this->skipToEndif()){
                Sys::runtimeError(58, m_currentProgramLine.number);
                return false;
            }
            return true;
        }
        else{
            //continue
            if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
                return false;
            }
            return true;
        }
    }
    else{
        if(!this->skipToEndif()){
            Sys::runtimeError(59, m_currentProgramLine.number);
            return false;
        }
        return true;
    }
    return false;
}

bool avil::endIfStatement(){
    //remove the element at the top of the if's stack
    if(m_nestedIfCounter == 0){
        if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
            return false;
        }
        return true;
    }
    m_nestedIfStack[m_nestedIfCounter - 1].condition = false;
    m_nestedIfStack[m_nestedIfCounter - 1].lineNumber = 0;
    m_nestedIfCounter--;

    if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
        return false;
    }
    return true;
}

bool avil::forStatement(){
    //check if the for is already int the stack
    bool l_bForAlreadyExists = false;

    for (uint8_t i = 0; i < MAX_NESTED_FOR; ++i){
        if (m_nestedForStack[i].forLineNumber == m_currentProgramLine.number){
            l_bForAlreadyExists = true;
            break;
        }
    }
    //retrieve for variable name
    char l_varName[MAX_VARNAMELEN + 1];
    m_tokenizer.next();
    if (!m_tokenizer.getTokenVariableName(l_varName)){
        Sys::runtimeError(61, m_currentProgramLine.number);
        return false;
    }
    //if not add it
    if (!l_bForAlreadyExists){

        if(m_nestedForCounter >= MAX_NESTED_FOR){
            Sys::runtimeError(60, m_currentProgramLine.number);
            return false;
        }
        //for line number
        m_nestedForStack[m_nestedForCounter].forLineNumber = m_currentProgramLine.number;
        //equal token
        m_tokenizer.next();
        if (m_tokenizer.getToken() != TOKEN_EQ){
            Sys::runtimeError(62, m_currentProgramLine.number);
            return false;
        }
        //starting value of for variable(could be an expression)
        Expressions l_expression;
        int l_nResult = 0;
        m_tokenizer.next();
        if(!l_expression.evaluate(m_tokenizer.getCurrStatements(), l_nResult)){
            Sys::runtimeError(63, m_currentProgramLine.number);
            return false;
        }
        if (!UsrPrgData::set(l_varName, l_nResult)){
            Sys::runtimeError(90, m_currentProgramLine.number);
            return false;
        }
        //"to" token
        m_tokenizer.next();
        if (m_tokenizer.getToken() != TOKEN_TO){
            Sys::runtimeError(65, m_currentProgramLine.number);
            return false;
        }
        //to index value (could be an expression)
        m_tokenizer.next();
        if(!l_expression.evaluate(m_tokenizer.getCurrStatements(), m_nestedForStack[m_nestedForCounter].toValue)){
            Sys::runtimeError(66, m_currentProgramLine.number);
            return false;
        }
        //"step" token
        m_tokenizer.next();
        if (m_tokenizer.getToken() != TOKEN_STEP){
            Sys::runtimeError(67, m_currentProgramLine.number);
            return false;
        }
        //step value (could be an expression)
        m_tokenizer.next();
        if(!l_expression.evaluate(m_tokenizer.getCurrStatements(), m_nestedForStack[m_nestedForCounter].step)){
            Sys::runtimeError(68, m_currentProgramLine.number);
            return false;
        }
        //look for endfor
        unsigned int l_currProgramLineNumber = m_currentProgramLine.number;
        if(!this->skipToEndFor()){
            Sys::runtimeError(69, m_currentProgramLine.number);
            return false;
        }
        m_nestedForStack[m_nestedForCounter].endForLineNumber = m_currentProgramLine.number;
        if(!this->setCurrentProgramLine(l_currProgramLineNumber)){
            return false;
        }
        m_nestedForCounter++;
    }

    //check for loop condition
    uint8_t i;
    for ( i = 0; i <= MAX_NESTED_FOR; ++i){
        if (m_nestedForStack[i].forLineNumber == m_currentProgramLine.number){
           break;
        }
    }
    bool l_bForCondition = false;
    //retrieve for variable value to determine the loop condition status
    int l_nValue;
    if(!UsrPrgData::get(l_varName, l_nValue)){
        Sys::runtimeError(64, m_currentProgramLine.number);
        return false;
    }
    if (m_nestedForStack[i].step > 0){
        l_bForCondition = l_nValue <= m_nestedForStack[i].toValue;
    }
    else if(m_nestedForStack[i].step < 0){
        l_bForCondition = l_nValue >= m_nestedForStack[i].toValue;
    }
    else{
        l_bForCondition = true;
    }

    if (l_bForCondition){
      //for loop iteration
      this->setCurrentProgramLine(m_currentProgramLine.number + 1);
    }
    else{
        //skip after endfor
        if(!this->skipToEndFor()){
            Sys::runtimeError(70, m_currentProgramLine.number);
            return false;
        }
        if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
            return false;
        }
        //remove for from stack!
        m_nestedForStack[i].forLineNumber = 0;
        m_nestedForStack[i].endForLineNumber = 0;
        m_nestedForStack[i].toValue = 0;
        m_nestedForStack[i].step = 0;
        m_nestedForCounter--;
    }
    return true;
}

bool avil::skipToEndFor(){

    unsigned int i = m_currentProgramLine.number ;
    int8_t l_nestedFor = 0;
    //look for the corresponding endfor of a for
    do{
        i++;
        if(!this->setCurrentProgramLine(i)){
            return false;
        }
        if(m_tokenizer.getToken() == TOKEN_FOR) l_nestedFor++;
        if(m_tokenizer.getToken() == TOKEN_ENDFOR && l_nestedFor >= 0) l_nestedFor--;
    }while(m_tokenizer.getToken() != TOKEN_ENDFOR || l_nestedFor >= 0);

    return true;
}

bool avil::endForStatement(){

    uint8_t i;

    for ( i = 0; i <= MAX_NESTED_FOR; ++i){
        if (m_nestedForStack[i].endForLineNumber == m_currentProgramLine.number){
           break;
        }
    }
    if (i == MAX_NESTED_FOR){
        Sys::runtimeError(71, m_currentProgramLine.number);
        return false;
    }
    //retrieve for variable name
    if(!this->setCurrentProgramLine(m_nestedForStack[i].forLineNumber)){
        return false;
    }
    char l_varName[MAX_VARNAMELEN + 1];
    m_tokenizer.next();
    if (!m_tokenizer.getTokenVariableName(l_varName)){
        Sys::runtimeError(72, m_currentProgramLine.number);
        return false;
    }
    //retrieve for variable value
    int l_nValue;
    if(!UsrPrgData::get(l_varName, l_nValue)){
        Sys::runtimeError(87, m_currentProgramLine.number);
        return false;
    }
    //update for loop variable value
    if(!UsrPrgData::set(l_varName, l_nValue + m_nestedForStack[i].step)){
        Sys::runtimeError(88, m_currentProgramLine.number);
        return false;
    }
    if(!this->setCurrentProgramLine(m_nestedForStack[i].forLineNumber)){
        return false;
    }
    return true;
}

bool avil::callStatement(){

    if(m_tokenizer.getToken() == TOKEN_CALL){
        m_tokenizer.next();
    }
    //get the program name
    char l_prgName[MAX_PRG_NAME_LENGTH + 1];
    if(!this->getPrgName(m_tokenizer.getCurrStatements(), l_prgName)){
        Sys::runtimeError(83, m_currentProgramLine.number);
        return false;
    }
    //check if exists
    if(!Sys::sysProgramExists(l_prgName)){
        Sys::runtimeError(85, m_currentProgramLine.number);
        return false;
    }
    //collect the program's arguments
    if(!this->getPrgArguments(m_tokenizer.getCurrStatements())){
        Sys::runtimeError(84, m_currentProgramLine.number);
        return false;
    }
    if(!Sys::callPrg(l_prgName)){
        //something went wrong during call execution
        Sys::runtimeError(82, m_currentProgramLine.number);
        return false;
    }
    if(!this->setCurrentProgramLine(m_currentProgramLine.number + 1)){
        return false;
    }
    return true;
}

bool avil::subStatement(){

    char l_subName[MAX_VARNAMELEN + 1];

    if(m_subProgramLine != 0){
        //we are already in a sub (and this is not inception!)
        Sys::runtimeError(81, m_currentProgramLine.number);
        return false;
    }
    //save the program line number
    m_subProgramLine = m_currentProgramLine.number;
    //get the sub name
    m_tokenizer.next();
    if(!m_tokenizer.getTokenVariableName(l_subName)){
        Sys::runtimeError(77, m_currentProgramLine.number);
        return false;
    }
    //execute it...or at least try...
    if(!this->skipToSub(l_subName)){
        Sys::runtimeError(78, m_currentProgramLine.number);
        return false;
    }
    return true;
}

bool avil::setCurrentProgramLine(unsigned int lineNumber){

    if(Sys::getFileLine(m_programName, lineNumber, m_currentProgramLine.statements)){
        m_currentProgramLine.number = lineNumber;
        m_tokenizer.setStatements(m_currentProgramLine.statements);
        m_tokenizer.next();
        return true;
    }
    else{
        Sys::runtimeError(73, m_currentProgramLine.number);
        m_currentProgramLine.number = 0;
        return false;
    }

}

bool avil::skipToToken(eTokenId token){

    bool l_bUnexpected;
    return this->skipToToken(token, l_bUnexpected);
}

bool avil::skipToToken(eTokenId token, bool &bUnexpected){
    //skip at the first occourrence of "token" in the current program line ignoring TOKEN_ENDOFINPUT
    //"bUnexpected" is set to true if the first token found it's not "token"
    unsigned int i = m_currentProgramLine.number;

    bUnexpected = false;

    while(m_tokenizer.getToken() != token){
        m_tokenizer.next();
        //if(m_tokenizer.getToken() == TOKEN_ENDOFINPUT || m_tokenizer.getToken() == TOKEN_CR){
        if(m_tokenizer.getToken() == TOKEN_ENDOFINPUT){
            i++;
            if(!this->setCurrentProgramLine(i)){
                return false;
            }
        }
        //if(m_tokenizer.getToken() != token && m_tokenizer.getToken() != TOKEN_ENDOFINPUT && m_tokenizer.getToken() != TOKEN_CR){
        if(m_tokenizer.getToken() != token && m_tokenizer.getToken() != TOKEN_ENDOFINPUT){
            bUnexpected = true;
        }
    }
    return true;
}

bool avil::expectedTokenSequence(eTokenId tokenSequence[], size_t size){

    uint8_t i = 0;
    eTokenId expectedToken = TOKEN_ERROR, currentToken = TOKEN_ERROR;
    do {
        expectedToken = tokenSequence[i];
        currentToken = m_tokenizer.getToken();
        if(currentToken != expectedToken){
            return false;
        }
        m_tokenizer.next();
        i++;
    } while (i < size);
    return true;
}

bool avil::skipToSub(char* subName){

    char l_subName[MAX_VARNAMELEN + 1];
    //starting from the top...
    this->setCurrentProgramLine(FIRST_PROGRAM_LINE);
    //in one way or another this loop will end...
    while(true){
        //lok for the next sub definition
        if(!this->skipToToken(TOKEN_BEGSUB)){
            Sys::runtimeError(79, m_currentProgramLine.number);
            return false;
        }
        //get the sub name
        m_tokenizer.next();
        if(!m_tokenizer.getTokenVariableName(l_subName)){
            Sys::runtimeError(80, m_currentProgramLine.number);
            return false;
        }
        if(strcmp(subName, l_subName)==0){
            //found it!...execute the sub code
            if(!this->setCurrentProgramLine(m_currentProgramLine.number+1)){
                return false;
            }
            return true;
        }
    }
}

bool avil::getPrgName(const char *program_name_and_args, char* program_name){

    //get the program name from the input cmd (made of program's name and arguments)
    const char* args = program_name_and_args;
    uint8_t len = 0;
    while(isalnum(*args) || *args=='.'){
        ++args;
        len++;
        if(len > MAX_PRG_NAME_LENGTH){
            return false;
        }
    }
    memcpy(program_name, program_name_and_args, args - program_name_and_args);
    program_name[args - program_name_and_args] = 0;
    return true;
}

bool avil::getPrgArguments(const char *program_name_and_args){

    const char* args = program_name_and_args;
    uint8_t len = 0;
    //move at the args beginning
    while(isalnum(*args) || *args=='.'){
        ++args;
        len++;
        if(len > MAX_PRG_NAME_LENGTH){
            return false;
        }
    }
    //set up a tokenizer to look for the arguments
    Tokenizer l_tokenizer;
    l_tokenizer.setStatements(args);
    l_tokenizer.next();

    uint8_t i = 1;

    while(l_tokenizer.getToken() != TOKEN_ENDOFINPUT){
        if (l_tokenizer.getToken() == TOKEN_NUMBER){
            int l_nArg;
            l_tokenizer.getTokenIntValue(l_nArg);
            if(!IOData::setArg(i, l_nArg)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_STRING){
            char l_sArg[MAX_STRINGLEN + 1];
            if(!l_tokenizer.getTokenStringValue(l_sArg)){
                return false;
            }
            if(!IOData::setArg(i, l_sArg)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_INPUT){
            char l_sArg[MAX_STRINGLEN + 1];
            strcpy(l_sArg, "INPUT");
            if(!IOData::setArg(i, l_sArg)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_OUTPUT){
            char l_sArg[MAX_STRINGLEN + 1];
            strcpy(l_sArg, "OUTPUT");
            if(!IOData::setArg(i, l_sArg)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_TRUE){
            if(!IOData::setArg(i, true)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_FALSE){
            if(!IOData::setArg(i, false)){
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        else if(l_tokenizer.getToken() == TOKEN_VAR_NAME){

            char l_varName[MAX_VARNAMELEN + 1];
            if(!l_tokenizer.getTokenVariableName(l_varName)){
                return false;
            }
            if(UsrPrgData::isBoolean(l_varName)){
                //boolean
                bool l_bResult = false;
                UsrPrgData::get(l_varName, l_bResult);
                IOData::setArg(i, l_bResult);
            }
            else if(UsrPrgData::isInteger(l_varName)){
                //integer
                int l_nResult;
                UsrPrgData::get(l_varName, l_nResult);
                IOData::setArg(i, l_nResult);
            }
            else if(UsrPrgData::isString(l_varName)){
                //string
                char* l_sResult;
                UsrPrgData::get(l_varName, l_sResult);
                IOData::setArg(i, l_sResult);
            }
            else{
                return false;
            }
            l_tokenizer.next();
            i++;
        }
        //else if(l_tokenizer.getToken() == TOKEN_LEFTPAREN || l_tokenizer.getToken() == TOKEN_RIGHTPAREN || l_tokenizer.getToken() == TOKEN_COMMA){
            //do nothing...just skip to the next
            //l_tokenizer.next();
        //}
        else{
            //Sys::userOutput("argerr");
            return false;
        }
    }
    return true;
}
