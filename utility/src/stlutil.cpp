
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

/** \file stlutil.hpp
    \author (c) 1999-2001 Frank Hoeppner <frank.hoeppner@ieee.org>

    \brief Implementation of string_less. 
*/

#ifndef STLUTIL_SOURCE
#define STLUTIL_SOURCE

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

#include "stlutil.hpp"


/** The comparison yields \c true, if the first comes
    lexicographically before the second (\c strcmp returns \c -1
    ). The function handles the special pointers \c NULL and \c 0xFFFF
    = \c ~0 as top and bottom elements, that is: a \c NULL string
    comes before any other string, a \c ~0 string after any other
    string. */

bool 
string_less::operator() 
  (
  char_cc x,
  char_cc y
  ) 
  const
  {
  // 0 is less than any other pointer but 0
  if (x==0)
    {
    return (y!=0);
    }
  else if (y==0)
    {
    return false;
    }
  // ~0 is greater (!less) than any other pointer but ~0
  else if (x==(char*) ~0)
    {
    return false;
    }
  else if (y==(char*) ~0)
    {
    return (x!=(char*) ~0);
    }
  else 
    {
    return (strcmp(x,y)<0);
    }
  }

#endif /* STLUTIL_SOURCE */
