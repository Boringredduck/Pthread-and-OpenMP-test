#include<pthread.h>
#include <iostream>
#include<chrono>
#include<semaphore.h>
#include<omp.h>

using namespace std;
int numsize;
const int NUM_THREADS = 10;//�����߳�����
double** A;//����


struct threadParam_t
{
    int t_id;//�߳�id
};

//barrier����
pthread_barrier_t barrier_Division;
pthread_barrier_t barrier_Elimination;

//�̺߳�������
void* threadFunc(void* param)
{
    threadParam_t* p = (threadParam_t*)param;
    int t_id = p->t_id;

    for (int k = 0; k < numsize; ++k)
    {
        if (t_id == 0)
        {
            for (int j = k + 1; j < numsize; j++)
                A[k][j] = A[k][j] / A[k][k];
            A[k][k] = 1;
        }
        pthread_barrier_wait(&barrier_Division);//��һ��ͬ����
        for (int i = k + 1 + t_id; i < numsize; i += NUM_THREADS)
        {
            for (int j = k + 1; j < numsize; ++j)
            {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
            A[i][k] = 0;
           
        }
       
    
    
        pthread_barrier_wait(&barrier_Elimination);//�ڶ���ͬ����
    }
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

    //��ʼ��barrier
    pthread_barrier_init(&barrier_Division, NULL, NUM_THREADS);
    pthread_barrier_init(&barrier_Elimination, NULL, NUM_THREADS);

    //�����߳�
    pthread_t handles[NUM_THREADS];//������Ӧ��handle
    threadParam_t param[NUM_THREADS];//������Ӧ�����ݽṹ
    for (int t_id = 0; t_id < NUM_THREADS; t_id++)
    {
        param[t_id].t_id = t_id;
        pthread_create(&handles[t_id], NULL, threadFunc, (void*)&param[t_id]);
    }

    for (int t_id = 0; t_id < NUM_THREADS; ++t_id)
        pthread_join(handles[t_id], NULL);

    //�����Գ���
    auto end_time = chrono::high_resolution_clock::now();

    // ����ִ��ʱ�䣨���룩
    double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    // ���ִ��ʱ��
    cout << "����ִ��ʱ��Ϊ��" << time << "ms" << endl;
    return 0;


}
