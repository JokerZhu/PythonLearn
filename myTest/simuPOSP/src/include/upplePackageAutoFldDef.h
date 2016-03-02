#ifndef _upplePackageAutoFldDef_
#define _upplePackageAutoFldDef_

#include "uppleErrCode.h"

typedef enum
{
	conFldValueSetMethod_NotAutoSet	= 1,		// 不自动赋值，必须由程序赋值
	conFldValueSetMethod_ReadRPUB = 1000,		// 取交易公共域
	conFldValueSetMethod_DefaultValue = 1001,	// 取缺省值
	conFldValueSetMethod_NullValue = 1002,		// 空值
	conFldValueSetMethod_ReadDb	= 1003,		//从数据库取值
	conFldValueSetMethod_ReadRPBite = 1004, //取交易公共域-bite型
	conFldValueSetMethod_Read8583Fld = 1101,	// 直接取指定的8583域
	conFldValueSetMethod_ReadTermFld = 1102,	// 直接取指定的Term域
	conFldValueSetMethod_ReadHostFld = 1103,	// 直接取指定的Host域
	conFldValueSetMethod_ReadYLFld = 1104,		// 直接取指定的YL域
	conFldValueSetMethod_ReadSPFld = 1105,		// 直接取指定的SP域
	conFldValueSetMethod_ReadOriPyrFld = 2104,	// 直接取指定的原付款方域
	conFldValueSetMethod_ReadOriPyeFld = 2105,	// 直接取指定的原收款方域
	conFldValueSetMethod_ReadRECFld = 4101,		// 直接取REC值
	conFldValueSetMethod_DefaultFun = 10001,	// 使用预定义的函数赋值
	conFldValueSetMethod_UserFun = 10002,		// 使用用户定义的函数值赋值
} TUpplePackageAutoFldValueSetMethod;

// 定义数据单元的数据类型
typedef enum
{
	conPackageAutoFldType_Bits = 0,		// 二进制数
	conPackageAutoFldType_String = 1,	// 字符串
	conPackageAutoFldType_Long = 2,		// 长整型
	conPackageAutoFldType_Int = 3,		// 整型
	conPackageAutoFldType_Date = 4,		// 日期
	conPackageAutoFldType_Double = 5,	// 有二个小数位的双精度数
	conPackageAutoFldType_Numberic = 6,	// 数字类型
	conPackageAutoFldType_Time = 7,		// 时间类型
	conPackageAutoFldType_Array = 8,	// 数组类型
} TUpplePackageAutoFldType;

// 定义一个packageFld	
typedef struct
{
	char					name[40+1];		// 域的名称,域名惟一，一个域名可以对应多个索引号
	int					index;			// 域的索引号
	TUpplePackageAutoFldType		type;			// 域的类型
	int					maxLen;			// 数据的最大长度
	TUpplePackageAutoFldValueSetMethod	method;			// 域的赋值方法
	char					var[128+1];		// 域的赋值方法使用的值
} TUpplePackageAutoFldDef;
typedef TUpplePackageAutoFldDef		*PUpplePackageAutoFldDef;

	
int UppleIsValidPackageAutoFldType(TUpplePackageAutoFldType type);

int UpplePrintPackageAutoFldDefToFile(PUpplePackageAutoFldDef pdef,FILE *fp);

TUpplePackageAutoFldType UppleConvertDefFlagIntoPackageAutoFldType(char *name);

TUpplePackageAutoFldValueSetMethod UppleConvertDefFlagIntoPackageAutoFldValueSetMethod(char *name);

#endif
