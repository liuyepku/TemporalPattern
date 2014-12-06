
/** \file copy.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Copy or convert a table.

    This simple program may be useful to extract fields or combine
    fields from different files into a single one. It can also be used
    to just convert between the different file formats. 

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
  INFO("copy ("<<PACKAGE<<' '<<VERSION<<')');

  SubSetTable data(cMASTER,"data");
  tuple_type value(&data,cREADOUT|cWRITEBACK|cSHARED,"value");

  evaluate_cmdline(argc,argv);                          // process user demands

  Algorithm *p = seq(fetch(&data),
		     writeback(&data));

  data.init();
  iterate_algorithm(&p);
  p->loop();
  data.close();
  };

#endif // EXCLUDE_FROM_TUTORIAL
