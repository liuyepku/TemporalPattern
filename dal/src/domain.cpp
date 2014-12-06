
/*  Copyright     : Copyright (C) 2000-2002 Frank Hoeppner

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation; either version 2.1
    of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307, USA.  */

/** \file domain.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Implementation of class Domain */

#ifndef DOMAIN_SOURCE
#define DOMAIN_SOURCE

#include "dal.hpp"
#include <stdio.h> // sprintf
#include "globdef.hpp"

namespace Data_Access_Library {

/** \class Domain
    \brief Handling of symbolic data.

    Domains are used to store a mapping of symbolical variables to
    integer numbers which are used to represent the symbols. If a
    data_type is instantiated with no domain, only numerical variables
    will be allowed. */

/** Default Constructor. */

Domain::Domain()
  : m_size(0)
  , m_next(0)
  , m_fixed_values(false)
  , m_precision(12)
  , m_mixed_mode(true)
  {
  }

/** Constructor. Mixed mode should be set to true if numerical as well
    as symbolic values are allowed in this domain. (If set to true,
    only symbolic labels are accepted, number will be considered as
    symbols.) */

Domain::Domain
  (
  const string& name, ///< name of domain
  bool mixmode ///< mixed mode (default=true)
  )
  : m_size(0) 
  , m_next(0)
  , m_fixed_values(false)
  , m_precision(12)
  , m_mixed_mode(mixmode)
  {
  gp_dm_domains->add(name,(long)this);
  }

/** Renumber integers assigned to labels. Sort the integer values
    according to lexicographical order of the symbolic labels. */

void
Domain::reassign_indices()
  {
  if (m_fixed_values) ERROR("reassign denied, fixed domain values");

  m_idx2str.clear();
  int n = 0;
  for (str2idx_type::iterator i=m_str2idx.begin();i!=m_str2idx.end();++i)
    { (*i).second = n; m_idx2str[n] = (*i).first; ++n; }
  INVARIANT(n==m_size,"n="<<n<<",m_size="<<m_size);
  m_next = n;
}

/** Remove all symbols from the domain. */

void
Domain::clear()
  {
  if (m_fixed_values) ERROR("clear denied, fixed domain values");

  m_size = 0;
  m_next = 0;
  m_fixed_values = false;
  m_str2idx.clear();
  m_idx2str.clear();
  m_words.clear();
  }

/** Search for a symbolic label. Search for a symbol \c label in the
    domain, return value is true if the symbols has been found. In
    this case, \c value contains the integer number associated with
    this label. If the symbols is not found, it will not be added to
    the domain. */

bool
Domain::find
  (
  const string& label,
  int &value
  ) 
  {
  // Test auf Vorhandensein
  str2idx_type::const_iterator i = m_str2idx.find(&label);
  // Wenn schon bekannt, dann Index zurueckmelden
  if (i==m_str2idx.end())
    { value=INT_MAX; return false; }
  else
    { value=(*i).second; return true; }
  }

/** Search for a symbolic label in stream. Check if the next symbols
    in the input stream \c is corresponds to a symbols in the domain,
    return value is true if a valid symbol has been found. In this
    case, \c value contains the integer number associated with this
    label (and the label is removed from the stream). If the symbols
    is not found, it will not be added to the domain (and the stream
    remains untouched). */

bool
Domain::find
  (
  istream& is,
  int& value
  ) 
  {
  //LOG_METHOD("DAL","Domain","find",*this);
  value=is_followed_by(is,m_str2idx,false,false,true,INT_MAX);
  if (value==INT_MAX)
    {
    TRACE("label in istream not known");
    return false;
    }
  else
    {
    TRACE("find in istream "<<value);
    return true;
    }
  }

/** Add a symbol to the domain (if not yet present). The number that
    will be identified with this label is returned by the method. */

int
Domain::add
  (
  const string& label
  ) 
  {
  //LOG_METHOD("DAL","Domain","add",*this);
  { // Test auf Vorhandensein
    str2idx_type::const_iterator i = m_str2idx.find(&label);
    // Wenn schon bekannt, dann Index zurueckmelden
    if (i!=m_str2idx.end()) 
      {
      TRACE("add label "<<label<<" already known as "<<(*i).second);
      return (*i).second;
      }
   }
   { // Anmeldung eines neuen Labels
    m_words.push_back(label);
    string *p = &(*m_words.rbegin());
    m_str2idx[p] = m_next; 
    TRACE("add label "<<label<<" index "<<m_next);
    if (m_idx2str.find(m_next)==m_idx2str.end()) m_idx2str[m_next] = p;
    ++m_size; ++m_next;
   }
  return (m_next-1);
  }

/** Add symbol to domain. Add the symbol \c label to the domain and
    use the integer \c value to represent it. Adding the same label
    multiple times is not allowed, while different labels may map to
    the same integer (e.g. yes and true may have the same integer
    representation). For output operations the label that has been
    added first will be used. */

void 
Domain::add
  (
  const string& label,
  int value
  ) 
  {
  //LOG_METHOD("DAL","Domain","add",*this);
    { // Test auf doppelten Bezeichner
    str2idx_type::const_iterator i = m_str2idx.find(&label);
    // Doppelanmeldung leise ignorieren
    if (i!=m_str2idx.end())
      {
      if ((*i).second==value) return;
      // Abbruch
      //ERROR("label "<<label<<" already in "<<m_words);
      }
    }
    { // Anmeldung 
    m_words.push_back(label);
    string *p = &(*m_words.rbegin());
    m_str2idx[p] = value;
    if (m_idx2str.find(value)==m_idx2str.end()) m_idx2str[value] = p;
    TRACE("add label "<<label<<" index "<<value);
    // Nachdem Werte direkt gesetzt wurden, keine Neuordnung mehr
    m_fixed_values = true;
    ++m_size; m_next=max(m_next,value)+1;
    }
  }

/** Return the label corresponding to an integer. If the integer \c
    value is not known in the domain, the function returns a \c NULL
    pointer! */

const string*
Domain::identifier
  (
  int value
  )
  const
  {
  idx2str_type::const_iterator i = m_idx2str.find(value);
  return (i==m_idx2str.end())?NULL:(*i).second;
  }

/** Print domain (for debugging purposes). */

void 
Domain::info
  (
  ostream& os
  )
  const
  {
  os << "<domain><name " << gp_dm_domains->identifier((long)this) 
     << "><fixed " << (m_fixed_values?"yes":"no") 
     << "></domain>" << endl;

  for (str2idx_type::const_iterator i=m_str2idx.begin();i!=m_str2idx.end();++i)
    {
    os << "<label><name " << *(*i).first 
       << "><value " << (*i).second 
       << "></domain>" << endl;
    }
  }

/** Print domain (for debugging purposes). */

ostream& operator<<(ostream& os,const Domain& d)
  {
  const string *p = gp_dm_domains->identifier((long)&d); 
  os <<'<'; 
  if (p!=NULL) os <<*p;
  for (str2idx_type::const_iterator i=d.m_str2idx.begin();i!=d.m_str2idx.end();++i)
    {
    os << ' ' << *(*i).first << ':' << (*i).second; 
    }
  os <<'>';
  return os;
  }

/** Search a domain by its name. All domains are stored in \c
    gp_dm_domains. If the domain is not known, the return value will
    be NULL. */

Domain*
get_domain
  (
  const string& name
  )
  {
  int i;
  return (gp_dm_domains->find(name,i)) ? (Domain*)(i) :  NULL; 
  }

}; // namespace

#endif // DOMAIN_SOURCE

