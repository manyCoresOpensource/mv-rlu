#include <stdio.h>
#include "include/mvrlu.h"

int main() {
    // initialize
	int ninit = RLU_INIT();
	printf("%d\n", ninit);

	mvrlu_thread_struct_t *self = RLU_THREAD_ALLOC();
	RLU_THREAD_INIT(self);

	// do
	int *numPtr1 =  RLU_ALLOC(sizeof(int)*4);
	
	*numPtr1 = 50;
	*(numPtr1+1) = 54;
	//numPtr1[0] = 50;
	//numPtr1[1] = 54;
	printf("%p, %p, %p, %p\n", &numPtr1[0], &numPtr1[1], &numPtr1[2], &numPtr1[3]);
	printf("%d, %d, %d, %d\n", *numPtr1, *(numPtr1+1), *(numPtr1+2), *(numPtr1+3));

    RLU_PRINT_STATS();
	printf("haha\n");
    

	// free
	RLU_FREE(self, numPtr1);
	RLU_THREAD_FREE(self);
	RLU_FINISH();

	RLU_PRINT_STATS();
	return 0;
   
}
