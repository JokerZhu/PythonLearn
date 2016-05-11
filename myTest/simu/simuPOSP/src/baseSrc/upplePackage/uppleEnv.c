#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "uppleEnv.h"
#include "uppleStr.h"
//#include "uppleLog.h"
#include "uppleErrCode.h"

int			guppleEnvMDLIsUsed = 0;
PUppleEnviVariable	puppleEnviVarTBL[MAXUPPLEENVIVARIABLES];
int			puppleEnviVarNum = 0;

// 2008/5/22,在文件最后增加一行	
int UppleAddStrToFile(char *fileName,char *str)
{
	FILE			*fp1 = NULL;
	int			ret;
	
	if ((fileName == NULL) || (str == NULL))
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"a")) == NULL)
	{
		printf("in UppleAddStrToFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(fp1,"%s\n",str);
	fclose(fp1);
	return(0);
}

int UppleInitEnvi(char *fileName)
{
	int 	i;
	char	tmpBuf[512];
	FILE	*fp;
	int	j;
	
	if (guppleEnvMDLIsUsed)
	{
		printf("in UppleInitEnvi:: EnvMDL is used by another!\n");
		return(errCodeEnviMDL_OccupiedByOther);
	}
	puppleEnviVarNum = 0;	
	for (i=0;i<MAXUPPLEENVIVARIABLES;i++)
	{
		puppleEnviVarTBL[i] = NULL;
	}
	
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		printf("in UppleInitEnvi:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	guppleEnvMDLIsUsed = 1;
	for (;;)
	{
		memset(tmpBuf,0,512);
		if (feof(fp))
			break;
		if (UppleReadOneFileLine(fp,tmpBuf) < 0)
			break;
		puppleEnviVarTBL[puppleEnviVarNum] = ReadUppleEnviVarFromStr(tmpBuf);
		if (puppleEnviVarTBL[puppleEnviVarNum] == NULL)
			continue;
		
		puppleEnviVarNum++;
	}
		
	fclose(fp);
	
	//UpplePrintEnvi();
	
	return(0);
}

int UpplePrintEnvi()
{
	int	i,j;
	
	for (i = 0; i < puppleEnviVarNum; i++)
	{
		if (puppleEnviVarTBL[i] == NULL)
			continue;
		for (j = 0; j < 10; j++)
		{
			if (puppleEnviVarTBL[i]->pVariableValue[j] == NULL)
				continue;
			printf("i = [%04d] j = [%04d] var = [%s]\n",i,j,puppleEnviVarTBL[i]->pVariableValue[j]);
		}
	}
	printf("in UppleInitEnv:: puppleEnviVarNum = [%d]\n",puppleEnviVarNum);
	return(0);
}

int UppleClearEnvi()
{
	int i,j;
	
	if (!guppleEnvMDLIsUsed)
		return(0);
	for (i=0;i<puppleEnviVarNum;i++)
		UppleFreeEnviVar(puppleEnviVarTBL[i]);
	guppleEnvMDLIsUsed = 0;
	return(0);
}

int UppleFreeEnviVar(PUppleEnviVariable pvar)
{
	int	i;
	
	if (pvar == NULL)
		return(0);
	for (i = 0; i < 10; i++)
		if (pvar->pVariableValue[i])
		{
			free(pvar->pVariableValue[i]);
			pvar->pVariableValue[i] = NULL;
		}
	free(pvar);
	pvar = NULL;
	return(0);
}

int UppleGetEnviVarNum()
{
	return(puppleEnviVarNum);
}

char *UppleGetEnviVarNameByIndex(int varIndex)
{
	if ((varIndex >= puppleEnviVarNum) || (varIndex >= MAXUPPLEENVIVARIABLES) || (varIndex < 0))
		return(NULL);
	
	if (puppleEnviVarTBL[varIndex] == NULL)
		return(NULL);
		
	//return(puppleEnviVarTBL[varIndex]->pVariableName);
	return(UppleGetEnviVarOfTheIndexByIndex(varIndex,0));
}
	
// 读取指定索引变量的，第N个值，N=varValueIndex
char *UppleGetEnviVarOfTheIndexByIndex(int varIndex,int varValueIndex)
{
	if ((varIndex >= puppleEnviVarNum) || (varIndex >= MAXUPPLEENVIVARIABLES) || (varValueIndex >= 10) || (varIndex < 0) || (varValueIndex < 0))
		return(NULL);
		
	if (puppleEnviVarTBL[varIndex] == NULL)
		return(NULL);
		
	return(puppleEnviVarTBL[varIndex]->pVariableValue[varValueIndex]);
}
	
char *UppleGetEnviVarByIndex(int varIndex)
{
	return(UppleGetEnviVarOfTheIndexByIndex(varIndex,1));
}
	
// 读取指定名称变量的，第N个值，N=varIndex
char *UppleGetEnviVarOfTheIndexByName(char *varName,int varValueIndex)
{
	int i;
	
	if ((varName == NULL) || (varValueIndex >= 10) || (varValueIndex < 0))
		return(NULL);
			
	//UppleToUpperCase(varName);
		
	for (i=0;i<puppleEnviVarNum;i++)
	{
		if (puppleEnviVarTBL[i] == NULL)
			continue;
		
		//if (puppleEnviVarTBL[i]->pVariableName == NULL)
		if (puppleEnviVarTBL[i]->pVariableValue[0] == NULL)
			continue;
	
		//if (strcmp(puppleEnviVarTBL[i]->pVariableName,varName) != 0)
		if (strcmp(puppleEnviVarTBL[i]->pVariableValue[0],varName) != 0)
			continue;

		return(puppleEnviVarTBL[i]->pVariableValue[varValueIndex]);
	}
	
	return(NULL);
}
		
char *UppleGetEnviVarByName(char *varName)
{
	return(UppleGetEnviVarOfTheIndexByName(varName,1));
}
		
// 2008/6/20增加
/*
功能
	读取指定名称的变量
输入参数
	varName		变量名称
	sizeOfBuf	接收变量的缓冲大小
输出参数
	buf		读取的变量
返回值
	>=0		变量值长度
	<0		错误码
*/
int UppleReadEnviStrVarByName(char *varName,char *buf,int sizeOfBuf)
{
	char *ptr;
	int	len;
	
	if ((ptr = UppleGetEnviVarByName(varName)) == NULL)
	{
		printf("in UppleReadEnviStrVarByName:: [%s]\n",varName);
		return(errCodeEnviMDL_VarNotExists);
	}
	if ((len = strlen(ptr)) >= sizeOfBuf)
	{
		printf("in UppleReadEnviStrVarByName:: [%s] value [%s] len = [%d] too long!\n",varName,ptr,len);
		return(errCodeSmallBuffer);
	}
	memcpy(buf,ptr,len);
	return(len);
}

// 2008/6/20增加
/*
功能
	读取指定名称的变量
输入参数
	varName		变量名称
输出参数
	value		读取的变量
返回值
	>=0		变量值长度
	<0		错误码
*/
int UppleReadEnviIntVarByName(char *varName,int *value)
{
	char *ptr;
	
	if ((ptr = UppleGetEnviVarByName(varName)) == NULL)
	{
		printf("in UppleReadEnviStrVarByName:: [%s]\n",varName);
		return(errCodeEnviMDL_VarNotExists);
	}
	*value = atoi(ptr);
	return(sizeof(*value));
}

// 2008/6/20增加
/*
功能
	读取指定名称的变量
输入参数
	varName		变量名称
输出参数
	value		读取的变量
返回值
	>=0		变量值长度
	<0		错误码
*/
int UppleReadEnviLongVarByName(char *varName,long *value)
{
	char *ptr;
	
	if ((ptr = UppleGetEnviVarByName(varName)) == NULL)
	{
		printf("in UppleReadEnviStrVarByName:: [%s]\n",varName);
		return(errCodeEnviMDL_VarNotExists);
	}
	*value = atol(ptr);
	return(sizeof(*value));
}

// 读取指定名称变量对应的索引号
int UppleGetVarIndexOfTheVarName(char *varName)
{
	int i;
	
	if (varName == NULL)
		return(errCodeParameter);
			
	for (i=0;i<puppleEnviVarNum;i++)
	{
		if (puppleEnviVarTBL[i] == NULL)
			continue;
		
		//if (puppleEnviVarTBL[i]->pVariableName == NULL)
		if (puppleEnviVarTBL[i]->pVariableValue[0] == NULL)
			continue;
	
		//if (strcmp(puppleEnviVarTBL[i]->pVariableName,varName) != 0)
		if (strcmp(puppleEnviVarTBL[i]->pVariableValue[0],varName) != 0)
			continue;

		return(i);
	}
	
	return(errCodeEnviMDL_VarNotExists);
}

PUppleEnviVariable ReadUppleEnviVarFromStr(char *str)
{
	int 	i;
	int	j;
	int	varValueNum;
	char	varName		[512];
	char	varValue	[512];
	PUppleEnviVariable pvar;
	int	len;
	
	//if (str == NULL)
	if (UppleIsRemarkFileLine(str) || UppleIsRecFormatDefLine(str))
		return(NULL);
		
	len = strlen(str);
	
	// 1.读取变量名称
	// 1.1取变量的第一个字符
	for (i=0;i<len;i++)
	{
		if (str[i] == '[')
			break;
		if (str[i] == '\0')
			return(NULL);
		if (str[i] == '#')
			return(NULL);
	}
	if (i == len)
		return(NULL);		
	// 1.2开始读取变量名	
	for (++i,j=0;i < len;i++,j++)
	{
		if (str[i] == '\0')
			return(NULL);
			
		if (str[i] == ']')
			break;
		
		varName[j] = str[i];
	}
	// 1.3变量名读取成功
	if (i == len)
		return(NULL);
	varName[j] = '\0';

	if ((pvar = (PUppleEnviVariable)malloc(sizeof(TUppleEnviVariable))) == NULL)
	{
		printf("in ReadUppleEnviVarFromStr:: malloc PUppleEnviVariable!\n");
		return(NULL);
	}
	for (varValueNum = 0; varValueNum < 10; varValueNum++)
		pvar->pVariableValue[varValueNum] = NULL;

	//if ((pvar->pVariableName = (char *)malloc(strlen(varName)+1)) == NULL)
	if ((pvar->pVariableValue[0] = (char *)malloc(strlen(varName)+1)) == NULL)
	{
		printf("in ReadUppleEnviVarFromStr:: malloc pvar->pVariableName!\n");
		free(pvar);
		return(NULL);
	}
	//UppleToUpperCase(varName);
	//strcpy(pvar->pVariableName,varName);
	strcpy(pvar->pVariableValue[0],varName);
	
	// 2.开始读取变量值
	for (varValueNum = 1; varValueNum < 10; varValueNum++)
	{
		// 2.1 读取变量值的第一个字母的位置
		for (++i;i<len;i++)
		{
			if (str[i] == '[')
				break;	
			if (str[i] == '\0')
				return(pvar);
		}
		if (i == len)
			return(pvar);
	
		// 2.2 读取变量值
		for (++i,j=0;i<len;i++,j++)
		{
			if (str[i] == '\0')
				return(pvar);
			if (str[i] == ']')
				break;
			varValue[j] = str[i];
		}
		if (i == len)
			return(pvar);
		varValue[j] = '\0';
	
		if ((pvar->pVariableValue[varValueNum] = (char *)malloc(strlen(varValue)+1)) == NULL)
			return(pvar);
	
		strcpy(pvar->pVariableValue[varValueNum],varValue);
	}
	
	return(pvar);
}

int UppleReadOneFileLine(FILE *fp,char *tmpBuf)
{
	char *p;
	char c;
	int  num;
	
	p = tmpBuf;
	num = 0;
	while (!feof(fp))
	{
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
		tmpBuf[num] = 0;
	if (num == 0)
		return(errCodeEnviMDL_NullLine);
	else
		return(num);
}

int UppleIsThisVarDefinitionLine(char *line,char *varName)
{
	PUppleEnviVariable 	pvar;
	int			yes;
	
	if ((pvar = ReadUppleEnviVarFromStr(line)) == NULL)
		return(0);
	
	if (pvar->pVariableValue[0] == NULL)
		yes = 0;
	else
	{	
		if (strcmp(pvar->pVariableValue[0],varName) == 0)
			yes = 1;
		else
			yes = 0;
	}
	UppleFreeEnviVar(pvar);
	
	return(yes);
}

int UppleReadVarNameFromLine(char *line,char *varName)
{
	PUppleEnviVariable 	pvar;
	
	if ((pvar = ReadUppleEnviVarFromStr(line)) == NULL)
		return(errCodeEnviMDL_NullLine);
	
	if (pvar->pVariableValue[0] == NULL)
		varName[0] = 0;
	else
		strcpy(varName,pvar->pVariableValue[0]);
		
	UppleFreeEnviVar(pvar);
	
	return(0);
}

char *UppleReadValueOfTheIndexOfEnviVar(PUppleEnviVariable pVar,int index)
{
	if ((pVar == NULL) || (index < 0) || (index > 10))
		return(NULL);
	return(pVar->pVariableValue[index]);
}
// 2006/8/21 增加函数
int UppleIsRemarkFileLine(char *str)
{
	if ((str == NULL) || (strlen(str) == 0))
		return(1);
	UppleFilterLeftBlank(str);
	if ((str[0] == '#') || (strncmp(str,"//",2) == 0))
		return(1);
	else
		return(0);
}

int UppleUpdateEnviVar(char *fileName,char *varName,char *fmt,...)
{
	FILE	*fp1 = NULL;
	FILE	*fp2 = NULL;
	int	ret;
	char	tmpBuf[1024+1];
	int	varUpdated = 0;
	char	varNameStr[128+1];
	char	*ptr;
	va_list 	args;
	TUppleEnviRecFldLength	recFldLen;
		
	if ((fileName == NULL) || (varName == NULL) || (strlen(varName) + 3 >= sizeof(varNameStr)))
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"r")) == NULL)
	{
		printf("in UppleUpdateEnviVar:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	sprintf(tmpBuf,"%s/%d.tmp",getenv("UPPLETEMP"),getpid());
	if ((fp2 = fopen(tmpBuf,"w")) == NULL)
	{
		printf("in UppleUpdateEnviVar:: fopen [%s]\n",tmpBuf);
		fclose(fp1);
		return(errCodeUseOSErrCode);
	}

	sprintf(varNameStr,"[%s]",varName);
	UppleInitRecFldLengthDef(recFldLen);
	for (;!feof(fp1);)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UppleReadOneFileLine(fp1,tmpBuf)) < 0)
			continue;
		if (ret == 0)
		{
			if (!feof(fp1))
				goto writeLine;
			else
				break;
		}
		if (varUpdated)				// 变量已更新
			goto writeLine;
		if (UppleIsRemarkFileLine(tmpBuf))	// 注释行
			goto writeLine;
		if (UppleIsRecFormatDefLine(tmpBuf))
			UppleReadEnviRecFldLengthFromStr(tmpBuf,recFldLen);
		if ((ptr = strstr(tmpBuf,varNameStr)) == NULL)	// 不是变量定义行
			goto writeLine;
		if (ptr != tmpBuf)			// 不是变量名定义
			goto writeLine;
		// 是变量定义行
		varUpdated = 1;
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%s",varNameStr);
		va_start(args,fmt);
		vsprintf(tmpBuf+strlen(varNameStr),fmt,args);
		va_end(args);
		if (strlen(tmpBuf+strlen(varNameStr)) == 0)	// 未定义变量值，删除这个变量
			continue;
		UppleConvertEnviRecStrIntoFormat(recFldLen,tmpBuf,sizeof(tmpBuf));
		fprintf(fp2,"%s\n",tmpBuf);
		continue;
writeLine:	// 不是变量定义行的写入
		fprintf(fp2,"%s\n",tmpBuf);
		continue;
	}
	fclose(fp1);
	fclose(fp2);
	if (varUpdated)	// 变量被更新了
	{		// 更新文件
		sprintf(tmpBuf,"cp %s/%d.tmp %s",getenv("UPPLETEMP"),getpid(),fileName);
		system(tmpBuf);
	}
	// 删除暂时文件
//	sprintf(tmpBuf,"rm %s/%d.tmp",getenv("UPPLETEMP"),getpid());
//	system(tmpBuf);
	sprintf(tmpBuf," %s/%d.tmp",getenv("UPPLETEMP"),getpid());
	unlink(tmpBuf);
	if (varUpdated)
		return(0);
	else
		return(errCodeEnviMDL_VarNotExists);
}

int UppleInsertEnviVar(char *fileName,char *varName,char *fmt,...)
{
	FILE			*fp1 = NULL;
	int			ret;
	char			tmpBuf[1024+1];
	va_list 		args;
	TUppleEnviRecFldLength	recFldLen;
	int			len;
	
	if (UppleExistEnviVarInFile(fileName,varName) >= 0)
	{
		printf("in UppleInsertEnviVar:: varName [%s] already exists in [%s]\n",varName,fileName);
		return(errCodeEnviMDL_VarAlreadyExists);
	}

	UppleReadEnviRecFldLengthDefFromFile(fileName,recFldLen);
		
	if ((fileName == NULL) || (varName == NULL))
		return(errCodeParameter);
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"[%s]",varName);
	len = strlen(tmpBuf);
	va_start(args,fmt);
	vsprintf(tmpBuf+len,fmt,args);
	va_end(args);
	if (strlen(tmpBuf+len) == 0)
		return(errCodeEnviMDL_NoValueDefinedForVar);
	if ((fp1 = fopen(fileName,"a")) == NULL)
	{
		printf("in UppleInitEnvi:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	UppleConvertEnviRecStrIntoFormat(recFldLen,tmpBuf,sizeof(tmpBuf));
	fprintf(fp1,"%s\n",tmpBuf);
	fclose(fp1);
	return(0);
}
	
int UppleDeleteEnviVar(char *fileName,char *varName)
{
	int	ret;
	
	if ((ret = UppleUpdateEnviVar(fileName,varName,"")) < 0)
	{
		printf("in UppleDeleteEnviVar:: UppleUpdateEnviVar [%s] [%s]\n",fileName,varName);
		return(ret);
	}
	return(ret);
}

int UppleExistEnviVarInFile(char *fileName,char *varName)
{
	FILE	*fp1 = NULL;
	int	ret;
	char	tmpBuf[1024+1];
	char	varNameStr[128+1];
	char	*ptr;
		
	if ((fileName == NULL) || (varName == NULL) || (strlen(varName) + 3 >= sizeof(varNameStr)))
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"r")) == NULL)
	{
		printf("in UppleInsertEnviVar:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	sprintf(varNameStr,"[%s]",varName);
	for (;!feof(fp1);)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (UppleReadOneFileLine(fp1,tmpBuf) <= 0)
			continue;
		if (UppleIsRemarkFileLine(tmpBuf))	// 注释行
			continue;
		if ((ptr = strstr(tmpBuf,varNameStr)) == NULL)	// 不是变量定义行
			continue;
		if (ptr != tmpBuf)			// 不是变量名定义
			continue;
		fclose(fp1);
		return(1);
	}
	fclose(fp1);
	return(errCodeEnviMDL_VarNotExists);
}

int UppleIsRecFormatDefLine(char *str)
{
	char	*ptr;
	
	if (UppleIsRemarkFileLine(str))
		return(0);
	
	if ((ptr = strstr(str,"RECFORMAT=")) == NULL)
		return(0);
	else
		return(1);
}

int UppleReadEnviRecFldLengthFromStr(char *str,TUppleEnviRecFldLength fldLenDef)
{
	int	index;
	char	*varStart;
	char	*varEnd;
	int	strLen;

	UppleInitRecFldLengthDef(fldLenDef);
		
	if (!UppleIsRecFormatDefLine(str))
		return(errCodeEnviMDL_NotRecFormatDefStr);

	if ((varStart = strstr(str,"RECFORMAT=")) == NULL)
		return(errCodeEnviMDL_NotRecFormatDefStr);
	varStart += 10;
	strLen = strlen(str);
	for (index = 0;index < conMaxFldNumOfEnviRec;index++)
	{
		if ((varEnd = strstr(varStart,";")) == NULL)
			break;
		*varEnd = 0;		
		fldLenDef[index] = atoi(varStart);
		*varEnd = ';';
		if (strLen + str <= varEnd)
			break;
		varStart = varEnd + 1;
	}
	return(0);
}

int UppleConvertEnviRecStrIntoFormat(TUppleEnviRecFldLength fldLenDef,char *recStr,int sizeOfRecStr)
{
	int	index;
	char	tmpBuf[512+1];
	int	strLen;
	char	*varStart;
	char	*varEnd;
	int	fldLen;
	int	offset = 0;
	char	tmpChar;
	
	if ((strLen = strlen(recStr)) >= sizeof(tmpBuf))
	{
		printf("in UppleConvertEnviRecStrIntoFormat:: recStr [%s] too long!\n",recStr);
		return(errCodeEnviMDL_NotRecStrTooLong);
	}
	strcpy(tmpBuf,recStr);
	varEnd = tmpBuf;
	for (index = 0; index < conMaxFldNumOfEnviRec; index++)
	{
		if ((varStart = strstr(varEnd,"[")) == NULL)
		{
			strcpy(recStr+offset,varEnd);
			offset += strlen(varEnd);
		}
		if ((varEnd = strstr(varStart,"]")) == NULL)
		{
			strcpy(recStr+offset,varStart);
			offset += strlen(varStart);
			break;
		}
		varEnd = varEnd+1;
		if (tmpBuf+strLen <= varEnd)
		{
			strcpy(recStr+offset,varStart);
			offset += strlen(varStart);
			break;
		}
		tmpChar = *varEnd;
		*varEnd = 0;
		fldLen = strlen(varStart);
		if (offset + fldLen >= sizeOfRecStr)
		{
			printf("in UppleConvertEnviRecStrIntoFormat:: sizeOfRecStr [%d] < expected [%d]\n",sizeOfRecStr,offset+fldLen);
			return(errCodeSmallBuffer);
		}
		strcpy(recStr+offset,varStart);
		if (fldLen < fldLenDef[index])
		{
			if (offset + fldLenDef[index] >= sizeOfRecStr)
			{
				printf("in UppleConvertEnviRecStrIntoFormat:: sizeOfRecStr [%d] < expected [%d]\n",sizeOfRecStr,offset+fldLenDef[index]);
				return(errCodeSmallBuffer);
			}
			memset(recStr+offset+fldLen,' ',fldLenDef[index]-fldLen);
			fldLen = fldLenDef[index];
		}
		offset += fldLen;
		*varEnd = tmpChar;
	}
	return(offset);
}
	
int UppleInitRecFldLengthDef(TUppleEnviRecFldLength fldLenDef)
{
	int	index;
	
	for (index = 0; index < conMaxFldNumOfEnviRec; index++)
		fldLenDef[index] = -1;
	return(0);
}

int UppleReadEnviRecFldLengthDefFromFile(char *fileName,TUppleEnviRecFldLength recFldLen)
{
	FILE			*fp1 = NULL;
	int			ret;
	char			tmpBuf[1024+1];
		
	UppleInitRecFldLengthDef(recFldLen);

	if (fileName == NULL)
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"r")) == NULL)
	{
		printf("in UppleReadEnviRecFldLengthDefFromFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	for (;!feof(fp1);)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UppleReadOneFileLine(fp1,tmpBuf)) < 0)
			continue;
		if (UppleIsRecFormatDefLine(tmpBuf))
		{
			UppleReadEnviRecFldLengthFromStr(tmpBuf,recFldLen);
			break;
		}
	}
	fclose(fp1);
	return(0);
}
int UppleReadEnviVarDefLineInFile(char *fileName,char *varName,char *varDefLine)
{
	FILE	*fp1 = NULL;
	int	ret;
	char	tmpBuf[1024+1];
	char	varNameStr[128+1];
	char	*ptr;
		
	if ((fileName == NULL) || (varName == NULL) || (strlen(varName) + 3 >= sizeof(varNameStr)) || (varDefLine == NULL))
		return(errCodeParameter);
	
	if ((fp1 = fopen(fileName,"r")) == NULL)
	{
		printf("in UppleInsertEnviVar:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	sprintf(varNameStr,"[%s]",varName);
	for (;!feof(fp1);)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (UppleReadOneFileLine(fp1,tmpBuf) <= 0)
			continue;
		if (UppleIsRemarkFileLine(tmpBuf))	// 注释行
			continue;
		if ((ptr = strstr(tmpBuf,varNameStr)) == NULL)	// 不是变量定义行
			continue;
		fclose(fp1);
		strcpy(varDefLine,tmpBuf);
		return(0);
	}
	fclose(fp1);
	return(errCodeEnviMDL_VarNotExists);
}
