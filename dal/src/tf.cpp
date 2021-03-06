
/* umbennenn in norm, beispiel fuer hierarchie-anwendung:

einmal alles normieren (etwa zeitreihe)
einmal partitionieren und dann stueckweise normieren (dazu fuer jedes stueck stat aufrufen, segmentierte summ+data collektionen */


#include "dal.hpp"
#include "matvecop.hpp"
#include "ddl.hpp"

using namespace Data_Analysis_Library;
#include "cells.hpp" // write_status

// label domain scale = normal|standard 
id_type TRANSFORM       ("transform",DAL_DOMAIN);
id_type  ZEROMEAN       ("zeromean",DAL_LABEL,TRANSFORM);
id_type  NORMAL         ("normal",DAL_LABEL,TRANSFORM);
id_type  UNITSCALE      ("unitscale",DAL_LABEL,TRANSFORM);
// argument of DAA_Transform
id_type TRANSFORMMETHOD ("transformation",DAL_INT,TRANSFORM,ARGS);

class DAA_Transform
  : public DAA_Algorithm
  {
  public:
    DAA_Transform(iterator_type *ap_data_iter,iterator_type *ap_summ_iter,iterator_type *ap_args_iter);
    virtual void operator()();
  private:
    iterator_type *mp_data_iter,*mp_summ_iter,*mp_args_iter;
    // summary
    tuple_type m_minimum,m_maximum,m_mean,m_variance; 
    // data
    tuple_type m_value;
    // args
    int_type m_transform;
  };


DAA_Transform::DAA_Transform
  (
  iterator_type *ap_data_iter,
  iterator_type *ap_summ_iter,
  iterator_type *ap_args_iter
  )
  : mp_data_iter(ap_data_iter)
  , mp_summ_iter(ap_summ_iter)
  , mp_args_iter(ap_args_iter)
  // summ
  , m_minimum(MINIMUM,DAL_MODE_READOUT|DAL_MODE_SHARED)
  , m_maximum(MAXIMUM,DAL_MODE_READOUT|DAL_MODE_SHARED)
  , m_mean(MEAN,DAL_MODE_READOUT|DAL_MODE_SHARED)
  , m_variance(VARIANCE,DAL_MODE_READOUT|DAL_MODE_SHARED)
  // data
  , m_value(DVALUE,DAL_MODE_READOUT|DAL_MODE_WRITEBACK)
  // args
  , m_transform(TRANSFORMMETHOD,DAL_MODE_READOUT|DAL_MODE_SHARED,NORMAL())
  {
  // data
  mp_data_iter->attach(&m_value);
  // summ
  mp_summ_iter->attach(&m_minimum);
  mp_summ_iter->attach(&m_maximum);
  mp_summ_iter->attach(&m_mean);
  mp_summ_iter->attach(&m_variance);
  // args
  mp_args_iter->attach(&m_transform);
  }

void 
DAA_Transform::operator()
  (
  )
  {
    /** Laenge und Variance "sicher" machen. */
    tuple_type::value_type length;
    matrix_copy(length,m_maximum());
    matrix_dec(length,m_minimum());
    for (int i=0;i<length.rows();++i) 
      {
      if (length[i]==0) length[i]=1;
      if (m_variance()[i]==0) m_variance()[i]=0.0001;
      }

    /** Auswertungsmodus bestimmen. */
    int mode;
#   define MODE_ZEROMEAN 0
#   define MODE_NORMAL 1
#   define MODE_UNITSCALE 2
#   define MODE_UNTOUCHED 3
    if (m_transform()==ZEROMEAN()) mode = MODE_ZEROMEAN; else
    if (m_transform()==NORMAL()) mode = MODE_NORMAL; else
    if (m_transform()==UNITSCALE()) mode = MODE_UNITSCALE; else
      { 
      mode = MODE_UNTOUCHED;
      ERROR(cerr << "unknown scaling method. Data untouched.");
      }

    /** Fuer jeden Wert die Skalierung durchfuehren. */
    for_all_in_any_level(*mp_data_iter) if (!mp_data_iter->empty())
      {
      switch (mode)
        {
        case MODE_ZEROMEAN :
          matrix_dec(m_value(),m_mean());
          break;
        case MODE_UNITSCALE :
          matrix_dec(m_value(),m_minimum());
          for (int i=0;i<m_value().rows();++i) m_value()[i] /= length[i];
          break;
        case MODE_NORMAL :
          matrix_dec(m_value(),m_mean());
          for (int i=0;i<m_value().rows();++i) m_value()[i] /= m_variance()[i];
          break;
        case MODE_UNTOUCHED : 
          break;
        }
      }

    /** Summary "verbrauchen" */
    ++(*mp_summ_iter);
  }


int main(int argc, char** argv)
  {
  InitialLog Entry("dal.id","dal.log");
  INFO("executing tf");

  iterator data(DATA),summ(SUMMARY),args(ARGS);

  evaluation_list_type iterlist;
  iterlist.push_back(evaluation_type(DATA,0,0,&data));
  iterlist.push_back(evaluation_type(SUMMARY,0,0,&summ));
  iterlist.push_back(evaluation_type(ARGS,0,0,&args));

  evaluate_cmdline(argc,argv,iterlist);
  DAA_Transform trans(&data,&summ,&args); 
  //write_status(cout);
  call_algorithm(iterlist,trans);
  }
