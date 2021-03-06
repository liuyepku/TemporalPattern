
/** \file stat.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Simple statistics.

    Reads the \c <value> and \c <time> fields of the \c <data> input
    table and calculates the mean, variance, etc. of the values and
    the time period of the time points.

    Recognized \c <data> fields:
    <ul>

    <li> \c <data/value> (numeric, in) : Vector of numerical input
    data. </li>

    <li> \c <data/time> (timestamp, in) : Real number determining the
    point in time associated with this record. </li>

    <li> \c <data/weight> (numeric, in) : Real number determining the
    weight of the data record: A weight of 2.0 is equivalent to having
    the same record twice in the table. The default value is 1.0. </li>

    </ul>

    \c <summ> fields:
    <ul>

    <li> \c <summ/weightsum> (count, out) : Total sum of weights of
    records in input data table. (Number of records if
 all weights are
    equal to 1.0.) </li>

    <li> \c <summ/mean> (numeric, out) : Mean of \c <data/value>
    fields in input data table. (Weighted mean, corresponds to
    conventional mean if all weights are 1.0.) </li>

    <li> \c <summ/variance> (numeric, out) : Variance of \c
    <data/value> fields in input data table. (Weighted variance,
    corresponds to conventional variance if all weights are 1.0.)
    </li>

    <li> \c <summ/minimum> (numeric, out) : (Pointwise) minimum of \c
    <data/value> fields in input data table. </li>

    <li> \c <summ/minimum> (numeric, out) : (Pointwise) maximum of \c
    <data/value> fields in input data table. </li>

    <li> \c <summ/period> (timestamp, out) : Smallest interval that
    contains all \c <data/time> values in input data table. </li>

    </ul>

*/

#ifndef EXCLUDE_FROM_TUTORIAL

#include "dal.hpp"
#include "algorithm.hpp"

using namespace Data_Access_Library;
using namespace Data_Analysis_Library;

int main
  (
  int argc, 
  char** argv
  )
  {
  LOGTRACE_INIT("dal.log","dal.id");                    // initialize debugging
  init_global();                                 // initialize global variables
  INFO("stat ("<<PACKAGE<<' '<<VERSION<<')');

  SubSetTable data(cMASTER,"data");
  tuple_type value(&data,cREADOUT,"value",gp_dm_numeric);
  tuple_type time(&data,cREADOUT,"time",gp_dm_timestamp);
  real_type weight(&data,cREADOUT,"weight",gp_dm_numeric);

  SubSetTable summ(cMASTER|cSUBSETOUT,"summ");
  tuple_type minimum(&summ,cWRITEBACK,"minimum",gp_dm_numeric);
  tuple_type maximum(&summ,cWRITEBACK,"maximum",gp_dm_numeric);
  tuple_type mean(&summ,cWRITEBACK,"mean",gp_dm_numeric);
  tuple_type variance(&summ,cWRITEBACK,"variance",gp_dm_numeric);
  tuple_type period(&summ,cWRITEBACK,"period",gp_dm_timestamp);
  real_type weightsum(&summ,cWRITEBACK,"weightsum",gp_dm_numeric);
  tuple_type datasum(&summ,cVOID,"datasum",gp_dm_numeric);
  tuple_type datasqsum(&summ,cVOID,"datasqsum",gp_dm_numeric);

  weight.set_context_default("1");                          // default settings
  evaluate_cmdline(argc,argv);                          // process user demands

  Algorithm *p =                                     // algorithm instantiation
      seq(embed_loop(fetch(&data),
		     detrange(&value,&minimum,&maximum),
		     detrange(&time,&period),
		     weightedvar(&weight,&value,&weightsum,&datasum,
				 &datasqsum,&mean,&variance)),
	  writeback(&summ));

  data.init(); summ.init();
  iterate_algorithm(&p);
  p->loop();                                             // algorithm execution
  data.close(); summ.close();
  };

#endif // EXCLUDE_FROM_TUTORIAL
