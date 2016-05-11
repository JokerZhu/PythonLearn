// 2006/6/23,�����˳���ָʾ����صĶ���
// �������� _bitFldLenFlag_ �궨����ص�����

#ifndef _UpplePackageDef_
#define _UpplePackageDef_

#include "uppleErrCode.h"

#define conMaxNumOfPackageDef			200

#define conMaxNumOfPackageFlds			(512+1)

#define conConfFileNameOfPackageDefGroup	"packageDefGrp.Def"

// ���� 3λ��ʾ��
// 	����λ��ʾTUpplePackageFldValueClass��ֵ
//	��1λ�ǲ���ֵʱ�Ĵ���ʽ��
//		���ֵ��0����ʾ��������ֵ�ˣ�����λ�Ͳ��ò�λ��ʽ������
//		���ֵ��1����ʾ����������ֵ����������Ҫ��λ��
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
	conAsciiFldRightNull = 7,	// ����λ�������ַ�
	conBitsFldLeftDuiQi = 8,	// �������������
	conBitsFldRightDuiQi = 9	// ���������Ҷ���
} TUpplePackageFldValueClass;

typedef enum
{
	conFldLenUseAscii = 0,
	conFldLenUseLeftDuiQi,
	conFldLenUseRightDuiQi
} TUpplePackageFldLenPresentation;

typedef struct
{
	int				index;		// �����
	int				lenOfLenFlag;	// �䳤�ֶγ���ָʾλ�ĳ��ȣ�
							// Ϊ0��ʾ�����ֶ�
	int				lenOfValue;	// �ֶεĳ��ȣ����ڱ䳤�ֶΣ�����󳤶ȣ�
							// ���ڶ����ֶΣ��ֶεĳ���
							// Ϊ0��ʾ���ֶβ�����
	TUpplePackageFldValueClass	valueType;
	char				remark[40+1];	// ��˵��
} TUpplePackageFldDef;
typedef TUpplePackageFldDef		*PUpplePackageFldDef;

typedef struct
{
	char				name[40+1];		// ������
	int				id;			// �ڱ��ĸ�ʽ��������е�������
	int				maxNum;			// �����Ŀ
	TUpplePackageFldDef		fldDefTBL[conMaxNumOfPackageFlds];		
	TUpplePackageFldLenPresentation	fldLenPresentation;	// �趨����ָʾ��ı�ʾ����
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
             
