#ifndef _upplePackageAutoFldDef_
#define _upplePackageAutoFldDef_

#include "uppleErrCode.h"

typedef enum
{
	conFldValueSetMethod_NotAutoSet	= 1,		// ���Զ���ֵ�������ɳ���ֵ
	conFldValueSetMethod_ReadRPUB = 1000,		// ȡ���׹�����
	conFldValueSetMethod_DefaultValue = 1001,	// ȡȱʡֵ
	conFldValueSetMethod_NullValue = 1002,		// ��ֵ
	conFldValueSetMethod_ReadDb	= 1003,		//�����ݿ�ȡֵ
	conFldValueSetMethod_ReadRPBite = 1004, //ȡ���׹�����-bite��
	conFldValueSetMethod_Read8583Fld = 1101,	// ֱ��ȡָ����8583��
	conFldValueSetMethod_ReadTermFld = 1102,	// ֱ��ȡָ����Term��
	conFldValueSetMethod_ReadHostFld = 1103,	// ֱ��ȡָ����Host��
	conFldValueSetMethod_ReadYLFld = 1104,		// ֱ��ȡָ����YL��
	conFldValueSetMethod_ReadSPFld = 1105,		// ֱ��ȡָ����SP��
	conFldValueSetMethod_ReadOriPyrFld = 2104,	// ֱ��ȡָ����ԭ�����
	conFldValueSetMethod_ReadOriPyeFld = 2105,	// ֱ��ȡָ����ԭ�տ��
	conFldValueSetMethod_ReadRECFld = 4101,		// ֱ��ȡRECֵ
	conFldValueSetMethod_DefaultFun = 10001,	// ʹ��Ԥ����ĺ�����ֵ
	conFldValueSetMethod_UserFun = 10002,		// ʹ���û�����ĺ���ֵ��ֵ
} TUpplePackageAutoFldValueSetMethod;

// �������ݵ�Ԫ����������
typedef enum
{
	conPackageAutoFldType_Bits = 0,		// ��������
	conPackageAutoFldType_String = 1,	// �ַ���
	conPackageAutoFldType_Long = 2,		// ������
	conPackageAutoFldType_Int = 3,		// ����
	conPackageAutoFldType_Date = 4,		// ����
	conPackageAutoFldType_Double = 5,	// �ж���С��λ��˫������
	conPackageAutoFldType_Numberic = 6,	// ��������
	conPackageAutoFldType_Time = 7,		// ʱ������
	conPackageAutoFldType_Array = 8,	// ��������
} TUpplePackageAutoFldType;

// ����һ��packageFld	
typedef struct
{
	char					name[40+1];		// �������,����Ωһ��һ���������Զ�Ӧ���������
	int					index;			// ���������
	TUpplePackageAutoFldType		type;			// �������
	int					maxLen;			// ���ݵ���󳤶�
	TUpplePackageAutoFldValueSetMethod	method;			// ��ĸ�ֵ����
	char					var[128+1];		// ��ĸ�ֵ����ʹ�õ�ֵ
} TUpplePackageAutoFldDef;
typedef TUpplePackageAutoFldDef		*PUpplePackageAutoFldDef;

	
int UppleIsValidPackageAutoFldType(TUpplePackageAutoFldType type);

int UpplePrintPackageAutoFldDefToFile(PUpplePackageAutoFldDef pdef,FILE *fp);

TUpplePackageAutoFldType UppleConvertDefFlagIntoPackageAutoFldType(char *name);

TUpplePackageAutoFldValueSetMethod UppleConvertDefFlagIntoPackageAutoFldValueSetMethod(char *name);

#endif
