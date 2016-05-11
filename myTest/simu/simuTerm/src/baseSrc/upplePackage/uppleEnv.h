#ifndef _UppleVar
#define _UppleVar

int UppleGetEnviVarNum();
char *UppleGetEnviVarByIndex(int index);
char *UppleGetEnviVarByName(char *varname);
int UppleInitEnvi(char *filename);
int UppleClearEnvi();
char *UppleGetEnviVarNameByIndex(int index);

#ifdef _MaxVarNum_1024_
#define MAXUPPLEENVIVARIABLES	1024
#else
#ifdef _MaxVarNum_2048_
#define MAXUPPLEENVIVARIABLES	2048
#else
#ifdef _MaxVarNum_4096_
#define MAXUPPLEENVIVARIABLES	4096
#else
#ifdef _LargeEnvTBL_
#define MAXUPPLEENVIVARIABLES	1000
#else
#define MAXUPPLEENVIVARIABLES	1024	
#endif	// _LargeEnvTBL_
#endif  // _MaxVarNum_4096_
#endif  // _MaxVarNum_2048_
#endif  // _MaxVarNum_1024_

#ifndef _LargeEnvVar_
typedef struct
{
	//char	*pVariableName;
	char	*pVariableValue[10];
} TUppleEnviVariable;
#else
#define conMaxNumOfValuesForAnEnvVar	100
typedef struct
{
	//char	*pVariableName;
	char	*pVariableValue[conMaxNumOfValuesForAnEnvVar];
} TUppleEnviVariable;
#endif
typedef TUppleEnviVariable	*PUppleEnviVariable;

// 读取指定索引变量的，第N个值，N=varValueIndex
char *UppleGetEnviVarOfTheIndexByIndex(int varIndex,int varValueIndex);
// 读取指定名称变量对应的索引号
int UppleGetVarIndexOfTheVarName(char *varName);

char *UppleReadValueOfTheIndexOfEnviVar(PUppleEnviVariable pVar,int index);

PUppleEnviVariable ReadUppleEnviVarFromStr(char *str);
int UppleReadOneFileLine(FILE *fp,char *tmpBuf);
int UppleFreeEnviVar(PUppleEnviVariable pvar);
int UppleIsThisVarDefinitionLine(char *line,char *varName);
int UppleReadVarNameFromLine(char *line,char *varName);

// 2006/08/24 增加以下定义
#define conMaxFldNumOfEnviRec	20
typedef int	TUppleEnviRecFldLength[conMaxFldNumOfEnviRec];
// 2006/8/21 增加函数
int UppleUpdateEnviVar(char *fileName,char *varName,char *fmt,...);
int UppleInsertEnviVar(char *fileName,char *varName,char *fmt,...);
int UppleDeleteEnviVar(char *fileName,char *varName);
int UppleIsRemarkFileLine(char *str);
int UppleExistEnviVarInFile(char *fileName,char *varName);
int UppleIsRecFormatDefLine(char *str);
int UppleReadEnviRecFldLengthFromStr(char *str,TUppleEnviRecFldLength fldLenDef);
int UppleConvertEnviRecStrIntoFormat(TUppleEnviRecFldLength fldLenDef,char *recStr,int sizeOfRecStr);
int UppleInitRecFldLengthDef(TUppleEnviRecFldLength fldLenDef);
int UppleReadEnviRecFldLengthDefFromFile(char *fileName,TUppleEnviRecFldLength recFldLen);
int UppleReadEnviVarDefLineInFile(char *fileName,char *varName,char *varDefLine);

#endif
