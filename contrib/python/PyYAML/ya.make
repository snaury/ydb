PY23_LIBRARY()

LICENSE(MIT)

OWNER(g:python-contrib) 

IF (PYTHON2) 
    PEERDIR( 
        contrib/python/PyYAML/py2 
    )
ELSE() 
    PEERDIR( 
        contrib/python/PyYAML/py3 
    )
ENDIF() 

NO_LINT()

END() 
 
RECURSE( 
    py2 
    py3 
) 
