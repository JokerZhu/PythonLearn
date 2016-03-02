#ifndef _UppleErrCode_
#define _UppleErrCode_

#include <errno.h>

// ������ƫ��������
#define errCodeOffsetOfCurrentMDL		-11000	// ��ǰģ��
#define errCodeOffsetOfTaskMDL			-13000	// ����ģ��
#define errCodeOffsetOfEnviMDL			-14000	// ��������ģ��
#define errCodeOffsetOfLogMDL			-16000	// ��־ģ��
#define errCodeOffsetOfSharedMemoryMDL		-17000	// �����ڴ�ģ��
#define errCodeOffsetOfMsgBufMDL		-18000	// ��Ϣ������ģ��
#define errCodeOffsetOfOpertatorMDL		-19000	// ����Աģ��
#define errCodeOffsetOfRECMDL			-20000	// RECģ��
#define errCodeOffsetOfSckCommMDL		-21000	// ͨѶģ��
#define errCodeOffsetOfSocketMDL		-22000	// Socketģ��
#define errCodeOffsetOfTransClassDefMDL		-24000	// ���׶���ģ��
#define errCodeOffsetOfPackageDefMDL		-25000	// ����ģ��
#define errCodeOffsetOfISO8583MDL		-26000	// 8583ģ��
#define errCodeOffsetOfDatabase			-32000	// ���ݿ�Ĵ�����
#define errCodeOffsetOfHsmCmdMDL		-9000	// �����ָ��ģ��
#define errCodeOffsetOfCommConfMDL		-7000	// �ⲿͨѶģ��
#define errCodeOffsetOfKeyCacheMDL		-6000	// ��Կ����ģ��
#define errCodeOffsetOfTransSpierBufMDL		-4000	// ���׼��ģ��
#define errCodeOffsetOfErrCodeMDL		-2000	// �������ģ��
#define errCodeOffsetOfHsmReturnCodeMDL		-1000	// �����������

// �����ǹ��ô�����
#define errCodeUseOSErrCode			(0-abs(errno))	// ʹ�ò���ϵͳ�Ĵ�����
#define errCodeParameter			-10001	// ��������
#define errCodeSharedMemoryModule		-10002	// �����ڴ��
#define errCodeCreateTaskInstance		-10003	// �����������
#define errCodeSmallBuffer			-10004	// ̫С�Ļ�����
#define errCodeInvalidIPAddr			-10005	// �Ƿ���IP��ַ
#define errCodeCallThirdPartyFunction		-10006	// ���õ�������������
#define errCodeTooShortLength			-10007	// ����̫С
#define errCodeUserSelectExit			-10008	// �û�ѡ�����˳�
#define errCodeDefaultErrCode			-10009	// ȱʡ������
#define errCodeIsRemarkLine			-10010	// ��ע����
#define errCodeNoneOperationDefined		-10011	// û�����������
#define errCodeNoneVarDefined			-10012	// û�������
#define errCodeTimeout				-10013	// ��ʱ
#define errCodeOutRange				-10014	// ����
#define errCodeUserForgetSetErrCode		-10015	// ����Ա�������ô�����
#define errCodeUserRequestHelpInfo		-10016	// �û�Ҫ�������Ϣ 2007/11/30����
#define errCodeTimerNotStart			-10017  // ��ʱû�п�ʼ 2008/6/20����
#define errCodeLiscenceCodeInvalid		-10018  // ��Ȩ���2008/7/16����
#define errCodeTcpipSvrNotDefined		-10019  // tcpipSvrû�ж���
#define errCodeFileEnd				-10020  // �ļ�������
#define errCodeRequestAndResponseNotSame	-10021  // ��������Ӧ��ƥ��
#define errCodeKeyWordAlreadyExists		-10022  // �ؼ����Ѵ���
#define errCodeKeyWordNotExists			-10023  // �ؼ��ֲ�����
#define errCodeKeyWordIsMyself			-10024  // �ؼ����Ǳ���
#define errCodeLockKeyWord			-10025  // ���ؼ��ֳ���
#define errCodeVarNameNotCorrect		-10026  // �������ƴ�
#define errCodeTestFileContentError		-10027  // ���������ļ������д�
#define errCodeSckConnNoData			-10028  // socket������û�д�������  
#define errCodeMarcoDefNameNotDefined		-10029  // �궨������û����
#define errCodeMarcoDefValueNotDefined		-10030  // �궨��ֵû����
#define errCodeDefaultValueTooLong		-10031  // ȱʡֵ̫��
#define errCodeFileAlreadyExists		-10032  // �ļ��Ѵ���
#define errCodeNullPointer			-10033  // ��ָ��
#define errCodeNullRecPointer			-10034  // �ռ�¼ָ��
#define errCodeNullRecStrPointer		-10035  // �ռ�¼��ָ��
#define errCodeTableNameReserved		-10036  // �����Ǳ�������
#define errCodeTooManyLoopTimes			-10037  // ѭ��̫�����

// �������ģ��
#define errCodeOffsetOfErrCodeMDL_CliErrCodeMustSpecified		(errCodeOffsetOfErrCodeMDL-1)	// �ͻ��˴���������ָ��
#define errCodeOffsetOfErrCodeMDL_CodeNotDefined			(errCodeOffsetOfErrCodeMDL-2)	// ������û����
#define errCodeOffsetOfErrCodeMDL_CodeAlreadyDefined			(errCodeOffsetOfErrCodeMDL-3)	// �������Ѿ�����

// offset = -9000
// �����ָ�����
#define errCodeHsmCmdMDL_ReturnLen			(errCodeOffsetOfHsmCmdMDL-1)	// ���س��ȴ�	
#define errCodeHsmCmdMDL_NoMK				(errCodeOffsetOfHsmCmdMDL-2)	// û������Կ
#define errCodeHsmCmdMDL_WrongTerminalKey		(errCodeOffsetOfHsmCmdMDL-3)	// ������ն���Կ
#define errCodeHsmCmdMDL_WKParity			(errCodeOffsetOfHsmCmdMDL-4)	// ��Կ����żУ���
#define errCodeHsmCmdMDL_InvalidTerminalKeyIndex	(errCodeOffsetOfHsmCmdMDL-5)	// �Ƿ����ն���Կ����
#define errCodeHsmCmdMDL_InvalidBmkIndex		(errCodeOffsetOfHsmCmdMDL-6)	// �Ƿ���BMK����
#define errCodeHsmCmdMDL_MacOrPinCheckFailure		(errCodeOffsetOfHsmCmdMDL-7)	// MAC������ԿУ���
#define errCodeHsmCmdMDL_FirstPikParity			(errCodeOffsetOfHsmCmdMDL-8)	// ��һ��PIK��żУ���
#define errCodeHsmCmdMDL_SecondPikParity		(errCodeOffsetOfHsmCmdMDL-9)	// �ڶ���PIK��żУ���
#define errCodeHsmCmdMDL_InvalidPinType			(errCodeOffsetOfHsmCmdMDL-10)	// �Ƿ���PIN����
#define errCodeHsmCmdMDL_MacDataLength			(errCodeOffsetOfHsmCmdMDL-11)	// MAC���ݵĳ��ȴ�
#define errCodeHsmCmdMDL_Mak1Parity			(errCodeOffsetOfHsmCmdMDL-13)	// ��һ��ZAK��żУ���
#define errCodeHsmCmdMDL_Mak2Parity			(errCodeOffsetOfHsmCmdMDL-14)	// �ڶ���ZAK��żУ���
#define errCodeHsmCmdMDL_PinType			(errCodeOffsetOfHsmCmdMDL-15)	// PIN�����ʹ�
#define errCodeHsmCmdMDL_InvalidCmd			(errCodeOffsetOfHsmCmdMDL-16)	// �Ƿ���ָ��
#define errCodeHsmCmdMDL_CmdTooShort			(errCodeOffsetOfHsmCmdMDL-17)	// ָ��̫��
#define errCodeHsmCmdMDL_CmdTooLong			(errCodeOffsetOfHsmCmdMDL-18)	// ����̫��
#define errCodeHsmCmdMDL_CommError			(errCodeOffsetOfHsmCmdMDL-19)	// ͨѶ����
#define errCodeHsmCmdMDL_InvalidChar			(errCodeOffsetOfHsmCmdMDL-20)	// �Ƿ��ַ�
#define errCodeHsmCmdMDL_Timeout			(errCodeOffsetOfHsmCmdMDL-21)	// ��ʱ
#define errCodeHsmCmdMDL_NoBmkOrBmkParity		(errCodeOffsetOfHsmCmdMDL-22)	// û��BMK��BMK��żУ���
#define errCodeHsmCmdMDL_Unknown			(errCodeOffsetOfHsmCmdMDL-23)	// δ֪����
#define errCodeHsmCmdMDL_ErrCodeNotSuccess		(errCodeOffsetOfHsmCmdMDL-24)	// ���صĴ����벻��00
#define errCodeHsmCmdMDL_VerifyFailure			(errCodeOffsetOfHsmCmdMDL-25)	// ��֤ʧ��
#define errCodeHsmCmdMDL_KeyLen				(errCodeOffsetOfHsmCmdMDL-26)	// ��Կ����
#define errCodeHsmCmdMDL_InvalidKeyType			(errCodeOffsetOfHsmCmdMDL-27)	// ��Կ����
#define errCodeHsmCmdMDL_KeyLenFlag			(errCodeOffsetOfHsmCmdMDL-28)	// ��Կ���ȱ�ʶ
#define errCodeHsmCmdMDL_HsmNotAvailable		(errCodeOffsetOfHsmCmdMDL-29)	// �����������
#define errCodeHsmCmdMDL_PinByLmk0203			(errCodeOffsetOfHsmCmdMDL-30)	// LMK02-03���ܵ�PIN��
#define errCodeHsmCmdMDL_InvalidPinInputData		(errCodeOffsetOfHsmCmdMDL-31)	// PIN��������
#define errCodeHsmCmdMDL_PrinterNotReady		(errCodeOffsetOfHsmCmdMDL-32)	// ��ӡ��û׼����
#define errCodeHsmCmdMDL_HSMNotAuthorized		(errCodeOffsetOfHsmCmdMDL-33)	// ���ܻ�û����Ȩ
#define errCodeHsmCmdMDL_FormatNotLoaded		(errCodeOffsetOfHsmCmdMDL-34)	// ��ʽδ����
#define errCodeHsmCmdMDL_DieboldTableInvalid		(errCodeOffsetOfHsmCmdMDL-35)	// DieboldTable
#define errCodeHsmCmdMDL_PinBlock			(errCodeOffsetOfHsmCmdMDL-36)	// PinBlock
#define errCodeHsmCmdMDL_InvalidIndex			(errCodeOffsetOfHsmCmdMDL-37)	// �Ƿ�����
#define errCodeHsmCmdMDL_InvalidAcc			(errCodeOffsetOfHsmCmdMDL-38)	// �Ƿ��˺�
#define errCodeHsmCmdMDL_PinBlockFormat			(errCodeOffsetOfHsmCmdMDL-39)	// PinBlock��ʽ
#define errCodeHsmCmdMDL_PinData			(errCodeOffsetOfHsmCmdMDL-40)	// Pin����
#define errCodeHsmCmdMDL_DecimalizationTable		(errCodeOffsetOfHsmCmdMDL-41)	// DecimalizationTable
#define errCodeHsmCmdMDL_KeyScheme			(errCodeOffsetOfHsmCmdMDL-42)	// ��Կ����
#define errCodeHsmCmdMDL_IncompitableKeyLen		(errCodeOffsetOfHsmCmdMDL-43)	// ��������Կ����
#define errCodeHsmCmdMDL_KeyType			(errCodeOffsetOfHsmCmdMDL-44)	// ��Կ����
#define errCodeHsmCmdMDL_KeyNotPermitted		(errCodeOffsetOfHsmCmdMDL-45)	// ������ʹ����Կ
#define errCodeHsmCmdMDL_ReferenceNumber		(errCodeOffsetOfHsmCmdMDL-46)	// ReferenceNumber
#define errCodeHsmCmdMDL_InsufficientSolicitation	(errCodeOffsetOfHsmCmdMDL-47)	// �����Solicitation
#define errCodeHsmCmdMDL_LMKKeyChangeCorrupted		(errCodeOffsetOfHsmCmdMDL-48)	// 
#define errCodeHsmCmdMDL_DesFailure			(errCodeOffsetOfHsmCmdMDL-49)	// DES����
#define errCodeHsmCmdMDL_DataLength			(errCodeOffsetOfHsmCmdMDL-50)	// ���ݳ���
#define errCodeHsmCmdMDL_LRCError			(errCodeOffsetOfHsmCmdMDL-51)	// LRC
#define errCodeHsmCmdMDL_InternalCommand		(errCodeOffsetOfHsmCmdMDL-52)	// InternalCommand
#define errCodeHsmCmdMDL_CmdResTooShort			(errCodeOffsetOfHsmCmdMDL-53)	// ָ����Ӧ̫��
#define errCodeHsmCmdMDL_CheckValue			(errCodeOffsetOfHsmCmdMDL-54)	// У��ֵ����
#define errCodeHsmCmdMDL_InvalidKeyLength		(errCodeOffsetOfHsmCmdMDL-55)	// �Ƿ���Կ����
#define errCodeHsmCmdMDL_InvalidPinLength		(errCodeOffsetOfHsmCmdMDL-56)	// �Ƿ�PIN����
#define errCodeHsmCmdMDL_InvalidAccNoLength		(errCodeOffsetOfHsmCmdMDL-57)	// �Ƿ��˺ų���
#define errCodeHsmCmdMDL_InvalidStorageIndex		(errCodeOffsetOfHsmReturnCodeMDL-21)	// �Ƿ��洢����	// 21
#define errCodeHsmCmdMDL_InvalidInputData		(errCodeOffsetOfHsmReturnCodeMDL-15)	// �������ݴ�	//15
#define errCodeHsmCmdMDL_LmkError			(errCodeOffsetOfHsmReturnCodeMDL-13)	// LMK����	// 13
#define errCodeHsmCmdMDL_NoKeyLoadsInStorage		(errCodeOffsetOfHsmReturnCodeMDL-12)	// û����Կ���ص��û��洢��	//12
#define errCodeHsmCmdMDL_ZmkParity			(errCodeOffsetOfHsmReturnCodeMDL-10)	// ZMK��żУ���	//10

// �ⲿͨѶģ��
// offset = -7000
#define errCodeCommConfMDL_CommConfNotDefined		(errCodeOffsetOfCommConfMDL-1)        // ͨѶ����δ����
#define errCodeCommConfMDL_AddCommConf			(errCodeOffsetOfCommConfMDL-2)        // ����ָ����ͨѶ����
#define errCodeCommConfMDL_CommDefTBLConnected		(errCodeOffsetOfCommConfMDL-3)        // ����ͨѶ�������
#define errCodeCommConfMDL_CommRemarkNotDefined		(errCodeOffsetOfCommConfMDL-4)        // ͨѶ����û�ж���
//#define errCodeCommConfMDL_CityIDNotDefined		(errCodeOffsetOfCommConfMDL-3)        // ������δ����

// offset = -26000
// 8583 ģ�����
#define errCodeISO8583MDL_MTINotDefined				(errCodeOffsetOfISO8583MDL-1)	// MTIδ����
#define errCodeISO8583MDL_8583PackageClassTBLConnected		(errCodeOffsetOfISO8583MDL-2)	// ����8583�����ඨ�����
#define errCodeISO8583MDL_8583TransDefTBLConnected		(errCodeOffsetOfISO8583MDL-3)	// ����8583���׶������
#define errCodeISO8583MDL_TransNotDefined			(errCodeOffsetOfISO8583MDL-4)	// ����δ����
#define errCodeISO8583MDL_YLMngTransDefTBLConnected		(errCodeOffsetOfISO8583MDL-5)	// �������������ཻ�ױ��
#define errCodeISO8583MDL_YLMngTransNotDefined			(errCodeOffsetOfISO8583MDL-6)	// ����������δ����
#define errCodeISO8583MDL_8583Fld90Length			(errCodeOffsetOfISO8583MDL-7)	// 90�򳤶ȴ���
#define errCodeISO8583MDL_FldNoValue				(errCodeOffsetOfISO8583MDL-8)	// ����ֵ
#define errCodeISO8583MDL_SPMngTransDefTBLConnected		(errCodeOffsetOfISO8583MDL-9)	// ����SP�����ཻ�ױ��
#define errCodeISO8583MDL_SPMngTransNotDefined			(errCodeOffsetOfISO8583MDL-10)	// SP������δ����
#define errCodeISO8583MDL_8583Fld61Length			(errCodeOffsetOfISO8583MDL-11)	// 61�򳤶ȴ���

// offset = -25000
// ����ģ�����
#define errCodePackageDefMDL_MaxNumOfPackageFldsNotDefined	(errCodeOffsetOfPackageDefMDL-1)	// �������������δ����
#define errCodePackageDefMDL_PackageDefGroupIsFull		(errCodeOffsetOfPackageDefMDL-2)	// ���Ķ���������
#define errCodePackageDefMDL_PackageDefNotDefined		(errCodeOffsetOfPackageDefMDL-3)	// ���Ķ���δ����
#define errCodePackageDefMDL_IDOfPackageNotDefined		(errCodeOffsetOfPackageDefMDL-4)	// ���ĵı�ʶδ����
#define errCodePackageDefMDL_InvalidIDOfPackage			(errCodeOffsetOfPackageDefMDL-5)	// �Ƿ��ı��ı�ʶ
#define errCodePackageDefMDL_PackageOfSameID			(errCodeOffsetOfPackageDefMDL-6)	// ���ı�ʶ�ظ�
#define errCodePackageDefMDL_InvalidMaxFldNum			(errCodeOffsetOfPackageDefMDL-7)	// �Ƿ����������Ŀ
#define errCodePackageDefMDL_PackageTypeNotDefined		(errCodeOffsetOfPackageDefMDL-8)	// ��������δ����
#define errCodePackageDefMDL_InvalidPackageType			(errCodeOffsetOfPackageDefMDL-9)	// �Ƿ��ı�������
#define errCodePackageDefMDL_InvalidPrimaryKey			(errCodeOffsetOfPackageDefMDL-10)	// �Ƿ�������
#define errCodePackageDefMDL_FldNotDefined			(errCodeOffsetOfPackageDefMDL-11)	// ��δ����
#define errCodePackageDefMDL_PackageDefOfSpecifiedIDDefined	(errCodeOffsetOfPackageDefMDL-12)	// ָ����ʶ�ı��Ķ����
#define errCodePackageDefMDL_PackFldIsNull			(errCodeOffsetOfPackageDefMDL-13)	// ����������
#define errCodePackageDefMDL_PackFldLength			(errCodeOffsetOfPackageDefMDL-14)	// �����򳤶ȴ�
#define errCodePackageDefMDL_FldNotDefinedInBitMap		(errCodeOffsetOfPackageDefMDL-15)	// λͼ��δ������
#define errCodePackageDefMDL_BitMapHaveValueForNullValueFld	(errCodeOffsetOfPackageDefMDL-16)	// λͼ�ж�����ֵΪ�յ���
#define errCodePackageDefMDL_PackageNotConnected		(errCodeOffsetOfPackageDefMDL-17)	// ����δ����
#define errCodePackageDefMDL_InvalidFldIndex			(errCodeOffsetOfPackageDefMDL-18)	// �Ƿ���������
#define errCodePackageDefMDL_BitMapFldCannotBeSet		(errCodeOffsetOfPackageDefMDL-19)	// λͼ��������
#define errCodePackageDefMDL_InvalidPackageIndex		(errCodeOffsetOfPackageDefMDL-20)	// �Ƿ��ı�������
#define errCodePackageDefMDL_TwoPackageOfSamePackageIndex	(errCodeOffsetOfPackageDefMDL-21)	// �������ľ�����ͬ�ı�������
#define errCodePackageDefMDL_ConnectPackage			(errCodeOffsetOfPackageDefMDL-22)	// ���ӱ��ĳ���
#define errCodePackageDefMDL_PackExchDefTBLOfSpecifiedIDDefined	(errCodeOffsetOfPackageDefMDL-23)	// ָ����ʶ�ı��Ľ�����������
#define errCodePackageDefMDL_IDOfPackExchNotDefined		(errCodeOffsetOfPackageDefMDL-24)	// ָ���ı��Ľ�������
#define errCodePackageDefMDL_InvalidIDOfPackExch		(errCodeOffsetOfPackageDefMDL-25)	// �Ƿ��ı��Ľ��������ʶ
#define errCodePackageDefMDL_PackExchDefTBLNotDefined		(errCodeOffsetOfPackageDefMDL-26)	// ���Ľ��������δ����
#define errCodePackageDefMDL_PackExchDefOfSameID		(errCodeOffsetOfPackageDefMDL-27)	// ���Ľ�����������ͬ�ı�ʶ
#define errCodePackageDefMDL_NotBitMapFld			(errCodeOffsetOfPackageDefMDL-28)	// ����λͼ��
#define errCodePackageDefMDL_TwoMuchBitMapLevel			(errCodeOffsetOfPackageDefMDL-29)	// ��
#define errCodePackageDefMDL_IsBitMapFld			(errCodeOffsetOfPackageDefMDL-30)	// ��λͼ��

// �������ʹ���
// offset = -24000
#define errCodeTransClassDefMDL_MaxNumOfTransClassDefNotDefined		(errCodeOffsetOfTransClassDefMDL-1)	// ���Ľ���������δ����
#define errCodeTransClassDefMDL_SpecifiedTransClassDefNotDefined	(errCodeOffsetOfTransClassDefMDL-2)	// ָ���Ľ�������δ����
#define errCodeTransClassDefMDL_TransDefNotDefined			(errCodeOffsetOfTransClassDefMDL-3)	// ����δ����

// ����ģ�����
// offset = -13000
#define errCodeTaskMDL_CreateTaskInstance	(errCodeOffsetOfTaskMDL-1)	// ��������ʵ����
#define errCodeTaskMDL_ConfFile			(errCodeOffsetOfTaskMDL-2)	// ���������ļ���
#define errCodeTaskMDL_NotConnected		(errCodeOffsetOfTaskMDL-3)	// ����ģ��δ����
#define errCodeTaskMDL_TaskNotExists		(errCodeOffsetOfTaskMDL-4)	// ���񲻴���
#define errCodeTaskMDL_TaskClassAlreadyExists	(errCodeOffsetOfTaskMDL-5)	// ���������Ѵ���
#define errCodeTaskMDL_TaskClassNameTooLong	(errCodeOffsetOfTaskMDL-6)	// ������������̫��
#define errCodeTaskMDL_TaskClassStartCmdTooLong	(errCodeOffsetOfTaskMDL-7)	// ������������̫��
#define errCodeTaskMDL_TaskClassLogFileNameIsNull	(errCodeOffsetOfTaskMDL-8)	// ������־����Ϊ��
#define errCodeTaskMDL_TaskClassNameIsNull	(errCodeOffsetOfTaskMDL-9)	// ������������Ϊ��
#define errCodeTaskMDL_TaskClassLogFileNameTooLong	(errCodeOffsetOfTaskMDL-10)	// ������־����̫��
#define errCodeTaskMDL_TaskClassTableIsFull	(errCodeOffsetOfTaskMDL-11)	// �������ͱ�����
#define errCodeTaskMDL_Connect			(errCodeOffsetOfTaskMDL-12)	// ������������
#define errCodeTaskMDL_TaskClassIsUsed		(errCodeOffsetOfTaskMDL-13)	// �����������ڱ�ʹ��
#define errCodeTaskMDL_TaskClassNameNotDefined	(errCodeOffsetOfTaskMDL-14)	// ������������δ����
#define errCodeTaskMDL_TaskClassNotExists	(errCodeOffsetOfTaskMDL-15)	// �������Ͳ�����
#define errCodeTaskMDL_TaskClassDefLineError	(errCodeOffsetOfTaskMDL-16)	// �������Ͷ������д�
#define errCodeTaskMDL_AnotherTaskOfNameExists	(errCodeOffsetOfTaskMDL-17)	// ͬ���������Ѿ����� 2007/11/30������������

// Socketģ�����
// offset = -22000
#define errCodeSocketMDL_Timeout		(errCodeOffsetOfSocketMDL-1)	// ��ʱ
#define errCodeSocketMDL_ReceiveLen		(errCodeOffsetOfSocketMDL-2)	// ���ճ���
#define errCodeSocketMDL_Error			(errCodeOffsetOfSocketMDL-3)	// ͨѶʧ��

// ͨѶģ�����
// offset = -21000
#define errCodeSckCommMDL_ConfFile		(errCodeOffsetOfSckCommMDL-1)	// �����ļ�
#define errCodeSckCommMDL_InvalidWorkingMode	(errCodeOffsetOfSckCommMDL-2)	// �Ƿ�����״̬
#define errCodeSckCommMDL_ServerNotDefined	(errCodeOffsetOfSckCommMDL-3)	// ������δ����
#define errCodeSckCommMDL_ServiceFailure	(errCodeOffsetOfSckCommMDL-4)	// ����ʧ��
#define errCodeSckCommMDL_NoWorkingServer	(errCodeOffsetOfSckCommMDL-5)	// ���ǹ����ŵķ�����
#define errCodeSckCommMDL_InternalError		(errCodeOffsetOfSckCommMDL-6)	// �ڲ�����
#define errCodeSckCommMDL_Connect		(errCodeOffsetOfSckCommMDL-7)	// ����ͨѶģ��


// RECģ�����
// offset = -20000
#define errCodeRECMDL_ConfFile			(errCodeOffsetOfRECMDL-1)	// �����ļ�����
#define errCodeRECMDL_VarNotExists		(errCodeOffsetOfRECMDL-2)	// ����������
#define errCodeRECMDL_VarType			(errCodeOffsetOfRECMDL-3)	// �������ʹ�
#define errCodeRECMDL_VarValue			(errCodeOffsetOfRECMDL-4)	// ����ֵ������
#define errCodeRECMDL_Connect			(errCodeOffsetOfRECMDL-5)	// ���ӱ�������ģ��
#define errCodeRECMDL_VarAlreadyExists		(errCodeOffsetOfRECMDL-6)	// �����Ѵ���
#define errCodeRECMDL_VarTBLIsFull		(errCodeOffsetOfRECMDL-7)	// ����������
#define errCodeRECMDL_VarNameLength		(errCodeOffsetOfRECMDL-8)	// �������Ƴ��ȴ�
#define errCodeRECMDL_VarValueTooLong		(errCodeOffsetOfRECMDL-9)	// ֵ̫����
#define errCodeRECMDL_RealNumError		(errCodeOffsetOfRECMDL-10)	// ��������Ŀ��

// ����Աģ�����
// offset = -19000
#define errCodeOperatorMDL_OperatorNum		(errCodeOffsetOfOpertatorMDL-1)	// ����Ա��Ŀ��
#define errCodeOperatorMDL_OperatorNotExists	(errCodeOffsetOfOpertatorMDL-2)	// ����Ա������
#define errCodeOperatorMDL_NotConnected		(errCodeOffsetOfOpertatorMDL-3)	// ����Աģ��δ����
#define errCodeOperatorMDL_TableFull		(errCodeOffsetOfOpertatorMDL-4)	// ����Ա����
#define errCodeOperatorMDL_ReadOperator		(errCodeOffsetOfOpertatorMDL-5)	// ������Ա
#define errCodeOperatorMDL_WrongPassword	(errCodeOffsetOfOpertatorMDL-6)	// ����Ա�����
#define errCodeOperatorMDL_PasswordLocked	(errCodeOffsetOfOpertatorMDL-7)	// ����Ա���뱻��ס��
#define errCodeOperatorMDL_AlreadyLogon		(errCodeOffsetOfOpertatorMDL-8)	// ����Ա�Ѿ���¼��
#define errCodeOperatorMDL_NotLogon		(errCodeOffsetOfOpertatorMDL-9)	// ����Ա��û�е�¼
#define errCodeOperatorMDL_TooLongTimeWithoutOperation		(errCodeOffsetOfOpertatorMDL-10)	// ����Ա̫��ʱ��û�в���
#define errCodeOperatorMDL_StillLogon		(errCodeOffsetOfOpertatorMDL-11)	// ����Ա�ڵ�¼״̬
#define errCodeOperatorMDL_CannotDeleteSelf	(errCodeOffsetOfOpertatorMDL-12)	// ����ɾ���Լ�
#define errCodeOperatorMDL_OperatorAlreadyExists	(errCodeOffsetOfOpertatorMDL-13)	// ����Ա�Ѵ���


// ��Ϣ����������
// offset = -18000
#define errCodeMsgBufMDL_NotConnected		(errCodeOffsetOfMsgBufMDL-1)	// ��Ϣ������δ����
#define errCodeMsgBufMDL_OutofRange		(errCodeOffsetOfMsgBufMDL-2)	// ����
#define errCodeMsgBufMDL_InitDef		(errCodeOffsetOfMsgBufMDL-3)	// ��ʼ�����
#define errCodeMsgBufMDL_ConnectIndexTBL	(errCodeOffsetOfMsgBufMDL-4)	// �����������
#define errCodeMsgBufMDL_ProviderExit		(errCodeOffsetOfMsgBufMDL-5)	// �ṩ�����˳�
#define errCodeMsgBufMDL_NotOriginMsg		(errCodeOffsetOfMsgBufMDL-6)	// ����ԭʼ��Ϣ
#define errCodeMsgBufMDL_InvalidIndex		(errCodeOffsetOfMsgBufMDL-7)	// �Ƿ�������
#define errCodeMsgBufMDL_ReadIndex		(errCodeOffsetOfMsgBufMDL-8)	// ��ȡ����
#define errCodeMsgBufMDL_MsgLen			(errCodeOffsetOfMsgBufMDL-9)	// ��Ϣ���ȴ�
#define errCodeMsgBufMDL_MsgOfTypeAndOfIndexNotExists	(errCodeOffsetOfMsgBufMDL-10)	// ����ָ�����ͺ���������Ϣ������
#define errCodeMsgBufMDL_MsgIndexAlreadyLosted	(errCodeOffsetOfMsgBufMDL-11)	// ��Ϣ�����Ѿ���ʧ

// ���׼����Ϣ����������
// offset = -4000
#define errCodeTransSpierBufMDL_NotConnected		(errCodeOffsetOfTransSpierBufMDL-1)	// ��Ϣ������δ����
#define errCodeTransSpierBufMDL_OutofRange		(errCodeOffsetOfTransSpierBufMDL-2)	// ����
#define errCodeTransSpierBufMDL_InitDef			(errCodeOffsetOfTransSpierBufMDL-3)	// ��ʼ�����
#define errCodeTransSpierBufMDL_ConnectIndexTBL		(errCodeOffsetOfTransSpierBufMDL-4)	// �����������
#define errCodeTransSpierBufMDL_ProviderExit		(errCodeOffsetOfTransSpierBufMDL-5)	// �ṩ�����˳�
#define errCodeTransSpierBufMDL_NotOriginMsg		(errCodeOffsetOfTransSpierBufMDL-6)	// ����ԭʼ��Ϣ
#define errCodeTransSpierBufMDL_InvalidIndex		(errCodeOffsetOfTransSpierBufMDL-7)	// �Ƿ�������
#define errCodeTransSpierBufMDL_ReadIndex		(errCodeOffsetOfTransSpierBufMDL-8)	// ��ȡ����
#define errCodeTransSpierBufMDL_MsgLen			(errCodeOffsetOfTransSpierBufMDL-9)	// ��Ϣ���ȴ�
#define errCodeTransSpierBufMDL_MsgOfTypeAndOfIndexNotExists	(errCodeOffsetOfTransSpierBufMDL-10)	// ����ָ�����ͺ���������Ϣ������

// �����ڴ�ģ�����
// offset = -17000
#define errCodeSharedMemoryMDL_MDLNotDefined	(errCodeOffsetOfSharedMemoryMDL-1)	// �����ڴ�ģ��δ����
#define errCodeSharedMemoryMDL_LockMDL		(errCodeOffsetOfSharedMemoryMDL-2)	// �������ڴ��
#define errCodeSharedMemoryMDL_Connect		(errCodeOffsetOfSharedMemoryMDL-3)	// ���ӹ����ڴ�ģ��

// ��־ģ�����
// offset = -16000
#define errCodeLogMDL_ConfFile			(errCodeOffsetOfLogMDL-1)	// �����ļ���
#define errCodeLogMDL_LogFileAlreadyExists	(errCodeOffsetOfLogMDL-2)	// ��־�Ѵ���
#define errCodeLogMDL_LogFileTblFull		(errCodeOffsetOfLogMDL-3)	// ��־������
#define errCodeLogMDL_LogFileUsedByUsers	(errCodeOffsetOfLogMDL-4)	// ��־����ʹ��
#define errCodeLogMDL_LogFileNotExists		(errCodeOffsetOfLogMDL-5)	// ��־������
#define errCodeLogMDL_NotConnected		(errCodeOffsetOfLogMDL-6)	// ��־ģ��δ����
#define errCodeLogMDL_Connect			(errCodeOffsetOfLogMDL-7)	// ������־ģ��
#define errCodeLogMDL_LogFileNameMustSpecified	(errCodeOffsetOfLogMDL-8)	// ����ָ����־����

// ��������ģ�����
// offset = -14000
#define errCodeEnviMDL_VarNotExists		(errCodeOffsetOfEnviMDL-1)	// ����������
#define errCodeEnviMDL_NullLine			(errCodeOffsetOfEnviMDL-2)	// ����
#define errCodeEnviMDL_OccupiedByOther		(errCodeOffsetOfEnviMDL-3)	// ��������ģ�鱻����ռ����
#define errCodeEnviMDL_NoValueDefinedForVar	(errCodeOffsetOfEnviMDL-4)	// ����ֵû����
#define errCodeEnviMDL_VarAlreadyExists		(errCodeOffsetOfEnviMDL-5)	// �����Ѵ���
#define errCodeEnviMDL_NotRecFormatDefStr	(errCodeOffsetOfEnviMDL-6)	// ���Ǽ�¼��ʽ�����ַ���
#define errCodeEnviMDL_NotRecStrTooLong		(errCodeOffsetOfEnviMDL-7)	// ��¼�ַ���̫��
#define errCodeEnviMDL_NotRecDefStr		(errCodeOffsetOfEnviMDL-8)	// ���Ǽ�¼���崮

// ��ԿCACHEģ�����
// offset = -6000
#define errCodeKeyCacheMDL_KeyNonExists		(errCodeOffsetOfKeyCacheMDL-1)	// ��Կ������
#define errCodeKeyCacheMDL_KeyNum		(errCodeOffsetOfKeyCacheMDL-2)	// ��Կ������
#define errCodeKeyCacheMDL_KeyDBFull		(errCodeOffsetOfKeyCacheMDL-3)	// ����
#define errCodeKeyCacheMDL_KeyDBNonConnected	(errCodeOffsetOfKeyCacheMDL-4)	// ��δ����
#define errCodeKeyCacheMDL_InvalidKeyLength	(errCodeOffsetOfKeyCacheMDL-5)	// �Ƿ���Կ����
#define errCodeKeyCacheMDL_KeyAlreadyExists	(errCodeOffsetOfKeyCacheMDL-6)	// ��Կ�Ѵ���
#define errCodeKeyCacheMDL_KeyOutdate		(errCodeOffsetOfKeyCacheMDL-7)	// ��Կ����
#define errCodeKeyCacheMDL_KeyValueWithoutEnd	(errCodeOffsetOfKeyCacheMDL-8)	// ��Կֵ�޽�����
#define errCodeKeyCacheMDL_WrongKeyName		(errCodeOffsetOfKeyCacheMDL-9)	// ��Կ���Ʋ���

// offset = -32000
// ���ݿ�Ĵ�����
#define errCodeDatabaseMDL_DatabaseAlreadyExist		(errCodeOffsetOfDatabase-1)	// ���ݿ��Ѿ�����
#define errCodeDatabaseMDL_DatabaseNotFound		(errCodeOffsetOfDatabase-2)	// ���ݿ�û���ҵ�
#define errCodeDatabaseMDL_TableAlreadyExist		(errCodeOffsetOfDatabase-3)	// ���Ѿ�����
#define errCodeDatabaseMDL_TableNotFound		(errCodeOffsetOfDatabase-4)	// ��û���ҵ�
#define errCodeDatabaseMDL_RecordNotFound		(errCodeOffsetOfDatabase-5)	// ��¼û���ҵ�
#define errCodeDatabaseMDL_RecordAlreadyExist		(errCodeOffsetOfDatabase-6)	// ��¼�Ѿ�����
#define errCodeDatabaseMDL_NoData			(errCodeOffsetOfDatabase-7)	// û������
#define errCodeDatabaseMDL_MoreRecordFound		(errCodeOffsetOfDatabase-8)	// �ҵ�������¼

#endif
