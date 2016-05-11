//	Author:		zhangyongding
//	Date:		20081014
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "upplePackageGrp.h"
#include "uppleStr.h"

PUpplePackage	pgupplePackageGrp[conMaxNumOfPackage];
int		pgupplePackageGrpConnected = 0;

int UppleIsPackageGrpConnected()
{
	return(pgupplePackageGrpConnected);
}

int UppleConnectPackageGrp()
{
	int	i;
	
	if (UppleIsPackageGrpConnected())
		return(0);
	for (i = 0; i < conMaxNumOfPackage; i++)
		pgupplePackageGrp[i] = NULL;
	pgupplePackageGrpConnected = 1;
	return(0);
}

int UppleDisconnectPackageGrp()
{
	int	i;
	
	if (!UppleIsPackageGrpConnected())
		return(0);
	for (i = 0; i < conMaxNumOfPackage; i++)
	{
		if (pgupplePackageGrp[i] != NULL)
			UppleDisconnectPackage(pgupplePackageGrp[i]);
		pgupplePackageGrp[i] = NULL;
	}
	pgupplePackageGrpConnected = 0;
	return(0);
}

int UppleIsValidPackageIndexInGroup(int packageIndex)
{
	if ((packageIndex < 0) || (packageIndex >= conMaxNumOfPackage))
		return(0);
	else
		return(1);
}

int UppleInsertPackageIntoGroup(int packageIndex,char *nameOfPackageDef)
{
	int	ret;
	
	if ((ret = UppleConnectPackageGrp()) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleInsertPackageIntoGroup:: UppleConnectPackageGrp!\n");
		return(ret);
	}
	if (!UppleIsValidPackageIndexInGroup(packageIndex))
	{
		printf(__FILE__,__LINE__,"in UppleInsertPackageIntoGroup:: UppleIsValidPackageIndexInGroup [%d]\n",packageIndex);
		return(errCodePackageDefMDL_InvalidPackageIndex);
	}
	if (pgupplePackageGrp[packageIndex] != NULL)
	{
		if (UppleIsValidPackage(pgupplePackageGrp[packageIndex]))
		{
			if (strcmp(pgupplePackageGrp[packageIndex]->pdef->name,nameOfPackageDef) != 0)
				printf(__FILE__,__LINE__,"in UppleInsertPackageIntoGroup:: package [%s] and [%s] of the same index [%d]!\n",
						pgupplePackageGrp[packageIndex]->pdef->name,nameOfPackageDef,packageIndex);
				return(errCodePackageDefMDL_TwoPackageOfSamePackageIndex);
		}
		UppleDisconnectPackage(pgupplePackageGrp[packageIndex]);
	}
	if ((pgupplePackageGrp[packageIndex] = UppleConnectPackage(nameOfPackageDef)) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleInsertPackageIntoGroup:: UppleConnectPackage [%s]!\n",nameOfPackageDef);
		return(errCodePackageDefMDL_ConnectPackage);
	}
	return(0);
}

PUpplePackage UppleFindPackageInGroup(int packageIndex)
{
	int	ret;
	
	if ((ret = UppleConnectPackageGrp()) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleFindPackageInGroup:: UppleConnectPackageGrp!\n");
		return(NULL);
	}
	if (!UppleIsValidPackageIndexInGroup(packageIndex))
	{
		printf(__FILE__,__LINE__,"in UppleFindPackageInGroup:: UppleIsValidPackageIndexInGroup [%d]\n",packageIndex);
		return(NULL);
	}
	return(pgupplePackageGrp[packageIndex]);
}
