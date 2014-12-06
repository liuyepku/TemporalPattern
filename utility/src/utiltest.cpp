
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

/** \file utiltest.cpp
    \author (c) 1999-2001 Frank Hoeppner <frank.hoeppner@ieee.org>

    \brief Main program to test some functions of the library. 
*/

#ifndef UTILTEST_SOURCE
#define UTILTEST_SOURCE

#ifdef HAVE_CONFIG_H
/*
//FILETREE_IFDEF HAVE_CONFIG_H
*/
#include "config.h"
/*
//FILETREE_ENDIF
*/
#endif

#include <strstream>
#include "strconv.hpp"
#include "ustream.hpp"

int 
main()
  {
  bool ok(true);
  bool gok(true);

  { // string conversion
  const int buflen(1024);
  char buffer[buflen];
  convert_to_tex("ä>ü_",buffer,1); 
    ok &= strcmp(buffer,"")==0;
  convert_to_tex("ä>ü_",buffer,buflen); 
    ok &= strcmp(buffer,"\\\"a$>$\\\"u\\_")==0;
  convert_to_html("äxü>",buffer,8); 
    ok &= strcmp(buffer,"&auml;x")==0;
  convert_to_html("äxü>",buffer,buflen); 
    ok &= strcmp(buffer,"&auml;x&uuml;&gt;")==0;
  strcpy(buffer,"   this   is  \n   a     book  ");
  delete_superfluous_blanks(buffer);
    ok &= strcmp(buffer,"this is a book")==0;
  }

  cout << "conversion [char] : " << (ok?"ok":"failure") << endl;
  gok &= ok; ok = true;

  { // string conversion
  string buffer;
  convert_to_tex(string("ä>ü_"),buffer); 
    ok &= buffer=="\\\"a$>$\\\"u\\_";
  buffer = "   this   is  \n   a     book  ";
  delete_superfluous_blanks(buffer);
    ok &= buffer=="this is a book";
  }

  cout << "conversion [string] : " << (ok?"ok":"failure") << endl;
  gok &= ok; ok = true;

  { // utilstream
  strstream s;
  char buffer[128];
  Chars c((char*)&buffer,128);
  s << "  hallo hallo";
  read_white(s);
  read_word(s,c); 
    ok &= c=="hallo";
  read_word_break(s,"l",c); 
    ok &= c=="ha";
  read_word(s,c); 
    ok &= c=="llo";
  }

  cout << "read words [Chars] : " << (ok?"ok":"failure") << endl;
  gok &= ok; ok = true;

  { // utilstream
  strstream s;
  string c;
  s << "a b c d\ne f g h";
  read_until(s,"c",c);
    ok &= c=="a b ";
  read_line(s);
  read_word(s,c);
    ok &= c=="e";
  read_line(s,c);
    ok &= c==" f g h";
  }

  cout << "read words [string] : " << (ok?"ok":"failure") << endl;
  gok &= ok; ok = true;

  { // utilstream
  strstream s;
  string c;
  s << "<value 8><output \"t<e>xt\">";
  ok &= is_followed_by(s,'<',false);
  ok &= !is_followed_by(s,"<text>");
  ok &= is_followed_by(s,"value"); // value not removed
  ok &= is_followed_by(s,"VALUE",false,true); // value removed
  read_until_matching_paranthesis(s,'<','>',c);
    ok &= c==" 8";
  read_until(s,"<");
  read_until_matching_paranthesis(s,'<','>',c);
    ok &= c=="output \"t<e>xt\"";
  }

  cout << "sneak & para [string] : " << (ok?"ok":"failure") << endl;
  gok &= ok; ok = true;

  float v;
  ProgressBar bar(0,&v,100,0.1,60);
  for (int i=0;i<=100;++i)
    {
    TTime now = actual_time(); 
    while (passed_time_since(now)<0.01) {}; 
    v=i; cout << bar << "\r" << flush; 
    }
  cout << bar << endl;

  cout << "overall utiltest : " << (gok?"ok":"failure") << endl;

  return (gok)?0:-1;
  }

#endif /* UTILTEST_SOURCE */
