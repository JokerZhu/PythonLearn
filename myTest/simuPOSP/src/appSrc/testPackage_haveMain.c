#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "upplePackage.h"

#define Mysimu

#ifndef Mysimu
#include "uppleTask.h"


PUppleTaskInstance	ptaskInstance = NULL;
#endif
//PUpplePackage		pupplePackage = NULL;

#ifndef Mysimu
int UppleHelp()
{
	//printf("Usage:: %s packageDefName fldsValueFile\n",UppleGetApplicationName());
	return(0);
}
void print_buf_hex( unsigned char *buf, int len ) // ¨®?¨®¨²¡ä¨°¨®?¡Á?¡¤?
{
        int i;
        for ( i = 0; i < len; i++ ){
                printf( "%02x" , buf[i] );
        }
        printf("\n");
}
#endif

#ifndef Mysimu
int UppleTaskActionBeforeExit()
{
	UppleDisconnectPackageDefGroup();
	UppleDisconnectTaskTBL();
	UppleDisconnectLogFileTBL();
	exit(0);
}
#endif
int main(int argc,char *argv[])
{
#ifndef Mysimu
	int	ret;
	char	fldsValueFile[512];
	FILE	*fp = NULL;
	unsigned char	tmpBuf[2048+1];
	int	fldIndex;
	int	len;
/*		
	UppleSetApplicationName(argv[0]);
	if (argc < 3)
		return(UppleHelp());
		
	if ((ptaskInstance = UppleCreateTaskInstance(UppleTaskActionBeforeExit,"%s",UppleGetApplicationName())) == NULL)
	{
		printf("UppleCreateTaskInstance Error!\n");
		return(UppleTaskActionBeforeExit());
	}
*/
	//sprintf(fldsValueFile,"%s/tmp/%s",getenv("HOME"),argv[2]);
	sprintf(fldsValueFile,"cfg/%s",argv[2]);
	if ((fp = fopen(fldsValueFile,"r")) == 0)
	{
		printf("fopen %s error!\n",fldsValueFile);
		goto errorExit;
	}
	printf("test %d \n",__LINE__);
	if ((pupplePackage = UppleConnectPackage(argv[1])) == NULL)
	{
		printf("UppleConnectPackage %s error!\n",argv[1]);
		goto errorExit;
	}

	while(!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		fldIndex = -1;
		fscanf(fp,"%d,%s,",&fldIndex,tmpBuf);
		printf("line: %d fldIndex = [%d],tmpBuf=[%s]\n  ",__LINE__,fldIndex,tmpBuf);
		if ((fldIndex < 0) || (strlen((char *)tmpBuf) == 0))
			continue;
		if ((ret = UppleSetPackageFld(pupplePackage,fldIndex,(char *)tmpBuf,strlen((char *)tmpBuf))) < 0)
			printf("UppleSetPackageFld [%04d] [%s] error!\n",fldIndex,tmpBuf);
		else
			printf("UppleSetPackageFld [%04d] [%s] OK!\n",fldIndex,tmpBuf);
	}
	fclose(fp);
	fp = NULL;
	if ((len = UpplePack(pupplePackage,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		printf("UpplePack error!\n");
		goto errorExit;
	}
	print_buf_hex(tmpBuf,len );
	printf("len = [%d]\n",len);
	//sprintf(fldsValueFile,"%s/tmp/%s.bits",getenv("HOME"),argv[2]);
	sprintf(fldsValueFile,"cfg/%s.bits",argv[2]);
	if ((fp = fopen(fldsValueFile,"w")) == 0)
	{
		printf("fopen %s error!\n",fldsValueFile);
		goto errorExit;
	}
	fwrite(tmpBuf,len,1,fp);
	fclose(fp);
	fp = NULL;
	
	if ((len = UppleUnpack(pupplePackage,tmpBuf,len)) < 0)
	{
		printf("UppleUnpack error!\n");
		goto errorExit;
	}
	
	for (fldIndex = 0; fldIndex < 256; fldIndex++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UppleReadPackageFld(pupplePackage,fldIndex,(char *)tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		printf("[%04d] [%04d] [%s]\n",fldIndex,len,tmpBuf);
	}
	
errorExit:
	if (fp != NULL)
		fclose(fp);
	if (pupplePackage != NULL)
		UppleDisconnectPackage(pupplePackage);
#ifndef Mysimu
	UppleTaskActionBeforeExit();
#endif
#endif
	char tmpBuf[1024 * 2 + 1] = {0};
	char tmp[1024 * 2 + 1] = {0};
	int ret = 0;
	int len = 0;
	int i = 0;

	if((ret = packageInit("./cfg/","term8583" )) < 0 ){
		printf("package init error \n");
		return ret;
	}
	if((ret = packageSet(0,"0200" )) < 0 ){
		printf("package init error \n");
		return ret;
	}
	if((ret = packageSet(2,"622637485398493" )) < 0 ){
		printf("package init error \n");
		return ret;
	}
	if((ret = packageSet(49,"156" )) < 0 ){
		printf("package init error \n");
		return ret;
	}

	if((ret = packageSet(64,"00000000" )) < 0 ){
		printf("package init error \n");
		return ret;
	}

	if((ret = packageSet(62,"08F859DB0F61C4FCBC15AF3642172861" )) < 0 ){
		printf("package init error \n");
		return ret;
	}
	for(i =0;i < 256;i++){
		if((len = getFldValue(i,tmp,sizeof(tmp))) <=0)
			continue;
		printf("[%04d][%04d] [%s]\n",i,len,tmp );
	}


	//packageShow();
	len = packageFinal(tmpBuf);
	printf("tmpBuf = [%s]\n",tmpBuf);


	len = unpack8583("./cfg/",tmpBuf,len );
	printf("len = [%d]\n",len);





	return(0);
}	
	

