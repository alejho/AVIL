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
#include "expressions.h"

Expressions::Expressions()
{

}


bool Expressions::evaluate(const char* expression, bool &result){
    //testare tutti questi casi!

    //kind of boolean assignement allowed:
    // bVar = true/false       direct assignment (ok)
    // bVar = bOp1 and bOp2    and between bools vars (ok)
    // bVar = bOp1 or bOp2     or between bools vars (ok)
    // bVar = bOp1 and true    mixed bools operations (ok)
    // bVar = bOp1 == bOp2     boolean comparison (ok)
    // bVar = n1 ><== n2       integer comparison (ok)
    // bVar = str1 == str2     string comparison (ok)

    char l_varName[MAX_VARNAMELEN + 1];

    m_tokenizer.setStatements(expression);

    m_tokenizer.next();
    eTokenId l_currToken = TOKEN_ERROR;
    l_currToken = m_tokenizer.getToken();

    switch(l_currToken)
    {
    case (TOKEN_TRUE):
        //it's a boolean operation
        if(!this->boolOperation(result)){
            return false;
        }
        return true;
        break;
    case (TOKEN_FALSE):
        //it's a boolean operation
        if(!this->boolOperation(result)){
            return false;
        }
        return true;
        break;
    case (TOKEN_VAR_NAME):
        if(!m_tokenizer.getTokenVariableName(l_varName)){
            return false;
        }
        if(UsrPrgData::isBoolean(l_varName)){
            //it's a boolean operation
            if(!this->boolOperation(result)){
                return false;
            }
            return true;
        }
        else if(UsrPrgData::isInteger(l_varName)){
            //is an integer comparison
            if(!this->IntComparison(result)){
                return false;
            }
            return true;
        }
        else if(UsrPrgData::isString(l_varName)){
            //is a string comparison
            if(!this->StringComparison(result)){
                return false;
            }
            return true;
        }
        else{
            //unknown variable type
            return false;
        }
        break;
    case (TOKEN_NUMBER):
        //is an integer comparison
        if(!this->IntComparison(result)){
            return false;
        }
        return true;
        break;
    case (TOKEN_STRING):
        //is a string comparison
        if(!this->StringComparison(result)){
            return false;
        }
        return true;
        break;
    default:
        //unexpected token
        return false;
        break;
    };
}

bool Expressions::evaluate(const char* expression, int &result){

    m_tokenizer.setStatements(expression);
    m_tokenizer.next();
    if(!this->IntOperation(result)){
        return false;
    }
    return true;
}

bool Expressions::evaluate(const char* expression, char* result){

    m_tokenizer.setStatements(expression);
    m_tokenizer.next();
    if(!this->StringOperation(result)){
        return false;
    }
    return true;
}

bool Expressions::IntComparison(bool &result){

    //first operand
    int l_nFirstOperand = 0;
    if(!this->getOperand(l_nFirstOperand)){
        return false;
    }
    //operator
    eTokenId opToken = TOKEN_ERROR;
    m_tokenizer.next();
    opToken = m_tokenizer.getToken();
    if(opToken != TOKEN_GT && opToken != TOKEN_LT && opToken != TOKEN_EQ_C){
        return false;
    }
    //second operand
    int l_nSecondOperand = 0;
    m_tokenizer.next();
    if(!this->getOperand(l_nSecondOperand)){
        return false;
    }
    //do the comparison
    switch(opToken)
    {
    case (TOKEN_GT):
        result = l_nFirstOperand > l_nSecondOperand;
        return true;
        break;
    case (TOKEN_LT):
        result = l_nFirstOperand < l_nSecondOperand;
        return true;
        break;
    case (TOKEN_EQ_C):
        result = l_nFirstOperand == l_nSecondOperand;
        return true;
        break;
    default:
        //unexpected operator
        return false;
        break;
    };
    return false;
}

bool Expressions::StringComparison(bool &result){


    //first operand
    char l_sFirstOperand[MAX_STRINGLEN + 1];
    if(!this->getOperand(l_sFirstOperand)){
        return false;
    }
    //operator
    eTokenId opToken = TOKEN_ERROR;
    m_tokenizer.next();
    opToken = m_tokenizer.getToken();
    if(opToken != TOKEN_EQ_C){
        //unexpected operator
        return false;
    }
    //second operand
    char l_sSecondOperand[MAX_STRINGLEN + 1];
    m_tokenizer.next();
    if(!this->getOperand(l_sSecondOperand)){
        return false;
    }
    //do the comparison
    switch(opToken)
    {
    case (TOKEN_EQ_C):
        result = strcmp(l_sFirstOperand,l_sSecondOperand) == 0;
        return true;
        break;
    default:
        //uexpected operator
        return false;
        break;
    };
    return false;

}

bool Expressions::boolOperation(bool &result){

    //get first operand
    bool l_bFirstOperand = false;
    if(!this->getOperand(l_bFirstOperand)){
        return false;
    }
    //check if it's a direct boolean assignment!
    m_tokenizer.next();
    //if(m_tokenizer.getToken() == TOKEN_ENDOFINPUT || m_tokenizer.getToken() == TOKEN_CR){
    if(m_tokenizer.getToken() == TOKEN_ENDOFINPUT){
        result = l_bFirstOperand;
        return true;
    }
    //logical operator;
    eTokenId opToken = TOKEN_ERROR;
    opToken = m_tokenizer.getToken();
    if(opToken != TOKEN_AND && opToken != TOKEN_OR && opToken != TOKEN_EQ_C){
        return false;
    }

    m_tokenizer.next();
    //get second operand
    bool l_bSecondOperand =false;
    if(!this->getOperand(l_bSecondOperand)){
        return false;
    }
    //execute the operation
    switch(opToken)
    {
    case (TOKEN_AND):
        result = l_bFirstOperand && l_bSecondOperand;
        return true;
        break;
    case (TOKEN_OR):
        result = l_bFirstOperand || l_bSecondOperand;
        return true;
        break;
    case (TOKEN_EQ_C):
        result = l_bFirstOperand == l_bSecondOperand;
        return true;
    break;
    default:
        //unexpected operator
        return false;
        break;
    };
    return false;
}

bool Expressions::IntOperation(int &result){

    //first operand
    int l_nFirstOperand = 0;
    if(!this->getOperand(l_nFirstOperand)){
        return false;
    }
    //check if it's a direct integer assignment!
    //(an integer expression could be finished also if a "to" or a "step" of a for loop header is encountered)
    m_tokenizer.next();
    //if(m_tokenizer.getToken() == TOKEN_ENDOFINPUT || m_tokenizer.getToken() == TOKEN_TO || m_tokenizer.getToken() == TOKEN_STEP || m_tokenizer.getToken() == TOKEN_CR){
    if(m_tokenizer.getToken() == TOKEN_ENDOFINPUT || m_tokenizer.getToken() == TOKEN_TO || m_tokenizer.getToken() == TOKEN_STEP){
        result = l_nFirstOperand;
        return true;
    }
    //operator
    eTokenId opToken = TOKEN_ERROR;
    opToken = m_tokenizer.getToken();
    if(opToken != TOKEN_PLUS && opToken != TOKEN_MINUS && opToken != TOKEN_ASTR && opToken != TOKEN_SLASH){
        //unexpected operator
        return false;
    }
    //second operand
    m_tokenizer.next();
    int l_nSecondOperand = 0;
    if(!this->getOperand(l_nSecondOperand)){
        return false;
    }
    //do the operation
    switch(opToken)
    {
    case (TOKEN_PLUS):
        result = l_nFirstOperand + l_nSecondOperand;
        return true;
        break;
    case (TOKEN_MINUS):
        result = l_nFirstOperand - l_nSecondOperand;
        return true;
        break;
    case (TOKEN_ASTR):
        result = l_nFirstOperand * l_nSecondOperand;
        return true;
        break;
    case (TOKEN_SLASH):
        result = l_nFirstOperand / l_nSecondOperand;
        return true;
        break;
    default:
        return false;
        break;
    };
    return false;
}

bool Expressions::StringOperation(char* result){


    //first operand
    char l_sFirstOperand[MAX_STRINGLEN + 1];
    if(!this->getOperand(l_sFirstOperand)){
        return false;
    }
    //check if it's a direct integer assignment!
    m_tokenizer.next();
    //if(m_tokenizer.getToken() == TOKEN_ENDOFINPUT || m_tokenizer.getToken() == TOKEN_CR){
    if(m_tokenizer.getToken() == TOKEN_ENDOFINPUT){
        strcpy(result, l_sFirstOperand);
        return true;
    }
    //operator
    eTokenId opToken = TOKEN_ERROR;
    opToken = m_tokenizer.getToken();
    if(opToken != TOKEN_PLUS){
        return false;
    }
    //second operand
    m_tokenizer.next();
    char l_sSecondOperand[MAX_STRINGLEN + 1];
    if(!this->getOperand(l_sSecondOperand)){
        return false;
    }

    //will be the result short enough?
    if(strlen(l_sFirstOperand) + strlen(l_sSecondOperand) > MAX_STRINGLEN){
        return false;
    }
    //do the operation
    switch(opToken)
    {
    case (TOKEN_PLUS):
        strcat(l_sFirstOperand, l_sSecondOperand);
        strcpy(result, l_sFirstOperand);
        return true;
        break;
    default:
        //unexpected operators
        return false;
        break;
    };
    return false;

}

bool Expressions::getOperand(bool &op){

    //get the boolean value of the next token

    char l_varName[MAX_VARNAMELEN + 1];
    eTokenId l_currToken = TOKEN_ERROR;

    l_currToken = m_tokenizer.getToken();

    switch(l_currToken)
    {
    case (TOKEN_TRUE):
        op = true;
        return true;
        break;
    case (TOKEN_FALSE):
        op = false;
        return true;
        break;
    case (TOKEN_VAR_NAME):
        if(!m_tokenizer.getTokenVariableName(l_varName)){
            return false;
        }
        if(!UsrPrgData::isBoolean(l_varName)){
            return false;
        }
        bool l_bValue;
        if(!UsrPrgData::get(l_varName, l_bValue)){
            return false;
        }
        op = l_bValue;
        return true;
        break;
    default:
        return false;
        break;
    };
    return false;
}

bool Expressions::getOperand(int &op){

    //get the integer value of the next token
    char l_varName[MAX_VARNAMELEN + 1];

    eTokenId l_currToken = m_tokenizer.getToken();
    bool l_bNegative = false;
    if(l_currToken == TOKEN_MINUS){
        l_bNegative = true;
        m_tokenizer.next();
        l_currToken = m_tokenizer.getToken();
    }

    switch(l_currToken)
    {
    case (TOKEN_VAR_NAME):
        if(!m_tokenizer.getTokenVariableName(l_varName)){
            return false;
        }
        if(!UsrPrgData::isInteger(l_varName)){
            return false;
        }
        int l_nValue;
        if(!UsrPrgData::get(l_varName, l_nValue)){
            return false;
        }
        op = l_nValue;
        break;

    case (TOKEN_NUMBER):
        if(!m_tokenizer.getTokenIntValue(op)){
            return false;
        }
        break;
    default:
        //unexpected token
        return false;
        break;
    };

    if (l_bNegative) op = -op;

    return true;
}

bool Expressions::getOperand(char* op){

    //get the string value of the next token
    char l_varName[MAX_VARNAMELEN + 1];
    eTokenId l_currToken = TOKEN_ERROR;
    l_currToken = m_tokenizer.getToken();

    switch(l_currToken)
    {
    case (TOKEN_VAR_NAME):
        if(!m_tokenizer.getTokenVariableName(l_varName)){
            return false;
        }
        if(!UsrPrgData::isString(l_varName)){
            return false;
        }
       //char l_sValue[MAX_STRINGLEN + 1];
        char* l_sValue;
        if(!UsrPrgData::get(l_varName, l_sValue)){
            return false;
        }
        strcpy(op, l_sValue);
        return true;
        break;
    case (TOKEN_STRING):
        if(!m_tokenizer.getTokenStringValue(op)){
            return false;
        }
        return true;
        break;
    default:
        return false;
        break;
    };
    return false;
}
