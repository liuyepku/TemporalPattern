
/** \file cmat.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Build a confusion matrix.


    Recognized fields:
    <ul>

    <li> \c <data/value> (numeric, in/out) : This is the only field
    that is copied by default. If you want to copy other fields,
    specify them on the command line with the \c :c modifier. </li>

    </ul>

*/

#ifndef EXCLUDE_FROM_TUTORIAL

#include "dal.hpp"
#include "algorithm.hpp"

using namespace Data_Analysis_Library;

int main
  (
  int argc, 
  char** argv
  )
  {
  LOGTRACE_INIT("dal.log","dal.id");                    // initialize debugging
  init_global();                                 // initialize global variables
  INFO("cmat ("<<PACKAGE<<' '<<VERSION<<')');

  SubSetTable data(cMASTER,"data");
  int_type xattr(&data,cREADOUT|cSHARED,"x",gp_dm_labels);
  int_type yattr(&data,cREADOUT|cSHARED,"y",gp_dm_labels);

  SubSetTable summ(cMASTER,"summ");
  int_tuple_type xlabels(&summ,cWRITEBACK|cSHARED,"xlabels",gp_dm_labels);
  int_tuple_type ylabels(&summ,cWRITEBACK|cSHARED,"ylabels",gp_dm_labels);
  int_matrix_type cmat(&summ,cWRITEBACK|cSHARED,"confmat",gp_dm_count);
  real_type rate(&summ,cWRITEBACK|cSHARED,"rate",gp_dm_numeric);

  xattr.set_context_default("NONE");
  yattr.set_context_default("NONE");
  evaluate_cmdline(argc,argv);                          // process user demands

  Algorithm *p = seq(embed_loop(fetch(&data),
				confmat(&xlabels,&ylabels,&cmat,
					&xattr,&yattr,&rate)),
		     writeback(&summ));

  data.init(); summ.init();
  iterate_algorithm(&p);
  p->loop();
  data.close(); summ.close();
  };

#endif // EXCLUDE_FROM_TUTORIAL
