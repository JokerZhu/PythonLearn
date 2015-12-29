#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int multiply(int num1, int num2)
{
	return num1 * num2;
}

int UppleToUpperCase(char *str)
{
    int i = 0;
    int len;
	printf("len of str %d ,str = [%s]\n",strlen(str),str);
    for (i = 0,len=strlen(str); i < len; i++)
    {
        if ((str[i] >= 'a') && (str[i] <= 'z'))
            str[i] = str[i] + 'A' - 'a';
    }
    return(0);
}


int UppleToUpperCase2(char *str,int lenStr)
{
    int i = 0;
    int len;
	printf("len of str %d ,str = [%s],len = [%d]\n",strlen(str),str,lenStr);
	len = lenStr;
    for (i = 0; i < len; i++)
    {
        if ((str[i] >= 'a') && (str[i] <= 'z'))
            str[i] = str[i] + 'A' - 'a';
    }
	return 0;
}
