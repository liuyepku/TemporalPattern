
/*  Copyright     : Copyright (C) 1999, 2000, 2001 Frank Hoeppner

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

/** \file logtrace.h
    \brief LogTrace library header, include this file to enable LogTrace.
    \author Frank Hoeppner <frank.hoeppner@ieee.org>

    This header file provides three types of macros: \li macros that
    print some information into the logfile (\c TRACE macros), \li
    macros that help to give the logfile some internal structure that
    reflect program flow (or calling depth) (\c LOG_... macros) and
    \li macros to log errors, warnings and invariant violations.

*/

#ifndef LOGTRACE_HEADER
#define LOGTRACE_HEADER
#endif // LOGTRACE_HEADER

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif


#ifdef LOGTRACE_DEBUG
#  ifdef LOGTRACE_LEVEL
     // everything is fine
#  else
#    define LOGTRACE_LEVEL 3
#  endif
#else
#  ifdef LOGTRACE_LEVEL
#    define LOGTRACE_DEBUG
#  else
#    define LOGTRACE_LEVEL 0
#  endif
#endif

#include <sstream>
#include <string>
using namespace std;


/** Abbreviation for constant string (char*) type. */

typedef char const * const char_cpc;

/** If we use namespaces (LOGTRACE_USE_NAMESPACES), we encapsulate
    LogTrace into the namespace logtrace and include standard headers
    by means of #include <x> instead of <x.h> */

#ifdef USE_NAMESPACES
#  include <iostream>
#else
#  include <iostream>
#endif // USE_NAMESPACES

#if (LOGTRACE_LEVEL>=2) 
#  include "logstream.hpp"
#  include <list>
#  include <map>
#  include <string>
#endif

//#ifdef USE_NAMESPACES
//   using namespace std;
   namespace logtrace {
//#endif // USE_NAMESPACES

#if (LOGTRACE_LEVEL>=2)
   extern char_cpc ALWAYS_ID;
   extern char_cpc NEVER_ID;
   extern char_cpc CONSTRUCTOR_ID;
   extern char_cpc DESTRUCTOR_ID;
   extern char_cpc LOGTRACE_BLOCK_TAG;
   extern char_cpc LOGTRACE_FUNCTION_TAG;
   extern char_cpc LOGTRACE_METHOD_TAG;
   extern char_cpc LOGTRACE_CONSTRUCTOR_TAG;
   extern char_cpc LOGTRACE_DESTRUCTOR_TAG;
   extern char_cpc LOGTRACE_ERROR_TAG;
   extern char_cpc LOGTRACE_WARNING_TAG;
   extern char_cpc LOGTRACE_VIOLATION_TAG;
   extern char_cpc LOGTRACE_INFO_OUT;
   extern char_cpc LOGTRACE_TEXT_BEGIN;
   extern char_cpc LOGTRACE_TEXT_END;

   extern char_cpc LOGTRACE_BEGIN_PREFIX;
   extern char_cpc LOGTRACE_END_PREFIX;
   extern char_cpc LOGTRACE_BEGIN_SUFFIX;
   extern char_cpc LOGTRACE_END_SUFFIX;
   extern char_cpc LOGTRACE_DATA_PREFIX;
   extern char_cpc LOGTRACE_DATA_SUFFIX;
#else
#  define LOGTRACE_ALWAYS_ID "always"
#  define LOGTRACE_NEVER_ID "never"
#  define LOGTRACE_BLOCK_TAG "block"
#  define LOGTRACE_FUNCTION_TAG "func"
#  define LOGTRACE_METHOD_TAG "func"
#  define LOGTRACE_CONSTRUCTOR_TAG "func"
#  define LOGTRACE_DESTRUCTOR_TAG "func"
#  define LOGTRACE_ERROR_TAG "error"
#  define LOGTRACE_WARNING_TAG "warning"
#  define LOGTRACE_VIOLATION_TAG "violation"
#  define LOGTRACE_ERROR_OUT "**[error:] "
#  define LOGTRACE_WARNING_OUT "**[warning:] "
#  define LOGTRACE_VIOLATION_OUT "**[violation:] "
#  define LOGTRACE_INFO_OUT "  [info:] "

#  define LOGTRACE_BEGIN_PREFIX " "
#  define LOGTRACE_END_PREFIX " "
#  define LOGTRACE_BEGIN_SUFFIX " "
#  define LOGTRACE_END_SUFFIX " "
#  define LOGTRACE_TEXT_BEGIN ""
#  define LOGTRACE_TEXT_END ""
#endif


#if (LOGTRACE_LEVEL>=2)

class IDMap
{
public:
  IDMap();
  ~IDMap();

  void add_id(char_cpc);
  void del_id(char_cpc);
  bool is_known(char_cpc,bool=false);
  bool is_known(char_cpc,char_cpc,char_cpc,bool=false);
  int get_value(char_cpc) const;
  void read(char_cpc);
  void usage(ostream&) const;

private:
  typedef map<string,int> map_type;
  map_type m_ids;
};

class LogPlain
{
public:
  typedef enum 
  { 
    LOG_INVALID=0, //< invalid log, logtrace has not been initialized
    LOG_PASSIVE=1, //< valid log, but does not cause output
    LOG_PASSFUNC=2,//< valid log, like PASSIVE but for functions
    LOG_NORMAL=3,  //< valid log, causes output
    LOG_ERROR=4,   //< valid log, problem report, prints also on screen
    LOG_FUNC=5     //< valid log, like LOG_NORMAL, contains function name
  } log_mode;

public:
  LogPlain(char_cpc tag,char_cpc fn, const int ln,log_mode m=LOG_NORMAL);
  virtual ~LogPlain();

  inline bool is_valid() const { return (m_active!=LOG_INVALID); }
  inline bool is_active() const { return (m_active>LOG_PASSFUNC); }
  inline bool is_error() const { return (m_active==LOG_ERROR); }
  inline bool is_func() const { return ((m_active==LOG_FUNC)||(m_active==LOG_PASSFUNC)); }
  inline void usage() {;}

  virtual void stackline(ostream&) const = 0;

protected:
  inline void write_fileloc(ostream&) const;
  inline void write_endtag(ostream&) const;

protected:
  char_cpc mp_filename; ///< source file name
  const int m_linenumber; ///< line of log in source file
  const int m_logline; ///< line in logfile where initline has been called
  string m_message; ///< message (e.g. warning message)
  char_cpc mp_tag; ///< XML tag used in logfile
  const log_mode m_active; ///< producing output?
};

class LogStack
{
public:
  LogStack();
  ~LogStack();

  void push(const LogPlain*);
  void pop();
  int maxdepth() const;
  bool is_active() const;
  void write_stack(ostream&) const;
  const LogPlain* get_last_func() const;

private:
  list<const LogPlain*> m_stack;
  int m_depth,m_max_depth;
};


extern bool g_init;
extern IDMap *gp_idmap;
extern LogStack *gp_stack;
extern logstream *gp_trace; 

#endif // LOGTRACE_LEVEL >= 2

/** Logtrace stream initialization.

    The InitLog class allocates and deletes the global trace
    stream. It should be instantiated by means of the LOGTRACE_INIT()
    macro. It is used in debug levels 2 and 3. */

#if (LOGTRACE_LEVEL>=2)

class InitLog
{
public:
  InitLog(char_cpc log,char_cpc id);
  InitLog(ostream*,char_cpc id);
  ~InitLog();
private:
  void init(ostream*,char_cpc);
private:
  ostream *mp_logfile;
};

#endif

/** Polymorphic write output using write method */

#if (LOGTRACE_LEVEL>=2)
template <class T>
class writemanip
{
public:
  writemanip(const T& t) : p_t(&t) {;}
  const T* const p_t;
};

template <class T>
inline ostream& operator<<(ostream& os,const writemanip<T>& w) 
{ w.p_t->write(os); return os; }

template <class T>
inline const writemanip<T> write(const T& t)
{ return writemanip<T>(t); }

#endif

/************************MACROS*****************************/

/** \def LOGTRACE_INIT(logfile,idfile)
    \brief Recommended macro to initialize logtrace.

    This macro initializes the LogTrace file. It can be placed within
    your main function, but also at the beginning of your variable
    declarations. The latter is useful, if your variables are classes
    and you use LogTrace within them. The ID file is read only in
    debug level 3, the logfile is created in debug level >= 2. 

    If you place \c TIMESTAMP=1 into your ID file, each line in the
    logfile will start with a time stamp, or with a line number if you
    put \c LINESTAMP=1. When using time stamps, an additional \c
    ABSTIME=1 provides you with absolute time values, otherwise you
    will get relative time starting with 0 at program initialization.
    
    Sometimes, when it is hard to locate a bug, enabling a large
    number of trace commands leads to tons of logfile data before the
    invariant violation actually occurs. You can reduce the size of
    your logfile by putting \c HISTORY=n into the ID file. Then,
    instead of writing all data into the logfile, the lines will be
    buffered in an internal ring-buffer of size \c n kilobytes. Only
    errors, warnings, and invariant violations will cause some output
    in the logfile. Together with the error message, you get the the
    content of the history buffer which contains the output of your
    trace commands just before the error occured. Remove the HISTORY
    entry to return to full trace mode. */

#if (LOGTRACE_LEVEL==3)

#  define LOGTRACE_INIT(logfile,idfile) \
     InitLog FIRSTLOGTRACEINSTANCE(logfile,idfile);

#elif (LOGTRACE_LEVEL==2)

#  define LOGTRACE_INIT(logfile,idfile) \
     InitLog FIRSTLOGTRACEINSTANCE(logfile,NULL);

#else

#  define LOGTRACE_INIT(logfile,idfile)

#endif

/** \def LOG_CONSTRUCTOR(namespace,class,command)
    \brief Declare constructor log.

    This macro "marks" the current block as the definition of a
    constructor namespace::class. The arguments of the constructor can
    be specified by means of the command parameter. Additional output
    in the logfile during the lifetime of this block is automatically
    indented for better readability. Constructor output is printed
    into the logfile only if the ID "Constructor" is provided in the
    ID file. */

#if (LOGTRACE_LEVEL>=2)

class LogCtor
  : public LogPlain
{
public:
  LogCtor(char_cpc ns,char_cpc cl,char_cpc fn, const int ln);
  virtual ~LogCtor();
  virtual void stackline(ostream&) const;
protected:
  inline void write_func(ostream&) const;
private:
  char_cpc mp_namespace;
  char_cpc mp_class;
};

#  define LOG_CONSTRUCTOR(ns,cl,msg) \
    if (g_init) { gp_trace->mark(); (*gp_trace) << msg; } \
    LogCtor CLOG(ns,cl,__FILE__,__LINE__); CLOG.usage();

#else

#define LOG_CONSTRUCTOR(ns,cl,msg)

#endif

/** \def LOG_DESTRUCTOR(ns,cl)
    \brief Declare destructor log.

    This macro "marks" the current block as the definition of a
    destructor of class \a cl (in namespace \a ns). Additional output
    in the logfile during the lifetime of this block is automatically
    indented for better readability. Destructor output is printed into
    the logfile only if the ID "Destructor" is provided in the ID
    file. */

#if (LOGTRACE_LEVEL>=2)

class LogDtor
  : public LogPlain
{
public:
  LogDtor(char_cpc ns,char_cpc cl,char_cpc fn, const int ln);
  virtual ~LogDtor();
  virtual void stackline(ostream&) const;
protected:
  inline void write_func(ostream&) const;
private:
  char_cpc mp_namespace;
  char_cpc mp_class;
};

#  define LOG_DESTRUCTOR(ns,cl) \
    LogDtor DLOG(ns,cl,__FILE__,__LINE__); DLOG.usage();

#else

#define LOG_DESTRUCTOR(ns,cl)

#endif

/** \def LOG_FUNCTION(namespace,function,command)
    \brief Declare function log.

    This macro "marks" the current block as the implementation of a
    function namespace::function. The arguments of the function can be
    specified by means of the command parameter. Additional output in
    the logfile during the lifetime of this block is automatically
    indented for better readability. 
    
    \attention The arguments in the \c command parameter are always printed
    into a stream buffer and stored for later access (e.g. log stack printing
    in case of an error). Avoid writing large data structures, because this may
    cause high computational overhead. If you need to print much information,
    use TRACE commands after the LOG_..., such that they will be printed only
    in case the respsonsible ID is active. */

#if (LOGTRACE_LEVEL>=2)

class LogFunc
  : public LogPlain
{
public:
  LogFunc(char_cpc tag,char_cpc ns,char_cpc cl,char_cpc mt,char_cpc fn, const int ln);
  virtual ~LogFunc();
  virtual void stackline(ostream&) const;
protected:
  inline void write_func(ostream&) const;
private:
  char_cpc mp_namespace;
  char_cpc mp_class;
  char_cpc mp_function;

  friend class LogError;
  friend class LogViolation;
};

#  define LOG_FUNCTION(ns,fn,msg) \
    if (g_init) { gp_trace->mark(); (*gp_trace) << msg; } \
    LogFunc FLOG(LOGTRACE_FUNCTION_TAG,ns,NULL,fn,__FILE__,__LINE__); FLOG.usage();

#else

#  define LOG_FUNCTION(ns,fn,msg)

#endif

/** \def LOG_METHOD(namespace,class,method,command)
    \brief Declare method log.

    This macro "marks" the current block as the implementation of a
    function namespace::class::method. The arguments of the method can
    be specified by means of the command parameter. Additional output
    in the logfile during the lifetime of this block is automatically
    indented for better readability. */

#if (LOGTRACE_LEVEL>=2)

#  define LOG_METHOD(ns,cl,fn,msg) \
    if (g_init) { gp_trace->mark(); (*gp_trace) << msg; } \
    LogFunc MLOG(LOGTRACE_METHOD_TAG,ns,cl,fn,__FILE__,__LINE__); MLOG.usage();

#else

#  define LOG_METHOD(ns,cl,fn,msg)

#endif

/** \def LOG_BLOCK(blockid,command)
    \brief Declare arbitrary log.

    This macro can be used to "mark" an arbitrary { ... } block,
    e.g. the implementation of a for-loop. Some "parameters" of the
    block can be specified by means of the \a msg
    parameter. Additional output in the logfile during the lifetime of
    this block is automatically indented for better readability. */

#if (LOGTRACE_LEVEL>=2)

class LogBlock
  : public LogPlain
{
public:
  LogBlock(char_cpc id,char_cpc fn, const int ln);
  virtual ~LogBlock();
  virtual void stackline(ostream&) const;
private:
  char_cpc mp_id;
};

#  define LOG_BLOCK(id,msg) \
    if (g_init) { gp_trace->mark(); (*gp_trace) << msg; } \
    LogBlock BLOG(id,__FILE__,__LINE__); BLOG.usage();

#else

#  define LOG_BLOCK(id,msg)

#endif

/** \def ERROR(command)
    \brief Error message.

    This macros prints an error message on the standard error stream
    (all debug levels) and additionally writes the error and log stack
    into the logfile (debug level >=2). If the debug level is 0, the
    file location (filename and line number) is omitted from the
    output. 

    (If there is already an error macro defined, we define \c LT_ERROR
    to be the Logtrace error macro.) */

#if (LOGTRACE_LEVEL>=2) 

class LogError
  : public LogPlain
{
public:
  LogError(char_cpc id,char_cpc fn, const int ln);
  virtual ~LogError();
  virtual void stackline(ostream&) const;
};

#  define LOGTRACE_ERROR(msg) \
    if (g_init) { gp_trace->mark(); (*gp_trace) << msg; \
    LogError(LOGTRACE_ERROR_TAG,__FILE__,__LINE__); }

#elif (LOGTRACE_LEVEL==1)

#  define LOGTRACE_ERROR(msg) \
    cerr << LOGTRACE_ERROR_OUT << msg \
         << " (" << __FILE__ << ':' << __LINE__ << ')' << endl;

#else

#  define LOGTRACE_ERROR(msg) \
    cerr << LOGTRACE_ERROR_OUT << msg << endl;

#endif

#ifndef ERROR
#  define ERROR(msg) LOGTRACE_ERROR(msg)
#else
#  define LT_ERROR(msg) LOGTRACE_ERROR(msg)
#endif

/** \def WARNING(command)
    \brief Warning message.

    This macros prints a warning message on the standard error stream
    (all debug levels) and additionally writes the warning (including
    the call stack) into the logfile (debug level >=2). If the debug
    level is 0, the file location (filename and line number) is
    omitted from the output. 

    (If there is already a warning macro defined, we define \c
    LT_WARNING to be the Logtrace warning macro.) */

#if (LOGTRACE_LEVEL>=2) 

#  define LOGTRACE_WARNING(msg) \
    if (g_init) { gp_trace->mark(); (*gp_trace) << msg; \
    LogError(LOGTRACE_WARNING_TAG,__FILE__,__LINE__); }

#elif (LOGTRACE_LEVEL==1)

#  define LOGTRACE_WARNING(msg) \
    cerr << LOGTRACE_WARNING_OUT << msg \
         << " (" << __FILE__ << ':' << __LINE__ << ')' << endl;

#else

#  define LOGTRACE_WARNING(msg) \
    cerr << LOGTRACE_WARNING_OUT << msg << endl;

#endif

#ifndef WARNING
#  define WARNING(msg) LOGTRACE_WARNING(msg)
#else
#  define LT_WARNING(msg) LOGTRACE_WARNING(msg)
#endif

/* VIOLATION */

#if (LOGTRACE_LEVEL>=2) 

class LogViolation
  : public LogPlain
{
public:
  LogViolation(char_cpc tag,bool c,char_cpc cond,char_cpc fn, const int ln);
  virtual ~LogViolation();
  virtual void stackline(ostream&) const;
private:
  char_cpc mp_text;
};

#  define VIOLATION(c,t,msg) \
    if ((!(c))&&g_init) { gp_trace->mark(); (*gp_trace) << msg; \
    LogViolation VLOG(LOGTRACE_VIOLATION_TAG,!(c),t#c,__FILE__,__LINE__); }
#  define LOG_VIOLATION(c,t,msg) \
    if (g_init) { gp_trace->mark(); (*gp_trace) << msg; } \
    LogViolation VLOG(LOGTRACE_VIOLATION_TAG,!(c),t#c,__FILE__,__LINE__); VLOG.usage();

#elif (LOGTRACE_LEVEL==1)

#  define VIOLATION(c,t,msg) \
    if (!(c)) cerr << LOGTRACE_VIOLATION_OUT \
                   << t << #c << "; " << msg \
                   << " (" << __FILE__ << ':' << __LINE__ << ')' << endl;
#  define LOG_VIOLATION(c,t,msg)

#else

#  define VIOLATION(c,t,msg) 
#  define LOG_VIOLATION(c,t,msg)

#endif

/** \def INVARIANT(c,msg)
    \brief Declare invariant.

    If the specified (boolean) invariant \c c is violated (evaluates
    to false), a message \c msg is printed on the standard error
    stream (debug level >=1) and is additionally written -- together
    with the log stack -- into the logfile (debug level >=2). */

#  define INVARIANT(c,msg) VIOLATION(c,"invariant ",msg)
#  define LOG_INVARIANT(c,msg) LOG_VIOLATION(c,"invariant ",msg)

/** \def PRECONDITION(c,msg)
    \brief Declare precondition.

    Does the same as INVARIANT(c,msg), but should be used to check
    conditions on the arguments of an algorithm.  */

#  define PRECONDITION(c,msg) VIOLATION(c,"precondition ",msg)
#  define LOG_INVARIANT(c,msg) LOG_VIOLATION(c,"invariant ",msg)

/** \def POSTCONDITION(c,msg)
    \brief Declare postcondition.

    Does essentially the same as INVARIANT(c,msg), but should be used
    to check conditions on the results of an algorithm. */

#  define POSTCONDITION(c,msg) VIOLATION(c,"postcondition ",msg)

/** \def ASSUME(c,msg)
    \brief Declare assumption.

    Behaves identical to INVARIANT(c,cmd), but the violation of an
    assumption does not cause a message in debug levels below level
    2. Whenever the programmer is aware of a limitation, for example,
    when writing down a loop statement he implicitly assumes that a
    variable is positive, then the programmer should write down an
    assumption. Right now, it does not seem necessary to handle other
    cases, but may be the programmer is wrong. If the assumption turns
    out to be wrong, it is very useful if the ASSUME(c,msg) macro
    raises a message, because from now on something happens that has
    not been foreseen at the time of writing (and often leads into
    erroneous results). */


#if (LOGTRACE_LEVEL>=2) 

#  define ASSUME(c,msg) VIOLATION(c,"assume ",msg)

#else

#  define ASSUME(c,msg)

#endif

/** \def TRACE(msg)
    \brief Write into logstream

    The macro writes the command \a msg into the log file, if the
    debug level is 3 and the enclosing Log is active. Otherwise, no
    output is produced.

    (If there is already a TRACE macro defined, we define LT_TRACE to
    be the Logtrace TRACE macro.)

    \attention Problems arise if you use these macros with static
    template class attributes. In TRACE_VAR(myclass<int,float>::m_val)
    the colon, which is part of the template class name, is
    interpreted as a macro argument separator. You will get an error
    message from the C++ preprocessor claiming that you use the macro
    with 2 instead of 1 arguments. Consider a typedef for your
    template class. */

#if (LOGTRACE_LEVEL==3)

#  define LOGTRACE_TRACE(msg) \
    if ((g_init) && (gp_stack->is_active())) \
      { (*gp_trace) << LOGTRACE_TEXT_BEGIN << msg << LOGTRACE_TEXT_END << endl; }

#else

#  define LOGTRACE_TRACE(msg)

#endif

#ifndef TRACE
#  define TRACE(msg) LOGTRACE_TRACE(msg)
#else
#  define LT_TRACE(msg) LOGTRACE_TRACE(msg)
#endif

/** \def TRACE_ID(id,msg)
    \brief Write conditionally into logstream

    The macro writes the command \a msg into the log file, if the
    debug level is 3 and the ID \a id has been found in the
    ID-map. Otherwise, no output is produced. See also TRACE(msg). */

#if (LOGTRACE_LEVEL==3)

#  define TRACE_ID(id,msg) \
    if ((g_init) && (gp_idmap->is_known(id,true))) \
      { (*gp_trace) << msg << endl; }

#else

#  define TRACE_ID(id,msg)

#endif

/** \def TRACE_TAG(tag,msg)
    \brief Write XML tag into logstream

    The macro writes the command \a msg into the log file, if the
    debug level is 3 and \a tag has been found in the
    ID-map. Otherwise, no output is produced. In contrast to
    TRACE_ID(id,msg), the output is in an XML-style <tag msg>. See
    also TRACE(msg). */

#if (LOGTRACE_LEVEL==3)

#  define TRACE_TAG(tag,msg) \
    if ((g_init) && (gp_idmap->is_known(tag,true))) \
      { (*gp_trace) << '<' << tag << "> " << LOGTRACE_TEXT_BEGIN << msg << LOGTRACE_TEXT_END << " </" << tag << '>' << endl; }

#else

#  define TRACE_TAG(tag,msg)

#endif

/** \def TRACE_VAR(var)
    \brief Write variable into logstream

    The macro writes the name of the variable \a var and its content
    into the log file, if the debug level is 3 and the the enclosing
    Log is active. Otherwise, no output is produced. See also
    TRACE(msg). */

#if (LOGTRACE_LEVEL==3)

#  define TRACE_VAR(var) \
    if ((g_init) && (gp_stack->is_active())) \
      { (*gp_trace) << LOGTRACE_DATA_PREFIX << "trace " << \
        #var << "='" << var << "'" << LOGTRACE_DATA_SUFFIX << endl; }

#else

#  define TRACE_VAR(var)

#endif

/** \def ID_IS_KNOWN(id) 
    \brief Check for an ID to be known.

    This macro can be used to check whether an ID is currently in the
    map of active IDs or not. The macros always returns false if the
    debug level is <= 2, because such a map is maintained only in
    debug level 3. */

#if (LOGTRACE_LEVEL>=2)

#  define ID_IS_KNOWN(id) \
    ((g_init)?gp_idmap->is_known(id):false)

#else

#  define ID_IS_KNOWN(id) false

#endif

/** \def COND_[SET|DEL]_ID(cond,id), COND_READ_IDS(cond,file)

    This set of macros can be used to change the list of active
    IDs. If the condition (1st parameter) is evaluated to true, the
    specified ID (in case of COND_[SET|DEL]_ID) is added or removed
    from the list or the content of the specified ID file (in case of
    COND_READ_IDS) added. Changing the ID list makes sense only in
    debug level >= 2 (because below level 2 the output does not depend
    on the list of IDs). */

#if (LOGTRACE_LEVEL>=2)

#  define COND_SET_ID(cond,id) \
    if ((cond) && (g_init)) { gp_idmap->add_id(id); }

#  define COND_DEL_ID(cond,id) \
    if ((cond) && (g_init)) { gp_idmap->del_id(id); }

#  define COND_READ_IDS(cond,file) \
    if ((cond) && (g_init)) { gp_idmap->read(file); }
 
#else

#  define COND_SET_ID(cond,id)
#  define COND_DEL_ID(cond,id)
#  define COND_READ_IDS(cond,file)

#endif

/** \def INFO(msg)
    \brief Print information line.
  
    Information that might be interesting for the end-user can be
    printed on the error stream (and the log file in case of an debug
    level >= 2) by means of the INFO macro.  */

#if (LOGTRACE_LEVEL >= 2)

#  define INFO(msg) \
    { TRACE_ID(ALWAYS_ID,"info:"<<msg); \
    cerr << LOGTRACE_INFO_OUT << msg << endl; }

#else

# define INFO(msg) \
    { cerr << LOGTRACE_INFO_OUT << msg << endl; }

#endif

/** \def INFO_PROGRESS(pb)
    \brief Print progress bar (see utility library).
  
    Print progress bar if it has changed. No output to Tracefile. */

#define INFO_PROGRESS(pb) \
   { if ((pb)()) cerr << LOGTRACE_INFO_OUT << (pb) << '\r' << flush; }

/** \def DO_NOT_PASS
    \brief Mark unreachable code.

    This macro is identical to ERROR("program execution never reaches
    this point") and should be used whenever the programmer assumes
    that a code portion is unreachable. For example, if you think that
    you have handled all possible cases in a switch statement over an
    integer value, you could add DO_NOT_PASS in the default branch. */

#  define DO_NOT_PASS \
     ERROR("program execution shall never reach this point")

/** \def VALID_POINTER(p)
    \brief Test for NULL pointer.

    This macro is an abbreviation for 
    INVARIANT((p)!=NULL,"illegal NULL pointer"). */

#  define VALID_POINTER(p) \
     INVARIANT((p)!=NULL,"illegal NULL pointer")

/** \def INVARIANT_THROW
    \brief Additionally throw exception if invariant is violated.

    Invariant violation is not reported on standard error stream, but
    in logfile. Exception of type \c Excep is thrown, which is
    initialized with the same message that is written into the
    logfile. This macro is useful if failure of a subroutine is not
    crucial for the whole program. For instance, if the routine is
    called frequently and during testing it does not matter if from
    time to time no results are produced, it makes sense to exit from
    the routine (via exception handling) rather than just reporting
    that something is wrong (and running into a core dump). */

#if (LOGTRACE_LEVEL>=2) 

void set_error_to_cerr(bool);
#  define INVARIANT_THROW(b,Excep,msg)  \
	if ((!(b))&&g_init) { \
	gp_trace->mark(); (*gp_trace) << msg; string text; gp_trace->get(text);\
	Excep e(text); set_error_to_cerr(false); \
	LogViolation VLOG(LOGTRACE_VIOLATION_TAG,!(b),"violation"#b,__FILE__, \
__LINE__); VLOG.usage();  set_error_to_cerr(true); throw e; }

#else
#   define INVARIANT_THROW(b,Excep,msg)  \
   {                                     \
   if (!(b))                             \
      {                                  \
      string st;                         \
      ostringstream str(st);             \
      str << msg << '\0';                \
      Excep e(st); throw e;              \
      }                                  \
   }

#endif // #if (LOGTRACE_LEVEL>=2) 

/** \def INVARIANT_THROW_RELEASE
    \brief Same as INVARIANT_THROW, except that nothing happens in debug level<=1
*/

#if (LOGTRACE_LEVEL>=1) 
#  define INVARIANT_THROW_RELEASE(b,Excep,msg)  INVARIANT_THROW(b,Excep,msg)
#else
#  define INVARIANT_THROW_RELEASE(b,Excep,msg)  
#endif


//#ifdef USE_NAMESPACES
} // namespace logtrace
//#endif // USE_NAMESPACES

//#endif // LOGTRACE_HEADER
