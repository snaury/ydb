#pragma once

#ifdef USE_PYTHON3
#include <contrib/tools/python3/Include/cpython/pydebug.h>
#else
#error "No <cpython/pydebug.h> in Python2"
#endif
