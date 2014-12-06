
/** \file bool3.hpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Three-valued boolean type. */

#ifndef bool3_HEADER
#define bool3_HEADER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

class bool3
  {
  public:
    enum value_type { true3,false3,unknown3 };

    inline bool3() : val(unknown3) {}
    inline bool3(bool b) : val((b)?true3:false3) {}
    inline bool3(value_type b) : val(b) {}

    inline void operator=(const bool b) { val=(b)?true3:false3; }

    inline void operator|=(const bool b) { if (b) val=true3; }
    inline void operator|=(const bool3 b) { if (b.val==true3) val=true3; }
    inline void operator|=(const value_type& v) { if (v==true3) val=true3; }

    inline bool operator==(const value_type& b) { return b==val; }
    inline bool operator==(const bool3& b) { return b.val==val; }

    inline bool operator!=(const value_type& x) { return x!=val; }
    inline bool operator!=(const bool3& x) { return x.val!=val; }

  private:
    value_type val;    
  };

#endif // bool3_HEADER
