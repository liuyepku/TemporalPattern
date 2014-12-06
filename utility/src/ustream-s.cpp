/*
  
  Context       : Basic Utility Files

  Author        : Frank Hoeppner, see also AUTHORS file 

  Description   : implementation of function module ustream-s
                  Source Documentation Handbook: tutorial-UTIL
  History       :
    

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

#ifndef UtilStream_String_SOURCE
#define UtilStream_String_SOURCE

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
#include "ustream-s.hpp"
#include "ustream-d.hpp"

/* private typedefs */


/* private functions */


/* data */


/* implementation */
int
read_white
  (
  istream& is
  , string& a_string
  )
  {
  char c;
  const int skipchars = 1;
  //const bool storechars(true); // unused
  a_string = "";
  size_t stringlen(0); // no. of characters read

  if (is.good() && !is.eof())
    {
    do
      {
      is.get(c);
      a_string += c;
      ++stringlen;
      }
    while ( (is.good()) && (!is.eof()) && (isspace(c)) );
    is.putback(c);
    }      

  a_string.resize(stringlen-skipchars);
  return (stringlen-skipchars);
  }
int
read_word_break
  (
  istream& is,
  const char * p_delimiter
  , string& a_string
  )
  {
  char c;
  int i;
  bool cont;
  bool nonempty((p_delimiter!=NULL) && (p_delimiter[0]!='\0'));
  bool notfirst(false);
  const int skipchars = 1;
  //const bool storechars(true); // unused
  a_string = "";
  size_t stringlen(0); // no. of characters read

  read_white(is);
  if (is.good())
    {
    do
      {
      is.get(c);
      cont = !isspace(c);
      if (cont && nonempty && notfirst) 
        { 
        i=0; 
        while (p_delimiter[i]!='\0') 
          { 
          cont &= (c!=p_delimiter[i]); 
          ++i; 
          }
        }
      notfirst=true;
      a_string += c;
      ++stringlen;
      }
    while (    (is.good())
            && (cont)
          );
    is.putback(c);
    }
    
  a_string.resize(stringlen-skipchars);
  return (stringlen-skipchars);
  }
bool
read_until
  (
  istream& is,
  const char * p_find
  , string& a_string
  )
  {
  int matching = 0;
  const int skipchars = strlen(p_find);
  //const bool storechars(true); // unused
  a_string = "";
  size_t stringlen(0); // no. of characters read

  if (    (p_find    != NULL)
       && (p_find[0] != '\0')
       && (is.good())
     )
    {
    char c;

    do
      {
      is.get(c);
      if (c==p_find[matching])
        {
        ++matching;
        }
      else
        {
        matching = 0;
        }  
      a_string += c;
      ++stringlen;
      }
    while (    (matching<skipchars)
            && (is.good())
          );

    a_string.resize(stringlen-skipchars);
    }

  return ((matching==skipchars) && (is.good() || is.eof()));
  }
int
read_until_eof
  (
  istream& is
  , string& a_string  
  )
  {
  char c;
  //const bool storechars(true); // unused
  a_string = "";
  size_t stringlen(0); // no. of characters read

  if (is.good())
    {
    do
      {
      is.get(c);
      a_string += c;
      ++stringlen;
      }
    while (!is.eof());
    }

  return stringlen;
  }
void 
read_entry
  (
  istream& is
  , string& a_string
  )
  {
  read_white(is);
  if (is_followed_by(is,"{",false))
    {
    read_until_matching_paranthesis(is,'{','}' , a_string);
    }
  else if (is_followed_by(is,"\"",false))
    {
    read_until(is,"\"" , a_string);
    }
  else
    {
    read_word_break(is,"," , a_string);
    }
  }
int
read_lines_starting_with
  (
  istream& is, 
  const char* p_find
  , string& a_string
  )
  {
  bool found;
  size_t storelen = 0;
  
  do
    {
    
    read_white(is , a_string);
    found = is_followed_by(is,p_find);
    if (found) 
      { 
      storelen += read_line(is , a_string);
      }
    }
  while (found);

  return storelen;
  }
int 
read_until_matching_paranthesis
  (
  istream& is, 
  char ip,
  char dp
  , string& a_string
  )
  {
  char c;
  const int skipchars = 1;
  //const bool storechars(true); // unused
  a_string = "";
  size_t stringlen(0); // no. of characters read
  int counter = 1;
  //invariant(ip!=dp,"cannot distinguish openening paranthesis from closing paranthesis!",SOURCELOC);

  while (    (counter!=0) 
          && (is.good()) 
        )
    {
    is.get(c);
    if ( c == ip )
      {
      ++counter;
      }
    else
      {
      if ( c == dp )
        {
        --counter;
        }
      }
    a_string += c;
    ++stringlen;
    }

  a_string.resize(stringlen-skipchars);
  return (stringlen-skipchars);
  }
bool
read_data
  (
  istream& a_input_stream,
  char_cc ap_keyword
  , string& a_string
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
        read_white(a_input_stream);
        read_until_matching_paranthesis(a_input_stream,'{','}' , a_string);
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

/* template instantiation */
#ifndef NO_EXPLICIT_TEMPLATE_INSTANTIATION
  template class basic_string<char>;
#endif


#endif /* UtilStream_String_SOURCE */
