//	Author:		zhangyongding
//	Date:		20081018
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>

#include "upplePackage.h"
#include "uppleStr.h"
#include "platFormEnv.h"
#include "platFormPackage.h"
#include "ylPackage.h"

#include "termPackage.h"
#include "specErrCode.h"
#include "transDB.h"

#include "buffer_pool.h"
#include "uppleModuleVar.h"

#include "ExpressionParsing.h"

#include "strUnPcakKeyValue.h"

extern XBUF *RPUB;

#define DBSEMKEY (key_t)0x2000 
extern int setsemvalue(key_t key); /*订单号生成需要的信号量*/

PUpplePackage pguppleTermPackage = NULL;
PUpplePackage pguppleTermPackageHeader = NULL;
int guppleLenOfTermPackageHeader = -1;
int gupplePlatFormIsOccurer = 0;

int UppleIsPlatFormAsOccurer()
{
	return (gupplePlatFormIsOccurer);
}

int UppleSetPlatFormAsOccurer()
{
	gupplePlatFormIsOccurer = 1;
	return (0);
}

int UppleSetExternalAsOccurer()
{
	gupplePlatFormIsOccurer = 0;
	return (0);
}

PUpplePackage UppleGetTermPackage()
{
	int ret;

	if ((ret = UppleConnectTermPackage()) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleGetTermPackage:: UppleConnectTermPackage!\n");
		return (NULL);
	}
	else
		return (pguppleTermPackage);
}

int UppleIsTermPackageConnected()
{
	if ((!UppleIsValidPackage(pguppleTermPackage)) || (!UppleIsValidPackage(
			pguppleTermPackageHeader)) || (guppleLenOfTermPackageHeader < 0))
		return (0);
	else
		return (1);
}

int UppleConnectTermPackage()
{
	PUpplePackage ppackage = NULL;
	int ret;
	PUpplePackageDef pdef;

	if (UppleIsTermPackageConnected())
		return (0);

	if ((pguppleTermPackage = UppleConnectPackage("term8583")) == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleConnectTermPackage:: UppleConnectPackage [%s]\n",
				"term8583");
		return (errCodeCurrentMDL_Connect8583PackageMDL);
	}
	if ((pguppleTermPackageHeader = UppleConnectPackage("termPackHeader"))
			== NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleConnectTermPackage:: UppleConnectPackage [%s]\n",
				"termPackHeader");
		return (errCodeCurrentMDL_Connect8583PackageMDL);
	}
	if ((guppleLenOfTermPackageHeader = UppleGetLengthOfTermPackageHeader())
			< 0)
		guppleLenOfTermPackageHeader = 0;
	return (0);
}

int UppleDisconnectTermPackage()
{
	UppleDisconnectPackage(pguppleTermPackageHeader);
	return (UppleDisconnectPackage(pguppleTermPackage));
}

int UppleUnpackTermPackage(unsigned char *data, int lenOfData)
{
	int offset;
	int ret;
	char rejectCode[10 + 1];

	if ((ret = UppleConnectTermPackage()) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleUnpackTermPackage:: UppleUnpackTermPackage!\n");
		return (ret);
	}

		//UppleMemLog2(__FILE__,__LINE__,"&&&&&&&&&&&&&&&&&&&TTTTtermpackage:",data,lenOfData);

	UppleLog2(__FILE__, __LINE__,
			"Unpack, guppleLenOfTermPackageHeader=[%d]\n",
			guppleLenOfTermPackageHeader);
	if (guppleLenOfTermPackageHeader > 0) // 有报文头
	{
		UppleInitPackage(pguppleTermPackageHeader);
		if ((ret = UppleUnpack(pguppleTermPackageHeader, data, lenOfData)) < 0)
		{
			UppleUserErrLog2(
					__FILE__,
					__LINE__,
					"in UppleUnpackTermPackage:: UppleUnpack pguppleTermPackageHeader [%d]\n",
					lenOfData);
			return (ret);
		}
		UpplePackageToMM(pguppleTermPackageHeader, "TH");
		UpplePackageToMM(pguppleTermPackageHeader, "PH");
	}
	UppleInitPackage(pguppleTermPackage);
	if ((ret = UppleUnpack(pguppleTermPackage, data
			+ guppleLenOfTermPackageHeader, lenOfData
			- guppleLenOfTermPackageHeader)) < 0)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"in UppleUnpackTermPackage:: UppleUnpack pguppleTermPackage [%d]\n",
				lenOfData - guppleLenOfTermPackageHeader);
		return (ret);
	}

	UpplePackageToMM(pguppleTermPackage, "TB");
	UpplePackageToMM(pguppleTermPackage, "PB");
	return (0);
}

int UpplePackTermPackageHeader(unsigned char *buf, int sizeOfBuf, int lenOfData)
{
	char tmpBuf1[64];
	char tmpBuf2[64];
	char *ptr;
	int ret;

	if ((ret = UppleConnectTermPackage()) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UpplePackTermPackageHeader:: UppleConnectTermPackage!\n");
		return (ret);
	}

	if (gupplePlatFormIsOccurer)
	{
		UppleSetPackageFld(pguppleTermPackageHeader, 0, "60", 2);
		UppleSetPackageFld(pguppleTermPackageHeader, 1,
				UppleGetSourceAddrOfPlatFormAsOccurer(), 4);
		UppleSetPackageFld(pguppleTermPackageHeader, 2,
				UppleGetDesAddrOfPlatFormAsOccurer(), 4);

		/*
		 //For temp test
		 UppleSetPackageFld(pguppleTermPackageHeader,3,"0000000012345678",16);
		 UppleSetPackageFld(pguppleTermPackageHeader,4,"0000000055555555",16);
		 */
	}
	else
	{
		// 原地址和目的地址互换
		memset(tmpBuf1, 0, sizeof(tmpBuf1));
		UppleReadPackageFld(pguppleTermPackageHeader, 1, tmpBuf1,
				sizeof(tmpBuf1));
		memset(tmpBuf2, 0, sizeof(tmpBuf2));
		UppleReadPackageFld(pguppleTermPackageHeader, 2, tmpBuf2,
				sizeof(tmpBuf2));

		UppleSetPackageFld(pguppleTermPackageHeader, 1, tmpBuf2, 4);
		UppleSetPackageFld(pguppleTermPackageHeader, 2, tmpBuf1, 4);
		/*
		 //For temp test
		 UppleSetPackageFld(pguppleTermPackageHeader,3,"0000000012345678",16);
		 UppleSetPackageFld(pguppleTermPackageHeader,4,"0000000055555555",16);
		 */
	}
	ret = UpplePack(pguppleTermPackageHeader, buf, sizeOfBuf);

	return ret;
}

int UpplePackTermPackage(unsigned char *buf, int sizeOfBuf)
{
	int lenOfTerm;
	int lenOfHeader;
	int ret;

	if ((ret = UppleConnectTermPackage()) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UpplePackTermPackage:: UppleConnectTermPackage!\n");
		return (ret);
	}

	if ((lenOfTerm = UpplePack(pguppleTermPackage, buf
			+ guppleLenOfTermPackageHeader, sizeOfBuf
			- guppleLenOfTermPackageHeader)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UpplePackTermPackage:: UpplePack pguppleTermPackage!\n");
		return (lenOfTerm);
	}

	UpplePackageToMM(pguppleTermPackage, "TB");
	UpplePackageToMM(pguppleTermPackage, "PB");

	if (lenOfTerm == 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UpplePackTermPackage:: term Pack = [%d]\n", lenOfTerm);
		return (errCodeCurrentMDL_LengthOf8583PackIsZero);
	}
	if (guppleLenOfTermPackageHeader <= 0) // 无报文头
		return (lenOfTerm);

	if ((lenOfHeader = UpplePackTermPackageHeader(buf,
			guppleLenOfTermPackageHeader, lenOfTerm)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UpplePackTermPackage:: UpplePack pguppleTermPackageHeader!\n");
		return (lenOfHeader);
	}

	UpplePackageToMM(pguppleTermPackageHeader, "TH");
	UpplePackageToMM(pguppleTermPackageHeader, "PH");

	//UppleMemLog2(__FILE__,__LINE__,"term package:",buf,lenOfTerm+lenOfHeader);
	return (lenOfTerm + lenOfHeader);
}

int UppleSetTermFld(int fldIndex, char *buf, int len)
{
	int ret;

	if ((ret = UppleConnectTermPackage()) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleSetTermFld:: UppleConnectTermPackage!\n");
		return (ret);
	}

	return (UppleSetPackageFld(pguppleTermPackage, fldIndex, buf, len));
}

int UppleSetTermFldNull(int fldIndex)
{
	int ret;

	if ((ret = UppleConnectTermPackage()) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleSetTermFldNull:: UppleConnectTermPackage!\n");
		return (ret);
	}

	return (UppleSetNullPackageFld(pguppleTermPackage, fldIndex));
}

int UppleReadTermFld(int fldIndex, char *buf, int sizeOfBuf)
{
	int ret;

	if ((ret = UppleConnectTermPackage()) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleReadTermFld:: UppleConnectTermPackage!\n");
		return (ret);
	}

	return (UppleReadPackageFld(pguppleTermPackage, fldIndex, buf, sizeOfBuf));
}

int UppleInitTermPackage()
{
	int ret;

	if ((ret = UppleConnectTermPackage()) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleInitTermPackage:: UppleConnectTermPackage!\n");
		return (ret);
	}
	if (((ret = UppleInitPackage(pguppleTermPackageHeader)) < 0) || ((ret
			= UppleInitPackage(pguppleTermPackage)) < 0))
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"in UppleInitTermPackage:: UppleInitPackage pguppleTermPackageHeader pguppleTermPackage!\n");
	return (ret);
}

int UppleIsTermRequestPackage(unsigned char *buf, int len)
{
	char mti[4 + 1];
	int ret;

	if ((ret = UppleUnpackTermPackage(buf, len)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleIsTermRequestPackage:: UppleUnpackTermPackage!\n");
		return (0);
	}

	memset(mti, 0, sizeof(mti));
	if ((ret = UppleReadPackageFld(pguppleTermPackage, 0, mti, sizeof(mti)))
			< 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleIsTermRequestPackage:: UppleReadPackageFld mti!\n");
		return (0);
	}

	if ((mti[2] - '0') % 2 == 0)
		return (1);
	else
		return (0);
}

PUpplePackageFldDef UppleFindTermFldDefOfIndex(int index)
{
	return (UppleFindPackageFldDefOfIndex("term8583", index));
}

int UppleSetTermResponseMTIFromRequestMTI()
{
	char tmpBuf[10];
	int ret;

	memset(tmpBuf, 0, sizeof(tmpBuf));
	if ((ret = UppleReadTermFld(0, tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleSetTermResponseMTIFromRequestMTI:: UppleReadTermFld 0!\n");
		return (ret);
	}
	if ((tmpBuf[2] + '0') % 2 != 0)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"in UppleSetTermResponseMTIFromRequestMTI:: mti not request [%s]\n",
				tmpBuf);
		return (errCodeCurrentMDL_Not8583RequestMTI);
	}
	tmpBuf[2] += 1;
	return (UppleSetTermFld(0, tmpBuf, 4));
}

int UpplePrintTermPackageToFile(FILE *fp)
{
	return (UpplePrintPackageToFile(pguppleTermPackage, fp));
}

int UppleReadCardNoFromTermPackage(char *value)
{
	int len;
	char cardNo[128 + 1];
	char tmpBuf[128 + 1];

	// 读取第2域
	/*
	 memset(cardNo,0,sizeof(cardNo));
	 if ((len = UppleReadTermFld(2,cardNo,sizeof(cardNo))) < 0)
	 {
	 UppleUserErrLog2(__FILE__,__LINE__,"in UppleReadCardNoFromTermPackage:: UppleReadTermFld 2!\n");
	 return(len);
	 }
	 if (len > 0)
	 {
	 memcpy(value,cardNo,len);
	 return(len);
	 }
	 */
	memset(cardNo, 0, sizeof(cardNo));
	UppleGet2(RPUB, "PB2", cardNo);
	UppleLog2(__FILE__, __LINE__, "#TEST:Get cardno ::PB2=[%s]\n", cardNo);
	len = strlen(cardNo);
	if (len > 0)
	{
		memcpy(value, cardNo, len);
		return (len);
	}

	// 读取第35域
	/*
	 memset(tmpBuf,0,sizeof(tmpBuf));
	 if ((len = UppleReadTermFld(35,tmpBuf,sizeof(tmpBuf))) < 0)
	 {
	 UppleUserErrLog2(__FILE__,__LINE__,"in UppleReadCardNoFromTermPackage:: UppleReadTermFld 35!\n");
	 return(len);
	 }
	 */
	memset(tmpBuf, 0, sizeof(tmpBuf));
	UppleGet2(RPUB, "PB35", tmpBuf);
	UppleLog2(__FILE__, __LINE__, "#TEST:Get cardno ::PB35=[%s]\n", tmpBuf);
	len = strlen(tmpBuf);
	if (len > 0)
	{
		memset(cardNo, 0, sizeof(cardNo));
		if ((len = UppleReadFldFromBuf(tmpBuf, '=', 1, cardNo)) > 19)
			if ((len = UppleReadFldFromBuf(tmpBuf, 'D', 1, cardNo)) > 19)
				return (errCodeCurrentMDL_8583PackageNotAcctno);
		memcpy(value, cardNo, len);
		return (len);
	}
	// 读取第36域
	/*
	 memset(tmpBuf,0,sizeof(tmpBuf));
	 if ((len = UppleReadTermFld(36,tmpBuf,sizeof(tmpBuf))) < 0)
	 {
	 UppleUserErrLog2(__FILE__,__LINE__,"in UppleReadCardNoFromTermPackage:: UppleReadTermFld 36!\n");
	 return(len);
	 }
	 */
	memset(tmpBuf, 0, sizeof(tmpBuf));
	UppleGet2(RPUB, "PB36", tmpBuf);
	UppleLog2(__FILE__, __LINE__, "#TEST:Get cardno ::PB36=[%s]\n", tmpBuf);
	len = strlen(tmpBuf);
	if (len > 0)
	{
		memset(cardNo, 0, sizeof(cardNo));
		if ((len = UppleReadFldFromBuf(tmpBuf + 4, '=', 1, cardNo)) > 19)
			if ((len = UppleReadFldFromBuf(tmpBuf + 4, 'D', 1, cardNo)) > 19)
				return (errCodeCurrentMDL_8583PackageNotAcctno);
		memcpy(value, cardNo, len);
		UppleSet2(RPUB, "CARD_NO", value);
		return (len);
	}
	return (errCodeCurrentMDL_8583PackageNotAcctno);
}

TUppleDealModuleID UppleGetDealMDLIDOfPackageReceivedFromTerm(
		unsigned char *buf, int len)
{
	int ret;
	char fld000[4 + 1];
	char cardno[19 + 1];
	char routeid[2 + 1];
	char fld003[6 + 1];

	/*	// 解包
	 if ((ret = UppleUnpackTermPackage(buf,len)) < 0)
	 {
	 UppleUserErrLog2(__FILE__,__LINE__,"  [%s-%d]in UppleGetDealMDLIDOfPackageReceivedFromTerm:: UppleUnpackTermPackage [%d] [%s]!\n",__FILE__,__LINE__,len,buf);
	 return(ret);
	 }
	 */
	memset(fld000, 0, sizeof(fld000));
	if ((ret = UppleReadTermFld(0, fld000, sizeof(fld000))) < 0)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"  [%s-%d]in UppleGetDealMDLIDOfPackageReceivedFromTerm:: UppleReadTermFld 0!\n",
				__FILE__, __LINE__);
		return (ret);
	}
	memset(fld003, 0, sizeof(fld003));
	if ((ret = UppleReadTermFld(3, fld003, sizeof(fld003))) < 0)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"  [%s-%d]in UppleGetDealMDLIDOfPackageReceivedFromTerm:: UppleReadTermFld 3!\n",
				__FILE__, __LINE__);
		return (ret);
	}

	// 管理类交易和非银行类交易，不需要路由标识
	if (((memcmp(fld000, "0100", 4) == 0)
			&& ((memcmp(fld003, "950000", 6) == 0) || (memcmp(fld003, "951000",
					6) == 0))) || (memcmp(fld003, "320000", 6) == 0)
			|| (memcmp(fld000, "0500", 4) == 0))
		return (UppleDecideTermTransDealMDLID(fld000, fld003, "--"));
	if (memcmp(fld003, "360000", 6) == 0 || memcmp(fld003, "960000", 6) == 0
			|| (memcmp(fld000, "0200", 4) == 0 || memcmp(fld000, "0400", 4)
					== 0) && memcmp(fld003, "500000", 6) == 0 || memcmp(fld003,
			"600000", 6) == 0)
		return (UppleDecideTermTransDealMDLID(fld000, fld003, "PO"));

	return (UppleDecideTermTransDealMDLID(fld000, fld003, "PP"));
}

int UppleReadTermTPDU(char *tpdu)
{
	char tmpBuf[42 + 1];

	if (guppleLenOfTermPackageHeader > 0) // 有报文头
	{
		memset(tmpBuf, 0, sizeof(tmpBuf));
		UppleReadPackageFld(pguppleTermPackageHeader, 0, tmpBuf, sizeof(tmpBuf));
		memcpy(tpdu, tmpBuf, 2);
		memset(tmpBuf, 0, sizeof(tmpBuf));
		UppleReadPackageFld(pguppleTermPackageHeader, 1, tmpBuf, sizeof(tmpBuf));
		memcpy(tpdu + 2, tmpBuf, 4);
		memset(tmpBuf, 0, sizeof(tmpBuf));
		UppleReadPackageFld(pguppleTermPackageHeader, 2, tmpBuf, sizeof(tmpBuf));
		memcpy(tpdu + 6, tmpBuf, 4);
	}
	return (0);
}

int UppleFormTermPayerPrimaryKeyFromPackage(PUppleTermPrimaryKey ptermKey)
{
	int ret;

	if (ptermKey == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayerPrimaryKeyFromPackage:: ptermKey is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptermKey, 0, sizeof(*ptermKey));

	if ((ret = UppleReadTermFld(11, ptermKey->termssn,
			sizeof(ptermKey->termssn))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayerPrimaryKeyFromPackage:: UppleReadTermFld 11!\n");
		return (ret);
	}
	if ((ret = UppleReadTermFld(41, ptermKey->termno, sizeof(ptermKey->termno)))
			< 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayerPrimaryKeyFromPackage:: UppleReadTermFld 41!\n");
		return (ret);
	}
	if ((ret = UppleReadTermFld(63, ptermKey->termbatchid,
			sizeof(ptermKey->termbatchid))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayerPrimaryKeyFromPackage:: UppleReadTermFld 63!\n");
		return (ret);
	}
	sprintf(ptermKey->payflag, "%d", conPayFlagIsPayer);
	return (0);

}

int UppleFormTermPayeePrimaryKeyFromPackage(PUppleTermPrimaryKey ptermKey)
{
	int ret;

	if (ptermKey == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayeePrimaryKeyFromPackage:: ptermKey is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptermKey, 0, sizeof(*ptermKey));

	if ((ret = UppleReadTermFld(11, ptermKey->termssn,
			sizeof(ptermKey->termssn))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayeePrimaryKeyFromPackage:: UppleReadTermFld 11!\n");
		return (ret);
	}
	if ((ret = UppleReadTermFld(41, ptermKey->termno, sizeof(ptermKey->termno)))
			< 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayeePrimaryKeyFromPackage:: UppleReadTermFld 41!\n");
		return (ret);
	}
	if ((ret = UppleReadTermFld(63, ptermKey->termbatchid,
			sizeof(ptermKey->termbatchid))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayeePrimaryKeyFromPackage:: UppleReadTermFld 63!\n");
		return (ret);
	}
	sprintf(ptermKey->payflag, "%d", conPayFlagIsPayee);
	return (0);

}

int UppleFormTermSaleRequestDBRecFromPackage(PUppleTermRequestDBRec ptermReqRec)
{
	int i;
	int ret;
	int offset = 0;
	char number[1 + 1];
	char tmpBuf[128 + 1];

	if (ptermReqRec == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormSaleTermRequestDBRecFromPackage:: ptermReqRec is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptermReqRec, 0, sizeof(*ptermReqRec));

	UppleFormTermPayerPrimaryKeyFromPackage(&(ptermReqRec->primaryKey));

	UppleReadTermTPDU(ptermReqRec->termtpdu);

	memset(tmpBuf, 0, sizeof(tmpBuf));
	UppleReadTermFld(0, tmpBuf, 4 + 1);
	UppleReadTermFld(3, tmpBuf + 4, 6 + 1);
	strcpy(ptermReqRec->termtransid, tmpBuf);

	UppleReadTermFld(4, ptermReqRec->termamount,
			sizeof(ptermReqRec->termamount));
	UppleReadTermFld(42, ptermReqRec->termmid, sizeof(ptermReqRec->termmid));
	UppleReadTermFld(2, ptermReqRec->paycard, sizeof(ptermReqRec->paycard));

	return (0);

}

int UppleFormTermReceiveRequestDBRecFromPackage(
		PUppleTermRequestDBRec ptermReqRec)
{
	int i;
	int ret;
	int offset = 0;
	char number[1 + 1];
	char tmpBuf[128 + 1];

	if (ptermReqRec == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormSaleTermRequestDBRecFromPackage:: ptermReqRec is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptermReqRec, 0, sizeof(*ptermReqRec));

	UppleFormTermPayerPrimaryKeyFromPackage(&(ptermReqRec->primaryKey));

	UppleReadTermTPDU(ptermReqRec->termtpdu);

	memset(tmpBuf, 0, sizeof(tmpBuf));
	UppleReadTermFld(0, tmpBuf, 4 + 1);
	UppleReadTermFld(3, tmpBuf + 4, 6 + 1);
	strcpy(ptermReqRec->termtransid, tmpBuf);

	UppleReadTermFld(4, ptermReqRec->termamount,
			sizeof(ptermReqRec->termamount));
	UppleReadTermFld(42, ptermReqRec->termmid, sizeof(ptermReqRec->termmid));
	/*	UppleReadTermFld(2,ptermReqRec->paycard,sizeof(ptermReqRec->paycard));

	 if(ptermReqRec->paycard==NULL){
	 UppleReadTermFld(35,fld35,sizeof(fld35));
	 i=0;
	 while(fld35[i]!='D')
	 {
	 ptermReqRec->paycard[i]=fld35[i];
	 i++;
	 }
	 ptermReqRec->paycard[i]=0;

	 }
	 */

	/*
	 if((ret=UppleReadCardNoFromTermPackage(ptermReqRec->paycard))<0){
	 UppleUserErrLog2(__FILE__,__LINE__,"in UppleFormSaleTermRequestDBRecFromPackage:: ptermReqRec is NULL!\n");
	 return(ret);

	 }
	 return(0);
	 */

}

int UppleFormTermPayerRequestDBRecFromPackage(
		PUppleTermRequestDBRec ptermReqRec)
{
	int i;
	int ret;
	int offset = 0;
	char number[1 + 1];
	char tmpBuf[128 + 1];
	if (ptermReqRec == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayerRequestDBRecFromPackage:: ptermReqRec is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptermReqRec, 0, sizeof(*ptermReqRec));

	UppleFormTermPayerPrimaryKeyFromPackage(&(ptermReqRec->primaryKey));

	UppleReadTermTPDU(ptermReqRec->termtpdu);

	memset(tmpBuf, 0, sizeof(tmpBuf));
	UppleReadTermFld(0, tmpBuf, 4 + 1);
	UppleReadTermFld(3, tmpBuf + 4, 6 + 1);
	strcpy(ptermReqRec->termtransid, tmpBuf);

	UppleReadTermFld(4, ptermReqRec->termamount,
			sizeof(ptermReqRec->termamount));
	UppleReadTermFld(42, ptermReqRec->termmid, sizeof(ptermReqRec->termmid));

	if ((ret = UppleReadCardNoFromTermPackage(ptermReqRec->paycard)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormSaleTermRequestDBRecFromPackage:: ptermReqRec is NULL!\n");
		return (ret);

	}

	/*
	 UppleReadTermFld(35,fld35,sizeof(fld35));
	 i=0;
	 while(fld35[i]!='D')
	 {
	 ptermReqRec->paycard[i]=fld35[i];
	 i++;
	 }
	 ptermReqRec->paycard[i]=0;

	 UppleReadTermFld(61,ptermReqRec->termorissn,sizeof(ptermReqRec->termorissn));

	 // 手机号
	 memset(tmpBuf,0,sizeof(tmpBuf));
	 if ((ret = UppleReadTermFld(44,tmpBuf,sizeof(tmpBuf))) <= 0)
	 return(0);

	 memset(number,0,sizeof(number));
	 memcpy(number,tmpBuf,1);
	 offset = 1;
	 for (i = 0; i < atoi(number); i++)
	 {
	 if (memcmp(tmpBuf+offset,"1",1) == 0)		// 付款方手机号
	 memcpy(ptermReqRec->pyrmobileno,tmpBuf+offset+1,11);
	 else if (memcmp(tmpBuf+offset,"2",1) == 0)	// 收款方手机号
	 memcpy(ptermReqRec->pyemobileno,tmpBuf+offset+1,11);
	 offset += 12;
	 }
	 */
	return (0);
}

int UppleFormTermPayeeRequestDBRecFromPackage(
		PUppleTermRequestDBRec ptermReqRec)
{
	int i;
	int ret;
	int offset = 0;
	char number[1 + 1];
	char tmpBuf[128 + 1];

	if (ptermReqRec == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayeeRequestDBRecFromPackage:: ptermReqRec is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptermReqRec, 0, sizeof(*ptermReqRec));

	UppleFormTermPayeePrimaryKeyFromPackage(&(ptermReqRec->primaryKey));

	UppleReadTermTPDU(ptermReqRec->termtpdu);

	memset(tmpBuf, 0, sizeof(tmpBuf));
	UppleReadTermFld(0, tmpBuf, 4 + 1);
	UppleReadTermFld(3, tmpBuf + 4, 6 + 1);
	strcpy(ptermReqRec->termtransid, tmpBuf);

	memset(tmpBuf, 0, sizeof(tmpBuf));
	UppleReadTermFld(62, tmpBuf, sizeof(tmpBuf));
	if (strlen(tmpBuf) >= 2)
		memcpy(ptermReqRec->termareacode, tmpBuf, 2);

	UppleReadTermFld(4, ptermReqRec->termamount,
			sizeof(ptermReqRec->termamount));
	UppleReadTermFld(61, ptermReqRec->termorissn,
			sizeof(ptermReqRec->termorissn));

	// 手机号
	memset(tmpBuf, 0, sizeof(tmpBuf));
	if ((ret = UppleReadTermFld(44, tmpBuf, sizeof(tmpBuf))) <= 0)
		return (0);

	memset(number, 0, sizeof(number));
	memcpy(number, tmpBuf, 1);
	offset = 1;
	for (i = 0; i < atoi(number); i++)
	{
		if (memcmp(tmpBuf + offset, "1", 1) == 0) // 付款方手机号
			memcpy(ptermReqRec->pyrmobileno, tmpBuf + offset + 1, 11);
		else if (memcmp(tmpBuf + offset, "2", 1) == 0) // 收款方手机号
			memcpy(ptermReqRec->pyemobileno, tmpBuf + offset + 1, 11);
		offset += 12;
	}
	return (0);
}

int UppleFormTermTransferRequestDBRecFromPackage(
		PUppleTransRequestDBRec ptransReqRec)
{
	int i;
	int ret;
	char accttype[1 + 1];
	char tmpBuf[1024 + 1];
	char fld062[64 + 1];

	if (ptransReqRec == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermTransferRequestDBRecFromPackage:: ptransReqRec is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptransReqRec, 0, sizeof(*ptransReqRec));

	// 20090910 增加
	UppleReadTermFld(48, ptransReqRec->adddata, sizeof(ptransReqRec->adddata));

	memset(tmpBuf, 0, sizeof(tmpBuf));
	UppleReadTermFld(48, tmpBuf, sizeof(tmpBuf));

	memset(accttype, 0, sizeof(accttype));
	memcpy(accttype, tmpBuf + 14, 1);

	memset(fld062, 0, sizeof(fld062));
	if ((ret = UppleReadTermFld(62, fld062, sizeof(fld062))) < 14)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermTransferRequestDBRecFromPackage:: UppleReadTermFld 62!\n");
		return (errCodeCurrentMDL_InvalidAccNoLength);
	}
	strcpy(ptransReqRec->acctno, fld062 + 2);

	if (strlen(tmpBuf) < 75)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"in UppleFormTermTransferRequestDBRecFromPackage:: term48[%s] is error!\n",
				tmpBuf);
		return (errCodeCurrentMDL_Term48FieldIsError);
	}

	memcpy(ptransReqRec->acctname, tmpBuf + 15, 60);
	UppleFilterHeadAndTailBlank(ptransReqRec->acctname);

	if (accttype[0] == '0') // 卡
	{
		if ((ret = UppleGetBankCodeByAcctnoFromCardBin(ptransReqRec->acctno,
				ptransReqRec->bankcode)) < 0)
		{
			UppleUserErrLog2(
					__FILE__,
					__LINE__,
					"in UppleFormTermTransferRequestDBRecFromPackage:: UppleGetBankCodeByAcctnoFromCardBin!\n");
			return (ret);
		}
	}
	else if (accttype[0] == '1') // 折
	{
		if (strlen(tmpBuf) < 87)
		{
			UppleUserErrLog2(
					__FILE__,
					__LINE__,
					"in UppleFormTermTransferRequestDBRecFromPackage:: len[%d] of term048 < 87 !\n",
					strlen(tmpBuf));
			return (errCodeCurrentMDL_Term48FieldIsError);
		}
		memcpy(ptransReqRec->bankcode, tmpBuf + 75, 12);
	}
	else
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"in UppleFormTermTransferRequestDBRecFromPackage:: accttype[%s] is error!\n",
				accttype);
		return (errCodeCurrentMDL_Term48FieldIsError);
	}
	UppleFilterHeadAndTailBlank(ptransReqRec->bankcode);
	if (strlen(ptransReqRec->bankcode) < 12)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"in UppleFormTermTransferRequestDBRecFromPackage:: bankcode[%s] is invalid !\n",
				ptransReqRec->bankcode);
		return (errCodeCurrentMDL_InvalidBankCode);
	}

	UppleReadTermFld(49, ptransReqRec->currtype, sizeof(ptransReqRec->currtype));

	UppleReadTermFld(4, ptransReqRec->amount, sizeof(ptransReqRec->amount));

	return (0);
}

int UppleFormTermRequestDBRecFromPackage(PUppleTransRequestDBRec ptransReqRec)
{
	int ret;
	int offset;
	long fee;
	char fld000[4 + 1];
	char fld003[6 + 1];
	char fld004[12 + 1];
	char fld048[1024 + 1];
	char fld062[128 + 1];
	char transType[10 + 1];
	char areaCode[2 + 1];
	char organCode[12 + 1];

	if (ptransReqRec == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermRequestDBRecFromPackage:: ptransReqRec is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptransReqRec, 0, sizeof(*ptransReqRec));

	UppleReadTermFld(48, ptransReqRec->adddata, sizeof(ptransReqRec->adddata));

	// 获取终端交易类型
	memset(fld000, 0, sizeof(fld000));
	UppleReadTermFld(0, fld000, sizeof(fld000));
	memset(fld003, 0, sizeof(fld003));
	UppleReadTermFld(3, fld003, sizeof(fld003));
	memset(transType, 0, sizeof(transType));
	sprintf(transType, "%s%s", fld000, fld003);

	if (memcmp(transType, "0200190000", 10) != 0) // 非缴费
		return (0);

	// 获取地区标识
	memset(fld062, 0, sizeof(fld062));
	UppleReadTermFld(62, fld062, sizeof(fld062));
	memset(areaCode, 0, sizeof(areaCode));
	memcpy(areaCode, fld062, 2);

	offset = 5;

	memset(fld048, 0, sizeof(fld048));
	UppleReadTermFld(48, fld048, sizeof(fld048));
	memset(organCode, 0, sizeof(organCode));
	memcpy(organCode, fld048 + offset, 12);
	UppleFilterHeadAndTailBlank(organCode);

	// 终端交易金额
	memset(fld004, 0, sizeof(fld004));
	UppleReadTermFld(4, fld004, sizeof(fld004));

	if ((fee = UppleGetTransFeeByTerminfo(transType, areaCode, organCode,
			organCode, atol(fld004))) <= 0)
		sprintf(ptransReqRec->addfield1, "%08ld", 0);
	else
		sprintf(ptransReqRec->addfield1, "%08ld", fee);

	return (0);
}

// 得到终端的路由标识
int UppleGetTermTransRouteID(char *routeid, int sizeOfBuf)
{
	int i;
	int ret;
	char cardno[19 + 1];
	char cardbin[8 + 1];
	char dbRecord[512 + 1];

	// 终端路由
	if ((ret = UppleReadDatabaseTableOfTerm()) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleGetTermTransRouteID:: UppleReadDatabaseTableOfTerm 3!\n");
		if (ret == errCodeCurrentMDL_8583PackageNotAcctno)
			return (errCodeCurrentMDL_TermIDNotDefined);
		else
			return (ret);
	}

	if ((ret = UppleReadFldFromTermByFldName("routeid", routeid, sizeOfBuf))
			< 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleGetTermTransRouteID:: UppleReadFldFromTermByFldName!\n");
		return (ret);
	}
	if (memcmp(routeid, "--", 2) != 0) // 终端不限制路由，继续
		return (0);

	// 读取卡号，无卡号，不限制路由
	memset(cardno, 0, sizeof(cardno));
	if ((ret = UppleReadCardNoFromTermPackage(cardno)) < 0)
	{
		if (ret == errCodeCurrentMDL_8583PackageNotAcctno)
		{
			strcpy(routeid, "--");
			return (0);
		}
		else
		{
			UppleUserErrLog2(__FILE__, __LINE__,
					"in UppleGetTermTransRouteID:: UppleReadCardNoFromTermPackage!\n");
			return (ret);
		}
	}

	if ((ret = UppleReadDatabaseTableOfCardBin(cardno, dbRecord,
			sizeof(dbRecord))) < 0)
	{
		UppleLog(
				"in UppleGetTermTransRouteID:: UppleReadDatabaseTableOfCardBin!\n",
				cardno);
		strcpy(routeid, "YL");
		return (0);
	}

	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord), "routeid",
			routeid, sizeOfBuf)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleGetTermTransRouteID:: fldName [%s] not find!\n",
				"routeid");
		return (ret);
	}
	return (0);
}

// 冲正交易付款方请求关键字
int UppleFormOriginTermPayerPrimaryKeyFromPackage(PUppleTermPrimaryKey ptermKey)
{
	int ret;
	char tmpBuf[128 + 1];

	if (ptermKey == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormOriginTermPayerPrimaryKeyFromPackage:: ptermKey is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptermKey, 0, sizeof(*ptermKey));

	if ((ret = UppleReadTermFld(61, ptermKey->termssn,
			sizeof(ptermKey->termssn))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormOriginTermPayerPrimaryKeyFromPackage:: UppleReadTermFld 61!\n");
		return (ret);
	}
	if (ret != 6)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"in UppleFormOriginTermPayerPrimaryKeyFromPackage:: fld length [%d] of 61 error!\n",
				ret);
		return (errCodeISO8583MDL_8583Fld61Length);
	}
	if ((ret = UppleReadTermFld(41, ptermKey->termno, sizeof(ptermKey->termno)))
			< 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormOriginTermPayerPrimaryKeyFromPackage:: UppleReadTermFld 41!\n");
		return (ret);
	}
	if ((ret = UppleReadTermFld(63, ptermKey->termbatchid,
			sizeof(ptermKey->termbatchid))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormOriginTermPayerPrimaryKeyFromPackage:: UppleReadTermFld 63!\n");
		return (ret);
	}
	sprintf(ptermKey->payflag, "%d", conPayFlagIsPayer);
	return (0);
}

// 冲正交易收款方款方请求关键字
int UppleFormOriginTermPayeePrimaryKeyFromPackage(PUppleTermPrimaryKey ptermKey)
{
	int ret;
	char tmpBuf[128 + 1];

	if (ptermKey == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormOriginTermPayeePrimaryKeyFromPackage:: ptermKey is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptermKey, 0, sizeof(*ptermKey));
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormOriginTermPayeePrimaryKeyFromPackage:: UppleReadTermFld 61!\n");
		return (ret);
	}
	if (ret != 6)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"in UppleFormOriginTermPayeePrimaryKeyFromPackage:: fld length [%d] of 61 error!\n",
				ret);
		return (errCodeISO8583MDL_8583Fld61Length);
	}
	if ((ret = UppleReadTermFld(41, ptermKey->termno, sizeof(ptermKey->termno)))
			< 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormOriginTermPayeePrimaryKeyFromPackage:: UppleReadTermFld 41!\n");
		return (ret);
	}
	if ((ret = UppleReadTermFld(63, ptermKey->termbatchid,
			sizeof(ptermKey->termbatchid))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormOriginTermPayeePrimaryKeyFromPackage:: UppleReadTermFld 63!\n");
		return (ret);
	}
	sprintf(ptermKey->payflag, "%d", conPayFlagIsPayee);
	return (0);
}

int FinishTermTransStatus()
{
	int ret;
	char fld000[4 + 1];
	char fld003[6 + 1];

	memset(fld000, 0, sizeof(fld000));
	if ((ret = UppleReadTermFld(0, fld000, sizeof(fld000))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]in InsertTermTransStatus:: UppleReadTermFld 0!\n",
				__FILE__, __LINE__);
		return (ret);
	}
	memset(fld003, 0, sizeof(fld003));
	if ((ret = UppleReadTermFld(3, fld003, sizeof(fld003))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]in InsertTermTransStatus:: UppleReadTermFld 3!\n",
				__FILE__, __LINE__);
		return (ret);
	}

	if (((memcmp(fld000, "0100", 4) == 0)
			&& ((memcmp(fld003, "950000", 6) == 0) || (memcmp(fld003, "951000",
					6) == 0) || (memcmp(fld003, "000000", 6) == 0) || (memcmp(
					fld003, "100000", 6) == 0) || (memcmp(fld003, "500000", 6)
					== 0) || (memcmp(fld003, "360000", 6) == 0)) || (memcmp(
			fld003, "320000", 6) == 0) || (memcmp(fld000, "0500", 4) == 0)
			|| (memcmp(fld003, "500001", 4) == 0) || (memcmp(fld003, "500002",
			4) == 0)))
	{
		return 0;
	}

	return UpdateTransStatus();

}

int InitTermTransStatus(char *package, int len)
{
	int ret;
	char fld000[4 + 1];
	char fld003[6 + 1];

	// 解包
	if ((ret = UppleUnpackTermPackage(package, len)) < 0)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"  [%s-%d]in UppleGetDealMDLIDOfPackageReceivedFromTerm:: UppleUnpackTermPackage [%d] [%s]!\n",
				__FILE__, __LINE__, len, package);
		return (ret);
	}
	memset(fld000, 0, sizeof(fld000));
	if ((ret = UppleReadTermFld(0, fld000, sizeof(fld000))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]in InsertTermTransStatus:: UppleReadTermFld 0!\n",
				__FILE__, __LINE__);
		return (ret);
	}
	memset(fld003, 0, sizeof(fld003));
	if ((ret = UppleReadTermFld(3, fld003, sizeof(fld003))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]in InsertTermTransStatus:: UppleReadTermFld 3!\n",
				__FILE__, __LINE__);
		return (ret);
	}

	/*
	 if (((memcmp(fld000,"0100",4) == 0)
	 &&(	(memcmp(fld003,"950000",6)==0)
	 ||(memcmp(fld003,"951000",6)==0)
	 ||(memcmp(fld003,"000000",6)==0 )
	 ||(memcmp(fld003,"100000",6)==0 )
	 ||(memcmp(fld003,"500000",6)==0 )
	 //||(memcmp(fld003,"360000",6)==0 )  //上POSP的需要记流水
	 )
	 ||(memcmp(fld003,"320000",6)==0)
	 || (memcmp(fld000,"0500",4)==0))){

	 return 0;
	 }
	 */

	UppleLog2(__FILE__, __LINE__, "登记流水开始......\n");
	ret = InsertTransStatus();
	//InsertICTransMessage();//ZWB 2013-12-18 IC域入库
	UppleLog2(__FILE__, __LINE__, "登记流水结束.\n");

	return ret;

}

int InitTermTransStatusTC(char *package, int len)
{
	int ret;
	char fld000[4 + 1];
	char fld003[6 + 1];

	/*
	 // 部分解包
	 if ((ret = UppleUnpackTermPackage(package,len)) < 0)
	 {
	 UppleUserErrLog2(__FILE__,__LINE__,"  [%s-%d]in UppleGetDealMDLIDOfPackageReceivedFromTerm:: UppleUnpackTermPackage [%d] [%s]!\n",__FILE__,__LINE__,len,package);
	 return(ret);
	 }

	 memset(fld000,0,sizeof(fld000));
	 if ((ret = UppleReadTermFld(0,fld000,sizeof(fld000))) < 0)
	 {
	 UppleUserErrLog2(__FILE__,__LINE__,"  [%s-%d]in InsertTermTransStatus:: UppleReadTermFld 0!\n",__FILE__,__LINE__);
	 return(ret);
	 }
	 memset(fld003,0,sizeof(fld003));
	 if ((ret = UppleReadTermFld(3,fld003,sizeof(fld003))) < 0)
	 {
	 UppleUserErrLog2(__FILE__,__LINE__,"  [%s-%d]in InsertTermTransStatus:: UppleReadTermFld 3!\n",__FILE__,__LINE__);
	 return(ret);
	 }

	 //判断是否透传包
	 if( isTCTrans() )
	 {
	 //记透传流水
	 InsertTransStatusTC();
	 return 1;
	 }
	 else
	 return 0;
	 */
	//判断是否透传包


	//是透传包的记流水
	InsertTransStatusTC();
	//InsertICTransMessage();//ZWB 2013-12-18 IC卡交易入库
	//不是透传包的不记流水

}

//add by honghui.cao

int UppleInitTransHeadInfo(PTUppleTransHeadInfo transheadinfo, char *pos_buf)
{
	if (NULL == transheadinfo)
	{
		UppleUserErrLog("in UppleInitTransHeadInfo:: transheadinfo is NULL!\n");
		return (errCodeParameter);
	}
	if (NULL == pos_buf)
	{
		UppleUserErrLog("in UppleInitTransHeadInfo:: pos_buf is NULL!\n");
		return (errCodeParameter);
	}
	memset(transheadinfo, 0, sizeof(TUppleTransHeadInfo));

	memcpy(transheadinfo->reserved, pos_buf, 8);//保留区
	memcpy(transheadinfo->msgtype, pos_buf + 8, 4); //消息类型
	memcpy(transheadinfo->msgstreamcode, pos_buf + 8 + 4, 24); //消息流水
	memcpy(transheadinfo->msgdatetime, pos_buf + 8 + 4 + 24, 14);//消费发出时间
	memcpy(transheadinfo->returncode, pos_buf + 8 + 4 + 24 + 14, 2); //返回编码
	memcpy(transheadinfo->errormsg, pos_buf + 8 + 4 + 24 + 14 + 2, 16); //错误返回信息
	memcpy(transheadinfo->transtype, pos_buf + 8 + 4 + 24 + 14 + 2 + 16, 4); //透传交易类型
	memcpy(transheadinfo->transcode, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4, 8);//业务代码
	memcpy(transheadinfo->ylmid, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8, 15);//银联商户号
	memcpy(transheadinfo->yltid, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8
			+ 15, 8);//银联终端号
/*询问终端，银联商户终端号是汇卡的，文档汇卡是客户端自动生成的号，所以不用处理--zwb at 2014-5-16*/
#if 1 
	memcpy(transheadinfo->hicardmid, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8
			+ 8 + 15, 15);//汇卡商户号
	memcpy(transheadinfo->hicardtid, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8
			+ 8 + 15 + 15, 8); //汇卡终端号
#else
	memcpy(transheadinfo->hicardmid, transheadinfo->ylmid, 15);//汇卡商户号
	memcpy(transheadinfo->hicardtid, transheadinfo->yltid, 8); //汇卡终端号
#endif
	memcpy(transheadinfo->cardno, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8
			+ 8 + 15 + 15 + 8, 20); //卡号
	UppleFilterHeadAndTailBlank(transheadinfo->cardno);
	memcpy(transheadinfo->hicardorderid, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4
			+ 8 + 8 + 15 + 15 + 8 + 20, 20); //汇卡订单号
	memcpy(transheadinfo->operator, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8
			+ 8 + 15 + 15 + 8 + 20 + 20, 30); //操作员

	UppleLog2(
			__FILE__,
			__LINE__,
			"消息类型：%s\n 消息流水：%s\n 消费发出时间：%s \n 返回编码：%s \n "
				"透传交易类型：%s \n 业务代码：%s \n 银联终端号：%s \n 银联商户号：%s \n 汇卡终端号：%s\n 汇卡商户号：%s\n 卡号：%s \n 汇卡订单号：%s \n 操作员：%s \n ",
			transheadinfo->msgtype, transheadinfo->msgstreamcode,
			transheadinfo->msgdatetime, transheadinfo->returncode,
			transheadinfo->transtype, transheadinfo->transcode,
			transheadinfo->yltid, transheadinfo->ylmid,
			transheadinfo->hicardtid, transheadinfo->hicardmid,
			transheadinfo->cardno, transheadinfo->hicardorderid,
			transheadinfo->operator);

	return 0;
}

int UppleJudgeEffectiveTrans(const PTUppleTransHeadInfo transheadinfo)
{
	if(0 != memcmp(transheadinfo->ylmid,transheadinfo->hicardmid,15))
	{
		UppleLog2(__FILE__,__LINE__,"in UppleJudgeEffectiveTrans:: ylmid != hicardmid\n");
		return -1;
	}
#if 0
	if(0 != memcmp(transheadinfo->yltid,transheadinfo->hicardtid,8))
        {
                UppleLog2(__FILE__,__LINE__,"in UppleJudgeEffectiveTrans:: yltid != hicardtid\n");
                return -1;
        }
#endif
	return 0;

} 

int UpplePackRepeatPrintHead(const PTUppleTransHeadInfo transheadinfo,
		char tempBuf[])
{
	char pos_buf[2];
	unsigned char lenStr[2 + 1];
	int len = 0;
	int ret = 0;
	unsigned char pr_buf[2048];
	char errCode[2 + 1];
	UppleLog2(__FILE__, __LINE__, "UpplePackRepeatPrintHead 组装重打印返回报文头\n");
	//保留区 8
	memset(tempBuf, 0X00, 8);
	len += 8;
	//消息类型4
	tempBuf[len] = '0';
	tempBuf[len + 1] = '0';
	tempBuf[len + 2] = '2';
	tempBuf[len + 3] = '5';
	len += 4;
	//消息流水24
	//memcpy(tempBuf + len, pos_buf + 8 + 4, 24);
	memcpy(tempBuf + len, transheadinfo->msgstreamcode, 24);
	len += 24;
	//消费发出时间14
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24, 14);
	memcpy(tempBuf + len, transheadinfo->msgdatetime, 14);
	len += 14;
	//返回编码2   默认00
	*(tempBuf + 8 + 4 + 24 + 14) = '0';
	*(tempBuf + 8 + 4 + 24 + 14 + 1) = '0';
	len += 2;
	//错误返回信息16
	//memcpy(tempBuf + len, "0000000000000000", 16);
	len += 16;
	// 透传交易类型4
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24 + 14 + 2 + 16, 4);
	memcpy(tempBuf + len, transheadinfo->transtype, 4);
	len += 4;
	//业务代码8
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4, 8);
	memcpy(tempBuf + len, transheadinfo->transcode, 8);
	len += 8;
	//汇卡商户号15
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8 + 8 + 15,
	//			15);
	memcpy(tempBuf + len, transheadinfo->hicardmid, 15);
	len += 15;
	//汇卡终端号8
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8 + 8 + 15
	//			+ 15, 8);
	memcpy(tempBuf + len, transheadinfo->hicardtid, 8);
	len += 8;
	//订单号20
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8 + 8 + 15
	//			+ 15 + 8, 20);
	memcpy(tempBuf + len, transheadinfo->hicardorderid, 20);
	len += 20;
	//交易返回码3
	memcpy(tempBuf + len, "000", 3);
	len += 3;

	UppleLog2(__FILE__, __LINE__, "test##########报文头长度：%d\n", len);

	memset(pr_buf, 0, sizeof(pr_buf));
	if ((ret = GetPrintMessage(transheadinfo, pr_buf)) < 0)
	{
		UppleLog2(__FILE__, __LINE__, "error in function GetPrintMessage");
		ret = 0;

		UppleLog2(__FILE__, __LINE__,
				"##################无打印头体################ \n");
		//凭证头长度1
		tempBuf[len] = 0;
		len += 1;
		//凭证体长度2
		tempBuf[len] = 0;
		tempBuf[len + 1] = 0;
		len += 2;
		//凭证签名域长度2
		tempBuf[len] = 0;
		tempBuf[len + 1] = 0;
		len += 2;
		//凭证页脚长度2
		tempBuf[len] = 0;
		tempBuf[len + 1] = 0;
		len += 2;

		//返回编码
		*(tempBuf + 8 + 4 + 24 + 14) = '2'; //返回错误编码25
		*(tempBuf + 8 + 4 + 24 + 14 + 1) = '5';

		strcpy(tempBuf + 52, "无打印记录"); //返回错误信息

	}
	else
	{
		memcpy(tempBuf + len, pr_buf, ret);
		len += ret;
	}
	memset(errCode, 0, sizeof(errCode));
	UppleGet2(RPUB, "PERRCODE", errCode);
	if (memcmp(errCode, "00", 2) != 0)
	{
		UppleLog2(__FILE__, __LINE__, "存在错误信息，请返回错误信息码！ errCode=[%s] \n",
				errCode);
	}
	UppleLog2(__FILE__, __LINE__, "返回信息码！ errCode=[%s] \n", errCode);

	UppleLog2(__FILE__, __LINE__,
			"UpplePackRepeatPrintHead 组装返回报文头长度：%d\n", len);
	return len;
}

int UpplePackReturnTransparentHead(const PTUppleTransHeadInfo transheadinfo,
		const char buf[], char tempBuf[])
{
	UppleLog2(__FILE__, __LINE__, "UpplePackReturnTransparentHead 组装返回报文头\n");
	unsigned char lenStr[2 + 1];
	int len = 0;
	int ret = 0;
	unsigned char pr_buf[1024];
	char b39[2 + 1];
	char errCode[2 + 1];
	char errormsg[16 + 1];

	//保留区 8
	memset(tempBuf, 0X00, 8);
	len += 8;
	//消息类型4
	tempBuf[len] = '0';
	tempBuf[len + 1] = '0';
	tempBuf[len + 2] = '2';
	tempBuf[len + 3] = '5';
	len += 4;
	//消息流水24
	//memcpy(tempBuf + len, pos_buf + 8 + 4, 24);
	memcpy(tempBuf + len, transheadinfo->msgstreamcode, 24);
	len += 24;
	//消费发出时间14
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24, 14);
	memcpy(tempBuf + len, transheadinfo->msgdatetime, 14);
	len += 14;
	//返回编码2
	*(tempBuf + 8 + 4 + 24 + 14) = '0';
	*(tempBuf + 8 + 4 + 24 + 14 + 1) = '0';
	len += 2;
	//错误返回信息16
	memcpy(tempBuf + len, "0000000000000000", 16);
	len += 16;
	// 透传交易类型4
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24 + 14 + 2 + 16, 4);
	memcpy(tempBuf + len, transheadinfo->transtype, 4);
	len += 4;
	//业务代码8
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4, 8);
	memcpy(tempBuf + len, transheadinfo->transcode, 8);
	len += 8;
	//汇卡商户号15
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8 + 8 + 15,
	//			15);
	memcpy(tempBuf + len, transheadinfo->hicardmid, 15);
	len += 15;
	//汇卡终端号8
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8 + 8 + 15
	//			+ 15, 8);
	memcpy(tempBuf + len, transheadinfo->hicardtid, 8);
	len += 8;
	//订单号20
	//	memcpy(tempBuf + len, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8 + 8 + 15
	//			+ 15 + 8, 20);
	memcpy(tempBuf + len, transheadinfo->hicardorderid, 20);
	len += 20;
	//交易返回码3
	memset(b39, 0x00, sizeof(b39));
	UppleReadYLFld(39, b39, sizeof(b39));
	if(0 == strlen(b39)){
		UppleGet2(RPUB,"PB39",b39);
	}
	memcpy(tempBuf + len, b39, 2);
	len += 3;

	memset(pr_buf, 0, sizeof(pr_buf));
	if ((ret = UpplePackPrintMessage(transheadinfo, pr_buf)) < 0)
	{
		UppleLog2(__FILE__, __LINE__,
				"error in function UpplePackPrintMessage \n");
		return ret;
	}
	UppleLog2(__FILE__, __LINE__, "打印消息体长度为：%d \n", ret);

	memcpy(tempBuf + len, pr_buf, ret);
	len += ret;

	memset(errCode, 0, sizeof(errCode));
	memset(errormsg, 0, sizeof(errormsg));
	UppleGet2(RPUB, "PERRCODE", errCode);
	if (memcmp(errCode, "00", 2) != 0 && strlen(errCode) > 0)
	{
		UppleGetMappingErrorMes(errCode, errormsg);
		memcpy(tempBuf + 8 + 4 + 24 + 14, errCode, 2);
	}else{
		hc_transform_errcode2( errormsg, b39, 0);
		memcpy(tempBuf + 8 + 4 + 24 + 14, b39, 2);
	}
	memcpy(tempBuf + 8 + 4 + 24 + 14 + 2, errormsg, 16);
	UppleLog2(__FILE__, __LINE__,
				"存在错误信息，请返回错误信息码！ errorcode=[%s] errormsg=[%s]\n", errCode,
				errormsg);
	UppleLog2(__FILE__, __LINE__, "返回信息码！ errCode=[%s] \n", errCode);

	UppleLog2(__FILE__, __LINE__,
			"UpplePackReturnTransparentHead 组装返回报文头长度：%d\n", len);
	return len;
}

void UppleGetMappingErrorMes(char *resp_code, char *resp_err_msg)
{
	int MAX_ERR_NUM = 50;
	int i = 0;
	char errMiror[][100] =
	{ "P1", "无效终端", // 请求终端不合法
			"P2", "无效商户", // 请求商户号不合法
			"P4", "银联忙,请重试", // 自身故障,或者和上级银行节点之间通讯出现故障或者失败
			"P5", "订单号为0", // 交易记录插入,更新等等操作失败
			"P6", "交易记录失败", // 交易记录插入,更新等等操作失败
			"P8", "订单查询失败", // 交易记录插入,更新等等操作失败
			"P9", "订单号重复", "PA", "校验计算失败", // 校验计算出现问题
			"A0", "MAC校验错,请重试",// MAC校验出现问题
			"Q1", "金额超限", "Q2", "此卡限制交易", "Q3", "终端检查失败",
			"05", "证件信息验证错误",
			"E0","商户号不匹配",
			"57","证件验证次数超限",
			"V4","证件验证次数超限",
			"V1","需要做身份认证",
			"V2","超出贷记卡数限制",
			"V5","借记卡张数受限",
			"V6","贷记卡张数受限",
			"V7","超出银行卡数限制",
			"V3","超出借记卡数限制" };

	for (i = 0; i < MAX_ERR_NUM; i += 2)
	{
		if (errMiror[i])
			if (!memcmp(resp_code, errMiror[i], strlen(errMiror[i]))) //Matched
			{
				strcpy(resp_err_msg, errMiror[i + 1]);
				return;
			}
	}
	//No Matched
	strcpy(resp_err_msg, "其它错误!");
	return;
}


int GetPrintMessage(const PTUppleTransHeadInfo transheadinfo, char*pr_buf)
{
	unsigned char mid[15 + 1];
	unsigned char tid[8 + 1];
	char where[1024 + 1];
	char dbRecord[1024 + 1];
	char flds[100];
	char orderno[20 + 1];
	int ret = 0;
	int len = 0;

	unsigned char sptitle[1024];
	unsigned char spbody[1024];
	unsigned char spsign[1024];
	unsigned char spfooter[1024];
	char printdate[20 + 1];

	memset(sptitle, 0, sizeof(sptitle));
	memset(spbody, 0, sizeof(spbody));
	memset(spsign, 0, sizeof(spsign));
	memset(spfooter, 0, sizeof(spfooter));
	memset(printdate, 0, sizeof(printdate));

	memset(tid, 0, sizeof(tid));
	memset(mid, 0, sizeof(mid));
	memset(orderno, 0, sizeof(orderno));

	//	memcpy(mid, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8 + 8 + 15, 15); //汇卡商户号
	//	memcpy(tid, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8 + 8 + 15 + 15, 8); //汇卡终端
	//	memcpy(orderno, pos_buf + 8 + 4 + 24 + 14 + 2 + 16 + 4 + 8 + 8 + 15 + 15
	//			+ 8, 20); //取得汇卡订单号

	memcpy(mid, transheadinfo->hicardmid, 15);//汇卡商户号
	memcpy(tid, transheadinfo->hicardtid, 8);//汇卡终端
	memcpy(orderno, transheadinfo->hicardorderid, 20);//取得汇卡订单号


#if 1
        char    ssn[64]={0};
        char    b3_pcode[64]={0};

        if((ret = UppelGetOrgTransSSN(mid,tid,orderno,ssn,b3_pcode)) < 0)
        {
                UppleLog2(__FILE__,__LINE__,"in GetPrintMessage::未找打原始消费记录\n");
                return (ret);
        }
        /*返回 1 是在流水表找到 原消费记录，返回 2 是在历史表找到 原交易记录*/
        if(ret == 1)
        {
                if((ret = UppleGetRevdbRecord(mid,tid,ssn,b3_pcode)) < 0)
                {
                        UppleLog2(__FILE__,__LINE__,"in GetPrintMessage::已发生了冲正，禁止重打印\n");
                        return (ret);
                }
        }
#endif

	memset(where, 0, sizeof(where));
	sprintf(
			where,
			" TID = '%s' and MID = '%s' and ORDERNO = '%s' order by PRINTTIME desc limit 1 ",
			tid, mid, orderno);

	memset(dbRecord, 0, sizeof(dbRecord));
	if ((ret = UppleSelectRealDBRecord("T_PRINT_DATA",
			" SPTITLE, SPBODY,SPSIGN,SPFOOTER ", where, dbRecord,
			sizeof(dbRecord), NULL)) < 0)
	{
		if (ret == errCodeDatabaseMDL_RecordNotFound)
		{
			UppleLog("  [%s-%d]:: condition[%s]!\n", __FILE__, __LINE__, where);
			return (ret);
		}
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]:: UppleSelectRealDBRecord!\n", __FILE__, __LINE__);
		return (ret);
	}

	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord), "SPTITLE",
			sptitle, sizeof(sptitle))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]in :: fldName [SPTITLE] not find!\n", __FILE__,
				__LINE__);
		return (ret);
	}
	UppleLog2(__FILE__, __LINE__, "sptitle in table=[%s]\n", sptitle);

	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord), "SPBODY",
			spbody, sizeof(spbody))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]in :: fldName [SPBODY] not find!\n", __FILE__,
				__LINE__);
		return (ret);
	}
	UppleLog2(__FILE__, __LINE__, "spbody in table=[%s]\n", spbody);

	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord), "SPSIGN",
			spsign, sizeof(spsign))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]in :: fldName [SPSIGN] not find!\n", __FILE__,
				__LINE__);
		return (ret);
	}
	UppleLog2(__FILE__, __LINE__, "SPSIGN in table=[%s] \n", spsign);

	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
			"SPFOOTER", spfooter, sizeof(spfooter))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]in :: fldName [SPFOOTER] not find!\n", __FILE__,
				__LINE__);
		return (ret);
	}
	UppleLog2(__FILE__, __LINE__, "spfooter in table=[%s]\n", spfooter);

	//凭证头长度1
	pr_buf[len] = strlen(sptitle);
	len += 1;
	//凭证头
	memcpy(pr_buf + len, sptitle, strlen(sptitle));
	len += strlen(sptitle);
	//添加备注信息
	if ((ret = replace(spbody, "UPREFERENCE", "重打印") < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPREFERENCE为：%s 失败", "重打印");
	}
	//更新打印时间
	gettime(printdate);
	UppleLog2(__FILE__, __LINE__, "打印时间：printdate: %s\n", printdate);

	if ((ret = replace(spbody, "PRINTDATE", printdate) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符PRINTDATE为：%s 失败", printdate);
	}

	//凭证体长度2
	pr_buf[len] = strlen(spbody) % 256;
	pr_buf[len + 1] = strlen(spbody) / 256;
	len += 2;
	//凭证体
	memcpy(pr_buf + len, spbody, strlen(spbody));
	len += strlen(spbody);

	UppleLog2(__FILE__, __LINE__, "凭证体长度：%d\n", strlen(spbody));
	UppleMemLog2(__FILE__, __LINE__, "凭证体信息：", spbody, strlen(spbody));

	//凭证签名域长度2
	pr_buf[len] = strlen(spsign) % 256;
	pr_buf[len + 1] = strlen(spsign) / 256;
	len += 2;
	//凭证签名域
	memcpy(pr_buf + len, spsign, strlen(spsign));
	len += strlen(spsign);

	UppleLog2(__FILE__, __LINE__, "签名域长度：%d\n", strlen(spsign));
	UppleMemLog2(__FILE__, __LINE__, "签名域信息：", spsign, strlen(spsign));

	//凭证页脚长度2
	pr_buf[len] = strlen(spfooter) % 256;
	pr_buf[len + 1] = strlen(spfooter) / 256;
	len += 2;
	//凭证页脚
	memcpy(pr_buf + len, spfooter, strlen(spfooter));
	len += strlen(spfooter);

	UppleLog2(__FILE__, __LINE__, "凭证页脚长度：%d\n", strlen(spfooter));
	UppleMemLog2(__FILE__, __LINE__, "凭证页脚信息：", spfooter, strlen(spfooter));

	UppleLog2(__FILE__, __LINE__, "凭证页脚信息：%s \n", spfooter);

	*(pr_buf + len) = '\0';

	memset(flds, 0, sizeof(flds));
	sprintf(flds, "REPRINTTIME=SYSDATE()");
	if ((ret = UppleUpdateRealDBRecord("T_PRINT_DATA", flds, where)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__, "重打印时间更新失败！\n");
		//return (ret);
	}

	/*********************************/
        UpdateRePrintStatu(mid,tid,orderno,1);
        /*********************************/

	return len;
}

int replace(char*src, const char*old_s, const char*new_s)
{
	char buf[1024];
	char* index;
	int len = 0;
	int slen1 = 0;
	int slen2 = 0;
	int slen3 = 0;
	if (src == NULL || old_s == NULL || (*old_s == 0) || new_s == NULL)
		return -1;
	if ((index = find_index(src, old_s)))
	{
		slen1 = strlen(src);
		slen2 = strlen(old_s);
		slen3 = strlen(new_s);

		memset(buf, 0, sizeof(buf));
		len = index - src;
		memcpy(buf, src, len - 1);
		memcpy(buf + len - 1, new_s, slen3);
		memcpy(buf + len + slen3 - 1, src + len + 1 + slen2, slen1 - slen2
				- len - 1);

		memset(src, 0, slen1);
		//memcpy(src,buf,strlen(buf));
		memcpy(src, buf, slen3 + slen1 - slen2 - 1);
		return 0;
	}
	else
		return 0;

}

int gettime(char * s)
{
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep); /*取得当地时间*/

	sprintf(s, "%d-%02d-%02d %02d:%02d:%02d", (1900 + p->tm_year), (p->tm_mon
			+ 1), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	return 0;
}

int changeTimeFormat(char*old, char*new)
{
	char mm[2 + 1]; //月份
	char dd[2 + 1]; //月号
	char hh[2 + 1]; //小时
	char mi[2 + 1]; //分钟
	char ss[2 + 1]; //秒
	time_t timep;
	struct tm *p;

	memset(mm, 0, sizeof(mm));
	memset(dd, 0, sizeof(dd));
	memset(hh, 0, sizeof(hh));
	memset(mi, 0, sizeof(mi));
	memset(ss, 0, sizeof(ss));

	memcpy(mm, old, 2);
	memcpy(dd, old + 2, 2);
	memcpy(hh, old + 2 + 2, 2);
	memcpy(mi, old + 2 + 2 + 2, 2);
	memcpy(ss, old + 2 + 2 + 2 + 2, 2);

	time(&timep);
	p = localtime(&timep); /*取得当地时间*/

	sprintf(new, "%d-%s-%s %s:%s:%s", (1900 + p->tm_year), mm, dd, hh, mi, ss);
	return 0;
}



int UpplePackPrintMessage(const PTUppleTransHeadInfo transheadinfo, char*pr_buf)
{
	int len = 0;
	char tempBuf[1024 + 1];

	char mti[4 + 1];
	char tid[8 + 1];
	char mid[15 + 1];
	char yltid[8 + 1];
	char ylmid[15 + 1];
	char b3[6 + 1];
	char b4[12 + 1];
	char b11[6 + 1];
	char b39[2 + 1];

	unsigned char sptitle[200];
	unsigned char spbody[2048+1];
	unsigned char spbodybackup[2048+1];
	unsigned char spsign[200];
	unsigned char spfooter[200];
	unsigned char modename[30 + 1];
	int ret;
	
	char    extra_date[1024]={0};
        UppleGet2(RPUB, "PB65",extra_date);
        UppleLog2(__FILE__,__LINE__,"in UpplePackPrintMessage::extra_date[%s]\n",extra_date);

	memset(mti, 0x00, sizeof(mti));
	memset(tid, 0x00, sizeof(tid));
	memset(mid, 0x00, sizeof(mid));
	memset(yltid, 0x00, sizeof(yltid));
	memset(ylmid, 0x00, sizeof(ylmid));

	memcpy(mid, transheadinfo->hicardmid, 15);//汇卡商户号
	memcpy(tid, transheadinfo->hicardtid, 8); //汇卡终端

	UppleReadYLFld(0, mti, sizeof(mti));
	UppleReadYLFld(41, yltid, sizeof(yltid));
	UppleReadYLFld(42, ylmid, sizeof(ylmid));

	memset(b3, 0x00, sizeof(b3));
	UppleReadYLFld(3, b3, sizeof(b3));
	memset(b4, 0x00, sizeof(b4));
	UppleReadYLFld(4, b4, sizeof(b4));
	memset(b11, 0x00, sizeof(b11));
	UppleReadYLFld(11, b11, sizeof(b11));
	memset(b39, 0x00, sizeof(b39));
	UppleReadYLFld(39, b39, sizeof(b39));

	memset(tempBuf, 0, sizeof(tempBuf));
	memset(sptitle, 0, sizeof(sptitle));
	memset(spbody, 0, sizeof(spbody));
	memset(spbodybackup, 0, sizeof(spbodybackup));
	memset(spsign, 0, sizeof(spsign));
	memset(spfooter, 0, sizeof(spfooter));
	memset(modename, 0, sizeof(modename));

	UppleLog2(__FILE__, __LINE__, "################## b39:%s \n", b39);

	#if 1
	char    Channel[128]={0};
	UppleGet2(RPUB,"#Channel",Channel);
	UppleLog2(__FILE__,__LINE__,"###打印签购单通过渠道号找模板Channel[%s]\n",Channel);
	#if 0 /*测试*/
		memset(Channel,0,sizeof(Channel));
		memcpy(extra_date,"Y",1);
	#endif
	if(strlen(Channel) <= 0)
	{
		//TransPushOrderRead();
		ReadChannelRecordByTermMsg();
		UppleGet2(RPUB,"#Channel",Channel);
        	UppleLog2(__FILE__,__LINE__,"###打印签购单通过渠道号找模板Channel2[%s]\n",Channel);
	}
        #endif
	/*消费成功才打印票头*/
	if ((atol(b4) > 0) && (memcmp(b39, "00", 2) == 0) && ((memcmp(mti, "0210",
			4) == 0) || (memcmp(mti, "0230", 4) == 0)))////消费成功才打印票头
	{
		char where[1024 + 1];
		char dbRecord[1024 + 1];
		char orderno[20 + 1];

		memset(orderno, 0, sizeof(orderno));
		memcpy(orderno, transheadinfo->hicardorderid, 20);/*取得订单号*/
		
		if((extra_date[0] == 'Y') || (extra_date[0] == 'K') || (extra_date[0] == 'F') || (extra_date[0] == 'C') || (extra_date[0] == 'S') )
                {
                        char    tmp[8]={0};
                        memcpy(tmp,extra_date,1);
			if((extra_date[0] == 'Y')&&(6002 == atoi(Channel)))
			{
				char conditions[20]={0};
				memcpy(conditions,Channel,4);
				memcpy(conditions+4,extra_date,1);
				sprintf(
                                        where,
                                                " TID = '%s' and MID = '%s' and MTI = '%s'  and TRANSTYPE = '%s' order by VERSION desc limit 1 ",
                                                        tid, mid, "0000",conditions);
			}
                        else
			{
				sprintf(
                                	where,
                                		" TID = '%s' and MID = '%s' and MTI = '%s'  and TRANSTYPE = '%s' order by VERSION desc limit 1 ",
                                			tid, mid, "0000",tmp);
			}

                        memset(dbRecord, 0, sizeof(dbRecord));
                        if ((ret = UppleSelectRealDBRecord("T_PRINT_PATTEN",
                                        " SPTITLE, SPBODY,SPSIGN,SPFOOTER,MODENAME ", where, dbRecord,
                                                sizeof(dbRecord), NULL)) < 0)
                        {
                                /*找默认打印模板*/
                                if (ret == errCodeDatabaseMDL_RecordNotFound)
                                {
                                        memset(where, 0, sizeof(where));
                                        memset(dbRecord, 0, sizeof(dbRecord));
					if((extra_date[0] == 'Y')&&(6002 == atoi(Channel)))
                        		{
                                		char conditions[20]={0};
                                		memcpy(conditions,Channel,4);
                                		memcpy(conditions+4,extra_date,1);
						sprintf(
                                                        where," TID = '99999999' and MID = '999999999999999' and MTI = '%s' and TRANSTYPE = '%s' \
                                                                order by VERSION desc limit 1 ","0000",conditions);
                        		}
					else
					{
                                        	sprintf(
                                                	where," TID = '99999999' and MID = '999999999999999' and MTI = '%s' and TRANSTYPE = '%s' \
								order by VERSION desc limit 1 ","0000",tmp);
					}

                                        if ((ret = UppleSelectRealDBRecord("T_PRINT_PATTEN",
                                                " SPTITLE, SPBODY,SPSIGN,SPFOOTER,MODENAME ", where,
                                                dbRecord, sizeof(dbRecord), NULL)) < 0)
                                        {
                                                UppleLog2(__FILE__, __LINE__, "找不到默认打印模板！！！\n");
                                                return ret;
                                        }
                                        UppleLog("  [%s-%d]:: condition[%s]!\n", __FILE__, __LINE__,
                                                        where);
                                }
                                else
                                {
                                        UppleUserErrLog2(__FILE__, __LINE__,
                                                        "  [%s-%d]:: UppleSelectRealDBRecord!\n", __FILE__,
                                                                __LINE__);
                                        return (ret);
                                }
                        }
                        /*at 2014-9-9 zwb add*/
                        parsing_add_data_to_Redis();
                        /*************************/
                }
                else
                {
		#if 0
			char	Channel[128]={0};
			UppleGet2(RPUB,"#Channel",Channel);
			UppleLog2(__FILE__,__LINE__,"###打印签购单通过渠道号找模板Channel[%s]\n",Channel);
		#endif

			if((6001 == atoi(Channel)) || (6002 == atoi(Channel)) || (6003 == atoi(Channel)) || (8001 == atoi(Channel)))
			{
				UppleLog2(__FILE__,__LINE__,"##本代本业务找对应模板\n");
				sprintf(where," TID = '%s' and MID = '%s' and MTI = '%s' and TRANSTYPE = '%s' order by VERSION desc limit 1 ",
                                        tid, mid, mti,Channel);
			}
			else
			{
				sprintf(where," TID = '%s' and MID = '%s' and MTI = '%s' order by VERSION desc limit 1 ",
					tid, mid, mti);
			}

			memset(dbRecord, 0, sizeof(dbRecord));
			if ((ret = UppleSelectRealDBRecord("T_PRINT_PATTEN",
					" SPTITLE, SPBODY,SPSIGN,SPFOOTER,MODENAME ", where, dbRecord,
						sizeof(dbRecord), NULL)) < 0)
			{
				/*找默认打印模板*/
				if (ret == errCodeDatabaseMDL_RecordNotFound)
				{
					memset(where, 0, sizeof(where));
					memset(dbRecord, 0, sizeof(dbRecord));
		
					if((6001 == atoi(Channel)) || (6002 == atoi(Channel)) || (6003 == atoi(Channel)) || (8001 == atoi(Channel)))
					{
						UppleLog2(__FILE__,__LINE__,"##本代本业务找通用模板\n");
						sprintf(where," TID = '00000000' and MID = '000000000000000' and TRANSTYPE = '%s' order by VERSION desc limit 1 ",\
								Channel);
					}
					else
					{
						sprintf(
							where,
								" TID = '00000000' and MID = '000000000000000' order by VERSION desc limit 1 ",
									tid, mid);
					}

					if ((ret = UppleSelectRealDBRecord("T_PRINT_PATTEN",
							" SPTITLE, SPBODY,SPSIGN,SPFOOTER,MODENAME ", where,
								dbRecord, sizeof(dbRecord), NULL)) < 0)
					{
						UppleLog2(__FILE__, __LINE__, "找不到默认打印模板！！！\n");
						return ret;
					}
					UppleLog("  [%s-%d]:: condition[%s]!\n", __FILE__, __LINE__,
							where);
				}
				else
				{
					UppleUserErrLog2(__FILE__, __LINE__,
						"  [%s-%d]:: UppleSelectRealDBRecord!\n", __FILE__,
							__LINE__);
					return (ret);
				}
			}
		}

		if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
				"SPTITLE", sptitle, sizeof(sptitle))) < 0)
		{
			UppleUserErrLog2(__FILE__, __LINE__,
					"  [%s-%d]in :: fldName [SPTITLE] not find!\n", __FILE__,
					__LINE__);
			return (ret);
		}
		UppleLog2(__FILE__, __LINE__, "sptitle in table=[%s]\n", sptitle);

		if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
				"SPBODY", spbody, sizeof(spbody))) < 0)
		{
			UppleUserErrLog2(__FILE__, __LINE__,
					"  [%s-%d]in :: fldName [SPBODY] not find!\n", __FILE__,
					__LINE__);
			return (ret);
		}
		UppleLog2(__FILE__, __LINE__, "spbody in table=[%s]\n", spbody);

		if((extra_date[0] == 'Y') || (extra_date[0] == 'K') || (extra_date[0] == 'F') || (extra_date[0] == 'C'))
		{
			/*add print mode*/
        		char    tmp[1024]={0};
        		memset(tmp,0,sizeof(tmp));
        		UppleGet2(RPUB,"#CollectionAccount",tmp);
        		if(strlen(tmp) > 0)
        		{
                		if ((ret = replace(spbody, "CollectionAccount", tmp) < 0))
                		{
                        		UppleLog2(__FILE__, __LINE__, "替换字符为CollectionAccount：%s 失败", tmp);
                		}
        		}

        		memset(tmp,0,sizeof(tmp));
        		UppleGet2(RPUB,"#PaymentAccount",tmp);
        		if(strlen(tmp) > 0)
        		{
                		if ((ret = replace(spbody, "PaymentAccount", tmp) < 0))
                		{
                        		UppleLog2(__FILE__, __LINE__, "替换字符为PaymentAccount：%s 失败", tmp);
                		}
        		}

        		memset(tmp,0,sizeof(tmp));
        		UppleGet2(RPUB,"#CollectionPeo",tmp);
        		UppleLog2(__FILE__,__LINE__,"##TEST,CollectionPeo=[%s]\n",tmp);
        		if(strlen(tmp) > 0)
        		{
                		if ((ret = replace(spbody, "CollectionPeo", tmp) < 0))
                		{
                        		UppleLog2(__FILE__, __LINE__, "替换字符为CollectionPeo：%s 失败", tmp);
                		}
        		}
			
			memset(tmp,0,sizeof(tmp));
                        UppleGet2(RPUB,"#Upstream_mid",tmp);
                        UppleLog2(__FILE__,__LINE__,"##TEST,Upstream_mid=[%s]\n",tmp);
                        if(strlen(tmp) > 0)
                        {
                                if ((ret = replace(spbody, "Upstream_mid", tmp) < 0))
                                {
                                        UppleLog2(__FILE__, __LINE__, "替换字符为Upstream_mid：%s 失败", tmp);
                                }
                        }

                        memset(tmp,0,sizeof(tmp));
                        UppleGet2(RPUB,"#Upstream_name",tmp);
                        UppleLog2(__FILE__,__LINE__,"##TEST,Upstream_name=[%s]\n",tmp);
                        if(strlen(tmp) > 0)
                        {
                                if ((ret = replace(spbody, "Upstream_name", tmp) < 0))
                                {
                                        UppleLog2(__FILE__, __LINE__, "替换字符为Upstream_name：%s 失败", tmp);
                                }
                        }

        		/**********add by zwb at 2014-9-9 end *******************/
		}

		/*增加汇卡ID，汇卡终端号*/
        	if ((ret = replace(spbody, "HKMERNNO",transheadinfo->hicardmid) < 0))
        	{
                	UppleLog2(__FILE__, __LINE__, "替换字符HKMERNNO为：%s 失败",transheadinfo->hicardmid);
        	}
        	if ((ret = replace(spbody, "HKTERMNO",transheadinfo->hicardtid) < 0))
        	{
                	UppleLog2(__FILE__, __LINE__, "替换字符HKTERMNO为：%s 失败", transheadinfo->hicardtid);
        	}
        	/*****************add end******************/

		if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
				"SPSIGN", spsign, sizeof(spsign))) < 0)
		{
			UppleUserErrLog2(__FILE__, __LINE__,
					"  [%s-%d]in :: fldName [SPSIGN] not find!\n", __FILE__,
					__LINE__);
			return (ret);
		}
		UppleLog2(__FILE__, __LINE__, "SPSIGN in table=[%s] \n", spsign);

		if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
				"SPFOOTER", spfooter, sizeof(spfooter))) < 0)
		{
			UppleUserErrLog2(__FILE__, __LINE__,
					"  [%s-%d]in :: fldName [SPFOOTER] not find!\n", __FILE__,
					__LINE__);
			return (ret);
		}
		UppleLog2(__FILE__, __LINE__, "spfooter in table=[%s]\n", spfooter);

		if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
				"MODENAME", modename, sizeof(modename))) < 0)
		{
			UppleUserErrLog2(__FILE__, __LINE__,
					"  [%s-%d]in :: fldName [MODENAME] not find!\n", __FILE__,
					__LINE__);
			return (ret);
		}
		UppleLog2(__FILE__, __LINE__, "modename in table=[%s]\n", modename);

		//凭证头长度1
		tempBuf[len] = strlen(sptitle);
		len += 1;
		//凭证头
		memcpy(tempBuf + len, sptitle, strlen(sptitle));
		len += strlen(sptitle);

		//凭证体长度2
		if ((ret = UpplePackSPBody(transheadinfo, spbody, spbodybackup,
				modename)) < 0)
		{
			UppleLog2(__FILE__, __LINE__, "error in function UpplePackSPBody");
			return ret;
		}
		tempBuf[len] = strlen(spbody) % 256;
		tempBuf[len + 1] = strlen(spbody) / 256;
		len += 2;
		//凭证体
		memcpy(tempBuf + len, spbody, strlen(spbody));
		len += strlen(spbody);

		//凭证签名域长度2
		tempBuf[len] = strlen(spsign) % 256;
		tempBuf[len + 1] = strlen(spsign) / 256;
		len += 2;
		//凭证签名域
		memcpy(tempBuf + len, spsign, strlen(spsign));
		len += strlen(spsign);

		//凭证页脚长度2
		tempBuf[len] = strlen(spfooter) % 256;
		tempBuf[len + 1] = strlen(spfooter) / 256;
		len += 2;
		//凭证页脚
		memcpy(tempBuf + len, spfooter, strlen(spfooter));
		len += strlen(spfooter);

		InsertPrintData(tid, mid, mti, b3, b11, orderno, sptitle, spbodybackup,
				spsign, spfooter); //记录打印数据
	}
	else
	{
		UppleLog2(__FILE__, __LINE__,
				"##################无打印头体################ \n");
		//凭证头长度1
		tempBuf[len] = 0;
		len += 1;
		//凭证体长度2
		tempBuf[len] = 0;
		tempBuf[len + 1] = 0;
		len += 2;
		//凭证签名域长度2
		tempBuf[len] = 0;
		tempBuf[len + 1] = 0;
		len += 2;
		//凭证页脚长度2
		tempBuf[len] = 0;
		tempBuf[len + 1] = 0;
		len += 2;
	}
	memcpy(pr_buf, tempBuf, len + 1);
	return len;
}

char* find_index(const char*src, const char*s)
{
	char * p = 0;
	int len = strlen(s);

	if (src == NULL || s == NULL || (*s == 0))
		return 0;

	p = strstr(src, s);
	while (p)
	{
		if (p != 0)
		{
			if (*(p - 1) == '%' && *(p + len) == '%')
			{
				//printf("find it\n");
				return p;
			}
			else
			{
				p = strstr(p + len, s);
			}
		}
	}
	return p;
}


int UpplePackSPBody(const PTUppleTransHeadInfo transheadinfo, char*spbody,
		char*spbodybackup, char *modename)
{
	int ret;
	char mid[15 + 1]; //42 域
	char tid[8 + 1]; //41 域
	//char flow_id[6 + 1]; //11
	//char msg_code[4 + 1]; //0
	//char trans_code[6 + 1]; //3

	char upmerchname[80 + 1]; //  商户名称取终端表中字段
	char upmerchno[15 + 1]; // 汇卡商户号
	char upterminalno[8 + 1]; // 汇卡 终端号

	char operatorno[3 + 1]; //   pos终端号 签到63域
	char uporderno[20 + 1]; //   汇卡订单号 从消息头取
	char uppaytype[10 + 1]; //支付方式
	char printdate[20 + 1]; //打印时间 当前系统时间
	char upreference[50 + 1]; //备注

	char upissuer[80 + 1]; //   发卡机构
	char upacquirer[80 + 1]; //  收单机构
	char upcardno[19 + 1]; //2域   卡号
	char uptranstype[30 + 1]; //交易类型  取数据表pattern中   modename
	char upexpdate[4 + 1]; //14域 卡有效期
	char upreferno[12 + 1]; //37域  参考号
	char upauthno[6 + 1]; //38  授权码
	char upbatchno[14 + 1]; //60 批次号
	char upvoucherno[6 + 1]; //11 凭证号
	char updatetime[10 + 1]; //12,13 交易时间
	char transupdatetime[20 + 1];//转换格式后的交易时间
	char yl012[6 + 1]; //12域
	char yl013[4 + 1]; //13域
	char upamount[13 + 1]; //4 交易金额saletid, salemid, midname
	char transtype[4 + 1];

	memset(tid, 0, sizeof(tid));
	memset(mid, 0, sizeof(mid));
	memset(upmerchname, 0, sizeof(upmerchname));
	memset(upmerchno, 0, sizeof(upmerchno));
	memset(upterminalno, 0, sizeof(upterminalno));
	memset(operatorno, 0, sizeof(operatorno));
	memset(uporderno, 0, sizeof(uporderno));
	memset(uppaytype, 0, sizeof(uppaytype));
	memset(printdate, 0, sizeof(printdate));
	memset(upreference, 0, sizeof(upreference));
	memset(upissuer, 0, sizeof(upissuer));
	memset(upacquirer, 0, sizeof(upacquirer));
	memset(upcardno, 0, sizeof(upcardno));
	memset(uptranstype, 0, sizeof(uptranstype));
	memset(upexpdate, 0, sizeof(upexpdate));
	memset(upreferno, 0, sizeof(upreferno));
	memset(upauthno, 0, sizeof(upauthno));
	memset(upbatchno, 0, sizeof(upbatchno));
	memset(upvoucherno, 0, sizeof(upvoucherno));
	memset(updatetime, 0, sizeof(updatetime));
	memset(yl012, 0, sizeof(yl012));
	memset(yl013, 0, sizeof(yl013));
	memset(upamount, 0, sizeof(upamount));
	memset(transtype, 0, sizeof(transtype));

	memcpy(upmerchno, transheadinfo->hicardmid, 15);//汇卡商户号
	memcpy(upterminalno, transheadinfo->hicardtid, 8);//汇卡终端

	UppleLog2(__FILE__, __LINE__, " upmerchno: %s\n", upmerchno);
	UppleLog2(__FILE__, __LINE__, " upterminalno: %s\n", upterminalno);

	memcpy(transtype, transheadinfo->transtype, 4);//取得报文头中透传交易类型
	UppleLog2(__FILE__, __LINE__, " transtype: %s \n", transtype);

	UppleReadYLFld(42, mid, sizeof(mid));
	UppleLog2(__FILE__, __LINE__, " mid: %s\n", mid);

	UppleReadYLFld(41, tid, sizeof(tid));
	UppleLog2(__FILE__, __LINE__, " tid: %s\n", tid);

	//UppleReadYLFld(2, upcardno, sizeof(upcardno));
	memcpy(upcardno, transheadinfo->cardno, 20);
#if 1	/*消费撤销，结构体内的卡号为空*/
	if(strlen(upcardno) <= 0)
	{
		UppleReadTermFld(2,upcardno,sizeof(upcardno));
		if(strlen(upcardno) <= 0)
		{
			UppleReadYLFld(2, upcardno, sizeof(upcardno));
		}
	}
	UppleLog2(__FILE__,__LINE__,"in UpplePackSPBody:: transheadinfo->cardno=[%s],len=[%d]\n",transheadinfo->cardno,strlen(transheadinfo->cardno));
	if((0 == memcmp(transheadinfo->cardno,"                    ",20)) || (0 == strlen(transheadinfo->cardno)))
	{
		UppleLog2(__FILE__,__LINE__,"in UpplePackSPBody:: transheadinfo->cardno NULL \n");
		memset(transheadinfo->cardno,0x00,sizeof(transheadinfo->cardno));
		memcpy(transheadinfo->cardno,upcardno,strlen(upcardno));
		UppleLog2(__FILE__,__LINE__,"in UpplePackSPBody:: transheadinfo->cardno=[%s] \n",transheadinfo->cardno);
	}
#endif
	UppleLog2(__FILE__, __LINE__, "upcardno: %s\n", upcardno);

	strcpy(uptranstype, modename);
	UppleLog2(__FILE__, __LINE__, "0 uptranstype: %s\n", uptranstype);
#if 0
	UppleReadYLFld(14, upexpdate, sizeof(upexpdate));
#else
	strcpy(upexpdate,"    " );
#endif
	UppleLog2(__FILE__, __LINE__, "14 upexpdate: %s\n", upexpdate);

	UppleReadYLFld(37, upreferno, sizeof(upreferno));
	UppleLog2(__FILE__, __LINE__, "37 upreferno: %s\n", upreferno);

	UppleReadYLFld(38, upauthno, sizeof(upauthno));
	UppleLog2(__FILE__, __LINE__, "38 upauthno: %s\n", upauthno);

	UppleReadYLFld(60, upbatchno, sizeof(upbatchno));
	UppleLog2(__FILE__, __LINE__, "60 upbatchno: %s\n", upbatchno);

	UppleReadYLFld(11, upvoucherno, sizeof(upvoucherno));
	UppleLog2(__FILE__, __LINE__, "11 upvoucherno: %s\n", upvoucherno);

	UppleReadYLFld(12, yl012, sizeof(yl012));
	UppleLog2(__FILE__, __LINE__, "12 yl012: %s\n", yl012);

	UppleReadYLFld(13, yl013, sizeof(yl013));
	UppleLog2(__FILE__, __LINE__, "13 yl013: %s\n", yl013);

	memcpy(updatetime, yl013, 4);
	memcpy(updatetime + 4, yl012, 6);
	changeTimeFormat(updatetime, transupdatetime); //转换交易时间格式
	UppleLog2(__FILE__, __LINE__, "13+12 updatetime: %s\n", updatetime);

	UppleReadYLFld(4, upamount, sizeof(upamount));
	UppleLog2(__FILE__, __LINE__, "4 upamount: %s\n", upamount);

	memcpy(uporderno, transheadinfo->hicardorderid, 20);//取得订单号
	UppleLog2(__FILE__, __LINE__, "订单号： uporderno: %s\n", uporderno);
        /* added by caiyucheng, 2013-07-23, begin */
        if(strncmp(transheadinfo->hicardorderid, "0000Z", 5) == 0 ) 
        {
                strcpy(uporderno, transheadinfo->hicardorderid + 4 );
	        UppleLog2(__FILE__, __LINE__, "脱机订单号去零后： uporderno: %s\n", uporderno);
        }else if(0 == strncmp(transheadinfo->hicardorderid, "0000", 4)){
		//zzq modify
                strcpy(uporderno, transheadinfo->hicardorderid + 4 );
	        UppleLog2(__FILE__, __LINE__, "联机订单号去零后： uporderno: %s\n", uporderno);
	}
        /* added by caiyucheng, 2013-07-23, end */
	strcpy(uppaytype, "银行卡");
	UppleLog2(__FILE__, __LINE__, "消费方式： uporderno: %s\n", uppaytype);

	gettime(printdate);
	UppleLog2(__FILE__, __LINE__, "打印时间：printdate: %s\n", printdate);
	//取得操作员号
	getOperatorNo(upterminalno, upmerchno, operatorno);
	UppleLog2(__FILE__, __LINE__, "operatorno: %s \n", operatorno);
	//取得商户名称发卡行收单行
	//getInfoFromTransJn(upterminalno, upmerchno, atol(upvoucherno), upissuer,
	//	upacquirer, upmerchname, uporderno);
	get_merchname_issuer_acquireer(transheadinfo, upissuer, upacquirer,
			upmerchname);

	UppleLog2(__FILE__, __LINE__, "upmerchname: %s \n", upmerchname);
	UppleLog2(__FILE__, __LINE__, "upissuer: %s\n", upissuer);
	UppleLog2(__FILE__, __LINE__, "upacquirer: %s\n", upacquirer);


	switch (atol(transtype))
	{
	case 7:
	case 15:
	{
		memset(uptranstype, 0, sizeof(uptranstype));
		strcpy(uptranstype, "消费撤销");
		break;
	}
	default:
		break;

	}

	if ((ret = replace(spbody, "UPMERCHNAME", upmerchname) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPMERCHNAME为：%s 失败", upmerchname);
	}

#if 0

 #if 0
	if ((ret = replace(spbody, "UPMERCHNO", upmerchno) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPMERCHNO为：%s 失败", upmerchno);
	}

	if ((ret = replace(spbody, "UPMERCHNO", upmerchno) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPMERCHNO为：%s 失败", upmerchno);
	}
 #else
	char printMID[15+1]={0};
        int res = CheckIsPrintYLMID(printMID);
        UppleLog2(__FILE__,__LINE__,"in UpplePackSPBody::CheckIsPrintYLMID printMID[%s],return[%d]\n",printMID,res);
        if(res != 1)
        {
                if ((ret = replace(spbody, "UPMERCHNO", upmerchno) < 0))
                {
                        UppleLog2(__FILE__, __LINE__, "替换字符UPMERCHNO为：%s 失败", upmerchno);
                }
        }
        else
        {
                if ((ret = replace(spbody, "UPMERCHNO", printMID) < 0))
                {
                        UppleLog2(__FILE__, __LINE__, "替换字符UPMERCHNO为：%s 失败", printMID);
                }
        }
 #endif

	if ((ret = replace(spbody, "UPTERMINALNO", upterminalno) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPTERMINALNO为：%s 失败", upterminalno);
	}
#else
	char 	printMID[20+1]={0};
	char 	printTID[20+1]={0};
	char	Channel[20]={0};

	char    extra_date[1024]={0};
        UppleGet2(RPUB, "PB65",extra_date);
        UppleGet2(RPUB, "#Channel",Channel);
	UppleLog2(__FILE__,__LINE__,"##附加域的值[%s]\n",extra_date);

        int res = CheckIsPrintYLMIDTID(printMID,printTID);
	//if(res == 1 || res == 3)
	if(res == 1)
	{
		if ((ret = replace(spbody, "UPMERCHNO", printMID) < 0))
                {
                        UppleLog2(__FILE__, __LINE__, "替换字符UPMERCHNO为：%s 失败", printMID);
                }
		if ((ret = replace(spbody, "UPTERMINALNO", upterminalno) < 0))
        	{
                	UppleLog2(__FILE__, __LINE__, "替换字符UPTERMINALNO为：%s 失败", upterminalno);
        	}
	}
	//else if(res == 2)
	else if(res == 2 || res == 3)
	{
		if ((ret = replace(spbody, "UPMERCHNO", printMID) < 0))
                {
                        UppleLog2(__FILE__, __LINE__, "替换字符UPMERCHNO为：%s 失败", printMID);
                }

		//if((extra_date[0] == 'Y') || (extra_date[0] == 'C'))
		if(((extra_date[0] == 'Y')&&(6002 != atoi(Channel))) || (extra_date[0] == 'C'))
		{
			if ((ret = replace(spbody, "UPTERMINALNO", upterminalno) < 0))
                	{
                        	UppleLog2(__FILE__, __LINE__, "替换字符UPTERMINALNO为：%s 失败", upterminalno);
                	}
		}
		else
		{
			if ((ret = replace(spbody, "UPTERMINALNO", printTID) < 0))
                	{
                        	UppleLog2(__FILE__, __LINE__, "替换字符UPTERMINALNO为：%s 失败", printTID);
                	}
		}
	}
	else
	{
		if ((ret = replace(spbody, "UPMERCHNO", upmerchno) < 0))
                {
                        UppleLog2(__FILE__, __LINE__, "替换字符UPMERCHNO为：%s 失败", upmerchno);
                }
		if ((ret = replace(spbody, "UPTERMINALNO", upterminalno) < 0))
                {
                        UppleLog2(__FILE__, __LINE__, "替换字符UPTERMINALNO为：%s 失败", upterminalno);
                }
	}
#endif

	if ((ret = replace(spbody, "OPERATORNO", operatorno) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符OPERATORNO为：%s 失败", operatorno);
	}

	if ((ret = replace(spbody, "UPORDERNO", uporderno) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPORDERNO为：%s 失败", uporderno);
	}

	if ((ret = replace(spbody, "UPPAYTPYE", uppaytype) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPPAYTPYE为：%s 失败", uppaytype);
	}

	if ((ret = replace(spbody, "UPISSUER", upissuer) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPISSUER为：%s 失败", upissuer);
	}

	if ((ret = replace(spbody, "UPACQUIRER", upacquirer) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPACQUIRER为：%s 失败", upacquirer);
	}

	TransCardNumDisplay(upcardno);
	if ((ret = replace(spbody, "UPCARDNO", upcardno) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPCARDNO为：%s 失败", upcardno);
	}

	if ((ret = replace(spbody, "UPTRANSTYPE", uptranstype) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPTRANSTYPE为：%s 失败", uptranstype);
	}

	if ((ret = replace(spbody, "UPEXPDATE", upexpdate) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPEXPDATE为：%s 失败", upexpdate);
	}

	if ((ret = replace(spbody, "UPREFERNO", upreferno) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPREFERNO为：%s 失败", upreferno);
	}

	if ((ret = replace(spbody, "UPAUTHNO", upauthno) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPAUTHNO为：%s 失败", upauthno);
	}

	memcpy(upbatchno, upbatchno + 2, 6);
	upbatchno[6] = 0;
	if ((ret = replace(spbody, "UPBATCHNO", upbatchno) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPBATCHNO为：%s 失败", upbatchno);
	}

	if ((ret = replace(spbody, "UPVOUCHERNO", upvoucherno) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPVOUCHERNO为：%s 失败", upvoucherno);
	}

	if ((ret = replace(spbody, "UPDATETIME", transupdatetime) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPDATETIME为：%s 失败", transupdatetime);
	}

	TranslationMoneyFld(upamount);
	if ((ret = replace(spbody, "UPAMOUNT", upamount) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPAMOUNT为：%s 失败", upamount);
	}
	strcpy(spbodybackup, spbody); //备份spbody 不包括备注栏位打印时间栏位

	if ((ret = replace(spbody, "UPREFERENCE", upreference) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符UPREFERENCE为：%s 失败", upreference);
	}

	if ((ret = replace(spbody, "PRINTDATE", printdate) < 0))
	{
		UppleLog2(__FILE__, __LINE__, "替换字符PRINTDATE为：%s 失败", printdate);
	}
#if 0
	/*增加汇卡ID，汇卡终端号*/
	if ((ret = replace(spbody, "HKMERNNO",transheadinfo->hicardmid) < 0))
        {
                UppleLog2(__FILE__, __LINE__, "替换字符HKMERNNO为：%s 失败",transheadinfo->hicardmid);
        }
	if ((ret = replace(spbody, "HKTERMNO",transheadinfo->hicardtid) < 0))
        {
                UppleLog2(__FILE__, __LINE__, "替换字符HKTERMNO为：%s 失败", transheadinfo->hicardtid);
        }
	/*****************add end******************/
#endif
	/*add print mode*/
        char    tmp[1024]={0};
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"#CollectionAccount",tmp);
        if(strlen(tmp) > 0)
        {
                if ((ret = replace(spbody, "CollectionAccount", tmp) < 0))
                {
                        UppleLog2(__FILE__, __LINE__, "替换字符为CollectionAccount：%s 失败", tmp);
                }
        }

        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"#PaymentAccount",tmp);
        if(strlen(tmp) > 0)
        {
                if ((ret = replace(spbody, "PaymentAccount", tmp) < 0))
                {
                        UppleLog2(__FILE__, __LINE__, "替换字符为PaymentAccount：%s 失败", tmp);
                }
        }

        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"#CollectionPeo",tmp);
        UppleLog2(__FILE__,__LINE__,"##TEST,CollectionPeo=[%s]\n",tmp);
        if(strlen(tmp) > 0)
        {
                if ((ret = replace(spbody, "CollectionPeo", tmp) < 0))
                {
                        UppleLog2(__FILE__, __LINE__, "替换字符为CollectionPeo：%s 失败", tmp);
                }
        }

	memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"#Upstream_mid",tmp);
        UppleLog2(__FILE__,__LINE__,"##TEST,Upstream_mid=[%s]\n",tmp);
        if(strlen(tmp) > 0)
        {
                 if ((ret = replace(spbody, "Upstream_mid", tmp) < 0))
                 {
                         UppleLog2(__FILE__, __LINE__, "替换字符为Upstream_mid：%s 失败", tmp);
                 }
        }

        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"#Upstream_name",tmp);
        UppleLog2(__FILE__,__LINE__,"##TEST,Upstream_name=[%s]\n",tmp);
        if(strlen(tmp) > 0)
        {
                if ((ret = replace(spbody, "Upstream_name", tmp) < 0))
                {
                          UppleLog2(__FILE__, __LINE__, "替换字符为Upstream_name：%s 失败", tmp);
                }
        }

        /**********add by zwb at 2014-9-9 end *******************/
{
		char tmp2[256 + 1] = {0};
		//银联钱包信息
		memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"PB63",tmp);
		if(strlen(tmp) > 3){
			if(memcmp(tmp,"CUP",3 ) == 0 ){
				if(strlen(tmp) > 63){
					UppleFilterAllBlankChars(tmp+3,60,tmp2,sizeof(tmp2));
				}else
					UppleFilterAllBlankChars(tmp+3,strlen(tmp) -3,tmp2,sizeof(tmp2));
				memset(tmp, 0x00, sizeof(tmp));
				sprintf(tmp,"{\"PrintAD\":\"%s\"}",tmp2);
				//UppleLog2(__FILE__,__LINE__,"tmp2 = [%s]\n",tmp2);
				UppleSet2(RPUB,"##PRINT",tmp);
				UppleLog2(__FILE__, __LINE__, "###返回终端:打印信息=[%s]\n", tmp);
			}

			replace(spbody, "preferential", tmp2);
		}else{
			replace(spbody, "preferential","");
		}
		
}

	return 0;
}

int TranslationMoneyFld(char*money)
{
	char t[13 + 1];
	char w[2 + 1];
	char m[10 + 1];

	memset(t, 0, sizeof(t));
	memset(w, 0, sizeof(w));
	memset(m, 0, sizeof(m));

	memcpy(t, money, 10);
	memcpy(w, money + 10, 2);
	sprintf(m, "%d.%s\n", atol(t), w);
	strcpy(money, m);

	return 0;
}

int InitTermTransStatusTC_NEW(const PTUppleTransHeadInfo transheadinfo, int len)
{
	//是透传包的记流水
	InsertTransStatusTC_New(transheadinfo, len);
	//不是透传包的不记流水
}

//honghui.cao
int InitTermTransStatus_101(char *package, int len)
{

	int ret;
	char fld000[4 + 1];
	char fld003[6 + 1];

	// 解包
	if ((ret = UppleUnpackTermPackage(package, len)) < 0)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"  [%s-%d]in UppleGetDealMDLIDOfPackageReceivedFromTerm:: UppleUnpackTermPackage [%d] [%s]!\n",
				__FILE__, __LINE__, len, package);
		return (ret);
	}
	memset(fld000, 0, sizeof(fld000));
	if ((ret = UppleReadTermFld(0, fld000, sizeof(fld000))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]in InsertTermTransStatus:: UppleReadTermFld 0!\n",
				__FILE__, __LINE__);
		return (ret);
	}
	memset(fld003, 0, sizeof(fld003));
	if ((ret = UppleReadTermFld(3, fld003, sizeof(fld003))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"  [%s-%d]in InsertTermTransStatus:: UppleReadTermFld 3!\n",
				__FILE__, __LINE__);
		return (ret);
	}
	UppleLog2(__FILE__, __LINE__, "登记流水开始......\n");
	ret = InsertTransStatus_101();
	UppleLog2(__FILE__, __LINE__, "登记流水结束.\n");

	return ret;
}

int TransCardNumDisplay(char*cardno)
{

	int i = 0;
	int len;

	if (cardno == NULL)
		return -1;

	len = strlen(cardno);
	for (i = 6; i < len - 4; i++)
	{
		cardno[i] = '*';
	}

	return 0;
}

int UppleInitDownloadHeadInfo(
		PTUppleDataDownloadHeadInfo PDataDownloadHeadInfo, char*pos_buf)
{
	if (NULL == PDataDownloadHeadInfo)
	{
		UppleUserErrLog(
				"in UppleInitDownloadHeadInfo:: PDataDownloadHeadInfo is NULL!\n");
		return (errCodeParameter);
	}
	if (NULL == pos_buf)
	{
		UppleUserErrLog("in UppleInitDownloadHeadInfo:: pos_buf is NULL!\n");
		return (errCodeParameter);
	}
	memset(PDataDownloadHeadInfo, 0, sizeof(TUppleDataDownloadHeadInfo));

	memcpy(PDataDownloadHeadInfo->reserved, pos_buf, 8);//保留区
	memcpy(PDataDownloadHeadInfo->msgtype, pos_buf + 8, 4); //消息类型
	memcpy(PDataDownloadHeadInfo->msgstreamcode, pos_buf + 8 + 4, 24); //消息流水
	memcpy(PDataDownloadHeadInfo->msgdatetime, pos_buf + 8 + 4 + 24, 14);//消费发出时间
	memcpy(PDataDownloadHeadInfo->transcode, pos_buf + 8 + 4 + 24 + 14, 8);//业务代码
	memcpy(PDataDownloadHeadInfo->hicardmid, pos_buf + 8 + 4 + 24 + 14 + 8, 15);//汇卡商户号
	memcpy(PDataDownloadHeadInfo->hicardtid,
			pos_buf + 8 + 4 + 24 + 14 + 8 + 15, 8); //汇卡终端号

	UppleLog2(__FILE__, __LINE__, "\n消息类型：%s\n消息流水：%s\n消费发出时间：%s \n"
		"业务代码：%s \n汇卡终端号：%s\n汇卡商户号：%s \n", PDataDownloadHeadInfo->msgtype,
			PDataDownloadHeadInfo->msgstreamcode,
			PDataDownloadHeadInfo->msgdatetime,
			PDataDownloadHeadInfo->transcode, PDataDownloadHeadInfo->hicardtid,
			PDataDownloadHeadInfo->hicardmid);

	return 0;
}

int UppleGetSearchData(char*sql, char*pReturndata)
{
	char columnsName[512];
	char tablesName[128];
	char where[512];
	char *head, *end;
	int ret;
	memset(columnsName, 0, sizeof(columnsName));
	memset(tablesName, 0, sizeof(tablesName));
	memset(where, 0, sizeof(where));

	if ((head = strstr(sql, "&&")) == NULL)
	{
		return -1;
	}
	if ((end = strstr(head + 2, "&&")) == NULL)
	{
		return -1;
	}
	memcpy(columnsName, head + 2, end - head - 2);

	if ((head = end) == NULL)
	{
		return -1;
	}
	if ((end = strstr(head + 2, "&&")) == NULL)
	{
		return -1;
	}
	memcpy(tablesName, head + 2, end - head - 2);

	if ((head = end) == NULL)
	{
		return -1;
	}
	if ((end = strstr(head + 2, "&&")) == NULL)
	{
		return -1;
	}
	memcpy(where, head + 2, end - head - 2);

	UppleLog2(__FILE__, __LINE__, "%s \n", columnsName);
	UppleLog2(__FILE__, __LINE__, "%s \n", tablesName);
	UppleLog2(__FILE__, __LINE__, "%s \n", where);

	if ((ret = UppleCheckTables(tablesName)) < 0)
	{
		UppleLog2(__FILE__, __LINE__, "不能搜索非法表数据！ \n");
		return -1;
	}
	/*
	 if ((ret = UppleCheckWhere(where)) < 0)
	 {
	 UppleLog2(__FILE__,__LINE__,"查询条件不含索引，请检查核实！ \n");
	 return -1;
	 }
	 */
	if ((ret = UppleSelectRealDBToBuf(tablesName, columnsName, where,
			pReturndata, 10240)) < 0)
	{
		UppleLog2(__FILE__, __LINE__, "数据查询失败 ! \n");
		return ret;
	}
	return ret;
}

int UppleCheckTables(char*tablesName)
{
	char*head, *end;
	char tmp[30];
	char * legalTablesName = "t_trans_jnl,T_TRANS_JNL";

	if (tablesName == NULL || strlen(tablesName) == 0)
		return -1;
	head = tablesName;
	while (NULL != (end = strchr(head, ',')))
	{
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, head, end - head);
		UppleFilterHeadAndTailBlank(tmp);
		UppleLog2(__FILE__, __LINE__, "%s ", tmp);
		if (*tmp == 0 || (!strstr(legalTablesName, tmp)))
			return -1;

		head = end + 1;
	}
	return 0;
}
//end
int UppleConnectTermPackageCUPS()
{
	PUpplePackage ppackage = NULL;
	int ret;
	PUpplePackageDef pdef;

	if (UppleIsTermPackageConnected())
		return (0);

	if ((pguppleTermPackage = UppleConnectPackage("term8583")) == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleConnectTermPackage:: UppleConnectPackage [%s]\n",
				"term8583");
		return (errCodeCurrentMDL_Connect8583PackageMDL);
	}
	if ((pguppleTermPackageHeader = UppleConnectPackage("termPackHeader"))
			== NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleConnectTermPackage:: UppleConnectPackage [%s]\n",
				"termPackHeader");
		return (errCodeCurrentMDL_Connect8583PackageMDL);
	}
	if ((guppleLenOfTermPackageHeader = UppleGetLengthOfTermPackageHeader())
			< 0)
		guppleLenOfTermPackageHeader = 0;
	return (0);
}
int UppleUnpackTermPackageCUPS(unsigned char *data, int lenOfData)
{
	int offset;
	int ret;
	char rejectCode[10 + 1];

	if ((ret = UppleConnectTermPackageCUPS()) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleUnpackTermPackage:: UppleUnpackTermPackage!\n");
		return (ret);
	}

	//	UppleMemLog2(__FILE__,__LINE__,"term package:",data,lenOfData);

	UppleLog2(__FILE__, __LINE__,
			"Unpack, guppleLenOfTermPackageHeader=[%d]\n",
			guppleLenOfTermPackageHeader);
	if (guppleLenOfTermPackageHeader > 0) // 有报文头
	{
		UppleInitPackage(pguppleTermPackageHeader);
		if ((ret = UppleUnpack(pguppleTermPackageHeader, data, lenOfData)) < 0)
		{
			UppleUserErrLog2(
					__FILE__,
					__LINE__,
					"in UppleUnpackTermPackage:: UppleUnpack pguppleTermPackageHeader [%d]\n",
					lenOfData);
			return (ret);
		}
		UpplePackageToMM(pguppleTermPackageHeader, "TH");
		UpplePackageToMM(pguppleTermPackageHeader, "PH");
	}
	UppleInitPackage(pguppleTermPackage);
	if ((ret = UppleUnpack(pguppleTermPackage, data
			+ guppleLenOfTermPackageHeader, lenOfData
			- guppleLenOfTermPackageHeader)) < 0)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"in UppleUnpackTermPackage:: UppleUnpack pguppleTermPackage [%d]\n",
				lenOfData - guppleLenOfTermPackageHeader);
		return (ret);
	}

	UpplePackageToMM(pguppleTermPackage, "TB");
	UpplePackageToMM(pguppleTermPackage, "PB");
	return (0);
}
int UppleSetTermResponseMTIFromRequestMTICUPS()
{
	char tmpBuf[10];
	int ret;

	memset(tmpBuf, 0, sizeof(tmpBuf));
	/*
	if ((ret = UppleReadTermFld(0, tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleSetTermResponseMTIFromRequestMTI:: UppleReadTermFld 0!\n");
		return (ret);
	}
	*/
	UppleGet2(RPUB,"PB0",tmpBuf);
	UppleLog2(__FILE__,__LINE__,"MTI = [%s]\n",tmpBuf);
	if ((tmpBuf[2] + '0') % 2 != 0)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"in UppleSetTermResponseMTIFromRequestMTI:: mti not request [%s]\n",
				tmpBuf);
		return (errCodeCurrentMDL_Not8583RequestMTI);
	}
	tmpBuf[2] += 1;
	return (UppleSetTermFld(0, tmpBuf, 4));
}
int InitTermTransStatusCUPS(char *package, int len)
{
	int ret;
	UppleLog2(__FILE__, __LINE__, "CUPS登记流水开始......\n");
	ret = InsertTransStatusCUPS();
	UppleLog2(__FILE__, __LINE__, "CUPS登记流水结束.\n");
	return ret;
}
/*
*	CUPS 拼装终端付款方请求数据域
*
*/
int UppleFormTermSaleRequestDBRecFromPackageCUPS(PUppleTermRequestDBRec ptermReqRec)
{
	int i;
	int ret;
	int offset = 0;
	char number[1 + 1];
	char tmpBuf[128 + 1];

	if (ptermReqRec == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormSaleTermRequestDBRecFromPackage:: ptermReqRec is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptermReqRec, 0, sizeof(*ptermReqRec));

	UppleFormTermPayerPrimaryKeyFromPackageCUPS(&(ptermReqRec->primaryKey));

	UppleReadTermTPDU(ptermReqRec->termtpdu);

	memset(tmpBuf, 0, sizeof(tmpBuf));
	UppleGet2( RPUB, "TB0", tmpBuf  );
	UppleGet2( RPUB, "TB3", tmpBuf  + 4 );
	UppleGet2( RPUB, "TB4", ptermReqRec->termamount  );
	UppleGet2( RPUB, "TB42", ptermReqRec->termmid  );
	UppleGet2( RPUB, "TB2", ptermReqRec->paycard  );
	UppleLog2(__FILE__,__LINE__,"tmpBuf = [%s]\n",tmpBuf);
	strcpy(ptermReqRec->termtransid, tmpBuf);

	return (0);

}
int UppleFormTermPayerPrimaryKeyFromPackageCUPS(PUppleTermPrimaryKey ptermKey)
{
	int ret;

	if (ptermKey == NULL)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayerPrimaryKeyFromPackage:: ptermKey is NULL!\n");
		return (errCodeParameter);
	}

	memset(ptermKey, 0, sizeof(*ptermKey));
/*
	if ((ret = UppleReadTermFld(11, ptermKey->termssn,
			sizeof(ptermKey->termssn))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayerPrimaryKeyFromPackage:: UppleReadTermFld 11!\n");
		return (ret);
	}
	if ((ret = UppleReadTermFld(41, ptermKey->termno, sizeof(ptermKey->termno)))
			< 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayerPrimaryKeyFromPackage:: UppleReadTermFld 41!\n");
		return (ret);
	}
	if ((ret = UppleReadTermFld(63, ptermKey->termbatchid,
			sizeof(ptermKey->termbatchid))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleFormTermPayerPrimaryKeyFromPackage:: UppleReadTermFld 63!\n");
		return (ret);
	}
*/
	UppleGet2( RPUB, "TB11", ptermKey->termssn  );
	UppleGet2( RPUB, "TB41", ptermKey->termno  );
	UppleGet2( RPUB, "TB63", ptermKey->termbatchid  );
	UppleLog2(__FILE__,__LINE__,"ptermKey->termssn = [%s],ptermKey->termno=[%s],ptermKey->termbatchid=[%s]\n",ptermKey->termssn,ptermKey->termno,ptermKey->termbatchid);
	
	sprintf(ptermKey->payflag, "%d", conPayFlagIsPayer);

	return (0);

}

int UpdateRePrintStatu(char *hk_mid,char *hk_tid,char *orderno,int statu)
{
        char where[1024 + 1];
        char dbRecord[1024 + 1];
        int ret = -1;
        char print_statu[16]={0};
        char flds[1024]={0};
#if 0
        char orderno[24]={0};
        char hk_mid[20]={0};
        char hk_tid[20]={0};
        memset(orderno,0,sizeof(orderno));
        memset(hk_mid,0,sizeof(hk_mid));
        memset(hk_tid,0,sizeof(hk_tid));

        memcpy(hk_mid, transheadinfo->hicardmid, 15);//汇卡商户号
        memcpy(hk_tid, transheadinfo->hicardtid, 8);//汇卡终端
        memcpy(orderno, transheadinfo->hicardorderid, 20);//取得汇卡订单号
#else
        if(hk_mid == NULL || hk_tid == NULL || orderno == NULL)
        {
                UppleUserErrLog2(__FILE__, __LINE__, "in UpdateRePrintStatu::PARAM error!\n");
                return -1;
        }
#endif
        memset(where, 0x00, sizeof(where));

        /*statu 为状态位，0是旧协议，1是新透传协议*/
        if(statu == 1)
        {
                sprintf(where,"mid = '%s' and termid = '%s' and b34_telnumber like '%%%s' and msgcode = '%s' and b3_pcode = '%s' and b39_respcode = '%s'",\
                                hk_mid,hk_tid,orderno,"0200","000000","00");
        }
        else
        {
                sprintf(where,"mid = '%s' and termid = '%s' and b34_telnumber like '%%%s' and msgcode = '%s' and b39_respcode = '%s'",\
                                hk_mid,hk_tid,orderno,"0200","00");
        }

        memset(dbRecord, 0x00, sizeof(dbRecord));
        if ((ret = UppleSelectRealDBRecord("t_trans_jnl",
                        "print_status", where, dbRecord, sizeof(dbRecord),
                        NULL)) < 0)
        {
                UppleUserErrLog2(__FILE__, __LINE__, "in UpdateRePrintStatu::not find the record!\n");
                if (ret == errCodeDatabaseMDL_RecordNotFound)
                        return 1;
                else
                        return -1;
        }

        memset(print_statu, 0x00, sizeof(print_statu));
        if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
                        "print_status", print_statu, sizeof(print_statu))) < 0)
        {
                UppleUserErrLog2(__FILE__, __LINE__, "  fldName [print_status] not find!\n");
                return -1;
        }

        if(strlen(print_statu) > 0)
        {
                UppleLog2(__FILE__,__LINE__,"in UpdateRePrintStatu::change the print_statu!\n");
                memcpy(print_statu+4,"1",1);

                memset(flds,0,sizeof(flds));
                sprintf(flds,"print_status = '%s'",print_statu);

                if ((ret = UppleUpdateRealDBRecord("t_trans_jnl", flds, where)) < 0) {
                                UppleUserErrLog2(__FILE__, __LINE__,
                                        "  [%s-%d]in UppleUpdateTransDBRec:: UppleInsertRealDBRecord!\n",
                                                __FILE__, __LINE__);
                        return (ret);
                }
        }

        return 1;
}

/*
*       ICBC 拼装终端付款方请求数据域
*
*/
int UppleFormTermSaleRequestDBRecFromPackageICBC(PUppleTermRequestDBRec ptermReqRec)
{
        int i;
        int ret;
        int offset = 0;
        char number[1 + 1];
        char tmpBuf[128 + 1];

        if (ptermReqRec == NULL)
        {
                UppleUserErrLog2(__FILE__, __LINE__,
                                "in UppleFormSaleTermRequestDBRecFromPackage:: ptermReqRec is NULL!\n");
                return (errCodeParameter);
        }

        memset(ptermReqRec, 0, sizeof(*ptermReqRec));

        UppleFormTermPayerPrimaryKeyFromPackageCUPS(&(ptermReqRec->primaryKey));

        UppleReadTermTPDU(ptermReqRec->termtpdu);

        memset(tmpBuf, 0, sizeof(tmpBuf));
        UppleGet2( RPUB, "TB0", tmpBuf  );
        UppleGet2( RPUB, "TB3", tmpBuf  + 4 );
        UppleGet2( RPUB, "TB4", ptermReqRec->termamount  );
        UppleGet2( RPUB, "TB42", ptermReqRec->termmid  );
        UppleGet2( RPUB, "TB2", ptermReqRec->paycard  );
        UppleLog2(__FILE__,__LINE__,"tmpBuf = [%s]\n",tmpBuf);
        strcpy(ptermReqRec->termtransid, tmpBuf);

        return (0);

}
/*added by zhuhui in 2014-8-11--begin */
int UppleFormTermSaleRequestDBRecFromPackageFTS2BOC(PUppleTermRequestDBRec ptermReqRec)
{
        int i;
        int ret;
        int offset = 0;
        char number[1 + 1];
        char tmpBuf[128 + 1];

        if (ptermReqRec == NULL)
        {
                UppleUserErrLog2(__FILE__, __LINE__,
                                "in UppleFormSaleTermRequestDBRecFromPackage:: ptermReqRec is NULL!\n");
                return (errCodeParameter);
        }

        memset(ptermReqRec, 0, sizeof(*ptermReqRec));

        UppleFormTermPayerPrimaryKeyFromPackageCUPS(&(ptermReqRec->primaryKey));

        UppleReadTermTPDU(ptermReqRec->termtpdu);

        memset(tmpBuf, 0, sizeof(tmpBuf));
        UppleGet2( RPUB, "TB0", tmpBuf  );
        UppleGet2( RPUB, "TB3", tmpBuf  + 4 );
        UppleGet2( RPUB, "TB4", ptermReqRec->termamount  );
        UppleGet2( RPUB, "TB42", ptermReqRec->termmid  );
        UppleGet2( RPUB, "TB2", ptermReqRec->paycard  );
        UppleLog2(__FILE__,__LINE__,"tmpBuf = [%s]\n",tmpBuf);
        strcpy(ptermReqRec->termtransid, tmpBuf);

        return (0);

}
/*added by zhuhui in 2014-8-11--end */

/* 
初始化栈 
参数：stackSize:栈的总容量大小 
typeSize:以后要入栈的单个元素的大小 
*/
Status InitStack(sqStack *s,unsigned stackSize,unsigned typeSize)
{
  s->base=malloc(stackSize);
  if(!s->base)
    return ERROR;
  s->top=s->base;
  s->stackSize=stackSize;
  s->typeSize=typeSize;
  return TRUE;
}

/* 
压栈 
*/
Status Push(sqStack *s,void *e)
{
  if((int)s->top-(int)s->base+s->typeSize>s->stackSize)
    return FALSE;
  memcpy(s->top,e,s->typeSize);
  s->top=(void*)((int)s->top+s->typeSize);
  return TRUE;
}

/* 
出栈 
*/
Status Pop(sqStack *s,void *e)
{
  if(s->top==s->base)
    return FALSE;
  s->top=(void*)((int)s->top-(int)s->typeSize);
  memcpy(e,s->top,s->typeSize);
  return TRUE;
}

/* 
清栈 
*/
Status ClearStack(sqStack *s)
{
  s->top=s->base;
  return TRUE;
}

/* 
毁栈 
*/
Status DestroyStack(sqStack *s)
{
  free(s->base);
  s->top=s->base=NULL;
  s->stackSize=s->typeSize=0;
  return TRUE;
}

/* 
获取已入栈元素个数 
*/
int GetLen(sqStack *s)
{
  return ((int)s->top-(int)s->base)/s->typeSize;
}

/* 
自定义错误处理函数 
*/
void my_err(char *str)
{
  perror(str);
  exit(1);
}

/* 
将中缀表达式转换为后缀表达式 
参数:infix 指向中缀表达式，以回车键即\n结尾。 
postfix 指向后缀表达式临时缓冲区，用来存放转换后的结果。 
附转换规则：从左到右遍历中缀表达式的每个数字和符号，若是数字则直接保存在postfix数组中；若是符号，则判断其与栈顶符号的优先级，是右括号或者优先级不大于栈顶符号，则栈顶元素依次出栈并输出，直到遇到左括号或者栈空时，才将刚才的那个符号入栈。 
*/  
Status InfixToPostfix(char *infix,char *postfix)  
{  
  sqStack s;  
  if(InitStack(&s,STACKELEMENT*sizeof(char),sizeof(char))==ERROR)  
        my_err("InfixToPostfix init stack error！");  
  char c,e;  
  int j=0,i=0;  
  c=*(infix+i); //取出中缀表达式中的第一个字符   
  i++; 
  int infix_len = strlen(infix); 
  while('\n'!=c || (i < infix_len)) //遇到换行符，表示转换结束   
    {  
       while(c>='0'&&c<='9') //先判断一下取出的字符是否是数字，如果是数字的话，则直接存入postfix数组   
        {   
          postfix[j++]=c;  
          c=*(infix+i);  
          i++;  
          if(c<'0'||c>'9') //如果不是数字，则在后面添加空格，以便区分各个符号   
            {  
            postfix[j++]=' ';  
            }  
        }
      if('#'==c)
      {
	char tmp[128]={0};
	c=*(infix+i);
        i++;
	int jj =0;
	while((c>='0'&&c<='9') || (c>='a'&&c<='z') || (c>='A'&&c<='Z'))
	{
		tmp[jj] = c;
		jj++;
		c=*(infix+i);
        	i++;
		if(i >= infix_len)
		{
			break;
		}
	}
	/*取的变量名之后，去取RPUB的值*/
#if 0
	/*以下是测试*/
	memcpy(postfix+j,tmp,jj);
	j += jj;
	memcpy(postfix+j," ",1);
	j += 1;
	/********************/
#else
	char tmp_RPUB[1024]={0};
	UppleGet2(RPUB,tmp,tmp_RPUB);
	UppleLog2(__FILE__,__LINE__,"in termPack::InfixToPostfix-RPUB[%s],value[%s]\n",tmp,tmp_RPUB);
	int len_rpub = strlen(tmp_RPUB);
	if((0 == strncmp(tmp,"PB4",3)) || (0 == strncmp(tmp,"PB04",4)))
	{
		UppleLog2(__FILE__,__LINE__,"in termPack::进入PB4判断\n");
		char tmp2[128]={0};
#if 0
		if((tmp_RPUB[11] != '0') || (tmp_RPUB[10] != '0'))
#else
		char	tmpp[8]={0};
		memcpy(tmpp,tmp_RPUB+10,2);
		if(49 < atoi(tmpp))
#endif
		{
			UppleLog2(__FILE__,__LINE__,"in termPack::tmp_RPUB+1");
			sprintf(tmp2,"%d",(atoi(tmp_RPUB)/100)+1);
		}
		else
		{
			sprintf(tmp2,"%d",atoi(tmp_RPUB)/100);
		}
		UppleLog2(__FILE__,__LINE__,"in termPack::tmp2[%s]\n",tmp2);
		len_rpub = strlen(tmp2);
		memcpy(postfix+j,tmp2,len_rpub);
                j += len_rpub;
                memcpy(postfix+j," ",1);
                j += 1;
	}
	else
	{
		UppleLog2(__FILE__,__LINE__,"in termPack::其他判断\n");
		memcpy(postfix+j,tmp_RPUB,len_rpub);
		j += len_rpub;
		memcpy(postfix+j," ",1);
		j += 1;
	}
#endif
      }  
      if(')'==c) //不是数字，则判断是否为右括号。[括号的优先级最高，所以，如果是右括号的话，就得先进行括号里的各种运算]   
        {  
          Pop(&s,(void*)&e);  
          while('('!=e) //直到遇到左括号为止   
            {  
              postfix[j++]=e;  
              postfix[j++]=' ';  
              Pop(&s,(void*)&e);  
            }  
        }  
      else if('+'==c||'-'==c || '>'==c || '<' == c || '='==c) //如果是加减号,因为他俩的优先级最低了，所以此时先将栈里的所有符号出栈后(除非遇到左括号)，再把此符号入栈   
        {  
          if(!GetLen(&s)) //如果是空栈，则直接将加减号入栈   
            {  
              Push(&s,(void*)&c);  
            }  
          else   
            {  
              do  
                {  
                  Pop(&s,(void*)&e);  
                  if('('==e)     
                    {  
                      Push(&s,(void*)&e);  
                    }  
                  else   
                    {  
                      postfix[j++]=e;  
                      postfix[j++]=' ';  
                    }  
                }while(GetLen(&s)&&'('!=e);  //将栈里的所有符号出栈(除非遇到左括号)   
              Push(&s,(void*)&c); //最后将新来的加减号再入栈   
            }  
        }  
      else if('*'==c||'/'==c||'('==c || '&' == c || '|' == c || '~' == c) //如果是乘除号或左括号，因为他们的优先级高，所以直接入栈。   
        {  
          Push(&s,(void*)&c);  
        }  
      else if('\n'==c) //判断一下，所有符号是否都已转换完成   
        {  
          break;  
        }  
      else //能走到这个else的，都是我不认识的符号了   
        {  
          // printf("\nError:input error,the character %d cann't recognize!\n",c);   
          return -1;  
        }  
      c=*(infix+i); //取出下一个字符进行转换   
      i++;  
    }  
  while(GetLen(&s)) //转换完成后，栈里可能还有没出栈的运算符号   
    {  
      Pop(&s,(void*)&e);  
      postfix[j++]=e;  
      postfix[j++]=' ';  
    }  
  DestroyStack(&s);  
  return TRUE;  
}  

/* 
计算后缀表达式的结果 
参数：arr使用空格分隔的后缀表达式字符串。例：arr="31 5 + " 
result 保存计算完毕后的结果 
注：如何利用栈来计算后缀表达式的结果：依次取出后缀表达式中的符号进行比较，如果是数字，则直接入栈；如果是符号，则出栈两次，弹出两个要计算的因数，进行计算，之后再将计算结果入栈。知道后缀表达式中所有符号都已比较完毕。 
*/  
Status Calculate(char *arr,void *result)  
{  
  // printf("%s\n",arr);   
  double d,e,f; //d,e 存放两个因数。f存放d,e计算后的结果.   
  sqStack s;   
  char *op; //存放后缀表达式中的每个因数或运算符   
  char *buf=arr; //声明bufhe saveptr两个变量，是strtok_r函数的需要。   
  char *saveptr=NULL;  
  if(InitStack(&s,STACKELEMENT*sizeof(double),sizeof(double))==ERROR)  
        my_err("Calculate init stack error!");  
  while((op=strtok_r(buf," ",&saveptr))!=NULL) //利用strtok_r函数分隔字符串   
    {  
      buf=NULL;  
      switch(op[0])  
        {  
        case '+':  
          Pop(&s,&d);  
          Pop(&s,&e);  
          f=d+e;  
          Push(&s,&f);  
          break;  
        case '-':  
          Pop(&s,&d);  
          Pop(&s,&e);  
          f=e-d;  
          Push(&s,&f);  
          break;  
        case '*':  
          Pop(&s,&d);  
          Pop(&s,&e);  
          f=d*e;  
          Push(&s,&f);  
          break;  
        case '/':  
          Pop(&s,&d);  
          Pop(&s,&e);  
          f=e/d;  
          Push(&s,&f);  
          break;
	case '&':
	  Pop(&s,&d);
          Pop(&s,&e);
          f=e&&d;
          Push(&s,&f);
          break;
	case '|':
          Pop(&s,&d);
          Pop(&s,&e);
          f=e||d;
          Push(&s,&f);
          break;
	case '>':
          Pop(&s,&d);
          Pop(&s,&e);
          f=e>d;
          Push(&s,&f);
          break;  
	case '<':
          Pop(&s,&d);
          Pop(&s,&e);
          f=e<d;
          Push(&s,&f);
          break;
	case '=':
          Pop(&s,&d);
          Pop(&s,&e);
          f=(e==d);
          Push(&s,&f);
          break;
        default:  
          d=atof(op); //不是运算符，就肯定是因数了。所以，用atof函数，将字符串转换为double类型   
          Push(&s,&d);  
          break;  
        }        
    }  
  Pop(&s,result);  
  DestroyStack(&s);  
  return TRUE;  
}  


/***********************以下代码是透传新打印新票格式*********************/
int parsing_add_data(char *add_data,int *argc,char *argv[])
{
        if(add_data == NULL)
        {
                //printf("in parsing_add_data::param error!!\n");
                UppleLog2(__FILE__,__LINE__,"in parsing_add_data::param error!!\n");
                return -1;
        }
        char    *value = NULL;
        char    *next = NULL;
        char    *tmp = NULL;
        value = add_data;
        int     number = 0;
        int     len = 0;
        int     i =0;
        int     tmp_len = 0;
        len = strlen(add_data);
        for(; i < len; i++)
        {
                if(add_data[i] == '+')
                {
                        number++;
                }
        }
        for(i= 0;i<number;i++)
        {
                tmp = strsep(&value,"+");
                tmp_len= strlen(tmp);
                //printf("tmp_data=[%s],len=[%d],add_offset[%s]\n",tmp,tmp_len,value);
                UppleLog2(__FILE__,__LINE__,"tmp_data=[%s],len=[%d],add_offset[%s]\n",tmp,tmp_len,value);
                argv[i] = (char *)malloc(tmp_len +2);
                memset(argv[i],0,sizeof(tmp_len +2));
                memcpy(argv[i],tmp,tmp_len);
                argv[i][tmp_len] = 0;
                tmp = NULL;

        }
        *argc = number;
}
int free_add_data(int *argc,char *argv[])
{
        if(*argc <= 0 )
        {
                //printf("in free_add_data::not to free!!\n");
                UppleLog2(__FILE__,__LINE__,"in free_add_data::not to free!!\n");
                return 0;
        }
        int i =0;
        for(; i < *argc;i++)
        {
                free(argv[i]);
                argv[i] = NULL;
        }
        return 0;
}
int parsing_add_data_to_Redis()
{
        char    PB65[1024]={0};
        char    *argv[64];
        int     argc = 0;
        int     ret = -1;
        UppleGet2(RPUB,"PB65",PB65);
        UppleLog2(__FILE__,__LINE__,"in parsing_add_data_to_Redis::PB65[%s]\n",PB65);
        parsing_add_data(PB65,&argc,argv);
        switch(PB65[0])
        {
                case    'K':    /*对外付款*/
                {
                        /*设置收款账号*/
                        UppleSet2(RPUB,"#CollectionAccount",argv[4]);
                        break;
                }
                case    'F':    /*资金归集*/
                {
                        /*设置付款账号*/
                        UppleSet2(RPUB,"#PaymentAccount",argv[4]);
                        /*设置收款人*/
                        UppleSet2(RPUB,"#CollectionPeo",argv[5]);
                        UppleLog2(__FILE__,__LINE__,"##TEST,CollectionPeo=[%s]\n",argv[5]);
                        break;
                }
		case    'Y':
                {
                        /*设置上游商户号*/
                        UppleSet2(RPUB,"#Upstream_mid",argv[20]);
                        /*设置上游商户名称*/
                        UppleSet2(RPUB,"#Upstream_name",argv[21]);
                        UppleLog2(__FILE__,__LINE__,"##TEST,Upstream_mid[%s],Upstream_name[%s]\n",argv[20],argv[21]);
                        break;
                }
                case    'C':
                {
                        /*设置上游商户号*/
                        UppleSet2(RPUB,"#Upstream_mid",argv[20]);
                        /*设置上游商户名称*/
                        UppleSet2(RPUB,"#Upstream_name",argv[21]);
                        UppleLog2(__FILE__,__LINE__,"##TEST,Upstream_mid[%s],Upstream_name[%s]\n",argv[20],argv[21]);
                        break;
                }
                default:
                        break;
        }
        free_add_data(&argc,argv);
        return 1;
}


/*
 * transfrom trans error code
 * add by zhouzhenfan
 */
void hc_transform_errcode2( char *remark, const char *rspcode, const int errflag)
{
        char channel[9] = {0};
        char PB44_t[128] = {0};
        int platform = 1;       /* Default PPP protocol */
        char chl_gz[9] = {0};
        char chl_hz[9] = {0};
        char chl_nb[9] = {0};
        char chl_fj[9] = {0};
        char chl_kq[9] = {0};
	char chl_yb[9] = {0};
        char chl_ecard[9] = {0};
        char chl_acp[9] = {0};
        char chl_acp_t[9] = {0};
	char type[9] = {0};

        UppleLog2(__FILE__, __LINE__, "[OCX-PACK] hc_transform_errcode start.\n");
        UppleLog2(__FILE__, __LINE__, "[OCX-PACK] Current PB39=[%s]\n", rspcode);
        UppleLog2(__FILE__, __LINE__, "[OCX-PACK] type=[%s],remark=[%s],rspcode=[%s],errflag=[%d]\n", type,remark,rspcode,errflag);

        /* Get current channel */
        UppleGet("#Channel", channel);
	if (channel == NULL)
	UppleGet2(RPUB, "#Channel", channel);
        UppleLog2(__FILE__, __LINE__, "[OCX-PACK] Current channel=[%s]\n", channel);

	if(UppleReadStringTypeRECVar("CHANNEL_GZ") )
        	sprintf(chl_gz, "%s", UppleReadStringTypeRECVar("CHANNEL_GZ"));
	if(UppleReadStringTypeRECVar("CHANNEL_HZ"))
        	sprintf(chl_hz, "%s", UppleReadStringTypeRECVar("CHANNEL_HZ"));
	if(UppleReadStringTypeRECVar("CHANNEL_NB"))
		sprintf(chl_nb, "%s", UppleReadStringTypeRECVar("CHANNEL_NB"));
	if(UppleReadStringTypeRECVar("CHANNEL_FJ"))
		sprintf(chl_fj, "%s", UppleReadStringTypeRECVar("CHANNEL_FJ"));
	if(UppleReadStringTypeRECVar("CHANNEL_KQ"))
		sprintf(chl_kq, "%s", UppleReadStringTypeRECVar("CHANNEL_KQ"));
	if(UppleReadStringTypeRECVar("CHANNEL_YB"))
		sprintf(chl_yb, "%s", UppleReadStringTypeRECVar("CHANNEL_YB"));
	if(UppleReadStringTypeRECVar("CHANNEL_ECARD"))
		sprintf(chl_ecard, "%s", UppleReadStringTypeRECVar("CHANNEL_ECARD"));
	if(UppleReadStringTypeRECVar("CHANNEL_ACP"))
		sprintf(chl_acp, "%s", UppleReadStringTypeRECVar("CHANNEL_ACP"));
	if(UppleReadStringTypeRECVar("CHANNEL_ACP_T"))
		sprintf(chl_acp_t, "%s", UppleReadStringTypeRECVar("CHANNEL_ACP_T"));
	



        UppleLog2(__FILE__, __LINE__, "[OCX-PACK] Current channel=[%s]\n", channel);
        if (strncmp(channel, chl_hz, 4) == 0) {
                strcpy(type, "H");      /* HangZhou YL */
                platform = 1;
        }
        else if (strncmp(channel, chl_nb, 4) == 0) {
                strcpy(type, "N");      /* NinBo YL */
                platform = 1;
        }
	else if (strncmp(channel, chl_yb, 4) == 0) {
                strcpy(type, "Y");      /* YI BAO */
                platform = 1;
        }
        else if (strncmp(channel, chl_fj, 4) == 0) {
                strcpy(type, "F");      /* FuJian YL */
                platform = 1;
        }
        else if (strncmp(channel, chl_kq, 4) == 0) {
                strcpy(type, "K");      /* KuaiQian */

		if (errflag)
			goto OCX_PACK_ERROR;
		if(2001 != atoi(channel))
		{
			/*
                 	* Get errcode remark by trans response
                 	*/
                	UppleGet2(RPUB, "PB44", PB44_t);
                	UppleLog2(__FILE__, __LINE__, "current PB44=[%s]\n", PB44_t);
                	if (strlen(PB44_t) > 16)
                        	strncpy(remark, PB44_t, 16);
                	else if (strlen(PB44_t) > 0)
                        	strcpy(remark, PB44_t);
                	else {
                        	/* no receive PB44 value */
                        	strcpy(remark, "");
               	 	}
		}
		else
		{
			UppleLog2(__FILE__, __LINE__, "[OCX-PACK] channel kq\n");
			memset(remark,0,sizeof(remark));
			hc_get_errcode_remark(remark, rspcode, 4);
		}
		return ;
        }
        else if (strncmp(channel, chl_ecard, 4) == 0) {
                strcpy(type, "E");      /* Ecard web */
                platform = 2;
        }
        else if ((strncmp(channel, chl_acp, 4) == 0) || (strncmp(channel, chl_acp_t, 4) == 0) ) {
                strcpy(type, "A");      /* ACP */
                platform = 5;
		}
        else {
                strcpy(type, "G");      /* Default GuangZhou YL */
                platform = 1;
		UppleLog2(__FILE__, __LINE__, "resp_err_msg\n");
        }

        UppleLog2(__FILE__, __LINE__, "[OCX-PACK] Current type=[%s]\n", type);
OCX_PACK_ERROR:
        /*
         * @errflag
         *   0 - Normal, trans return normal
         *   1 - Unnormal, ocxpack part error
         */

        /*
         * Get remark by errcode table
         * @platform:
         *     1- PPP
         *     2- ECARD
         */
	#if 1
	UppleLog(__FILE__,__LINE__,"in PACK OCX_PACK_ERROR !!\n");
	if (strncmp(channel, chl_kq, 4) == 0) {
		UppleLog(__FILE__,__LINE__,"in PACK errorCode table\n");
		platform = 4;
	}
	#endif
        if (hc_get_errcode_remark(remark, rspcode, platform) < 0)
        {
               strcpy(remark, "交易失败，请重试");
               UppleLog2(__FILE__, __LINE__,
               "hc_get_errcode_remark failure.\n");
        }
        UppleLog2(__FILE__, __LINE__, "[OCX-PACK] 错误返回:[%s][%s].\n",
               rspcode, remark);

        return ;
}


/*API接入*/
int     UppleInitAPIHead(PTUppleAPIHead UppleAPIHead)
{
        if(UppleAPIHead == NULL)
        {
                UppleLog2(__FILE__,__LINE__,"in UppleInitAPIHead::param error!\n");
                return (errCodeParameter);
        }
        memset(UppleAPIHead, 0, sizeof(TUppleAPIHead));
        return 0;
}


int     UppleSetMessageAPIHead(PTUppleAPIHead UppleAPIHead,char *message,int message_len)
{
        if(UppleAPIHead == NULL)
        {
                UppleLog2(__FILE__,__LINE__,"in UppleSetMessageAPIHead::param error!\n");
                return (errCodeParameter);
        }

        if(message_len != 65)
        {
                UppleLog2(__FILE__,__LINE__,"in UppleSetMessageAPIHead::message_len error!\n");
                return (errCodeParameter);
        }

        UppleInitAPIHead(UppleAPIHead);

        int     offset = 0;
        /*获取API头*/
        memcpy(UppleAPIHead->API_Head,message+offset,4);
        offset += 4;
        /*获取API流水号*/
        memcpy(UppleAPIHead->API_ssn_code,message+offset,24);
        offset += 24;
        /*获取API消息类型*/
        memcpy(UppleAPIHead->API_msg_code,message+offset,4);
        offset += 4;
        /*获取API发送时间*/
        memcpy(UppleAPIHead->API_send_time,message+offset,14);
        offset += 14;
        /*获取API错误码*/
        memcpy(UppleAPIHead->API_return_code,message+offset,3);
        offset += 3;
        /*获取API错误描述*/
        memcpy(UppleAPIHead->API_err_describe,message+offset,16);
        offset += 16;

        return 1;
}

int     UppleUnAPIMessage(char *tmpBuf,int message_len)
{
        int     offset = 0;             /*用于计算偏移量*/
        int     tmp_len = 0;            /*临时计算子域长度*/
        int     len_hex = 0;
        char    tmp[128]={0};            /*源报文子域长度*/
        char    HK_org_id[128]={0};      /*汇卡证书ID*/
        char    oper_no[128]={0};        /*操作员*/
        char    orgappid[128]={0};      /*机构应用ID*/
        char    orgappver[128]={0};      /*机构应用版本*/
        char    devid[128]={0};          /*设备唯一ID*/
	char	order_no[128]={0};
        char    Rsa_encryption_data[10000]={0};  /*RSA加密数据*/
        char    Rsa_encryption_data_hex[10000]={0};  /*RSA加密数据*/
        /*解出报文头*/
        if(message_len < 65)
        {
                UppleLog2(__FILE__,__LINE__,"##API_解包模块--报文长度异常!!\n");
                return -1;
        }
        TUppleAPIHead UppleAPIHead;
        UppleSetMessageAPIHead(&UppleAPIHead,tmpBuf,65);
        UppleLog2(__FILE__,__LINE__,"##API_Head=[%s]\n",UppleAPIHead.API_Head);
        UppleLog2(__FILE__,__LINE__,"##API_ssn_code=[%s]\n",UppleAPIHead.API_ssn_code);
        UppleLog2(__FILE__,__LINE__,"##API_msg_code=[%s]\n",UppleAPIHead.API_msg_code);
        UppleLog2(__FILE__,__LINE__,"##API_send_time=[%s]\n",UppleAPIHead.API_send_time);
        UppleLog2(__FILE__,__LINE__,"##API_return_code=[%s]\n",UppleAPIHead.API_return_code);
        UppleLog2(__FILE__,__LINE__,"##API_err_describe=[%s]\n",UppleAPIHead.API_err_describe);

        /*设置入redis变量*/
        UppleSet2(RPUB,"##API_Head",UppleAPIHead.API_Head);
        UppleSet2(RPUB,"##API_ssn_code",UppleAPIHead.API_ssn_code);
        UppleSet2(RPUB,"##API_msg_code",UppleAPIHead.API_msg_code);
        UppleSet2(RPUB,"##API_send_time",UppleAPIHead.API_send_time);
        UppleSet2(RPUB,"##API_return_code",UppleAPIHead.API_return_code);
        UppleSet2(RPUB,"##API_err_describe",UppleAPIHead.API_err_describe);

        offset += 65;

        /*定长的汇卡证书ID*/
        memset(HK_org_id,0,sizeof(HK_org_id));
        memcpy(HK_org_id,tmpBuf+offset,5);
        UppleLog2(__FILE__,__LINE__,"##API_汇卡证书ID=[%s]\n",HK_org_id);
        offset += 5;
        UppleSet2(RPUB,"##API_HK_org_id",HK_org_id);

        /*操作员*/
        memset(tmp,0,sizeof(tmp));
        memcpy(tmp,tmpBuf+offset,2);
        offset += 2;
        tmp_len = atoi(tmp);
        memset(oper_no,0,sizeof(oper_no));
        memcpy(oper_no,tmpBuf+offset,tmp_len);
        UppleLog2(__FILE__,__LINE__,"##API_操作员=[%s]\n",oper_no);
        offset += tmp_len;
        UppleSet2(RPUB,"##API_oper_no",oper_no);

        /*机构应用ID*/
        memset(orgappid,0,sizeof(orgappid));
        memcpy(orgappid,tmpBuf+offset,tmp_len);
        UppleLog2(__FILE__,__LINE__,"##API_机构应用ID=[%s]\n",orgappid);
        offset += 10;
        UppleSet2(RPUB,"##API_orgappid",orgappid);

        /*机构应用版本*/
        memset(tmp,0,sizeof(tmp));
        memcpy(tmp,tmpBuf+offset,2);
        offset += 2;
        tmp_len = atoi(tmp);
        memset(orgappver,0,sizeof(orgappver));
        memcpy(orgappver,tmpBuf+offset,tmp_len);
        UppleLog2(__FILE__,__LINE__,"##API_机构应用版本ID=[%s]\n",orgappver);
        offset += tmp_len;
        UppleSet2(RPUB,"##API_orgappver",orgappver);

        /*设备唯一ID*/
        memset(tmp,0,sizeof(tmp));
        memcpy(tmp,tmpBuf+offset,2);
        offset += 2;
        tmp_len = atoi(tmp);
        memset(devid,0,sizeof(devid));
        memcpy(devid,tmpBuf+offset,tmp_len);
        UppleLog2(__FILE__,__LINE__,"##API=设备唯一ID[%s]\n",devid);
        offset += tmp_len;
        UppleSet2(RPUB,"##API_devid",devid);

#if 1	/*2015-11-03增加订单号*/
	memset(tmp,0,sizeof(tmp));
	memcpy(tmp,tmpBuf+offset,2);
        offset += 2;
        tmp_len = atoi(tmp);
	if(tmp_len == 20){
		memset(order_no,0,sizeof(order_no));
		memcpy(order_no,tmpBuf+offset,tmp_len);
		UppleLog2(__FILE__,__LINE__,"##API=汇卡订单号[%s]\n",order_no);
        	offset += tmp_len;
        	UppleSet2(RPUB,"##API_order_no",order_no);
	}else if(tmp_len == 0){
		UppleLog2(__FILE__,__LINE__,"##需要前置生成订单\n");
		int ntype = 1;/*前置生成订单*/
		char	fld034[30]={0};
		setsemvalue(DBSEMKEY);
		createorder(ntype,fld034,sizeof(fld034));
		UppleSet2(RPUB,"##API_order_no",fld034);
	}else {
		UppleLog2(__FILE__,__LINE__,"##订单号数据异常!\n");
		return -1;
	}
#endif

        /*RSA加密数据*/
        memset(tmp,0,sizeof(tmp));
        memcpy(tmp,tmpBuf+offset,4);
        offset += 4;
        tmp_len = atoi(tmp);
        memset(Rsa_encryption_data,0,sizeof(Rsa_encryption_data));
        memcpy(Rsa_encryption_data,tmpBuf+offset,tmp_len);
        //UppleMemLog2(__FILE__,__LINE__,"##API_RSA加密数据\n",Rsa_encryption_data,tmp_len);
        offset += tmp_len;

#if 0	/*上来的数据是ASCII，需转成HEX*/
        //终端送上来可见字符，需要压缩一下
        len_hex = aschex_to_bcdhex(Rsa_encryption_data,tmp_len,Rsa_encryption_data_hex);
        //UppleSet2B(RPUB,"##API_Rsa_En_data",Rsa_encryption_data,tmp_len);
        UppleSet2B(RPUB,"##API_Rsa_En_data",Rsa_encryption_data_hex,len_hex);
#else /*上来的是base64转码数据，需要转成HEX*/
	int	de_base64_len = 0;
	char	*pde_base64_data = NULL;
	int	ret = -1;

	UppleLog2(__FILE__,__LINE__,
			"##url_salf_data=[%s]\n",Rsa_encryption_data);

	if((ret = url_b64_salf2unsalf(Rsa_encryption_data,&de_base64_len)) < 0){
		UppleLog2(__FILE__,__LINE__,"##转码base64数据失败[urlsalf2urlunsalf]!\n");
                return -1;
	}

	UppleLog2(__FILE__,__LINE__,
			"##url_unsalf_data=[%s]\n",Rsa_encryption_data);

	pde_base64_data = base64_decrypt(Rsa_encryption_data,&de_base64_len);

	UppleLog2(__FILE__,__LINE__,"##API_BASE64转换,数据长度=[%d]\n",de_base64_len);

	if(pde_base64_data != NULL){
		memcpy(Rsa_encryption_data_hex,pde_base64_data,de_base64_len);
		UppleSet2B(RPUB,"##API_Rsa_En_data",Rsa_encryption_data_hex,de_base64_len);
		free(pde_base64_data);
		pde_base64_data = NULL;
	}else{
		UppleLog2(__FILE__,__LINE__,"##转码base64数据失败!\n");
		return -1;
	}
#endif

        return 1;
}

int     UppleCheakRsaDecryDataLegitimacy(unsigned char *Data,int DataLen)
{
        if(Data == NULL || DataLen <= 0)
        {
                UppleLog2(__FILE__,__LINE__,"in UppleCheakRsaDecryDataLegitimacy::param error!!\n");
                return (errCodeParameter);
        }
        int     sum_or = 0;
        int     sum_eq = 0;
        int     i = 0;
        for(;i<DataLen;i++)
        {
                if(Data[i] == '&')
                        sum_or++;
                if(Data[i] == '=')
                        sum_eq++;
        }
        if((sum_or != (sum_eq - 1)) || (sum_or == 0) )
        {
                UppleLog2(__FILE__,__LINE__,"in UppleCheakRsaDecryDataLegitimacy::key-value structure error!!\n");
                return -1;
        }
        UppleLog2(__FILE__,__LINE__,"in UppleCheakRsaDecryDataLegitimacy::check ok!\n");
        return 1;
}

int     UppleUnPackTheKeyValueAndSetRedis(unsigned char *Data,int DataLen)
{
        if(Data == NULL || DataLen <= 0)
        {
                UppleLog2(__FILE__,__LINE__,"in UppleUnPackTheKeyValueAndSetRedis::param error!!\n");
                return (errCodeParameter);
        }
        char    tmp_Buffer[8192]={0};
        int     ret = -1;
        memcpy(tmp_Buffer,Data,DataLen);

        KeyValue  *pkeyHead;/*解析汇卡返回数据*/
        /*申请动态内存，需要手动释放*/
        pkeyHead = (KeyValue *)malloc(sizeof(KeyValue));
        pkeyHead->next = NULL;
        pkeyHead->Value = NULL;
        pkeyHead->ValueName = NULL;

        if(( ret = UnPackStrtoKeyValue(tmp_Buffer,pkeyHead)) < 0)
        {
                UppleLog2(__FILE__,__LINE__,"解RSA明文数据包失败\n");
                DropList(pkeyHead);
                pkeyHead = NULL;
                return -1;
        }

        struct KeyValue *pNext;
        pNext = pkeyHead;

        for (;pNext != NULL; pNext = pNext->next)
        {
                if((pNext->ValueName != NULL)&&(pNext->Value != NULL))
                {
                        UppleSet2(RPUB,pNext->ValueName,pNext->Value);
                        UppleLog2(__FILE__,__LINE__,"in UppleUnPackTheKeyValueAndSetRedis::key[%s],Value[%s]\n",pNext->ValueName,pNext->Value);
                #if 0
                        char    tmp[1024]={0};
                        UppleGet2(RPUB,pNext->ValueName,tmp);
                        UppleLog2(__FILE__,__LINE__,"##测试Redis取值,Key[%s],Value[%s]\n",pNext->ValueName,tmp);
                #endif
                }
        }

        DropList(pkeyHead);
        pkeyHead = NULL;
        return 1;
}

int     UpplePackAPIHeadReturn(char *Data,int *len)
{
        if(Data == NULL)
        {
                UppleLog2(__FILE__,__LINE__,"in UpplePackAPIHeadReturn::param error!!");
                return (errCodeParameter);
        }
        //UppleSet2(RPUB,"##API_Head",UppleAPIHead.API_Head);
        char    tmp[256]={0};
        int     offset = 0;

        memset(Data,0,sizeof(Data));
        memcpy(Data+offset,"0000",4);
        offset += 4;

        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"##API_ssn_code",tmp);
        if(strlen(tmp) != 24)
                sprintf(Data+offset,"000000000000000000000000",24);
        else
                memcpy(Data+offset,tmp,24);
        offset += 24;

        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"##API_msg_code",tmp);
        if(strlen(tmp) != 4)
                sprintf(Data+offset,"0000",4);
        else
                memcpy(Data+offset,tmp,4);
        offset += 4;

        memset(tmp,0,sizeof(tmp));
        UppleGetFullSystemDateTime(tmp);
        if(strlen(tmp) != 14)
                sprintf(Data+offset,"00000000000000",14);
        else
                memcpy(Data+offset,tmp,14);
        offset += 14;

        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"API_Error_Code",tmp);
        if(strlen(tmp) <= 0)
                sprintf(Data+offset,"A99",3);
        else
                memcpy(Data+offset,tmp,3);
        offset += 3;

#if 0           
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"##API_err_describe",tmp);
        if(strlen(tmp) == 0)
                sprintf(Data+offset,"0000000000000000",16);
        else
                memcpy(Data+offset,tmp,16);
        offset += 16;
#else
        char    error_code[8]={0};
        memcpy(error_code,tmp,strlen(tmp));
        memset(tmp,0,sizeof(tmp));
        UppleGetAPIErrorDescribe(error_code,tmp);
        if(strlen(tmp) == 0)
                sprintf(Data+offset,"0000000000000000",16);
        else
                memcpy(Data+offset,tmp,16);
        offset += 16;
#endif

        *len = offset;

        return 1;
}


int     UpplePackAPIBodyOfRePrint(char *Data,int *len)
{
        if(Data == NULL)
        {
                UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfRePrint::param error!!");
                return (errCodeParameter);
        }

        char    tmp_body[1024]={0};
        char    tmp[1024]={0};
        int     offset = 0;
        int     sub_len = 0;

#if 0   
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"outorder",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::outorder[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,32);
        offset += 32;
#else
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"outorder",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfRePrint::outorder[%s]\n",tmp);
        sub_len = strlen(tmp);
        sprintf(tmp_body+offset,"%02d",sub_len);
        offset += 2;
        memcpy(tmp_body+offset,tmp,sub_len);
        offset += sub_len;
#endif

        /*汇卡订单号*/
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"HK_Order_No",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfRePrint::HK_Order_No[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,20);
        offset += 20;

#if 0
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"cashier",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::cashier[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,32);
        offset += 32;
#else
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"##API_oper_no",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfRePrint::cashier[%s]\n",tmp);
        sub_len = strlen(tmp);
        sprintf(tmp_body+offset,"%02d",sub_len);
        offset += 2;
        memcpy(tmp_body+offset,tmp,sub_len);
        offset += sub_len;
#endif


        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"merId",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfRePrint::merId[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,15);
        offset += 15;

        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"tremId",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfRePrint::tremId[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,8);
        offset += 8;

#if 0
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"organNo",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::orgid[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,15);
        offset += 15;
#else
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"organId",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfRePrint::orgid[%s]\n",tmp);
        sub_len = strlen(tmp);
        sprintf(tmp_body+offset,"%02d",sub_len);
        offset += 2;
        memcpy(tmp_body+offset,tmp,sub_len);
        offset += sub_len;
#endif

        memset(Data,0,sizeof(Data));
        memcpy(Data,tmp_body,offset);
        *len = offset;
        return 1;
}


int     UpplePackAPIBodyOfAuth(char *Data,int *len)
{
        if(Data == NULL)
        {
                UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::param error!!");
                return (errCodeParameter);
        }

        char    tmp_body[1024]={0};
        char    tmp[1024]={0};
        int     offset = 0;
        int     sub_len = 0;

#if 0   
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"outorder",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::outorder[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,32);
        offset += 32;
#else
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"outorder",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::outorder[%s]\n",tmp);
        sub_len = strlen(tmp);
        sprintf(tmp_body+offset,"%02d",sub_len);
        offset += 2;
        memcpy(tmp_body+offset,tmp,sub_len);
        offset += sub_len;
#endif


#if 1	/*增加汇卡的订单号返回 -- 2015-11-12*/
	memset(tmp,0,sizeof(tmp));                                                                                                                   
        UppleGet2(RPUB,"##API_order_no",tmp);                                                                                                              
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::##API_order_no[%s]\n",tmp);                                                                
        sub_len = strlen(tmp);                                                                                                                       
        sprintf(tmp_body+offset,"%02d",sub_len);                                                                                                     
        offset += 2;                                                                                                                                 
        memcpy(tmp_body+offset,tmp,sub_len);                                                                                                         
        offset += sub_len;
#endif

        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"orderAmount",tmp);
        if(strlen(tmp) > 0)
        {
                UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::orderAmount[%s]\n",tmp);
                memcpy(tmp_body+offset,"000000000000",12-strlen(tmp));
                memcpy(tmp_body+offset+12-strlen(tmp),tmp,strlen(tmp));
        }
        offset += 12;

        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"currCode",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::currCode[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,3);
        offset += 3;

#if 0
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"cashier",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::cashier[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,32);
        offset += 32;
#else
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"##API_oper_no",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::cashier[%s]\n",tmp);
        sub_len = strlen(tmp);
        sprintf(tmp_body+offset,"%02d",sub_len);
        offset += 2;
        memcpy(tmp_body+offset,tmp,sub_len);
        offset += sub_len;
#endif

#if 0
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"defgoodsid",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::defgoodsid[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,32);
        offset += 32;
#else
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"defgoodsid",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::defgoodsid[%s]\n",tmp);
        sub_len = strlen(tmp);
        sprintf(tmp_body+offset,"%02d",sub_len);
        offset += 2;
        memcpy(tmp_body+offset,tmp,sub_len);
        offset += sub_len;
#endif

#if 0
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"transType",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::transType[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,4);
        offset += 4;
#else
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"transType",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::transType[%s]\n",tmp);
        sub_len = strlen(tmp);
        sprintf(tmp_body+offset,"%02d",sub_len);
        offset += 2;
        memcpy(tmp_body+offset,tmp,sub_len);
        offset += sub_len;
#endif

        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"merId",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::merId[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,15);
        offset += 15;

#if 0
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"organNo",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::orgid[%s]\n",tmp);
        memcpy(tmp_body+offset,tmp,15);
        offset += 15;
#else
        memset(tmp,0,sizeof(tmp));
        UppleGet2(RPUB,"organId",tmp);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBodyOfAuth::orgid[%s]\n",tmp);
        sub_len = strlen(tmp);
        sprintf(tmp_body+offset,"%02d",sub_len);
        offset += 2;
        memcpy(tmp_body+offset,tmp,sub_len);
        offset += sub_len;
#endif

        memset(Data,0,sizeof(Data));
        memcpy(Data,tmp_body,offset);
        *len = offset;
        return 1;
}
int     UpplePackAPIBody(char *Data,int *len)
{
        if(Data == NULL)
        {
                UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBody::param error!!");
                return (errCodeParameter);
        }
        int     ret  = -1;
        char    Trans_type[8]={0};
        int     tmp_body_len = 0;
        char    tmp_body[1024]={0};

        UppleGet2(RPUB,"##API_msg_code",Trans_type);
        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBody::Trans_type[%s]\n",Trans_type);

        switch(atoi(Trans_type))
        {
                case 122:
                {
                        if((ret =UpplePackAPIBodyOfAuth(tmp_body,&tmp_body_len)) >0)
                        {
                                //UppleInsertAPIMessageToAuthSer_Info();
                        }
                        break;
                }
                case 124:
                {
                        if((ret =UpplePackAPIBodyOfRePrint(tmp_body,&tmp_body_len)) >0)
                        {
                                //UppleInsertAPIMessageToAuthSer_Info();
                        }
                        break;
                }
                defualt:
                {
                        UppleLog2(__FILE__,__LINE__,"in UpplePackAPIBody::未知的交易类型");
                        return -1;
                }
        }
        memset(Data,0,sizeof(Data));
        memcpy(Data,tmp_body,tmp_body_len);
        *len = tmp_body_len;
        return 1;
}

/*base64转换成URL安全字符串*/
int 	url_b64_unsalf2salf(char *url_un_salf_b64_date,int *out_len)
{
	if(url_un_salf_b64_date == NULL){
		UppleLog2(__FILE__,__LINE__,"##url-base64-unsalf2salf 数据为空!\n");
		return -1;
	}
	int	len = strlen(url_un_salf_b64_date);
	int 	i = 0;
	*out_len = 0;
	for(; i<len ; i++){
		if(url_un_salf_b64_date[i] == '+'){
			url_un_salf_b64_date[i] = '-';
		}
		if(url_un_salf_b64_date[i] == '/'){
                        url_un_salf_b64_date[i] = '_';
                }
		if(url_un_salf_b64_date[i] == '='){
                        url_un_salf_b64_date[i] = '\0';
                }
	}
	*out_len = strlen(url_un_salf_b64_date);
	return *out_len;
}

/*base64从URL安全字符串转为原字符*/
int 	url_b64_salf2unsalf(char *url_salf_b64_date,int *out_len)
{
	if(url_salf_b64_date == NULL){
                UppleLog2(__FILE__,__LINE__,"##url-base64-salf2unsalf 数据为空!\n");
                return -1;
        }
	int len = strlen(url_salf_b64_date);
	int sub_len = len%4;
	int i = 0;
	for(; i<len ; i++){
                if(url_salf_b64_date[i] == '-'){
                        url_salf_b64_date[i] = '+';
                }
                if(url_salf_b64_date[i] == '_'){
                        url_salf_b64_date[i] = '/';
                }
        }
	memcpy(url_salf_b64_date+len,"====",4-sub_len);
	*out_len = len + 4 - sub_len;
	return *out_len;
}

int	UppleCheckAPIAuthData()
{
	char	cacerid[100]={0};
	char	cashier[100]={0};
	char	orgappid[100]={0};
	char	orgappver[100]={0};
	char	devid[100]={0};
	char	version[100]={0};
	char	merId[100]={0};
	char	organId[100]={0};
	char	settleNo[100]={0};
	char	transType[100]={0};
	char	orderAmount[100]={0};
	char	currCode[100]={0};
	char	callBackUrl[500]={0};
	char	resultMode[100]={0};
	char	reserved[128]={0};
	char	charge[100]={0};
	char	outorder[100]={0};
	char	reqtime[100]={0};
	char	orgcerid[100]={0};
	int	ret = -1;
	UppleGet2(RPUB,"cacerid",cacerid);
	UppleGet2(RPUB,"cashier",cashier);
	UppleGet2(RPUB,"orgappid",orgappid);
	UppleGet2(RPUB,"orgappver",orgappver);
	UppleGet2(RPUB,"devid",devid);
	UppleGet2(RPUB,"version",version);
	UppleGet2(RPUB,"merId",merId);
	UppleGet2(RPUB,"organId",organId);
	UppleGet2(RPUB,"settleNo",settleNo);
	UppleGet2(RPUB,"transType",transType);
	UppleGet2(RPUB,"orderAmount",orderAmount);
	UppleGet2(RPUB,"currCode",currCode);
	UppleGet2(RPUB,"callBackUrl",callBackUrl);
	UppleGet2(RPUB,"resultMode",resultMode);
	UppleGet2(RPUB,"reserved",reserved);
	UppleGet2(RPUB,"charge",charge);
	UppleGet2(RPUB,"outorder",outorder);
	UppleGet2(RPUB,"reqtime",reqtime);
	UppleGet2(RPUB,"orgcerid",orgcerid);

	if(strlen(cacerid) > 0){
		if(5 != strlen(cacerid)){
			UppleLog2(__FILE__,__LINE__,"##汇卡证书ID长度错\n");
			return -1;
		}
	}
	if(strlen(cashier) > 0){
		if(strlen(cashier) > 32){
			UppleLog2(__FILE__,__LINE__,"##操作员长度错\n");
                        return -1;
		}
	}
	if(strlen(orgappid) > 0){
		if(strlen(orgappid) > 10){
			UppleLog2(__FILE__,__LINE__,"##机构应用ID长度错\n");
                        return -1;
		}
	}
	if(strlen(orgappver) > 0){
		if(strlen(orgappver) > 32){
			UppleLog2(__FILE__,__LINE__,"##机构应用版本长度错\n");
                        return -1;
		}
	}
	if(strlen(devid) > 0){
		if(strlen(devid) > 64){
			UppleLog2(__FILE__,__LINE__,"##设备唯一ID长度错\n");
                        return -1;
		}
	}
	if(strlen(version) > 0){
		if(strlen(version) > 8){
			UppleLog2(__FILE__,__LINE__,"##接口版本长度错\n");
                        return -1;
		}
	}
	if(strlen(merId) > 0){
		if(15 != strlen(merId)){
			UppleLog2(__FILE__,__LINE__,"##汇卡商户号长度错\n");
                        return -1;
		}
	}
	if(strlen(organId) > 0){
		if(strlen(organId) > 10){
			UppleLog2(__FILE__,__LINE__,"##机构号长度错\n");
                        return -1;
		}
	}
	if(strlen(settleNo) > 0){
		if( 15 != strlen(settleNo)){
			UppleLog2(__FILE__,__LINE__,"##汇卡清算商户号长度错\n");
                        return -1;
		}
	}
	if(strlen(transType) > 0){
		
	}
	if(strlen(orderAmount) > 0){
		if(strlen(orderAmount) > 12){
			UppleLog2(__FILE__,__LINE__,"##订单金额长度错\n");
                        return -1;
		}
	}
	if(strlen(currCode) > 0){
		if(strlen(currCode) > 3){
			UppleLog2(__FILE__,__LINE__,"##货币代码长度错\n");
                        return -1;
		}
	}
	if(strlen(callBackUrl) > 0){
		if(strlen(callBackUrl) > 400){
			UppleLog2(__FILE__,__LINE__,"##推送回调地址长度错\n");
                        return -1;
		}
	}
	if(strlen(resultMode) > 0){
		if(strlen(resultMode) > 1){
			UppleLog2(__FILE__,__LINE__,"##推送模式长度错\n");
                        return -1;
		}
	}
	if(strlen(reserved) > 0){
		if(strlen(reserved) > 100){
			UppleLog2(__FILE__,__LINE__,"##保留域长度错\n");
                        return -1;
		}
	}
	if(strlen(charge) > 0){
		if(strlen(charge) > 1){
			UppleLog2(__FILE__,__LINE__,"##手续费支付方长度错\n");
                        return -1;
		}
	}
	if(strlen(outorder) > 0){
		if(strlen(outorder) > 32){
			UppleLog2(__FILE__,__LINE__,"##外部订单号长度错\n");
                        return -1;
		}
	}
	if(strlen(reqtime) > 0){
		if(strlen(reqtime) > 14){
			UppleLog2(__FILE__,__LINE__,"##请求时间长度错\n");
                        return -1;
		}
	}
	if(strlen(orgcerid) > 0){
		if(strlen(orgcerid) > 15){
			UppleLog2(__FILE__,__LINE__,"##机构证书索引长度错\n");
                        return -1;
		}
	}
	UppleLog2(__FILE__,__LINE__,"##数据长度正常\n");
	return 0;
}
