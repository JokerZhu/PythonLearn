#include<string.h>
#include <stdio.h>
#include <stdlib.h>


int main()
{
	char str2[] = "zhujingwei";
	int ret = 0;
	ret = UppleToUpperCase(str2);
	printf("ret = [%d],str2 = [%s]\n",ret,str2);
	return 0;
}
