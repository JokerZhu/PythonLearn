//	Author:		zhangyongding
//	Date:		20081020
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "uppleLog.h"
#include "uppleCommand.h"
#include "uppleVersion.h"
#include "uppleTask.h"

#include "termTransDefTBL.h"

PUppleTaskInstance	ptaskInstance = NULL;

int UppleHelp()
{
	printf("Usage:: %s command\n",UppleGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the transClassDefTBL\n");
	printf("  -all		print all transClassDef definition\n");
	printf("  -print fld000/fld003\n");
	printf("  fld000/fld003\n");
	return(0);
}

int UppleTaskActionBeforeExit()
{
	UppleDisconnectTermTransDefTBL();
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
		if (!UppleConfirm("Are you sure of reloading TermTransDefTBL?"))
			return(-1);
		if ((ret = UppleReloadTermTransDefTBL()) < 0)
			printf("UppleReloadTermTransDefTBL Error! ret = [%d]\n",ret);
		else
			printf("UppleReloadTermTransDefTBL OK!\n");
	}
	else if (strcmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UppleReloadTermTransDefTBL()) < 0)
			printf("UppleReloadTermTransDefTBL Error! ret = [%d]\n",ret);
		else
			printf("UppleReloadTermTransDefTBL OK!\n");
	}
	else if (strcmp(argv[1],"-ALL") == 0)
		UpplePrintTermTransDefTBLToFile(stdout);
	else if (strcmp(argv[1],"-PRINT") == 0)
		PrintTransDef(argc-2,&argv[2]);			
	else
		PrintTransDef(argc-1,&argv[1]);	
	return(UppleTaskActionBeforeExit());
}

int PrintTransDef(int num,char *vars[])
{
	int	i;
	int	ret;
	char	*p;
		
	for (i = 0; i < num; i++)
	{
		p = vars[i];
		if ((ret = UpplePrintTermTransDefOfFld000OrFld003ToFile(p,stdout)) < 0)
			printf("UpplePrintTermTransDefOfFld000OrFld003ToFile [%s] Error! ret = [%d]\n",p,ret);
	}
	
	if (num > 0)
		return(0);
	
loopInput:
	p = UppleInput("Input fld000/fld003::");
	if (UppleIsQuit(p))
		return(0);
	if ((ret = UpplePrintTermTransDefOfFld000OrFld003ToFile(p,stdout)) < 0)
		printf("UpplePrintTermTransDefOfFld000OrFld003ToFile [%s] Error! ret = [%d]\n",p,ret);
	goto loopInput;
}
