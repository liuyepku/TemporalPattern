
/** \file matinvert.hpp
    \author Frank Hoeppner <frank.hoeppner@ieee.org>
    \brief Functions for matrix inversion.

    This file implements the Gauss-Jordan method to invert matrizes.

    We do not implement a single \c matrix_invert function for all
    types of matrizes, because diagonal matrizes can be inverted much
    easier than other matrizes. Furthermore, diagonal matrizes do not
    allow an assignment to elements offside the diagonal. A generic
    function would access all elements and is not useable for diagonal
    matrizes.

    Therefore, we have different functions for matrix inversion. But
    every matrix type implements (or specializes) the \c matrix_invert
    function for appropriately. The specializations will call an invert
    function which has been implemented here. 

    \todo In fact, specialization is only possible for an explicitly
    known type, for example for \c DiagMatrix<2,2,float> or \c
    DiagMatrix<4,4,double>, but not in general for all kinds of \c
    DiagMatrix. Defining an \c .invert method and a \c
    matrix_invert(A) function that calls A.invert() is probably the
    best solution. */

#ifndef FH_MATINVERT_HEADER
#define FH_MATINVERT_HEADER

/** Gauss-Jordan Inversion.

    This function inverts a matrix of full rank using the method of
    Gauss and Jordan \cite{Stoer:93}, page 161. The result is returned
    in the argument \a A. */

template <class M>
void gauss_jordan(M& A);

#endif // FH_MATINVERT_HEADER





