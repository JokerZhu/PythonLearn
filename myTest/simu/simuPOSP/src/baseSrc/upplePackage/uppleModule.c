//	Author:		zhangyongding
//	Date:		20080922
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "uppleModule.h"
#include "uppleEnv.h"
#include "uppleErrCode.h"
//#include "uppleIndexTBL.h"

#define USE_REDIS
//#define DEBUG_REDIS_MEM


// 以下变量2007/12/26增加
TUppleSharedMemoryGroup		guppleSharedMemoryGrp;
int				guppleIsSharedMemoryGrpInited = 0;

// 2007/10/26增加
int UppleOutputAllRegisteredShareMemory(char *buf,int sizeOfBuf)
{
	int	index;
	int	ret;
	int	offset = 0;
	PUppleSharedMemoryModule	pmdl;

#ifdef _AIX_	
	UppleInitSharedMemoryGroup();
	
	sprintf(buf,"num=%04d|",guppleSharedMemoryGrp.num);
	offset = strlen(buf);
	for (index = 0; index < guppleSharedMemoryGrp.num; index++)
	{
		pmdl = guppleSharedMemoryGrp.pshmPtrGrp[index];
		if ((pmdl == NULL) || (pmdl == -1))
			continue;
		if (offset + strlen(pmdl->name) + 8 >= sizeOfBuf)
			break;
		sprintf(buf+offset,"name%02d=%s|",index,pmdl->name);
		offset += (strlen(pmdl->name) + 8);
	}
	return(offset);
#else
	return(0);
#endif
}

// 2007/10/26增加
int UppleIsSharedMemoryGroupInited()
{
#ifdef _AIX_
	return(guppleIsSharedMemoryGrpInited);
#else
	return(1);
#endif
}

// 2007/10/26增加
void UppleInitSharedMemoryGroup()
{
	int	index;
#ifdef _AIX_
	if (guppleIsSharedMemoryGrpInited)
		return;
		
	for (index = 0; index < conMaxNumOfSharedMemoryPerProcess; index++)
		guppleSharedMemoryGrp.pshmPtrGrp[index] = NULL;
	guppleSharedMemoryGrp.num = 0;
	guppleIsSharedMemoryGrpInited = 1;
#endif
	return;
}

// 2007/10/26增加
void UppleAddIntoSharedMemoryGroup(PUppleSharedMemoryModule pmdl)
{
#ifdef _AIX_
	if ((pmdl == NULL) || (pmdl == -1))
		return;
	
	UppleInitSharedMemoryGroup();
	if (guppleSharedMemoryGrp.num >= conMaxNumOfSharedMemoryPerProcess)
	{
		printf("in UppleAddIntoSharedMemoryGroup:: shared memory group is full! [%x] not added\n",pmdl);
		return;
	}
	guppleSharedMemoryGrp.pshmPtrGrp[guppleSharedMemoryGrp.num] = pmdl;
	guppleSharedMemoryGrp.num += 1;
	//UppleProgramerLog("in UppleAddIntoSharedMemoryGroup:: [%x] of [%s] added ok!\n",pmdl,pmdl->name);
#endif
	return;
}

// 2007/10/26增加
void UppleDeleteFromSharedMemoryGroup(PUppleSharedMemoryModule pmdl)
{
	int	index;
#ifdef _AIX_	
	if ((pmdl == NULL) || (pmdl == -1))
		return;
	
	UppleInitSharedMemoryGroup();
	for (index = 0; index < guppleSharedMemoryGrp.num; index++)
	{
		if (guppleSharedMemoryGrp.pshmPtrGrp[index] == pmdl)
		{
			guppleSharedMemoryGrp.pshmPtrGrp[index] = guppleSharedMemoryGrp.pshmPtrGrp[guppleSharedMemoryGrp.num-1];
			guppleSharedMemoryGrp.num -= 1;
			shmdt(pmdl);
			pmdl = NULL;
			return;
		}
	}
	printf("in UppleDeleteFromSharedMemoryGroup:: shared memory [%x] not exists!\n",pmdl);
#endif
	return;
}

// 2007/10/26增加
int UppleIsSharedMemoryGroupIsFull()
{
#ifdef _AIX_
	UppleInitSharedMemoryGroup();
	if (guppleSharedMemoryGrp.num >= conMaxNumOfSharedMemoryPerProcess)
		return(1);
	else
		return(0);
#else
	return(0);
#endif
}

// 2007/10/26增加
int UppleIsSharedMemoryInited(char *mdlName)
{
	int	index;
	
#ifdef _AIX_
	UppleInitSharedMemoryGroup();
	for (index = 0; index < guppleSharedMemoryGrp.num; index++)
	{
		if ((guppleSharedMemoryGrp.pshmPtrGrp[index] == NULL) || (guppleSharedMemoryGrp.pshmPtrGrp[index] == -1))
			continue;
		if (strcmp(guppleSharedMemoryGrp.pshmPtrGrp[index]->name,mdlName) == 0)
			return(1);
	}
	return(0);
#else
	return(1);
#endif
}

// 2007/10/26增加
void UppleFreeUnnecessarySharedMemory()
{
	int				index;
	PUppleSharedMemoryModule	pmdl;

#ifdef _AIX_	
	UppleInitSharedMemoryGroup();	
	for (index = 0; index < guppleSharedMemoryGrp.num; index++)
	{
		pmdl = guppleSharedMemoryGrp.pshmPtrGrp[index];
		if ((pmdl == NULL) || (pmdl == -1))
			continue;
		if (strcmp(pmdl->name,"UppleTaskTBLMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UppleLogTBLMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UppleSJL06TBLMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UppleRECMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UppleDesKeyDBMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UpplePKDBMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UppleMsgBufMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UppleKDBSvrMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UppleTransSpierBufMDL") == 0)
			continue;
		guppleSharedMemoryGrp.pshmPtrGrp[index] = guppleSharedMemoryGrp.pshmPtrGrp[guppleSharedMemoryGrp.num-1];
		guppleSharedMemoryGrp.num -= 1;
		UppleProgramerLog("in UppleFreeUnnecessarySharedMemory:: [%s] [%x] freed!\n",pmdl->name,pmdl);
		shmdt(pmdl);
		pmdl = NULL;
		return;
	}
#endif		
	return;
}
	
PUppleSharedMemoryModule UppleConnectExistingSharedMemoryModule(char *mdlName,long sizeOfUserSpace)
{
	int				ret;
	TUppleSharedMemoryModule	mdl;
	int				resID;
	PUppleSharedMemoryModule	pmdl;
	int				i;

	// 20071026增加	
	if (UppleIsSharedMemoryGroupIsFull())
	{
		UppleFreeUnnecessarySharedMemory();
		if (UppleIsSharedMemoryGroupIsFull())
		{
			printf("in UppleConnectExistingSharedMemoryModule:: shared memory group is full!\n");
			return(NULL);
		}
	}
	// 20071026增加结束
	
	memset(&mdl,0,sizeof(mdl));
	if ((ret = UppleReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		printf("in UppleConnectExistingSharedMemoryModule:: UppleReadSharedMemoryModuleDef [%s]!\n",mdlName);
		return(NULL);
	}
	
	if (mdl.id <= 0)
	{
		printf("in UppleConnectExistingSharedMemoryModule:: mdl.id = [%d] Error!\n",mdl.id);
		return(NULL);
	}
	
	if (sizeOfUserSpace < 0)
	{
		printf("in UppleConnectExistingSharedMemoryModule:: sizeOfUserSpace = [%d] Error!\n",sizeOfUserSpace);
		return(NULL);
	}
	
	if ((resID = shmget(mdl.id,sizeof(TUppleSharedMemoryModule) + sizeOfUserSpace,0666)) == -1)
	{
		printf("in UppleConnectExistingSharedMemoryModule:: shmget [%d] [%ld]!\n",mdl.id,sizeOfUserSpace);
		return(NULL);
	}
		
	//printf("in UppleConnectExistingSharedMemoryModule:: shmget [%d] OK newCreated = [%d]!\n",mdl.id,mdl.newCreated);
	
	// 2007/10/26修改以下代码
	if (((pmdl = (PUppleSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL) || (pmdl == -1))
	{
		if (errno == 24)
		{
			UppleFreeUnnecessarySharedMemory();
			pmdl = (PUppleSharedMemoryModule)shmat(resID,0,SHM_RND);
		}
		if ((pmdl == NULL) || (pmdl == -1))
		{
			printf("in UppleConnectExistingSharedMemoryModule:: shmat [%d] errno[%d]!\n",mdl.id,errno);
			return(NULL);
		}
	}
	// 2007/10/26修改结束

	
	++(pmdl->users);
	pmdl->newCreated = 0;
	
	if (sizeOfUserSpace == 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));

	UppleAddIntoSharedMemoryGroup(pmdl);	// 2007/10/26 增加

	return(pmdl);
}

int UppleGetNameOfSharedMemoryModuleTBL(char *fileName)
{
	sprintf(fileName,"%s/sharedMemory.cfg",getenv("UPPLEETC"));
	return(0);
}

int UppleReadSharedMemoryModuleDef(char *mdlName,PUppleSharedMemoryModule pmdl)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	
	if ((mdlName == NULL) || (pmdl == NULL))
	{
		printf("in UppleReadSharedMemoryModuleDef:: mdlName or pmdl is NULL!\n");
		return(errCodeParameter);
	}
	if (strlen(mdlName) > sizeof(pmdl->name) - 1)
	{
		printf("in UppleReadSharedMemoryModuleDef:: mdlName [%s] longer than expected [%d]!\n",mdlName,sizeof(pmdl->name)-1);
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
	UppleGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf("in UppleReadSharedMemoryModuleDef:: UppleInitEnvi!\n");
		return(ret);
	}

	if ((i = UppleGetVarIndexOfTheVarName(mdlName)) < 0)
	{
		printf("in UppleReadSharedMemoryModuleDef:: UppleGetVarIndexOfTheVarName for [%s]\n",mdlName);
		goto abnormalExit;
	}
	
	// 找到了该模块的定义
	memset(pmdl,0,sizeof(*pmdl));
	strcpy(pmdl->name,mdlName);
	// 读取ID号
	if ((p = UppleGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
	{
		printf("in UppleReadSharedMemoryModuleDef:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
		goto abnormalExit;
	}
	pmdl->id = atoi(p);
	// 读取自动加载命令
	if ((p = UppleGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
	{
		printf("in UppleReadSharedMemoryModuleDef:: UppleGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
		goto abnormalExit;
	}
	if (strlen(p) > sizeof(pmdl->reloadCommand) - 1)
	{
		printf("in UppleReadSharedMemoryModuleDef:: reloadCommand [%s] longer than expected [%d]!\n",p,sizeof(pmdl->reloadCommand)-1);
		goto abnormalExit;
	}
	strcpy(pmdl->reloadCommand,p);
	pmdl->index = i;
	pmdl->sizeOfUserSpace = 0;
	pmdl->puserSpace = NULL;
	pmdl->users = 0;
	pmdl->newCreated = 0;
	pmdl->readingLocks = 0;
	pmdl->writingLocks = 0;
	
	//printf("in UppleReadSharedMemoryModuleDef:: [%s] [%d] [%s] [%d]\n",pmdl->name,pmdl->id,pmdl->reloadCommand,pmdl->index);
		
	UppleClearEnvi();
	return(0);

abnormalExit:
	printf("in UppleReadSharedMemoryModuleDef:: [%s] not defined in file [%s]\n",mdlName,fileName);
	
	UppleClearEnvi();
	return(errCodeSharedMemoryMDL_MDLNotDefined);
}

int UppleIsNewCreatedSharedMemoryModule(PUppleSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
		return(0);
	return(pmdl->newCreated);
}

unsigned char *UppleGetAddrOfSharedMemoryModuleUserSpace(PUppleSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
		return(NULL);
	return(pmdl->puserSpace);
}
	
int UpplePrintSharedMemoryModuleToFile(PUppleSharedMemoryModule pmdl,FILE *fp)
{
	if ((fp == NULL) || (pmdl == NULL))
	{
		printf("in UpplePrintSharedMemoryModuleToFile:: fp or pmdl is NULL!\n");
		return(errCodeParameter);
	}
	
	fprintf(fp,"\nThis SharedMemoryModule\n");
	fprintf(fp,"[name]			[%s]\n",pmdl->name);
	fprintf(fp,"[id]			[%d]\n",pmdl->id);
	fprintf(fp,"[reloadCommand]		[%s]\n",pmdl->reloadCommand);
	fprintf(fp,"[index]			[%d]\n",pmdl->index);
	fprintf(fp,"[sizeOfUserSpace]	[%ld]\n",pmdl->sizeOfUserSpace);
	fprintf(fp,"[userSpaceAddress]	[%0x]\n",pmdl->puserSpace);
	fprintf(fp,"[users]			[%d]\n",pmdl->users);
	fprintf(fp,"[newCreated]		[%d]\n",pmdl->newCreated);
	fprintf(fp,"[readingLocks]		[%d]\n",pmdl->readingLocks);
	fprintf(fp,"[writingLocks]		[%d]\n",pmdl->writingLocks);
	fflush(fp);
	
	return(0);
}
		
int UpplePrintSharedMemoryModule(PUppleSharedMemoryModule pmdl)
{
	return(UpplePrintSharedMemoryModuleToFile(pmdl,stdout));
}

int UppleRemoveAllSharedMemoryModule()
{
	int			ret;
	struct shmid_ds		buf;
	int 			resID;
	char			fileName[512];
	char			*p;
	int			id;
	int			i;
	int			num;
	
	memset(fileName,0,sizeof(fileName));
	UppleGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf("in UppleRemoveAllSharedMemoryModule:: UppleInitEnvi!\n");
		return(ret);
	}
	
	for (i = 0,num = 0; i < UppleGetEnviVarNum(); i++)
	{
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
			continue;
		printf("i = %d	idOfShareMemoryID = [%d]\n",i,atoi(p));
		if ((id = atoi(p)) <= 0)
			continue;
		if ((resID = shmget(id,sizeof(TUppleSharedMemoryModule),0666)) == -1)
		{
			printf("in UppleRemoveAllSharedMemoryModule:: shmget [%d]\n",id);
			continue;
		}
		// 2008/09/24 增加开始
		if (strcmp(p,"UppleMsgBufMDL") == 0)	// 是消息交换区模块，删除对应的队列
		{
			//if ((ret = UppleRemoveIndexStatusTBL(id)) < 0)
			{
				printf("in UppleRemoveAllSharedMemoryModule:: UppleRemoveIndexStatusTBL id = [%d]\n",id);
			}
		}
		// 2008/09/24 增加结束
		if (shmctl(resID,IPC_RMID,&buf) != 0)
		{
			printf("in UppleRemoveAllSharedMemoryModule:: shmctl IPC_RMID resID = [%d]\n",id);
			continue;
		}
		num++;
		/*
		// 读取模块名称,以下是20060726增加
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
			continue;
		if (strcmp(p,"UppleMsgBufMDL") != 0)	// 不是消息交换区模块
			continue;
		// 是消息交换区模块，删除对应的队列
		if ((ret = UppleRemoveIndexStatusTBL(id)) < 0)
		{
			printf("in UppleRemoveAllSharedMemoryModule:: UppleRemoveIndexStatusTBL id = [%d]\n",id);
		}
		*/
	}

	UppleClearEnvi();
	
	return(num);
}

PUppleSharedMemoryModule UppleConnectSharedMemoryModule(char *mdlName,long sizeOfUserSpace)
{
	int				ret;
	TUppleSharedMemoryModule	mdl;
	int				resID;
	PUppleSharedMemoryModule	pmdl;
	int				i;
	
	// 20071026增加	
	if (UppleIsSharedMemoryGroupIsFull())
	{
		UppleFreeUnnecessarySharedMemory();
		if (UppleIsSharedMemoryGroupIsFull())
		{
			printf("in UppleConnectSharedMemoryModule:: shared memory group is full!\n");
			return(NULL);
		}
	}
	// 20071026增加结束
	memset(&mdl,0,sizeof(mdl));
	if ((ret = UppleReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		printf("in UppleConnectSharedMemoryModule:: UppleReadSharedMemoryModuleDef [%s]!\n",mdlName);
		return(NULL);
	}
	
	if (mdl.id <= 0)
	{
		printf("in UppleConnectSharedMemoryModule:: mdl.id = [%d] Error!\n",mdl.id);
		return(NULL);
	}
	
	if (sizeOfUserSpace < 0)
	{
		printf("in UppleConnectSharedMemoryModule:: sizeOfUserSpace = [%d] Error!\n",sizeOfUserSpace);
		return(NULL);
	}
	
	if ((resID = shmget(mdl.id,sizeof(TUppleSharedMemoryModule) + sizeOfUserSpace,0666)) == -1)
	{
		if ((resID = shmget(mdl.id,sizeof(TUppleSharedMemoryModule) + sizeOfUserSpace,IPC_CREAT|0666)) == -1)
		{
			printf("in UppleConnectSharedMemoryModule:: shmget [%d] [%ld]!\n",mdl.id,sizeOfUserSpace);
			return(NULL);
		}
		mdl.newCreated = 1;
	}
	else
		mdl.newCreated = 0;
		
	//printf("in UppleConnectSharedMemoryModule:: shmget [%d] OK newCreated = [%d]!\n",mdl.id,mdl.newCreated);
	
	// 2007/10/26修改以下代码
	if (((pmdl = (PUppleSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL) || (pmdl == -1))
	{
		if (errno == 24)
		{
			UppleFreeUnnecessarySharedMemory();
			pmdl = (PUppleSharedMemoryModule)shmat(resID,0,SHM_RND);
		}
		if ((pmdl == NULL) || (pmdl == -1))
		{
			printf("in UppleConnectExistingSharedMemoryModule:: shmat [%d]!\n",mdl.id);
			return(NULL);
		}
	}
	// 2007/10/26修改结束

	if (!mdl.newCreated)
	{
		++(pmdl->users);
		pmdl->newCreated = 0;
	}
	else	// 新建
	{		
		memcpy(pmdl,&mdl,sizeof(mdl));
		pmdl->users = 1;
		pmdl->readingLocks = 0;
		pmdl->writingLocks = 0;
		pmdl->sizeOfUserSpace = sizeOfUserSpace;
	}
	if (sizeOfUserSpace == 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
	if ((pmdl->newCreated) && (pmdl->puserSpace))
		memset(pmdl->puserSpace,0,sizeOfUserSpace);

	UppleAddIntoSharedMemoryGroup(pmdl);	// 2007/10/26 增加
		
	return(pmdl);
}
	
int UppleDisconnectShareModule(PUppleSharedMemoryModule pmdl)
{
	if ((pmdl == NULL) || (pmdl == -1))
		return(0);
	if (pmdl->users >= 0)
		--pmdl->users;
	UppleDeleteFromSharedMemoryGroup(pmdl);	// 2007/10/26 增加
	pmdl = NULL;
	return(0);
}

int UppleRemoveSharedMemoryModule(char *mdlName)
{
	int				ret;
	struct shmid_ds			buf;
	int 				resID;
	TUppleSharedMemoryModule	mdl;
	
	memset(&mdl,0,sizeof(mdl));
	if ((ret = UppleReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		printf("in UppleRemoveSharedMemoryModule:: UppleReadSharedMemoryModuleDef for [%s]\n",mdlName);
		return(ret);
	}
	
	if ((resID = shmget(mdl.id,sizeof(TUppleSharedMemoryModule),0666)) == -1)
	{
		printf("in UppleRemoveAllSharedMemoryModule:: shmget [%d]\n",mdl.id);
		return(errCodeUseOSErrCode);
	}
	
	if (shmctl(resID,IPC_RMID,&buf) != 0)
	{
		printf("in UppleRemoveAllSharedMemoryModule:: shmctl IPC_RMID resID = [%d]\n",mdl.id);
		return(errCodeUseOSErrCode);
	}

	return(0);
}	

int UpplePrintSharedMemoryModuleToFileByModuleName(char *mdlName,FILE *fp)
{
	PUppleSharedMemoryModule	pmdl;

	if ((fp == NULL) || (mdlName == NULL))
		return(errCodeParameter);
	
	if ((pmdl = UppleConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		printf("in UpplePrintSharedMemoryModuleToFileByModuleName:: UppleConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	UpplePrintSharedMemoryModuleToFile(pmdl,fp);
	
	if (UppleIsNewCreatedSharedMemoryModule(pmdl))
	{
		UppleDisconnectShareModule(pmdl);
		UppleRemoveSharedMemoryModule(mdlName);
		return(0);
	}
	else
	{	
		UppleDisconnectShareModule(pmdl);
		return(0);
	}
}

int UppleApplyWritingLocks(PUppleSharedMemoryModule pmdl)
{
	int	i;

	if (pmdl == NULL)
	{
		printf("in UppleApplyWritingLocks:: null Pointer!\n");
		return(errCodeParameter);
	}

	for (i = 0; i < 100; i++)
	{
		//if ((pmdl->readingLocks <= 0) && (pmdl->writingLocks <= 0))
		if (pmdl->writingLocks <= 0)
		{
			pmdl->writingLocks++;
			if (pmdl->writingLocks > 0)
				return(0);
			pmdl->writingLocks = 1;
			return(0);
		}
		else
		{
			usleep(1000);
			continue;
		}
	}
	
	return(errCodeSharedMemoryMDL_LockMDL);
}

int UppleReleaseWritingLocks(PUppleSharedMemoryModule pmdl)
{
	int	i;

	if (pmdl == NULL)
	{
		printf("in UppleReleaseWritingLocks:: null Pointer!\n");
		return(errCodeParameter);
	}

	pmdl->writingLocks--;
	if (pmdl->writingLocks >= 0)
		return(0);
		
	pmdl->writingLocks = 0;
	return(0);
}

int UppleApplyReadingLocks(PUppleSharedMemoryModule pmdl)
{
	int	i;

	if (pmdl == NULL)
	{
		printf("in UppleApplyReadingLocks:: null Pointer!\n");
		return(errCodeParameter);
	}

	for (i = 0; i < 100; i++)
	{
		if (pmdl->writingLocks <= 0)
		{
			pmdl->readingLocks++;
			if (pmdl->readingLocks > 0)
				return(0);
			pmdl->readingLocks = 1;
			return(0);
		}
		else
		{
			usleep(1000);
			continue;
		}
	}
	
	return(errCodeSharedMemoryMDL_LockMDL);
}

int UppleReleaseReadingLocks(PUppleSharedMemoryModule pmdl)
{
	int	i;

	if (pmdl == NULL)
	{
		printf("in UppleReleaseReadingLocks:: null Pointer!\n");
		return(errCodeParameter);
	}

	pmdl->readingLocks--;
	if (pmdl->readingLocks >= 0)
		return(0);
	pmdl->readingLocks = 0;
	return(0);
}

int UppleResetWritingLocks(char *mdlName)
{
	PUppleSharedMemoryModule	pmdl;

	if (mdlName == NULL)
		return(errCodeParameter);
	
	if ((pmdl = UppleConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		printf("in UppleResetWritingLocks:: UppleConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	pmdl->writingLocks = 0;
	
	if (UppleIsNewCreatedSharedMemoryModule(pmdl))
	{
		UppleDisconnectShareModule(pmdl);
		UppleRemoveSharedMemoryModule(mdlName);
		return(0);
	}
	else
	{	
		UppleDisconnectShareModule(pmdl);
		return(0);
	}
}

int UppleResetReadingLocks(char *mdlName)
{
	PUppleSharedMemoryModule	pmdl;

	if (mdlName == NULL)
		return(errCodeParameter);
	
	if ((pmdl = UppleConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		printf("in UppleResetReadingLocks:: UppleConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	pmdl->readingLocks = 0;
	
	if (UppleIsNewCreatedSharedMemoryModule(pmdl))
	{
		UppleDisconnectShareModule(pmdl);
		UppleRemoveSharedMemoryModule(mdlName);
		return(0);
	}
	else
	{	
		UppleDisconnectShareModule(pmdl);
		return(0);
	}
}

int UppleGetUserIDOfSharedMemoryModule(PUppleSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
		return(errCodeParameter);
	return(pmdl->id);
}

int UppleReloadSharedMemoryModule(char *mdlName)
{
	int				ret;
	TUppleSharedMemoryModule	mdl;
	char				command[512];
	
	memset(&mdl,0,sizeof(mdl));
	if ((ret = UppleReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		printf("in UppleReloadSharedMemoryModule:: UppleReadSharedMemoryModuleDef for [%s]\n",mdlName);
		return(ret);
	}
	if (strlen(mdl.reloadCommand) == 0)
		return(0);
	strcpy(command,mdl.reloadCommand);
	UppleToUpperCase(command);
	if (strcmp(command,"NULL") == 0)
		return(0);
	sprintf(command,"%s -reload",mdl.reloadCommand);
	printf("in UppleReloadAllSharedMemoryModule:: command = [%s]\n",command);
	return(system(command));
}	

int UppleReloadAllSharedMemoryModule(char *anywayOrNot)
{
	int			ret;
	struct shmid_ds		buf;
	int 			resID;
	char			fileName[512];
	char			*p;
	int			i;
	int			num;
	char			command[256];
	int			reloadWithoutConfirm = 0;
	
	if (anywayOrNot != NULL)
	{
		UppleToUpperCase(anywayOrNot);
		if (strcmp(anywayOrNot,"-RELOADALL") == 0)
			reloadWithoutConfirm = 0;
		else if (strcmp(anywayOrNot,"-RELOADALLANYWAY") == 0)
			reloadWithoutConfirm = 1;
		else
		{
			printf("in UppleReloadAllSharedMemoryModule:: wrong command [%s]\n",anywayOrNot);
			return(0);
		}
	}
	
	memset(fileName,0,sizeof(fileName));
	UppleGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf("in UppleReloadAllSharedMemoryModule:: UppleInitEnvi!\n");
		return(ret);
	}
	
	for (i = 0,num = 0; i < UppleGetEnviVarNum(); i++)
	{
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
			continue;
		strcpy(command,p);
		if (strlen(p) == 0)
			continue;
		UppleToUpperCase(command);
		if (strcmp(command,"NULL") == 0)
			continue;		
		if (strstr(p,"-reload") == NULL)
		{
			if (reloadWithoutConfirm)
				sprintf(command,"%s -reloadanyway",p);
			else
				sprintf(command,"%s -reload",p);
		}
		else
			sprintf(command,"%s",p);
		printf("in UppleReloadAllSharedMemoryModule:: command = [%s]\n",command);
		system(command);
		num++;
	}

	UppleClearEnvi();
	
	return(num);
}
// 2006/12/27 增加以下函数
PUppleSharedMemoryModule UppleConnectExistedSharedMemoryModule(char *mdlName)
{
	int				ret;
	TUppleSharedMemoryModule	mdl;
	int				resID;
	PUppleSharedMemoryModule	pmdl;
	int				i;
	
	// 20071026增加	
	if (UppleIsSharedMemoryGroupIsFull())
	{
		UppleFreeUnnecessarySharedMemory();
		if (UppleIsSharedMemoryGroupIsFull())
		{
			printf("in UppleConnectSharedMemoryModule:: shared memory group is full!\n");
			return(NULL);
		}
	}
	// 20071026增加结束

	memset(&mdl,0,sizeof(mdl));
	if ((ret = UppleReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		printf("in UppleConnectExistedSharedMemoryModule:: UppleReadSharedMemoryModuleDef [%s]!\n",mdlName);
		return(NULL);
	}
	
	if (mdl.id <= 0)
	{
		printf("in UppleConnectExistedSharedMemoryModule:: mdl.id = [%d] Error!\n",mdl.id);
		return(NULL);
	}
	
	if ((resID = shmget(mdl.id,sizeof(TUppleSharedMemoryModule),0666)) == -1)
	{
		//UppleAuditLog("in UppleConnectExistedSharedMemoryModule:: shmget [%d]!\n",mdl.id);
		return(NULL);
	}
	mdl.newCreated = 0;
		
	// 2007/10/26修改以下代码
	if (((pmdl = (PUppleSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL) || (pmdl == -1))
	{
		if (errno == 24)
		{
			UppleFreeUnnecessarySharedMemory();
			pmdl = (PUppleSharedMemoryModule)shmat(resID,0,SHM_RND);
		}
		if ((pmdl == NULL) || (pmdl == -1))
		{
			printf("in UppleConnectExistingSharedMemoryModule:: shmat [%d]!\n",mdl.id);
			return(NULL);
		}
	}
	// 2007/10/26修改结束
	
	if (pmdl->sizeOfUserSpace <= 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
	
	UppleAddIntoSharedMemoryGroup(pmdl);	// 2007/10/26 增加

	return(pmdl);
}

// 2006/12/27 增加
int UpplePrintExistedSharedMemoryModuleToFileByModuleName(char *mdlName,FILE *fp)
{
	PUppleSharedMemoryModule	pmdl;
	int				index;
	char				indexBuf[40+1];
	char				binDataBuf[100+1];
	char				ascDataBuf[100+1];
	int				offset = 0;
	
	if ((fp == NULL) || (mdlName == NULL))
		return(errCodeParameter);
	
	if ((pmdl = UppleConnectExistedSharedMemoryModule(mdlName)) == NULL)
	{
		printf("in UpplePrintExistedSharedMemoryModuleToFileByModuleName:: UppleConnectExistedSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	fprintf(fp,"\n********************************************************************************\n");	
	UpplePrintSharedMemoryModuleToFile(pmdl,fp);
	
	memset(ascDataBuf,0,sizeof(ascDataBuf));
	memset(binDataBuf,0,sizeof(binDataBuf));
	sprintf(indexBuf,"%08Xh:",0);
	for (index = 0,offset = 0; index < pmdl->sizeOfUserSpace; index++)
	{
		sprintf(binDataBuf+offset*3,"%02X ",pmdl->puserSpace[index]);
		if ((pmdl->puserSpace[index] <= 0) || (pmdl->puserSpace[index] >= 128))
			ascDataBuf[offset] = '.';
		else
			ascDataBuf[offset] = pmdl->puserSpace[index];
		offset++;
		if ((index+1) % 16 == 0)
		{
			fprintf(fp,"%s %s; %s\n",indexBuf,binDataBuf,ascDataBuf);
			memset(ascDataBuf,0,sizeof(ascDataBuf));
			memset(binDataBuf,0,sizeof(binDataBuf));
			sprintf(indexBuf,"%08Xh:",index+1);
			offset = 0;
		}
	}
	if (index % 16 != 0)
	{
		for (; offset < 16; offset++)
			memcpy(binDataBuf+offset*3,"   ",3);
		fprintf(fp,"%s %s; %s\n",indexBuf,binDataBuf,ascDataBuf);
	}
	fprintf(fp,"********************************************************************************\n");	
	return(0);
}

// 2006/12/27增加
int UpplePrintAllExistedSharedMemoryModule(char *outputFile)
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i;
	FILE			*fp;
	char			mdlNameGrp[200][64+1];
	int			mdlNum;
	int			num = 0;
		
	memset(fileName,0,sizeof(fileName));
	UppleGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UppleInitEnvi(fileName)) < 0)
	{
		printf("in UpplePrintAllExistedSharedMemoryModule:: UppleInitEnvi!\n");
		return(ret);
	}
	for (i = 0,mdlNum = 0; (i < UppleGetEnviVarNum()) && (mdlNum < sizeof(mdlNameGrp)); i++)
	{
		if ((p = UppleGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
			continue;
		memset(mdlNameGrp[mdlNum],0,sizeof(mdlNameGrp[mdlNum]));
		strcpy(mdlNameGrp[mdlNum],p);
		mdlNum++;
	}
	UppleClearEnvi();
	if (strcmp(outputFile,"stderr") == 0)
		fp = stderr;
	else if (strcmp(outputFile,"stdout") == 0)
		fp = stdout;
	else
	{
		if ((fp = fopen(outputFile,"w")) == NULL)
		{
			printf("in UpplePrintAllExistedSharedMemoryModule:: UppleInitEnvi!\n");
			return(errCodeUseOSErrCode);
		}
	}
		
	for (i = 0; i < mdlNum; i++)
	{
		if ((ret = UpplePrintExistedSharedMemoryModuleToFileByModuleName(mdlNameGrp[i],fp)) < 0)
			printf("Print [%s] Failure! ret = [%d]\n",mdlNameGrp[i],ret);
		else
		{
			printf("Print [%s] OK!\n",mdlNameGrp[i]);
			num++;
		}
	}
	fclose(fp);
	return(num);
}
int UppleReadingLockModuleByModuleName(char *mdlName)
{
	PUppleSharedMemoryModule	pmdl;
	int				ret;

	if (mdlName == NULL)
		return(errCodeParameter);
	
	if ((pmdl = UppleConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		printf("in UppleReadingLockModuleByModuleName:: UppleConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	return(UppleApplyReadingLocks(pmdl));
}

int UppleWritingLockModuleByModuleName(char *mdlName)
{
	PUppleSharedMemoryModule	pmdl;
	int				ret;

	if (mdlName == NULL)
		return(errCodeParameter);
	
	if ((pmdl = UppleConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		printf("in UppleWritingLockModuleByModuleName:: UppleConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	return(UppleApplyWritingLocks(pmdl));
}



PUppleSharedMemoryModule UppleConnectSharedMemoryModule_Redis(char *mdlName,long sizeOfUserSpace)
{
	int				ret;
	TUppleSharedMemoryModule	mdl;
	int				resID;
	PUppleSharedMemoryModule	pmdl;
	int				i;
/*
	char SName[ 128 ];

//打包相关函数理论上是只读的，可以不用共享内存;
//计划将打包相关函数内的共享内存去掉，改为用进程内存和REDIS存贮
	memset( SName, 0x00, sizeof(SName) );
	sprintf( SName, "#%d.%s", getpid(), mdlName );

	ret = GET( SName, &pmdl );
	if( ret > 0 )
	    {
#ifdef DEBUG_REDIS_MEM
		UppleLog2( __FILE__,__LINE__, "REDIS_MEM:GET SName=[%s], pmdl=[%ld].\n", SName, (long)pmdl);
#endif
		//已经加载过，不用再次加载了。
		pmdl->newCreated = 0;
		return pmdl;	//进程内已经分配过的模块，直接返回
	    }
*/

	//使用进程内存
	pmdl =  (PUppleSharedMemoryModule)malloc( sizeof(TUppleSharedMemoryModule)+sizeOfUserSpace+1 );

	memset(pmdl,0,sizeof(*pmdl));
	strcpy(pmdl->name,mdlName);
	strcpy(pmdl->reloadCommand,"COMMAND_REDIS_TEST");
	//pmdl->index = i;
	pmdl->index = 0;
	pmdl->sizeOfUserSpace = 0;
	pmdl->puserSpace = NULL;
	pmdl->users = 0;
	pmdl->readingLocks = 0;
	pmdl->writingLocks = 0;

	pmdl->users = 1;
	pmdl->sizeOfUserSpace = sizeOfUserSpace;

	if (sizeOfUserSpace == 0)
		pmdl->puserSpace = NULL;
	else
		//应用内存指向结构的尾端
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));

	memset(pmdl->puserSpace,0,sizeOfUserSpace);

/*
	SET( SName, &pmdl, sizeof(pmdl) );
#ifdef DEBUG_REDIS_MEM
    	UppleLog2( __FILE__,__LINE__, "REDIS_MEM:SET SName=[%s], pmdl=[%ld].\n", SName, (long)pmdl);
#endif
*/

	//每次都要加载配置？
	pmdl->newCreated = 1;
	//pmdl->newCreated = 0;   //TEST

	return(pmdl);
	
}

unsigned char *UppleGetAddrOfSharedMemoryModuleUserSpace_Redis(PUppleSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
		return(NULL);
	return(pmdl->puserSpace);
}

int UppleIsNewCreatedSharedMemoryModule_Redis(PUppleSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
		return(0);
	return(pmdl->newCreated);
}

int UppleRemoveSharedMemoryModule_Redis(char *mdlName)
{
	int				ret;
	struct shmid_ds			buf;
	int 				resID;
	TUppleSharedMemoryModule	mdl;
	PUppleSharedMemoryModule	pmdl;
	char SName[ 128 ];
	
	memset( SName, 0x00, sizeof(SName) );
	sprintf( SName, "#%d.%s", getpid(), mdlName );

	//ret = GET( SName, &pmdl );
	if( ret > 0 )
	    {
#ifdef DEBUG_REDIS_MEM
		UppleLog2( __FILE__,__LINE__, "REDIS_MEM:DEL SName=[%s], pmdl=[%ld].\n", SName, (long)pmdl);
#endif
		//进程内已经分配过的模块，清除
		if( pmdl )
			free( pmdl );
		//DEL( SName );
		return 0;
	    }

	return -1;
}	
