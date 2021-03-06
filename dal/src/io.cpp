
/** \file io.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org>
    \brief Implementation of symbolic input */

#ifndef IO_SOURCE
#define IO_SOURCE

#include "dal.hpp" // Definition von SymbolicIO
#include "globdef.hpp"
#include <iomanip>

namespace Data_Access_Library {

/** \class template<class T> SymbolicIO
    \brief Kombinierte numerisch/symbolische Eingabe.

    Bezug zu Domains */

/** Wir benutzen INT zum Speichern von ganzen Zahlen und als Referenz
    auf Label. In einem Stream stehen dann die Label, intern benutzen
    wir die INTs. Die Konvertierung INT<->LABEL geschieht durch diese
    beiden Klassen. Aber auch fuer reelle Zahlen gibt es symbolische
    Werte, bspw. koennen wir NaN mit einem bestimmten Wert
    belegen. 

    string_type's werden hier nicht eingelesen, dafuer gibt es eine
    Spezialisierung von field_type<T>::read(). */

template <class T>
void
SymbolicIO<T>::read(
    istream &is,
    T& a_value,
    const char* p_break
    )
    const
{
    if (mp_domain == NULL)
    {
	is >> a_value;
    }
    else if ((is.eof()) || (!is.good()))
    {
	return;
    }
    else
    {
	int x;
	if (mp_domain->find(is,x))
	{ // symbolischer Bezeichner direkt gefunden
	    a_value = x;
	}
	else
	{
	    read_white(is);
	    char c; is.get(c); 
	    if ( (mp_domain->mixed_mode()) && (isdigit(c)||(c=='.')||(c=='-')) )
	    { // numerischer Wert, unveraendert lesen (nur bei !mixed_mode)
		is.putback(c);
		is >> a_value;
	    }
	    else if (c=='\"')
	    { // symbolisches Label in ".." eingeklammert
		string entry;
		read_until(is,"\"",entry);
		a_value = mp_domain->add(entry);   
	    }
	    else
	    { // naechstes Wort als neues symbolisches Label uebernehmen
		is.putback(c);
		string entry;
		read_word_break(is,p_break,entry);
		if (mp_domain->is_fixed())
		{ // keine neuen Label erlaubt (bzw. unklar welcher Wert zugewiesen werden soll)
		    ERROR("do not know how to handle label \"" << entry 
			  << "\" (in fixed domain "<<*mp_domain<<")");
		}
		else 
		    a_value = mp_domain->add(entry);           
	    }
	}
    }
}
    
/** write

    Ausgabe von symbolischen Werten. Zwei Faelle: Wenn die Label
    vorher eingelesen wurden, dann haben wir symbolische Bezeichner,
    die wir nun ausgeben koennen. Wenn der Werte-Bereich (gewollt oder
    ungewollt) vergroesset wurde, dann geben wir einfach die
    entsprechende Zahl aus. */

template <class T>
void
SymbolicIO<T>::write
  (
  ostream &os,
  const T& a_value
  )
const
  {
  if (mp_domain == NULL)
    { 
      os << a_value;
    }
  else
    {
    os << setprecision( mp_domain->precision() );
    const string * p = mp_domain->identifier(a_value);
    if (p==NULL) os << a_value; else os << (*p);
    }
  }

} // namespace

#endif // IO_SOURCE

