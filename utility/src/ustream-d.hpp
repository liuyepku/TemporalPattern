/*
  
  Context       : Basic Utility Files

  Author        : Frank Hoeppner, see also AUTHORS file 

  Description   : header of function module ustream-d
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

#ifndef UtilStream_Drop_HEADER
#define UtilStream_Drop_HEADER

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
//#define INLINE inline

/* global types and constants */
struct keyword_entry
  {
  char *name;
  int identifier;
  };
#define SYNTAX_ERROR_EXPECTED(x) \
  {\
  cerr << "syntax error: ´" << x << "´ expected" << endl;\
  exit(-1);\
  }

/* data interface */


/* UtilStream interface */
int read_white(istream& );
inline int read_word(istream& );
int read_word_break(istream&,const char * );
bool read_until(istream&,const char * );
inline bool read_line(istream& );
int read_until_eof(istream& );
bool is_followed_by(istream&,const char*,bool put_back_if_found=true,
                    bool capital_letters=false,bool skip_white=true);
int is_followed_by(istream&,const keyword_entry*,bool put_back_if_found=true,
                   bool capital_letters=true,bool skip_white=true,int default_value=-1);
bool is_followed_by(istream&,char,bool put_back_if_found=true,
                   bool capital_letters=true);
char* get_keyword(const keyword_entry*,int);
void read_entry(istream& );
int read_lines_starting_with(istream&,const char* );
int read_until_matching_paranthesis(istream&,char,char );
template <class DATA> inline bool read_data(istream&,char_cc,DATA&);
bool read_data(istream&,char_cc );

/* inline implementation */

inline 
int 
read_word
  (
  istream& is
  
  )
  { 
  return read_word_break(is,"" ); 
  }
inline 
bool
read_line
  (
  istream& is
  
  )
  {
  return read_until(is,"\n" );
  }
template <class DATA> 
inline bool
read_data
  (
  istream& a_input_stream,
  char_cc ap_keyword,
  DATA& a_data
  )
  {
  a_input_stream.clear();
  a_input_stream.seekg(ios::beg);
  bool done;

  do
    {
    done = (!read_until(a_input_stream,"{"));
    if (!done)
      {
      if (is_followed_by(a_input_stream,ap_keyword,false))
        {
        a_input_stream >> a_data;
        return true;
        }
      else
        {
        read_until_matching_paranthesis(a_input_stream,'{','}');
        }
      done = itob(a_input_stream.eof());  
      }
    }
  while (!done);    
  return false;
  }



#endif /* UtilStream_Drop_HEADER */
