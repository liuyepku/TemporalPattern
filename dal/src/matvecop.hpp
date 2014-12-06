
#ifndef MATVECOP_HEADER
#define MATVECOP_HEADER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
template <class T> class DynMatrix;
#include <functional> // unary_function etc.
#include <algorithm> // min
#include <fstream> // i/o
#include "logtrace.h"
#include "define.hpp"
#include "stlutil.hpp" // compose
#include "primitives.hpp" // set_to etc.
#include "ustream.hpp" // read_white
#include "mread.hpp"

using namespace logtrace;

////////////////////////////////////////////////////////////////////
// Comparison

template <class M1,class M2> 
bool matrix_numeric_equal (const M1& A,const M2& B);

template <class M1,class M2> 
bool matrix_same_prefix(const M1& A,const M2& B,const int rs,const int cs);

template <class M1,class M2> 
bool matrix_same_suffix(const M1& A,const M2& B,const int rs,const int cs);

template <class M1,class M2> 
bool matrix_lexico_less(const M1& A, const M2& B);

template <class M1,class M2,class CMP>
class matrix_compare
  : public binary_function<M1,M2,compare_type>
{
public:
  matrix_compare(const CMP& cmp) : m_cmp_op(cmp) {}
  compare_type operator()(const M1&,const M2&) const;
private:
  CMP m_cmp_op;
};

///////////////////////////////////////////////////////////////
// Transformations

template <class M1,class BF1,class M2,class BF2,class M3> inline 
void
matrix_transform // f(A,g(B,C))
  (
        M1&  A,
  const BF1& f,
  const M2&  B,
  const BF2& g,
  const M3&  C
  )
  {
  int rs(min(A.rows(),B.rows())); // no of rows
  int cs(min(A.cols(),B.cols())); // no of cols
  for (int r=0;r<rs;++r)
    {
    for (int c=0;c<cs;++c)
      {
      f( A(r,c) , g( B(r,c), C(r,c) ) );
      }
    }
  }

template <class M1,class BF,class UF,class M2> inline 
void
matrix_transform // f(A,g(B))
  (
        M1& A,
  const BF& f,
  const UF& g,
  const M2& B
  )
  {
  int rs(min(A.rows(),B.rows())); // no of rows
  int cs(min(A.cols(),B.cols())); // no of cols
  for (int r=0;r<rs;++r)
    {
    for (int c=0;c<cs;++c)
      {
      f( A(r,c) , g( B(r,c) ) );
      }
    }
  }

template <class D,class BF1,class M1,class BF2,class M2> inline 
typename M1::value_type
matrix_transform_value // f(x,g(B,C))
  (
  const D    init,
  const BF1& f,
  const M1&  A,
  const BF2& g,
  const M2&  B
  )
  {
  INVARIANT( (A.rows()<=B.rows()) && (A.cols()<=B.cols()),
	     "operation assumes relaxed matrix dimensions");

  int rs(A.rows()); // no of rows
  int cs(A.cols()); // no of cols
  typename M1::value_type value(init); // return value
  for (int r=0;r<rs;++r)
    {
    for (int c=0;c<cs;++c)
      {
      f( value, g( A(r,c), B(r,c) ) );
      }
    }
  return value;  
  }

///////////////////////////////////////////////////////////
// Simple Matrix Operations

template <class M1,class D> inline void 
matrix_set_scalar(M1& A, const D s)
  {
  set_to<typename M1::value_type,D> assign_op;
  int rs(A.rows()); // no of rows
  int cs(A.cols()); // no of cols
  for (int r=0;r<rs;++r)
    {
    for (int c=0;c<cs;++c)
      {
      assign_op(A(r,c),s);
      }
    }
  }
template <class M1,class D> inline void 
matrix_inc_scalar(M1& A, const D s)
  {
  incr_by<typename M1::value_type,D> incr_op;
  int rs(A.rows()); // no of rows
  int cs(A.cols()); // no of cols
  for (int r=0;r<rs;++r)
    {
    for (int c=0;c<cs;++c)
      {
      incr_op(A(r,c),s);
      }
    }
  }
template <class V> inline void
matrix_set_vector(V& A,const char* ap_text)
  {

  {
    int r(1),maxc(1),c(1),i(0);
    while (ap_text[i]!='\0') 
      { 
      if (ap_text[i]==':') { ++r; c=1; } 
      if (ap_text[i]==',') { ++c; if (c>maxc) maxc=c; }
      ++i; 
      }
    A.adjust(r,c);
    matrix_set_scalar(A,-1);
  }
/*
  {
    istrstream is(ap_text,strlen(ap_text));
    matrix_read_colon(is,A);
  }
*/
  }
template <class M1,class D> inline 
void matrix_set_vector(M1& M, const D s)
  {
  set_to<typename M1::value_type,D> assign_op;
  M.adjust(1);
  assign_op(M[0],s);
  }
template <class M1,class D> inline 
void matrix_set_vector(M1& M, const D s, const D t)
  {
  set_to<typename M1::value_type,D> assign_op;
  M.adjust(2);
  assign_op(M[0],s);
  assign_op(M[1],t);
  }
template <class M1,class D> inline 
void matrix_set_vector(M1& M, const D s, const D t, const D u)
  {
  set_to<typename M1::value_type,D> assign_op;
  M.adjust(3);
  assign_op(M(0),s);
  assign_op(M(1),t);
  assign_op(M(2),u);
  }
template <class M1,class D> inline 
void matrix_set_vector(M1& M, const D s, const D t, const D u, const D v)
  {
  set_to<typename M1::value_type,D> assign_op;
  M.adjust(4);
  assign_op(M(0),s);
  assign_op(M(1),t);
  assign_op(M(2),u);
  assign_op(M(3),v);
  }
template <class M1,class D> inline 
void matrix_set_vector(M1& M, const D s, const D t, const D u,
  const D v, const D w)
  {
  set_to<typename M1::value_type,D> assign_op;
  M.adjust(5);
  assign_op(M(0),s);
  assign_op(M(1),t);
  assign_op(M(2),u);
  assign_op(M(3),v);
  assign_op(M(4),w);
  }
template <class M1,class D> inline 
void matrix_set_track(M1& M, const D s, const D t)
  {
  set_to<typename M1::value_type,D> assign_op;
  M.adjust(2,2);
  assign_op(M(0,0),s);
  assign_op(M(1,1),t);
  }
template <class M1,class D> inline 
void matrix_set_track(M1& M, const D s, const D t, const D u)
  {
  set_to<typename M1::value_type,D> assign_op;
  M.adjust(3,3);
  assign_op(M(0,0),s);
  assign_op(M(1,1),t);
  assign_op(M(2,2),u);
  }
template <class M1,class D> inline 
void matrix_set_track(M1& M, const D s, const D t, const D u, const D v)
  {
  set_to<typename M1::value_type,D> assign_op;
  M.adjust(4,4);
  assign_op(M(0,0),s);
  assign_op(M(1,1),t);
  assign_op(M(2,2),u);
  assign_op(M(3,3),v);
  }
template <class M1,class D> inline 
void matrix_set_track(M1& M, const D s, const D t, const D u,
  const D v, const D w)
  {
  set_to<typename M1::value_type,D> assign_op;
  M.adjust(5,5);
  assign_op(M(0,0),s);
  assign_op(M(1,1),t);
  assign_op(M(2,2),u);
  assign_op(M(3,3),v);
  assign_op(M(4,4),w);
  }
template <class M1> inline 
void matrix_set_identity(M1& M)
  {
  //M.clear();
  matrix_set_scalar(M,0);
  int n( min(M.rows(),M.cols()) );
  for (int i=0;i<n;++i) M(i,i) = (typename M1::value_type)1;
  }
template <class M1> inline 
bool matrix_is_identity(const M1& M)
  {
  int rs(M.rows()); // no of rows
  int cs(M.cols()); // no of cols
  int r,c;
  typename M1::value_type one(1),zero(0);
  num_equal<typename M1::value_type,typename M1::value_type> equal;
  
  for (r=0;r<rs;++r)
    {
    for (c=0;c<cs;++c)
      {
      if (r==c) { if (!equal(M(r,c),one)) return false; }
      else      { if (!equal(M(r,c),zero)) return false; }
      }
    }

  return true;
  }

template <class M1,class M2> inline 
void matrix_set(M1& A,const M2& B)
  {
  matrix_transform
    (
    A,
    set_to<typename M1::value_type,typename M2::value_type>(),
    identity<typename M2::value_type>(),
    B
    );
  }
template <class M1,class M2> inline 
void matrix_copy(M1& A,const M2& B)
  {
  // A=B; does not work in case A and B are different types!
  A.alloc(B.rows(),B.cols()); 
  matrix_set(A,B);
  }

template <class M1,class M2> inline 
void matrix_inc(M1& A,const M2& B)
  {
  matrix_transform
    (
    A,
    incr_by<typename M1::value_type,typename M2::value_type>(),
    identity<typename M2::value_type>(),
    B
    ); 
  }

template <class M1,class M2> inline 
void matrix_dec(M1& A,const M2& B)
  {
  matrix_transform
    (
    A,
    decr_by<typename M1::value_type,typename M2::value_type>(),
    identity<typename M2::value_type>(),
    B
    ); 
  }

template <class M1,class M2> inline 
void matrix_set_diff(M1& A, const M2& B, const M2& C)
  {
  matrix_transform
    (
    A,
    set_to<typename M1::value_type,typename M2::value_type>(),
    B,
    minus<typename M2::value_type>(),
    C
    ); 
  }

template <class D,class M1> inline 
void matrix_scale(M1& A, const D s)
  {
  matrix_transform
    (
    A,
    set_to<typename M1::value_type,typename M1::value_type>(),
    bind1st(multiplies<typename M1::value_type>(),s),
    A
    ); 
  }

template <class M1,class D,class M2> inline 
void matrix_set_scaled(M1& A, const D s, const M2& B)
  {
  matrix_transform
    (
    A,
    set_to<typename M1::value_type,typename M2::value_type>(),
    bind1st(multiplies<typename M2::value_type>(),s),
    B
    ); 
  }

template <class M1,class D,class M2> inline 
void matrix_inc_scaled(M1& A,const D s, const M2& B)
  { 
  matrix_transform
    (
    A,
    incr_by<typename M1::value_type,typename M2::value_type>(),
    bind1st(multiplies<typename M2::value_type>(),s),
    B
    ); 
  }

template <class M1,class D,class M2> inline 
void matrix_dec_scaled(M1& A,const D s, const M2& B)
  { 
  matrix_transform
    (
    A,
    decr_by<typename M1::value_type,D>(),
    bind1st(multiplies<typename M2::value_type>(),s),
    B
    ); 
  }

template <class M1,class M2> inline 
void matrix_div(M1& A,const M2& B)
  {
  matrix_transform
    (
    A,
    div_by<typename M1::value_type,typename M2::value_type>(),
    identity<typename M2::value_type>(),
    B
    ); 
  }

template <class M1,class D1,class M2,class D2> inline 
void matrix_set_combi(M1& A,const D1 s,const M2& B,const D2 t,const M2& C)
  { 
  matrix_transform
    (
    A,
    set_to<typename M1::value_type,typename M2::value_type>(),
    B,
    compose
      (
      plus<typename M2::value_type>(),
      bind2nd(multiplies<typename M2::value_type>(),s),
      bind2nd(multiplies<typename M2::value_type>(),t) 
      ),
    C
    ); 
  }

template <class M1,class D1,class M2,class D2> inline 
void matrix_inc_combi(M1& A,const D1 s,const M2& B,const D2 t,const M2& C)
  { 
  matrix_transform
    (
    A,
    incr_by<typename M1::value_type,typename M2::value_type>(),
    B,
    compose(
      plus<typename M2::value_type>(),
      bind1st(multiplies<typename M2::value_type>(),s),
      bind1st(multiplies<typename M2::value_type>(),t) 
      ),
    C
    ); 
  }

template <class M1,class M2> inline 
typename M1::value_type matrix_dot_product(const M1& A, const M2& B)
  { 
  return matrix_transform_value
    (
    0,
    incr_by<typename M1::value_type,typename M2::value_type>(),
    A,
    multiplies<typename M1::value_type>(),
    B
    ); 
  }

template <class M1,class M2,class M3> inline 
void vector_cross_product(M1& A, const M2& B, const M3& C)
  {
  INVARIANT( (A.rows()<=B.rows()) && (A.cols()<=B.cols()) &&
             (A.rows()<=C.rows()) && (A.cols()<=C.cols()),
             "operation assumes relaxed matrix dimensions");
  INVARIANT( (A.rows()==3) && (A.cols()==1),
             "operation assumes 3d vector");
  A[0] =   B[1]*C[2]-B[2]*C[1] ;
  A[1] = -(B[0]*C[2]-B[2]*C[0]);
  A[2] =   B[0]*C[1]-B[1]*C[0] ;
  }

template <class M1,class M2,class M3> inline 
void matrix_set_product(M1& R, const M2& A, const M3& B)
  {
  set_to<typename M1::value_type,float> assign_op;
  INVARIANT( (A.cols()==B.rows()) , "matrix_set_product: dimension mismatch");
  int r,c,i;
  TData h;
  // Gr"o\3e anpassen
  R.adjust(A.rows(),B.cols());
  // Multiplikation A=B*C ausf"uhren
  for (r=0;r<R.rows();r++)
    // HOLD n=c+r*col
    for (c=0;c<R.cols();c++)
      {
      h=0;
      // i-te Spalte (row) von A mit i-ter Zeile (col) von B multiplizieren
      for (i=0;i<A.cols();i++)
        h+=(TData)(A(r,i)*B(i,c));
      assign_op(R(r,c),h);
      }
  }

template <class M1,class M2,class M3> inline 
void matrix_inc_scaled_product(M1& R,const typename M1::value_type s,const M2& A, const M3& B)
  {
  incr_by<typename M1::value_type,float> incr_op;
  INVARIANT( (A.cols()==B.rows()) , "matrix_set_product: dimension mismatch");
  int r,c,i;
  TData h;
  // Gr"o\3e anpassen
  R.adjust(A.rows(),B.cols());
  // Multiplikation A=B*C ausf"uhren
  for (r=0;r<R.rows();r++)
    // HOLD n=c+r*col
    for (c=0;c<R.cols();c++)
      {
      h=0;
      // i-te Spalte (row) von A mit i-ter Zeile (col) von B multiplizieren
      for (i=0;i<A.cols();i++)
        h+=(TData)(A(r,i)*B(i,c));
      incr_op(R(r,c),s*h);
      }
  }
template <class M> inline
typename M::value_type matrix_det(const M& A)
  {
  typename M::value_type d(0);
  INVARIANT(A.rows()==A.cols(),"det(nxn matrix)");

  if (A.rows()==2)
    {
    d = A(0,0)*A(1,1) - A(0,1)*A(1,0);
    }
  else
    {
    DynMatrix<typename M::value_type> buffer;
    buffer.adjust(A.rows(),A.cols());
    matrix_set(buffer,A);
    typename M::value_type factor,pivot;
    int col,row,i;

    for (col=0;col<A.cols();++col)
      {
      pivot = buffer(col,col);
      if (pivot==0) { return 0; }
      else
        {
        for (row=col+1;row<A.rows();++row)
          {
          factor = -buffer(row,col) / pivot;
          for (i=0;i<A.cols();++i)
            {
            buffer(row,i) += buffer(col,i) * factor;
            }
          }
        }
      }

    d = 1;
    for (col=0;col<A.cols();++col)
      { d *= buffer(col,col); }
    }
  return d;
  }

template <class M1> inline 
typename M1::value_type matrix_square_norm(const M1& A)
  { 
  return matrix_transform_value
    (
    0,
    incr_by<typename M1::value_type,typename M1::value_type>(),
    A,
    multiplies<typename M1::value_type>(),
    A
    ); 
  }

template <class M1,class M2> inline 
typename M1::value_type matrix_maximum_distance(const M1& A, const M2& B)
  { 
  return matrix_transform_value
    (
    0,
    set_to_max<typename M1::value_type,typename M1::value_type>(),
    A,
    difference_abs<typename M1::value_type,typename M2::value_type>(),
    B
    ); 
  }

template <class M1,class M2> inline 
typename M1::value_type matrix_square_distance(const M1& A, const M2& B)
  { 
  return matrix_transform_value
    (
    0,
    incr_by<typename M1::value_type,typename M1::value_type>(),
    A,
    difference_sqr<typename M1::value_type,typename M2::value_type>(),
    B
    ); 
  }

/** Granularization. Returns location in vector of interval bounds,
    input (3 4 8) refers to 4 intervals ]-oo:3[, [3:4[, [4:8[, [8,oo[,
    return value is index of interval (starting with 0) to which val
    belongs. */

template <class V>
int vector_range
  (
  const V& bounds,
  const typename V::value_type val
  )
  {
  const int dim = bounds.rows();
  if (dim<=0) return -1;

  int i=0;
  while ( (i<dim) && (val >= bounds[i]) ) ++i;
  return i;
  }

template <class V1> inline 
void 
vector_concat_scalar
  (
  V1& A,
  const typename V1::value_type& x
  )
  {
  const int size = A.rows();
  A.adjust(size+1); A[size]=x;
  }

/** Concatenate vector \c A and \c B, result in \c A. */

template <class V1,class V2> inline 
void 
vector_concat
  (
  V1& A,
  const V2& B
  )
  {
  int oldsize = A.rows();
  A.adjust(oldsize+B.rows());
  for (int i=0;i<B.rows();++i) A[oldsize+i]=B[i];
  }

/** The parameter \c A is considered as a signal. The function
    calculates the derivative of \c A by differencing. */

template <class V> inline 
void 
vector_differentiate(V& A)        
  {
  for (int i=A.size()-1;i>0;--i) A[i]-=A[i-1];
  A[0]=A[1];
  }

/** The parameter \c A is considered as a signal. The function
    calculates the integral of \c A by summation. */

template <class V> inline 
void 
vector_integrate(V& A)        
  {
  for (int i=1;i<A.size();++i)
    A[i] += A[i-1];
  }

/** We turn every element of the tuple \c A into its absolute value. */

template <class V> inline 
void 
vector_absolute(V& A)        
  {
  for (int i=0;i<A.size();++i)
    A[i] = fabs(A[i]);
  }

template <class V> inline 
void 
vector_orthogonalize(V& A)        
  {
  INVARIANT(A.rows() > 1,"orthogonalize assumes vector (rows>1)");
  INVARIANT(A.cols() == 1,"orthogonalize assumes vector (cols==1)");

  int non_zero_axis[2];
  int non_zero_counter = 0;
  int axis = 0;
  while (    (non_zero_counter<2) 
          && (axis<A.rows())
        )
    {
    if (A(axis)!=0.0)
      {
      non_zero_axis[non_zero_counter] = axis;
      ++non_zero_counter;
      }
    ++axis;
    }
  INVARIANT(non_zero_counter!=0, "zero-vector cannot be orthogonalized");
  if (non_zero_counter==1)
    {
    axis = non_zero_axis[0];
    if (axis==0)
      {
      A(axis+1) = 1.0; // Assumption: A.rows()>2, see invariant above
      }
    else
      {
      A(axis-1) = 1.0; // axis is not the first axis since axis>0
      }
    A(axis) = 0.0;
    }
  else // non_zero_counter>1
    {
    swap(A(non_zero_axis[0]),A(non_zero_axis[1]));
    A(non_zero_axis[0]) *= -1.0;
    for (axis=0;axis<A.rows();++axis)
      {
      if ((axis!=non_zero_axis[0]) && (axis!=non_zero_axis[1]))
        {
        A(axis) = 0.0;
        }
      }
    }
  }

template <class M1> inline 
void matrix_normalize(M1& A)
  {
  typename M1::value_type n(sqrt(matrix_square_norm(A)));
  if (n!=0) matrix_scale(A,1.0/sqrt(matrix_square_norm(A)));
  }

template <class T>
class StandardIO
  {
  public:
    inline void read(istream& is,T& val,const char* = NULL) const { is >> val; }
    inline void write(ostream& os,const T& val) const { os << val; }
    inline bool symbolic() const { return false; }
  };

#endif /* MATVECOP_HEADER */
