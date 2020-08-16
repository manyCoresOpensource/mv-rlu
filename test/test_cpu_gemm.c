#include <stdio.h>
#include "include/mvrlu.h"

// Original function
void cpu_gemm_nn(int TA, int TB, int M, int N, int K, float ALPHA,
		float *A, int lda,
		float *B, int ldb,
		float BETA,		
		float *C, int ldc)
{
	int i,j,k;
	for(i = 0; i < M; ++i) {
		for(k = 0; k < K; ++k){
			register float A_PART = ALPHA *	A[i * lda + k];
			for(j = 0; j < N; ++j){
				C[i*ldc+j]	+=	A_PART*B[k*ldb+j];		
			}	
		}	
	}
}

// function with MV-RLU written by SeungminJeon, 2020.08.16
void rlu_cpu_gemm_nn(rlu_thread_data_t *self,
		int TA, int TB, int M, int N, int K, float ALPHA,
		float *A, int lda,
		float *B, int ldb,
		float BETA,		
		float *C, int ldc)
{
	int i=0,j=0,k=0;

restart:
	RLU_READER_LOCK(self);

	printf("[Before] C=%p, C[0]=%f, C[1]=%f\n", C, C[0], C[1]);

	// 동시에 한 스레드만 C에 write 가능하도록
	if(!RLU_TRY_LOCK(self, &C)) { // 이 시점에 새로운 commit clock을 가진 C 생성
		RLU_ABORT(self);
		printf("[ABORT].. \n");
		goto restart; // 다른 스레드가 C에 write하고 있으면 재시도
	}
	printf("[After] C=%p, C[0]=%f, C[1]=%f\n", C, C[0], C[1]); // 본래 값은 C[0]만 복사된다.

	while(i<M) {
		//k = (k<0) ? -k : 0; 
		k = 0;
		while(k<K) {
			j = 0;
			register float A_PART = ALPHA * A[i * lda + k];
			while(j<N) {
				printf("i=%d, k=%d, j=%d\n", i, k, j);
				C[i*ldc+j] += A_PART * B[k*ldb+j];
				printf("C[%d] = %f, &C[%d]=%p\n", i*ldc+j, C[i*ldc+j], i*ldc+j, &C[i*ldc+j]);
				++j;
			}
			++k;
		}
		++i;
	}
	RLU_READER_UNLOCK(self);
}

int main() {
    // Initialize MV_RLU Thread
	int ninit = RLU_INIT();
	printf("%d\n", ninit);
	mvrlu_thread_struct_t *t0 = RLU_THREAD_ALLOC(); 
	mvrlu_thread_struct_t *t1 = RLU_THREAD_ALLOC();

	RLU_THREAD_INIT(t0);
	RLU_THREAD_INIT(t1);

	// Initialize
	int M=5, N=4, K=3;
	float *A = RLU_ALLOC(sizeof(float) * K * M); // K*M
	float *B = RLU_ALLOC(sizeof(float) * N * K); // N*K
	float *C = RLU_ALLOC(sizeof(float) * M * N); // M*N

	int m, n, k;
	printf("-------- A --------\n");
	for(k=0; k<K; k++) {
		for(m=0; m<M; m++){
			A[k*M+m] = (float)k*M+m+1;
			printf("%f ", A[k*M+m]);
		}
		printf("\n");
	}
	printf("--------- B --------\n");
	for(n=0; n<N; n++){
		for(k=0; k<K; k++){
			B[n*K+k] = n*K+k+1;
			printf("%f ", B[n*K+k]);
		}
		printf("\n");
	}
	C[0] = 10;
	C[1] = 20;
	

	// Calculate
	int TA=1, TB=1, lda=1, ldb=0, ldc=1;
	float ALPHA=1, BETA=1;

	printf("\n[main] &C = %p, C = %p\n", &C, C);
	rlu_cpu_gemm_nn(t0,
			TA, TB, M, N, K, ALPHA, 
			A, lda,
			B, ldb, BETA,
			C, ldc);

	// Print result	
	printf("\n----------- C ----------\n");

	RLU_READER_LOCK(t0);
	float *C_DEREF = (RLU_DEREF(t0, C)); 
	printf("(C)=%d(d), %p(p)\n", (C), (C));
	printf("(C_DEREF)=%d(d), %p(p)\n", (C_DEREF), (C_DEREF));
	for(m=0; m<M; m++) {
		for(n=0; n<N; n++) {
			printf("ix=%d, C_DEREF[ix]=%f, &C_DEREF[ix]=%p\n", m*N+n, C_DEREF[m*N+n], &C_DEREF[m*N+n]);
		}
		printf("\n");
	}
	RLU_READER_UNLOCK(t0);

	// Finish
	RLU_FREE(t0, A);
	RLU_FREE(t0, B);
	RLU_FREE(t0, C);

	RLU_THREAD_FREE(t0);
	RLU_THREAD_FREE(t1);

	RLU_PRINT_STATS();
	
	RLU_FINISH();

	printf("done\n");
	return 0;
   
}
