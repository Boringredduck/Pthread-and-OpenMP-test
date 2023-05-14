#include<pthread.h>
#include <iostream>
#include<chrono>
#include<semaphore.h>
using namespace std;
int numsize;
const int NUM_THREADS = 10;//定义线程数量
double** A;//数组


struct threadParam_t
{
    int t_id;//线程id
};
//信号量定义
// 信号量定义
sem_t sem_leader;
sem_t sem_Division[NUM_THREADS];
sem_t sem_Elimination[NUM_THREADS];



void* threadFunc(void* param)
{
    threadParam_t* p = (threadParam_t*)param;
    int t_id = p->t_id;

    for (int k = 0; k < numsize; ++k)
    {
        if (t_id == 0)
        {
            for (int j = k + 1; j < numsize; j++)
            {
                A[k][j] = A[k][j] / A[k][k];
            }
            A[k][k] = 1;
        }
        else
            sem_wait(&sem_Division[t_id - 1]);//阻塞，等待除法完成
        if (t_id == 0)
        {
            for (int i = 0; i < NUM_THREADS - 1; ++i)
            {
                sem_post(&sem_Division[i]);
            }
        }
        //循环分配任务
        for (int i = k + 1 + t_id; i < numsize; i += NUM_THREADS)
        {
            for (int j = k + 1; j < numsize; ++j)
            {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
            A[i][k] = 0;
        }
        if (t_id == 0)
        {
            for (int j = k + 1; j <NUM_THREADS-1; ++j)
                sem_wait(&sem_leader);
            for (int i = 0; i < NUM_THREADS - 1; ++i)
                sem_post(&sem_Elimination[i]);//通知其他woker进入下一轮
        }
        else
        {
            sem_post(&sem_leader);
            sem_wait(&sem_Elimination[t_id - 1]);
        }
    }
    pthread_exit(NULL);
    return NULL;
}


double** NewArray(int n) {
    // 创建二维数组
    double** arr = new double* [n];
    for (int i = 0; i < n; i++) {
        arr[i] = new double[n];
    }

    // 遍历数组
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
    //待测试程序

    //初始化信号量
    sem_init(&sem_leader, 0, 0);
    for (int i = 0; i < NUM_THREADS-1; ++i)
    {
        sem_init(&sem_Division[i], 0, 0);
        sem_init(&sem_Elimination[i], 0, 0);
    }

    //创建线程
    pthread_t handles[NUM_THREADS];//建立对应的handle
    threadParam_t param[NUM_THREADS];//建立对应的数据结构
    for (int t_id = 0; t_id < NUM_THREADS; t_id++)
    {
        param[t_id].t_id = t_id;
        pthread_create(&handles[t_id], NULL, threadFunc, (void*)&param[t_id]);
    }

    for (int t_id = 0; t_id < NUM_THREADS; ++t_id)
        pthread_join(handles[t_id], NULL);

    //待测试程序
    auto end_time = chrono::high_resolution_clock::now();

    // 计算执行时间（毫秒）
    double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    // 输出执行时间
    cout << "代码执行时间为：" << time << "ms" << endl;
    return 0;


}
