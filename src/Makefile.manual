BIN=dumpait
SRCS+=main.cpp klog.cpp kutils.cpp dvb_demux.cpp dvb_sections.cpp dvb_scanner.cpp

IMAGE=vusolo2
TOP=/home/kos/works/projects/vuplus-1.6
STAGING=$(TOP)/$(IMAGE)/build/tmp/staging/mipsel-oe-linux/usr
CROSS=$(TOP)/$(IMAGE)/build/tmp/cross/mipsel/bin/mipsel-oe-linux-

CXX=$(CROSS)g++
STRIP=$(CROSS)strip
RM=rm -f

SILENT=@

CFLAGS=-g -I$(STAGING)/include
LDFLAGS=-L$(STAGING)/lib -lpthread -ldvbsi++

OBJS=$(SRCS:.cpp=.o)

.SUFFIXES : .cpp .o
.PHONY : all clean

.cpp.o: 
	$(SILENT)$(CXX) $(CFLAGS) -c $< 

all:$(OBJS)
	$(SILENT)$(CXX) -o $(BIN) $(OBJS) $(LDFLAGS)
	$(SILENT)$(STRIP) $(BIN)

clean:
	$(SILENT)$(RM) $(BIN) *.o

