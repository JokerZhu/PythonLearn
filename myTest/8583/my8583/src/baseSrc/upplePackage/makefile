UPPLELIBDIR = ../..

HOMEDIR = .
INCDIR = $(HOMEDIR)/

PROLIBDIR=$(UPPLELIBDIR)/lib
INCLIBDIR=$(UPPLELIBDIR)/include

INCL = -I $(INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall					\
	finalProduct				\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs =	upplePackageDef.o		\
		upplePackage.o			\
		upplePackageGrp.o		\
		upplePackageAutoFldDef.o	\
		uppleEnv.o					\
		uppleStr.o					\
		uppleModule.o
makeall:	$(allobjs)


finalProduct:	$(null)
		mv *.o					$(PROLIBDIR)
		cp $(INCDIR)upplePackageDef.h		$(INCLIBDIR)
		cp $(INCDIR)upplePackageAutoFldDef.h	$(INCLIBDIR)
		cp $(INCDIR)upplePackage.h		$(INCLIBDIR)
		cp $(INCDIR)upplePackageGrp.h		$(INCLIBDIR)
		cp $(INCDIR)uppleEnv.h		$(INCLIBDIR)
		cp $(INCDIR)uppleErrCode.h		$(INCLIBDIR)
		cp $(INCDIR)uppleModule.h		$(INCLIBDIR)
		cp $(INCDIR)uppleStr.h		$(INCLIBDIR)

rmTmpFiles:	$(null)

clean:
	rm $(PROLIBDIR)/upplePackageDef.o 
	rm $(PROLIBDIR)/upplePackage.o 
	rm $(PROLIBDIR)/upplePackageGrp.o 
	rm $(PROLIBDIR)/upplePackageAutoFldDef.o 
	rm $(PROLIBDIR)/uppleEnv.o 
	rm $(PROLIBDIR)/uppleStr.o 
	rm $(PROLIBDIR)/uppleModule.o 
	
