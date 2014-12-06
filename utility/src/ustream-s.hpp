/*
  
  Context       : Basic Utility Files

  Author        : Frank Hoeppner, see also AUTHORS file 

  Description   : header of function module ustream-s
                  Source Documentation Handbook: tutorial-UTIL
  History       : see source file

  Comment       : 
    This file was generated automatically. DO NOT EDIT.

  Copyright     : Copyright (C) 1999-2000 Frank Hoeppner

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef UtilStream_String_HEADER
#define UtilStream_String_HEADER

/* configuration include */
#ifdef HAVE_CONFIG_H
/*
//FILETREE_IFDEF HAVE_CONFIG_H
*/
#include "config.h"
/*
//FILETREE_ENDIF
*/
#endif



/* necessary includes */
#include <iostream.h>
#include <ctype.h> // isspace, tolower, toupper
#include <string.h> // strlen
#include "defbool.hpp" // bool
#include "define.hpp" // char_cc
#include <string> // stl string
//#define INLINE inline

/* global types and constants */



/* data interface */


/* UtilStream interface */
int read_white(istream& , string& a_string);
inline int read_word(istream& , string& a_string);
int read_word_break(istream&,const char * , string& a_string);
bool read_until(istream&,const char * , string& a_string);
inline bool read_line(istream& , string& a_string);
int read_until_eof(istream& , string& a_string);
void read_entry(istream& , string& a_string);
int read_lines_starting_with(istream&,const char* , string& a_string);
int read_until_matching_paranthesis(istream&,char,char , string& a_string);
bool read_data(istream&,char_cc , string& a_string);

/* inline implementation */

inline 
int 
read_word
  (
  istream& is
  , string& a_string
  )
  { 
  return read_word_break(is,"" , a_string); 
  }
inline 
bool
read_line
  (
  istream& is
  , string& a_string
  )
  {
  return read_until(is,"\n" , a_string);
  }



#endif /* UtilStream_String_HEADER */
