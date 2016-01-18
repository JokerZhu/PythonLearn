#ifndef _upplePackageGrp_
#define _upplePackageGrp_

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "upplePackage.h"

#define conMaxNumOfPackage	50

int UppleIsPackageGrpConnected();

int UppleConnectPackageGrp();

int UppleDisconnectPackageGrp();

int UppleIsValidPackageIndexInGroup(int packageIndex);

int UppleInsertPackageIntoGroup(int packageIndex,char *nameOfPackageDef);

PUpplePackage UppleFindPackageInGroup(int packageIndex);

#endif
