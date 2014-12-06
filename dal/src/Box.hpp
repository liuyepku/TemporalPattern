/*
  
  Context       : Matrix and Vector Operation

  Author        : Frank Hoeppner, see also AUTHORS file 

  Description   : header of class Box
                  
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

#ifndef Box_HEADER
#define Box_HEADER

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



// necessary includes
#include <fstream.h>
#include "define.hpp"
//#define INLINE inline

// global types, constants, definitions


// class definition
template <class DATA>
class Box
  
  {
  
  public:
    
    Box();
    ~Box();
    Box(const Box&);
    Box(const DATA&,const DATA&);
    void reset();
    void adjust(int r,int c=1);
    inline int rows() const { return lowerBound.rows(); }
    inline int cols() const { return lowerBound.cols(); }
    void include(const DATA&);
    void include(const Box&);
    void enlarge_1_to_1();
    void enlarge_percent(TData);
    DATA random();
    inline typename DATA::value_type minimum(int r,int c=0) const;
    inline typename DATA::value_type maximum(int r,int c=0) const;
    inline typename DATA::value_type mean(int r,int c=0) const;
    inline typename DATA::value_type length(int r,int c=0) const;
    bool encloses(const DATA&);
    bool encloses(const Box&);
    void write(ostream&) const;
    void read(istream&);
    DATA upperBound;
    DATA lowerBound;
  protected:
    
    
    
  private:
    
    
    
    
  };

// class related functions and definitions


/* inline implementation */


template <class DATA> inline
typename DATA::value_type 
Box<DATA>::minimum(int r,int c) const
  { return lowerBound(r,c); }  

template <class DATA> inline
typename DATA::value_type 
Box<DATA>::maximum(int r,int c) const
  { return upperBound(r,c); }  

template <class DATA> inline
typename DATA::value_type 
Box<DATA>::mean(int r,int c) const
  { return (lowerBound(r,c)+upperBound(r,c))*0.5; }  

template <class DATA> inline
typename DATA::value_type 
Box<DATA>::length(int r,int c) const 
  { return upperBound(r,c)-lowerBound(r,c); }  
template <class DATA>
inline istream& operator>>
  (
  istream &is, 
  Box<DATA>& a_var
  ) 
  { 
  a_var.read(is);
  return is; 
  }
template <class DATA>
inline ostream& operator<<
  (
  ostream &os, 
  const Box<DATA>& a_var
  ) 
  {
  a_var.write(os);
  return os; 
  }


#endif // Box_HEADER
