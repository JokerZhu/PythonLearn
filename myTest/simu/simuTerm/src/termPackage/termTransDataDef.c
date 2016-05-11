//	Author:		zhangyongding
//	Date:		20081019
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "uppleLog.h"
#include "uppleModule.h"
#include "uppleEnv.h"
#include "uppleCommand.h"
#include "uppleREC.h"

#include "termTransDataDef.h"
#include "termPackage.h"
#include "termTransFldDefTBL.h"
#include "spPackage.h"
#include "ylPackage.h"
#include "defaultFldSetFun.h"
#include "userFldSetFun.h"
#include "specErrCode.h"
#include "platFormPackage.h"

#include "buffer_pool.h"
#include "uppleModuleVar.h"

extern XBUF *RPUB;

TUppleTermTransDataDef	guppleTermTransDataDef;
int			guppleIsTermTransDataDefConnected = 0;

int UppleGetNameOfTermTransDataConfFile(char *transName,char *fileName)
{
	sprintf(fileName,"%s/termDef/%s.toTerm",getenv("UPPLEETC"),transName);
	return(0);
}

int UppleGetMaxNumOfTermTransDataFld()
{
	return(conMaxNumOfTermTransFld);
}

int UppleIsTermTransDataDefTBLConnected()
{
	return(guppleIsTermTransDataDefConnected);
}

int UppleDisconnectTermTransDataDefTBL()
{
	guppleIsTermTransDataDefConnected = 0;
	return(0);
}

int UppleConnectTermTransDataDefTBL(char *transName)
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i;
	
	if (UppleIsTermTransDataDefTBLConnected())	// �Ѿ�����
		return(0);

	if ((ret = UppleConnectTermTransFldDefTBL()) < 0)
	{
		UppleUserErrLog("in UppleConnectTermTransDataDefTBL:: UppleConnectTermTransFldDefTBL!\n");
		return(ret);
	}
	
	for (i = 0; i < UppleGetMaxNumOfTermTransDataFld(); i++)
		guppleTermTransDataDef.prec[i] = NULL;

	memset(guppleTermTransDataDef.name,0,sizeof(guppleTermTransDataDef.name));
	if (strlen(transName) >= sizeof(guppleTermTransDataDef.name))
		memcpy(guppleTermTransDataDef.name,transName,sizeof(guppleTermTransDataDef.name)-1);
	else
		strcpy(guppleTermTransDataDef.name,transName);
		
	memset(fileName,0,sizeof(fileName));
	UppleGetNameOfTermTransDataConfFile(transName,fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		UppleUserErrLog("in UppleConnectTermTransDataDefTBL:: UppleInitEnvi [%s]!\n",fileName);
		return(ret);
	}
	
	guppleTermTransDataDef.realNum = 0;
		
	for (i = 0; (i < UppleGetEnviVarNum()) && (guppleTermTransDataDef.realNum < UppleGetMaxNumOfTermTransDataFld()); i++)
	{
		// ��ȡ����
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UppleUserErrLog("in UppleConnectTermTransDataDefTBL:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if ((guppleTermTransDataDef.prec[i] = UppleFindTermTransFldDef(p)) == NULL)
		{
			UppleUserErrLog("in UppleConnectTermTransDataDefTBL:: UppleFindTermTransFldDef [%s]\n",p);
			continue;
		}
		guppleTermTransDataDef.realNum += 1;
	}

	UppleClearEnvi();
	
	//UppleLog("in UppleConnectTermTransDataDefTBL:: guppleTermTransDataDef.realNum[%d]\n",guppleTermTransDataDef.realNum);

	guppleIsTermTransDataDefConnected = 1;	
	return(guppleTermTransDataDef.realNum);
}

int UppleSetTermPackageFldAutomatically()
{
	int	ret;
	int	i;
	
	if (!UppleIsTermTransDataDefTBLConnected())
	{
		UppleUserErrLog("in UppleSetTermPackageFldAutomatically:: !UppleIsTermTransDataDefTBLConnected!\n");
		return(errCodeCurrentMDL_8583TransDataDefNotConnected);
	}
	;

	//UppleLog("#TEST:in UppleSetTermPackageFldAutomatically:: set flds for transName = [%s]\n",guppleTermTransDataDef.name);
	for (i = 0; i < guppleTermTransDataDef.realNum; i++)
	{
		if ((ret = UppleSetTermFldAutomatically(guppleTermTransDataDef.prec[i])) < 0)
		{
			UppleUserErrLog("in UppleSetTermPackageFldAutomatically:: UppleSetTermFldAutomatically [%s] [%d]!\n",
				guppleTermTransDataDef.prec[i]->name,guppleTermTransDataDef.prec[i]->index);
			return(ret);
		}
	}
	//UppleLog("#TEST:in UppleSetTermPackageFldAutomatically:: set flds for transName = [%s] OK!\n",guppleTermTransDataDef.name);
	return(0);
}
		
int UppleSetTermFldAutomatically(PUpplePackageAutoFldDef pfldDef)
{
	int	ret;
	int	len;
	char	*ptr;
	char	tmpBuf[1024+1];
	
	if (pfldDef == NULL)
	{
		UppleUserErrLog("in UppleSetTermFldAutomatically:: pfldDef = [%0x]\n",pfldDef);
		return(errCodeCurrentMDL_8583PackageFldDefIsNull);
	}
	//UppleLog("#TEST:in UppleSetTermFldAutomatically:: [%04d] [%s]\n",pfldDef->index,pfldDef->var);
	switch (pfldDef->method)
	{
		case	conFldValueSetMethod_NotAutoSet:	// ���Զ���ֵ�������ɳ���ֵ
			return(0);
		case	conFldValueSetMethod_NullValue:		// ��ֵ
			return(UppleSetTermFldNull(pfldDef->index));
		case	conFldValueSetMethod_DefaultValue:	// ȡȱʡֵ
			return(UppleSetTermFld(pfldDef->index,pfldDef->var,strlen(pfldDef->var)));
		case	conFldValueSetMethod_ReadTermFld:	// ֱ��ȡָ����term�򣬼�����ԭ���򲻱�
			return(0);
		case	conFldValueSetMethod_ReadYLFld:		// ֱ��ȡ�����������ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleReadYLFld(atoi(pfldDef->var),tmpBuf,sizeof(tmpBuf)-1)) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleReadYLFld [%s]\n",pfldDef->var);
				return(len);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case	conFldValueSetMethod_ReadSPFld:		// ֱ��ȡSP�������ֵ
			return(0);
		case	conFldValueSetMethod_ReadRECFld:	// ֱ��ȡREC���ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleReadRECPackageFld(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleReadRECPackageFld [%s]\n",pfldDef->var);
				return(errCodeRECMDL_VarNotExists);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case	conFldValueSetMethod_DefaultFun:	// ʹ��Ԥ����ĺ�����ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleCallDefaultFldSetFun(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleCallDefaultFldSetFun [%s]\n",pfldDef->var);
				return(len);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case	conFldValueSetMethod_UserFun:		// ʹ���û�����ĺ���ֵ��ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleCallUserFldSetFun(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleCallUserFldSetFun [%s]\n",pfldDef->var);
				return(len);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
/*
		case	conFldValueSetMethod_ReadOriPyrFld:	// ʹ��ԭ�����ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleReadOriginPyrTransFld(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleReadOriginPyrTransFld [%s]\n",pfldDef->var);
				return(len);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
*/
		case	conFldValueSetMethod_ReadRPUB:	// ʹ�ý��׹�����ֵ
		    memset( tmpBuf, 0x00, sizeof(tmpBuf) );
		    UppleGet2( RPUB, pfldDef->var,  tmpBuf);
		    //UppleLog2(__FILE__,__LINE__, "#TEST:pfldDef->var[%s]=[%s]\n", pfldDef->var, tmpBuf);
		    len = strlen(tmpBuf);
		    if(len==0)
			return(UppleSetTermFldNull(pfldDef->index));
		    return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			

		default:					// ȱʡ��ֵ
			return(UppleSetTermFldNull(pfldDef->index));
	}
}
int UppleSetTermPackageFldAutomaticallyP()
{
	int	ret;
	int	i;
	
	if (!UppleIsTermTransDataDefTBLConnected())
	{
		UppleUserErrLog("in UppleSetTermPackageFldAutomatically:: !UppleIsTermTransDataDefTBLConnected!\n");
		return(errCodeCurrentMDL_8583TransDataDefNotConnected);
	}
	//UppleLog("in UppleSetTermPackageFldAutomatically:: set flds for transName = [%s]\n",guppleTermTransDataDef.name);
	for (i = 0; i < guppleTermTransDataDef.realNum; i++)
	{
		if ((ret = UppleSetTermFldAutomaticallyP(guppleTermTransDataDef.prec[i])) < 0)
		{
			UppleUserErrLog("in UppleSetTermPackageFldAutomatically:: UppleSetTermFldAutomatically [%s] [%d]!\n",
				guppleTermTransDataDef.prec[i]->name,guppleTermTransDataDef.prec[i]->index);
			return(ret);
		}
	}
	//UppleLog("in UppleSetTermPackageFldAutomatically:: set flds for transName = [%s] OK!\n",guppleTermTransDataDef.name);
	return(0);
}
		
int UppleSetTermFldAutomaticallyP(PUpplePackageAutoFldDef pfldDef)
{
	int	ret;
	int	len;
	char	*ptr;
	char	tmpBuf[1024+1];
	
	if (pfldDef == NULL)
	{
		UppleUserErrLog("in UppleSetTermFldAutomatically:: pfldDef = [%0x]\n",pfldDef);
		return(errCodeCurrentMDL_8583PackageFldDefIsNull);
	}
//	UppleLog2(__FILE__,__LINE__,"in UppleSetTermFldAutomatically:: [%04d] [%s]\n",pfldDef->index,pfldDef->var);
	switch (pfldDef->method)
	{
		case	conFldValueSetMethod_NotAutoSet:	// ���Զ���ֵ�������ɳ���ֵ
			return(0);
		case	conFldValueSetMethod_NullValue:		// ��ֵ
			return(UppleSetTermFldNull(pfldDef->index));
		case	conFldValueSetMethod_DefaultValue:	// ȡȱʡֵ
			return(UppleSetTermFld(pfldDef->index,pfldDef->var,strlen(pfldDef->var)));
		case	conFldValueSetMethod_ReadTermFld:	// ֱ��ȡָ����term�򣬼�����ԭ���򲻱�
			return(0);
		case	conFldValueSetMethod_ReadYLFld:		// ֱ��ȡ�����������ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleReadYLFldP(atoi(pfldDef->var),tmpBuf,sizeof(tmpBuf)-1)) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleReadYLFld [%s]\n",pfldDef->var);
				return(len);
			}
		//	UppleLog2(__FILE__,__LINE__,"in UppleSetTermFldAutomatically:: [%04d] [%s] [%s]\n",pfldDef->index,pfldDef->var,tmpBuf);
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case	conFldValueSetMethod_ReadSPFld:		// ֱ��ȡSP�������ֵ
			return(0);
		case	conFldValueSetMethod_ReadRECFld:	// ֱ��ȡREC���ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleReadRECPackageFld(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleReadRECPackageFld [%s]\n",pfldDef->var);
				return(errCodeRECMDL_VarNotExists);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case	conFldValueSetMethod_DefaultFun:	// ʹ��Ԥ����ĺ�����ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleCallDefaultFldSetFun(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleCallDefaultFldSetFun [%s]\n",pfldDef->var);
				return(len);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case	conFldValueSetMethod_UserFun:		// ʹ���û�����ĺ���ֵ��ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleCallUserFldSetFun(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleCallUserFldSetFun [%s]\n",pfldDef->var);
				return(len);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
/*
		case	conFldValueSetMethod_ReadOriPyrFld:	// ʹ��ԭ�����ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleReadOriginPyrTransFld(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleReadOriginPyrTransFld [%s]\n",pfldDef->var);
				return(len);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
*/
		case	conFldValueSetMethod_ReadRPUB:	// ʹ�ý��׹�����ֵ
		    memset( tmpBuf, 0x00, sizeof(tmpBuf) );
		    UppleGet2( RPUB, pfldDef->var,  tmpBuf);
		    //UppleLog2(__FILE__,__LINE__, "#TEST:pfldDef->var[%s]=[%s]\n", pfldDef->var, tmpBuf);
		    len = strlen(tmpBuf);
		    if(len==0)
			return(UppleSetTermFldNull(pfldDef->index));
		    return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			

		default:					// ȱʡ��ֵ
		return(UppleSetTermFldNull(pfldDef->index));
	}
}


int UppleSetTermPackageFldAutomaticallyWhenPlatFormError()
{
	int	ret;
	int	i;
	
	if (!UppleIsTermTransDataDefTBLConnected())
	{
		UppleUserErrLog("in UppleSetTermPackageFldAutomaticallyWhenPlatFormError:: !UppleIsTermTransDataDefTBLConnected!\n");
		return(errCodeCurrentMDL_8583TransDataDefNotConnected);
	}
	UppleLog2(__FILE__,__LINE__, "in UppleSetTermPackageFldAutomaticallyWhenPlatFormError:: set flds for transName = [%s]\n",guppleTermTransDataDef.name);
	for (i = 0; i < guppleTermTransDataDef.realNum; i++)
	{
		if ((ret = UppleSetTermFldAutomaticallyWhenPlatFormError(guppleTermTransDataDef.prec[i])) < 0)
		{
			UppleUserErrLog("in UppleSetTermPackageFldAutomaticallyWhenPlatFormError:: UppleSetTermFldAutomaticallyWhenPlatFormError [%s] [%d]!\n",
				guppleTermTransDataDef.prec[i]->name,guppleTermTransDataDef.prec[i]->index);
			return(ret);
		}
	}
	//UppleLog("in UppleSetTermPackageFldAutomaticallyWhenPlatFormError:: set flds for transName = [%s] OK!\n",guppleTermTransDataDef.name);
	return(0);
}
		
int UppleSetTermFldAutomaticallyWhenPlatFormError(PUpplePackageAutoFldDef pfldDef)
{
	int	ret;
	int	len;
	char	*ptr;
	char	tmpBuf[256];
	
	if (pfldDef == NULL)
	{
		UppleUserErrLog("in UppleSetTermFldAutomaticallyWhenPlatFormError:: pfldDef = [%0x]\n",pfldDef);
		return(errCodeCurrentMDL_8583PackageFldDefIsNull);
	}
	//UppleLog("in UppleSetTermFldAutomaticallyWhenPlatFormError:: [%04d] [%s]\n",pfldDef->index,pfldDef->var);
	switch (pfldDef->method)
	{
		case	conFldValueSetMethod_NotAutoSet:	// ���Զ���ֵ�������ɳ���ֵ
			return(0);
		case	conFldValueSetMethod_NullValue:		// ��ֵ
			return(UppleSetTermFldNull(pfldDef->index));
		case	conFldValueSetMethod_DefaultValue:	// ȡȱʡֵ
			return(UppleSetTermFld(pfldDef->index,pfldDef->var,strlen(pfldDef->var)));
		case	conFldValueSetMethod_ReadTermFld:	// ֱ��ȡָ����term�򣬼�����ԭ���򲻱�
			return(0);
		case	conFldValueSetMethod_ReadYLFld:		// ֱ��ȡ�����������ֵ
			return(0);
		case	conFldValueSetMethod_ReadSPFld:		// ֱ��ȡSP�������ֵ
			return(0);
		case	conFldValueSetMethod_ReadRECFld:	// ֱ��ȡREC���ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleReadRECPackageFld(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomaticallyWhenPlatFormError:: UppleReadRECPackageFld [%s]\n",pfldDef->var);
				return(errCodeRECMDL_VarNotExists);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case	conFldValueSetMethod_DefaultFun:	// ʹ��Ԥ����ĺ�����ֵ
			return(0);
		case	conFldValueSetMethod_UserFun:		// ʹ���û�����ĺ���ֵ��ֵ
			return(0);
		default:					// ȱʡ��ֵ
			return(UppleSetTermFldNull(pfldDef->index));
	}
}
/*
*	CUPS �����ն�����
*	20131021 ADD BY ZJW
*/


int UppleSetTermPackageFldAutomaticallyCUPS()
{
	int	ret;
	int	i;
	
	if (!UppleIsTermTransDataDefTBLConnected())
	{
		UppleUserErrLog("in UppleSetTermPackageFldAutomatically:: !UppleIsTermTransDataDefTBLConnected!\n");
		return(errCodeCurrentMDL_8583TransDataDefNotConnected);
	}
	UppleLog("in UppleSetTermPackageFldAutomaticallyCUPS:: set flds for transName = [%s]\n",guppleTermTransDataDef.name);
	for (i = 0; i < guppleTermTransDataDef.realNum; i++)
	{
		if ((ret = UppleSetTermFldAutomaticallyCUPS(guppleTermTransDataDef.prec[i])) < 0)
		{
			UppleUserErrLog("in UppleSetTermPackageFldAutomatically:: UppleSetTermFldAutomatically [%s] [%d]!\n",
				guppleTermTransDataDef.prec[i]->name,guppleTermTransDataDef.prec[i]->index);
			return(ret);
		}
	}
	UppleLog("in UppleSetTermPackageFldAutomaticallyCUPS:: set flds for transName = [%s] OK!\n",guppleTermTransDataDef.name);
	return(0);
}
int UppleSetTermFldAutomaticallyCUPS(PUpplePackageAutoFldDef pfldDef)
{
	int	ret;
	int	len;
	char	*ptr;
	char	tmpBuf[1024+1];
	
	if (pfldDef == NULL)
	{
		UppleUserErrLog("in UppleSetTermFldAutomatically:: pfldDef = [%0x]\n",pfldDef);
		return(errCodeCurrentMDL_8583PackageFldDefIsNull);
	}
	//UppleLog2(__FILE__,__LINE__,"in UppleSetTermFldAutomatically:: [%04d] [%s]\n",pfldDef->index,pfldDef->var);
	switch (pfldDef->method)
	{
		case	conFldValueSetMethod_NotAutoSet:	// ���Զ���ֵ�������ɳ���ֵ
			return(0);
		case	conFldValueSetMethod_NullValue:		// ��ֵ
			return(UppleSetTermFldNull(pfldDef->index));
		case	conFldValueSetMethod_DefaultValue:	// ȡȱʡֵ
			return(UppleSetTermFld(pfldDef->index,pfldDef->var,strlen(pfldDef->var)));
		case	conFldValueSetMethod_ReadTermFld:	// ֱ��ȡָ����term�򣬼�����ԭ���򲻱�
			return(0);
		case	conFldValueSetMethod_ReadYLFld:		// ֱ��ȡ�����������ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleReadYLFldCUPS(atoi(pfldDef->var),tmpBuf,sizeof(tmpBuf)-1)) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleReadYLFld [%s]\n",pfldDef->var);
				return(len);
			}
			//UppleLog2(__FILE__,__LINE__,"in UppleSetTermFldAutomatically:: [%04d] [%s] [%s]\n",pfldDef->index,pfldDef->var,tmpBuf);
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case	conFldValueSetMethod_ReadSPFld:		// ֱ��ȡSP�������ֵ
			return(0);
		case	conFldValueSetMethod_ReadRECFld:	// ֱ��ȡREC���ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleReadRECPackageFld(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleReadRECPackageFld [%s]\n",pfldDef->var);
				return(errCodeRECMDL_VarNotExists);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case	conFldValueSetMethod_DefaultFun:	// ʹ��Ԥ����ĺ�����ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleCallDefaultFldSetFun(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleCallDefaultFldSetFun [%s]\n",pfldDef->var);
				return(len);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case	conFldValueSetMethod_UserFun:		// ʹ���û�����ĺ���ֵ��ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleCallUserFldSetFun(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleCallUserFldSetFun [%s]\n",pfldDef->var);
				return(len);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
/*
		case	conFldValueSetMethod_ReadOriPyrFld:	// ʹ��ԭ�����ֵ
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((len = UppleReadOriginPyrTransFld(pfldDef->var,tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UppleUserErrLog("in UppleSetTermFldAutomatically:: UppleReadOriginPyrTransFld [%s]\n",pfldDef->var);
				return(len);
			}
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
*/
		case	conFldValueSetMethod_ReadRPUB:	// ʹ�ý��׹�����ֵ
		    memset( tmpBuf, 0x00, sizeof(tmpBuf) );
		    UppleGet2( RPUB, pfldDef->var,  tmpBuf);
		    UppleLog2(__FILE__,__LINE__, "#TEST:pfldDef->var[%s]=[%s]\n", pfldDef->var, tmpBuf);
		    len = strlen(tmpBuf);
		    if(len==0)
			return(UppleSetTermFldNull(pfldDef->index));
		    return(UppleSetTermFld(pfldDef->index,tmpBuf,len));			
		case    conFldValueSetMethod_ReadRPBite:        // ʹ�ý��׹�����ֵ
			memset( tmpBuf, 0x00, sizeof(tmpBuf) );
			UppleGet2B( RPUB, pfldDef->var,  tmpBuf,&len);
		//	UppleMemLog2(__FILE__,__LINE__,"############term bite:\n",tmpBuf,len);
			if(len==0)
				return(UppleSetTermFldNull(pfldDef->index));
			return(UppleSetTermFld(pfldDef->index,tmpBuf,len));

		default:					// ȱʡ��ֵ
		return(UppleSetTermFldNull(pfldDef->index));
	}
}
