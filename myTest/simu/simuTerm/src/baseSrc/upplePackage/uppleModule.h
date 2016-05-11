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
	char	name[40+1];		// 名称
	int	id;			// id号
	char	reloadCommand[80+1];	// 自动加载命令
	int	index;			// 模块在模块表中的索引号,自动分配
	long	sizeOfUserSpace;	// 用户自定义数据空间的大小
	unsigned char *puserSpace;	// 指向用户自定义数据空间的指针
	int	users;			// 当前使用本模块的用户数目
	int	newCreated;		// 模块是否新建标识
	int	writingLocks;		// 读锁数目
	int	readingLocks;		// 写锁数目
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

// 2006/12/27 增加
int UpplePrintExistedSharedMemoryModuleToFileByModuleName(char *mdlName,FILE *fp);
// 2006/12/27 增加以下函数
PUppleSharedMemoryModule UppleConnectExistedSharedMemoryModule(char *mdlName);

int UppleReadingLockModuleByModuleName(char *mdlName);

int UppleWritingLockModuleByModuleName(char *mdlName);

// 2007/10/26 增加
#define conMaxNumOfSharedMemoryPerProcess	32
typedef struct
{
	int				num;
	PUppleSharedMemoryModule	pshmPtrGrp[conMaxNumOfSharedMemoryPerProcess];
} TUppleSharedMemoryGroup;
typedef TUppleSharedMemoryGroup		*PUppleSharedMemoryGroup;

// 2007/10/26增加
int UppleIsSharedMemoryGroupInited();

// 2007/10/26增加
void UppleInitSharedMemoryGroup();

// 2007/10/26增加
void UppleAddIntoSharedMemoryGroup(PUppleSharedMemoryModule pmdl);

// 2007/10/26增加
void UppleDeleteFromSharedMemoryGroup(PUppleSharedMemoryModule pmdl);

// 2007/10/26增加
int UppleIsSharedMemoryGroupIsFull();

// 2007/10/26增加
int UppleIsSharedMemoryInited(char *mdlName);

// 2007/10/26增加
int UppleOutputAllRegisteredShareMemory(char *buf,int sizeOfBuf);

// 2007/10/26增加
void UppleFreeUnnecessarySharedMemory();

#endif
