#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "upplePackage.h"

#define Mysimu
//#define _DEBUG__

PUpplePackage		pupplePackage = NULL;
void print_buf_hex( unsigned char *buf, int len ) // ó?óú′òó?×?·?
{
        int i;
        for ( i = 0; i < len; i++ ){
#ifdef _DEBUG__
                printf( "%02x" , buf[i] );
#endif
        }
}
/*
*	pack8583Def:基础8583包定义
*	packDef: 交易各个域的定义
*	buf:结果
*	return : 长度
*/

int Pack8583(char *pack8583Def, char *packDef,char *buf,int sizeofbuf)
{
	int	ret;
	char	fldsValueFile[512];
	FILE	*fp = NULL;
	unsigned char	tmpBuf[2048+1];
	int	fldIndex;
	int	len;

	sprintf(fldsValueFile,"cfg/%s",packDef);
	if ((fp = fopen(fldsValueFile,"r")) == 0)
	{
#ifdef _DEBUG__
		printf("fopen %s error!\n",fldsValueFile);
#endif
		return -1;
	}
#ifdef _DEBUG__
	printf("test %d \n",__LINE__);
#endif
	if ((pupplePackage = UppleConnectPackage(pack8583Def)) == NULL)
	{
#ifdef _DEBUG__
		printf("UppleConnectPackage %s error!\n",pack8583Def);
#endif
		return -1;
	}

	while(!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		fldIndex = -1;
		fscanf(fp,"%d,%s",&fldIndex,tmpBuf);
		//printf("line: %d fldIndex = [%d],tmpBuf=[%s]\n  ",__LINE__,fldIndex,tmpBuf);
		if ((fldIndex < 0) || (strlen((char *)tmpBuf) == 0))
			continue;
		if ((ret = UppleSetPackageFld(pupplePackage,fldIndex,(char *)tmpBuf,strlen((char *)tmpBuf))) < 0){
#ifdef _DEBUG__
			printf("UppleSetPackageFld [%04d] [%s] error!\n",fldIndex,tmpBuf);
#endif
		}else{
#ifdef _DEBUG__
			//printf("UppleSetPackageFld [%04d] [%s] OK!\n",fldIndex,tmpBuf);
#endif
		}
	}
	fclose(fp);
	fp = NULL;
	if ((len = UpplePack(pupplePackage,tmpBuf,sizeof(tmpBuf) )) < 0)
	{
#ifdef _DEBUG__
		printf("UpplePack error!\n");
#endif
		return -1;
	}else{
		//print_buf_hex(tmpBuf,len );
		len = bcdhex_to_aschex(tmpBuf,len,buf );
		//memcpy(buf,tmpBuf,len);
		return len;
	}
	

	return 0;
}
int IsPackageConnect()
{
	if(!UppleIsValidPackage(pupplePackage))
		return 0;
	else
		return 1;
}


int packageInit( char *path,char *filename )
{
/*
	if(IsPackageConnect())
		return 0;
*/
#ifdef _DEBUG__
	printf("sdsds\n");
#endif

	//if ((pupplePackage = UppleConnectPackage("term8583.Def")) == NULL)
	if ((pupplePackage = UppleConnectPackageNew(path, filename )) == NULL)
	{
#ifdef _DEBUG__
		printf("UppleConnectPackage term8583 error!\n");
#endif
		return -1;
	}else{
		return 0;
	}
}

int packageSet(int fldIndex,char *tmpBuf )
{
	int ret = 0 ;
	int len = 0;
	char hex[1024 + 1] = {0};

	if ((ret = UppleSetPackageFld(pupplePackage,fldIndex,(char *)tmpBuf,strlen((char *)tmpBuf))) < 0){
#ifdef _DEBUG__
		printf("UppleSetPackageFld [%04d] [%s] error!\n",fldIndex,tmpBuf);
#endif
	}else{
#ifdef _DEBUG__
		printf("UppleSetPackageFld [%04d] [%s] OK!\n",fldIndex,tmpBuf);
#endif
	}
	return ret;

}
int packageSetHex(int fldIndex,char *tmpBuf )
{
	int ret = 0 ;
	int len = 0;
	char hex[1024 + 1] = {0};


	len = aschex_to_bcdhex(tmpBuf,strlen(tmpBuf),hex);

	if ((ret = UppleSetPackageFld(pupplePackage,fldIndex,hex,len)) < 0){
#ifdef _DEBUG__
		printf("UppleSetPackageFld [%04d] [%s] error!\n",fldIndex,tmpBuf);
#endif
	}else{

#ifdef _DEBUG__
		printf("UppleSetPackageFld [%04d] [%s] OK!\n",fldIndex,tmpBuf);
#endif
	}

	return ret;

}

int packageFinal(char *buf )
{
	unsigned char	tmpBuf[2048+1] = {0};
	int len = 0;
	if ((len = UpplePack(pupplePackage,tmpBuf,sizeof(tmpBuf) )) < 0)
	{
#ifdef _DEBUG__
		printf("UpplePack error!\n");
#endif
		if (pupplePackage != NULL)
			UppleDisconnectPackage(pupplePackage);
		return -1;
	}else{
		//print_buf_hex(tmpBuf,len );
		len = bcdhex_to_aschex(tmpBuf,len,buf );
		if (pupplePackage != NULL){
#ifdef _DEBUG__
			printf("disconnect package\n");
#endif
		}
		return len;
	}
}
int packageShow()
{
	char tmpBuf[1024 + 1] = {0};
	int fldIndex = 0;
	int len = 0;
	for (fldIndex = 0; fldIndex < 256; fldIndex++)                                                                                                                     
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UppleReadPackageFld(pupplePackage,fldIndex,(char *)tmpBuf,sizeof(tmpBuf))) <= 0)
		continue;
#ifdef _DEBUG__
		printf("[%04d] [%04d] [%s]\n",fldIndex,len,tmpBuf);
#endif
	}
	return 0;
}

int getFldValue(int fldIndex,char *tmpBuf,int bufLen )
{
	return UppleReadPackageFld(pupplePackage,fldIndex,(char *)tmpBuf,bufLen );
}

int unpack8583(char *filePath,char *tmpBuf,int lenBuf)
{
	int ret ;
	int len;
	char package[2048 + 1] = {0};
	char fileName[] = "term8583";

	if ((ret = packageInit(filePath,fileName )) < 0){
		return ret;
	}
#ifdef _DEBUG__
	printf("lenBuf = [%d] \n",lenBuf);
#endif

	len = aschex_to_bcdhex(tmpBuf,lenBuf,package );

	print_buf_hex(package,len );

	if ((len = UppleUnpack(pupplePackage,package,len)) < 0)
	{
#ifdef _DEBUG__
        printf("unpack8583 error!\n");
#endif
    }
	//UppleDisconnectPackage(pupplePackage);
	return len;

}
int unpackFinal()
{
	return UppleDisconnectPackage(pupplePackage);
}
int packFree()
{
	return UppleDisconnectPackage(pupplePackage);
}

