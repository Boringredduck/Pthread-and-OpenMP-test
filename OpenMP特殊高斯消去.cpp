#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include<chrono>
using namespace std;
#define BATCH_SIZE 200
int** A, * B;
int n, num_threads;

// ��Ԫ������
double** Act;
int act_size, * act_row;
int* act_flag;

void eliminate(int k) {
    int i, j;
    int factor;

    for (i = 0; i < act_size; i++) {
        if (Act[i][k] == 0) continue;

        // ����ǰ��Ԫ���뱻��Ԫ�������
        factor = Act[i][k];
#pragma omp parallel for private(j)
        for (j = k; j < n; j++) {
            A[k][j] =  factor*Act[i][j];
        }
        B[k] ^= factor * B[act_row[i]];
    }

    // ����������ڷ�Χ�ڣ���������Ϊ�µ���Ԫ��
    if (k < act_row[0]) {
#pragma omp parallel for private(i)
        for (i = 0; i < n; i++) {
            Act[act_size][i] = A[k][i];
        }
        B[n] = B[k];
        act_row[act_size] = k;
        act_flag[k] = 0;
        act_size++;
    }
}

int main(int argc, char* argv[]) {
    int i, j, k, tid;

    // ��ȡ�������
    cout << "��������С���߳���";
    cin >> n;

    cin >> num_threads;

    // ��ʼ������
    A = (int**)malloc(n * sizeof(int*));
    for (i = 0; i < n; i++) {
        A[i] = (int*)malloc(n * sizeof(int));
        for (j = 0; j < n; j++) {
            A[i][j] = rand() % 10 + 1;
        }
    }

    B = (int*)malloc((n + 1) * sizeof(int));
    for (i = 0; i < n; i++) {
        B[i] = rand() % 10 + 1;
    }

    auto start_time = chrono::high_resolution_clock::now();

    // ��ʼ��Ԫ����
    Act = (double**)malloc(BATCH_SIZE * sizeof(double*));
    for (i = 0; i < BATCH_SIZE; i++) {
        Act[i] = (double*)malloc(n * sizeof(double));
    }

    act_row = (int*)malloc(BATCH_SIZE * sizeof(int));
    act_flag = (int*)malloc(n * sizeof(int));


#pragma omp parallel shared(n, A, B, Act, act_row, act_flag, act_size)
    {
        int tid = omp_get_thread_num();

#pragma omp for private(k)
        for (k = 0; k < n; k++) {
            if (act_flag[k] == 0) continue;
            eliminate(k);
        }
    }

   

    // �ش����
    for (i = n - 1; i >= 0; i--) {
        for (j = i + 1; j < n; j++) {
            B[i] ^= A[i][j] * B[j];
        }
        B[i] /= A[i][i];
    }
    //�����Գ���
    auto end_time = chrono::high_resolution_clock::now();

    // ����ִ��ʱ�䣨���룩
    double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    // ���ִ��ʱ��
    cout << "����ִ��ʱ��Ϊ��" << time << "ms" << endl;
    // �ͷ��ڴ�
    for (i = 0; i < n; i++) {
        free(A[i]);
    }
    free(A);

    free(B);

    for (i = 0; i < BATCH_SIZE; i++) {
        free(Act[i]);
    }
    free(Act);

    free(act_row);
    free(act_flag);

    return 0;
}
