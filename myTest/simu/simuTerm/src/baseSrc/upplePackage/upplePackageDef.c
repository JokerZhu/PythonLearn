//	Author:		zhangyongding
//	Date:		20081014
//	Version:	1.0

// 2006/6/23,增加了长度指示域相关的定义
// 即增加了 _bitFldLenFlag_ 宏定义相关的内容

// 2006/6/23增加_bitFldLenFlag_宏定义
#ifndef _bitFldLenFlag_
#define _bitFldLenFlag_
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

//#include "uppleLog.h"
//#include "uppleModule.h"
#include "uppleEnv.h"
//#include "uppleCommand.h"
#include "uppleModule.h"
#include "upplePackageDef.h"

//#define USE_REDIS

int				gupplePackageDefMDLConnected = 0;
PUppleSharedMemoryModule	pgupplePackageDefMDLGrp	[conMaxNumOfPackageDef];
PUpplePackageDef		pgupplePackageDefGrp	[conMaxNumOfPackageDef];

// 2006/6/23,增加这个函数
int UppleIsValidPackageFldLenPresentation(TUpplePackageFldLenPresentation flag)
{
	switch (flag)
	{
		case	conFldLenUseAscii:
		case	conFldLenUseLeftDuiQi:
		case	conFldLenUseRightDuiQi:
			return(1);
		default:
			return(0);
	}
}
	
int UppleGetConfFileNameOfPackageDefGroup(char *fileName)
{
	sprintf(fileName,"../cfg/%s",conConfFileNameOfPackageDefGroup);
	//printf("fileName = [%s] [%s] [%d]\n",fileName,__FILE__,__LINE__);
	return(0);
}

int UppleGetNameOfPackageDefOfIndex(int index,char *packageName)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		id;
	int		i;
			
	memset(fileName,0,sizeof(fileName));
	UppleGetConfFileNameOfPackageDefGroup(fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleGetNameOfPackageDefOfIndex:: UppleInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	for (i = 0; i < conMaxNumOfPackageDef; i++)
	{
		if ((p = UppleGetEnviVarByIndex(i)) == NULL)
			continue;
		if (atoi(p) == index)
		{
			if ((p = UppleGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
			{
				printf(__FILE__,__LINE__,"in UppleGetNameOfPackageDefOfIndex:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
				ret = errCodePackageDefMDL_PackageDefOfSpecifiedIDDefined;
			}
			else
			{
				strcpy(packageName,p);
				ret = 0;
			}
			break;
		}
	}
	if (i == conMaxNumOfPackageDef)
	{
		printf(__FILE__,__LINE__,"in UppleGetNameOfPackageDefOfIndex:: index = [%d]\n",index);
		ret = errCodePackageDefMDL_PackageDefOfSpecifiedIDDefined;
	}
	UppleClearEnvi();
	return(ret);	
}

int UppleGetIDOfSpecifiedPackageDef(char *packageName)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		id;
		
	memset(fileName,0,sizeof(fileName));
	UppleGetConfFileNameOfPackageDefGroup(fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleGetIDOfSpecifiedPackageDef:: UppleInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UppleGetEnviVarByName(packageName)) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleGetIDOfSpecifiedPackageDef:: UppleGetEnviVarByName for [%s]\n!",packageName);
		id = errCodePackageDefMDL_IDOfPackageNotDefined;
	}
	else
		id = atoi(p);
	
	UppleClearEnvi();
	
	if (UppleIsValidPackageID(id))
		return(id);
	else
		return(errCodePackageDefMDL_InvalidIDOfPackage);	
}

int UppleIsValidPackageID(int id)
{
	if ((id >= 0) && (id < conMaxNumOfPackageDef))
		return(1);
	else
		return(0);
}

int UppleIsValidPackageMaxFldNum(int num)
{
	if ((num <= 0) || (num >= conMaxNumOfPackageFlds))
		return(0);
	else
		return(1);
}

int UppleIsPackageDefGroupConnected()
{
	return(gupplePackageDefMDLConnected);
}

int UppleIsValidPackageFldIndex(PUpplePackageDef pdef,int fldIndex)
{
	if (pdef == NULL)
		return(0);
	if ((fldIndex < 0) || (fldIndex >= pdef->maxNum))
		return(0);
	//if (pdef->fldDefTBL[fldIndex].lenOfValue <= 0)
	if (pdef->fldDefTBL[fldIndex].lenOfValue < 0)
		return(0);
	return(1);
}

int UppleConnectPackageDefGroup()
{
	int	ret;
	int	i;
	char	packageName[128];
	
	if (!UppleIsPackageDefGroupConnected())
	{
		gupplePackageDefMDLConnected = 1;
		for (i = 0; i < conMaxNumOfPackageDef; i++)
		{
			pgupplePackageDefMDLGrp[i] = NULL;
			pgupplePackageDefGrp[i] = NULL;
		}
	}

	for (i = 0; i < conMaxNumOfPackageDef; i++)
	{
		memset(packageName,0,sizeof(packageName));
		if ((ret = UppleGetNameOfPackageDefOfIndex(i,packageName)) < 0)
			continue;
		if ((ret = UppleConnectPackageDef(packageName)) < 0)
		{
			printf(__FILE__,__LINE__,"in UppleConnectPackageDefGroup:: UppleConnectPackageDef [%s]\n",packageName);
			continue;
		}
	}	
	return(0);
}

int UppleReloadPackageDefGroup()
{
	int	ret;
	int	i;
	char	packageName[128];
	
	if (!UppleIsPackageDefGroupConnected())
	{
		gupplePackageDefMDLConnected = 1;
		for (i = 0; i < conMaxNumOfPackageDef; i++)
		{
			pgupplePackageDefMDLGrp[i] = NULL;
			pgupplePackageDefGrp[i] = NULL;
		}
	}

	for (i = 0; i < conMaxNumOfPackageDef; i++)
	{
		memset(packageName,0,sizeof(packageName));
		if ((ret = UppleGetNameOfPackageDefOfIndex(i,packageName)) < 0)
			continue;
		if ((ret = UppleReloadPackageDef(packageName)) < 0)
		{
			printf(__FILE__,__LINE__,"in UppleReloadPackageDefGroup:: UppleReloadPackageDef [%s]\n",packageName);
			continue;
		}
	}	
	return(0);
}

int UppleDisconnectPackageDefGroup()
{
	int	i;
	
	for (i = 0; i < conMaxNumOfPackageDef; i++)
	{
		if (pgupplePackageDefGrp[i] == NULL)
			continue;
		else
		{
			UppleDisconnectPackageDef(pgupplePackageDefGrp[i]->name);
			pgupplePackageDefGrp[i] = NULL;
		}	
	}
	return(0);
}

int UppleRemovePackageDefGroup()
{
	int	i;
	
	for (i = 0; i < conMaxNumOfPackageDef; i++)
	{
		if (pgupplePackageDefGrp[i] == NULL)
			continue;
		else
		{
			UppleRemovePackageDef(pgupplePackageDefGrp[i]->name);
			pgupplePackageDefGrp[i] = NULL;
			pgupplePackageDefMDLGrp[i] = NULL;
		}
	}
	return(0);
}

int UpplePrintPackageDefGroupToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	realNum = 0;
	
	if (fp == NULL)
	{
		printf(__FILE__,__LINE__,"in UpplePrintPackageDefGroup:: fp is null!\n");
		return(errCodeParameter);
	}
	
	if ((ret = UppleConnectPackageDefGroup()) < 0)
	{
		printf(__FILE__,__LINE__,"in UpplePrintPackageDefGroup:: UppleConnectPackageDefGroup!\n");
		return(ret);
	}
	
	for (i = 0; i < conMaxNumOfPackageDef; i++)
	{
		if (pgupplePackageDefGrp[i] == NULL)
			continue;
		if (UpplePrintPackageDefBaseInfoToFile(pgupplePackageDefGrp[i],fp) < 0)
			continue;
		realNum++;
	}
	fprintf(fp,"realPackageDef = [%d]\n",realNum);
	return(0);
}
	
int UppleGetConfFileNameOfPackageDef(char *packageName,char *fileName)
{
	//sprintf(fileName,"%s/packDef/%s.Def",getenv("UPPLEETC"),packageName);
	sprintf(fileName,"../cfg/%s.Def",packageName);
	//printf("fileName = [%s] [%s] [%d]\n",fileName,__FILE__,__LINE__);
	return(0);
}

int UppleGetMaxFldNumOfSpecifiedPackageDef(char *packageName)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		num;
	
	memset(fileName,0,sizeof(fileName));
	UppleGetConfFileNameOfPackageDef(packageName,fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleGetMaxFldNumOfSpecifiedPackageDef:: UppleInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UppleGetEnviVarByName("maxFldsNum")) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleGetMaxFldNumOfSpecifiedPackageDef:: UppleGetEnviVarByName for [%s]\n!","maxFldsNum");
		num = errCodePackageDefMDL_MaxNumOfPackageFldsNotDefined;
	}
	else
		num = atoi(p);
	
	UppleClearEnvi();
	return(num);	
}

int UppleIsPackageDefConnected(char *packageName)
{
	int	i;
	
	for (i = 0; i < conMaxNumOfPackageDef; i++)
	{
		if ((pgupplePackageDefGrp[i] == NULL) || (pgupplePackageDefMDLGrp[i] == NULL))
			continue;
		if (strcmp(pgupplePackageDefGrp[i]->name,packageName) == 0)
			return(1);
	}
	return(0);
}

PUpplePackageDef UppleFindPackageDef(char *packageName)
{
	int	i;
	int	ret;
	
	    //UppleLog2( __FILE__,__LINE__, "#TEST:Before UppleConnectPackageDef...1\n" );
	if ((ret = UppleConnectPackageDef(packageName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleFindPackageDef:: UppleConnectPackageDef!\n");
		return(NULL);
	}
	
	for (i = 0; i < conMaxNumOfPackageDef; i++)
	{
		if ((pgupplePackageDefGrp[i] == NULL) || (pgupplePackageDefMDLGrp[i] == NULL))
			continue;
		if (strcmp(pgupplePackageDefGrp[i]->name,packageName) == 0)
			return(pgupplePackageDefGrp[i]);
	}
	printf(__FILE__,__LINE__,"in UppleFindPackageDef:: [%s] not defined!\n",packageName);
	return(NULL);
}

int UppleFindPackageDefIndex(char *packageName)
{
	int	i;
	int	ret;
	
	if ((ret = UppleConnectPackageDef(packageName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleFindPackageDefIndex:: UppleConnectPackageDef!\n");
		return(ret);
	}
	
	for (i = 0; i < conMaxNumOfPackageDef; i++)
	{
		if ((pgupplePackageDefGrp[i] == NULL) || (pgupplePackageDefMDLGrp[i] == NULL))
			continue;
		if (strcmp(pgupplePackageDefGrp[i]->name,packageName) == 0)
			return(i);
	}
	printf(__FILE__,__LINE__,"in UppleFindPackageDefIndex:: [%s] not defined!\n",packageName);
	return(errCodePackageDefMDL_PackageDefNotDefined);
}

int UppleGetMDLNameOfPackageDef(char *packageName,char *mdlName)
{
	sprintf(mdlName,"%sDefMDL",packageName);
	return(0);
}

int UppleConnectPackageDef(char *packageName)
{
	int	num;
	int	index;
	char	mdlName[100];
	char FullPackageName[1024 + 1] = {0};
	
	
	if (UppleIsPackageDefConnected( packageName))
		return(0);


	
	if (strlen(packageName) >= sizeof(pgupplePackageDefGrp[0]->name))
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: packageName [%s] too long!\n",packageName);
		return(errCodeParameter);
	}
	
	if (!UppleIsValidPackageMaxFldNum(num = UppleGetMaxFldNumOfSpecifiedPackageDef(packageName)))
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: UppleIsValidPackageMaxFldNum [%s] [%d]\n",packageName,num);
		return(errCodePackageDefMDL_InvalidMaxFldNum);
	}
	//printf("test packageName = [%s]\n",packageName);	
	if (!UppleIsValidPackageID(index = UppleGetIDOfSpecifiedPackageDef(packageName)))
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: UppleIsValidPackageID index = [%d] for [%s]\n",index,packageName);
		return(errCodePackageDefMDL_InvalidPackageIndex);
	}
	//printf("test\n");	
	if (pgupplePackageDefGrp[index] != NULL)
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: package [%s] and package [%s] of the same id [%d]\n",
			pgupplePackageDefGrp[index]->name,packageName,index);
		return(errCodePackageDefMDL_PackageOfSameID);
	}
	memset(mdlName,0,sizeof(mdlName));
	UppleGetMDLNameOfPackageDef(packageName,mdlName);


	if ((pgupplePackageDefMDLGrp[index] = UppleConnectSharedMemoryModule_Redis(mdlName,
			sizeof(TUpplePackageDef))) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: UppleConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgupplePackageDefGrp[index] = (PUpplePackageDef)UppleGetAddrOfSharedMemoryModuleUserSpace_Redis(pgupplePackageDefMDLGrp[index])) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: PUpplePackageDef!\n");
		pgupplePackageDefMDLGrp[index] = NULL;
		return(errCodeSharedMemoryModule);
	}
	strcpy(pgupplePackageDefGrp[index]->name,packageName);
	pgupplePackageDefGrp[index]->id = index;

	if (UppleIsNewCreatedSharedMemoryModule_Redis(pgupplePackageDefMDLGrp[index]))
		return(UppleReloadPackageDef(packageName));
	else
		return(0);

}

int UppleDisconnectPackageDef(char *packageName)
{
	int	index;
	
	if (!UppleIsPackageDefConnected(packageName))
		return(0);

	if ((index = UppleFindPackageDefIndex(packageName)) < 0)
		return(index);
	
	pgupplePackageDefGrp[index] = NULL;
	UppleDisconnectShareModule(pgupplePackageDefMDLGrp[index]);
	return(0);
}
	
int UppleRemovePackageDef(char *packageName)
{
	char	mdlName[100];
	
	memset(mdlName,0,sizeof(mdlName));
	UppleGetMDLNameOfPackageDef(packageName,mdlName);
	UppleDisconnectPackageDef(packageName);
	return(UppleRemoveSharedMemoryModule_Redis(mdlName));
}

int UppleReloadPackageDef(char *packageName)
{
	int			ret;
	char			*p;
	PUpplePackageFldDef	pFldDef;
	char			fldName[10];
	char			fileName[512];
	int			realNum = 0;
	int			index,i,indexOfVarFld;

#ifdef USE_REDIS
	//再次读入从REDIS内存中读
	{
	    char GName[ 128 ];
	    char sIndex[ 32 ];
	    char sRealNum[ 32 ];


	    //读出数组号
	    sprintf( GName, "#PackDef.%s.index", packageName );
	    memset( sIndex, 0x00, sizeof(sIndex) );
	    GET0( GName, sIndex );

	    if( atoi(sIndex) )  //存入过
		{
		    index = atol(sIndex);
		    sprintf( GName, "#PackDef.%s", packageName );
		    memset( pgupplePackageDefGrp[index], 0x00, sizeof(TUpplePackageDef) );
		    GET0( GName, pgupplePackageDefGrp[index] );

		    //读出字段数
		    sprintf( GName, "#PackDef.%s.realNum", packageName );
		    memset( sRealNum, 0x00, sizeof(sRealNum) );
		    GET0( GName, sRealNum );

		    //UppleLog2(__FILE__,__LINE__,"#TEST:Reload Packdef[%s] from REDIS, index=[%ld], realNum=[%ld]...\n",packageName, atol(sIndex), atol(sRealNum) );

/*
	for (i = 0; i < pgupplePackageDefGrp[index]->maxNum ; i++)
	{
		pFldDef = &(pgupplePackageDefGrp[index]->fldDefTBL[i]);
		UppleNullLog("%04d %02d %04d %d %40s\n",
				pFldDef->index,
				pFldDef->lenOfLenFlag,
				pFldDef->lenOfValue,
				pFldDef->valueType,
				pFldDef->remark);
	}
*/
		    return atol(sRealNum);
		}
	}
#endif
	
	if ((ret = UppleConnectPackageDef(packageName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleConnectPackageDef [%s]\n",packageName);
		return(ret);
	}
	if (!UppleIsValidPackageID(index = UppleFindPackageDefIndex(packageName)))
	{
		printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleFindPackageDefIndex [%s]!\n",packageName);
		return(errCodePackageDefMDL_InvalidPackageIndex);
	}
	strcpy(pgupplePackageDefGrp[index]->name,packageName);
	pgupplePackageDefGrp[index]->id = index;
	memset(fileName,0,sizeof(fileName));
	UppleGetConfFileNameOfPackageDef(packageName,fileName);	
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleInitEnvi [%s]!\n",fileName);
		return(ret);
	}
	if ((p = UppleGetEnviVarByName("maxFldsNum")) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleGetEnviVarByName [%s]!\n","maxFldsNum");
		ret = errCodePackageDefMDL_MaxNumOfPackageFldsNotDefined;
		goto errorExit;
	}
	if (!UppleIsValidPackageMaxFldNum(pgupplePackageDefGrp[index]->maxNum = atoi(p)))
	{
		printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleIsValidPackageMaxFldNum [%s] [%d]\n",packageName,pgupplePackageDefGrp[index]->maxNum);
		ret = errCodePackageDefMDL_InvalidMaxFldNum;
		goto errorExit;
	}
	// 2006/6/23，增加
//	pgupplePackageDefGrp[index]->fldLenPresentation = conFldLenUseAscii;
	pgupplePackageDefGrp[index]->fldLenPresentation = conFldLenUseRightDuiQi;
	if ((p = UppleGetEnviVarByName("fldLenPresentation")) != NULL)
	{
		if (!UppleIsValidPackageFldLenPresentation(pgupplePackageDefGrp[index]->fldLenPresentation = atoi(p)))
		{
			printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: [%s] = [%d] not valid!\n","fldLenPresentation",atoi(p));
			pgupplePackageDefGrp[index]->fldLenPresentation = conFldLenUseAscii;
		}
	}
	// 2006/6/23 结束
	for (i = 0; i < pgupplePackageDefGrp[index]->maxNum ; i++)
	{
		pFldDef = &(pgupplePackageDefGrp[index]->fldDefTBL[i]);
		memset(pFldDef,0,sizeof(*pFldDef));
		pFldDef->index = i;
		sprintf(fldName,"%04d",i);
		if ((indexOfVarFld = UppleGetVarIndexOfTheVarName(fldName)) < 0)
		{
			continue;
		}
		// 读取变长指示域
		if ((p = UppleGetEnviVarOfTheIndexByIndex(indexOfVarFld,1)) == NULL)
		{
			printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",indexOfVarFld,1);
			continue;
		}
		pFldDef->lenOfLenFlag = atoi(p);
		// 读取域长度
		if ((p = UppleGetEnviVarOfTheIndexByIndex(indexOfVarFld,2)) == NULL)
		{
			printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",indexOfVarFld,2);
			continue;
		}
		pFldDef->lenOfValue = atoi(p);
		// 读取域值的类型
		if ((p = UppleGetEnviVarOfTheIndexByIndex(indexOfVarFld,3)) == NULL)
		{
			printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",indexOfVarFld,3);
			continue;
		}
		pFldDef->valueType = atoi(p);
		// 读取域说明
		if ((p = UppleGetEnviVarOfTheIndexByIndex(indexOfVarFld,4)) != NULL)
		{
			if (strlen(p) > sizeof(pFldDef->remark) - 1)
				memcpy(pFldDef->remark,p,sizeof(pFldDef->remark)-1);
			else
				strcpy(pFldDef->remark,p);
		}
	/*
		printf("%04d %02d %04d %d %40s\n",
				pFldDef->index,
				pFldDef->lenOfLenFlag,
				pFldDef->lenOfValue,
				pFldDef->valueType,
				pFldDef->remark);
	*/
		realNum++;
	}
	UppleClearEnvi();		

#ifdef USE_REDIS
//第一次读出后，存入REDIS内存
{
	char GName[ 128 ];
	char sIndex[ 32 ];
	char sRealNum[ 32 ];
	sprintf( GName, "#PackDef.%s", packageName );
	//SET0( GName, pgupplePackageDefGrp[index], sizeof(*(pgupplePackageDefGrp[index]))+1 );

	//目前支持两种方式，BCD方式和ASCII方式。
	if( strchr(packageName, 'P' ) )
		pgupplePackageDefGrp[index]->fldLenPresentation = conFldLenUseAscii;

	UppleLog2(__FILE__,__LINE__,"#TEST:fldLenPresentation =[%d]...\n", pgupplePackageDefGrp[index]->fldLenPresentation );

	SET0( GName, pgupplePackageDefGrp[index], sizeof(TUpplePackageDef)+1 );
	
	//记下字段数
	sprintf( GName, "#PackDef.%s.realNum", packageName );
        memset( sRealNum, 0x00, sizeof(sRealNum) );
	sprintf( sRealNum, "%d", realNum );
	SET0( GName, sRealNum, strlen(sRealNum)+1 );
        
        //记下数组号
	sprintf( GName, "#PackDef.%s.index", packageName );
        memset( sIndex, 0x00, sizeof(sIndex) );
	sprintf( sIndex, "%d", index );
	SET0( GName, sIndex, strlen(sIndex)+1 );

	UppleLog2(__FILE__,__LINE__,"#TEST:Store Packdef[%s] to REDIS, index=[%s],realNum=[%s] ...\n",packageName, sIndex, sRealNum );
}
#endif

	return(realNum);
errorExit:
	UppleClearEnvi();
	return(ret);
}

int UpplePrintSpecifiedPackageDefToFile(char *packageName,FILE *fp)
{
	UpplePrintPackageDefToFile(UppleFindPackageDef(packageName),fp);		
	UpplePrintPackageDefBaseInfoToFile(UppleFindPackageDef(packageName),fp);
	return(0);		
}

int UpplePrintPackageDefBaseInfoToFile(PUpplePackageDef ppackageDef,FILE *fp)
{
	if ((fp == NULL) || (ppackageDef == NULL))
	{
		printf(__FILE__,__LINE__,"in UpplePrintPackageDefBaseInfoToFile:: null pointer!\n");
		return(errCodeParameter);
	}

	fprintf(fp,"%04d %30s %04d %02d\n",ppackageDef->id,ppackageDef->name,ppackageDef->maxNum,ppackageDef->fldLenPresentation);
	return(0);
}

int UpplePrintPackageDefToFile(PUpplePackageDef ppackageDef,FILE *fp)
{
	int	i;
	int	ret;
	int	realNum = 0;
	
	if ((fp == NULL) || (ppackageDef == NULL))
	{
		printf(__FILE__,__LINE__,"in UpplePrintPackageDefToFile:: null pointer!\n");
		return(errCodeParameter);
	}
	
	for (i = 0; i < ppackageDef->maxNum; i++)
	{
		if (UpplePrintPackageFldDefToFile(&(ppackageDef->fldDefTBL[i]),fp) < 0)
			continue;
		realNum++;
		if ((realNum != 0) && (realNum % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			//if (UppleIsQuit(UpplePressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
	}
	fprintf(fp,"realFldsNum = [%d]\n",realNum);
	return(0);
}

int UpplePrintPackageFldDefToFile(PUpplePackageFldDef pFldDef,FILE *fp)
{
	int	i;
	
	if ((pFldDef == NULL) || (fp == NULL))
	{
		printf(__FILE__,__LINE__,"in UpplePrintPackageFldDefToFile:: null Pointer!\n");
		return(errCodeParameter);
	}
	if (pFldDef->lenOfValue <= 0)
		return(errCodePackageDefMDL_FldNotDefined);
	fprintf(fp,"%04d %02d %04d ",pFldDef->index,pFldDef->lenOfLenFlag,pFldDef->lenOfValue);
	switch (pFldDef->valueType % 100)
	{
		case	conBitMapFld:
			fprintf(fp,"%8s","位图");
			break;
		case	conBitsFld:
			fprintf(fp,"%8s","二进制数");
			break;
		case	conAsciiFldFullLen:
			fprintf(fp,"%8s","必须足位");
			break;
		case	conAsciiFldLeftBlank:
			fprintf(fp,"%8s","左补空格");
			break;
		case	conAsciiFldRightBlank:
			fprintf(fp,"%8s","右补空格");
			break;
		case	conAsciiFldLeftZero:
			fprintf(fp,"%8s","左补零");
			break;
		case	conAsciiFldRightZero:
			fprintf(fp,"%8s","右补零");
			break;
		case	conAsciiFldRightNull:
			fprintf(fp,"%8s","右补null");
			break;
		case	conBitsFldLeftDuiQi:
			fprintf(fp,"%8s","左二进制");
			break;
		case	conBitsFldRightDuiQi:
			fprintf(fp,"%8s","右二进制");
			break;
		default:
			fprintf(fp,"%8s","必须足位");
			break;
	}
	fprintf(fp,"  %s\n",pFldDef->remark);
	return(0);
}

PUpplePackageFldDef UppleFindPackageFldDefOfIndex(char *packageName,int index)
{
	PUpplePackageDef	ppackageDef;
	int			i;
	
	if ((ppackageDef = UppleFindPackageDef(packageName)) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleFindPackageFldDefOfIndex:: UppleFindPackageDef!\n");
		return(NULL);
	}
	
	for (i = 0; i < ppackageDef->maxNum; i++)
	{
		if (ppackageDef->fldDefTBL[i].index == index)
			return(&(ppackageDef->fldDefTBL[i]));
	}
	printf(__FILE__,__LINE__,"in UppleFindPackageFldDefOfIndex:: not found def for fld of index [%d]\n",index);
	return(NULL);
}

int UppleGetRealNumOfPackageFld(char *packageName)
{
	PUpplePackageDef	ppackageDef;
	int			i;
	
	if ((ppackageDef = UppleFindPackageDef(packageName)) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleGetRealNumOfPackageFld:: UppleFindPackageDef!\n");
		return(errCodePackageDefMDL_PackageDefNotDefined);
	}
	return(ppackageDef->maxNum);
}




//----------------------------------------------------



PUpplePackageDef UppleFindPackageDefNew(char *path, char *packageName)
{
	int	i;
	int	ret;
	
	    //UppleLog2( __FILE__,__LINE__, "#TEST:Before UppleConnectPackageDef...1\n" );
	if ((ret = UppleConnectPackageDefNew(path,packageName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleFindPackageDef:: UppleConnectPackageDef!\n");
		return(NULL);
	}
	
	for (i = 0; i < conMaxNumOfPackageDef; i++)
	{
		if ((pgupplePackageDefGrp[i] == NULL) || (pgupplePackageDefMDLGrp[i] == NULL))
			continue;
		if (strcmp(pgupplePackageDefGrp[i]->name,packageName) == 0)
			return(pgupplePackageDefGrp[i]);
	}
	printf(__FILE__,__LINE__,"in UppleFindPackageDef:: [%s] not defined!\n",packageName);
	return(NULL);
}
int UppleConnectPackageDefNew( char *path,char *packageName)
{
	int	num;
	int	index;
	char	mdlName[100];
	char FullPackageName[1024 + 1] = {0};
	
	
	if (UppleIsPackageDefConnected( packageName))
		return(0);


	
	if (strlen(packageName) >= sizeof(pgupplePackageDefGrp[0]->name))
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: packageName [%s] too long!\n",packageName);
		return(errCodeParameter);
	}
	
	if (!UppleIsValidPackageMaxFldNum(num = UppleGetMaxFldNumOfSpecifiedPackageDefNew(path,packageName)))
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: UppleIsValidPackageMaxFldNum [%s] [%d]\n",packageName,num);
		return(errCodePackageDefMDL_InvalidMaxFldNum);
	}
	//printf("test packageName = [%s]\n",packageName);	
	if (!UppleIsValidPackageID(index = UppleGetIDOfSpecifiedPackageDefNew(path,packageName)))
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: UppleIsValidPackageID index = [%d] for [%s]\n",index,packageName);
		return(errCodePackageDefMDL_InvalidPackageIndex);
	}
	//printf("test\n");	
	if (pgupplePackageDefGrp[index] != NULL)
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: package [%s] and package [%s] of the same id [%d]\n",
			pgupplePackageDefGrp[index]->name,packageName,index);
		return(errCodePackageDefMDL_PackageOfSameID);
	}
	memset(mdlName,0,sizeof(mdlName));
	UppleGetMDLNameOfPackageDef(packageName,mdlName);

	//printf("test\n");	

	if ((pgupplePackageDefMDLGrp[index] = UppleConnectSharedMemoryModule_Redis(mdlName,
			sizeof(TUpplePackageDef))) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: UppleConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgupplePackageDefGrp[index] = (PUpplePackageDef)UppleGetAddrOfSharedMemoryModuleUserSpace_Redis(pgupplePackageDefMDLGrp[index])) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackageDef:: PUpplePackageDef!\n");
		pgupplePackageDefMDLGrp[index] = NULL;
		return(errCodeSharedMemoryModule);
	}
	strcpy(pgupplePackageDefGrp[index]->name,packageName);
	pgupplePackageDefGrp[index]->id = index;

	if (UppleIsNewCreatedSharedMemoryModule_Redis(pgupplePackageDefMDLGrp[index]))
		return(UppleReloadPackageDefNew(path,packageName));
	else
		return(0);

}
int UppleGetIDOfSpecifiedPackageDefNew(char *path,char *packageName)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		id;
		
	memset(fileName,0,sizeof(fileName));
	UppleGetConfFileNameOfPackageDefGroupNew(path,fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleGetIDOfSpecifiedPackageDef:: UppleInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UppleGetEnviVarByName(packageName)) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleGetIDOfSpecifiedPackageDef:: UppleGetEnviVarByName for [%s]\n!",packageName);
		id = errCodePackageDefMDL_IDOfPackageNotDefined;
	}
	else
		id = atoi(p);
	
	UppleClearEnvi();
	
	if (UppleIsValidPackageID(id))
		return(id);
	else
		return(errCodePackageDefMDL_InvalidIDOfPackage);	
}
int UppleGetConfFileNameOfPackageDefGroupNew(char *path,char *fileName)
{
	sprintf(fileName,"%s%s",path,conConfFileNameOfPackageDefGroup);
	//printf("fileName = [%s] [%s] [%d]\n",fileName,__FILE__,__LINE__);
	return(0);
}

int UppleGetMaxFldNumOfSpecifiedPackageDefNew(char *path,char *packageName)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		num;
	
	memset(fileName,0,sizeof(fileName));
	UppleGetConfFileNameOfPackageDefNew(path,packageName,fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleGetMaxFldNumOfSpecifiedPackageDef:: UppleInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UppleGetEnviVarByName("maxFldsNum")) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleGetMaxFldNumOfSpecifiedPackageDef:: UppleGetEnviVarByName for [%s]\n!","maxFldsNum");
		num = errCodePackageDefMDL_MaxNumOfPackageFldsNotDefined;
	}
	else
		num = atoi(p);
	
	UppleClearEnvi();
	return(num);	
}
int UppleGetConfFileNameOfPackageDefNew(char *path,char *packageName,char *fileName)
{
	//sprintf(fileName,"%s/packDef/%s.Def",getenv("UPPLEETC"),packageName);
	sprintf(fileName,"%s%s.Def",path,packageName);
	//printf("fileName = [%s] [%s] [%d]\n",fileName,__FILE__,__LINE__);
	return(0);
}
int UppleReloadPackageDefNew(char *path,char *packageName)
{
	int			ret;
	char			*p;
	PUpplePackageFldDef	pFldDef;
	char			fldName[10];
	char			fileName[512];
	int			realNum = 0;
	int			index,i,indexOfVarFld;

	if ((ret = UppleConnectPackageDefNew(path,packageName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleConnectPackageDef [%s]\n",packageName);
		return(ret);
	}
	if (!UppleIsValidPackageID(index = UppleFindPackageDefIndex(packageName)))
	{
		printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleFindPackageDefIndex [%s]!\n",packageName);
		return(errCodePackageDefMDL_InvalidPackageIndex);
	}
	strcpy(pgupplePackageDefGrp[index]->name,packageName);
	pgupplePackageDefGrp[index]->id = index;
	memset(fileName,0,sizeof(fileName));
	UppleGetConfFileNameOfPackageDefNew(path,packageName,fileName);	
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleInitEnvi [%s]!\n",fileName);
		return(ret);
	}
	if ((p = UppleGetEnviVarByName("maxFldsNum")) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleGetEnviVarByName [%s]!\n","maxFldsNum");
		ret = errCodePackageDefMDL_MaxNumOfPackageFldsNotDefined;
		goto errorExit;
	}
	if (!UppleIsValidPackageMaxFldNum(pgupplePackageDefGrp[index]->maxNum = atoi(p)))
	{
		printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleIsValidPackageMaxFldNum [%s] [%d]\n",packageName,pgupplePackageDefGrp[index]->maxNum);
		ret = errCodePackageDefMDL_InvalidMaxFldNum;
		goto errorExit;
	}
	// 2006/6/23，增加
	pgupplePackageDefGrp[index]->fldLenPresentation = conFldLenUseRightDuiQi;
	if ((p = UppleGetEnviVarByName("fldLenPresentation")) != NULL)
	{
		if (!UppleIsValidPackageFldLenPresentation(pgupplePackageDefGrp[index]->fldLenPresentation = atoi(p)))
		{
			printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: [%s] = [%d] not valid!\n","fldLenPresentation",atoi(p));
			pgupplePackageDefGrp[index]->fldLenPresentation = conFldLenUseAscii;
		}
	}
	// 2006/6/23 结束
	for (i = 0; i < pgupplePackageDefGrp[index]->maxNum ; i++)
	{
		pFldDef = &(pgupplePackageDefGrp[index]->fldDefTBL[i]);
		memset(pFldDef,0,sizeof(*pFldDef));
		pFldDef->index = i;
		sprintf(fldName,"%04d",i);
		if ((indexOfVarFld = UppleGetVarIndexOfTheVarName(fldName)) < 0)
		{
			continue;
		}
		// 读取变长指示域
		if ((p = UppleGetEnviVarOfTheIndexByIndex(indexOfVarFld,1)) == NULL)
		{
			printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",indexOfVarFld,1);
			continue;
		}
		pFldDef->lenOfLenFlag = atoi(p);
		// 读取域长度
		if ((p = UppleGetEnviVarOfTheIndexByIndex(indexOfVarFld,2)) == NULL)
		{
			printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",indexOfVarFld,2);
			continue;
		}
		pFldDef->lenOfValue = atoi(p);
		// 读取域值的类型
		if ((p = UppleGetEnviVarOfTheIndexByIndex(indexOfVarFld,3)) == NULL)
		{
			printf(__FILE__,__LINE__,"in UppleReloadPackageDef:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",indexOfVarFld,3);
			continue;
		}
		pFldDef->valueType = atoi(p);
		// 读取域说明
		if ((p = UppleGetEnviVarOfTheIndexByIndex(indexOfVarFld,4)) != NULL)
		{
			if (strlen(p) > sizeof(pFldDef->remark) - 1)
				memcpy(pFldDef->remark,p,sizeof(pFldDef->remark)-1);
			else
				strcpy(pFldDef->remark,p);
		}
	/*
		printf("%04d %02d %04d %d %40s\n",
				pFldDef->index,
				pFldDef->lenOfLenFlag,
				pFldDef->lenOfValue,
				pFldDef->valueType,
				pFldDef->remark);
	*/
		realNum++;
	}
	UppleClearEnvi();		

	return(realNum);
errorExit:
	UppleClearEnvi();
	return(ret);
}
