/*	create by zwb 	*/
/*	2014-03-18	*/
#include "strUnPcakKeyValue.h"
#include "platFormEnv.h"
#include "platFormPackage.h"
#include "uppleErrCode.h"
#include "specErrCode.h"

/*1、创建一个首节点*/
int createListHead(KeyValue *pKeyValueHead,char *ValueName,char *Value)
{
		if (pKeyValueHead == NULL)
		{
			//pKeyValueHead = (KeyValue *)malloc(sizeof(KeyValue));
			return -1;
		}
		if (pKeyValueHead)
		{
			memset(pKeyValueHead,0,sizeof(KeyValue));
			pKeyValueHead->next = NULL;
			pKeyValueHead->Value = NULL;
			pKeyValueHead->ValueName = NULL;
			int len = 0;
			int len_v = 0;
			len = strlen(ValueName);
			len_v = strlen(Value);
			pKeyValueHead->Value = (char *)malloc(len_v+1);
			memset(pKeyValueHead->Value,0,len_v+1);
			pKeyValueHead->ValueName = (char *)malloc(len+1);
			memset(pKeyValueHead->ValueName,0,len+1);
			memcpy(pKeyValueHead->Value,Value,len_v);
			memcpy(pKeyValueHead->ValueName,ValueName,len);
		}
		return 0;
			
}

/*2、增加一个节点*/
int addListNode(KeyValue *pKeyValueHead,char *ValueName,char *Value)
{
	if ( ValueName == NULL || Value == NULL)
	{
		return -1;
	}
	struct KeyValue *pNext = NULL;
	struct KeyValue *pTmp = NULL;
	pNext = pKeyValueHead;
	while(pNext->next != NULL)
	{
		pNext=pNext->next;
	}
	pTmp = (KeyValue *)malloc(sizeof(KeyValue));
	int len = strlen(ValueName);
	int len_v = strlen(Value);
	pTmp->ValueName = (char *)malloc(len +1);
	pTmp->Value = (char *)malloc(len_v +1);
	memset(pTmp->ValueName,0,len+1);
	memset(pTmp->Value,0,len_v +1);
	memcpy(pTmp->ValueName,ValueName,len);
	memcpy(pTmp->Value,Value,len_v);
	pTmp->next = NULL;
	pNext->next = pTmp;
	return 0;
}

/*3、删除一个节点*/
int deteteListNode(KeyValue *pKeyValueHead,char *ValueName)
{
	struct KeyValue *pNext = NULL;
	struct KeyValue *pTmp = NULL;
	struct KeyValue *pTmp2 = NULL;
	if (pKeyValueHead == NULL || ValueName == NULL)
	{
		return -1;
	}
	if (pKeyValueHead->next == NULL)
	{
		if (!(memcmp(pKeyValueHead->ValueName,ValueName,strlen(pKeyValueHead->ValueName))))
		{
			if(pKeyValueHead->Value != NULL)
			{
				free(pKeyValueHead->Value);
				pKeyValueHead->Value = NULL;
			}
			if(pKeyValueHead->ValueName != NULL)
			{
				free(pKeyValueHead->ValueName);
				pKeyValueHead->ValueName =NULL;
			}
			if(pKeyValueHead != NULL)
			{
				free(pKeyValueHead);
				pKeyValueHead = NULL;
			}
			return 0;
		}
	}
	else if ( (pKeyValueHead->next !=NULL) &&(!memcmp(pKeyValueHead->ValueName,ValueName,strlen(pKeyValueHead->ValueName))))
	{
		pNext = pKeyValueHead;
		pKeyValueHead  = pKeyValueHead->next;
		if(pNext->Value != NULL)
		{
			free(pNext->Value);
			pNext->Value = NULL;
		}
		if(pNext->ValueName != NULL)
		{
			free(pNext->ValueName);
			pNext->ValueName = NULL;
		}
		if(pNext != NULL)
		{
			free(pNext);
			pNext = NULL;
		}
		return 1;
	}
	else
	{
		pNext = pKeyValueHead;
		for(;pNext != NULL;pNext = pNext->next)
		{
			if ( (memcmp(pNext->ValueName,ValueName,strlen(pNext->ValueName))))
			{
				if (pNext->next != NULL)
				{
					if (!(memcmp(pNext->next->ValueName,ValueName,strlen(pNext->next->ValueName))))
					{
						pTmp = pNext->next;
						pNext->next = pTmp->next;
						if(pTmp->Value != NULL)
						{
							free(pTmp->Value);
							pTmp->Value = NULL;
						}
						if(pTmp->ValueName != NULL)
						{
							free(pTmp->ValueName);
							pTmp->ValueName = NULL;
						}
						if(pTmp != NULL)
						{
							free(pTmp);
							pTmp = NULL;
						}
						return 2;
					}
				}
				else
				{
					return 3;
				}
			}
		
		}
	}
	return 3;
}
/*4、获得某个特定节点的值*/
int getListValue(KeyValue *pKeyValueHead,char *ValueName,char *Value)
{
	struct KeyValue *pNext;
	if (pKeyValueHead == NULL || ValueName == NULL)
	{
		return -1;
	}
	pNext = pKeyValueHead;
	for (;pNext != NULL; pNext = pNext->next)
	{
		if (!(memcmp(pNext->ValueName,ValueName,strlen(ValueName))))
		{
			if(pNext->Value != NULL)
			{
				memcpy(Value,pNext->Value,strlen(pNext->Value));
			}
			return 0;
		}
	}
	return 1;
}

/*5、清除整个链表(单项链表需正向删除)*/
int DropList(KeyValue *pKeyValueHead)
{
	struct KeyValue *pNext,*pTmp;
	if (pKeyValueHead == NULL)
	{
		UppleLog2(__FILE__,__LINE__,"DropList pKeyValueHead == null\n");
		return 0;
	}
	if(pKeyValueHead->next == NULL)
	{
		UppleLog2(__FILE__,__LINE__,"唯一节点\n");
		if(pKeyValueHead->Value != NULL)
		{	
			free(pKeyValueHead->Value);
			pKeyValueHead->Value = NULL;
		}
		if(pKeyValueHead->ValueName != NULL)
		{
			free(pKeyValueHead->ValueName);
			pKeyValueHead->ValueName = NULL;
		}
		if(pKeyValueHead != NULL)
		{
			free(pKeyValueHead);
			pKeyValueHead = NULL;
		}
		
		UppleLog2(__FILE__,__LINE__,"唯一节点删除完成\n");
		return 0;
	}
	else
	{
		
		UppleLog2(__FILE__,__LINE__,"多个节点\n");
		while(pKeyValueHead != NULL)
		{
			pTmp = pKeyValueHead;
			pKeyValueHead = pKeyValueHead->next;
			if(pTmp->Value != NULL)
			{
				free(pTmp->Value);
				pTmp->Value = NULL;
			}
			if(pTmp->ValueName != NULL)
			{
				free(pTmp->ValueName);
				pTmp->ValueName = NULL;
			}
			if(pTmp != NULL)
			{
				free(pTmp);
				pTmp = NULL;
			}
		}
		UppleLog2(__FILE__,__LINE__,"多节点删除完成\n");
		return 0;
	}
	return -1;
}

/*6、解包字符串到结构体*/
int UnPackStrtoKeyValue(char *input_buffer,KeyValue *pKeyHead)
{
	if(input_buffer == NULL || pKeyHead == NULL)
	{
		return -1;
	}
	char *pbuffer = NULL;
	char *name,*value,*next;
	pbuffer = input_buffer;
	value = input_buffer;
	int count = 0;
	int offset = 0;
	for(;offset < strlen(input_buffer); offset++)
	{
		if(!(memcmp(pbuffer+offset,"=",1)))
		{
			count++;
		}
	}
	int i = 0;
	for(;i <  count; i++)
	{
		name = strsep(&value,"=");
		next =value;
		value=strsep(&next,"&");
		if( 0 == i)
		{
			createListHead(pKeyHead,name,value);
		}
		else
		{
			addListNode(pKeyHead,name,value);
		}
		value=next;
	}

#if 0 /*测试使用*/
	KeyValue *pp = NULL;
	pp = pKeyHead;
	for(;pp != NULL; pp = pp->next)
	{
		printf("[%s] [%s]\n",pp->ValueName,pp->Value);
	}
#endif
	return 0;	
}







#if 0
int main()
{
	KeyValue * pkey = NULL;
	KeyValue  *pkeyHead = (KeyValue *)malloc(sizeof(KeyValue));
	KeyValue * pkeyNext = NULL;
	char test[128]={"asdc=123&cvmbg=7665543&wemmk=234754&promg=23fe2"};

#if 0
	int ret = 0;
	ret = createListHead(pkeyHead,"cdfgg","12324");
	addListNode(pkeyHead,"FFFF","123456");
	addListNode(pkeyHead,"bbbb","34565767");
	//pkeyNext =addListNode(pkeyNext,"bccc","35767");
	pkey = pkeyHead;
	for (;pkey != NULL;pkey = pkey->next)
	{
		printf("[%s]	[%s]\n",pkey->ValueName,pkey->Value);
	}
	getListValue(pkeyHead,"FFFF",test);
	printf("[%s]	[%s]\n","cdfgg",test);
	ret = deteteListNode(pkeyHead,"bbbb");
	printf("[%d]\n",ret);
	//for (;pkey != NULL;pkey = pkey->next)
	{
	//	printf("[%s]	[%s]\n",pkey->ValueName,pkey->Value);
	}
	DropList(pkeyHead);
#else
	UnPackStrtoKeyValue(test,pkeyHead);
	DropList(pkeyHead);
#endif
	return 0;
}
#endif
