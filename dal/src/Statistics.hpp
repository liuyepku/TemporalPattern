/*
  
  Context       : Matrix and Vector Operation

  Author        : Frank Hoeppner, see also AUTHORS file 

  Description   : header of class Statistics
                  
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

#ifndef Statistics_HEADER
#define Statistics_HEADER

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
//#define INLINE inline

// global types, constants, definitions


// class definition
template <class DATA>
class Statistics
  
  {
  
  public:
    
    Statistics();
    ~Statistics();
    void reset();
    void adjust(int r,int c=1);
    inline int rows() const { return m_mean.rows(); }
    inline int cols() const { return m_mean.cols(); }
    void include(const DATA&);
    void include(const Statistics&);
    inline typename DATA::value_type mean(int r,int c=0) const;
    inline typename DATA::value_type variance(int r,int c=0) const;
    inline const DATA& mean() const;
    inline const DATA& variance() const;
    void update() const;
    void write(ostream&) const;
    void read(istream&);
    
  protected:
    
    
    
  private:
    
    
    DATA m_sum;
    DATA m_squared_sum;
    int m_counter;
    // mutable elements for delayed evaluation
    mutable DATA m_mean;
    mutable DATA m_variance;
    mutable bool m_changed;
    
  };

// class related functions and definitions


/* inline implementation */


template <class DATA> inline
typename DATA::value_type 
Statistics<DATA>::mean(int r,int c) const
  { if (m_changed) update(); return m_mean(r,c); }  

template <class DATA> inline
typename DATA::value_type 
Statistics<DATA>::variance(int r,int c) const
  { if (m_changed) update(); return m_variance(r,c); }  

template <class DATA> inline
const DATA&
Statistics<DATA>::mean() const
  { if (m_changed) update(); return m_mean; }

template <class DATA> inline
const DATA&
Statistics<DATA>::variance() const
  { if (m_changed) update(); return m_variance; }
template <class DATA>
inline istream& operator>>
  (
  istream &is, 
  Statistics<DATA>& a_var
  ) 
  { 
  a_var.read(is);
  return is; 
  }
template <class DATA>
inline ostream& operator<<
  (
  ostream &os, 
  const Statistics<DATA>& a_var
  ) 
  {
  a_var.write(os);
  return os; 
  }


#endif // Statistics_HEADER
