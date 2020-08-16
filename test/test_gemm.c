#include <stdio.h>
#include "include/mvrlu.h"

// Original function
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

// need test.  written by seungminJeon 2020.08.15
void rlu_gemm_bin_k(rlu_thread_data_t *self,
		int i, int N, int k, 
		int *A, int lda,   // A, B, C were must made of RLU_ALLOC
		int *B, int ldb,
		int *C, int ldc)
{
	printf("[DEBUG] i=%d N=%d k=%d lda=%d ldb=%d ldc=%d\n", i, N, k, lda, ldb,
			ldc);
	int j=0, A_VISITED=0;
	char A_PART;

recontinue:
	RLU_READER_LOCK(self);

	if(!A_VISITED) {
		A_PART = A[i*lda+k];
		A_VISITED = 1;
	}

	if(A_PART){
		while(j<N) {
			int B_PART = B[k*ldb+j];

			if(!RLU_TRY_LOCK(self, &C)) { // C[i*ldc+j]만 lock 못하나?
				RLU_ABORT(self);
				goto recontinue;
			}

			// .... write 도중 read가 되면 같은 코드도 다른 결과 나올 수 있는거 아닌가?
			C[i*ldc+j] += B_PART;
			++j;
		}
	} else {
		while(j<N) {
			int B_PART = B[k*ldb+j];
			if(!RLU_TRY_LOCK(self, &C)) {
				RLU_ABORT(self);
				goto recontinue;
			}
			C[i*ldc+j] -= B_PART;
			++j;
		}
	}

	RLU_READER_UNLOCK(self);
}

int main() {
    // Initialize MV_RLU Thread
	int ninit = RLU_INIT();
	printf("%d\n", ninit);
	mvrlu_thread_struct_t *t0 = RLU_THREAD_ALLOC(); 
	mvrlu_thread_struct_t *t1 = RLU_THREAD_ALLOC();
	mvrlu_thread_struct_t *t2 = RLU_THREAD_ALLOC();
	mvrlu_thread_struct_t *t3 = RLU_THREAD_ALLOC();

	RLU_THREAD_INIT(t0);
	RLU_THREAD_INIT(t1);
	RLU_THREAD_INIT(t2);
	RLU_THREAD_INIT(t3);

	// Initialize A, B, C
	int M=5, N=4, K=3;
	int *A = RLU_ALLOC(sizeof(int) * K * M); // K*M
	int *B = RLU_ALLOC(sizeof(int) * N * K); // N*K
	int *C = RLU_ALLOC(sizeof(int) * M * N); // M*N

	int m, n, k;
	printf("-------- A --------\n");
	for(k=0; k<K; k++) {
		for(m=0; m<M; m++){
			A[k*M+m] = k*M+m;
			printf("%d ", A[k*M+m]);
		}
		printf("\n");
	}
	printf("--------- B --------\n");
	for(n=0; n<N; n++){
		for(k=0; k<K; k++){
			B[n*K+k] = n*K+k;
			printf("%d ", B[n*K+k]);
		}
		printf("\n");
	}

	// Start
	int lda=1, ldb=0, ldc=1;	
	int i,j;
	for(i = 0; i < M; i++) {
		for(k = 0; k < K; ++k){
			printf("entree i=%d, k=%d\n", i, k);
			if(k==0) {
				rlu_gemm_bin_k(t0, i, N, k, A, lda, B, ldb, C, ldc);
			}
			else if(k==1) {
				rlu_gemm_bin_k(t1, i, N, k, A, lda, B, ldb, C, ldc);
			}
			else if(k==2) {
				rlu_gemm_bin_k(t2, i, N, k, A, lda, B, ldb, C, ldc);
			}
		}		
	}

	// Print result
	printf("----------- C ----------\n");
	for(m=0; m<M; m++) {
		for(n=0; n<N; n++) {
			printf("%d ", C[m*N+n]);
		}
		printf("\n");
	}

	/*
	int *numPtr1 =  RLU_ALLOC(sizeof(int)*4);

	*numPtr1 = 50;
	*(numPtr1+1) = 54;
	//numPtr1[0] = 50;
	//numPtr1[1] = 54;
	printf("%p, %p, %p, %p\n", &numPtr1[0], &numPtr1[1], &numPtr1[2], &numPtr1[3]);
	printf("%d, %d, %d, %d\n", *numPtr1, *(numPtr1+1), *(numPtr1+2), *(numPtr1+3));

    //RLU_PRINT_STATS();
	printf("haha\n");    

	// free
	RLU_FREE(t0, numPtr1);
	*/

	// FREE
	RLU_THREAD_FREE(t0);
	RLU_THREAD_FREE(t1);
	RLU_THREAD_FREE(t2);
	RLU_THREAD_FREE(t3);
	RLU_FINISH();

	printf("ha finish");
	//RLU_PRINT_STATS();
	return 0;
   
}
