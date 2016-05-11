UPPLELIBDIR = ..


PROLIBDIR=$(UPPLELIBDIR)/lib
INCLIBDIR=$(UPPLELIBDIR)/include

INCL = -I $(INCLIBDIR)
DEFINES = -D _UNIX  #$(OSSPEC)

all:	makeall					\
	finalProduct				\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) -fPIC $(DEFINES) $(INCL) -c $*.c

allobjs = testPackage_haveMain.o	\
		testPackage.o

makeall:	$(allobjs)


finalProduct:	$(null)
		mv *.o					$(PROLIBDIR)

rmTmpFiles:	$(null)

clean:
	rm $(PROLIBDIR)/testPackage.o 
	rm $(PROLIBDIR)/testPackage_haveMain.o 
