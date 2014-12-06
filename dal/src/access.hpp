
/** \file access.hpp
    \author Frank Hoppner <frank.hoeppner@ieee.org> */

#ifndef ACCESS_HEADER
#define ACCESS_HEADER

#include <strstream>
#include "dal.hpp"

namespace Data_Access_Library {

extern const matrix_separators g_matrix_ddl_separators;
extern const matrix_separators g_matrix_csv_separators;
extern const matrix_separators g_matrix_tab_separators;
extern const matrix_separators g_matrix_cml_separators;

/* neu: Context angeben, darueber collname und filename
   klar. bedingungen an key+ref beim construktor uebergeben. Auslesen
   des keys ueber domain des contextes */

class IOBase
{
public:
    IOBase(Context*);
    virtual ~IOBase();
    virtual void attach(abstract_data_type*);
    virtual bool init();
    virtual void getpos(marker_type&);
    virtual void setpos(const marker_type&);
    virtual bool operate() = 0;
    virtual const string& name() = 0; 

    inline int ref() const { return m_curr_ref; }
    inline int nextref() const { return m_next_ref; }
    inline int nextkey() const { return m_next_key; }
    inline bool done() const { return m_done; }
    inline Context* context() { return mp_context; }

protected:
    virtual bool fetch() = 0;
    void sort_fields_by_intag();
    void sort_fields_by_offset();

protected:
    Context *mp_context;
    const matrix_separators *mp_matsep;
    adtp_list m_fields;
    streampos m_curr_loc;
    streampos m_next_loc;
    streampos m_ahead_loc;
    ix_type m_curr_key;
    ix_type m_next_key;
    ix_type m_curr_ref;
    ix_type m_next_ref;
    bool m_done;
    string m_recbegin,m_recend;
    string m_fieldbegin,m_fieldend;
};
    
class Writer
    : public IOBase
{
public:
    typedef ofstream file_type;

public:
    Writer(Context*,index_type*,index_type*,index_type*,index_type*,ostream*); 
    virtual ~Writer();
    virtual bool init();
    virtual void setpos(const marker_type&);
    virtual bool operate();
    virtual const string& name() { return mp_context->outfile(); }

protected:
    virtual bool fetch();
    
    inline streampos tell() {return (mp_stream==&cout)?0:mp_stream->tellp();}
    inline void seek(streampos p) {if (mp_stream!=&cout) mp_stream->seekp(p);}
    
    inline void open_file_stream(const char* p) 
	{ dynamic_cast<file_type*>(mp_stream)->open(p,ios::out); }
    inline void close_file_stream()
	{ (*mp_stream) << endl; dynamic_cast<file_type*>(mp_stream)->close(); }

protected:
    ostream* mp_stream;
    index_type *mp_output_key,*mp_output_ref,*mp_input_key,*mp_required_key;
};

class Reader
    : public IOBase
{
public:
    typedef ifstream file_type;

public:
    Reader(Context*,index_type*,index_type*,index_type*,index_type*,istream*);
    virtual ~Reader();
    virtual bool init();
    virtual void setpos(const marker_type&);
    virtual bool operate();
    virtual const string& name() { return mp_context->infile(); }

protected:
    virtual bool fetch();
    virtual void load(istream&) = 0;
    virtual void sneak(istream&) = 0;
    
    inline streampos tell() {return (mp_stream==&cin)?0:mp_stream->tellg();}
    inline void seek(streampos p) {if (mp_stream!=&cin) mp_stream->seekg(p);}
    
    inline void open_file_stream(const char* p) 
	{ dynamic_cast<file_type*>(mp_stream)->open(p,ios::in); }
    inline void close_file_stream()
	{ dynamic_cast<file_type*>(mp_stream)->close(); }

protected:
    istream* mp_stream;
    string m_lookahead;
    index_type *mp_key,*mp_ref,*mp_required_key,*mp_required_ref;
};

class NULL_Writer
    : public Writer
{
public:
    NULL_Writer(Context*,index_type*,index_type*,index_type*,index_type*);
    virtual ~NULL_Writer() {}
    virtual bool init();
    virtual bool operate();
};

class NULL_Reader
  : public Reader
{
public:
    NULL_Reader(Context*,index_type*,index_type*,index_type*,index_type*);
    virtual ~NULL_Reader() {}
    virtual bool init();
    virtual bool operate();
protected:
    virtual bool fetch();
    virtual void load(istream&);
    virtual void sneak(istream&);
};

class TAG_Writer // sparse tab
    : public Writer
{
public:
    TAG_Writer(Context*,index_type*,index_type*,index_type*,index_type*,ostream*);
    virtual ~TAG_Writer() {}
    virtual bool init();
    virtual bool operate();
};

class TAG_Reader
    : public Reader
{
public:
    TAG_Reader(Context*,index_type*,index_type*,index_type*,index_type*,istream*);
    virtual ~TAG_Reader();
    virtual bool init();
protected:
    virtual void load(istream&);
    virtual void sneak(istream&);
protected:
    keyword_entry *mp_keywords; ///< sortierte intag()-Auflistung 
    char *mp_text; ///< Buffer fuer sortierte intag()-Auflistung
    int m_key_index; /// Index des Hauptschluessels in mp_keywords
    int m_ref_index; /// Index des Elternschluessels in mp_keywords
    int m_size; ///< Anzahl der Felder
    int m_length; ///< Summe der intag()-Laengen der Felder
};

class CSV_Reader
    : public Reader
{
public:
    CSV_Reader(Context*,index_type*,index_type*,index_type*,index_type*,istream*);
    virtual ~CSV_Reader();
    virtual bool init();
protected:
    virtual void load(istream&);
    virtual void sneak(istream&);
protected:
    int m_key_index; /// Offset des Hauptschluessels in Tabelle
    int m_ref_index; /// Offset des Elternschluessels in Tabelle
};

class CSV_Writer
  : public Writer
{
public:
    CSV_Writer(Context*,index_type*,index_type*,index_type*,index_type*,ostream*);
    virtual ~CSV_Writer() {}
    virtual bool init();
    virtual bool operate();
    void write_header();
};

class TAB_Reader
    : public Reader
{
public:
    TAB_Reader(Context*,index_type*,index_type*,index_type*,index_type*,istream*);
    virtual ~TAB_Reader();
    virtual bool init();
protected:
    virtual void load(istream&);
    virtual void sneak(istream&);
protected:
    int m_key_index; /// Offset des Hauptschluessels in Tabelle
    int m_ref_index; /// Offset des Elternschluessels in Tabelle
};

class TAB_Writer
    : public Writer
{
public:
    TAB_Writer(Context*,index_type*,index_type*,index_type*,index_type*,ostream*);
    virtual ~TAB_Writer() {}
    virtual bool init();
    virtual bool operate();
    void write_header();
};

template <class T>
class File
    : public T
{
public:
    File(Context*,index_type*,index_type*,index_type*,index_type*);
    virtual ~File();
    virtual bool init();
    virtual void setpos(const marker_type&);
//    virtual void rename(const string& a_name) { m_filename=a_name; }
protected:
    string m_fname;
};

typedef File<TAG_Reader> TAG_File_Reader;
typedef File<TAG_Writer> TAG_File_Writer;
typedef File<CSV_Reader> CSV_File_Reader;
typedef File<CSV_Writer> CSV_File_Writer;
typedef File<TAB_Reader> TAB_File_Reader;
typedef File<TAB_Writer> TAB_File_Writer;

/** Diese Klasse erweitert die *_Reader Klassen um die
    Moeglichkeit mehrere Dateien nacheinander einzulesen, als wenn sie
    eine einzige Datei waeren. Die angegebene Datei wird wortweise
    ausgelesen, jedes Wort als Filename interpretiert und eine
    *_Reader des Template-Typs T instanziiert. Beginnend mit dem
    aktuelle Reader schalten wir am Dateiende auf den naechsten
    Reader, Dateiende wird mit Dateiende der letzten Datei
    gemeldet. */
#ifdef XXX

 BAUSTELLE: muss fetch+load etc. implementieren, damit Vorausschau auf naechsten Eintrag (ggf. in anderer Datei) moeglich.

template <class T>
class File_List
  : public T /* T muss ein Reader sein! */
  {
  public:
    File_List(Context*,index_type*,index_type*,index_type*,index_type*);
    virtual ~File_List();
    virtual void attach(abstract_data_type*);
    virtual bool init();
    virtual bool operate();
    virtual void getpos(marker_type&);
    virtual void setpos(const marker_type&);
  protected:
    string_type m_filename;
    list<string> m_filenames;
    list<string>::iterator mi_file;
    adtp_list m_indirect_fields;
    bool m_delayed_reset;
    marker_type m_delayed_pos;
    int m_curr_file;
    Reader *mp_reader;
  };

typedef File_List<TAG_File_Reader> TAG_File_List_Reader;
typedef File_List<CSV_File_Reader> CSV_File_List_Reader;
typedef File_List<TAB_File_Reader> TAB_File_List_Reader;
#endif

/** Allokation von Readern */

Reader* get_reader(
    Context *ap_context, index_type *ap_key, index_type *ap_ref,
    index_type *ap_rkey, index_type *ap_rref);

/** Allokation von Writern */

Writer* get_writer(
    Context *ap_context, index_type* ap_okey, index_type* ap_oref,
    index_type* ap_ikey, index_type* ap_iref /*oder required_key*/ );

}; // namespace Data_Analysis_Library

#endif // ACCESS_HEADER

