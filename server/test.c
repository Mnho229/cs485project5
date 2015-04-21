#include "stdio.h"
int main()
{	
	int SecretKey; 
	scanf("%d",&SecretKey)
	int SECRET_KEY_SIZE = (SecretKey == 0 ? 1 : (int)(log10(SecretKey)+1)); 
	printf("length%d\t", SECRET_KEY_SIZE); 
	return(0);
}
