
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

/** \file ustream.cpp
    \author (c) 1999-2001 Frank Hoeppner <frank.hoeppner@ieee.org>

    \brief Stream i/o.
*/

#ifndef UTILSTREAM_SOURCE
#define UTILSTREAM_SOURCE

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

#include "ustream.hpp"

/** \fn is_followed_by
    \brief Sneak the next character in the stream.
   
    White spaces are not skipped. */

bool is_followed_by
  (
  istream& is,
  const char a_letter,
  bool put_back_if_found /* = true */,
  bool capital_letters /* = true */
  )
  {
  char c;
  if (is.eof() || !is.good()) return false;
  is.get(c);
  bool match( (capital_letters)?toupper(c)==toupper(a_letter):c==a_letter );
  if ((put_back_if_found) || (!match)) { is.putback(c); }
  return match;
  }

/** \fn is_followed_by 
    \brief Check which word in a sorted list of keywords comes next in
    the stream.

    A structure \c keyword_entry is used to store a sorted (!) list of
    keywords. The function returns the \c identifier field of the
    keyword whose name has been found next in the stream. If no
    keyword is found the return value is \c default_value. If \c
    put_back_if_found is \c true (default), then no character will be
    removed from the stream. If \c capital_letters is \c true
    (default), the read characters will be compared in upper case
    writing. If \c skip_white is \c true (default), white characters
    will be overread at the beginning (these characters will not be
    restored in case no keyword follows).

    If the keyword array contains entries which are prefixes of other
    entries (like "book" and "booktitle", then the longest match is
    returned.

    \attention If \c capital_letters is true, then the list of
    keywords has to be in capital letters, too. We do not transform
    the keywords into upper characters within in the function, because
    this alone is not sufficient: the ordering of keywords may also be
    affected. And we do not want to sort the keyword entries whenever
    the function is called. */

int is_followed_by
  (
  istream& is,
  const keyword_entry *p_keyarray,
  bool put_back_if_found /* = true */,
  bool capital_letters /* = true */,
  bool skip_white /* = true */,
  int default_value /* = -1 */
  )
  {
  char c,cmpc;
  int i=-1;
  bool match = false;
  bool busy;
  const keyword_entry *p_bestmatch = NULL, *p_keyword = p_keyarray;
  
  if (skip_white) read_white(is);

  do
    {
    busy = itob(is.good());
    if (busy)
      {
      is.get(c); 
      ++i;
      if (capital_letters) cmpc=toupper(c); else cmpc=c;

      match = (p_keyword->name!=NULL) && ((p_keyword->name[i]=='?')||(cmpc == p_keyword->name[i]));
      busy = (p_keyword->name!=NULL) && (p_keyword->name[i+1] != '\0');

      if (!match && (p_keyword->name!=NULL))
	{
        const keyword_entry *p = p_keyword;
        while ((p->name!=NULL) && (cmpc>p->name[i])) ++p;
        if (p->name!=NULL)
	  {
          match = (cmpc == p->name[i]);
          for (int j=0;j<i;++j) 
            match &= (p_keyword->name[j] == p->name[j]);
          if (match) { p_keyword=p; busy = (p_keyword->name[i+1] != '\0'); }
	  }        
        }

      if (!busy&&match) // found, check if we have found only a
        {               //  prefix and can find a longer match
        p_bestmatch=p_keyword; 
        if (p_keyword->name!=NULL)
          {
          const keyword_entry *p = p_keyword; ++p;
          bool prfx = (p->name!=NULL);
          if (prfx) for (int s=0;s<=i;++s) prfx &= (p->name[s]==p_keyword->name[s]);
	  if (prfx) { busy=true; match=true; p_keyword=p; }
	  }
	}
      }
    }
  while (match && busy);

  // we have to restore the last char if we did not end with match=true
  if (!match || busy) // busy == !done
    {
    is.putback(c);
    --i;
    }

  // if we have found a prefix-match, this is also a success
  match |= (p_bestmatch!=NULL);

  // write back if necessary (and possible; not possible if capital match)
  if ((put_back_if_found || !match) && (!capital_letters))
    {
    for (;i>=0;--i)
      {
      is.putback(p_keyword->name[i]);
      }
    }

  return (match)?p_bestmatch->identifier:default_value;
  }


/** Same as above, but keywords are stored in a string map rather than
    a sorted char array. */

int is_followed_by
  (
  istream& is,
  const str2idx_type& map,
  bool put_back_if_found /* = true */,
  bool capital_letters /* = true */,
  bool skip_white /* = true */,
  int default_value /* = -1 */
  )
  {
  char c,cmpc;
  int i=-1;
  bool match = false;
  bool busy;
  str2idx_type::const_iterator bestmatch=map.end(),keyword=map.begin();
  
  if (skip_white) read_white(is);

  do
    {
    busy = itob(is.good());
    if (busy)
      {
      is.get(c); 
      ++i;

      if (capital_letters) cmpc=toupper(c); else cmpc=c;

      match = (keyword!=map.end()) && (((*(*keyword).first)[i]=='?')||(cmpc == (*(*keyword).first)[i]));
      busy = (keyword!=map.end()) && ((*(*keyword).first).length()>i+1);

      if (!match && (keyword!=map.end()))
	{
        str2idx_type::const_iterator k = keyword;
        while ((k!=map.end()) && (cmpc>(*(*k).first)[i])) ++k;
        if (k!=map.end())
	  {
          match = (cmpc == (*(*k).first)[i]);
          for (int j=0;j<i;++j) 
            match &= ((*(*keyword).first)[j] == (*(*k).first)[j]);
          if (match) { keyword=k; busy = ((*(*keyword).first).length()>i+1); }
	  }        
        }

      if (!busy&&match) // found, check if we have found only a
        {               //  prefix and can find a longer match
        bestmatch=keyword; 
        if (keyword!=map.end())
          {
          str2idx_type::const_iterator k = keyword; ++k;
          bool prfx = (k!=map.end());
          if (prfx) for (int s=0;s<=i;++s) prfx &= ((*(*k).first)[s]==(*(*keyword).first)[s]);
	  if (prfx) { busy=true; match=true; keyword=k; }
	  }
	}

      } // if busy
    }
  while (match && busy);

  if (!match || busy) // busy == !done
    {
    is.putback(c);
    --i;
    }

  // if we have found a prefix-match, this is also a success
  match |= (bestmatch!=map.end());

  // write back if necessary (and possible; not possible if capital match)
  if ((put_back_if_found || !match) && (!capital_letters))
    {
    for (;i>=0;--i)
      {
      is.putback((*(*keyword).first)[i]);
      }
    }

  return (match)?(*bestmatch).second:default_value;
  }

/** Sneak for a word in the input stream. 

    \attention If you set \c capital_letters to \c true, the keyword
    \c p_find must also be in capital letters. */

bool is_followed_by
  (
  istream& is, ///< input stream
  const char* p_find, ///< is this word next in the input stream?
  bool put_back_if_found /* = true */,
  bool capital_letters /* = false */,
  bool skip_white /* = true */
  )
  {
  keyword_entry single_entry[2] = { {(char*)p_find,1},{NULL,0} };
  return (-1 != is_followed_by(is,(keyword_entry*)&single_entry,put_back_if_found,capital_letters,skip_white));
  }

/** Return the keyword associated with a given return index.

    Function seeks for the entry in ap_keywords with identifier \c
    value and returns the associated keyword name. */

char*
get_keyword
  (
  const keyword_entry* ap_keywords,
  int value
  )
  {
  while ((ap_keywords->identifier!=value) &&
         (ap_keywords->name!=NULL))
    {
    ++ap_keywords;
    }
  return ap_keywords->name;
  }

/** Read white spaces from stream.

    This function reads all white characters (like spaces, tabs, line
    feeds) from the stream. */

template <class S>
void read_white
  (
  istream& is,
  S& text
  )
  {
  char c;
  size_t stringlen = 0; 
  size_t maxlen = text.max_size();
  text.resize(0);

  if (is.good() && !is.eof())
    {
    do
      {
      is.get(c);
      if (stringlen<maxlen) { text+=c; ++stringlen; }
      }
    while ( (is.good()) && (!is.eof()) && (isspace(c)) );
    is.putback(c);
    }      
  text.resize(stringlen);
  }

/** read white-space characters from stream into \c string */
template void read_white(istream&,string&);
/** read white-space characters from stream into \c Chars */
template void read_white(istream&,Chars&);
/** read white-space characters from stream into \c char* */
void read_white(istream& is,char* p,size_t l) 
  { Chars c(p,l); read_white(is,c); }
/** read white-space characters from stream */
void read_white(istream& is) 
  { NoChars c; read_white(is,c); }

/** Read from stream until break character found.

    Reading is stopped if any of the characters in the second
    parameter is found. Reading is also stopped at white space
    characters. If the first (non-white) character is a break
    character, it will not stop reading. The break character will not
    be removed from the input stream. */

template <class S>
void read_word_break
  (
  istream& is,
  const char * p_delimiter,
  S& text
  )
  {
  char c;
  int i;
  bool cont;
  bool nonempty((p_delimiter!=NULL) && (p_delimiter[0]!='\0'));
  bool notfirst = false;
  size_t stringlen = 0;
  size_t maxlen = text.max_size();
  text.resize(0);

  read_white(is);
  if (is.good())
    {
    do
      {
      is.get(c);
      cont = !isspace(c) && (is.good()) && (!is.eof());
      if (cont && nonempty && notfirst) 
        {
        i=0; 
        while ((p_delimiter[i]!='\0') && cont)
          { cont &= (c!=p_delimiter[i]); ++i; }
        }
      notfirst=true;
      if (cont&&(stringlen<maxlen)) { text+=c; ++stringlen; }
      }
    while (cont);
    is.putback(c);
    }
  text.resize(stringlen);
  }

template void read_word_break(istream&,const char*,string&);
template void read_word_break(istream&,const char*,Chars&);
void read_word_break(istream& is,const char *d,char* p,size_t l) 
  { Chars c(p,l); read_word_break(is,d,c); }
void read_word_break(istream& is,const char *d) 
  { NoChars c; read_word_break(is,d,c); }

/** Read from stream until keyword is found. 

    This function reads all characters from the stream until the
    specified keyword (2nd parameter) has been found. The return value
    specifies whether the keyword has been found or not (end of
    file). */

template <class S>
bool read_until
  (
  istream& is,
  const char * p_find,
  S& text,
  bool clear_text /* = true */
  )
  {
  int matching = 0;
  if (clear_text) text.resize(0);
  size_t stringlen = text.length();
  const int skipchars = strlen(p_find);
  size_t maxlen = text.max_size();

  if (    (p_find    != NULL)
       && (p_find[0] != '\0')
       && (is.good())
     )
    {
    char c;

    do
      {
      is.get(c);
      if (c==p_find[matching])
        { ++matching; }
      else
        { matching = 0; }
      if (stringlen<maxlen) { text+=c; }
      ++stringlen;
      }
    while ( (matching<skipchars) && (is.good()) );

    size_t storelen(0);
    if (stringlen-skipchars<maxlen)
      { storelen = stringlen-skipchars; }
    else
      { storelen = maxlen; }
    text.resize(storelen);
    }

  return ((matching==skipchars) && (is.good() || is.eof()));
  }

template bool read_until(istream&,const char*,string&,bool);
template bool read_until(istream&,const char*,Chars&,bool);
bool read_until(istream& is,const char *d,char* p,size_t l,bool b) 
  { Chars c(p,l); return read_until(is,d,c,b); }
bool read_until(istream& is,const char *d) 
  { NoChars c; return read_until(is,d,c,false); }

/** Read remaining characters until end of stream. */

template <class S>
void read_until_eof
  (
  istream& is,
  S& text
  )
  {
  char c;
  size_t stringlen = 0;
  size_t maxlen = text.max_size();
  text.resize(0);

  if (is.good())
    {
    do
      {
      is.get(c);
      if (stringlen<maxlen) { text+=c; ++stringlen; }
      }
    while (!is.eof());
    }
  }

template void read_until_eof(istream&,string&);
template void read_until_eof(istream&,Chars&);
void read_until_eof(istream& is,char* p,size_t l) 
  { Chars c(p,l); return read_until_eof(is,c); }

/** Read lines with a certain prefix.

    This function reads all lines from the stream starting with a
    certain prefix (2nd parameter). (Useful to skip all comment lines
    introduced by a certain string.) */

void read_lines_starting_with
  (
  istream& is, 
  const char* p_find
  )
  {
  bool found;
  do
    {
    read_white(is);
    found = is_followed_by(is,p_find);
    if (found) { read_line(is); }
    }
  while (found);
  }

/** Read text in parantheses.

    This function assumes that a certain ``opening'' paranthesis (2nd
    parameter) has already been read from the stream. The function
    reads all characters until the matching ``closing'' paranthesis
    (3rd parameter) has been found. */

template <class S>
void read_until_matching_paranthesis
  (
  istream& is, 
  char ip,
  char dp,
  S& text,
  bool clear_text /* = true */
  )
  {
  char c;
  if (clear_text) text.resize(0);
  size_t stringlen = text.length();
  size_t maxlen = text.max_size();
  int counter = 1;
  //INVARIANT(ip!=dp,"parantheses must be distinguishable");

  while (    (counter!=0) 
          && (is.good()) 
        )
    {
    is.get(c);
    if ( c == ip ) ++counter;
    else if ( c == dp ) --counter;
    if ((stringlen<maxlen) && (counter>0)) { text += c; ++stringlen; }
    }
  }

template void read_until_matching_paranthesis(istream&,char,char,string&,bool);
template void read_until_matching_paranthesis(istream&,char,char,Chars&,bool);
void read_until_matching_paranthesis(istream& is,char i,char d,char* p,size_t l,bool b) 
  { Chars c(p,l); return read_until_matching_paranthesis(is,i,d,c,b); }
void read_until_matching_paranthesis(istream& is,char i,char d) 
  { NoChars c; return read_until_matching_paranthesis(is,i,d,c,false); }

/** Read data entry from stream. 

    This function reads a single word, the text between "hyphens" or
    {parantheses}. This format is used within BibTeX files, for
    example. In case of a single word, a final separating colon will
    not be removed from the stream. If \c keep_delimiter is \c true,
    the hyphens and parantheses will also be stored in the \c text
    output.  */

template <class S>
void read_entry
  (
  istream& is,
  S& text,
  bool keep_delimiter /* = false */
  )
  {
  read_white(is);
  text.resize(0);
  if (is_followed_by(is,'{',false))
    {
    if (keep_delimiter) text += '{';
    read_until_matching_paranthesis(is,'{','}',text,false);
    if (keep_delimiter) text += '}';
    }
  else if (is_followed_by(is,'\"',false))
    {
    if (keep_delimiter) text += '\"';
    read_until(is,"\"",text,false);
    if (keep_delimiter) text += '\"';
    }
  else
    {
    read_word_break(is,", }",text);
    }
  }

template void read_entry(istream&,string&,bool);
template void read_entry(istream&,Chars&,bool);
void read_entry(istream& is,char* p,size_t l,bool b) 
  { Chars c(p,l); return read_entry(is,c,b); }
void read_entry(istream& is) 
  { NoChars c; return read_entry(is,c,false); }

/** Fortschritts-Anzeige */

ProgressBar::ProgressBar
  (
  float minimum,
  float *val,
  float maximum,
  TTime mindelay,
  int length
  )
  : mp_value(val)
  , m_min(minimum)
  , m_max(maximum)
  , m_length(length)
  , m_mindelay(mindelay)
  , m_lastcall(actual_time())
  , m_progress(0)
  , m_tick(0)
  {
  m_length = min(m_length,81-2-1);
  m_bar[0] = '[';
  for (int i=1;i<=m_length;++i) m_bar[i]=' ';
  m_bar[m_length+1] = ']';
  m_bar[m_length+2] = '\0';
  }

bool
ProgressBar::operator()() const
  {
  return (passed_time_since(m_lastcall)>m_mindelay);
  }

void
ProgressBar::write
  (
  ostream& os
  )
  const
  {
  static char display[] = "pbdqo";
  if (passed_time_since(m_lastcall)>m_mindelay)
    {
    m_lastcall = actual_time(); ++m_tick;

    float value = max(min(m_max,*mp_value),m_min);
    int progress = static_cast<int>((value-m_min)/(m_max-m_min)*m_length);
    int percent = static_cast<int>((value-m_min)/(m_max-m_min)*100.0);

    if (progress>m_progress) 
      {
      for (;m_progress<progress;++m_progress) m_bar[m_progress+1] = display[4];
      }
    if (m_progress<m_length) m_bar[m_progress+1]=display[ m_tick % 4 ];

    os << m_bar;
    os.width(3);
    os << percent << '%';
    }
  }

#endif /* UTILSTREAM_SOURCE */
