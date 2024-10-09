PY2TEST()

SUBSCRIBER(g:python-contrib)

VERSION(1.2.3)

ORIGINAL_SOURCE(mirror://pypi/s/scipy/scipy-1.2.3.tar.gz)

SIZE(MEDIUM)

FORK_TESTS()

PEERDIR(
    contrib/python/scipy/py2
    contrib/python/scipy/py2/scipy/conftest
)

NO_LINT()

NO_CHECK_IMPORTS()

TEST_SRCS(
    __init__.py
    mpsig.py
    test_array_tools.py
    test_bsplines.py
    test_cont2discrete.py
    test_dltisys.py
    test_filter_design.py
    test_fir_filter_design.py
    test_ltisys.py
    test_max_len_seq.py
    test_peak_finding.py
    test_savitzky_golay.py
    test_signaltools.py
    test_spectral.py
    test_upfirdn.py
    test_waveforms.py
    test_wavelets.py
    test_windows.py
)

END()
