
/** Table-Field-Conversion. 

    Sequenz von 2 Konvertierern (inconv + outconf)

    Akkumulierend (Komposition)
      Werte -> Vektor
      Vektor -> Spalten einer Matrix
      Vektor -> Zeilen einer Matrix
    Extrahierend (Projektion)
      Matrix -> Spalten einer Matrix
      Matrix -> Zeilen einer Matrix
      Vektor -> Werte

    Jeder Record 

      <data><key k><value n-VECTOR></data> 
 
    wird umgewandelt in
   
      <data><key k></data>
      <proj><key 0><parent k><value VECTOR[0]></proj>
      ...
      <proj><key n-1><parent k><value VECTOR[n-1]></proj>

    und umgekehrt. Die Konvertierungsrichtung wird von der Dimension
    der <value> Felder ab. */

#include "dal.hpp"
#include "matvecop.hpp"

using namespace Data_Access_Library;

class TableFieldConversion
  : public DAA_Algorithm
  {
  public:
    TableFieldConversion(table_type *,table_type *);
    virtual void operator()();
  private:
    table_type *mp_data,*mp_prot;
    tuple_type m_input;
    tuple_type m_output;
    real_type m_time;
  };


TableFieldConversion::TableFieldConversion
  (
  table_type *ap_data,
  table_type *ap_prot
  )
  : mp_data(ap_data)
  , mp_prot(ap_prot)
  // data
  , m_input(DVALUE,DAL_MODE_READOUT|DAL_MODE_SHARED)
  // prot
  , m_output(MVALUE,DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  , m_time(MTIME,DAL_MODE_WRITEBACK|DAL_MODE_SHARED)
  {
  // data
  mp_data->attach(&m_input);
  // prot
  mp_prot->attach(&m_output);
  mp_prot->attach(&m_time);
  }

void 
TableFieldConversion::operator()
  (
  )
  {
  enum { UNKNOWN,CREATETUPLE,CREATESET } state = UNKNOWN;
  int dim = 0;

  for_all_in_any_level(*mp_data) 
   if ((!mp_data->empty()) && (m_input().rows()>0))
    {

    if (state==UNKNOWN)
      { 
      state = (m_input().rows()==1) ? CREATETUPLE : CREATESET;
      m_output().adjust( (state==CREATETUPLE)?16:1 );
      }

    if (state==CREATETUPLE)
      {
      if (dim >= m_output().rows())
        {
        m_output().adjust( m_output().rows()+16 );
        }
      m_output()[dim] = m_input()[0];
      ++dim;
      }
    else // nun state==CREATESET
      {
      mp_prot->inc_level();
      for (int i=0;i<m_input().rows();++i)
        {
	  // Anm: eigentlich nach jedem ++prot: output.adjust(1) =>
	  // nicht noetig wegen DAL_MODE_SHARED!
        m_output()[0]=m_input()[i];
        m_time()=1.0*i/(m_input().rows()-1.0);
        ++(*mp_prot);
        }
      mp_prot->dec_level();
      }
    }

  if (state==CREATETUPLE)
    {
    m_output().adjust(dim);
    ++(*mp_prot);
    }
  }


int main(int argc, char** argv)
  {
  InitialLog Entry("dal.id","dal.log");
  INFO("executing stc");

  table_type data(DATA),prot(MODEL);

//  evaluation_list_type iterlist;
//  iterlist.push_back(evaluation_type(DATA,0,0,&data));
//  iterlist.push_back(evaluation_type(MODEL,0,0,&prot));

//  evaluate_cmdline(argc,argv,iterlist);
  TableFieldConversion stc(&data,&prot); 
  stc();
//  call_algorithm(iterlist,stc);
  }
