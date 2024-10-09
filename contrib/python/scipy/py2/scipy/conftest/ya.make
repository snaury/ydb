PY2_LIBRARY()

SUBSCRIBER(g:python-contrib)

VERSION(1.2.3)

ORIGINAL_SOURCE(mirror://pypi/s/scipy/scipy-1.2.3.tar.gz)

LICENSE(BSD-3-Clause)

NO_LINT()

SRCDIR(contrib/python/scipy/py2/scipy)

TEST_SRCS(
    conftest.py
)

END()
