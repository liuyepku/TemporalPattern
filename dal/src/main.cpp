using namespace Data_Analysis_Library;
 
int main()
  {
  InitialLog Entry("x.id","x.log");

  // Frage: ist das FCM_ noetig? Ich denke nicht. Die Alg. koennen ja nun auch
  // nacheinander sequentiell aufgerufen werden. Dazwischen koennten die
  // label ggf. umbenannt werden.
/*
  id_type ARGS_COLLECTION("console",DAL_XML_SOURCE,DAL_MODE_RW);
  id_type DEFAULT_COLLECTION("default.ddl",DAL_XML_SOURCE);

  id_type ARGS("args",DAL_COLLECTION,ARGS_COLLECTION);
  id_type ARGS_FUZZIFIER("fuzzifier",DAL_REAL,ARGS);
  
  id_type DATA("data",DAL_COLLECTION,DEFAULT_COLLECTION);
  id_type DATA_WEIGHT("weight",DAL_REAL,DATA);
  id_type DATA_VECTOR("vector",DAL_TUPLE,DATA);
  id_type DATA_LEVEL("level",DAL_INT,DATA);

  id_type EXTRA("extra",DAL_COLLECTION,DEFAULT_COLLECTION);
   
//  DAL_Base::write(cout);


  //fuer cells:
  //DAL_Base::change_tag("data$weight","gewicht"); <= nur sinnig VOR DEM einlesen
  //DAL_Base::change_souce("data$weight",mysource);
  //dazu von hinten anfangen, in dictionary suchen, naechstes wort suchen
  //und pruefen ob erstes in collection des zweiten war, etc.

  real_type fuzzifier(2);
  real_type weight;
  tuple_type vector;
  int level;

  {
    DAL_XML_StdIO_Stream source(ARGS);
    source.attach(ARGS_FUZZIFIER,fuzzifier);
    source.next();
    fuzzifier=8;
    source.close();
  }
  cout << "fuzzifier " <<fuzzifier << endl;

// beim ATTACH weiteres Attribut: r/w/rw 

  DAL_XML_File_Stream data(DATA,true);
  data.attach(DATA_WEIGHT,weight);
  data.attach(DATA_VECTOR,vector);
  data.attach(DATA_LEVEL,level);
  
  while (data.next())
    {
    cout << weight << vector << level << endl;
    }
  data.close();
*/

  string_type filename("test2.ddl");
  
  id_type XMLFILE  (filename,DAL_XML_SOURCE,DAL_MODE_RW);

  // data collection
  id_type DOCUMENT ("document",DAL_COLLECTION,XMLFILE,DAL_MODE_RW);
  id_type DATA     ("data",DAL_COLLECTION,DOCUMENT,DAL_MODE_RW);
  
//  id_type CLASS_DOMAIN("class",DAL_DOMAIN);
//  id_type DATA_CLASS("class",CLASS_DOMAIN,DATA);
//  id_type DATA_PREDICTED("predict",CLASS_DOMAIN,DATA);
  id_type DATA_CLASS("class",DAL_CATEGORY,DATA);
  id_type DATA_PREDICTED("predict",DAL_CATEGORY,DATA);

  // summary collection
  id_type SUMMARY  ("summary",DAL_COLLECTION,DOCUMENT,DAL_MODE_RW);
  id_type CONFUSION("confusion",DAL_MATRIX,SUMMARY,DAL_MODE_WO);

// folgende iteration iteriert ueber ALLE daten in ALLEN documents

  category_type given_class(0),predicted_class(0);
  matrix_type confusion;

  // iterate over data set
  {
  const path_type path("ddl/document/data",DAL_PREFIX_ANY);
  iterator_type iter(path);
  iter.attach(DATA_CLASS,given_class);
  iter.attach(DATA_PREDICTED,predicted_class);

confusion.adjust(6,6);
  while (iter.next())
    {
    ++confusion(given_class,predicted_class);
    }
cerr << confusion << endl;
  }

/*
  {
  iterator_type iter;
  iter.attach(DOCUMENT);
  iter.
  
path list<id_type> + list<index,level>  
  }
  
  }
  */

/*
erste anwendung? -- echtes one-pass
stat
vector in statbox, boundbox
ausgabe von <minimum> <maximum> <mean> <variance>

danach eventuell normalize -- echtes one-pass, wenn summary gegeben

zweite anwendung?
confusion matrix
felder <class> + <predicted> auswerten
<confusion> matrix rauswerfen
=> gut fuer demo, <class> und <predicted> mal aus derselben Datei, mal
verschiedene Dateien

jetzt:
 file1 : daten + class
 file2 : predicted
 file3 : confusion matrix

a.out -Itest.ddl -Oout.ddl
data$class=klasse@test1.ddl data$predict=klasse@test2.ddl output$confusion=confusion@result.ddl



cini|fcm|gk|pick

default: I+O=console
cini -Idata.ddl -Oconsole

in output von cini steht referenz auf alte dateien, wenn diese nicht angefasst wurden
<data> = data@data.ddl

<ddl>
  <job>
    <data> .. </data>
  </job>
</ddl>

statt iterator_type Kapselung: 
  innerhalb eines Accessors instanziierung neuer Accessors, wenn eine
  file= directive kommt.

cini:
<prolog>
  <domain index="classes"> <classes A> <classes B> <classes C> </domain>
</prolog>

2 Fälle: input.ddl enthält nur 1x Daten, die in jeder analysis benutzt werden
     oder entält für jede analyse neue daten, die fortlaufend benutzt werden
<ddl>
  <import analysis$data file=input.ddl tag=ddl$analysis$data>
  <analysis>
    <data file=input.ddl tag=ddl$data>
    <prot> ... </prot>
  </analysis>
  <analysis>
    <data see file ...>
    <prot> ... </prot> 2x
  </analysis>
</ddl>

beim scannen einer xml datei:
  z.b. <args> ... </args> oder <fuzzifier> gefunden => wird aber nicht benutzt 
  bei flachen werten (keine collections) in unrecognized kopieren+wieder ausgeben
  bei ungenutzten collections: <args file=x.xml tag=ddl§analysis§data index=1 level=1> // selbe "position", nur andere datei

locator = pfad mit nummern (index) fuer jede collection
wenn index fehlt, dann gleicher wie aktueller pfad
also:  ddl/analysis.4/args/fuzzifier  in der 4. <analysis> der <args> <fuzzifer> wert
oder:  ddl/analysis/data.2  das zweite datum in der correspondierenden analysis
also wenn cini -r3:4 mit ddl aufgerufen wird, die 2 daten enthaelt, dann
<ddl>
  <analysis index=1>
    <data locator=ddl/analysis.1/data@data.ddl>   <= wenn locator, dann kein index
    <prot> .. </prot>
  </analysis>
  <analysis index=2>
    <data locator=ddl/analysis.1/data@data.ddl>
    <prot> .. </prot>
    <prot> .. </prot>
  </analysis>
  <analysis index=3>
    <data locator=ddl/analysis.2/data@data.ddl>
    <prot> .. </prot>
  </analysis>
  ...
</ddl>
    

assrules

*/

