
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

/** \file ttime.cpp
    \author (c) 1999-2001 Frank Hoeppner <frank.hoeppner@ieee.org>

    \brief A couple of C functions providing floating point time.
*/

#ifndef TTIME_SOURCE
#define TTIME_SOURCE

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

#include "ttime.hpp"
#include <math.h>

/** To get the current time we need to call system functions. Two
    system are supported: Windows 95/NT (tested under Borland C++
    5.02) and UNIX (tested under SGI IRIX, Linux). Depending on the
    system we use different system functions and internal time
    representations. On UNIX systems the configure script
    automatically determines which time representation (timeb or
    timeval) can be used. On Windows systems we simply check for
    __BORLANDC__ and _MSC_VER macros (and use timeb). */

#ifdef TTIME_USE_TIMEB
#  include <sys/timeb.h>
#  include <time.h>
#elif TTIME_USE_TIMEVAL
#  include <sys/time.h>
#  include <fstream.h>
#elif defined(__BORLANDC__) || defined(_MSC_VER)
#  include <sys\timeb.h>
#  include <time.h>
#  define TTIME_USE_TIMEB
#else
#  error "No system specific code provided."
#endif

// struct timeb
//   {
//   long  time;
//   short millitm;
//   short timezone;
//   short dstflag;
//   };

// struct timeval
//   {
//   long tv_sec;        /* seconds since Jan. 1, 1970 */
//   long tv_usec;       /* and microseconds */
//   };


#if defined(TTIME_USE_TIMEB)

struct timeb init_zero_time(const long a_sec)
  {
  struct timeb systemtime;
  if (a_sec==0)
    {
    ftime(&systemtime);
    }
  else
    {
    systemtime.time = a_sec;
    systemtime.millitm = 0;
    }
  return systemtime;
  }

# elif defined(TTIME_USE_TIMEVAL)

struct timeval init_zero_time(const long a_sec)
  {
  struct timeval systemtime;
  struct timezone systemzone;
  if (a_sec==0)
    {
    gettimeofday(&systemtime,&systemzone);
    }
  else
    {
    systemtime.tv_sec = a_sec;
    }
  systemtime.tv_usec=0;
  return systemtime;
  }

#else
#  error "No system specific code provided."
#endif

/** The global \c g_zerotime variable stores the point in time (system
    dependent representation) which is associated with (float)0.0. */

#if defined(TTIME_USE_TIMEB)
  struct timeb g_zerotime = init_zero_time(0);
#elif defined(TTIME_USE_TIMEVAL)
  struct timeval g_zerotime = init_zero_time(0);
#else
# error "No system specific code provided."
#endif

/** Determine zero time.  

    The function \c set_zero_time moves the zero point in time to the
    actual point in time. */

void set_zero_time() 
  { 
  g_zerotime = init_zero_time(0);
  }

/** Return current time -- relative to zero time. */

TTime actual_time()
  {
  TTime floattime;
# if defined(TTIME_USE_TIMEB)
    struct timeb systemtime;
    ftime(&systemtime); 
    floattime =
                (systemtime.time-g_zerotime.time)
      + MILLI * (systemtime.millitm-g_zerotime.millitm);
# elif defined(TTIME_USE_TIMEVAL)
    struct timeval systemtime;
    struct timezone systemzone;
    gettimeofday(&systemtime,&systemzone);
    floattime = 
                (systemtime.tv_sec-g_zerotime.tv_sec) 
      + MICRO * (systemtime.tv_usec);
# else
#   error "No system specific code provided."
# endif
  return floattime;
  }

/** Return time passed since \c a_floattime. */

TTime passed_time_since
  (
  TTime a_floattime
  )
  {
  TTime now( actual_time() );
  return (now-a_floattime);
  }

/** Return a pointer to a string representation of \c a_floattime. 

    The \c decode_time function converts the time \c a_floattime into
    a character string that contains the global time (not the floating
    point number). Since the system time representation is involved, we
    need again some system dependent instructions. */

char* decode_time
  (
  TTime a_floattime
  )
  {
# if defined(TTIME_USE_TIMEB)
    long l = (long int)(a_floattime) + g_zerotime.time; 
    return asctime( gmtime( &l ) );
# elif defined(TTIME_USE_TIMEVAL)
    time_t l = (time_t)(a_floattime) + (time_t)(g_zerotime.tv_sec);
    return asctime( gmtime( &l ) );
# else
#   error "No system specific code provided."
# endif
  }

/** Convert from 1970 time.

    Converts a point in time (specified via secs and microsecs from
    Jan, 1st, 1970) in the floating point time representation. */

TTime set_1970_time
  (
  const long a_sec,
  const long a_usec
  )
  {
  TTime floattime;
# if defined(TTIME_USE_TIMEB)
    floattime = 
                (a_sec-g_zerotime.time)
      + MICRO * (a_usec-g_zerotime.millitm*KILO);
# elif defined(TTIME_USE_TIMEVAL)
    floattime = 
                 (a_sec-g_zerotime.tv_sec) 
       + MICRO * a_usec; 
# else
#   error "No system specific code provided."
# endif
  return floattime;
  }

/** Convert to 1970 time.

    Converts a floating point time into seconds and microseconds
    passed since Jan, 1st, 1970. */

void get_1970_time
  (
  const TTime& a_floattime,
  long &ar_sec,
  long &ar_usec
  )
  {
# if defined(TTIME_USE_TIMEB)
    long secs = (long)(a_floattime);
    ar_sec  = g_zerotime.time+secs; 
    ar_usec = (long)(g_zerotime.millitm+MEGA*(a_floattime-secs)); 
    if ((secs<0) && (ar_sec>0) && (ar_usec<0))
      {
      --ar_sec;
      ar_usec += (long)MEGA;
      }
# elif defined(TTIME_USE_TIMEVAL)
    long secs = (long)(a_floattime); 
    ar_sec  = g_zerotime.tv_sec+secs; 
    ar_usec = (long)(MEGA*(a_floattime-secs)); 
    if ((secs<0) && (ar_sec>0) && (ar_usec<0))
      {
      --ar_sec;
      ar_usec += (long)MEGA;
      }
# else
#   error "No system specific code provided."
# endif
  }

/** Set the zero floating-point time to the time \c a_sec seconds
    after Jan, 1st, 1970. */

void set_zero_time
  (
  const long a_sec
  )
  {
  g_zerotime = init_zero_time(a_sec);
  }

#endif /* TTIME_SOURCE */
