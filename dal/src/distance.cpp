
/*  Copyright     : Copyright (C) 2000, 2001 Frank Hoeppner

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation; either version 2.1
    of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307, USA.  */

/** \file distance.cpp
    \author Frank Hoeppner <frank.hoeppner@ieee.org>
    \brief Implementation of various distance functions. */

#ifndef DISTANCE_SOURCE
#define DISTANCE_SOURCE

#include "algorithm.hpp"

namespace Data_Analysis_Library {

///////////////////////////////////////////////////////////////////

/** \class EuclideanDistance
    \brief Euclidean distance.

    This one-step algorithm returns the Euclidean distance between
    arguments \c x and \c y: \f$ \|x-y\|^2 \f$.  Distance calculation
    \f$ \|x-y\|^2 \f$. */

Algorithm::term_type
EuclideanDistance::step()
{
    LOG_METHOD("DAA","EuclideanDistance","step","");

    mp_dist->var() = matrix_square_distance((*mp_x)(),(*mp_y)());
    TRACE_ID("distance",(*mp_dist)()<<"= eucldist("<<
	     (*mp_x)()<<","<<(*mp_y)()<<")");

    return UNKNOWN;
}

////////////////////////////////////////////////////////////////
#ifdef NOTYET
/** \class ScalarProdDistance
    \brief Scalar product distance. 

    This one-step algorithm returns the squared scalar product between
    \c x and \c y: \f$(x^\top y)^2\f$. If \c inc is true, then the
    scalar product is subtracted from an \c output value: \f$(o-x^\top
    y)^2\f$ */

/** The squared distance between \c x and \c y is stored in \c
    p_dist. The boolean variable \c inc determines whether \c output
    is subtracted from the scalar product of \c x and \c y. */

ScalarProdDistance::ScalarProdDistance(
    real_type *p_dist,
    tuple_type *p_x,
    tuple_type *p_y,
    real_type *p_output,
    bool inc,
    Algorithm *p_succ
    )
    : Algorithm(p_succ)
    , mp_dist(p_dist)
    , mp_output(p_output)
    , mp_x(p_x)
    , mp_y(p_y)
    , m_inc(inc) 
{}

/** Distance calculation \f$(o-x^\top y)^2\f$. */

Algorithm::term_type
ScalarProdDistance::step()
{
    real_type::value_type d = mp_output->val();
    if (mp_x->val().rows()==mp_y->val().rows()) 
	d -= matrix_dot_product((*mp_x)(),(*mp_y)());
    if (m_inc) mp_dist->var() += d; else mp_dist->var() = d;
    d *= d;
    TRACE_ID("distance",d<<"= dotdist("<<(*mp_x)()<<","<<(*mp_y)()<<")");

    return call_step(UNKNOWN,mp_succ);
}

////////////////////////////////////////////////////////////////

/** \class ScaledEuclideanDistance
    \brief Scaled Euclidean distance.

    Similar to EuclideanDistance, but each axis is multiplied with a
    scale factor provided by \c yscale. */

ScaledEuclideanDistance::ScaledEuclideanDistance(
    real_type *p_dist,
    tuple_type *p_x,
    tuple_type *p_y,
    real_type *p_yscale,
    Algorithm *p_succ
    )
    : Algorithm(p_succ)
    , mp_dist(p_dist)
    , mp_yscale(p_yscale)
    , mp_x(p_x)
    , mp_y(p_y) 
{}

/** Calculate scaled Euclidean distance. A scaling factor \c
    *mp_yscale for \c *mp_y is determined such that the Euclidean
    distance between \c *mp_x and the scaled \c *mp_y is minimized. */

Algorithm::term_type
ScaledEuclideanDistance::step()
{
    mp_yscale->var() = matrix_dot_product(mp_x->val(),mp_y->val())
	/ matrix_square_norm(mp_y->val());
    static tuple_type::value_type tmp; tmp.alloc(mp_x->val().rows());
    matrix_set_combi(tmp,1.0,mp_x->val(),-mp_yscale->val(),mp_y->val());
    mp_dist->var() = matrix_square_norm(tmp);
    TRACE_ID("distance",(*mp_dist)()<<"= scaleddist("<<
	     (*mp_x)()<<","<<(*mp_y)()<<")");

    return call_step(UNKNOWN,mp_succ);
}

//////////////////////////////////////////////////////////////////

/** \class MahalanobisDistance
    \brief Mahalanobis distance.

    Distance induced by norm matrix \c A: \f$(x-y)^\top A (x-y)\f$. */
    
MahalanobisDistance::MahalanobisDistance(
    real_type *p_dist,
    tuple_type *p_x,
    tuple_type *p_y,
    matrix_type *p_normmat,
    Algorithm *p_succ
    )
    : Algorithm(p_succ)
    , mp_dist(p_dist)
    , mp_x(p_x)
    , mp_y(p_y) 
    , mp_normmat(p_normmat)
{}

Algorithm::term_type
MahalanobisDistance::step()
{
    static tuple_type::value_type diff,single;
    static matrix_type::value_type temp;
    diff.alloc(mp_x->val().rows());
    temp.alloc(1,mp_x->val().rows());
    single.alloc(1);

    matrix_set_diff(diff,mp_x->val(),mp_y->val());
    
    if (mp_normmat->val().rows()==0)
    {
	single(0,0) = matrix_square_norm(diff);
    }
    else
    {
	matrix_set_product(temp,transposed(diff),mp_normmat->val());
	matrix_set_product(single,temp,diff);
	if (single(0,0)<0)
	{ single(0,0)=1E-8; INVARIANT(false,"positiv definite matrix"); }
    }
    
    mp_dist->var() = single(0,0);

    TRACE_ID("distance",(*mp_dist)()<<"= mahalanobixdist("<<
	     (*mp_x)()<<","<<(*mp_y)()<<")");

    return call_step(UNKNOWN,mp_succ);
}
#endif
}; // namespace

#endif DISTANCE_SOURCE
