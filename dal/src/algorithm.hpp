
/** \file algorithm.hpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Header file for various algorithms. */

#ifndef ALGORITHM_HEADER
#define ALGORITHM_HEADER

#include <iostream>
#include <stdlib.h>
#include "dal.hpp"
#include "matvecop.hpp"
#include "ttime.hpp"

using namespace Data_Access_Library;
namespace Data_Analysis_Library {

///////////////////////////////////////////////////////////////////
///////////////// BASIC ALGORITHMS (algorithm.cpp) ////////////////
///////////////////////////////////////////////////////////////////

class Algorithm
{
public:
    enum term_type { TERMINATE=3, CONTINUE=2, UNKNOWN=1 };

    inline Algorithm() {}
    virtual ~Algorithm();

    virtual void init();
    virtual term_type step();
    virtual void done();
    virtual void loop();
protected:
    typedef list<Algorithm*> alg_list;
};

///////////////////////////////////////////////////////////////////

void iterate_algorithm(Algorithm**);

////////////////////////////////////////////////////////////////////

inline Algorithm::term_type 
call_step(Algorithm::term_type t,Algorithm* p)
{
    return (p==NULL) ? t
	: static_cast<Algorithm::term_type>(
	    max(static_cast<int>(t),static_cast<int>(p->step()))); 
}

inline Algorithm::term_type 
eval_stop(Algorithm::term_type t,bool b)
{
    return static_cast<Algorithm::term_type>(
	max(static_cast<int>(t),
	    static_cast<int>(b ? Algorithm::TERMINATE : Algorithm::CONTINUE))); 
}

///////////////////////////////////////////////////////////////////

class ParallelBind
    : public Algorithm
{
public:
    inline ParallelBind() {}
    virtual ~ParallelBind();

    virtual void init();
    virtual term_type step();
    virtual void done();

    inline void push_back(Algorithm *p) 
	{ VALID_POINTER(p); m_succ.push_back(p); }
    inline void push_front(Algorithm *p) 
	{ VALID_POINTER(p); m_succ.push_front(p); }

protected:
    list<Algorithm*> m_succ;
};

inline ParallelBind* seq(Algorithm *a)
{ ParallelBind *p=new ParallelBind(); 
 p->push_back(a); return p; }

inline ParallelBind* seq(Algorithm *a,Algorithm *b)
{ ParallelBind *p=new ParallelBind(); 
 p->push_back(a); p->push_back(b); return p; }

inline ParallelBind* seq(Algorithm *a,Algorithm *b,Algorithm *c)
{ ParallelBind *p=new ParallelBind(); 
 p->push_back(a); p->push_back(b); p->push_back(c); return p; }

inline ParallelBind* seq(Algorithm *a,Algorithm *b,Algorithm *c,Algorithm *d)
{ ParallelBind *p=new ParallelBind(); p->push_back(a); 
 p->push_back(b); p->push_back(c); p->push_back(d); return p; }

inline ParallelBind* seq(Algorithm *a,Algorithm *b,Algorithm *c,
			 Algorithm *d,Algorithm *e)
{ ParallelBind *p=new ParallelBind(); p->push_back(a); p->push_back(b); 
 p->push_back(c); p->push_back(d); p->push_back(e); return p; }

inline ParallelBind* seq(Algorithm *a,Algorithm *b,Algorithm *c,
			 Algorithm *d,Algorithm *e,Algorithm *f)
{ ParallelBind *p=new ParallelBind(); p->push_back(a); p->push_back(b); 
 p->push_back(c); p->push_back(d); p->push_back(e); p->push_back(f); 
 return p; }

inline ParallelBind* seq(Algorithm *a,Algorithm *b,Algorithm *c,
			 Algorithm *d,Algorithm *e,Algorithm *f,Algorithm *g)
{ ParallelBind *p=new ParallelBind(); p->push_back(a); p->push_back(b); 
 p->push_back(c); p->push_back(d); p->push_back(e); p->push_back(f); 
 p->push_back(g); return p; }

inline ParallelBind* seq(Algorithm *a,Algorithm *b,Algorithm *c,
			 Algorithm *d,Algorithm *e,Algorithm *f,
			 Algorithm *g,Algorithm *h)
{ ParallelBind *p=new ParallelBind(); p->push_back(a); p->push_back(b); 
 p->push_back(c); p->push_back(d); p->push_back(e); p->push_back(f); 
 p->push_back(g); p->push_back(h); return p; }

///////////////////////////////////////////////////////////////////

class Embedding
    : public ParallelBind
{
public:
    enum { INIT=1,STEP=2,DONE=4,ALL=7 };

    inline Embedding(int w=STEP) : m_where(w & ALL) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

protected:
    int m_where;
};

inline Embedding* embed_loop(Algorithm *a)
{ Embedding *p = new Embedding();
 p->push_back(a); return p; }

inline Embedding* embed_loop(int w,Algorithm *a)
{ Embedding *p = new Embedding(w);
 p->push_back(a); return p; }

inline Embedding* embed_loop(Algorithm *a,Algorithm *b)
{ Embedding *p=new Embedding(); 
 p->push_back(a); p->push_back(b); return p; }

inline Embedding* embed_loop(int w,Algorithm *a,Algorithm *b)
{ Embedding *p=new Embedding(w); 
 p->push_back(a); p->push_back(b); return p; }

inline Embedding* embed_loop(Algorithm *a,Algorithm *b,Algorithm *c)
{ Embedding *p=new Embedding(); 
 p->push_back(a); p->push_back(b); p->push_back(c); return p; }

inline Embedding* embed_loop(int w,Algorithm *a,Algorithm *b,Algorithm *c)
{ Embedding *p=new Embedding(w); 
 p->push_back(a); p->push_back(b); p->push_back(c); return p; }

inline Embedding* embed_loop(Algorithm *a,Algorithm *b,Algorithm *c,
			     Algorithm *d)
{ Embedding *p=new Embedding(); p->push_back(a); 
 p->push_back(b); p->push_back(c); p->push_back(d); return p; }

inline Embedding* embed_loop(Algorithm *a,Algorithm *b,Algorithm *c,
			     Algorithm *d,Algorithm *e)
{ Embedding *p=new Embedding(); p->push_back(a); p->push_back(b); 
 p->push_back(c); p->push_back(d); p->push_back(e); return p; }

///////////////////////////////////////////////////////////////////

class ExchangedEmbedding
    : public Algorithm
{
public:
    inline ExchangedEmbedding(Algorithm *iter,Algorithm *alg) 
	: mp_iter(iter), mp_alg(alg) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

protected:
    Algorithm *mp_iter,*mp_alg;
};

inline ExchangedEmbedding* xembed_loop(Algorithm *i,Algorithm *a)
{ return new ExchangedEmbedding(i,a); }

///////////////////////////////////////////////////////////////////

template <class T>
class WhileGreater
    : public Algorithm
{
public:
    inline WhileGreater(T *value,T *upper)
	: mp_value(value), mp_upper(upper) {}
    virtual term_type step();

protected:
    Algorithm *mp_iter;
    T *mp_value,*mp_upper;
};

template <class T>
inline WhileGreater<T>*
while_ge(T *value,T *upperbound)
{ return new WhileGreater<T>(value,upperbound); }

///////////////////////////////////////////////////////////////////

template <class T>
class Tracer
    : public Algorithm
{
public:
    inline Tracer(T *var) : mp_var(var) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

private:
    T *mp_var;
};

template <class T>
inline Algorithm* inspect(T *var)
{ return new Tracer<T>(var); }

///////////////////////////////////////////////////////////////////

template <class T>
class IfDefined
    : public Algorithm
{
public:
    inline IfDefined(T *v,Algorithm *a,bool n) 
	: mp_value(v), mp_alg(a), m_negate(n) {}
    virtual term_type step();
private:
    T *mp_value;
    Algorithm *mp_alg;
    bool m_negate;
};

template <class T> inline IfDefined<T>* 
embed_loop_ifdef(T *value,Algorithm *alg) 
{ return new IfDefined<T>(value,alg,false); }

template <class T> inline IfDefined<T>* 
embed_loop_ifndef(T *value,Algorithm *alg) 
{ return new IfDefined<T>(value,alg,true); }

///////////////////////////////////////////////////////////////////

class ElapsedTime
    : public Algorithm
{
public:
    inline ElapsedTime(real_type *t,Algorithm *a) 
	: mp_alg(a), mp_time(t) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

protected:
    Algorithm *mp_alg;
    real_type *mp_time;
};

inline ElapsedTime* elapsed(real_type* t,Algorithm* q)
{ return new ElapsedTime(t,q); }

inline ElapsedTime* elapsed(real_type* t,Algorithm* a,Algorithm *b)
{ return new ElapsedTime(t,seq(a,b)); }

inline ElapsedTime* elapsed(real_type* t,Algorithm* a,Algorithm *b,Algorithm *c)
{ return new ElapsedTime(t,seq(a,b,c)); }

////////////////////////////////////////////////////////////////////

class ComposeTuple
    : public Algorithm
{
public:
    inline ComposeTuple(tuple_type *t,int_type *i,real_type *v,
			bool dyadic=false)
	: mp_value(v), mp_tuple(t), mp_index(i), m_dyadic(dyadic) {}

    virtual term_type step();
    virtual void done();
private:
    real_type *mp_value;
    tuple_type *mp_tuple;
    int_type *mp_index;
    bool m_dyadic;
  };

inline ComposeTuple*
compose(tuple_type *t,int_type *i,real_type *r,bool b) 
{ return new ComposeTuple(t,i,r,b); }

////////////////////////////////////////////////////////////////////

class DecomposeTuple
    : public Algorithm
{
public:
    inline DecomposeTuple(real_type *v,tuple_type *t,int_type *i)
	: mp_value(v), mp_tuple(t), mp_index(i) {}
    virtual term_type step();

private:
    real_type *mp_value;
    tuple_type *mp_tuple;
    int_type *mp_index;
};

inline DecomposeTuple*
decompose(real_type *r,tuple_type *t,int_type *i) 
{ return new DecomposeTuple(r,t,i); }


///////////////////////////////////////////////////////////////////
/////////// ITERATION AND TABLE ALGORITHMS (iterate.cpp) //////////
///////////////////////////////////////////////////////////////////

class WriteRecord
    : public Algorithm
{
public:
    inline WriteRecord(BaseTable *t) : mp_table(t) {}
    virtual term_type step();
private:
    BaseTable *mp_table;
};

inline WriteRecord* 
writeback(BaseTable *t) 
{ return new WriteRecord(t); }

///////////////////////////////////////////////////////////////////

class TableIndent
    : public Algorithm
{
public:
    inline TableIndent(HMasterTable *t) : mp_table(t) {}
    virtual void init();
    virtual term_type step();
    virtual void done();
protected:
    HMasterTable *mp_table;
};

inline TableIndent*
indent(HMasterTable *t)
{ return new TableIndent(t); }

///////////////////////////////////////////////////////////////////

class ReadRecord
  : public Algorithm
{
public:
    inline ReadRecord(BaseTable *t,int_type *c) : mp_table(t), mp_count(c) {}

    virtual void init();
    virtual term_type step();
    virtual void done();
protected:
    BaseTable *mp_table;
    int_type *mp_count;
    word m_count;
};

inline ReadRecord* 
fetch(BaseTable *t) 
{ return new ReadRecord(t,NULL); }

inline ReadRecord* 
fetch(BaseTable *t,int_type *c) 
{ return new ReadRecord(t,c); }

////////////////////////////////////////////////////////////////////

class ReadRecordSubSet
    : public ReadRecord
{
public:
    inline ReadRecordSubSet(const int_matrix_type::value_type& hmask,
			    SubSetTable *t,int_type *cnt,Algorithm *iter)
	: ReadRecord(t,cnt), m_hmask(hmask), mp_iter(iter) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

private:
    int_matrix_type::value_type m_hmask;
    Algorithm* mp_iter;
};

inline ReadRecordSubSet* 
fetch(const int_matrix_type::value_type& hmask,SubSetTable *t,
      Algorithm *iter,int_type *cnt=NULL) 
{ return new ReadRecordSubSet(hmask,t,cnt,iter); }

////////////////////////////////////////////////////////////////////

template <class T>
class SwapInputOutput
    : public Algorithm
{
public:
    inline SwapInputOutput(T *t) : mp_table(t) { VALID_POINTER(mp_table); }
    virtual term_type step();

protected:
    T *mp_table; ///< T must be a BufferedTable<> type
};

template <class T> inline SwapInputOutput<T>*
swap(T *t) { return new SwapInputOutput<T>(t); }
////////////////////////////////////////////////////////////////////

template <class T>
class SubmitBuffered
    : public Algorithm
{
public:
    inline SubmitBuffered(T *t) : mp_table(t) { VALID_POINTER(mp_table); }
    virtual term_type step();

protected:
    T *mp_table; ///< T must be a BufferedTable<> type
};

template <class T> inline SubmitBuffered<T>*
submit(T *t) { return new SubmitBuffered<T>(t); }

////////////////////////////////////////////////////////////////////

class DomainIterate
    : public Algorithm
{
public:
    inline DomainIterate(Domain *t,int_type *lbl,int_type *cnt)
	: mp_domain(t), mp_label(lbl), mp_count(cnt) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

private:
    Domain *mp_domain;
    int_type *mp_label,*mp_count;

    Domain::const_iterator m_labeliter;
    int m_count;
};

inline DomainIterate* 
fetch(Domain *t,int_type *lbl,int_type *cnt=NULL) 
  { return new DomainIterate(t,lbl,cnt); }

////////////////////////////////////////////////////////////////////

class SortedTableIteration
    : public Algorithm
{
public:
    inline SortedTableIteration(SimpleTable *t,tuple_type *k,tuple_type *k2)
	: mp_data(t), mp_key(k), mp_key2(k2) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

private:
    SimpleTable *mp_data;
    tuple_type *mp_key,*mp_key2;
    Algorithm *mp_iter;

    typedef multimap< tuple_type::value_type, marker_type > index_type;
    index_type m_index;
    index_type::const_iterator m_position;
};

inline SortedTableIteration*
sorted_fetch(SimpleTable *t,tuple_type *k,tuple_type *k2)
{ return new SortedTableIteration(t,k,k2); }

///////////////////////////////////////////////////////////////////
/////////////// NUMERIC ALGORITHMS (numeric.cpp) //////////////////
///////////////////////////////////////////////////////////////////

class DetermineRange
    : public Algorithm
{
public:
    inline DetermineRange(tuple_type *val,tuple_type *min,tuple_type *max)
	: mp_value(val), mp_minimum(min), mp_maximum(max) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

private:
    tuple_type *mp_value; ///< values under consideration
    tuple_type *mp_minimum;  ///< minimum of all values seen in step() calls
    tuple_type *mp_maximum; ///< maximum of all values seen in step() calls

    tuple_type::value_type m_helpmax;
    int dim;
};

inline DetermineRange*
detrange(tuple_type *value,tuple_type *min,tuple_type *max=NULL) 
{ return new DetermineRange(value,min,max); }

////////////////////////////////////////////////////////////////////

class WeightedMean
    : public Algorithm
{
public:
    inline WeightedMean(real_type *w,tuple_type *v,real_type *ws,
			tuple_type *vs,tuple_type *m,real_type *chg)
	: mp_weight(w), mp_value(v), mp_datasum(vs)
	, mp_mean(m), mp_weightsum(ws), mp_change(chg) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

protected:
    real_type *mp_weight;
    tuple_type *mp_value, *mp_datasum, *mp_mean;
    real_type *mp_weightsum, *mp_change;
};

inline WeightedMean*
weightedmean(real_type *w,tuple_type *v,real_type *ws,
	     tuple_type *vs,tuple_type *m,real_type *c=NULL) 
{ return new WeightedMean(w,v,ws,vs,m,c); }

////////////////////////////////////////////////////////////////////

class WeightedVariance
    : public WeightedMean
{
public:
    inline WeightedVariance(real_type *w,tuple_type *v,real_type *ws,
			    tuple_type *vs,tuple_type *vss,
			    tuple_type *m,tuple_type *var)
	: WeightedMean(w,v,ws,vs,m,NULL), mp_datasqsum(vss)
	, mp_variance(var) {}
    
    virtual void init();
    virtual term_type step();
    virtual void done();
private:
    tuple_type *mp_datasqsum, *mp_variance;
};

inline WeightedVariance*
weightedvar(real_type *w,tuple_type *v,real_type *ws,tuple_type *vs,
	    tuple_type *vss,tuple_type *m,tuple_type *var) 
{ return new WeightedVariance(w,v,ws,vs,vss,m,var); }

////////////////////////////////////////////////////////////////////

class ConfusionMatrix
    : public Algorithm
{
public:
    inline ConfusionMatrix(int_tuple_type *xl,int_tuple_type *yl,
			   int_matrix_type *cm,int_type *x,int_type *y,
			   real_type *r)
	: mp_xlabels(xl), mp_ylabels(yl), mp_confmat(cm), mp_xattr(x)
	, mp_yattr(y), mp_rate(r) {}
    
    virtual void init();
    virtual term_type step();
    virtual void done();
private:
    int_tuple_type *mp_xlabels,*mp_ylabels;
    int_matrix_type *mp_confmat;
    int_type *mp_xattr,*mp_yattr;
    real_type *mp_rate;
};

inline ConfusionMatrix*
confmat(int_tuple_type *xl,int_tuple_type *yl,int_matrix_type *cm,
	int_type *x,int_type *y,real_type *r)
{ return new ConfusionMatrix(xl,yl,cm,x,y,r); }

////////////////////////////////////////////////////////////////////

// TODO: WeightedVariance + WeightedCoVariance von WeightedMean
// ableiten (mean schon berechnet)
/*
class DetermineStatistics
    : public Algorithm
{
public:
    inline DetermineStatistics(tuple_type *val,tuple_type *mean,
			       tuple_type *var,int_type *cnt)
	: mp_value(val), mp_mean(mean), mp_variance(var), mp_count(cnt) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

private:
    tuple_type *mp_value;
    tuple_type *mp_mean; 
    tuple_type *mp_variance;
    int_type *mp_count;
    tuple_type::value_type m_sum,m_squaredsum;
    int dim;
};

inline DetermineStatistics*
detstat(tuple_type *value,tuple_type *mean,tuple_type *var,
	int_type *count) 
{ return new DetermineStatistics(value,mean,var,count); }
*/
///////////////////////////////////////////////////////////////////
////////////////// FILTER ALGORITHMS (filter.cpp) /////////////////
///////////////////////////////////////////////////////////////////

class FilterKOutOfN
    : public Algorithm
{
public:
    inline FilterKOutOfN(int_type *s,int_type *k,int_type *n,
			 int_type *incl, Algorithm *f)
	: mp_filtered(f), mp_subset(s), mp_k(k), mp_n(n), mp_included(incl) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

private:
    Algorithm *mp_filtered;
    int_type *mp_subset,*mp_k,*mp_n,*mp_included;

    set<int> m_selection;
    set<int>::const_iterator m_pos;
    int m_ctr;
    bool m_inverted;
};

inline FilterKOutOfN*
koutofn(int_type *subset,int_type *k,int_type *n,
	int_type *incl,Algorithm* f)
{ return new FilterKOutOfN(subset,k,n,incl,f); }

///////////////////////////////////////////////////////////////////

class FilterValue
    : public Algorithm
{
public:
    inline FilterValue(int_type *a,int_type *v,int_type *eq, Algorithm *s)
	: mp_attribute(a), mp_value(v), mp_equal(eq), mp_succ(s)
	, m_valid((a!=NULL)&&(v!=NULL)&&(eq!=NULL)) {}
    
    virtual term_type step();
protected:
    int_type *mp_attribute,*mp_value,*mp_equal;
    Algorithm* mp_succ;
    bool m_valid;
};

inline FilterValue*
select(int_type *att,int_type *val,int_type *eq,Algorithm *p = NULL) 
  { return new FilterValue(att,val,eq,p); }

///////////////////////////////////////////////////////////////////

template <class T>
class FilterExtremum
    : public Algorithm
{
public:
    inline FilterExtremum(real_type *inspect,const T *value,T *retval,
			  Algorithm *f)
	: mp_filtered(f), mp_inspect(inspect)
	, mp_value(value), mp_return_value(retval) {}

    virtual void init();
    virtual term_type step();
    virtual void done();

private:
    Algorithm *mp_filtered;
    real_type *mp_inspect;
    real_type::value_type m_extremum;
    const T *mp_value;
    T *mp_return_value;
};

template <class T>
inline FilterExtremum<T>*
selectmin(real_type *inspect,const T *value,T *retval,
	  Algorithm *f)
  { return new FilterExtremum<T>(inspect,value,retval,f); }

///////////////////////////////////////////////////////////////////
////////////// DISTANCE ALGORITHMS (distance.cpp) /////////////////
///////////////////////////////////////////////////////////////////

class EuclideanDistance
    : public Algorithm
{
public:
    inline EuclideanDistance(real_type* d,tuple_type* x,tuple_type* y)
	: mp_dist(d), mp_x(x), mp_y(y) {}
    virtual term_type step();

private:
    real_type *mp_dist;
    tuple_type *mp_x,*mp_y;
};

inline EuclideanDistance* 
euclidsqdist(real_type *d,tuple_type *x,tuple_type *y)
{ return new EuclideanDistance(d,x,y); }

//////////////////////////////////////////////////////////////////

class ScalarProdDistance
    : public Algorithm
{
public:
    ScalarProdDistance(real_type*,tuple_type*,tuple_type*,
		       real_type*,bool = false,Algorithm* = NULL);
    virtual term_type step();

private:
    real_type *mp_dist,*mp_output;
    tuple_type *mp_x,*mp_y;
    bool m_inc;
};

inline ScalarProdDistance* 
dotdist(real_type *d,tuple_type *x,tuple_type *y,real_type *o,
	bool inc=false,Algorithm *p=NULL)
{ return new ScalarProdDistance(d,x,y,o,inc,p); }

//////////////////////////////////////////////////////////////////

class ScaledEuclideanDistance
    : public Algorithm
{
public:
    ScaledEuclideanDistance(real_type*,tuple_type*,tuple_type*,
			    real_type*,Algorithm* = NULL);
    virtual term_type step();

private:
    real_type *mp_dist,*mp_yscale;
    tuple_type *mp_x,*mp_y;
};

inline ScaledEuclideanDistance* 
seuclidsqdist(real_type *d,tuple_type *x,tuple_type *y,
	      real_type *ys,Algorithm *p=NULL)
{ return new ScaledEuclideanDistance(d,x,y,ys,p); }

///////////////////////////////////////////////////////////////////

class MahalanobisDistance
    : public Algorithm
{
public:
    MahalanobisDistance(real_type*,tuple_type*,tuple_type*,
			matrix_type*,Algorithm* = NULL);
    virtual term_type step();

private:
    real_type *mp_dist;
    tuple_type *mp_x,*mp_y;
    matrix_type *mp_normmat;
};

inline MahalanobisDistance* 
matdist(real_type *d,tuple_type *x,tuple_type *y,
        matrix_type *nm,Algorithm *p=NULL)
{ return new MahalanobisDistance(d,x,y,nm,p); }

}; // namespace

#endif // ALGORITHM_HEADER
