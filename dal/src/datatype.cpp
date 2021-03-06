
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

/** \file datatype.cpp 
    \author Frank Hoeppner <frank.hoeppner@ieee.org>

    Implementation of (template-) classes that store integers, reals,
    vectors, matrices, etc. from the data files. File will be included
    by instances.cpp, where the template instances will be
    instantiated explicitly. */

#ifndef DATATYPE_SOURCE
#define DATATYPE_SOURCE

#include "dal.hpp"
#include "access.hpp" // g_matrix_ddl_separators
#include <strstream>

namespace Data_Access_Library {

////////////////////////////////////////////////////////////////////
// abstract_data_type

/** Constructor. Every data object needs a context. */

abstract_data_type::abstract_data_type(
  Context *ap_context
  )
  : mp_context(ap_context)
{
  INVARIANT(mp_context!=NULL,"");
}

/** Reading the data object from a stream. Since this class is
    abstract, an error will be reported if this method is called. */

void
abstract_data_type::read
  (
  istream& is,
  const matrix_separators*
  )
  {
  DO_NOT_PASS;
  }

/** Reading the data object. Since this class is abstract, an error
    will be reported if this method is called. In contrast to the
    other read() method, this one will be called when vectors are read
    from a .tab file. In this case we have to ``simulate'' an vector
    input by reading \c r consecutive data objects, since .tab files
    do not support vector data. */

void
abstract_data_type::read
  (
  istream& is,
  int r,
  const matrix_separators* p_sep
  )
  {
  DO_NOT_PASS;
  }

/** Writing the data object. Since this class is abstract, an error
    will be reported if this method is called. */

void
abstract_data_type::write
(
  ostream& os,
  const matrix_separators* 
)
const
{
  DO_NOT_PASS;
}

////////////////////////////////////////////////////////////////////
// data_type<T>

/** Constructor. Every data object needs a context. */

template <class T>
data_type<T>::data_type
  (
  Context* ap_context
  ) 
  : abstract_data_type(ap_context)
  , mp_value(NULL)
  {
  // Wenn noch kein g_global_default existiert (erste Instanz), dann erstellen
  if (m_global_default_ix==g_illegal_ix)
  {
    m_global_default_ix = m_repository.insert(T()); 
  }
  // Default setzen
  load_default();
  }

/** Check for default value. The method returns true, if the data
    object has the default value. The default value is either defined
    by the global default value for the data_type, or specified
    individually for the given context. */

template <class T>
bool 
data_type<T>::is_default() 
    const
{ 
    ix_type contextdefault = mp_context->get_default_ix();
    if (contextdefault==g_illegal_ix)
    { // kein Context-spezifischer Default-Wert, also Test auf
	// globalen Default-Wert
	return (m_ix==m_global_default_ix);
    }
    else
    { 
	// wenn es context-default gibt, dann ist globaler Default
	// ungueltig. Andernfalls kann das zu schwerwiegenden Fehler
	// fuehren, wenn wir nun annehmen, dass m_ix kein Default-Wert
	// ist, denn dann wird u.U. der Default-Eintrag im Repository
	// freigegeben!
	INVARIANT(m_ix!=m_global_default_ix,"no global default is context default");
	return (m_ix==contextdefault);
    }
}

/** Load default value to data object. The space that has been used by
    this data object in the repository is then no longer used (but is
    not freed by this method). */

template <class T>
void
data_type<T>::load_default()
{ 
    m_ix = mp_context->get_default_ix();
    if (m_ix==g_illegal_ix)
    { // kein Context-spezifischer Default-Wert
	m_ix = m_global_default_ix;
    }
    mp_value = &m_repository[m_ix];
}

/** Copy default value to data object. In contrast to the load_default
    method, the default value is copied to the respective field in the
    repository and thus the space in the repository is not freed. This
    is used for SHARED fields. */

template <class T>
void
data_type<T>::copy_default()
{ 
    ix_type ix = mp_context->get_default_ix();
    if (ix==g_illegal_ix)
    { // kein Context-spezifischer Default-Wert
	ix = m_global_default_ix;
    }
    (*mp_value) = m_repository[ix];
}

/** Allocate a new field in the repository for the data object. */

template <class T>
void 
data_type<T>::allocate
  (
  )
  { 
  m_ix = m_repository.insert(); 
  mp_value = &m_repository[m_ix];
  *mp_value = m_repository[get_default_ix()];
  }

/** Copy a value from the repository to this data object. */

template <class T>
void 
data_type<T>::load(
  ix_type a_ix
) 
{ 
  mp_value = &m_repository[a_ix]; 
  m_ix = a_ix; 
} 

/** Copy a value from the repository to this data object. */

template <class T>
void 
data_type<T>::copy(
  ix_type a_ix
)
{
    allocate();
    (*mp_value) = m_repository[a_ix]; 
}

/** Free the data object. The value of the data object will be the
    default value afterwards. Depending on whether it is a shared
    field or not, the space in the repository will be freed or not. */

template <class T>
void 
data_type<T>::free() 
{
    if (!is_default())
    { 
	if (is_shared())
	    copy_default();
	else
	{
	    m_repository.free(m_ix); 
	    load_default();
	}
    }
}

/** Free a specific field in the repository. */

template <class T>
void 
data_type<T>::free( 
    ix_type a_ix
    ) 
{
    if (a_ix!=get_default_ix())
    {
	if (is_shared())
	    copy_default();
	else
	{
	    m_repository.free(a_ix); 
	    if (m_ix==a_ix) load_default();
	}
    }
}

/** Return index of default value for this data object. This index
    leads to the context-specific default value (if it has been set)
    and otherwise to the global default value. */

template <class T>
ix_type 
data_type<T>::get_default_ix
  (
  ) 
const
  {
  if (mp_context->get_default_ix()==g_illegal_ix)
    // kein Context-spezifischer Default-Wert, also globaler Default-Wert
    return m_global_default_ix;
  else
    return mp_context->get_default_ix();
  }

/** Store the given value \c x in the repository (not in this data
    object) and return the assigned index. */

template <class T>
ix_type 
data_type<T>::store
(
  const T& x
) 
{ 
  return m_repository.insert(x); 
}

/** Output for debugging purposes. */

template <class T>
void
data_type<T>::info
  (
  ostream& os
  )
const
  {
  os << "<field>";
  if (!is_default()) os << "<value " << (*mp_value) << ">";
  os << (*mp_context);
  os << "</field>" << endl;
  }

/** Setting the global default value. This static method should be
    called once (globdef.cpp) to initialize the global default
    value. (g_illegal_ix for index fields, 0 for integer, etc.) */

template <class T>
void
data_type<T>::set_global_default
  ( 
  const T& a_default
  )
  { 
  // Wenn noch kein g_global_default existiert (noch keine Instanz
  // zuvor erzeugt), dann erstellen
  if (m_global_default_ix==g_illegal_ix)
    {
    m_global_default_ix = m_repository.insert(a_default); 
    }
  else
    {
    m_repository[m_global_default_ix]=a_default; 
    }
  }

/** Setting the context-specific default value. If the data object
    currently contains the (global) default value, it will contain the
    context-specific default value afterwards. */

template <class T>
void
data_type<T>::set_context_default( 
    const string& name,
    const matrix_separators* p_matsep
    )
{ 
    istrstream is(name.c_str());
    if (p_matsep==NULL) p_matsep = &g_matrix_ddl_separators;
    bool restore = !is_default();
    ix_type old_ix = m_ix;              // Index auf alten Wert zwischenspeichern
    if (mp_context->get_default_ix()==g_illegal_ix)
    {   // noch kein Context-spezifischer Default-Wert, also glob. Default-Wert
	allocate();                                         // neuen Platz bekommen
	read(is,p_matsep);                                 // Default-Wert auslesen
	mp_context->set_default_ix(m_ix);               // und im Context speichern
    }
    else
    {  // bereits Context-spez. Default-Wert bekannt, alten Wert ueberschreiben
	load(mp_context->get_default_ix());             // alten Default-Wert laden
	// Das Einlesen ueber read(is) ueberschreibt den Default-Wert
	// nicht, also muessen wir die Default-Eigenschaft kurzfristig
	// aufheben.
	mp_context->set_default_ix(g_illegal_ix);          // Default-Wert aufheben
	read(is,p_matsep);                           // neuen Default-Wert auslesen
	mp_context->set_default_ix(m_ix);              // Default-Wert reaktivieren
    }
    // Spezialfall: vorher global default, jetzt context-default, dann
    // duerfen wir auf keinen Fall den alten Inhalt wiederherstellen.
    if (restore) load(old_ix);
}

////////////////////////////////////////////////////////////////////
// field_type<T>

/** Constructor. */

template <class T>
field_type<T>::field_type(
  BaseTable *ap_table, ///< table to read field from (resp. write to)
  int mode, ///< i/o operation mode
  const string name, ///< name of the field
  Domain *d=gp_dm_numeric ///< domain of the field
  )
  : data_type<T>(new Context(Context::FIELD,mode,name,name,name,d))
  , m_io(context()->domain())
{
  if (ap_table!=NULL)
    { // Feld an eine Tabelle anhaengen
    INVARIANT(!ap_table->context()->is_field(),"");
    ap_table->attach(this);
    }
}

/** Constructor. This constructore allows to specify special tags for
    input and output (otherwise the name of the field is used). */

template <class T>
field_type<T>::field_type
  (
  BaseTable *ap_table, ///< table to read field from (resp. write to)
  int mode, ///< i/o operation mode
  const string name, ///< name of the field
  const string intag, ///< tag to be used for input
  const string outtag, ///< tag to be used for output
  Domain *d=gp_dm_numeric ///< domain of the field
  ) 
  : data_type<T>(new Context(Context::FIELD,mode,name,intag,outtag,d))
  , m_io(context()->domain())
  {
  if (ap_table!=NULL)
    { // Feld an eine Tabelle anhaengen
    INVARIANT(!ap_table->context()->is_field(),"");
    ap_table->attach(this);
    }
  }

/** Destructor. Deletes the context. */

template <class T>
field_type<T>::~field_type
(
)
{
// etwas ungluecklich: delete mp_context nicht erlaubt, weil Destruktoren (~data_type<T>) auf Kontext noch zugreifen (ueber free())...
//  if (mp_context!=NULL) delete mp_context;
//  mp_context=NULL;
}

/** Auslesen eines Wertes.

    Die Implementierung von data_type<T>::read() in field_type und
    array_type statt nur einmal in data_type hat folgenden Grund: Wir
    wollen immer davon ausgehen, dass die Eingabe in Matrixform sein
    kann, um dann ueber einen Select-String aus dieser Matrix das
    einzelne Feld oder die Teilmatrix auszuwaehlen. Dazu brauchen wir
    einen (statischen) DynMatrix<T>-Buffer, wobei T der Basistyp (int,
    double, etc.)  it. In data_type<T> waere T aber bereits vom Typ
    DynTuple/DynMatrix und wir braeuchten in dem Fall einen Buffer vom
    Typ DynMatrix<typename T::value_type>. Daher wird die
    Funktionalitaet hierher ausgelagert. 

    Die Funktion liest in den a_buffer und schreibt das Ergebnis der
    Auswahl (ueber den select-string) dann nach a_field und
    a_array. */


template <class T>
void
field_type<T>::read
  (
  istream& is,
  const matrix_separators* p_sep
  )
  {
  static DynMatrix<T> tmp; 
  matrix_read_dim(is,tmp,m_io,p_sep);

  if (mp_context->select().size()==0)
    var() = tmp(0,0);
  else
    var() = SelectTuple< DynMatrix<T>,DynMatrix<int> >(tmp,mp_context->select()).operator()(0);

  }

/** Lesen von r Werten (in einem .tab-Feld) */

template <class T>
void
field_type<T>::read
  (
  istream& is,
  int r,
  const matrix_separators* p_sep
  )
  {
  static DynMatrix<T> tmp; 

  // Wann soll das Lesen eines (symbolischen) Labels abgebrochen
  // werden? (noetig bei (a b), hier soll bspw. das "b" als Label
  // erkannt werden, nicht das Wort "b)")
  char breakchars[4];
  breakchars[0] = p_sep->m_seq_end;
  breakchars[1] = p_sep->m_field_sep;
  breakchars[2] = p_sep->m_val_sep;
  breakchars[3] = '\0';

  tmp.adjust(r);
  for (int i=0;i<r;++i) m_io.read(is,tmp[i],breakchars);

  if (mp_context->select().size()==0)
    var() = tmp(0,0);
  else
    var() = SelectTuple< DynMatrix<T>,DynMatrix<int> >(tmp,mp_context->select()).operator()(0);

  }

/** Spezialisierung fuer das Auslesen von Strings. Bei einem String
    lesen wir bis zum Feld-Ende-Zeichen (gemaess matrix_separators)
    (andernfalls kann es passieren, dass im XML Format ohne
    Leerzeichen Felder miteinander vermischt werden). Um das Einlesen
    von Leerzeichen zu erlauben, koennen Stringfelder mit
    Anfuehrungszeichen geklammert werden. */

template <>
void
field_type<string>::read
  (
  istream& is,
  const matrix_separators *ap_matsep 
  )
  {
  char endchar[2];
  endchar[1] = '\0';

  var().erase();

  if (is_followed_by(is,'\"',false)) 
    endchar[0] = '\"';
  else
    endchar[0] = ap_matsep->m_field_sep;

  read_until(is,endchar,var());
  is.putback(endchar[0]); // Feld-Ende-Zeichen nicht entnehmen
  }

/** Lesen von r Werten (in einem Feld) */

template <>
void
field_type<string>::read
  (
  istream& is,
  int r,
  const matrix_separators* p_sep
  )
  {
  INVARIANT(r==1,"field_type<string> reads single value only");
  read(is,p_sep);
  }


template <class T>
void
field_type<T>::write
(
  ostream& os,
  const matrix_separators*
)
const
{
  m_io.write(os,val());
}


template <>
void
field_type<string>::write
(
  ostream& os,
  const matrix_separators*
)
const
{
  os << operator()();
}

////////////////////////////////////////////////////////////////////
// array_type<T>


template <class T>
array_type<T>::array_type
  (
  BaseTable *ap_table,
  int mode,
  const string& name,
  Domain *d=gp_dm_numeric
  )
  : data_type<T>(new Context(Context::MATVEC,mode,name,name,name,d))
  , m_io(context()->domain())
  {
  if (ap_table!=NULL)
    { // Feld an eine Tabelle anhaengen
    INVARIANT(!ap_table->context()->is_field(),"");
    ap_table->attach(this);
    }
  }

/** wird vom set_type aufgerufen, set_type ist von array_type geerbt,
    aber nicht MATVEC sondern SETORRECORD. */

template <class T>
array_type<T>::array_type
  (
  Context *ap_context
  )
  : data_type<T>(ap_context)
  , m_io(ap_context->domain())
  {}

template <class T>
array_type<T>::~array_type()
  {
  delete mp_context;
  }


template <class T>
void
array_type<T>::read(
  istream& is,
  const matrix_separators* p_sep
  )
  {
  static DynMatrix<typename T::value_type> tmp;
  matrix_read_dim(is,tmp,m_io,p_sep);

  if (mp_context->select().size()==0)
    matrix_copy(var(),tmp);
  else
    matrix_copy(var(),SelectTuple< DynMatrix<typename T::value_type>,DynMatrix<int> >(tmp,mp_context->select()));
  }

/** Lesen von r Werten (in einem Feld) */

template <class T>
void
array_type<T>::read
  (
  istream& is,
  int r,
  const matrix_separators* p_sep
  )
  {
  // Wann soll das Lesen eines (symbolischen) Labels abgebrochen
  // werden? (noetig bei (a b), hier soll bspw. das "b" als Label
  // erkannt werden, nicht das Wort "b)")
  char breakchars[4];
  breakchars[0] = p_sep->m_seq_end;
  breakchars[1] = p_sep->m_field_sep;
  breakchars[2] = p_sep->m_val_sep;
  breakchars[3] = '\0';

  var().adjust(r);
  for (int i=0;i<r;++i) m_io.read(is,var()[i],breakchars);
  }

template <class T>
void
array_type<T>::write
  (
  ostream& os,
  const matrix_separators* p_sep
  )
  const
  {
  matrix_write_dim(os,operator()(),m_io,p_sep);
  }

}; // namespace

#endif // DATATYPE_SOURCE
