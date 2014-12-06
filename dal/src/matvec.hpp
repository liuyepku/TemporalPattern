
/** \file matvec.hpp
    \author Frank Hoeppner <frank.hoeppner@ieee.org>
    \brief Einfache Vektoren- und Matrizenklassen

    Die Philosophie hinter diesen Matrix/Vektor-Klassen ist nur, den
    Umgang mit den statischen oder dynamischen Arrays,
    Speicherallokation, etc. zu vereinheitlichen, so dass verschiedene
    Implementationen ausgetauscht werden koennen. Diese
    Implementationen sind alle "so inline wie moeglich". Die "echte"
    Funktionalitaet ist in Template-Funktionen fuer alle Klassen
    geschrieben. */

#ifndef MATVEC_HEADER
#define MATVEC_HEADER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "matvecop.hpp"
#include <string.h> // memcpy

/** Neudimensionierung eines 1D/2D Feldes (zeilenweise gespeichert)
    unter Beibehaltung der zuvor bereits gesetzten Eintraege. */
 
template <class T>
T* adjust(T* p_field,int &rows,int &cols,int &cap,int newrows,int newcols)
{
  int newcap = newrows*newcols;
  T* p;

  if (((cols==1)&&(newcols==1))||((rows==1)&&(newrows==1)))
  { // Kopie von Vektor nach Vektor ist unkritisch, Reihenfolge bleibt identisch
    if (newcap>cap)
    { // mehr Speicher erforderlich
      p = new T[newcap];
      memcpy(p,p_field,sizeof(T)*cap);
      delete[] p_field;
    }
    else
    { // bisheriger Speicher reicht, alles bleibt
      newcap = cap;
      p = p_field;
    }
  }
  else 
  { // echte Matrixkopie (kein Vektor)
    if (newcap>cap)
    { // mehr Speicher erforderlich
      p = new T[newcap];
      for (int i=0;i<min(rows,newrows);++i)
        for (int j=0;j<min(cols,newcols);++j)
          p[i*newcols+j] = p_field[i*cols+j];
      delete[] p_field;
    }
    else 
    { // bisheriger Speicher reicht, lokale Doppelkopie (kann man
      // vielleicht besser machen)
      T temp[newrows][newcols];
      for (int i=0;i<min(rows,newrows);++i)
        for (int j=0;j<min(cols,newcols);++j)
          temp[i][j] = p_field[i*cols+j];
      memcpy(p_field,temp,sizeof(T)*newcap);
      newcap = cap;
      p = p_field;
    }
  }

  // Feld hat neue Dimensionen
  rows=newrows; cols=newcols; cap=newcap;
  return p;
}

/** Makros zur Vereinfachung von Bereichspruefungen. */

#define MVRNG(val,upp) \
  INVARIANT((0<=(val))&&((val)<(upp)),"matrix access error 0<="<<(val)<<'<'<<(upp));
#define MVZERO(c) \
  INVARIANT(c==0,"vector type, illegal request c="<<c);
#define MVONE(c) \
  INVARIANT(c==1,"vector type, illegal adjust request c="<<c);

/** Vektor mit dynamischer Groesse. Einmal allokierter Speicher wird
    erst wieder im Destruktor freigegeben. */

template <class T>
class DynTuple
{
public:
  typedef T value_type;

  DynTuple()
    : m_array_size(0), mp_field(NULL), m_rows(0) {}
  DynTuple(const DynTuple& M)
    : m_array_size(M.m_rows), mp_field(NULL), m_rows(M.m_rows)
    { if (m_array_size>0) { mp_field=new T[m_array_size]; memcpy(mp_field,M.mp_field,sizeof(T)*m_array_size); } }
  explicit DynTuple(const T a)
    : m_array_size(0), mp_field(NULL), m_rows(0)
    { matrix_set_vector(*this,a); }
  explicit DynTuple(const T a,const T b)
    : m_array_size(0), mp_field(NULL), m_rows(0)
    { matrix_set_vector(*this,a,b); }
  explicit DynTuple(const T a,const T b,const T c)
    : m_array_size(0), mp_field(NULL), m_rows(0)
    { matrix_set_vector(*this,a,b,c); }
  explicit DynTuple(const T a,const T b,const T c,const T d)
    : m_array_size(0), mp_field(NULL), m_rows(0)
    { matrix_set_vector(*this,a,b,c,d); }
  explicit DynTuple(const T a,const T b,const T c,const T d,const T e)
    : m_array_size(0), mp_field(NULL), m_rows(0)
    { matrix_set_vector(*this,a,b,c,d,e); }
 ~DynTuple()
    { if (mp_field!=NULL) { delete[] mp_field; mp_field=NULL; } }

  const DynTuple& operator=(const DynTuple& M)
    { alloc(M.m_rows); memcpy(mp_field,M.mp_field,sizeof(T)*M.size()); return *this; }
  bool operator<(const DynTuple& M) const
    { return matrix_lexico_less(*this,M); }
  bool operator==(const DynTuple& M) const
    { return matrix_numeric_equal(*this,M); }

  inline value_type operator()(const int r,const int c=0) const
    { MVRNG(r,m_rows); MVZERO(c); return mp_field[r]; }
  inline value_type operator[](const int r) const
    { MVRNG(r,m_rows); return mp_field[r]; }
  inline value_type& operator()(const int r,const int c=0)
    { MVRNG(r,m_rows); MVZERO(c); return mp_field[r]; }
  inline value_type& operator[](const int r)
    { MVRNG(r,m_rows); return mp_field[r]; }

  inline int rows() const { return m_rows; }
  inline int cols() const { return 1; }
  inline int size() const { return m_rows; }
  inline int capacity() const { return m_array_size; }

  void alloc(int r,int c=1)
    { MVONE(c); if (r>m_array_size) { if (mp_field!=NULL) delete[] mp_field; mp_field=new T[r]; m_array_size=r; }
      m_rows=r; } 
  void adjust(int r,int c=1)
    { MVONE(c); int cols=1; mp_field = ::adjust(mp_field,m_rows,cols,m_array_size,r,1); }
  void write(ostream& os) const
    { matrix_write_dim(os,*this,StandardIO<T>()); }
  void read(istream& is)
    { matrix_read_dim(is,*this,StandardIO<T>()); }

protected:
  int m_array_size; ///< Groesse des allokierten Feldes
  T *mp_field; ///< Zeiger auf dynamisch allokiertes Feld
  int m_rows; ///< Groesse des Vektors
};

template <class T>
inline istream& operator>>(istream &is,DynTuple<T>& a_var) 
  { a_var.read(is); return is; }

template <class T>
inline ostream& operator<<(ostream &os,const DynTuple<T>& a_var) 
  { a_var.write(os); return os; }

/** Tuple mit statischer Groesse, nicht in der Groesse
    veraenderbar. Die Groesse wird ueber die Template-Parameter
    festgelegt. */

template <short ROWS, class T>
class StatTuple
{
public:
  typedef T value_type;

  StatTuple() 
    {}
  StatTuple(const StatTuple& M)
    { memcpy(m_field,M.m_field,sizeof(T)*ROWS); }
  explicit StatTuple(const T a)
    { matrix_set_vector(*this,a); }
  explicit StatTuple(const T a,const T b)
    { matrix_set_vector(*this,a,b); }
  explicit StatTuple(const T a,const T b,const T c)
    { matrix_set_vector(*this,a,b,c); }
  explicit StatTuple(const T a,const T b,const T c,const T d)
    { matrix_set_vector(*this,a,b,c,d); }
  explicit StatTuple(const T a,const T b,const T c,const T d,const T e)
    { matrix_set_vector(*this,a,b,c,d,e); }

  const StatTuple& operator=(const StatTuple& M)
    { memcpy(m_field,M.m_field,sizeof(T)*ROWS); return *this; }
  bool operator<(const StatTuple& M) const
    { return matrix_lexico_less(*this,M); }
  bool operator==(const StatTuple& M) const
    { return matrix_numeric_equal(*this,M); }

  inline value_type operator()(const int r,const int c=0) const
    { MVRNG(r,ROWS); MVZERO(c); return m_field[r]; }
  inline value_type operator[](const int r) const
    { MVRNG(r,ROWS); return m_field[r]; }
  inline value_type& operator()(const int r,const int c=0)
    { MVRNG(r,ROWS); MVZERO(c); return m_field[r]; }
  inline value_type& operator[](const int r)
    { MVRNG(r,ROWS); return m_field[r]; }

  inline int rows() const { return ROWS; }
  inline int cols() const { return 1; }
  inline int size() const { return ROWS; }
  inline int capacity() const { return ROWS; }

  void alloc(int r,int c=1)
    { INVARIANT((r==ROWS)&&(c==1),"StatTuple not resizable"); }
  void adjust(int r,int c=1)
    { INVARIANT((r==ROWS)&&(c==1),"StatTuple not resizable"); }
  void write(ostream& os) const
    { matrix_write_dim(os,*this,StandardIO<T>()); }
  void read(istream& is)
    { matrix_read_dim(is,*this,StandardIO<T>()); }

protected:
  T m_field[ROWS];
};

template <short ROWS,class T>
inline istream& operator>>(istream &is,StatTuple<ROWS,T>& a_var) 
  { a_var.read(is); return is; }

template <short ROWS,class T>
inline ostream& operator<<(ostream &os,const StatTuple<ROWS,T>& a_var) 
  { a_var.write(os); return os; }

/** Matrix mit dynamischer Groesse. Einmal allokierter Speicher wird
    erst im Destruktor wieder freigegeben. */

template <class T>
class DynMatrix
{
public:
  typedef T value_type;

  DynMatrix() 
    : m_array_size(0), mp_field(NULL), m_rows(0), m_cols(0) {}
  DynMatrix(const DynMatrix& M)
    : m_array_size(M.m_array_size), mp_field(new T[M.size()]), m_rows(M.m_rows)
    , m_cols(M.m_cols) { memcpy(mp_field,M.mp_field,sizeof(T)*M.size()); }
  explicit DynMatrix(const T a)
    : m_array_size(0), mp_field(NULL), m_rows(0), m_cols(0)
    { matrix_set_vector(*this,a); }
  explicit DynMatrix(const T a,const T b)
    : m_array_size(0), mp_field(NULL), m_rows(0), m_cols(0)
    { matrix_set_track(*this,a,b); }
  explicit DynMatrix(const T a,const T b,const T c)
    : m_array_size(0), mp_field(NULL), m_rows(0), m_cols(0)
    { matrix_set_track(*this,a,b,c); }
  explicit DynMatrix(const T a,const T b,const T c,const T d)
    : m_array_size(0), mp_field(NULL), m_rows(0), m_cols(0)
    { matrix_set_track(*this,a,b,c,d); }
  explicit DynMatrix(const T a,const T b,const T c,const T d,const T e)
    : m_array_size(0), mp_field(NULL), m_rows(0), m_cols(0)
    { matrix_set_track(*this,a,b,c,d,e); }
  ~DynMatrix()
    { if (mp_field!=NULL) { delete[] mp_field; mp_field=NULL; } }

  const DynMatrix& operator=(const DynMatrix& M)
    { alloc(M.m_rows,M.m_cols); memcpy(mp_field,M.mp_field,sizeof(T)*M.size()); return *this; }
  bool operator<(const DynMatrix& M) const
    { return matrix_lexico_less(*this,M); }
  bool operator==(const DynMatrix& M) const
    { return matrix_numeric_equal(*this,M); }

  inline value_type operator()(const int r,const int c=0) const
    { MVRNG(r,m_rows); MVRNG(c,m_cols); return mp_field[r*m_cols+c]; }
  inline value_type operator[](const int r) const
    { MVRNG(r,m_rows); return mp_field[r]; }
  inline value_type& operator()(const int r,const int c=0)
    { MVRNG(r,m_rows); MVRNG(c,m_cols); return mp_field[r*m_cols+c]; }
  inline value_type& operator[](const int r)
    { MVRNG(r,m_rows); return mp_field[r]; }

  inline int rows() const { return m_rows; }
  inline int cols() const { return m_cols; }
  inline int size() const { return m_rows*m_cols; }
  inline int capacity() const { return m_array_size; }

  void alloc(int r,int c=1)
    { if (r*c>m_array_size) { if (mp_field!=NULL) delete[] mp_field; mp_field=new T[r*c]; m_array_size=r*c; }
      m_rows=r; m_cols=c; } 
  void adjust(int r,int c=1)
    { mp_field = ::adjust(mp_field,m_rows,m_cols,m_array_size,r,c); }
  void write(ostream& os) const
    { matrix_write_dim(os,*this,StandardIO<T>()); }
  void read(istream& is)
    { matrix_read_dim(is,*this,StandardIO<T>()); }

protected:
  int m_array_size; ///< Groesse des allokierten Feldes
  T *mp_field; ///< Zeiger auf das allokierte Feld
  int m_rows; ///< Anzahl der Zeilen der Matrix
  int m_cols; ///< Anzahl der Spalten der Matrix
};

template <class T>
inline istream& operator>>(istream &is,DynMatrix<T>& a_var) 
  { a_var.read(is); return is; }

template <class T>
inline ostream& operator<<(ostream &os,const DynMatrix<T>& a_var) 
  { a_var.write(os); return os; }

template <class T>
void matrix_invert(DynMatrix<T>& A,const DynMatrix<T>& B)
  { matrix_set(A,B); gauss_jordan(A); }


/** Matrix mit statischer Groesse, nicht in der Groesse
    veraenderbar. Die Groesse wird ueber die Template-Parameter
    festgelegt. */

template <short ROWS,short COLS,class T>
class StatMatrix
{
public:
  typedef T value_type;

  StatMatrix() 
    {}
  StatMatrix(const StatMatrix& M)
    { memcpy(m_field,M.m_field,sizeof(T)*size()); }
  explicit StatMatrix(const T a)
    { matrix_set_vector(*this,a); }
  explicit StatMatrix(const T a,const T b)
    { matrix_set_track(*this,a,b); }
  explicit StatMatrix(const T a,const T b,const T c)
    { matrix_set_track(*this,a,b,c); }
  explicit StatMatrix(const T a,const T b,const T c,const T d)
    { matrix_set_track(*this,a,b,c,d); }
  explicit StatMatrix(const T a,const T b,const T c,const T d,const T e)
    { matrix_set_track(*this,a,b,c,d,e); }

  const StatMatrix& operator=(const StatMatrix& M)
    { memcpy(m_field,M.m_field,sizeof(T)*size()); return *this; }
  bool operator<(const StatMatrix& M) const
    { return matrix_lexico_less(*this,M); }
  bool operator==(const StatMatrix& M) const
    { return matrix_numeric_equal(*this,M); }

  inline value_type operator()(const int r,const int c=0) const
    { MVRNG(r,ROWS); MVRNG(c,COLS); return m_field[r][c]; }
  inline value_type operator[](const int r) const
    { MVRNG(r,ROWS); return ((T*)m_field)[r]; }
  inline value_type& operator()(const int r,const int c=0)
    { MVRNG(r,ROWS); MVRNG(c,COLS); return m_field[r][c]; }
  inline value_type& operator[](const int r)
    { MVRNG(r,ROWS); return ((T*)m_field)[r]; }

  inline int rows() const { return ROWS; }
  inline int cols() const { return COLS; }
  inline int size() const { return ROWS*COLS; }
  inline int capacity() const { return ROWS*COLS; }

  void alloc(int r,int c=1)
    { INVARIANT((r==ROWS)&&(c==COLS),r<<"vs"<<ROWS<<','<<c<<"vs"<<COLS); }
  void adjust(int r,int c=1)
    { INVARIANT((r==ROWS)&&(c==COLS),r<<"vs"<<ROWS<<','<<c<<"vs"<<COLS); }
  void write(ostream& os) const
    { matrix_write_dim(os,*this,StandardIO<T>()); }
  void read(istream& is)
    { matrix_read_dim(is,*this,StandardIO<T>()); }

protected:
  T m_field[ROWS][COLS]; ///< Matrix
};

template <short ROWS,short COLS,class T>
inline istream& operator>>(istream &is,StatMatrix<ROWS,COLS,T>& a_var) 
  { a_var.read(is); return is; }

template <short ROWS,short COLS,class T>
inline ostream& operator<<(ostream &os,const StatMatrix<ROWS,COLS,T>& a_var) 
  { a_var.write(os); return os; }

template <short ROWS,short COLS,class T>
void matrix_invert(StatMatrix<ROWS,COLS,T>& A,const StatMatrix<ROWS,COLS,T>& B)
  { matrix_set(A,B); gauss_jordan(A); }

/** Transponierte Matrix. Der Template-Parameter ist selbst ein
    Matrix-Typ. Die transponierte Matrix TransMatrix<M> ist keine
    wirklich selbstaendige Matrix, es wird direkt auf eine
    "Originalmatrix" zugegriffen, auf die sich aller
    Schreiboperationen dann (transponiert) beziehen. */

template <class M>
class TransMatrix
{
public:
  typedef typename M::value_type T;
  typedef typename M::value_type value_type;

  TransMatrix()
    : mp_matrix(NULL) {}
  TransMatrix(M& m)
    : mp_matrix(&m) {}
  explicit TransMatrix(const T a)
    { matrix_set_vector(*this,a); }
  explicit TransMatrix(const T a,const T b)
    { matrix_set_track(*this,a,b); }
  explicit TransMatrix(const T a,const T b,const T c)
    { matrix_set_track(*this,a,b,c); }
  explicit TransMatrix(const T a,const T b,const T c,const T d)
    { matrix_set_track(*this,a,b,c,d); }
  explicit TransMatrix(const T a,const T b,const T c,const T d,const T e)
    { matrix_set_track(*this,a,b,c,d,e); }

  const TransMatrix& operator=(const TransMatrix& m);
    // nicht definiert, Kopie macht keinen Sinn
  bool operator<(const TransMatrix& m) const
    { return matrix_lexico_less(*this,m); }
  bool operator==(const TransMatrix& m) const
    { return matrix_numeric_equal(*this,m); }

  inline value_type operator()(const int r,const int c=0) const
    { return mp_matrix->operator()(c,r); }
  inline value_type operator[](const int r) const
    { return mp_matrix->operator()(0,r); }
  inline value_type& operator()(const int r,const int c=0)
    { return mp_matrix->operator()(c,r); }
  inline value_type& operator[](const int r)
    { return mp_matrix->operator()(0,r); }

  inline int rows() const { return mp_matrix->cols(); }
  inline int cols() const { return mp_matrix->rows(); }
  inline int size() const { return mp_matrix->size(); }
  inline int capacity() const { return mp_matrix->capacity(); }

  void alloc(int r,int c=1)
    { mp_matrix->alloc(c,r); }
  void adjust(int r,int c=1)
    { mp_matrix->adjust(r,c); }
  void write(ostream& os) const
    { matrix_write_dim(os,*this,StandardIO<T>()); }
  void read(istream& is)
    { matrix_read_dim(is,*this,StandardIO<T>()); }

protected:
  M *mp_matrix;
};

template <class M>
inline istream& operator>>(istream &is,TransMatrix<M>& m) 
  { m.read(is); return is; }

template <class M>
inline ostream& operator<<(ostream &os,const TransMatrix<M>& m) 
  { m.write(os); return os; }

template <class M>
TransMatrix<M> transposed(M& m)
  { return TransMatrix<M>(m); }

template <class M>
const TransMatrix<const M> transposed(const M& m)
  { return TransMatrix<const M>(m); }

/** SelectTuple. Kein Schreibzugriff. */

template <class M,class S>
class SelectTuple  
{
public:
  typedef typename M::value_type T;
  typedef typename M::value_type value_type;

  SelectTuple()
    : mp_matrix(NULL), mp_select(NULL) {}
  SelectTuple(const M& m,const S& s)
    : mp_matrix(&m), mp_select(&s) {}
  ~SelectTuple()
    {}

  const SelectTuple& operator=(const SelectTuple& m);
    // nicht definiert, Kopie macht keinen Sinn
  bool operator<(const SelectTuple& m) const
    { return matrix_lexico_less(*this,m); }
  bool operator==(const SelectTuple& m) const
    { return matrix_numeric_equal(*this,m); }

  inline value_type operator()(const int r,const int c=0) const
    { MVZERO(c); return value(r); } 
  inline value_type operator[](const int r) const
    { return value(r); }

  inline int rows() const 
    { return ((mp_select->rows()==0)?mp_matrix->rows():mp_select->rows()); }
  inline int cols() const { return 1; }
  inline int size() const { return rows(); }
  inline int capacity() const { return rows(); }

  void alloc(int r,int c=1) const
    { MVRNG(r,rows()); MVONE(c); }
  void adjust(int r,int c=1) const
    { MVRNG(r,rows()); MVONE(c); }
  void write(ostream& os) const
    { matrix_write_dim(os,*this,StandardIO<T>()); }
    
protected:
  value_type value(int row) const
    { 
    INVARIANT((row<rows()),"matrix access out of range");
    if (mp_select->rows()==0) return mp_matrix->operator()(row,0);
    typename M::value_type value(1);
    int index,c=0; bool cont;
    do
      {
      index = mp_select->operator()(row,c);
      cont = ((index>=0) && (index<mp_matrix->rows()));
      if (cont) 
        { 
        value *= mp_matrix->operator()(index);
        ++c;
        cont   = c < mp_select->cols();
        }
      }
    while (cont);
    return value;
    }

private:    
  const M *mp_matrix;
  const S *mp_select;
};

template <class M,class S>
inline ostream& operator<<(ostream &os,const SelectTuple<M,S>& m) 
  { m.write(os); return os; }

template <class M,class S>
const SelectTuple<M,S> selected(const M& m, const S& s)
  { return SelectTuple<M,S>(m,s); }


/** Streichungsmatrix. Der Template-Parameter ist selbst ein
    Matrix-Typ. Aus der Ursprungsmatrix wird eine Zeile und Spalte
    gestrichen.  */

template <class M>
class CutMatrix
{
public:
  typedef typename M::value_type T;
  typedef typename M::value_type value_type;

  CutMatrix()
    : mp_matrix(NULL) {}
  CutMatrix(M& m,int r,int c)
    : mp_matrix(&m), m_cutrow(r), m_cutcol(c=r) {}

  const CutMatrix& operator=(const CutMatrix& m);
    // nicht definiert, Kopie macht keinen Sinn
  bool operator<(const CutMatrix& m) const
    { return matrix_lexico_less(*this,m); }
  bool operator==(const CutMatrix& m) const
    { return matrix_numeric_equal(*this,m); }

  inline value_type operator()(const int r,const int c=0) const
    { return mp_matrix->operator()(r+((r>=m_cutrow)?1:0),c+((c>=m_cutcol)?1:0)); }
  inline value_type operator[](const int r) const
    { return mp_matrix->operator()(r+((r>=m_cutrow)?1:0)); }
  inline value_type& operator()(const int r,const int c=0)
    { return mp_matrix->operator()(r+((r>=m_cutrow)?1:0),c+((c>=m_cutcol)?1:0)); }
  inline value_type& operator[](const int r)
    { return mp_matrix->operator()(r+((r>=m_cutrow)?1:0)); }

  inline int rows() const { return mp_matrix->cols()-1; }
  inline int cols() const { return mp_matrix->rows()-1; }
  inline int size() const { return rows()*cols(); }
  inline int capacity() const { return mp_matrix->capacity(); }

  void alloc(int r,int c=1)
    { INVARIANT((r==rows())&&(c==cols()),"CutMatrix not resizable"); }
  void adjust(int r,int c=1)
    { INVARIANT((r==rows())&&(c==cols()),"CutMatrix not resizable"); }
  void write(ostream& os) const
    { matrix_write_dim(os,*this,StandardIO<T>()); }
  void read(istream& is)
    { matrix_read_dim(is,*this,StandardIO<T>()); }

protected:
  M *mp_matrix;
  int m_cutrow;
  int m_cutcol;
};

template <class M>
inline istream& operator>>(istream &is,CutMatrix<M>& m) 
  { m.read(is); return is; }

template <class M>
inline ostream& operator<<(ostream &os,const CutMatrix<M>& m) 
  { m.write(os); return os; }

template <class M>
CutMatrix<M> cutmatrix(M& m,int r,int c)
  { c = r; return CutMatrix<M>(m,r,c); }

/** Streichungsmatrix. Der Template-Parameter ist selbst ein
    Matrix-Typ. Aus der Ursprungsmatrix wird eine Zeile und Spalte
    gestrichen.  */

template <class M>
class SubMatrix
{
public:
  typedef typename M::value_type T;
  typedef typename M::value_type value_type;

  SubMatrix()
    : mp_matrix(NULL) {}
  SubMatrix(M& m,int minr,int maxr,int minc=0,int maxc=0)
    : mp_matrix(&m), m_minrow(minr), m_maxrow(maxr), m_mincol(minc), m_maxcol(maxc) 
    { INVARIANT((mp_matrix->rows()>maxr)&&(mp_matrix->cols()>maxc),"proper submatrix"); }

  const SubMatrix& operator=(const SubMatrix& m);
    // nicht definiert, Kopie macht keinen Sinn
  bool operator<(const SubMatrix& m) const
    { return matrix_lexico_less(*this,m); }
  bool operator==(const SubMatrix& m) const
    { return matrix_numeric_equal(*this,m); }

  inline value_type operator()(const int r,const int c=0) const
    { MVRNG(r,m_maxrow-m_minrow+1); MVRNG(c,m_maxcol-m_mincol+1);
      return mp_matrix->operator()(r+m_minrow,c+m_mincol); }
  inline value_type operator[](const int r) const
    { MVRNG(r,m_maxrow-m_minrow+1); return mp_matrix->operator()(r+m_minrow); }
  inline value_type& operator()(const int r,const int c=0)
    { MVRNG(r,m_maxrow-m_minrow+1); MVRNG(c,m_maxcol-m_mincol+1);
      return mp_matrix->operator()(r+m_minrow,c+m_mincol); }
  inline value_type& operator[](const int r)
    { MVRNG(r,m_maxrow-m_minrow+1); return mp_matrix->operator()(r+m_minrow); }

  inline int rows() const { return m_maxrow-m_minrow+1; }
  inline int cols() const { return m_maxcol-m_mincol+1; }
  inline int size() const { return rows()*cols(); }
  inline int capacity() const { return mp_matrix->capacity(); }

  void alloc(int r,int c=1)
    { INVARIANT((r==rows())&&(c==cols()),"SubMatrix not resizable"); }
  void adjust(int r,int c=1)
    { INVARIANT((r==rows())&&(c==cols()),"SubMatrix not resizable"); }
  void write(ostream& os) const
    { matrix_write_dim(os,*this,StandardIO<T>()); }
  void read(istream& is)
    { matrix_read_dim(is,*this,StandardIO<T>()); }

protected:
  M *mp_matrix;
  int m_minrow,m_maxrow,m_mincol,m_maxcol;
};

template <class M>
inline istream& operator>>(istream &is,SubMatrix<M>& m) 
  { m.read(is); return is; }

template <class M>
inline ostream& operator<<(ostream &os,const SubMatrix<M>& m) 
  { m.write(os); return os; }

template <class M>
SubMatrix<M> submatrix(M& m,int minr,int maxr,int minc=0,int maxc=0)
  { return SubMatrix<M>(m,minr,maxr,minc,maxc); }

template <class M>
const SubMatrix<M> submatrix(const M& m,int minr,int maxr,int minc=0,int maxc=0)
  { return SubMatrix<M>(const_cast<M&>(m),minr,maxr,minc,maxc); }


#endif // MATVEC_HEADER
