
/*

 Histogram

 DATA R/W   <value> als real_tuple lesen und als <discretevalue> schreiben
 SUMM R/W   <min/max> einlesen, ein n-Tuple <samples> und
            ein (n+1)-Tuple <range>
 ARGS R     mode EQUIDIST, EQUISAMPLE

  spaeter dann meinetwegen auch QUARTILES => dazu sortieren um
  Bucketgrenzen festlegen zu koennen.

 2001 Feb 07   first version
 2001 Feb 19   changed resolution in fraction
 2001 Feb 28   bug in calculation of fraction

*/

#include <stdio.h> // sprintf
#include "dal.hpp"
#include "matvecop.hpp"
#include "ddl.hpp"

using namespace Data_Analysis_Library;

id_type LANDMARKS  ("landmarks",DAL_TUPLE,SUMMARY);
id_type LANDLABEL  ("landlabel",DAL_INT_TUPLE,DATALABELS,SUMMARY);
id_type HISTOGRAM  ("histogram",DAL_INT_TUPLE,SUMMARY);

id_type PARTITION  ("partition",DAL_DOMAIN);
id_type  EQUIDATA  ("equidata",DAL_LABEL,PARTITION);
id_type  EQUILENGTH("equilength",DAL_LABEL,PARTITION);
id_type  MERGE     ("merge",DAL_LABEL,PARTITION);
id_type PARTMODE   ("partition",DAL_INT,PARTITION,ARGS);

//NATURAL: 2,3,5,7
//TODO: OPTION: ALLE ACHSEN SELBER EINTEILUNG
//LABELSATZ? small,medium,large

id_type FRACTION   ("fraction",DAL_TUPLE,ARGS);
id_type MAXBUCKETS ("maxbucket",DAL_INT,ARGS);

class DAA_Histogram
  : public DAA_Algorithm
  {
  public:
    DAA_Histogram(iterator_type*,iterator_type*,iterator_type*);
    virtual void operator()();
  private:
    iterator_type *mp_data_iter,*mp_summ_iter,*mp_args_iter;
    // data
    tuple_type m_value;
    // summ
    int_type m_samples;
    tuple_type m_minimum;
    tuple_type m_maximum;
    tuple_type m_landmarks;
    int_tuple_type m_histogram;
    int_tuple_type m_landlabel;
    // args
    tuple_type m_fraction;
    int_type m_maxbuckets;
    int_type m_mode;
  };


DAA_Histogram::DAA_Histogram
  (
  iterator_type *ap_data_iter,
  iterator_type *ap_summ_iter,
  iterator_type *ap_args_iter
  )
  : mp_data_iter(ap_data_iter)
  , mp_summ_iter(ap_summ_iter)
  , mp_args_iter(ap_args_iter)
  // data
  , m_value(DVALUE,DAL_MODE_READOUT|DAL_MODE_SHARED)
  // summ
  , m_samples(SAMPLES,DAL_MODE_READOUT|DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  , m_minimum(MINIMUM,DAL_MODE_READOUT|DAL_MODE_SHARED)
  , m_maximum(MAXIMUM,DAL_MODE_READOUT|DAL_MODE_SHARED)
  , m_landmarks(LANDMARKS,DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  , m_histogram(HISTOGRAM,DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  , m_landlabel(LANDLABEL,DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  // args
  , m_fraction(FRACTION,DAL_MODE_READOUT|DAL_MODE_SHARED)
  , m_maxbuckets(MAXBUCKETS,DAL_MODE_READOUT|DAL_MODE_SHARED,2000)
  , m_mode(PARTMODE,DAL_MODE_READOUT|DAL_MODE_SHARED,EQUIDATA())
  {
  // data
  mp_data_iter->attach(&m_value);
  // summ
  mp_summ_iter->attach(&m_samples);
  mp_summ_iter->attach(&m_minimum);
  mp_summ_iter->attach(&m_maximum);
  mp_summ_iter->attach(&m_landmarks);
  mp_summ_iter->attach(&m_histogram);
  mp_summ_iter->attach(&m_landlabel);
  // args
  mp_args_iter->attach(&m_fraction);
  mp_args_iter->attach(&m_maxbuckets);
  mp_args_iter->attach(&m_mode);
  }

void 
DAA_Histogram::operator()
  (
  )
  {
  int datadim(-1);
  int_matrix_type::value_type freqtable;

  /** Die Anzahl der Buckets: Nicht groesser als die Haelfte der
      Datenanzahl (sonst waere es ueberdimensioniert.) */
  invariant(m_samples()>0,"samples>0 (invalid summary?)");
  int buckets( min(m_maxbuckets(),2*m_samples()) );

  for_all_in_any_level(*mp_data_iter) if (!mp_data_iter->empty())
    {
      /** Initialisierung */
      if (datadim==-1)
	{
        datadim = m_value().rows();
        freqtable.adjust(buckets,datadim);
        if ((m_minimum().rows() != datadim) || (m_maximum().rows()!=datadim))
	  { WARNING("data dimensions mismatches summary dimension"); exit(-1); }
	}
      invariant(m_value().rows()==datadim,"assume constant data dimension");

      /** Buckets sind aequidistant ueber den Bereich min-max verteilt. */
      for (int d=0;d<datadim;++d)
	{
        float x( (m_value()[d]-m_minimum()[d]) / (m_maximum()[d]-m_minimum()[d]) );
        int b( (int)( 0.5 + x * (buckets-1) ) );
        ++freqtable(b,d);
	}
    }

  /** EQUIDATA: Von links nach recht durchlaufen, Abschnittsgrenze
      einfuegen, wenn mittlere Datenanzahl erreicht oder
      ueberschritten. */
  if (m_mode()==EQUIDATA())
    {

    /** Normierung der Bruchteile fuer die gewuenschte Partitionierung. */
      {
      float sum(0);
      int i;
      for (i=0;i<m_fraction().rows();++i) sum += m_fraction()[i];
      for (i=0;i<m_fraction().rows();++i) m_fraction()[i] *= m_samples()/sum;
      if (m_fraction().rows()<=1)
        { WARNING("fraction vector not meaningful (rows<=1)"); exit(-1); }
      }

    mp_summ_iter->inc_level();
    for (int d=0;d<datadim;++d)
      {
      m_landmarks().adjust(m_fraction().rows()-1);
      m_landlabel().adjust(m_fraction().rows());
      m_histogram().adjust(m_fraction().rows());
      matrix_set_scalar(m_histogram(),0);
      int n(0),sum(0);
      float avg = m_fraction()[n];
 
      for (int i=0;i<buckets;++i)
        {
	  /** Vergleich: Weichen wir vom avg mehr ab, wenn wir das
              naechste Datum der linken oder der rechten Seite
              zuschlagen? (Wenn wir beim letzten Bucket sind, dann
              brauchen wir uns ueber die Aufteilung keine Gedanken
              mehr machen (2. Bedingung)) */
          if (sum+freqtable(i,d)-avg > avg-sum)
	    {
            m_histogram()[n]=sum;
            if (n<m_fraction().rows()-1) m_landmarks()[n] = 
                m_minimum()[d]+(m_maximum()[d]-m_minimum()[d])/buckets*i;
            ++n; 
            if (n<m_fraction().rows()-1) avg = m_fraction()[n];
            sum=0;
            }
          sum+=freqtable(i,d);
	}
      m_histogram()[ m_fraction().rows()-1 ] += sum; // Restdaten

      // Label erstellen
      {
        char name[25]; 
        for (n=0;n<m_fraction().rows();++n)
	  {
          if (n==0)
            sprintf(name,"[-oo:%.2f)",m_landmarks()[n]); 
          else if (n==m_fraction().rows()-1)
            sprintf(name,"[%.2f:oo]",m_landmarks()[n-1]);
          else
            sprintf(name,"[%.2f:%.2f)",m_landmarks()[n-1],m_landmarks()[n]);
          m_landlabel()[n] = announce_label(name,DATALABELS);
	  }
      }
      mp_summ_iter->push();
      }
    mp_summ_iter->dec_level();  
    } // mode=equidata

  /** MERGE: Schrittweise Intervalle vereinigen, bis gewuenschte
      Anzahl erreicht wurde. Es werden immer diejenigen, benachbarten
      Intervalle vereinigt, die gemeinsam die geringste Anzahl von
      Daten aufweisen. Wir deuten ein "geloeschtes" Intervall durch
      eine negative Sample-Zahl an. */
  if (m_mode()==MERGE())
    {
  for (int d=0;d<datadim;++d)
    {
    int n(buckets);
    while (n>m_fraction().rows())
      {

      int lastvalue,minindex(-1),minvalue(INT_MAX);
      int i(0); 
      while (freqtable(i,d)==-1) ++i;
      lastvalue=freqtable(i,d); ++i;
      for (;i<buckets;++i) if (freqtable(i,d)!=-1)
	{
        if (lastvalue+freqtable(i,d) < minvalue)
	  { minvalue=lastvalue+freqtable(i,d); minindex=i; }
        lastvalue=freqtable(i,d);
	}
      i=minindex; lastvalue=freqtable(i,d); freqtable(i,d)=-1;
      while (freqtable(i,d)==-1) --i;
      freqtable(i,d) += lastvalue;
      --n;
      }
    }

  /** Ausgabe: fuer jede Dimension eine SUMMARY Subcollection. Darin
      zwei Vektoren ausgeben: einmal die Intervallgrenzen (n-1) und
      die Daten im Interval (n). */
  mp_summ_iter->inc_level();
  for (int d=0;d<datadim;++d)
    {
    m_landmarks().adjust(m_fraction().rows()-1);
    m_histogram().adjust(m_fraction().rows());

    int n(0);
    for (int i=0;i<buckets;++i)
      {
      if (freqtable(i,d)!=-1) 
        { 
        m_histogram()[n]=freqtable(i,d); 
        m_landmarks()[n]=m_minimum()[d]+(m_maximum()[d]-m_minimum()[d])/buckets*i;
        ++n; 
        }
      }
    //m_landmarks()[n]=m_maximum()[d];
    mp_summ_iter->push();
    }
  mp_summ_iter->dec_level();
    } // MODE=MERGE

    /** Argumente verbrauchen */
    ++(*mp_args_iter);
  }


int main(int argc, char** argv)
  {
  InitialLog Entry("dal.id","dal.log");
  INFO("executing hist");

  iterator data(DATA),summ(SUMMARY),args(ARGS);

  evaluation_list_type iterlist;
  iterlist.push_back(evaluation_type(DATA,0,0,&data));
  iterlist.push_back(evaluation_type(SUMMARY,0,0,&summ));
  iterlist.push_back(evaluation_type(ARGS,0,0,&args));

  evaluate_cmdline(argc,argv,iterlist);
  DAA_Histogram copy(&data,&summ,&args); 
  call_algorithm(iterlist,copy);
  }
