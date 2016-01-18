//	Author:		zhangyongding
//	Date:		20081014
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

//#include "uppleLog.h"
#include "uppleEnv.h"
#include "uppleStr.h"

#include "upplePackageAutoFldDef.h"

int UppleIsValidPackageAutoFldType(TUpplePackageAutoFldType type)
{
	switch (type)
	{
		case	conPackageAutoFldType_Bits:
		case	conPackageAutoFldType_String:
		case	conPackageAutoFldType_Long:
		case	conPackageAutoFldType_Int:
		case	conPackageAutoFldType_Date:
		case	conPackageAutoFldType_Double:
		case	conPackageAutoFldType_Numberic:
		case	conPackageAutoFldType_Time:
		case	conPackageAutoFldType_Array:
			return(1);
		default:
			return(0);
	}
}


int UpplePrintPackageAutoFldDefToFile(PUpplePackageAutoFldDef pdef,FILE *fp)
{
	if ((pdef == NULL) || (fp == NULL))
	{
		printf(__FILE__,__LINE__,"in UpplePrintPackageAutoFldDefToFile:: pdef = [%0x] fp = [%0x]\n",pdef,fp);
		return(errCodeParameter);
	}
	fprintf(fp,"%28s %03d %04d ",pdef->name,pdef->index,pdef->maxLen);
	
	switch (pdef->type)
	{
		case	conPackageAutoFldType_Bits:
			fprintf(fp,"������");
			break;
		case	conPackageAutoFldType_String:
			fprintf(fp,"�ַ���");
			break;
		case	conPackageAutoFldType_Long:
			fprintf(fp,"������");
			break;
		case	conPackageAutoFldType_Int:
			fprintf(fp," ���� ");
			break;
		case	conPackageAutoFldType_Date:
			fprintf(fp," ���� ");
			break;
		case	conPackageAutoFldType_Double:
			fprintf(fp,"˫����");
			break;
		case	conPackageAutoFldType_Numberic:
			fprintf(fp,"������");
			break;
		case	conPackageAutoFldType_Time:
			fprintf(fp," ʱ�� ");
			break;
		case	conPackageAutoFldType_Array:
			fprintf(fp," ���� ");
			break;
		default:
			fprintf(fp," ���� ");
			break;
	}
	switch (pdef->method)
	{
		case	conFldValueSetMethod_NotAutoSet:
			fprintf(fp," �ⲿ��ֵ");
			break;
		case	conFldValueSetMethod_NullValue:
			fprintf(fp," ��ֵ    ");
			break;
		case	conFldValueSetMethod_DefaultValue:
			fprintf(fp," ȱʡֵ  ");
			break;
		case	conFldValueSetMethod_Read8583Fld:
			fprintf(fp," 8583��  ");
			break;
		case	conFldValueSetMethod_ReadTermFld:
			fprintf(fp," �ն���  ");
			break;
		case	conFldValueSetMethod_ReadHostFld:
			fprintf(fp," ������  ");
			break;
		case	conFldValueSetMethod_ReadYLFld:
			fprintf(fp," ������  ");
			break;
		case	conFldValueSetMethod_ReadSPFld:
			fprintf(fp," SP��    ");
			break;
		case	conFldValueSetMethod_ReadRECFld:
			fprintf(fp," REC��   ");
			break;			
		case	conFldValueSetMethod_DefaultFun:
			fprintf(fp," ȱʡ����");
			break;
		case	conFldValueSetMethod_UserFun:
			fprintf(fp," �û�����");
			break;
		case	conFldValueSetMethod_ReadOriPyrFld:
			fprintf(fp," ԭ������");
			break;
		case	conFldValueSetMethod_ReadOriPyeFld:
			fprintf(fp," ԭ�շ���");
			break;
		case	conFldValueSetMethod_ReadRPUB:
			fprintf(fp," ������  ");
			break;
		default:
			fprintf(fp,"   ����  ");
			break;
	}	
	fprintf(fp," %s\n",pdef->var);
	return(0);
}

TUpplePackageAutoFldType UppleConvertDefFlagIntoPackageAutoFldType(char *name)
{
	TUpplePackageAutoFldType	type;
	
	if (name == NULL)
	{
		printf("in UppleConvertDefFlagIntoPackageAutoFldType:: name [%0x] is null, default string type!\n",name);
		return(conPackageAutoFldType_String);
	}
	UppleToUpperCase(name);
	if (strstr(name,"STR") != NULL)
		return(conPackageAutoFldType_String);
	else if (strstr(name,"BIT") != NULL)
		return(conPackageAutoFldType_Bits);
	else if (strstr(name,"LONG") != NULL)
		return(conPackageAutoFldType_Long);
	else if (strstr(name,"INT") != NULL)
		return(conPackageAutoFldType_Int);
	else if (strstr(name,"DATE") != NULL)
		return(conPackageAutoFldType_Date);
	else if (strstr(name,"DOUBLE") != NULL)
		return(conPackageAutoFldType_Double);
	else if (strstr(name,"NUM") != NULL)
		return(conPackageAutoFldType_Numberic);
	else if (strstr(name,"BIT") != NULL)
		return(conPackageAutoFldType_Bits);
	else if (strstr(name,"TIME") != NULL)
		return(conPackageAutoFldType_Time);
	else if (strstr(name,"ARRAY") != NULL)
		return(conPackageAutoFldType_Array);
	if (!UppleIsDigitStr(name))
	{
		printf("in UppleConvertDefFlagIntoPackageAutoFldType:: invalid name [%s] default string type!\n",name);
		return(conPackageAutoFldType_String);
	}		
	switch (type = atoi(name))
	{
		case	conPackageAutoFldType_Bits:
		case	conPackageAutoFldType_String:
		case	conPackageAutoFldType_Long:
		case	conPackageAutoFldType_Int:
		case	conPackageAutoFldType_Date:
		case	conPackageAutoFldType_Double:
		case	conPackageAutoFldType_Numberic:
		case	conPackageAutoFldType_Time:
		case	conPackageAutoFldType_Array:
			return(type);
		default:
			printf("in UppleConvertDefFlagIntoPackageAutoFldType:: invalid name [%s] default string type!\n",name);
			return(conPackageAutoFldType_String);
	}
}

TUpplePackageAutoFldValueSetMethod UppleConvertDefFlagIntoPackageAutoFldValueSetMethod(char *name)
{
	TUpplePackageAutoFldValueSetMethod	type;
	
	if (name == NULL)
	{
		printf("in UppleConvertDefFlagIntoPackageAutoFldValueSetMethod:: name [%0x] is null,null value method used!\n",name);
		return(conFldValueSetMethod_NullValue);
	}
	UppleToUpperCase(name);
	if ((strstr(name,"NOTAUTO") != NULL) || (strstr(name,"���Զ�") != NULL) || (strstr(name,"����ֵ") != NULL))
		return(conFldValueSetMethod_NotAutoSet);
	else if (((strstr(name,"DEFAULT") != NULL) && (strstr(name,"DEFAULTFUN") == NULL)) || (strstr(name,"ȱʡ") != NULL))
		return(conFldValueSetMethod_DefaultValue);
	else if ((strstr(name,"NULL") != NULL) || (strstr(name,"��ֵ") != NULL))
		return(conFldValueSetMethod_NullValue);
	else if ((strstr(name,"READ8583FLD") != NULL) || (strstr(name,"ȡ8583��") != NULL))
		return(conFldValueSetMethod_Read8583Fld);
	else if ((strstr(name,"READTERMFLD") != NULL) || (strstr(name,"ȡ�ն���") != NULL))
		return(conFldValueSetMethod_ReadTermFld);
	else if ((strstr(name,"READHOSTFLD") != NULL) || (strstr(name,"ȡ������") != NULL))
		return(conFldValueSetMethod_ReadHostFld);
	else if ((strstr(name,"READYLFLD") != NULL) || (strstr(name,"ȡ������") != NULL))
		return(conFldValueSetMethod_ReadYLFld);
	else if ((strstr(name,"READSPFLD") != NULL) || (strstr(name,"ȡSP��") != NULL))
		return(conFldValueSetMethod_ReadSPFld);
	else if ((strstr(name,"READRECFLD") != NULL) || (strstr(name,"ȡREC��") != NULL))
		return(conFldValueSetMethod_ReadRECFld);
	else if ((strstr(name,"DEFAULTFUN") != NULL) || (strstr(name,"ȱʡ����") != NULL))
		return(conFldValueSetMethod_DefaultFun);
	else if ((strstr(name,"USERFUN") != NULL) || (strstr(name,"�û�����") != NULL))
		return(conFldValueSetMethod_UserFun);
	else if ((strstr(name,"READORIPYRFLD") != NULL) || (strstr(name,"ȡԭ�����") != NULL))
		return(conFldValueSetMethod_ReadOriPyrFld);
	else if ((strstr(name,"READORIPYEFLD") != NULL) || (strstr(name,"ȡԭ�տ��") != NULL))
		return(conFldValueSetMethod_ReadOriPyeFld);
	else if ((strstr(name,"READRPUB") != NULL) || (strstr(name,"ȡ���׹���ֵ") != NULL))
		return(conFldValueSetMethod_ReadRPUB);
	else if ((strstr(name,"READRPBITE") != NULL) || (strstr(name,"ȡ���׹���ֵ") != NULL))
		return(conFldValueSetMethod_ReadRPBite);
	if (!UppleIsDigitStr(name))
	{
		printf("in UppleConvertDefFlagIntoPackageAutoFldValueSetMethod:: invalid name [%s] ,null value method used!\n",name);
		return(conFldValueSetMethod_NullValue);
	}		
	switch (type = atoi(name))
	{
		case	conFldValueSetMethod_NotAutoSet:
		case	conFldValueSetMethod_DefaultValue:
		case	conFldValueSetMethod_NullValue:
		case	conFldValueSetMethod_Read8583Fld:
		case	conFldValueSetMethod_ReadTermFld:
		case	conFldValueSetMethod_ReadHostFld:
		case	conFldValueSetMethod_ReadYLFld:
		case	conFldValueSetMethod_ReadSPFld:
		case	conFldValueSetMethod_ReadRECFld:
		case	conFldValueSetMethod_DefaultFun:
		case	conFldValueSetMethod_UserFun:
		case	conFldValueSetMethod_ReadOriPyrFld:
		case	conFldValueSetMethod_ReadOriPyeFld:
		case	conFldValueSetMethod_ReadRPUB:
		case    conFldValueSetMethod_ReadRPBite:
			return(type);
		default:
			printf("in UppleConvertDefFlagIntoPackageAutoFldValueSetMethod:: invalid name [%s] ,null value method used!\n",name);
			return(conFldValueSetMethod_NullValue);
	}
}
