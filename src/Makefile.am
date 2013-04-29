## Process this file with automake to produce Makefile.in
bin_PROGRAMS = dumpait
dumpait_SOURCES = main.cpp
dumpait_SOURCES += dvb_demux.cpp
dumpait_SOURCES += dvb_scanner.cpp
dumpait_SOURCES += dvb_sections.cpp
dumpait_SOURCES += klog.cpp
dumpait_SOURCES += ksingletone.cpp
dumpait_SOURCES += kutils.cpp

dumpait_LDADD = $(LIBLTDL) @BASE_LIBS@ @PTHREAD_LIBS@


