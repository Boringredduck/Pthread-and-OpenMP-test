#include<pthread.h>
#include <iostream>
#include<chrono>
using namespace std;
int numsize;
double** A;//����


struct threadParam_t 
{
    int k;//��ȥ������
    int t_id;//�߳�id
};

void* threadFunc(void* param) 
{
    threadParam_t* p = (threadParam_t*)param;
    int k = p->k;
    int t_id = p->t_id;
    int i = k + t_id + 1;//��ȡ�Լ����������
    for (int j = k + 1; j < numsize; ++j)
        A[i][j] = A[i][j] - A[i][k] * A[k][j];
    A[i][k] = 0;
    pthread_exit(NULL);
    return NULL;
}


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
    for (int k = 0; k < n; ++k)
    {
        for (int j = k + 1; j < n; j++)
            A[k][j] = A[k][j] / A[k][k];
        A[k][k] = 0;

        int worker_count = n-1-k;//�����߳���
        pthread_t* handles = static_cast<pthread_t*>(malloc(sizeof(pthread_t) * worker_count));
        threadParam_t* param =(threadParam_t*)malloc(sizeof(threadParam_t) * worker_count);
        for (int t_id = 0; t_id < worker_count; t_id++)
        {
            param[t_id].k = k;
            param[t_id].t_id = t_id;
        }
        for (int t_id = 0; t_id < worker_count; t_id++)
            pthread_create(&handles[t_id], NULL, threadFunc, (void*)&param[t_id]);
        for (int t_id = 0; t_id < worker_count; t_id++)
            pthread_join(handles[t_id], NULL);
    }

    //�����Գ���
    auto end_time = chrono::high_resolution_clock::now();

    // ����ִ��ʱ�䣨���룩
    double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    // ���ִ��ʱ��
    cout << "����ִ��ʱ��Ϊ��" << time << "ms" << endl;
    return 0;


}
