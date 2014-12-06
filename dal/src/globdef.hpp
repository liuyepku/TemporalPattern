
#ifndef GLOBDEF_HEADER
#define GLOBDEF_HEADER

#include "dal.hpp"

namespace Data_Access_Library {

// globale Domains

extern Domain *gp_dm_domains;
extern Domain *gp_dm_tables;

// LABEL-Konstanten, 0-Werte bezeichnen Default-Werte

#define LABEL_CLASS_UNKNOWN 0

# define LABEL_SAMPLEMODE_SUBSAMP 0
# define LABEL_SAMPLEMODE_RANDOMBOX 1
# define LABEL_SAMPLEMODE_SUBSAMPPROB 2
# define LABEL_SAMPLEMODE_GRID 3

/** Information ueber benutzten Speicher.  */
void mem_info(ostream&);

}; // namespace

#endif
