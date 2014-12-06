
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

/** \file strconv.hpp
    \author (c) 1999-2001 Frank Hoeppner <frank.hoeppner@ieee.org>

    \brief String conversion routines for char*, string, and Chars.
*/

#ifndef STRCONVERT_HEADER
#define STRCONVERT_HEADER

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

#include "Chars.hpp" // Chars
#include <string> // string

void turn_to_lower(char*);
template <class S> void turn_to_lower(S&);

void turn_to_upper(char*);
template <class S> void turn_to_upper(S&);

void convert_to_tex(char const*,char*,size_t);
template <class S> void convert_to_tex(const S&,S&);

void convert_to_html(char const *, char * const,size_t);
template <class S> void convert_to_html(const S&,S&);

void convert_from_post(char const *, char * const,size_t);
template <class S> void convert_from_post(const S&,S&);

void delete_superfluous_blanks(char*);
template <class S> void delete_superfluous_blanks(S&);

void convert_to_robust_name(char*);
template <class S> void convert_to_robust_name(S&);

#endif /* STRCONVERT_HEADER */
