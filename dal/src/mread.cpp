
/** \file mread.cpp
    \author Frank Hoeppner <frank.hoeppner@ieee.org>
    \brief Routinen zur Ein/Ausgabe von Vektoren und Matrizen

    Matrixschreibweise zeilenweise [r x c]((row 1)(row 2)(...)(row n))
*/

#ifndef MREAD_SOURCE
#define MREAD_SOURCE

#include "mread.hpp"
#include "logtrace.h"
#include "matvecop.hpp"
#include <ctype.h> // isspace

/* matrix_read_dim => hier wird durch Angabe der Dimensionen die
Reihenfolge und Lage der Daten in der Matrix eindeutig, bevor wir die
Daten gelesen habe. Das beschleunigt die Speicherallokation und
erlaubt auch das benutzerseitige Einstellen der separatoren. 

 Angabe der Dimensionen [rows x cols] ((zeile1)(zeile2)(zeile3))
 Modifikator [rows {x cols} {: M}]
   mit L fuer sparse ((row col val)(row col val)(row col val))
   mit D fuer diagonal (diagonal)
   mit S fuer symmetric
   mit U fuer unit matrix
   mit M fuer normale Matrix (default)

mglw noch T fuer transponierten Vektor (einzelne zeile)
*/

# define MV_SKIP(c) if ((!is_followed_by(is,c,false)) && (!isspace(c))) ERROR("missing "<<#c<<" "<<c);

/** Struktur zur Spezifizierung der Ein/Ausgabesymbole.

    Bedingungen: m_seq_begin != m_seq_end, sonst koennen wir am Ende
    einer Zeile nicht erkennen, ob die Matrix zu Ende ist, oder noch
    eine Zeile kommt. Auch keine whitespace Zeichen, da Felder mit
    whitespaces ueberlesen werden und daher nicht zur Unterscheidung
    herangezogen werden koennen. */

matrix_separators g_matrix_console_separators =
  { m_eval_hdr:true
  , m_row_lf:false
  , m_flat:false
  , m_field_sep:'\0'
  , m_dim_begin:'['
  , m_dim_end:']'
  , m_cross:'x'
  , m_colon:':'
  , m_seq_begin:'('
  , m_val_sep:' '
  , m_seq_end:')'
  , m_mod_sparse:'L'
  , m_mod_symm:'S'
  , m_mod_unit:'U'
  , m_mod_full:'M'
  , m_mod_diag:'D'
  };

matrix_separators g_matrix_table_separators =
  { m_eval_hdr:false
  , m_row_lf:false
  , m_flat:true
  , m_field_sep:' '
  , m_dim_begin:' '
  , m_dim_end:' '
  , m_cross:' '
  , m_colon:' '
  , m_seq_begin:' '
  , m_val_sep:' '
  , m_seq_end:' '
  , m_mod_sparse:'L'
  , m_mod_symm:'S'
  , m_mod_unit:'U'
  , m_mod_full:'M'
  , m_mod_diag:'D'
  };

matrix_separators g_matrix_colon_separators =
  { m_eval_hdr:false
  , m_row_lf:false
  , m_flat:false
  , m_field_sep:'\0'
  , m_dim_begin:' '
  , m_dim_end:' '
  , m_cross:' '
  , m_colon:' '
  , m_seq_begin:':'
  , m_val_sep:','
  , m_seq_end:';'
  , m_mod_sparse:'L'
  , m_mod_symm:'S'
  , m_mod_unit:'U'
  , m_mod_full:'M'
  , m_mod_diag:'D'
  };

matrix_separators g_matrix_readable_separators =
  { m_eval_hdr:true
  , m_row_lf:true
  , m_flat:false
  , m_field_sep:'\0'
  , m_dim_begin:'['
  , m_dim_end:']'
  , m_cross:'x'
  , m_colon:':'
  , m_seq_begin:' '
  , m_val_sep:' '
  , m_seq_end:' '
  , m_mod_sparse:'L'
  , m_mod_symm:'S'
  , m_mod_unit:'U'
  , m_mod_full:'M'
  , m_mod_diag:'D'
  };

/** Diese Funktion analysiert eine Matrix auf einen Spezialfall
    (symmetrisch, diagonal, Einheitsmatrix, etc.). Der
    char-Rueckgabewert erfolgt gemaess der
    matrix_separators-Struktur. */

template <class M> 
char matrix_get_type
(
  const M& A,
  matrix_separators const * p = NULL
)
{
  // Wenn keine Separatoren angegeben, dann Voreinstellung nutzen 
  if (p==NULL) p = &g_matrix_console_separators;
  char mod = p->m_mod_full;

  // symmetrisch?
  int r,c,rows=A.rows(),cols=A.cols();
  bool symm=(rows==cols), zero=true, unit=true;
  for (r=0;r<rows;++r)
    for (c=0;c<cols;++c)
    { 
      if ((symm)&&(r >c)) symm &= equal(A(r,c),A(c,r)); 
      if ((zero)&&(r!=c)) zero &= equal(A(r,c),(typename M::value_type)0); 
    }

  if (symm) mod=p->m_mod_symm;
  if (zero) 
    {
      for (int i=0;i<min(A.cols(),A.rows());++i) 
        unit &= equal(A(i,i),(typename M::value_type)1);
      if (unit) mod=p->m_mod_unit; else mod=p->m_mod_diag;
    }

  return mod;
}

/** Funktion zum Einlesen einer Matrix ohne Kenntnis der
    Dimension. Dies funktioniert nicht bei allen
    matrix_separator-Instanzen, die Grenzen zwischen den
    Zeilen/Spalten muessen eindeutig erkennbar sein. Default:
    console-Einstellungen.

    Achtung: um Umspeichern zu vermeiden gehen wir davon aus, dass die
    Matrizen ZEILENWEISE abgespeichert werden, und dass mit dem
    operator[] linear auf den Speicher zugegriffen werden kann. Dann
    koennen wir die Matrix einfach linear einlesen und die Dimension
    ganz am Ende richtig setzen.  */

template <class M,class IO>
void
matrix_read
(
  istream& is, 
  M& A, 
  const IO& io,
  matrix_separators const * p = NULL
)
{

  // Wenn keine Separatoren angegeben, dann Voreinstellung nutzen 
  if (p==NULL) p = &g_matrix_console_separators;

  // Wann soll das Lesen eines (symbolischen) Labels abgebrochen
  // werden? (noetig bei (a b), hier soll bspw. das "b" als Label
  // erkannt werden, nicht das Wort "b)")
  char breakchars[4];
  breakchars[0] = p->m_seq_end;
  breakchars[1] = p->m_field_sep;
  breakchars[2] = p->m_val_sep;
  breakchars[3] = '\0';

  read_white(is);

  if (!is_followed_by(is,p->m_seq_begin,false))
  { // einzelne Zahl als 1x1 Matrix lesen
    A.alloc(1,1);
    io.read(is,A(0,0),breakchars);
  }
  else
  {
    // Wir lesen alle Zahlen in einen Vektor ein (geringere Kosten bei
    // Neudimensionierung) und machen daraus am Ende per alloc() eine
    // Matrix. Zunaechst machen wir aus A einen Vektor:
    A.alloc(A.capacity());

    if (is_followed_by(is,p->m_seq_begin))
    { // Matrix einlesen
      int r = 0, lastcols = 0, i = 0;
      while (is_followed_by(is,p->m_seq_begin,false))
      {
        int c = 0;
        while (!is_followed_by(is,p->m_seq_end))
        {
          if (c!=0) MV_SKIP(p->m_val_sep);
          // anfangs um einen Wert, spaeter um eine Zeile vergroessern
          if (!(i<A.capacity())) A.adjust(i+((lastcols==0)?1:lastcols));
          io.read(is,A[i],breakchars);
          ++c; ++i;
        }
        if (lastcols==0) lastcols=c;
        if (c!=lastcols) ERROR("matrix with varying row size");
        ++r;
        if (!is_followed_by(is,p->m_seq_end,false)) ERROR(p->m_seq_end);
      }
      // Und jetzt aus dem Vektor per alloc() eine Matrix machen
      A.alloc(r,lastcols);
    }
    else
    { // Vector einlesen (Spaltenvektor)
      int r(0);
      while (!is_followed_by(is,p->m_seq_end))
      {
        if (r!=0) MV_SKIP(p->m_val_sep);
        // hier erhoehen wir immer um 2 Felder
        if (!(r<A.capacity())) A.adjust(r+2);
        io.read(is,A[r],breakchars);
        ++r;
      }
      A.alloc(r,1);
    }
    MV_SKIP(p->m_seq_end);
  }
}

/** Erweiterte Funktionalitaet zum Auslesen einer Matrix aus einem
    Stream. Diese Funktion behandelt Spezialfaelle wie symmetrische
    Matrizen, Diagonalmatrizen, etc. Neben den Werten selbst wird
    dabei auch die Dimension der Matrix erwartet, was das Auslesen
    (und dynamische Allokieren) beschleunigt. Falls keine Dimensionen
    angegeben sind, wird matrix_read aufgerufen (siehe Kommentare
    dort). */

template <class M,class IO>
void matrix_read_dim
(  
  istream& is,
  M& matrix, 
  const IO& io, 
  matrix_separators const * p = NULL
)
{
  read_white(is);

  // Wenn keine Separatoren angegeben, dann Voreinstellung nutzen 
  if (p==NULL) p = &g_matrix_console_separators;
  char mod = p->m_mod_full;

  // Vorspann bearbeiten ? (wenn kein Vorspann, dann rufen wir die
  // Routine ohne Dimensionbetrachtungen auf)
  if ((!p->m_eval_hdr) || (!is_followed_by(is,p->m_dim_begin))) 
    { matrix_read(is,matrix,io,p); return; }

  // Wann soll das Lesen eines (symbolischen) Labels abgebrochen
  // werden? (noetig bei (a b), hier soll bspw. das "b" als Label
  // erkannt werden, nicht das Wort "b)")
  char breakchars[4];
  breakchars[0] = p->m_seq_end;
  breakchars[1] = p->m_field_sep;
  breakchars[2] = p->m_val_sep;
  breakchars[3] = '\0';

  // Dimensionen einlesen
  int rows,cols; 
  MV_SKIP(p->m_dim_begin);
  is >> rows;
  if (!is_followed_by(is,p->m_cross,false)) cols=1; else is >> cols;
  // Modifikator einlesen
  if (is_followed_by(is,p->m_colon,false)) is >> mod;
  MV_SKIP(p->m_dim_end);
  // Matrix initialisieren
  matrix.adjust(rows,cols); 
  matrix_set_scalar(matrix,0);

  // Modifikator auswerten
  if (mod==p->m_mod_full)
  { // Volle Matrix lesen
    MV_SKIP(p->m_seq_begin);
    for (int r=0;r<rows;++r)
    {
      if (cols==1)
      {
        if (r!=0) MV_SKIP(p->m_val_sep); 
        io.read(is,matrix(r),breakchars);
      }
      else
      {
        MV_SKIP(p->m_seq_begin);
        for (int c=0;c<cols;++c) 
          { if (c!=0) MV_SKIP(p->m_val_sep); io.read(is,matrix(r,c),breakchars); }
        MV_SKIP(p->m_seq_end);
      }
    }
    MV_SKIP(p->m_seq_end);
  }

  else if (mod==p->m_mod_symm)
  { // symmetrische Matrix auslesen
    if (rows!=cols) ERROR("symmetric mode only in square matrices");
    MV_SKIP(p->m_seq_begin);
    typename M::value_type v;
    for (int r=0;r<rows;++r)
    {
      if (cols>1) MV_SKIP(p->m_seq_begin);
      for (int c=r;c<cols;++c) 
      { 
        if (c!=r) MV_SKIP(p->m_val_sep); 
        io.read(is,v,breakchars); matrix(r,c)=matrix(c,r)=v; 
      }
      if (cols>1) MV_SKIP(p->m_seq_end);
    }
    MV_SKIP(p->m_seq_end);
  }

  else if (mod==p->m_mod_diag)
  { // Diagonalmatrix als Vektor auslesen
    MV_SKIP(p->m_seq_begin);
    for (int i=0;i<min(cols,rows);++i) 
      { io.read(is,matrix(i,i),breakchars); MV_SKIP(p->m_val_sep); }
    MV_SKIP(p->m_seq_end);
  }

  else if (mod==p->m_mod_unit)
  { // Einheitsmatrix braucht nicht gelesen zu werden
    for (int i=0;i<min(cols,rows);++i) matrix(i,i)=1;
  }

  else if (mod==p->m_mod_sparse)
  { // Duenn besetzte Matrix auslesen
    MV_SKIP(p->m_seq_begin);
    int r,c; typename M::value_type v;
    while (is_followed_by(is,p->m_seq_begin,false))
    { 
      is >> r; MV_SKIP(p->m_val_sep);
      is >> c; MV_SKIP(p->m_val_sep);
      is >> v; MV_SKIP(p->m_seq_end);
      if (r>rows) ERROR("row exceeds dimensions in sparse matrix");
      if (c>cols) ERROR("col exceeds dimensions in sparse matrix");
      matrix(r-1,c-1)=v;
    }
    MV_SKIP(p->m_seq_end);
  }

  else ERROR("unknown matrix modifier "<<mod);
}

template <class M,class IO> 
void matrix_write_dim
  (  
  ostream& os,
  const M& matrix, 
  const IO& io, 
  matrix_separators const * p = NULL,
  char modifier = '\0'
  )
  {
  // Wenn keine Separatoren angegeben, dann Voreinstellung nutzen 
  if (p==NULL) p = &g_matrix_console_separators;

  // Nur eine Zahl?
  if (matrix.size()==1) { io.write(os,matrix(0,0)); return; }

  // Vorspann-Ausgabe?
  if (p->m_eval_hdr)
    {
    // Dimensionen ausgeben
    os << p->m_dim_begin << matrix.rows();
    if (matrix.cols()>1) os << p->m_cross << matrix.cols();

    // ACHTUNG: Eigenschaft NICHT bei SYMBOLISCHER Ausgabe
    // vornehmen. Was ist eine Einheitsmatrix bei symbolischen Labeln?
    // Haengt von der Verteilung der Label/Indices ab, kann daher von
    // Fall zu Fall verschieden sein.
    if (io.symbolic()) modifier = p->m_mod_full;

    // Ermittle Eigenschaft?
    if (modifier=='\0')
      modifier= (!io.symbolic())?p->m_mod_full:matrix_get_type(matrix);

    if (modifier!=p->m_mod_full) os << p->m_colon << modifier;
    os << p->m_dim_end;
    }
  // Wenn keine Ausgabe des Vorspanns, dann nur volle Ausgabe
  else modifier = p->m_mod_full;

  if (modifier==p->m_mod_symm)
    { // symmetrische Matrix ausgeben
    os << p->m_seq_begin;
    for (int r=0;r<matrix.rows();++r)
      {
      if (matrix.cols()>1) os << p->m_seq_begin;
      for (int c=r;c<matrix.cols();++c) 
        { if (c!=r) os << p->m_val_sep; io.write(os,matrix(r,c)); }
      if (matrix.cols()>1) os << p->m_seq_end;
      if (p->m_row_lf) os << '\n';
      }
    os << p->m_seq_end;
    }

  else if (modifier==p->m_mod_unit)
    { // bei Einheitsmatrix keine weitere Ausgabe
    }

  else if (modifier==p->m_mod_diag)
    { // Diagonalmatrix ausgeben
    os << p->m_seq_begin;
    for (int i=0;i<min(matrix.rows(),matrix.cols());++i)
      { if (i!=0) os << p->m_val_sep; io.write(os,matrix(i,i)); }
    os << p->m_seq_end;
    }

  else if (modifier==p->m_mod_full)
    { // volle Matrix ausgeben
    os << p->m_seq_begin;
    if (p->m_row_lf) os << '\n';
    for (int r=0;r<matrix.rows();++r)
      {
      if (matrix.cols()==1) 
        { // Vektorausgabe 
        if (r!=0) os << p->m_val_sep; 
        io.write(os,matrix(r)); 
        }
      else
        { // Matrixausgabe
        os << p->m_seq_begin;
        for (int c=0;c<matrix.cols();++c) 
          { if (c!=0) os << p->m_val_sep; io.write(os,matrix(r,c)); }
        os << p->m_seq_end;
        if (p->m_row_lf) os << '\n';
        }
      }
    os << p->m_seq_end;
    }

  else ERROR("unknown matrix type "<<modifier);
  }

#ifdef MATHTEST
/** Testprogramm fuer Matrix-IO 

    Es werden Zufallsmatrizen erzeugt, in einen strstream geschrieben,
    wieder ausgelesen und mit dem Original verglichen. */

bool matrix_read_test()
{
  DynMatrix<float> X,Y;
  bool ok = true;

  // Test, ob die Matrixklasse zeilenweise speichert (wie C/C++ Arrays)
  X.alloc(7,8);
  float test[7][8];
  float *test2 = (float*)test;
  for (int i=0;i<7;++i) for (int j=0;j<8;++j) test[i][j]=X(i,j)=rand()%1000;
  for (int i=0;i<7*8;++i) ok &= equal(test2[i],X[i]);
  if (!ok) cout << "matrices have do be stored by row!" << endl;

  for (int run=0;run<5000;++run)
  {
    // Zufallsmatrix erstellen
    int r = rand()%10+1; 
    int c = rand()%10+1;
    X.alloc(r,c); matrix_set_scalar(X,0);
    switch (rand()%10)
    { case 0 : // diagonal 
        for (int i=0;i<min(r,c);++i) X(i,i)=rand()%1000; 
        break;
      case 1 : // Einheitsmatrix 
        for (int i=0;i<min(r,c);++i) X(i,i)=1; 
        break;
      case 2 : // symmetrisch
        r=c; X.alloc(r,c);
        for (int i=0;i<r;++i) for (int j=i;j<c;++j) X(i,j)=X(j,i)=rand()%1000;
        break;
      default: // volle Matrix
        for (int i=0;i<r;++i) for (int j=0;j<c;++j) X(i,j)=rand()%1000; 
        break;
    }
#   ifdef SHOWTEST
    cout << "X:" << X << endl;
#   endif

    matrix_separators * p=NULL;
    switch (rand()%4)
    { case 0 : 
        p = &g_matrix_readable_separators; 
        break;
      //case 1 : p = &g_matrix_table_separators; break;
      case 2 : 
        p = &g_matrix_colon_separators; 
        break;
      default: 
        p = &g_matrix_console_separators; 
        p->m_eval_hdr = (rand()%2==0);
        break;
    }

    // ausgeben
    strstream s;
    matrix_write_dim(s,X,StandardIO<float>(),p); s << '\0';
#   ifdef SHOWTEST
    cout << "s:" << s.str() << endl;
#   endif
    // wieder einlesen
    matrix_read_dim(s,Y,StandardIO<float>(),p);
#   ifdef SHOWTEST
    cout << "Y:" << Y << endl << endl;
#   endif
    // identisch?
    if (!matrix_numeric_equal(X,Y))
    {
      cout << "MISMATCH in run " << run << endl
	   << "X:" << X << endl
           << "Y:" << Y << endl
           << "s:" << s.str() << endl << endl;
      ok = false;
    }
  }

  return ok;
}
#endif // MATHTEST

#endif // MREAD_SOURCE
