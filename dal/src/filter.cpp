
/** \file filter.cpp
    \author Frank Hoeppner <frank.hoeppner@ieee.org> */

#ifndef FILTER_SOURCE
#define FILTER_SOURCE

#include "algorithm.hpp"
#include "logtrace.h"

using namespace Data_Access_Library;
namespace Data_Analysis_Library {

//////////////////////////////////////////////////////////////////////

/** \class IfDefined
    \brief Invoke successor only if variable has default value. 

    The algorithm \c alg is embedded into the step() call if \c value
    is defined, that is, it does not have the default value. (If
    negate is true the logic is inverted) */

template <class T>
Algorithm::term_type 
IfDefined<T>::step()
{ 

    /*   default  negate   call?
 	    F       F        T
	    T       F        F
	    F       T        F
	    T       T        T    */
       
    if (mp_alg!=NULL)
	if (!(m_negate ^ (mp_value->is_default())))
	    mp_alg->loop();

    return UNKNOWN;
}

template IfDefined<int_type>;
template IfDefined<real_type>;
template IfDefined<tuple_type>;

//////////////////////////////////////////////////////////////////////

/** \class FilterKOutOfN
    \brief Selecting k out of n step() calls.

    This multi-step algorithm selects k step()-calls out of a total
    number of n step() calls (this happens in init()). During the
    execution of this algorithm only in the selected cases the step()
    method of the successor will be invoked. */

/** Constructor. The \c incl denotes a variable which is 1 if the
    step() has been selected and otherwise zero. Apparently this makes
    only sense in combination with another parameter \c subset
    (because usually we see only records with incl=1 in the
    output). If \c subset it is set to false, the step() method of the
    successor is invoked every time (and not only in k out of n
    cases).  */

/** Select k out of n. The selection of k out of n values is done
    here. When init() is called, the values of arguments k and n must
    be loaded.  */

void
FilterKOutOfN::init()
{
    LOG_METHOD("DAA","FilterKOutOfN","init","");
    
    // wenn k > n/2 dann aussuchen, was NICHT gewaehlt wurde
    int k=mp_k->val(),n=mp_n->val();
    if (k==0) WARNING("do you really need an empty selection?");
    if (n<k) ERROR("N="<<n<<" must be larger than k="<<k);
    if (k > n/2) { k = n-k; m_inverted=true; } else m_inverted=false;
    
    // k Elemente zufaellig auswaehlen
    m_selection.clear();
    for (int i=0;i<k;++i)
	m_selection.insert(static_cast<int>(1.0*rand()/RAND_MAX*n));
    
    // falls Zahlen doppelt, kann |selection| zu klein sein
    while (m_selection.size()!=static_cast<word>(k))
	m_selection.insert(static_cast<int>(1.0*rand()/RAND_MAX*n));
    //cout << mp_n->val() << m_selection << endl;
    
    // Zaehlerei initialisieren
    m_pos = m_selection.begin();
    m_ctr=0;
    
    if (mp_filtered!=NULL) mp_filtered->init();
}

Algorithm::term_type FilterKOutOfN::step()
{
    LOG_METHOD("DAA","FilterKOutOfN","step","");

    bool push = false;
    
    if (m_pos!=m_selection.end())
    {
	if (m_inverted) {
	    if (m_ctr != (*m_pos)) push=true; else ++m_pos;
	} else {
	    if (m_ctr == (*m_pos)) { push=true; ++m_pos; }
	}
	++m_ctr;
    }
    mp_included->var() = push;

    // Grunsaetzlich waere es hier moeglich, nach Erreichen des
    // letzten Elements im subset zu terminieren. Wird sample
    // allerdings iteriert (sample data(*,*)<...), dann wird der
    // verbleibende Rest faelschlich als neuer SubSet interpretiert!!
    term_type t = UNKNOWN;
    if (push||(!mp_subset->val())) t = call_step(t,mp_filtered);
    return t;
}

/** Return value is always NULL. */

void FilterKOutOfN::done()
{
    LOG_METHOD("DAA","FilterKOutOfN","done","");
    
    if (mp_filtered!=NULL) mp_filtered->done();
}

////////////////////////////////////////////////////////////////////

/** The successor algorithm is called only if the variable \c
    ap_attribute is equal to \c ap_value (given that \c mp_equal is
    true). If \c mp_equal is false, the successor is called if the
    variable is not equal \c mp_value. */

Algorithm::term_type 
FilterValue::step()
{
    bool call = false;
    if (m_valid)
      {
      call = (mp_attribute->val() == mp_value->val());
      if (!mp_equal->val()) call = !call;
      }

    term_type t = UNKNOWN;
    if (call) call_step(t,mp_succ);
    return t;
}

////////////////////////////////////////////////////////////////////

/** \class FilterExtremum
    \brief Select a value that is in some sense extremal.

    The step() method of the successor is called only once. The
    content of the \c value field at this step() call is the content
    of the field when \c sel_var became maximal.

\code
      CALLER      FILTER
      init() ---> filtered.init() -->
                  succ.init() -->
      step() ---> succ.step()
      done() ---> filtered.step() -->
                  filtered.done() -->
                  succ.done() -->
\endcode

*/

template <class T> 
void
FilterExtremum<T>::init()
{
    LOG_METHOD("DAA","FilterExtremum","init","");

    m_extremum = POS_IMPOSSIBLE_RANGE; 
    if (mp_filtered!=NULL) mp_filtered->init();
}

template <class T> 
Algorithm::term_type
FilterExtremum<T>::step()
{
    LOG_METHOD("DAA","FilterExtremum","step","");

    if (mp_inspect->val() < m_extremum)
    {
	m_extremum = mp_inspect->val();
	mp_return_value->var() = mp_value->val();
    }

    return UNKNOWN; // kein step()-Aufruf !!!
}

/** Return value is the selected value. When done() is called, the
    extremum is determined and we call the step() method of the
    successor (and the done() method). */

template <class T>
void FilterExtremum<T>::done()
{
    LOG_METHOD("DAA","FilterExtremum","done","");

    if (mp_filtered!=NULL)
    {
	mp_filtered->step();
	mp_filtered->done();
    }
}

template FilterExtremum<index_type>;
template FilterExtremum<int_type>;
template FilterExtremum<real_type>;
template FilterExtremum<tuple_type>;

} // namespace

#endif // FILTER_SOURCE

