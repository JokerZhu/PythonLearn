// 2006/6/23,增加了长度指示域相关的定义
// 即增加了 _bitFldLenFlag_ 宏定义相关的内容

// 2006/6/23增加_bitFldLenFlag_宏定义
#ifndef _bitFldLenFlag_
#define _bitFldLenFlag_
#endif

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "upplePackage.h"
#include "uppleStr.h"

//#include "buffer_pool.h"
//extern XBUF *RPUB;

PUpplePackage UppleConnectPackage(char *nameOfPackageDef)
{
	PUpplePackage		ppackage = NULL;
	int			ret;
	PUpplePackageDef	pdef;
	
	if ((pdef = UppleFindPackageDef(nameOfPackageDef)) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackage:: UppleFindPackageDef [%s]!\n",nameOfPackageDef);
		return(NULL);
	}


	//if ((ppackage = (PUpplePackage)malloc(sizeof(*ppackage))) == NULL)
	if ((ppackage = (PUpplePackage)malloc(sizeof(TUpplePackage))) == NULL)
	{
		printf("in UppleConnectPackage:: malloc!\n");
		return(NULL);
	}
	
	memset( ppackage, 0x00, sizeof(TUpplePackage) );  //Add by ZZH, 2011-11-28

	ppackage->connected = 0;
	ppackage->dynamicPackage = 1;
	ppackage->pdef = pdef;

	if ((ret = UppleInitPackage(ppackage)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackage:: UppleInitPackage [%s]!\n",nameOfPackageDef);
		goto errExit;
	}

	ppackage->connected = 1;


	return(ppackage);
errExit:
	UppleDisconnectPackage(ppackage);
	return(NULL);
}
PUpplePackage UppleConnectPackageNew(char *PathOfPackageDef, char *nameOfPackageDef)
{
	PUpplePackage		ppackage = NULL;
	int			ret;
	PUpplePackageDef	pdef;
	
	if ((pdef = UppleFindPackageDefNew(PathOfPackageDef,nameOfPackageDef)) == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleConnectPackage:: UppleFindPackageDef [%s]!\n",nameOfPackageDef);
		return(NULL);
	}

	printf("test [%s] [%d]\n",__FILE__,__LINE__);
	//if ((ppackage = (PUpplePackage)malloc(sizeof(*ppackage))) == NULL)
	if ((ppackage = (PUpplePackage)malloc(sizeof(TUpplePackage))) == NULL)
	{
		printf("in UppleConnectPackage:: malloc!\n");
		return(NULL);
	}
	
	memset( ppackage, 0x00, sizeof(TUpplePackage) );

	ppackage->connected = 0;
	ppackage->dynamicPackage = 1;
	ppackage->pdef = pdef;

	if ((ret = UppleInitPackage(ppackage)) < 0)
	{
		printf("in UppleConnectPackage:: UppleInitPackage [%s] [%s] [%d]!\n",nameOfPackageDef,__FILE__,__LINE__);
		goto errExit;
	}

	ppackage->connected = 1;


	return(ppackage);
errExit:
	UppleDisconnectPackage(ppackage);
	return(NULL);
}

int UppleDisconnectPackage(PUpplePackage ppackage)
{
	if (ppackage == NULL)
		return(0);
	UppleInitPackage(ppackage);
	if (ppackage->pdef != NULL)
	{
		UppleDisconnectPackageDef(ppackage->pdef->name);
		ppackage->pdef = NULL;
	}
	if (ppackage->dynamicPackage)
		free(ppackage);
	ppackage = NULL;
	return(0);
}

int UppleIsValidPackage(PUpplePackage ppackage)
{
	if ((ppackage == NULL) || (ppackage->pdef == NULL) || (!ppackage->connected))
		return(0);
	else
		return(1);
}

int UppleInitPackage(PUpplePackage ppackage)
{
	int	i;
	
	if (ppackage == NULL)
	{
		printf(__FILE__,__LINE__,"in UppleInitPackage:: wrong parameter!\n");
		return(errCodeParameter);
	}

	for (i = 0; i < conMaxNumOfPackageFlds; i++)
	{
		ppackage->flds[i].index = i;
		ppackage->flds[i].len = 0;
		if (ppackage->connected)
		{
			if (ppackage->flds[i].value)
				free(ppackage->flds[i].value);
		}	
		ppackage->flds[i].value = NULL;
	}

	return(0);
}

int UppleLogPackageFld(PUpplePackageFldDef pfldDef,PUpplePackageFld pfld)
{
	char		*ptr = NULL;
	char		nullPtr[] = "null pointer";
	char		tmpBuf[512+1];
	
	if ((pfld == NULL) || (pfldDef == NULL))
	{
		printf(__FILE__,__LINE__,"in UppleLogPackageFldToFile:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if (pfld->len <= 0)
		return(errCodePackageDefMDL_PackFldLength);
		
	if ((ptr = (char *)(pfld->value)) == NULL)
		ptr = nullPtr;
	if (pfldDef->valueType % 100 == conBitMapFld)	// 是位图
	{
		if (pfld->len * 2 > sizeof(tmpBuf))
		{
			printf(__FILE__,__LINE__,"in UppleLogPackageFldToFile:: bit map too long!\n");
			return(errCodeParameter);
		}
		bcdhex_to_aschex(ptr,pfld->len,tmpBuf);
		tmpBuf[pfld->len*2] = 0;
		ptr = tmpBuf;
	}
	printf("[%04d] [%04d] [%s]\n",pfld->index,pfld->len,ptr);
	return(0);
}
					
int UppleLogPackage(PUpplePackage ppackage)
{
	int		i;
	
	if (!UppleIsValidPackage(ppackage))
	{
		printf(__FILE__,__LINE__,"in UppleLogPackageFldToFile:: wrong parameter!\n");
		return(errCodeParameter);
	}
	
	for (i = 0; i < ppackage->pdef->maxNum; i++)
	{
		UppleLogPackageFld(&(ppackage->pdef->fldDefTBL[i]),&(ppackage->flds[i]));
	}
	printf("\n");

	return(0);
}

/*
int UppleSerializePackagePackageFld(PUpplePackage ppackage, PUpplePackageFldDef pfldDef,PUpplePackageFld pfld)
{
	char		*ptr = NULL;
	char		nullPtr[] = "null pointer";
	char		tmpBuf[512+1];
	
	if ((pfld == NULL) || (pfldDef == NULL))
	{
		printf(__FILE__,__LINE__,"in UppleLogPackageFldToFile:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if (pfld->len <= 0)
		return(errCodePackageDefMDL_PackFldLength);
		
	if ((ptr = (char *)(pfld->value)) == NULL)
		ptr = nullPtr;
	if (pfldDef->valueType % 100 == conBitMapFld)	// 是位图
	{
		if (pfld->len * 2 > sizeof(tmpBuf))
		{
			printf(__FILE__,__LINE__,"in UppleLogPackageFldToFile:: bit map too long!\n");
			return(errCodeParameter);
		}
		bcdhex_to_aschex(ptr,pfld->len,tmpBuf);
		tmpBuf[pfld->len*2] = 0;
		ptr = tmpBuf;
	}
	ppackage.sbuf.index = pfld->index;
	ppackage.sbuf.len = pfld->len;
	memcpy( ppackage.sbuf.value+ppackage.sbuf.offset, ptr, ppackage.sbuf.len );
	ppackage.sbuf.offset += pfld->len;

	printf("##Serialize##,[%04d][%04d][%4d][%s]\n",ppackage.sbuf.index,ppackage.sbuf.offset,ppackage.sbuf.offset, ptr );

	return(0);
}

int UppleSerializePackage(PUpplePackage ppackage)
{
	int		i;
	
	if (!UppleIsValidPackage(ppackage))
	{
		printf(__FILE__,__LINE__,"Error in UppleLogPackageFldToFile:: wrong parameter!\n");
		return(errCodeParameter);
	}
	
	for (i = 0; i < ppackage->pdef->maxNum; i++)
	{
		UppleSerializePackagePackageFld(ppackage, &(ppackage->pdef->fldDefTBL[i]),&(ppackage->flds[i]));
	}
	printf("\n");

	return(0);
}
*/

int UppleSetPackageBitMapFld(PUpplePackage ppackage,int fldIndex)
{
	unsigned char	*p;
	unsigned char	g_caBIT[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	int		i,j,k;
	char		tmpBuf[100];
	int		lastExistFldIndex;
		
	if (!UppleIsValidPackage(ppackage))
	{
		printf(__FILE__,__LINE__,"in UppleSetPackageBitMapFld:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if (!UppleIsValidPackageFldIndex(ppackage->pdef,fldIndex))
	{
		printf(__FILE__,__LINE__,"in UppleSetPackageBitMapFld:: UppleIsValidPackageFldIndex [%d] for [%s]!\n",fldIndex,ppackage->pdef->name);
		return(errCodePackageDefMDL_InvalidFldIndex);
	}
	if (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100 != conBitMapFld)
	{
		printf(__FILE__,__LINE__,"in UppleSetPackageBitMapFld:: the fld [%d] is not a bit map field!\n",fldIndex);
		return(errCodePackageDefMDL_NotBitMapFld);
	}
	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfValue <= 0)
	{
		printf(__FILE__,__LINE__,"in UppleSetPackageBitMapFld:: the fld [%d] not defined!\n",fldIndex);
		return(errCodePackageDefMDL_FldNotDefined);
	}
	if (ppackage->flds[fldIndex].value)
	{
		free(ppackage->flds[fldIndex].value);
		ppackage->flds[fldIndex].value = NULL;
	}
	ppackage->flds[fldIndex].len = ppackage->pdef->fldDefTBL[fldIndex].lenOfValue;
	if ((ppackage->flds[fldIndex].value = (unsigned char *)malloc(ppackage->flds[fldIndex].len+1)) == NULL)
	{
		printf("in UppleSetPackageBitMapFld:: malloc [%d]!\n",ppackage->flds[i].len);
		return(errCodeUseOSErrCode);
	}
	memset(ppackage->flds[fldIndex].value,0,ppackage->flds[fldIndex].len+1);
	//printf("in UppleSetPackageBitMapFld:: fldIndex = [%d] len = [%d] maxFlds = [%d]\n",
	//	fldIndex,ppackage->flds[fldIndex].len,ppackage->pdef->maxNum);
	lastExistFldIndex = fldIndex;
	for (i = fldIndex; i < ppackage->pdef->maxNum; i++)
	{
		if (i - fldIndex >= 8 * ppackage->flds[fldIndex].len)	// 此后的域不在该位图中反映
		{
			//printf("in UppleSetPackageBitMapFld:: [%d] should not defined in bitmaps!\n",i);
			break;
		}
		if (ppackage->pdef->fldDefTBL[i].lenOfValue <= 0)	// 此域没定义
		{
			//printf("in UppleSetPackageBitMapFld:: [%d] not defined!\n",i);
			continue;
		}
		if (ppackage->flds[i].len <= 0)	// 此域无值
		{
			//printf("in UppleSetPackageBitMapFld:: [%d] have no value!\n",i);
			continue;
		}
		//printf("in UppleSetPackageBitMapFld:: [%d] have a value!\n",i);
		p = ppackage->flds[fldIndex].value + (i - fldIndex) / 8;
		p[0] = p[0] | g_caBIT[(i - fldIndex) % 8];
		lastExistFldIndex = i;
	}
	if ((ppackage->flds[fldIndex].len = ((lastExistFldIndex-fldIndex+64) / 64) * 8) > ppackage->pdef->fldDefTBL[fldIndex].lenOfValue)
		ppackage->flds[fldIndex].len = ppackage->pdef->fldDefTBL[fldIndex].lenOfValue;
	p = ppackage->flds[fldIndex].value;
	for (i = 0; i < ppackage->flds[fldIndex].len - 8; i = i + 8)
		p[i] = p[i] | 0x80;
	if (ppackage->flds[fldIndex].len - 8 >= 0)
		p[ppackage->flds[fldIndex].len - 8] = p[ppackage->flds[fldIndex].len - 8] & 0x7F;
	memset(tmpBuf,0,sizeof(tmpBuf));
	bcdhex_to_aschex((char *)ppackage->flds[fldIndex].value,ppackage->flds[fldIndex].len,tmpBuf);
	printf("in UppleSetPackageBitMapFld:: bitMaps len = [%d] buf = [%s]\n",ppackage->flds[fldIndex].len,tmpBuf);
	return(0);
}

int UpplePackP(PUpplePackage ppackage,unsigned char *buf,int sizeOfBuf)
{
	ppackage->pdef->fldLenPresentation=0;
	return UpplePack(ppackage,buf,sizeOfBuf);
}
int UpplePackCUPS(PUpplePackage ppackage,unsigned char *buf,int sizeOfBuf)
{
	//ppackage->pdef->fldLenPresentation=0;
	printf(__FILE__,__LINE__,"in UpplePackCUPS\n");
	return UpplePack(ppackage,buf,sizeOfBuf);
}
int UpplePack(PUpplePackage ppackage,unsigned char *buf,int sizeOfBuf)
{
	int		i;
	int		len;	// The return value, Length of 8583 package
	unsigned char	*p;		// Work pointer, point to 8583 package
	char		tmpBuf[256+1];
	char		*ptr;
	int		bitMapFldIndex;
	int		ret;
	int		lenBuf[100];;
		
	if (!UppleIsValidPackage(ppackage) || (buf == NULL) || (sizeOfBuf <= 0))
	{
		printf(__FILE__,__LINE__,"in UpplePack:: wrong parameter!\n");
		return(errCodeParameter);
	}

	printf("Before pack:: %s max field [%d] lenflag[%d]\n",ppackage->pdef->name,ppackage->pdef->maxNum,ppackage->pdef->fldLenPresentation);
	UppleLogPackage(ppackage);

	i = 0;
	len = 0;
	p = buf;
pack:
	// 压缩无位图的各个域	
	
	for (; i < ppackage->pdef->maxNum; i++)
	{
		if ((ret = UpplePutPackageFldIntoStr(0,ppackage,i,p,sizeOfBuf-len)) < 0)
		{
			if (ret == errCodePackageDefMDL_IsBitMapFld)	// 这个域是位图域
			{
				bitMapFldIndex = i;
				break;
			}
			printf(__FILE__,__LINE__,"in UpplePack:: UpplePutPackageFldIntoStr fldIndex = [%d] ret = [%d]\n",i,ret);
			return(ret);
		}
		len += ret;
		p += ret;
		if (ret == 0)
			continue;
		//printf(__FILE__,__LINE__,"fldIndex = [%d] fldLen in Str = [%d]\n",i,ret);
		//UppleProgramerMemLog("in UpplePack::",buf,len);
	}
	if (i == ppackage->pdef->maxNum)	// 包已打完
	{	
		printf("Pack OK![%s] [%d] \n",__FILE__,__LINE__);
		//UppleProgramerMemLog("in UpplePack::",buf,len);
		return(len);
	}

	// 包未打完，而是遇到了一个位图域
packBitMapFields:
	if ((ret = UppleSetPackageBitMapFld(ppackage,bitMapFldIndex)) < 0)
	{
		printf(__FILE__,__LINE__,"in UpplePack:: UppleSetPackageBitMapFld [%d]\n",bitMapFldIndex);
		return(ret);
	}
	// 压缩有位图的域
	for (; (i < ppackage->pdef->maxNum) && (i - bitMapFldIndex < ppackage->flds[bitMapFldIndex].len * 8); i++)
	{
		if ((ret = UpplePutPackageFldIntoStr(1,ppackage,i,p,sizeOfBuf-len)) < 0)
		{
			printf(__FILE__,__LINE__,"in UpplePack:: UpplePutPackageFldIntoStr fldIndex = [%d] ret = [%d]\n",i,ret);
			return(ret);
		}
		len += ret;
		p += ret;
		if (ret == 0)
			continue;
		//printf(__FILE__,__LINE__,"fldIndex = [%d] fldLen in Str = [%d]\n",i,ret);
		//UppleProgramerMemLog("in UpplePack::",buf,len);
	}
	goto pack;	
}

int UppleExistPackageFld(TUpplePackageFld *pfldMap,int fldIndex)
{
	unsigned char	*p;
	unsigned char	g_caBIT[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

	if ((pfldMap == NULL) || (pfldMap->len <= 0) || (pfldMap->value == NULL))
	{
		printf(__FILE__,__LINE__,"in UppleExistPackageFld:: wrong parameter!\n");
		return(0);
	}
	if ((fldIndex < pfldMap->index) || (fldIndex >= pfldMap->index + pfldMap->len * 8))
	{
		printf(__FILE__,__LINE__,"in UppleExistPackageFld:: fldIndex [%d] not in this BitMaps [%d] defined!\n",fldIndex,pfldMap->index);
		return(errCodePackageDefMDL_FldNotDefinedInBitMap);
	}

	p = pfldMap->value + ((fldIndex - pfldMap->index) / 8);
	if (p[0] & g_caBIT[(fldIndex - pfldMap->index) % 8 ])
		return(1);
	else
		return(0);
}

int UppleGetPackageBitMapFldRealLength(PUpplePackageFldDef pfldDef,unsigned char *data)
{
	int		len;
	int		i;
		
	if ((pfldDef == NULL) || (data == NULL))
	{
		printf(__FILE__,__LINE__,"in UppleGetPackageBitMapFldRealLength:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if (pfldDef->valueType % 100 != conBitMapFld)
	{
		printf(__FILE__,__LINE__,"in UppleGetPackageBitMapFldRealLength:: the fld is not a bit map field!\n");
		return(errCodePackageDefMDL_NotBitMapFld);
	}
	if (pfldDef->lenOfValue <= 0)
	{
		printf(__FILE__,__LINE__,"in UppleGetPackageBitMapFldRealLength:: the fld not defined!\n");
		return(errCodePackageDefMDL_FldNotDefined);
	}
	for (i = 0,len = 8; i < pfldDef->lenOfValue; i = i + 8)
	{
		if (data[i] & 0x80)
			len += 8;
		else
			break;
	}
	if (len > pfldDef->lenOfValue)
		len = pfldDef->lenOfValue;
	printf("in UppleGetPackageBitMapFldRealLength:: bitMapFldRealLength = [%d]\n",len);
	return(len);
}

int UppleUnpack(PUpplePackage ppackage,unsigned char *buf,int len)
{
	int		i,j;
	unsigned char	*p;
	char		tmpBuf[128];
	int		bitMapFldIndex = -1;
	int		realLen = 0;
	int		ret;
		
	if (!UppleIsValidPackage(ppackage) || (buf == NULL) || (len <= 0))
	{
		printf(__FILE__,__LINE__,"in UppleUnpack:: wrong parameter!\n");
		return(errCodeParameter);
	}

	if ((ret = UppleInitPackage(ppackage)) < 0)
	{
		printf(__FILE__,__LINE__,"in UppleUnpack:: UppleInitPackage!\n");
		return(ret);
	}

	p = buf;
	i = 0;
	//UppleProgramerMemLog("in UppleUnpack::",buf,len);
unpack:	
	// 解无位图域指示的报文段
	for (; i < ppackage->pdef->maxNum; i++)
	{
		if ((ret = UppleReadPackageFldFromStr(ppackage,i,p,len - realLen)) < 0)
		{
			printf(__FILE__,__LINE__,"in UppleUnpack:: UppleReadPackageFldFromStr no bitMap! fldIndex = [%d]\n",i);
			return(ret);
		}
		//if (ppackage->flds[i].len > 0)
		//	printf("%04d %04d %s\n",i,ppackage->flds[i].len,ppackage->flds[i].value);
		realLen += ret;
		p += ret;
		ppackage->flds[i].index = i;
		if ((ppackage->pdef->fldDefTBL[i].valueType % 100 == conBitMapFld) && (ppackage->pdef->fldDefTBL[i].lenOfValue > 0))
		{
			bitMapFldIndex = i;
			break;
		}
	}
	if (i == ppackage->pdef->maxNum)	// 已解完包
	{
		//printf(__FILE__,__LINE__,"After unpack: %s\n",ppackage->pdef->name);
		UppleLogPackage(ppackage);
		//printf(__FILE__,__LINE__,"#TEST:After unpack: %s\n",ppackage->pdef->name);
		return(realLen);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	bcdhex_to_aschex((char *)(ppackage->flds[bitMapFldIndex].value),ppackage->flds[bitMapFldIndex].len,tmpBuf);
//	printf(__FILE__,__LINE__,"in UppleUnpack:: bitMapIndex = [%d] bitMap = [%s]\n",bitMapFldIndex,tmpBuf);
	// 未解完包，继续解有位图域指示的报文段
	for (++i; i < ppackage->pdef->maxNum; i++)
	{
		if (i - bitMapFldIndex >= 8 * ppackage->flds[bitMapFldIndex].len)	// 此域已超出位图的定义
			break;
		if ((i - bitMapFldIndex) % 64 == 0)	// 位图指示域
			continue;
		if (!UppleExistPackageFld(&(ppackage->flds[bitMapFldIndex]),i))		// 域未在位图中定义
			continue;
		if (ppackage->pdef->fldDefTBL[i].lenOfValue <= 0)	// 这个域没定义
		{
			printf(__FILE__,__LINE__,"in UppleUnpack:: the bitMap [%d] have value for [%d],which should have no value!\n",bitMapFldIndex,i);
			return(errCodePackageDefMDL_BitMapHaveValueForNullValueFld);
		}
		if ((ret = UppleReadPackageFldFromStr(ppackage,i,p,len - realLen)) < 0)
		{
			printf(__FILE__,__LINE__,"in UppleUnpack:: UppleReadPackageFldFromStr fldIndex = [%d]\n",i);
			return(ret);
		}
		//if (ppackage->flds[i].len > 0)
		//	printf("%04d %04d %s\n",i,ppackage->flds[i].len,ppackage->flds[i].value);
		realLen += ret;
		p += ret;
		ppackage->flds[i].index = i;
		//printf(__FILE__,__LINE__,"in UppleUnpack:: [%04d] [%04d] [%s]\n",i,ppackage->flds[i].len,ppackage->flds[i].value);
	}
	goto unpack;
}		

int UppleSetPackageFld(PUpplePackage ppackage,int fldIndex,char *value,int len)
{
	int	ret;
	
	if (!UppleIsValidPackage(ppackage) || (value == NULL) || (len < 0))
	{
		printf(__FILE__,__LINE__,"in UppleSetPackageFld:: wrong parameter!\n");
		return(errCodeParameter);
	}

//	printf(__FILE__,__LINE__,"in UppleSetPackageFld:: [%s] [%04d] [%04d] [%s]\n",ppackage->pdef->name,fldIndex,len,value);

	// 索引超界了
	if ((fldIndex < 0) || (fldIndex >= ppackage->pdef->maxNum))
	{
		printf(__FILE__,__LINE__,"in UppleSetPackageFld:: ppackage->pdef->maxNum=[%d]! \n",ppackage->pdef->maxNum );
		printf(__FILE__,__LINE__,"in UppleSetPackageFld:: Field index [%d] is invalid! \n",fldIndex);
		return(errCodePackageDefMDL_InvalidFldIndex);
	}
	
	// 是位图域
	if (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100 == conBitMapFld)
	{
		printf(__FILE__,__LINE__,"in UppleSetPackageFld:: fld [%d][%d] is bitMapFld!\n",fldIndex,ppackage->pdef->fldDefTBL[fldIndex].valueType);
		return(errCodePackageDefMDL_BitMapFldCannotBeSet);
	}
	
	// 如果值的缓冲区不为空
	if (ppackage->flds[fldIndex].value)
	{
		free(ppackage->flds[fldIndex].value);
		ppackage->flds[fldIndex].value = NULL;
	}
	
	// 如果值的长度超过了最大定义长度，将值的长度置为最大长度
	switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
	{
		case	conBitsFld:	// 是二进制数域
			if (len > ppackage->pdef->fldDefTBL[fldIndex].lenOfValue * 2)
			{
				//printf(__FILE__,__LINE__,"in UppleSetPackageFld:: The field [%d] length [%d] larger than defined length [%d]!\n",
				printf(__FILE__,__LINE__,"in UppleSetPackageFld:: The field [%d] length [%d] larger than defined length [%d]!\n",
						fldIndex,len,ppackage->pdef->fldDefTBL[fldIndex].lenOfValue * 2);
				len = ppackage->pdef->fldDefTBL[fldIndex].lenOfValue * 2;
			}
			break;
		default:		// 不是二进制数域
			if (len > ppackage->pdef->fldDefTBL[fldIndex].lenOfValue)
			{
				//printf(__FILE__,__LINE__,"in UppleSetPackageFld:: The field [%d] length [%d] larger than defined length [%d]!\n",
				printf(__FILE__,__LINE__,"in UppleSetPackageFld:: The field [%d] length [%d] larger than defined length [%d]!\n",
						fldIndex,len,ppackage->pdef->fldDefTBL[fldIndex].lenOfValue);
				len = ppackage->pdef->fldDefTBL[fldIndex].lenOfValue;
			}
			break;
	}
	// 置域的长度
	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag == 0)	// 定长域
	{
		switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
		{
			case	conBitsFld:	// 是二进制数域
				ppackage->flds[fldIndex].len = ppackage->pdef->fldDefTBL[fldIndex].lenOfValue * 2;
				break;
			default:
				ppackage->flds[fldIndex].len = ppackage->pdef->fldDefTBL[fldIndex].lenOfValue;
				break;
		}
	}
	else	// 变长域
		ppackage->flds[fldIndex].len = len;
		

	// 为域值分配存储缓冲区
	if ((ppackage->flds[fldIndex].value = (unsigned char *)malloc(ppackage->flds[fldIndex].len+1)) == NULL)
	{
		printf("in UppleSetPackageFld:: malloc [%d]!\n",ppackage->flds[fldIndex].len);
		return(errCodeUseOSErrCode);
	}

	// 拷贝值
	memset(ppackage->flds[fldIndex].value,0,ppackage->flds[fldIndex].len+1);
	memcpy(ppackage->flds[fldIndex].value,value,len);
//	printf(__FILE__,__LINE__,"in UppleSetPackageFld:: after [%s] [%04d] [%04d] [%s]\n",ppackage->pdef->name,fldIndex,ppackage->flds[fldIndex].len,ppackage->flds[fldIndex].value);
	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag > 0)	// 变长域
		return(ppackage->flds[fldIndex].len);
	// 定长域
	switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
	{
		case	conBitsFld:	// 是二进制数域
			return(ppackage->flds[fldIndex].len);
		default:
			break;
	}
		
	// 值的长度与定义长度相同
	if (len == ppackage->pdef->fldDefTBL[fldIndex].lenOfValue)
		return(ppackage->flds[fldIndex].len);

	// 2007/4/29，增加
	/* 如果长度是0，并且长度是0时，定义为不赋值，在此返回，否则向下执行*/	
	if ((len == 0) && (ppackage->pdef->fldDefTBL[fldIndex].valueType / 100 == conUnpatchWhenFldValueLenIsZero))
	{
		printf("in UppleSetPackageFld:: len = 0 and not set this field!\n");
		//ppackage->pdef->fldDefTBL[fldIndex].lenOfValue = 0;
		ppackage->flds[fldIndex].len = 0;
		return(0);
	}
	
	// 值的长度与定义长度不相同，进行补位处理
	switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
	{
		case conAsciiFldLeftBlank:
			memset(ppackage->flds[fldIndex].value,' ',ppackage->flds[fldIndex].len);
			memcpy(ppackage->flds[fldIndex].value+ppackage->flds[fldIndex].len-len,value,len);
			break;
		case conAsciiFldRightBlank:
			memset(ppackage->flds[fldIndex].value,' ',ppackage->flds[fldIndex].len);
			memcpy(ppackage->flds[fldIndex].value,value,len);
			break;
		case conAsciiFldLeftZero:
		case conBitsFldLeftDuiQi:
			memset(ppackage->flds[fldIndex].value,'0',ppackage->flds[fldIndex].len);
			memcpy(ppackage->flds[fldIndex].value+ppackage->flds[fldIndex].len-len,value,len);
			break;
		case conAsciiFldRightZero:
		case conBitsFldRightDuiQi:	
			memset(ppackage->flds[fldIndex].value,'0',ppackage->flds[fldIndex].len);
			memcpy(ppackage->flds[fldIndex].value,value,len);
			break;
		case conAsciiFldRightNull:
			memset(ppackage->flds[fldIndex].value,0,ppackage->flds[fldIndex].len);
			memcpy(ppackage->flds[fldIndex].value,value,len);
			break;
		default:
			printf(__FILE__,__LINE__,"in UppleSetPackageFld:: invalid valueType [%d] def or length [%d] != expected [%d] for fld [%d]\n",
				ppackage->pdef->fldDefTBL[fldIndex].valueType,
				len,
				ppackage->pdef->fldDefTBL[fldIndex].lenOfValue,
				fldIndex);
			return(errCodePackageDefMDL_PackFldLength);
	}
	return(ppackage->flds[fldIndex].len);
}

int UppleReadPackageFld(PUpplePackage ppackage,int fldIndex,char *value,int sizeOfBuf)
{
	if (!UppleIsValidPackage(ppackage) || (value == NULL) || (sizeOfBuf <= 0))
	{
		printf(__FILE__,__LINE__,"in UppleReadPackageFld:: wrong parameter!\n");
		return(errCodeParameter);
	}
	
	// 索引超界
	if ((fldIndex < 0) || (fldIndex >= ppackage->pdef->maxNum))
	{
		//printf(__FILE__,__LINE__,"in UppleReadPackageFld:: Field index [%d] is invalid!\n",fldIndex);
		return(errCodePackageDefMDL_InvalidFldIndex);
	}
	
	// 值的长度错
	if (ppackage->flds[fldIndex].len < 0)
	{
		//printf(__FILE__,__LINE__,"in UppleReadPackageFld:: fldIndex = [%d] real len = [%d] or null value!\n",
		//	fldIndex,ppackage->flds[fldIndex].len);
		return(errCodeISO8583MDL_FldNoValue);
	}
	// 没有值
	if ((ppackage->flds[fldIndex].len == 0) || (ppackage->flds[fldIndex].value == NULL))
		return(0);
	
	// 拷贝值
	memset(value,0,sizeOfBuf);
	if (sizeOfBuf < ppackage->flds[fldIndex].len + 1)
	{
		printf(__FILE__,__LINE__,"in UppleReadPackageFld:: bufsize [%d] smaller than expected [%d] of index [%d]\n",
				sizeOfBuf,ppackage->flds[fldIndex].len + 1,fldIndex);
		return(errCodeSmallBuffer);
	}
	memcpy(value,ppackage->flds[fldIndex].value,ppackage->flds[fldIndex].len);

	switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
	{
		case	conBitsFld:	// 是二进制数域
		case	conBitsFldRightDuiQi:
		case	conBitsFldLeftDuiQi:
		case	conBitMapFld:
			return(ppackage->flds[fldIndex].len);
		default:
			// 去掉空格
			//UppleFilterHeadAndTailBlank(value);
			//return(strlen(value));
			return(ppackage->flds[fldIndex].len);
	}
}

int UppleSetPackageFldWithDefaultValue(PUpplePackage ppackage,int fldIndex)
{
	if (!UppleIsValidPackage(ppackage))
	{
		printf(__FILE__,__LINE__,"in UppleSetPackageFldWithDefaultValue:: wrong parameter!\n");
		return(errCodeParameter);
	}
	
	if ((fldIndex < 0) || (fldIndex >= ppackage->pdef->maxNum))
	{
		printf(__FILE__,__LINE__,"in UppleSetPackageFldWithDefaultValue:: Field index [%d] is invalid!\n",fldIndex);
		return(errCodePackageDefMDL_InvalidFldIndex);
	}
	
	if (ppackage->flds[fldIndex].value)
	{
		free(ppackage->flds[fldIndex].value);
		ppackage->flds[fldIndex].value = NULL;
	}
	
	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag > 0)	// 变长域
	{
		ppackage->flds[fldIndex].len = 0;
		return(0);
	}
	
	// 是定长域
	switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
	{
		case	conBitsFld:	// 是二进制数域
		case	conBitMapFld:
			ppackage->flds[fldIndex].len = ppackage->pdef->fldDefTBL[fldIndex].lenOfValue * 2;
			break;
		default:		// 是ASCII域
			ppackage->flds[fldIndex].len = ppackage->pdef->fldDefTBL[fldIndex].lenOfValue;
			break;
	}
	if ((ppackage->flds[fldIndex].value = (unsigned char *)malloc(ppackage->flds[fldIndex].len+1)) == NULL)
	{
		printf("in UppleSetPackageFldWithDefaultValue:: malloc [%d]!\n",ppackage->flds[fldIndex].len);
		return(errCodeUseOSErrCode);
	}
	switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
	{
		case	conBitsFld:
		case	conBitMapFld:
		case	conBitsFldRightDuiQi:
		case	conBitsFldLeftDuiQi:
			memset(ppackage->flds[fldIndex].value,0,ppackage->flds[fldIndex].len);
			break;
		case	conAsciiFldLeftBlank:
		case	conAsciiFldRightBlank:
			memset(ppackage->flds[fldIndex].value,' ',ppackage->flds[fldIndex].len);
			break;
		case	conAsciiFldLeftZero:
		case	conAsciiFldRightZero:
			memset(ppackage->flds[fldIndex].value,'0',ppackage->flds[fldIndex].len);
			break;
		default:
			memset(ppackage->flds[fldIndex].value,' ',ppackage->flds[fldIndex].len);
			break;
	}
	return(ppackage->flds[fldIndex].len);
}

int UppleSetNullPackageFld(PUpplePackage ppackage,int fldIndex)
{
	if (!UppleIsValidPackage(ppackage))
	{
		printf(__FILE__,__LINE__,"in UppleSetNullPackageFld:: wrong parameter!\n");
		return(errCodeParameter);
	}
	
	if ((fldIndex < 0) || (fldIndex >= ppackage->pdef->maxNum))
	{
		printf(__FILE__,__LINE__,"in UppleSetNullPackageFld:: Field index [%d] is invalid!\n",fldIndex);
		return(errCodePackageDefMDL_InvalidFldIndex);
	}
	
	if (ppackage->flds[fldIndex].value)
	{
		free(ppackage->flds[fldIndex].value);
		ppackage->flds[fldIndex].value = NULL;
	}
	ppackage->flds[fldIndex].len = 0;
	return(0);
}

int UpplePrintPackageFldToFile(PUpplePackageFldDef pfldDef,PUpplePackageFld pfld,FILE *fp)
{
	unsigned char	*ptr = NULL;
	int	newPtr = 0;
	unsigned char	nullPtr[] = "null pointer";
	
	if ((pfld == NULL) || (pfldDef == NULL) || (fp == NULL))
	{
		printf(__FILE__,__LINE__,"in UppleLogPackageFldToFile:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if (pfld->len <= 0)
		return(errCodePackageDefMDL_PackFldLength);
		
	if (pfld->value == NULL)
		ptr = nullPtr;
	else
	{
		switch (pfldDef->valueType % 100)
		{
			case	conBitMapFld:
			case	conBitsFld:
				if ((ptr = (unsigned char *)malloc(pfld->len * 2 + 1)) == NULL)
					ptr = pfld->value;
				else
				{
					newPtr = 1;
					bcdhex_to_aschex((char *)(pfld->value),pfld->len,(char *)ptr);
					ptr[pfld->len * 2] = 0;
				}
				break;
			default:
				ptr = pfld->value;
				break;
		}
	}
	fprintf(fp,"[%04d] [%04d] [%s]\n",pfld->index,pfld->len,ptr);
	if (newPtr)
	{
		free(ptr);
		ptr = NULL;
	}
	return(0);
}	

int UpplePrintPackageToFile(PUpplePackage ppackage,FILE *fp)
{
	int		i;
	
	if ((!UppleIsValidPackage(ppackage)) || (fp == NULL))
	{
		printf(__FILE__,__LINE__,"in UppleLogPackageFldToFile:: wrong parameter!\n");
		return(errCodeParameter);
	}
	
	fprintf(fp,"\n");
	for (i = 0; i < ppackage->pdef->maxNum; i++)
	{
		UpplePrintPackageFldToFile(&(ppackage->pdef->fldDefTBL[i]),&(ppackage->flds[i]),fp);
	}
	fprintf(fp,"\n");

	return(0);
}


int UpplePutBcdBitStrIntoStr(int isLeftDuiQi,char buQiChar,char *bcdBitStr,int len,char *desStr,int sizeOfBuf)
{
	char	tmpBuf[2+1];
	
	if ((len < 0) || (bcdBitStr == NULL) || (desStr == NULL))
	{
		printf(__FILE__,__LINE__,"in UpplePutBcdBitStrIntoStr:: len = [%d] or null pointer %x/%x\n",len,bcdBitStr,desStr);
		return(errCodeParameter);
	}
	if (len == 0)
		return(0);
//	UppleMemLog2(__FILE__,__LINE__,"before package",bcdBitStr,len);
	if (len % 2 == 0)	// 长度是偶数位
	{
		if (len / 2 > sizeOfBuf)
		{
			printf(__FILE__,__LINE__,"in UpplePutBcdBitStrIntoStr:: len [%d] of value [%s] too long!\n",len,bcdBitStr);
			return(errCodeParameter);
		}
		aschex_to_bcdhex(bcdBitStr,len,desStr);
		return(len / 2);
	}
	if (len / 2 + 1 > sizeOfBuf)
	{
		printf(__FILE__,__LINE__,"in UpplePutBcdBitStrIntoStr:: len [%d] of value [%s] too long!\n",len,bcdBitStr);
		return(errCodeParameter);
	}
	// 长度不是偶数位
	if (isLeftDuiQi)	// 左对齐
	{
		aschex_to_bcdhex(bcdBitStr,len-1,desStr);
		tmpBuf[0] = bcdBitStr[len-1];
		tmpBuf[1] = buQiChar;
		tmpBuf[2]=0;
		aschex_to_bcdhex(tmpBuf,2,desStr + len / 2);
	}
	else	// 右对齐
	{
		tmpBuf[0] = buQiChar;
		tmpBuf[1] = bcdBitStr[0];
		aschex_to_bcdhex(tmpBuf,2,desStr);
		aschex_to_bcdhex(bcdBitStr+1,len-1,desStr+1);
	}
	//UppleMemLog2(__FILE__,__LINE__,"after package",desStr,sizeOfBuf);
	return(len / 2 + 1);
}

// 将一个域的长度指示域打入到字符串中
// 返回打入到字符串中的长度
int UpplePutPackageFldLenIntoStr(PUpplePackage ppackage,int fldIndex,unsigned char *buf,int sizeOfBuf)
{
	char	lenBuf[128];
	int	ret;

	if (!UppleIsValidPackage(ppackage) || (buf == NULL) || (sizeOfBuf <= 0))
	{
		printf(__FILE__,__LINE__,"in UpplePutPackageFldLenIntoStr:: wrong parameter!\n");
		return(errCodeParameter);
	}
	
	if ((fldIndex < 0) || (fldIndex >= ppackage->pdef->maxNum))
	{
		printf(__FILE__,__LINE__,"in UpplePutPackageFldLenIntoStr:: Field index [%d] is invalid!\n",fldIndex);
		return(errCodePackageDefMDL_InvalidFldIndex);
	}
	
	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag > sizeOfBuf)
	{
		printf(__FILE__,__LINE__,"in UpplePutPackageFldLenIntoStr:: sizeOfBuf [%d] smaller than expected [%d] of index [%d]\n",
				sizeOfBuf,ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag,fldIndex);
		return(errCodeSmallBuffer);
	}
	// 将长度域转换为字符串
	memset(lenBuf,0,sizeof(lenBuf));
	UppleConvertIntIntoStr(ppackage->flds[fldIndex].len,ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag,lenBuf);
//		printf(__FILE__,__LINE__,"in UpplePutPackageFldLenIntoStr::fldLenPresentation[%d],len[%s] \n",ppackage->pdef->fldLenPresentation,lenBuf);
	switch (ppackage->pdef->fldLenPresentation)	// 判断长度域的表示方法
	{
		case	conFldLenUseLeftDuiQi:
			if ((ret = UpplePutBcdBitStrIntoStr(1,'0',lenBuf,strlen(lenBuf),(char *)buf,sizeOfBuf)) < 0)
			{
				printf(__FILE__,__LINE__,"in UpplePutPackageFldLenIntoStr:: UpplePutBcdBitStrIntoStr index = [%d]\n",fldIndex);
				return(ret);
			}
			return(ret);
		case	conFldLenUseRightDuiQi:
			if ((ret = UpplePutBcdBitStrIntoStr(0,'0',lenBuf,strlen(lenBuf),(char *)buf,sizeOfBuf)) < 0)
			{
				printf(__FILE__,__LINE__,"in UpplePutPackageFldLenIntoStr:: UpplePutBcdBitStrIntoStr index = [%d]\n",fldIndex);
				return(ret);
			}
			return(ret);
		default:
			memcpy(buf,lenBuf,ret = strlen(lenBuf));
			return(ret);
	}
}

//: 将一个域打入到字符串中
// 第一个参数表明是否由位图指示域存在与否
int UpplePutPackageFldIntoStr(int isFldIndentifiedByBitMap,PUpplePackage ppackage,int fldIndex,unsigned char *buf,int sizeOfBuf)
{
	int	ret;
	int	len = 0;
	int	tmpLen;
		
	if (!UppleIsValidPackage(ppackage) || (buf == NULL) || (sizeOfBuf <= 0))
	{
		printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: wrong parameter!\n");
		return(errCodeParameter);
	}
	
	if ((fldIndex < 0) || (fldIndex >= ppackage->pdef->maxNum))
	{
		printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: Field index [%d] is invalid!\n",fldIndex);
		return(errCodePackageDefMDL_InvalidFldIndex);
	}
	
	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfValue <= 0)	// 该域定义为空域，忽略实际域值
		return(0);
	
	if (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100 == conBitMapFld)
	{
		if (!isFldIndentifiedByBitMap)
			return(errCodePackageDefMDL_IsBitMapFld);
	}

	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag > 0)	// 变长域
	{
		if (ppackage->flds[fldIndex].len <= 0)	// 未给该域赋值
		{
			if (isFldIndentifiedByBitMap)	// 是含有位图的报文
				return(0);
			// 不是含有位图的域
			if ((ret = UppleSetPackageFldWithDefaultValue(ppackage,fldIndex)) <0)
			{
				printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: UppleSetPaUppleSetPackageFldWithDefaultValueckageFld fld [%d]!\n",fldIndex);
				return(ret);
			}
		}
		// 将长度域打入包中
		if ((len = UpplePutPackageFldLenIntoStr(ppackage,fldIndex,buf,sizeOfBuf)) < 0)
		{
			printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: UpplePutPackageFldLenIntoStr fldIndex = [%d]\n",fldIndex);
			return(len);
		}
	}
	else	// 定长域
	{
		if (ppackage->flds[fldIndex].len <= 0)	// 未给该域赋值
		{
			if (isFldIndentifiedByBitMap)	// 是含有位图的报文
				return(0);
			// 不是含有位图的报文
			if ((ret = UppleSetPackageFldWithDefaultValue(ppackage,fldIndex)) < 0)
			{
				printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: UppleSetPackageFldWithDefaultValue fld [%d]!\n",fldIndex);
				return(ret);
			}
		}
		switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
		{
			case	conBitsFld:
				if (ppackage->flds[fldIndex].len != ppackage->pdef->fldDefTBL[fldIndex].lenOfValue * 2)
				{
					printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: fld [%d] type = [%d] len = [%d] != defined [%d]\n",fldIndex,ppackage->pdef->fldDefTBL[fldIndex].valueType,ppackage->flds[fldIndex].len,ppackage->pdef->fldDefTBL[fldIndex].lenOfValue * 2);
					return(errCodePackageDefMDL_PackFldLength);
				}
				break;
			case	conBitMapFld:
				if (ppackage->flds[fldIndex].len % 8 != 0)
				{
					printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: fld [%d] type = [%d] len = [%d] error!\n",fldIndex,ppackage->pdef->fldDefTBL[fldIndex].valueType,ppackage->flds[fldIndex].len);
					return(errCodePackageDefMDL_PackFldLength);
				}
				break;
			default:
				if (ppackage->flds[fldIndex].len != ppackage->pdef->fldDefTBL[fldIndex].lenOfValue)
				{
					printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: fld [%d] type = [%d] len = [%d] != defined [%d]\n",fldIndex,ppackage->pdef->fldDefTBL[fldIndex].valueType,ppackage->flds[fldIndex].len,ppackage->pdef->fldDefTBL[fldIndex].lenOfValue);
					return(errCodePackageDefMDL_PackFldLength);
				}
				break;
		}
	}
	// 拷贝值
//	printf(__FILE__,__LINE__,"fld[%d]   len[%d]  type[%d-%d]  value[%s]\n",fldIndex,ppackage->flds[fldIndex].len,isFldIndentifiedByBitMap,ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag,ppackage->flds[fldIndex].value);	
	if (ppackage->flds[fldIndex].value == NULL)
	{
		printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: fld [%d] len = [%d] but value is null pointer!\n",fldIndex,ppackage->flds[fldIndex].len);
		return(errCodePackageDefMDL_PackFldIsNull);
	}
	switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
	{
		case	conBitsFld:
			if ((tmpLen = ppackage->flds[fldIndex].len % 2) != 0)
				tmpLen++;
			if (len + tmpLen > sizeOfBuf)
			{
				printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: sizeOfBuf [%d] smaller than expected [%d] of index [%d]\n",
					sizeOfBuf,len + tmpLen,fldIndex);
				return(errCodeSmallBuffer);
			}
			break;
		default:
			if (len + ppackage->flds[fldIndex].len > sizeOfBuf)
			{
				printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: sizeOfBuf [%d] smaller than expected [%d] of index [%d]\n",
					sizeOfBuf,len + ppackage->flds[fldIndex].len,fldIndex);
				return(errCodeSmallBuffer);
			}
			break;
	}
//	printf(__FILE__,__LINE__,"before pack fld[%d]\n",fldIndex);
//	UppleMemLog2(__FILE__,__LINE__,"package",buf,len);
	switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
	{
		case	conBitsFld:
			aschex_to_bcdhex((char *)(ppackage->flds[fldIndex].value),ppackage->flds[fldIndex].len,(char *)buf+len);
			len = len + ppackage->flds[fldIndex].len / 2;
			break;
		case	conBitsFldLeftDuiQi:	// 左对齐二进制数
			if ((ret = UpplePutBcdBitStrIntoStr(1,'0',(char *)(ppackage->flds[fldIndex].value),
					ppackage->flds[fldIndex].len,(char *)buf+len,sizeOfBuf-len)) < 0)
			{
				printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: UpplePutBcdBitStrIntoStr fldIndex = [%d]\n",fldIndex);
				return(ret);
			}
			len += ret;
			break;
		case	conBitsFldRightDuiQi:	// 右对齐二进制数
			if ((ret = UpplePutBcdBitStrIntoStr(0,'0',(char *)(ppackage->flds[fldIndex].value),
					ppackage->flds[fldIndex].len,(char *)buf+len,sizeOfBuf-len)) < 0)
			{
				printf(__FILE__,__LINE__,"in UpplePutPackageFldIntoStr:: UpplePutBcdBitStrIntoStr fldIndex = [%d]\n",fldIndex);
				return(ret);
			}
			len += ret;
			break;
		default:
			memcpy(buf+len,ppackage->flds[fldIndex].value,ppackage->flds[fldIndex].len);
			len = len + ppackage->flds[fldIndex].len;
			break;
	}
//	printf(__FILE__,__LINE__,"after pack fld[%d]\n",fldIndex);
//	UppleMemLog2(__FILE__,__LINE__,"package",buf,len);
	return(len);
}

int UppleReadBcdBitStrFromStr(int isLeftDuiQi,char *oriStr,int lenOfOriStr,char *bcdBitStr,int lenOfRead)
{
	char	tmpBuf[2048+1];
	
	if ((lenOfOriStr < 0) || (bcdBitStr == NULL) || (oriStr == NULL))
	{
		printf(__FILE__,__LINE__,"in UppleReadBcdBitStrFromStr:: lenOfRead = [%d] or null pointer %x/%x\n",lenOfOriStr,bcdBitStr,oriStr);
		return(errCodeParameter);
	}
	if (lenOfRead == 0)
		return(0);
	if (lenOfRead % 2 == 0)	// 长度是偶数位
	{
		if (lenOfRead / 2 > lenOfOriStr)
		{
			printf(__FILE__,__LINE__,"in UppleReadBcdBitStrFromStr:: lenOfRead [%d] > [%d]!\n",lenOfRead/2,lenOfOriStr);
			return(errCodeParameter);
		}
		bcdhex_to_aschex(oriStr,lenOfRead / 2,bcdBitStr);
		return(lenOfRead / 2);
	}
	// 长度不是偶数位
	if (lenOfRead / 2 + 1 > lenOfOriStr)
	{
		printf(__FILE__,__LINE__,"in UppleReadBcdBitStrFromStr:: lenOfRead [%d] > [%d]!\n",lenOfRead/2+1,lenOfOriStr);
		return(errCodeParameter);
	}
	if (lenOfRead/2+1 > sizeof(tmpBuf))
	{
		printf(__FILE__,__LINE__,"in UppleReadBcdBitStrFromStr:: lenOfRead [%d] too long!\n",lenOfRead);
		return(errCodeParameter);
	}
	bcdhex_to_aschex(oriStr,lenOfRead / 2 + 1,tmpBuf);
	if (isLeftDuiQi)	// 左对齐
		memcpy(bcdBitStr,tmpBuf,lenOfRead);
	else
		memcpy(bcdBitStr,tmpBuf+1,lenOfRead);
	return(lenOfRead/2+1);
}

// 从一个串中读出域的长度指示域
// 返回域的长度指示域在串中的长度
int UppleReadPackageFldLenFromStr(PUpplePackage ppackage,int fldIndex,unsigned char *buf,int lenOfStr)
{
	char	lenBuf[128];
	int	ret;

	if (!UppleIsValidPackage(ppackage) || (buf == NULL) || (lenOfStr <= 0))
	{
		printf(__FILE__,__LINE__,"in UppleReadPackageFldLenFromStr:: wrong parameter!\n");
		return(errCodeParameter);
	}
	
	if ((fldIndex < 0) || (fldIndex >= ppackage->pdef->maxNum))
	{
		printf(__FILE__,__LINE__,"in UppleReadPackageFldLenFromStr:: Field index [%d] is invalid!\n",fldIndex);
		return(errCodePackageDefMDL_InvalidFldIndex);
	}
	
	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag <= 0)	// 不是变长域
	{
		printf(__FILE__,__LINE__,"in UppleReadPackageFldLenFromStr:: fldIndex [%d] not len-variable field!\n",fldIndex);
		return(errCodeParameter);
	}
	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag >= sizeof(lenBuf))	// 变长域的长度指示域检查
	{
		printf(__FILE__,__LINE__,"in UppleReadPackageFldLenFromStr:: fldIndex [%d] lenOfLenFlag [%d] too large!\n",fldIndex,ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag);
		return(errCodeParameter);
	}
	memset(lenBuf,0,sizeof(lenBuf));
	// 获取长度指示域
	switch (ppackage->pdef->fldLenPresentation)	// 判断长度域的表示方法
	{
		case	conFldLenUseLeftDuiQi:
			if ((ret = UppleReadBcdBitStrFromStr(1,(char *)buf,lenOfStr,lenBuf,ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag)) < 0)
			{
				printf(__FILE__,__LINE__,"in UppleReadPackageFldLenFromStr:: UppleReadBcdBitStrFromStr index = [%d]\n",fldIndex);
				return(ret);
			}
			break;
		case	conFldLenUseRightDuiQi:
			if ((ret = UppleReadBcdBitStrFromStr(0,(char *)buf,lenOfStr,lenBuf,ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag)) < 0)
			{
				printf(__FILE__,__LINE__,"in UppleReadPackageFldLenFromStr:: UppleReadBcdBitStrFromStr index = [%d]\n",fldIndex);
				return(ret);
			}
			break;
		default:
			if (ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag > lenOfStr)
			{
				printf(__FILE__,__LINE__,"in UppleReadPackageFldLenFromStr:: lenOfStr [%d] too small for fldIndex [%d]\n",lenOfStr,fldIndex);
				return(errCodeParameter);
			}
			memcpy(lenBuf,buf,ret = ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag);
			break;
	}
	ppackage->flds[fldIndex].len = atoi(lenBuf);
	return(ret);
}

// 从一个串中读出一个域
// 第一个参数表明是否由位图指示域存在与否
int UppleReadPackageFldFromStr(PUpplePackage ppackage,int fldIndex,unsigned char *buf,int lenOfStr)
{
	int	ret;
	int	realLen = 0;
	int	fldLen;
	
	if (!UppleIsValidPackage(ppackage) || (buf == NULL) || (lenOfStr < 0))
	{
		printf(__FILE__,__LINE__,"in UppleReadPackageFldFromStr:: wrong parameter! lenOfStr = [%d] [%x][%x]\n",lenOfStr,buf,ppackage);
		return(errCodeParameter);
	}
	
	if ((fldIndex < 0) || (fldIndex >= ppackage->pdef->maxNum))
	{
		printf(__FILE__,__LINE__,"in UppleReadPackageFldFromStr:: Field index [%d] is invalid!\n",fldIndex);
		return(errCodePackageDefMDL_InvalidFldIndex);
	}
	
	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfValue <= 0)	// 该域定义为空域，忽略实际域值
		return(0);

	if (ppackage->flds[fldIndex].value != NULL)
		free(ppackage->flds[fldIndex].value);
		
	if (ppackage->pdef->fldDefTBL[fldIndex].lenOfLenFlag > 0)	// 变长域
	{
		if ((realLen = UppleReadPackageFldLenFromStr(ppackage,fldIndex,buf,lenOfStr)) < 0)
		{
			printf(__FILE__,__LINE__,"in UppleUnpack:: UppleReadPackageFldLenFromStr fldIndex [%d]\n",fldIndex);
			return(realLen);
		}
	}
	else	// 定长域
	{
		if (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100 == conBitMapFld)
			ppackage->flds[fldIndex].len = UppleGetPackageBitMapFldRealLength(&(ppackage->pdef->fldDefTBL[fldIndex]),buf);
		else
			ppackage->flds[fldIndex].len = ppackage->pdef->fldDefTBL[fldIndex].lenOfValue;
	}
	if (ppackage->flds[fldIndex].len <= 0) // 域无值
		return(realLen);
	
	// 以下读取域
	fldLen = ppackage->flds[fldIndex].len;
	switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
	{
		case	conBitsFld:
			ppackage->flds[fldIndex].len *= 2;
			break;
		default:
			break;
	}
	if ((ppackage->flds[fldIndex].value = (unsigned char *)malloc(ppackage->flds[fldIndex].len + 1)) == NULL)
	{
		printf("in UppleUnpack:: malloc [%d]!\n",ppackage->flds[fldIndex].len + 1);
		return(errCodeUseOSErrCode);
	}
	memset(ppackage->flds[fldIndex].value,0,ppackage->flds[fldIndex].len + 1);
	switch (ppackage->pdef->fldDefTBL[fldIndex].valueType % 100)
	{
		case	conBitsFld:
			bcdhex_to_aschex((char *)buf+realLen,fldLen,(char *)(ppackage->flds[fldIndex].value));
			realLen += fldLen;
			break;
		case	conBitsFldLeftDuiQi:
			if ((ret = UppleReadBcdBitStrFromStr(1,(char *)buf+realLen,lenOfStr-realLen,(char *)(ppackage->flds[fldIndex].value),fldLen)) < 0)
			{
				printf(__FILE__,__LINE__,"in UppleReadPackageFldFromStr:: UppleReadBcdBitStrFromStr fldIndex = [%d]\n",fldIndex);
				return(ret);
			}
			realLen += ret;
			break;
		case	conBitsFldRightDuiQi:
			if ((ret = UppleReadBcdBitStrFromStr(0,(char *)buf+realLen,lenOfStr-realLen,(char *)(ppackage->flds[fldIndex].value),fldLen)) < 0)
			{
				printf(__FILE__,__LINE__,"in UppleReadPackageFldFromStr:: UppleReadBcdBitStrFromStr fldIndex = [%d]\n",fldIndex);
				return(ret);
			}
			realLen += ret;
			break;
		default:
			memcpy(ppackage->flds[fldIndex].value,buf+realLen,fldLen);
			realLen += fldLen;
			break;
	}
	return(realLen);
}


int UppleGetFlds(PUpplePackage ppackage,TUppleTransFld ptransfld)
{
	int		i;
	char		*ptr = NULL;
	char		nullPtr[] = "null pointer";
	char		tmpBuf[512+1];
	PUpplePackageFldDef pfldDef;
	PUpplePackageFld pfld;
	int index;
		
	if (!UppleIsValidPackage(ppackage))
	{
		printf(__FILE__,__LINE__,"  [%s-%d]in UppleGetFlds:: wrong parameter!\n",__FILE__,__LINE__);
		return(errCodeParameter);
	}
	
	for (i = 0; i < ppackage->pdef->maxNum; i++)
	{
		pfldDef=&(ppackage->pdef->fldDefTBL[i]);
		pfld=&(ppackage->flds[i]);
		
		index=pfld->index;	
		ptransfld[index][0]=0;	
		
		if ((pfld == NULL) || (pfldDef == NULL))
        	{
        		continue;
		}
        	if (pfld->len <= 0)
			continue;
        	if ((ptr = (char *)(pfld->value)) == NULL)
                	ptr = nullPtr;
        	if (pfldDef->valueType % 100 == conBitMapFld)   // 是位图
        	{
                	if (pfld->len * 2 > sizeof(tmpBuf))
                	{
                        	printf(__FILE__,__LINE__,"  [%s-%d]in UppleGetFlds:: bit map too long!\n",__FILE__,__LINE__);
                        	return(errCodeParameter);
                	}
                	bcdhex_to_aschex(ptr,pfld->len,tmpBuf);
                	tmpBuf[pfld->len*2] = 0;
                	ptr = tmpBuf;
        	}
		strncpy(ptransfld[index],ptr,pfld->len);
		//printf(__FILE__,__LINE__,"index[%d],len [%d] value[%s]\n",index,pfld->len,ptransfld[index]);
	}

 //                       	printf(__FILE__,__LINE__,"  [%s-%d]in UppleGetFlds:: \n",__FILE__,__LINE__);
	return 0;
}
#if 0
int UpplePackageFldToMM(PUpplePackageFldDef pfldDef,PUpplePackageFld pfld,char *prefix)
{
	unsigned char	*ptr = NULL;
	int	newPtr = 0;
	unsigned char	nullPtr[] = "null pointer";
	char  mm_name[ 128 ];

	//printf(__FILE__,__LINE__,"###UppleSet RPUB,...1\n");
	
	if ((pfld == NULL) || (pfldDef == NULL) )
	{
		printf(__FILE__,__LINE__,"in UppleLogPackageFldToFile:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if (pfld->len <= 0)
		return(errCodePackageDefMDL_PackFldLength);
		
/*
	if (pfld->value == NULL)
		ptr = nullPtr;
	else
*/
	//printf(__FILE__,__LINE__,"###UppleSet RPUB,...2\n");
	if(pfld->value)
	{
	//printf(__FILE__,__LINE__,"###UppleSet RPUB,...3\n");
		switch (pfldDef->valueType % 100)
		{
/*
			case	conBitMapFld:
			case	conBitsFld:
				if ((ptr = (unsigned char *)malloc(pfld->len * 2 + 1)) == NULL)
					ptr = pfld->value;
				else
				{
					newPtr = 1;
					bcdhex_to_aschex((char *)(pfld->value),pfld->len,(char *)ptr);
					ptr[pfld->len * 2] = 0;
				}
				break;
*/
			default:
				ptr = pfld->value;
				break;
		}
	}

if( ptr )
{
	memset( mm_name, 0x00, sizeof(mm_name) );
	sprintf( mm_name, "%s%d", prefix, pfld->index );

	//printf(__FILE__,__LINE__,"###UppleSet RPUB,...4\n");
	//UppleSet( mm_name, ptr );
	UppleSet2( RPUB, mm_name, ptr );

	//printf(__FILE__,__LINE__,"###UppleSet RPUB,[%s]=[%s] \n", mm_name, ptr );

/*
	printf(__FILE__,__LINE__,"###UppleSet RPUB,[%s]=[%s] \n", mm_name, ptr );
{
	char buf3[ 1024 ];
	//printf(__FILE__,__LINE__,"###UppleSet RPUB,[%s]=[%s] \n", mm_name, ptr );
	memset( buf3, 0x00, sizeof(buf3) );
	UppleGet2( RPUB, mm_name, buf3 );
	//printf(__FILE__,__LINE__,"###TEST UppleGet,[%s]=[%s] \n", mm_name, buf3 );
}
*/

	if (newPtr)

		free(ptr);
		ptr = NULL;
	}
/*
{
	char buf3[ 1024 ];
	//printf(__FILE__,__LINE__,"###UppleSet RPUB,[%s]=[%s] \n", mm_name, ptr );
	memset( buf3, 0x00, sizeof(buf3) );
	UppleGet2( RPUB, mm_name, buf3 );
	printf(__FILE__,__LINE__,"###TEST UppleGet,[%s]=[%s] \n", mm_name, buf3 );
}
*/

	return(0);
}	


int UpplePackageToMM(PUpplePackage ppackage,char *prefix)
{
	int		i;
	
	//printf(__FILE__,__LINE__,"###UpplePackageToMM begin...\n");

	if ((!UppleIsValidPackage(ppackage)))
	{
		printf(__FILE__,__LINE__,"in UppleLogPackageFldToFile:: wrong parameter!\n");
		return(errCodeParameter);
	}
	
	//fprintf(fp,"\n");
	for (i = 0; i < ppackage->pdef->maxNum; i++)
	{
	//printf(__FILE__,__LINE__,"###UpplePackageToMM i=[%d]...\n", i);
		UpplePackageFldToMM(&(ppackage->pdef->fldDefTBL[i]),&(ppackage->flds[i]),prefix);
	}
	//fprintf(fp,"\n");

	//printf(__FILE__,__LINE__,"###UpplePackageToMM end...\n");




	return(0);
}
#endif
