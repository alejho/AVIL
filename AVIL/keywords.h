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
#ifndef KEYWORDS
#define KEYWORDS

#include "compopt.h"

//this header work as "config" for the tokenizer class
//for memory optimization reason in low ram environment it includes some system-depending's stuff (to save the ram space of the tokenizer keywords)
//(...i know it's not elegant!)


//#define ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))

typedef enum {

  TOKEN_ERROR,
  TOKEN_NUMBER,
  TOKEN_STRING,
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_VAR_NAME,
  TOKEN_VARS_DECL_BEGIN,
  TOKEN_VARS_DECL_END,
  TOKEN_INT_T,
  TOKEN_STRING_T,
  TOKEN_BOOL_T,

  TOKEN_BEGIN,
  TOKEN_PRINT,
  TOKEN_PRINTLN,
  TOKEN_DELAY,
  TOKEN_GET_INT,
  TOKEN_GET_STR,
  TOKEN_IF,
  TOKEN_THEN,
  TOKEN_ELSE,
  TOKEN_ENDIF,
  TOKEN_FOR,
  TOKEN_TO,
  TOKEN_STEP,
  TOKEN_ENDFOR,
  TOKEN_GOTO,
  TOKEN_LABEL_T,
  TOKEN_RETURN,
  TOKEN_CALL,
  TOKEN_ENDOFINPUT,
  TOKEN_END,

  TOKEN_COMMA,
  TOKEN_COMMENT,
  TOKEN_COLON,
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_ASTR,
  TOKEN_SLASH,
  //TOKEN_MOD,
  //TOKEN_HASH,
  TOKEN_LEFTPAREN,
  TOKEN_RIGHTPAREN,
  TOKEN_LT,
  TOKEN_GT,
  TOKEN_EQ,
  TOKEN_NEQ,
  TOKEN_EQ_C,
  TOKEN_CR,

  TOKEN_INPUT,
  //TOKEN_INPUT_PU,
  TOKEN_OUTPUT,

} eTokenId;

typedef struct tokenStruct{

    const char* keyword;
    eTokenId tokenId;

} token;


const token keywords[] PROGMEM = {

    {"var", TOKEN_VARS_DECL_BEGIN},
    {"endvar", TOKEN_VARS_DECL_END},
    {"string", TOKEN_STRING_T},
    {"int", TOKEN_INT_T},
    {"bool", TOKEN_BOOL_T},
    {"true", TOKEN_TRUE},
    {"false", TOKEN_FALSE},
    {"begin", TOKEN_BEGIN},
    {"println", TOKEN_PRINTLN},
    {"print", TOKEN_PRINT},
    {"delay", TOKEN_DELAY},
    {"getInt", TOKEN_GET_INT},
    {"getStr", TOKEN_GET_STR},
    {"if", TOKEN_IF},
    {"then", TOKEN_THEN},
    {"else", TOKEN_ELSE},
    {"endif", TOKEN_ENDIF},
    {"for", TOKEN_FOR},
    {"to", TOKEN_TO},
    {"step", TOKEN_STEP},
    {"endfor", TOKEN_ENDFOR},
    {"goto", TOKEN_GOTO},
    {"label", TOKEN_LABEL_T},
    {"return", TOKEN_RETURN},
    {"call", TOKEN_CALL},
    {"end", TOKEN_END},
    {"\r", TOKEN_CR},
    {",", TOKEN_COMMA},
    {"//", TOKEN_COMMENT},
    {":", TOKEN_COLON},
    {"+", TOKEN_PLUS},
    {"-", TOKEN_MINUS},
    {"and", TOKEN_AND},
    {"or", TOKEN_OR},
    {"*", TOKEN_ASTR},
    {"/", TOKEN_SLASH},
    //{"%", TOKEN_MOD},
    {"(", TOKEN_LEFTPAREN},
    //{"#", TOKEN_HASH},
    {")", TOKEN_RIGHTPAREN},
    {"<", TOKEN_LT},
    {">", TOKEN_GT},
    {"==", TOKEN_EQ_C},
    {"=", TOKEN_EQ},
    {"INPUT", TOKEN_INPUT},
    {"OUTPUT", TOKEN_OUTPUT},

    {NULL, TOKEN_ERROR}
};

#endif // KEYWORDS

