
/*  utility package -- Copyright (C) 1999-2001 Frank Hoeppner

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

/** \file strconv.cpp
    \author (c) 1999-2001 Frank Hoeppner <frank.hoeppner@ieee.org>

    \brief Implementation of string conversion routines. 
*/

#ifndef STRCONVERT_SOURCE
#define STRCONVERT_SOURCE

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

#include "strconv.hpp"
#include <ctype.h> // isspace, tolower, toupper
#include <iostream>
#include "defbool.hpp" // bool

/** Lower-case conversion for \c Chars and \c string. */

template <class S>
void turn_to_lower
  (
  S& text
  )
  {
  if (text.empty()) return;
  size_t len(text.length());

  for (size_t i=0;i<len;++i)
    { text[i] = tolower(text[i]); }
  }

template void turn_to_lower(string&);
template void turn_to_lower(Chars&);
void turn_to_lower(char* p) { Chars c(p); turn_to_lower(c); }

/** Upper-case conversion for \c Chars and \c string. */

template <class S>
void turn_to_upper
  (
  S& text
  )
  {
  if (text.empty()) return;
  size_t len(text.length());

  for (size_t i=0;i<len;++i)
    { text[i] = toupper(text[i]); }
  }

template void turn_to_upper(string&);
template void turn_to_upper(Chars&);
void turn_to_upper(char* p) { Chars c(p); turn_to_upper(c); }

/** Translates characters of string into LaTeX commands. */

template <class S> 
void convert_to_tex
  (
  const S& a_text, 
  S& a_tex
  )
  {
  a_tex.erase();
  size_t len(a_text.length());
  for (size_t i=0;i<len;++i)
    {

    switch (a_text[i])
      {
      case '>' : a_tex += "$>$"; break;
      case '<' : a_tex += "$<$"; break;
      case 'ä' : a_tex += "\\\"a"; break;
      case 'Ä' : a_tex += "\\\"A"; break;
      case 'ü' : a_tex += "\\\"u"; break;
      case 'Ü' : a_tex += "\\\"U"; break;
      case 'ö' : a_tex += "\\\"o"; break;
      case 'Ö' : a_tex += "\\\"O"; break;
      case 'ß' : a_tex += "\\3"; break;
      case '_' : a_tex += "\\_"; break;
      default  : a_tex += a_text[i]; break;
      }

    }
  }

template void convert_to_tex(const string&,string&);
template void convert_to_tex(const Chars&,Chars&);
void convert_to_tex(char const* p,char* q,size_t l)
  { const Chars cp(const_cast<char*>(p)); Chars cq(q,l); 
    convert_to_tex(cp,cq); }

/** Translates characters of string into HTML characters. */

template <class S>
void convert_to_html
  ( 
  const S& a_text, 
  S& a_html
  )
  {
  a_html.erase();
  size_t len(a_text.length());
  for (size_t i=0;i<len;++i)
    {

    switch (a_text[i])
      {
      case '\"' : a_html += "&quot"; break;
      case '\n' : a_html += "<br>"; break;
      case '<'  : a_html += "&lt;"; break;
      case '>'  : a_html += "&gt;"; break;
      case '&'  : a_html += "&amp;"; break;
      case 'ä'  : a_html += "&auml;"; break;
      case 'Ä'  : a_html += "&Auml;"; break;
      case 'ü'  : a_html += "&uuml;"; break;
      case 'Ü'  : a_html += "&Uuml;"; break;
      case 'ö'  : a_html += "&ouml;"; break;
      case 'Ö'  : a_html += "&Ouml;"; break;
      case 'ß'  : a_html += "&szlig;"; break;
      default   : a_html += a_text[i]; break;
      }

    }
  }

template void convert_to_html(const string&,string&);
template void convert_to_html(const Chars&,Chars&);
void convert_to_html(char const* p,char* q,size_t l)
  { const Chars cp(const_cast<char*>(p),strlen(p)); Chars cq(q,l); 
    convert_to_html(cp,cq); }

/** Convert a return string from a post command to ASCII. */

template <class S>
void convert_from_post
  (
  const S& a_post,
  S& a_text
  )
  {
  int k; 
  size_t i=0;
  char c;
  a_text.erase();
  while (i<a_post.length())
    {
    switch (a_post[i])
      {
      case '+' : a_text += ' '; break;
      case '%' : // hex number to ascii
                 k=0;
                 ++i; c=a_post[i];
                 if (c>='A' && c<='F') k += 16*(10+c-'A'); else
                 if (c>='a' && c<='f') k += 16*(10+c-'a'); else
                 if (c>='0' && c<='9') k += 16*(c-'0');
                 ++i; c=a_post[i];
                 if (c>='A' && c<='F') k += (10+c-'A'); else
                 if (c>='a' && c<='f') k += (10+c-'a'); else
                 if (c>='0' && c<='9') k += (c-'0');
                 a_text += ((char)k);
                 break;
      default  : a_text += a_post[i]; break;
      }
    ++i;
    }
  }

template void convert_from_post(const string&,string&);
template void convert_from_post(const Chars&,Chars&);
void convert_from_post(char const* p,char* q,size_t l)
  { const Chars cp(const_cast<char*>(p),strlen(p)); Chars cq(q,l); 
    convert_from_post(cp,cq); }

/** Remove leading blanks etc. */

template <class S>
void delete_superfluous_blanks
  (
  S& text
  )
  {
  if (text.empty()) return;
  size_t len(text.length());

  size_t i(0),j(0);
  bool blank(false);
  for (j=0;j<len;++j)
    {
    if (!isspace(text[j]))
      { 
      if ((i>0) && (blank)) { text[i] = ' '; ++i; }
      text[i] = text[j]; ++i; 
      }
    blank = itob(isspace(text[j]));
    }

  text.resize(i);
  }

template void delete_superfluous_blanks(string&);
template void delete_superfluous_blanks(Chars&);
void delete_superfluous_blanks(char* p)
  { Chars cp(p,strlen(p)); return delete_superfluous_blanks(cp); }

/** Remove or replace characters that are not allowed in filenames. */ 

template <class S>
void convert_to_robust_name
  (
  S& text
  )
  {
  if (text.empty()) return;
  size_t len(text.length());

  for (size_t i=0;i<len;++i)
    {
    switch (text[i])
      {
      case ' ' :
      case '-' : text[i]='_'; break;
      }
    }
  }

template void convert_to_robust_name(string&);
template void convert_to_robust_name(Chars&);
void convert_to_robust_name(char* p)
  { Chars cp(p,strlen(p)); convert_to_robust_name(cp); }

#endif /* STRCONVERT_SOURCE */
