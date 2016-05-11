//	Author:		zhangyongding
//	Date:		20081018
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "uppleLog.h"
#include "uppleModule.h"
#include "uppleEnv.h"
#include "uppleCommand.h"

#include "termPackage.h"
#include "upplePackageDef.h"
#include "termTransFldDefTBL.h"

#define USE_REDIS

PUppleSharedMemoryModule		pguppleTermTransFldDefMDL = NULL;
PUppleTermTransFldDefTBL		pguppleTermTransFldDefTBL = NULL;

int UppleGetNameOfTermTransFldDefTBL(char *fileName)
{
	sprintf(fileName,"%s/termDef/%s",getenv("UPPLEETC"),conConfFileNameOfTermTransFldDefTBL);
	return(0);
}

int UppleGetMaxNumOfTermTransFldDef()
{
	return(conMaxNumOfYLFldDef);
}

int UppleIsTermTransFldDefTBLConnected()
{
	if ((pguppleTermTransFldDefTBL == NULL) || (pguppleTermTransFldDefMDL == NULL))
		return(0);
	else
		return(1);
}

int UppleConnectTermTransFldDefTBL()
{
	int				ret;
	
	if (UppleIsTermTransFldDefTBLConnected())	// 已经连接
		return(0);
	
#ifdef USE_REDIS
	if ((pguppleTermTransFldDefMDL = UppleConnectSharedMemoryModule_Redis(conMDLNameOfTermTransFldDefTBL,
			sizeof(TUppleTermTransFldDefTBL))) == NULL)
	{
		UppleUserErrLog("in UppleConnectTermTransFldDefTBL:: UppleConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pguppleTermTransFldDefTBL = (PUppleTermTransFldDefTBL)UppleGetAddrOfSharedMemoryModuleUserSpace_Redis(pguppleTermTransFldDefMDL)) == NULL)
	{
		UppleUserErrLog("in UppleConnectTermTransFldDefTBL:: PUppleTermTransFldDefTBL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if (UppleIsNewCreatedSharedMemoryModule_Redis(pguppleTermTransFldDefMDL))
	{
		pguppleTermTransFldDefTBL->realNum = 0;
		return(UppleReloadTermTransFldDefTBL());
	}
	else
		return(0);

#else
	if ((pguppleTermTransFldDefMDL = UppleConnectSharedMemoryModule(conMDLNameOfTermTransFldDefTBL,
			sizeof(TUppleTermTransFldDefTBL))) == NULL)
	{
		UppleUserErrLog("in UppleConnectTermTransFldDefTBL:: UppleConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pguppleTermTransFldDefTBL = (PUppleTermTransFldDefTBL)UppleGetAddrOfSharedMemoryModuleUserSpace(pguppleTermTransFldDefMDL)) == NULL)
	{
		UppleUserErrLog("in UppleConnectTermTransFldDefTBL:: PUppleTermTransFldDefTBL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if (UppleIsNewCreatedSharedMemoryModule(pguppleTermTransFldDefMDL))
	{
		pguppleTermTransFldDefTBL->realNum = 0;
		return(UppleReloadTermTransFldDefTBL());
	}
	else
		return(0);
#endif
}

int UppleDisconnectTermTransFldDefTBL()
{
	pguppleTermTransFldDefTBL = NULL;
	return(UppleDisconnectShareModule(pguppleTermTransFldDefMDL));
}

int UppleRemoveTermTransFldDefTBL()
{
	UppleDisconnectTermTransFldDefTBL();
	return(UppleRemoveSharedMemoryModule(conMDLNameOfTermTransFldDefTBL));
}

int UppleReloadTermTransFldDefTBL()
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i;
	PUpplePackageAutoFldDef	pautoFldDef = NULL;
	int			newAdded = 0;
	PUpplePackageFldDef 	pfldDef = NULL;
	int			index;
	
#ifdef USE_REDIS
	{
	    char GName[ 128 ];

	    char s_realNum[20];
	    int realNum= 0;

	    sprintf( GName, "#TermTransFldDefTBL.realNum" );
	    memset( s_realNum, 0x00, sizeof(s_realNum) );
	    GET0( GName, s_realNum);

	    if( *s_realNum)
		{

		    sprintf( GName, "#TermTransFldDefTBL" );
		    memset( pguppleTermTransFldDefTBL, 0x00, sizeof(TUppleTermTransFldDefTBL)  );
		    GET0( GName, pguppleTermTransFldDefTBL );
                        /* modified by caiyucheng, 2013-08-22, begin*/
#if 0
		    UppleLog2(__FILE__,__LINE__, "#TEST:TermTransFldDefTBL...2,pguppleTermTransFldDefTBL->realNum=[%d]\n", pguppleTermTransFldDefTBL->realNum);

		    return(pguppleTermTransFldDefTBL->realNum);
#else
                        UppleLog2(__FILE__,__LINE__, "#TEST:TermTransFldDefTBL...2,pguppleTermTransFldDefTBL->realNum=[%d],s_realNum=[%s], s_realNum[0]=[%x]\n", pguppleTermTransFldDefTBL->realNum, s_realNum, *s_realNum);
                         if(pguppleTermTransFldDefTBL->realNum <= 0)
                                UppleLog2(__FILE__,__LINE__, "#TEST:TermTransFldDefTBL...error,pguppleTermTransFldDefTBL->realNum=[%d] <= 0, load again\n", pguppleTermTransFldDefTBL->realNum);
                        else /* 确保读取到的REC配置项的数目大于零,才返回正确 */
                                return(pguppleTermTransFldDefTBL->realNum);
#endif
                        /* modified by caiyucheng, 2013-08-22, end*/
		}

	}

#endif

	    UppleLog2(__FILE__,__LINE__, "#TEST:TermTransFldDefTBL...3  \n"  );
	if ((ret = UppleConnectTermTransFldDefTBL()) < 0)
	{
		UppleUserErrLog("in UppleReloadTermTransFldDefTBL:: UppleConnectTermTransFldDefTBL!\n");
		return(ret);
	}
	    UppleLog2(__FILE__,__LINE__, "#TEST:TermTransFldDefTBL...4  \n"  );
	
	memset(fileName,0,sizeof(fileName));
	UppleGetNameOfTermTransFldDefTBL(fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		UppleUserErrLog("in UppleReloadTermTransFldDefTBL:: UppleInitEnvi [%s]!\n",fileName);
		return(ret);
	}
	
	if (pguppleTermTransFldDefTBL->realNum < 0)
		pguppleTermTransFldDefTBL->realNum = 0;
		
	for (i = 0; (i < UppleGetEnviVarNum()) && (pguppleTermTransFldDefTBL->realNum < UppleGetMaxNumOfTermTransFldDef()); i++)
	{
		// 读取索引号
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UppleUserErrLog("in UppleReloadTermTransFldDefTBL:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		index = atoi(p);
		// 读取名称
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UppleUserErrLog("in UppleReloadTermTransFldDefTBL:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		if ((pautoFldDef = UppleFindTermTransFldDef(p)) == NULL)
		{
			pautoFldDef = &(pguppleTermTransFldDefTBL->rec[pguppleTermTransFldDefTBL->realNum]);
			newAdded = 1;
		}
		else
			newAdded = 0;
		if (strlen(p) >= sizeof(pautoFldDef->name))
		{
			UppleUserErrLog("in UppleReloadTermTransFldDefTBL:: name [%s] too long for index [%d]\n",p,i);
			continue;
		}
		strcpy(pautoFldDef->name,p);
		// 设置索引号
		pautoFldDef->index = index;
		// 读取赋值方法
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UppleUserErrLog("in UppleReloadTermTransFldDefTBL:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
			continue;
		}
		UppleLog("in UppleReloadTermTransFldDefTBL:: i = [%04d] p = [%s]\n",i,p);
		pautoFldDef->method = UppleConvertDefFlagIntoPackageAutoFldValueSetMethod(p);
		// 读取赋值方法使用的参数
		memset(pautoFldDef->var,0,sizeof(pautoFldDef->var));
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,3)) != NULL)
		{
			if (strlen(p) >= sizeof(pautoFldDef->var))
				memcpy(pautoFldDef->var,p,sizeof(pautoFldDef->var)-1);
			else
				strcpy(pautoFldDef->var,p);
		}
		pautoFldDef->maxLen = 0;
		pautoFldDef->type = conPackageAutoFldType_String;
		if (newAdded)
			pguppleTermTransFldDefTBL->realNum += 1;
	}

	UppleClearEnvi();
	
	for (i = 0; i < pguppleTermTransFldDefTBL->realNum; i++)
	{
		pautoFldDef = &(pguppleTermTransFldDefTBL->rec[i]);
		if ((pfldDef = UppleFindTermFldDefOfIndex(pautoFldDef->index)) == NULL)
		{
			UppleUserErrLog("in UppleReloadTermTransFldDefTBL:: UppleFindTermFldDefOfIndex [%d] [%s]\n",i,p);
			continue;
		}
		//UpplePrintPackageFldDefToFile(pfldDef,stdout);
		// 设置类型
		switch (pfldDef->valueType)
		{
			case	conBitMapFld:
			case	conBitsFld:
				pautoFldDef->type = conPackageAutoFldType_Bits;
				break;
			default:
				pautoFldDef->type = conPackageAutoFldType_String;
				break;
		}
		// 设置最大长度
		pautoFldDef->maxLen = pfldDef->lenOfValue;
	}

	    UppleLog2(__FILE__,__LINE__, "#TEST:TermTransFldDefTBL...5  \n"  );
#ifdef USE_REDIS
	{
	    char GName[ 128 ];

	    char s_realNum[20];
	    int realNum= 0;

	    memset( s_realNum, 0x00, sizeof(s_realNum) );
	    sprintf( s_realNum, "%d", pguppleTermTransFldDefTBL->realNum);

            sprintf( GName, "#TermTransFldDefTBL" );
	    SET0( GName, pguppleTermTransFldDefTBL, sizeof(TUppleTermTransFldDefTBL)+1  );
	    //UppleLog2(__FILE__,__LINE__, "#TEST:TermTransFldDefTBL...6, realNum=[%s]  \n", s_realNum);
	    sprintf( GName, "#TermTransFldDefTBL.realNum" );
	    SET0( GName, s_realNum, strlen(s_realNum)+1 );	    

	    //UppleLog2(__FILE__,__LINE__, "#TEST:TermTransFldDefTBL...7, sizeof(TUppleTermTransFldDefTBL)=[%d]  \n", sizeof(TUppleTermTransFldDefTBL));
	}
#endif
			
	return(pguppleTermTransFldDefTBL->realNum);
}

int UpplePrintTermTransFldDefTBLToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if (fp == NULL)
	{
		UppleUserErrLog("in UpplePrintTermTransFldDefTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UppleConnectTermTransFldDefTBL()) < 0)
	{
		UppleUserErrLog("in UpplePrintTermTransFldDefTBLToFile:: UppleConnectTermTransFldDefTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pguppleTermTransFldDefTBL->realNum; i++)
	{
		if (UpplePrintPackageAutoFldDefToFile(&(pguppleTermTransFldDefTBL->rec[i]),fp) < 0)
			continue;
		num++;
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UppleIsQuit(UpplePressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
	}
	fprintf(fp,"termTransFldDefNum  = [%d]\n",num);
	return(0);
}

PUpplePackageAutoFldDef UppleFindTermTransFldDef(char *name)
{
	int	i;
	int	ret;
		
	if (name == NULL)
	{
		UppleUserErrLog("in UppleFindTermTransFldDef:: name is NULL!\n");
		return(NULL);
	}
	if ((ret = UppleConnectTermTransFldDefTBL()) < 0)
	{
		UppleUserErrLog("in UppleFindTermTransFldDef:: UppleConnectTermTransFldDefTBL!\n");
		return(NULL);
	}
	
	for (i = 0; i < pguppleTermTransFldDefTBL->realNum; i++)
	{
		if (strcmp(pguppleTermTransFldDefTBL->rec[i].name,name) == 0)
			return(&(pguppleTermTransFldDefTBL->rec[i]));
	}
	UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransFldDef:: not find for [%s]\n",name);
	return(NULL);
}

int UpplePrintTermTransFldDefOfNameInTBLToFile(char *name,FILE *fp)
{
	return(UpplePrintPackageAutoFldDefToFile(UppleFindTermTransFldDef(name),fp));
}

int UpplePrintTermTransFldDefOfIndexToFile(int index,FILE *fp)
{
	int	i;
	int	ret;
	int	num = 0;
	
	if (fp == NULL)
	{
		UppleUserErrLog("in UpplePrintTermTransFldDefOfIndexToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
		
	if ((ret = UppleConnectTermTransFldDefTBL()) < 0)
	{
		UppleUserErrLog("in UpplePrintTermTransFldDefOfIndexToFile:: UppleConnectTermTransFldDefTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < pguppleTermTransFldDefTBL->realNum; i++)
	{
		if (pguppleTermTransFldDefTBL->rec[i].index != index)
			continue;
		if (UpplePrintPackageAutoFldDefToFile(&(pguppleTermTransFldDefTBL->rec[i]),fp) < 0)
			continue;
		num++;
		if ((num != 0) && (num % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UppleIsQuit(UpplePressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
	}
	fprintf(fp,"termTransFldDefNum  = [%d]\n",num);
	return(0);
}

