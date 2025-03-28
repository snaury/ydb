LIBRARY()

SRCS(
    sparse_bitmap.cpp
    sparse_bitmap.h
)

END()

RECURSE_FOR_TESTS(
    ut
)
