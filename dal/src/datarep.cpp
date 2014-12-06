
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

/** \file datarep.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Implementation of class Repository. 

    Since Repository<T> is a template, this file will be included by
    instances.cpp. */

#ifndef REPOSITORY_SOURCE
#define REPOSITORY_SOURCE

namespace Data_Access_Library {

/** Illegal index. This global variable is used for several purposes,
    it always indicates that no specific values has been set and
    instead the default value should be used (if there is one). */

ix_type g_illegal_ix = (ix_type)-1;

/** \class Repository
    \brief Data repository

    All instances of a specific variable type (like string_type,
    int_type, real_type, etc.) will be stored collectively in a
    repository (and the data_type variables use a pointer to an entry
    in the repository to access the variable). All instances will be
    allocated and organized by an STL deque<T> type and instances
    allocated once will be reused later without freeing and
    reallocating the memory. The instances can be accessed by their
    index in the deque array. */

/** Insert instance. A new instance is inserted into the repository
    and initialized with \c a_value. If the list of free entries is
    not empty, an old instance is reused, otherwise a new one will be
    allocated. The return value is the index in the deque. */

template <class T>
ix_type 
Repository<T>::insert(
    const T& a_value
    )
{
    if (m_free.empty())
    { 
	elements.insert(elements.end(),a_value);
	return next_element++;
    }
    else
    {
	ix_type ix(*m_free.begin());
	elements[ix]=a_value;
	m_free.pop_front();
	return ix;
    }
}
    
/** Insert empty instance. A new instance is inserted into the
    repository and initialized with the default value. If the list of
    free entries is not empry, an old instance will be reused,
    otherwise a new one will be allocated. The return value is the
    index in the deque. */

template <class T>
ix_type 
Repository<T>::insert()
{
    if (m_free.empty())
    { 
	T value;
	elements.insert(elements.end(),value);
	return next_element++;
    }
    else
    {
	ix_type ix(*m_free.begin());
	m_free.pop_front();
	return ix;
    }
}

/** Debugging output. */

template <class T>
void
Repository<T>::info(
    ostream& os
    ) 
    const
{
    word n(0);
    os << "repository" << endl;
    for (deque<T>::const_iterator i=elements.begin(); i!=elements.end(); ++i)
    { os << setw(5) << n << " " << (*i) << endl; ++n; }
}

/** Debugging output to track memory consumption. */

template <class T>
void
Repository<T>::mem_info(
    ostream& os
    ) 
    const
{
    const int n = next_element;
    const int free = m_free.size();
    const int percentfull = (n==0)?100:(n-free)*100/n;
    os << n << '(' << percentfull << "%)";
}

} // namespace

#endif REPOSITORY_SOURCE
