//	Author:		zhangyongding
//	Date:		20081018
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

#include "termTransFldDefTBL.h"

PUppleTaskInstance	ptaskInstance = NULL;

int UppleHelp()
{
	printf("Usage:: %s command\n",UppleGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the DDUDefTBL\n");
	printf("  -all		print all DDUDef definition\n");
	printf("  name/index\n");
	return(0);
}

int UppleTaskActionBeforeExit()
{
	UppleDisconnectTermTransFldDefTBL();
	UppleDisconnectTaskTBL();
	UppleDisconnectLogFileTBL();
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
	{
		if (!UppleConfirm("Are you sure of reloading termTransFldDefTBL?"))
			return(-1);
		if ((ret = UppleReloadTermTransFldDefTBL()) < 0)
			printf("UppleReloadTermTransFldDefTBL Error! ret = [%d]\n",ret);
		else
			printf("UppleReloadTermTransFldDefTBL OK!\n");
	}
	else if (strcmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UppleReloadTermTransFldDefTBL()) < 0)
			printf("UppleReloadTermTransFldDefTBL Error! ret = [%d]\n",ret);
		else
			printf("UppleReloadTermTransFldDefTBL OK!\n");
	}
	else if (strcmp(argv[1],"-ALL") == 0)
		UpplePrintTermTransFldDefTBLToFile(stdout);
	else if (strcmp(argv[1],"-PRINT") == 0)
		PrintTermTransFldDef(argc-2,&argv[2]);			
	else 
		PrintTermTransFldDef(argc-1,&argv[1]);			
	return(UppleTaskActionBeforeExit());
}


int PrintTermTransFldDef(int num,char *name[])
{
	int	i;
	int	ret;
	char	*p;
		
	for (i = 0; i < num; i++)
	{
		if ((ret = UpplePrintTermTransFldDefOfNameInTBLToFile(name[i],stdout)) <= 0)
		{
			if (!UppleIsDigitStr(name[i]))
			{
				printf("ddu of name [%s] not defined\n",name[i]);
				continue;
			}
			if ((ret = UpplePrintTermTransFldDefOfIndexToFile(atoi(name[i]),stdout)) < 0)
				printf("ddu of index [%s] not defined!\n",name[i]);
		}
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	p = UppleInput("Input nameOfDDU::");
	if (UppleIsQuit(p))
		return(0);
	if ((ret = UpplePrintTermTransFldDefOfNameInTBLToFile(p,stdout)) <= 0)
	{
		if (!UppleIsDigitStr(p))
		{
			printf("ddu of name [%s] not defined\n",p);
			goto loopInput;
		}
		if ((ret = UpplePrintTermTransFldDefOfIndexToFile(atoi(p),stdout)) < 0)
			printf("ddu of index [%s] not defined!\n",p);
	}
	goto loopInput;
}
