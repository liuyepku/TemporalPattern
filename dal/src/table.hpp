
/** \file table.hpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Header of record_type */

#ifndef TABLE_HEADER
#define TABLE_HEADER

#include "dal.hpp"
#include "matvec.hpp"
#include "instances.cpp"

namespace Data_Access_Library {

//////////////////////////////////////////////////////////////

class record_type
    : public data_type< DynTuple<ix_type> > 
{
public:
    explicit record_type(Context *ap_context);
    virtual ~record_type();
    
    void attach(abstract_data_type *ap_value);
    virtual void info(ostream& os) const;
    virtual void load_default();
    virtual int size() const { return m_size; }
    
    virtual void load(word);
    virtual void copy(ix_type); 
    virtual void free();
    virtual void free(ix_type ix);
    void init();
    void store();
    
public:
    adtp_list m_fields;
    int m_size;
};

} // Data_Access_Library

#endif // TABLE_HEADER
