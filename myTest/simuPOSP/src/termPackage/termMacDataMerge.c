//	Author:		zhangyongding
//	Date:		20081025
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "uppleLog.h"
#include "uppleStr.h"

#include "macFldTBLDef.h"
#include "upplePackage.h"
#include "termMacDataMerge.h"

#include "specErrCode.h"

int UppleTermMergeMacData(PUpplePackage ppackage,PUppleMacFldTBLDef pmacFldTBLDef,char *macData,int sizeOfBuf)
{
	int	ret;
	int	i,j;
	char	tmpBuf[1024];
	char	tmpMacData[8192+1];
	int	len,totalLen = 0;
	char	*p;
	int	firstFld = 1;
	int	fldIndex;
	
	if ((!UppleIsValidPackage(ppackage)) || (pmacFldTBLDef ==  NULL) || (tmpMacData == NULL))
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTermMergeMacData:: ppackage=[%x]pmacFldTBLDef=[%x]tmpMacData=[%x]\n",ppackage,pmacFldTBLDef,tmpMacData);
		return(errCodeParameter);
	}
	
	//UppleLogPackage(ppackage);
	
	//UppleLog("in UppleTermMergeMacData:: %d %d\n",pmacFldTBLDef->dataMergeMethod,conMacDataMergeMethodOfChinaPay);
	memset(tmpMacData,0,sizeof(tmpMacData));
	for (i = 0; i < pmacFldTBLDef->maxNum; i++)
	{
		//UppleNullLog("totalLen = [%d] buf = [%s]\n",totalLen,tmpMacData);
		if (!UppleIsValidPackageFldIndex(ppackage->pdef,fldIndex=pmacFldTBLDef->fldTBL[i]))
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleTermMergeMacData:: !UppleIsValidPackageFldIndex [%d]\n",fldIndex);
			return(errCodeCurrentMDL_InvalidMacFldIndex);
		}		
		//UppleNullLog("read mac fld [%04d]\n",fldIndex);
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UppleReadPackageFld(ppackage,fldIndex,tmpBuf,sizeof(tmpBuf)-1)) < 0)
		{
			//UppleNullLog("in UppleTermMergeMacData:: UppleReadPackageFld [%d]\n",fldIndex);
			continue;
		}
		//UppleNullLog("fld [%04d] len = [%d] buf = [%s]\n",fldIndex,len,tmpBuf);
		if (len == 0)
			continue;
		switch (pmacFldTBLDef->dataMergeMethod)
		{
			case	conMacDataMergeMethodOfChinaPay:				
				if (!firstFld)
				{
					memcpy(tmpMacData+totalLen," ",1);
					totalLen++;
				}
				else
					firstFld = 0;
				break;
			default:
				break;
		}
		if (ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag > 0)
		{
			if (totalLen + ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag > sizeof(tmpMacData))
			{
				UppleUserErrLog2(__FILE__,__LINE__,"in UppleTermMergeMacData:: sizeof(tmpMacData) [%d] smaller than expected [%d]!\n",
					sizeof(tmpMacData),totalLen + ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag);
				return(errCodeSmallBuffer);
			}
			UppleConvertIntIntoStr(len,ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag,tmpMacData+totalLen);
			totalLen += ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag;
		}

		switch (fldIndex)
		{
			case	1:	// 二磁道

			{ 	char tmpBuf2[ 64 ];

				memset( tmpBuf2, 0x00, sizeof(tmpBuf2) );
				memcpy( tmpBuf2, tmpBuf, len );

				memset( tmpBuf, 0x00, sizeof(tmpBuf) );
				bcdhex_to_aschex( tmpBuf2, len, tmpBuf);
				len = len * 2;
				UppleUserErrLog2(__FILE__,__LINE__,"in UppleTermMergeMacData:: len=[%d],bitmap=[%s].\n", len, tmpBuf );
			}
				break;

			case	35:	// 二磁道
				/*
				if (len < 18)
					memset(tmpBuf+len,' ',18-len);
				len = 18;
				for (j = 0; j < len; j++)
					if (tmpBuf[j] == 'D')
						tmpBuf[j] = '=';
				*/
				break;
			case	36:	// 三磁道
				/*
				if (len < 17)
					memset(tmpBuf+len,' ',17-len);
				len = 17;
				for (j = 0; j < len; j++)
					if (tmpBuf[j] == 'D')
						tmpBuf[j] = '=';
				*/
				break;
			default:
				break;
		}

		if (len + totalLen > sizeof(tmpMacData))
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleTermMergeMacData:: sizeof(tmpMacData) [%d] smaller than expected [%d]!\n",
				sizeof(tmpMacData),len+totalLen);
			return(errCodeSmallBuffer);
		}
		memcpy(tmpMacData+totalLen,tmpBuf,len);
		totalLen += len;
	}
	UppleLog("in UppleTermMergeMacData:: macDataLen = [%d] tmpMacData, = [%s]\n",totalLen,tmpMacData);
	memset(macData,0,sizeOfBuf);
	if (pmacFldTBLDef->dataMergeMethod == conMacDataMergeMethodOfChinaPay)
	{
		totalLen = UppleProduceTermMacData(tmpMacData,totalLen,macData,sizeOfBuf);
	}
	else
	{
		if (totalLen >= sizeOfBuf)
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleTermMergeMacData:: sizeOfBuf [%d] smaller than expected [%d]\n",sizeOfBuf,totalLen);
			return(errCodeSmallBuffer);
		}
		memcpy(macData,tmpMacData,totalLen);
	}
	UppleDebugLog("in UppleTermMergeMacData:: macDataLen = [%d] macData = [%s]\n",totalLen,macData);
	return(totalLen);
}

int UppleProduceTermMacData(char *tmpMacData,int lenOfTmpMacData,char *macData,int sizeOfBuf)
{
	int	i;
	int	j;
	int	FirstBlank = 1;

	macData[0] = 0;
	for (i = 0,j = 0,FirstBlank=1; (i < lenOfTmpMacData) && (j < sizeOfBuf - 1);i++)
	{
		if (tmpMacData[i] == ' ')
		{
			if (j == 0)
				continue;
			if (FirstBlank)
			{
				FirstBlank = 0;
				macData[j] = tmpMacData[i];
				j++;
				continue;
			}
			else
				continue;
		}
		if (((tmpMacData[i] >= 'A') && (tmpMacData[i] <= 'Z')) ||
			((tmpMacData[i] >= '0') && (tmpMacData[i] <= '9')) ||
			(tmpMacData[i] == ',') ||
			(tmpMacData[i] == '.') )
		{
			FirstBlank = 1;
			macData[j] = tmpMacData[i];
			j++;
			continue;
		}
		if ((tmpMacData[i] >= 'a') && (tmpMacData[i] <= 'z'))
		{
			FirstBlank = 1;
			macData[j] = tmpMacData[i] - 'a' + 'A';
			j++;
			continue;
		}
	}
	if (i < lenOfTmpMacData)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleProduceTermMacData:: sizeOfBuf [%d] smaller than expected [%d]\n",sizeOfBuf,lenOfTmpMacData);
		return(errCodeSmallBuffer);
	}
	if (macData[j-1] == ' ')
		j--;
	macData[j] = 0;
	return(j);	// Length of macData
}
