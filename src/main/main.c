#include <stdio.h>
#include <testdef.h>

int main(){
	int num;
	printf("Give a num: ");
	scanf("%d", &num);
	if ( f(num) )
		printf("Num %d is odd\n", num);
	else
		printf("Num %d is even\n", num);
	return 0;
		
}
