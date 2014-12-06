/*
  
  Context       : Matrix and Vector Operation

  Author        : Frank Hoeppner, see also AUTHORS file 

  Description   : implementation of class module Box
                  
  History       :
    box.nw:
     980722 fh: first version 
     980909 fh: implemented inline functions minimum, maximum, mean

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

#ifndef Box_SOURCE
#define Box_SOURCE

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
#include "Box.hpp"

// data


// implementation
template <class DATA>
Box<DATA>::Box
  (
  )
  
  {
  reset();
  }

template <class DATA>
Box<DATA>::~Box
  (
  )
  {
  
  }


template <class DATA>
Box<DATA>::Box
  (
  const Box<DATA>& a_box
  )
  : upperBound(a_box.upperBound)
  , lowerBound(a_box.lowerBound)
  {
  }

template <class DATA>
Box<DATA>::Box
  (
  const DATA& ar_LowerBound,
  const DATA& ar_UpperBound
  )
  : upperBound(ar_UpperBound)
  , lowerBound(ar_LowerBound)
  {
  }

template <class DATA>
void Box<DATA>::reset
  (
  )
  {
  for (int r=0;r<lowerBound.rows();++r)
    {
    for (int c=0;c<lowerBound.cols();++c)
      {
      lowerBound(r,c) = Constants<typename DATA::value_type>().max();
      upperBound(r,c) = Constants<typename DATA::value_type>().min();
      } 
    } // for r  
  }  

template <class DATA>
void Box<DATA>::adjust
  (
  int a_rows,
  int a_cols
  )
  {
  INVARIANT((a_rows>=0) && (a_cols>=0),"non negative bbox size");

  int old_rows(lowerBound.rows());
  int old_cols(lowerBound.cols());
  if ((old_rows==a_rows) && (old_cols==a_cols)) return;

  lowerBound.adjust(a_rows,a_cols);
  upperBound.adjust(a_rows,a_cols);
  // if size grows, set constants
  for (int r=0;r<lowerBound.rows();++r)
    {
    for (int c=0;c<lowerBound.cols();++c)
      {
      if ((r>=old_rows) || (c>=old_cols))
        { 
        lowerBound(r,c) = Constants<typename DATA::value_type>().max();
        upperBound(r,c) = Constants<typename DATA::value_type>().min();
        }
      } 
    } // for r  
  }  

template <class DATA>
void Box<DATA>::include
  (
  const DATA& ar_DataVector
  )
  {
  // enlarge bbox if necessary
  adjust(max(rows(),ar_DataVector.rows()),max(cols(),ar_DataVector.cols()));

  typename DATA::value_type value;
  for (int r=0;r<ar_DataVector.rows();++r)
    {
    for (int c=0;c<ar_DataVector.cols();++c)
      {
      value = ar_DataVector(r,c);
      if (lowerBound(r,c) > value) { lowerBound(r,c) = value; }
      if (upperBound(r,c) < value) { upperBound(r,c) = value; }
      } // for c
    } // for r
  }

template <class DATA>
void Box<DATA>::include
  (
  const Box<DATA>& ar_Box
  )
  {
  include(ar_Box.lowerBound);
  include(ar_Box.upperBound);
  }

template <class DATA>
void Box<DATA>::enlarge_1_to_1
  (
  )
  {
  typename DATA::value_type l(0);
  int r;
  for (r=0;r<lowerBound.rows();++r)
    {
    l = max(l,length(r));
    }
  for (r=0;r<lowerBound.rows();++r)
    {
    lowerBound(r) -= (l-length(r))/2.0;
    upperBound(r) += (l-length(r))/2.0;
    }
  }

template <class DATA>
void Box<DATA>::enlarge_percent
  (
  TData a_percent
  )
  {
  int r;
  for (r=0;r<lowerBound.rows();++r)
    {
    lowerBound(r) -= length(r)*a_percent/100.0;
    upperBound(r) += length(r)*a_percent/100.0;
    }
  }

template <class DATA>
DATA Box<DATA>::random
  (
  )
  {
  DATA point; point.adjust(lowerBound.rows());
  int r;
  for (r=0;r<lowerBound.rows();++r)
    {
    point(r) = lowerBound(r) + (::random()%10000)/10000.0*length(r);
    }
  return point;
  }

template <class DATA>
bool Box<DATA>::encloses
  (
  const DATA& ar_DataVector
  )
  {
  bool inside = true;
  for (int r=0;r<ar_DataVector.rows();++r)
    {
    for (int c=0;c<ar_DataVector.cols();++c)
      {
      inside &= (lowerBound(r,c) <= ar_DataVector(r,c));
      inside &= (upperBound(r,c) >= ar_DataVector(r,c));
      }
    } // for r  
  return inside;         
  }

template <class DATA>
bool Box<DATA>::encloses
  (
  const Box<DATA>& ar_Box
  )
  {
  return (encloses(ar_Box.upperBound) && encloses(ar_Box.lowerBound));
  }

template <class DATA>
void Box<DATA>::write
  (
  ostream& os
  )
  const
  {
  lowerBound.write(os);
  upperBound.write(os);
  }

template <class DATA>
void Box<DATA>::read
  (
  istream& is
  )
  {
  is >> lowerBound;
  is >> upperBound;
  }

// template instantiation



#endif // Box_SOURCE
