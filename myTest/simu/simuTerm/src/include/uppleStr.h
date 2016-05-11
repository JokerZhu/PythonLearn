//	Author:		zhangyongding
//	Date:		20080922
//	Version:	1.0


#ifndef _UppleString_
#define _UppleString_

// 20090519 ����
/* ȥ����ߵ�0 */
int UppleFilterLeftZero(char *Str);

// 2008/10/19������
// ����
int strReverse(char *str);

/* 
����	��10�����ַ���ת��Ϊ2,8,16������
�������
	m	10�����ַ���
	base	������
�������
	result	��Ӧ������
*/
int intTo2_8_16Str(long m,int base,char *result);

/* 
����	��16�����ַ���ת��Ϊ10������
�������
	strSource	16�����ַ���
����ֵ
	> 0		10������
	= 0		����
*/
int strHexToInt(char *strSource);

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

int UppleConvertOneFldSeperatorInRecStrIntoAnother(char *oriStr,int lenOfOriStr,char oriFldSeperator,char desFldSeperator,char *desStr,int sizeOfDesStr);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UppleReadIntTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,int *value);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UppleReadLongTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,long *value);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UppleReadDoubleTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,double *value);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UppleReadCharTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value);

// 2008/7/18,����
// ��һ����¼�������ַ�����
// ����ֵ�Ǽ�¼���ڴ��еĳ���
int UpplePutRecFldIntoRecStr(char *fldName,char *value,int lenOfValue,char *recStr,int sizeOfRecStr);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UppleReadRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf);

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
int UppleDealWithIPAddr(char *ipAddr,int level,char *outIPAddr);

// 2008/5/15����
// ���һ���ַ��Ƿ������У��
int UppleIsOddParityChar(char chCheckChar);

// 2008/5/15����
// ��һ���ַ���������У��
int UppleMakeStrIntoOddParityStr(char *pData,int nLenOfData);

// 2008/5/15����
// ���һ���ַ����Ƿ������У��
// ������У�鷵��ֵΪ1�����򷵻�ֵΪ0
int UppleCheckStrIsOddStr(char *pData,int nLenOfData);

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
int UppleReadFldFromBuf(char *buf,char spaceMark,int index,char *fld);

// ȥ���ַ��������пո�2008-3-19
int UppleFilterAllBlankChars(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

// 2008/3/17����
// ȥ���ַ����еĶ���ո��tab����������ո�ѹ��Ϊһ���ո�ȥ��ͷ��β�Ŀո��tab
int UppleFilterRubbisBlankAndTab(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

// �ж��Ƿ�Ϸ��ı�����
int UppleIsValidAnsiCVarName(char *varName);

// ���ı��ļ���ȡһ��
int UppleReadOneLineFromTxtStr(FILE *fp,char *buf,int sizeOfBuf);

// ��һ���ַ����ж�ȡ����
/* ���������
	str	�ַ���
   �������
   	par	�Ŷ����Ĳ���
   	maxNum	Ҫ��������������Ŀ
   ����ֵ
   	���ض�ȡ�Ĳ�����Ŀ
*/
int UppleReadParFromStr(char *str,char par[][128+1],int maxNum);

// �������Сд���ת��Ϊ��д���
int UppleSmallAmtToBigAmt(char *smallAmt, char *bigAmt);

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
int UppleReadParFromParGrp(char *parGrp[],int parNum,char *parID,char *parValue,int sizeOfParValue);

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
int UppleReadParFromCmdStr(char *cmdStr,char *parID,char *parValue,int sizeOfParValue);

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
int UppleReadFldFromRecStr(char *recStr,char *fldName,char *fldTypeName,unsigned char *fldValue,int sizeOfFldValue);

// ����PK���ĳ���
int UppleGetPKOutOfRacalHsmCmdReturnStr(unsigned char *racalPKStr,int lenOfRacalPKStr,char *pk,int *lenOfPK,int sizeOfBuf);

int UppleFormFullDateFromMMDDDate(char *mmDDDate,char *fullDate);

// �жϴ����Ƿ���.
int UppleIsStringHasDotChar(char *str);

// ȥ���ַ����еĶ���ո񣬼�������ո�ѹ��Ϊһ���ո�ȥ��ͷ��β�Ŀո�
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
//��ȡ�̻������
int GetMidnameShorthand(char *src,int destlenght,char *dest,int sizedest);

int _count_string(char *data, char *key);
char *malloc_replace(char *data, char *rep, char *to, int free_data);
void StrReplace(char *data, char *rep, char *to);
#endif
