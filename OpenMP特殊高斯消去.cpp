#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include<chrono>
using namespace std;
#define BATCH_SIZE 200
int** A, * B;
int n, num_threads;

// 消元子数组
double** Act;
int act_size, * act_row;
int* act_flag;

void eliminate(int k) {
    int i, j;
    int factor;

    for (i = 0; i < act_size; i++) {
        if (Act[i][k] == 0) continue;

        // 将当前消元子与被消元行做异或
        factor = Act[i][k];
#pragma omp parallel for private(j)
        for (j = k; j < n; j++) {
            A[k][j] =  factor*Act[i][j];
        }
        B[k] ^= factor * B[act_row[i]];
    }

    // 如果首项仍在范围内，则将其升格为新的消元子
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

    // 获取输入参数
    cout << "输入矩阵大小和线程数";
    cin >> n;

    cin >> num_threads;

    // 初始化矩阵
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

    // 开始消元操作
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

   

    // 回代求解
    for (i = n - 1; i >= 0; i--) {
        for (j = i + 1; j < n; j++) {
            B[i] ^= A[i][j] * B[j];
        }
        B[i] /= A[i][i];
    }
    //待测试程序
    auto end_time = chrono::high_resolution_clock::now();

    // 计算执行时间（毫秒）
    double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    // 输出执行时间
    cout << "代码执行时间为：" << time << "ms" << endl;
    // 释放内存
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
