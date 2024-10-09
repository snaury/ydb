/* Generated by Cython 0.29.37 */

#ifndef __PYX_HAVE__scipy__special___ufuncs
#define __PYX_HAVE__scipy__special___ufuncs

#include "Python.h"

#ifndef __PYX_HAVE_API__scipy__special___ufuncs

#ifndef __PYX_EXTERN_C
  #ifdef __cplusplus
    #define __PYX_EXTERN_C extern "C"
  #else
    #define __PYX_EXTERN_C extern
  #endif
#endif

#ifndef DL_IMPORT
  #define DL_IMPORT(_T) _T
#endif

__PYX_EXTERN_C int wrap_PyUFunc_getfperr(void);

#endif /* !__PYX_HAVE_API__scipy__special___ufuncs */

/* WARNING: the interface of the module init function changed in CPython 3.5. */
/* It now returns a PyModuleDef instance instead of a PyModule instance. */

#if PY_MAJOR_VERSION < 3
PyMODINIT_FUNC init_ufuncs(void);
#else
PyMODINIT_FUNC PyInit__ufuncs(void);
#endif

#endif /* !__PYX_HAVE__scipy__special___ufuncs */
