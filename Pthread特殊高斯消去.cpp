#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include<iostream>
#include<chrono>
using namespace std;
#define BATCH_SIZE 50
#define     MAX_SIZE 10
int** A, * B;
int n, num_threads;



// 消元子数组
double** Act;
int act_size, * act_row;
int* act_flag;

void* eliminate(void* arg) {
    int tid = *((int*)arg);
    int i, j, k;
    int factor;

    for (k = tid * BATCH_SIZE; k < (tid + 1) * BATCH_SIZE; k++) {
        if (k >= n || act_flag[k] == 0) continue;

        // 对被消元行进行操作
        for (i = 0; i < act_size; i++) {
            if (Act[i][k] == 0) continue;

            // 将当前消元子与被消元行做异或
            factor = Act[i][k];
            for (j = k; j < n; j++) {
                A[k][j] = factor * Act[i][j];
            }
            B[k] ^= factor * B[act_row[i]];
        }

        // 如果首项仍在范围内，则将其升格为新的消元子
        if (k < act_row[0]) {
            for (i = 0; i < n; i++) {
                Act[act_size][i] = A[k][i];
            }
            B[n] = B[k];
            act_row[act_size] = k;
            act_flag[k] = 0;
            act_size++;
        }
    }

    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char* argv[]) {
    int i, j, k;

    // 获取输入参数
    cin >> n;
    pthread_t threads[MAX_SIZE];
    int thread_args[MAX_SIZE];


    // 动态分配内存空间
    A = (int**)malloc(n * sizeof(int*));
    for (i = 0; i < n; i++) {
        A[i] = (int*)malloc(n * sizeof(int));
    }
    B = (int*)malloc(n * sizeof(int));
    Act = (double**)malloc(BATCH_SIZE * sizeof(double*));
    for (i = 0; i < BATCH_SIZE; i++) {
        Act[i] = (double*)malloc(n * sizeof(double));
    }
    act_row = (int*)malloc(BATCH_SIZE * sizeof(int));
    act_flag = (int*)malloc(n * sizeof(int));

    // 初始化矩阵和向量
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            A[i][j] = rand() % 10 + 1;
        }
        B[i] = rand() % 10 + 1;
    }
    auto start_time = chrono::high_resolution_clock::now();
    // 开始消元操作
    act_size = 0;
    for (k = 0; k < n; k += BATCH_SIZE) {
        // 将需要消元的子矩阵存入 Act 数组中
        for (i = k; i < k + BATCH_SIZE && i < n; i++) {
            for (j = k; j < n; j++) {
                Act[i - k][j] = A[i][j];
            }
            B[i] = B[i];
            act_row[i - k] = i;
            act_flag[i] = 1;
        }
        act_size = BATCH_SIZE;

        // 创建线程进行计算
        for (i = 0; i < num_threads; i++) {
            thread_args[i] = i;
            pthread_create(&threads[i], NULL, eliminate, &thread_args[i]);
        }

        // 等待线程结束
        for (i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        // 清空消元子数组
        act_size = 0;
    }

    // 回代求解
    for (i = n - 1; i >= 0; i--) {
        for (j = i + 1; j < n; j++) {
            B[i] ^= A[i][j] * B[j];
        }
        B[i] /= A[i][i];

        // 释放动态分配的内存空间
        for (i = 0; i < n; i++) {
            free(A[i]);
        }
        auto end_time = chrono::high_resolution_clock::now();

        // 计算执行时间（毫秒）
        double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

        // 输出执行时间
        cout << "代码执行时间为：" << time << "ms" << endl;
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
}

