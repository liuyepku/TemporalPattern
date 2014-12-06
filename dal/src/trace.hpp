
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

/** \file   trace.hpp
    \brief  Compatibility file for prior releases of Logtrace. 
    \author Frank Hoeppner <frank.hoeppner@ieee.org>

    In prior releases of Logtrace, the interface was defined in the
    file trace.hpp. Thus, this file assures (limited) downward
    compatibility. If you start a new project, include logtrace.h
    instead. */

#ifndef TRACE_HEADER
#define TRACE_HEADER

#include "logtrace.h"
#include "stlstream.hpp"

#define SOURCELOC "",0

/** Trace functions */

#ifdef LOGTRACE_DEBUG

#ifdef USE_NAMESPACES
   using namespace std;
   using namespace logtrace;
#endif

template <class DATA> inline void 
  trace(char_cpc i,char_cpc t,const DATA& d) 
  { TRACE_ID(i,t << ' ' << d); }
template <class DATA> inline void 
  tracew(char_cpc i,char_cpc t,const DATA& d) 
  { TRACE_ID(i,t << ' ' << write(d)); }
template <class DATA> inline void 
  trace(char_cpc t,const DATA& d) 
  { LOGTRACE_TRACE(t << ' ' << d); }
template <class DATA> inline void 
  tracew(char_cpc t,const DATA& d) 
  { LOGTRACE_TRACE(t << ' ' << write(d)); }
template <class DATA> inline void 
  trace_id(char_cpc i,char_cpc t,const DATA& d) 
  { TRACE_ID(i,t << ' ' << d); }
template <class DATA> inline void 
  tracew_id(char_cpc i,char_cpc t,const DATA& d) 
  { TRACE_ID(i,t << ' '  << write(d)); }
inline void trace_id(char_cpc i,char_cpc t) 
  { TRACE_ID(i,t); }
inline void trace(char_cpc t) 
  { LOGTRACE_TRACE(t); }

#else

template <class DATA> inline void 
  trace(char_cpc,char_cpc,const DATA&) {}
template <class DATA> inline void 
  tracew(char_cpc,char_cpc,const DATA&) {}
template <class DATA> inline void 
  trace(char_cpc,const DATA&) {}
template <class DATA> inline void 
  tracew(char_cpc,const DATA&) {}
template <class DATA> inline void 
  trace_id(char_cpc,char_cpc,const DATA&) {}
template <class DATA> inline void 
  tracew_id(char_cpc,char_cpc,const DATA&) {}
inline void trace_id(char_cpc,char_cpc) {}
inline void trace(char_cpc) {}

#endif

/** Invariant functions */

#ifdef LOGTRACE_DEBUG

template <class DATA> inline void 
  invariant(bool b,char_cpc t,const DATA& d) 
  { INVARIANT(b,t << ' ' << d); }
template <class DATA> inline void 
  invariantw(bool b,char_cpc t,const DATA& d) 
  { INVARIANT(b,t << ' ' << write(d)); }
template <class DATA> inline void 
  invariant(bool b,char_cpc t,const DATA& d,char_cpc f,int l)
  { if ((!b)&&g_init) { gp_trace->mark(); (*gp_trace) << t << ' ' << d;
  LogViolation VLOG(LOGTRACE_VIOLATION_TAG,!b,"invariant",f,l); } }
template <class DATA> inline void 
  invariantw(bool b,char_cpc t,const DATA& d,char_cpc f,int l)
  { if ((!b)&&g_init) { gp_trace->mark(); (*gp_trace) << t << ' ' << write(d);
    LogViolation VLOG(LOGTRACE_VIOLATION_TAG,!b,"invariant",f,l); } }
#define invariant_try(invar, text, ret_value) \
  if (!(invar)) \
    { \
    invariant(false,text); \
    return ret_value; \
    }

#define DO_IF_ID_IS_KNOWN(ID, anw) \
   if (id_is_known (ID)) \
     { \
     anw; \
     }
inline const char* MakeTraceInvarString(char * p) {return p;}
inline const char* MakeTraceInvarString(char const* p) {return p;}
inline void invariant(bool b,char_cpc t)
  { INVARIANT(b,t); }
inline void invariant(bool b,char_cpc t,char_cpc d)
  { INVARIANT(b,t << ' ' << d); }
inline void invariant(bool b,char_cpc t,char_cpc f,int l)
  { if ((!b)&&g_init) { gp_trace->mark(); (*gp_trace) << t;
    LogViolation VLOG(LOGTRACE_VIOLATION_TAG,!b,"invariant",f,l); } }

#else

template <class DATA> inline void 
  invariant(bool,char_cpc,const DATA&) {}
template <class DATA> inline void 
  invariantw(bool,char_cpc,const DATA&) {}
template <class DATA> inline void 
  invariant(bool,char_cpc,const DATA&,char_cpc,int) {}
template <class DATA> inline void 
  invariantw(bool,char_cpc,const DATA&,char_cpc,int) {}
#define invariant_try(invar, text, ret_value) 
#define DO_IF_ID_IS_KNOWN(ID, anw) 
inline void invariant(bool,char_cpc) {}
inline void invariant(bool,char_cpc,char_cpc) {}
inline void invariant(bool,char_cpc,char_cpc,int) {}

#endif

/** error & warning functions */

#ifdef LOGTRACE_DEBUG

inline void error(char_cpc t) 
  { LOGTRACE_ERROR(t); }
inline void error(char_cpc t,char_cpc d) 
  { LOGTRACE_ERROR(t << ' ' << d); }
inline void error(char_cpc t,char_cpc f,int l) 
  { if (g_init) { gp_trace->mark(); (*gp_trace) << t;
    LogError(LOGTRACE_ERROR_TAG,f,l); } }
template <class DATA> inline void 
  error(char_cpc t,const DATA& d,char_cpc f=NULL,int l=0) 
  { if (g_init) { gp_trace->mark(); (*gp_trace) << t << ' ' << d;
    LogError(LOGTRACE_ERROR_TAG,f,l); } }
inline void warning(char_cpc t) 
  { LOGTRACE_WARNING(t); }
inline void warning(char_cpc t,char_cpc d) 
  { LOGTRACE_WARNING(t << ' ' << d); }
inline void warning(char_cpc t,char_cpc f,int l) 
  { if (g_init) { gp_trace->mark(); (*gp_trace) << t; 
    LogError(LOGTRACE_WARNING_TAG,f,l); } }
inline int no_of_warnings() 
  { warning("no_of_warnings not yet supported"); return 0; }
inline int no_of_errors() 
  { warning("no_of_errors not yet supported"); return 0; }

#else

inline void error(char_cpc) {}
inline void error(char_cpc,char_cpc) {}
inline void error(char_cpc,char_cpc,int) {}
template <class DATA> inline void error(char_cpc,const DATA&,char_cpc f=NULL,int l=0) {}
inline void warning(char_cpc) {}
inline void warning(char_cpc,char_cpc) {}
inline void warning(char_cpc,char_cpc,int) {}
inline int no_of_warnings() { return 0; }
inline int no_of_errors() { return 0; }

#endif

#ifdef LOGTRACE_DEBUG

inline void* trace_look_at( char_cpc f) 
  { COND_READ_IDS(true,f); return NULL; }
inline ostream& trspc( ostream& os ) { return os; }
inline ostream& trace_stream() { return (*gp_trace); }
inline bool id_is_known(char_cpc id) { return ID_IS_KNOWN(id); }
#define FUNCTION_LOG(id,cmd) LOG_BLOCK(id,cmd)
#define FUNCLOG(id) LOG_BLOCK(id,id)
#define CONSTRUCTOR_LOG(id,txt,tp) LOG_BLOCK(id,"Constructor "<<txt)
#define DESTRUCTOR_LOG(id,txt,tp) LOG_BLOCK(id,"Destructor "<<txt)

class CLog 
{
public:
  CLog(char_cpc id, char_cpc txt, void*)
   : pblock(NULL)
   { if ((g_init)&&(ID_IS_KNOWN(CONSTRUCTOR_ID))) 
       { gp_trace->mark(); (*gp_trace) << "Constructor " << txt; 
         pblock =  new LogBlock(id,__FILE__,__LINE__); }
   }
  ~CLog() { delete pblock; }
private:
  LogBlock*  pblock;
};

class DLog 
{
public:
  DLog(char_cpc id, char_cpc txt, void*)
   : pblock(NULL)
   { if ((g_init)&&(ID_IS_KNOWN(DESTRUCTOR_ID))) 
       { gp_trace->mark(); (*gp_trace) << "Destructor " << txt; 
     	 pblock =  new LogBlock(id,__FILE__,__LINE__); }
   }
  ~DLog() {delete pblock;}
private:
  LogBlock*  pblock;
};

class Log 
{
public:
  Log(char_cpc id, char_cpc txt)
    : pblock(NULL)
    { if (g_init) 
        { gp_trace->mark(); (*gp_trace) << txt; 
          pblock =  new LogBlock(id,__FILE__,__LINE__); }
    }
  ~Log() {delete pblock;}
private:
  LogBlock*  pblock;
};

#else

inline void* trace_look_at( char_cpc f) { return NULL; }
inline ostream& trspc( ostream& os ) { return os; }
inline ostream& trace_stream() { return cerr; }
inline bool id_is_known(char_cpc id) { return false; }
#define FUNCTION_LOG(id,txt)
#define FUNCLOG(id)
#define CONSTRUCTOR_LOG(id,txt,tp)
#define DESTRUCTOR_LOG(id,txt,tp)

class CLog 
{
public:
  inline CLog(char_cpc id, char_cpc txt, void* tp) { }
};
class DLog 
{
public:
  inline DLog(char_cpc id, char_cpc txt, void* tp) { }
};
class Log {
public:
  inline Log(char_cpc id, char_cpc txt) { }
};

#endif

inline void set_error_stream(ostream *) {}
inline void set_error_stream(ostream *,ostream *) {}

#ifdef LOGTRACE_DEBUG

class InitialLog
  {
  public:
    InitialLog(char_cpc a,char_cpc b) : m_il(b,a) {;}
    InitialLog(char_cc a,ostream *b) : m_il(b,a) {;}
    ~InitialLog() {;}
  private:
    InitLog m_il;
  };

#else

class InitialLog
  {
  public:
    InitialLog(char_cpc a,char_cpc b) {;}
    InitialLog(char_cpc,ostream *) {;}
    ~InitialLog() {;}
  };

#endif

#endif /* TRACE_HEADER */
