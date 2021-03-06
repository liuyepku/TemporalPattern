
/** \file sample.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Data subsampling.

    Reads a \c <data> collection of size \c n and selects \c
    <args/count> records to be included in the subsampled dataset. The
    ordering of the records is prevailed.

    Recognized fields:
    <ul>

    <li> \c <data/included> (boolean, out) : Boolean field, 1 if
    record is included in selection, 0 otherwise. </li>

    <li> \c <args/count> (count, in) : Number of records to be
    contained in the randomly subsampled dataset. Must be smaller than
    the total number of records. </li>

    <li> \c <args/subset> (boolean, in) : Boolean field, default value
    is true. In the default case only the subset is contained in the
    output data table. Otherwise, the output table contains the whole
    input table, the \c <data/included> field can then be used to
    decide wether a record is in the selection or not. </li>

    <li> \c <summ/total> (count, in/out) : Total number of records in
    input table. Will be determined automatically if not provided.

    </ul>

    Example:
    sample "data<iris.ddl data/value:c args/count=44"

*/

//TODO: normal distribution (mean,variance, mixture via args(*))
//      uniform distribution (minimum,maximum)

#ifndef EXCLUDE_FROM_TUTORIAL

#include "dal.hpp"
#include "algorithm.hpp"

using namespace Data_Access_Library;
using namespace Data_Analysis_Library;

int main(
    int argc, 
    char** argv
    )
{
    LOGTRACE_INIT("dal.log","dal.id");                  // initialize debugging
    init_global();                               // initialize global variables
    INFO("sample ("<<PACKAGE<<' '<<VERSION<<')');
    
    SubSetTable data(cMASTER,"data");
    int_type included(&data,cREADOUT|cWRITEBACK,"included",gp_dm_boolean);

    SubSetTable args(cMASTER,"args");
    int_type count(&args,cREADOUT,"count",gp_dm_count);
    int_type writesubsetonly(&args,cREADOUT,"subset",gp_dm_boolean);

    SubSetTable summ(cMASTER,"summ");
    int_type total(&summ,cREADOUT,"count",gp_dm_count);

    writesubsetonly.set_context_default("true");            // default settings
    evaluate_cmdline(argc,argv);                        // process user demands

    Algorithm *p = 
	seq(embed_loop_ifndef(&total,          // if <summ/total> not specified
			      fetch(&data,&total)),       // then count records
	    embed_loop(fetch(&data),
		       koutofn(&writesubsetonly,&count,&total,&included,
			       writeback(&data))));

    data.init(); args.init(); summ.init();
    iterate_algorithm(&p);
    p->loop();                                                // alg. execution
    data.close(); args.close(); summ.close();
};

#endif // EXCLUDE_FROM_TUTORIAL





#ifdef OOOOLDDD

  gp_dm_samplemode = new Domain("samplemode");           ///< Modi von "sample"
  gp_dm_samplemode->add("random",LABEL_SAMPLEMODE_RANDOMBOX);
  gp_dm_samplemode->add("prob",LABEL_SAMPLEMODE_SUBSAMPPROB);
  gp_dm_samplemode->add("subsample",LABEL_SAMPLEMODE_SUBSAMP);
  gp_dm_samplemode->add("grid",LABEL_SAMPLEMODE_GRID);


#include "dal.hpp"
#include "globdef.hpp"
#include "Box.cpp"

using namespace Data_Access_Library;

class Sampling
//  : public Algorithm
{
public:
  Sampling(iterator_type*,iterator_type*,iterator_type*);
  virtual void operator()();
protected:
  void sub_sampling();
  void sub_sampling_prob();
  void random_box_sampling();
  void grid_sampling();
private:
  iterator_type *mp_data_iter,*mp_summ_iter,*mp_args_iter;
  // data
  tuple_type m_value;
  // summ
  tuple_type m_minimum,m_maximum;
  int_type m_count;
  // args
  real_type m_takeoverprob;
  int_type m_howmany;
  int_type m_sampmode;
};


Sampling::Sampling
  (
  iterator_type *ap_data_iter, 
  iterator_type *ap_summ_iter,
  iterator_type *ap_args_iter
  )
  : mp_data_iter(ap_data_iter)
  , mp_summ_iter(ap_summ_iter)
  , mp_args_iter(ap_args_iter)
  // data
  , m_value(mp_data_iter,cREADOUT|cWRITEBACK|cSHARED,"value")
  // summ
  , m_minimum(mp_summ_iter,cREADOUT,"minimum")
  , m_maximum(mp_summ_iter,cREADOUT,"maximum")
  , m_count(mp_summ_iter,cREADOUT,"samples")
  // args
  , m_takeoverprob(mp_args_iter,cREADOUT|cSHARED,"takeoverprob")
  , m_howmany(mp_args_iter,cREADOUT|cSHARED,"samples")
  , m_sampmode(mp_args_iter,cREADOUT|cSHARED,"mode",gp_dm_samplemode)
  {
  m_sampmode.set_context_default("subsample");
  m_howmany.set_context_default("10");
  m_takeoverprob.set_context_default("0.05");
  }

void 
Sampling::operator()
  (
  )
  {
  LOG_METHOD("","Sampling","operator()","");

  /** Neue Argumente und Summary laden. */
  mp_args_iter->pop();
  mp_summ_iter->pop();

  switch (m_sampmode())
    {
    case LABEL_SAMPLEMODE_RANDOMBOX : random_box_sampling(); break;
    case LABEL_SAMPLEMODE_SUBSAMPPROB : sub_sampling_prob(); break;
    case LABEL_SAMPLEMODE_SUBSAMP : sub_sampling(); break;
    case LABEL_SAMPLEMODE_GRID : grid_sampling(); break;
    default : ERROR("unknown sample mode");
    }
  }

/** Lies Hyperbox (min/max) aus args collection, erzeuge Zufallsdaten
    gleichmaessig in der Hyperbox verteilt. */

void 
Sampling::random_box_sampling
  (
  )
  {
  LOG_METHOD("","Sampling","random_box_sampling","");

  /** Zufaellige Auswahl aus der umschliessenden Hyperbox */
  if (m_minimum().rows()>0)
    {
    Box<tuple_type::value_type> bbox;
    bbox.adjust(0);
    bbox.include(m_minimum());
    bbox.include(m_maximum());

    // Vaterknoten erzeugen (???WARUM HIER HIERARCHIE UND ANDERSWO NICHT???)
    //    mp_data_iter->push();
    //    mp_data_iter->inc_level();
    // Soehne durch random box uniform sampling
    for (int i=0;i<m_howmany();++i)
      {
      matrix_copy(m_value.var(),bbox.random());
      mp_data_iter->push();
      }
    //    mp_data_iter->dec_level();
    }
  }

/** Laufe einmal durch data collection, entscheide fuer jedes Datum
    mit Wahrscheinlichkeit <takeoverprob> ueber die Uebernahme in die
    Ausgabe. */

void 
Sampling::sub_sampling_prob
  (
  )
  {
  LOG_METHOD("","Sampling","sub_sampling_prob","");

  read_all_in_any_level(*mp_data_iter) if (m_value().rows()>0)
    {
    if ( fabs(1.0*rand()/(RAND_MAX+1)) <= m_takeoverprob() ) 
      mp_data_iter->push();
    }
  }


/** Erzeuge eine List von HOWMANY verschiedenen Indices aus [1:Anzahl
    Daten]. Lauf einmal durch data collection und schreibe die vorher
    zufaellig ausgewaehlten Daten raus, ignoriere den Rest. */

void 
Sampling::sub_sampling
  (
  )
  {
  LOG_METHOD("","Sampling","sub_sampling","");

  PRECONDITION(m_count()>0,"non-empty data");
  const int size = min(m_count(),m_howmany());
  PRECONDITION(size>0,"non-empty sample");
  list<int> selection;

  /** Auswahl der Daten */
  while (selection.size()!=(word)size)
    {
    selection.push_back(static_cast<int>(1.0*rand()/RAND_MAX*m_count()));
    selection.sort();
    selection.unique();
    }
  cout << selection << endl;
  /** Schreiben der ausgewaehlten Daten */
  int i=0;
  list<int>::const_iterator s=selection.begin();
  read_all_in_any_level(*mp_data_iter) if (m_value().rows()>0)
    {
    if ((s!=selection.end()) && (i==*s)) 
      {
      mp_data_iter->push();
      ++s;
      }
    ++i;
    }
  }

/** Lies Hyperbox (min/max) aus args collection, erzeuge
    gleichmaessiges Grid in der Hyperbox */

void 
Sampling::grid_sampling
  (
  )
  {
  LOG_METHOD("","Sampling","grid_sampling","");

  if ((m_minimum().rows()>0) && (m_howmany()>0))
    {
    const int dim = m_minimum().rows();

    /** Anzahl der Daten n pro Dimension (insgesamt also n^dim)
        festlegen, wobei n^dim < m_samples gelten soll */
    int samp_per_dim = (int) (exp(log(m_howmany())/dim)+0.5);
    int samp_total = (int) (pow(samp_per_dim,dim)+0.5); 
    if (samp_total > m_howmany()) 
    { --samp_per_dim; samp_total=static_cast<int>(pow(samp_per_dim,dim)); }

    /** Offset in den Achsen berechnen */
    tuple_type::value_type offset;
    offset.adjust(dim); 
    for (int i=0;i<dim;++i) 
      {
      offset[i]=(m_maximum()[i]-m_minimum()[i])/(samp_per_dim-1.0);
      m_maximum.var()[i] += offset[i]/2.0;
      }

    /** Grid erzeugen */
    mp_data_iter->inc_level();
    bool again; 
    int d=1;
    m_value.var().adjust(dim); 
    matrix_set(m_value.var(),m_minimum());
    for (int i=0;i<samp_total;++i)
    { 
      // "Einruecken" fuer jede Iteration in 1. Dimension
      if (d>0) mp_data_iter->inc_level(); 

      mp_data_iter->push();

      d=0; 
      do
      {
        m_value.var()[d] += offset[d];
        again = (m_value()[d] > m_maximum()[d]);
        if (again)
	  { m_value.var()[d]=m_minimum()[d]; ++d; again=true; }
      }
      while ((again) && (d<dim));

      // "Einruecken" nach jeder Iteration in 1. Dimension
      if (d>0) mp_data_iter->dec_level();
    }
    mp_data_iter->dec_level();

    } // if 
  } // method

int main(int argc, char** argv)
  {
  LOGTRACE_INIT("dal.log","dal.id");
  init_global();
  INFO("executing sample");

  HMasterTable data(cMASTER,"data");
  HMasterTable summ(cMASTER,"summ");
  HMasterTable args(cMASTER,"args");
/*
  evaluation_list_type iterlist;
  iterlist.push_back(evaluation_type(DATA,0,0,&data));
  iterlist.push_back(evaluation_type(SUMMARY,0,0,&summ));
  iterlist.push_back(evaluation_type(ARGS,0,0,&args));
*/
  Sampling sample(&data,&summ,&args); // iteratoren init
  evaluate_cmdline(argc,argv/*,iterlist*/); // umbiegen
  sample();
/*
  call_algorithm(iterlist,sample);
*/
  }

#endif
