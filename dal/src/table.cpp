
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

/** \file table.cpp 
    \author Frank Hoeppner <frank.hoeppner@ieee.org>
    \brief Zugriff auf ASCII-Datenfiles. */

#ifndef TABLE_SOURCE
#define TABLE_SOURCE

#include <fstream>
#include "dal.hpp"
#include "table.hpp"
#include "access.hpp"
#include "globdef.hpp"

namespace Data_Access_Library {

///////////////////////////////////////////////////////////

/** \class record_type
    \brief Contains indices to all fields in a record.

    A record consists of several fields, which are attach()ed to a
    record_type. Each field is stored in its associated repository,
    therefore the whole record can be represented as a vector of
    indices (vector size is the number of fields). The record-vectors
    themselves can be stored in another repository, thus record_type
    is inherited from data_type< DynTuple<ix_type> >. 

    There is exactly on record_type for each BaseTable, only BaseTable
    creates instances of it. BaseTable and record_type therefore share
    the same context class.

    Key- and reference-fields are also stored in the record_type, but
    the creation of these fields will be done by the table
    classes. However, only the key-field and reference-field of the
    main reader will be stored, keys and references of sub reader will
    not. */


record_type::record_type(Context *ap_context)
    : data_type< DynTuple<ix_type> >(ap_context)
    , m_size(0)
{
}

record_type::~record_type()
{
}

/** Announce a field to the record. The field will be stored in \c
    m_fields and the vector size will increase by one. */

void 
record_type::attach(abstract_data_type *ap_field)
{
    LOG_METHOD("DAL","record_type","attach",ap_field);
    INVARIANT(  (!mp_context->is_initialized())
		||(ap_field->context()->is_keyref()),
		"once initialized, only keys should be attached");
    
    // store pointer to field in list
    m_fields.push_back(ap_field);
    ++m_size;
}

/** Determine whether the record (resp. associated table) is readonly
    or writeonly ny setting the tags READOUT and
    WRITEBACK. Afterwards, set the INITIALIZED tag. */

void
record_type::init()
{
    INVARIANT(!mp_context->is_initialized(),"");
    
    // Die READOUT und WRITEBACK Bits von den Feldern disjunktiv
    // uebernehmen, wobei cKEYREF Felder nicht beruecksichtigt werden
    // (weil die automatisch erzeugt wurden)
    int mask = Context::READOUT|Context::WRITEBACK;
    adtp_list::iterator f;
    for (f=m_fields.begin();f!=m_fields.end();++f)
    {
	INVARIANT((*f)->context()->is_field(),"record contains fields only");
	if (IS_NO_TAG((*f)->context()->mode(),cKEYREF))
	    SET_TAG(mp_context->mode(),
		    mp_context->mode()|((*f)->context()->mode()&mask));
    }
    
    SET_TAG(mp_context->mode(),Context::INITIALIZED);
}

/** Load default value for all fields. The field values will not be
    deleted from the repository! The record_type, which is itself a
    data_type, is also set to the default value (zero-dimensional
    vector), which indicates that all of its fields have default
    values. */

void 
record_type::load_default()
{
    data_type< DynTuple<ix_type> >::load_default();
    for (adtp_list::const_iterator f=m_fields.begin();f!=m_fields.end();++f)
    { (*f)->load_default(); }
}

/** Load a record. The \a a_ix index is the index in the record_type
    repository. All fields of the records will also be loaded. */

void
record_type::load(ix_type a_ix)
{
    LOG_METHOD("Table","record_type","load",a_ix);

    data_type< DynTuple<ix_type> >::load(a_ix);
    // If a_ix is the default value, all fields must be set to default
    // values, too.
    if (val().rows()==0) 
	load_default();
    else
    {
	INVARIANT(val().rows()==m_size,"record size unchanged");
	adtp_list::const_iterator f; int i;
	for (f=m_fields.begin(),i=0;f!=m_fields.end();++f,++i)
	{ 
	    (*f)->load( val()[i] ); 
	}
    }

    TRACE_VAR(val());
}

/** Copy a record. The \a a_ix index is the index in the record_type
    repository that shall be copied. New entries in the repositories
    are allocated for all non-default fields. */

void
record_type::copy(ix_type a_ix)
{
    LOG_METHOD("Table","record_type","copy",context()->name()<<'@'<<a_ix);

    // this implicitly creates a new entry in the repository, because
    // we have loaded the default values before
    data_type< DynTuple<ix_type> >::load_default(); 
    var().alloc(m_size);

    if (m_repository[a_ix].rows()==0) 
    { var().alloc(0); return; } // bei default nix kopieren

    adtp_list::const_iterator f; int i;
    for (f=m_fields.begin(),i=0;f!=m_fields.end();++f,++i)
    { 
	// the default value shall not be copied [note that all fields
	// are copies, not only those marked as WRITEBACK, WRITEBACK
	// refers to the final output only]
	if ((*f)->get_default_ix() != m_repository[a_ix][i])  
	{
	    (*f)->copy( m_repository[a_ix][i] ); 
	    var()[i] = (*f)->get_ix();
	    (*f)->info(*gp_trace);
	}
	else
	{
	    var()[i] = (*f)->get_default_ix();
	}
    }

    TRACE_VAR(val());
}

/** Update a record. When a record has been instantiated, some of its
    fields may have default values. In the meantime, they may have
    different values and thus there are new repository-indices
    associated with these fields. Thus, to keep the reference up to
    date, we have to reload the vector from the field indices. */

void
record_type::store()
{
    LOG_METHOD("Table","record_type","store",context()->name()<<'@'<<m_ix);

    adtp_list::const_iterator f; int i;
    var().alloc(m_size);
    for (f=m_fields.begin(),i=0;f!=m_fields.end();++f,++i)
    {
	var()[i] = (*f)->get_ix(); 
	(*f)->info(*gp_trace);
    }

    TRACE_VAR(val());
}

/** Remove record and fields from repository (thereby setting all
    fields to default values). */

void
record_type::free()
{
    adtp_list::const_iterator f;
    for (f=m_fields.begin();f!=m_fields.end();++f)
    { (*f)->free(); }
    data_type< DynTuple<ix_type> >::free();
}

/** Remove record \a a_ix and its fields from repository (thereby
    setting all fields to default values). */

void
record_type::free(ix_type a_ix)
{
    // Default-Collection kann man nicht freigeben
    if (m_repository[a_ix].rows()==0) return;

    INVARIANT(m_repository[a_ix].rows()==m_size,"size match "
	      <<m_repository[a_ix].rows()<<'='<<m_size);
    adtp_list::const_iterator f; int i;
    for (f=m_fields.begin(),i=0;f!=m_fields.end();++f,++i)
    {
	(*f)->free( m_repository[a_ix][i] );
    }
    data_type< DynTuple<ix_type> >::free(a_ix);
}

/** Record output for debugging purposes. */

void
record_type::info(ostream &os) 
    const
{
    for (adtp_list::const_iterator f=m_fields.begin();f!=m_fields.end();++f)
    { (*f)->info(os); }
}

//////////////////////////////////////////////////////////////////

/** \class BaseTable
    \brief Base class for data table access.

    BaseTable and derived table classes simplify access to data stored
    in ASCII files of various formats. The file handling is actually
    done by several Reader/Writer classes inherited from IOBase. Field
    such as int_type, real_type, etc. can be attached to tables and
    the fields are loaded with the respective record values by calling
    pop(). The (modified) fields can be written into a (different)
    output file via push(). */

BaseTable::BaseTable(
    int mode,
    const string& name,
    Domain *d=NULL
    )
    : mp_record(new record_type(new Context(Context::SETORRECORD,mode,name,name,name,d)))
    , mp_input_key(NULL)
    , mp_input_ref(NULL)
    , mp_output_key(NULL)
    , mp_output_ref(NULL)
{
    // ALLE Tabellen (auch Detailtabellen) werden in gp_dm_tables
    // gespeichert.
    gp_dm_tables->add(name,(int)this);
    
    // mp_input/output_key/ref fuer main reader/writer setzen (werden
    // in get_main_reader/writer spaeter ggf. noch modifiziert, aber
    // werden hier schon instantiiert, um Zugriff ueber
    // evaluate_command_line zu ermoeglichen). Ob es sub-reader/writer
    // geben wird, wissen wir hier noch nicht...
    mp_input_key = new index_type(NULL,cKEYREF|cREADOUT,
				  "inkey","key","origkey",context()->domain());
    if (!context()->is_subsetout())
	SET_TAG(mp_input_key->context()->mode(),cWRITEBACK);

    mp_output_key = new index_type(NULL,cKEYREF|cWRITEBACK,
				   "outkey","key","key",context()->domain());
    
    // globales Feld, Anmeldung im record_type
    attach(mp_input_key);
    attach(mp_output_key);
}

/** Destruktor. */

BaseTable::~BaseTable()
{
    delete mp_record;
    //delete mp_record->mp_context; <- wird vom array_type<> Destruktor gemacht
    if (mp_input_key!=NULL) { delete mp_input_key; mp_input_key=NULL; }
    if (mp_input_ref!=NULL) { delete mp_input_ref; mp_input_ref=NULL; }
    if (mp_output_key!=NULL) { delete mp_output_key; mp_output_key=NULL; }
    if (mp_output_ref!=NULL) { delete mp_output_ref; mp_output_ref=NULL; }
}

/** Initialisierung. */

void
BaseTable::init()
{
    LOG_METHOD("Table","BaseTable","init",'<'<<context()->name()<<'>');
    
    mp_record->init();

    // Nur Felder attached?
    adtp_list::const_iterator f;
    for (f=mp_record->m_fields.begin();f!=mp_record->m_fields.end();++f)
    { INVARIANT((*f)->context()->is_field(),""); }

    init_reader();
    init_writer();
}

/** Tabellen-Context. */

inline Context const * 
BaseTable::context() const
  { return mp_record->context(); }

inline Context* 
BaseTable::context() 
  { return mp_record->context(); }

/** Feld-Anmeldung. Ein Feld wird beim record_type angemeldet, die
    Anmeldung bzw. Verteilung auf die Reader/Writer erfolgt erst bei
    ::init. */

void
BaseTable::attach
  (
  abstract_data_type *ap_value
  ) 
  { 
  mp_record->attach(ap_value); 
  }

/** Suche nach einem Feld in einer Tabelle. Zugriff auf Felder ueber
    ihren Feldnamen (langsam, lineare Suche). */

abstract_data_type* 
BaseTable::get_field
  (
  const string& name
  )
  const
  {
  adtp_list::const_iterator f=mp_record->m_fields.begin();
  while ((f!=mp_record->m_fields.end())&&((*f)->context()->name()!=name)) ++f;
  return (f==mp_record->m_fields.end())?NULL:(*f);
  }

/** Ausgabe des Records fuer Debugging-Zwecke. */

void
BaseTable::info(ostream &os) 
    const
{
    os << "<table>" << (*context());
    if (!mp_record->is_default()) os << "<value " << mp_record->operator()() << ">";
    os << "</table>" << endl;
    mp_record->info(os);
}

/** Freigeben des aktuellen Records. Wird nach dem Schreiben
    aufgerufen und loescht alle Felder. */

void
BaseTable::free
  (
  )
  {
  LOG_METHOD("Table","BaseTable","free",'<'<<context()->name()<<'>')

  mp_record->free();
  }

/** Charakterisierung der Tabellen

    MasterTable: (im Sinne von: "keine Detailtabelle")
    - keine REFs
    - alle KEYs aus derselben Domain
    - alle INPUT KEYs muessen in den subreadern gleich dem MAIN INPUT KEY sein
    - OUTPUT KEYs sind mglw. ungleich INPUT KEYs (wenn !SUBSETOUT), in diesem Fall geben wir die INPUT KEYs im Mainreader mit aus
    - alle OUPUT KEYs muessen in den subwritern gleich dem MAIN OUTPUT KEY sein
*/

Reader*
BaseTable::get_main_reader()
{
    // mp_input_key/ref und mp_output_key/ref wurden im BaseTable-
    // Konstruktor angelegt (damit man ueber evaluate_command_line auf
    // diese Felder bereits Zugriff hat).
    
    // Reader erzeugen  
    return get_reader(context(),mp_input_key,mp_input_ref,NULL,NULL);
}

Writer*
BaseTable::get_main_writer()
{
    // mp_input_key/ref und mp_output_key/ref wurden im BaseTable-
    // Konstruktor angelegt (damit man ueber evaluate_command_line auf
    // diese Felder bereits Zugriff hat).

    // Writer erzeugen    
    return get_writer(context(),mp_output_key,mp_output_ref,mp_input_key,/*req_key:*/NULL);
}

Reader*
BaseTable::get_sub_reader(Context *p)
{
    // key/ref Felder von Subreadern werden hier erst erzeugt, weil
    // man anfangs noch nicht weiss, aus wieviel verschiedenen Dateien
    // die Daten gelesen werden. Folglich gibt es keinen Zugriff auf
    // diese Felder ueber die Kommandozeile
    index_type *p_sub_key = new index_type(NULL,cKEYREF|cREADOUT,"inkey","key","key",p->domain());
    
    return get_reader(p,p_sub_key,/*p_sub_ref:*/NULL,/*req_key:*/mp_input_key,/*req_ref:*/NULL);
}

Writer*
BaseTable::get_sub_writer()
{
    index_type *p_sub_key = new index_type(NULL,cKEYREF|cWRITEBACK,"outkey","key","key",context()->domain());
    return get_writer(context(),p_sub_key,/*p_sub_ref:*/NULL,/*input_key:*/mp_input_key,/*req_key:*/mp_input_key);
}

///////////////////////////////////////////////////////////////////////
// SimpleTable

/** Construktor. */

SimpleTable::SimpleTable
  (
  int mode,
  const string& name,
  Domain *d=NULL
  )
  : BaseTable(mode,name,d)
  , mp_reader(NULL)
  , mp_writer(NULL)
  {
  }

/** Destruktor. */

SimpleTable::~SimpleTable()
{
    close();
}

void SimpleTable::init_reader()
{
    adtp_list::const_iterator f;
    if (context()->is_readout())
    {
	mp_reader = get_main_reader(); 
	for (f=mp_record->m_fields.begin();f!=mp_record->m_fields.end();++f) 
	    if ((*f)->is_readout()) mp_reader->attach(*f);
	if (!mp_reader->init()) SET_TAG(context()->mode(),Context::ENDOFDATA);
	mp_reader->getpos(m_rstart);
    }
}

void SimpleTable::init_writer()
{
    adtp_list::const_iterator f;
    if (context()->is_writeback())
    {
	mp_writer = get_main_writer();
	for (f=mp_record->m_fields.begin();f!=mp_record->m_fields.end();++f)
	    if ((*f)->is_writeback()) mp_writer->attach(*f);
	mp_writer->init();
	mp_writer->getpos(m_wstart);
    }
}

/** Anfang der Eingabe setzen. */

void SimpleTable::mark()
{
    LOG_METHOD("Table","BaseTable","mark",'<'<<context()->name()<<'>');
	
    if (!context()->is_initialized()) init();
    if (!context()->is_readout()) return;
    if (mp_reader!=NULL) mp_reader->getpos(m_rstart);
}

/** Zurueckspulen der Eingabe. */

void SimpleTable::rewind()
{
    LOG_METHOD("Table","BaseTable","rewind",'<'<<context()->name()<<'>');
	
    if (!context()->is_initialized()) init();
    if (!context()->is_readout()) return;
    
    free();
    
    CLEAR_TAG(context()->mode(),Context::ENDOFDATA);
    if (mp_reader!=NULL) mp_reader->setpos(m_rstart);
}

/** Neubeginn der Ausgabe. */

void SimpleTable::clear()
{
    LOG_METHOD("Table","BaseTable","rewind",'<'<<context()->name()<<'>');
    
    if (!context()->is_initialized()) init();
    
    if (mp_writer!=NULL) mp_writer->setpos(m_wstart);
}


/** Schreiben des aktuellen Records in den Ausgabestrom. Wenn REF =
    g_illegal_ix dann wird */

void
SimpleTable::push()
{
    if (!context()->is_initialized()) init();
    if (!context()->is_writeback()) return;
    
    // Schreiben
    mp_writer->operate();
    
    // Alten Werte sind nun ungueltig und werden freigegeben.
// warum? free() in pop()!!    free();
}

/** Lesen eines neuen aktuellen Records. 

    Die neuen Werte werden aus den Eingabestroemen ausgelesen (alle
    Reader werden aufgerufen). Auch das Index-Tupel des Records wird
    gesetzt.

    Der Rueckgabewert ist \c true, wenn neue Daten gelesen
    wurden. Sind wir am Ende der Daten angekommen, konnten also keine
    neuen Daten gelesen werden, wird \c false zurueckgegeben.  */

bool
SimpleTable::pop()
{
    LOG_METHOD("Table","BaseTable","pop",'<'<<context()->name()<<'>');
	
    if (!context()->is_initialized()) init(); 
    free(); // Alte Werte freigegeben, falls noch nicht geschehen
    if (done()) return false;
    
    // Reader aufrufen
    if (!mp_reader->operate()) SET_TAG(context()->mode(),Context::ENDOFDATA);
    
    return true;
}

/** Liefert true wenn Tabellenende erreicht. Im Fall einer
    Detailtabelle bezieht sich das Tabellenende darauf, dass es keinen
    weiteren Eintrag zum aktuellen Masterschluessel gibt. */

bool
SimpleTable::done
  (
  )
  {
  if (!context()->is_initialized()) init();

  return ((!context()->is_readout())||(context()->is_endofdata()));
  }

/** Beenden */

void
SimpleTable::close
  (
  )
  {
  CLEAR_TAG(context()->mode(),cINITIALIZED|cENDOFDATA);

  if (mp_reader!=NULL) { delete mp_reader; mp_reader=NULL; }
  if (mp_writer!=NULL) { delete mp_writer; mp_writer=NULL; }
  }

void 
SimpleTable::setrpos(const marker_type& m) 
{
  CLEAR_TAG(context()->mode(),Context::ENDOFDATA);
  if (mp_reader!=NULL) mp_reader->setpos(m); 
}

void 
SimpleTable::getrpos(marker_type& m) 
{ 
  if (mp_reader!=NULL) mp_reader->getpos(m); 
}

void 
SimpleTable::setwpos(const marker_type& m) 
{ 
  if (mp_writer!=NULL) mp_writer->setpos(m); 
}

void 
SimpleTable::getwpos(marker_type& m) 
{ 
  if (mp_writer!=NULL) mp_writer->getpos(m); 
}

//////////////////////////////////////////////////////////////
// SyncTable

SyncTable::SyncTable(
    int mode,
    const string& name,
    Domain *d=NULL
    )
    : BaseTable(mode,name,d)
    , mp_reader(new list<Reader*>)
    , mp_writer(new list<Writer*>)
{}

SyncTable::~SyncTable()
{
    close();
}

void 
SyncTable::init_reader()
{
    LOG_METHOD("Table","SyncTable","init_reader",'<'<<context()->name()<<'>');
    
    adtp_list::const_iterator f;
    list<Reader*>::iterator r;
    Reader *p_rmain;
    int i;
    
    if (context()->is_readout())
    {
	// Main-Reader erstellen (keine Bedingungen an die gefundenen Records)
	p_rmain = get_main_reader(); mp_reader->push_back(p_rmain);
	// Sub-Reader erstellen und Felder attachen
	for (f=mp_record->m_fields.begin();f!=mp_record->m_fields.end();++f) 
	    if ((*f)->is_readout())
	    {
		r=mp_reader->begin();
		if (!(*f)->context()->infile().empty())
		{ 
		    // wenn Dateiname angegeben, dann den richtigen Reader suchen/erstellen
		    while ((r!=mp_reader->end())&&((*r)->name()!=(*f)->context()->infile())) ++r;
		    if (r==mp_reader->end()) // Bedingung an gefundene Records: gleicher key wie rmain
		    {
			// neuen Datei- & Collectionnamen setzen
			Context *p = new Context(*(*f)->context());
			p->intag() = p->outtag() = (*f)->context()->coll();
			r = mp_reader->insert(mp_reader->end(),get_sub_reader(p));
		    }
		}
		(*r)->attach(*f);
	    }
	// Wenn Main-Reader schon EOF meldet, dann war es das (keine Daten vorhanden)
	if (!p_rmain->init()) SET_TAG(context()->mode(),Context::ENDOFDATA);
	// restliche Reader initialisieren
	m_rstart.adjust(mp_reader->size()); p_rmain->getpos(m_rstart[0]);
	for (i=1,r=mp_reader->begin(),++r;r!=mp_reader->end();++r,++i) 
	{ (*r)->init(); (*r)->getpos(m_rstart[i]); }
    }
    m_rsize = mp_reader->size();
}

void 
SyncTable::init_writer()
{
    LOG_METHOD("Table","SyncTable","init_reader",'<'<<context()->name()<<'>');
    adtp_list::const_iterator f;
    list<Writer*>::iterator w;
    Writer *p_wmain;
    int i;
    
    if (context()->is_writeback())
    {
	// Main-Writer erstellen (Bedingung: KEY groesser gleich dem des Main-Readers)
	p_wmain = get_main_writer(); mp_writer->push_back(p_wmain);
	// Sub-Writer erstellen und Felder attachen
	for (f=mp_record->m_fields.begin();f!=mp_record->m_fields.end();++f)
	    if ((*f)->is_writeback())
        {
	    w=mp_writer->begin();
	    if (!(*f)->context()->outfile().empty())
	    { // wenn Dateiname angegeben, dann den richtigen Writer suchen/erstellen
		while ((w!=mp_writer->end())&&((*w)->name()!=(*f)->context()->outfile())) ++w;
		if (w==mp_writer->end()) // Bedingung an geschriebene Records: gleicher key wie wmain
		    w = mp_writer->insert(mp_writer->end(),get_sub_writer());
	    }
	    (*w)->attach(*f);
	}
	// Initialisieren
	m_wstart.adjust(mp_writer->size()); 
	p_wmain->init(); p_wmain->getpos(m_wstart[0]);
	for (i=1,w=mp_writer->begin(),++w;w!=mp_writer->end();++w,++i) 
	{ (*w)->init(); (*w)->getpos(m_wstart[i]); }
    }
    m_wsize = mp_writer->size();
}

/** Setzen des Eingabe-Beginns. */

void
SyncTable::mark()
{
    LOG_METHOD("Table","SyncTable","mark",'<'<<context()->name()<<'>');
	
    if (!context()->is_initialized()) init();
    if (!context()->is_readout()) return;

    list<Reader*>::iterator r;
    int i;
    for (i=0,r=mp_reader->begin();r!=mp_reader->end();++r,++i)
    { (*r)->getpos(m_rstart[i]); }
}

/** Zuruecksetzen der Eingabe. */

void
SyncTable::rewind()
{
    LOG_METHOD("Table","BaseTable","rewind",'<'<<context()->name()<<'>');
	
    if (!context()->is_initialized()) init();
    if (!context()->is_readout()) return;
    
    free();
    
    CLEAR_TAG(context()->mode(),Context::ENDOFDATA);
    int i;
    
    list<Reader*>::iterator r;
    for (i=0,r=mp_reader->begin();r!=mp_reader->end();++r,++i)
    { (*r)->setpos(m_rstart[i]); }
    
//  list<Writer*>::iterator w;
//  for (i=0,w=mp_writer->begin();w!=mp_writer->end();++w,++i)
//    { (*w)->setpos(m_wstart[i]); }
}

/** Neubeginn der Ausgabe. */

void
SyncTable::clear()
{
    LOG_METHOD("Table","BaseTable","clear",'<'<<context()->name()<<'>');
	
    if (!context()->is_initialized()) init();
    
    int i;
    list<Writer*>::iterator w;
    for (i=0,w=mp_writer->begin();w!=mp_writer->end();++w,++i)
    { (*w)->setpos(m_wstart[i]); }
}

/** Schreiben des aktuellen Records in den Ausgabestrom. Wenn REF =
    g_illegal_ix dann wird */

void
SyncTable::push()
{
    LOG_METHOD("Table","BaseTable","push",'<'<<context()->name()<<'>');
	
    if (!context()->is_initialized()) init();
    if (!context()->is_writeback()) return;
    
    // Ausgabe
    list<Writer*>::iterator w;
    for (w=mp_writer->begin();w!=mp_writer->end();++w) 
    { (*w)->operate(); }
    
    // Alten Werte sind nun ungueltig und werden freigegeben.
//    free();
}

/** Lesen eines neuen aktuellen Records. Die neuen Werte werden aus
    den Eingabestroemen ausgelesen (alle Reader werden
    aufgerufen). Auch das Index-Tupel des Records wird gesetzt. */

bool
SyncTable::pop()
{
    LOG_METHOD("Table","BaseTable","pop",'<'<<context()->name()<<'>');
    
    if (!context()->is_initialized()) init(); 
    free(); // Alte Werte freigegeben, falls noch nicht geschehen
    if (done()) return false;
    
    // Alle Reader aufrufen, der Mainreader bestimmt, wann wir fertig sind
    {
	list<Reader*>::iterator r=mp_reader->begin();
	bool ok = (*r)->operate();
	++r; for (;r!=mp_reader->end();++r) (*r)->operate();
	if (!ok) SET_TAG(context()->mode(),Context::ENDOFDATA);
    }
    
    return true;
}

/** Liefert true wenn Tabellenende erreicht. Im Fall einer
    Detailtabelle bezieht sich das Tabellenende darauf, dass es keinen
    weiteren Eintrag zum aktuellen Masterschluessel gibt. */

bool
SyncTable::done()
{
    if (!context()->is_initialized()) init();
    
    return ((!context()->is_readout())||(context()->is_endofdata()));
}

/** Beenden */

void
SyncTable::close()
{
    CLEAR_TAG(context()->mode(),cINITIALIZED|cENDOFDATA);
    
    for (list<Reader*>::iterator r=mp_reader->begin();r!=mp_reader->end();++r)
    { delete (*r); }
    mp_reader->clear();
    
    for (list<Writer*>::iterator w=mp_writer->begin();w!=mp_writer->end();++w)
    { delete (*w); }
    mp_writer->clear();
}


////////////////////////////////////////////////////////////////////
// Master Table


/** Construktor. */

MasterTable::MasterTable
  (
  int mode,
  const string& name,
  Domain *d
  )
  : SyncTable(mode,name,d)
  {
  SET_TAG(context()->mode(),cMASTER);
  }

/** Destruktor. */

MasterTable::~MasterTable()
{
    close();
}

////////////////////////////////////////////////////////////////////
// Hierarchical Master Table

/** \class HMasterTable

    Zugriff auf hierarchisch organisierte Records in einer ASCII
    Tabelle. Die Hierarchie wird ueber \c <key> und \c <ref> Felder
    kodiert: Ein Record A ist einem Record B untergeordnet, wenn das
    \c <ref> Feld von A gleich dem \c <key> Feld von B ist. Es sind
    nur Abhaengigkeiten zu vorangegangenen Records erlaubt.

    Die Hierarchie laesst sich als Baum auffassen, der als Knoten
    Tupel mit \c <key> Werten enthaelt. Die Wurzel des Baumes ist ein
    0-Tupel ohne \c <key>. Soehne erster Ordnung (die normalen
    Eintraege in einer Tabelle) haben eine 1-Tupel als Baumschluessel
    (ihr eigene \c <key> Wert). Soehne zweiter Ordnung werden durch
    ein Paar (2-Tupel) aus \c <key> des Vaters und eigenem \c <key>
    identifiziert.

    Wird fuer einen neuen Record kein \c <ref> Feld vorgefunden, so
    handelt es sich um einen normalen Record (Sohn der
    Wurzel). Andernfalls kann im n-Tupel des Vorgaengerrecords von
    rechts (hohe Tupelindices) nach links (kleine Tupelindices)
    gesucht werden, bis der Wert mit dem aktuellen \c <ref> Wert
    uebereinstimmt. Auf diese Weise wird dann eindeutig der
    Vorgaengerrecord identifziert. Dabei wird immer so ein Index
    gefunden, weil nur Abhaengigkeiten zu vorangegangenen Records
    erlaubt sind. Bei dieser Vorgehensweise ist ein einmaliges
    sequentielles Einlesen der Datei (siehe Annahme [SEQ]) zur
    Rekonstruktion der Hierarchie ausreichend. 

    Diese n-Tupel als Schluessel in der Hierarchie sind jedoch fuer
    die Kommandozeilen-Spezifikation nicht praktisch, weil der
    Benutzer die genauen Werte der \c <key> Felder kennen muss, um
    ueber ein Schluessel-Template \c (17,*) eine Teilmenge auswaehlen
    zu koennen. Darum wird neben den n-Tupeln mit absoluten Referenzen
    ein n-Tupel mit relativen Referenzen mitgefuehrt (Attribut
    m_relkey). Dann geben die Zahlen im letzten Tupelelement also an,
    um den wievielten Sohn des Vorgaengerrecords es sich handelt. */

/** Konstruktor. */

HMasterTable::HMasterTable(
    int mode,
    const string& name,
    Domain *d
    )
    : SyncTable(mode,name,d)
    , m_level_change(0)
    , m_ilevel_change(0)
{
    SET_TAG(context()->mode(),cMASTER|Context::HIERTABLE);

    // Hier erstmalig REF Felder instantiieren
    mp_input_ref = new index_type(NULL,cKEYREF|cREADOUT,
				  "inref","ref","origref",context()->domain());

    mp_output_ref = new index_type(NULL,cKEYREF|cWRITEBACK,
				   "outref","ref","ref",context()->domain());

    attach(mp_input_ref);   
    attach(mp_output_ref);
}

/** Destruktor. */

HMasterTable::~HMasterTable()
{
    close();
}

/** Erst nach der Initialisierung, wenn die Reader/Writer instanziiert
    wurden, koennen wir die m_rhier Variable setzen... */

void
HMasterTable::init()
{
    SyncTable::init();
    m_rhier = (mp_input_ref!=NULL) && 
	(mp_input_key->context()->domain()==mp_input_ref->context()->domain());
    m_whier = m_rhier; /// kann auch durch inc/dec_level gesetzt werden
}

bool 
HMasterTable::match(
    const int_matrix_type::value_type& hmask
    )
    const
{
    LOG_METHOD("Table","HMasterTable","match",hmask);

    bool match = hmask.rows() <= rdepth();
    if (!match) return false;
    for (int i=0;i<hmask.rows();++i)
//	match &= (hmask(i)==g_illegal_ix) || (hmask(i)==rkey(i)); // absolut
	match &= (hmask(i)==g_illegal_ix) || (hmask(i)==m_relkey[i]); // relativ

    TRACE_VAR(m_inkey);
    TRACE_VAR(match);

    return match;
}

bool 
HMasterTable::match_next(
    const int_matrix_type::value_type& hmask
    )
    const
{
    LOG_METHOD("Table","HMasterTable","match_next",hmask);

    ix_type nextref = (*mp_reader->begin())->nextref();
    ix_type nextkey = (*mp_reader->begin())->nextkey();

    int depth = rdepth(),olddepth = depth;
    // Hierarchie vorhanden?
    if (m_rhier)
    { while ((depth>0)&&(rkey(depth-1)!=nextref)) --depth; }
    else // keine Hierarchie
    { depth = 0; }
    ++depth;

    bool match = hmask.rows() <= depth;
    if (!match) return false;
    for (int i=0;i<hmask.rows();++i)
//	match &= (hmask(i)==g_illegal_ix) // absolut
//	    ||   ((i==depth-1)?(hmask(i)==nextkey):(hmask(i)==rkey(i))) ;
	if (hmask(i)!=g_illegal_ix)
	    if (i<olddepth) 
		match &= (hmask(i)==m_relkey[i]);
	    else // neuer Eintrag, kein m_relkey vorhanden
		match &= (hmask(i)==1);

    TRACE_VAR(m_inkey);
    TRACE_VAR(m_relkey);
    TRACE_VAR(match);

    return match;
}

/** Charakterisierung der Tabellen

    HMasterTabelle:
    - es gibt REFs
    - alle KEYs und REFs aus derselben Domain (REFs sind Selbstverweise)
    - alle INPUT KEYs muessen in den subreadern gleich dem MAIN INPUT KEY sein
    - INPUT REFs in den subreadern werden ignoriert, die Hierarchie wird durch mainreader bestimmt
    - OUTPUT KEYs sind mglw. ungleich INPUT KEYs (wenn !SUBSETOUT)
    - alle OUPUT KEYs muessen in den subwritern gleich dem MAIN OUTPUT KEY sein
    - OUTPUT REFs in den subwritern werden ignoriert

*/

Reader* HMasterTable::get_main_reader()
{
    // mp_input_key/ref und mp_output_key/ref wurden im BaseTable-
    // Konstruktor angelegt (damit man ueber evaluate_command_line auf
    // diese Felder bereits Zugriff hat).

    // Reader erzeugen
    return get_reader(context(),mp_input_key,mp_input_ref,NULL,NULL);
}

/** benutzt mp_input_key, muss also nach get_main_reader aufgerufen werden! */

Writer*
HMasterTable::get_main_writer()
{
    // mp_input_key/ref und mp_output_key/ref wurden im BaseTable-
    // Konstruktor angelegt (damit man ueber evaluate_command_line auf
    // diese Felder bereits Zugriff hat).

    // Writer erzeugen    
    return get_writer(context(),mp_output_key,mp_output_ref,mp_input_key,/*req_key:*/NULL);
}

Reader*
HMasterTable::get_sub_reader(Context *p)
{
    index_type *p_sub_key = new index_type(NULL,cKEYREF|cREADOUT,"inkey","key","key",p->domain());
    return get_reader(p,p_sub_key,/*p_sub_ref:*/NULL,/*req_key:*/mp_input_key,/*req_ref:*/NULL);
}

Writer*
HMasterTable::get_sub_writer()
{
    index_type *p_sub_key = new index_type(NULL,cKEYREF|cWRITEBACK,"outkey","key","key",context()->domain());
    return get_writer(context(),p_sub_key,/*p_sub_ref:*/NULL,/*input_key:*/mp_input_key,/*req_key:*/mp_input_key);
}


bool
HMasterTable::pop()
{
    LOG_METHOD("Table","HMasterTable","pop","");

    bool ok = SyncTable::pop();

    // Pfad mitfuehren
    int depth = rdepth(),olddepth=m_inkey.rows();
    // Hierarchie vorhanden?
    if (m_rhier)
    { while ((depth>0)&&(rkey(depth-1)!=mp_input_ref->val())) --depth; }
    else // keine Hierarchie
    { depth = 0; }
    m_inkey.adjust(depth+1); rkey(depth)=mp_input_key->val();
    TRACE_VAR(m_inkey);

    // relativen Schluessel anpassen
    m_relkey.adjust(depth+1); 
    if (depth+1>olddepth) m_relkey[depth]=1; else ++m_relkey[depth];
    TRACE_VAR(m_relkey);

    // Hierarchie in Eingabe auf Ausgabe abbilden. += und nicht =
    // falls bis zum naechsten push() mehrere pop() liegen. Birgt aber
    // die Gefahr, dass wir einen level-increase>1 erhalten...
    m_ilevel_change += m_inkey.rows()-olddepth;

    return ok;
}

/** Schreiben von Records. Zur Vereinfachung der Erstellung von
    Hierarchien (und wegen Abwaertskompatibilitaet mit frueheren
    Versionen) wird inc_level() und dec_level() bereitgestellt, womit
    der Hierarchie-Level erhoeht oder erniedrigt werden kann. Die
    Aenderung des levels wird in m_level_change gespeichert und wird
    vor dem BaseTable::push() benutzt, um REF automatisch zu setzen,
    sofern das nicht vorher manuell passiert ist. */

void
HMasterTable::push()
{
    LOG_METHOD("Table","HMasterTable","push",'<'<<context()->name()<<'>');
    
    if (!context()->is_initialized()) init();
    if (!context()->is_writeback()) return;

    m_level_change += m_ilevel_change;
    if (m_level_change>1) m_level_change=1; // level increase > 1 (siehe pop)
    m_ilevel_change=0;

    if ((mp_output_ref!=NULL)&&(mp_output_ref->val()==g_illegal_ix))
    {
	// m_level_change=1 : outref->var() = m_outkey[m_outkey.rows()-1];
	//                0 : outref->var() = m_outkey[m_outkey.rows()-2];
	// ...
	//cout << m_outkey << m_level_change << mp_output_key->val() << endl;
	if (m_outkey.rows()+m_level_change-2 >= 0)
	{
	    mp_output_ref->var() = m_outkey[ m_outkey.rows()+m_level_change-2 ];
	}
    }
    m_level_change=0;   
    
    SyncTable::push();

    // Pfad mitfuehren
    int depth = wdepth();
    // Hierarchie vorhanden?
    if (m_whier) { 
	while ((depth>0)&&(wkey(depth-1)!=mp_output_ref->val())) --depth; 
    } else { // keine Hierarchie
	depth = 0; 
    }
    m_outkey.adjust(depth+1); wkey(depth)=mp_output_key->val();
    TRACE_VAR(m_outkey);

//    free();
}

///////////////////////////////////////////////////////////////////
// SubSetTable

SubSetTable::SubSetTable(
  int mode,
  const string& name,
  Domain *d=NULL
  )
  : HMasterTable(mode,name,d)
  , m_underflow(false)
{
    SET_TAG(context()->mode(),Context::SUBSETTABLE);
}

SubSetTable::~SubSetTable()
{
}

/** Match hierarchy mask with current data path. Return true if match
    is ok. The type of \c hmask is int_matrix_type, because the \c
    m_select field of the \c Domain class is (mis-) used. */
  
bool SubSetTable::match(
    const int_matrix_type::value_type& hmask
    )
{
    matrix_copy(m_hmask,hmask);
    return HMasterTable::match(hmask);
}

/** Read next record. Set the \c m_underflow flag if the next record
    will not be contained in the current subset. This can be
    determined by considering the hierarchy level of the next record. */

bool SubSetTable::pop()
{
    LOG_METHOD("Table","SubSetTable","pop","");

    bool ok = HMasterTable::pop();
    m_underflow =  ! HMasterTable::match_next(m_hmask);
    TRACE_VAR(done());
    return ok;
}

bool SubSetTable::done()
{
    return m_underflow || HMasterTable::done();
}

void SubSetTable::init()
{
    LOG_METHOD("Table","SubSetTable","init","");

    m_underflow = false;
    HMasterTable::init();
}

void SubSetTable::rewind()
{
    LOG_METHOD("Table","SubSetTable","rewind","");

    m_underflow = false;
    HMasterTable::rewind();
}

///////////////////////////////////////////////////////////////////

/** \class BufferedTable
    \brief Table I/O is done in memory.

    For small but frequently used tables BufferedTables can speedup
    the processing and allow for reading and writing the same table
    iteratively. 

    The complete table is read at the beginning (init()) in memory.
    The push() method writes the records into memory rather than into
    the output file. The final result can be written into a file by
    means of submit() . Calling swap() makes the output generated so
    far being the input for the next read loop. */

template <class T>
BufferedTable<T>::BufferedTable(
    int mode,
    const string& name,
    Domain *d
    )
    : T(mode,name,d)
    , mp_input(new list<ix_type>)
    , mp_output(new list<ix_type>)
    , m_inpos(mp_input->end())
    , mp_temp_writer(new list<Writer*>)
{
}

template <class T>
BufferedTable<T>::~BufferedTable()
{
    // hier DARF kein submit() aufgerufen werden, weil aller Voraussicht
    // nach die angehaengten Felder schon nicht mehr existieren (ob deren
    // Destruktor schon aufgerufen wurde oder nicht, koennen wir hier
    // nicht wissen)

    delete mp_input; mp_input=NULL;
    delete mp_output; mp_output=NULL;
}

/** We do not only initialize the reader but read out the whole table
    and store it in memory. Then, the list of readers is replaced by a
    NULL_Reader. */

template <class T>
void
BufferedTable<T>::init_reader()
{
    LOG_METHOD("Table","BufferedTable","init_reader",context()->name());

    // Alle Daten auslesen
    T::init_reader();
    while (!T::done())
    {
	mp_record->load_default();
	T::pop();
	mp_record->store(); // eingelesene Daten konservieren
	mp_input->push_back(mp_record->get_ix());
    }
    m_inpos = mp_input->end();
    CLEAR_TAG(context()->mode(),Context::ENDOFDATA);
    TRACE_VAR(*mp_input);

    // Reader wieder loeschen
    for (list<Reader*>::iterator r=mp_reader->begin();r!=mp_reader->end();++r)
    { delete (*r); }
    mp_reader->clear();
  
    // NULL_Reader einsetzen
    context()->infile() = "null";
    mp_reader->push_back( get_main_reader() );
    (*mp_reader->begin())->init();

    // Zuruecksetzen, damit Zugriff auf Felder des letzten Records unmoeglich
    mp_record->load_default();
}

/** Only a NULL_Writer is instantiated. File output will be performed
    in submit(). */

template <class T>
void
BufferedTable<T>::init_writer()
{
    // richtige Writer instantiieren aber als temp speichern
    T::init_writer();
    list<Writer*> *h = mp_writer; mp_writer=mp_temp_writer; mp_temp_writer=h;

    // NULL_Writer einsetzen
    context()->outfile() = "null";
    mp_writer->push_back( get_main_writer() );
    (*mp_writer->begin())->init();
}

/** While the other table classes remove the used entries in the
    repository before pop(), we shall not drop the fields in a
    buffered table, because we may need them later. Instead,
    of deleting them, we assign the default values to the fields. 

   free() muss weg: wird vor pop() aufgerufen, loescht i.a. daten, nun wollen wir aber vor pop() die Daten aus dem speicher holen (und die sollen nicht gleich wieder geloescht werden) */

template <class T>
void
BufferedTable<T>::free()
{
    LOG_METHOD("Table","BufferedTable<T>","free",'<'<<context()->name()<<'>');
}

template <class T>
bool
BufferedTable<T>::done()
{
    return (T::done())&&(m_inpos==mp_input->end());
}

/** Turn output into input of next loop. */

template <class T>
void
BufferedTable<T>::swap()
{
    LOG_METHOD("Table","BufferedTable","swap",context()->name());
    TRACE_VAR(mp_input->size());
    TRACE_VAR(mp_output->size());

    if (!mp_input->empty())
    {
	for (list<ix_type>::iterator i=mp_input->begin();i!=mp_input->end();++i)
	    mp_record->free(*i);
	mp_input->clear();
    }
    list<ix_type> *h=mp_output; mp_output=mp_input; mp_input=h; // swap
    m_inpos = mp_input->begin();
}

/** Restart input. */

template <class T>
void
BufferedTable<T>::rewind()
{
    LOG_METHOD("DAL","BufferedTable<T>","rewind",context()->name());

    if (!context()->is_initialized()) init();
//    if (!context()->is_readout()) return; bufferedtable ist immer auch readout!

    // nun Default-Werte laden (aber alte Werte nicht durch free()
    // loeschen), damit bei spaeterem Laden (pop()) nicht dieselben
    // Felder wiederverwendet werden
    mp_record->load_default();

    TRACE_VAR(mp_input->size());
    m_inpos = mp_input->begin();
    CLEAR_TAG(context()->mode(),Context::ENDOFDATA);
}

/** Restart (thereby deleting previously pushed records). */

template <class T>
void BufferedTable<T>::clear()
{
    LOG_METHOD("DAL","BufferedTable<T>","clear",context()->name());

    for (list<ix_type>::iterator i=mp_input->begin();i!=mp_input->end();++i)
	mp_record->free(*i);
    mp_input->clear();

    for (list<ix_type>::iterator i=mp_output->begin();i!=mp_output->end();++i)
	mp_record->free(*i);
    mp_output->clear();
}

/** Read next record. In the first run the data is read from the file
    table (Reader), later it will be read from memory (the m_input
    list). */

template <class T>
bool
BufferedTable<T>::pop()
{
    LOG_METHOD("DAL","BufferedTable<T>","pop",context()->name());

    // kein free, weil wir die alten Werte noch mal benutzen wollen,
    // aber die bisherigen Zellen muessen wir schon freigeben, sonst
    // werden sie ueberschrieben (das free() in T::pop() macht nichts
    // mehr!)
    mp_record->free(); // load_default();

    if (m_inpos==mp_input->end()) return false;

    mp_record->copy(*m_inpos); // aus Hauptspeicher auslesen
    T::pop(); // nicht wirklich lesen (NULL_Reader), aber Hierarchie nachziehen
    ++m_inpos;
    if (m_inpos==mp_input->end()) 
	SET_TAG(context()->mode(),Context::ENDOFDATA);

    TRACE_VAR(mp_input->size());
    TRACE_VAR(mp_output->size());

    return true;
}

/** Write record into memory. */

template <class T>
void
BufferedTable<T>::push()
{
    LOG_METHOD("DAL","BufferedTable<T>","push",context()->name());

    T::push();

    // mglw. geaenderte Felderindizes aktualisieren
    mp_record->store();
    // record speichern
    mp_output->push_back(mp_record->get_ix());
    // fuer eventuelle weitere push()s den alten Inhalt kopieren (und
    // damit Operationen auf dem gepush()ten Record vermeiden
    // (alternativ: load_default)
    mp_record->copy(mp_record->get_ix());

    TRACE_VAR(mp_input->size());
    TRACE_VAR(mp_output->size());
}

/** Finally write buffered output into file. */

template <class T>
void
BufferedTable<T>::submit()
{
    // kurzfristig fuer Ausgabe richtige Writer benutzen
    list<Writer*> *h = mp_writer; mp_writer=mp_temp_writer; mp_temp_writer=h;

    for (list<ix_type>::iterator i=mp_output->begin();i!=mp_output->end();++i)
    {
	mp_record->load(*i); 
	// kein push() weil push() versuchen wuerde die key/ref neu zu
	// setzen...
	list<Writer*>::iterator w;
	for (w=mp_writer->begin();w!=mp_writer->end();++w) 
	{ (*w)->operate(); }
    }

    h = mp_writer; mp_writer=mp_temp_writer; mp_temp_writer=h;
}

template <class T>
void
BufferedTable<T>::close()
{
    T::close();
    clear();

    for (list<Writer*>::iterator w=mp_temp_writer->begin();w!=mp_temp_writer->end();++w)
    { delete (*w); }
    mp_temp_writer->clear();
}

template BufferedTable<MasterTable>;
template BufferedTable<HMasterTable>;

//////////////////////////////////////////////////////////////

BaseTable* get_table(
    const string& name
    )
{
    int i;
    return (gp_dm_tables->find(name,i)) ? (BaseTable*)(i) :  NULL; 
} 

}; // namespace

#endif // TABLE_SOURCE
