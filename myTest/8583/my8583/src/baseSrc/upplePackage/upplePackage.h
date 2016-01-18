#ifndef _UpplePackage_
#define _UpplePackage_

#include "uppleErrCode.h"
#include "upplePackageDef.h"

/*
#define MAX_FLDS   	       256
#define MAX_SERIALIZEBUF_BUF   (1024*16)

typedef struct
{
	int			index[MAX_FLDS];	// ���
	int			len[MAX_FLDS];		// ����
	int			offset[MAX_FLDS];	// ƫ�� 
	unsigned char		value[MAX_SERIALIZEBUF_BUF];	// ֵ
} TSerializeBuf;
*/

typedef struct
{
	int			index;		// ���
	int			len;		// ����
	unsigned char		*value;		// ֵ
} TUpplePackageFld;
typedef TUpplePackageFld	*PUpplePackageFld;

typedef struct
{
	int			connected;		// ��ʶ�Ƿ�������
	int			dynamicPackage; 	// �Ƿ��Ƕ�̬�����Package
	PUpplePackageDef	pdef;			// ����	
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

// 2006/6/23 ���������º���
int UpplePutBcdBitStrIntoStr(int isLeftDuiQi,char buQiChar,char *bcdBitStr,int len,char *desStr,int sizeOfBuf);
// ��һ����ĳ���ָʾ����뵽�ַ�����
// ���ش��뵽�ַ����еĳ���
int UpplePutPackageFldLenIntoStr(PUpplePackage ppackage,int fldIndex,unsigned char *buf,int sizeOfBuf);
// ��һ������뵽�ַ�����
// ��һ�����������Ƿ���λͼָʾ��������
// ���ش��뵽�ַ����еĳ���
int UpplePutPackageFldIntoStr(int isFldIndentifiedByBitMap,PUpplePackage ppackage,int fldIndex,unsigned char *buf,int sizeOfBuf);

int UppleReadBcdBitStrFromStr(int isLeftDuiQi,char *oriStr,int lenOfOriStr,char *bcdBitStr,int lenOfRead);
// ��һ�����ж�����ĳ���ָʾ��
// ������ĳ���ָʾ���ڴ��еĳ���
int UppleReadPackageFldLenFromStr(PUpplePackage ppackage,int fldIndex,unsigned char *buf,int lenOfStr);
// ��һ�����ж���һ����
// ��һ�����������Ƿ���λͼָʾ��������
int UppleReadPackageFldFromStr(PUpplePackage ppackage,int fldIndex,unsigned char *buf,int lenOfStr);

int UppleGetFlds(PUpplePackage ppackage,TUppleTransFld ptransfld);

#endif
