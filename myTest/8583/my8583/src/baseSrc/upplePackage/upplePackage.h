#ifndef _UpplePackage_
#define _UpplePackage_

#include "uppleErrCode.h"
#include "upplePackageDef.h"

/*
#define MAX_FLDS   	       256
#define MAX_SERIALIZEBUF_BUF   (1024*16)

typedef struct
{
	int			index[MAX_FLDS];	// 序号
	int			len[MAX_FLDS];		// 长度
	int			offset[MAX_FLDS];	// 偏移 
	unsigned char		value[MAX_SERIALIZEBUF_BUF];	// 值
} TSerializeBuf;
*/

typedef struct
{
	int			index;		// 序号
	int			len;		// 长度
	unsigned char		*value;		// 值
} TUpplePackageFld;
typedef TUpplePackageFld	*PUpplePackageFld;

typedef struct
{
	int			connected;		// 标识是否已连接
	int			dynamicPackage; 	// 是否是动态申请的Package
	PUpplePackageDef	pdef;			// 域定义	
	TUpplePackageFld	flds[conMaxNumOfPackageFlds];
//TSerializeBuf		sbuf;
} TUpplePackage;
typedef TUpplePackage		*PUpplePackage;

typedef	char TUppleTransFld[256][1024];

PUpplePackage UppleConnectPackage(char *nameOfPackageDef);
int UppleDisconnectPackage(PUpplePackage ppackage);
int UppleIsValidPackage(PUpplePackage ppackage);
int UppleInitPackage(PUpplePackage ppackage);
int UppleLogPackageFld(PUpplePackageFldDef pfldDef,PUpplePackageFld pfld);				
int UppleLogPackage(PUpplePackage ppackage);
int UppleSetPackageBitMapFld(PUpplePackage ppackage,int fldIndex);
int UpplePack(PUpplePackage ppackage,unsigned char *buf,int sizeOfBuf);
int UpplePackP(PUpplePackage ppackage,unsigned char *buf,int sizeOfBuf);
int UppleExistPackageFld(TUpplePackageFld *pfldMap,int fldIndex);
int UppleUnpack(PUpplePackage ppackage,unsigned char *buf,int len);
int UppleSetPackageFld(PUpplePackage ppackage,int fldIndex,char *value,int len);
int UppleReadPackageFld(PUpplePackage ppackage,int fldIndex,char *value,int sizeOfBuf);
int UppleSetPackageFldWithDefaultValue(PUpplePackage ppackage,int fldIndex);
int UppleSetNullPackageFld(PUpplePackage ppackage,int fldIndex);

int UpplePrintPackageToFile(PUpplePackage ppackage,FILE *fp);
int UpplePrintPackageFldToFile(PUpplePackageFldDef pfldDef,PUpplePackageFld pfld,FILE *fp);

// 2006/6/23 增加了以下函数
int UpplePutBcdBitStrIntoStr(int isLeftDuiQi,char buQiChar,char *bcdBitStr,int len,char *desStr,int sizeOfBuf);
// 将一个域的长度指示域打入到字符串中
// 返回打入到字符串中的长度
int UpplePutPackageFldLenIntoStr(PUpplePackage ppackage,int fldIndex,unsigned char *buf,int sizeOfBuf);
// 将一个域打入到字符串中
// 第一个参数表明是否由位图指示域存在与否
// 返回打入到字符串中的长度
int UpplePutPackageFldIntoStr(int isFldIndentifiedByBitMap,PUpplePackage ppackage,int fldIndex,unsigned char *buf,int sizeOfBuf);

int UppleReadBcdBitStrFromStr(int isLeftDuiQi,char *oriStr,int lenOfOriStr,char *bcdBitStr,int lenOfRead);
// 从一个串中读出域的长度指示域
// 返回域的长度指示域在串中的长度
int UppleReadPackageFldLenFromStr(PUpplePackage ppackage,int fldIndex,unsigned char *buf,int lenOfStr);
// 从一个串中读出一个域
// 第一个参数表明是否由位图指示域存在与否
int UppleReadPackageFldFromStr(PUpplePackage ppackage,int fldIndex,unsigned char *buf,int lenOfStr);

int UppleGetFlds(PUpplePackage ppackage,TUppleTransFld ptransfld);

#endif
