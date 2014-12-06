
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

/** \file define.hpp
    \author (c) 1999-2001 Frank Hoeppner <frank.hoeppner@ieee.org>

    \brief Types and constants.

    This file contains several type and constant definitions. Most of
    the (numerical) constants are given by means of the preprocessor
    directive \c #define instead of a \c const \c float declaration,
    because this might cause problems when initializing constants
    using function calls in shared libraries\footnote{For instance,
    const float SQRT2 ( sqrt(2.0) ); may cause problems when defined
    in a shared library because it is not clear when the
    initialization is executed. It may happen that other code sections
    address {\tt SQRT2} before it is initialized.}. Furthermore, most
    compiler evaluate a product of several \c const \c float at
    run-time, whereas a product of defined constants is evaluated at
    compile-time.

*/

#ifndef CONSTANTS_HEADER
#define CONSTANTS_HEADER

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

#include <limits.h>
#include <float.h> // defines FLT_MAX
#include <math.h>
#include "defbool.hpp"

/** Types 

    To make the use of [[[const] char [const] * [const]]] more
    intuitive, we introduce typedefs for {\sl constant pointer
    [[cp]]}, {\sl constant characters [[cc]]}, and both {\sl constant
    pointer and characters [[cpc]]}. */

typedef double TData;
typedef float TCost;
typedef float TRange;
typedef float TLambda;
typedef unsigned int word;
typedef char * const char_cp;
typedef char const * char_cc; // const char * == char const *
typedef const char * const char_cpc;

/** Mathematical Constants */

/** natural logarithm of 2 */
#define LOG2  ((double) 0.69314718056)
/** square root of 2 */
#define SQRT2 ((double) 1.414213562373)
/** square root of 3 */
#define SQRT3 ((double) 1.7320508076)
/** square root of 5 */
#define SQRT5 ((double) 2.2360679775)

#define HALF ((double) 0.5)
#define ONE  ((double) 1.0)
#define TWO  ((double) 2.0)

#define DEKA ((int) 10)
#define HEKTO ((int) 100)
#define KILO ((int) 1000)
#define KILOBYTE ((size_t) 1024)
#define MEGA ((double) 1.0E6)
#define MEGABYTE ((double) 1048576)
#define GIGA ((double) 1.0E9)
#define TERA ((double) 1.0E12)

#define DEZI ((double) 0.1)
#define CENTI ((double) 0.01)
#define MILLI ((double) 0.001)
#define MICRO ((double) 1.0E-6)
#define NANO ((double) 1.0E-9)
#define PIKO ((double) 1.0E-12)
#define FEMTO ((double) 1.0E-15)

/** Temporal Constants */

/** minutes in one hour */
#define MIN_PER_HOUR (60)
/** seconds in one hour */
#define SEC_PER_MIN (60)
/** conversion factor hour to seconds */
#define HOUR_TO_SEC (MIN_PER_HOUR*SEC_PER_MIN)
/** conversion factor seconds to hours */
#define SEC_TO_HOUR (1.0/HOUR_TO_SEC)

/** Linear Measures & Conversions */

/** one nautical mile in metre (Gieck, A5) */
#define ONE_NAUTICAL_MILE ((double) 1852.0)
/** conversion factor for nautical miles to metres */
#define NM_TO_M (ONE_NAUTICAL_MILE)
/** conversion factor for metres to nautical miles */
#define M_TO_NM (1.0/ONE_NAUTICAL_MILE)
/** length of one feet in metre (Gieck, A4) */
#define ONE_FEET ((double) 0.3048)
/** conversion factor for feet to metre */
#define FEET_TO_METRE ((double) ONE_FEET)
/** conversion factor for metre to feet */
#define METRE_TO_FEET (1.0/FEET_TO_METRE)
/** conversion factor for (nautical) knots to miles per second */
#define KTS_TO_MPS (ONE_NAUTICAL_MILE/HOUR_TO_SEC) 
/** conversion factor for miles per second to knots */
#define MPS_TO_KTS (1.0/KTS_TO_MPS) 

/** Physical Constants */

/** gravitational acceleration, m / s^2 */
#define GRAVITATIONAL_ACCELERATION ((double) 9.8066)
/** universal gas constant for air m^2 / K s^2 */
#define UNIVERSAL_GAS_CONSTANT_AIR ((double) 287.04)
/** isentropic expansion air (BADA (3.2-17)) */
#define ISENTROPIC_EXPANSION_AIR ((double) 1.4)

/** ASCII Constants */

#define BEEP_CHAR char(7)
#define ESCAPE_ASCII 27
#define BLANK_ASCII 32
#define PLUS_ASCII 43
#define COLON_ASCII 44
#define MINUS_ASCII 45
#define POINT_ASCII 46
#define LOWER_A_ASCII 97
#define LOWER_B_ASCII 98
#define LOWER_C_ASCII 99
#define LOWER_D_ASCII 100
#define LOWER_E_ASCII 101
#define LOWER_F_ASCII 102
#define LOWER_G_ASCII 103
#define LOWER_H_ASCII 104
#define LOWER_I_ASCII 105
#define LOWER_J_ASCII 106
#define LOWER_K_ASCII 107
#define LOWER_L_ASCII 108
#define LOWER_M_ASCII 109
#define LOWER_N_ASCII 110
#define LOWER_O_ASCII 111
#define LOWER_P_ASCII 112
#define LOWER_Q_ASCII 113
#define LOWER_R_ASCII 114
#define LOWER_S_ASCII 115
#define LOWER_T_ASCII 116
#define LOWER_U_ASCII 117
#define LOWER_V_ASCII 118
#define LOWER_W_ASCII 119
#define LOWER_X_ASCII 120
#define LOWER_Y_ASCII 121
#define LOWER_Z_ASCII 122

/** Inline & Templates */

/** logarithm to base 2 */
template <class T> inline T log2(T x) { return static_cast<T>(log(x)/LOG2); }
/** square value */
template <class T> inline T SQR(T a) { return a*a; }
/** negation */
template <class B> inline B NOT(B b) { return !b; }
/** set i.th bit */
inline int SET_BIT(int i) { return (1<<i); }
/** test if all bits \c tag are set in \c status modulo \c mask */
inline bool IS_MASKED(int status,int mask,int tag) { return (status&mask)==tag; }
/** test if at least one bit in \c tag is set in \c status */
inline bool IS_TAG(int status,int tag) { return (status&tag)!=0; }
inline bool IS_NO_TAG(int status,int tag) { return (status&tag)==0; }
inline void SET_TAG(int& status,int tag) { status = status|tag; }
inline void CLEAR_TAG(int& status,int tag) { status &= ~tag; }

/** Obsolete Constants */

#define POS_IMPOSSIBLE 1.0E20
#define NEG_IMPOSSIBLE -1.0E20
#define EPSILON 0.0001
#define MIN_LAMBDA ((TLambda)0.0)
#define MAX_LAMBDA ((TLambda)1.0)
#define POS_IMPOSSIBLE_LAMBDA ((TLambda)2.0)
#define NEG_IMPOSSIBLE_LAMBDA ((TLambda)-2.0)
#define NO_CONFLICT_LAMBDA POS_IMPOSSIBLE_LAMBDA
#define CONFLICT_LAMBDA ((TLambda)0.0)
#define ZERO_COST (0.0)
#define POS_IMPOSSIBLE_COST (1.0E20)
#define NEG_IMPOSSIBLE_COST (-1.0E20)
#define ZERO_RANGE (0.0)
#define EPSILON_RANGE (0.001)
#define POS_IMPOSSIBLE_RANGE (1.0E20)
#define NEG_IMPOSSIBLE_RANGE (-1.0E20)
#define MAX_LEN_TAG_STRING 15
#define MAX_LEN_NAME_STRING 31
//#define MAX_LEN_BUFFER_STRING 127
#define ZERO(a)     (fabs(a)<EPSILON)
#define NOTZERO(a)  (fabs(a)>=EPSILON)
#define POSITIVE(a) ((a) >  EPSILON)
#define NEGATIVE(a) ((a) < -EPSILON)
#define EQUAL ==

template <class DATA>
struct Constants
  {
  inline DATA max() const;
  inline DATA min() const;
  };

TEMPLATE_SPECIALIZATION inline int 
Constants<int>::max() const { return INT_MAX; }
TEMPLATE_SPECIALIZATION inline int 
Constants<int>::min() const { return INT_MIN; }

TEMPLATE_SPECIALIZATION inline float 
Constants<float>::max() const { return 1E30; }
TEMPLATE_SPECIALIZATION inline float 
Constants<float>::min() const { return -1E30; }

TEMPLATE_SPECIALIZATION inline double 
Constants<double>::max() const { return 1E30; }
TEMPLATE_SPECIALIZATION inline double 
Constants<double>::min() const { return -1E30; }

#endif /* CONSTANTS_HEADER */
