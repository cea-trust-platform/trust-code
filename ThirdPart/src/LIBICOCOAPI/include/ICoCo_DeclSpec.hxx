// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found at the following URL:
//
//    https://github.com/cea-trust-platform/icoco-coupling

#ifndef ICoCo_DeclSpec_included
#define ICoCo_DeclSpec_included

#ifdef WIN32
#  if defined medicoco_EXPORTS
#    define ICOCO_EXPORT __declspec( dllexport )
#  else
#    define ICOCO_EXPORT __declspec( dllimport )
#  endif
#else
#  define ICOCO_EXPORT
#endif

#endif // ICoCo_DeclSpec_included
