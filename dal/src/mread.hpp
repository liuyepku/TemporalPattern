
/** \file mread.hpp
    \author Frank Hoeppner <frank.hoeppner@ieee.org>
    \brief Routinen zur Ein/Ausgabe von Vektoren und Matrizen
*/

#ifndef MREAD_HEADER
#define MREAD_HEADER

#include "defbool.hpp"
#include <iostream>

typedef struct matrix_separators
{
  bool m_eval_hdr; ///< Matrix mit Dimensionsangaben?
  bool m_row_lf; ///< Zeilenumbruch nach jeder Zeile?
  bool m_flat; ///< true, wenn Felder "einzeln" ausgegeben werden (ohne Zusammenhalt)
  char m_field_sep; ///< Zeichen zwischen benachbarten Feldern (bei Listen von Matrizen)
  char m_dim_begin; ///< Startzeichen der Dimensionsangaben
  char m_dim_end; ///< Endezeichen der Dimensionangaben
  char m_cross; ///< Zeichen zwischen Zeilen und Spaltenanzahl
  char m_colon; ///< Zeichen zwischen Dimension und Typbezeichner
  char m_seq_begin; ///< Startzeichen eines Vektors oder einer Zeile in einer Matrix
  char m_val_sep; ///< Trennzeichen zwischen Werten in einem Vektor/Matrix
  char m_seq_end; ///< Endezeichen eines Vektors oder einer Zeile in einer Matrix
  char m_mod_sparse; ///< Typbezeichner fuer duenn besetzte Matrizen
  char m_mod_symm; ///< Typbezeichner fuer symmetrische Matrizen
  char m_mod_unit; ///< Typbezeichner fuer Einheitsmatrizen
  char m_mod_full; ///< Typbezeichner fuer voll besetzte Matrizen
  char m_mod_diag; ///< Typbezeichner fuer Diagonalmatrizen
};

extern matrix_separators g_matrix_console_separators;
extern matrix_separators g_matrix_table_separators;
extern matrix_separators g_matrix_colon_separators;
extern matrix_separators g_matrix_readable_separators;

template <class M> 
char matrix_get_type(const M& A,matrix_separators const * p = NULL);

template <class M,class IO>
void matrix_read(istream& is,M& A,const IO& io,matrix_separators const * p = NULL);

template <class M,class IO>
void matrix_read_dim(istream& is,M& matrix,const IO& io,matrix_separators const * p = NULL);

template <class M,class IO> 
void matrix_write_dim(ostream& os,const M& matrix,const IO& io,matrix_separators const * p = NULL,char modifier = '\0');

#endif /* MREAD_HEADER */
