//	Author:		zhangyongding
//	Date:		20080922
//	Version:	1.0

#ifndef _UppleSharedMemoryModuleMDL_
#define _UppleSharedMemoryModuleMDL_

#define conMDLNameOfUppleSharedMemoryModuleTBL	"UppleSharedMemoryModuleTBLMDL"

int UppleReloadSharedMemoryModule(char *mdlName);
int UppleReloadAllSharedMemoryModule(char *anywayOrNot);

typedef struct
{
	char	name[40+1];		// ����
	int	id;			// id��
	char	reloadCommand[80+1];	// �Զ���������
	int	index;			// ģ����ģ����е�������,�Զ�����
	long	sizeOfUserSpace;	// �û��Զ������ݿռ�Ĵ�С
	unsigned char *puserSpace;	// ָ���û��Զ������ݿռ��ָ��
	int	users;			// ��ǰʹ�ñ�ģ����û���Ŀ
	int	newCreated;		// ģ���Ƿ��½���ʶ
	int	writingLocks;		// ������Ŀ
	int	readingLocks;		// д����Ŀ
} TUppleSharedMemoryModule;
typedef TUppleSharedMemoryModule			*PUppleSharedMemoryModule;

int UppleReadSharedMemoryModuleDef(char *mdlName,PUppleSharedMemoryModule pmdl);
int UpplePrintSharedMemoryModuleToFile(PUppleSharedMemoryModule pmdl,FILE *fp);
int UpplePrintSharedMemoryModuleToFileByModuleName(char *mdlName,FILE *fp);
int UpplePrintSharedMemoryModule(PUppleSharedMemoryModule pmdl);

int UppleGetNameOfSharedMemoryModuleTBL(char *fileName);

int UppleRemoveAllSharedMemoryModule();

PUppleSharedMemoryModule UppleConnectSharedMemoryModule(char *mdlName,long sizeOfUserSpace);
PUppleSharedMemoryModule UppleConnectExistingSharedMemoryModule(char *mdlName,long sizeOfUserSpace);
int UppleDisconnectShareModule(PUppleSharedMemoryModule);
int UppleRemoveSharedMemoryModule(char *mdlName);

int UppleIsNewCreatedSharedMemoryModule(PUppleSharedMemoryModule pmdl);
unsigned char *UppleGetAddrOfSharedMemoryModuleUserSpace(PUppleSharedMemoryModule pmdl);

int UppleApplyWritingLocks(PUppleSharedMemoryModule pmdl);
int UppleReleaseWritingLocks(PUppleSharedMemoryModule pmdl);
int UppleApplyReadingLocks(PUppleSharedMemoryModule pmdl);
int UppleReleaseReadingLocks(PUppleSharedMemoryModule pmdl);

int UppleResetWritingLocks(char *mdlName);
int UppleResetReadingLocks(char *mdlName);

int UppleGetUserIDOfSharedMemoryModule(PUppleSharedMemoryModule pmdl);

// 2006/12/27 ����
int UpplePrintExistedSharedMemoryModuleToFileByModuleName(char *mdlName,FILE *fp);
// 2006/12/27 �������º���
PUppleSharedMemoryModule UppleConnectExistedSharedMemoryModule(char *mdlName);

int UppleReadingLockModuleByModuleName(char *mdlName);

int UppleWritingLockModuleByModuleName(char *mdlName);

// 2007/10/26 ����
#define conMaxNumOfSharedMemoryPerProcess	32
typedef struct
{
	int				num;
	PUppleSharedMemoryModule	pshmPtrGrp[conMaxNumOfSharedMemoryPerProcess];
} TUppleSharedMemoryGroup;
typedef TUppleSharedMemoryGroup		*PUppleSharedMemoryGroup;

// 2007/10/26����
int UppleIsSharedMemoryGroupInited();

// 2007/10/26����
void UppleInitSharedMemoryGroup();

// 2007/10/26����
void UppleAddIntoSharedMemoryGroup(PUppleSharedMemoryModule pmdl);

// 2007/10/26����
void UppleDeleteFromSharedMemoryGroup(PUppleSharedMemoryModule pmdl);

// 2007/10/26����
int UppleIsSharedMemoryGroupIsFull();

// 2007/10/26����
int UppleIsSharedMemoryInited(char *mdlName);

// 2007/10/26����
int UppleOutputAllRegisteredShareMemory(char *buf,int sizeOfBuf);

// 2007/10/26����
void UppleFreeUnnecessarySharedMemory();

#endif
