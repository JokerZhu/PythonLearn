
CMM2008LIBDIR = ./lib/

targetlib = lib8583.so
targetbin = testPackage 

all : $(targetbin)		\
		$(targetlib)



packageDefLibs=	$(CMM2008LIBDIR)upplePackageGrp.o		\
		$(CMM2008LIBDIR)upplePackageDef.o		\
		$(CMM2008LIBDIR)upplePackage.o		\
		$(CMM2008LIBDIR)uppleStr.o		\
		$(CMM2008LIBDIR)uppleEnv.o		\
		$(CMM2008LIBDIR)uppleModule.o		\
		$(CMM2008LIBDIR)upplePackageAutoFldDef.o

bintest =	$(CMM2008LIBDIR)testPackage_haveMain.o			\
		$(CMM2008LIBDIR)testPackage.o		\
		$(packageDefLibs)
$(targetbin):	$(null)
		cc -fPIC -o $@ $(bintest) -lm


libtest =	$(CMM2008LIBDIR)testPackage.o			\
		$(packageDefLibs)
$(targetlib):	$(null)
		cc  -fPIC -shared -o $@ $(libtest) -lm


clean:
	rm $(targetbin) 
	rm $(targetlib) 
install:
	mv $(targetlib) ../lib/
