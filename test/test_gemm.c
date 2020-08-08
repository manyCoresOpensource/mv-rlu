#include <stdio.h>
#include "include/mvrlu.h"


void gemm_bin(int M, int N, int K, float ALPHA,
		char  *A, int lda,
		float *B, int ldb,
		float *C, int ldc)
{
	int i,j,k;
	for(i = 0; i < M; ++i) {
		for(k = 0; k < K; ++k) {
			char A_PART = A[i*lda+k];
			if(A_PART){
				for(j = 0; j < N; ++j){
					C[i*ldc+j] += B[k*ldb+j];	
				}
			} else {
				for(j = 0; j < N; ++j){
					C[i*ldc+j] -= B[k*ldb+j];	
				}	
			}
		}		
	}
}



// need test.  written by seungminJeon 2020.08.09
void rlu_gemm_bin_k(rlu_thread_data_t *self,
		int k, int N, 
		char *A, int lda,   // A, B, C were must made of RLU_ALLOC
		float *B, int ldb,
		float *C, int ldc)
{
	int j=0, A_VISITED=0;
	char A_PART;

recontinue:
	RLU_READER_LOCK(self);

	if(!A_VISITED) {
		A_PART = (char)RLU_DEREF(self, &A+(i*lda+k)); // A[i*lda+k];
		A_VISITED = 1;
	}

	if(A_PART){
		while(j<N) {
			float B_PART = (float)RLU_DEREF(self, &B+(k*ldb+j)); // B[k*ldb+j];
			if(!RLU_TRY_LOCK(self, &C+(i*ldc+j))) { // C[i*ldc+j];
				RLU_ABORT(self);
				goto recontinue;
			}

			// .... write 도중 read가 되면 같은 코드도 다른 결과 나올 수 있는거 아닌가?
			C[i*ldc+j] += B_PART;
			++j;
		}
	} else {
		while(j<N) {
			float B_PART = (float)RLU_DEREF(self, &B+(k_bias+j)); // B[k*ldb+j];
			if(!RLU_TRY_LOCK(self, &C+(i*ldc+j))) { // C[i*ldc+j];
				RLU_ABORT(self);
				goto recontinue;
			}
			C[i*ldc+j] -= B_PART;
			++j;
		}
	}

	RLU_READER_UNLOCK(self);
}

void rlu_gemm_bin(int M, int N, int K, float ALPHA,
		char  *A, int lda,
		float *B, int ldb,
		float *C, int ldc)
{
	int i,j,k;
	for(i = 0; i < M; ++i) {
		for(k = 0; k < K; ++k){
			rlu_gemm_bin_k(,,,,);
		}		
	}
}

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
