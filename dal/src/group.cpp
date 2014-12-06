
/** \file group.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Group records by their labels in a hierarchy.

    For all labels \c l of the field \c <data/label> a \c <data>
    subset is generated. In the execution mode \c args/mode=group all
    records in a subset have the same label \c l, for \c
    args/mode=separate all records in a subset have any label but \c
    l.

    Recognized fields:
    <ul>

    <li> \c <args/mode> (in) : Execution mode (see above). </li>

    <li> \c <data/label> (labels, in/out) : Label that is used to
    group or separate the records. </li>

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
  INFO("executing group");

  MasterTable args(cMASTER,"args");                           // argument table
  int_type mode(&args,cREADOUT,"mode",gp_dm_boolean);
  gp_dm_boolean->add("group",1);
  gp_dm_boolean->add("separate",0);

  HMasterTable data(cMASTER,"data");                              // data table
  int_type label(&data,cREADOUT|cWRITEBACK,"label",gp_dm_labels);
  
  MasterTable summ(cMASTER,"summ");                            // summary table
  int_type count(&summ,cVOID,"samples",gp_dm_count);

  int_type selected(NULL,cSHARED,"currentlabel");         // temporary variable

  mode.set_context_default("group");                // default mode: group data
  evaluate_cmdline(argc,argv);                          // process user demands

  Algorithm *p =
      seq(embed_loop(fetch(&data,&count)),  // determine count, read all labels
	  embed_loop(fetch(gp_dm_labels,&selected),
		     embed_loop(fetch(&data,&count),
				indent(&data),
				select(&label,&selected,&mode,
				       writeback(&data)))));

  data.init(); args.init(); summ.init();
  iterate_algorithm(&p);
  p->loop();
  data.close(); args.close(); summ.close();
  };

#endif // EXCLUDE_FROM_TUTORIAL

#ifdef X

class DAA_Group
  : public DAA_Algorithm
  {
  public:
    DAA_Group(iterator_type*,iterator_type*);
    virtual void operator()();
  private:
    iterator_type *mp_data_iter,*mp_args_iter;
    // data
    tuple_type m_input;
    real_type m_time;
    tuple_type m_period;
    int_type m_label;
    // args
    int_type m_groupmode;
  };


DAA_Group::DAA_Group
  (
  iterator_type *ap_data_iter,
  iterator_type *ap_args_iter
  )
  : mp_data_iter(ap_data_iter)
  , mp_args_iter(ap_args_iter)
  // data
  , m_input(DVALUE,DAL_MODE_READOUT|DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  , m_time(DTIME,DAL_MODE_READOUT|DAL_MODE_SHARED)
  , m_period(DPERIOD,DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  , m_label(DLABEL,DAL_MODE_READOUT|DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  // args
  , m_groupmode(GROUP,DAL_MODE_READOUT,BYLABEL())
{
  // data
  mp_data_iter->attach(&m_input);
  mp_data_iter->attach(&m_label);
  mp_data_iter->attach(&m_time);
  mp_data_iter->attach(&m_period);
  // args
  mp_args_iter->attach(&m_groupmode);
}

void 
DAA_Group::operator()
  (
  )
{
  string sign; ///< string fuer aktuelle Vorzeichen
  int label=-1; ///< letztes label
  int dim=-1; ///< Datendimension

  mp_data_iter->inc_level(); ///< neuen Hierarchie-Level beginnen

  for_all_in_any_level(*mp_data_iter) if (!mp_data_iter->empty())
  {
    /** Anfangsbelegung Dimension, sign-String in Laenge justieren */
    if (dim==-1) 
    {
      dim=m_input().rows();
      sign.erase();
      for (int i=0;i<dim;++i) sign += ' ';
    }

    /** BYSIGN: Label erstellen, das Wechsel in den Vorzeichen
        beschreibt */
    if (m_groupmode()==BYSIGN())
    {
      INVARIANT(dim==m_input().rows(),"dim="<<dim);
      for (int i=0;i<dim;++i) 
      {
        switch (signum(m_input()[i]))
        {
	  case -1 : sign[i] = '-'; break;
	  case  0 : sign[i] = 'o'; break;
	  case  1 : sign[i] = '+'; break;
	}
      }
      m_label() = announce_label(sign,DATALABELS);
    }

    /** Anfangsbelegung Label */
    if (label==-1) label=m_label();

    /** Bei einem Wechsel im Label Collection beenden und neue
        Level-1-Collection beginnen. */
    if (label!=m_label())
    {
      mp_data_iter->dec_level();
      mp_data_iter->inc_level();
    }
    label = m_label();

  }

  /** Und wieder den alten Level restaurieren. */
  mp_data_iter->dec_level();

} // operator()


int main(int argc, char** argv)
{
  InitialLog Entry("dal.id","dal.log");
  INFO("executing group");

  iterator data(DATA),args(ARGS);

  evaluation_list_type iterlist;
  iterlist.push_back(evaluation_type(DATA,0,0,&data));
  iterlist.push_back(evaluation_type(ARGS,0,0,&args));

  evaluate_cmdline(argc,argv,iterlist);
  DAA_Group group(&data,&args); 
  call_algorithm(iterlist,group);
}
#endif X
