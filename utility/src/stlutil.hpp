
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

    \brief This file provides a couple of functors which are helpful
    when using the standard template library (STL). To use a STL
    [[set<T,...>]] or [[map<T,...>]] data structure, one must provide
    a [[binary_function<T,T,bool>]] to compare the [[set]] (resp.\
    [[map]]) contents. We define some of those binary functions for
    frequently used types [[T]]. Furthermore, we present some
    composition functors to compose binary functions. */

#ifndef STLUTIL_HEADER
#define STLUTIL_HEADER

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

#include <string.h> // strcmp
#include <functional> // binary_function, unary_compose
#include <ext/functional> // binary_function, unary_compose
#include <utility>
#include "define.hpp"
using namespace std;
using namespace __gnu_cxx;

/** Virtual unary function.

    In some cases, one wants to use a [[unary_function]] as a virtual
    function, and it seems to be convenient to adapt the notation of
    the STL. */

template <class Arg, class Result>
struct virtual_unary_function 
  : public unary_function<Arg,Result>
  {
  public:
    virtual Result operator()(Arg) = 0;
  };

/** Virtual binary function. \see virtual_unary_function */
template <class Arg1, class Arg2, class Result>
struct virtual_binary_function 
  : public binary_function<Arg1,Arg2,Result>
  {
  public:
    virtual Result operator()(Arg1,Arg2) = 0;
  };      

/** Binary function to compare char* pointer via strcmp. */
struct string_less
  : public binary_function<char_cc,char_cc,bool>
  {
  bool operator() (char_cc,char_cc) const;
  };

/** Binary function to compare pointer to classes via dereferencing
    them and calling operator<. This functor is extremly useful for
    storing pointers T* to classes T in a
    set<T*,less_dereference<T*>>, using the order provided by
    T::operator<. */

template <class POINTER>
struct less_dereference 
  : public binary_function<const POINTER, const POINTER, bool>
  {
  bool operator() (const POINTER ap_x, const POINTER ap_y) const
    { return (*ap_x) < (*ap_y); }
  };

/* obsolete 
template <class POINTER>
struct less_cost_dereference 
  : public binary_function<const POINTER, const POINTER, bool>
  {
  bool operator() ( const POINTER ap_x, const POINTER ap_y ) const
    { return ap_x->get_cost() < ap_y->get_cost(); }
  };

template <class POINTER>
struct less_name_dereference 
  : public binary_function<const POINTER, const POINTER, bool>
  {
  bool operator() ( const POINTER ap_x, const POINTER ap_y ) const
    { return strcmp(ap_x->get_name(),ap_y->get_name())<0; }
  };

template <class POINTER>
struct less_number_dereference 
  : public binary_function<const POINTER, const POINTER, bool>
  {
  bool operator() ( const POINTER ap_x, const POINTER ap_y ) const
    { return ap_x->get_number() < ap_y->get_number(); }
  };
*/

template <class Op1, class Op2, class Op3>
class mixed_compose 
  : public binary_function
      <typename Op2::argument_type,typename Op3::argument_type,typename Op1::result_type>
  {
  public:
    mixed_compose(const Op1& x, const Op2& y, const Op3& z) 
      : op1(x), op2(y), op3(z) {}
    typename mixed_compose::result_type operator()(const typename mixed_compose::first_argument_type& x, const typename mixed_compose::second_argument_type& y) const
      { return op1( op2(x), op3(y) ); }
  protected:
    Op1 op1;
    Op2 op2;
    Op3 op3;
  };

/** compose(u1,u2) is a unary function u with u(x)=u1(u2(x)). */
template <class Operation1, class Operation2>
unary_compose<Operation1,Operation2> 
compose(const Operation1& op1,const Operation2& op2)
  { return unary_compose<Operation1,Operation2>(op1,op2); }

/** compose(u1,u2,u3) is a binary function u with u(x,y)=u1(u2(x),u3(y)). */
template <class Operation1, class Operation2, class Operation3>
mixed_compose<Operation1,Operation2,Operation3> 
compose(const Operation1& op1,const Operation2& op2,const Operation3& op3)
  { return mixed_compose<Operation1,Operation2,Operation3>(op1,op2,op3); }

/** class to delete pointers via for_each */
template <class T> 
struct Deleter
  { 
  inline void operator() (T* p) { delete p; } 
  };

#endif /* STLUTIL_HEADER */



