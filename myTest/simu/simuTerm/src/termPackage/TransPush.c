/*
*	20121221 ADD BY ZJW
*	交易推送 
*/

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>

#include "ylPackage.h"
#include "upplePackage.h"
#include "uppleStr.h"
#include "platFormEnv.h"
#include "platFormPackage.h"

#include "termPackage.h"
#include "specErrCode.h"
#include "transDB.h"

#include "buffer_pool.h"
#include "uppleModuleVar.h"

#include "curl/curl.h"
#include "curl/easy.h"

//char PushBackData[512];
void PushFail(char *PUSHDATA);
/*
*	回调函数，用于接收汇卡的返回
*
*/
size_t CopyData(void *ptr, size_t size, size_t nmemb, void *stream)
{
        memset(stream, 0, sizeof(stream));
        memcpy(stream, ptr, size * nmemb);
        return size * nmemb;
}
#if 1
//测试环境使用：推送地址从附加域取得
int GetPushAddrFormExtraData(char *postaddr)
{
        char extra_data[256];
        char *Addr_start = NULL;
        char *Addr_end = NULL;
        int offset = 0;


        memset(extra_data,0x00,sizeof(extra_data));
        UppleGet2(RPUB, "#extradata", extra_data);
        UppleLog2(__FILE__, __LINE__, "#extra_data =[%s]\n", extra_data);
	UppleLog2(__FILE__,__LINE__,"PushAddrTail = [%s]\n",UppleReadStringTypeRECVar("PushAddrTail"));
        if(strlen(extra_data) > 0 )
        {
                Addr_end = strrchr(extra_data,'+');
                Addr_start = strrchr(extra_data,'#');
                offset = Addr_end - Addr_start;
        UppleLog2(__FILE__, __LINE__, "offset =[%d]\n", offset);
                if(offset > 0)
                {
                        memcpy(postaddr,Addr_start + 1, offset -1 );
			strcat(postaddr,UppleReadStringTypeRECVar("PushAddrTail"));
                        UppleLog2(__FILE__,__LINE__,"postaddr = [%s]\n",postaddr);
                        if(strlen(postaddr) > 0)
                                return 1;
                        else
                                return 0;
                }
                else
                        return 0;
        }
        else
                return 0;

}

#endif



/*
*	使用curl将数据推送到汇卡网
*
*/
int UseCurlTosend(char *PushData,char *PushBackData)
{
	char des_addr[256];
	int ret;
	CURL *curl;
	int PushTimes = 0;
	struct curl_slist *Headers = NULL;

//20130226 ADD BY ZJW
#if 1 
       memset( des_addr, 0x00, sizeof(des_addr) );
       if(GetPushAddrFormExtraData(des_addr) == 0)
       {


#endif

                        memset( des_addr, 0x00, sizeof(des_addr) );
                        sprintf( des_addr, "%s%s", UppleReadStringTypeRECVar("PUSHTRANS_IP"), UppleReadStringTypeRECVar("PUSHTRANS_ADDR") );
}
//20130226 ADD BY ZJW
/*

	memset(des_addr, 0x00, sizeof(des_addr));
        sprintf(des_addr, "%s%s", UppleReadStringTypeRECVar(
                "PUSHTRANS_IP"), UppleReadStringTypeRECVar(
                "PUSHTRANS_ADDR"));
        UppleLog("###des_addr=[%s]\n", des_addr);
*/
        UppleLog("###des_addr=[%s]\n", des_addr);
PushAgain:
	curl = curl_easy_init();
        if (curl)
           {
    //          curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.10.34:9080/cgi-bin/entrans");
              curl_easy_setopt(curl, CURLOPT_URL, des_addr);
              curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CopyData);
	      curl_easy_setopt(curl, CURLOPT_WRITEDATA, PushBackData);
              curl_easy_setopt(curl, CURLOPT_POST, 1);
              curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
              curl_easy_setopt(curl, CURLOPT_HEADER, 0);
              Headers = curl_slist_append(Headers,
              "Content-Type: text/html");
             curl_easy_setopt(curl, CURLOPT_HTTPHEADER, Headers);
           }
        else
           {
           exit(-1);
           }

	memset(PushBackData,0x00,sizeof(PushBackData));
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, PushData);
        if ((ret = curl_easy_perform(curl)) < 0)
        {
           UppleUserErrLog2(__FILE__, __LINE__,
                                "\n----------\nsend to server Error!\n----------\n");
                                        return ret;
                                }

	UppleLog2(__FILE__,__LINE__,"PUSHBACK = [%s]\n",PushBackData);
	
	if(strncmp(PushBackData,"00",2) == 0)
	{
		UppleLog2(__FILE__,__LINE__,"PUSH SUCCESS!\n");
	}
	else
	{
		PushTimes ++;
		UppleLog2(__FILE__,__LINE__,"第[%d]次 PUSH FAILED!\n",PushTimes);
		if(PushTimes < 3)
		{
			sleep(3);
			goto PushAgain; 
		}
		UppleLog2(__FILE__,__LINE__,"  ERR MAP: \n01:该交易已存在!\n02:商户号或者订单号格式错误!\n03:商户号或订单号为空!\n04:MAC校验失败!\n08:数据发送成功,数据异常\n");

	//	PushFail(PushData);
	}

	if(curl != NULL)
	{	
		UppleLog2(__FILE__,__LINE__,"关闭连接!!\n");
		curl_easy_cleanup(curl);
		curl == NULL;
	}

	return 0;
}

/*
*	得到账户类型
*/
int GetAccountType(char *AccountType)
{
	char type[3];
	char b3[6 + 1];
	char orgB3[6 + 1];
	char channel[6 + 1] = {0};
	memset(type,0x00,sizeof(type));
	memset(b3,0x00,sizeof(b3));

	UppleGet(  "type",  type);
	UppleGet( "b3_pcode",  b3 );
	UppleGet2(RPUB, "#Channel",channel );
	UppleGet2(RPUB, "orgPB3",orgB3 );
	if(strlen(type) == 1)
	{
		if(strncmp(type,"E",1) == 0)
		{
			if(strncmp(b3,"010000",6) == 0)
			strcpy(AccountType,"12");
			else	
			strcpy(AccountType,"11");
		}
		else if(strncmp(type,"C",1) == 0)
			strcpy(AccountType,"00");
		else
			strcpy(AccountType,"01");
	}
	else
			strcpy(AccountType,"01");

	if(0 == strncmp(channel,"3006",4 )){
	
		if((strncmp(b3,"380000",6) == 0) && (0 == strncmp(orgB3,"170000",6)))
			strcpy(AccountType,"04");
		if((strncmp(b3,"380000",6) == 0) && (0 == strncmp(orgB3,"280000",6)))
			strcpy(AccountType,"05");
		if(strncmp(b3,"280000",6) == 0)
			strcpy(AccountType,"05");


	}

	UppleLog2(__FILE__,__LINE__,"Type = [%s]\n",type);	
	UppleLog2(__FILE__,__LINE__,"AccountType = [%s]\n",AccountType);	
}
/*
*	得到交易类型
*/
int GetTransType(char *TransType)
{
	char b0[5];
	char b3[7];
	char orgB3[7] = {0};
	char type[3];
	char b23[4 + 1];
	char b55[255 + 1];
	int IsIC = 0;


	memset(b0,0x00,sizeof(b0));
	memset(b3,0x00,sizeof(b3));
	memset(type,0x00,sizeof(type));
	memset(b23,0x00,sizeof(b23));
	memset(b55,0x00,sizeof(b55));

	UppleGet2( RPUB, "TB0",  b0 );
	if(0 == strlen(b0)){
		UppleGet2( RPUB, "PB0",  b0 );
	}
	UppleGet2(RPUB, "orgPB3",orgB3 );
	UppleGet( "b3_pcode",  b3 );
	UppleGet(  "type",  type);
	UppleGet2( RPUB, "TB23",  b23 );
	UppleGet2( RPUB, "TB55",  b55 );
	UppleLog2(__FILE__,__LINE__,"b0 = [%s],b3 = [%s],type = [%s] b23 = [%s],b55 = [%s],orgB3 = [%s] \n",b0,b3,type,b23,b55,orgB3);
	
	if(strlen(b23) > 0 || strlen(b55) > 0 )
		IsIC = 1;
	else
		IsIC = 0;
	UppleLog2(__FILE__,__LINE__,"IsIC = [%d]\n",IsIC);
	/*
	if(strncmp(b0,"0210",4) == 0)
	{
		if(strncmp(b3,"510000",6) == 0 || strncmp(b3,"000000",6) == 0 || strncmp(b3,"010000",6) == 0 || strncmp(b3,"500000",6) == 0)
			strncpy(TransType,"14",2);      				//银行卡消费
		if(strncmp(b3,"280000",6) == 0 || strncmp(b3,"200000",6) == 0 || strncmp(b3,"220000",6) == 0)
		{
			if((strlen(type) == 1) && (strncmp(type,"E",1) == 0))
				strncpy(TransType,"24",2);				//E卡撤销
			else
				strncpy(TransType,"15",2);				//银行卡撤销
		}
		if(strlen(type) == 2 && strncmp(type,"ET",2) == 0)	
			strncpy(TransType,"25",2);
	}
	else if(strncmp(b0,"0410",4) == 0)
	{
		strncpy(TransType,"09",2);						//冲正
	}
	*/
	if(strncmp(b0,"0210",4) == 0 || strncmp(b0,"0200",4) == 0)
	{
		switch(type[0])
		{
			case 'T':
			if(strncmp(b3,"810000",6) == 0)
				strcpy(TransType,"052");
			if(strncmp(b3,"000000",6) == 0)
                                strcpy(TransType,"14");
                        else if(strncmp(b3,"840000",6) == 0)//20130723 增加物流POS消费撤销(脱机订单)
                                strcpy(TransType,"15");
                        else if(strncmp(b3,"850000",6) == 0)//20130805 增加物流POS消费撤销(联机订单)
                                strcpy(TransType,"15");
                        else if(strncmp(b3,"500000",6) == 0)//20131106 增加乐纷期消费
                                strcpy(TransType,"27");	
                        else if(strncmp(b3,"530000",6) == 0)//20131106 增加乐纷期撤销
                                strcpy(TransType,"28");
				break;
			case 'E':
			if(strncmp(b3,"820000",6) == 0)
				strcpy(TransType,"053");
			else if((strncmp(b3,"000000",6) == 0) || (strncmp(b3,"010000",6) == 0))
				strcpy(TransType,"23");
			else if(strncmp(b3,"200000",6) == 0)
				strcpy(TransType,"24");
			else if(strncmp(b3,"710000",6) == 0) //20140311 增加E卡现金充值
				strcpy(TransType,"29");
				break;
			case 'C':
			if(strncmp(b3,"830000",6) == 0)
				strcpy(TransType,"051");
				break;
			case 'g':
			if(IsIC)
			{
				if(strncmp(b3,"000000",6) == 0)
					strcpy(TransType,"06");
                                else if(strncmp(b3,"200000",6) == 0)
                                        strcpy(TransType,"07");
				else if(strncmp(b3,"310000",6) == 0)
                                        strcpy(TransType,"08");
			}
			else
			{
				if(strncmp(b3,"000000",6) == 0)
					strcpy(TransType,"14");
				else if(strncmp(b3,"200000",6) == 0)
					strcpy(TransType,"15");
				else if(strncmp(b3,"310000",6) == 0)
                                        strcpy(TransType,"08");
			}
				break;
			default:
			if(IsIC)
			{
				if(strncmp(b3,"000000",6) == 0 || strncmp(b3,"510000",6) == 0)
					strcpy(TransType,"06");
                                else if(strncmp(b3,"200000",6) == 0 || strncmp(b3,"280000",6) == 0)
                                        strcpy(TransType,"07");
			}
			else
			{
			if(strncmp(b3,"280000",6) == 0 || strncmp(b3,"200000",6) == 0)
				strcpy(TransType,"15");
			else if(strncmp(b3,"510000",6) == 0 || strncmp(b3,"000000",6) == 0)
				strcpy(TransType,"14");
			}
				break;
		}
		if(strlen(type) == 2)
		{
			if(strncmp(type,"ET",2) == 0)
			{
				strcpy(TransType,"25");
			}
		}
	}
	else if(strncmp(b0,"0410",4) == 0)
	{
		if(strncmp(b3,"200000",6) == 0)
		strcpy(TransType,"26");
		else
		strcpy(TransType,"09");
	}else if( strncmp(b0,"0710",4) == 0 || strncmp(b0,"0700",4) == 0){

		if((strncmp(b3,"380000",6) == 0) && (0 == strncmp(orgB3,"170000",6)))
			strcpy(TransType,"32");
		if((strncmp(b3,"380000",6) == 0) && (0 == strncmp(orgB3,"280000",6)))
			strcpy(TransType,"33");
		if(strncmp(b3,"280000",6) == 0)
			strcpy(TransType,"33");
	}
	return 0;
}
/*
*	得到渠道号
*/
int GetPayChannel(char *PayChannel)
{
	char Channel[5];
	char type[3];
	
	memset(Channel,0x00,sizeof(Channel));
	memset(type,0x00,sizeof(type));
	
	UppleGet(  "type",  type);
	UppleLog2(__FILE__,__LINE__,"type = [%s]\n",type);
	
	if(strlen(type) == 1 )
	{

		if(strncmp(type,"L",1) == 0)
		{
			strncpy(PayChannel,"L",1);
			goto END;
		}
		else if(strncmp(type,"A",1) == 0)
		{
			strncpy(PayChannel,"D",1);
			goto END;
		}
		/*
		else if(strncmp(type,"g",1) == 0)
		{
			strncpy(PayChannel,"A",1);
                        goto END;
		}
		*/
		/*
		switch(type)
		{
			case 'L':
				strcpy(TransType,"L",1);
                        	goto END;
			case 'g':
				strcpy(TransType,"A",1);
                        	goto END;
		}
		*/	
	}	
	UppleGet2( RPUB, "#Channel",Channel);
	UppleLog2(__FILE__,__LINE__,"channel = [%s]\n",Channel);
	switch(atoi(Channel))
	{
		case 1001:
			strncpy(PayChannel,"G",1);
			break;
		case 1002:
			strncpy(PayChannel,"B",1);
			break;
		case 1003:
			strncpy(PayChannel,"N",1);
			break;
		case 1007:
			strncpy(PayChannel,"Z",1);
			break;
		case 1008:
			strncpy(PayChannel,"A",1);
			break;
		case 1009:
			strncpy(PayChannel,"F",1);
			break;
		case 2001:
			strncpy(PayChannel,"K",1);
			break;
		case 6001:
		case 6003:
			strncpy(PayChannel,"I",1);
                        break;
		case 6002:
			strncpy(PayChannel,"X",1);
		case 3006:
			strncpy(PayChannel,"W",1);
			break;
		case 8001:
			strncpy(PayChannel,"O",1);
                        break;
		defult :
			strncpy(PayChannel,"G",1);
			break;
	}
END:
	return 0;
	
}
/*
*	获取批次号
*/

int GetBatchNo(char *BatchNo)
{
	char b60[103 + 1];
	char b63[515 + 1];
	char type[3];

	memset(b60,0x00,sizeof(b60));
	memset(b63,0x00,sizeof(b63));
	memset(type,0x00,sizeof(type));	

	UppleGet(  "type",  type);
	if(strncmp(type,"E",1) == 0)
	{
		UppleGet("EBatchNo",BatchNo); 
	}
	else
	{	
		UppleGet("##b60",b60);	
		if(strlen(b60) < 8){
			strcpy(BatchNo,"000001");
		}else
			memcpy(BatchNo,b60 + 2,6);
	}
	/*	20130910 ADD BY ZJW
	*	做电子签购单需要，批次号必须传
	*/
	if(strlen(BatchNo) == 0 || strlen(BatchNo) < 6)
		memcpy(BatchNo,"000001",6);

	return 0;
}
/*
*	消费撤销的时候，获取原流水号、原交易日期
*/
int GetVoidData(char *VoidSn,char *VoidDate)
{
	char b90[42 + 1];
	char b61[256 + 1];

	memset(b90,0x00,sizeof(b90));
	memset(b61,0x00,sizeof(b61));

	UppleGet("PB90",b90);
	if(strlen(b90) > 0)
	{
		memcpy(VoidSn,b90 + 4,6);
		memcpy(VoidDate,b90 + 4 + 6,4);
	}else{
		UppleGet2(RPUB, "##b61", b61);
		UppleGet2(RPUB, "PB13", VoidDate);
		if(strlen(b61) > 0){
		strncpy(VoidSn,b61 + 6,6);
		}

	}
	return 0;

}
/*
*	获取卡有效期
*/
int GetExpCardDate(char *expDate)
{
	char tmpBuf[16 + 1];
	char tmpa[8 + 1];
	char tmpb[8 + 1];
	return 0;

	memset(tmpa, 0x00, sizeof(tmpa));
	memset(tmpb, 0x00, sizeof(tmpb));
	memset(tmpBuf, 0x00, sizeof(tmpBuf));
	UppleGet("#validdate#",tmpBuf);
	
	if (0 < strlen(tmpBuf)){
		memcpy(tmpa, tmpBuf, 2);
		memcpy(tmpb, tmpBuf + 2, 2);
		sprintf(expDate, "20%s/%s", tmpa, tmpb);
	}else
		return -1;


}


/*
*       获取银联商户号，终端号
*/
int	GetSaleMidSaleTid(char *SaleMid,char *SaleTid)
{
	char	tmp[40]={0};
	memset(SaleMid,0,sizeof(SaleMid));
	memset(SaleTid,0,sizeof(SaleTid));
	
	memset(tmp,0,sizeof(tmp));
	UppleGet2(RPUB, "SALEMID", tmp);
	memcpy(SaleMid,tmp,strlen(tmp));

	memset(tmp,0,sizeof(tmp));
	UppleGet2(RPUB, "SALETID", tmp);
	memcpy(SaleTid,tmp,strlen(tmp));
	UppleLog2(__FILE__,__LINE__,"in GetSaleMidSaleTid::SaleMid=[%s],SaleTid=[%s]\n",SaleMid,SaleTid);
	return 1;
}

/*
*	推送失败：设置推送失败标记，并将需要推送的信息插入数据库内
*/
void PushFail(char *PUSHDATA)
{
	char PB0[5];
	
	UppleGet2( RPUB, "PB0",  PB0 );
	if(strcmp(PB0,"0210") == 0)
		PushFalilSetTransStatus(4, '9');//消费推送失败标记TransStatus第5位
	else if(strcmp(PB0,"0410") == 0)
		PushFalilSetTransStatus(5, '9');//冲正推送失败标记TransStatus第6位
	//InsertPushData(PUSHDATA);
}
/*
*	  无论推送成功与否，都将相应的推送数据入库
*/
/*
int PushNote(char *PUSHDATA,char *PushBackData)
{
	SavePushNoteToDB(PUSHDATA,PushBackData);
}
*/
/*
*	查询数据库并拼接数�
*	r0_cmd 		: transpush
*	r1_orderid	: 订单号
*	r2_merchno	: 商户号
*	r3_pan 		: 卡号
*	r4_termid	: 终端号
*	r5_systraceno	: 流水号
*	r6_sysrefno	：系统参考号
*	r7_mac		: MD5校验
*	r8_amount	: 金额
*	r9_transtime	: 交易时间
*	r10_settledat	: 清算日期
*	r11_batch 	: 此交易的批次号
*	r12_expcard	: 卡有效期
*	r13_bank_faka	: 发卡行
*	r14_bank_sodan	: 收单行
*	r15_void_sn	: 原终端流水号
*	r16_void_data	: 原交易日期
*	r17_account_type: 账户类型
*	r18_transtype	: 交易类型
*	r19_pay_channels: 支付渠道
*/


int TransPushToHK(void *GoodInfo)

{
	int ret;	
	char PUSHDATA[4096];
	char PUSHDATA_TABLE[4096];
	char PushBackData[128];

	char OrderNo[20 + 1];
	char MerchNo[15 + 1];
	char Pan[20 + 1];
	char TermId[20 + 1];
	char SysTraceNo[30 + 1];
	char SysRefNo[30 +1];
	char Mac[32 + 1];
	char MacData[512];
	char Amount[12 + 1];
	char TransDate[4 + 1];
	char TransTime[10 + 1];
	char SettleDate[8 + 1];
	char BatchNo[6 + 1];
	char Expcard[14 + 1];
	char BankF[128];
	char BankS[128];
	char VoidSn[6 + 1];
	char VoidDate[10 +1];
	char SALEMID[15 + 1];

	char AccountType[2 + 1];
	char TransType[3 + 1];
	char PayChannel[1 + 1];
	
	char ReturnCode[2 + 1];
	char PrinfMessage[256 + 1];

	char SaleMid[40] = {0};
	char SaleTid[40] = {0};
	//int PushTimes = 0;

	memset(OrderNo,0x00,sizeof(OrderNo));
	memset(MerchNo,0x00,sizeof(MerchNo));
	memset(Pan,0x00,sizeof(Pan));
	memset(TermId,0x00,sizeof(TermId));
	memset(SysTraceNo,0x00,sizeof(SysTraceNo));
	memset(SysRefNo,0x00,sizeof(SysRefNo));
	memset(Mac,0x00,sizeof(Mac));
	memset(MacData,0x00,sizeof(MacData));
	memset(Amount,0x00,sizeof(Amount));
	memset(TransDate,0x00,sizeof(TransDate));
	memset(TransTime,0x00,sizeof(TransTime));
	memset(SettleDate,0x00,sizeof(SettleDate));
	memset(BatchNo,0x00,sizeof(BatchNo));
	memset(Expcard,0x00,sizeof(Expcard));
	memset(BankF,0x00,sizeof(BankF));
	memset(BankS,0x00,sizeof(BankS));
	memset(VoidSn,0x00,sizeof(VoidSn));
	memset(VoidDate,0x00,sizeof(VoidDate));
	memset(PUSHDATA,0x00,sizeof(PUSHDATA));
	memset(PUSHDATA_TABLE,0x00,sizeof(PUSHDATA_TABLE));
	memset(AccountType,0x00,sizeof(AccountType));
	memset(TransType,0x00,sizeof(TransType));
	memset(PayChannel,0x00,sizeof(PayChannel));
	memset(ReturnCode,0x00,sizeof(ReturnCode));
	memset(SALEMID,0x00,sizeof(SALEMID));
	memset(PrinfMessage,0x00,sizeof(PrinfMessage));
	
	UppleGet("PB39",ReturnCode);
	UppleLog2(__FILE__,__LINE__,"ReturnCode = [%s]\n",ReturnCode);
	if(memcmp(ReturnCode,"00",2) != 0)
	{
		memset(ReturnCode,0x00,sizeof(ReturnCode));
		UppleGet2(RPUB,"PB39",ReturnCode);
		if(memcmp(ReturnCode,"00",2) != 0)
		{
		UppleLog2(__FILE__,__LINE__,"39域=[%s]\n",ReturnCode);
		UppleLog2(__FILE__,__LINE__,"交易不成功，不推送！39域=[%s]\n",ReturnCode);
		return -1;
		}

	}	
//test

	UppleLog2(__FILE__,__LINE__,"GOODSIFO = [%s]\n",GoodInfo);

//test



	if(TransPushOrderRead() <= 0)
		return 0;			//查询数据库
	UppleGet( "##OrderNo",OrderNo   );
        UppleLog2(__FILE__,__LINE__,"OrderNo=[%s]\n",OrderNo );
	UppleGet( "PB42",MerchNo);
	/*工行做兼容,2014-10-28*/
	char    tmp[30+1]={0};
	if(strlen(MerchNo) <= 0)
	{
		UppleGet2( RPUB,"XB41",tmp);
		if(strlen(tmp) > 3)
		{
			memcpy(MerchNo,tmp,strlen(tmp)-3);
		}
	}
        UppleLog2(__FILE__,__LINE__,"MerchNo = [%s]\n",MerchNo );	
	UppleGet( "PB2",Pan);
        UppleLog2(__FILE__,__LINE__,"Pan = [%s]\n",Pan);	
	UppleGet( "PB41",TermId);
        UppleLog2(__FILE__,__LINE__,"TermId = [%s]\n",TermId);	
	UppleGet( "PB11",SysTraceNo);
        UppleLog2(__FILE__,__LINE__,"SysTraceNo = [%s]\n",SysTraceNo);	
	UppleGet( "PB37",SysRefNo);
        UppleLog2(__FILE__,__LINE__,"SysRefNo = [%s]\n",SysRefNo);	
	UppleGet( "PB4",Amount);
	if( 0 == atoi(Amount))
		UppleGet2(RPUB, "orgPB4",Amount);
        UppleLog2(__FILE__,__LINE__,"Amount = [%s]\n",Amount);	
	UppleGet( "##trans_date",TransDate);
        UppleLog2(__FILE__,__LINE__,"TransDate = [%s]\n",TransDate);	
	UppleGet( "##trans_time",TransTime);
        UppleLog2(__FILE__,__LINE__,"trans_time = [%s]\n",TransTime);	
	UppleGet( "PB15",SettleDate);
        UppleLog2(__FILE__,__LINE__,"SettleDate = [%s]\n",SettleDate);	
	UppleGet( "##bank_inst_name",BankF);
        UppleLog2(__FILE__,__LINE__,"BankF = [%s]\n",BankF);	
	UppleGet( "##rec_bank_name",BankS);
	/*
	*	20130910 ADD BY ZJW
	*	电子签购单数据，推送必填收单行
	*/
	if(strlen(BankS) == 0){
		UppleGet( "SALEMID",SALEMID);
		GetRec_Bank_Name(SALEMID,BankS);
	}
        UppleLog2(__FILE__,__LINE__,"BankS = [%s]\n",BankS);	



	sprintf(MacData,"transpush%s%s%s%s%s%s%s%s%s123",OrderNo,MerchNo,Pan,TermId,SysTraceNo,SysRefNo,Amount,TransDate,TransTime);
	GetMD5(MacData,Mac);

	UppleLog2(__FILE__,__LINE__,"Macdata = [%s]\n mac = [%s]\n",MacData,Mac);
	
	GetAccountType(AccountType);
	GetTransType(TransType);
	GetPayChannel(PayChannel);
	GetBatchNo(BatchNo);
	GetVoidData(VoidSn,VoidDate);
	GetExpCardDate(Expcard);
	GetSaleMidSaleTid(SaleMid,SaleTid);
	UppleGet2(RPUB,"##PRINT",PrinfMessage);
	UppleLog2(__FILE__,__LINE__,"PrinfMessage = [%s]\n",PrinfMessage);
		
	UppleLog2(__FILE__,__LINE__,"AccountType = [%s],TransType = [%s],PayChannel = [%s] \n",AccountType,TransType,PayChannel);
	
	sprintf(PUSHDATA,"r0_cmd=transpush&r1_orderid=%s&r2_merchno=%s&r3_pan=%s&r4_termid=%s&r5_systraceno=%s&r6_sysrefno=%s&r7_mac=%s&r8_amount=%s&r9_transtime=%s%s&r10_settledate=%s&r11_batchno=%s&r12_expcard=%s&r13_bank_faka=%s&r14_bank_sodan=%s&r15_void_sn=%s&r16_void_date=%s&r17_account_type=%s&r18_transtype=%s&r19_pay_channels=%s&r20_pntylqb=%s&r30_goodsInf=%s&r21_transmid=%s&r22_transtid=%s",
		OrderNo,
		MerchNo,
		Pan,
		TermId,
		SysTraceNo,
		SysRefNo,
		Mac,
		Amount,
		TransDate,
		TransTime,
		SettleDate,
		BatchNo,
		Expcard,			
		BankF,
		BankS,	
		VoidSn,
		VoidDate,
		AccountType,
		TransType,
		PayChannel,
		PrinfMessage,
		GoodInfo,
		SaleMid,
		SaleTid	
		);
	strncpy(PUSHDATA_TABLE,PUSHDATA,strlen(PUSHDATA));
	UppleLog2(__FILE__,__LINE__,"PUSHDATA = [%s]\n",PUSHDATA_TABLE);
	UseCurlTosend(PUSHDATA,PushBackData);
	UppleLog2(__FILE__,__LINE__,"PUSHDATA = [%s]\n",PUSHDATA);
	UppleLog2(__FILE__,__LINE__,"PushBackData = [%s]\n",PushBackData);
//生产入库暂时不上
//	PushNote(PUSHDATA,PushBackData,TransType);
//	ReadTransPush();
        return 0;
}
//zzq add 2015-05-25
//微信结果推送
int WxPushToHk()
{
        char ctranstype[10]={0};
        UppleGet2(RPUB,"wx_transtype",ctranstype);
        int ret1 = atoi(ctranstype);
        char creturncode[20]={0};
        UppleGet2(RPUB,"wx_returncode",creturncode);//结果
        char ctransresult[20]={0};//业务结果
        UppleGet2(RPUB,"wx_transresult",ctransresult);
        UppleLog2(__FILE__,__LINE__,"transtype=[%s],returncode=[%s],transresult=[%s]\n",ctranstype,creturncode,ctransresult);
        int ret2 = strcmp(creturncode,"SUCCESS");
        int ret6 = strcmp(ctransresult,"SUCCESS");
        char ctradestate[20]={0};
        int ret3 = -1;
        int ret4 = -1;
        int ret5 = -1;
        UppleGet2(RPUB,"wx_tradestate",ctradestate);//支付状态
        UppleLog2(__FILE__,__LINE__,"支付状态=[%s]\n",ctradestate);
        ret3 = strcmp(ctradestate,"SUCCESS");//成功
        ret4 = strcmp(ctradestate,"REFUND"); //退款
        ret5 = strcmp(ctradestate,"REVOKED"); //已撤销
        //状态码和业务结果都必须返回成功才需要推送
        if((200==ret1 || 300==ret1) && (0 == ret2 && 0 == ret6)){
                if(300 == ret1 && (0 != ret3 && 0 != ret4 && 0 != ret5)){
                        UppleLog2(__FILE__,__LINE__,"不需要推送\n");
                        return 0;
                }
                UppleLog2(__FILE__,__LINE__,"需要推送到汇卡\n");
                char corderno[21]={0};
                UppleGet2(RPUB,"wx_hkorder",corderno);
                //从数据库中读取数据
                char cwhere[100]={0};
                snprintf(cwhere,sizeof(cwhere),"hk_orderno='%s' and hk_transtype='200'",corderno);
                char dbRecord[1024]={0};
                if (UppleSelectRealDBRecord("wx_trans_jnl", "hk_mchid,total_fee,wx_orderno,time_end",cwhere, dbRecord, sizeof(dbRecord),NULL) < 0) {
                        //没有记录
                        UppleLog2(__FILE__,__LINE__,"记录没有找到\n");
                        return -1;
                }
                //商户号
                char chkmchno[16]={0};
                if (UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),"hk_mchid", chkmchno,sizeof(chkmchno)) < 0) {
                        UppleUserErrLog2(__FILE__, __LINE__,"  fldName [hk_mchid] not found!\n");
                        return -1;
                }
                //总金额 元为单位
                char cmoney[20]={0};
                if (UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),"total_fee",cmoney,sizeof(cmoney)) < 0) {
                        UppleUserErrLog2(__FILE__, __LINE__,"  fldName [total_fee] not found!\n");
                        return -1;
                }
		double dmoney = atof(cmoney);
		//推送以分为单位
		dmoney *= 100;
		memset(cmoney,0x00,sizeof(cmoney));
		snprintf(cmoney,sizeof(cmoney),"%.0f",dmoney);
                //微信订单号
                char cwxorderno[32]={0};
                if (UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),"wx_orderno",cwxorderno,sizeof(cwxorderno)) < 0) {
			UppleUserErrLog2(__FILE__, __LINE__,"  fldName [wx_orderno] not found!\n");
                        return -1;
                }
                //交易时间
                char ctimeend[20]={0};
                if (UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),"time_end", ctimeend,sizeof(ctimeend)) < 0) {
                        UppleUserErrLog2(__FILE__, __LINE__,"  fldName [time_end] not found!\n");
                        return -1;
                }
                //区分主扫还是被扫 需要检查订单号+商户号+100 是否有记录 如果有 则表明是主扫 否则被扫
                char ctradetype[5]={0};
                memset(cwhere,0x00,sizeof(cwhere));
                snprintf(cwhere,sizeof(cwhere),"hk_orderno='%s' and hk_mchid='%s' and hk_transtype='100'",corderno,chkmchno);
                memset(dbRecord,0x00,sizeof(dbRecord));
                char caccount[3]={0};
                if (UppleSelectRealDBRecord("wx_trans_jnl","*",cwhere, dbRecord, sizeof(dbRecord),NULL) < 0) {
                        //没有记录
                        UppleLog2(__FILE__,__LINE__,"没有找到记录,被扫支付\n");
                        memcpy(ctradetype,"31",2);
                        memcpy(caccount,"03",2);
                }else{
                        UppleLog2(__FILE__,__LINE__,"找到记录,主扫支付\n");
                        memcpy(ctradetype,"30",2);
                        memcpy(caccount,"02",2);
                }

                char cmd5data[100]={0};
                //订单号补齐20位
                char ctemp[21]={0};
                memset(ctemp,'0',20);
                int norder = strlen(corderno);
                memcpy(ctemp+20-norder,corderno,norder);
                memset(corderno,0x00,sizeof(corderno));
                memcpy(corderno,ctemp,strlen(ctemp));
                //20位订单号+商户号+金额+交易时间
                snprintf(cmd5data,sizeof(cmd5data),"transpush%s%s%s%s123",corderno,chkmchno,cmoney,ctimeend+4);
                char cmd5[33]={0};
                GetMD5(cmd5data,cmd5);
                UppleLog2(__FILE__,__LINE__,"md5data=[%s],md5=[%s]\n",cmd5data,cmd5);
                //组发送数据
                char cpushdata[1024]={0};
                snprintf(cpushdata,sizeof(cpushdata),"r0_cmd=transpush&r1_orderid=%s&r2_merchno=%s&r3_pan=&r4_termid=&"\
                                "r5_systraceno=&r6_sysrefno=&r7_mac=%s&r8_amount=%s&r9_transtime=%s&r10_settledate=&"\
                                "r11_batchno=&r12_expcard=&r13_bank_faka=&r14_bank_sodan=&r15_void_sn=&r16_void_date=&"\
                                "r17_account_type=%s&r18_transtype=%s&r19_pay_channels=&r20_pntylqb=&r30_goodsInf=&"\
                                "r21_transmid=&r22_transtid=&r23_wxorderno=%s",corderno,chkmchno,cmd5,cmoney,ctimeend+4,caccount,ctradetype,cwxorderno);

                UppleLog2(__FILE__,__LINE__,"PUSHDATA = [%s]\n",cpushdata);
                char cpushback[128]={0};
                UseCurlTosend(cpushdata,cpushback);
                UppleLog2(__FILE__,__LINE__,"PushBackData = [%s]\n",cpushback);

                return 0;
        }
        UppleLog2(__FILE__,__LINE__,"不需要推送\n");
	return 0;

}
//zzq add end
