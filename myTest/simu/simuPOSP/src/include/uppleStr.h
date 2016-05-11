//	Author:		zhangyongding
//	Date:		20080922
//	Version:	1.0


#ifndef _UppleString_
#define _UppleString_

// 20090519 增加
/* 去掉左边的0 */
int UppleFilterLeftZero(char *Str);

// 2008/10/19，增加
// 倒序
int strReverse(char *str);

/* 
功能	把10进制字符串转换为2,8,16进制数
输入参数
	m	10进制字符串
	base	进制数
输出参数
	result	相应进制数
*/
int intTo2_8_16Str(long m,int base,char *result);

/* 
功能	把16进制字符串转换为10进制数
输入参数
	strSource	16进制字符串
返回值
	> 0		10进制数
	= 0		出错
*/
int strHexToInt(char *strSource);

// 2008/7/29，增加
/*
功能	使用通用的域分隔符替换指定的分隔符
输入参数
	oriStr		串
	lenOfOriStr	串长度
	oriFldSeperator	源分隔符
	desFldSeperator	目标分隔符
	sizeOfDesStr	目标串缓冲的大小
输出参数
	desStr		目标串
返回值
	>= 0		目标串的长度
	<0		出错代码
*/

int UppleConvertOneFldSeperatorInRecStrIntoAnother(char *oriStr,int lenOfOriStr,char oriFldSeperator,char desFldSeperator,char *desStr,int sizeOfDesStr);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UppleReadIntTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,int *value);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UppleReadLongTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,long *value);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UppleReadDoubleTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,double *value);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UppleReadCharTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value);

// 2008/7/18,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UpplePutRecFldIntoRecStr(char *fldName,char *value,int lenOfValue,char *recStr,int sizeOfRecStr);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UppleReadRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf);

// 2008/5/22，增加
/* 保留指定的IP地址级别
输入参数
	ipAddr	原始ip地址
	level	要保留的ip地址的级别
		4，保留4部分
		3，保留前3部分
		2，保留前2部分
		1，保留前1部分
		0，全部不保留
输出参数
	outIPAddr	加工后的ip地址
			不保留的部分以XXX替代
返回值
	0	成功
	负值	出错代码
*/
int UppleDealWithIPAddr(char *ipAddr,int level,char *outIPAddr);

// 2008/5/15增加
// 检查一个字符是否符合奇校验
int UppleIsOddParityChar(char chCheckChar);

// 2008/5/15增加
// 对一个字符串增加奇校验
int UppleMakeStrIntoOddParityStr(char *pData,int nLenOfData);

// 2008/5/15增加
// 检查一个字符串是否符合奇校验
// 符合奇校验返回值为1，否则返回值为0
int UppleCheckStrIsOddStr(char *pData,int nLenOfData);

/* 2008/3/31增加
功能：从一个字符串得到指定第几个域的值
输入参数:
	buf		输入字符串
	spaceMark	分隔符
	index		第几个值
输出参数：
	fld		第index的值

返回值：
	返回值的长度
*/
int UppleReadFldFromBuf(char *buf,char spaceMark,int index,char *fld);

// 去掉字符串中所有空格，2008-3-19
int UppleFilterAllBlankChars(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

// 2008/3/17增加
// 去掉字符串中的多余空格和tab，即将多个空格压缩为一个空格。去掉头和尾的空格和tab
int UppleFilterRubbisBlankAndTab(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

// 判断是否合法的变量名
int UppleIsValidAnsiCVarName(char *varName);

// 从文本文件读取一行
int UppleReadOneLineFromTxtStr(FILE *fp,char *buf,int sizeOfBuf);

// 从一个字符串中读取参数
/* 输入参数：
	str	字符串
   输出参数
   	par	放读出的参数
   	maxNum	要读出的最大参数数目
   返回值
   	返回读取的参数数目
*/
int UppleReadParFromStr(char *str,char par[][128+1],int maxNum);

// 将人民币小写金额转换为大写金额
int UppleSmallAmtToBigAmt(char *smallAmt, char *bigAmt);

// 从参数组中读取参数
// 参数组中
/* 输入参数：
	parGrp,参数组
	parNum，参数数目
	parID,参数名称
	sizeOfParValue,参数缓冲的大小
   输出参数
   	parValue,接受参数的缓冲
   返回值：
   	<0,错误码
   	>=0,参数长度
*/
int UppleReadParFromParGrp(char *parGrp[],int parNum,char *parID,char *parValue,int sizeOfParValue);

// 从命令串中读取参数
// 命令串的定义格式为:
//	-parID parValue
/* 输入参数：
	cmdStr,命令串
	parID,参数名称
	sizeOfParValue,参数缓冲的大小
   输出参数
   	parValue,接受参数的缓冲
   返回值：
   	<0,错误码
   	>=0,参数长度
*/
int UppleReadParFromCmdStr(char *cmdStr,char *parID,char *parValue,int sizeOfParValue);

// 从记录定义串中读取域值
// 记录串的定义格式为:
//	fldName=fldValue;fldName=fldValue;...fldName=fldValue;
/* 输入参数：
	recStr,记录串
	fldName,域名
	fldTypeName,类型名
		合法值	char/short/int/long/double/string
	sizeOfFldValue,域值缓冲的大小
   输出参数
   	fldValue,接受域值的缓冲
   返回值：
   	<0,错误码
   	>=0,域值长度
*/
int UppleReadFldFromRecStr(char *recStr,char *fldName,char *fldTypeName,unsigned char *fldValue,int sizeOfFldValue);

// 返回PK串的长度
int UppleGetPKOutOfRacalHsmCmdReturnStr(unsigned char *racalPKStr,int lenOfRacalPKStr,char *pk,int *lenOfPK,int sizeOfBuf);

int UppleFormFullDateFromMMDDDate(char *mmDDDate,char *fullDate);

// 判断串中是否含有.
int UppleIsStringHasDotChar(char *str);

// 去掉字符串中的多余空格，即将多个空格压缩为一个空格。去掉头和尾的空格
int UppleFilterRubbisBlank(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

int UppleConvertLongStrIntoDoubleOf2TailStr(char *str);
int UppleConvertDoubleOf2TailStrIntoLongStr(char *str);

int UppleIsBCDStr(char *str1);
int UppleIsDigitStr(char *str);

int UppleConvertIntIntoStr(int data,int len,char *str);
int UppleConvertLongIntoStr(long data,int len,char *str);

int UppleConvertIntStringToInt(char *str,int lenOfStr);
long UppleConvertLongStringToLong(char *str,int lenOfStr);

int UppleConvertIntoLen(char *str,int intLen);

int UppleToUpper(char *lowerstr,char *upperstr);
int UppleUpperMySelf(char *str,int lenOfStr);
int UppleToUpperCase(char *str);
int UppleToLowerCase(char *str);
int UppleFilterRightBlank(char *Str);
int UppleFilterLeftBlank(char *Str);
int UppleVerifyDateStr(char *Str);
int UppleIsValidIPAddrStr(char *ipAddr);
int UppleCopyFilterHeadAndTailBlank(char *str1,char *str2);
int UppleFilterHeadAndTailBlank(char *str);
int UppleReadDirFromStr(char *str,int dirLevel,char *dir);

char hextoasc(int xxc);
char hexlowtoasc(int xxc);
char hexhightoasc(int xxc);
char asctohex(char ch1,char ch2);
int aschex_to_bcdhex(char aschex[],int len,char bcdhex[]);
int bcdhex_to_aschex(char bcdhex[],int len,char aschex[]);
int byte2int_to_bcd(int aa,char xx[]);
int IntToBcd(int aa,unsigned char xx[]);
int BcdToInt(char xx[]);

int UppleFormANSIX80LenString(int len,char *str,int sizeOfStr);
int UppleFormANSIDERRSAPK(char *pk,int lenOfPK,char *derPK,int sizeOfDERPK);
int UppleGetPKOutOfANSIDERRSAPK(char *derPK,int lenOfDERPK,char *pk,int sizeOfPK);
int UppleFilterRSASignNullChar(char *str,int len);

int UppleIsUnixShellRemarkLine(char *line);
int UppleConvertUnixShellStr(char *oldStr,int lenOfOldStr,char *newStr,int sizeOfNewStr);
int UppleReadUnixEnvVarFromStr(char *envVarStr,int lenOfEnvVarStr,char *envVar,int *envVarNameLen);
int UppleDeleteSuffixVersionFromStr(char *oldStr,char *newStr);
int UppleStrContainCharStar(char *str);

int UppleIsValidFullDateStr(char *date);

int UppleIsStringContainingOnlyZero(char *str,int lenOfStr);
//获取商户名简称
int GetMidnameShorthand(char *src,int destlenght,char *dest,int sizedest);

int _count_string(char *data, char *key);
char *malloc_replace(char *data, char *rep, char *to, int free_data);
void StrReplace(char *data, char *rep, char *to);
#endif
