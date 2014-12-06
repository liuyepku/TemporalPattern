
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

/** \file dal.hpp 
    \author Frank Hoeppner <frank.hoeppner@ieee.org>

    DAL Library Header
*/

#ifndef DAL_LIBRARY_HEADER
#define DAL_LIBRARY_HEADER

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "ustream.hpp"
#include "matvec.hpp"
#include "stlstream.hpp"
#include "table.hpp"
#include <deque>
#include <list>

using namespace logtrace;
namespace Data_Access_Library {
 

#define DAL_CHANGE_NOTHING   (0)
#define DAL_CHANGE_WRITEBACK (SET_BIT(1))
#define DAL_CHANGE_SILENT    (SET_BIT(2))
#define DAL_CHANGE_FLAT      (SET_BIT(3))
#define DAL_CHANGE_SELECT    (SET_BIT(4))

#define DAL_ACCESS_UNTOUCHED 0
#define DAL_ACCESS_INIT (SET_BIT(0))
#define DAL_ACCESS_DATAREAD (SET_BIT(1))
#define DAL_ACCESS_ENDOFDATA (SET_BIT(2))
#define DAL_ACCESS_CLOSED (SET_BIT(3))
#define DAL_ACCESS_INPUT (SET_BIT(4))
#define DAL_ACCESS_OUTPUT (SET_BIT(5))

/** ix_type

    Typ fuer Indizes (nicht-negative Integer). */

typedef word ix_type;

/** g_illegal_ix

    g_illegal_ix ist eine globale Konstante fuer einen illegalen
    Index-Wert. [datarep.cpp] */

extern ix_type g_illegal_ix;

/** ix_tuple_type

    Typ fuer ein Tuple aus Indizes. */

typedef DynTuple<ix_type> ix_tuple_type;
    
class Domain
{
public:
    Domain();
    Domain(const string&,bool mixedmode=true);
    void clear();
    bool find(const string&,int&);
    bool find(istream&,int&);
    int add(const string&);
    void add(const string&,int value);
    void reassign_indices();
    const string* identifier(int) const;
    void info(ostream& os) const;
    friend ostream& operator<<(ostream&,const Domain&);
    
    bool is_fixed() const { return m_fixed_values; }
    int size() const { return m_size; }
    int precision() const { return m_precision; }
    int& precision() { return m_precision; }
    bool mixed_mode() const { return m_mixed_mode; }
    
private:
    //typedef map< string const*, int, less_dereference<string const*> > str2idx_type;
    typedef map< int, string const* > idx2str_type;
    typedef list<string> strlist_type;
    str2idx_type m_str2idx;
    idx2str_type m_idx2str;
    strlist_type m_words;
    int m_size,m_next;
    bool m_fixed_values;
    int m_precision;
    bool m_mixed_mode; ///< mixed numerical and symbolic values
    
public:
    typedef str2idx_type::const_iterator const_iterator;
    inline const_iterator begin() const { return m_str2idx.begin(); }
    inline const_iterator end() const { return m_str2idx.end(); }
};

Domain* get_domain(const string& name);

extern Domain *gp_dm_boolean;
extern Domain *gp_dm_direction;
extern Domain *gp_dm_numeric;
extern Domain *gp_dm_timestamp;
extern Domain *gp_dm_count;
extern Domain *gp_dm_labels;
extern Domain *gp_dm_longint;
extern Domain *gp_dm_fixes;
typedef enum label_suffix { DROP_SUFFIX=0, NO_SUFFIX=1 };

/** Klasse zum Einlesen von symbolischen Werten (Strings, die intern
    als Integer gehandelt werden). Die Definition der Strings erfolgt
    ueber eine Referenz zu einer Domain-Klasse. [io.cpp] */

template <class T>
class SymbolicIO
{
public:
    SymbolicIO(Domain *ap_domain) : mp_domain(ap_domain) {}
    
    void read(istream& is,T&,const char* = NULL) const;
    void write(ostream& os,const T&) const;
    inline bool symbolic() const { return (mp_domain!=NULL); }

private:
    Domain *mp_domain;
};

class Context
{
public:
    
    enum context_type { SETORRECORD, FIELD, MATVEC };

    enum mode_type { VOID=0, INITIALIZED=(1<<0), SUBSETOUT=(1<<1), 
		     ENDOFDATA=(1<<2), READOUT=(1<<3), WRITEBACK=(1<<4), 
		     SHARED=(1<<5), DELAYED=(1<<6), 
		     DYNALLOC=(1<<7), MULTITABLE=(1<<8), 
		     ORDERED=(1<<10), DETAIL=(1<<11), FLAT=(1<<12), 
		     KEYREF=(1<<13), HIERTABLE=(1<<14), SUBSETTABLE=(1<<15)
    };

    Context(context_type ct,int mode,const string& name,
	    const string& itag,const string& otag,Domain *d=NULL);
    
    inline const Domain* domain() const { return mp_domain; }
    inline Domain* domain() { return mp_domain; }
    inline context_type type() const { return m_type; }
    inline int mode() const { return m_mode; }
    inline int& mode() { return m_mode; }
    void set_mode(int);
    
    inline bool is_field() const { return (m_type!=SETORRECORD); }
    inline bool is_endofdata() const { return IS_TAG(m_mode,ENDOFDATA); }
    inline bool is_underflow() const { return IS_TAG(m_mode,UNDERFLOW); }
    inline bool is_subsetout() const { return IS_TAG(m_mode,SUBSETOUT); }
    inline bool is_initialized() const { return IS_TAG(m_mode,INITIALIZED); }
    inline bool is_readout() const { return IS_TAG(m_mode,READOUT); }
    inline bool is_writeback() const { return IS_TAG(m_mode,WRITEBACK); }
    inline bool is_master() const { return (!is_field())&&IS_NO_TAG(m_mode,DETAIL); }
    inline bool is_detail() const { return (!is_field())&&IS_TAG(m_mode,DETAIL); }
    inline bool is_ordered() const { return IS_TAG(m_mode,ORDERED); }
    inline bool is_multitable() const { return IS_TAG(m_mode,MULTITABLE); }
    inline bool is_flat() const { return IS_TAG(m_mode,FLAT); }
    inline bool is_keyref() const { return IS_TAG(m_mode,KEYREF); }
    inline bool is_subsettable() const { return IS_TAG(m_mode,SUBSETTABLE); }
    inline bool is_hiertable() const { return IS_TAG(m_mode,HIERTABLE); }
    
    inline ix_type get_default_ix() const { return m_default_ix; }
    inline void set_default_ix(ix_type ix) { m_default_ix=ix; }
    
    inline const string& name() const { return m_name; }
    inline const string& coll() const { return m_coll; }
    inline string& coll() { return m_coll; }
    inline const string& intag() const { return m_intag; }
    inline string& intag() { return m_intag; }
    inline const string& outtag() const { return m_outtag; }
    inline string& outtag() { return m_outtag; }
    inline const string& infile() const { return m_infile; }
    inline string& infile() { return m_infile; }
    inline const string& outfile() const { return m_outfile; }
    inline string& outfile() { return m_outfile; }
    inline ix_type offset() const { return m_offset; }
    inline ix_type& offset() { return m_offset; }
    inline int rows() const { INVARIANT(m_type!=SETORRECORD,""); return m_rows; }
    inline int& rows() { INVARIANT(m_type!=SETORRECORD,""); return m_rows; }
    inline int cols() const { INVARIANT(m_type!=SETORRECORD,""); return m_cols; }
    inline int& cols() { INVARIANT(m_type!=SETORRECORD,""); return m_cols; }
    inline int size() const { return m_rows*m_cols; }
    inline DynMatrix<int>& select() { INVARIANT(m_type!=SETORRECORD,""); return m_select; } 
    inline const DynMatrix<int>& select() const { INVARIANT(m_type!=SETORRECORD,""); return m_select; } 

    inline int idepth() const { INVARIANT(m_type==SETORRECORD,""); return m_select.rows(); }
    inline int istep() const { INVARIANT(m_type==SETORRECORD,""); return m_cols; }
    inline int& istep() { INVARIANT(m_type==SETORRECORD,""); return m_cols; }
    inline DynMatrix<int>& hmask() { INVARIANT(m_type==SETORRECORD,""); return m_select; } 
    inline const DynMatrix<int>& hmask() const { INVARIANT(m_type==SETORRECORD,""); return m_select; } 
    
private: 
    context_type m_type; ///< Context fuer welche Art von Daten?
    int m_mode; ///< Schreib/Lesemodus
    
    string m_name; ///< Name des Feldes zur Referenz
    string m_coll; ///< Name der Collection (bei umgeleiteten Feldern)
    string m_intag; ///< Name des Feldes bei Eingabe
    string m_outtag; ///< Name des Feldes bei Ausgabe
    string m_infile; ///< Name der Eingabedatei
    string m_outfile; ///< Name der Ausgabedatei
    ix_type m_offset; ///< Position des Feldes in einer TAB-Eingabe
    int m_rows,m_cols; ///< Groesse des Feldes bei TAB-Eingabe, bei SubsetTable Iterationstiefe (idepth(), m_rows) und Reihenfolge (istep(), m_cols) 
    DynMatrix<int> m_select; ///< Selector-String als Eingabemaske
    
    ix_type m_default_ix; ///< Defaultwert als Index im entspr. Repository
    
    Domain *mp_domain; ///< Domain bei symbolischen Werten
};

ostream& operator<<(ostream&,const Context&);


#define cINITIALIZED Context::INITIALIZED
#define cENDOFDATA Context::ENDOFDATA
#define cREADOUT Context::READOUT
#define cWRITEBACK Context::WRITEBACK
#define cDETAIL Context::DETAIL
#define cMASTER Context::VOID
#define cSUBSETOUT Context::SUBSETOUT
#define cORDERED Context::ORDERED
#define cSHARED Context::SHARED
#define cMULTITABLE Context::MULTITABLE
#define cFLAT Context::FLAT
#define cKEYREF Context::KEYREF
#define cVOID Context::VOID

template <class T>
class Repository
{
public:
    typedef typename deque<T>::const_iterator const_iterator;
    
    explicit Repository() : next_element(0) {}
    
    ix_type insert(const T& a_value);
    ix_type insert();
    
    inline const T& operator[](ix_type a_no) const { return elements[a_no]; }
    inline T& operator[](ix_type a_no) { return elements[a_no]; }
    
    inline void free(ix_type a_no) { m_free.push_back(a_no); }
    
    void info(ostream& os) const;
    void mem_info(ostream& os) const;
    inline const_iterator begin() const { return elements.begin(); }
    inline const_iterator end() const { return elements.end(); }

private:
    deque<T> elements;
    ix_type next_element;
    list<ix_type> m_free;
};

template <class T>
inline ostream& operator<<(ostream& os,const Repository<T>& a_rep)
  { a_rep.info(os); return os; }

/** abstract_data_type 

    Diese Klasse definiert einen abstrakten Datentyp. In der
    oeffentlichen Schnittstelle stehen Funktionen zum Schreiben des
    Datum bereit. Weitere Zugriffsfunktionen kommen in der Klasse
    data_type hinzu, die den abstract_data_type um den Typ der
    tatsaechlich gespeicherten Daten erweitert. Diese abstrakte
    Basisklasse ist notwendig, um die data_type<T> Klassen mit einen
    einheitlichen Schnittstelle zu versehen, die fuer die iterator
    Zugriffsklassen benoetigt wird.
 
    In der privaten Schnittstelle sind Methoden zur Verwaltung
    mehrerer Daten dieses Datentyps in einem Repository vorhanden. */

class abstract_data_type
{
    friend class record_type;
    friend class BaseTable;
    
public:
    explicit abstract_data_type(Context*); 
    virtual ~abstract_data_type() {}
    abstract_data_type(const abstract_data_type&); // CC not defined
    
    virtual void read(istream& is,const matrix_separators* = NULL); 
    virtual void read(istream& is,int,const matrix_separators* = NULL);
    virtual void write(ostream& os,const matrix_separators* = NULL) const;
    virtual void info(ostream& os) const = 0;
    virtual int size() const = 0; ///< Anzahl der Felder in einer Tabelle
    virtual bool is_default() const = 0; //{ DO_NOT_PASS; return true; }
    virtual void load_default() = 0;
    virtual void copy_default() = 0;
    virtual void set_context_default(const string&,const matrix_separators* = NULL) = 0;
    
    inline bool is_readout() const { return IS_TAG(mp_context->mode(),Context::READOUT); }
    inline bool is_writeback() const { return IS_TAG(mp_context->mode(),Context::WRITEBACK); }
    inline bool is_shared() const { return IS_TAG(mp_context->mode(),Context::SHARED); }
    inline void set_mode(int m) { mp_context->set_mode(m); }
    inline Context const * context() const { return mp_context; }
    inline Context* context() { return mp_context; }
    inline Context::context_type type() const { return mp_context->type(); }
    inline ix_type get_ix() const { return m_ix; } ///< Repository-Index zurueckgeben
    
protected:
    virtual void load(ix_type) = 0; ///< Variable mit Wert aus Repository laden
    virtual void copy(ix_type) = 0; ///< Variable mit Kopie aus Repository laden
    virtual void free() = 0; ///< Variable auf Default setzen, Repository freigeben
    virtual void free(ix_type) = 0; ///< Wert aus Repository freigeben
    virtual ix_type get_default_ix() const = 0; ///< Repository-Index des Default-Wertes liefern
    
protected:
    Context *mp_context; ///< Context des Feldes (Feldbeschreibung)
    ix_type m_ix; ///< Index auf das Feld im entsprechenden Repository
};

typedef list< abstract_data_type* > adtp_list;

inline ostream& operator<<(ostream& os,const abstract_data_type& d)
  { d.write(os); return os; }

/** data_type<T>

    Diese Klasse erweitert den abstract_data_type um einen Datentyp T,
    sowie Zugriffsfunktionen auf ein Datum dieses Typs
    (operator()). Die Funktionen zum Lesen/Schreiben in Streams werden
    noch nicht spezifiziert. */

template <class T>
class data_type
    : public abstract_data_type
{
public: 
    typedef T value_type;
    
public: 
    explicit data_type(Context*);
    ~data_type() { free(); }
    
    virtual void info(ostream& os) const;
    static void mem_info(ostream& os) { m_repository.mem_info(os); }
    
    /** Schreib-Zugriff auf Feld. Bei Schreib-Anforderung neuen Speicher
	allokieren, wenn bisher nur der Default-Wert geladen war, da wir
	diesen nicht ueberschreiben wollen. Leseanforderung
	unkritisch. Zuvor hatte ich operator()() fuer beide Faelle
	benutzt, aber bei Anweisungen wie (x||field()) wird vom Compiler
	dann die non-const Variante aufgerufen, was zur unnoetigen
	Allokation von Speicher fuehrt. Daher wird der Schreibzugriff
	nun explizit durch Aufruf von .set(). */
    inline T& var() { if (this->is_default()) allocate(); return (*mp_value); }
    /** Lese-Zugriff auf Feld. */
    inline const T& operator()() const { return (*mp_value); }
    inline const T& val() const { return (*mp_value); }
    
    virtual bool is_default() const;
    virtual void load_default();
    virtual void copy_default();
    
    static void set_global_default(const T&);
    virtual void set_context_default(const string&,const matrix_separators* = NULL);
    
protected:
    void allocate(); ///< Ein neues Feld (auch im Repository) anlegen
    virtual void load(ix_type); 
    virtual void copy(ix_type);
    virtual void free();
    virtual void free(ix_type);
    virtual ix_type get_default_ix() const;
    virtual ix_type store(const T& x); ///< Ein Feld im Repository speichern

private:
    T* mp_value; ///< Zeiger auf die Feldinstanz im Repository (privat um Zugriff zu steuern)
protected:
    static ix_type m_global_default_ix; ///< Default-Index fuer alle T (lokal im Context)
    static Repository<T> m_repository; ///< Daten-Repository fuer T
};

/** field_type<T>

    Verfeinerung von data_type<T> fuer skalare Typen (int und
    double). */

class BaseTable;

template <class T>
class field_type
    : public data_type<T>
{
public:
    typedef T scalar_type;
    
    explicit field_type(BaseTable*,int,const string,Domain *d=gp_dm_numeric);
    explicit field_type(BaseTable*,int,const string,const string,const string,Domain *d=gp_dm_numeric);
    virtual ~field_type();
    virtual int size() const { return 1; }
    virtual void read(istream& is,const matrix_separators* = NULL);
    virtual void read(istream& is,int,const matrix_separators* = NULL);
    virtual void write(ostream& os,const matrix_separators* = NULL) const;

protected:
    const SymbolicIO<T> m_io; ///< IO-Operation fuer symbolische Werte
};

/** array_type<T>

    Verfeinerung von data_type<T> fuer indizierte Typen (tuple und
    matrix). */

template <class T>
class array_type
    : public data_type<T>
{
public:
    typedef typename T::value_type scalar_type;

    explicit array_type(BaseTable*,int mode,const string& name,Domain *d=gp_dm_numeric);
    virtual ~array_type();
    virtual int size() const { return (this->operator()()).size(); }
    virtual void read(istream& is,const matrix_separators* = NULL);
    virtual void read(istream& is,int,const matrix_separators* = NULL);
    virtual void write(ostream& os,const matrix_separators* = NULL) const;

protected:
    explicit array_type(Context *);

protected:
    const SymbolicIO<typename T::value_type> m_io; ///< IO-Operation fuer symbolische Werte
};

typedef field_type< int > int_type;
typedef field_type< word > index_type;
typedef field_type< long int > long_type;
typedef field_type< double > real_type;
typedef field_type< string > string_type;

typedef array_type< DynTuple<int> > int_tuple_type;
typedef array_type< DynTuple<double> > tuple_type;

typedef array_type< DynMatrix<int> > int_matrix_type;
typedef array_type< DynMatrix<double> > matrix_type;

/** Tabelle. */

class Reader;
class Writer;

class BaseTable
{
    //friend class record_type;
public:
    explicit BaseTable(int,const string&,Domain *d=NULL);
    virtual ~BaseTable();
    
    inline Context const * context() const;
    inline Context* context();
    void info(ostream&) const;
    
    virtual void attach(abstract_data_type *ap_value);
    abstract_data_type* get_field(const string& name) const;
    
    virtual void init();
    virtual void mark()=0;
    virtual void rewind() = 0; ///< Spult Eingabe zurueck
    virtual void clear() = 0; ///< Beginnt Ausgabe erneut
    virtual void push() = 0;
    virtual bool pop() = 0;
    virtual bool done() = 0;
    virtual void close() = 0;
    virtual void free();
    
    inline index_type const * keyptr() 
	{ INVARIANT(context()->is_initialized(),"init+pop required"); 
        return (context()->is_readout()) ? mp_input_key : mp_output_key; }
    inline ix_type get_input_key() 
	{ INVARIANT(context()->is_initialized(),"init+pop required");
	return (*mp_input_key)(); }
    inline void set_output_key(ix_type x) 
	{ if (!context()->is_initialized()) init();
        if (mp_output_key!=NULL) mp_output_key->var()=x; 
        else mp_input_key->var()=x; } 
    
protected:

    virtual void init_reader() = 0;
    virtual void init_writer() = 0;
    virtual Reader* get_main_reader();
    virtual Reader* get_sub_reader(Context*);
    virtual Writer* get_main_writer();
    virtual Writer* get_sub_writer();
    
protected:
    record_type *mp_record;
    index_type *mp_input_key,*mp_input_ref,*mp_output_key,*mp_output_ref;
};

BaseTable* get_table(const string&); 

/////////////////////////////////

/** \struct marker_type
    \brief Dateipositions-Markierung.
    
    Markierung einer Dateiposition durch eine Datei-ID, einen
    Schluessel und eine Stream-Position. */

struct marker_type
{
    marker_type() 
	: m_file(0),m_spos(0) {}
    marker_type(streampos p) 
	: m_file(0),m_spos(p) {}
    
    inline bool operator==(const marker_type& m)
	{ return (m_file==m.m_file)&&(m_spos==m.m_spos); }
    
    int m_file;
    int m_key;
    streampos m_spos;
};

inline ostream& operator<<(ostream& os,const marker_type& m)
{ return os << '(' << m.m_file << ',' << (long)m.m_spos << ')'; }

/** Einfache Tabelle, ohne Hierarchie, nur eine Eingabequelle. */

class SimpleTable
    : public BaseTable
{
public:
    explicit SimpleTable(int,const string&,Domain *d=NULL);
    virtual ~SimpleTable();
    
    virtual void mark();
    virtual void rewind();
    virtual void clear();
    virtual void push();
    virtual bool pop();
    virtual bool done();
    virtual void close();
    
    void setrpos(const marker_type&);
    void getrpos(marker_type&);
    void setwpos(const marker_type&);
    void getwpos(marker_type&);
    
protected:

    virtual void init_reader();
    virtual void init_writer();

    Reader *mp_reader;
    Writer *mp_writer;
    marker_type m_rstart,m_wstart;
};

////////////////////////////////

/** Tabelle ohne Hierarchie, mehrere Eingabequellen. */

class SyncTable
    : public BaseTable
{
public:
    explicit SyncTable(int,const string&,Domain *d=NULL);
    virtual ~SyncTable();
    
    virtual void mark();
    virtual void rewind();
    virtual void clear();
    virtual void push();
    virtual bool pop();
    virtual bool done();
    virtual void close();

protected:
    virtual void init_reader();
    virtual void init_writer();

    
protected:

    list<Reader*> *mp_reader;
    list<Writer*> *mp_writer;
    int m_rsize; ///< Anzahl der Reader
    int m_wsize; ///< Anzahl der Writer
    DynTuple<marker_type> m_rstart; ///< Positionen der Reader
    DynTuple<marker_type> m_wstart; ///< Positionen der Writer
};

/** Master-Table (ohne Hierarchie). */

class
MasterTable
    : public SyncTable
{
public:
    explicit MasterTable(int,const string&,Domain *d=NULL);
    virtual ~MasterTable();
};

/** Hierarchischer Master-Table. */

class
HMasterTable
    : public SyncTable
{
public:
    explicit HMasterTable(int,const string&,Domain *d=NULL);
    virtual ~HMasterTable();
    
    virtual void init();
    virtual void push();
    virtual bool pop();
    
    bool match(const int_matrix_type::value_type&) const;
    bool match_next(const int_matrix_type::value_type&) const;
    /*
    int level(); 
    int next_level(); 
    */

    void inc_level() 
	{ 
	    m_level_change = 1; 
	    if (mp_output_ref!=NULL) mp_output_ref->load_default(); 
	    // ein zusaetzlicher Eintrag zur Erhoehung der Tiefe hat keinen
	    // Originaleintrag in der Eingabedatei
	    //mp_input_key->load_default();
	    m_whier=true;
	}
    void dec_level() 
	{ 
	    m_level_change = -1; 
	    if (mp_output_ref!=NULL) mp_output_ref->load_default(); 
	    m_whier=true;
	}
    
protected:

    virtual Reader* get_main_reader();
    virtual Reader* get_sub_reader(Context*);
    virtual Writer* get_main_writer();
    virtual Writer* get_sub_writer();
    
    inline ix_type& rkey(int i) { return m_inkey[i]; }
    inline ix_type rkey(int i) const { return m_inkey[i]; }
    inline ix_type& wkey(int i) { return m_outkey[i]; }
    inline int rdepth() const { return m_inkey.rows(); }
    inline int wdepth() const { return m_outkey.rows(); }
    
protected:
    int m_level_change; ///< level change by user
    int m_ilevel_change; ///< level change by input file

    DynTuple<ix_type> m_inkey,m_outkey,m_relkey;
    bool m_rhier,m_whier;
};

template <class T>
class BufferedTable
    : public T
{
public:
    explicit BufferedTable(int,const string&,Domain *d=NULL);
    ~BufferedTable();
    
    virtual void rewind();
    virtual void clear();
    virtual void push();
    virtual bool pop();
    virtual bool done();
    virtual void close();
    virtual void submit();
    virtual void free();
    void swap();
  
protected:
    virtual void init_reader();
    virtual void init_writer();

private:
    list<ix_type> *mp_input;
    list<ix_type> *mp_output;
    list<ix_type>::iterator m_inpos;
    list<Writer*> *mp_temp_writer;
};

typedef BufferedTable<MasterTable> BMasterTable;
typedef BufferedTable<HMasterTable> BHMasterTable;

/** SubSetTable. Nur fuer HMaster sinnig. */

class SubSetTable
    : public HMasterTable
{
public:
    SubSetTable(int,const string&,Domain *d=NULL);
    virtual ~SubSetTable();
    
    inline void next_subset() { m_underflow = false; }
    bool match(const int_matrix_type::value_type&);
    
    virtual void init();
    virtual void rewind();
    virtual bool pop();
    virtual bool done();
    
private:
    int_matrix_type::value_type m_hmask;
    bool m_underflow;
};

/** Globale Funktionen. */

int combine_label_prefix(const int_type& prefix,int_type& label);
int combine_label_suffix(int_type& label,const int_type& suffix);
void init_global(); ///< Initialisierung globaler Variablen
void info(ostream&);
void evaluate_cmdline(int argc,char** argv);

typedef HMasterTable iterator_type;
typedef HMasterTable iterator;
typedef BHMasterTable buffered_iterator;

// eigentlich while (!done) { pop() .do.it.. push(); }, aber das geht
// mit for() nicht, das letzte push bleibt weg. Daher liefert pop()
// jetzt bool-ok-Wert und dann :
#define for_all_in_any_level(i) \
  for ((i).rewind();(i).pop();(i).push())
#define read_all_in_any_level(i) \
  for ((i).rewind();(i).pop();)
/*
#define for_all_in_level_of(l,i) \
  for (;(i).level()>0;++(i)) if ((l)==(i).level())
#define for_all_in_same_level(i) \
  if ((i).level()>0) for (ix_type l=(i).level();l==(i).level();++(i))
#define read_all_in_any_level(i) \
  if ((i).level()>0) for (ix_type l=(i).level();l<=(i).level();(i).pop())
#define read_atmost_in_any_level(ctr,i) \
  if ((i).level()>0) for (ix_type l=(i).level();(l<=(i).level())&&((ctr)>0);(i).pop(),--(ctr))
#define for_all_in_sublevel(i) \
  if ((i).level()>0) for (ix_type l=(i).level();l+1==(i).level();++(i))
*/

}; // namespace 

#endif // DAL_LIBRARY_HEADER

