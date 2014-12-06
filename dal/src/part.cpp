
/*

Partitionierung von Daten

=> Einlesen SUMMARY und DATA

=> fuer alle Achsen in DATA: SUMMARY (landmarks) lesen
   [-oo:-0.234554)
   [-0.234554:-1.23)

=> Wandeln eines Zahlentupels in ein label
   (2 4 5) => [0:2]x[
 07 Feb 2001 first version

*/

#include "dal.hpp"
#include "matvecop.hpp"
#include "ddl.hpp"
#include <strstream.h>

using namespace Data_Analysis_Library;

id_type LANDMARKS  ("landmarks",DAL_TUPLE,SUMMARY);
id_type LANDLABEL  ("landlabel",DAL_INT_TUPLE,DATALABELS,SUMMARY);

class DAA_Partition
  : public DAA_Algorithm
  {
  public:
    DAA_Partition(iterator_type*,iterator_type*);
    virtual void operator()();
  private:
    iterator_type *mp_summ_iter,*mp_data_iter;
    // data
    tuple_type m_value;
    int_type m_label;
    // summ
    tuple_type m_landmarks;
    int_tuple_type m_landlabel;
  };


DAA_Partition::DAA_Partition
  (
  iterator_type *ap_summ_iter,
  iterator_type *ap_data_iter
  )
  : mp_summ_iter(ap_summ_iter)
  , mp_data_iter(ap_data_iter)
  // data
  , m_value(DVALUE,DAL_MODE_READOUT|DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  , m_label(DLABEL,DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  // summ
  , m_landmarks(LANDMARKS,DAL_MODE_READOUT)
  , m_landlabel(LANDLABEL,DAL_MODE_READOUT)
  {
  // data
  mp_data_iter->attach(&m_value);
  mp_data_iter->attach(&m_label);
  // summ
  mp_summ_iter->attach(&m_landmarks);
  mp_summ_iter->attach(&m_landlabel);
  }

void 
DAA_Partition::operator()
  (
  )
  {
    /** Landmarks und -Label auslesen, merken (buffered_iterator) */
    int datadim(0);
    for_all_in_any_level(*mp_summ_iter) if (!mp_summ_iter->empty())
      {
      ++datadim;
      }

    /** Bei 1D-Daten keine neuen Label erzeugen... */
    if (datadim==1)
      {

      mp_summ_iter->rewind();
      while (mp_summ_iter->empty()) ++(*mp_summ_iter);

      for_all_in_any_level(*mp_data_iter) if (!mp_data_iter->empty())
        {
        invariant(m_value().rows()==1,"assume 1D data",SOURCELOC);
        int i(0);
        while ((i<m_landmarks().rows())&&(m_value()[0] > m_landmarks()[i]))
          ++i;
        m_label() = m_landlabel()[i];
        }

      }
    else
    /** Bei hoeherdimensionalen Daten setzen wir die Label aus den
        einzelnen Labeln zusammen (Kreuzprodukt der Label). */

    /** Auf alle Daten anwenden */
    for_all_in_any_level(*mp_data_iter) if (!mp_data_iter->empty())
      {
        strstream label;
        int d(0);
        mp_summ_iter->rewind();
        for_all_in_any_level(*mp_summ_iter) if (!mp_summ_iter->empty())
          {
          int i(0);
          while ((i<m_landmarks().rows())&&(m_value()[d] > m_landmarks()[i]))
            ++i;
          if (d>0) label << '_';
          m_landlabel.m_io.write(label,m_landlabel()[i]);
          ++d;
          }
        label << '\0';
        m_label() = announce_label(label.str(),DATALABELS); 
      }
  } // method


int main(int argc, char** argv)
  {
  InitialLog Entry("dal.id","dal.log");
  INFO("executing part");

  iterator data(DATA);
  buffered_iterator summ(SUMMARY);

  evaluation_list_type iterlist;
  iterlist.push_back(evaluation_type(DATA,0,0,&data));
  iterlist.push_back(evaluation_type(SUMMARY,0,0,&summ));

  evaluate_cmdline(argc,argv,iterlist);
  DAA_Partition part(&summ,&data); 
  call_algorithm(iterlist,part);
  }
