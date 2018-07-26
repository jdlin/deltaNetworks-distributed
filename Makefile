P4_HOME_DIR = /usr/local/p4-1.2a 
INCLUDEDIR = $(P4_HOME_DIR)/include
LIBDIR = $(P4_HOME_DIR)/lib

#-------------------------------------------------------------------------------
all:   762m 762s	
	make 762m 762s

#-------------------------------------------------------------------------------
762m.o: 762m.c 762.h
	cc -c -L$(LIBDIR) -I$(INCLUDEDIR) 762m.c -lp4 -lm  
762s.o:  762s.c 762.h
	cc -c -L$(LIBDIR) -I$(INCLUDEDIR) 762s.c  -lp4 -lm

762m: 762m.o 762s.o 
	cc -o 762m -L$(LIBDIR) -I$(INCLUDEDIR) 762m.o 762s.o  -lp4 -lm  

762s:  762s.o $(LIBDIR)/p4_cmain.o 
	cc -o 762s  -L$(LIBDIR) -I$(INCLUDEDIR) $(LIBDIR)/p4_cmain.o 762s.o -lp4 -lm
#-------------------------------------------------------------------------------
clean : 
	'rm' *.o
