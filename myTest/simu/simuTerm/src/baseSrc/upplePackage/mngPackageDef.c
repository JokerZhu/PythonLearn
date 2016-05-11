//	Author:		zhangyongding
//	Date:		20081014
//	Version:	1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "uppleLog.h"
#include "uppleCommand.h"
#include "uppleVersion.h"
#include "uppleTask.h"

#include "upplePackageDef.h"

PUppleTaskInstance	ptaskInstance = NULL;

int UppleHelp()
{
	printf("Usage:: %s command\n",UppleGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reloadall	realod all the packageDef\n");
	printf("  -reload name	reload the specified packageDef\n");
	printf("  -all	 	print all the packageDef\n");
	printf("  -print name	print specified packageDef\n");
	printf("  -vi name	vi specified packageDef\n");
	return(0);
}

int UppleTaskActionBeforeExit()
{
	UppleDisconnectPackageDefGroup();
	UppleDisconnectTaskTBL();
	UppleDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	*p;
	
	UppleSetApplicationName(argv[0]);
	if (argc < 2)
		return(UppleHelp());
		
	if ((ptaskInstance = UppleCreateTaskInstance(UppleTaskActionBeforeExit,"%s",UppleGetApplicationName())) == NULL)
	{
		printf("UppleCreateTaskInstance Error!\n");
		return(UppleTaskActionBeforeExit());
	}

	if (UppleExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UppleTaskActionBeforeExit());
	
	if (strcmp(argv[1],"-RELOAD") == 0)
		ReloadSpecifiedPackageDef(1,argc-2,&argv[2]);
	if (strcmp(argv[1],"-RELOADANYWAY") == 0)
		ReloadSpecifiedPackageDef(0,argc-2,&argv[2]);
	else if (strcmp(argv[1],"-RELOADALL") == 0)
	{
		if (UppleConfirm("Are you sure of reloading all packageDef?"))
		{
			if ((ret = UppleReloadPackageDefGroup()) < 0)
				printf("UppleReloadPackageDefGroup Failure! ret = [%d]\n",ret);
			else
				printf("UppleReloadPackageDefGroup OK!\n");
		}
	}
	else if (strcmp(argv[1],"-RELOADALLANYWAY") == 0)
	{
		if ((ret = UppleReloadPackageDefGroup()) < 0)
			printf("UppleReloadPackageDefGroup Failure! ret = [%d]\n",ret);
		else
			printf("UppleReloadPackageDefGroup OK!\n");
	}
	else if (strcmp(argv[1],"-ALL") == 0)
		UpplePrintPackageDefGroupToFile(stdout);
	else if (strcmp(argv[1],"-PRINT") == 0)
		PrintSpecifiedPackageDef(argc-2,&argv[2]);			
	else if (strcmp(argv[1],"-VI") == 0)
		ViSpecifiedPackageDef(argc-2,&argv[2]);			
	else
		PrintSpecifiedPackageDef(argc-1,&argv[1]);	
	return(UppleTaskActionBeforeExit());
}

int PrintSpecifiedPackageDef(int num,char *name[])
{
	int	i;
	int	ret;
	char	*p;
	char	tmpname[100];
		
	for (i = 0; i < num; i++)
	{
		if ((ret = UpplePrintSpecifiedPackageDefToFile(name[i],stdout)) < 0)
			printf("UpplePrintSpecifiedPackageDefToFile [%s] Error! ret = [%d]\n",name[i],ret);
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	p = UppleInput("\n\nInput packageName::");
	strcpy(tmpname,p);
	if (UppleIsQuit(p))
		return(0);
	if ((ret = UpplePrintSpecifiedPackageDefToFile(tmpname,stdout)) < 0)
		printf("UpplePrintSpecifiedPackageDefToFile [%s] Error! ret = [%d]\n",tmpname,ret);
	goto loopInput;
}

int ReloadSpecifiedPackageDef(int confirmNeeded,int num,char *name[])
{
	int	i;
	int	ret;
	char	*p;
	char	tmpname[100];
		
	for (i = 0; i < num; i++)
	{
		if (confirmNeeded)
		{
			if (!UppleConfirm("Are you sure of reloading packageDef for [%s]",name[i]))
				continue;
		}
		if ((ret = UppleReloadPackageDef(name[i])) < 0)
			printf("UppleReloadPackageDef [%s] Error! ret = [%d]\n",name[i],ret);
		else
			printf("UppleReloadPackageDef [%s] OK!\n",name[i]);
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	p = UppleInput("\n\nInput packageName::");
	strcpy(tmpname,p);
	if (UppleIsQuit(p))
		return(0);
	if (confirmNeeded)
	{
		if (!UppleConfirm("Are you sure of reloading packageDef for [%s]",tmpname))
			goto loopInput;
	}
	if ((ret = UppleReloadPackageDef(tmpname)) < 0)
		printf("UppleReloadPackageDef [%s] Error! ret = [%d]\n",tmpname,ret);
		else
			printf("UppleReloadPackageDef [%s] OK!\n",tmpname);
	goto loopInput;
}
int ViSpecifiedPackageDef(int num,char *name[])
{
	int	i;
	int	ret;
	char	*p;
	char	tmpname[512];
	char	command[1024];	
	for (i = 0; i < num; i++)
	{
		memset(tmpname,0,sizeof(tmpname));
		UppleGetConfFileNameOfPackageDef(name[i],tmpname);
		sprintf(command,"vi %s",tmpname);
		system(command);
		system("clear");
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	p = UppleInput("\n\nInput packageName::");
	memset(tmpname,0,sizeof(tmpname));
	UppleGetConfFileNameOfPackageDef(p,tmpname);
	if (UppleIsQuit(p))
		return(0);
	sprintf(command,"vi %s",tmpname);
	system(command);
	system("clear");
	goto loopInput;
}
