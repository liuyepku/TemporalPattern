
/* \file context.cpp
   \author Frank Hoppner <frank.hoeppner@ieee.org>
   \brief Implementation of class content */

#ifndef CONTEXT_SOURCE
#define CONTEXT_SOURCE

#include "dal.hpp"

namespace Data_Access_Library {

///// Context::context_type

/** \var Context::context_type Context::SETORRECORD
    \brief Context of record_type.

    Indicates that this context refers to a record_type or table. */

/** \var Context::context_type Context::FIELD
    \brief Context of field_type.

    Indicates that this context referes to a univariate field (such as
    int_type, real_type, string_type, etc.). */

/** \var Context::context_type Context::MATVEC
    \brief Context of array_type.

    Indicates that this context referes to a multivariate field (such
    as int_tuple_type, matrix_type, etc.). */

///// Context::mode_type

/** \enum Context::mode_type

    The mode of a Context addresses some properties of the field/table
    that is described by the context, for instance, whether it is a
    readonly or writeonly field etc. */

/** \var Context::mode_type Context::VOID

    If the m_mode Variable of a Context is VOID, then nothing has
    happened yet. */

/** \var Context::mode_type Context::INITIALIZED

    This flag indicates whether a table has been initialized, that is,
    all readers and writers have been instantiated and we are ready to
    operate on the table. */

/** \var Context::mode_type Context::SUBSETOUT

    This flag can be set to true, if the output is identical to a
    subset of the input (same order, no new records). In this case,
    the output key can be identical to the input keys (ordering
    preserved, gaps do not matter). However, as soon as hierarchical
    output is used (like sampling twice from the same dataset), the
    SUBSETOUT property does no longer hold, because the keys of the
    second sample are not necessarily larger than those of the
    first. */

/** \var Context::mode_type Context::ENDOFDATA

    Indicates that the last record of a table has been read, no more
    data will be read by consecutive pop() operations. (Only valid for
    SETORRECORD) */

/** \var Context::mode_type Context::READOUT

    In case of SETORRECORD this flag indicates whether we need a
    Reader for this table (that is, there is at least one field marked
    as READOUT). For FIELD or MATVEC it indicates whether a particular
    field shall be read from the table. */

/** \var Context::mode_type Context::WRITEBACK

    In case of SETORRECORD this flag indicates whether we need a
    Writer for this table (that is, there is at least one field marked
    as WRITEBACK). For FIELD or MATVEC it indicates whether a
    particular field shall be written into the table. */

/** \var Context::mode_type Context::SHARED

    Some fields may have the flavour of a temporal variable. If such a
    variable is a local one (that is, calculated for each record
    separately), the SHARED flag can be set. Then, we do not free()
    and allocate() new fields with each pop() command but reuse the
    same entry in the repository. */

/** \var Context::mode_type Context::DELAYED

    Obsolete. */

/** \var Context::mode_type Context::DYNALLOC

    Obsolete. */

/** \var Context::mode_type Context::MULTITABLE

    Undocumented features. Indicates whether there are records from
    multiple tables in the same .tab file. In this case, we need a
    leading field denoting the table to which the data line belongs. */

/** \var Context::mode_type Context::ORDERED

    Obsolete. */

/** \var Context::mode_type Context::DETAIL

    Flag is set for detail tables, to distinguish them from master
    tables. */

/** \var Context::mode_type Context::FLAT

    Flag is set if the output shall not contain hierarchical
    information; key and ref fields are surpressed in this case. */

/** \var Context::mode_type Context::KEYREF

    Fields that carry this flag are either key or ref fields. */

/** \var Context::mode_type Context::SUBSETTABLE

    Indicates whether this SETORRECORD is an SubSetTable or not. */

/** Context Constructor. */ 

Context::Context( 
  Context::context_type ct, ///< type of context 
  int mode, ///< mode (bitwise OR of mode_type's)
  const string& name, ///< name of the context
  const string& intag, ///< tag used for reading 
  const string& outtag, ///< tag used for writing
  Domain *d ///< domain of context
  )
  : m_type(ct)
  , m_mode(mode)
  , m_name(name)
  , m_intag(intag)
  , m_outtag(outtag)
  , m_offset(g_illegal_ix)
  , m_rows(0)
  , m_cols(0)
  , m_default_ix(g_illegal_ix)
  , mp_domain(d) 
  {
  INVARIANT(m_mode == (m_mode&(SUBSETOUT|READOUT|WRITEBACK|SHARED|ORDERED|DETAIL|FLAT|KEYREF)),"legal attributes");
  }

/** Debugging output. */

ostream& operator<<(ostream& os,const Context& c)
  {
      os << "<name " << c.name()
	 << "><coll " << c.coll()
	 << "><intag " << c.intag()
	 << "><outtag " << c.outtag()
	 << "><infile " << c.infile()
	 << "><outfile " << c.outfile()
//	 << "><offset " << c.offset()
//	 << "><rows " << c.rows()
//	 << "><cols " << c.cols()
	 << "><mode " << (c.is_initialized()?'I':'.')
	 << (c.is_endofdata()?'E':'.')
	 << (c.is_readout()?'R':'.')
	 << (c.is_writeback()?'W':'.')
	 << (c.is_detail()?'D':'M')
	 << "><defaultix " << c.get_default_ix()
     << ">";
  return os;
  }

void
Context::set_mode(int mode)
  {
  int mask = (SUBSETOUT|READOUT|WRITEBACK|SHARED|ORDERED|DETAIL|FLAT|KEYREF);
  CLEAR_TAG(m_mode,mask);
  SET_TAG(m_mode,mode&mask);
  }

} // namespace

#endif CONTEXT_SOURCE
