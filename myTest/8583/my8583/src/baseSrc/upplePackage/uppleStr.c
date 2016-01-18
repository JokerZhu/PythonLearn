//	Author:		zhangyongding
//	Date:		20080922
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>

#include "uppleStr.h"
#include "uppleErrCode.h"

/* ȥ����ߵ�0 */
int UppleFilterLeftZero(char *Str)
{ 
	int	i;
	int	Len;
	int	j;

	for (i = 0,Len = strlen(Str); i < Len; i++)
	{
		if (Str[i] != '0')
			break;
	}
	if (i == 0)
		return(Len);

	for (j = 0; j < Len - i; j++)
	{
		Str[j] = Str[i + j];
	}

	Str[Len - i] = 0;

	return(Len - i);
}
// 2008/10/19������
// ����
int strReverse(char *str)
{   
	char	c;   
	int	iTemp;  
	int	len; 
	
	len = strlen(str);   
	iTemp = len / 2 - 1;   
	while(iTemp >= 0)
	{   
		c = *(str + iTemp);   
		*(str + iTemp) = *(str + len - iTemp - 1);   
		*(str + len - iTemp - 1) = c;   
		iTemp--;   
	}   
}

/* 
����	��10�����ַ���ת��Ϊ2,8,16������
�������
	m	10�����ַ���
	base	������
�������
	result	��Ӧ������
*/
int intTo2_8_16Str(long m,int base,char *result)
{
	int	r;
	char	buf[128+1];
	char	*p;
	
	p = buf;
	while (m > 0)
	{
		r = m % base;
		if (r < 10)
			*p = r + 48;
		else 
			*p = r + 55;
		m = m / base;
		p++;
	}
	*p = '\0';
	strReverse(buf);
	strcpy(result,buf);
	return(0);
}


/* 
����	��16�����ַ���ת��Ϊ10������
�������
	strSource	16�����ַ���
����ֵ
	> 0		10������
	= 0		����
*/
int strHexToInt(char *strSource)
{
	int	i;
	int	result = 0;
	int	nDecNum;

	for (i = 0; i < strlen(strSource); i++)
	{
		switch(strSource[i])
		{
			case 'a':
			case 'A':
				nDecNum = 10;
				break;
			case 'b':
			case 'B':
				nDecNum = 11;
				break;
			case 'c':
			case 'C':
				nDecNum = 12;
				break;
			case 'd':
			case 'D':
				nDecNum = 13;
				break;
			case 'e':
			case 'E':
				nDecNum = 14;
				break;
			case 'f':
			case 'F':
				nDecNum = 15;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				nDecNum = strSource[i] - '0';
				break;
			default:
				return(0);			
		}
		result += nDecNum * pow(16,strlen(strSource)-i -1);
	}
	return(result);
}

// 2008/10/5������
/*
����	���ļ��ж�ȡһ�������У����˵�ע����
�������
	fp		�ļ�����
	sizeOfBuf	��������С
�������
	buf		����������
����ֵ
	>= 0		���������ݵĳ���
	<0		�������
*/
int UppleReadOneDataLineFromTxtFile(FILE *fp,char *buf,int sizeOfBuf)
{
	int	lineLen;

	while (!feof(fp))
	{
		memset(buf,0,sizeOfBuf);
		if (((lineLen = UppleReadOneLineFromTxtStr(fp,buf,sizeOfBuf)) < 0) && (lineLen != errCodeEnviMDL_NullLine))
		{
			printf("in UppleReadOneDataLineFromTxtFile:: UppleReadOneLineFromTxtStr!\n");
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if (UppleIsUnixShellRemarkLine(buf))	// ע����
			continue;
		//printf("in UppleReadOneDataLineFromTxtFile:: [%04d][%s]\n",lineLen,buf);
		return(lineLen);
	}
	return(errCodeFileEnd);
}

// 2008/7/29������
/*
����	ʹ��ͨ�õ���ָ����滻ָ���ķָ���
�������
	oriStr		��
	lenOfOriStr	������
	oriFldSeperator	Դ�ָ���
	desFldSeperator	Ŀ��ָ���
	sizeOfDesStr	Ŀ�괮����Ĵ�С
�������
	desStr		Ŀ�괮
����ֵ
	>= 0		Ŀ�괮�ĳ���
	<0		�������
*/
int UppleConvertOneFldSeperatorInRecStrIntoAnother(char *oriStr,int lenOfOriStr,char oriFldSeperator,char desFldSeperator,char *desStr,int sizeOfDesStr)
{
	int	index;
	
	if ((oriStr == NULL) || (desStr == NULL) || (lenOfOriStr >= sizeOfDesStr) || (lenOfOriStr < 0))
	{
		printf("in UppleConvertOneFldSeperatorInRecStrIntoAnother:: parameter error!\n");
		return(errCodeParameter);
	}
	for (index = 0; index < lenOfOriStr; index++)
	{
		if (oriStr[index] != oriFldSeperator)
			desStr[index] = oriStr[index];
		else
			desStr[index] = desFldSeperator;
	}
	desStr[lenOfOriStr] = 0;
	return(lenOfOriStr);
}

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UppleReadCharTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value)
{
	char	tmpVar[100+1];
        int     ret;
	
	memset(tmpVar,0,sizeof(tmpVar));
	if ((ret = UppleReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,tmpVar,sizeof(tmpVar))) < 0)
	{
		printf("in UppleReadCharTypeRecFldFromRecStr:: UppleReadRecFldFromRecStr!\n");
		return(ret);
	}
	*value = tmpVar[0];
	return(0);
}

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UppleReadDoubleTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,double *value)
{
	char	tmpVar[100+1];
        int     ret;
	
	memset(tmpVar,0,sizeof(tmpVar));
	if ((ret = UppleReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,tmpVar,sizeof(tmpVar))) < 0)
	{
		printf("in UppleReadDoubleTypeRecFldFromRecStr:: UppleReadRecFldFromRecStr!\n");
		return(ret);
	}
	*value = atof(tmpVar);
	return(0);
}

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UppleReadLongTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,long *value)
{
	char	tmpVar[100+1];
        int     ret;

	memset(tmpVar,0,sizeof(tmpVar));
	if ((ret = UppleReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,tmpVar,sizeof(tmpVar))) < 0)
	{
		printf("in UppleReadLongTypeRecFldFromRecStr:: UppleReadRecFldFromRecStr!\n");
		return(ret);
	}
	*value = atol(tmpVar);
        return(0);
}

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UppleReadIntTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,int *value)
{
	char	tmpVar[100+1];
        int     ret;

	memset(tmpVar,0,sizeof(tmpVar));
	if ((ret = UppleReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,tmpVar,sizeof(tmpVar))) < 0)
	{
		printf("in UppleReadIntTypeRecFldFromRecStr:: UppleReadRecFldFromRecStr!\n");
		return(ret);
	}
	*value = atoi(tmpVar);
        return(0);
}

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UppleReadRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf)
{
	char	*ptr;
	int	lenOfValue;
	char	fldTag[100];
	
	if ((recStr == NULL) || (lenOfRecStr < 0) || (fldName == NULL))
	{
		printf("in UppleReadRecFldFromRecStr:: null pointer or lenOfRecStr = [%d]!\n",lenOfRecStr);
		return(errCodeParameter);
	}
	if (lenOfRecStr == 0)
	{
		//printf("in UppleReadRecFldFromRecStr:: fld [%s] not exists!\n",fldName);
		return(errCodeParameter);
	}

	recStr[lenOfRecStr] = 0;
	sprintf(fldTag,"%s=",fldName);
	if (strncmp(recStr,fldTag,strlen(fldTag)) == 0 )
	{
		ptr = recStr;
	}
	else
	{
		sprintf(fldTag,"|%s=",fldName);
		if ((ptr = strstr(recStr,fldTag)) == NULL)
		{
			//printf("in UppleReadRecFldFromRecStr:: fld [%s] not exists!\n",fldName);
			return(errCodeParameter);
		}
	}
	if (value == NULL)	// ����Ҫ������
		return(0);
	ptr = ptr + strlen(fldTag);
	for (lenOfValue = 0; lenOfValue < lenOfRecStr; lenOfValue++)
	{
		if (ptr[lenOfValue] != '|')
			continue;
		else
			break;
	}
	if (sizeOfBuf <= lenOfValue)
	{
		printf("in UppleReadRecFldFromRecStr:: sizeOfBuf [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfBuf,lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}
	memcpy(value,ptr,lenOfValue);
	value[lenOfValue] = 0;
	UppleFilterHeadAndTailBlank(value);
	return(strlen(value));
}

// 2008/7/18,����
// ��һ����¼�������ַ�����
// ����ֵ�Ǽ�¼���ڴ��еĳ���
int UpplePutRecFldIntoRecStr(char *fldName,char *value,int lenOfValue,char *recStr,int sizeOfRecStr)
{
	int	fldTagLen;
	char 	buf[8092+1];
	int 	buflen = 0;
	
	if ((recStr == NULL) || (lenOfValue < 0) || (fldName == NULL))
	{
		printf("in UpplePutRecFldIntoRecStr:: null pointer or lenOfValue = [%d]!\n",lenOfValue);
		return(errCodeParameter);
	}
	
	if ((fldTagLen = strlen(fldName) + 2)+lenOfValue >= sizeOfRecStr)
	{
		printf("in UpplePutRecFldIntoRecStr:: sizeOfRecStr = [%04d] < expected [%04d] for fld [%s]\n",
				sizeOfRecStr,fldTagLen+lenOfValue,fldName);
		return(errCodeSmallBuffer);
	}
	
	//modi by hzh 2007.12.11
	if (lenOfValue >= sizeof(buf))
	{
		printf("in UpplePutRecFldIntoRecStr:: lenOfValue = [%d] too long!\n",lenOfValue);
		return(errCodeParameter);
	}
	
	memset(buf,0,sizeof(buf));
	memcpy(buf,value,lenOfValue);
	
	sprintf(recStr,"%s=%s|",fldName,buf);
	return(fldTagLen+lenOfValue);
}

// 2008/5/22������
/* ����ָ����IP��ַ����
�������
	ipAddr	ԭʼip��ַ
	level	Ҫ������ip��ַ�ļ���
		4������4����
		3������ǰ3����
		2������ǰ2����
		1������ǰ1����
		0��ȫ��������
�������
	outIPAddr	�ӹ����ip��ַ
			�������Ĳ�����XXX���
����ֵ
	0	�ɹ�
	��ֵ	�������
*/
int UppleDealWithIPAddr(char *ipAddr,int level,char *outIPAddr)
{
	int	index = 0,currentLevel = 0;
	int	inlen,outLen;
	
	if (!UppleIsValidIPAddrStr(ipAddr))
	{
		printf("in UppleDealWithIPAddr:: not validIPAddr [%s]\n",ipAddr);
		return(errCodeParameter);
	}
	if ((level < 0) || (level > 4))
	{
		strcpy(outIPAddr,ipAddr);
		return(0);
	}
	memset(outIPAddr,0,15+1);
	outLen = 0;
	for (inlen = strlen(ipAddr); index < inlen; index++)
	{
		if (level == currentLevel)
		{
			for (;currentLevel < 4; currentLevel++)
			{
				memcpy(outIPAddr+outLen,"XXX",3);
				outLen += 3;
				if (currentLevel < 3)
				{
					outIPAddr[outLen] = '.';
					outLen++;
				}
			}
			return(0);
		}				
		outIPAddr[outLen] = ipAddr[index];
		outLen++;
		if (ipAddr[index] == '.')
			currentLevel++;
	}
	return(0);
}

// 2008/5/15����
// ���һ���ַ��Ƿ������У��
int UppleIsOddParityChar(char chCheckChar)
{
	int		nNumber,i;
	unsigned char	ch,ch1;

	nNumber=0;
	ch=(unsigned char)0x01;
	for (i=0;i<8;i++)
	{
		ch1=(unsigned char)(chCheckChar & ch);
		if (ch1!=(unsigned char)0x00) nNumber++;
		ch=(unsigned char)(ch << 1);
	}

	if (nNumber%2 == 0) 
		return(0);
	else
		return(1);
}

// 2008/5/15����
// ��һ���ַ���������У��
int UppleMakeStrIntoOddParityStr(char *pData,int nLenOfData)
{
        int     i,nLength;
        char    aCompData[2048+1];

        memset(aCompData,0,sizeof(aCompData));
        aschex_to_bcdhex(pData,nLenOfData,aCompData);
        for (i = 0,nLength = nLenOfData / 2; i < nLength; i++)
        {
                if (UppleIsOddParityChar(aCompData[i]))
                	continue;
                else
                        aCompData[i] ^= 0x01;
        }
	bcdhex_to_aschex(aCompData,nLength,pData);
        return(0);
}

// 2008/5/15����
// ���һ���ַ����Ƿ������У��
// ������У�鷵��ֵΪ1�����򷵻�ֵΪ0
int UppleCheckStrIsOddStr(char *pData,int nLenOfData)
{
        int     i,nLength;
        char    aCompData[2048+1];

        memset(aCompData,0,sizeof(aCompData));
        aschex_to_bcdhex(pData,nLenOfData,aCompData);
        for (i = 0,nLength = nLenOfData / 2; i < nLength; i++)
        {
                if (UppleIsOddParityChar(aCompData[i]))
                	continue;
                else
                	return(0);
        }
        return(1);
}

/* 2008/3/31����
���ܣ���һ���ַ����õ�ָ���ڼ������ֵ
�������:
	buf		�����ַ���
	spaceMark	�ָ���
	index		�ڼ���ֵ
���������
	fld		��index��ֵ

����ֵ��
	����ֵ�ĳ���
*/
int UppleReadFldFromBuf(char *buf,char spaceMark,int index,char *fld)
{
	int	i,len;
	char	tmpBuf[10240+1];
	char	*p,*ptr;
	
	if (index < 1)
		return(-1);
	
	if ((len = strlen(buf)) > sizeof(tmpBuf) -2)
		return(-2);
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,buf);
	if (tmpBuf[len-1] != spaceMark)
		tmpBuf[len] = spaceMark;
	
	for(i = 0; i < index; i++)
	{
		p = tmpBuf;
		if (ptr = memchr(tmpBuf, spaceMark, strlen(tmpBuf)))
		{
			if (i != index - 1)
				strcpy(tmpBuf,ptr + 1);
		}
		else
			return(-1);
	}
	memcpy(fld,tmpBuf,ptr-p);
	return(ptr-p);
}

// ȥ���ַ��������пո�2008-3-19
int UppleFilterAllBlankChars(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf)
{
	int	i;
	int	j;
	// int	FirstBlank = 1;

	destStr[0] = 0;
	for (i = 0,j = 0; (i < lenOfSourStr) && (j < sizeOfBuf - 1);i++)
	{
		if (sourStr[i] == ' ')
			continue;
		/*
		{
			if (j == 0)
				continue;
			if (FirstBlank)
			{
				FirstBlank = 0;
				destStr[j] = sourStr[i];
				j++;
				continue;
			}
			else
				continue;
		}
		FirstBlank = 1;
		*/
		destStr[j] = sourStr[i];
		j++;
		continue;
	}
	if (i < lenOfSourStr)
	{
		//printf("in UppleFilterRubbisBlank:: sizeOfBuf [%d] smaller than expected [%d]\n",sizeOfBuf,lenOfSourStr);
		return(errCodeSmallBuffer);
	}
	if (destStr[j-1] == ' ')
		j--;
	destStr[j] = 0;
	return(j);	// Length of destStr
}

// �ж��Ƿ�Ϸ��ı�����
int UppleIsValidAnsiCVarName(char *varName)
{
	int	len;
	int	index;
	
	if (varName == NULL)
		return(0);
	for (index = 0,len = strlen(varName); index < len;index++)
	{
		if ((varName[index] >= 'A') && (varName[index] <= 'Z'))
			continue;
		else if ((varName[index] >= 'a') && (varName[index] <= 'z'))
			continue;
		else if ((varName[index] >= '0') && (varName[index] <= '9'))
		{
			if (index == 0)
				return(0);
			else
				continue;
		}
		else if (varName[index] == '_')
		{
			if ((index == 0) || (index == len - 1))
				return(0);
			else
				continue;
		}
		else
			return(0);
	}
	return(1);
}

// ��һ���ַ����ж�ȡ����
/* ���������
	str	�ַ���
   �������
   	par	�Ŷ����Ĳ���
   	maxNum	Ҫ��������������Ŀ
   ����ֵ
   	���ض�ȡ�Ĳ�����Ŀ
*/
int UppleReadParFromStr(char *str,char par[][128+1],int maxNum)
{
	int	num = 0;
	int	yinhaoExists = 0;
	int	len;
	int	index;
	int	nextParExists = 0;
	int	parCharIndex = 0;
	
	len = strlen(str);
	
	for (index = 0; index < len; index++)
	{
		if (num >= maxNum)
			break;
		if (nextParExists && (parCharIndex >= sizeof(par[num])))	// ����ֵ̫��
		{
			//printf("����ֵ̫�� num = [%d] parCharIndex = [%d]\n",num,parCharIndex);
			return(errCodeParameter);
		}
		switch (str[index])
		{
			case	' ':	// �ո�
				if (!nextParExists)
					continue;
				if (yinhaoExists)	// �������ڵĿո��ǲ�����һ����
				{
					par[num][parCharIndex] = str[index];
					parCharIndex++;
					continue;
				}
				// ������һ������
				par[num][parCharIndex] = 0;
				num++;
				// ��ʼ������һ������
				nextParExists = 0;
				parCharIndex = 0;
				continue;
			case	'"':	// ��˫����
				if (!yinhaoExists) // �ǲ�������ʼ����
				{	// ��ʼ���ŵ�ǰһ���ַ������ǿո񣬻�����ʼ�����ǵ�һ���ַ�
					if (index > 0)
					{
						if (str[index-1] != ' ')
						{
							/*
							printf("����ǰһ���ַ����ǿո�! num = [%d]\n",num);
							par[num][parCharIndex] = 0;
							for (index = 0; index <= num; index++)
								printf("[%02d] [%s]\n",index,par[index]);
							*/
							return(errCodeParameter);	// �����ַ���������
						}
					}
					yinhaoExists = 1;
					nextParExists = 1;
					parCharIndex = 0;
					continue;
				}
				// �ǲ����Ľ�������
				par[num][parCharIndex] = 0;
				num++;
				yinhaoExists = 0;
				// ��ʼ������һ������
				nextParExists = 0;
				parCharIndex = 0;
				continue;
			default:	//�ǲ����е�һ���ַ�				
				if (!nextParExists)
					nextParExists = 1;
				par[num][parCharIndex] = str[index];
				parCharIndex++;
				continue;
		}
	}
	if (yinhaoExists)
	{
		//printf("����û����ƥ������ num = [%d]\n",num);
		return(errCodeParameter);
	}
	if (parCharIndex > 0)
	{
		par[num][parCharIndex] = 0;
		num++;
	}
	return(num);
}
						
// �������Сд���ת��Ϊ��д���
int UppleSmallAmtToBigAmt(char *smallAmt, char *bigAmt)
{ 
	int	i,j;
	char	in[128+1];
	char	out[128+1];
	char	number[100];
	char	units[100];
	char	*p;
	char	ch; 
	
	memset(number,0,sizeof(number));
	strcpy(number,"��Ҽ��������½��ƾ�"); 
	memset(units,0,sizeof(units));
	strcpy(units,"�ֽ�Ԫʰ��Ǫ��ʰ��Ǫ��");

	memset(in,0,sizeof(in));
	// ȥ����ͷ��0
	for (i = 0; i < strlen(smallAmt); i++)
	{
		if (smallAmt[i] == '0')
			continue;
		else
		{
			strcpy(in,smallAmt + i);
			break;
		}
	}
	// ȥ��С����
	for (i = 0; i < strlen(in); i++)
	{
		if (in[i] == '.')
		{
			in[i] = in[i+1];
			in[i+1] = in[i+2];	
			in[i+2] = 0;	
			break;	
		}
	}
	memset(out,0,sizeof(out));
	p = out;
	i = (strlen(in) -1 ) * 2;
	for (j = 0; j < strlen(in); j++) 
	{ 
		ch = in[j];
		memcpy(p,&number[atoi(&ch) * 2],2);

		p = p + 2;
		memcpy(p,&units[i],2);
		i = i - 2;
		p = p + 2;
	}
	strcpy(bigAmt,out);
}

// �Ӳ������ж�ȡ����
// ��������
/* ���������
	parGrp,������
	parNum��������Ŀ
	parID,��������
	sizeOfParValue,��������Ĵ�С
   �������
   	parValue,���ܲ����Ļ���
   ����ֵ��
   	<0,������
   	>=0,��������
*/
int UppleReadParFromParGrp(char *parGrp[],int parNum,char *parID,char *parValue,int sizeOfParValue)
{
	char	parIDDef[128+1];
	int	lenOfPar;
	int	index;
	int	times = 0;
	char	par[128+1];
			
	if ((parGrp == NULL) || (parID == NULL) || (strlen(parID) >= sizeof(parIDDef)-1) || (parNum < 0))
	{
		printf("in UppleReadParFromParGrp:: parGrp or parID is NULL! or parID too long\n");
		return(errCodeParameter);
	}

	if (parID[0] != '-')
		sprintf(parIDDef,"-%s",parID);
	else
		sprintf(parIDDef,"%s",parID);

retry:
	if (times > 0)
		UppleToUpperCase(parIDDef);
	for (index = 0; index < parNum; index++)
	{
		strcpy(par,parGrp[index]);
		UppleToUpperCase(par);
		if (strcmp(par,parIDDef) != 0)
			continue;
		if (++index >= parNum)
		{
			printf("in UppleReadParFromParGrp:: parID [%s] not defined in this parGrp\n",parID);
			return(errCodeEnviMDL_VarNotExists);
		}
		if ((lenOfPar = strlen(parGrp[index])) >= sizeOfParValue)
		{
			printf("in UppleReadParFromParGrp:: lenOfPar [%04d] of [%s] too long!\n",lenOfPar,parIDDef);
			return(errCodeSmallBuffer);
		}
		memcpy(parValue,parGrp[index],lenOfPar);
		return(lenOfPar);
	}
	if (times == 0)
	{
		times++;
		goto retry;
	}
	printf("in UppleReadParFromParGrp:: parID [%s] not defined in this parGrp\n",parID);
	return(errCodeEnviMDL_VarNotExists);
}

// ������ж�ȡ����
// ����Ķ����ʽΪ:
//	-parID parValue
/* ���������
	cmdStr,���
	parID,��������
	sizeOfParValue,��������Ĵ�С
   �������
   	parValue,���ܲ����Ļ���
   ����ֵ��
   	<0,������
   	>=0,��������
*/
int UppleReadParFromCmdStr(char *cmdStr,char *parID,char *parValue,int sizeOfParValue)
{
	char	parIDDef[256+1];
	char	*start;
	char	*end;
	char	tmpChar;
	char	tmpBuf[2048+1];
	int	lenOfPar;
	
	if ((cmdStr == NULL) || (parID == NULL) || (strlen(parID) >= sizeof(parIDDef)-1))
	{
		printf("in UppleReadParFromCmdStr:: cmdStr or parID is NULL! or parID too long\n");
		return(errCodeParameter);
	}

	if (parID[0] != '-')
		sprintf(parIDDef,"-%s",parID);
	else
		sprintf(parIDDef,"%s",parID);
	if ((start = strstr(cmdStr,parIDDef)) == NULL)
	{
		printf("in UppleReadParFromCmdStr:: parID [%s] not defined in this cmdStr [%s]\n",parID,cmdStr);
		return(errCodeEnviMDL_VarNotExists);
	}
	start += strlen(parIDDef);
	memset(tmpBuf,0,sizeof(tmpBuf));
	sscanf(start,"%s",tmpBuf);
	if (((lenOfPar = strlen(tmpBuf)) == 0) || ((strlen(tmpBuf) == 1) && (tmpBuf[0] == '-')))
	{
		printf("in UppleReadParFromCmdStr:: cmdStr [%s] error!\n",cmdStr);
		return(errCodeEnviMDL_VarNotExists);
	}
	if (lenOfPar >= sizeOfParValue)
	{
		printf("in UppleReadParFromCmdStr:: lenOfPar [%04d] of [%s] too long!\n",lenOfPar,parIDDef);
		return(errCodeSmallBuffer);
	}
	memcpy(parValue,tmpBuf,lenOfPar);
	return(lenOfPar);
}

// �Ӽ�¼���崮�ж�ȡ��ֵ
// ��¼���Ķ����ʽΪ:
//	fldName=fldValue;fldName=fldValue;...fldName=fldValue;
/* ���������
	recStr,��¼��
	fldName,����
	fldTypeName,������
		�Ϸ�ֵ	char/short/int/long/double/string
	sizeOfFldValue,��ֵ����Ĵ�С
   �������
   	fldValue,������ֵ�Ļ���
   ����ֵ��
   	<0,������
   	>=0,��ֵ����
*/
int UppleReadFldFromRecStr(char *recStr,char *fldName,char *fldTypeName,unsigned char *fldValue,int sizeOfFldValue)
{
	char	fldNameDef[256+1];
	char	*start;
	char	*end;
	int	tmpInt;
	long	tmpLong;
	double	tmpDouble;
	char	tmpChar;
	short	tmpShort;
	unsigned char	*pvalue;
	int	minSizeOfValue;
	
	if ((recStr == NULL) || (fldName == NULL) || (strlen(fldName) >= sizeof(fldNameDef)-1))
	{
		//printf("in UppleReadFldFromRecStr:: recStr or fldName is NULL! or fldName too long\n");
		return(errCodeParameter);
	}
	sprintf(fldNameDef,"%s=",fldName);
	if ((start = strstr(recStr,fldNameDef)) == NULL)
	{
		//printf("in UppleReadFldFromRecStr:: fldName [%s] not defined in this recStr [%s]\n",fldName,recStr);
		return(errCodeEnviMDL_VarNotExists);
	}
	start += strlen(fldNameDef);
	if ((end = strstr(start,";")) == NULL)
	{
		//printf("in UppleReadFldFromRecStr:: fldName [%s] defined in this recStr [%s] without end char ';'\n",fldName,recStr);
		return(errCodeEnviMDL_NotRecFormatDefStr);
	}
	*end = 0;
	if (strcmp(fldTypeName,"int") == 0)
	{
		tmpInt = atoi(start);
		pvalue = (unsigned char *)(&tmpInt);
		minSizeOfValue = sizeof(int);
	}
	else if (strcmp(fldTypeName,"long") == 0)
	{
		tmpLong = atol(start);
		pvalue = (unsigned char *)(&tmpLong);
		minSizeOfValue = sizeof(long);
	}
	else if (strcmp(fldTypeName,"double") == 0)
	{
		tmpDouble = atof(start);
		pvalue = (unsigned char *)(&tmpDouble);
		minSizeOfValue = sizeof(double);
	}
	else if (strcmp(fldTypeName,"char") == 0)
	{
		tmpChar = start[0];
		pvalue = (unsigned char *)(&tmpChar);
		minSizeOfValue = sizeof(char);
	}
	else if (strcmp(fldTypeName,"short") == 0)
	{
		tmpShort = atoi(start);
		pvalue = (unsigned char *)(&tmpShort);
		minSizeOfValue = sizeof(short);
	}
	else
	{
		pvalue = (unsigned char *)start;
		minSizeOfValue = strlen(start);
	}
	if (minSizeOfValue > sizeOfFldValue)
	{
		//printf("in UppleReadFldFromRecStr:: sizeOfFldValue [%d] < expected = [%d]\n",sizeOfFldValue,minSizeOfValue);
		*end = ';';
		return(errCodeSmallBuffer);
	}
	memset(fldValue,0,sizeOfFldValue);
	memcpy(fldValue,pvalue,minSizeOfValue);
	*end = ';';
	return(minSizeOfValue);
}

// ����PK���ĳ���
int UppleGetPKOutOfRacalHsmCmdReturnStr(unsigned char *racalPKStr,int lenOfRacalPKStr,char *pk,int *lenOfPK,int sizeOfBuf)
{
	int	offset;
	int	i;
	int	lenOfNextPart;
	int	bitsOfLenFlag;
	int	lenOfStr;
	int	pkStrOffset = 0;
		
	if ((racalPKStr == NULL) || (lenOfRacalPKStr <= 0))
	{
		//printf("in UppleGetPKOutOfRacalHsmCmdReturnStr:: racalPKStr is null or lenOfRacalPKStr = [%d] error!\n",lenOfRacalPKStr);
		return(errCodeParameter);
	}
	
	// ��ʼ��־λ
	offset = 0;
	if (racalPKStr[offset] != 0x30)
	{
		//printf("in UppleGetPKOutOfRacalHsmCmdReturnStr:: invalid start flag [%c]\n",racalPKStr[offset]);
		return(errCodeParameter);
	}
	offset++;
	
	// �ж���һ���ֵĳ���
	if (racalPKStr[offset] <= 0x80)
	{
		lenOfNextPart = racalPKStr[offset];
		offset++;
	}
	else
	{
		// ����ָʾλ
		bitsOfLenFlag = racalPKStr[offset] - 0x80;
		offset++;
		for (i = 0,lenOfNextPart = 0; i < bitsOfLenFlag; i++,offset++)
			lenOfNextPart += racalPKStr[offset];
	}
	lenOfStr = lenOfNextPart + offset;
	//UppleDebugLog("in UppleGetPKOutOfRacalHsmCmdReturnStr:: lenOfNextPart = [%d] offset = [%d] lenOfStr = [%d]\n",lenOfNextPart,offset,lenOfStr);
	if (lenOfNextPart + offset > lenOfRacalPKStr)
	{
		//printf("in UppleGetPKOutOfRacalHsmCmdReturnStr:: lenOfRacalPKStr [%d] too short!\n",lenOfRacalPKStr);
		return(errCodeParameter);
	}
	
	// �ж��ǲ��Ƿָ�λ
	if (racalPKStr[offset] != 0x02)
	{
		//printf("in UppleGetPKOutOfRacalHsmCmdReturnStr:: invalid sperator flag [%c]\n",racalPKStr[offset]);
		return(errCodeParameter);
	}
	offset++;
	lenOfNextPart--;
	
	// �ж�PK�ĳ���	
	if (racalPKStr[offset] <= 0x80)
	{
		*lenOfPK = racalPKStr[offset];
		offset++;
	}
	else
	{
		// ����ָʾλ
		bitsOfLenFlag = racalPKStr[offset] - 0x80;
		offset++;
		for (i = 0,*lenOfPK = 0; i < bitsOfLenFlag; i++,offset++)	
			*lenOfPK += racalPKStr[offset];		// 2007/11/15 �޸���һ��
	}
	//UppleDebugLog("in UppleGetPKOutOfRacalHsmCmdReturnStr:: lenOfPK = [%d] offset = [%d] lenOfStr = [%d]\n",lenOfPK,offset,lenOfStr);
	
	// ȡ��PK��ֵ
	if (*lenOfPK + offset > lenOfRacalPKStr)
	{
		//printf("in UppleGetPKOutOfRacalHsmCmdReturnStr:: lenOfRacalPKStr [%d] too short!\n",lenOfRacalPKStr);
		return(errCodeParameter);
	}
	// 2007/11/15 ����
	while (*lenOfPK % 128 != 0)
	{
		if (racalPKStr[offset] != 0x00)
		{
			return(errCodeParameter);
		}
		offset++;
		--(*lenOfPK);
	}
	// 2007/11/15 ���ӽ���
	if (*lenOfPK * 2 > sizeOfBuf)
	{
		//printf("in UppleGetPKOutOfRacalHsmCmdReturnStr:: sizeOfBuf [%d] < expected [%d]!\n",sizeOfBuf,*lenOfPK*2);
		return(errCodeParameter);
	}	
	bcdhex_to_aschex((char *)racalPKStr+offset,*lenOfPK,pk);
	*lenOfPK = *lenOfPK * 2;
	
	return(lenOfStr);
}

int UppleGetFullSystemDate(char *strdate)
{
	time_t tmptime;
	struct tm *tmptm;
	int  tmpyear;
	
	time(&tmptime);
	tmptm = (struct tm *)localtime(&tmptime);
	if(tmptm->tm_year > 90)
		tmpyear = 1900 + tmptm->tm_year%1900;
	else
		tmpyear = 2000 + tmptm->tm_year;
		
	sprintf(strdate,"%04d%02d%02d",tmpyear,tmptm->tm_mon+1,tmptm->tm_mday);
	
	return(0);
}


int UppleFormFullDateFromMMDDDate(char *mmDDDate,char *fullDate)
{
	char	tmpBuf[10];
	int	larger;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	UppleGetFullSystemDate(tmpBuf);
	memcpy(fullDate,tmpBuf,4);
	memcpy(fullDate+4,mmDDDate,4);
	if ((larger = strncmp(fullDate,tmpBuf,6)) == 0)	// ��ͬ��ͬ��
		return(0);
	if ((strncmp(fullDate+4,"12",2) == 0) && (strncmp(tmpBuf+4,"01",2) == 0))	// ƴװ�����������һ���£�����ǰ�����ǵ�һ����
	{
		tmpBuf[4] = 0;
		sprintf(fullDate,"%04d",atoi(tmpBuf) - 1);
		memcpy(fullDate+4,mmDDDate,4);
		return(0);
	}
	if ((strncmp(fullDate+4,"01",2) == 0) && (strncmp(tmpBuf+4,"12",2) == 0))	// ƴװ�������ǵ�һ���£�����ǰ���������һ����
	{
		tmpBuf[4] = 0;
		sprintf(fullDate,"%04d",atoi(tmpBuf) + 1);
		memcpy(fullDate+4,mmDDDate,4);
		return(0);
	}
	return(0);
}	

// �жϴ����Ƿ���.
int UppleIsStringHasDotChar(char *str)
{
	int	i,len;
	
	for (i = 0, len = strlen(str); i < len; i++)
	{
		if (str[i] == '.')
			return(1);
	}
	return(0);
}

// ȥ���ַ����еĶ���ո񣬼�������ո�ѹ��Ϊһ���ո�ȥ��ͷ��β�Ŀո�
int UppleFilterRubbisBlank(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf)
{
	int	i;
	int	j;
	int	FirstBlank = 1;

	destStr[0] = 0;
	for (i = 0,j = 0,FirstBlank=1; (i < lenOfSourStr) && (j < sizeOfBuf - 1);i++)
	{
		if (sourStr[i] == ' ')
		{
			if (j == 0)
				continue;
			if (FirstBlank)
			{
				FirstBlank = 0;
				destStr[j] = sourStr[i];
				j++;
				continue;
			}
			else
				continue;
		}
		FirstBlank = 1;
		destStr[j] = sourStr[i];
		j++;
		continue;
	}
	if (i < lenOfSourStr)
	{
		//printf("in UppleFilterRubbisBlank:: sizeOfBuf [%d] smaller than expected [%d]\n",sizeOfBuf,lenOfSourStr);
		return(errCodeSmallBuffer);
	}
	if (destStr[j-1] == ' ')
		j--;
	destStr[j] = 0;
	return(j);	// Length of destStr
}

// 2008/3/17����
// ȥ���ַ����еĶ���ո��tab����������ո�ѹ��Ϊһ���ո�ȥ��ͷ��β�Ŀո��tab
int UppleFilterRubbisBlankAndTab(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf)
{
	int	i;
	int	j;
	int	FirstBlank = 1;

	destStr[0] = 0;
	for (i = 0,j = 0,FirstBlank=1; (i < lenOfSourStr) && (j < sizeOfBuf - 1);i++)
	{
		if ((sourStr[i] == ' ') || (sourStr[i] == '\t'))
		{
			if (j == 0)
				continue;
			if (FirstBlank)
			{
				FirstBlank = 0;
				destStr[j] = sourStr[i];
				j++;
				continue;
			}
			else
				continue;
		}
		FirstBlank = 1;
		destStr[j] = sourStr[i];
		j++;
		continue;
	}
	if (i < lenOfSourStr)
	{
		//printf("in UppleFilterRubbisBlank:: sizeOfBuf [%d] smaller than expected [%d]\n",sizeOfBuf,lenOfSourStr);
		return(errCodeSmallBuffer);
	}
	if ((destStr[j-1] == ' ') || (sourStr[j-1] == '\t'))
		j--;
	destStr[j] = 0;
	return(j);	// Length of destStr
}

int UppleConvertDoubleOf2TailStrIntoLongStr(char *str)
{
	char	tmpBuf[100];
	int	i,len;
	
	if ((len = strlen(str)) >= sizeof(tmpBuf))
		return(len);

	if (len == 0)
		return(len);

	memset(tmpBuf,0,sizeof(tmpBuf));
	for (i = 0; i < len ; i++)
	{
		if ((tmpBuf[i] = str[i]) == '.')
			break;
	}
	if (i == len)
		return(len);
	if (i == len - 1)
	{
		str[len-1] = 0;
		return(len-1);
	}
	memcpy(tmpBuf+i,str+i+1,len-i-1);
	strcpy(str,tmpBuf);
	return(len-1);
}
		
int UppleConvertLongStrIntoDoubleOf2TailStr(char *str)
{
	char	tmpBuf[100];
	char	tmpBuf2[100];
	int	len;
	int	offset = 0;
	int	retOffset = 0;
	int	i;
	
	//sprintf(tmpBuf,"%ld",atol(str));
	strcpy(tmpBuf2,str);
	if ((len = strlen(tmpBuf2)) > 0)
	{
		switch (tmpBuf2[0])
		{
			case	'-':
				offset = 1;
				retOffset = 1;
				str[0] = '-';
				break;
			case	'+':
				offset = 1;
				break;
			default:
				break;
		}
	}
	for (i = offset; i < len; i++)
	{
		if (tmpBuf2[i] == '0')
			continue;
		else
			break;
	}
	memcpy(tmpBuf,tmpBuf2+i,len-i);
	tmpBuf[len-i] = 0;
	switch (len = strlen(tmpBuf))
	{
		case	0:
			strcpy(str+retOffset,"0.00");
			break;
		case	1:
			strcpy(str+retOffset,"0.0");
			retOffset += 3;
			strcpy(str+retOffset,tmpBuf);
			break;
		case	2:
			strcpy(str+retOffset,"0.");
			retOffset += 2;
			strcpy(str+retOffset,tmpBuf);
			break;
		default:
			memcpy(str+retOffset,tmpBuf,len-2);
			retOffset += len - 2;
			memcpy(str+retOffset,".",1);
			retOffset += 1;
			memcpy(str+retOffset,tmpBuf+len-2,2);
			retOffset += 2;
			str[retOffset] = 0;
			break;
	}
	return(strlen(str));
}
	
int UppleIsDigitStr(char *str)
{
	int	i;
	int	len;

	for (i = 0,len = strlen(str); i < len; i++)
		if (!isdigit(str[i]))
			return(0);
	return(1);
}

int UppleFilterRSASignNullChar(char *str,int len)
{
	int	offset;
        int     validLen = 0;

	for (offset = len - 1; offset >= 0; offset--)
	{
                if (str[offset] != 0x00)
                        validLen++;
                else
                {
                        offset++;
                        if (offset == len)
                                return(0);
		        memcpy(str,str+offset,validLen);
        		return(validLen);
                }
	}
	return(len);
}

int UppleGetPKOutOfANSIDERRSAPK(char *derPK,int lenOfDERPK,char *pk,int sizeOfPK)
{
	int	len;
	int	ret;
	int	realPKLen;
	char	tmpBuf[100];
	
	if (lenOfDERPK < 64)
		return(errCodeParameter);

	len = 0;
	if (derPK[len] != 0x30)
		return(errCodeParameter);
	len++;
	bcdhex_to_aschex(derPK+len,20,tmpBuf);
	if (strncmp(tmpBuf,"470240",6) == 0)
	{
		len += 3;
		realPKLen = 64;
	}
	else if (strncmp(tmpBuf,"8188028180",10) == 0)
	{
		len += 5;
		realPKLen = 128;
	}
	else if (strncmp(tmpBuf,"82010902820100",14) == 0)
	{
		len += 7;
		realPKLen = 256;
	}
	else
		return(errCodeParameter);

	if ((len+realPKLen > lenOfDERPK) || (realPKLen * 2 > sizeOfPK))
		return(errCodeParameter);
		
	bcdhex_to_aschex(derPK+len,realPKLen,pk);
	return(realPKLen * 2);
}

int UppleConvertLongIntoStr(long data,int len,char *str)
{
	char	tmpBuf[40];
	
	memset(tmpBuf,0,sizeof(tmpBuf));	
	switch (len)
	{
		case	1:
			sprintf(tmpBuf,"%ld",data%10);
			break;
		case	2:
			sprintf(tmpBuf,"%02ld",data%100);
			break;
		case	3:
			sprintf(tmpBuf,"%03ld",data%1000);
			break;
		case	4:
			sprintf(tmpBuf,"%04ld",data%10000);
			break;
		case	5:
			sprintf(tmpBuf,"%05ld",data%100000);
			break;
		case	6:
			sprintf(tmpBuf,"%06ld",data%1000000);
			break;
		case	7:
			sprintf(tmpBuf,"%07ld",data%10000000);
			break;
		case	8:
			sprintf(tmpBuf,"%08ld",data%100000000);
			break;
		case	9:
			sprintf(tmpBuf,"%09ld",data%1000000000);
			break;
		case	10:
			sprintf(tmpBuf,"%010ld",data);
			break;
		case	11:
			sprintf(tmpBuf,"%011ld",data);
			break;
		case	12:
			sprintf(tmpBuf,"%012ld",data);
			break;
		default:
			return(errCodeParameter);
	}
	memcpy(str,tmpBuf,len);
	return(len);
}

int UppleConvertIntIntoStr(int data,int len,char *str)
{
	char	tmpBuf[40];
	
	memset(tmpBuf,0,sizeof(tmpBuf));	
	switch (len)
	{
		case	1:
			sprintf(tmpBuf,"%d",data%10);
			break;
		case	2:
			sprintf(tmpBuf,"%02d",data%100);
			break;
		case	3:
			sprintf(tmpBuf,"%03d",data%1000);
			break;
		case	4:
			sprintf(tmpBuf,"%04d",data%10000);
			break;
		case	5:
			sprintf(tmpBuf,"%05d",data%100000);
			break;
		default:
			return(errCodeParameter);
	}
	memcpy(str,tmpBuf,len);
	return(len);
}

long UppleConvertLongStringToLong(char *str,int lenOfStr)
{
	int	i;
	long	tmpInt = 0;
	int	isMinus = 1;
	int	maxLen = 12;
	
	for (i = 0; (i < lenOfStr) && (i < maxLen); i++)
	{
		if (!isdigit(str[i]))
		{
			if (i != 0)
				return(tmpInt*isMinus);
			switch (str[i])
			{
				case '-':
					isMinus = -1;
					maxLen = 13;
					break;
				case '+':
					isMinus = 1;
					maxLen = 13;
					break;
				default:
					return(tmpInt*isMinus);
			}
			continue;
		}
		tmpInt = tmpInt * 10 + str[i] - '0';
	}
	return(tmpInt * isMinus);
}

int UppleConvertIntStringToInt(char *str,int lenOfStr)
{
	int	i;
	int	tmpInt = 0;
	int	isMinus = 1;

	for (i = 0; i < lenOfStr; i++)
	{
		if (!isdigit(str[i]))
		{
			if (i != 0)
				return(tmpInt*isMinus);
			switch (str[i])
			{
				case '-':
					isMinus = -1;
					break;
				case '+':
					isMinus = 1;
					break;
				default:
					return(tmpInt*isMinus);
			}
			continue;
		}
		if (tmpInt >= 3276)
			return(tmpInt * isMinus);
		tmpInt = tmpInt * 10 + str[i] - '0';
	}
	return(tmpInt * isMinus);
}
		
int UppleIsStringContainingOnlyZero(char *str,int lenOfStr)
{
	int	i;
	for (i = 0; i < lenOfStr; i++)
	{
		if (str[i] != '0')
			return(0);
	}
	return(1);
}

// Mary modify this function, 2004-3-18, for changing Exponent from 3 to 65537
int UppleFormANSIDERRSAPK(char *pk,int lenOfPK,char *derPK,int sizeOfDERPK)
{
	int	len;
	int	ret;
	int	realPKLen;
	//char	tmpBuf[4096+1];
	//int	pkLenFlagLen;
	
	realPKLen = lenOfPK / 2;
	
	// Mary modify, 2004-3-18
	//if (sizeOfDERPK < 9 + realPKLen)
	if (sizeOfDERPK < 11 + realPKLen)
	{
		//printf("in UppleFormANSIDERRSAPK:: too small sizeOfDERPK [%d]\n",sizeOfDERPK);
		return(errCodeParameter);
	}
	
	len = 0;
	derPK[len] = 0x30;
	len++;
	
	switch (realPKLen)
	{
		case	64:
			derPK[len] = 0x47;
			len++;
			derPK[len] = 0x02;
			len++;
			derPK[len] = 0x40;
			len++;
			break;
		case	128:
			derPK[len] = 0x81;
			len++;
			derPK[len] = 0x88;
			len++;
			derPK[len] = 0x02;
			len++;
			derPK[len] = 0x81;
			len++;
			derPK[len] = 0x80;
			len++;
			break;
		case	256:
			derPK[len] = 0x82;
			len++;
			derPK[len] = 0x01;
			len++;
			derPK[len] = 0x09;
			len++;
			derPK[len] = 0x02;
			len++;
			derPK[len] = 0x82;
			len++;
			derPK[len] = 0x01;
			len++;
			derPK[len] = 0x00;
			len++;
			break;
		default:
			return(errCodeParameter);
	}
	
	aschex_to_bcdhex(pk,lenOfPK,derPK+len);
	len += realPKLen;
	
	/* Mary delete, 2004-3-18
	derPK[len] = 0x02;
	len++;
	derPK[len] = 0x01;
	len++;
	derPK[len] = 0x03;
	len++;
	*/
	// Mary add begin, 2004-3-18
	derPK[len] = 0x02;
	len++;
	derPK[len] = 0x03;
	len++;
	derPK[len] = 0x01;
	len++;
	derPK[len] = 0x00;
	len++;
	derPK[len] = 0x01;
	len++;
	// Mary add end, 2004-3-18
	
	//memset(tmpBuf,0,sizeof(tmpBuf));
	//bcdhex_to_aschex(derPK,len,tmpBuf);
	//printf("in UppleFormANSIDERRSAPK:: pk = [%s]\n",tmpBuf);
	
	return(len);
}
	
int UppleFormANSIX80LenString(int len,char *str,int sizeOfStr)
{
	/*
	int	i;
	int	bal;
	
	if (len < 0)
	{
		//printf("in UppleFormANSIX80LenString:: len [%d] erorr!\n",len);
		return(errCodeParameter);
	}
	
	if ((len < 128 * 128) && (len >= 128))
	{
		if (sizeOfStr < 2)
		{
			//printf("in UppleFormANSIX80LenString:: sizeOfStr [%d] too small!\n",sizeOfStr);
			return(errCodeParameter);
		}
		str[0] = 0x80 + len / 128;
		str[1] = 0x80 + len % 128;
		return(2);
	}
	
	if (len < 128)
	{
		if (sizeOfStr < 1)
		{
			//printf("in UppleFormANSIX80LenString:: sizeOfStr [%d] too small!\n",sizeOfStr);
			return(errCodeParameter);
		}
		str[0] = 0x80 + len % 128;
		return(1);
	}
	*/
        int i;
        int bal;

	if (len < 0)
	{
		//printf("in UppleFormANSIX80LenString:: len [%d] erorr!\n",len);
		return(errCodeParameter);
	}
        for (i = 0,bal = len % 128;(len / 128 > 0) && (i < sizeOfStr);i++,len = len - 128)
                str[i] = 0x80 + len / 128;
        if (i > 0)
                str[i] = bal + 0x80;
        else
                str[i] = bal;

        return(i+1);
	
	//printf("in UppleFormANSIX80LenString:: len [%d] erorr!\n",len);
	//return(errCodeParameter);
}

int UppleConvertIntoLen(char *str,int intLen)
{
	int	len,i;

	if ((intLen <= 0) || (intLen > 5))
		return(errCodeParameter);
	len = 0;
	for (i = 0; i < intLen; i++)
	{
		if (!isdigit(str[i]))	
			return(errCodeParameter);
		len = len * 10 + (str[i] - '0');
	}
	return(len);
}
	
int UppleIsBCDStr(char *str1)
{
	int	i;
	int	len;
	
	for (i = 0,len = strlen(str1); i < len; i++)
	{
		if ((str1[i] >= '0') && (str1[i] <= '9'))
			continue;
		if ((str1[i] >= 'a') && (str1[i] <= 'f'))
			continue;
		if ((str1[i] >= 'A') && (str1[i] <= 'F'))
			continue;
		return(0);
	}
	return(1);
}

int UppleToUpper(char *lowerstr,char *upperstr)
{
	int	i;
	int	Len;
	
	for (i = 0, Len = strlen(lowerstr); i < Len; i++)
		upperstr[i] = toupper(lowerstr[i]);
	
	return(0);
}

int UppleUpperMySelf(char *str,int lenOfStr)
{
        int     i = 0;

	for (i = 0; i < lenOfStr; i++)
		str[i] = toupper(str[i]);
        return(0);
}

int UppleToUpperCase(char *str)
{
	int	i = 0;
	int	len;
	
	for (i = 0,len=strlen(str); i < len; i++)
	{
		if ((str[i] >= 'a') && (str[i] <= 'z'))
			//str[i] = toupper(str[i]);
			str[i] = str[i] + 'A' - 'a';
	}
	return(0);
}

int UppleToLowerCase(char *str)
{
	int	i = 0;
	int	len;
	
	while (i < (len = strlen(str)))
	{
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
			str[i] = tolower(str[i]);
		i++;
	}
	return(0);
}

int UppleFilterRightBlank(char *Str)
{
	int	i;

	for (i = strlen(Str); i > 0; i--)
	{
		if ((Str[i-1] != ' ') && (Str[i-1] != 0x09))
			return(i);
		else
			Str[i-1] = 0;
	}
	return(0);
}

int UppleFilterLeftBlank(char *Str)
{
	int	i;
	int	Len;
	int	j;

	for (i = 0,Len = strlen(Str); i < Len; i++)
	{
		if ((Str[i] != ' ') && (Str[i] != 0x09))
			break;
	}
	if (i == 0)
		return(Len);

	for (j = 0; j < Len - i; j++)
	{
		Str[j] = Str[i + j];
	}

	Str[Len - i] = 0;

	return(Len - i);
}

int UppleVerifyDateStr(char *DateStr)
{
	int	Year;
	int	Month;
	int	Day;

	char	TmpBuf[5];

	if (DateStr == NULL)
		return(0);

	if (strlen(DateStr) != 8)
		return(0);

	memset(TmpBuf,0,sizeof(TmpBuf));
	memcpy(TmpBuf,DateStr,4);
	Year = atoi(TmpBuf);
	memset(TmpBuf,0,sizeof(TmpBuf));
	memcpy(TmpBuf,DateStr+4,2);
	Month = atoi(TmpBuf);
	memset(TmpBuf,0,sizeof(TmpBuf));
	memcpy(TmpBuf,DateStr+6,2);
	Day = atoi(TmpBuf);
	
	switch (Month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if ((Day >= 1) && (Day <= 31))
				return(1);
			else
				return(0);
		case 4:
		case 6:
		case 9:
		case 11:
			if ((Day >= 1) && (Day <= 30))
				return(1);
			else
				return(0);
		case 2:
			if ((Year % 4 == 0) && (Year % 100 != 0))
			{
				if ((Day >= 1) && (Day <= 29))
					return(1);
				else
					return(0);
			}
			else
			{
				if ((Day >= 1) && (Day <= 28))
					return(1);
				else
					return(0);
			}
		default:
			return(0);
	}
}

int UppleCopyFilterHeadAndTailBlank(char *str1,char *str2)
{
	strcpy(str2,str1);
	UppleFilterLeftBlank(str2);
	return(UppleFilterRightBlank(str2));
}

int UppleFilterHeadAndTailBlank(char *str)
{
	UppleFilterLeftBlank(str);
	return(UppleFilterRightBlank(str));
}

int UppleIsValidIPAddrStr(char *ipAddr)
{
	unsigned int 	i,j,k;
	char	tmpBuf[4];

	if (strlen(ipAddr) > 15)
		return(0);

	for (i = -1,k = 0; k < 4; k++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		for (++i,j = 0; (j < 3) && (i < strlen(ipAddr)); i++,j++)
		{
			if (ipAddr[i] == '.') // ����һ��.,һ�ڽ���
				break;
			else
			{
				if (!isdigit(ipAddr[i])) // ����������
					return(0);
				else
					tmpBuf[j] = ipAddr[i];
			}
		}
		if (j == 0) // ����2��.��������
			return(0);
		if ((atoi(tmpBuf) > 255) || (atoi(tmpBuf) < 0))
			return(0);	// ����255��С��0
		if (k == 3) // ���һ�ڽ���
			break;
		if (ipAddr[i] != '.') // һ�ڵĽ���������.
			return(0);
	}
	if (strlen(ipAddr) != i)
		return(0);	// �Ľڽ����󣬻����ַ�
	else
		return(1);
}

char hextoasc(int xxc)
{
    xxc&=0x0f;
    if (xxc<0x0a) xxc+='0';
    else xxc+=0x37;
    return (char)xxc;
}

char hexlowtoasc(int xxc)
{
    xxc&=0x0f;
    if (xxc<0x0a) xxc+='0';
    else xxc+=0x37;
    return (char)xxc;
}

char hexhightoasc(int xxc)
{
    xxc&=0xf0;
    xxc = xxc>>4;
    if (xxc<0x0a) xxc+='0';
    else xxc+=0x37;
    return (char)xxc;
}

char asctohex(char ch1,char ch2)
{
    char ch;
    if (ch1>='A') ch=(char )((ch1-0x37)<<4);
    else ch=(char)((ch1-'0')<<4);
    if (ch2>='A') ch|=ch2-0x37;
    else ch|=ch2-'0';
    return ch;
}
 
int aschex_to_bcdhex(char aschex0[],int len,char bcdhex[])
{
    int i,j;
    char *aschex=(char *)malloc(len+2);
	
	strncpy(aschex, aschex0,len );
  

	if (len % 2 == 0)
		j = len / 2;
	else
		//j = len / 2 + 1;
		{
		j = (len+1)/2;
		aschex[2*(j-1)+1]='0';
		}

    for (i = 0; i < j; i++)
        bcdhex[i] = asctohex(aschex[2*i],aschex[2*i+1]);
	
    free(aschex);

    return(j);
}

int bcdhex_to_aschex(char bcdhex[],int len,char aschex[])
{
    int i;

    for (i=0;i<len;i++)
    {
        aschex[2*i]   = hexhightoasc(bcdhex[i]);
        aschex[2*i+1] = hexlowtoasc(bcdhex[i]);
    }

    return(len*2);
}

int byte2int_to_bcd(int aa,char xx[])
{
	if (aa >= 256 * 256)
		return(errCodeParameter);

	xx[0] = aa/256;
	xx[1] = aa%256;

	return(0);
}

int IntToBcd(int aa,unsigned char xx[])
{
	if (aa > 256 * 256)
		return(errCodeParameter);
	xx[0] = (unsigned char)(aa/256);
	xx[1] = (unsigned char)(aa%256);
	return(0);
}

int BcdToInt(char xx[])
{
	return(xx[0]*256+xx[1]);
}

int UppleReadDirFromStr(char *str,int dirLevel,char *dir)
{
	int	strIndex=0,dirIndex=0,tmpIndex=0;
	char	tmpBuf[512];
	char	envName[100];
	int	curDirLevel = 0;
	int	i;
	char	*p;
	
	/* 2008/3/20��ɾ���˶�
	if (dirLevel <= 0)
	{
		dir[0] = 0;
		return(0);
	}
	*/
	memset(tmpBuf,0,sizeof(tmpBuf));
	while (str[strIndex])
	{
		if (str[strIndex] != '$')
		{
			tmpBuf[tmpIndex] = str[strIndex];
			++tmpIndex;
			++strIndex;
			continue;
		}
		memset(envName,0,sizeof(envName));
		i = 0;
		++strIndex;
		while ((str[strIndex] != '/') && (str[strIndex]))
		{
			envName[i] = str[strIndex];
			i++;
			strIndex++;
		}
		envName[i] = 0;
		if (getenv(envName) == NULL)
			return(errCodeParameter);
		sprintf(tmpBuf + tmpIndex,"%s",getenv(envName));
		tmpIndex = strlen(tmpBuf);
		if (!str[strIndex])
			break;
		tmpBuf[tmpIndex] = str[strIndex];
		tmpIndex++;
		strIndex++;
	}
	tmpBuf[tmpIndex] = 0;
	// 2008/3/20����
	if (dirLevel <= 0)
	{
		memcpy(dir,tmpBuf,tmpIndex);
		return(tmpIndex);
	}
	// 2008/3/20���ӽ���
	
	dirIndex = 0;
	while (tmpBuf[dirIndex])
	{
		if (tmpBuf[dirIndex] != '/')
			continue;
		curDirLevel++;
		if (curDirLevel == dirLevel)
		{
			memcpy(dir,tmpBuf,dirIndex);
			dir[dirIndex] = 0;
			return(dirIndex);
		}
	}
	curDirLevel++;
	if (curDirLevel == dirLevel)
	{
		memcpy(dir,tmpBuf,dirIndex);
		dir[dirIndex] = 0;
		return(dirIndex);
	}
	
	return(errCodeParameter);
}

int UppleIsUnixShellRemarkLine(char *line)
{
	int	i=0;
	
	if (line == NULL)
		return(0);
	
	while (line[i])
	{
		switch (line[i])
		{
			case	'#' :
				return(1);
			case	' ':
				++i;
				continue;
			default:
				return(0);
		}
	}
	return(1);
}

int UppleReadUnixEnvVarFromStr(char *envVarStr,int lenOfEnvVarStr,char *envVar,int *envVarNameLen)
{
	int	i = 0, j = 0;
	char	varName[256];
	
	if (envVarStr[0] != '$')
		return(0);
	i = 1;
	while (i < lenOfEnvVarStr)
	{
		if (isdigit(envVarStr[i]) || isalpha(envVarStr[i]) || (envVarStr[i] == '_'))
		{
			if (j >= sizeof(varName) - 1)
			{
				//printf("in UppleReadUnixEnvVarFromStr:: sizeOfEnvVarName [%d] too small!\n",sizeof(varName)-1);
				return(errCodeParameter);
			}
			varName[j] = envVarStr[i];
			i++;
			j++;
		}
		else
			break;
	}
	varName[j] = 0;
	sprintf(envVar,"%s",getenv(varName));
	*envVarNameLen = i;
	return(strlen(envVar));
}

int UppleConvertUnixShellStr(char *oldStr,int lenOfOldStr,char *newStr,int sizeOfNewStr)
{
	int	i = 0,j = 0;
	int	ret;
	int	varNameLen;
	
	while (i < lenOfOldStr)
	{
		if (j >= sizeOfNewStr)
		{
			//printf("in UppleConvertUnixShellStr:: too small sizeOfNewStr [%d]\n",sizeOfNewStr);
			return(errCodeParameter);
		}
		
		if (oldStr[i] == '$')
		{
			if ((ret = UppleReadUnixEnvVarFromStr(oldStr+i,lenOfOldStr-i,newStr+j,&varNameLen)) < 0)
			{
				//printf("in UppleConvertUnixShellStr:: UppleReadUnixEnvVarFromStr!\n");
				return(ret);
			}
			i += varNameLen;
			j += ret;
		}
		else
		{
			newStr[j] = oldStr[i];
			++i;
			++j;
		}
	}
	return(j);
}

int UppleDeleteSuffixVersionFromStr(char *oldStr,char *newStr)
{
	int	len;
	
	strcpy(newStr,oldStr);
	len = strlen(oldStr);
	
	while (len > 0)
	{
		if (isdigit(newStr[len-1]) || (newStr[len-1] == '.'))
			len--;
		else
			break;
	}
	newStr[len] = 0;
	return(len);
}	

int UppleStrContainCharStar(char *str)
{
	int	i=0;
	
	while (str[i])
	{
		if (str[i] == '*')
			return(1);
		i++;
	}
	return(0);
}

int UppleIsValidFullDateStr(char *date)
{
	int	tmp,month,day,year;
	char	str[10];
	
	if (strlen(date) != 8)
		return(0);
	
	for(tmp = 0; tmp < 8; tmp++)
		if (!isdigit(date[tmp]))
			return(0);
	memset(str,0,sizeof(str));
	memcpy(str,date+4,2);
	if (((month = atoi(str)) < 1) || (month > 12))
		return(0);
	memset(str,0,sizeof(str));
	memcpy(str,date+6,2);
	if (((day = atoi(str)) < 0) || (day > 31))
		return(0);
	switch (month)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return(1);
		case 4:
		case 6:
		case 9:
		case 11:
			if (day <= 30)
				return(1);
			else
				return(0);
		case 2:
			memset(str,0,sizeof(str));
			memcpy(str,date,4);
			year = atoi(str);
			if ((year % 4 == 0) && (year % 100 != 0))
			{
				if (day <= 29)
					return(1);
				else
					return(0);
			}
			else
			{
				if (day <= 28)
					return(1);
				else
					return(0);
			}
		default:
			return(0);
	}
}

// ���ı��ļ���ȡһ��
int UppleReadOneLineFromTxtStr(FILE *fp,char *buf,int sizeOfBuf)
{
	char *p;
	char c;
	int  num;
	
	if ((fp == NULL) || (buf == NULL) || (sizeOfBuf <= 0))
		return(errCodeParameter);
		
	p = buf;
	num = 0;
	while (!feof(fp))
	{
		if (num >= sizeOfBuf - 1)
		{
			*p = 0;
			//printf("in UppleReadOneFileLine:: line [%s] too large!\n",buf);
			return(-1);
		}	
		fscanf(fp,"%c",&c);
		if (c == '\n')
		{
			*p = 0;
			return(num);
		}
		*p = c;
		p++;
		num++;
	}
	
	*p = 0;
	num--;
	if (num >= 0)
		buf[num] = 0;
	if (num == 0)
		return(errCodeEnviMDL_NullLine);
	else
		return(num);
}

/*
*	��ȡ�̻����ļ��
*	src:Դ�̻���
*	lenght:������̻�����Ƴ���	
*	dest:��Ž����BUF
*	destlen:dest�Ŀռ�
*/

int GetMidnameShorthand(char *src,int lenght,char *dest,int destlen)
{
	int i = 0;
	unsigned char ch;
	if(!src || lenght <= 0 || (strlen(src) < lenght) || (lenght > destlen))
		return -1;
	for(i =0;i < lenght;i++){
		ch = src[i];
		if( ch < 127){
			dest[i] = src[i];
		}else{
			strncpy(dest + i,src + i,2);
			i++;
		}
	}
	return 0;
}
int _count_string(char *data, char *key)
{
	int count = 0;
	int klen = strlen(key);
	char *pos_start = data, *pos_end;
	while (NULL != (pos_end = strstr(pos_start, key))) {
		pos_start = pos_end + klen;
		count++;
	}
	return count;
}

char *malloc_replace(char *data, char *rep, char *to, int free_data)
{
    int rep_len = strlen(rep);
    int to_len  = strlen(to);
    int counts  = _count_string(data, rep);
    int m = strlen(data) + counts * (to_len - rep_len);
    char *new_buf = (char *) malloc(m + 1);
    if (NULL == new_buf) {
        free(data);
        return NULL;
    }
    memset(new_buf, 0, m + 1);
    char *pos_start = data, *pos_end, *pbuf = new_buf;
    int copy_len;
    while (NULL != (pos_end = strstr(pos_start, rep))) {
        copy_len = pos_end - pos_start;
        strncpy(pbuf, pos_start, copy_len);
        pbuf += copy_len;
        strcpy(pbuf, to);
        pbuf += to_len;
        pos_start  = pos_end + rep_len;
    }
    strcpy(pbuf, pos_start);
	if (free_data)
    	free(data);
    return new_buf;
}

void StrReplace(char *data, char *rep, char *to)
{
	char *new_buf = malloc_replace(data, rep, to, 0);
	if (NULL != new_buf) {
		strcpy(data, new_buf);
		free(new_buf);
	}
}
