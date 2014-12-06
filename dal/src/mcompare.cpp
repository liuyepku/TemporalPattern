

#ifndef MCOMPARE_SOURCE
#define MCOMPARE_SOURCE

/** Vergleich auf numerische Gleichheit auf den ersten rs Zeilen und
    cs Spalten. Vorbedingung: Beide Matrizen besitzen wenigstens rs
    Zeilen und cs Spalten. */

template <class M1,class M2>
bool 
matrix_same_prefix
(
  const M1& A, 
  const M2& B,
  const int rs,
  const int cs
)
{
  LOG_FUNCTION("MATH","matrix_same_prefix","");
  PRECONDITION(rs<=min(A.rows(),B.rows()),"");
  PRECONDITION(cs<=min(A.cols(),B.cols()),"");

  num_equal<typename M1::value_type,typename M2::value_type> equal;  

  for (int r=0;r<rs;++r)
  {
    for (int c=0;c<cs;++c)
    {
      if (!equal( A(r,c),B(r,c) )) return false;
    }
  }
  return true;
}

/** Vergleich auf numerische Gleichheit auf den letzten rs Zeilen und
    cs Spalten. Vorbedingung: Beide Matrizen besitzen wenigstens rs
    Zeilen und cs Spalten. */

template <class M1,class M2> 
bool 
matrix_same_suffix
(
  const M1& A, 
  const M2& B,
  const int rs,
  const int cs
)
{
  LOG_FUNCTION("MATH","matrix_same_suffix","");
  PRECONDITION(rs<min(A.rows(),B.rows()),"");
  PRECONDITION(cs<min(A.cols(),B.cols()),"");

  int rowsa(A.rows());
  int rowsb(B.rows());
  int colsa(A.cols());
  int colsb(B.cols());    
  num_equal<typename M1::value_type,typename M2::value_type> equal;

  for (int r=0;r<rs;++r)
  {
    for (int c=0;c<cs;++c)
    {
      if (!equal( A(rowsa-r,colsa-c),B(rowsb-r,colsb-c) )) return false;
    }
  }    
  return true;
}

/** Vergleich auf numerische Gleichheit. Es wird nur ein Test auf
    Enthaltensein durchgefuehrt (ob alle Elemente in A gleich mit
    denen in B sind, B darf groesser als A sein), fuer echte
    Gleichheit muss noch Gleichheit in Anzahl Spalten/Zeilen geprueft
    werden. */

template <class M1,class M2> inline
bool 
matrix_numeric_equal 
(
  const M1& A, 
  const M2& B
)
{
  LOG_FUNCTION("MATH","matrix_numerical_equal","");
  return matrix_same_prefix(A,B,A.rows(),A.cols());
}

/** Lexicographische Ordnung auf Matrizen -- reduziert auf '<' (siehe
    matrix_compare Klasse fuer detaillierteren Vergleich) */

template <class M1,class M2> 
bool 
matrix_lexico_less 
(
  const M1& A, 
  const M2& B
)
{
  default_operator_compare<typename M1::value_type,typename M2::value_type> c1;
  matrix_compare<M1,M2,default_operator_compare<typename M1::value_type,typename M2::value_type> > c2(c1);
  return c2(A,B)==cmp_less;
}

/** Lexicographischer Vergleich von Matrizen mit Rueckgabewert
    compare_type. Es koennen Matrizen unterschiedlicher Groesse
    verglichen werden, Matrizen die ein Praefix einer anderen Matrix
    sind kommen dabei lexicographisch zuerst. */

template <class M1,class M2,class CMP>
compare_type matrix_compare<M1,M2,CMP>::operator()
(
  const M1& A,
  const M2& B
)
const
{
  int rs(min(A.rows(),B.rows())); // no of rows
  int cs(min(A.cols(),B.cols())); // no of cols
  compare_type result;

  for (int r=0;r<rs;++r)
  {
    for (int c=0;c<cs;++c)
    {
      result=m_cmp_op(A(r,c),B(r,c));
      if (result!=cmp_equal) return result;
    }
  }

  // in case of different no. of cols: comparison not defined!
  if (A.cols()!=B.cols()) return cmp_none;
  // same number of cols; equal elements so far; 
  return (A.rows()<B.rows()) ? cmp_less :
        ((A.rows()==B.rows()) ? cmp_equal : cmp_greater );
}

#endif // MCOMPARE_SOURCE
