
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

/** \file Chars.hpp
    \author (c) 1999-2001 Frank Hoeppner <frank.hoeppner@ieee.org>

    \brief A simple class to encapsulate char * handling.

    We implement a very simple \c Chars template class for \c char[n]
    variables. If you need dynamic and more complicated string
    handling, use the STL \c string class. Here, we just try to
    encapsulate the various \c strcmp, \c strncpy ... C functions to
    handle character strings of type \c char*. We define the common
    operators for string concatenation, comparison, etc. Function
    names are related to the names used in the STL string class. */

#ifndef CHARS_HEADER
#define CHARS_HEADER

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

#include <iostream>
#include <stdio.h>
#include "defbool.hpp"
#include <string.h> // strlen

using namespace std;

class NoChars
  {
  public:

    /****** CONSTRUCTOR ******/

    explicit inline NoChars() {}

    /****** ACCESS ******/

    inline int max_size() const { return 0; }
    inline size_t length() const { return 0; }
    inline bool empty() const { return true; }
    inline void resize(size_t i) {}
    inline void erase() {}

    /****** ASSIGNMENT ******/
  
    inline void operator=(char c) {}
    inline void operator=(const char* p) {}
    inline void operator=(const NoChars& c) {}
    inline void operator+=(const char* p) {}
    inline void operator+=(const char c) {}
    inline void operator+=(const NoChars& S) {}
    inline void operator+=(int n) {} 
    inline void operator-=(size_t i) {}

    /****** I/O ******/

    inline void read(istream& is) {}
    inline void write(ostream& os) const {}
  };

/** \class Chars

    Substitute for standard char[] handling, used by string conversion
    routines in strconv.cpp to implememt string routines for char* and
    string. */

class Chars
  {
  public:

    /****** CONSTRUCTOR ******/

    /** Constructor takes ref to char* array and length of array. */
    explicit inline Chars(char* p,int l=0)
      : mp_text(p), m_size(l) 
      { if (mp_text==NULL) cerr << "no null pointer!" << endl; 
      if (l==0) l=strlen(p); }

    /****** ACCESS ******/

    /** Read/write access to char* array. */
    inline char* operator()() { return mp_text; }
    /** Read only access to char* array. */
    inline const char* operator()() const { return mp_text; }
    inline char& operator[](int i) { return mp_text[i]; }
    inline char operator[](int i) const { return mp_text[i]; }
    inline int max_size() const { return m_size; }
    inline size_t length() const { return strlen(mp_text); }
    inline bool empty() const { return (mp_text[0]=='\0'); }
    inline void resize(size_t i) { if (i<m_size) mp_text[i]='\0'; }
    inline void erase() { if (m_size>0) mp_text[0]='\0'; }

    /****** ASSIGNMENT ******/
  
    /** assign value of a char */
    inline void operator=(char c) 
      { mp_text[0]=c; mp_text[1]='\0'; }
    /** assign value of char* */
    inline void operator=(const char* p ) 
      { strncpy(mp_text,p,m_size-1); }
    /** assign value of another Char */
    inline void operator=(const Chars& c ) 
      { strncpy(mp_text,c.mp_text,m_size-1); }
    /** concatenate char string */
    inline void operator+=(const char* p)
      { strncat(mp_text,p,m_size-strlen(mp_text)-1); }
    /** concatenate char */
    inline void operator+=(const char c)
      { int l=strlen(mp_text); mp_text[l]=c; mp_text[l+1]='\0'; }
    /** concatenate content of another Char */
    inline void operator+=(const Chars& S)
      { strncat(mp_text,S.mp_text,m_size-strlen(mp_text)-1); }
    /** concatenate string representation of int n */
    inline void operator+=(int n)
      { char p[7]; sprintf(p,"%d",n); operator+=(p); }
    /** remove last i characters */
    inline void operator-=(size_t i)
      { if (i>strlen(mp_text)) i=0; else i=strlen(mp_text)-i; mp_text[i]='\0'; }

    /***** COMPARISON ******/

    inline bool operator<(const Chars& S) const
      { return strcmp(mp_text,S.mp_text)<0; }
    inline bool operator>(const Chars& S) const
      { return strcmp(mp_text,S.mp_text)>0; }
    inline bool operator==(const Chars& S) const
      { return strcmp(mp_text,S.mp_text)==0; }
    inline bool operator!=(const Chars& S) const
      { return strcmp(mp_text,S.mp_text)!=0; }

    inline bool operator<(const char *p_S) const
      { return strcmp(mp_text,p_S)<0; }
    inline bool operator>(const char *p_S) const
      { return strcmp(mp_text,p_S)>0; }
    inline bool operator==(const char *p_S ) const
      { return strcmp(mp_text,p_S)==0; }
    inline bool operator!=(const char *p_S ) const
      { return strcmp(mp_text,p_S)!=0; }

    /****** I/O ******/

    inline void read(istream& is)
      { is >> mp_text; }
    inline void write(ostream& os) const
      { os << mp_text; }

  private:
    char* mp_text;
    size_t m_size;
  };

inline istream& operator>>(istream &is,Chars& a_var) 
  { a_var.read(is); return is; }

inline ostream& operator<<(ostream &os,const Chars& a_var) 
  { a_var.write(os); return os; }


template <int SIZE=128>
class StatChars
  : public Chars
  {
  public:

    /****** CONSTRUCTORS ******/

    inline StatChars()
      : Chars((char*)(&m_text),SIZE) 
      { m_text[0]='\0'; }
    inline StatChars(char c) 
      : Chars((char*)(&m_text),SIZE) 
      { m_text[0]=c; m_text[1]='\0'; }
    inline StatChars(const Chars& c) 
      : Chars((char*)(&m_text),SIZE) 
      { strcpy(m_text,c.mp_text); }
    inline StatChars(const char *p_c)
      : Chars((char*)(&m_text),SIZE) 
      { if (p_c==NULL) m_text[0]='\0'; else strncpy(m_text,p_c,SIZE); }

  private:
    char m_text[SIZE];
  };

#endif /* CHARS_HEADER */



