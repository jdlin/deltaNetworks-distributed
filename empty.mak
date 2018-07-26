
P4_HOME_DIR = /usr/local/p4-1.2a 
INCLUDEDIR = $(P4_HOME_DIR)/include
LIBDIR = $(P4_HOME_DIR)/lib

#-------------------------------------------------------------------------------
all:   762me 762se	
	make 762me 762se

#-------------------------------------------------------------------------------
762me.o: 762me.c 762e.h
	cc -c -L$(LIBDIR) -I$(INCLUDEDIR) 762me.c -lp4 -lm  
762se.o:  762se.c 762e.h
	cc -c -L$(LIBDIR) -I$(INCLUDEDIR) 762se.c  -lp4 -lm

762me: 762me.o 762se.o 
	cc -o 762me -L$(LIBDIR) -I$(INCLUDEDIR) 762me.o 762se.o  -lp4 -lm  

762se:  762se.o $(LIBDIR)/p4_cmain.o 
	cc -o 762se  -L$(LIBDIR) -I$(INCLUDEDIR) $(LIBDIR)/p4_cmain.o 762se.o -lp4 -lm
#-------------------------------------------------------------------------------
clean : 
	'rm' *.o
