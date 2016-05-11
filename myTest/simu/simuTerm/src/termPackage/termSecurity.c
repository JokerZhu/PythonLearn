//	Author:		zhangyongding
//	Date:		20081018
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "platFormEnv.h"
#include "uppleREC.h"
#include "securityConf.h"

#include "termPackage.h"
#include "termSecurity.h"
#include "hsmCmd.h"
#include "racalHsmAPI.h"

#include "specErrCode.h"

#include "buffer_pool.h"
#include "uppleModuleVar.h"

#include <openssl/sha.h>
#include <openssl/rsa.h> 
#include <openssl/evp.h> 
#include <openssl/objects.h> 
#include <openssl/x509.h> 
#include <openssl/err.h> 
#include <openssl/pem.h> 
#include <openssl/ssl.h> 
#include <openssl/pkcs12.h>

extern XBUF *RPUB;

#define MACLEN  8

int UppleIsVerifyTermMacNeeded()
{
	char	fld039[2+1];

	memset(fld039,0,sizeof(fld039));
	UppleReadTermFld(39,fld039,sizeof(fld039));
	if (strlen(fld039) == 0)		// 是请求，要验证MAC
		return(1);
	// 是响应
	if (strcmp("00",fld039) == 0)	// 成功响应
		return(1);
	else				// 失败不验证
		return(0);
}

int UppleGenerateTermTransMac()
{
	int	ret;
	char	tmk[48+1];
	char	tak[32+1];
	char	macData[8192+1];
	int	lenOfMacData;
	char	mac[32+1];

	UppleUserErrLog2(__FILE__,__LINE__,"Begin  UppleGenerateTermTransMac\n" );

	if (!UppleIsTermTransMacExists())			// 检查MAC域是否存在，在环境变量中设置
		return(0);

	memset(tak,0,sizeof(tak));
	if ((ret = UppleReadHsmTak(tak,sizeof(tak))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleGenerateTermTransMac:: UppleReadHsmTak!\n");
		return(ret);		
	}

	memset(macData,0,sizeof(macData));
	if ((lenOfMacData = UppleTermMergeMacData(UppleGetTermPackage(),UppleFindMacFldTBLDef("termTransMacTBL"),macData,sizeof(macData)-1)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleGenerateTermTransMac:: UppleTermMergeMacData!\n");
		return(lenOfMacData);
	}
	macData[lenOfMacData] = 0;
	memset(mac,0,sizeof(mac));
	if (UppleIsUseRacalHsm())
	{
		if ((ret = UppleGenerateMACUseRacal(tak,macData,lenOfMacData,mac)) < 0)
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleGenerateTermTransMac:: UppleGenerateMACUseRacal!\n");
			return(ret);
		}
	}
	else
	{
		if ((ret = UppleGenerateMacPOSP(tak,macData,lenOfMacData,mac)) < 0)
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleGenerateTermTransMac:: UppleGenerateMac!\n");
			return(ret);
		}
	}
	UppleDebugLog("in UppleGenerateTermTransMac:: mac = [%s] macData = [%s] macDataLen = [%d]\n",mac,macData,lenOfMacData);
	mac[8] = 0;
	/*
#if 1

strncpy(mac,"11111111",8);
#endif	
	 */
	return(UppleSetTermFld(64,mac,8));
}

int UppleVerifyPospTermTransMac(char *buf,int len)
{
	int ret;
	char    localMac[32+1];
	char    remoteMac[32+1];
	char    macData[8192+1];
	int     lenOfMacData;
	char    tak[32+1];
	int	lenOfTermHeader;

	if (!UppleIsTermTransMacExists())			// 检查MAC域是否存在，在环境变量中设置
		return(0);
	if (!UppleIsVerifyTermMacNeeded())		// 检查是否要验证MAC
		return(0);


	memset(remoteMac,0,sizeof(remoteMac));
	if ((ret = UppleReadTermFld(64,remoteMac,sizeof(remoteMac))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleReadTermFld 64!\n");
		return(ret);
	}
	if(0 == strlen(remoteMac)){                                                                                                                                   
		UppleGet2(RPUB,"TB64",remoteMac);
	}
	lenOfTermHeader=UppleGetLengthOfTermPackageHeader(); 
	lenOfMacData=len-lenOfTermHeader-MACLEN;

	memset(macData,0,sizeof(macData));
	memcpy(macData,buf+lenOfTermHeader,len-lenOfTermHeader-MACLEN);

	UppleMemLog2(__FILE__,__LINE__,"macData=",macData,lenOfMacData);

	if ((ret = UppleGeneratePospTermMac(macData,lenOfMacData,localMac)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleGenerateMac!\n");
		return(ret);
	}

	if (strncmp(localMac,remoteMac,8) != 0)
	{
		UppleLog("in UppleVerifyTermTransMac:: remoteMac = [%s] localMac = [%s] macData = [%s] macDataLen = [%d]\n",
				remoteMac,localMac,macData,lenOfMacData);
		return(errCodeCurrentMDL_8583PackageMacNotSameAsThatLocalGenerated);
	}
	UppleUserErrLog2(__FILE__,__LINE__,"###较验终端MAC正确:: remoteMac = [%s] localMac = [%s]\n", remoteMac,localMac );

	return(0);



}


int UppleVerifyTermTransMac()
{
	int	ret;
	char	localMac[32+1];
	char	remoteMac[32+1];
	char	macData[8192+1];
	int	lenOfMacData;
	char	tmk[48+1];
	char	tak[32+1];
	int	retryTimes = 0;

	if (!UppleIsTermTransMacExists())			// 检查MAC域是否存在，在环境变量中设置
		return(0);
	if (!UppleIsVerifyTermMacNeeded())		// 检查是否要验证MAC
		return(0);

	memset(tak,0,sizeof(tak));
	if ((ret = UppleReadHsmTak(tak,sizeof(tak))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleReadHsmTak!\n");
		return(ret);		
	}

	memset(remoteMac,0,sizeof(remoteMac));
	if ((ret = UppleReadTermFld(64,remoteMac,sizeof(remoteMac))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleReadTermFld 64!\n");
		return(ret);
	}
	memset(macData,0,sizeof(macData));
	if ((lenOfMacData = UppleTermMergeMacData(UppleGetTermPackage(),UppleFindMacFldTBLDef("termTransMacTBL"),macData,sizeof(macData)-1)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleTermMergeMacData!\n");
		return(lenOfMacData);
	}
	macData[lenOfMacData] = 0;
	memset(localMac,0,sizeof(localMac));
	if (UppleIsUseRacalHsm())
	{
		if ((ret = UppleGenerateMACUseRacal(tak,macData,lenOfMacData,localMac)) < 0)
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleGenerateMACUseRacal!\n");
			return(ret);
		}
	}
	else
	{
		if ((ret = UppleGenerateMacPOSP(tak,macData,lenOfMacData,localMac)) < 0)
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleGenerateMac!\n");
			return(ret);
		}
	}
	if (strncmp(localMac,remoteMac,8) != 0)
	{
		UppleLog("in UppleVerifyTermTransMac:: remoteMac = [%s] localMac = [%s] macData = [%s] macDataLen = [%d]\n",
				remoteMac,localMac,macData,lenOfMacData);
		return(errCodeCurrentMDL_8583PackageMacNotSameAsThatLocalGenerated);
	}
	UppleUserErrLog2(__FILE__,__LINE__,"###较验终端MAC正确:: remoteMac = [%s] localMac = [%s]\n", remoteMac,localMac );
	return(0);
}

int UppleTranslateTermPinToYLPin(char *ylPin)
{
	char	zpk[48+1];
	char	tpk[48+1];
	char	tpk2[48+1];
	char	tmk[48+1];
	char	cardNo[40];
	char	termPin[32+1];
	char	up_sek[64+1];
	char    term_sek[8+1];
	char Channel[10];
	char TransType[7];
	char Fld23[4];
	int	ret;
	int flag = 0;

	memset(termPin,0,sizeof(termPin));
	if ((ret = UppleReadTermFld(52,termPin,sizeof(termPin))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadTermFld 52!\n");
		return(ret);
	}

	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if(ret==0){
		return 0;
	}

	ReadUPWorkingKeyfromTable( "PINKEY", zpk, up_sek );
	//strcpy(zpk,UppleReadQZJYLZpk());
	if( !memcmp( zpk, "\x00\x00\x00\x000", 4 ) )  //没取到
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
		return(ret);
	}

	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);		
	}

	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}

	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}

	memset(tpk2,0,sizeof(tpk2));
	UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);

	memset(term_sek,0,sizeof(term_sek));
	if ((ret = UppleReadTermSEK(term_sek,sizeof(term_sek))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}

	//UppleExchKey("1","SEK1","SEK2",tmk,tpk,tpk2);
	UppleExchKey("1",term_sek,term_sek,tmk,tpk,tpk2);

	//return(TranslatePin("SEK1","SEK2",tpk2,zpk,termPin,cardNo,1,6,ylPin));
	//return(TranslatePin("SEK1",up_sek,tpk2,zpk,termPin,cardNo,1,6,ylPin));
	memset(Channel,0x00,sizeof(Channel));
	UppleGet2(RPUB, "#Channel", Channel);
	UppleLog2(__FILE__,__LINE__,"@@Channel = [%s]\n",Channel);
	if(atoi(Channel) == 1009 ) 
	{
		return(TranslatePin(term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,1,ylPin));
	}else if(atoi(Channel) == 5001){
		UppleLog2(__FILE__,__LINE__,"@@Channel = [%s]\n",Channel);
		return(TranslatePin(term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,1,ylPin));
	}
	else
		return(TranslatePin(term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,6,ylPin));   


}
int UppleTranslateTermPinToPosPin(char *ylPin)
{
	char	term_tpk[48+1];
	char	term_tpk2[48+1];
	char	posp_tpk[48+1];
	char	posp_tpk2[48+1];
	char	term_tmk[48+1];
	char	posp_tmk[48+1];

	char	cardNo[40+1];
	char	termPin[32+1];
	int	ret;

	memset(termPin,0,sizeof(termPin));
	if ((ret = UppleReadTermFld(52,termPin,sizeof(termPin))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslatePospTermPinToYLPin:: UppleReadTermFld 52!\n");
		return(ret);
	}

	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########,ret=[%d]\n",termPin, ret);
	if(ret==0){
		return 0;
	}

	memset(posp_tmk,0,sizeof(posp_tmk));
	if ((ret = UppleReadHsmTmk(posp_tmk,sizeof(posp_tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslatePospTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}
	//UppleLog2(__FILE__,__LINE__,"################MASTERKEY:[%s]###########\n",termPin);

	memset(term_tmk,0,sizeof(term_tmk));
	if ((ret = UppleReadTermTmk(term_tmk,sizeof(term_tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslatePospTermPinToYLPin:: UppleReadTermTmk!\n");
		return(ret);		
	}
	//UppleLog2(__FILE__,__LINE__,"################SALEMASTERKEY:[%s]###########\n",termPin);

	if(strcmp(term_tmk,posp_tmk)==0){
		strcpy(ylPin,termPin);
		return strlen(ylPin);
	}

	memset(posp_tpk,0,sizeof(posp_tpk));
	if ((ret = UppleReadHsmTpk(posp_tpk,sizeof(posp_tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToPosPin:: UppleReadHsmTpk!\n");
		return(ret);		
	}
	//UppleLog2(__FILE__,__LINE__,"################PINKEY:[%s]###########\n",termPin);

	memset(term_tpk,0,sizeof(term_tpk));
	if ((ret = UppleReadTermTpk(term_tpk,sizeof(term_tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToPosPin:: UppleReadTermTpk!\n");
		return(ret);		
	}
	//UppleLog2(__FILE__,__LINE__,"################SALEPINKEY:[%s]###########\n",termPin);


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}

	memset(term_tpk2,0,sizeof(term_tpk2));
	//UppleLog2(__FILE__,__LINE__, "termPin = [%s] term pik = [%s] cardNo = [%s]\n",termPin,term_tpk,cardNo);
	UppleExchKey("1","SEK1","SEK2",term_tmk,term_tpk,term_tpk2);
	//UppleLog2(__FILE__,__LINE__, "term_tpk2=[%s]\n",term_tpk2);

	memset(posp_tpk2,0,sizeof(posp_tpk2));
	//UppleLog2(__FILE__,__LINE__, "termPin = [%s] posp tpk = [%s] cardNo = [%s]\n",termPin,posp_tpk,cardNo);
	UppleExchKey("1","SEK1","SEK2",posp_tmk,posp_tpk,posp_tpk2);
	//UppleLog2(__FILE__,__LINE__, "posp_tpk2=[%s]\n",posp_tpk2);

	return(TranslatePin("SEK1","SEK2",term_tpk2,posp_tpk2,termPin,cardNo,1,1,ylPin));
}



int UppleGenerateTermMac(char *tak,char *macData,int lenOfMacData,char *localMac)
{
	return 0;
}

int UppleGeneratePospTermMac(char *macData,int lenOfMacData,char *localMac)
{

	int ret;
	char xor_res[8];
	char aschex[16+1];
	char mid_mak[8];
	char temp[16+1];
	char temp2[32+1];
	char high[8];
	char tak[48+1];
	char tak2[48+1];
	char tmk[48+1];


	//	UppleLog2(__FILE__,__LINE__,"###tak string=[%s]\n",tak);

	memset(xor_res,0,sizeof(xor_res));	
	aschex[16]=0;
	memset(tak2,0,sizeof(tak2));
	memset(tmk,0,sizeof(tmk));

	Xor(macData,lenOfMacData,xor_res);
	bcdhex_to_aschex(xor_res,8,aschex);

	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	memset(tak,0,sizeof(tak));
	if ((ret = UppleReadTermTak(tak,sizeof(tak))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleReadHsmTak!\n");
		return(ret);		
	}


	//	UppleMemLog2(__FILE__,__LINE__,"###xor result1\n",xor_res,8);

	memcpy(high,aschex,8);

	memset( temp2, 0x00, sizeof(temp2) );
	memset( temp, 0x00, sizeof(temp) );

	UppleExchKey("1","SEK1","SEK2",tmk,tak,tak2);	

	UppleHsmDes(tak2,high,8,temp);

	aschex_to_bcdhex(temp, 16, mid_mak);

	//	UppleMemLog2(__FILE__,__LINE__,"in  UppleGenerateTermMac mid_mak1\n",mid_mak,8);
	memcpy(aschex,mid_mak,8);

	//	UppleMemLog2(__FILE__,__LINE__,"###Before xor 2\n",aschex,16);

	Xor(aschex,16,xor_res);

	//	UppleMemLog2(__FILE__,__LINE__,"###xor result2\n",xor_res,8);

	memset( temp2, 0x00, sizeof(temp2) );
	memset( temp, 0x00, sizeof(temp) );
	bcdhex_to_aschex(xor_res,8,temp2);

	//	UppleLog2(__FILE__,__LINE__,"###high before DES=[%s]",temp2);


	UppleHsmDes(tak2,xor_res,8,temp);

	aschex_to_bcdhex(temp, 16, mid_mak);

	//	UppleMemLog2(__FILE__,__LINE__,"###mid_mak2 hex",mid_mak,8);

	bcdhex_to_aschex(mid_mak,8,aschex);

	//	UppleLog2(__FILE__,__LINE__,"in  UppleGenerateTermMac xor result[%s]\n",aschex);

	memcpy(localMac,aschex,8);	
	//	UppleGenerateMacPOSP(tak,macData,lenOfMacData,localMac);
	//UppleMemLog2(__FILE__,__LINE__,"###HSM localMac",localMac,8);

	return 0;
}

int UppleTranslateOCXPinToPPPPin(char *ylPin)
{
	char	zpk[48+1];
	char	tpk[48+1];
	char	tpk2[48+1];
	char	tmk[48+1];
	char	cardNo[40];
	char	termPin[32+1];
	char	up_sek[64+1];
	int	ret;
	char    term_sek[8+1];
	char channel[10];
	char ProcessCode[10];
	char CardNo[20];
	int flag = 0;
	char up_channel[10] = {0};
#if 1	/*易结款、对外付款、资金归集特殊处理*/
	long 	port = 0;
	UppleGet2L(RPUB, "#port", &port);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToPPPPin::port=[%ld]\n",port);
#endif

	memset(termPin,0,sizeof(termPin));
	if ((ret = UppleReadTermFld(52,termPin,sizeof(termPin))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadTermFld 52!\n");
		return(ret);
	}

	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if(ret==0){
		return 0;
	}


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);		
	}

	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}

	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	memset(CardNo,0,sizeof(CardNo));
	//福建银联算pin需要送卡号
	memset(channel,0,sizeof(channel));		
	memset(ProcessCode,0,sizeof(ProcessCode));
	UppleGet2(RPUB, "#up_Channel", up_channel);
	if(8385 == port)
	{
		UppleGet2(RPUB, "#up_Channel", channel);
		UppleLog2(__FILE__,__LINE__," up_channel = [%s],CardNo = [%s]\n",channel,CardNo);
	}
	else
	{
		UppleGet2(RPUB, "#Channel", channel);	
	}
	UppleReadTermFld(3, ProcessCode, sizeof(ProcessCode));
	UppleReadTermFld(2,CardNo,sizeof(CardNo));
	UppleLog2(__FILE__,__LINE__," channel = [%s]   ProcessCode = [%s] CardNo = [%s]\n",channel,ProcessCode,CardNo);
	if(strlen(channel) == 0 && (atoi(ProcessCode) == 280000))
	{				
		char fld041[9],fld042[16];
		memset(fld041,0,sizeof(fld041));
		UppleGet2( RPUB, "TB41", fld041 );
		UppleLog2(__FILE__,__LINE__, "#TEST:TB41=[%s]\n", fld041 );

		memset(fld042,0,sizeof(fld042));
		UppleGet2( RPUB, "TB42", fld042 );
		UppleLog2(__FILE__,__LINE__, "#TEST:TB42=[%s]\n", fld042 );
		FindChannelByMidTid(fld042,fld041,channel);
		UppleLog2(__FILE__,__LINE__," channel = [%s]\n",channel);
		if(atoi(channel) == 1009)
			flag = 1;
	}

	if(atoi(channel) == 1009 || atoi(channel) == 5001)
	{

		if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
		{
			UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
			return(ret);
		}
	}
	//福建银联算pin需要送卡号
	memset(tpk2,0,sizeof(tpk2));
	UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);

	if ((ret = UppleReadTermSEK(term_sek,sizeof(term_sek))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}

	//UppleExchKey("1","SEK1","SEK2",tmk,tpk,tpk2);
	UppleExchKey("1",term_sek,term_sek,tmk,tpk,tpk2);

	memset( zpk, 0x00, sizeof(zpk) );
	memset( up_sek, 0x00, sizeof(up_sek) );
	ReadUPWorkingKeyfromTable( "PINKEY", zpk, up_sek );

	//strcpy(zpk,UppleReadQZJYLZpk());
	if( !memcmp( zpk, "\x00\x00\x00\x000", 4 ) )  //没取到
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
		return(ret);
	}

	if(8385 == port)
	{

		UppleLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToPPPPin::TC TranslatePin\n");
		if(strlen(cardNo) <= 0)
		{
			UppleGet2(RPUB,"TB2",cardNo);
			if(strlen(cardNo) <= 0)
			{
				UppleGet2(RPUB,"PB2",cardNo);
			}		
		}
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToPPPPin::TC TranslatePin card_no=[%s]\n",cardNo);
		if(atoi(channel) == 1009)
			return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,1,ylPin));
		else if(atoi(channel) == 5001 ){
			if(atoi(up_channel) == 1001){
				return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,6,ylPin));
			}else{
				UppleLog2(__FILE__,__LINE__,"CUPS PIN");
				return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,1,ylPin));
			}
		}
		else
			return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,6,ylPin));
	}
	else
	{
		if(atoi(channel) == 1009)
			return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,6,1,ylPin));
		else if(atoi(channel) == 5001){
			if(atoi(up_channel) == 1001){
				return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,6,6,ylPin));
			}else{
				UppleLog2(__FILE__,__LINE__,"CUPS PIN");
				return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,6,1,ylPin));
			}
		}
		else
			return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,6,6,ylPin));
	}
}

int UppleTranslateOCXPinToBOCPin(char *ylPin)
{
	char	tmpBuf[256+1];
	char	zpk[48+1];
	char	zpk2[48+1];
	char	tmk_boc[48+1];
	char	tpk[48+1];
	char	tpk2[48+1];
	char	tmk[48+1];
	char	cardNo[40];
	char	termPin[32+1];
	int	ret;

	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "PB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if(ret==0){
		return 0;
	}


	UppleReadDatabaseTableOfTerm();


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);		
	}
	UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}
	UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}


	//读出渠道的PINKEY,用特定的商户号和终端号存在终端表中
	/*
	   strcpy( tmpBuf, "00000002" );   //termid for BOC channel.
	   UppleSetTermFld(41,tmpBuf,strlen(tmpBuf));
	   UppleSet2( RPUB, "PB41",  tmpBuf );

	   strcpy( tmpBuf, "channel11111111" );   //mid for BOC channel.
	   UppleSetTermFld(42,tmpBuf,strlen(tmpBuf));
	   UppleSet2( RPUB, "PB42",  tmpBuf );

	   UppleReadDatabaseTableOfTerm();
	 */
	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}
	UppleLog2(__FILE__,__LINE__, "BOC MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToBOCPin:: Error read BOC zpk !\n");
		return(ret);		
	}
	UppleLog2(__FILE__,__LINE__, "BOC PINKEY = [%s]\n", zpk );

	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK_BOCSZ","SEK_BOCSZ",tmk_boc,zpk,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	return(TranslatePin("SEK1","SEK_BOCSZ",tpk2,zpk2,termPin,cardNo,6,1,ylPin));

}

// by zhouzhenfan
int UppleTranslateOCXPinToKQPin(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char    tmk_p[48+1];
	char    zpk_p[48+1];


	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "PB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"################ field 52:[%s]###########\n",termPin);
	if(ret==0){
		return 0;
	}


	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));
	/*
	 *       判断是否为资金归集、对外付款交易.是则不重新读取终端表,否则重新读取终端表(原流程)
	 */
	if((ret = is_cash_sweep()) > 0){
		/*不用查询数据库*/
		UppleLog2(__FILE__,__LINE__,"test string ZZZZZZ\n");
	}
	else
		UppleReadDatabaseTableOfTerm();
	//         UppleReadDatabaseTableOfTerm();


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}

	char    up_sek[64+1];

#if 1
	/* tmk */
	memset(tmk_boc,0,sizeof(tmk_boc));
	//ReadUPWorkingKeyfromTableKQ( "MASTERKEY", tmk_boc, up_sek );
	//UppleLog2(__FILE__,__LINE__,"tmk=[%s]\n", tmk_boc);

	/* tmk */
	memset(zpk,0,sizeof(zpk));
	ReadUPWorkingKeyfromTableKQ( "PINKEY", zpk, up_sek );
	//UppleLog2(__FILE__,__LINE__,"zpk=[%s]\n", zpk);

#else    
	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "KQ MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToBOCPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "KQ PINKEY = [%s]\n", zpk );
#endif

#if 0
	if ((ret = add_parity_tmk("SEK_KQ","TEK1",tmk_boc,tmk_p)) < 0)
		//if ((ret = add_parity_tmk_gen("SEK_KQ","TEK1",tmk,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
		return(ret);
	}


#endif    

#if 0    
	memset(zpk_p,0,sizeof(zpk_p));
	if ((ret = add_parity_tmk_gen(UppleReadStringTypeRECVar("SEK_KQ"),UppleReadStringTypeRECVar("TEK1"),zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to zpk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "zpk_p= [%s]\n", zpk_p);
#endif

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	return(TranslatePin("SEK1","SEK_KQ",tpk2,zpk,termPin,cardNo,6,1,ylPin));
	//return(TranslatePin("SEK1","SEK_KQ",tpk2,zpk_p,termPin,cardNo,6,1,ylPin));
}

// by zhouzhenfan
int UppleTranslateOCXPinToKQPin_fastpay(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char    tmk_p[48+1];
	char    zpk_p[48+1];


	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "TB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"fastpay ################field 52:[%s]###########\n",termPin);

	/*ZWB 修改2013-7-27 快钱无密交易*/
	UppleLog2(__FILE__,__LINE__,"###快钱交易，转PIN操作函数\n");
	ret = strlen(termPin);
	/////////////////////////////////

	if(ret==0){
		return 0;
	}


#if 1
	char tid_tmp[8 + 1] = {0};
	char mid_tmp[15 + 1] = {0};
	char fld_tmp[2048 + 1] = {0};
	char dbRecord_tmp[4096 + 1] = {0};

	UppleGet2(RPUB, "TB41", tid_tmp);
	UppleGet2(RPUB, "TB42", mid_tmp);
	UppleLog2(__FILE__, __LINE__, "#TEST:PB41=[%s]\n", tid_tmp);
	UppleLog2(__FILE__, __LINE__, "#TEST:PB42=[%s]\n", mid_tmp);

	sprintf(fld_tmp, "TID = '%s' and MID = '%s' ", tid_tmp, mid_tmp);
	if ((ret = UppleSelectRealDBRecord("TERMINALPARA", "*", fld_tmp,
					dbRecord_tmp, sizeof(dbRecord_tmp), NULL)) < 0)
	{
		UppleLog2( __FILE__,__LINE__, "无记录.\n\n");
		return ret;
	}

	// SALEMASTERKEY
	if ((ret = UppleReadRecFldFromRecStr(dbRecord_tmp, strlen(dbRecord_tmp), "SALEMASTERKEY",
					tmk, sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"UppleReadFldFromTermByFldName:: fldName [SALEMASTERKEY] not find!\n");
		return (ret);
	}
	UppleLog2(__FILE__,__LINE__, "SALEMASTERKEY = [%s]\n",tmk);

	// SALEPINKEY
	if ((ret = UppleReadRecFldFromRecStr(dbRecord_tmp, strlen(dbRecord_tmp), "SALEPINKEY",
					tpk, sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"UppleReadFldFromTermByFldName:: fldName [SALEPINKEY] not find!\n");
		return (ret);
	}
	UppleLog2(__FILE__,__LINE__, "SALEPINKEY= [%s]\n",tpk);
#endif

	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

#if 0
	UppleReadDatabaseTableOfTerm();


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);
#endif

	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__,"cardno=[%s]\n", cardNo);

	char    up_sek[64+1];

#if 1
	/* tmk */
	memset(tmk_boc,0,sizeof(tmk_boc));
	//ReadUPWorkingKeyfromTableKQ( "MASTERKEY", tmk_boc, up_sek );
	//UppleLog2(__FILE__,__LINE__,"masterkey=[%s]\n", tmk_boc);

	/* tmk */
	memset(zpk,0,sizeof(zpk));
	ReadUPWorkingKeyfromTableKQ( "PINKEY", zpk, up_sek );
	UppleLog2(__FILE__,__LINE__,"pinkey=[%s]\n", zpk);

#else
	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "KQ MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToBOCPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "KQ PINKEY = [%s]\n", zpk );
#endif

#if 0
	if ((ret = add_parity_tmk("SEK_KQ","TEK1",tmk_boc,tmk_p)) < 0)
		//if ((ret = add_parity_tmk_gen("SEK_KQ","TEK1",tmk,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
		return(ret);
	}


#endif

#if 0    
	memset(zpk_p,0,sizeof(zpk_p));
	if ((ret = add_parity_tmk_gen(UppleReadStringTypeRECVar("SEK_KQ"),UppleReadStringTypeRECVar("TEK1"),zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to zpk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "zpk_p= [%s]\n", zpk_p);
#endif

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	return(TranslatePin("SEK1","SEK_KQ",tpk2,zpk,termPin,cardNo,1,1,ylPin));
	//return(TranslatePin("SEK1","SEK_KQ",tpk2,zpk_p,termPin,cardNo,6,1,ylPin));
}

static int set_tid_mid()
{
	char tid[8 + 1] = {0};
	char mid[15 + 1] = {0};
	char fld[2048 + 1] = {0};
	char dbRecord[2048 + 1] = {0};
	int ret;

	UppleGet2(RPUB, "PB41", tid);
	UppleGet2(RPUB, "PB42", mid);
	UppleLog2(__FILE__, __LINE__, "#TEST:PB41=[%s]\n", tid);
	UppleLog2(__FILE__, __LINE__, "#TEST:PB42=[%s]\n", mid);

	sprintf(fld, "SALETID = '%s' and SALEMID='%s' ", tid, mid);
	if ((ret = UppleSelectRealDBRecord("TERMINALPARA", "tid, mid", fld,
					dbRecord, sizeof(dbRecord), NULL)) < 0)
	{
		UppleLog2( __FILE__,__LINE__, "Channel Select notfound.\n\n");
		return -1;
	}

	char tmp[24] = {0};
	memset(tmp, 0, sizeof(tmp));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
					"tid", tmp, sizeof(tmp))) < 0)
	{
		UppleLog2( __FILE__,__LINE__, "Channel tid notfound.\n\n");
		return -1;
	}
	UppleLog2( __FILE__,__LINE__, "Channel tid =[%s]\n", tmp);
	//strcpy(tid_t, tmp);
	UppleSet2(RPUB, "PB41", tmp);

	memset(tmp, 0, sizeof(tmp));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
					"mid", tmp, sizeof(tmp))) < 0)
	{
		UppleLog2( __FILE__,__LINE__, "Channel mid notfound.\n\n");
		return -1;
	}
	UppleLog2( __FILE__,__LINE__, "Channel mid =[%s]\n", tmp);
	//strcpy(mid_t, tmp);
	UppleSet2(RPUB, "PB42", tmp);

	return 0;
}

int UppleTranslateOCXPinToNBPin(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char	tmk_p[48+1];
	char	zpk_p[48+1];


	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "PB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if(ret==0){
		return 0;
	}


	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

	UppleReadDatabaseTableOfTerm();


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}

	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "BOC MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToBOCPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "NB PINKEY = [%s]\n", zpk );

	if ((ret = add_parity_tmk("SEK_NB","TEK_NB",tmk_boc,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak("SEK_NB","SEK_NB",tmk_p,zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
		return(ret);
	}

	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK_NB","SEK_NB",tmk_p,zpk_p,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	return(TranslatePin("SEK1","SEK_NB",tpk2,zpk2,termPin,cardNo,6,1,ylPin));

}

/*
   易宝OCX转PIN
 */
int UppleTranslateOCXPinToYBPin(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char    tmk_p[48+1];
	char    zpk_p[48+1];


	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "PB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	ret = strlen(termPin);
	if( memcmp(termPin,"FFFFFFFFFFFFFFFF",16) == 0)
	{
		UppleLog2(__FILE__,__LINE__,"无密交易YB\n");
		UppleSetYLFld(22,"022",3);
		return 0;
	}
	if(ret==0){
		return 0;
	}


	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

	UppleReadDatabaseTableOfTerm();


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}

	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk_YB(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "BOC MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk_YB(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToBOCPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "YB PINKEY = [%s]\n", zpk );

	if ((ret = add_parity_tmk("SEK_YB","TEK_YB",tmk_boc,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak("SEK_YB","SEK_YB",tmk_p,zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
		return(ret);
	}

	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK_YB","SEK_YB",tmk_p,zpk_p,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	return(TranslatePin("SEK1","SEK_YB",tpk2,zpk2,termPin,cardNo,6,1,ylPin));

}

/*
 * 宁波无线物流转PIN处理
 * by zhouzhenfan
 */
int UppleTranslatePOSPinToNBPospPin(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char	tmk_p[48+1];
	char	zpk_p[48+1];


	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "PB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);

	/*ZWB 2013-07-27*/
	ret = strlen(termPin);
	////////////////////////

	if(ret==0){
		return 0;
	}


	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

	UppleReadDatabaseTableOfTerm();


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}

	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "BOC MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "NB PINKEY = [%s]\n", zpk );

	if ((ret = add_parity_tmk("SEK_NB","TEK_NB",tmk_boc,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak("SEK_NB","SEK_NB",tmk_p,zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: add parity to tmk!\n");
		return(ret);
	}

	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK_NB","SEK_NB",tmk_p,zpk_p,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	return(TranslatePin("SEK1","SEK_NB",tpk2,zpk2,termPin,cardNo,1,1,ylPin));

}



/*易宝转PIN*/
/*create at 2013-11-04*/
int UppleTranslatePOSPinToYBPospPin(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char	tmk_p[48+1];
	char	zpk_p[48+1];


	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "PB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);

	/*ZWB 2013-07-27*/
	ret = strlen(termPin);
	////////////////////////

	if(ret==0){
		return 0;
	}


	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

	UppleReadDatabaseTableOfTerm();


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}

	memset(tmk_boc,0,sizeof(tmk_boc));
	//if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	if ((ret = UppleReadHsmTmk_YB(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "BOC MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	//if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)
	if ((ret = UppleReadHsmTpk_YB(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "YB PINKEY = [%s]\n", zpk );

	if ((ret = add_parity_tmk("SEK_YB","TEK_YB",tmk_boc,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak("SEK_YB","SEK_YB",tmk_p,zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: add parity to tmk!\n");
		return(ret);
	}

	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK_YB","SEK_YB",tmk_p,zpk_p,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	return(TranslatePin("SEK1","SEK_YB",tpk2,zpk2,termPin,cardNo,1,1,ylPin));

}
/*
 * wireless EBS translate PIN 
 * by zhouzhenfan
 */
#if 0
int EBSTranslatePin(char *ylPin, int flag)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char	tmk_p[48+1];
	char	zpk_p[48+1];


	memset(termPin,0,sizeof(termPin));

	if (flag == 1)
	{
		/* Get PinBlock from FLD52 */
		UppleGet2( RPUB, "PB52",  termPin );
		UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	}
	else
	{
		/*
		 * Just Transfer and putcash
		 * Get PinBlock from FLD63
		 */
		char tmp_pin[32+1] = {0};
		int pinlen = 0;
		pinlen = UppleReadTermFld(63, tmp_pin, sizeof(tmp_pin));
		bcdhex_to_aschex(tmp_pin, 8, termPin);
		UppleLog2(__FILE__,__LINE__,"################field 63:[%s]###########\n",termPin);
	}


	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

	/* Queury table TERMINALPARA */
	UppleReadDatabaseTableOfTerm();

	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "CardNo = [%s]\n", cardNo);

#if     1 /* add by zhouzhenfan, 2012.11.23 */
	char fld_46[12] = {0};
	int len_46 = 0;
	if ((len_46 = UppleReadTermFld(46, fld_46, sizeof(fld_46))) < 0)
	{
		UppleLog2(__FILE__, __LINE__, "ReadTermFld 46 failure.\n");
		return len_46;
	}
	if (len_46 > 0) /* NetPay */
	{
		memset(cardNo, 0x0, sizeof(cardNo));
	}
	UppleLog2(__FILE__,__LINE__, "CardNo end = [%s]\n", cardNo);
#endif

#if 1
	/* tmk 获取主密钥 */
	memset(tmk_boc,0,sizeof(tmk_boc));
	UppleReadTermEcardMasterKey(tmk_boc, sizeof(tmk_boc));
	UppleLog2(__FILE__,__LINE__,"ecard_masterkey=[%s]\n", tmk_boc);

	/* tak 获取PINKEY */
	memset(zpk,0,sizeof(zpk));
	UppleReadTermEcardPinKey(zpk, sizeof(zpk));
	UppleLog2(__FILE__,__LINE__,"ecard_pinkey=[%s]\n", zpk);
#else
	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "BOC MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "NB PINKEY = [%s]\n", zpk );
#endif

	if ((ret = add_parity_tmk("SEK1","TEK1",tmk_boc,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak("SEK1","SEK1",tmk_p,zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: add parity to tmk!\n");
		return(ret);
	}

	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK1","SEK1",tmk_p,zpk_p,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	if (len_46 > 0)
		return(TranslatePin("SEK1","SEK1",tpk2,zpk2,termPin,cardNo,6,6,ylPin));
	else
		return(TranslatePin("SEK1","SEK1",tpk2,zpk2,termPin,cardNo,1,1,ylPin));

}
#else
int EBSTranslatePin(char *ylPin, int flag)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char    tmk_p[48+1];
	char    zpk_p[48+1];
	int     len_47 = 0;
	char    fld047[128]={0};

	memset(termPin,0,sizeof(termPin));

	if (flag == 1)
	{
		/* Get PinBlock from FLD52 */
		UppleGet2( RPUB, "PB52",  termPin );
		UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	}
	else
	{
		/*
		 * Just Transfer and putcash
		 * Get PinBlock from FLD63
		 */
		char tmp_pin[32+1] = {0};
		int pinlen = 0;
		pinlen = UppleReadTermFld(63, tmp_pin, sizeof(tmp_pin));
		bcdhex_to_aschex(tmp_pin, 8, termPin);
		UppleLog2(__FILE__,__LINE__,"################field 63:[%s]###########\n",termPin);
		if((memcmp(termPin,"FFFFFFFFFFFFFFFF",16) == 0) || (pinlen == 0))
		{
			UppleLog2(__FILE__,__LINE__,"##############e卡无密交易########\n",termPin);
			return 0;
		}
	}


	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

	/* Queury table TERMINALPARA */
	UppleReadDatabaseTableOfTerm();

	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);


	// 读取卡号
	/*2014-3-17 zwb 增加e卡手机号支付特征处理*/
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadCardNoFromTermPackage!\n");
		//return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "CardNo = [%s]\n", cardNo);

	/*如果是手机号支付，PINBLOCK 不需要卡号参与运算*/
	len_47 = UppleReadTermFld(47,fld047,sizeof(fld047));
	UppleLog2(__FILE__,__LINE__,"UppleReadTermFld 47 len[%d]\n",len_47);

#if     1 /* add by zhouzhenfan, 2012.11.23 */
	char fld_46[12] = {0};
	int len_46 = 0;
	if ((len_46 = UppleReadTermFld(46, fld_46, sizeof(fld_46))) < 0)
	{
		UppleLog2(__FILE__, __LINE__, "ReadTermFld 46 failure.\n");
		return len_46;
	}
	if (len_46 > 0) /* NetPay */
	{
		memset(cardNo, 0x0, sizeof(cardNo));
	}
	UppleLog2(__FILE__,__LINE__, "CardNo end = [%s]\n", cardNo);
#endif

#if 1
	/* tmk 获取主密钥 */
	memset(tmk_boc,0,sizeof(tmk_boc));
	UppleReadTermEcardMasterKey(tmk_boc, sizeof(tmk_boc));
	UppleLog2(__FILE__,__LINE__,"ecard_masterkey=[%s]\n", tmk_boc);

	/* tak 获取PINKEY */
	memset(zpk,0,sizeof(zpk));
	UppleReadTermEcardPinKey(zpk, sizeof(zpk));
	UppleLog2(__FILE__,__LINE__,"ecard_pinkey=[%s]\n", zpk);
#else
	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "BOC MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "NB PINKEY = [%s]\n", zpk );
#endif

	if ((ret = add_parity_tmk("SEK1","TEK1",tmk_boc,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak("SEK1","SEK1",tmk_p,zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: add parity to tmk!\n");
		return(ret);
	}

	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK1","SEK1",tmk_p,zpk_p,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	if (len_46 > 0)
	{
		UppleLog2(__FILE__,__LINE__,"e卡转PIN，存在46域!\n");
		return(TranslatePin("SEK1","SEK1",tpk2,zpk2,termPin,cardNo,6,6,ylPin));
	}
	else if((len_47 > 0)&&(strlen(cardNo) <= 0))            /*此情况使用与手机号激活及手机号支付*/
	{
		UppleLog2(__FILE__,__LINE__,"e卡转PIN，存在47域且不存在卡号!\n");
		return(TranslatePin("SEK1","SEK1",tpk2,zpk2,termPin,cardNo,6,6,ylPin));
	}
	else
	{
		UppleLog2(__FILE__,__LINE__,"e卡转PIN,带卡号!\n");
		return(TranslatePin("SEK1","SEK1",tpk2,zpk2,termPin,cardNo,1,1,ylPin));
	}

}
#endif

int UppleTranslateOCXPinToGenPospPin(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char	tmk_p[48+1];
	char	zpk_p[48+1];
	char channel[8];
	char sek_index[9];
	char tek_index[9];
	char parity[9];


	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "PB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if(ret==0){
		return 0;
	}


	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

	UppleReadDatabaseTableOfTerm();


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //SaleMASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}

	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "BOC MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)  //Pinkey
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToBOCPin:: Error read BOC zpk !\n");
		return(ret);
	}
	//UppleLog2(__FILE__,__LINE__, "NB PINKEY = [%s]\n", zpk );

	memset(channel, 0, sizeof(channel));
	UppleGet2(RPUB,"#Channel", channel);

	memset(sek_index, 0, sizeof(sek_index));
	memset(tek_index, 0, sizeof(tek_index));
	memset(parity, 0, sizeof(parity));
	if ((ret = get_channel_key(channel, sek_index, tek_index, parity)) < 0) {

		UppleUserErrLog2(__FILE__, __LINE__,
				"in generate gen posp mac :: get channel info!\n");
		return (ret);
	}


	/*
	   if ((ret = add_parity_tmk("SEK_NB","TEK_NB",tmk_boc,tmk_p)) < 0)
	   {
	   UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
	   return(ret);
	   }


	   if ((ret = add_parity_tak("SEK_NB","SEK_NB",tmk_p,zpk,zpk_p)) < 0)
	   {
	   UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
	   return(ret);
	   }
	 */
	if (atoi(parity)) {
		if ((ret =
					add_parity_tmk_gen(sek_index, tek_index, tmk_boc, tmk_p)) < 0) {
			UppleUserErrLog2(__FILE__, __LINE__,
					"in generate nb mac  :: add parity to tmk!\n");
			return (ret);
		}

		if ((ret =
					add_parity_tak_gen(sek_index, sek_index, tmk_p, zpk,
						zpk_p)) < 0) {
			UppleUserErrLog2(__FILE__, __LINE__,
					"in generate nb mac  :: add parity to tmk!\n");
			return (ret);
		}
		//UppleMemLog2(__FILE__,__LINE__,"###xor result1\n",xor_res,8);
	} else {
		memcpy(tmk_p, tmk_boc, strlen(tmk_boc));
		memcpy(zpk_p, zpk, strlen(zpk));
	}




	memset(zpk2,0,sizeof(zpk2));
	//        UppleExchKey("1","SEK_NB","SEK_NB",tmk_p,zpk_p,zpk2);
	UppleExchKeyGen("1", sek_index, sek_index, tmk_p, zpk_p, zpk2);
	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	return(TranslatePinGenPosp("SEK1",sek_index,tpk2,zpk2,termPin,cardNo,6,1,ylPin));

}

/*
 *       判断是否资金归集、对外付款交易
 */


int is_cash_sweep()
{
	char    extra_data[512];

	memset(extra_data,0x00,sizeof(extra_data));

	UppleGet2(RPUB, "PB65", extra_data);
	UppleLog2(__FILE__,__LINE__,"###extra_data = [%s]\n",extra_data);

	switch(extra_data[0]){

		case 'F':
			return 1;
		case 'K':
			return 1;
		case 'Y':
			return 1;
		case 'C':
			return 1;
		default:
			return -1;
	}
}

int UppleTranslateKQPinToPPPin(char *ylPin)
{

	char    zpk[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    tmk2[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	char    up_sek[64+1];
	int     ret;
	char    term_sek[8+1];
	char channel[10];
	char ProcessCode[10];
	char CardNo[20];
	int flag = 0;

	memset(termPin,0,sizeof(termPin));
	if ((ret = UppleReadTermFld(52,termPin,sizeof(termPin))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadTermFld 52!\n");
		return(ret);
	}       

	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if(ret==0){
		return 0;
	}       


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);            
	}       

	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);            
	}       


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0){
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__,"tpk = [%s],tmk = [%s],cardNo = [%s]\n",tpk,tmk,cardNo);


	memset(term_sek,0x00,sizeof(term_sek));

	if ((ret = UppleReadTermSEK(term_sek,sizeof(term_sek))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}

	//strcpy(term_sek,"SEK_KQ");
	UppleLog2(__FILE__,__LINE__,"term_sek = [%s]\n",term_sek);

	memset(tpk2,0,sizeof(tpk2));
	memset(tmk2,0,sizeof(tmk2));
	//add_parity_tmk("SEK_KQ","0001",tmk,tmk2);
	//UppleExchKey("1","SEK1","SEK2",tmk,tpk,tpk2);
	UppleExchKey("1",term_sek,term_sek,tmk,tpk,tpk2);

	memset( zpk, 0x00, sizeof(zpk) );
	memset( up_sek, 0x00, sizeof(up_sek) );
	ReadUPWorkingKeyfromTable( "PINKEY", zpk, up_sek );

	//strcpy(zpk,UppleReadQZJYLZpk());
	if( !memcmp( zpk, "\x00\x00\x00\x000", 4 ) )  //没取到
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__,"term_sek = [%s],up_sek = [%s]\n",term_sek,up_sek);
	return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,6,1,ylPin));

}

#if 1
// yangzhijun 12-02-2013 begin 
/*******************************************************************************
 *FuncName:    	UppleVerifyYlTransMacByTMK 
 *Function:     自助终端:支付时验MAC
 *Para:         @
 *Return:       0               成功
 1               失败
 *Author:       Yang zhijun
 *Date:         10-29-2013
 *******************************************************************************/
int UppleVerifySelfTermTransMac()
{
	int     ret;
	char    localMac[32+1];
	char    remoteMac[32+1];
	char    macData[8192+1];
	int     lenOfMacData;
	char    tmk[48+1];
	char    tak[32+1];
	int     retryTimes = 0;

	if (!UppleIsTermTransMacExists())               // 检查MAC域是否存在，在环境变量中设置
		return(0);
	if (!UppleIsVerifyTermMacNeeded())              // 检查是否要验证MAC
		return(0);

	memset(tak,0,sizeof(tak));
	if ((ret = UppleReadHsmTak(tak,sizeof(tak))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleReadHsmTak!\n");
		return(ret);
	}

	memset(remoteMac,0,sizeof(remoteMac));
	if ((ret = UppleReadTermFld(64,remoteMac,sizeof(remoteMac))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleReadTermFld 64!\n");
		return(ret);
	}
	memset(macData,0,sizeof(macData));
	if ((lenOfMacData = UppleTermMergeMacData(UppleGetTermPackage(),UppleFindMacFldTBLDef("termTransMacTBL"),macData,sizeof(macData)-1)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleTermMergeMacData!\n");
		return(lenOfMacData);
	}
	macData[lenOfMacData] = 0;
	memset(localMac,0,sizeof(localMac));

	if ((ret = GenSelfTermMac(macData, lenOfMacData, localMac)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleVerifyTermTransMac:: UppleGenerateMac!\n");
		return (ret);
	}
	UppleLog2(__FILE__, __LINE__, "#SelfTerm localMac=[%s]\n", localMac);
	UppleLog2(__FILE__, __LINE__, "#SelfTerm remoteMac=[%s]--term64\n", remoteMac);

#if 1
	//test mac=123456
	char testMac[128+1];
	UppleLog2(__FILE__, __LINE__, "#######用123456作为macData计算开始testMac=[%s]#######\n", testMac);
	memset(testMac, 0x00, sizeof(testMac));
	if ((ret = GenSelfTermMac("123456", 6, testMac)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleVerifyTermTransMac:: UppleGenerateMac!\n");
		return (ret);
	}
	UppleLog2(__FILE__, __LINE__, "#SelfTerm testMac=[%s]\n", testMac);
	UppleLog2(__FILE__, __LINE__, "#SelfTerm remoteMac=[%s]--term64\n", remoteMac);
	UppleLog2(__FILE__, __LINE__, "#######用123456作为macData计算结束testMac=[%s]#######\n", testMac);


	//test mac=0210702000C042C008031962226037300022
	memset(testMac, 0x00, sizeof(testMac));

	char testData[64+1];
	memset(testData, 0x00, sizeof(testData));
	memcpy(testData, "0210702000C042C008031962226037300022", sizeof(testData));
	UppleLog2(__FILE__, __LINE__, "#######用0210702000C042C008031962226037300022作为macData计算开始testMac=[%s]#######\n", testMac);
#endif
	if ((ret = GenSelfTermMac(testData, strlen(testData), testMac)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleVerifyTermTransMac:: UppleGenerateMac!\n");
		return (ret);
	}
	UppleLog2(__FILE__, __LINE__, "#SelfTerm testMac=[%s]\n", testMac);
	UppleLog2(__FILE__, __LINE__, "#SelfTerm remoteMac=[%s]--term64\n", remoteMac);
	UppleLog2(__FILE__, __LINE__, "#######用0210702000C042C008031962226037300022作为macData计算结束testMac=[%s]#######\n", testMac);

	if (!memcmp(localMac, remoteMac, 8))
		UppleLog2(__FILE__, __LINE__, "###校验SelfTerm OK.\n");
	else
	{
		UppleLog2(__FILE__, __LINE__, "###校验SelfTerm ERROR!.\n");
		return(errCodeCurrentMDL_8583PackageMacNotSameAsThatLocalGenerated);
	}

	UppleUserErrLog2(__FILE__,__LINE__,"###较验终端MAC正确:: remoteMac = [%s] localMac = [%s]\n", remoteMac,localMac );
	return(0);
}

/*******************************************************************************
 *FuncName:    	GenSelfTermMac 
 *Function:     自助终端:连加密机生成MAC
 *Para:         @
 *Return:       0               成功
 1               失败
 *Author:       Yang zhijun
 *Date:         10-29-2013
 *******************************************************************************/
int GenSelfTermMac(char *macData, int lenOfMacData, char *localMac)
{

	int ret;
	char xor_res[8];
	char aschex[16 + 1];
	char mid_mak[8];
	char temp[16 + 1];
	char temp2[32 + 1];
	char high[8];
	char tak[48 + 1];
	char tak2[48 + 1];
	char tmk[48 + 1];
	char term_type[8 + 1];
	char term_sek[8 + 1];

	memset(temp, 0x00, sizeof(temp));
	UppleGet("PB42", temp);
	UppleGet2(RPUB, "PB42", temp);
	UppleUserErrLog2(__FILE__,__LINE__,"ttttttttttttttttttttttt###PB42 = [%s]\n", temp );
	if ((ret = UppleSetTermFld(42, temp, 15)) < 0)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"  [%s-%d]in UppleReadDatabaseTableOfTerm:: UppleReadTermFld 41!\n",
				__FILE__, __LINE__);
		return (ret);
	}
	// 读取终端表
	if ((ret = UppleReadDatabaseTableOfTerm()) < 0)
	{
		UppleUserErrLog2(
				__FILE__,
				__LINE__,
				"  [%s-%d]in UppleTranslateTermPinToYLPin:: UppleReadDatabaseTableOfTerm!\n",
				__FILE__, __LINE__);
		return (ret);
	}

	memset(tmk, 0, sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk, sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return (ret);
	}

	memset(tak, 0, sizeof(tak));
	if ((ret = UppleReadTermTak(tak, sizeof(tak))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleVerifyTermTransMac:: UppleReadHsmTak!\n");
		return (ret);
	}
	memset(term_type, 0, sizeof(term_type));
	if (ret = UppleReadFldFromTermByFldName("TermType", term_type,
				sizeof(term_type)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__, "fldName [termType] not find!\n"); //
		return (ret);
	}
	UppleLog2(__FILE__, __LINE__, "term_type=[%s]\n", term_type);

	UppleSet2(RPUB, "#term_type", term_type);

	//民生终端，专用前端密钥
	memset(term_sek, 0x00, sizeof(term_sek));
	sprintf(term_sek, "SEK%d", atol(term_type));
	UppleLog2(__FILE__, __LINE__, "TERM SEK=[%s]\n", term_sek);

	memset(tak2, 0, sizeof(tak2));
	UppleExchKey("1", term_sek, term_sek, tmk, tak, tak2);

	memset(temp, 0x00, sizeof(temp));
	bcdhex_to_aschex(tak2, 16, temp);

	UppleGenerateMacJN_KQ("2", term_sek, tak2, macData, lenOfMacData, localMac);

	return 0;
}
// yangzhijun 12-02-2013 end 
#endif


/*ZWB add by e card sale cancel*/
#if 1
int EBSTranslatePinSaleVoid(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char    tmk_p[48+1];
	char    zpk_p[48+1];


	memset(termPin,0,sizeof(termPin));

#if 0

	if (flag == 1)
	{
		/* Get PinBlock from FLD52 */
		UppleGet2( RPUB, "PB52",  termPin );
		UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	}
	else
	{
		/*
		 * Just Transfer and putcash
		 * Get PinBlock from FLD63
		 */
		char tmp_pin[32+1] = {0};
		int pinlen = 0;
		pinlen = UppleReadTermFld(63, tmp_pin, sizeof(tmp_pin));
		bcdhex_to_aschex(tmp_pin, 8, termPin);
		UppleLog2(__FILE__,__LINE__,"################field 63:[%s]###########\n",termPin);
	}

#else
	int len_fld52 = -1;
	UppleGet2( RPUB, "PB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	len_fld52 = strlen(termPin);
	if( len_fld52 == 0)
	{
		UppleLog2(__FILE__,__LINE__,"##ecard no 52\n");
		return 0;
	}
#endif
	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

	/* Queury table TERMINALPARA */
	UppleReadDatabaseTableOfTerm();

	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);


	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "CardNo = [%s]\n", cardNo);

#if     1 /* add by zhouzhenfan, 2012.11.23 */
	char fld_46[12] = {0};
	int len_46 = 0;
	if ((len_46 = UppleReadTermFld(46, fld_46, sizeof(fld_46))) < 0)
	{
		UppleLog2(__FILE__, __LINE__, "ReadTermFld 46 failure.\n");
		return len_46;
	}
	if (len_46 > 0) /* NetPay */
	{
		memset(cardNo, 0x0, sizeof(cardNo));
	}
	UppleLog2(__FILE__,__LINE__, "CardNo end = [%s]\n", cardNo);
#endif

#if 1
	/* tmk 获取主密钥 */
	memset(tmk_boc,0,sizeof(tmk_boc));
	UppleReadTermEcardMasterKey(tmk_boc, sizeof(tmk_boc));
	UppleLog2(__FILE__,__LINE__,"ecard_masterkey=[%s]\n", tmk_boc);

	/* tak 获取PINKEY */
	memset(zpk,0,sizeof(zpk));
	UppleReadTermEcardPinKey(zpk, sizeof(zpk));
	UppleLog2(__FILE__,__LINE__,"ecard_pinkey=[%s]\n", zpk);
#else
	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "BOC MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "NB PINKEY = [%s]\n", zpk );
#endif

	if ((ret = add_parity_tmk("SEK1","TEK1",tmk_boc,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak("SEK1","SEK1",tmk_p,zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: add parity to tmk!\n");
		return(ret);
	}

	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK1","SEK1",tmk_p,zpk_p,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	if (len_46 > 0)
		return(TranslatePin("SEK1","SEK1",tpk2,zpk2,termPin,cardNo,6,6,ylPin));
	else
		return(TranslatePin("SEK1","SEK1",tpk2,zpk2,termPin,cardNo,1,1,ylPin));

}
#endif  

/*
//易结款（对外付款、资金归集）接宁波渠道，转后端的PIN
//因宁波是POSP签到，且易结款的后端商户和消费的商户号可能不是同一个，需新起一个转PIN
//add by zwb 2014-04-01
 */
int UppleTranslatePOSPinToNBPospPinYJK(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char    tmk_p[48+1];
	char    zpk_p[48+1];

	char s_mid[20]={0};
	char s_tid[12]={0};
	char where[2048] = {0};
	char dbRecord[2048] = {0};
	char fld041[20]={0};
	char fld042[20]={0};
	long port = 0;
	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "PB52",  termPin );
	UppleGet2L( RPUB, "#port", &port );
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s],prot=[%ld]###########\n",termPin,port);

	/*ZWB 2013-07-27*/
	ret = strlen(termPin);
	////////////////////////

	if(ret==0 || (memcmp(termPin,"FFFFFFFFFFFFFFFF",16) == 0))
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslatePOSPinToNBPospPinYJK :: not find PIN\n");
		char fld023[8]={0};
		UppleReadYLFld(23,fld023,sizeof(fld023)-1);
		if(strlen(fld023) > 0)
		{
			UppleLog2(__FILE__,__LINE__,"in UppleTranslatePOSPinToNBPospPinYJK :: IC not PIN\n");
			UppleSetYLFld(22,"052",3);
			UppleSetYLFldNull(26);
		}
		else
		{
			UppleLog2(__FILE__,__LINE__,"in UppleTranslatePOSPinToNBPospPinYJK :: no IC not PIN\n");
			UppleSetYLFld(22,"022",3);
			UppleSetYLFldNull(26);
		}
		return 0;
	}


	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

	UppleReadDatabaseTableOfTerm();

#if 0
	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPinYJK:: UppleReadHsmTpk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPinYJK:: UppleReadHsmTmk!\n");
		return(ret);
	}
	// UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);
#else
	memset(fld041,0,sizeof(fld041));
	memset(fld042,0,sizeof(fld042));
	if((ret = UppleReadTermFld(41,fld041,sizeof(fld041))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPinYJK:: read Term fld 41 error!\n");
		return(ret);
	}
	if((ret = UppleReadTermFld(42,fld042,sizeof(fld042))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPinYJK:: read Term fld 42 error!\n");
		return(ret);
	}
	memset(where, 0, sizeof(where));
	sprintf(where,"tid like '%08s' and mid like '%15s'",fld041,fld042);

	memset(dbRecord, 0, sizeof(dbRecord));
	if ((ret = UppleSelectRealDBRecord("TERMINALPARA", "SALEMASTERKEY,SALEPINKEY",
					where, dbRecord, sizeof(dbRecord), NULL)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleTranslateOCXPinToNBPospPinYJK::UppleSelectRealDBRecord not find record!\n");
		return ret;
	}

	memset(tmk, 0, sizeof(tmk));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
					"SALEMASTERKEY", tmk, sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleTranslateOCXPinToNBPospPinYJK::UppleSelectRealDBRecord [SALEMASTERKEY] not found!\n");
		return ret;
	}

	memset(tpk, 0, sizeof(tpk));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
					"SALEPINKEY", tpk , sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleTranslateOCXPinToNBPospPinYJK::UppleSelectRealDBRecord [SALEPINKEY] not found!\n");
		return ret;
	}
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPinYJK:: term_fld41=[%s],term_fld42=[%s]\n\
			SALEMASTERKEY=[%s],SALEPINKEY=[%s]\n",\
			fld041,fld042,tmk,tpk);
#endif

	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPinYJK:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}

#if 0   /*易结款（对外付款、资金归集）的后端密钥不取同一条终端表记录的*/
	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "BOC MASTERKEY = [%s]\n",tmk_boc);

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "NB PINKEY = [%s]\n", zpk );
#else

	if((ret = UppleReadYLFld(41,s_tid,sizeof(s_tid))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPinYJK:: read yl fld 41 error!\n");
		return(ret);
	}
	if((ret = UppleReadYLFld(42,s_mid,sizeof(s_mid))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPinYJK:: read yl fld 42 error!\n");
		return(ret);
	}
	memset(where, 0, sizeof(where));
	sprintf(where,"tid like '%08s' and mid like '%15s'",s_tid,s_mid);

	memset(dbRecord, 0, sizeof(dbRecord));
	if ((ret = UppleSelectRealDBRecord("TERMINALPARA", "MASTERKEY,PINKEY",
					where, dbRecord, sizeof(dbRecord), NULL)) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleTranslateOCXPinToNBPospPinYJK::UppleSelectRealDBRecord not find record!\n");
		return ret;
	}

	memset(tmk_boc, 0, sizeof(tmk_boc));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
					"MASTERKEY", tmk_boc, sizeof(tmk_boc))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleTranslateOCXPinToNBPospPinYJK::UppleSelectRealDBRecord [MASTERKEY] not found!\n");
		return ret;
	}

	memset(zpk, 0, sizeof(zpk));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
					"PINKEY", zpk , sizeof(zpk))) < 0)
	{
		UppleUserErrLog2(__FILE__, __LINE__,
				"in UppleTranslateOCXPinToNBPospPinYJK::UppleSelectRealDBRecord [PINKEY] not found!\n");
		return ret;
	}
#endif

	if ((ret = add_parity_tmk("SEK_NB","TEK_NB",tmk_boc,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPinYJK:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak("SEK_NB","SEK_NB",tmk_p,zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToNBPospPinYJK:: add parity to tmk!\n");
		return(ret);
	}

	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK_NB","SEK_NB",tmk_p,zpk_p,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	if(8385 == port)
	{
		UppleLog2(__FILE__,__LINE__,"int UppleTranslatePOSPinToNBPospPinYJK:: NEW pos");
		return(TranslatePin("SEK1","SEK_NB",tpk2,zpk2,termPin,cardNo,1,1,ylPin));
	}
	else
	{
		UppleLog2(__FILE__,__LINE__,"int UppleTranslatePOSPinToNBPospPinYJK:: old pos");
		return(TranslatePin("SEK1","SEK_NB",tpk2,zpk2,termPin,cardNo,6,1,ylPin));
	}

}


/*
 *	CUPS 验证终端MAC
 */
int UppleVerifyCUPSTermTransMac(char *buf,int len)
{
	int ret;
	char    localMac[32+1];
	char    remoteMac[32+1];
	char    macData[8192+1];
	int     lenOfMacData;
	char    tak[32+1];
	int	lenOfTermHeader;

	if (!UppleIsTermTransMacExists())			// 检查MAC域是否存在，在环境变量中设置
		return(0);
	if (!UppleIsVerifyTermMacNeeded())		// 检查是否要验证MAC
		return(0);


	memset(remoteMac,0x00,sizeof(remoteMac));
	UppleGet2(RPUB,"TB64",remoteMac);
	lenOfTermHeader=UppleGetLengthOfTermPackageHeader(); 
	lenOfMacData=len-lenOfTermHeader-MACLEN;

	memset(macData,0,sizeof(macData));
	memcpy(macData,buf+lenOfTermHeader,len-lenOfTermHeader-MACLEN);

	UppleMemLog2(__FILE__,__LINE__,"macData=",macData,lenOfMacData);

	if ((ret = UppleGenerateCUPSTermMac(macData,lenOfMacData,localMac)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyCUPSTermTransMac:: UppleGenerateMac!\n");
		return(ret);
	}

	if (strncmp(localMac,remoteMac,8) != 0)
	{
		UppleLog("in UppleVerifyTermTransMac:: remoteMac = [%s] localMac = [%s] macData = [%s] macDataLen = [%d]\n",
				remoteMac,localMac,macData,lenOfMacData);
		return(errCodeCurrentMDL_8583PackageMacNotSameAsThatLocalGenerated);
	}
	UppleUserErrLog2(__FILE__,__LINE__,"###较验终端MAC正确:: remoteMac = [%s] localMac = [%s]\n", remoteMac,localMac );

	return(0);

}
int UppleTranslateTermPinToCUPSPin(char *ylPin)
{
	char	zpk[48+1];
	char	tpk[48+1];
	char	tpk2[48+1];
	char	tmk[48+1];
	char	cardNo[40];
	char	termPin[32+1];
	char	up_sek[64+1];
	char    term_sek[8+1];
	int	ret;

	memset(termPin,0,sizeof(termPin));
	/*
	   if ((ret = UppleReadTermFld(52,termPin,sizeof(termPin))) < 0)
	   {
	   UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadTermFld 52!\n");
	   return(ret);
	   }
	 */
	UppleGet2(RPUB, "PB52", termPin);
	UppleLog2(__FILE__, __LINE__, "#TEST:Get TERM PIN = [%s]\n", termPin);
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if(strlen(termPin)==0){
		return 0;
	}

	ReadUPWorkingKeyfromTableCUPS( "PINKEY", zpk, up_sek );
	//strcpy(zpk,UppleReadQZJYLZpk());
	if( !memcmp( zpk, "\x00\x00\x00\x000", 4 ) )  //没取到
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
		return(ret);
	}

	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);		
	}

	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}

	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}

	memset(tpk2,0,sizeof(tpk2));
	UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);

	memset(term_sek,0,sizeof(term_sek));
	if ((ret = UppleReadTermSEK(term_sek,sizeof(term_sek))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}

	//UppleExchKey("1","SEK1","SEK2",tmk,tpk,tpk2);
	UppleExchKey("1",term_sek,term_sek,tmk,tpk,tpk2);

	//return(TranslatePin("SEK1","SEK2",tpk2,zpk,termPin,cardNo,1,6,ylPin));
	return(TranslatePin("SEK1",up_sek,tpk2,zpk,termPin,cardNo,1,1,ylPin));

}
int UppleTranslateOCXPinToCUPSPin(char *ylPin)
{
	char	zpk[48+1];
	char	tpk[48+1];
	char	tpk2[48+1];
	char	tmk[48+1];
	char	cardNo[40];
	char	termPin[32+1];
	char	up_sek[64+1];
	int	ret;
	char    term_sek[8+1];
	int flag = 0;

	memset(termPin,0,sizeof(termPin));
	/*
	   if ((ret = UppleReadTermFld(52,termPin,sizeof(termPin))) < 0)
	   {
	   UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadTermFld 52!\n");
	   return(ret);
	   }
	 */
	UppleGet2(RPUB, "PB52", termPin);
	UppleLog2(__FILE__, __LINE__, "#TEST:Get TERM PIN = [%s]\n", termPin);
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if(strlen(termPin)==0){
		return 0;
	}


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);		
	}

	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}

	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0){
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	memset(tpk2,0,sizeof(tpk2));
	UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);

	if ((ret = UppleReadTermSEK(term_sek,sizeof(term_sek))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);		
	}

	//UppleExchKey("1","SEK1","SEK2",tmk,tpk,tpk2);
	UppleExchKey("1",term_sek,term_sek,tmk,tpk,tpk2);

	memset( zpk, 0x00, sizeof(zpk) );
	memset( up_sek, 0x00, sizeof(up_sek) );
	ReadUPWorkingKeyfromTable( "PINKEY", zpk, up_sek );

	//strcpy(zpk,UppleReadQZJYLZpk());
	if( !memcmp( zpk, "\x00\x00\x00\x000", 4 ) )  //没取到
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
		return(ret);
	}

	return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,6,1,ylPin));
}
/*
 *	生成CUPS终端MAC
 */

int UppleGenerateCUPSTermMac(char *macData,int lenOfMacData,char *localMac)
{

	int ret;
	char xor_res[8];
	char aschex[16+1];
	char mid_mak[8];
	char temp[16+1];
	char temp2[32+1];
	char high[8];
	char tak[48+1];
	char tak2[48+1];
	char tmk[48+1];


	//	UppleLog2(__FILE__,__LINE__,"###tak string=[%s]\n",tak);
	//	UppleReadDatabaseTableOfTerm();
	UppleReadDatabaseTableOfTermCUPS();

	memset(xor_res,0,sizeof(xor_res));	
	aschex[16]=0;
	memset(tak2,0,sizeof(tak2));
	memset(tmk,0,sizeof(tmk));

	Xor(macData,lenOfMacData,xor_res);
	bcdhex_to_aschex(xor_res,8,aschex);

	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	memset(tak,0,sizeof(tak));
	if ((ret = UppleReadTermTak(tak,sizeof(tak))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyTermTransMac:: UppleReadHsmTak!\n");
		return(ret);		
	}


	//	UppleMemLog2(__FILE__,__LINE__,"###xor result1\n",xor_res,8);

	memcpy(high,aschex,8);

	memset( temp2, 0x00, sizeof(temp2) );
	memset( temp, 0x00, sizeof(temp) );

	UppleExchKey("1","SEK1","SEK2",tmk,tak,tak2);	

	UppleHsmDes(tak2,high,8,temp);

	aschex_to_bcdhex(temp, 16, mid_mak);

	//	UppleMemLog2(__FILE__,__LINE__,"in  UppleGenerateTermMac mid_mak1\n",mid_mak,8);
	memcpy(aschex,mid_mak,8);

	//	UppleMemLog2(__FILE__,__LINE__,"###Before xor 2\n",aschex,16);

	Xor(aschex,16,xor_res);

	//	UppleMemLog2(__FILE__,__LINE__,"###xor result2\n",xor_res,8);

	memset( temp2, 0x00, sizeof(temp2) );
	memset( temp, 0x00, sizeof(temp) );
	bcdhex_to_aschex(xor_res,8,temp2);

	//	UppleLog2(__FILE__,__LINE__,"###high before DES=[%s]",temp2);


	UppleHsmDes(tak2,xor_res,8,temp);

	aschex_to_bcdhex(temp, 16, mid_mak);

	//	UppleMemLog2(__FILE__,__LINE__,"###mid_mak2 hex",mid_mak,8);

	bcdhex_to_aschex(mid_mak,8,aschex);

	//	UppleLog2(__FILE__,__LINE__,"in  UppleGenerateTermMac xor result[%s]\n",aschex);

	memcpy(localMac,aschex,8);	
	//	UppleGenerateMacPOSP(tak,macData,lenOfMacData,localMac);
	//UppleMemLog2(__FILE__,__LINE__,"###HSM localMac",localMac,8);

	return 0;
}
/*
 *	CUPS 验证终端MAC
 */
int UppleVerifyICBCTermTransMac(char *buf,int len)
{
	int ret;
	char    localMac[32+1];
	char    remoteMac[32+1];
	char    macData[8192+1];
	int     lenOfMacData;
	char    tak[32+1];
	int	lenOfTermHeader;

	if (!UppleIsTermTransMacExists())			// 检查MAC域是否存在，在环境变量中设置
		return(0);
	if (!UppleIsVerifyTermMacNeeded())		// 检查是否要验证MAC
		return(0);


	memset(remoteMac,0x00,sizeof(remoteMac));
	UppleGet2(RPUB,"TB64",remoteMac);
	lenOfTermHeader=UppleGetLengthOfTermPackageHeader(); 
	lenOfMacData=len-lenOfTermHeader-MACLEN;

	memset(macData,0,sizeof(macData));
	memcpy(macData,buf+lenOfTermHeader,len-lenOfTermHeader-MACLEN);

	UppleMemLog2(__FILE__,__LINE__,"macData=",macData,lenOfMacData);

	if ((ret = UppleGenerateCUPSTermMac(macData,lenOfMacData,localMac)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyICBCTermTransMac:: UppleGenerateMac!\n");
		return(ret);
	}

	if (strncmp(localMac,remoteMac,8) != 0)
	{
		UppleLog("in UppleVerifyTermTransMac:: remoteMac = [%s] localMac = [%s] macData = [%s] macDataLen = [%d]\n",
				remoteMac,localMac,macData,lenOfMacData);
		return(errCodeCurrentMDL_8583PackageMacNotSameAsThatLocalGenerated);
	}
	UppleUserErrLog2(__FILE__,__LINE__,"###较验终端MAC正确:: remoteMac = [%s] localMac = [%s]\n", remoteMac,localMac );

	return(0);

}

/*ICBC-转PIN*/
int UppleTranslateTermPinToICBCPin(char *ylPin)
{
	char    zpk[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	char    up_sek[64+1];
	char    term_sek[8+1];
	int     ret;
	long    port = 0;
	char    t_tid[20]={0};
	char    t_mid[20]={0};
	char    up_tid[20]={0};
	char    up_mid[20]={0};
	char    up_channel[10]={0};
	char    up_masetrkey[128]={0};
	char    up_masetrkey_p[128]={0};
	char    up_pinkey[128]={0};
	char    up_pinkey_p[128]={0};
	char    up_keyindex[10]={0};
	char    up_pinkey_sek[128]={0};

	memset(termPin,0,sizeof(termPin));
	/*
	   if ((ret = UppleReadTermFld(52,termPin,sizeof(termPin))) < 0)
	   {
	   UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadTermFld 52!\n");
	   return(ret);
	   }
	 */
	UppleGet2L(RPUB, "#port", &port);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin::prot=[%ld]\n",port);

	UppleGet2(RPUB, "PB52", termPin);
	UppleLog2(__FILE__, __LINE__, "#TEST:Get TERM PIN = [%s]\n", termPin);
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if((strlen(termPin)==0)||(0 == memcmp(termPin,"FFFFFFFFFFFFFFFF",16))){
		return 0;
	}
#if 0
	ReadUPWorkingKeyfromTableICBC( "PINKEY", zpk, up_sek );
#else
	UppleGet2(RPUB, "TB41", t_tid);
	UppleGet2(RPUB, "TB42", t_mid);
	UppleSet2(RPUB,"##MID",t_mid);
	UppleSet2(RPUB,"##TID",t_tid);
	sprintf(up_channel,"6001");
	findtheupchannelinfo(up_mid,up_tid,up_channel);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin::t_tid[%s],t_mid[%s],up_tid[%s],up_mid[%s]\n",t_tid,t_mid,up_tid,up_mid);
	ReadWorkingKeyfromUpchannelKey(up_mid,up_tid,up_channel,"up_masterkey",up_masetrkey);
	ReadWorkingKeyfromUpchannelKey(up_mid,up_tid,up_channel,"up_pinkey",up_pinkey);
	ReadWorkingKeyfromUpchannelKey(up_mid,up_tid,up_channel,"up_key_index",up_keyindex);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin::up_masetrkey[%s],up_pinkey[%s],up_keyindex[%s]\n",up_masetrkey,up_pinkey,up_keyindex);


#endif
	//strcpy(zpk,UppleReadQZJYLZpk());
	if( !memcmp( up_pinkey, "\x00\x00\x00\x000", 4 ) )  //没取到
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
		return(ret);
	}

	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}

	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}

	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin::card_no[%s]\n",cardNo);

	if ((ret = add_parity_tmk(up_keyindex,"TEK_ICBC",up_masetrkey,up_masetrkey_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak(up_keyindex,up_keyindex,up_masetrkey_p,up_pinkey,up_pinkey_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin:: add parity to tmk!\n");
		return(ret);
	}

	memset(up_pinkey_sek,0,sizeof(up_pinkey_sek));
	UppleExchKey("1",up_keyindex,up_keyindex,up_masetrkey_p,up_pinkey_p,up_pinkey_sek);

	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin::UppleExchKey 1!\n");

	memset(tpk2,0,sizeof(tpk2));
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin::UppleExchKey over!\n");

	if( 6768 == port)
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin1::port=[%ld]\n",port);
		return(TranslatePin("SEK1",up_keyindex,tpk2,up_pinkey_sek,termPin,cardNo,6,1,ylPin));
	}
	else if((8385 == port) || (8383 == port))
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin3::port=[%ld]\n",port);
		return(TranslatePin("SEK1",up_keyindex,tpk2,up_pinkey_sek,termPin,cardNo,1,1,ylPin));
	}
	else
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCPin2::port=[%ld]\n",port);
		return(TranslatePin("SEK1",up_keyindex,tpk2,up_pinkey_sek,termPin,cardNo,1,1,ylPin));
	}

}


/*
 *	广银联POSP接入OCX转PIN
 *
 */
int UppleTranslateOCXPinToPOSPPinGD(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char	tmk_p[48+1];
	char	zpk_p[48+1];


	long 	port = 0;
	UppleGet2L(RPUB, "#port", &port);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToPPPPin::port=[%ld]\n",port);

	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "PB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if(ret==0){
		return 0;
	}


	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));

	UppleReadDatabaseTableOfTerm1();


	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}


	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //SALEMASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}



	memset(tmk_boc,0,sizeof(tmk_boc));
	if ((ret = UppleReadHsmTmk(tmk_boc,sizeof(tmk_boc))) < 0)  //MASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadHsmTpk(zpk,sizeof(zpk))) < 0)		//PINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToBOCPin:: Error read BOC zpk !\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);
	UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);
	UppleLog2(__FILE__,__LINE__, "MASTERKEY = [%s]\n",tmk_boc);
	UppleLog2(__FILE__,__LINE__, "PINKEY = [%s]\n", zpk );
	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "cardNo = [%s]\n", cardNo );
#if 1 
	if ((ret = add_parity_tmk("SEK_GD_POSP","TEK_NB",tmk_boc,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak("SEK_GD_POSP","SEK_GD_POSP",tmk_p,zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
		return(ret);
	}
	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK_GD_POSP","SEK_GD_POSP",tmk_p,zpk_p,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	//UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);
	if(6768 == port)
		return(TranslatePin("SEK1","SEK_GD_POSP",tpk2,zpk2,termPin,cardNo,6,1,ylPin));
	else
		return(TranslatePin("SEK1","SEK_GD_POSP",tpk2,zpk2,termPin,cardNo,1,1,ylPin));
#else
#if 0
	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK_GD_POSP","SEK_GD_POSP",tmk_p,zpk_p,zpk2);
	memset(tpk2,0,sizeof(tpk2));
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);
	return(TranslatePin("SEK1","SEK_GD_POSP",tpk2,zpk2,termPin,cardNo,6,1,ylPin));
#else
	memset(tpk2,0,sizeof(tpk2));
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);
	return(TranslatePin("SEK1","SEK_GD_POSP",tpk2,zpk,termPin,cardNo,6,1,ylPin));
#endif
#endif


}

/*ABC-转PIN*/
int UppleTranslateTermPinToABCPin(char *ylPin)
{
	char    zpk[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	char    up_sek[64+1];
	char    term_sek[8+1];
	int     ret;
	long    port = 0;
	char    t_tid[20]={0};
	char    t_mid[20]={0};
	char    up_tid[20]={0};
	char    up_mid[20]={0};
	char    up_channel[10]={0};
	char    up_masetrkey[128]={0};
	char    up_masetrkey_p[128]={0};
	char    up_pinkey[128]={0};
	char    up_pinkey_p[128]={0};
	char    up_keyindex[10]={0};
	char    up_pinkey_sek[128]={0};

	memset(termPin,0,sizeof(termPin));
	/*
	   if ((ret = UppleReadTermFld(52,termPin,sizeof(termPin))) < 0)
	   {
	   UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadTermFld 52!\n");
	   return(ret);
	   }
	 */
	UppleGet2L(RPUB, "#port", &port);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::prot=[%ld]\n",port);

	UppleGet2(RPUB, "PB52", termPin);
	UppleLog2(__FILE__, __LINE__, "#TEST:Get TERM PIN = [%s]\n", termPin);
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if((strlen(termPin)==0) || (0 == memcmp(termPin,"FFFFFFFFFFFFFFFF",16))){
		return 0;
	}
	UppleGet2(RPUB, "TB41", t_tid);
	UppleGet2(RPUB, "TB42", t_mid);
	UppleSet2(RPUB,"##MID",t_mid);
	UppleSet2(RPUB,"##TID",t_tid);
#if 0
	sprintf(up_channel,"6001");
#else
	UppleGet2(RPUB,"#up_channel",up_channel);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::up_channel[%s]\n",up_channel);
#endif
	findtheupchannelinfo(up_mid,up_tid,up_channel);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::t_tid[%s],t_mid[%s],up_tid[%s],up_mid[%s]\n",t_tid,t_mid,up_tid,up_mid);
	ReadWorkingKeyfromUpchannelKey(up_mid,up_tid,up_channel,"up_masterkey",up_masetrkey);
	ReadWorkingKeyfromUpchannelKey(up_mid,up_tid,up_channel,"up_pinkey",up_pinkey);
	ReadWorkingKeyfromUpchannelKey(up_mid,up_tid,up_channel,"up_key_index",up_keyindex);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::up_masetrkey[%s],up_pinkey[%s],up_keyindex[%s]\n",up_masetrkey,up_pinkey,up_keyindex);
	//strcpy(zpk,UppleReadQZJYLZpk());
	if( !memcmp( up_pinkey, "\x00\x00\x00\x000", 4 ) )  //没取到
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
		return(ret);
	}

	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}

	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}

	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::card_no[%s]\n",cardNo);

	if ((ret = add_parity_tmk(up_keyindex,"TEK_ICBC",up_masetrkey,up_masetrkey_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak(up_keyindex,up_keyindex,up_masetrkey_p,up_pinkey,up_pinkey_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin:: add parity to tmk!\n");
		return(ret);
	}

	memset(up_pinkey_sek,0,sizeof(up_pinkey_sek));
	UppleExchKey("1",up_keyindex,up_keyindex,up_masetrkey_p,up_pinkey_p,up_pinkey_sek);

	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::UppleExchKey 1!\n");

	memset(tpk2,0,sizeof(tpk2));
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::UppleExchKey over!\n");

	if( 6768 == port)
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin1::port=[%ld]\n",port);
		return(TranslatePin("SEK1",up_keyindex,tpk2,up_pinkey_sek,termPin,cardNo,6,1,ylPin));
	}
	else if((8385 == port) || (8383 == port))
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin3::port=[%ld]\n",port);
		return(TranslatePin("SEK1",up_keyindex,tpk2,up_pinkey_sek,termPin,cardNo,1,1,ylPin));
	}
	else
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin2::port=[%ld]\n",port);
		return(TranslatePin("SEK1",up_keyindex,tpk2,up_pinkey_sek,termPin,cardNo,1,1,ylPin));
	}

	return 0;
}

/**added by zhuhui for FTS2BOC in 20140728 -begin*/
/*
 *	验证终端MAC
 */

int UppleVerifyFTS2BOCTermTransMac(char *buf,int len)
{
	int ret;
	char    localMac[32+1];
	char    remoteMac[32+1];
	char    macData[8192+1];
	int     lenOfMacData;
	char    tak[32+1];
	int	lenOfTermHeader;
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToFTS2BOCPin::begin\n");
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToFTS2BOCPin::end\n");
	return(0);
}

/**added by zhuhui for FTS2BOC in 20140728 -end*/


int UppleTranslateTermPinToICBCNPin(char *ylPin)
{
	char    zpk[128+1]={0};
	char    tpk[128+1]={0};
	char    tpk2[128+1]={0};
	char    tmk[128+1]={0};
	char    cardNo[40]={0};
	char    termPin[32+1]={0};
	char    up_sek[64+1]={0};
	int     ret;
	char    term_sek[8+1]={0};
	char channel[10]={0};
	char ProcessCode[10]={0};
	char CardNo[20+1]={0};
	int flag = 0;
	char up_channel[10] = {0};

	char    where[2048]={0};
	char    dbRecord[2048]={0};

	long    port = 0;
	UppleGet2L(RPUB, "#port", &port);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToPPPPin::port=[%ld]\n",port);

	memset(termPin,0,sizeof(termPin));
	UppleGet2(RPUB,"PB52",termPin);
	if(( 0 == strlen(termPin)) || (0 == memcmp(termPin,"FFFFFFFFFFFFFFFF",16)))
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCNPin::无密交易，不需要转PIn!!\n");
		return 0;
	}

	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}

	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}

	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	get_card_no_from_TB(cardNo);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCNPin::参与转PIN的卡号[%s]\n",cardNo);

	UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYLPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);

	if ((ret = UppleReadTermSEK(term_sek,sizeof(term_sek))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}

	//UppleExchKey("1","SEK1","SEK2",tmk,tpk,tpk2);
	UppleExchKey("1",term_sek,term_sek,tmk,tpk,tpk2);

	memset( zpk, 0x00, sizeof(zpk) );
	memset( up_sek, 0x00, sizeof(up_sek) );
#if 0
	ReadUPWorkingKeyfromTable( "PINKEY", zpk, up_sek );
#else
	memset(where,0,sizeof(where));
	sprintf(where,"mid = 'channel_6003' and Channel = '6003'");
	memset(dbRecord, 0, sizeof(dbRecord));
	if ((ret = UppleSelectRealDBRecord("TERMINALPARA", "PINKEY,SALET0ACCTNO",
					where, dbRecord, sizeof(dbRecord), NULL)) < 0)
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCNPin::没找到上级表数据!!\n");
		return -1;
	}

	memset(zpk,0,sizeof(zpk));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
					"PINKEY",zpk, sizeof(zpk))) < 0)
	{
		UppleLog2(__FILE__, __LINE__,"in UppleTranslateTermPinToICBCNPin::读不到上级PIN密钥\n");
		return -1;
	}

	memset(up_sek,0,sizeof(up_sek));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
					"SALET0ACCTNO",up_sek, sizeof(up_sek))) < 0)
	{
		UppleLog2(__FILE__, __LINE__,"in UppleTranslateTermPinToICBCNPin::读不到上级密钥索引\n");
		return -1;
	}
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCNPin::up_pin_key[%s],up_sek_index[%s]\n",zpk,up_sek);
#endif

	//strcpy(zpk,UppleReadQZJYLZpk());
	if( !memcmp( zpk, "\x00\x00\x00\x000", 4 ) )  //没取到
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
		return(ret);
	}

	if(6768 == port)
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCNPin::port[%ld]\n",port);
		return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,6,1,ylPin));
	}
	else if((0 == port) || (8385 == port))
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCNPin::port[%ld]\n",port);
		return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,1,ylPin));
	}
	else
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToICBCNPin::port[%ld]\n",port);
		return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,1,ylPin));
	}
}

int UppleTranslateTermPinToChanjetPin(char *ylPin)
{
	char    zpk[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	char    up_sek[64+1];
	char    term_sek[8+1];
	int     ret;
	long    port = 0;
	char    t_tid[20]={0};
	char    t_mid[20]={0};
	char    up_tid[20]={0};
	char    up_mid[20]={0};
	char    up_channel[10]={0};
	char    up_masetrkey[128]={0};
	char    up_masetrkey_p[128]={0};
	char    up_pinkey[128]={0};
	char    up_pinkey_p[128]={0};
	char    up_keyindex[10]={0};
	char    up_pinkey_sek[128]={0};

	memset(termPin,0,sizeof(termPin));
	/*
	   if ((ret = UppleReadTermFld(52,termPin,sizeof(termPin))) < 0)
	   {
	   UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadTermFld 52!\n");
	   return(ret);
	   }
	 */
	UppleGet2L(RPUB, "#port", &port);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::prot=[%ld]\n",port);

	UppleGet2(RPUB, "PB52", termPin);
	UppleLog2(__FILE__, __LINE__, "#TEST:Get TERM PIN = [%s]\n", termPin);
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if((strlen(termPin)==0) || (0 == memcmp(termPin,"FFFFFFFFFFFFFFFF",16))){
		return 0;
	}
#if 0
	if((ret = UppleReadHsmTmk(up_masetrkey,sizeof(up_masetrkey))) < 0)
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToChanjetPin::读MASTER_KEY错!!\n");
		return -1;
	}

	if(strlen(up_masetrkey) <= 0)
	{
		UppleLog2(__FILE__,__LINE__,"##没有读到上级终端主密钥，重新加载终端表!\n");
		UppleReadDatabaseTableOfTermCUPS();
		if((ret = UppleReadHsmTmk(up_masetrkey,sizeof(up_masetrkey))) < 0)
		{
			UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToChanjetPin::读MASTER_KEY错2!!\n");
			return -1;
		}
	}

	if((ret = UppleReadHsmTpk(up_pinkey,sizeof(up_pinkey))) < 0)
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToChanjetPin::读PIN_KEY错!!\n");
		return -1;
	}
#else
	char    up_termid[20]={0};
	char    up_tertid[20]={0};
	memset(up_tertid,0,sizeof(up_tertid));
	if((ret = UppleReadYLFldChanjet(41,up_tertid,sizeof(up_tertid))) <0)
	{
		UppleLog2(__FILE__,__LINE__,"##读银联包的41域错误!\n");
		return -1;
	}
	memset(up_termid,0,sizeof(up_termid));
	if((ret = UppleReadYLFldChanjet(42,up_termid,sizeof(up_termid))) <0)
	{
		UppleLog2(__FILE__,__LINE__,"##读银联包的42域错误!\n");
		return -1;
	}
	UppleLog2(__FILE__,__LINE__,"##UppleTranslateTermPinToChanjetPin::上级商户号[%s],上级终端号[%s]\n",up_termid,up_tertid);
	memset(up_channel,0,sizeof(up_channel));
	strcpy(up_channel,"6004");
	ReadWorkingKeyfromUpchannelKey(up_termid,up_tertid,up_channel,"up_masterkey",up_masetrkey);
	ReadWorkingKeyfromUpchannelKey(up_termid,up_tertid,up_channel,"up_pinkey",up_pinkey);
#endif
	strcpy(up_keyindex,"SEK_CJ");
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::up_masetrkey[%s],up_pinkey[%s],up_keyindex[%s]\n",up_masetrkey,up_pinkey,up_keyindex);

	//strcpy(zpk,UppleReadQZJYLZpk());
	if( !memcmp( up_pinkey, "\x00\x00\x00\x000", 4 ) )  //没取到
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
		return(ret);
	}

	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}

	memset(tmk,0,sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}

	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::card_no[%s]\n",cardNo);

	if ((ret = add_parity_tmk(up_keyindex,"TEK_ICBC",up_masetrkey,up_masetrkey_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak(up_keyindex,up_keyindex,up_masetrkey_p,up_pinkey,up_pinkey_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin:: add parity to tmk!\n");
		return(ret);
	}

	memset(up_pinkey_sek,0,sizeof(up_pinkey_sek));
	UppleExchKey("1",up_keyindex,up_keyindex,up_masetrkey_p,up_pinkey_p,up_pinkey_sek);

	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::UppleExchKey 1!\n");

	memset(tpk2,0,sizeof(tpk2));
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);

	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::UppleExchKey over!\n");

	if( 6768 == port)
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin1::port=[%ld]\n",port);
		return(TranslatePin("SEK1",up_keyindex,tpk2,up_pinkey_sek,termPin,cardNo,6,1,ylPin));
	}
	else if((8385 == port) || (8383 == port))
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin3::port=[%ld]\n",port);
		return(TranslatePin("SEK1",up_keyindex,tpk2,up_pinkey_sek,termPin,cardNo,1,1,ylPin));
	}
	else
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin2::port=[%ld]\n",port);
		return(TranslatePin("SEK1",up_keyindex,tpk2,up_pinkey_sek,termPin,cardNo,1,1,ylPin));
	}

	return 0;
}


int RSAencrypt(char *str,int len,char *path_key,char *PinBlockEnc)
{
	char *p_en;
	RSA *p_rsa;
	FILE *file;
	int flen,rsa_len;
	PKCS12*     p12 = NULL;  
	X509 *     x509 = NULL;  
	STACK_OF(X509) *ca = NULL;  
	EVP_PKEY *     pkey = NULL;  

	if((file=fopen(path_key,"r"))==NULL){
		perror("open key file error");
		return 0;    
	}   
	BIO* b;
	b=BIO_new_file(path_key,"r");
	x509 = PEM_read_bio_X509(b, NULL, NULL, NULL);
	if (x509 == NULL) {
		ERR_print_errors_fp (stderr);
		return 0;
	} 
	pkey=X509_get_pubkey(x509);  
	p_rsa =	 EVP_PKEY_get1_RSA(pkey);
	rsa_len=RSA_size(p_rsa);
	p_en=(unsigned char *)malloc(rsa_len+1);
	memset(p_en,0,rsa_len+1);
	if(RSA_public_encrypt(len,str,p_en,p_rsa,RSA_PKCS1_PADDING)<0){
		printf("加密失败\n");
		return 0;
	}
	memcpy(PinBlockEnc,p_en,rsa_len);
	fclose(file);
	free(p_en);
	return rsa_len;
}
/*
 *	全渠道转PIN函数
 *
 */
int UppleTranslateTermPinToACPPin(char *ACPPIN)
{
	char fld052[32 + 1] = {0};
	char Tmk[32 + 1] = {0};
	char TMK[32 + 1] = {0};
	char TPK[32 + 1] = {0};
	char DcTpk[32 + 1] = {0};
	char PinBlockSource[32 + 1] = {0};
	char PinBlockSourceHex[32 + 1] = {0};
	char PinBlockEnc[256 + 1] = {0};
	char *PinBlockBase = NULL;
	char PASSWD[32 + 1] = {0};
	char tmp[64 + 1] = {0};
	char tpk2[48 + 1] = {0};
	char cardNo[20 + 1] = {0};
	int len = 0;
	char file[128] = {0};
	long port = 0;
	int ret;

	memset(tmp,0,sizeof(tmp));
	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	UppleReadTermFld(52, fld052, sizeof(fld052));
	UppleReadFldFromTermByFldName("SALEMASTERKEY", Tmk,sizeof(Tmk));
	UppleReadFldFromTermByFldName("SALEPINKEY", TPK,sizeof(TPK));
#if 0
	sprintf(file,"%s%s",UppleReadStringTypeRECVar("ACP_CERT"),UppleReadStringTypeRECVar("ACP_EN_PUB"));
#else
	UppleGet2(RPUB,"ACP_P",file);
	UppleGet2(RPUB,"ACP_PIN_PUB",tmp);
	strcat(file,tmp);
#endif
	UppleGet2L(RPUB, "#port", &port);
	if(port == 6768){
		memset(tpk2,0,sizeof(tpk2));
		memset(tmp,0,sizeof(tmp));
		UppleExchKey("1","SEK1","SEK1",Tmk,TPK,tpk2);
		TranslatePin("SEK1","SEK1",tpk2,tpk2,fld052,cardNo,6,1,tmp);
		memset(fld052,0x00,sizeof(fld052));
		strcpy(fld052,tmp);
	}

	if(strlen(Tmk) == 16){
		Upple1DesDecrypt64BitsText("1234567890ABCDEF",Tmk, TMK);
		Upple1DesDecrypt64BitsText(TMK, TPK, DcTpk);
		Upple1DesDecrypt64BitsText(DcTpk, fld052,PASSWD);
		strcpy(PinBlockSource,PASSWD);
	}else if(strlen(Tmk) == 32){

		Upple3DesDecrypt64BitsText("1234567890ABCDEF1234567890ABCDEF",Tmk,TMK );
		Upple3DesDecrypt64BitsText("1234567890ABCDEF1234567890ABCDEF",Tmk+16,TMK+16 );
		Upple3DesDecrypt64BitsText(TMK,TPK,DcTpk );
		Upple3DesDecrypt64BitsText(TMK,TPK+16,DcTpk+16 );
		Upple3DesDecrypt64BitsText(DcTpk,fld052,PASSWD );


		strcpy(PinBlockSource,PASSWD);

	}else{
		return -1;	
	}
	len = aschex_to_bcdhex(PinBlockSource, strlen(PinBlockSource), PinBlockSourceHex);
	len = RSAencrypt(PinBlockSourceHex,len,file,PinBlockEnc);
	PinBlockBase = base64_encrypt(PinBlockEnc,len);
	strcpy(ACPPIN,PinBlockBase);
	return len;

}

int RSAEncryptByModule(char  *type,char *Bnn,char *Bne,unsigned char *in,unsigned char *out ) 
{

	int ret, flen;
	BIGNUM *bnn, *bne, *bnd;
	unsigned char *out_buf;
	long BNE;
	bnn = BN_new();
	bne = BN_new();
	bnd = BN_new();
	if(strncmp(Bne,"RSA_F4",6) == 0)
		BNE = RSA_F4;
	else if(strncmp(Bne,"RSA_3",5) == 0)
		BNE = RSA_3;
	else
		return -1;
	switch(atoi(type)){
		case 10:
			BN_dec2bn(&bnn, Bnn);
			BN_set_word(bne, BNE);
			break;
		case 16:
			BN_hex2bn(&bnn, Bnn);
			BN_set_word(bne, BNE);
			break;
		default:
			UppleLog2(__FILE__,__LINE__,"err type\n");
			return -1;
	}

	RSA *r = RSA_new();
	r->n = bnn;
	r->e = bne;
	r->d = bnd;
	flen = RSA_size(r);
	out_buf = ( unsigned char *)malloc(flen);
	memset(out_buf, 0, flen);
	ret = RSA_public_encrypt(strlen(in),in,out_buf,r,RSA_PKCS1_PADDING);
	if (ret < 0)
	{
		UppleLog2(__FILE__,__LINE__,"Encrypt failed!\n");
		return ret;
	}
	memcpy(out,out_buf,ret);
	free(out_buf);
	RSA_free(r);
	return ret;
}
int UppleAddTranslateOCXPinToPOSPPinGD(char *ylPin)
{
	char    tmpBuf[256+1];
	char    zpk[48+1];
	char    zpk2[48+1];
	char    tmk_boc[48+1];
	char    tpk[48+1];
	char    tpk2[48+1];
	char    tmk[48+1];
	char    cardNo[40];
	char    termPin[32+1];
	int     ret;
	char	tmk_p[48+1];
	char	zpk_p[48+1];
	long 	port = 0;
	char tid[8 + 1] = {0};
	char mid[15 + 1] = {0};

	UppleGet2L(RPUB, "#port", &port);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateOCXPinToPPPPin::port=[%ld]\n",port);

	memset(termPin,0,sizeof(termPin));
	UppleGet2( RPUB, "PB52",  termPin );
	UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
	if(ret==0){
		return 0;
	}
	memset(tmk_p,0,sizeof(tmk_p));
	memset(zpk_p,0,sizeof(zpk_p));
	memset(tmk,0,sizeof(tmk));
	memset(tpk,0,sizeof(tpk));
	memset(tmk_boc,0,sizeof(tmk_boc));
	memset(zpk,0,sizeof(zpk));
	memset(tid,0,sizeof(tid));
	memset(mid,0,sizeof(mid));

	UppleReadDatabaseTableOfTerm1();

	if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)  //SALEPINKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}
	if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)  //SALEMASTERKEY
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}
	if ((ret = UppleReadYLFld(41,tid,sizeof(tid))) < 0){
		UppleLog2(__FILE__,__LINE__,"Read ylFld 41 error\n");
		return ret;
	}
	if ((ret = UppleReadYLFld(42,mid,sizeof(mid))) < 0){
		UppleLog2(__FILE__,__LINE__,"Read ylFld 42 error\n");
		return ret;
	}

	if((ret = ReadWorkingKeyfromUpchannelKey(mid,tid,"1011","up_masterkey",tmk_boc ) ) < 0 ){
		UppleLog2(__FILE__,__LINE__,"Read up_masterkey error\n");
		return ret;
	}
	if((ret = ReadWorkingKeyfromUpchannelKey(mid,tid,"1011","up_pinkey",zpk ) ) < 0 ){
		UppleLog2(__FILE__,__LINE__,"Read up_mackey error\n");
		return ret;
	}
	UppleLog2(__FILE__,__LINE__, "Local SALEPINKEY = [%s]\n",tpk);
	UppleLog2(__FILE__,__LINE__, "Local SALEMASTERKEY = [%s]\n",tmk);
	UppleLog2(__FILE__,__LINE__, "MASTERKEY = [%s]\n",tmk_boc);
	UppleLog2(__FILE__,__LINE__, "PINKEY = [%s]\n", zpk );
	// 读取卡号
	memset(cardNo,0,sizeof(cardNo));
	if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
		return(ret);
	}
	UppleLog2(__FILE__,__LINE__, "cardNo = [%s]\n", cardNo );
	if ((ret = add_parity_tmk("SEK_GD_POSP","TEK_NB",tmk_boc,tmk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
		return(ret);
	}


	if ((ret = add_parity_tak("SEK_GD_POSP","SEK_GD_POSP",tmk_p,zpk,zpk_p)) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: add parity to tmk!\n");
		return(ret);
	}
	memset(zpk2,0,sizeof(zpk2));
	UppleExchKey("1","SEK_GD_POSP","SEK_GD_POSP",tmk_p,zpk_p,zpk2);

	memset(tpk2,0,sizeof(tpk2));
	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);
	if(6768 == port)
		return(TranslatePin("SEK1","SEK_GD_POSP",tpk2,zpk2,termPin,cardNo,6,1,ylPin));
	else
		return(TranslatePin("SEK1","SEK_GD_POSP",tpk2,zpk2,termPin,cardNo,1,1,ylPin));
}

int UppleTranslateTermPinToYFTPin(char *ylPin)
{
	char    zpk[128+1]={0};
	char    tpk[128+1]={0};
	char    tpk2[128+1]={0};
	char    tmk[128+1]={0};
	char    cardNo[40]={0};
	char    termPin[32+1]={0};
	char    up_sek[64+1]={0};
	int     ret;
	char    term_sek[8+1]={0};
	char    channel[10]={0};
	char    ProcessCode[10]={0};
	char    CardNo[20+1]={0};
	int     flag = 0;
	char    up_channel[10] = {0};
	char    where[2048]={0};
	char    dbRecord[2048]={0};
	long    port = 0;
	UppleGet2L(RPUB, "#port", &port);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYFTPin::port=[%ld]\n", port);

	memset(termPin, 0, sizeof(termPin));
	UppleGet2(RPUB, "PB52", termPin);
	if(( 0 == strlen(termPin)) || (0 == memcmp(termPin,"FFFFFFFFFFFFFFFF",16)))
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYFTPin::无密交易，不需要转PIn!!\n");
		return(0);
	}

	memset(tpk, 0, sizeof(tpk));
	if ((ret = UppleReadTermTpk(tpk, sizeof(tpk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTpk!\n");
		return(ret);
	}

	memset(tmk, 0, sizeof(tmk));
	if ((ret = UppleReadTermTmk(tmk, sizeof(tmk))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYLPin:: UppleReadHsmTmk!\n");
		return(ret);
	}

	// 读取卡号
	memset(cardNo, 0, sizeof(cardNo));
	get_card_no_from_TB(cardNo);
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYFTPin::参与转PIN的卡号[%s]\n", cardNo);

	UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToYFTPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n", termPin, tpk, cardNo);

	if ((ret = UppleReadTermSEK(term_sek, sizeof(term_sek))) < 0)
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYFTPin:: UppleReadHsmTmk!\n");
		return(ret);
	}

	UppleExchKey("1", term_sek, term_sek, tmk, tpk, tpk2);

	memset(zpk, 0x00, sizeof(zpk));
	memset(up_sek, 0x00, sizeof(up_sek));
	memset(where, 0, sizeof(where));
	sprintf(where, "mid = 'channel_6005' and Channel = '6005'");
	memset(dbRecord, 0, sizeof(dbRecord));
	if ((ret = UppleSelectRealDBRecord("TERMINALPARA", "PINKEY,SALET0ACCTNO",
					where, dbRecord, sizeof(dbRecord), NULL)) < 0)
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYFTPin::没找到上级表数据!!\n");
		return(-1);
	}

	memset(zpk, 0, sizeof(zpk));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
					"PINKEY",zpk, sizeof(zpk))) < 0)
	{
		UppleLog2(__FILE__, __LINE__,"in UppleTranslateTermPinToYFTPin::读不到上级PIN密钥\n");
		return(-1);
	}

	memset(up_sek, 0, sizeof(up_sek));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
					"SALET0ACCTNO",up_sek, sizeof(up_sek))) < 0)
	{
		UppleLog2(__FILE__, __LINE__,"in UppleTranslateTermPinToYFTPin::读不到上级密钥索引\n");
		return(-1);
	}
	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYFTPin::up_pin_key[%s],up_sek_index[%s]\n", zpk, up_sek);

	if (!memcmp(zpk, "\x00\x00\x00\x000", 4))  //没取到
	{
		UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYFTPin:: UppleReadHsmzpk!\n");
		return(ret);
	}

	if (6768 == port)
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYFTPin::port[%ld]\n", port);
		return(TranslatePin(term_sek, up_sek, tpk2, zpk, termPin, cardNo, 6, 1, ylPin));
	}
	else if((0 == port) || (8385 == port))
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYFTPin::port[%ld]\n", port);
		return(TranslatePin(term_sek, up_sek, tpk2, zpk, termPin, cardNo, 1, 1, ylPin));
	}
	else
	{
		UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToYFTPin::port[%ld]\n", port);
		return(TranslatePin(term_sek, up_sek, tpk2, zpk, termPin, cardNo, 1, 1, ylPin));
	}
}



int UppleGenerateTermMacIns(char *macData,int lenOfMacData,char *localMac)
{
	char		tak[32+1] = {0};
	char        	xor_res[8+1]  = {0};
	char        	aschex[16+1] = {0};
	char        	mid_mak[8+1] = {0};
	char        	temp[16+1] = {0};
	char        	temp2[32+1] = {0};
	char        	high[8+1] = {0};
	char        	tak2[48+1] = {0};
	char        	tmk[48+1] = {0};
	char        	tmk_p[48+1] = {0};
	char        	tak_p[48+1] = {0};
	char		b32[24] = {0};
	int         	ret;
	
	memset(tak,0,sizeof(tak));
	UppleGet2(RPUB,"institution_tak",tak);
	if(strlen(tak) <= 0){
		if((ret = UppleLoadInstitutionsMessage()) < 0){
			UppleLog2(__FILE__,__LINE__,"##加载终端渠道信息出错\n");
			return -1;
		}
		UppleGet2(RPUB,"institution_tak",tak);
		if(strlen(tak) <= 0){
			UppleLog2(__FILE__,__LINE__,"##加载终端渠道tak信息出错\n");
                        return -1;
		}
	}
	UppleLog2(__FILE__,__LINE__,"###tak string=[%s]\n",tak);

	UppleLog2(__FILE__,__LINE__,"##mac数据长度[%d]\n",lenOfMacData);
	UppleMemLog2(__FILE__,__LINE__,"##MAC数据\n",macData,lenOfMacData);

	
	memset(xor_res,0,sizeof(xor_res));
	aschex[16]=0;
	
	Xor(macData,lenOfMacData,xor_res);
	bcdhex_to_aschex(xor_res,8,aschex);
	
	//UppleMemLog2(__FILE__,__LINE__,"###MAC数据做异或后得到结果\n",xor_res,8);
	
	memcpy(high,aschex,8);
	
	memset( temp2, 0x00, sizeof(temp2) );
	memset( temp, 0x00, sizeof(temp) );
	
	//  UppleHsmDes(tak2,high,8,temp);
	UppleHsmDes0(tak,high,8,temp,"SEK1");
	aschex_to_bcdhex(temp, 16, mid_mak);
	//UppleMemLog2(__FILE__,__LINE__,"对异或后的结果加密\n",mid_mak,8);
	memcpy(aschex,mid_mak,8);

	//   UppleMemLog2(__FILE__,__LINE__,"###Before xor 2\n",aschex,16);
	Xor(aschex,16,xor_res);
	//     UppleMemLog2(__FILE__,__LINE__,"###xor result2\n",xor_res,8);
	memset( temp2, 0x00, sizeof(temp2) );
	memset( temp, 0x00, sizeof(temp) );
	bcdhex_to_aschex(xor_res,8,temp2);
	//UppleLog2(__FILE__,__LINE__,"###high before DES=[%s]",temp2);
	UppleHsmDes0(tak,xor_res,8,temp,"SEK1");
	aschex_to_bcdhex(temp, 16, mid_mak);
	//UppleMemLog2(__FILE__,__LINE__,"###mid_mak2 hex",mid_mak,8);
	bcdhex_to_aschex(mid_mak,8,aschex);
	//UppleLog2(__FILE__,__LINE__,"in  UppleGenerateTermMac xor result[%s]\n",aschex);
	memcpy(localMac,aschex,8);
	//UppleMemLog2(__FILE__,__LINE__,"###HSM localMac",localMac,8);
	return 0;
}

int	UppleVerifyTermMacIns(char *buf,int len)
{
	char    localMac[32+1] = {0};
        char    remoteMac[32+1] = {0};
        char    macData[8192+1] = {0};
        int     lenOfMacData = 0;
	int	len_TPDU = 0;
	int 	ret;
	len_TPDU = UppleGetLengthOfTermUnifiedTPDU();
	memset(remoteMac,0x00,sizeof(remoteMac));
	UppleGet2(RPUB,"TB64",remoteMac);
	lenOfMacData = len - len_TPDU - MACLEN;
	memcpy(macData,buf+len_TPDU,lenOfMacData);
	UppleMemLog2(__FILE__,__LINE__,"##终端验MAC数据\n",macData,lenOfMacData);
	UppleGenerateTermMacIns(macData,lenOfMacData,localMac);
	UppleLog2(__FILE__,__LINE__,"##终端报文送的MAC值[%s],前置算出来的终端MAC值[%s]\n",remoteMac,localMac);
	if(0 == memcmp(remoteMac,localMac,8)){
		UppleLog2(__FILE__,__LINE__,"##验终端MAC成功\n");
		return 0;
	}else{
		UppleLog2(__FILE__,__LINE__,"##验终端MAC成功\n");
                return -1;
	}
	return -1;
}


int UppleTranslateTermPinToyenPin(char *ylPin)
{
        char    zpk[128+1]={0};
        char    tpk[128+1]={0};
        char    tpk2[128+1]={0};
        char    tmk[128+1]={0};
        char    cardNo[40]={0};
        char    termPin[32+1]={0};
        char    up_sek[64+1]={0};
        int     ret;
        char    term_sek[8+1]={0};
        char channel[10]={0};
        char ProcessCode[10]={0};
        char CardNo[20+1]={0};
        int flag = 0;
        char up_channel[10] = {0};

        char    where[2048]={0};
        char    dbRecord[2048]={0};

        long    port = 0;
        UppleGet2L(RPUB, "#port", &port);
        UppleLog2(__FILE__,__LINE__,
			"in UppleTranslateTermPinToyenPin::port=[%ld]\n",port);

        memset(termPin,0,sizeof(termPin));
        UppleGet2(RPUB,"PB52",termPin);
        if(( 0 == strlen(termPin)) || (0 == memcmp(termPin,"FFFFFFFFFFFFFFFF",16)))
        {
                UppleLog2(__FILE__,__LINE__,
				"in UppleTranslateTermPinToyenPin::无密交易，不需要转PIn!!\n");
                return 0;
        }

        memset(tpk,0,sizeof(tpk));
        if ((ret = UppleReadTermTpk(tpk,sizeof(tpk))) < 0)
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToyenPin:: UppleReadHsmTpk!\n");
                return(ret);
        }

        memset(tmk,0,sizeof(tmk));
        if ((ret = UppleReadTermTmk(tmk,sizeof(tmk))) < 0)
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToyenPin:: UppleReadHsmTmk!\n");
                return(ret);
        }

        // 读取卡号
        memset(cardNo,0,sizeof(cardNo));
        get_card_no_from_TB(cardNo);
        UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToyenPin::参与转PIN的卡号[%s]\n",cardNo);

        UppleLog2(__FILE__,__LINE__, "in UppleTranslateTermPinToyenPin:: termPin = [%s] tpk = [%s] cardNo = [%s]\n",termPin,tpk,cardNo);

        if ((ret = UppleReadTermSEK(term_sek,sizeof(term_sek))) < 0)
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToyenPin:: UppleReadHsmTmk!\n");
                return(ret);
        }

        //UppleExchKey("1","SEK1","SEK2",tmk,tpk,tpk2);
        UppleExchKey("1",term_sek,term_sek,tmk,tpk,tpk2);

        memset( zpk, 0x00, sizeof(zpk) );
        memset( up_sek, 0x00, sizeof(up_sek) );
        
	memset(where,0,sizeof(where));
        sprintf(where,"mid = 'channel_6006' and Channel = '6006'");
        memset(dbRecord, 0, sizeof(dbRecord));
        if ((ret = UppleSelectRealDBRecord("TERMINALPARA", "PINKEY,SALET0ACCTNO",
                                        where, dbRecord, sizeof(dbRecord), NULL)) < 0)
        {
                UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToyenPin::没找到上级表数据!!\n");
                return -1;
        }

        memset(zpk,0,sizeof(zpk));
        if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
                                        "PINKEY",zpk, sizeof(zpk))) < 0)
        {
                UppleLog2(__FILE__, __LINE__,"in UppleTranslateTermPinToyenPin::读不到上级PIN密钥\n");
                return -1;
        }

        memset(up_sek,0,sizeof(up_sek));
        if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
                                        "SALET0ACCTNO",up_sek, sizeof(up_sek))) < 0)
        {
                UppleLog2(__FILE__, __LINE__,"in UppleTranslateTermPinToyenPin::读不到上级密钥索引\n");
                return -1;
        }
        UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToyenPin::up_pin_key[%s],up_sek_index[%s]\n",zpk,up_sek);

        //strcpy(zpk,UppleReadQZJYLZpk());
        if( !memcmp( zpk, "\x00\x00\x00\x000", 4 ) )  //没取到
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
                return(ret);
        }

        if(6768 == port)
        {
                UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToyenPin::port[%ld]\n",port);
                return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,6,1,ylPin));
        }
        else if((0 == port) || (8385 == port))
        {
                UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToyenPin::port[%ld]\n",port);
                return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,1,ylPin));
        }
        else
        {
                UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToyenPin::port[%ld]\n",port);
                return(TranslatePin( term_sek,up_sek,tpk2,zpk,termPin,cardNo,1,1,ylPin));
        }
}


int UppleTranslateTermPinToyenPinIns(char *ylPin)
{
        char    zpk[128+1]={0};
        char    tpk[128+1]={0};
        char    tpk2[128+1]={0};
        char    tmk[128+1]={0};
        char    cardNo[40]={0};
        char    termPin[32+1]={0};
        char    up_sek[64+1]={0};
        int     ret;
        char    term_sek[8+1]={0};
        char 	channel[10]={0};
        char 	ProcessCode[10]={0};
        char 	CardNo[20+1]={0};
        int 	flag = 0;
        char 	up_channel[10] = {0};
        char    where[2048]={0};
        char    dbRecord[2048]={0};
        long    port = 0;
	char	b32[20]={0};
        UppleGet2L(RPUB, "#port", &port);
        UppleLog2(__FILE__,__LINE__,
                        "in UppleTranslateTermPinToyenPin::port=[%ld]\n",port);

        memset(termPin,0,sizeof(termPin));
        UppleGet2(RPUB,"PB52",termPin);
        if(( 0 == strlen(termPin))){
                UppleLog2(__FILE__,__LINE__,
                                "in UppleTranslateTermPinToyenPin::无密交易，不需要转PIn!!\n");
                return 0;
        }

	UppleGet2(RPUB,"TB32",b32);
	if(strlen(b32) <= 0){
		UppleLog2(__FILE__,__LINE__,
				"##终端上来的机构号为空\n");
		return -1;
	}
	memset(where,0,sizeof(where));
	sprintf(where,"institution_id = '%s'",b32);

	memset(dbRecord,0,sizeof(dbRecord));
	if ((ret = UppleSelectRealDBRecord("institutions", "institution_tmk,institution_tpk",
                                        where, dbRecord, sizeof(dbRecord), NULL)) < 0){
                UppleLog2(__FILE__,__LINE__,
				"in UppleTranslateTermPinToyenPinIns::没找到机构表数据!!\n");
                return -1;
        }
	memset(tpk,0,sizeof(tpk));
	if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
                                        "institution_tpk",tpk, sizeof(tpk))) < 0){
                UppleLog2(__FILE__, __LINE__,
				"in UppleTranslateTermPinToyenPinIns::读不到机构PIN密钥\n");
                return -1;
        }

#if 0	
	memset(tmk,0,sizeof(tmk));
        if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
                                        "institution_tmk",tmk, sizeof(tmk))) < 0){
                UppleLog2(__FILE__, __LINE__,
                                "in UppleTranslateTermPinToyenPinIns::读不到机构TMK密钥\n");
                return -1;
        }

	UppleExchKey("1","SEK1","SEK1",tmk,tpk,tpk2);
#endif
		
        // 读取卡号
        memset(cardNo,0,sizeof(cardNo));
        get_card_no_from_TB(cardNo);
        UppleLog2(__FILE__,__LINE__,
			"in UppleTranslateTermPinToyenPinIns::参与转PIN的卡号[%s]\n",cardNo);

        UppleLog2(__FILE__,__LINE__, 
			"in UppleTranslateTermPinToyenPinIns:: "
			"termPin = [%s] tpk = [%s] cardNo = [%s]\n",
			termPin,tpk,cardNo);

        memset( zpk, 0x00, sizeof(zpk) );
        memset( up_sek, 0x00, sizeof(up_sek) );

        memset(where,0,sizeof(where));
        sprintf(where,"mid = 'channel_6006' and Channel = '6006'");
        memset(dbRecord, 0, sizeof(dbRecord));
        if ((ret = UppleSelectRealDBRecord("TERMINALPARA", "PINKEY,SALET0ACCTNO",
                                        where, dbRecord, sizeof(dbRecord), NULL)) < 0){
                UppleLog2(__FILE__,__LINE__,
				"in UppleTranslateTermPinToyenPinIns::"
				"没找到上级表数据!!\n");
                return -1;
        }

        memset(zpk,0,sizeof(zpk));
        if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
                                        "PINKEY",zpk, sizeof(zpk))) < 0){
                UppleLog2(__FILE__, __LINE__,
				"in UppleTranslateTermPinToyenPin::"
				"读不到上级PIN密钥\n");
                return -1;
        }

        memset(up_sek,0,sizeof(up_sek));
        if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
                                        "SALET0ACCTNO",up_sek, sizeof(up_sek))) < 0){
                UppleLog2(__FILE__, __LINE__,
				"in UppleTranslateTermPinToyenPin::"
				"读不到上级密钥索引\n");
                return -1;
        }
        UppleLog2(__FILE__,__LINE__,
			"in UppleTranslateTermPinToyenPin::"
			"up_pin_key[%s],up_sek_index[%s]\n",
			zpk,up_sek);

        if( !memcmp( zpk, "\x00\x00\x00\x000", 4 ) )  //没取到
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
                return(ret);
        }

	//return(TranslatePin( "SEK1",up_sek,tpk2,zpk,termPin,cardNo,1,1,ylPin));
	return(TranslatePin( "SEK1",up_sek,tpk,zpk,termPin,cardNo,1,1,ylPin));

}


/*终端机构接畅捷渠道*/
int UppleTranslateTermPinToChanjetPinIns(char *ylPin)
{
        char    zpk[48+1] = {0};
        char    tpk[48+1] = {0};
        char    tpk2[48+1] = {0};
        char    tmk[48+1] = {0};
        char    cardNo[40] = {0};
        char    termPin[32+1] = {0};
        char    up_sek[64+1] = {0};
        char    term_sek[8+1] = {0};
        int     ret;
        long    port = 0;
        char    t_tid[20]={0};
        char    t_mid[20]={0};
        char    up_tid[20]={0};
        char    up_mid[20]={0};
        char    up_channel[10]={0};
        char    up_masetrkey[128]={0};
        char    up_masetrkey_p[128]={0};
        char    up_pinkey[128]={0};
        char    up_pinkey_p[128]={0};
        char    up_keyindex[10]={0};
        char    up_pinkey_sek[128]={0};
        char    up_termid[20]={0};
        char    up_tertid[20]={0};
	char	b32[20] = {0};
	char	where[2048] = {0};
	char	dbRecord[2048] = {0};

        memset(termPin,0,sizeof(termPin));
        UppleGet2L(RPUB, "#port", &port);
        UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::prot=[%ld]\n",port);

        UppleGet2(RPUB, "PB52", termPin);
        UppleLog2(__FILE__, __LINE__, "#TEST:Get TERM PIN = [%s]\n", termPin);
        UppleLog2(__FILE__,__LINE__,"################field 52:[%s]###########\n",termPin);
        if((strlen(termPin)==0) || (0 == memcmp(termPin,"FFFFFFFFFFFFFFFF",16))){
                return 0;
        }
        memset(up_tertid,0,sizeof(up_tertid));
        if((ret = UppleReadYLFldChanjet(41,up_tertid,sizeof(up_tertid))) <0)
        {
                UppleLog2(__FILE__,__LINE__,"##读银联包的41域错误!\n");
                return -1;
        }
        memset(up_termid,0,sizeof(up_termid));
        if((ret = UppleReadYLFldChanjet(42,up_termid,sizeof(up_termid))) <0)
        {
                UppleLog2(__FILE__,__LINE__,"##读银联包的42域错误!\n");
                return -1;
        }
        UppleLog2(__FILE__,__LINE__,"##UppleTranslateTermPinToChanjetPin::上级商户号[%s],上级终端号[%s]\n",up_termid,up_tertid);
        memset(up_channel,0,sizeof(up_channel));
        strcpy(up_channel,"6004");
        ReadWorkingKeyfromUpchannelKey(up_termid,up_tertid,up_channel,"up_masterkey",up_masetrkey);
        ReadWorkingKeyfromUpchannelKey(up_termid,up_tertid,up_channel,"up_pinkey",up_pinkey);
        strcpy(up_keyindex,"SEK_CJ");
        UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::up_masetrkey[%s],up_pinkey[%s],up_keyindex[%s]\n",up_masetrkey,up_pinkey,up_keyindex);

        //strcpy(zpk,UppleReadQZJYLZpk());
        if( !memcmp( up_pinkey, "\x00\x00\x00\x000", 4 ) )  //没取到
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleVerifyYlTransMacByTMK:: UppleReadHsmzpk!\n");
                return(ret);
        }

	UppleGet2(RPUB,"TB32",b32);
        if(strlen(b32) <= 0){
                UppleLog2(__FILE__,__LINE__,
                                "##终端上来的机构号为空\n");
                return -1;
        }
        memset(where,0,sizeof(where));
        sprintf(where,"institution_id = '%s'",b32);

        memset(dbRecord,0,sizeof(dbRecord));
        if ((ret = UppleSelectRealDBRecord("institutions", "institution_tmk,institution_tpk",
                                        where, dbRecord, sizeof(dbRecord), NULL)) < 0){
                UppleLog2(__FILE__,__LINE__,
                                "in UppleTranslateTermPinToyenPinIns::没找到机构表数据!!\n");
                return -1;
        }
        memset(tpk,0,sizeof(tpk));
        if ((ret = UppleReadRecFldFromRecStr(dbRecord, strlen(dbRecord),
                                        "institution_tpk",tpk, sizeof(tpk))) < 0){
                UppleLog2(__FILE__, __LINE__,
                                "in UppleTranslateTermPinToyenPinIns::读不到机构PIN密钥\n");
                return -1;
        }


        // 读取卡号
        memset(cardNo,0,sizeof(cardNo));
        if ((ret = UppleReadCardNoFromTermPackage(cardNo)) < 0)
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleDecryptTermTransPin:: UppleReadCardNoFromTermPackage!\n");
                return(ret);
        }
        UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::card_no[%s]\n",cardNo);

        if ((ret = add_parity_tmk(up_keyindex,"TEK_ICBC",up_masetrkey,up_masetrkey_p)) < 0)
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin:: add parity to tmk!\n");
                return(ret);
        }


        if ((ret = add_parity_tak(up_keyindex,up_keyindex,up_masetrkey_p,up_pinkey,up_pinkey_p)) < 0)
        {
                UppleUserErrLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin:: add parity to tmk!\n");
                return(ret);
        }

        memset(up_pinkey_sek,0,sizeof(up_pinkey_sek));
        UppleExchKey("1",up_keyindex,up_keyindex,up_masetrkey_p,up_pinkey_p,up_pinkey_sek);

        UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin::UppleExchKey 1!\n");

	UppleLog2(__FILE__,__LINE__,"in UppleTranslateTermPinToABCPin2::port=[%ld]\n",port);
	return(TranslatePin("SEK1",up_keyindex,tpk,up_pinkey_sek,termPin,cardNo,1,1,ylPin));
}

