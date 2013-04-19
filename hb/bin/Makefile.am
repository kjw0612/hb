bin_PROGRAMS=hb test
AM_CPPFLAGS=-I../src
hb_SOURCES=../src/hb.cpp
test_SOURCES = ../src/test/testmain.cpp ../src/test/testinclude.cpp ../src/common/mempool.cpp
test_LDADD = -lgtest -lpthread -ljemalloc
