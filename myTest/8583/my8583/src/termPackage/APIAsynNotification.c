/*
 *  Created on: 2014-11-25
 *      Author: zwb
 *    function: API异步通知 
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

extern void GetMD5 (char *inString, char *outMD5);

char PushBackData[2048] = {0};/*推送返回信息*/

size_t copy_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	memset(PushBackData, 0, sizeof(PushBackData));
	memcpy(PushBackData, ptr, size * nmemb);
	return size * nmemb;
}

int 	UpplePackAPIAsynNotiData(char	*PushData);
int	UppleGetAPIOutOrderByHkOrderNo(char *hk_order_no);
int	SetAPITransStatus(int index,char Statu);
int     UppleGetOrderMessage_WX();
int     UpplePackAPI_WX_AsynNotiData(char   *PushData);

/**/
int APIAsynNotification(char *hk_order_no)
{
	CURL *curl;
	struct curl_slist *headers = NULL;

	char push_addr[512 + 1] = {0};/*推送地址*/
	char PushData[1024 + 1] = {0};/*推送数据*/
	int TIMES = 0;
	char tmpBuf[128 + 1] = {0};
	int timeout = 0;
	char	resultMode[8]={0};
	int	ret = -1;

	UppleLog2(__FILE__, __LINE__, "开始推送数据！！！\n");
	
	/*通过汇卡订单号找到API相关信息*/
	if(((ret = UppleGetAPIOutOrderByHkOrderNo(hk_order_no)) < 0)){
		UppleLog2(__FILE__,__LINE__,
				"##未找到API关联订单信息，"
				"不需要推送到对应服务器\n");
		return errCodeCurrentMDL_API_Not_fount_associated_order;
	}

	/*取推送地址*/
	memset(push_addr, 0x00, sizeof(push_addr));
	UppleGet2(RPUB,"callBackUrl",push_addr);
	UppleLog2(__FILE__, __LINE__, "推送地址des_addr=[%s]\n", push_addr);
	
	timeout = UppleReadIntTypeRECVar("API_PUSH_TIMEOUT");
	UppleLog2(__FILE__, __LINE__, "推送TIME_OUT=[%d]\n", timeout);
	/*判断是否API交易的推送*/
	UppleGet2(RPUB,"resultMode",resultMode);

	/*组API异步通知数据*/
	UppleLog2(__FILE__, __LINE__, "开始组推送送数据\n");
	
	if((ret =UpplePackAPIAsynNotiData(PushData)) < 0)
	{
		UppleLog2(__FILE__,__LINE__,"in APIAsynNotification::UpplePackAPIAsynNotiData error!!\n");
		return -2;
	}
	UppleLog2(__FILE__, __LINE__, "推送数据 PushData[%s]\n", PushData);

	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, push_addr);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, copy_data);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT,timeout); /*设置20秒超时*/
		headers = curl_slist_append(headers, "Content-Type: text/html");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	}
	else
	{
		UppleLog2(__FILE__, __LINE__, "URL初始化失败\n");
		return -1;
	}

	/*设置发送状态*/
	SetAPITransStatus(0,'1');

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, PushData);

	UppleLog2(__FILE__, __LINE__, "向目标服务器[%s]发送数据\n",push_addr);

	LOOP: if ((ret = curl_easy_perform(curl)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"\n----------send to server Error!----------\n");
		return ret;
	}
	UppleLog2(__FILE__, __LINE__, "PushBackData = [%s]\n", PushBackData);
	
	/*设置发送状态*/
        SetAPITransStatus(1,'1');

	if (strcmp(PushBackData, "") == 0)
	{
		UppleLog2(__FILE__, __LINE__, "接收服务器故障！！\n");
		goto error;
	}
	else if (strlen(PushBackData) > 0 )
	{
		/*设置发送状态*/
        	SetAPITransStatus(2,'1');
		UppleLog2(__FILE__,__LINE__,"返回数据[%s]\n",PushBackData);
		
	}
	else
	{
		TIMES++;
		if (TIMES < 3)
		{
			UppleLog2(__FILE__, __LINE__, "第[%d]次 推送失败！！\n", TIMES);
			goto LOOP;
		}
	}
	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	UppleLog2(__FILE__, __LINE__, "推送数据结束！！！\n");
	return 1;
error:
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        UppleLog2(__FILE__, __LINE__, "推送数据结束！！！\n");
        return -1;
}

int API_wx_AsynNotification(char *hk_order_no)
{
        CURL *curl;
        struct curl_slist *headers = NULL;

        char push_addr[512 + 1] = {0};/*推送地址*/
        char PushData[1024 + 1] = {0};/*推送数据*/
        int TIMES = 0;
        char tmpBuf[128 + 1] = {0};
        int timeout = 0;
        char    resultMode[8]={0};
        int     ret = -1;

        UppleLog2(__FILE__, __LINE__, "开始推送数据！！！\n");

        /*通过汇卡订单号找到API相关信息*/
        if(((ret = UppleGetAPIOutOrderByHkOrderNo(hk_order_no)) < 0)){
                UppleLog2(__FILE__,__LINE__,
                                "##未找到API关联订单信息，"
                                "不需要推送到对应服务器\n");
                return errCodeCurrentMDL_API_Not_fount_associated_order;
        }

        /*取推送地址*/
        memset(push_addr, 0x00, sizeof(push_addr));
        UppleGet2(RPUB,"callBackUrl",push_addr);
        UppleLog2(__FILE__, __LINE__, "推送地址des_addr=[%s]\n", push_addr);

        timeout = UppleReadIntTypeRECVar("API_PUSH_TIMEOUT");
        UppleLog2(__FILE__, __LINE__, "推送TIME_OUT=[%d]\n", timeout);
        /*判断是否API交易的推送*/
        UppleGet2(RPUB,"resultMode",resultMode);

        /*组API异步通知数据*/
        UppleLog2(__FILE__, __LINE__, "开始组推送送数据\n");

        if((ret =UpplePackAPI_WX_AsynNotiData(PushData)) < 0)
        {
                UppleLog2(__FILE__,__LINE__,"in APIAsynNotification::UpplePackAPIAsynNotiData error!!\n");
                return -2;
        }
        UppleLog2(__FILE__, __LINE__, "推送数据 PushData[%s]\n", PushData);

        curl = curl_easy_init();
        if (curl)
        {
                curl_easy_setopt(curl, CURLOPT_URL, push_addr);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, copy_data);
                curl_easy_setopt(curl, CURLOPT_POST, 1);
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
                curl_easy_setopt(curl, CURLOPT_HEADER, 0);
                curl_easy_setopt(curl, CURLOPT_TIMEOUT,timeout); /*设置20秒超时*/
                headers = curl_slist_append(headers, "Content-Type: text/html");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
        else
        {
                UppleLog2(__FILE__, __LINE__, "URL初始化失败\n");
                return -1;
        }

        /*设置发送状态*/
        SetAPITransStatus(0,'1');

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, PushData);

        UppleLog2(__FILE__, __LINE__, "向目标服务器[%s]发送数据\n",push_addr);

        LOOP: if ((ret = curl_easy_perform(curl)) < 0)
        {
                UppleUserErrLog2(__FILE__, __LINE__,
                                "\n----------send to server Error!----------\n");
                return ret;
        }
        UppleLog2(__FILE__, __LINE__, "PushBackData = [%s]\n", PushBackData);

        /*设置发送状态*/
        SetAPITransStatus(1,'1');

        if (strcmp(PushBackData, "") == 0)
        {
                UppleLog2(__FILE__, __LINE__, "接收服务器故障！！\n");
                goto error;
        }
        else if (strlen(PushBackData) > 0 )
        {
                /*设置发送状态*/
                SetAPITransStatus(2,'1');
                UppleLog2(__FILE__,__LINE__,"返回数据[%s]\n",PushBackData);

        }
        else
        {
                TIMES++;
                if (TIMES < 3)
                {
                        UppleLog2(__FILE__, __LINE__, "第[%d]次 推送失败！！\n", TIMES);
                        goto LOOP;
                }
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        UppleLog2(__FILE__, __LINE__, "推送数据结束！！！\n");
        return 1;
error:
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        UppleLog2(__FILE__, __LINE__, "推送数据结束！！！\n");
        return -1;
}



int 	UppleGetOrderMessage()
{
	char    term_mid[20]={0};
        char    term_tid[20]={0};
        char    order_no[30]={0};
        char    dbRecord[8192]={0};
	char	where[1024]={0};
	char 	*key[]={"b2_pan","cardno","b4_amt","b37_refnumber","b41_termnumber","b42_upmerchnumber"};
	char	tmp[1024]={0};
	int	count = 6;
	int	i = 0;
	int 	ret = -1;
	UppleGet2(RPUB,"PB41",term_tid);
        UppleGet2(RPUB,"PB42",term_mid);
        UppleGet2(RPUB,"##order_no",order_no);
	memset(where,0,sizeof(where));
	sprintf(where,"b34_telnumber = '%s' and mid = '%s' and termid = '%s' "
			"and b39_respcode = '00' and msgcode = '0200' "
			"and b3_pcode not like '310000' "
			"order by trans_date desc limit 1 ",
			order_no,term_mid,term_tid);
	if ((ret = UppleSelectRealDBRecord("t_trans_jnl",
                                " * ",
                                where, dbRecord, sizeof(dbRecord),
                                NULL)) < 0) {
                UppleUserErrLog2(__FILE__, __LINE__,
                        "  [%s-%d]:: UppleSelectRealDBRecord!\n",
                        __FILE__, __LINE__);
                return (ret);
        }
	for(; i<count; i++){
		memset(tmp,0,sizeof(tmp));
                if ((ret = UppleReadRecFldFromRecStr(dbRecord,
                        strlen(dbRecord),
                        key[i],tmp,sizeof(tmp))) < 0) {
                        UppleUserErrLog2(__FILE__, __LINE__,
                                "  [%s-%d]in :: fldName [%s] not find!\n",
                                __FILE__, __LINE__,key[i]);
                        return (ret);
                }
                UppleSet2(RPUB,key[i],tmp);
                UppleLog2(__FILE__,__LINE__,
                                "字段名[%s],内容[%s]\n",key[i],tmp);
	}
	return 1;
}

int	UpplePackAPIAsynNotiData(char	*PushData)
{
	char	order_msg[8192]={0};
	char	b2_pan[22]={0};
	char	b4_amt[14]={0};
	char	b37_refnumber[30]={0};
	char	b41_termnumber[20]={0};
	char	b42_upmerchnumber[20]={0};
	char	term_tid[20]={0};
	char	term_mid[20]={0};
	char	out_order[128]={0};
	char	order_no[30]={0};
	char	cover_card_no[22]={0};
	char	md5_data[1024]={0};
	char	md5[128]={0};
	int	ret = -1;
	int	i = 0;

	if(PushData == NULL)
	{
		UppleLog2(__FILE__,__LINE__,"in UpplePackAPIAsynNotiData::param error!!\n");
		return -1;
	}
	memset(PushData,0,sizeof(PushData));

	if((ret = UppleGetOrderMessage(order_msg)) < 0){
		UppleLog2(__FILE__,__LINE__,
				"##获取原交易信息失败!\n");
		return -1;
	}
	UppleGet2(RPUB,"b2_pan",b2_pan);
	if(strlen(b2_pan) <= 0)
		UppleGet2(RPUB,"cardno",b2_pan);

	memcpy(cover_card_no,b2_pan,strlen(b2_pan));
	for(i=6;i <strlen(b2_pan)-4;i++)
		cover_card_no[i] = '*';

	UppleGet2(RPUB,"b4_amt",b4_amt);
	UppleGet2(RPUB,"b37_refnumber",b37_refnumber);
	UppleGet2(RPUB,"b41_termnumber",b41_termnumber);
	UppleGet2(RPUB,"b42_upmerchnumber",b42_upmerchnumber);
	UppleGet2(RPUB,"PB41",term_tid);
	UppleGet2(RPUB,"PB42",term_mid);
	UppleGet2(RPUB,"##order_no",order_no);
	UppleGet2(RPUB,"outorder",out_order);
	
	sprintf(PushData,"a1_cardno=%s&a2_amount=%0.2f&a3_hk_orderno=%s&"
			 "a4_outorderno=%s&a5_termid=%s&a6_mernid=%s&"
			 "a7_transtid=%s&a8_transmid=%s&a9_refnumber=%s",
			cover_card_no,atof(b4_amt)/100,order_no,
			out_order,term_tid,term_mid,
			b41_termnumber,b42_upmerchnumber,b37_refnumber);

	sprintf(md5_data,"123%s456",PushData);
	UppleLog2(__FILE__,__LINE__,
			"##参与MD5预算的数据[%s]\n",md5_data);
	GetMD5(md5_data,md5);
	strcat(PushData,"&sign=");
	strcat(PushData,md5);
	return (strlen(PushData));
}

int     UppleGetAPIOutOrderByHkOrderNo(char *hk_order_no)
{
	char	dbRecord[2048]={0};
	char	where[1024]={0};
	char 	*API_key[]={"org_id","app_id","dev_id","merch_id","outorder",\
			"remarks","cashier","HK_Order_no","callBackUrl","callBackStatus"};
	char	tmp[1024]={0};
	/*根据key定义的个数，库增加字段后这个值需要对应变化*/
	int	API_KEY_COUNT = 10;
	/**************************************************/	
	int	ret = -1;
	int	i = 0;

	if((hk_order_no == NULL) || (strlen(hk_order_no) <= 0)){
		UppleLog2(__FILE__,__LINE__,
				"##取汇卡订单号为空!\n");
		return -1;
	}
	memset(where,0,sizeof(where));
	sprintf(where,"HK_Order_no = '%s' order by reqtime desc limit 1",hk_order_no);
	
	memset(dbRecord,0,sizeof(dbRecord));
	if ((ret = UppleSelectRealDBRecord("AuthSer_Info",
                                " * ",
                                where, dbRecord, sizeof(dbRecord),
                                NULL)) < 0) {
                UppleUserErrLog2(__FILE__, __LINE__,
                        "  [%s-%d]:: UppleSelectRealDBRecord!\n",
                        __FILE__, __LINE__);
                return (ret);
        }
	for(; i<API_KEY_COUNT; i++){
		memset(tmp,0,sizeof(tmp));
		if ((ret = UppleReadRecFldFromRecStr(dbRecord,
                        strlen(dbRecord),
                        API_key[i],tmp,sizeof(tmp))) < 0) {
                        UppleUserErrLog2(__FILE__, __LINE__,
                                "  [%s-%d]in :: fldName [%s] not find!\n",
                                __FILE__, __LINE__,API_key[i]);
                        return (ret);
                }
		UppleSet2(RPUB,API_key[i],tmp);
		UppleLog2(__FILE__,__LINE__,
				"字段名[%s],内容[%s]\n",API_key[i],tmp);
	}
	return 1;
}

int	SetAPITransStatus(int index,char Statu)
{
	char	Status[16]={0};
	char	outorder[128]={0};
	char	HK_Order_no[30]={0};
	char	org_id[128]={0};
	char	merch_id[128]={0};
	char 	fld[1024]={0};
	char	where[1024]={0};
	int	ret = -1;

	if(index > 8){
		UppleLog2(__FILE__,__LINE__,
				"##设置API状态参数有误\n");
		return -1;
	}
	UppleGet2(RPUB,"callBackStatus",Status);
	UppleGet2(RPUB,"outorder",outorder);
	UppleGet2(RPUB,"HK_Order_no",HK_Order_no);
	UppleGet2(RPUB,"org_id",org_id);
	UppleGet2(RPUB,"merch_id",merch_id);
	Status[index] = Statu;

	sprintf(fld,"callBackStatus = '%s'",Status);

	sprintf(where,"org_id = '%s' and merch_id = '%s' and "
			"outorder = '%s' and HK_Order_no = '%s'",
			org_id,merch_id,outorder,HK_Order_no);
	
	if ((ret = UppleUpdateRealDBRecord(
                        "AuthSer_Info",
                        fld,
                        where)) < 0){
                UppleUserErrLog2(__FILE__, __LINE__,
                                "  [%s-%d]in API授权更新状态失败\n",
                                __FILE__, __LINE__);
                return (ret);
        }
	UppleSet2(RPUB,"callBackStatus",Status);
	return 1;
}

int     UpplePackAPI_WX_AsynNotiData(char   *PushData)
{
	char	hk_mchid[30]={0};
	char	hk_orderno[30]={0};
	char	wx_tradetype[30]={0};
	char	total_fee[64]={0};
	char	extradata[999]={0};
	char	result_code[24]={0};
	char	coupon_fee[180]={0};
	char	wx_orderno[128]={0};
        char    md5_data[1024]={0};
        char    md5[128]={0};
        int     ret = -1;
        int     i = 0;

        if(PushData == NULL)
        {
                UppleLog2(__FILE__,__LINE__,"in UpplePackAPIAsynNotiData::param error!!\n");
                return -1;
        }
        memset(PushData,0,sizeof(PushData));

        if((ret = UppleGetOrderMessage_WX()) < 0){
                UppleLog2(__FILE__,__LINE__,
                                "##获取原交易信息失败!\n");
                return -1;
        }
        UppleGet2(RPUB,"hk_mchid",hk_mchid); /*汇卡商户号*/
        UppleGet2(RPUB,"hk_orderno",hk_orderno);/*汇卡订单号*/
        UppleGet2(RPUB,"wx_tradetype",wx_tradetype);/*交易类型*/
        UppleGet2(RPUB,"total_fee",total_fee);/*订单金额*/
        UppleGet2(RPUB,"extradata",extradata);/*附加域*/
        UppleGet2(RPUB,"result_code",result_code);/*微信返回码*/
        UppleGet2(RPUB,"coupon_fee",coupon_fee);/*优惠金额*/
        UppleGet2(RPUB,"wx_orderno",wx_orderno);/*微信订单号*/

	sprintf(PushData,"a0_trans_type='%s'&a1_hk_mchid='%s'&a2_wx_tradetype='%s'&"
			"a3_total_fee='%s'&a4_extradata='%s'&a5_result_code='%s'&"
			"a6_coupon_fee='%s'&a7_wx_orderno='%s'&a8_hk_orderno='%s'",\
			"wx",hk_mchid,wx_tradetype,\
			total_fee,extradata,result_code,\
			coupon_fee,wx_orderno,hk_orderno);

        sprintf(md5_data,"123%s456",PushData);
        UppleLog2(__FILE__,__LINE__,
                        "##参与MD5预算的数据[%s]\n",md5_data);
        GetMD5(md5_data,md5);
        strcat(PushData,"&sign=");
        strcat(PushData,md5);
        return (strlen(PushData));
}

int     UppleGetOrderMessage_WX()
{
        char    term_mid[20]={0};
        char    term_tid[20]={0};
        char    order_no[30]={0};
        char    dbRecord[8192]={0};
        char    where[1024]={0};
        char    *key[]={"hk_mchid","wx_submchid","hk_orderno","wx_tradetype","total_fee","extradata",\
			"result_code","coupon_fee","wx_orderno","wx_appid","wx_mer_id"};
        char    tmp[1024]={0};
        int     count = 6;
        int     i = 0;
        int     ret = -1;
        UppleGet2(RPUB,"PB42",term_mid);
        UppleGet2(RPUB,"##order_no",order_no);
        memset(where,0,sizeof(where));
        sprintf(where,"hk_mchid = '%s' and hk_orderno = '%s' and hk_transtype = '200' and result_code = '00' "
                        "order by orderdate desc limit 1 ",
                        term_mid,order_no);
        if ((ret = UppleSelectRealDBRecord("wx_trans_jnl",
                                " * ",
                                where, dbRecord, sizeof(dbRecord),
                                NULL)) < 0) {
                UppleUserErrLog2(__FILE__, __LINE__,
                        "  [%s-%d]:: UppleSelectRealDBRecord!\n",
                        __FILE__, __LINE__);
                return (ret);
        }
        for(; i<count; i++){
                memset(tmp,0,sizeof(tmp));
                if ((ret = UppleReadRecFldFromRecStr(dbRecord,
                        strlen(dbRecord),
                        key[i],tmp,sizeof(tmp))) < 0) {
                        UppleUserErrLog2(__FILE__, __LINE__,
                                "  [%s-%d]in :: fldName [%s] not find!\n",
                                __FILE__, __LINE__,key[i]);
                        return (ret);
                }
                UppleSet2(RPUB,key[i],tmp);
                UppleLog2(__FILE__,__LINE__,
                                "字段名[%s],内容[%s]\n",key[i],tmp);
        }
        return 1;
}
