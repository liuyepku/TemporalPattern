
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

/** \file ttime.hpp
    \author (c) 1999-2001 Frank Hoeppner <frank.hoeppner@ieee.org>

    \brief A couple of C functions provide floating point time representation.

    The point in time with TTime representation 0.0 is set to the
    point in time when the object code is initialized. The floating
    point representation corresponds to the human's imagination of
    time. Furthermore, all operators to add, subtract times etc. are
    given naturally. */

#ifndef TTIME_HEADER
#define TTIME_HEADER

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

#include "define.hpp" // types/constants

typedef float TTime;
#define POS_IMPOSSIBLE_TIME ((TTime)1.0E10)
#define NEG_IMPOSSIBLE_TIME ((TTime)-1.0E10)
#define ZERO_TIME ((TTime)0.0)
#define ONE_SECOND ((TTime)1.0)
#define TEN_SECONDS ((TTime)10.0)
#define HUNDRED_SECONDS ((TTime)100.0)

void set_zero_time();
TTime actual_time();
TTime passed_time_since(TTime);
char* decode_time(TTime);
TTime set_1970_time(const long,const long);
void get_1970_time(const TTime&,long&,long&);
void set_zero_time(const long);

#endif /* TTIME_HEADER */
