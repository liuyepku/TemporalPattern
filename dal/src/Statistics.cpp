/*
  
  Context       : Matrix and Vector Operation

  Author        : Frank Hoeppner, see also AUTHORS file 

  Description   : implementation of class module Statistics
                  
  History       :
    Stat.nw:
     2000-May-11: first version

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

#ifndef Statistics_SOURCE
#define Statistics_SOURCE

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
#include "Statistics.hpp"

// data


// implementation
template <class DATA>
Statistics<DATA>::Statistics
  (
  )
  
  {
  reset();
  }

template <class DATA>
Statistics<DATA>::~Statistics
  (
  )
  {
  
  }


template <class DATA>
void Statistics<DATA>::reset
  (
  )
  {
  matrix_set_scalar(m_sum,0);
  matrix_set_scalar(m_squared_sum,0);
  m_counter=0;
  m_changed=true;
  }  

template <class DATA>
void Statistics<DATA>::adjust
  (
  int a_rows,
  int a_cols
  )
  {
  INVARIANT((a_rows>=0) && (a_cols>=0),"data dimension > 0");
  if ((a_rows!=m_sum.rows()) || (a_cols!=m_sum.cols()))
    {
    m_sum.adjust(a_rows,a_cols); matrix_set_scalar(m_sum,0);
    m_squared_sum.adjust(a_rows,a_cols); matrix_set_scalar(m_squared_sum,0);
    m_mean.adjust(a_rows,a_cols); matrix_set_scalar(m_mean,0);
    m_variance.adjust(a_rows,a_cols); matrix_set_scalar(m_variance,0);
    }
  }

template <class DATA>
void Statistics<DATA>::include
  (
  const DATA& a_datum
  )
  {
  // enlarge bStat if necessary
  adjust(max(rows(),a_datum.rows()),max(cols(),a_datum.cols()));

  matrix_inc(m_sum,a_datum);
  for (int i=0;i<a_datum.rows();++i)
    m_squared_sum(i) += SQR(a_datum(i));
  ++m_counter;
  m_changed = true;
  }

template <class DATA>
void Statistics<DATA>::include
  (
  const Statistics<DATA>& a_stat
  )
  {
  // enlarge bStat if necessary
  adjust(max(rows(),a_stat.rows()),max(cols(),a_stat.cols()));

  matrix_inc(m_sum,a_stat.m_sum);
  matrix_inc(m_squared_sum,a_stat.m_squared_sum);
  m_counter += a_stat.m_counter;
  m_changed = true;
  }
template <class DATA> void
Statistics<DATA>::update
  (
  )
  const
  {
  matrix_set_scaled(m_mean,1.0/m_counter,m_sum);
  if (m_counter<2)
    {
    matrix_set_scalar(m_variance,0);
    }
  else
    {
    for (int i=0;i<m_variance.rows();++i)
      m_variance(i) = sqrt(1.0/(m_counter-1) * 
        (m_squared_sum(i) - SQR(m_mean(i))*m_counter));
    }
  m_changed = false;
  }

template <class DATA>
void Statistics<DATA>::write
  (
  ostream& os
  )
  const
  {
  if (m_changed) update();
  m_mean.write(os);
  m_variance.write(os);
  }

template <class DATA>
void Statistics<DATA>::read
  (
  istream& is
  )
  {
  is >> m_mean;
  is >> m_variance;
  m_changed=false;
  }

// template instantiation



#endif // Statistics_SOURCE
