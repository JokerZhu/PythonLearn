/*
*	20121221 ADD BY ZJW
*	½»Ò×ÍÆËÍ 
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
*	»Øµ÷º¯Êý£¬ÓÃÓÚ½ÓÊÕ»ã¿¨µÄ·µ»Ø
*
*/
size_t CopyData(void *ptr, size_t size, size_t nmemb, void *stream)
{
        memset(stream, 0, sizeof(stream));
        memcpy(stream, ptr, size * nmemb);
        return size * nmemb;
}
#if 1
//²âÊÔ»·¾³Ê¹ÓÃ£ºÍÆËÍµØÖ·´Ó¸½¼ÓÓòÈ¡µÃ
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
*	Ê¹ÓÃcurl½«Êý¾ÝÍÆËÍµ½»ã¿¨Íø
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
		UppleLog2(__FILE__,__LINE__,"µÚ[%d]´Î PUSH FAILED!\n",PushTimes);
		if(PushTimes < 3)
		{
			sleep(3);
			goto PushAgain; 
		}
		UppleLog2(__FILE__,__LINE__,"  ERR MAP: \n01:¸Ã½»Ò×ÒÑ´æÔÚ!\n02:ÉÌ»§ºÅ»òÕß¶©µ¥ºÅ¸ñÊ½´íÎó!\n03:ÉÌ»§ºÅ»ò¶©µ¥ºÅÎª¿Õ!\n04:MACÐ£ÑéÊ§°Ü!\n08:Êý¾Ý·¢ËÍ³É¹¦,Êý¾ÝÒì³£\n");

	//	PushFail(PushData);
	}

	if(curl != NULL)
	{	
		UppleLog2(__FILE__,__LINE__,"¹Ø±ÕÁ¬½Ó!!\n");
		curl_easy_cleanup(curl);
		curl == NULL;
	}

	return 0;
}

/*
*	µÃµ½ÕË»§ÀàÐÍ
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
*	µÃµ½½»Ò×ÀàÐÍ
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
			strncpy(TransType,"14",2);      				//ÒøÐÐ¿¨Ïû·Ñ
		if(strncmp(b3,"280000",6) == 0 || strncmp(b3,"200000",6) == 0 || strncmp(b3,"220000",6) == 0)
		{
			if((strlen(type) == 1) && (strncmp(type,"E",1) == 0))
				strncpy(TransType,"24",2);				//E¿¨³·Ïú
			else
				strncpy(TransType,"15",2);				//ÒøÐÐ¿¨³·Ïú
		}
		if(strlen(type) == 2 && strncmp(type,"ET",2) == 0)	
			strncpy(TransType,"25",2);
	}
	else if(strncmp(b0,"0410",4) == 0)
	{
		strncpy(TransType,"09",2);						//³åÕý
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
                        else if(strncmp(b3,"840000",6) == 0)//20130723 Ôö¼ÓÎïÁ÷POSÏû·Ñ³·Ïú(ÍÑ»ú¶©µ¥)
                                strcpy(TransType,"15");
                        else if(strncmp(b3,"850000",6) == 0)//20130805 Ôö¼ÓÎïÁ÷POSÏû·Ñ³·Ïú(Áª»ú¶©µ¥)
                                strcpy(TransType,"15");
                        else if(strncmp(b3,"500000",6) == 0)//20131106 Ôö¼ÓÀÖ·×ÆÚÏû·Ñ
                                strcpy(TransType,"27");	
                        else if(strncmp(b3,"530000",6) == 0)//20131106 Ôö¼ÓÀÖ·×ÆÚ³·Ïú
                                strcpy(TransType,"28");
				break;
			case 'E':
			if(strncmp(b3,"820000",6) == 0)
				strcpy(TransType,"053");
			else if((strncmp(b3,"000000",6) == 0) || (strncmp(b3,"010000",6) == 0))
				strcpy(TransType,"23");
			else if(strncmp(b3,"200000",6) == 0)
				strcpy(TransType,"24");
			else if(strncmp(b3,"710000",6) == 0) //20140311 Ôö¼ÓE¿¨ÏÖ½ð³äÖµ
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
*	µÃµ½ÇþµÀºÅ
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
*	»ñÈ¡Åú´ÎºÅ
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
	*	×öµç×ÓÇ©¹ºµ¥ÐèÒª£¬Åú´ÎºÅ±ØÐë´«
	*/
	if(strlen(BatchNo) == 0 || strlen(BatchNo) < 6)
		memcpy(BatchNo,"000001",6);

	return 0;
}
/*
*	Ïû·Ñ³·ÏúµÄÊ±ºò£¬»ñÈ¡Ô­Á÷Ë®ºÅ¡¢Ô­½»Ò×ÈÕÆÚ
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
*	»ñÈ¡¿¨ÓÐÐ§ÆÚ
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
*       »ñÈ¡ÒøÁªÉÌ»§ºÅ£¬ÖÕ¶ËºÅ
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
*	ÍÆËÍÊ§°Ü£ºÉèÖÃÍÆËÍÊ§°Ü±ê¼Ç£¬²¢½«ÐèÒªÍÆËÍµÄÐÅÏ¢²åÈëÊý¾Ý¿âÄÚ
*/
void PushFail(char *PUSHDATA)
{
	char PB0[5];
	
	UppleGet2( RPUB, "PB0",  PB0 );
	if(strcmp(PB0,"0210") == 0)
		PushFalilSetTransStatus(4, '9');//Ïû·ÑÍÆËÍÊ§°Ü±ê¼ÇTransStatusµÚ5Î»
	else if(strcmp(PB0,"0410") == 0)
		PushFalilSetTransStatus(5, '9');//³åÕýÍÆËÍÊ§°Ü±ê¼ÇTransStatusµÚ6Î»
	//InsertPushData(PUSHDATA);
}
/*
*	  ÎÞÂÛÍÆËÍ³É¹¦Óë·ñ£¬¶¼½«ÏàÓ¦µÄÍÆËÍÊý¾ÝÈë¿â
*/
/*
int PushNote(char *PUSHDATA,char *PushBackData)
{
	SavePushNoteToDB(PUSHDATA,PushBackData);
}
*/
/*
*	²éÑ¯Êý¾Ý¿â²¢Æ´½ÓÊý¾
*	r0_cmd 		: transpush
*	r1_orderid	: ¶©µ¥ºÅ
*	r2_merchno	: ÉÌ»§ºÅ
*	r3_pan 		: ¿¨ºÅ
*	r4_termid	: ÖÕ¶ËºÅ
*	r5_systraceno	: Á÷Ë®ºÅ
*	r6_sysrefno	£ºÏµÍ³²Î¿¼ºÅ
*	r7_mac		: MD5Ð£Ñé
*	r8_amount	: ½ð¶î
*	r9_transtime	: ½»Ò×Ê±¼ä
*	r10_settledat	: ÇåËãÈÕÆÚ
*	r11_batch 	: ´Ë½»Ò×µÄÅú´ÎºÅ
*	r12_expcard	: ¿¨ÓÐÐ§ÆÚ
*	r13_bank_faka	: ·¢¿¨ÐÐ
*	r14_bank_sodan	: ÊÕµ¥ÐÐ
*	r15_void_sn	: Ô­ÖÕ¶ËÁ÷Ë®ºÅ
*	r16_void_data	: Ô­½»Ò×ÈÕÆÚ
*	r17_account_type: ÕË»§ÀàÐÍ
*	r18_transtype	: ½»Ò×ÀàÐÍ
*	r19_pay_channels: Ö§¸¶ÇþµÀ
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
		UppleLog2(__FILE__,__LINE__,"39Óò=[%s]\n",ReturnCode);
		UppleLog2(__FILE__,__LINE__,"½»Ò×²»³É¹¦£¬²»ÍÆËÍ£¡39Óò=[%s]\n",ReturnCode);
		return -1;
		}

	}	
//test

	UppleLog2(__FILE__,__LINE__,"GOODSIFO = [%s]\n",GoodInfo);

//test



	if(TransPushOrderRead() <= 0)
		return 0;			//²éÑ¯Êý¾Ý¿â
	UppleGet( "##OrderNo",OrderNo   );
        UppleLog2(__FILE__,__LINE__,"OrderNo=[%s]\n",OrderNo );
	UppleGet( "PB42",MerchNo);
	/*¹¤ÐÐ×ö¼æÈÝ,2014-10-28*/
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
	*	µç×ÓÇ©¹ºµ¥Êý¾Ý£¬ÍÆËÍ±ØÌîÊÕµ¥ÐÐ
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
//Éú²úÈë¿âÔÝÊ±²»ÉÏ
//	PushNote(PUSHDATA,PushBackData,TransType);
//	ReadTransPush();
        return 0;
}
//zzq add 2015-05-25
//Î¢ÐÅ½á¹ûÍÆËÍ
int WxPushToHk()
{
        char ctranstype[10]={0};
        UppleGet2(RPUB,"wx_transtype",ctranstype);
        int ret1 = atoi(ctranstype);
        char creturncode[20]={0};
        UppleGet2(RPUB,"wx_returncode",creturncode);//½á¹û
        char ctransresult[20]={0};//ÒµÎñ½á¹û
        UppleGet2(RPUB,"wx_transresult",ctransresult);
        UppleLog2(__FILE__,__LINE__,"transtype=[%s],returncode=[%s],transresult=[%s]\n",ctranstype,creturncode,ctransresult);
        int ret2 = strcmp(creturncode,"SUCCESS");
        int ret6 = strcmp(ctransresult,"SUCCESS");
        char ctradestate[20]={0};
        int ret3 = -1;
        int ret4 = -1;
        int ret5 = -1;
        UppleGet2(RPUB,"wx_tradestate",ctradestate);//Ö§¸¶×´Ì¬
        UppleLog2(__FILE__,__LINE__,"Ö§¸¶×´Ì¬=[%s]\n",ctradestate);
        ret3 = strcmp(ctradestate,"SUCCESS");//³É¹¦
        ret4 = strcmp(ctradestate,"REFUND"); //ÍË¿î
        ret5 = strcmp(ctradestate,"REVOKED"); //ÒÑ³·Ïú
        //×´Ì¬ÂëºÍÒµÎñ½á¹û¶¼±ØÐë·µ»Ø³É¹¦²ÅÐèÒªÍÆËÍ
        if((200==ret1 || 300==ret1) && (0 == ret2 && 0 == ret6)){
                if(300 == ret1 && (0 != ret3 && 0 != ret4 && 0 != ret5)){
                        UppleLog2(__FILE__,__LINE__,"²»ÐèÒªÍÆËÍ\n");
                        return 0;
                }
                UppleLog2(__FILE__,__LINE__,"ÐèÒªÍÆËÍµ½»ã¿¨\n");
                char corderno[21]={0};
                UppleGet2(RPUB,"wx_hkorder",corderno);
                //´ÓÊý¾Ý¿âÖÐ¶ÁÈ¡Êý¾Ý
                char cwhere[100]={0};
                snprintf(cwhere,sizeof(cwhere),"hk_orderno='%s' and hk_transtype='200'",corderno);
                char dbRecord[1024]={0};
                if (UppleSelectRealDBRecord("wx_trans_jnl", "hk_mchid,total_fee,wx_orderno,time_end",cwhere, dbRecord, sizeof(dbRecord),NULL) < 0) {
                        //Ã»ÓÐ¼ÇÂ¼
                        UppleLog2(__FILE__,__LINE__,"¼ÇÂ¼Ã»ÓÐÕÒµ½\n");
                        return -1;
                }
                //ÉÌ»§ºÅ
                char chkmchno[16]={0};
                if (UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),"hk_mchid", chkmchno,sizeof(chkmchno)) < 0) {
                        UppleUserErrLog2(__FILE__, __LINE__,"  fldName [hk_mchid] not found!\n");
                        return -1;
                }
                //×Ü½ð¶î ÔªÎªµ¥Î»
                char cmoney[20]={0};
                if (UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),"total_fee",cmoney,sizeof(cmoney)) < 0) {
                        UppleUserErrLog2(__FILE__, __LINE__,"  fldName [total_fee] not found!\n");
                        return -1;
                }
		double dmoney = atof(cmoney);
		//ÍÆËÍÒÔ·ÖÎªµ¥Î»
		dmoney *= 100;
		memset(cmoney,0x00,sizeof(cmoney));
		snprintf(cmoney,sizeof(cmoney),"%.0f",dmoney);
                //Î¢ÐÅ¶©µ¥ºÅ
                char cwxorderno[32]={0};
                if (UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),"wx_orderno",cwxorderno,sizeof(cwxorderno)) < 0) {
			UppleUserErrLog2(__FILE__, __LINE__,"  fldName [wx_orderno] not found!\n");
                        return -1;
                }
                //½»Ò×Ê±¼ä
                char ctimeend[20]={0};
                if (UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),"time_end", ctimeend,sizeof(ctimeend)) < 0) {
                        UppleUserErrLog2(__FILE__, __LINE__,"  fldName [time_end] not found!\n");
                        return -1;
                }
                //Çø·ÖÖ÷É¨»¹ÊÇ±»É¨ ÐèÒª¼ì²é¶©µ¥ºÅ+ÉÌ»§ºÅ+100 ÊÇ·ñÓÐ¼ÇÂ¼ Èç¹ûÓÐ Ôò±íÃ÷ÊÇÖ÷É¨ ·ñÔò±»É¨
                char ctradetype[5]={0};
                memset(cwhere,0x00,sizeof(cwhere));
                snprintf(cwhere,sizeof(cwhere),"hk_orderno='%s' and hk_mchid='%s' and hk_transtype='100'",corderno,chkmchno);
                memset(dbRecord,0x00,sizeof(dbRecord));
                char caccount[3]={0};
                if (UppleSelectRealDBRecord("wx_trans_jnl","*",cwhere, dbRecord, sizeof(dbRecord),NULL) < 0) {
                        //Ã»ÓÐ¼ÇÂ¼
                        UppleLog2(__FILE__,__LINE__,"Ã»ÓÐÕÒµ½¼ÇÂ¼,±»É¨Ö§¸¶\n");
                        memcpy(ctradetype,"31",2);
                        memcpy(caccount,"03",2);
                }else{
                        UppleLog2(__FILE__,__LINE__,"ÕÒµ½¼ÇÂ¼,Ö÷É¨Ö§¸¶\n");
                        memcpy(ctradetype,"30",2);
                        memcpy(caccount,"02",2);
                }

                char cmd5data[100]={0};
                //¶©µ¥ºÅ²¹Æë20Î»
                char ctemp[21]={0};
                memset(ctemp,'0',20);
                int norder = strlen(corderno);
                memcpy(ctemp+20-norder,corderno,norder);
                memset(corderno,0x00,sizeof(corderno));
                memcpy(corderno,ctemp,strlen(ctemp));
                //20Î»¶©µ¥ºÅ+ÉÌ»§ºÅ+½ð¶î+½»Ò×Ê±¼ä
                snprintf(cmd5data,sizeof(cmd5data),"transpush%s%s%s%s123",corderno,chkmchno,cmoney,ctimeend+4);
                char cmd5[33]={0};
                GetMD5(cmd5data,cmd5);
                UppleLog2(__FILE__,__LINE__,"md5data=[%s],md5=[%s]\n",cmd5data,cmd5);
                //×é·¢ËÍÊý¾Ý
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
        UppleLog2(__FILE__,__LINE__,"²»ÐèÒªÍÆËÍ\n");
	return 0;

}
//zzq add end
