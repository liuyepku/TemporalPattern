/* acconfig.h.  Generated by Frank Hoeppner. */

@TOP@

/* Standard Template Library (STL) was found on this system (presupposed) */
#undef STL_FOUND

/* compiler has internal boolean type */
#undef BOOL_DEFINED

/* STL contains stl_config.h */
#undef BOOL_DEFINED_IN_STL_CONFIG_H

/* STL contains bool.h */
#undef BOOL_DEFINED_IN_BOOL_H

/* Some compilers have problems with template<> */
#undef TEMPLATE_SPECIALIZATION

/* Time representation used by ttime */
#undef TTIME_USE_TIMEB
#undef TTIME_USE_TIMEVAL

@BOTTOM@

/* With Microsoft VC we need no explicit template instantiation */
#ifdef _MSC_VER
#  define NO_EXPLICIT_TEMPLATE_INSTANTIATION
#endif


