
/** Bei Master-Detail Tabellen wird die Detailtabelle nicht an die
    Mastertabelle attached. Die Angabe der Mastertabelle dient zur
    Verknuepfung der Key-Domains. */

#include "dal.hpp"
#include "globdef.hpp"

using namespace logtrace;
using namespace Data_Access_Library;

/** algorithmen als tabellengeneratoren -- noch problematisch: was ist, wenn ein algorithmus mehrere tabellen generieren soll (wie fcm: data+prot)?


class STC_Table
  : public table_type
{
public:
STC_Table(table_type *p_source);
attach(abstract_data_type* p_field)
{ 
    if (name="min") mp_min=p_field;
...
} 
virtual void pop()
	{

	}

};

*/


int main()
{
  LOGTRACE_INIT("dal.log","dal.id");
  init_global();

/*
  // Laden von symbolischen Labeln
  Domain dm_domain_keys;
  MasterTable dom(NULL,cREADOUT,"domain");
  index_type domain_key(&dom,cREADOUT|cORDERED|cKEY,"key",&dm_domain_keys);
  string_type domain_name(&dom,cREADOUT,"name"); 
  string_type domain_intag(&dom,cREADOUT,"intag"); 
  int_type domain_fixed(&dom,cREADOUT,"fixed",gp_dm_boolean);

  Domain dm_label_keys;
  DetailTable label(&dom,cDETAIL|cREADOUT,"label"); 
  index_type label_key(&label,cREADOUT|cKEY|cORDERED,"key",&dm_label_keys);
  index_type label_dmref(&label,cREADOUT|cREF|cORDERED,"ref",&dm_domain_keys);
  string_type label_name(&label,cREADOUT,"name"); 
  int_type label_value(&label,cREADOUT,"value"); 

  while (!dom.done())                           // Iteration ueber alle Domains
  { 
    dom.pop();                                                  // Domain laden
    cout << "dom.pop():" << dom.mark() << endl; dom.info(cout);
    Domain *p_domain = get_domain(domain_name());         // Domain bekannt?
    if (p_domain==NULL) p_domain = new Domain(domain_name());    // neue Domain
    bool fixed = (p_domain->is_fixed())||(domain_fixed());//vorher schon fixed?

    while (!label.done())            // Iteration ueber die Label in der Domain
    { 
      label.pop();                                               // Label laden
      cout << "label.pop():" << label.mark() << endl; label.info(cout);

      if (fixed) 
        p_domain->add(label_name(),label_value());   // Label mit Wert anmelden
      else
        p_domain->add(label_name());          // nur Label (ohne Wert) anmelden
    }
    if (!fixed) p_domain->reassign_indices();              // sort if not fixed
  }

  // Laden von Tags und Setzen der Ein/Ausgabedateien
  table_type tables(NULL,cREADOUT,"table");
  string_type table_name(&tables,cREADOUT,"name"); 
  string_type table_intag(&tables,cREADOUT,"intag");
  string_type table_outtag(&tables,cREADOUT,"outtag");
  string_type table_infile(&tables,cREADOUT,"infile");
  string_type table_outfile(&tables,cREADOUT,"outfile");

  table_type fields(&tables,cREADOUT,"field");
  string_type field_name(&fields,cREADOUT,"name"); 
  string_type field_intag(&fields,cREADOUT,"intag");
  string_type field_outtag(&fields,cREADOUT,"outtag");
  string_type field_infile(&fields,cREADOUT,"infile");
  string_type field_outfile(&fields,cREADOUT,"outfile");
  string_type field_default(&fields,cREADOUT,"default");
  int_type field_offset(&fields,cREADOUT,"offset",gp_dm_intvalued);
  int_type field_rows(&fields,cREADOUT,"rows",gp_dm_intvalued);
  int_type field_cols(&fields,cREADOUT,"cols",gp_dm_intvalued);
  int_type field_domain(&fields,cREADOUT,"domain",gp_dm_domains);
  string_type field_select(&fields,cREADOUT,"select");
  int_type field_write(&fields,cREADOUT,"write",gp_dm_boolean);

  while (!tables.done())                       // Iteration ueber alle Tabellen
  { 
    tables.pop();                              
    AbstractTable *p_table = get_table(table_name());
    if (!table_intag.is_default()) p_table->context()->intag() = table_intag();
    if (!table_outtag.is_default()) p_table->context()->outtag() = table_outtag();
    if (!table_infile.is_default()) p_table->context()->infile() = table_infile();
    if (!table_outfile.is_default()) p_table->context()->outfile() = table_outfile();

    while (!fields.done())        // Iteration ueber alle Felder in der Tabelle
    {
      fields.pop();
      abstract_data_type *p_field = p_table->get_field(field_name());
      if (p_field==NULL)
      { 
//        ERROR("no field <"<<field_name()<<"> in table <"<<table_name()<<">");
        continue;
      }
      if (!field_intag.is_default()) p_field->context()->intag() = field_intag();
      if (!field_outtag.is_default()) p_field->context()->outtag() = field_outtag();
      if (!field_infile.is_default()) p_field->context()->infile() = field_infile();
      if (!field_outfile.is_default()) p_field->context()->outfile() = field_outfile();
      if (!field_default.is_default()) p_field->set_context_default(field_default());
      if (!field_offset.is_default()) p_field->context()->offset() = field_offset();
      if (!field_rows.is_default()) p_field->context()->rows() = field_rows();
      if (!field_cols.is_default()) p_field->context()->cols() = field_cols();
#     define DEF_TAG(s,t,b) if (b) SET_TAG((s),(t)); else CLEAR_TAG((s),(t));
      if (!field_write.is_default()) DEF_TAG(p_field->context()->mode(),cWRITEBACK,field_write());
    }
  }
*/
/** Aus dem XML Format ergibt sich: der Reader muss Buch fuehren ueber
    die Hierarchie (depth). Diese Information muessen wir uns nicht
    fuer alle Records merken, es reicht wenn wir fuer jeden Record
    <depth> speichern. Die volle Information ueber alle Vorgaenger
    brauchen wir nur fuer die "Front" zum weiteren Durcharbeiten durch
    die DB.

    Multi-Set-Strukturen

    Wir sprechen von einer Multi-Set-Struktur, wenn ein set_type einen
    anderen set_type enthaelt. 

    beispiel: data enthaelt x und y sets
      data-set
        x-set 
        y-set

    und der benutzer iteriert erst ueber y und will danach ueber x
    iterieren => geht nicht in einem table_type, weil zum Lesen
    von y bereits x ueberlesen wurde. Wir brauchen also fuer jeden
    record_type einen eigenen Reader.

    Der top-level set_type gibt das init() und pop() an alle
    darunterliegenden set_types weiter.  Fuer die Implementierung der
    done() Funktionalitaet brauchen wir eine "Vorschau" (lookahead,
    sneak). Jeder Reader liest nach einem pop bereits die naechste
    Zeile in einem Puffer. Uns interessiert dabei, ob wir in der
    Hierachie weiter absteigen (also parent = vorgaenger, noch nicht
    done()) oder nicht (parent != vorgaenger, neuer Record,
    done()!). 

    Nach einem pop() muessen alle Subreader weitergesetzt werden. Da
    wir die Subreader u.U. aus einem anderen File lesen genuegt es
    nicht, die Fileposition der Subreader auf die Fileposition im
    Hauptreader zu setzen, sondern der Subreader muss solange lesen,
    bis ein Eintrag gefunden wurde, dessen parent der key des neuen
    Hauptrecords ist. Wenn es gar keinen Subreader-Eintrag gibt, dann
    besteht die Gefahr, dass wir zu weit lesen, bereits
    Subrecords fuer spaetere Hauptrecords ueberlesen. Um dies
    zu vermeiden nutzen wir den lookahead aus: 

      - Datensatz auslesen
      - solange weiterlesen, bis wir naechsten Datensatz finden
      - Key/Parent der beiden Datensaetze (aktuell+sneak) 
        an Subreader reichen
      - der Subreader kennt jetzt (aufsteigende Reihenfolge der Keys 
        vorausgesetzt) einen Key, ab dem ein Eintrag ganz sicher 
        nicht mehr zu diesem Datensatz gehoeren kann. Der Subreader
        stoppt also, sobald ein key oder parent-key gefunden wurde,
        der groesser oder gleich dem sneak_key ist.

*/

  info(cout);


}
/*

g++ test.cpp -DHAVE_CONFIG_H -I../../math/src -I/home/hoeppner/my/local/include -I. -I.. -L/home/hoeppner/my/local/lib -llogtrace /home/hoeppner/my/local/lib/libutility.a -g -Wall -O0

*/
