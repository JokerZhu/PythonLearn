#ifndef _UppleErrCode_
#define _UppleErrCode_

#include <errno.h>

// 错误码偏移量定义
#define errCodeOffsetOfCurrentMDL		-11000	// 当前模块
#define errCodeOffsetOfTaskMDL			-13000	// 任务模块
#define errCodeOffsetOfEnviMDL			-14000	// 环境变量模块
#define errCodeOffsetOfLogMDL			-16000	// 日志模块
#define errCodeOffsetOfSharedMemoryMDL		-17000	// 共享内存模块
#define errCodeOffsetOfMsgBufMDL		-18000	// 消息交换区模块
#define errCodeOffsetOfOpertatorMDL		-19000	// 操作员模块
#define errCodeOffsetOfRECMDL			-20000	// REC模块
#define errCodeOffsetOfSckCommMDL		-21000	// 通讯模块
#define errCodeOffsetOfSocketMDL		-22000	// Socket模块
#define errCodeOffsetOfTransClassDefMDL		-24000	// 交易定易模块
#define errCodeOffsetOfPackageDefMDL		-25000	// 报文模块
#define errCodeOffsetOfISO8583MDL		-26000	// 8583模块
#define errCodeOffsetOfDatabase			-32000	// 数据库的错误码
#define errCodeOffsetOfHsmCmdMDL		-9000	// 密码机指令模块
#define errCodeOffsetOfCommConfMDL		-7000	// 外部通讯模块
#define errCodeOffsetOfKeyCacheMDL		-6000	// 密钥缓冲模块
#define errCodeOffsetOfTransSpierBufMDL		-4000	// 交易监控模块
#define errCodeOffsetOfErrCodeMDL		-2000	// 错误代码模块
#define errCodeOffsetOfHsmReturnCodeMDL		-1000	// 密码机返回码

// 以下是共用错误码
#define errCodeUseOSErrCode			(0-abs(errno))	// 使用操作系统的错误码
#define errCodeParameter			-10001	// 参数错误
#define errCodeSharedMemoryModule		-10002	// 共享内存错
#define errCodeCreateTaskInstance		-10003	// 创建任务出错
#define errCodeSmallBuffer			-10004	// 太小的缓冲区
#define errCodeInvalidIPAddr			-10005	// 非法的IP地址
#define errCodeCallThirdPartyFunction		-10006	// 调用第三方函数出错
#define errCodeTooShortLength			-10007	// 长度太小
#define errCodeUserSelectExit			-10008	// 用户选择了退出
#define errCodeDefaultErrCode			-10009	// 缺省错误码
#define errCodeIsRemarkLine			-10010	// 是注释行
#define errCodeNoneOperationDefined		-10011	// 没定义操作命令
#define errCodeNoneVarDefined			-10012	// 没定义参数
#define errCodeTimeout				-10013	// 超时
#define errCodeOutRange				-10014	// 超界
#define errCodeUserForgetSetErrCode		-10015	// 程序员忘记了置错误码
#define errCodeUserRequestHelpInfo		-10016	// 用户要求帮助信息 2007/11/30增加
#define errCodeTimerNotStart			-10017  // 计时没有开始 2008/6/20增加
#define errCodeLiscenceCodeInvalid		-10018  // 授权码错，2008/7/16增加
#define errCodeTcpipSvrNotDefined		-10019  // tcpipSvr没有定义
#define errCodeFileEnd				-10020  // 文件结束了
#define errCodeRequestAndResponseNotSame	-10021  // 请求与响应不匹配
#define errCodeKeyWordAlreadyExists		-10022  // 关键字已存在
#define errCodeKeyWordNotExists			-10023  // 关键字不存在
#define errCodeKeyWordIsMyself			-10024  // 关键字是本身
#define errCodeLockKeyWord			-10025  // 锁关键字出错
#define errCodeVarNameNotCorrect		-10026  // 变量名称错
#define errCodeTestFileContentError		-10027  // 测试数据文件内容有错
#define errCodeSckConnNoData			-10028  // socket连接上没有传送数据  
#define errCodeMarcoDefNameNotDefined		-10029  // 宏定义名称没定义
#define errCodeMarcoDefValueNotDefined		-10030  // 宏定义值没定义
#define errCodeDefaultValueTooLong		-10031  // 缺省值太长
#define errCodeFileAlreadyExists		-10032  // 文件已存在
#define errCodeNullPointer			-10033  // 空指针
#define errCodeNullRecPointer			-10034  // 空记录指针
#define errCodeNullRecStrPointer		-10035  // 空记录串指针
#define errCodeTableNameReserved		-10036  // 表名是保留表名
#define errCodeTooManyLoopTimes			-10037  // 循环太多次了

// 错误代码模块
#define errCodeOffsetOfErrCodeMDL_CliErrCodeMustSpecified		(errCodeOffsetOfErrCodeMDL-1)	// 客户端错误代码必须指明
#define errCodeOffsetOfErrCodeMDL_CodeNotDefined			(errCodeOffsetOfErrCodeMDL-2)	// 错误码没定义
#define errCodeOffsetOfErrCodeMDL_CodeAlreadyDefined			(errCodeOffsetOfErrCodeMDL-3)	// 错误码已经定义

// offset = -9000
// 密码机指令错误
#define errCodeHsmCmdMDL_ReturnLen			(errCodeOffsetOfHsmCmdMDL-1)	// 返回长度错	
#define errCodeHsmCmdMDL_NoMK				(errCodeOffsetOfHsmCmdMDL-2)	// 没有主密钥
#define errCodeHsmCmdMDL_WrongTerminalKey		(errCodeOffsetOfHsmCmdMDL-3)	// 错误的终端密钥
#define errCodeHsmCmdMDL_WKParity			(errCodeOffsetOfHsmCmdMDL-4)	// 密钥的奇偶校验错
#define errCodeHsmCmdMDL_InvalidTerminalKeyIndex	(errCodeOffsetOfHsmCmdMDL-5)	// 非法的终端密钥索引
#define errCodeHsmCmdMDL_InvalidBmkIndex		(errCodeOffsetOfHsmCmdMDL-6)	// 非法的BMK索引
#define errCodeHsmCmdMDL_MacOrPinCheckFailure		(errCodeOffsetOfHsmCmdMDL-7)	// MAC或者密钥校验错
#define errCodeHsmCmdMDL_FirstPikParity			(errCodeOffsetOfHsmCmdMDL-8)	// 第一个PIK奇偶校验错
#define errCodeHsmCmdMDL_SecondPikParity		(errCodeOffsetOfHsmCmdMDL-9)	// 第二个PIK奇偶校验错
#define errCodeHsmCmdMDL_InvalidPinType			(errCodeOffsetOfHsmCmdMDL-10)	// 非法的PIN类型
#define errCodeHsmCmdMDL_MacDataLength			(errCodeOffsetOfHsmCmdMDL-11)	// MAC数据的长度错
#define errCodeHsmCmdMDL_Mak1Parity			(errCodeOffsetOfHsmCmdMDL-13)	// 第一个ZAK奇偶校验错
#define errCodeHsmCmdMDL_Mak2Parity			(errCodeOffsetOfHsmCmdMDL-14)	// 第二个ZAK奇偶校验错
#define errCodeHsmCmdMDL_PinType			(errCodeOffsetOfHsmCmdMDL-15)	// PIN的类型错
#define errCodeHsmCmdMDL_InvalidCmd			(errCodeOffsetOfHsmCmdMDL-16)	// 非法的指令
#define errCodeHsmCmdMDL_CmdTooShort			(errCodeOffsetOfHsmCmdMDL-17)	// 指令太短
#define errCodeHsmCmdMDL_CmdTooLong			(errCodeOffsetOfHsmCmdMDL-18)	// 找令太长
#define errCodeHsmCmdMDL_CommError			(errCodeOffsetOfHsmCmdMDL-19)	// 通讯出错
#define errCodeHsmCmdMDL_InvalidChar			(errCodeOffsetOfHsmCmdMDL-20)	// 非法字符
#define errCodeHsmCmdMDL_Timeout			(errCodeOffsetOfHsmCmdMDL-21)	// 超时
#define errCodeHsmCmdMDL_NoBmkOrBmkParity		(errCodeOffsetOfHsmCmdMDL-22)	// 没有BMK或BMK奇偶校验错
#define errCodeHsmCmdMDL_Unknown			(errCodeOffsetOfHsmCmdMDL-23)	// 未知错误
#define errCodeHsmCmdMDL_ErrCodeNotSuccess		(errCodeOffsetOfHsmCmdMDL-24)	// 返回的错误码不是00
#define errCodeHsmCmdMDL_VerifyFailure			(errCodeOffsetOfHsmCmdMDL-25)	// 验证失败
#define errCodeHsmCmdMDL_KeyLen				(errCodeOffsetOfHsmCmdMDL-26)	// 密钥长度
#define errCodeHsmCmdMDL_InvalidKeyType			(errCodeOffsetOfHsmCmdMDL-27)	// 密钥类型
#define errCodeHsmCmdMDL_KeyLenFlag			(errCodeOffsetOfHsmCmdMDL-28)	// 密钥长度标识
#define errCodeHsmCmdMDL_HsmNotAvailable		(errCodeOffsetOfHsmCmdMDL-29)	// 密码机不可用
#define errCodeHsmCmdMDL_PinByLmk0203			(errCodeOffsetOfHsmCmdMDL-30)	// LMK02-03加密的PIN错
#define errCodeHsmCmdMDL_InvalidPinInputData		(errCodeOffsetOfHsmCmdMDL-31)	// PIN输入数据
#define errCodeHsmCmdMDL_PrinterNotReady		(errCodeOffsetOfHsmCmdMDL-32)	// 打印机没准备好
#define errCodeHsmCmdMDL_HSMNotAuthorized		(errCodeOffsetOfHsmCmdMDL-33)	// 加密机没有授权
#define errCodeHsmCmdMDL_FormatNotLoaded		(errCodeOffsetOfHsmCmdMDL-34)	// 格式未加载
#define errCodeHsmCmdMDL_DieboldTableInvalid		(errCodeOffsetOfHsmCmdMDL-35)	// DieboldTable
#define errCodeHsmCmdMDL_PinBlock			(errCodeOffsetOfHsmCmdMDL-36)	// PinBlock
#define errCodeHsmCmdMDL_InvalidIndex			(errCodeOffsetOfHsmCmdMDL-37)	// 非法索引
#define errCodeHsmCmdMDL_InvalidAcc			(errCodeOffsetOfHsmCmdMDL-38)	// 非法账号
#define errCodeHsmCmdMDL_PinBlockFormat			(errCodeOffsetOfHsmCmdMDL-39)	// PinBlock格式
#define errCodeHsmCmdMDL_PinData			(errCodeOffsetOfHsmCmdMDL-40)	// Pin数据
#define errCodeHsmCmdMDL_DecimalizationTable		(errCodeOffsetOfHsmCmdMDL-41)	// DecimalizationTable
#define errCodeHsmCmdMDL_KeyScheme			(errCodeOffsetOfHsmCmdMDL-42)	// 密钥类型
#define errCodeHsmCmdMDL_IncompitableKeyLen		(errCodeOffsetOfHsmCmdMDL-43)	// 不兼容密钥长度
#define errCodeHsmCmdMDL_KeyType			(errCodeOffsetOfHsmCmdMDL-44)	// 密钥类型
#define errCodeHsmCmdMDL_KeyNotPermitted		(errCodeOffsetOfHsmCmdMDL-45)	// 不允许使用密钥
#define errCodeHsmCmdMDL_ReferenceNumber		(errCodeOffsetOfHsmCmdMDL-46)	// ReferenceNumber
#define errCodeHsmCmdMDL_InsufficientSolicitation	(errCodeOffsetOfHsmCmdMDL-47)	// 不足的Solicitation
#define errCodeHsmCmdMDL_LMKKeyChangeCorrupted		(errCodeOffsetOfHsmCmdMDL-48)	// 
#define errCodeHsmCmdMDL_DesFailure			(errCodeOffsetOfHsmCmdMDL-49)	// DES出错
#define errCodeHsmCmdMDL_DataLength			(errCodeOffsetOfHsmCmdMDL-50)	// 数据长度
#define errCodeHsmCmdMDL_LRCError			(errCodeOffsetOfHsmCmdMDL-51)	// LRC
#define errCodeHsmCmdMDL_InternalCommand		(errCodeOffsetOfHsmCmdMDL-52)	// InternalCommand
#define errCodeHsmCmdMDL_CmdResTooShort			(errCodeOffsetOfHsmCmdMDL-53)	// 指令响应太短
#define errCodeHsmCmdMDL_CheckValue			(errCodeOffsetOfHsmCmdMDL-54)	// 校验值错误
#define errCodeHsmCmdMDL_InvalidKeyLength		(errCodeOffsetOfHsmCmdMDL-55)	// 非法密钥长度
#define errCodeHsmCmdMDL_InvalidPinLength		(errCodeOffsetOfHsmCmdMDL-56)	// 非法PIN长度
#define errCodeHsmCmdMDL_InvalidAccNoLength		(errCodeOffsetOfHsmCmdMDL-57)	// 非法账号长度
#define errCodeHsmCmdMDL_InvalidStorageIndex		(errCodeOffsetOfHsmReturnCodeMDL-21)	// 非法存储索引	// 21
#define errCodeHsmCmdMDL_InvalidInputData		(errCodeOffsetOfHsmReturnCodeMDL-15)	// 输入数据错	//15
#define errCodeHsmCmdMDL_LmkError			(errCodeOffsetOfHsmReturnCodeMDL-13)	// LMK错误	// 13
#define errCodeHsmCmdMDL_NoKeyLoadsInStorage		(errCodeOffsetOfHsmReturnCodeMDL-12)	// 没有密钥加载到用户存储区	//12
#define errCodeHsmCmdMDL_ZmkParity			(errCodeOffsetOfHsmReturnCodeMDL-10)	// ZMK奇偶校验错	//10

// 外部通讯模块
// offset = -7000
#define errCodeCommConfMDL_CommConfNotDefined		(errCodeOffsetOfCommConfMDL-1)        // 通讯配置未定义
#define errCodeCommConfMDL_AddCommConf			(errCodeOffsetOfCommConfMDL-2)        // 增加指定的通讯配置
#define errCodeCommConfMDL_CommDefTBLConnected		(errCodeOffsetOfCommConfMDL-3)        // 连接通讯定义出错
#define errCodeCommConfMDL_CommRemarkNotDefined		(errCodeOffsetOfCommConfMDL-4)        // 通讯描述没有定义
//#define errCodeCommConfMDL_CityIDNotDefined		(errCodeOffsetOfCommConfMDL-3)        // 城市码未定义

// offset = -26000
// 8583 模块错误
#define errCodeISO8583MDL_MTINotDefined				(errCodeOffsetOfISO8583MDL-1)	// MTI未定义
#define errCodeISO8583MDL_8583PackageClassTBLConnected		(errCodeOffsetOfISO8583MDL-2)	// 连接8583报文类定义出错
#define errCodeISO8583MDL_8583TransDefTBLConnected		(errCodeOffsetOfISO8583MDL-3)	// 连接8583交易定义出错
#define errCodeISO8583MDL_TransNotDefined			(errCodeOffsetOfISO8583MDL-4)	// 交易未定义
#define errCodeISO8583MDL_YLMngTransDefTBLConnected		(errCodeOffsetOfISO8583MDL-5)	// 连接银联管理类交易表错
#define errCodeISO8583MDL_YLMngTransNotDefined			(errCodeOffsetOfISO8583MDL-6)	// 银联管理交易未定义
#define errCodeISO8583MDL_8583Fld90Length			(errCodeOffsetOfISO8583MDL-7)	// 90域长度错误
#define errCodeISO8583MDL_FldNoValue				(errCodeOffsetOfISO8583MDL-8)	// 域无值
#define errCodeISO8583MDL_SPMngTransDefTBLConnected		(errCodeOffsetOfISO8583MDL-9)	// 连接SP管理类交易表错
#define errCodeISO8583MDL_SPMngTransNotDefined			(errCodeOffsetOfISO8583MDL-10)	// SP管理交易未定义
#define errCodeISO8583MDL_8583Fld61Length			(errCodeOffsetOfISO8583MDL-11)	// 61域长度错误

// offset = -25000
// 报文模块错误
#define errCodePackageDefMDL_MaxNumOfPackageFldsNotDefined	(errCodeOffsetOfPackageDefMDL-1)	// 报文最大域数量未定义
#define errCodePackageDefMDL_PackageDefGroupIsFull		(errCodeOffsetOfPackageDefMDL-2)	// 报文定义组满了
#define errCodePackageDefMDL_PackageDefNotDefined		(errCodeOffsetOfPackageDefMDL-3)	// 报文定义未定义
#define errCodePackageDefMDL_IDOfPackageNotDefined		(errCodeOffsetOfPackageDefMDL-4)	// 报文的标识未定义
#define errCodePackageDefMDL_InvalidIDOfPackage			(errCodeOffsetOfPackageDefMDL-5)	// 非法的报文标识
#define errCodePackageDefMDL_PackageOfSameID			(errCodeOffsetOfPackageDefMDL-6)	// 报文标识重复
#define errCodePackageDefMDL_InvalidMaxFldNum			(errCodeOffsetOfPackageDefMDL-7)	// 非法的最大域数目
#define errCodePackageDefMDL_PackageTypeNotDefined		(errCodeOffsetOfPackageDefMDL-8)	// 报文类型未定义
#define errCodePackageDefMDL_InvalidPackageType			(errCodeOffsetOfPackageDefMDL-9)	// 非法的报文类型
#define errCodePackageDefMDL_InvalidPrimaryKey			(errCodeOffsetOfPackageDefMDL-10)	// 非法的主键
#define errCodePackageDefMDL_FldNotDefined			(errCodeOffsetOfPackageDefMDL-11)	// 域未定义
#define errCodePackageDefMDL_PackageDefOfSpecifiedIDDefined	(errCodeOffsetOfPackageDefMDL-12)	// 指定标识的报文定义错
#define errCodePackageDefMDL_PackFldIsNull			(errCodeOffsetOfPackageDefMDL-13)	// 报文域满了
#define errCodePackageDefMDL_PackFldLength			(errCodeOffsetOfPackageDefMDL-14)	// 报文域长度错
#define errCodePackageDefMDL_FldNotDefinedInBitMap		(errCodeOffsetOfPackageDefMDL-15)	// 位图中未定义域
#define errCodePackageDefMDL_BitMapHaveValueForNullValueFld	(errCodeOffsetOfPackageDefMDL-16)	// 位图中定义了值为空的域
#define errCodePackageDefMDL_PackageNotConnected		(errCodeOffsetOfPackageDefMDL-17)	// 报文未连接
#define errCodePackageDefMDL_InvalidFldIndex			(errCodeOffsetOfPackageDefMDL-18)	// 非法的域索引
#define errCodePackageDefMDL_BitMapFldCannotBeSet		(errCodeOffsetOfPackageDefMDL-19)	// 位图域不能设置
#define errCodePackageDefMDL_InvalidPackageIndex		(errCodeOffsetOfPackageDefMDL-20)	// 非法的报文索引
#define errCodePackageDefMDL_TwoPackageOfSamePackageIndex	(errCodeOffsetOfPackageDefMDL-21)	// 两个报文具有相同的报文索引
#define errCodePackageDefMDL_ConnectPackage			(errCodeOffsetOfPackageDefMDL-22)	// 连接报文出错
#define errCodePackageDefMDL_PackExchDefTBLOfSpecifiedIDDefined	(errCodeOffsetOfPackageDefMDL-23)	// 指定标识的报文交换定义表定义错
#define errCodePackageDefMDL_IDOfPackExchNotDefined		(errCodeOffsetOfPackageDefMDL-24)	// 指定的报文交换定义
#define errCodePackageDefMDL_InvalidIDOfPackExch		(errCodeOffsetOfPackageDefMDL-25)	// 非法的报文交换定义标识
#define errCodePackageDefMDL_PackExchDefTBLNotDefined		(errCodeOffsetOfPackageDefMDL-26)	// 报文交换定义表未定义
#define errCodePackageDefMDL_PackExchDefOfSameID		(errCodeOffsetOfPackageDefMDL-27)	// 报文交换定义有相同的标识
#define errCodePackageDefMDL_NotBitMapFld			(errCodeOffsetOfPackageDefMDL-28)	// 不是位图域
#define errCodePackageDefMDL_TwoMuchBitMapLevel			(errCodeOffsetOfPackageDefMDL-29)	// ？
#define errCodePackageDefMDL_IsBitMapFld			(errCodeOffsetOfPackageDefMDL-30)	// 是位图域

// 交易类型错误
// offset = -24000
#define errCodeTransClassDefMDL_MaxNumOfTransClassDefNotDefined		(errCodeOffsetOfTransClassDefMDL-1)	// 最大的交易类型数未定义
#define errCodeTransClassDefMDL_SpecifiedTransClassDefNotDefined	(errCodeOffsetOfTransClassDefMDL-2)	// 指定的交易类型未定义
#define errCodeTransClassDefMDL_TransDefNotDefined			(errCodeOffsetOfTransClassDefMDL-3)	// 交易未定义

// 任务模块错误
// offset = -13000
#define errCodeTaskMDL_CreateTaskInstance	(errCodeOffsetOfTaskMDL-1)	// 创建任务实例错
#define errCodeTaskMDL_ConfFile			(errCodeOffsetOfTaskMDL-2)	// 任务配置文件错
#define errCodeTaskMDL_NotConnected		(errCodeOffsetOfTaskMDL-3)	// 任务模块未连接
#define errCodeTaskMDL_TaskNotExists		(errCodeOffsetOfTaskMDL-4)	// 任务不存在
#define errCodeTaskMDL_TaskClassAlreadyExists	(errCodeOffsetOfTaskMDL-5)	// 任务类型已存在
#define errCodeTaskMDL_TaskClassNameTooLong	(errCodeOffsetOfTaskMDL-6)	// 任务类型名称太长
#define errCodeTaskMDL_TaskClassStartCmdTooLong	(errCodeOffsetOfTaskMDL-7)	// 任务启动命令太长
#define errCodeTaskMDL_TaskClassLogFileNameIsNull	(errCodeOffsetOfTaskMDL-8)	// 任务日志名称为空
#define errCodeTaskMDL_TaskClassNameIsNull	(errCodeOffsetOfTaskMDL-9)	// 任务类型名称为空
#define errCodeTaskMDL_TaskClassLogFileNameTooLong	(errCodeOffsetOfTaskMDL-10)	// 任务日志名称太长
#define errCodeTaskMDL_TaskClassTableIsFull	(errCodeOffsetOfTaskMDL-11)	// 任务类型表已满
#define errCodeTaskMDL_Connect			(errCodeOffsetOfTaskMDL-12)	// 连接任务表出错
#define errCodeTaskMDL_TaskClassIsUsed		(errCodeOffsetOfTaskMDL-13)	// 任务类型正在被使用
#define errCodeTaskMDL_TaskClassNameNotDefined	(errCodeOffsetOfTaskMDL-14)	// 任务类型名称未定义
#define errCodeTaskMDL_TaskClassNotExists	(errCodeOffsetOfTaskMDL-15)	// 任务类型不存在
#define errCodeTaskMDL_TaskClassDefLineError	(errCodeOffsetOfTaskMDL-16)	// 任务类型定义行有错
#define errCodeTaskMDL_AnotherTaskOfNameExists	(errCodeOffsetOfTaskMDL-17)	// 同名的任务已经存在 2007/11/30，王纯军增加

// Socket模块错误
// offset = -22000
#define errCodeSocketMDL_Timeout		(errCodeOffsetOfSocketMDL-1)	// 超时
#define errCodeSocketMDL_ReceiveLen		(errCodeOffsetOfSocketMDL-2)	// 接收长度
#define errCodeSocketMDL_Error			(errCodeOffsetOfSocketMDL-3)	// 通讯失败

// 通讯模块错误
// offset = -21000
#define errCodeSckCommMDL_ConfFile		(errCodeOffsetOfSckCommMDL-1)	// 定义文件
#define errCodeSckCommMDL_InvalidWorkingMode	(errCodeOffsetOfSckCommMDL-2)	// 非法工作状态
#define errCodeSckCommMDL_ServerNotDefined	(errCodeOffsetOfSckCommMDL-3)	// 服务器未定义
#define errCodeSckCommMDL_ServiceFailure	(errCodeOffsetOfSckCommMDL-4)	// 服务失败
#define errCodeSckCommMDL_NoWorkingServer	(errCodeOffsetOfSckCommMDL-5)	// 不是工作着的服务器
#define errCodeSckCommMDL_InternalError		(errCodeOffsetOfSckCommMDL-6)	// 内部错误
#define errCodeSckCommMDL_Connect		(errCodeOffsetOfSckCommMDL-7)	// 连接通讯模块


// REC模块错误
// offset = -20000
#define errCodeRECMDL_ConfFile			(errCodeOffsetOfRECMDL-1)	// 配置文件出错
#define errCodeRECMDL_VarNotExists		(errCodeOffsetOfRECMDL-2)	// 变量不存在
#define errCodeRECMDL_VarType			(errCodeOffsetOfRECMDL-3)	// 变量类型错
#define errCodeRECMDL_VarValue			(errCodeOffsetOfRECMDL-4)	// 变量值有问题
#define errCodeRECMDL_Connect			(errCodeOffsetOfRECMDL-5)	// 连接变量配置模块
#define errCodeRECMDL_VarAlreadyExists		(errCodeOffsetOfRECMDL-6)	// 变量已存在
#define errCodeRECMDL_VarTBLIsFull		(errCodeOffsetOfRECMDL-7)	// 变量表已满
#define errCodeRECMDL_VarNameLength		(errCodeOffsetOfRECMDL-8)	// 变量名称长度错
#define errCodeRECMDL_VarValueTooLong		(errCodeOffsetOfRECMDL-9)	// 值太长了
#define errCodeRECMDL_RealNumError		(errCodeOffsetOfRECMDL-10)	// 变量的数目错

// 操作员模块错误
// offset = -19000
#define errCodeOperatorMDL_OperatorNum		(errCodeOffsetOfOpertatorMDL-1)	// 操作员数目错
#define errCodeOperatorMDL_OperatorNotExists	(errCodeOffsetOfOpertatorMDL-2)	// 操作员不存在
#define errCodeOperatorMDL_NotConnected		(errCodeOffsetOfOpertatorMDL-3)	// 操作员模块未连接
#define errCodeOperatorMDL_TableFull		(errCodeOffsetOfOpertatorMDL-4)	// 操作员表满
#define errCodeOperatorMDL_ReadOperator		(errCodeOffsetOfOpertatorMDL-5)	// 读操作员
#define errCodeOperatorMDL_WrongPassword	(errCodeOffsetOfOpertatorMDL-6)	// 操作员密码错
#define errCodeOperatorMDL_PasswordLocked	(errCodeOffsetOfOpertatorMDL-7)	// 操作员密码被锁住了
#define errCodeOperatorMDL_AlreadyLogon		(errCodeOffsetOfOpertatorMDL-8)	// 操作员已经登录了
#define errCodeOperatorMDL_NotLogon		(errCodeOffsetOfOpertatorMDL-9)	// 操作员还没有登录
#define errCodeOperatorMDL_TooLongTimeWithoutOperation		(errCodeOffsetOfOpertatorMDL-10)	// 操作员太长时间没有操作
#define errCodeOperatorMDL_StillLogon		(errCodeOffsetOfOpertatorMDL-11)	// 操作员在登录状态
#define errCodeOperatorMDL_CannotDeleteSelf	(errCodeOffsetOfOpertatorMDL-12)	// 不能删除自己
#define errCodeOperatorMDL_OperatorAlreadyExists	(errCodeOffsetOfOpertatorMDL-13)	// 操作员已存在


// 消息交换区错误
// offset = -18000
#define errCodeMsgBufMDL_NotConnected		(errCodeOffsetOfMsgBufMDL-1)	// 消息交换区未连接
#define errCodeMsgBufMDL_OutofRange		(errCodeOffsetOfMsgBufMDL-2)	// 超界
#define errCodeMsgBufMDL_InitDef		(errCodeOffsetOfMsgBufMDL-3)	// 初始定义错
#define errCodeMsgBufMDL_ConnectIndexTBL	(errCodeOffsetOfMsgBufMDL-4)	// 连接索引表错
#define errCodeMsgBufMDL_ProviderExit		(errCodeOffsetOfMsgBufMDL-5)	// 提供者已退出
#define errCodeMsgBufMDL_NotOriginMsg		(errCodeOffsetOfMsgBufMDL-6)	// 不是原始信息
#define errCodeMsgBufMDL_InvalidIndex		(errCodeOffsetOfMsgBufMDL-7)	// 非法的索引
#define errCodeMsgBufMDL_ReadIndex		(errCodeOffsetOfMsgBufMDL-8)	// 读取索引
#define errCodeMsgBufMDL_MsgLen			(errCodeOffsetOfMsgBufMDL-9)	// 消息长度错
#define errCodeMsgBufMDL_MsgOfTypeAndOfIndexNotExists	(errCodeOffsetOfMsgBufMDL-10)	// 具有指定类型和索引的消息不存在
#define errCodeMsgBufMDL_MsgIndexAlreadyLosted	(errCodeOffsetOfMsgBufMDL-11)	// 消息索引已经丢失

// 交易监控消息交换区错误
// offset = -4000
#define errCodeTransSpierBufMDL_NotConnected		(errCodeOffsetOfTransSpierBufMDL-1)	// 消息交换区未连接
#define errCodeTransSpierBufMDL_OutofRange		(errCodeOffsetOfTransSpierBufMDL-2)	// 超界
#define errCodeTransSpierBufMDL_InitDef			(errCodeOffsetOfTransSpierBufMDL-3)	// 初始定义错
#define errCodeTransSpierBufMDL_ConnectIndexTBL		(errCodeOffsetOfTransSpierBufMDL-4)	// 连接索引表错
#define errCodeTransSpierBufMDL_ProviderExit		(errCodeOffsetOfTransSpierBufMDL-5)	// 提供者已退出
#define errCodeTransSpierBufMDL_NotOriginMsg		(errCodeOffsetOfTransSpierBufMDL-6)	// 不是原始信息
#define errCodeTransSpierBufMDL_InvalidIndex		(errCodeOffsetOfTransSpierBufMDL-7)	// 非法的索引
#define errCodeTransSpierBufMDL_ReadIndex		(errCodeOffsetOfTransSpierBufMDL-8)	// 读取索引
#define errCodeTransSpierBufMDL_MsgLen			(errCodeOffsetOfTransSpierBufMDL-9)	// 消息长度错
#define errCodeTransSpierBufMDL_MsgOfTypeAndOfIndexNotExists	(errCodeOffsetOfTransSpierBufMDL-10)	// 具有指定类型和索引的消息不存在

// 共享内存模块错误
// offset = -17000
#define errCodeSharedMemoryMDL_MDLNotDefined	(errCodeOffsetOfSharedMemoryMDL-1)	// 共享内存模块未定义
#define errCodeSharedMemoryMDL_LockMDL		(errCodeOffsetOfSharedMemoryMDL-2)	// 锁共享内存错
#define errCodeSharedMemoryMDL_Connect		(errCodeOffsetOfSharedMemoryMDL-3)	// 连接共享内存模块

// 日志模块错误
// offset = -16000
#define errCodeLogMDL_ConfFile			(errCodeOffsetOfLogMDL-1)	// 配置文件错
#define errCodeLogMDL_LogFileAlreadyExists	(errCodeOffsetOfLogMDL-2)	// 日志已存在
#define errCodeLogMDL_LogFileTblFull		(errCodeOffsetOfLogMDL-3)	// 日志表满了
#define errCodeLogMDL_LogFileUsedByUsers	(errCodeOffsetOfLogMDL-4)	// 日志正被使用
#define errCodeLogMDL_LogFileNotExists		(errCodeOffsetOfLogMDL-5)	// 日志不存在
#define errCodeLogMDL_NotConnected		(errCodeOffsetOfLogMDL-6)	// 日志模块未连接
#define errCodeLogMDL_Connect			(errCodeOffsetOfLogMDL-7)	// 连接日志模块
#define errCodeLogMDL_LogFileNameMustSpecified	(errCodeOffsetOfLogMDL-8)	// 必须指明日志名称

// 环境变量模块错误
// offset = -14000
#define errCodeEnviMDL_VarNotExists		(errCodeOffsetOfEnviMDL-1)	// 变量不存在
#define errCodeEnviMDL_NullLine			(errCodeOffsetOfEnviMDL-2)	// 空行
#define errCodeEnviMDL_OccupiedByOther		(errCodeOffsetOfEnviMDL-3)	// 环境变量模块被别人占用了
#define errCodeEnviMDL_NoValueDefinedForVar	(errCodeOffsetOfEnviMDL-4)	// 变量值没定义
#define errCodeEnviMDL_VarAlreadyExists		(errCodeOffsetOfEnviMDL-5)	// 变量已存在
#define errCodeEnviMDL_NotRecFormatDefStr	(errCodeOffsetOfEnviMDL-6)	// 不是记录格式定义字符串
#define errCodeEnviMDL_NotRecStrTooLong		(errCodeOffsetOfEnviMDL-7)	// 记录字符串太长
#define errCodeEnviMDL_NotRecDefStr		(errCodeOffsetOfEnviMDL-8)	// 不是记录定义串

// 密钥CACHE模块错误
// offset = -6000
#define errCodeKeyCacheMDL_KeyNonExists		(errCodeOffsetOfKeyCacheMDL-1)	// 密钥不存在
#define errCodeKeyCacheMDL_KeyNum		(errCodeOffsetOfKeyCacheMDL-2)	// 密钥数量错
#define errCodeKeyCacheMDL_KeyDBFull		(errCodeOffsetOfKeyCacheMDL-3)	// 库满
#define errCodeKeyCacheMDL_KeyDBNonConnected	(errCodeOffsetOfKeyCacheMDL-4)	// 库未连接
#define errCodeKeyCacheMDL_InvalidKeyLength	(errCodeOffsetOfKeyCacheMDL-5)	// 非法密钥长度
#define errCodeKeyCacheMDL_KeyAlreadyExists	(errCodeOffsetOfKeyCacheMDL-6)	// 密钥已存在
#define errCodeKeyCacheMDL_KeyOutdate		(errCodeOffsetOfKeyCacheMDL-7)	// 密钥过期
#define errCodeKeyCacheMDL_KeyValueWithoutEnd	(errCodeOffsetOfKeyCacheMDL-8)	// 密钥值无结束符
#define errCodeKeyCacheMDL_WrongKeyName		(errCodeOffsetOfKeyCacheMDL-9)	// 密钥名称不对

// offset = -32000
// 数据库的错误码
#define errCodeDatabaseMDL_DatabaseAlreadyExist		(errCodeOffsetOfDatabase-1)	// 数据库已经存在
#define errCodeDatabaseMDL_DatabaseNotFound		(errCodeOffsetOfDatabase-2)	// 数据库没有找到
#define errCodeDatabaseMDL_TableAlreadyExist		(errCodeOffsetOfDatabase-3)	// 表已经存在
#define errCodeDatabaseMDL_TableNotFound		(errCodeOffsetOfDatabase-4)	// 表没有找到
#define errCodeDatabaseMDL_RecordNotFound		(errCodeOffsetOfDatabase-5)	// 记录没有找到
#define errCodeDatabaseMDL_RecordAlreadyExist		(errCodeOffsetOfDatabase-6)	// 记录已经存在
#define errCodeDatabaseMDL_NoData			(errCodeOffsetOfDatabase-7)	// 没有数据
#define errCodeDatabaseMDL_MoreRecordFound		(errCodeOffsetOfDatabase-8)	// 找到多条记录

#endif
