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
#include "avilTok.h"

Tokenizer::Tokenizer()
{
    m_statements = 0;
    m_statementsPtrCurrent = NULL;
    m_statementsPtrNext = NULL;
    //m_keywords = NULL;
    m_currentToken = TOKEN_ERROR;
    //m_keywords = keywords;
}

eTokenId Tokenizer::getToken(){
    return m_currentToken;
}

void Tokenizer::setStatements(const char* statements){

    m_statements = statements;
    m_statementsPtrCurrent = statements;
    m_statementsPtrNext = statements;
}

const char* Tokenizer::getStatements(){
    //return the complete statements line the tokenizer have to analyze
    return m_statements;
}

const char* Tokenizer::getCurrStatements(){
    //return the remaining statements tokenizer
    return m_statementsPtrCurrent;
}

/*token Tokenizer::getKeyword(eTokenId keywordTokenId){

    token keywordToRet;
    unsigned int i = 0;
    while(keywordToRet.tokenId != keywordTokenId || keywordToRet.tokenId == TOKEN_ERROR){
        PROGMEM_get(&keywords[i], keywordToRet);
        i++;
    }
    return keywordToRet;

}*/

void Tokenizer::next(){

    //set the current token to the next one
    m_currentToken = TOKEN_ERROR;

    if(!this->finished()) {

        m_statementsPtrCurrent = m_statementsPtrNext;
        //move the pointer to the beginning of the next token
        while(*m_statementsPtrCurrent == ' ' || *m_statementsPtrCurrent == '\t' || *m_statementsPtrCurrent == '\n' || *m_statementsPtrCurrent == '\r') {
            ++m_statementsPtrCurrent;
          }
        //read what is the next token
        m_currentToken = this->readToken();
    }
    else {
        m_currentToken = TOKEN_ENDOFINPUT;
        m_statements = NULL;
        m_statementsPtrCurrent = NULL;
        m_statementsPtrNext = NULL;
    }
}

bool Tokenizer::finished(){

    if (m_statementsPtrCurrent == NULL){
        return true;
    }
    if (*m_statementsPtrCurrent == '\0'){
        return true;
    }
    if (m_currentToken == TOKEN_ENDOFINPUT){
        return true;
    }
    return false;
}

eTokenId Tokenizer::readToken(){

    if(*m_statementsPtrCurrent == 0) {
        return TOKEN_ENDOFINPUT;
    }
    else if(isdigit(*m_statementsPtrCurrent)) {
        //number
        int i=0;
        for(i = 0; i < MAX_NUMLEN; ++i) {
          if(!isdigit(m_statementsPtrCurrent[i])){
            if(i > 0) {
              m_statementsPtrNext = m_statementsPtrCurrent + i;
              return TOKEN_NUMBER;
            } else {
              //number is too short
              return TOKEN_ERROR;
            }
          }
        }
        //number is too long
        return TOKEN_ERROR;
    }
    else if(*m_statementsPtrCurrent == '"'){
      //string
      m_statementsPtrNext = m_statementsPtrCurrent;
       do{
        ++m_statementsPtrNext;
       }while(*m_statementsPtrNext != '"' && *m_statementsPtrNext != 0);

       if(*m_statementsPtrNext != 0){
           ++m_statementsPtrNext;
           return TOKEN_STRING;
       }
       else{
           //endless string
           return TOKEN_ERROR;
       }
    }
    else{
        //keywords
        token currKeyword;

        for(unsigned int i = 0; i<ARRAYSIZE(keywords)-1; i++) {

              PROGMEM_get(&keywords[i], currKeyword);
              bool l_bKeywordFound = strncmp(m_statementsPtrCurrent, currKeyword.keyword, strlen(currKeyword.keyword)) == 0 && strlen(currKeyword.keyword)>0;
              //bool l_bNoTrailingAlpha = *(m_statementsPtrCurrent+strlen(keywordPtr->keyword)) == ' ' || *(m_statementsPtrCurrent+strlen(keywordPtr->keyword)) == '\t' || *(m_statementsPtrCurrent+strlen(keywordPtr->keyword)) == '\0' || isdigit(*(m_statementsPtrCurrent+strlen(keywordPtr->keyword)));
              bool l_bNoTrailingAlpha = true;
              if(l_bKeywordFound && l_bNoTrailingAlpha){
                m_statementsPtrNext = m_statementsPtrCurrent + strlen(currKeyword.keyword);
                return currKeyword.tokenId;
              }
            }
        //variables
        m_statementsPtrNext =  m_statementsPtrCurrent;
        //if(isalnum(*m_statementsPtrNext) || *name_end==*IN_ARGS_IDENTIFIER || *name_end==*OUT_VALS_IDENTIFIER){
            unsigned int i = 0;
            while((isalnum(*m_statementsPtrNext) || *m_statementsPtrNext==*IN_ARGS_IDENTIFIER || *m_statementsPtrNext==*OUT_VALS_IDENTIFIER) && *m_statementsPtrNext != ' '){
                ++m_statementsPtrNext;
                i++;
                if(i > MAX_VARNAMELEN){
                    //variable name is too long
                    return TOKEN_ERROR;
                }
            }
            return TOKEN_VAR_NAME;
        //}
    }
    //can't recognize this token
    return TOKEN_ERROR;
}

bool Tokenizer::getTokenVariableName(char* dst){

    const char *name_end;
    size_t string_len;
    int i;

    if(m_currentToken != TOKEN_VAR_NAME) {
      //unexpected token
      return false;
    }

    i = 0;
    name_end = m_statementsPtrCurrent;
    //while(*name_end != ' ' && *name_end != 0 && *name_end != '=' && *name_end != ':' && *name_end != '\t') {
    while(isalnum(*name_end) || *name_end==*IN_ARGS_IDENTIFIER || *name_end==*OUT_VALS_IDENTIFIER ) {
        ++name_end;
        i++;
        if(i > MAX_VARNAMELEN){
            //variable name is too long
            return false;
        }
    }
    string_len = name_end - m_statementsPtrCurrent;
    if (string_len == 0) return false;

    memcpy(dst, m_statementsPtrCurrent, string_len);
    dst[string_len] = 0;
    return true;
}

bool Tokenizer::getTokenStringValue(char* dst) {

  const char *string_end;
  size_t string_len;
  int i;

  if(m_currentToken != TOKEN_STRING) {
    //unexpected token
    return false;
  }
  i = 0;
  string_end = m_statementsPtrCurrent + 1;
  while(*string_end != '"') {
      ++string_end;
      i++;
      if(i > MAX_STRINGLEN){
          //string value is too long!
          return false;
      }
  }
  string_len = string_end - m_statementsPtrCurrent - 1;
  memcpy(dst, m_statementsPtrCurrent + 1, string_len);
  dst[string_len] = 0;
  return true;
}

bool Tokenizer::getTokenIntValue(int &dst){

    if(m_currentToken != TOKEN_NUMBER) {
      //unexpected token!
      return false;
    }
    dst = atoi(m_statementsPtrCurrent);
    return true;
}

