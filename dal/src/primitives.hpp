
/** \file primitives.hpp
    \author Frank Hoeppner <frank.hoeppner@ieee.org>
    \brief Primitive numerical operations.

    This file provides some very basic math functions. Amongst others
    we redefine assignment, increment, decrement, equality, etc.  In
    contrast to the default C/C++ operations we make these operations
    type-sensitive. For example, some basic operators like \c = or \c
    += are re-implemented type-sensitive by means of specialized
    templates set_to() and incr_by(). This way we want to achieve that
    an assignment of 0.52 to an integer variable is rounded to 1
    (instead of 0). */

#ifndef FH_PRIMITIVES_HEADER
#define FH_PRIMITIVES_HEADER

#include <functional> // STL binary_function
#include <complex> // complex<DATA>
#include <iostream>

/** Used for indices type */
//typedef int index_type;

/** Sign of a value.

    This function returns $1$ for positive numbers, $-1$ for negative
    numbers and $0$ for zero. */

template <class DATA> inline int
signum
(
  const DATA& a_value
)
{
  return (a_value>0) ? 1 : ( (a_value<0) ? -1 : 0 );
}

/** Shift value into interval.

    This function takes an interval with minimum value \a a_min and
    maximum value \a a_max, and a value \c a_value that should stay
    within this interval. The function returns the current value if it
    already lies between minium and maximum and otherwise the nearest
    bound. */

template <class DATA> inline
DATA
bound
 (
  const DATA a_min,
  const DATA a_value,
  const DATA a_max
)
{
  return (a_value<a_min) ? a_min : ( (a_value>a_max) ? a_max : a_value );
}

/** Check if value lies in interval. 

    Similar to the bound() function, but the adjusted value is
    returned in the \a a_value argument and the function returns 0 if
    the value stays within its bounds, 1 otherwise.

    \attention Don't interpret the integer return value as a boolean
    value. Consider the following piece of code:

    \code
    float x(-1),y(2);
    bool ok = (respect_bounds(0,x,1)) && (respect_bounds(0,y,1));
    \endcode

    In this case the first function call returns 0, which is
    interpreted as \c false. No evaluation of the second function call
    is necessary to get the final result of \c ok. Therefore, at the
    end we have \c x=0 but still \c y=2.  This is probably not what we
    wanted to achieve. We should interpret the return value as a
    change counter:

    \code
    float x(-1),y(2);
    bool fail_ctr = (respect_bounds(0,x,1)) + (respect_bounds(0,y,1));
    \endcode

    Now every function call has to be executed by the program and
    adjustment took place if \c fail_ctr>0. */

template <class DATA> inline
int
respect_bounds
  (
  const DATA a_min,
  DATA& a_value,
  const DATA a_max
  )
  {
  if (a_value<a_min) { a_value = a_min; return 1; }
  else if (a_value>a_max) { a_value = a_max; return 1; }
  else { return 0; }
  }

/** Result of a comparison.

    In general \c operator< is sufficient to check for \c a==b by
    means of \f$ \neg (a<b) \wedge \neg (b>a) \f$. But if comparison
    is an expensive operation, it is often cheaper to check for \c
    a<b, \c a==b, and \c a>b simultaneously instead of calling an \c
    operator< twice. We therefore introduce the \c compare_type which
    denotes the result of a comparison. The fourth result type \c
    cmp_none becomes important if we do not have a total order. */

enum compare_type { cmp_less=-1,cmp_equal=0,cmp_greater=1,cmp_none=99 };

/** comparison via \c operator<.

    If we have defined \c operator<, we obtain a comparison operator
    from the template \c default_operator_compare. 

    \see compare_type. */

template <class M1,class M2>
struct default_operator_compare
  : public binary_function<M1,M2,compare_type>
  {
  inline compare_type operator()(const M1& a,const M2& b) const
    { return (a<b)?cmp_less:
            ((b<a)?cmp_greater:cmp_equal); }
  };

/** comparison for bit inclusion.

    An example for a non-total order is given by bit-operations. We
    interpret integer numbers as set-representatives, e.g. \f$
    5_{\mbox{dec}}=101_{\mbox{bin}} \f$ as \f$\{4,1\}\f$ or
    \f$\{2^2,2^0\}\f$. Then we can define an order for sets \c A and
    \c B as follows: \f$ A<B :\Leftrightarrow A\cup B=A \f$ (or
    equivalently \f$A\subset B\f$). Note that we do not have always
    \f$A\subset B\f$ or \f$B\subset A\f$, therefore the comparison
    might return \c cmp_none.

    \see compare_type. */

struct bit_inclusion_compare
  : public binary_function<int,int,compare_type>
  {
  inline compare_type operator()(const int a,const int b) const
    { return (  (a==b)    ? cmp_equal:
             ( ((a&b)==a) ? cmp_less:
             ( ((a&b)==b) ? cmp_greater:cmp_none   ))); }
  };

/** Minimum of three values. */

template <class T>
inline T min(const T& a,const T& b,const T& c)
  { return min(min(a,b),c); }

/** Minimum of four values. */

template <class T>
inline T min(const T& a,const T& b,const T& c,const T& d)
  { return min(min(a,b),min(c,d)); }

/** Maximum of three values. */

template <class T>
inline T max(const T& a,const T& b,const T& c)
  { return max(max(a,b),c); }

/** Maximum of four values */

template <class T>
inline T max(const T& a,const T& b,const T& c,const T& d)
  { return max(max(a,b),max(c,d)); }

/** Type sensitive assignment.

    The set_to template implements the assignment operator [[=]]. Its
    implementation consists of several specializations and considers
    mixed integer and floating point datatypes. */

template <class S,class T>
struct set_to : public binary_function<S, T, void> 
  {
  inline void operator()(S& x, const T& y) const;
  };

/* The generic [[set_to]] implementation looks like this: */

template <class S,class T>
inline void set_to<S,T>::operator()(S& x, const T& y) const 
  { x = (S) y; }

/* If we assign a floating point number (float or double) to an
integer value int or long), we round the floating point value before
assignment. */

template<>
inline void set_to<int,float>::operator()(int& x,const float& y) const 
  { x = (int) ((y<0) ? y-0.5 : y+0.5); }

template<>
inline void set_to<long,float>::operator()(long& x,const float& y) const 
  { x = (int) ((y<0) ? y-0.5 : y+0.5); }

template<>
inline void set_to<int,double>::operator()(int& x,const double& y) const 
  { x = (int) ((y<0) ? y-0.5 : y+0.5); }

template<>
inline void set_to<long,double>::operator()(long& x,const double& y) const 
  { x = (int) ((y<0) ? y-0.5 : y+0.5); }

/** Type sensitive equality check.

    Another type-sensitive operation is numerical equality (\c
    operator==). Depending on the concerned types we want to take
    different rounding errors into account. Equality is decided
    numerically, e.g. two numbers are equal if they differ only by
    \f$\varepsilon\f$ from each other. The exact value of \f$\varepsilon\f$
    depends on the floating point types under consideration. If we
    have different types to compare, the \f$\varepsilon\f$ of the less
    precise type is used. And we have to use the relative
    error:

    \f[ \frac{ 2 |x-y| }{ 1 + |x| + |y| | } \le \varepsilon 
        \Leftrightarrow |x-y| \le \frac{\varepsilon}{2} (1+|x|+|y|) \f] 

    (We need the summand 1 in case \c x or \c y is zero.) */

template <class S,class T> 
struct num_equal : public  binary_function<S,T,bool> 
  {
  inline bool operator()(const S&,const T&) const;
  };

/* We use the following constants for \f$\frac{\varepsilon}{2}\f$: */

#define EPSILON_INT 0.5
#define EPSILON_FLOAT 1E-4
#define EPSILON_DOUBLE 1E-8

/* The generic \c num_equal implementation uses the default \c ==
   operator. */

template <class S,class T>
inline bool num_equal<S,T>::operator()(const S& x, const T& y) const 
  { 
  //warning("no specialization for comparison...");
    //warning();
    //std::cout<<"no specialization for comparison..."<<std::endl;
  return (x==y); 
  }

/* For comparison between different floating point types, we use the
   abovementioned strategy by means of the following
   specializations. */

#define ONEf ((float)1.0)
#define ONEd ((double)1.0)

template<>
inline bool num_equal<word,word>::operator()(const word& x, const word& y) const 
  { return (x==y); }

template<>
inline bool num_equal<int,int>::operator()(const int& x, const int& y) const 
  { return (x==y); }

template<>
inline bool num_equal<long int,long int>::operator()(const long int& x, const long int& y) const 
  { return (x==y); }

template<>
inline bool num_equal<double,int>::operator()(const double& x, const int& y) const 
  { return (fabs(x-y) <= EPSILON_INT*(ONEf+fabs(x)+fabs(y))); }

template<>
inline bool num_equal<float,int>::operator()(const float& x, const int& y) const 
  { return (fabs(x-y) <= EPSILON_FLOAT*(ONEf+fabs(x)+fabs(y))); }

template<>
inline bool num_equal<double,double>::operator()(const double& x, const double& y) const 
  { return (fabs(x-y) <= EPSILON_DOUBLE*(ONEd+fabs(x)+fabs(y))); }

template<>
inline bool num_equal<float,double>::operator()(const float& x, const double& y) const 
  { return (fabs(x-y) <= EPSILON_FLOAT*(ONEf+fabs(x)+fabs(y))); }

template<>
inline bool num_equal<double,float>::operator()(const double& x, const float& y) const 
  { return (fabs(x-y) <= EPSILON_FLOAT*(ONEf+fabs(x)+fabs(y))); }

template<>
inline bool num_equal<float,float>::operator()(const float& x, const float& y) const 
  { return (fabs(x-y) <= EPSILON_FLOAT*(ONEf+fabs(x)+fabs(y))); }

template<>
inline bool num_equal< complex<float>, float >::operator()(const complex<float>& x, const float& y) const 
  { float n( ONEf+norm(x)+fabs(y) );
    return (fabs(x.real()-y) <= EPSILON_FLOAT*n) && 
           (fabs(x.imag()) <= EPSILON_FLOAT*n); }
template<>
inline bool num_equal< float,complex<float> >::operator()(const float& x, const complex<float>& y) const 
  { float n( ONEf+fabs(x)+norm(y) );
    return (fabs(y.real()-x) <= EPSILON_FLOAT*n) && 
           (fabs(y.imag()) <= EPSILON_FLOAT*n); }

template<>
inline bool num_equal< complex<float>,complex<float> >::operator()(const complex<float>& x, const complex<float>& y) const 
  { float n( ONEf+norm(x)+norm(y) );
    return (fabs(x.real()-y.real()) <= EPSILON_FLOAT*n) && 
           (fabs(x.imag()-y.imag()) <= EPSILON_FLOAT*n); }

template<>
inline bool num_equal< complex<float>,double >::operator()(const complex<float>& x, const double& y) const 
  { float n( ONEf+norm(x)+fabs(y) );
    return (fabs(x.real()-y) <= EPSILON_FLOAT*n) && 
           (fabs(x.imag()) <= EPSILON_FLOAT*n); }

template<>
inline bool num_equal< complex<double>,double >::operator()(const complex<double>& x, const double& y) const 
  { double n( ONEd+norm(x)+fabs(y) );
    return (fabs(x.real()-y) <= EPSILON_DOUBLE*n) && 
           (fabs(x.imag()) <= EPSILON_DOUBLE*n); }

template<>
inline bool num_equal< complex<double>,float >::operator()(const complex<double>& x, const float& y) const 
  { double n( ONEd+norm(x)+fabs(y) );
    return (fabs(x.real()-y) <= EPSILON_FLOAT*n) && 
           (fabs(x.imag()) <= EPSILON_FLOAT*n); }

template<>
inline bool num_equal< complex<double>,complex<double> >::operator()(const complex<double>& x, const complex<double>& y) const 
  { double n( ONEd+norm(x)+norm(y) );
    return (fabs(x.real()-y.real()) <= EPSILON_DOUBLE*n) && 
           (fabs(x.imag()-y.imag()) <= EPSILON_DOUBLE*n); }

/*
\begin{comment} test code
<<test>>=
  complex<float> x(-1.31107e+09, 1.6056e-07);
  complex<float> z(-1.31107e+09,0.0);
  float y(-1.31107e+09);
  cout << equal(x,y) << endl;
  cout << equal(x,z) << endl;
  cout << equal(z,y) << endl;
*/


/** Type sensitive equality comparison.

    \see num_equal */

template <class S,class T>
inline bool equal(const S& s,const T& t) 
{ num_equal<S,T> eq; return eq(s,t); }

/** Type sensitive incrementation. 

    \todo refine incr_by by template specialization */

template <class S,class T>
struct incr_by : public binary_function<S, T, void> 
  {
  inline void operator()(S& x, const T& y) const;
  };

/* default implementation */

template <class S,class T>
inline void incr_by<S,T>::operator()(S& x, const T& y) const
  { x += (S) y; }

/** Type sensitive decrementation.

    \todo refine decr_by by template specialization */

template <class S,class T>
struct decr_by : public binary_function<S, T, void> 
  {
  inline void operator()(S& x, const T& y) const;
  };

/* default implementation */

template <class S,class T>
inline void decr_by<S,T>::operator()(S& x, const T& y) const
  { x -= (S) y; }

/** Type sensitive squared incrementation.

    \todo refine incr_by_sqr by template specialization */

template <class S,class T>
struct incr_by_sqr : public binary_function<S, T, void> 
  {
  void operator()(S& x, const T& y) const { x += (S) (y*y); }
  };

/** Type sensitive maximum. 

    \todo refine set_to_max by template specialization */

template <class S,class T>
struct set_to_max : public binary_function<S, T, void> 
  {
  void operator()(S& x, const T& y) const { if (y>x) x=y; }
  };

/** Type sensitive absolute difference.

    \todo refine difference_abs by template specialization */

template <class S,class T>
struct difference_abs : public binary_function<S, T, S> 
  {
  S operator()(const S& x, const T& y) const { return fabs(x-y); }
  };

/** Type sensitive squared difference.

    \todo refine difference_sqr by template specialization */

template <class S,class T>
struct difference_sqr : public binary_function<S, T, S> 
  {
  S operator()(const S& x, const T& y) const { S h = x-y; return h*h; }
  };

/** Type sensitive division.

    \todo refine div_by by template specialization. */

template <class S,class T>
struct div_by : public binary_function<S, T, void> 
  {
  inline void operator()(S& x, const T& y) const;
  };

/* default implementation */

template <class S,class T>
inline void div_by<S,T>::operator()(S& x, const T& y) const
  { x /= (S) y; }


#endif // FH_PRIMITIVES_HEADER
