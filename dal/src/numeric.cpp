
/** \file detstat.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org> */

#ifndef NUMERIC_SOURCE
#define NUMERIC_SOURCE

#include "algorithm.hpp"
#include "logtrace.h"

using namespace Data_Access_Library;
namespace Data_Analysis_Library {

/** \class DetermineRange
    \brief Determine range of variable values.

    This multi-step algorithm determines the (pointwise) \c minimum
    and \c maximum of the \c value variables during the step()
    invocations. All attribute vectors \c value must have the same
    size (or dimension zero indicating an empty vector, which will be
    ignored). */

/** If \c maximum is a NULL pointer, then \c minimum and \c maximum
    will be concatenated in done(). For instance, if \c value is a
    real number, \c minimum contains the pair of minimum and maximum
    value at the end. */

void DetermineRange::init()
{
    LOG_METHOD("DAA","DetermineRange","init","");

    dim = -1; // value dimension unknown

    // load_default statt adjust(0)! Letzteres Kommando waere eine
    // Wertzuweisung, die dazu fuehrt, dass mp_minimum auf jeden Fall
    // ausgegeben wird.
    mp_minimum->load_default(); 
    if (mp_maximum!=NULL) mp_maximum->load_default();
    m_helpmax.adjust(0);
}

/** Consider current \c value for calculation of \c minimum and \c
    maximum. */

Algorithm::term_type
DetermineRange::step()
{
    LOG_METHOD("DAA","DetermineRange","step","");

    int rows = mp_value->val().rows();

    if ((dim==-1) && (rows>0)) 
    {
	dim=rows;
	mp_minimum->var().adjust(dim); matrix_set_scalar(mp_minimum->var(),1E30);
	if (mp_maximum!=NULL) mp_maximum->var().adjust(dim);
	m_helpmax.adjust(dim); matrix_set_scalar(m_helpmax,-1E30);
    }

    if (rows>0)
    {
	if (rows!=dim) ERROR("DetermineRange requires vectors of same size");
	for (int i=0;i<rows;++i)
	{
	    mp_minimum->var()[i] = min(mp_minimum->val()[i],mp_value->val()[i]);
	    m_helpmax[i] = max(m_helpmax[i],mp_value->val()[i]);
	}
    }

    return UNKNOWN;
}

void DetermineRange::done()
{
    LOG_METHOD("DAA","DetermineRange","done","");

    if (dim>0)
    {
	if (mp_maximum==NULL)
	    vector_concat(mp_minimum->var(),m_helpmax);
	else
	    matrix_copy(mp_maximum->var(),m_helpmax);
    }
}

/////////////////////////////////////////////////////////////////////

void
WeightedMean::init()
{ 
    LOG_METHOD("DAA","WeightedMean","init","");

    mp_weightsum->var() = 0;
    matrix_set_scalar(mp_datasum->var(),0);
}

Algorithm::term_type
WeightedMean::step() 
{
    LOG_METHOD("DAA","WeightedMean","step","");

    if (mp_value->val().rows()>0)
    {
	if ((*mp_datasum)().rows()==0)
	{
	    mp_datasum->var().adjust((*mp_value)().rows()); 
	    matrix_set_scalar(mp_datasum->var(),0);
	}
	
	mp_weightsum->var() += mp_weight->val();
	matrix_inc_scaled(mp_datasum->var(),mp_weight->val(),mp_value->val());
    }

    return UNKNOWN;
}

void
WeightedMean::done() 
{
    LOG_METHOD("DAA","WeightedMean","done","");

    mp_mean->var().alloc( (*mp_datasum)().rows() );
    if (mp_change!=NULL)
    {
	// Unterschied ermitteln
	matrix_dec_scaled(mp_mean->var(),1.0/mp_weightsum->val(),mp_datasum->val());
	mp_change->var() += matrix_square_norm(mp_mean->val());
	TRACE_VAR(mp_change->val());
    }

    // Neuen Mittelwert setzen
    matrix_set_scaled(mp_mean->var(),1.0/mp_weightsum->val(),mp_datasum->val());
    TRACE_VAR(mp_mean->val());
}

/////////////////////////////////////////////////////////////////////

/** \class DetermineStatistics
    \brief Determine mean and variance of multivariate values.

    This algorithm determines the \c mean and the
    (pointwise) \c variance of the \c value fields during the step()
    invocations. All attribute vectors \c value must have the same
    size (or dimension zero indicating an empty vector, which will be
    ignored.) */

void 
WeightedVariance::init()
{
    LOG_METHOD("DAA","WeightedVariance","init","");

    WeightedMean::init();
    matrix_set_scalar(mp_datasqsum->var(),0);
}

/** Consider current \c value for calculation of \c mean and \c
    variance. */

Algorithm::term_type
WeightedVariance::step()
{
    LOG_METHOD("DAA","WeightedVariance","step",mp_value->val());

    WeightedMean::step();

    if (mp_value->val().rows()>0)
    {
	if (mp_datasqsum->val().rows()==0)
	{
	    mp_datasqsum->var().adjust(mp_value->val().rows());
	    matrix_set_scalar(mp_datasqsum->var(),0);
	}
	
	for (int i=0;i<mp_datasqsum->val().rows();++i)
	{
	    real_type::value_type v = mp_value->val()[i];
	    mp_datasqsum->var()[i] += v*v;
	}
    }    

    return UNKNOWN;
}

/** Determine \c mean and \c variance (final averaging at end of data
    set). Return value is NULL. */

void WeightedVariance::done()
{
    LOG_METHOD("DAA","WeightedVariance","done","");

    WeightedMean::done();
    mp_variance->var().alloc( (*mp_datasqsum)().rows() );
    matrix_set_scalar(mp_variance->var(),0);

    real_type::value_type n = mp_weightsum->val();
    if (n>0)
    {
	for (int i=0;i<mp_variance->val().rows();++i)
	    mp_variance->var()[i] = 
		sqrt(1.0/(n-1) * 
		     (mp_datasqsum->val()[i] - SQR(mp_datasum->val()[i])/n));
    }
}

////////////////////////////////////////////////////////////////////

void
ConfusionMatrix::init()
{
    LOG_METHOD("DAA","ConfusionMatrix","init","");

    mp_confmat->var().adjust(0,0);
    mp_xlabels->var().adjust(0);
    mp_ylabels->var().adjust(0);
}

Algorithm::term_type
ConfusionMatrix::step()
{
    LOG_METHOD("DAA","ConfusionMatrix","step","");

    int maxx = mp_xlabels->val().rows();
    int maxy = mp_ylabels->val().rows();

cout << mp_xattr->val() << " -- " << mp_yattr->val() << endl;

    if ((!mp_xattr->is_default()) && (!mp_yattr->is_default()))
    {
	// fuer den Fall, dass die Indices der Labels lueckenhaft
	// sind, machen wir uns die Muehe einer eigenen Ordnung
	int x = 0;
	while ( (x<maxx) && (mp_xlabels->val()[x] != mp_xattr->val()) ) ++x;
	if (x==maxx) 
	{
	    mp_xlabels->var().adjust(maxx+1); 
	    mp_xlabels->var()[maxx] = mp_xattr->val();
	    mp_confmat->var().adjust(maxx+1,maxy);
	    for (int i=0;i<maxy;++i) mp_confmat->var()(maxx,i)=0;
	    ++maxx;
	}

	int y = 0;
	while ( (y<maxy) && (mp_ylabels->val()[y] != mp_yattr->val()) ) ++y;
	if (y==maxy) 
	{
	    mp_ylabels->var().adjust(maxy+1); 
	    mp_ylabels->var()[maxy] = mp_yattr->val();
	    mp_confmat->var().adjust(maxx,maxy+1);
	    for (int i=0;i<maxx;++i) mp_confmat->var()(i,maxy)=0;
	    ++maxy;
	}

	// Die Groesse der Confusion Matrix muss staendig ermittelt
	// und angepasst werden, da wir erst beim Durchlaufen durch
	// die Datei sehen, wieviele Cluster/Classes es gibt.
	++mp_confmat->var()(x,y);
	TRACE_VAR(mp_confmat->val());
    }

    return UNKNOWN;
}

void
ConfusionMatrix::done()
{
    LOG_METHOD("DAA","ConfusionMatrix","done","");

    // Zur Bestimmung der Trefferrate schlagen wir jedes cluster
    // derjenigen Klasse zu, bei der wir am meisten Treffer hatten.
    int samples = 0;
    mp_rate->var() = 0;
    for (int i=0;i<mp_ylabels->val().rows();++i)
    {
	int maxfit(0);
	for (int j=0;j<mp_xlabels->val().rows();++j) 
	{ 
	    maxfit = max(maxfit,mp_confmat->val()(j,i)); 
	    samples += mp_confmat->val()(j,i);
	}
	mp_rate->var() += maxfit;
	
    }
    if (samples>0) mp_rate->var() /= samples; 
    TRACE_VAR(mp_rate->val());
}
    
} // namespace

#endif // NUMERIC_SOURCE

