//	Author:		zhangyongding
//	Date:		20081020
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "uppleLog.h"
#include "uppleModule.h"
#include "uppleEnv.h"
#include "uppleCommand.h"

#include "termTransDefTBL.h"

#define USE_REDIS

PUppleSharedMemoryModule	pguppleTermTransDefTBLMDL = NULL;
PUppleTermTransDefTBL		pguppleTermTransDefTBL = NULL;
char				guppleUnknownTermTransName[40];

int UppleInitTermTransDef(PUppleTermTransDef pTermTransDef)
{
	int	i;

	if (pTermTransDef == NULL)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleInitTermTransDef:: null pointer!\n");
		return(errCodeParameter);
	}
	memset(pTermTransDef,0,sizeof(*pTermTransDef));
	return(0);
}

int UppleGetConfFileNameOfTermTransDefTBL(char *fileName)
{
	sprintf(fileName,"%s/termDef/%s",getenv("UPPLEETC"),conConfFileNameOfTermTransDefTBL);
	return(0);
}

int UppleIsTermTransDefTBLConnected()
{
	if ((pguppleTermTransDefTBL == NULL) || (pguppleTermTransDefTBLMDL == NULL))
		return(0);
	else
		return(1);
}

int UppleGetMaxNumOfTermTransDef()
{
	return(conMaxNumOfTermTransDef);	
}

int UppleConnectTermTransDefTBL()
{
	int				ret;

	if (UppleIsTermTransDefTBLConnected())	// 已经连接
		return(0);

#ifdef USE_REDIS
	if ((pguppleTermTransDefTBLMDL = UppleConnectSharedMemoryModule_Redis(conMDLNameOfTermTransDefTBL,
					sizeof(TUppleTermTransDefTBL))) == NULL)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleConnectTermTransDefTBL:: UppleConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pguppleTermTransDefTBL = (PUppleTermTransDefTBL)UppleGetAddrOfSharedMemoryModuleUserSpace_Redis(pguppleTermTransDefTBLMDL)) == NULL)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleConnectTermTransDefTBL:: PUppleTermTransDef!\n");
		return(errCodeSharedMemoryModule);
	}

	if (UppleIsNewCreatedSharedMemoryModule_Redis(pguppleTermTransDefTBLMDL))
	{
		return(UppleReloadTermTransDefTBL());
	}
	else
		return(0);

#else
	if ((pguppleTermTransDefTBLMDL = UppleConnectSharedMemoryModule(conMDLNameOfTermTransDefTBL,
					sizeof(TUppleTermTransDefTBL))) == NULL)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleConnectTermTransDefTBL:: UppleConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pguppleTermTransDefTBL = (PUppleTermTransDefTBL)UppleGetAddrOfSharedMemoryModuleUserSpace(pguppleTermTransDefTBLMDL)) == NULL)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleConnectTermTransDefTBL:: PUppleTermTransDef!\n");
		return(errCodeSharedMemoryModule);
	}

	if (UppleIsNewCreatedSharedMemoryModule(pguppleTermTransDefTBLMDL))
	{
		return(UppleReloadTermTransDefTBL());
	}
	else
		return(0);
#endif
}


int UppleDisconnectTermTransDefTBL()
{
	pguppleTermTransDefTBL = NULL;
	return(UppleDisconnectShareModule(pguppleTermTransDefTBLMDL));
}

int UppleRemoveTermTransDefTBL()
{
	UppleDisconnectTermTransDefTBL();
	return(UppleRemoveSharedMemoryModule(conMDLNameOfTermTransDefTBL));
}

int UppleReloadTermTransDefTBL()
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i;
	int			realNum = 0;

#ifdef USE_REDIS
	{
		char GName[ 128 ];

		char s_realNum[20];
		int realNum= 0;

		sprintf( GName, "#TermTransDefTBL.realNum" );
		memset( s_realNum, 0x00, sizeof(s_realNum) );
		GET0( GName, s_realNum);

		if( *s_realNum)
		{

			sprintf( GName, "#TermTransDefTBL" );
			memset( pguppleTermTransDefTBL, 0x00, sizeof(TUppleTermTransDefTBL)  );
			GET0( GName, pguppleTermTransDefTBL );

			UppleLog2(__FILE__,__LINE__, "#TEST:TermTransDefTBL...2,pguppleTermTransDefTBL->realNum=[%d]\n", pguppleTermTransDefTBL->realNum);

			if(pguppleTermTransDefTBL->realNum <= 0)
				UppleLog2(__FILE__,__LINE__, "#TEST:TermTransDefTBL...error,pguppleTermTransDefTBL->realNum=[%d] <= 0, load again\n", pguppleTermTransDefTBL->realNum);
			else /* 确保读取到的REC配置项的数目大于零,才返回正确 */
				return(pguppleTermTransDefTBL->realNum);
		}

	}
#endif

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleReloadTermTransDefTBL:: UppleConnectTermTransDefTBL!\n");
		return(ret);
	}

	for (i = 0,pguppleTermTransDefTBL->maxNumOfRecords = UppleGetMaxNumOfTermTransDef(); i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{
		UppleInitTermTransDef(&(pguppleTermTransDefTBL->records[i]));
	}

	memset(fileName,0,sizeof(fileName));
	UppleGetConfFileNameOfTermTransDefTBL(fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleReloadTermTransDefTBL:: UppleInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	for (i = 0; (i < UppleGetEnviVarNum()) && (realNum < pguppleTermTransDefTBL->maxNumOfRecords); i++)
	{
		// 读取处理模块的ID号
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleReloadTermTransDefTBL:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (!UppleIsValidTermTransDealMDLID(pguppleTermTransDefTBL->records[realNum].idOfDealMDL = UppleGenerateDealMDLIDOfTermTrans(atol(p))))
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleReloadTermTransDefTBL:: UppleIsValidTermTransDealMDLID [%lld]\n",pguppleTermTransDefTBL->records[realNum].idOfDealMDL);
			continue;
		}
		// 读取MTI
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleReloadTermTransDefTBL:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (strlen(p) > sizeof(pguppleTermTransDefTBL->records[realNum].fld000) - 1)
			continue;
		strcpy(pguppleTermTransDefTBL->records[realNum].fld000,p);
		// 读取处理码
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleReloadTermTransDefTBL:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		if (strlen(p) > sizeof(pguppleTermTransDefTBL->records[realNum].fld003) - 1)
			continue;
		strcpy(pguppleTermTransDefTBL->records[realNum].fld003,p);
		// 路由标识
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,3)) == NULL)
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleReloadTermTransDefTBL:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		if (strlen(p) > sizeof(pguppleTermTransDefTBL->records[realNum].routeid) - 1)
			continue;
		strcpy(pguppleTermTransDefTBL->records[realNum].routeid,p);
		// 读取说明
		memset(pguppleTermTransDefTBL->records[realNum].remark,0,sizeof(pguppleTermTransDefTBL->records[realNum].remark));
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,4)) != NULL)
		{
			if (strlen(p) > sizeof(pguppleTermTransDefTBL->records[realNum].remark) - 1)
				memcpy(pguppleTermTransDefTBL->records[realNum].remark,p,sizeof(pguppleTermTransDefTBL->records[realNum].remark) - 1);
			else
				strcpy(pguppleTermTransDefTBL->records[realNum].remark,p);
		}
		realNum++;
	}
	pguppleTermTransDefTBL->maxNumOfRecords = realNum;
	pguppleTermTransDefTBL->realNum = realNum;
	UppleClearEnvi();

#ifdef USE_REDIS
	{
		char GName[ 128 ];

		char s_realNum[20];
		//int realNum= 0;

		memset( s_realNum, 0x00, sizeof(s_realNum) );
		sprintf( s_realNum, "%d", pguppleTermTransDefTBL->realNum);	    

		sprintf( GName, "#TermTransDefTBL" );
		SET0( GName, pguppleTermTransDefTBL, sizeof(TUppleTermTransDefTBL)+1  );
		sprintf( GName, "#TermTransDefTBL.realNum" );
		/*caiyucheng, test*/
		//SET0( GName, s_realNum, strlen(s_realNum)+1 );
		SET0( GName, s_realNum, strlen(s_realNum));
	}
#endif

	return(realNum);
}

int UpplePrintTermTransDefTBLToFile(FILE *fp)
{
	int	i;
	int	ret;
	int	realNum = 0;

	if (fp == NULL)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UpplePrintTermTransDefTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UpplePrintTermTransDefTBLToFile:: UppleConnectTermTransDefTBL!\n");
		return(errCodeISO8583MDL_8583PackageClassTBLConnected);
	}

	for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{
		if (UpplePrintTermTransDefToFile(&(pguppleTermTransDefTBL->records[i]),fp) < 0)
			continue;
		realNum++;
		if ((realNum != 0) && (realNum % 23 == 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UppleIsQuit(UpplePressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
	}
	fprintf(fp,"termTransDefNum = [%d]\n",realNum);
	return(realNum);
}

int UpplePrintTermTransDefToFile(PUppleTermTransDef pTermTransDef,FILE *fp)
{
	char	tmpBuf[128+1];

	if ((pTermTransDef == NULL) || (fp == NULL))
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UpplePrintTermTransDefTBLToFile:: fp is NULL!\n");
		return(errCodeParameter);
	}
	fprintf(fp,"%012d %04s %06s %2s %s\n",pTermTransDef->idOfDealMDL,pTermTransDef->fld000,pTermTransDef->fld003,pTermTransDef->routeid,pTermTransDef->remark);
	return(0);
}

int UpplePrintTermTransDefOfFld000OrFld003ToFile(char *fld,FILE *fp)
{
	int	i;
	int	ret;
	int	realNum = 0;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UpplePrintTermTransDefOfFld000OrFld003ToFile:: UppleConnectTermTransDefTBL!\n");
		return(errCodeISO8583MDL_8583PackageClassTBLConnected);
	}
	for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{
		if (strcmp(fld,pguppleTermTransDefTBL->records[i].fld000) == 0)
		{
			if (UpplePrintTermTransDefToFile(&(pguppleTermTransDefTBL->records[i]),fp) < 0)
				continue;
			realNum++;
		}
	}
	if (realNum > 0)
		goto exitNow;
	for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{
		if (strcmp(fld,pguppleTermTransDefTBL->records[i].fld003) == 0)
		{
			if (UpplePrintTermTransDefToFile(&(pguppleTermTransDefTBL->records[i]),fp) < 0)
				continue;
			realNum++;
		}
	}
exitNow:
	fprintf(fp,"num = %d\n",realNum);
	return(realNum);
}

PUppleTermTransDef UppleFindTermTransDef(char *fld000,char *fld003,char *routeid)
{
	int			i;
	int			ret;
	PUppleTermTransDef	pdef = NULL;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDef:: UppleConnectTermTransDefTBL!\n");
		return(NULL);
	}

	if ((fld000[2] - '0') % 2 != 0)		// 是响应
		fld000[2] = fld000[2] - 1;

	for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{
		if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
				(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
				(strncmp(routeid,pguppleTermTransDefTBL->records[i].routeid,2) == 0))
		{
			pdef = &(pguppleTermTransDefTBL->records[i]);
			return(pdef);
		}
	}
	return(NULL);
}

char *UppleGetNameOfTermTrans(char *fld000,char *fld003,char *routeid)
{
	PUppleTermTransDef pdef;

	if ((pdef = UppleFindTermTransDef(fld000,fld003,routeid)) == NULL)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleGetNameOfTermTrans:: UppleFindTermTransDef [%s|%s|%s]!\n",fld000,fld003,routeid);
		return(NULL);
	}
	return(pdef->remark);
}

int UppleGetTermFld000AndFld003ByRemark(char *remark,char *fld000,char *fld003,char *routeid)
{
	int	i;
	int	ret;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleGetTermFld000AndFld003ByRemark:: UppleConnectTermTransDefTBL!\n");
		return(errCodeISO8583MDL_8583PackageClassTBLConnected);
	}
	for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{
		if (strcmp(remark,pguppleTermTransDefTBL->records[i].remark) == 0)
		{
			strcpy(fld000,pguppleTermTransDefTBL->records[i].fld000);
			strcpy(fld003,pguppleTermTransDefTBL->records[i].fld003);
			strcpy(routeid,pguppleTermTransDefTBL->records[i].routeid);
			return(0);
		}
	}
	return(errCodeISO8583MDL_TransNotDefined);
}

TUppleDealModuleID UppleDecideTermTransDealMDLID(char *fld000,char *fld003,char *routeid)
{
	int			i;
	int			ret;
	TUppleDealModuleID	commMDLID = -1;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecideTermTransDealMDLID:: UppleConnectTermTransDefTBL!\n");
		return(errCodeISO8583MDL_8583TransDefTBLConnected);
	}
	for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{

		if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
				(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
				(strncmp(routeid,pguppleTermTransDefTBL->records[i].routeid,2) == 0))
		{
			return(pguppleTermTransDefTBL->records[i].idOfDealMDL);
		}
	}
	UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecideTermTransDealMDLID:: [%s|%s|%s] not defined!\n",fld000,fld003,routeid);
	return(errCodeISO8583MDL_TransNotDefined);
}

TUppleDealModuleID UppleDecideTermTransDealMDLIDByRemarkOrItemID(char *remarkOrItemID)
{
	int			i;
	int			ret;
	long			id;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecideTermTransDealMDLIDByRemarkOrItemID:: UppleConnectTermTransDefTBL!\n");
		return(errCodeISO8583MDL_8583TransDefTBLConnected);
	}
	for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{
		if (strcmp(remarkOrItemID,pguppleTermTransDefTBL->records[i].remark) == 0)
		{
			/*test only, caiyucheng */
			printf("[%s-%d][%s],len[%d]--[%s]len[%d],id[%lld]\n",__FILE__,__LINE__,remarkOrItemID,strlen(remarkOrItemID),pguppleTermTransDefTBL->records[i].remark,strlen(pguppleTermTransDefTBL->records[i].remark),pguppleTermTransDefTBL->records[i].idOfDealMDL);
			return(*((TUppleDealModuleID *)(& (pguppleTermTransDefTBL->records[i].idOfDealMDL))));
		}
	}
	if (!UppleIsDigitStr(remarkOrItemID))
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecideTermTransDealMDLIDByRemarkOrItemID:: remark [%s]!\n",remarkOrItemID);
		return(errCodeISO8583MDL_TransNotDefined);
	}
	id = atol(remarkOrItemID);
	for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{
		if (id == UppleGetItemMDLIDOutOfDealMDLID(pguppleTermTransDefTBL->records[i].idOfDealMDL))
			return(pguppleTermTransDefTBL->records[i].idOfDealMDL);
	}
	UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecideTermTransDealMDLIDByRemarkOrItemID:: id [%s]!\n",remarkOrItemID);
	return(errCodeISO8583MDL_TransNotDefined);
}
int UppleDecideTermTransDealMDLIDByRemarkOrItemID_X(char *remarkOrItemID, TUppleDealModuleID *pRst )
{
	int			i;
	int			ret;
	long			id;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecideTermTransDealMDLIDByRemarkOrItemID:: UppleConnectTermTransDefTBL!\n");
		return(errCodeISO8583MDL_8583TransDefTBLConnected);
	}
	for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{
		if (strcmp(remarkOrItemID,pguppleTermTransDefTBL->records[i].remark) == 0)
		{
			/*test only, caiyucheng */
			printf("[%s-%d][%s],len[%d]--[%s]len[%d],id[%lld]\n",__FILE__,__LINE__,remarkOrItemID,strlen(remarkOrItemID),pguppleTermTransDefTBL->records[i].remark,strlen(pguppleTermTransDefTBL->records[i].remark),pguppleTermTransDefTBL->records[i].idOfDealMDL);
			*pRst = (TUppleDealModuleID)(pguppleTermTransDefTBL->records[i].idOfDealMDL);
			return 0;
		}
	}
	if (!UppleIsDigitStr(remarkOrItemID))
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecideTermTransDealMDLIDByRemarkOrItemID:: remark [%s]!\n",remarkOrItemID);
		return(errCodeISO8583MDL_TransNotDefined);
	}
	id = atol(remarkOrItemID);
	for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	{
		if (id == UppleGetItemMDLIDOutOfDealMDLID(pguppleTermTransDefTBL->records[i].idOfDealMDL))
		{
			*pRst = (TUppleDealModuleID)(pguppleTermTransDefTBL->records[i].idOfDealMDL);
			return 0;
		}

	}
	UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecideTermTransDealMDLIDByRemarkOrItemID:: id [%s]!\n",remarkOrItemID);
	return(errCodeISO8583MDL_TransNotDefined);
}

int UppleFindTermTransDefName(char *fld000,char *fld003,char *fld038,char *CfgName)
{
	int			i;
	int			ret;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDef:: UppleConnectTermTransDefTBL!\n");
		return(NULL);
	}
	if(0 == strlen(fld038) ){
		for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
		{
			if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) && 
					(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
					(strncmp("CU",pguppleTermTransDefTBL->records[i].routeid,2) == 0)	) {
				UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
				strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
				return 0;
			}
		}
	} else{
		for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
		{
			if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) && 
					(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
					(strncmp("PR",pguppleTermTransDefTBL->records[i].routeid,2) == 0)	) {
				UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
				strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
				return 0;
			}
		}

	}
	return(0);
}

int UppleFindTermTransDefNameICBC(char *fld000,char *fld003,char *fld038,long port,char *CfgName)
{
	int                     i;
	int                     ret;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDef:: UppleConnectTermTransDefTBL!\n");
		return(NULL);
	}
	if(0 == strlen(fld038) ){

		if((port == 0)&&((0 == atoi(fld003))&&((200 == atoi(fld000)))))
		{
			for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
			{
				if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
						(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
						(strncmp("WC",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
					UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
							i,pguppleTermTransDefTBL->records[i].remark);
					strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
					return 0;
				}
			}
		}
		else
		{
			for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
			{
				if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
						(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
						(strncmp("IC",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
					UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
							i,pguppleTermTransDefTBL->records[i].remark);
					strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
					return 0;
				}
			}
		}
	} else{
		for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
		{
			if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
					(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
					(strncmp("BC",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
				UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
				strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
				return 0;
			}
		}

	}
	return(0);
}


int UppleFindTermTransDefNameABC(char *fld000,char *fld003,char *fld038,char *CfgName)
{
	int                     i;
	int                     ret;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDef:: UppleConnectTermTransDefTBL!\n");
		return(NULL);
	}
	if(0 == strlen(fld038) ){
		for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
		{
			if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
					(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
					(strncmp("AC",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
				UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
				strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
				return 0;
			}
		}
	} else{
		for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
		{
			if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
					(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
					(strncmp("AB",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
				UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
				strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
				return 0;
			}
		}

	}
	return(0);
}



int UppleFindTermTransDefNameABCAddPort(char *fld000,char *fld003,char *fld038,long port,char *CfgName)
{
	int                     i;
	int                     ret;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDef:: UppleConnectTermTransDefTBL!\n");
		return(NULL);
	}
	if(0 == strlen(fld038) ){

		if((port == 0)&&((0 == atoi(fld003))&&((200 == atoi(fld000)))))
		{
			for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
			{
				if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
						(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
						(strncmp("CC",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
					UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
							i,pguppleTermTransDefTBL->records[i].remark);
					strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
					return 0;
				}
			}
		}
		else
		{
			for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
			{
				if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
						(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
						(strncmp("AC",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
					UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
							i,pguppleTermTransDefTBL->records[i].remark);
					strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
					return 0;
				}
			}
		}
	} else{
		for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
		{
			if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
					(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
					(strncmp("AA",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
				UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
				strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
				return 0;
			}
		}

	}
	return(0);
}

/******************************************************************/
/*             功 能: 获取交易配置文件名字                        */
/*             描 述: 通过第一域、第三域、路由标识                */
/*                    端口号获取配置文件名                        */
/*             作 者: zhu hui                                     */
/*             时 间: 2014-7-28                                   */
/******************************************************************/
int UppleFindTermTransDefNameFTS2BOC(char *fld000,char *fld003,char *fld038,long port,char *CfgName)
{           
	int                     i;
	int                     ret;

	UppleLog2(__FILE__,__LINE__,"in UppleFindTermTransDefNameFTS2BOC::begin\n");
	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{           
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDefNameFTS2BOC:: UppleConnectTermTransDefTBL!\n");
		return -1;
	}
	//zzq add
	char ctype[10]={0};
	if(8385 == port || 6788 == port)
		memcpy(ctype,"AX",2);
	else if(6768 == port)
		memcpy(ctype,"AZ",2);
	else
		memcpy(ctype,"AY",2);
	for(i=0;i<pguppleTermTransDefTBL->maxNumOfRecords;++i){
		if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
				(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
				(strncmp(ctype,pguppleTermTransDefTBL->records[i].routeid,2) == 0)){
			UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
			strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
			break;
		}
	}
	ret = (i == pguppleTermTransDefTBL->maxNumOfRecords) ? -1:0;
	UppleLog2(__FILE__,__LINE__,"in UppleFindTermTransDefNameFTS2BOC::end!\n");
	return ret;
	/*
	   if(8385 == port )
	   {

	   for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	   {
	   if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
	   (strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
	   (strncmp("AX",pguppleTermTransDefTBL->records[i].routeid,2) == 0))
	   {
	   UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
	   strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
	   }
	   }
	   }
	   else if( 6768 == port)
	   {
	   for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	   {
	   if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
	   (strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
	   (strncmp("AZ",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   )
	   {
	   UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
	   strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
	   }
	   }

	   }
	   else
	   {
	   for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
	   {
	   if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
	   (strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
	   (strncmp("AY",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   )
	   {
	   UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
	   strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
	   }
	   }

	   }
	   UppleLog2(__FILE__,__LINE__,"in UppleFindTermTransDefNameFTS2BOC::end!\n");
	   return(0);//*/
}

int UppleFindTermTransDefNameICBCNAddPort(char *fld000,char *fld003,char *fld038,long port,char *CfgName)
{
	int                     i;
	int                     ret;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDef:: UppleConnectTermTransDefTBL!\n");
		return(NULL);
	}
	if(0 == strlen(fld038) ){

		if((port == 0)&&((0 == atoi(fld003))&&((200 == atoi(fld000)))))
		{
			for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
			{
				if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
						(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
						(strncmp("II",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
					UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
							i,pguppleTermTransDefTBL->records[i].remark);
					strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
					return 0;
				}
			}
		}
		else
		{
			for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
			{
				if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
						(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
						(strncmp("IN",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
					UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
							i,pguppleTermTransDefTBL->records[i].remark);
					strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
					return 0;
				}
			}
		}
	} else{
		for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
		{
			if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
					(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
					(strncmp("NN",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
				UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
				strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
				return 0;
			}
		}

	}
	return(0);
}

int UppleFindTermTransDefNameChanjetAddPort(char *fld000,char *fld003,char *fld038,long port,char *CfgName)
{
	int                     i;
	int                     ret;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDef:: UppleConnectTermTransDefTBL!\n");
		return(NULL);
	}
	if(0 == strlen(fld038) ){

		if((port == 0)&&((0 == atoi(fld003))&&((200 == atoi(fld000)))))
		{
			for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
			{
				if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
						(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
						(strncmp("CJ",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
					UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
							i,pguppleTermTransDefTBL->records[i].remark);
					strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
					return 0;
				}
			}
		}
		else if(8801 == port)
                {
                        for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
                        {
                                if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
                                                (strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
                                                (strncmp("CP",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
                                        UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
                                                        i,pguppleTermTransDefTBL->records[i].remark);
                                        strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
                                        return 0;
                                }
                        }
                }
		else
		{
			for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
			{
				if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
						(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
						(strncmp("JJ",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
					UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
							i,pguppleTermTransDefTBL->records[i].remark);
					strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
					return 0;
				}
			}
		}
	} else{
		for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
		{
			if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
					(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
					(strncmp("JC",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
				UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
				strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
				return 0;
			}
		}

	}
	return(0);
}

int UppleFindTermTransDefNameYFTAddPort(char *fld000,char *fld003,char *fld038,long port,char *CfgName)
{
	int                     i;
	int                     ret;

	if ((ret = UppleConnectTermTransDefTBL()) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDefNameYFTAddPort:: UppleConnectTermTransDefTBL!\n");
		return(NULL);
	}
	if (0 == strlen(fld038))
	{

		if ((port == 0)&&((0 == atoi(fld003))&&((200 == atoi(fld000)))))
		{
			for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
			{
				if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
						(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
						(strncmp("YY",pguppleTermTransDefTBL->records[i].routeid,2) == 0))
				{
					UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
							i,pguppleTermTransDefTBL->records[i].remark);
					strcpy(CfgName, pguppleTermTransDefTBL->records[i].remark);
					return(0);
				}
			}
		}
		else
		{
			for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
			{
				if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
						(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
						(strncmp("YF",pguppleTermTransDefTBL->records[i].routeid,2) == 0))
				{
					UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
							i,pguppleTermTransDefTBL->records[i].remark);
					strcpy(CfgName, pguppleTermTransDefTBL->records[i].remark);
					return(0);
				}
			}
		}
	}
	else
	{
		for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
		{
			if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
					(strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
					(strncmp("YF",pguppleTermTransDefTBL->records[i].routeid,2) == 0))
			{
				UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
				strcpy(CfgName, pguppleTermTransDefTBL->records[i].remark);
				return(0);
			}
		}

	}
	return(0);
}

int UppleFindTermTransDefNameUnified(char *fld000,char *fld003,char *fld038,char *CfgName)
{
        int                     i;
        int                     ret;

        if ((ret = UppleConnectTermTransDefTBL()) < 0)
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDef:: UppleConnectTermTransDefTBL!\n");
                return(NULL);
        }
        if(0 == strlen(fld038) ){
                for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
                {
                        if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
                                        (strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
                                        (strncmp("UN",pguppleTermTransDefTBL->records[i].routeid,2) == 0)       ) {
                                UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
                                strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
                                return 0;
                        }
                }
        } else{
                for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
                {
                        if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
                                        (strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
                                        (strncmp("UF",pguppleTermTransDefTBL->records[i].routeid,2) == 0)       ) {
                                UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
                                strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
                                return 0;
                        }
                }

        }
        return(0);
}

int UppleFindTermTransDefNameyenAddPort(char *fld000,char *fld003,char *fld038,long port,char *CfgName)
{
        int                     i;
        int                     ret;

        if ((ret = UppleConnectTermTransDefTBL()) < 0)
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleFindTermTransDef:: UppleConnectTermTransDefTBL!\n");
                return(NULL);
        }
        if(0 == strlen(fld038) ){

                if((port == 0)&&((0 == atoi(fld003))&&((200 == atoi(fld000)))))
                {
                        for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
                        {
                                if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
                                                (strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
                                                (strncmp("YN",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
                                        UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
                                                        i,pguppleTermTransDefTBL->records[i].remark);
                                        strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
                                        return 0;
                                }
                        }
                }else if(port == 8801){
                        for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
                        {
                                if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
                                                (strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
                                                (strncmp("YE",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
                                        UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
                                                        i,pguppleTermTransDefTBL->records[i].remark);
                                        strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
                                        return 0;
                                }
                        }
                }else{
                        for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
                        {
                                if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
                                                (strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
                                                (strncmp("YQ",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
                                        UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",\
                                                        i,pguppleTermTransDefTBL->records[i].remark);
                                        strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
                                        return 0;
                                }
                        }
                }
        } else{
                for (i = 0; i < pguppleTermTransDefTBL->maxNumOfRecords; i++)
                {
                        if ((strncmp(fld000,pguppleTermTransDefTBL->records[i].fld000,4) == 0) &&
                                        (strncmp(fld003,pguppleTermTransDefTBL->records[i].fld003,6) == 0) &&
                                        (strncmp("YP",pguppleTermTransDefTBL->records[i].routeid,2) == 0)   ) {
                                UppleLog2(__FILE__,__LINE__,"pguppleTermTransDefTBL->records[%d].remark = [%s]\n",i,pguppleTermTransDefTBL->records[i].remark);
                                strcpy(CfgName,pguppleTermTransDefTBL->records[i].remark);
                                return 0;
                        }
                }

        }
        return(0);
}

