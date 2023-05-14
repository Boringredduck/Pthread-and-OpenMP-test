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



// ��Ԫ������
double** Act;
int act_size, * act_row;
int* act_flag;

void* eliminate(void* arg) {
    int tid = *((int*)arg);
    int i, j, k;
    int factor;

    for (k = tid * BATCH_SIZE; k < (tid + 1) * BATCH_SIZE; k++) {
        if (k >= n || act_flag[k] == 0) continue;

        // �Ա���Ԫ�н��в���
        for (i = 0; i < act_size; i++) {
            if (Act[i][k] == 0) continue;

            // ����ǰ��Ԫ���뱻��Ԫ�������
            factor = Act[i][k];
            for (j = k; j < n; j++) {
                A[k][j] = factor * Act[i][j];
            }
            B[k] ^= factor * B[act_row[i]];
        }

        // ����������ڷ�Χ�ڣ���������Ϊ�µ���Ԫ��
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

    // ��ȡ�������
    cin >> n;
    pthread_t threads[MAX_SIZE];
    int thread_args[MAX_SIZE];


    // ��̬�����ڴ�ռ�
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

    // ��ʼ�����������
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            A[i][j] = rand() % 10 + 1;
        }
        B[i] = rand() % 10 + 1;
    }
    auto start_time = chrono::high_resolution_clock::now();
    // ��ʼ��Ԫ����
    act_size = 0;
    for (k = 0; k < n; k += BATCH_SIZE) {
        // ����Ҫ��Ԫ���Ӿ������ Act ������
        for (i = k; i < k + BATCH_SIZE && i < n; i++) {
            for (j = k; j < n; j++) {
                Act[i - k][j] = A[i][j];
            }
            B[i] = B[i];
            act_row[i - k] = i;
            act_flag[i] = 1;
        }
        act_size = BATCH_SIZE;

        // �����߳̽��м���
        for (i = 0; i < num_threads; i++) {
            thread_args[i] = i;
            pthread_create(&threads[i], NULL, eliminate, &thread_args[i]);
        }

        // �ȴ��߳̽���
        for (i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        // �����Ԫ������
        act_size = 0;
    }

    // �ش����
    for (i = n - 1; i >= 0; i--) {
        for (j = i + 1; j < n; j++) {
            B[i] ^= A[i][j] * B[j];
        }
        B[i] /= A[i][i];

        // �ͷŶ�̬������ڴ�ռ�
        for (i = 0; i < n; i++) {
            free(A[i]);
        }
        auto end_time = chrono::high_resolution_clock::now();

        // ����ִ��ʱ�䣨���룩
        double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

        // ���ִ��ʱ��
        cout << "����ִ��ʱ��Ϊ��" << time << "ms" << endl;
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

