#include <vaccel.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define M 512
#define N 512
#define K 512

#define ELEM_2D(array, i, j, ld) (*((array) + i*ld + j))

void init(int m, int n, int k, float *A, float *B, float *C)
{
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < k; ++j)
			ELEM_2D(A, i, j, k) = ((float) i * j) / m;

	for (int i = 0; i < k; ++i)
		for (int j = 0; j < n; ++j)
			ELEM_2D(B, i, j, n) = ((float) i * j) / m;

	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			ELEM_2D(C, i, j, n) = 0;
}

int main(int argc, char *argv[])
{
	int data_fp = -1;
	int ret = VACCEL_EINVAL;

	if (argc == 2) {
		data_fp = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (data_fp < 0)
			fprintf(stderr, "Could not open file for writing. Will not write results\n");
	}

	float alpha = 32412.0, beta = 2123.0;

	//float A[M*K];
	//float B[K*N];
	//float C[M*N];

	float *A, *B, *C;
	int64_t *m, *n, *k;

	A = (float*) malloc(M * K * sizeof(float));
	if (!A) {
		fprintf(stderr, "Could not allocate memory for Input A matrix\n");
		ret = VACCEL_ENOMEM;
		goto free_out;
	}
	B = (float*) malloc(K * N * sizeof(float));
	if (!B) {
		fprintf(stderr, "Could not allocate memory for target matrix\n");
		ret = VACCEL_ENOMEM;
		goto free_out_1;
	}

	C = (float*) malloc(M * N * sizeof(float));
	if (!C) {
		fprintf(stderr, "Could not allocate memory for target matrix\n");
		ret = VACCEL_ENOMEM;
		goto free_out_2;
	}

	m = malloc(sizeof(int64_t));
	n = malloc(sizeof(int64_t));
	k = malloc(sizeof(int64_t));

	*m = M;
	*n = N;
	*k = K;

	init(M, N, K, A, B, C);

	struct vaccel_session session;

	ret = vaccel_sess_init(&session, 0);
	if (ret)
		return ret;

	ret = vaccel_sgemm(
		&session,
		M, N, K,
		alpha,
		(float *)A, K,
		(float *)B, N,
		beta,
		(float *)C, N
	);
	if (ret) {
		fprintf(stderr, "Could not run sgemm\n");
		goto out;
	}

	if (data_fp > 0)
		write(data_fp, C, sizeof(float) * M * N);

out:
	vaccel_sess_free(&session);
	free(C);
free_out_2:
	free(A);
free_out_1:
	free(B);
free_out:
	return ret;
}
