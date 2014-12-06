
/*  Copyright     : Copyright (C) 2000-2002 Frank Hoeppner

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

/** \file access.cpp
    \author Frank Hoppner <frank.hoeppner@ieee.org> 

    Low-level Klassen zum Zugriff auf ASCII Datenfiles in
    verschiedenen Formaten. Ueber den hier definierten Klassen liegt
    eine weitere Klassenschicht (BaseTable), mit deren Hilfe der
    Benutzer auf die hier implementierten Klassen zugreift. */

#ifndef ACCESS_SOURCE
#define ACCESS_SOURCE

#include "access.hpp"
#include "define.hpp"
#include "mread.hpp" // matrix_separators
#include <stdio.h>

/** \namespace Data_Access_Library
    \brief Low-Level ASCII Datenfile-Zugriff. */

namespace Data_Access_Library {

/** Kommentarzeichen. In Tabellendateien (.tab und .csv) werden
    Zeilen, die mit diesem Zeichen beginnen, als Kommentare
    ueberlesen. (In .ddl Dateien koennen Kommentare beinahe beliebig
    plaziert werden.) */

char g_char_comment = '#';

/** Platzhalter fuer Default-Werte. In Tabellendateien (.tab und .csv)
    wird ein Feld auf dem Defaultwert gelassen, wenn das folgende
    Zeichen als Feldinhalt gelesen wird. (In .ddl Dateien wird ein
    Feld einfach weggelassen, um den Default-Wert nicht zu
    veraendern.) */
 
char g_char_emptyfield = '*';

/** Setzen der Record und Feld-Begrenzer in .ddl Dateien. */

void set_xml_separators(string &m_recb,string &m_rece,string &m_fieldb,string &m_fielde)
  { m_recb = "<%s>"; m_rece = "</%s>"; m_fieldb = "<%s "; m_fielde = ">"; }

/** Setzen der Record und Feld-Begrenzer in .csv Dateien. Der
    Feldende-Begrenzer muss eindeutig sein, d.h. ein Leerzeichen
    reicht nicht, weil Leerzeichen auch in Vektoren vorkommen. (Ein
    Leerzeichen in einem Vektorfeld koennte versehentlich als Feldende
    angesehen und das naechste Vektorelement als neues Feld...) */

void set_comma_separators(string &m_recb,string &m_rece,string &m_fieldb,string &m_fielde)
  { m_recb = "%s "; m_rece = "\n"; m_fieldb = ""; m_fielde = ","; }

/** Setzen der Record und Feld-Begrenzer .tab Dateien. In .tab Dateien
    sind Vektoren und Matrizen nicht erlaubt, daher kann hier das
    Leerzeichen als Feldende-Begrenzer fungieren. */

void set_table_separators(string &m_recb,string &m_rece,string &m_fieldb,string &m_fielde)
  { m_recb = "%s "; m_rece = "\n"; m_fieldb = ""; m_fielde = " "; }

/** Vektor- und Matrix-Begrenzer in .ddl Dateien. Siehe
    mread.hpp im math Paket. */

const matrix_separators g_matrix_ddl_separators =
  { m_eval_hdr:true
  , m_row_lf:false
  , m_flat:false
  , m_field_sep:'>'
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

/** Vektor- und Matrix-Begrenzer in .csv Dateien. Siehe
    mread.hpp im math Paket. */

const matrix_separators g_matrix_csv_separators =
  { m_eval_hdr:true
  , m_row_lf:false
  , m_flat:false
  , m_field_sep:','
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

/** Vektor- und Matrix-Begrenzer in .tab Dateien. Siehe
    mread.hpp im math Paket. */

const matrix_separators g_matrix_tab_separators =
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

/** Vektor- und Matrix-Begrenzer fuer console. Auch hier duerfen keine
    Leerzeichen verwendet werden, weil diese auf der Kommandozeile
    bereits benutzt werden, um die einzelnen Kommandos zu trennen. Siehe
    mread.hpp im math Paket. */

const matrix_separators g_matrix_cml_separators =
  { m_eval_hdr:true
  , m_row_lf:false
  , m_flat:false
  , m_field_sep:','
  , m_dim_begin:'['
  , m_dim_end:']'
  , m_cross:'x'
  , m_colon:':'
  , m_seq_begin:'('
  , m_val_sep:','
  , m_seq_end:')'
  , m_mod_sparse:'L'
  , m_mod_symm:'S'
  , m_mod_unit:'U'
  , m_mod_full:'M'
  , m_mod_diag:'D'
  };

/** \class IOBase
    \brief Abstrakte Basisklasse fuer Datenzugriff.

    IOBase ist eine abstrakte Basisklasse fuer Ein/Ausgabeoperationen.
    Spezialisierungen von IOBase sind entweder Reader (Leseoperation)
    oder Writer (Schreiboperation), aber niemals beides
    gleichzeitig. Es gibt Spezialisierungen fuer Tabellenfiles (.tab),
    komma-separierte-Werte (.csv) und Daten in einem XML-Stil
    (.ddl). Zukuenftige Erweiterungen sind eine Ausgabe im LaTeX
    Tabellenformat und eine SQL Datenbankanbindung. (Und ggf. ein echtes
    XML-Format (<data value="dd" line="8"/>)

    (1) Datenfelder werden ueber die Methode attach() beim Reader oder
    Writer angemeldet und in \c m_fields gespeichert. Die Attribute
    ... enthalten einige Angaben ueber die Position der Felder im
    Eingabe- oder Ausgabestrom, auf die Reihenfolge der Felder in \c
    m_fields ist relevant. Alle diese Angaben werden in init() richtig
    gesetzt, nachdem alle Felder angemeldet wurden. Nach dem Aufruf
    von init() ist kein attach() mehr erlaubt.

    (2) Die "Operation", also das Einlesen bei Readern und das
    Auslesen bei Writern, geschieht in zwei Phasen: bei einem fetch()
    Aufruf werden Informationen ueber die naechste Operation
    vorbereitet (bei einem Reader wird der naechste Record in den
    Speicher geladen), aber die Felder werden noch nicht
    gefuellt. Beim Aufruf von operate() wird dann ggf. Information aus
    der fetch()-Phase analysiert (bei einem Reader wird der Buffer
    analysiert und die Felder werden gefuellt). Jeder operate() Aufruf
    schliesst mit einem erneuten fetch() Aufruf, so dass alles fuer
    die naechste Lese-/Schreib-Operation vorbereitet ist. Der
    Rueckgabewert von operate() ist \c true, wenn die
    Lese-/Schreib-Operation erfolgreich war. Die Methode done()
    liefert \c true, wenn keine weitere Operation mehr moeglich ist
    (d.h. wir bei einer Eingabe-Operation am Ende der Tabelle
    angelangt sind, bei Ausgabe-Operation wird done() niemals \c
    true).

    (3) Alle Operationen sind "flach", d.h. es wird immer nur ein
    Record aus einer Tabelle gelesen, es wird keine hierarchische
    Struktur abgebildet. Dennoch kennt IOBase spezielle Felder, die
    ID-Felder (\c <key>) und Referenzen (\c <ref>) darstellen, und
    stellt spezielle Funktionalitaet fuer die darueberliegende Schicht
    (BaseTable) zur Verfuegung. 

    (4) Wir machen folgende Grundannahme [SEQ]: Die meiste Zeit wird
    sequentiell durch alle Daten iteriert, wahlfreier Direktzugriff
    ist die Ausnahme. Trotzdem werden rudimentaere
    Positionierungs-Moeglichkeiten zur Verfuegung gestellt. Die
    aktuelle Ein/Ausgabeposition kann in einem marker_type gespeichert
    werden. Die Methoden setpos() und getpos() erlauben das Setzen und
    Auslesen von Positionen. */


/** \var Context *IOBase::mp_context

    Zeiger auf den Kontext des IOBase Objekts. Enthaelt Informationen
    ueber Dateiname, Collectionname, Anzahl der Felder, etc. */

/** \var const matrix_separators *IOBase::mp_matsep

    Zeiger auf die Struktur mit Separator-Zeichen fuer die
    Schreibweise von Matrizen und Vektoren, Feldtrennzeichen, etc. Je
    nach Dateityp (.tab, .csv, .ddl) sind unterschiedliche
    Trennzeichen zu verwenden. Zeiger wird einmal in IOBase::init()
    gesetzt. */

/** \var adtp_list IOBase::m_fields

    Liste der Felder, die von einem IOBase Objekt
    ausgelesen/geschrieben werden sollen. */

/** \var streampos IOBase::m_curr_loc 

    Streamposition des Records, der aktuell geladen ist. Wenn es sich
    nicht um eine Datei handelt, sondern cin/cout, wird die Position
    bei jedem fetch() hochgezaehlt, um unterschiedliche Positionen
    fuer unterschiedliche Records zu bekommen. Eine Positionierung ist
    in cin/cout nicht moeglich. */

/** \var streampos IOBase::m_next_loc 

    Dateiposition des naechsten Records. Siehe auch m_curr_loc. */

/** \var ix_type IOBase::m_curr_key
    \brief Schluessel <key> des Records, der aktuell geladen ist. */

/** \var ix_type IOBase::m_next_key
    \brief Schluessel <key> des naechsten Records. */

/** \var ix_type IOBase::m_curr_ref
    \brief Referenz <ref> des Records, der aktuell geladen ist. */

/** \var ix_type IOBase::m_next_key
    \brief Referenz <ref> des naechsten Records. */

/** \var bool IOBase::m_done

    Wird wahr, nachdem der letzte Record aus dem Datenfile gelesen
    wurde. Abfrage ueber IOBase:done(). */

/** \var string IOBase::m_recbegin

    Zeichenkette, ueber die der Record-Anfang identifiziert wird (bei
    XML bspw. \c <args>). Ist von der Art des Datenfiles abhaengig,
    wird in IOBase::init gesetzt. */

/** \var string IOBase::m_recend 

    Zeichenkette, ueber die das Ende eines Records identifiziert wird
    (bei XML bspw. \c </args>). Ist von der Art des Datenfiles
    abhaengig, wird in IOBase::init gesetzt. */

/** \var string IOBase::m_fieldbegin

    Zeichenkette, ueber die ein Feld-Anfang identifiziert wird (bei
    XML bspw. \c <, der folgende Teil wird dann als Name des Feldes
    interpretiert). Ist von der Art des Datenfiles abhaengig, wird in
    IOBase::init gesetzt. */

/** \var string IOBase::m_fieldend 

    Zeichenkette, ueber die das Ende eines Feldes identifiziert wird
    (beim XML bspw. \c >). Ist von der Art des Datenfiles abhaengig,
    wird in IOBase::init gesetzt. */

/** \fn int IOBase::ref() const

    Liefert den Inhalt des Referenz-Feldes \c <ref> des aktuellen
    Records, d.h. das Attribut IOBase::m_curr_ref. */

/** \fn int IOBase::nextref() const

    Liefert den Inhalt des Referenz-Feldes \c <ref> des naechsten
    Records, d.h. das Attribut IOBase::m_next_ref. */

/*

m_ahead_loc = position des datensatzes, der im lookahead steht. wird in fetch() gesetzt und in operate() geloescht.

    inline int nextkey() const { return m_next_key; }
    inline bool done() const { return m_done; }
    inline bool eof() const { return m_done; }
//    inline bool is_empty() const { return m_empty; }
    inline Context* context() { return mp_context; }
protected:
    void sort_fields_by_intag();
    void sort_fields_by_offset();
protected:
}; */


/** Every i/o operation needs a context, which specifies filenames,
    type of file, etc.. */

IOBase::IOBase(
    Context *ap_context
    ) 
    : mp_context(ap_context)
    , mp_matsep(NULL)
    , m_done(false)
{}

IOBase::~IOBase()
{}

/** Anmelden eines Datenfeldes zur Ein/Ausgabe. Ob es sich um Ein-
    oder Ausgabe handelt, wie der Feldname heisst, etc., ist alles im
    Context des abstract_data_type notiert.

    Das Feld wird in das IOBase::m_fields Attribut eintragen. Die
    Reihenfolge, in der sie in der Liste stehen, bestimmt die
    Ausgabereihenfolge. Daher wird bei KEYREF Feldern am Listenanfang
    und sonst am Listenende eingefuegt, so dass Schluesselfelder im
    vorne stehen. */

void
IOBase::attach(abstract_data_type *ap_field)
{
    if (ap_field->context()->is_keyref())
	m_fields.push_front(ap_field);
    else
	m_fields.push_back(ap_field);
}

/** Initialisierung der Ein/Ausgabe. Bei der Eingabe heisst das, dass
    die Datei geoeffnet und ggf. Headerinformationen gelesen
    werden. Bei der Ausgabe wird entsprechend der Header geschrieben,
    sofern dass bei der Initialisierung schon moeglich ist. 

    Die Anfangs- und Ende-Bezeichner von Records werden hier
    gesetzt. Bei Dateien im XML-Stil wird der Name der Collection in
    diesen Bezeichnern integriert (z.B. der Tag \c <args>). */

bool 
IOBase::init()
{
    string::size_type i;
    
    i = m_recbegin.find("%s");
    if (i!=string::npos) m_recbegin.replace(i,2,mp_context->intag());
    
    i = m_recend.find("%s");
    if (i!=string::npos) m_recend.replace(i,2,mp_context->intag());
    
    // Fileposition ist per Default der Dateianfang
    m_curr_loc = m_next_loc = 0;
    // key/ref-Informationen liegen nicht vor.
    m_curr_key = g_illegal_ix; 
    m_next_key = g_illegal_ix; 
    m_curr_ref = g_illegal_ix;
    m_next_ref = g_illegal_ix;
    // nichts im lookahead
    m_ahead_loc = -1;
    return true;
}

/** Liefert einen marker_type mit dessen Hilfe spaeter auf aktuelle
    Position zurueckgesprungen werden kann (wahlfreier Zugriff,
    bspw. fuer den Aufbau eines Index). Die Unterstuetzung des
    wahlfreien Zugriffs ist wegen der Grundannahme [SEQ] nur
    rudimentaer. 

    Der Marker marker_type besteht aus File-ID und File-Position
    (Attribut IOBase::m_curr_loc). File-ID ist per default zunaechst
    immer -1, bekommt erst bei FileList<T> eine Bedeutung. 

    Neben der Position muss auch der key gespeichert werden, falls die
    keys nicht in der Datei enthalten sind (also keine <key> Eintraege
    vorhanden sind). Bei einem Ruecksprung laesst sich ohne diese
    Information nicht rekonstruieren, welchen <key> ein Record hat,
    weil er automatisch inkrementell erzeugt wird.  */

void 
IOBase::getpos(marker_type& mark)
{
    mark.m_file = -1;
    mark.m_key = m_curr_key;
    mark.m_spos = m_curr_loc;
}

/** Setzt die Ein/Ausgabe auf die angegebene Position. Bei einer
    Ausgabe ist das gleichbedeutend damit, dass alle Ausgaben seit dem
    letzten Besuch dieser Position geloescht werden (weil nun folgende
    Schreiboperationen die alten Daten ueberschreiben werden).

    Das Setzen einer neuen Position ist bspw. bei cin nicht
    moeglich. Die IOBase Implementierung bricht mit einem Fehler ab,
    wenn der Marker ungleich dem Dateianfang ist. Methode wird fuer
    Dateiein/ausgabe ueberladen. */

void
IOBase::setpos(const marker_type& mark)
{
    if ((mark.m_spos!=0) || (m_curr_key!=g_illegal_ix))
    {
	ERROR("request for seeking in a non-seekable stream. Fatal error.");
	exit(-1);
    }
}

/** \fn const string& IOBase::name()

    Liefert einen Namen fuer das Ein/Ausgabeobjekt. Das ist
    ueblicherweise ein Dateiname, aber kann auch "console" sein, wenn
    es sich bei der Ein/Ausgabe um cin/cout handelt. */


/** \fn bool IOBase::fetch()

    Bevor die Felder eines Records ueber IOBase::pop geladen werden,
    muss mit IOBase::fetch der Eintrag in den Speicher geladen werden
    (in den String IOBase::m_lookahead). Dies vereinfacht das spaetere
    Auslesen von Feldern (siehe IOBase::pop in den
    Spezialisierungen). Darueber hinaus wird in IOBase::fetch
    ermittelt, welchen Schluessel <key> und welche Referenz <ref> im
    naechsten Record vorliegen (Attribute IOBase::m_next_key und
    IOBase::m_next_ref). Diese Funktionalitaet wird von der naechsten
    Abstraktionsschicht (BaseTable) benutzt, um hierarchische Records
    auszuwerten. */

/** Je nach Art des IOBase Objektes wird beim Aufruf dieser Methode
    ein neuer Datensatz gelesen oder der aktuelle Datesatz
    geschrieben.

    Im Eingabemodus wird der zuvor durch IOBase::fetch geladene Record
    nun analysiert und die Felder (IOBase::m_fields) werden
    geladen. Am Ende der Operation erfolgt ein Aufruf von
    IOBase::fetch, um fuer den naechsten Aufruf von IOBase::operate
    geruestet zu sein und die keys/refs des naechsten Records liefern
    zu koennen. */

bool
IOBase::operate
  (
  )
  {
  LOG_METHOD("Access","IOBase","operate",context()->name());
  m_ahead_loc = -1; // look ahead aufgebraucht nach operate()
  return true;
  }

void
IOBase::sort_fields_by_offset()
  {
  // wir sortieren m_fields so um, dass die Reihenfolge durch die
  // offsets gegeben ist
  if (m_fields.size()>1)
    {
    bool sorted;
    do
      {
      sorted=true;
      adtp_list::iterator i_curr(m_fields.begin()),i_next(i_curr); ++i_next;
      while (  (i_next!=m_fields.end())
            && ((*i_curr)->context()->offset() <= (*i_next)->context()->offset()))
        { ++i_curr; ++i_next; }
      if (  (i_next!=m_fields.end()) 
         && ((*i_curr)->context()->offset() > (*i_next)->context()->offset()))
        { swap(*i_curr,*i_next); sorted=false; }
      }
    while (!sorted);
    }
  }

void 
IOBase::sort_fields_by_intag()
{
    // Alle angemeldeten Felder werden nach ihrem intag() sortiert. 
    if (m_fields.size()>1)
    {
	bool sorted;
	do
	{
	    sorted=true;
	    adtp_list::iterator i_curr(m_fields.begin()),i_next(i_curr); 
            ++i_next;
	    while ((i_next!=m_fields.end()) &&
		   ((*i_curr)->context()->intag() <= (*i_next)->context()->intag()))
	    { ++i_curr; ++i_next; }
	    if ((i_next!=m_fields.end()) && 
		((*i_curr)->context()->intag() > (*i_next)->context()->intag()))
	    { swap(*i_curr,*i_next); sorted=false; }
	}
	while (!sorted);
    }
}

/** Writer *********************************************************/

/** \class Writer

    Die Writer-Klasse ist eine Spezialisierung von IOBase fuer die
    Ausgabe von Tabellen. Eine Reihe von virtuellen Methoden werden
    neu definiert, insbesondere fetch() und operate(). */

/** Konstruktor fuer einen Writer. Die diversen Schluesselfelder
    werden lokal als Zeiger gespeichert. Das Einfuegen (attach) in die
    Feldliste erfolgt aber spaeter (durch den Erzeuger des Writers,
    siehe table.cpp). */

Writer::Writer(
    Context *ap_context,
    index_type *ap_output_key,
    index_type *ap_output_ref,
    index_type *ap_input_key,
    index_type *ap_required_key,
    ostream* os
    )
    : IOBase(ap_context)
    , mp_stream(os)
    , mp_output_key(ap_output_key)
    , mp_output_ref(ap_output_ref)
    , mp_input_key(ap_input_key)
    , mp_required_key(ap_required_key)
{
}

Writer::~Writer()
{
}

bool
Writer::init() 
{
    LOG_METHOD("Access","Writer","init",context()->name());
    if (!IOBase::init()) return false;
    INFO("writing <"<<context()->name()<<"> to '"<<context()->outfile()<<"'");
    m_done = !fetch();
    return !m_done;
}

/** Setzen der Ausgabeposition.

    Die Position im Ausgabestrom, bei der der naechste Schreibvorgang
    erfolgen soll, wird auf \c mark gesetzt. Daten, die hinter
    dieser Position liegen, werden ueberschrieben. Die aktuellen \c
    <key> und \c <ref> Angaben werden ungueltig, der naechste Record
    (an der angegebenen Position) wird vorbereitet (durch Aufruf von
    fetch()). */

void
Writer::setpos(const marker_type& mark) 
{
    LOG_METHOD("Access","Writer","setpos",mark);

    // keine aktuelle Position mehr
    m_curr_key = g_illegal_ix;
    m_curr_ref = g_illegal_ix;
    m_curr_loc = 0;
    
    // naechste Position festlegen (Positionierung ueber seek erfolgt
    // in File<T>::setpos, bei cin/cout duerfen wir kein
    // stream::seek() aufrufen)
    m_next_loc = mark.m_spos;
    m_next_key = mark.m_key;
    m_done=!fetch(); 
}

bool
Writer::operate()
{
    LOG_METHOD("Access","Writer","operate",context()->name());
    
    IOBase::operate();

    // aktuellen KEY immer automatisch setzen
    m_curr_key = mp_output_key->var() = m_next_key; 
    // aktuelle REF uebernehmen oder setzen (wenn nicht manuell gesetzt)
    if ((mp_output_ref!=NULL)&&(!mp_output_ref->is_default())) 
	m_curr_ref = mp_output_ref->val(); 
    else 
    { 
	m_curr_ref = m_next_ref; 
	if ((mp_output_ref!=NULL)&&(m_next_ref!=g_illegal_ix))
	    mp_output_ref->var()=m_next_ref; 
    } 
    // Location uebernehmen
    m_curr_loc = m_next_loc; 
    TRACE("write curr: key="<<m_curr_key<<", ref="<<m_curr_ref
	  <<", loc="<<m_curr_loc);
    
    // Folgeposition
    m_done = !fetch();
    TRACE("write next: key="<<m_next_key<<", ref="<<m_next_ref
	  <<", loc="<<m_next_loc);
    
    // Rueckgabewert: Operation (=Schreiben) erfolgreich
    return !m_done;
}

/** Es werden alle m_next_* Variablen gesetzt. */

bool
Writer::fetch()
{
    // Automatisch KEY inkrementieren
    if (mp_required_key!=NULL) 
	m_next_key=mp_required_key->val(); 
    else
	// wenn cSUBSETOUT dann outputkey=inputkey
	if ((mp_context->is_subsetout())&&(mp_context->is_readout())&&(mp_input_key!=NULL))
	{
	    INVARIANT(mp_input_key->val()!=g_illegal_ix,"SUBSETOUT: read before write");
	    m_next_key = mp_input_key->val();
	}
	else
	{
	    if (m_next_key==g_illegal_ix) // erster Key
		m_next_key = 0; 
	    else 
		m_next_key = m_curr_key+1; 
	}

    m_next_ref = g_illegal_ix;
    if (mp_stream==NULL) ++m_next_loc; else m_next_loc = tell();
    m_ahead_loc = m_next_loc;

    return (mp_stream==NULL)?true:mp_stream->good();
}

/** \class Reader */

/** \fn void Reader::sneak()

    Diese Methode liest aus dem lookahead (ohne ihn zu zerstoeren) den
    <key> in \c m_next_key und \c <ref> in \c m_next_ref. sneak MUSS
    immer wenigstens eine key zurueckliefern, incrementiert alten key
    wenn kein expliziter key vorhanden. Um diese Inkrementierung
    implementieren zu koennen, setzt die Methode voraus, dass beim
    Aufruf m_next_key den vorherigen m_next_key enthaelt. */

/** Konstruktor fuer einen Reader. Die diversen Schluesselfelder
    werden lokal als Zeiger gespeichert. Das Einfuegen (attach) in die
    Feldliste erfolgt aber spaeter (durch den Erzeuger des Readers,
    siehe table.cpp). */

Reader::Reader(
    Context *ap_context,
    index_type *ap_key,
    index_type *ap_ref,
    index_type *ap_required_key,
    index_type *ap_required_ref,
    istream* is
    )
    : IOBase(ap_context)
    , mp_stream(is)
    , mp_key(ap_key)
    , mp_ref(ap_ref)
    , mp_required_key(ap_required_key)
    , mp_required_ref(ap_required_ref)
{
}

Reader::~Reader()
{
}

bool
Reader::init()
{
    LOG_METHOD("Access","Reader","init",context()->name());
    if (!IOBase::init()) return false;
    INFO("reading <"<<context()->name()<<"> from '"<<context()->infile()<<"'");
    // Reader::init wird NACH parent::init aufgerufen, Header wurde
    // schon gelesen, tell() liefert Position nach dem Header (wichtig
    // bei CSV, da dort der Header kein Kommentar ist)
    m_curr_loc = m_next_loc = (mp_stream==NULL)?0:tell();
    
    m_done = !fetch();
    return !m_done;
}

/** Setzen der Eingabeposition.

    Die Position im Eingabestrom, bei der der naechste Lesevorgang
    erfolgen soll, wird auf \c mark gesetzt. Die aktuellen \c <key>
    und \c <ref> Angaben werden ungueltig, das Lesen des naechsten
    Records (an der angegebenen Position) wird durch den Aufruf von
    fetch() vorbereitet. 

    Es passiert nichts, wenn die markierte Position gleich der
    aktuellen Position ist. Dann wird eine Repositionierung des Stream
    unterdrueckt, was fuer eine Verwendung im Zusammenhang mit \c cin
    erforderlich ist. */

void 
Reader::setpos(const marker_type& mark)
{
    LOG_METHOD("Access","Reader","setpos",mark);

    // selbe Position, lookahead gelesen, aber noch nicht ausgewertet?
    bool sameposition = ( (m_curr_loc == mark.m_spos) && 
			  (m_ahead_loc == mark.m_spos) );

    if (sameposition)
    {
	// lookahead noch unbenutzt, keine Aenderung
	TRACE("same position");
//	m_done=false;
    }
    else
    {
	// keine aktuelle Position mehr
	m_curr_key = g_illegal_ix;
	m_curr_ref = g_illegal_ix;
	m_curr_loc = 0;

	m_lookahead.erase(); 
	m_next_loc = mark.m_spos;
	m_next_key = mark.m_key;
	m_done=!fetch(); 
    }
}

/** Variablen werden geladen, lookahead wird geloescht. */

bool
Reader::operate()
{
    LOG_METHOD("Access","Reader","operate",context()->name());

    IOBase::operate();

    // Wenn schon EOF festgestellt, brauchen wir nichts weiter tun
    if (done()) { TRACE("DONE."); return false; }

    // Ggf. noch nicht die richtige Zeile gefetcht (bei Subreadern),
    // weil zum Zeitpunkt des letzten fetch() der richtige
    // Hauptdatensatz noch nicht geladen war.
    if ((mp_required_key!=NULL)&&(mp_required_key->val()!=m_curr_key)) fetch();

    // Zeile wurde schon bei fetch() in m_lookahead ausgelesen...
    istrstream is(m_lookahead.c_str());
    load(is);
    // lookahead verbraucht
    m_lookahead.erase();
    
    // aktuellen Key setzen, falls nicht ausgelesen
    bool autokey = (mp_key==NULL)||(mp_key->val()==g_illegal_ix);
    if (autokey) 
    { m_curr_key = m_next_key; if (mp_key!=NULL) mp_key->var()=m_curr_key; }
    else
    { m_curr_key = mp_key->val(); }

    // aktuelle Ref setzen, falls nicht ausgelesen
    bool autoref = (mp_ref==NULL)||(mp_ref->val()==g_illegal_ix);
    if (autoref)
    {
	m_curr_ref = m_next_ref; 
	/* 21.8.02 fh. Eigentlich sollte diese Anweisung unnoetig
	   sein, weil entweder in load() das <ref> Feld gelesen wurde
	   oder andernfalls autoref die Hierarchie nicht veraendern
	   sollte... */
//	if (mp_ref!=NULL) mp_ref->var()=m_curr_ref; 
    }
    else
    { m_curr_ref = mp_ref->val(); }

    // aktuelle Position setzen
    m_curr_loc = m_next_loc; 

    TRACE("read curr: key="<<m_curr_key<<", ref="<<m_curr_ref<<", loc="<<m_curr_loc);
    
    // Vorschau auf naechste Position
    m_done = !fetch();
    TRACE("read next: key="<<m_next_key<<
	  ", ref="<<m_next_ref<<", loc="<<m_next_loc);
    
    return !m_done;
}

/** Es werden alle m_next_* Variablen gesetzt. */

bool
Reader::fetch()
{
    LOG_METHOD("Access","Reader","fetch","");

    // Beim Auslesen aus mehreren Dateien: mp_required_key gibt an,
    // welcher key-Wert gegeben sein muss.
    ix_type required_key = (mp_required_key==NULL) 
	? g_illegal_ix 
	: mp_required_key->val(); 
//// WIR SIND HIER IN EINER FETCH OPERATION, LADEN ALSO IN DEN LOOKAHEAD WAS ALS NAECHSTES RELEVANT WIRD. BEI SUBREADERN ENTSPRICHT DAS DEM RECORD, DER MIT DEM NEXT_KEY DES VATERRECORDS ZUSAMMENHAENGT, ALSO MUESSTE REQUIRED KEY NICHT INPUT_KEY SONDER NEXT_INPUT_KEY SEIN! ***

    // Solange weiterlesen bis ein Detailrecord gefunden wurde, der
    // entweder zum Master passt oder erst zu einem spaeteren Master
    // (dann koennen wir auch aufhoeren zu suchen)
    while (   (m_lookahead.empty())
	      || ( (required_key!=g_illegal_ix)&&
		   ((m_next_key==g_illegal_ix)||(required_key>m_next_key)) )
        )
    {
	m_lookahead.erase();
	
	// Kommentare ueberlesen
	read_white(*mp_stream);
	while (is_followed_by(*mp_stream,g_char_comment,false)) 
	    read_line(*mp_stream);
	if ((mp_stream->eof())||(!mp_stream->good())) { return false; }
	
	// lesen, bis wir am Zeilenanfang m_recbegin finden
	m_next_loc = (mp_stream==NULL)?m_next_loc+1:tell();
	if (!m_recbegin.empty())
	{
	    while ((!mp_stream->eof())&&(mp_stream->good())&&
		   (!is_followed_by(*mp_stream,m_recbegin.c_str(),false))) 
	    { read_line(*mp_stream); m_next_loc=tell(); }
	}
	if ((mp_stream->eof())||(!mp_stream->good())) { return false; }
	read_until(*mp_stream,m_recend.c_str(),m_lookahead);
	
	// wenn zeilenanfang eindeutig identifizierbar ist, dann
	// akzeptieren wir auch leere Eintraege (wie in
	// <args></args>), aber nicht in table-Dateien (wo Leerzeielen
	// ignoriert werden)
	if (!m_lookahead.empty()) //((!m_lookahead.empty()) || (!m_recbegin.empty()))
	{
	    m_lookahead += '\0';
	    istrstream is(m_lookahead.c_str());
	    //m_next_key = g_illegal_ix; Vorbed. fuer sneak: alter next_key!
	    m_next_ref = g_illegal_ix;
	    sneak(is);
	}
	TRACE_VAR(m_lookahead);
	TRACE("nextkey="<<m_next_key<<" reqkey="<<required_key);
    }
    m_ahead_loc = m_next_loc;
/*
    // Automatisch KEY inkrementieren => muss in sneak passieren!
    insbesonder auch im fall mehrfacher iteration!!! in obiger schleife muss eine mehrfache inkrementierung erfolgen!
    if (m_curr_key==g_illegal_ix) // erster Key
	m_next_key = 0; 
    else if (m_next_key==g_illegal_ix) // kein Key gelesen
	m_next_key = m_curr_key+1; 
    else if (m_next_key<=m_curr_key) // Key gelesen, aber falsche Reihenfolge
    {
	WARNING("overriding unsorted keys; next="
		<<m_next_key<<", curr="<<m_curr_key);
	m_next_key = m_curr_key+1;
    }
*/    
    // Wenn wir schon zu weit sind, haben wir das "Ende" bzgl des
    // aktuellen Masterrecords erreicht (keine weiteren Detailrecords
    // vorhanden)
    if (required_key<m_curr_key) { return false; }
    
    return true;
}

/** Klasse NULL_Writer *******************************************/

NULL_Writer::NULL_Writer(
    Context *ap_context,
    index_type *ap_output_key,
    index_type *ap_output_ref,
    index_type *ap_input_key,
    index_type *ap_required_key
    )
    : Writer(ap_context,ap_output_key,ap_output_ref,ap_input_key,ap_required_key,NULL)
{
}

bool
NULL_Writer::init() 
{
    LOG_METHOD("Access","NULL_Writer","init",context()->name());

    Writer::init();
    return false;
}

bool
NULL_Writer::operate()
{
    LOG_METHOD("Access","NULL_Writer","operate",context()->name());

    Writer::operate();
    return false;
}

/** Klasse TAG_Writer *******************************************/

TAG_Writer::TAG_Writer(
    Context *ap_context,
    index_type *ap_output_key,
    index_type *ap_output_ref,
    index_type *ap_input_key,
    index_type *ap_required_key,
    ostream *ap_os
    )
    : Writer(ap_context,ap_output_key,ap_output_ref,ap_input_key,ap_required_key,ap_os)
{
    set_xml_separators(m_recbegin,m_recend,m_fieldbegin,m_fieldend);
    mp_matsep = &g_matrix_ddl_separators;
}

bool
TAG_Writer::init() 
{
    LOG_METHOD("Access","TAG_Writer","init",context()->name());
    return (Writer::init()) && (mp_stream->good());
}

bool
TAG_Writer::operate()
{
    LOG_METHOD("Access","TAG_Writer","operate",context()->name());

    Writer::operate();

    (*mp_stream) << m_recbegin;
    
    size_t pos = m_fieldbegin.find("%s"); 
    
    for (adtp_list::const_iterator f=m_fields.begin();f!=m_fields.end();++f)
    { 
	if ((!(*f)->is_default())&&
	    ((!mp_context->is_flat())||(!(*f)->context()->is_keyref())))
	{
	    (*mp_stream) << m_fieldbegin.substr(0,pos);
	    if (pos!=string::npos) 
		(*mp_stream) << (*f)->context()->outtag() 
			     << m_fieldbegin.substr(pos+2);
	    (*f)->write(*mp_stream,mp_matsep); 
	    TRACE_ID("writefield","write <"<<mp_context->name()<<">: "
		     <<(*f)->context()->name()<<':'<<(**f)); 
	    (*mp_stream) << m_fieldend;
	}
    }
    
    (*mp_stream) << m_recend << endl; mp_stream->flush();
    
    return mp_stream->good();
}

/** Klasse NULL_Reader *********************************************/

NULL_Reader::NULL_Reader(
    Context *ap_context,
    index_type *ap_key,
    index_type *ap_ref,
    index_type *ap_required_key,
    index_type *ap_required_ref
    )
    : Reader(ap_context,ap_key,ap_ref,ap_required_key,ap_required_ref,NULL)
{
}

bool
NULL_Reader::init() 
{
    LOG_METHOD("Access","NULL_Reader","init",context()->name());
    return false;
}

bool
NULL_Reader::fetch()
{
    LOG_METHOD("Access","NULL_Reader","fetch",context()->name());
    m_next_key = m_next_ref = g_illegal_ix;
    return false;
}

bool
NULL_Reader::operate()
{
    LOG_METHOD("Access","NULL_Reader","operate",context()->name());
    return false;
}

void
NULL_Reader::load(istream&)
{
    LOG_METHOD("Access","NULL_Reader","load",context()->name());
    m_curr_key=m_curr_ref=g_illegal_ix;
}

void
NULL_Reader::sneak(istream&)
{
    LOG_METHOD("Access","NULL_Reader","sneak",context()->name());
    m_next_key=m_next_ref=g_illegal_ix;
}

/** Klasse TAG_Reader *********************************************/

TAG_Reader::TAG_Reader(
    Context *ap_context,
    index_type *ap_key,
    index_type *ap_ref,
    index_type *ap_required_key,
    index_type *ap_required_ref,
    istream *ap_is 
    )
    : Reader(ap_context,ap_key,ap_ref,ap_required_key,ap_required_ref,ap_is)
    , mp_keywords(NULL)
    , mp_text(NULL)
    , m_key_index(-2)
    , m_ref_index(-2)
{
    set_xml_separators(m_recbegin,m_recend,m_fieldbegin,m_fieldend);
    mp_matsep = &g_matrix_ddl_separators;
}

TAG_Reader::~TAG_Reader()
{
    if (mp_keywords!=NULL) delete[] mp_keywords;
    if (mp_text!=NULL) delete[] mp_text;
}

bool
TAG_Reader::init() 
{
    LOG_METHOD("Access","TAG_Reader","init",context()->name());
    
    /** Wir bestimmen die Anzahl der Felder und ihre intag() Laenge */
    m_size = 0; m_length = 0;
    for (adtp_list::iterator f = m_fields.begin();f!=m_fields.end();++f)
    { ++m_size; m_length += (*f)->context()->intag().length(); }
    
    /** Alle angemeldeten Felder werden nach ihrem intag() sortiert. */
    sort_fields_by_intag();
    
    /** Das mp_keywords Feld wird mit den sortieren intags()
	gefuellt. */
    mp_text = new char[m_size*m_fieldbegin.length()+m_length];
    mp_keywords = new keyword_entry[m_size+1];
    
    word x=0,n=0;
    for (adtp_list::iterator f=m_fields.begin();f!=m_fields.end();++f)
    {
	if ((*f)==mp_key) m_key_index=x;
	if ((*f)==mp_ref) m_ref_index=x;
	mp_keywords[x].name = mp_text+n;
	mp_keywords[x].identifier = x;
	++x;
	
	sprintf(mp_text+n,m_fieldbegin.c_str(),(*f)->context()->intag().c_str());
	n += m_fieldbegin.length()-2 + ((*f)->context()->intag().length())+1;
    }
    mp_keywords[x].name=NULL;
    mp_keywords[x].identifier=-1;
    //for (x=0;x<n;++x) mp_text[x]=toupper(mp_text[x]);
    
    return Reader::init();
}

void
TAG_Reader::sneak(istream& is)
{
    LOG_METHOD("Access","TAG_Reader","sneak",context()->name());

    bool foundkey = false, foundref = false;
    while ((is.good()) && ((!foundkey)||(!foundref)))
    {
	int index = is_followed_by(is,mp_keywords,false,false);
	if (index!=-1)
	{
	    if (index==m_key_index)
	    { 
		is >> m_next_key; // BAUSTELLE: sollte m_io & Context benutzen!
		foundkey=true;
		TRACE_ID("readfield","sneak key<"<<mp_context->name()<<">="<<m_next_key);
	    }
	    else if (index==m_ref_index)
	    { 
		is >> m_next_ref; 
		foundref=true;
		TRACE_ID("readfield","sneak ref<"<<mp_context->name()<<">="<<m_next_ref);
	    }
	}
	read_until(is,m_fieldend.c_str());
    }

    if (!foundkey) 
	if (m_next_key==g_illegal_ix) m_next_key=0; else ++m_next_key;
}

void
TAG_Reader::load(istream& is)
{
    LOG_METHOD("Access","TAG_Reader","load",context()->name());
    
    while (is.good())
    {
	int index = is_followed_by(is,mp_keywords,false,false);
	if (index!=-1)
	{ // relevantes Feld, auslesen
	    
	    // wieviele Felder erhalten ihren Wert aus dieser Eingabe?
	    int ctr = 1;
	    while ((index+ctr<m_size)&&
		   (strcmp(mp_keywords[index].name,mp_keywords[index+ctr].name)==0))
		++ctr;
	    
	    // diese Felder in der Liste ausfinding machen (ab Position index)
	    adtp_list::iterator f = m_fields.begin(); 
	    while (index>0) { ++f; --index; }
	    
	    // und auslesen
	    streampos p = is.tellg();
	    for (;ctr>0;--ctr) 
	    { 
		is.seekg(p); (*f)->read(is,mp_matsep); 
		TRACE_ID("readfield","read <"<<mp_context->name()<<">: "
			 <<(*f)->context()->name()<<':'<<(**f)); 
		++f; 
	    }
	}
	read_until(is,m_fieldend.c_str());
    }
}

/** Klasse CSV_Reader ******************************************/

CSV_Reader::CSV_Reader(
  Context *ap_context,
  index_type *ap_key,
  index_type *ap_ref,
  index_type *ap_required_key,
  index_type *ap_required_ref,
  istream *ap_is 
  )
  : Reader(ap_context,ap_key,ap_ref,ap_required_key,ap_required_ref,ap_is)
  , m_key_index(-2)
  , m_ref_index(-2)
  {
  set_comma_separators(m_recbegin,m_recend,m_fieldbegin,m_fieldend);
  mp_matsep = &g_matrix_csv_separators;
  }

CSV_Reader::~CSV_Reader
  (
  )
  {
  }

bool
CSV_Reader::init
  (
  ) 
  {
  LOG_METHOD("Access","CSV_Reader","init",context()->name());

  // Wir gehen davon aus, dass es einen Header gibt.
  read_white(*mp_stream);
  string name;
  read_line(*mp_stream,name); read_white(*mp_stream);
  istrstream header(name.c_str());
  TRACE("header=" << name);

  /** Multi-Table ?  (d.h. sind mehrere Tabellen in der Datei
      beschrieben? In diesem Fall muss die erste Spalte "table"
      lauten. Wenn nicht, wird das g_begin_record auf "" gesetzt. */
  if (!is_followed_by(header,"table",false)) 
    {
    CLEAR_TAG(context()->mode(),cMULTITABLE);
    m_recbegin.erase(); 
    }
  else
    SET_TAG(context()->mode(),cMULTITABLE);

  /** Header-Stream analysieren */
  ix_type index(0);
  string field;
  bool done(false);
  do 
    {
    read_white(header);
    read_word_break(header,m_fieldend.c_str(),field); ++index;
    is_followed_by(header,m_fieldend.c_str(),false);
    done = header.eof() || !header.good();
    // folgender test funktioniert nicht, wenn feldname praefix eines
    // anderen feldnamens ist....
    //if (is_followed_by(header,field.c_str()))  
    //  WARNING("duplicate field name <"<<field<<"> in file.");
    TRACE("field:"<<field<<", index:"<<index);

    // Eintrag speichern, doppelte beruecksichtigen (doppelt in dem
    // Sinne, dass verschiedene Felder auf dasselbe Feld in der
    // Tabelle abgebildet werden)

    // Suche den Eintrag in der Feldliste
    adtp_list::const_iterator f=m_fields.begin();
    while (f!=m_fields.end())
      {
      if ((*f)->context()->intag()==field)  
        {
	TRACE("field match"); 
        
        // Offset speichern, Eine Uebernahme der Felderindizes erfolgt
        // nur, wenn sie nicht bereits vorher definiert wurden. 
        if ((*f)->context()->offset()==g_illegal_ix) 
          (*f)->context()->offset()=index;

        // Positionen von key/parent merken fuer sneak()
        if ((*f)==mp_key) m_key_index = index;
        if ((*f)==mp_ref) m_ref_index = index;
        }
      ++f;
      }

    } 
  while (!done); 

  // Reihenfolge nach offset sortieren
  sort_fields_by_offset();

  return Reader::init();
  } // Ende der init Methode

void
CSV_Reader::sneak(istream& is)
{
    LOG_METHOD("Access","CSV_Reader","sneak",context()->name());

  int index = 1;
  int stop_index = max(m_key_index,m_ref_index);
  while ((is.good())&&(index<=stop_index))
    {
    read_white(is);
    bool empty = is_followed_by(is,m_fieldend.c_str(),true);
    if ((index==m_key_index)&&(!empty))
      { is >> m_next_key; } // BAUSTELLE: sollte m_io benutzen?
    else if ((index==m_ref_index)&&(!empty))
      { is >> m_next_ref; }
    read_until(is,m_fieldend.c_str());
    ++index;
    }
  }

void
CSV_Reader::load
  (
  istream& is
  )
  {
  LOG_METHOD("Access","CSV_Reader","load",context()->name());

  adtp_list::iterator g,f = m_fields.begin(); 
  ix_type index = 1;
  while ((is.good())&&(f!=m_fields.end()))
    {
    read_white(is);
    if (index==(*f)->context()->offset())
      { // relevantes Feld, auslesen

      // wieviele Felder erhalten ihren Wert aus dieser Eingabe?
      g=f; ++g;
      while (    (g!=m_fields.end())
              && ((*g)->context()->offset()==index)
            ) ++g;

      // wenn leer, auslesen der Felder ueberspringen
      bool empty = is_followed_by(is,m_fieldend.c_str(),true);
      if (empty) 
        f=g;
      else
        { // auslesen 
        streampos p = is.tellg();
        for (;f!=g;++f) 
          { 
          is.seekg(p); (*f)->read(is,mp_matsep); 
          TRACE_ID("readfield","read <"<<mp_context->name()<<">: "
                  <<(*f)->context()->name()<<':'<<(**f)); 
          }
        }
      }
    read_until(is,m_fieldend.c_str());
    ++index;
    }
  }

/** Klasse CSV_Writer *******************************************/

CSV_Writer::CSV_Writer
  (
  Context *ap_context,
  index_type *ap_output_key,
  index_type *ap_output_ref,
  index_type *ap_input_key,
  index_type *ap_required_key,
  ostream *ap_os
  )
  : Writer(ap_context,ap_output_key,ap_output_ref,ap_input_key,ap_required_key,ap_os)
  { 
  set_comma_separators(m_recbegin,m_recend,m_fieldbegin,m_fieldend);
  mp_matsep = &g_matrix_csv_separators;
  }

bool
CSV_Writer::init
  (
  ) 
  {
  LOG_METHOD("Access","CSV_Writer","init",context()->name());
  return (Writer::init()) && (mp_stream->good());
  }

void
CSV_Writer::write_header()
  {
  if (context()->is_multitable()) 
    (*mp_stream) << "table" << m_fieldend; 
  else 
    m_recbegin="";

  adtp_list::iterator f = m_fields.begin(); 
  bool first = true;
  while (f!=m_fields.end())
    {
    Context* p_context = (*f)->context();
    if (    (p_context->is_writeback())
            // keine Ausgabe wenn Table-Context=FLAT und Field-Context=KEYREF
         && ((!mp_context->is_flat())||(!p_context->is_keyref())) )
      {
      if (!first) (*mp_stream) << m_fieldend;
      first = false;

      (*mp_stream) << p_context->outtag();
      }
    ++f;
    }
  (*mp_stream) << endl;
  }

bool
CSV_Writer::operate
  (
  )
  {
  LOG_METHOD("Access","CSV_Writer","operate",context()->name());

  Writer::operate();
    
  if (m_next_loc==0) write_header();

  (*mp_stream) << m_recbegin;

  size_t pos = m_fieldbegin.find("%s"); 

  bool first = true;
  for (adtp_list::const_iterator f=m_fields.begin();f!=m_fields.end();++f)
    // keine Ausgabe wenn Table-Context=FLAT und Field-Context=KEYREF
    if ((!mp_context->is_flat())||(!(*f)->context()->is_keyref()))
    { 
    (*mp_stream) << m_fieldbegin.substr(0,pos);
    if (pos!=string::npos)
      (*mp_stream) << (*f)->context()->outtag()
                   << m_fieldbegin.substr(pos+2);

    if (!first) (*mp_stream) << m_fieldend; 
    first = false;

    if (!(*f)->is_default())
      (*f)->write(*mp_stream,mp_matsep); 
    TRACE_ID("writefield","write <"<<mp_context->name()<<">: "
             <<(*f)->context()->name()<<':'<<(**f)); 
    }

  (*mp_stream) << m_recend; mp_stream->flush();

  return mp_stream->good();
  }

/** Klasse TAB_Reader ******************************************/

TAB_Reader::TAB_Reader
  (
  Context *ap_context,
  index_type *ap_key,
  index_type *ap_ref,
  index_type *ap_required_key,
  index_type *ap_required_ref,
  istream *ap_is 
  )
  : Reader(ap_context,ap_key,ap_ref,ap_required_key,ap_required_ref,ap_is)
  , m_key_index(-2)
  , m_ref_index(-2)
  {
  set_table_separators(m_recbegin,m_recend,m_fieldbegin,m_fieldend);
  mp_matsep = &g_matrix_tab_separators;
  }

TAB_Reader::~TAB_Reader
  (
  )
  {
  }

bool
TAB_Reader::init
  (
  ) 
  {
  LOG_METHOD("Access","TAB_Reader","init",context()->name());

  // Gibt es ueberhaupt einen Header?
  read_white(*mp_stream);
  if (!is_followed_by(*mp_stream,g_char_comment)) 
    { 
    INFO("no labels in table header"); 
    if (context()->is_multitable()) m_recbegin="";
    }
  else
    {
    /** Kommentarzeilen einlesen, nur die letzte wird als Header
        ausgewertet. */
    string name;
    while (is_followed_by(*mp_stream,g_char_comment,false)) 
      { read_line(*mp_stream,name); read_white(*mp_stream); }
    istrstream header(name.c_str());
    TRACE("header=" << name);

    /** Multi-Table ?  (d.h. sind mehrere Tabellen in der Datei
        beschrieben? In diesem Fall muss die erste Spalte "table"
        lauten. Wenn nicht, wird das g_begin_record auf "" gesetzt. */
    if (!is_followed_by(header,"table",false)) 
      {
      CLEAR_TAG(context()->mode(),cMULTITABLE);
      m_recbegin.erase(); 
      }
    else
      SET_TAG(context()->mode(),cMULTITABLE);

    /** Header-Stream analysieren */
    ix_type index(1),start(1);
    string field;
    bool done(false);
    do 
      {
      read_word(header,field); ++index;
      done = header.eof() || !header.good();
      if (!done) 
        {
        while (is_followed_by(header,field.c_str(),false)) { ++index; }
        done = header.eof() || !header.good();
        }
      TRACE("field:"<<field<<", index:"<<start);
  
      // Eintrag speichern, doppelte beruecksichtigen (doppelt in dem
      // Sinne, dass verschiedene Felder auf dasselbe Feld in der
      // Tabelle abgebildet werden)

      adtp_list::const_iterator f=m_fields.begin();
      while (f!=m_fields.end())
        {
        if ((*f)->context()->intag()==field)
	  {
          /** Eine Uebernahme der Felderindizes erfolgt nur, wenn sie nicht
              bereits vorher definiert wurden. */
          TRACE("field match");
          if ((*f)->context()->offset()==g_illegal_ix) (*f)->context()->offset()=start;
          if ((*f)->context()->rows()==0)
            { //  wenn keine rows() angegeben, als Vektor interpretieren
            (*f)->context()->rows()=index-start; 
            (*f)->context()->cols()=1; 
            }
          else
            { // wenn rows() angegeben, pruefen, ob es von der Groesse "reinpasst"
            if ((*f)->context()->cols()==0) (*f)->context()->cols()=1;
            ix_type size = (*f)->context()->cols()*(*f)->context()->rows();
            if (size>index-start) 
              ERROR("field size "<<(*f)->context()->rows()<<'x'<<(*f)->context()->cols()
                  <<" exceeds actual size (="<<index-start<<") in table");
            }

          // Positionen von key/parent merken fuer sneak()
          if ((*f)==mp_key) m_key_index = start;
          if ((*f)==mp_ref) m_ref_index = start;
          }
	++f;
	}
      
      start=index;
      } 
    while (!done); 
    } // Ende der Header-Auswertung

  // Schliesslich sortieren wir m_fields so um, dass die Reihenfolge
  // durch die offsets gegeben ist (vereinfacht pop()).
  sort_fields_by_offset();

  return Reader::init();
  } // Ende der init Methode

void
TAB_Reader::sneak(istream& is)
{
    LOG_METHOD("Access","TAB_Reader","sneak",context()->name());

    int index = 1;
    int stop_index = max(m_key_index,m_ref_index);
    while ((is.good())&&(index<=stop_index))
    {
	read_white(is);
	if ((index==m_key_index)&&(!is_followed_by(is,g_char_emptyfield,false)))
	{ is >> m_next_key; } // BAUSTELLE: sollte m_io benutzen?
	else if ((index==m_ref_index)&&(!is_followed_by(is,g_char_emptyfield,false)))
	{ is >> m_next_ref; }
	read_until(is,m_fieldend.c_str());
	++index;
    }
}

void
TAB_Reader::load
  (
  istream& is
  )
  {
  LOG_METHOD("Access","TAB_Reader","load",context()->name());

  adtp_list::iterator g,f = m_fields.begin(); 
  ix_type index = 1;
  while ((is.good())&&(f!=m_fields.end()))
    {
    read_white(is);
    if (index==(*f)->context()->offset())
      { // relevantes Feld, auslesen

      // wieviele Felder erhalten ihren Wert aus dieser Eingabe?
      g=f; ++g;
      while (    (g!=m_fields.end())
              && ((*g)->context()->offset()==index)
            ) ++g;

      // wenn leer, auslesen der Felder ueberspringen
      bool empty = is_followed_by(is,g_char_emptyfield,false);
      if (empty) 
        f=g;
      else
        { // auslesen 
        streampos p = is.tellg(); 
        for (;f!=g;++f) 
          { 
          // vorgegebene Anzahl von Werten auslesen
          (*f)->read(is,(*f)->context()->rows(),mp_matsep); 
          TRACE_ID("readfield","read <"<<mp_context->name()<<">: "
                  <<(*f)->context()->name()<<':'<<(**f)); 
	  // wieder zurueck-positionieren fuer naechstes Feld bei
	  // dieser Position. Aber auch nach dem Lesen des letzten
	  // Feldes zurueckspringen: dann wird im anschliessenden
	  // read_until(fieldend) nur ein Feld gelesen und wir koennen
	  // ueberlappende Feldauslesungen realisieren
          is.seekg(p);
          }
        }
      }
    read_until(is,m_fieldend.c_str());
    ++index;
    }
  }

/** Klasse TAB_Writer *******************************************/

TAB_Writer::TAB_Writer
  (
  Context *ap_context,
  index_type *ap_output_key,
  index_type *ap_output_ref,
  index_type *ap_input_key,
  index_type *ap_required_key,
  ostream *ap_os
  )
  : Writer(ap_context,ap_output_key,ap_output_ref,ap_input_key,ap_required_key,ap_os)
  { 
  set_table_separators(m_recbegin,m_recend,m_fieldbegin,m_fieldend);
  mp_matsep = &g_matrix_tab_separators;
  }

bool
TAB_Writer::init
  (
  ) 
  {
  LOG_METHOD("Access","TAB_Writer","init",context()->name());
  return (Writer::init()) && (mp_stream->good());
  }

void
TAB_Writer::write_header()
  {
  (*mp_stream) << "# ";
  if (context()->is_multitable()) 
    (*mp_stream) << "table" << m_fieldend; 
  else 
    m_recbegin="";

  adtp_list::iterator f = m_fields.begin(); 
  while (f!=m_fields.end())
    {
    Context* p_context = (*f)->context();
    if (    (p_context->is_writeback())
            // keine Ausgabe wenn Table-Context=FLAT und Field-Context=KEYREF
         && ((!mp_context->is_flat())||(!p_context->is_keyref())) )
      {
      // bei der Ausgabe des ersten Wertes die Feldgroesse merken
      const int n = (*f)->size();
      p_context->rows()=n; p_context->cols()=1;
      for (int i=0;i<n;++i) (*mp_stream) << p_context->outtag() << m_fieldend;
      }
    ++f;
    }
  (*mp_stream) << endl;
  }

bool
TAB_Writer::operate
  (
  )
  {
  LOG_METHOD("Access","TAB_Writer","operate",context()->name());

    Writer::operate();
    
  if (m_next_loc==0) write_header();

  (*mp_stream) << m_recbegin;

  size_t pos = m_fieldbegin.find("%s"); 

  for (adtp_list::const_iterator f=m_fields.begin();f!=m_fields.end();++f)
    // keine Ausgabe wenn Table-Context=FLAT und Field-Context=KEYREF
    if ((!mp_context->is_flat())||(!(*f)->context()->is_keyref()))
    { 
    (*mp_stream) << m_fieldbegin.substr(0,pos);
    if (pos!=string::npos)
      (*mp_stream) << (*f)->context()->outtag()
                   << m_fieldbegin.substr(pos+2);

    // gleichbleibende Groesse garantieren, keine Default-*-Ausgabe bei Feldern
    if ((*f)->context()->type()==Context::FIELD)
    {
      if ((*f)->is_default()) (*mp_stream) << g_char_emptyfield;
      else (*f)->write(*mp_stream,mp_matsep); 
    }
    else
    {
      if ((*f)->context()->rows()!=(*f)->size())
        ERROR("varying size of field not allowed in table file format");
      (*f)->write(*mp_stream,mp_matsep); 
    }

    TRACE_ID("writefield","write <"<<mp_context->name()<<">: "
             <<(*f)->context()->name()<<':'<<(**f)); 
    (*mp_stream) << m_fieldend;
    }

  (*mp_stream) << m_recend; mp_stream->flush();

  return mp_stream->good();
  }

/** Klasse File ********************************************/

template <class T>
File<T>::File
  (
  Context *ap_context,
  index_type *ap_key,
  index_type *ap_ref,
  index_type *ap_required_key,
  index_type *ap_required_ref
  )
  : T(ap_context,ap_key,ap_ref,ap_required_key,ap_required_ref,new typename T::file_type)  
  {
  }

template <class T>
File<T>::~File
  (
  )
  {
  close_file_stream();
  delete mp_stream;
  }

template <class T>
bool
File<T>::init()
  {
  LOG_METHOD("Access","File<T>","init",context()->name());

  m_fname = name();
  open_file_stream(m_fname.c_str());
  if (!mp_stream->good()) 
    {
      /** Versuchen wir es im DATAPATH. */
      char *p_path( getenv("DAL_DATAPATH") );
      if (p_path!=NULL)
        {
        m_fname = p_path; m_fname += '/'; m_fname += name();
        open_file_stream(m_fname.c_str());
        }
    }
  if (!mp_stream->good())
    {
    ERROR("file '" << m_fname << "' not found"); exit(-1);
    return false; 
    }
  else
    {
    return T::init();
    }
  }

template <class T>
void
File<T>::setpos(const marker_type& mark)
{
    LOG_METHOD("Access","File<T>","reset",context()->name());
    
    // selbe Position, lookahead gelesen, aber noch nicht ausgewertet?
    bool sameposition = ( (m_curr_loc == mark.m_spos) && 
			  (m_ahead_loc == mark.m_spos) );
    
    // Scheusslich, aber manchmal passiert ein Beschaedigen des Stream
    // am Ende des Auslesens eines TABLEs, weil es kein eindeutiges
    // Endsymbol gibt (sondern man laeuft ins !good).
    if (!mp_stream->good())
    { 
	close_file_stream(); 
	open_file_stream(m_fname.c_str()); 
	mp_stream->clear(); 
    }
    
    // Repositionierung, aber nur wenn noetig (um konsistent mit
    // T::setpos zu bleiben)
    if (!sameposition) seek(mark.m_spos); 
    T::setpos(mark);

    INVARIANT(mp_stream->good(),"ok after seek");
}

template File<TAG_Reader>;
template File<TAG_Writer>;
template File<CSV_Reader>;
template File<CSV_Writer>;

#ifdef XXX
/** Klasse File_List<T> **************************************/

/** Theoretisch ist es moeglich, dass die Position der Felder von
    Datei zu Datei unterschiedlich ist. Diese Position wird in den
    cells gespeichert, daher koennen wir nur einen Reader fuer ein
    Feld initialisieren, andernfalls wuerden die Reader das
    Positionsfeld in der cell mehrfach ueberschreiben. */

    File_List(ix_type a_ix,const string& a_name);
    virtual ~File_List();
    virtual void init();
    virtual bool operate(const id_type&,field_list_type&);
    virtual void getpos(marker_type&);
    virtual void setpos(const marker_type&);

template <class T>
File_List<T>::File_List
  (
  Context *ap_context,
  index_type *ap_key,
  index_type *ap_ref,
  index_type *ap_required_key,
  index_type *ap_required_ref
  )
  : T(ap_context,ap_key,ap_ref,ap_required_key,ap_required_ref,new typename T::file_type)  
  , m_filename(this,cREADOUT|cSHARED,"file")
  , mp_reader(NULL)
  {

  /** Wir lesen die Daten gleich aus, damit wir vor- und zuruecklaufen
      koennen. Weil wir nur an einem String je Record interessiert
      ist, gehen wir davon aus, dass dies nicht viel Speicher
      kostet. */
  while (!done())
    {
    pop();
    m_filenames.push_back(m_filename);
    push();
    }
  close();
  mi_file = m_filenames.begin();
  }

template <class T>
File_List<T>::~File_List
  (
  )
  {
  m_filenames.clear();
  }

/** Attach-Kommandos werden nicht an diesen Reader gegeben, sondern
    fuer die noch zu erzeugenden Reader gepuffert. */

template <class T>
void
FileList<T>::attach
  (
  abstract_data_type *ap_field
  )
  {
  m_indirect_fields.push_back(ap_field);
  }

/** Nach dem Auslesen neue Datei oeffnen, wenn Dateiende erreicht
    wurde. */

template <class T> void
File_List<T>::operate
  (
  const id_type& a_collection_id,
  field_list_type& a_fields
  )
  {
  LOG_METHOD("Access","File_List<T>","operate",a_collection_id.no);

  if ((p_reader==NULL) || (m_delayed_reset)) // neue Datei?
    {
    // Filename suchen
    int n = 0; mi_file = m_filenames.begin();
    while (n!=m_delayed_pos.m_file) { ++n; ++mi_file; }

    if (n!=m_curr_file)
      { // neuen Reader oeffnen 
      if (mp_reader!=NULL) delete mp_reader;
      mp_context->infile() = *mi_file;
      mp_reader = get_reader(mp_context,...);
      bool ok = mp_reader->init(a_indirect_fields);
      }

    // Reader positionieren
    mp_reader->setpos(m_delayed_pos);

    m_delayed_reset=false;
    }

  if (!mp_reader->done())
    {
    mp_reader->operate(a_collection_id,a_fields);
    }
  else
    {
    bool fileok = true;
    while ((mi_file!=m_filenames.end()) && (fileok))
      {
      fileok=false;
      ++mi_file;
      ++m_curr_file; 
      if (mi_file!=m_filenames.end())
        {
        TRACE_VAR(*mi_file);
        delete mp_reader;
        mp_context->infile() = *mi_file;
        mp_reader = get_reader(mp_context,...);
        fileok = mp_reader->init(a_fields);
        } // if
      }  // while
    if (fileok) mp_reader->operate(a_collection_id,a_fields);
  }

/** Neupositionierung auf m_mark, wobei ggf. andere Datei als die
    aktuelle geoffnet werden muss. Fuer das init() der neuen Datei
    brauchen wir die field_list, die wir hier nicht haben. Muss eine
    neue Datei geoeffnet werden, so verlagern wir dies in den
    naechsten ::operate Aufruf (der bei rewind() auch immer erfolgt). */

template <class T> 
void
File_List<T>::reset
  (
  )
  {
    m_delayed_reset = (m_old.m_file_no != m_mark.m_file_no);
    if (!m_delayed_reset) T::reset(); else m_delayed_pos = m_mark;
  }

/** selbe Schwierigkeit wie bei reset(), ggf. muessen wir ein neues
    File oeffnen. */

template <class T>
void
File_List<T>::set_position
  (
  const marker_type &a_marker
  )
  {
    m_delayed_reset = (m_old.m_file_no != a_marker.m_file_no);
    if (!m_delayed_reset) T::set_position(a_marker); else m_delayed_pos=a_marker;
  }

template <class T> 
char_cpc
File_List<T>::name
  (
  )
  {
    return m_listname.c_str();
  }

template File_List<TAG_File_Mark_Reader>;
template File_List<CSV_File_Mark_Reader>;
#endif

const char* gp_console = "console";
const char* gp_nulldev = "null";

/** Create a new instance of a Reader class. The default Reader is a
    NULL_Reader (no input) */

Reader* 
get_reader(
    Context* ap_context,
    index_type *ap_key,
    index_type *ap_ref,
    index_type *ap_rkey,
    index_type *ap_rref
    )
{
    LOG_FUNCTION("Access","get_reader",ap_context->infile());

    string *p_name = &(ap_context->infile());
    Reader *p_reader;
    if (*p_name==gp_console)
    {
	*p_name = gp_console;
	p_reader = new TAG_Reader(ap_context,ap_key,ap_ref,ap_rkey,ap_rref,&cin);
    }
    else if (p_name->find(".ddl")!=string::npos)
    {
	p_reader = new TAG_File_Reader(ap_context,ap_key,ap_ref,ap_rkey,ap_rref);
    }
    else if (p_name->find(".csv")!=string::npos)
    {
	p_reader = new CSV_File_Reader(ap_context,ap_key,ap_ref,ap_rkey,ap_rref);
    }
    else if (p_name->find(".tab")!=string::npos)
    {
	p_reader = new TAB_File_Reader(ap_context,ap_key,ap_ref,ap_rkey,ap_rref);
    }
    else if ((p_name->empty())||(*p_name==gp_nulldev)) // null is default
    {
	p_reader = new NULL_Reader(ap_context,ap_key,ap_ref,ap_rkey,ap_rref);
    }
    else
    { 
	ERROR("unknown file type "<<*p_name); exit(-1); 
    }
    return p_reader;
}

/** Create a new instance of a Writer class. The default Writer is a
    TAG_Writer writing to console. */

Writer* 
get_writer(
    Context *ap_context,
    index_type* ap_okey,
    index_type* ap_oref,
    index_type* ap_ikey,
    index_type* ap_iref // oder required_key
    )
{
    LOG_FUNCTION("Access","get_writer",ap_context->outfile());
    
    string *p_name = &(ap_context->outfile());
    Writer *p_writer;
    if ((p_name->empty())||(*p_name==gp_console)) // console is default 
    {
	*p_name = gp_console;
	p_writer = new TAG_Writer(ap_context,ap_okey,ap_oref,ap_ikey,ap_iref,&cout);
    }
    else if (p_name->find(".ddl")!=string::npos)
    {
	p_writer = new TAG_File_Writer(ap_context,ap_okey,ap_oref,ap_ikey,ap_iref);
    }
    else if (p_name->find(".csv")!=string::npos)
    {
	p_writer = new CSV_File_Writer(ap_context,ap_okey,ap_oref,ap_ikey,ap_iref);
    }
    else if (p_name->find(".tab")!=string::npos)
    {
	p_writer = new TAB_File_Writer(ap_context,ap_okey,ap_oref,ap_ikey,ap_iref);
    }
    else if (*p_name==gp_nulldev)
    {
	p_writer = new NULL_Writer(ap_context,ap_okey,ap_oref,ap_ikey,ap_iref);
    }
    else
    { 
	ERROR("unknown file type "<<*p_name); exit(-1); 
    }
    return p_writer;
}


}; // namespace Data_Analysis_Library

#endif // ACCESS_SOURCE
