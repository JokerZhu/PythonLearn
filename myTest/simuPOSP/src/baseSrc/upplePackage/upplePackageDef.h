// 2006/6/23,增加了长度指示域相关的定义
// 即增加了 _bitFldLenFlag_ 宏定义相关的内容

#ifndef _UpplePackageDef_
#define _UpplePackageDef_

#include "uppleErrCode.h"

#define conMaxNumOfPackageDef			200

#define conMaxNumOfPackageFlds			(512+1)

#define conConfFileNameOfPackageDefGroup	"packageDefGrp.Def"

// 采用 3位表示：
// 	后两位表示TUpplePackageFldValueClass的值
//	第1位是不赋值时的处理方式。
//		如果值是0，表示，当作赋值了，不足位就采用补位方式来处理。
//		如果值是1，表示，当作不赋值处理，即不需要补位。
#define conPatchWhenFldValueLenIsZero 0
#define conUnpatchWhenFldValueLenIsZero 1
typedef enum
{
	conBitMapFld = 0,
	conBitsFld = 1,
	conAsciiFldFullLen = 2,
	conAsciiFldLeftBlank = 3,
	conAsciiFldRightBlank = 4,
	conAsciiFldLeftZero = 5,
	conAsciiFldRightZero = 6,
	conAsciiFldRightNull = 7,	// 不足位，补空字符
	conBitsFldLeftDuiQi = 8,	// 二进制数左对齐
	conBitsFldRightDuiQi = 9	// 二进制数右对齐
} TUpplePackageFldValueClass;

typedef enum
{
	conFldLenUseAscii = 0,
	conFldLenUseLeftDuiQi,
	conFldLenUseRightDuiQi
} TUpplePackageFldLenPresentation;

typedef struct
{
	int				index;		// 域序号
	int				lenOfLenFlag;	// 变长字段长度指示位的长度，
							// 为0表示定长字段
	int				lenOfValue;	// 字段的长度，对于变长字段，是最大长度，
							// 对于定长字段，字段的长段
							// 为0表示该字段不存在
	TUpplePackageFldValueClass	valueType;
	char				remark[40+1];	// 域说明
} TUpplePackageFldDef;
typedef TUpplePackageFldDef		*PUpplePackageFldDef;

typedef struct
{
	char				name[40+1];		// 包名称
	int				id;			// 在报文格式定义表组中的索引号
	int				maxNum;			// 域的数目
	TUpplePackageFldDef		fldDefTBL[conMaxNumOfPackageFlds];		
	TUpplePackageFldLenPresentation	fldLenPresentation;	// 设定长度指示域的表示方法
} TUpplePackageDef;
typedef TUpplePackageDef		*PUpplePackageDef;

int UppleGetConfFileNameOfPackageDefGroup(char *fileName);
int UppleGetNameOfPackageDefOfIndex(int index,char *packageName);
int UppleGetIDOfSpecifiedPackageDef(char *packageName);		
int UppleIsValidPackageID(int id);
int UppleIsValidPackageMaxFldNum(int num);
int UppleIsValidPackageFldIndex(PUpplePackageDef pdef,int fldIndex);
int UppleIsPackageDefGroupConnected();
int UppleConnectPackageDefGroup();
int UppleReloadPackageDefGroup();
int UppleDisconnectPackageDefGroup();
int UppleRemovePackageDefGroup();
int UpplePrintPackageDefGroupToFile(FILE *fp);
int UppleGetConfFileNameOfPackageDef(char *packageName,char *fileName);
int UppleGetMaxFldNumOfSpecifiedPackageDef(char *packageName);
int UppleIsPackageDefConnected(char *packageName);
PUpplePackageDef UppleFindPackageDef(char *packageName);
int UppleFindPackageDefIndex(char *packageName);
int UppleGetMDLNameOfPackageDef(char *packageName,char *mdlName);
int UppleConnectPackageDef(char *packageName);
int UppleRemovePackageDef(char *packageName);
int UppleDisconnectPackageDef(char *packageName);
int UppleReloadPackageDef(char *packageName);
int UpplePrintSpecifiedPackageDefToFile(char *packageName,FILE *fp);
int UpplePrintPackageDefBaseInfoToFile(PUpplePackageDef ppackageDefTBL,FILE *fp);
int UpplePrintPackageDefToFile(PUpplePackageDef ppackageDefTBL,FILE *fp);
int UpplePrintPackageFldDefToFile(PUpplePackageFldDef pFldDef,FILE *fp);

PUpplePackageFldDef UppleFindPackageFldDefOfIndex(char *packageName,int index);

int UppleGetRealNumOfPackageFld(char *packageName);


#endif
             
