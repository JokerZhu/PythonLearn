CMM2008DIR = $(UPPLELIBDIR)/
CMM2008LIBDIR = $(CMM2008DIR)lib/
CMM2008INCDIR = $(CMM2008DIR)include/

# 定义产品模块目录
PROLIBDIR=$(PRODUCTHOUSEWARE)/libLibs/
PROINCDIR=$(PRODUCTHOUSEWARE)/includeLibs/

# 定义开发目录
HOMEDIR = $(DEVDIROFTHISPROJECT)/
INCDIR = $(HOMEDIR)include/
XMLINCDIR = $(HOME)/xmlLibs/include/libxml2/
OPENSSL_DIR = $(USRINCDIR)ssl/include/

INCL = -I $(INCDIR) -I $(XMLINCDIR) -I $(CMM2008INCDIR) -I $(OPENSSL_DIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall					\
	finalProduct				\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs =	termPackage.o			\
		termSecurity.o			\
		termMacDataMerge.o		\
		termTransFldDefTBL.o		\
		mngTermTransFldDefTBL.o		\
		termTransDataDef.o		\
		mngTermTransDefTBL.o		\
		termTransDefTBL.o		\
		strUnPcakKeyValue.o		\
		APIAsynNotification.o		\
		TransPush.o
makeall:	$(allobjs)


finalProduct:	$(null)
		mv *.o					$(PROLIBDIR)
		cp $(INCDIR)termPackage.h		$(PROINCDIR)
		cp $(INCDIR)termSecurity.h		$(PROINCDIR)
		cp $(INCDIR)termMacDataMerge.h		$(PROINCDIR)
		cp $(INCDIR)termTransFldDefTBL.h	$(PROINCDIR)
		cp $(INCDIR)termTransDataDef.h		$(PROINCDIR)
		cp $(INCDIR)termTransDefTBL.h		$(PROINCDIR)
		cp $(INCDIR)strUnPcakKeyValue.h		$(PROINCDIR)

rmTmpFiles:	$(null)
