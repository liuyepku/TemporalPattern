
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

/** \file defbool.cpp
    \author (c) 1999-2001 Frank Hoeppner <frank.hoeppner@ieee.org>

    \brief Implementation of bool-char conversions. 
*/

#ifndef DEFBOOL_SOURCE
#define DEFBOOL_SOURCE

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

#include "defbool.hpp"
#include <string.h>

const char booltext[][6] = {"true","false","yes","no"};

bool
strtob
  ( 
  const char * ap_text
  )
  {
  if (strcmp(ap_text,"yes")==0) return true; else
  if (strcmp(ap_text,"no")==0) return false; else
  if (strcmp(ap_text,"true")==0) return true; else
  if (strcmp(ap_text,"false")==0) return false; 
  return false;
  }

const char*
btostr
  (
  bool a_boolean
  )
  {
  return (a_boolean)?(const char*)booltext[0]:(const char*)booltext[1];
  }

#endif /* DEFBOOL_SOURCE */

