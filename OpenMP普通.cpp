#include<pthread.h>
#include <iostream>
#include<chrono>
#include<semaphore.h>
#include<omp.h>

using namespace std;
int numsize;
const int NUM_THREADS = 5;//�����߳�����
double** A;//����

double** NewArray(int n) {
    // ������ά����
    double** arr = new double* [n];
    for (int i = 0; i < n; i++) {
        arr[i] = new double[n];
    }

    // ��������
    double num = 0.1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            arr[i][j] = num;
            num += 0.1;
            while (arr[i][j] == 0) {
                num += 0.1;
                arr[i][j] = num;
            }
        }
    }

    return arr;
}
int main()
{
    int n;
    cin >> n;
    numsize = n;

    A = NewArray(n);
    auto start_time = chrono::high_resolution_clock::now();
    //�����Գ���
    int i, j, k;
    double tmp;
#pragma omp parallel if(n>1),num_threads(NUM_THREADS),private(i,j,k,tmp)
    for (k = 1; k < numsize; ++k)
    {
#pragma omp single
        {
            tmp = A[k][k];
            for (int j = k + 1; j < n; ++j) {
                A[k][j] = A[k][j] / tmp;
            }
            A[k][k] = 1;
        }

        //���л��ֲ���
#pragma omp for
        for (i = k + 1; i < n; ++i)
        {
            tmp = A[i][k];
            for (j = k + 1; j < n; ++j) {
                tmp = A[i][k];
                for (j = k + 1; j < n; ++j) {
                    A[i][j] = A[i][j] - tmp * A[k][j];
                }
                A[i][k] = 0;
            }
            //�뿪forѭ��ʱ�������߳�Ĭ��ͬ����������һ�еĴ���
        }
    }
    

    //�����Գ���
    auto end_time = chrono::high_resolution_clock::now();

    // ����ִ��ʱ�䣨���룩
    double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    // ���ִ��ʱ��
    cout << "����ִ��ʱ��Ϊ��" << time << "ms" << endl;
    return 0;


}
