
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

/** \file instances.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Invoke code generation for template classes. 

    All necessary templates will be instantiated in this file. */

#ifndef INSTANCES_SOURCE
#define INSTANCES_SOURCE

#include "dal.hpp"

using namespace Data_Access_Library;

//////////////////////////////////////////////////////////////
// matrices and vectors

#include "mread.cpp"
#include "mcompare.cpp"

template DynTuple<ix_type>;
template DynTuple<int>;
template DynTuple<float>;
template DynTuple<double>;

template DynMatrix<int>;
template DynMatrix<double>;

template TransMatrix< DynTuple<double> >;
template TransMatrix< DynMatrix<double> >;

template CutMatrix< DynMatrix<int> >;

template SelectTuple< DynTuple<double>,DynMatrix<int> >;

//////////////////////////////////////////////////////////////
// fields

#include "datatype.cpp"

template void data_type<int>::set_global_default(const int&);
template void data_type<ix_type>::set_global_default(const ix_type&);
template void data_type<long>::set_global_default(const long&);
template void data_type<double>::set_global_default(const double&);

template field_type< int >;
template field_type< ix_type >;
template field_type< long >;
template field_type< string >;
template field_type< double >;

template array_type< DynTuple<int> >;
template array_type< DynTuple<ix_type> >;
template array_type< DynTuple<double> >;

template array_type< DynMatrix<int> >;
template array_type< DynMatrix<double> >;


//////////////////////////////////////////////////////////////
// SymbolicIO

#include "io.cpp"

template SymbolicIO<int>;
template SymbolicIO<ix_type>;
template SymbolicIO<double>;

//////////////////////////////////////////////////////////////
// Repository

#include "datarep.cpp"

Repository< ix_type > data_type< ix_type >::m_repository;
ix_type data_type< ix_type >::m_global_default_ix(g_illegal_ix);
template void data_type< ix_type >::mem_info(ostream&);

Repository<int> data_type<int>::m_repository;
ix_type data_type<int>::m_global_default_ix(g_illegal_ix);
template void data_type< int >::mem_info(ostream&);

Repository<long> data_type<long>::m_repository;
ix_type data_type<long>::m_global_default_ix(g_illegal_ix);
template void data_type< long >::mem_info(ostream&);

Repository<double> data_type<double>::m_repository;
ix_type data_type<double>::m_global_default_ix(g_illegal_ix);
template void data_type< double >::mem_info(ostream&);

Repository<string> data_type<string>::m_repository;
ix_type data_type<string>::m_global_default_ix(g_illegal_ix);
template void data_type< string >::mem_info(ostream&);

Repository< DynTuple<ix_type> > data_type< DynTuple<ix_type> >::m_repository;
ix_type data_type< DynTuple<ix_type> >::m_global_default_ix(g_illegal_ix);
template void data_type< DynTuple<ix_type> >::mem_info(ostream&);

Repository< DynTuple<int> > data_type< DynTuple<int> >::m_repository;
ix_type data_type< DynTuple<int> >::m_global_default_ix(g_illegal_ix);
template void data_type< DynTuple<int> >::mem_info(ostream&);

Repository< DynTuple<double> > data_type< DynTuple<double> >::m_repository;
ix_type data_type< DynTuple<double> >::m_global_default_ix(g_illegal_ix);
template void data_type< DynTuple<double> >::mem_info(ostream&);

Repository< DynMatrix<int> > data_type< DynMatrix<int> >::m_repository;
ix_type data_type< DynMatrix<int> >::m_global_default_ix(g_illegal_ix);
template void data_type< DynMatrix<int> >::mem_info(ostream&);

Repository< DynMatrix<double> > data_type< DynMatrix<double> >::m_repository;
ix_type data_type< DynMatrix<double> >::m_global_default_ix(g_illegal_ix);
template void data_type< DynMatrix<double> >::mem_info(ostream&);

#endif // INSTANCES_SOURCE




