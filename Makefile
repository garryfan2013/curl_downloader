OUTPUT = my_downloader
CCOMPILE = gcc
CPPCOMPILE = g++
COMPILEOPTION =  -c -g --std=c++11
INCLUDEDIR	    =	 -I ./include
LINK = g++
LINKOPTION = -g -o $(OUTPUT)
DEFS = 
LIBDIRS = 

OBJS =  ./src/download_manager.o \
		./src/thread_pool.o \
		./src/http_curl_downloader.o \
		./src/main.o
		 
SHAREDLIB = ./lib/libcurl.a -lpthread -lrt

$(OUTPUT): $(OBJS) $(APPENDLIB)
	$(LINK) $(LINKOPTION) $(LIBDIRS) $(OBJS) $(SHAREDLIB)
	rm $(OBJS)

clean: 
	rm -f $(OBJS)
	rm -f $(OUTPUT)
	
all: clean $(OUTPUT) 

.PRECIOUS:%.cpp %.c %.C
.SUFFIXES:
.SUFFIXES:  .c .o .cpp .ecpp .pc .ec .C .cc .cxx

.cpp.o:
	$(CPPCOMPILE) -c -o $*.o $(DEFS) $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp
	
.cc.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cx

.cxx.o:
	$(CPPCOMPILE) -c -o $*.o $(DEFS) $(COMPILEOPTION) $(INCLUDEDIR)  $*.cxx

.c.o:
	$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.c

.C.o:
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.C	

.ecpp.C:
	$(ESQL) -e $(ESQL_OPTION) $(INCLUDEDIR) $*.ecpp 
	
.ec.c:
	$(ESQL) -e $(ESQL_OPTION) $(INCLUDEDIR) $*.ec
	
.pc.cpp:
	$(PROC)  CPP_SUFFIX=cpp $(PROC_OPTION)  $*.pc
