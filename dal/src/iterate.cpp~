
/** \file iterate.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Iteration algorithms. */

#ifndef ITERATE_SOURCE
#define ITERATE_SOURCE

#include "algorithm.hpp"
#include "logtrace.h"

using namespace Data_Access_Library;
namespace Data_Analysis_Library {

////////////////////////////////////////////////////////////////////

Algorithm::term_type 
WriteRecord::step()
{
    LOG_METHOD("DAA","WriteRecord","step",mp_table->context()->name());

    mp_table->push();
    return UNKNOWN;
}

//////////////////////////////////////////////////////////////////////

/** \class TableIndent
    \brief Increase hierarchy level in output table.
 */

void 
TableIndent::init()  
{ 
    LOG_METHOD("DAA","TableIndent","init",mp_table->context()->name());

    if (!mp_table->context()->is_flat()) {
	mp_table->free(); 
	mp_table->push(); 
	mp_table->inc_level();
//	((BHMasterTable*)mp_table)->submit();
    } 
}

Algorithm::term_type 
TableIndent::step()
{ 
    LOG_METHOD("DAA","TableIndent","step",mp_table->context()->name());

    return UNKNOWN;
}

void 
TableIndent::done() 
{
    LOG_METHOD("DAA","TableIndent","done",mp_table->context()->name());

    if (!mp_table->context()->is_flat()) {
        mp_table->dec_level(); 
//	((BHMasterTable*)mp_table)->submit();
    }
}

//////////////////////////////////////////////////////////////////////

/** \class TableIteration 

    The successor algorithm is embedded into the done() method. 

    The \c count field contains an index of the current record in the
    table (starting always with 0). In the done() method, count is set
    to the total number of records. */


/* Constructor: Pop a record from a table \c t whenever step() is
   invoked. The optional \t c is zero at the beginning and is in each
   step() call. */

void ReadRecord::init()
{
    LOG_METHOD("DAA","ReadRecord","init",mp_table->context()->name());
    
    mp_table->rewind();
    m_count=0;
}

Algorithm::term_type 
ReadRecord::step()
{
    LOG_METHOD("DAA","ReadRecord","step",mp_table->context()->name()<<':'<<m_count);
    
    term_type t = (mp_table->done()) ? TERMINATE : CONTINUE;
    if (t == CONTINUE) 
    { 
	mp_table->pop(); 

	if (mp_count!=NULL) mp_count->var() = m_count;
	++m_count; 

	t = eval_stop(t,mp_table->done());
    } 

    return t;
}

void 
ReadRecord::done()
{
    LOG_METHOD("DAA","ReadRecord","done",mp_table->context()->name());
    
    if (mp_count!=NULL) mp_count->var() = m_count;
}

////////////////////////////////////////////////////////////////////

/* The successor algorithm is embedded into the done() method.  Acts
as a filter, iterator algorithm is called only for elements in the
resp. subset. */


void ReadRecordSubSet::init()
{
    ReadRecord::init();
    if (mp_iter!=NULL) mp_iter->init();
}

Algorithm::term_type 
ReadRecordSubSet::step()
{
    LOG_METHOD("DAA","ReadRecordSubSet","step",mp_table->context()->name()<<m_count);
    
    term_type t = (mp_table->done()) ? TERMINATE : CONTINUE;

    if (t == CONTINUE) 
    { 
	mp_table->pop(); // get next record
	dynamic_cast<SubSetTable*>(mp_table)->mark();
	if ((mp_iter!=NULL)
	    &&(dynamic_cast<SubSetTable*>(mp_table)->match(m_hmask))) 
        { 
	    if (mp_count!=NULL) mp_count->var() = m_count;
	    ++m_count;

	    t = call_step(t,mp_iter); 
	}

	// reset subset (this is to avoid a stop=true due to end of
	// subset rather than end of database)
	dynamic_cast<SubSetTable*>(mp_table)->next_subset(); 
	t = eval_stop(t, mp_table->done() ); // stop due to end of table?
    }

    return t;
}

void 
ReadRecordSubSet::done()
{
    ReadRecord::done();
    if (mp_iter!=NULL) mp_iter->done();
}

////////////////////////////////////////////////////////////////////

template <class T>
Algorithm::term_type
SwapInputOutput<T>::step()
{
    LOG_METHOD("DAA","SwapInputOutput","step",mp_table->context()->name());

    mp_table->swap(); 
    return UNKNOWN;
}

template SwapInputOutput< BMasterTable >;
template SwapInputOutput< BHMasterTable >;

////////////////////////////////////////////////////////////////////

template <class T>
Algorithm::term_type
SubmitBuffered<T>::step()
{
    LOG_METHOD("DAA","SubmitBuffered","step",mp_table->context()->name());

    mp_table->submit(); 
    return UNKNOWN;
}

template SubmitBuffered< BMasterTable >;
template SubmitBuffered< BHMasterTable >;

////////////////////////////////////////////////////////////////////

/** \class DomainIterate

    The successor algorithm is embedded in done(). */

/** Iteration uber Label in ihrer alphabetischen Reihenfolge. */

void
DomainIterate::init()
{
    LOG_METHOD("DAA","DomainIterate","init","");

    m_count = 0;
    m_labeliter = mp_domain->begin();
}

Algorithm::term_type
DomainIterate::step()
{
    LOG_METHOD("DAA","DomainIterate","step","");

    term_type t = (m_labeliter==mp_domain->end()) ? TERMINATE : CONTINUE;

    if (t == CONTINUE) 
    {
	mp_label->var() = (*m_labeliter).second; 
	if (mp_count!=NULL) mp_count->var() = m_count;
	++m_count;

        ++m_labeliter; 
	t = eval_stop(t,m_labeliter==mp_domain->end());
    } 

    return t;
}

void DomainIterate::done()
{
    LOG_METHOD("DAA","DomainIterate","done","");
    if (mp_count!=NULL) mp_count->var() = mp_domain->size();
}

////////////////////////////////////////////////////////////////////

/** Sortieren einer Tabelle. Die Tabelle \c data wird einmal komplette
    durchlaufen und key/key2 wird gelesen; aus beiden keys wird ein
    Schlussel aufgebaut, der als Gesamtschluessel in einer map
    dient. Dann wird die Tabelle in der Reihenfolge der Map-Eintraege
    nochmals ausgelesen und dieses Mal auch geschrieben. */

/** Create sorted index of table. For every record the sorting key is
    constructed and stored together with the table file position in \c
    m_index. */

void
SortedTableIteration::init()
{
    LOG_METHOD("DAA","SortedTableIteration","init","");

    tuple_type::value_type value;
    marker_type m;
    m_index.clear();    

    // In sortierten Index einlesen
    mp_data->rewind();
    while (!mp_data->done())
    {
	mp_data->pop(); mp_data->getrpos(m);
	matrix_copy(value,mp_key->val());
	vector_concat(value,mp_key2->val());
	m_index.insert( index_type::value_type(value,m) );
    }
    m_position = m_index.begin();
}

/** Iterate over sorted table. The records are read a second time from
    the table in the order given by \c m_index. */

Algorithm::term_type
SortedTableIteration::step()
{
    LOG_METHOD("DAA","SortedTableIteration","step","");

    term_type t = (m_position==m_index.end()) ? TERMINATE : CONTINUE;

    if (t == CONTINUE) 
    { 
	mp_data->setrpos((*m_position).second); 
	mp_data->pop(); 

        ++m_position; 
	t = eval_stop(t,m_position==m_index.end());
    } 

    return t;
}

void SortedTableIteration::done()
{
}

//////////////////////////////////////////////////////////////////////

/** value is set to zero before step() is called. */

template <class T>
Algorithm::term_type
WhileGreater<T>::step()
{
    LOG_METHOD("DAA","WhileGreater","step",mp_value->val()<<'>'<<mp_upper->val());

    term_type t = (mp_value->val() > mp_upper->val()) ? CONTINUE : TERMINATE;
    mp_value->var()=0;
    return t;
}

template WhileGreater<index_type>;
template WhileGreater<int_type>;
template WhileGreater<real_type>;

}; // namespace

#endif // ITERATE_SOURCE
