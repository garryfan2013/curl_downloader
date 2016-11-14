# Makefile for my_downloader

##################### vars #############################

TARGET = my_downloader
CC = gcc
CPP = g++
COMPILEOPTIONS = -c -g -std=c++11
INCLUDEDIR =
LINK = g++
LINKOPTIONS = -g -o $(TARGET)
DEFS = 
LIBDIR = 

OBJS = file_handler.o download_manager.o main.o
LIBS = ./curl/lib/libcurl.a -lpthread

###################### rules ##############################

all: $(OBJS:.o:.d) $(TARGET)

$(TARGET): $(OBJS)
	$(LINK) $(LINKOPTIONS) $(LIBDIRS) $(OBJS) $(LIBS)

clean: 
	rm -f *.o *.d.* *.d
	rm -f $(TARGET)

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CPP) -MM $(COMPILEOPTIONS) $(INCLUDEDIR) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(OBJS:.o=.d)

.cpp.o:
	$(CPP) -c -o $*.o $(DEFS) $(COMPILEOPTIONS) $(INCLUDEDIR)  $*.cpp
	
.c.o:
	$(CC) -c -o $*.o $(COMPILEOPTIONS) $(INCLUDEDIR) $*.c
