
/** confmat.cpp -- (c) Frank Hoeppner

Programm zur Berechnung der "confusion matrix" - Es wird /data/class
mit /data/cluster verglichen. Confusion Matrix, Anzahl der Daten und
Trefferrate werden in die /summary Kollektion geschrieben.

Als Anwendungsbeispiel: iris.ddl mit FCM clustern und zugeordnete
Cluster mit den Originalklassen vergleichen:
      confmat data=iris.ddl data/cluster=iris.fcm.tmp/data/cluster
oder  confmat data=iris.fcm.tmp data/class=iris.ddl/class
Die Uebereinstimmungsrate kann dann statistisch ausgewertet werden:
      stat data/value=iris.summary.ddl/summary/rate
*/

#include "dal.hpp"
#include "ddl.hpp"

using namespace Data_Analysis_Library;

class DAA_Frequency
  : public DAA_Algorithm
  {
  public:
    DAA_Frequency(iterator_type*,iterator_type*);
    virtual void operator()();

  protected:
    iterator_type *mp_data_iter,*mp_summ_iter;
    // data
    int_type m_class;
    int_type m_cluster;
    // summ
    int_matrix_type m_confusion;
    int_type m_samples;
    real_type m_rate;
  };


DAA_Frequency::DAA_Frequency
  (
  iterator_type *ap_data_iter,
  iterator_type *ap_summ_iter
  )
  : mp_data_iter(ap_data_iter)
  , mp_summ_iter(ap_summ_iter)
  // data
  , m_class(CLASS,DAL_MODE_READOUT,-1)
  , m_cluster(CLUSTER,DAL_MODE_READOUT,-1)
  // summ
  , m_confusion(CONFUSION,DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  , m_samples(SAMPLES,DAL_MODE_WRITEBACK|DAL_MODE_SHARED,0)
  , m_rate(RECOGRATE,DAL_MODE_WRITEBACK|DAL_MODE_SHARED,0.0)
  {
  // data
  mp_data_iter->attach(&m_class);
  mp_data_iter->attach(&m_cluster);
  // summ
  mp_summ_iter->attach(&m_confusion);
  mp_summ_iter->attach(&m_samples);
  mp_summ_iter->attach(&m_rate);
  }

void 
DAA_Frequency::operator()
  (
  )
  {
  int max_class(0),max_cluster(0);
  m_samples() = 0;
  m_confusion().adjust(0,0);

  for_all_in_any_level(*mp_data_iter)
    {
      /** Die Groesse der Confusion Matrix muss staendig ermittelt und
          angepasst werden, da wir erst beim Durchlaufen durch die
          Datei sehen, wieviele Cluster/Classes es gibt. */
      if (m_class()!=-1)
        max_class = max(max_class,m_class());
      if (m_cluster()!=-1)
        max_cluster = max(max_cluster,m_cluster());
      m_confusion().adjust(max_class+1,max_cluster+1);

      /** Ein Eintrag in die Confusion Matrix erfolgt allerdings nur,
          wenn sowohl class als auch cluster vorhanden sind. */
      if ((m_class()!=-1) && (m_cluster()!=-1))
        { 
          ++m_confusion()(m_class(),m_cluster()); 
          ++m_samples();
        }
    }

  /** Zur Bestimmung der Trefferrate schlagen wir jedes cluster
      derjenigen Klasse zu, bei der wir am meisten Treffer hatten. */
  m_rate() = 0;
  for (int i=0;i<=max_cluster;++i)
    {
    int maxfit(0);
    for (int j=0;j<=max_class;++j) 
      { maxfit = max(maxfit,m_confusion()(j,i)); }
    m_rate() += maxfit;
    }
  if (m_samples()>0) m_rate() /= m_samples();
  ++(*mp_summ_iter);
  }

int main(int argc, char** argv)
  {
  InitialLog Entry("dal.id","dal.log");
  INFO("executing freq");

  iterator data(DATA),summ(SUMMARY);

  evaluation_list_type iterlist;
  iterlist.push_back(evaluation_type(DATA,0,0,&data));
  iterlist.push_back(evaluation_type(SUMMARY,0,0,&summ));

  evaluate_cmdline(argc,argv,iterlist);
  DAA_Frequency freq(&data,&summ); 
  //write_status(*gp_trace);
  call_algorithm(iterlist,freq);
  }
