OUTPUT = my_downloader
CC = gcc
CPP = g++
COMPILEOPTION =  -c -g -DUSE_MMAP -std=c++11
INCLUDEDIR =
LINK = g++
LINKOPTION = -g -o $(OUTPUT)
DEFS = 
LIBDIRS = 

OBJS = file_handler.o main.o download_manager.o
		 
SHAREDLIB = ./curl/lib/libcurl.a -lpthread

all: $(OBJS:.o:.d) $(OUTPUT)

$(OUTPUT): $(OBJS) $(APPENDLIB)
	$(LINK) $(LINKOPTION) $(LIBDIRS) $(OBJS) $(SHAREDLIB)

clean: 
	rm -f *.o *.d.* *.d
	rm -f $(OUTPUT)


.PRECIOUS:%.cpp %.c %.C
.SUFFIXES:
.SUFFIXES:  .c .o .cpp .ecpp .pc .ec .C .cc .cxx

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CPP) -MM $(COMPILEOPTION) $(INCLUDEDIR) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(OBJS:.o=.d)

.cpp.o:
	$(CPP) -c -o $*.o $(DEFS) $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp
	
.cc.o:
	$(CC) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cx

.cxx.o:
	$(CPP) -c -o $*.o $(DEFS) $(COMPILEOPTION) $(INCLUDEDIR)  $*.cxx

.c.o:
	$(CC) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.c

.C.o:
	$(CPP) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.C	

.ecpp.C:
	$(ESQL) -e $(ESQL_OPTION) $(INCLUDEDIR) $*.ecpp 
	
.ec.c:
	$(ESQL) -e $(ESQL_OPTION) $(INCLUDEDIR) $*.ec
	
.pc.cpp:
	$(PROC)  CPP_SUFFIX=cpp $(PROC_OPTION)  $*.pc
