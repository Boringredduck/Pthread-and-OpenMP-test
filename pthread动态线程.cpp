#include<pthread.h>
#include <iostream>
#include<chrono>
using namespace std;
int numsize;
double** A;//数组


struct threadParam_t 
{
    int k;//消去的轮数
    int t_id;//线程id
};

void* threadFunc(void* param) 
{
    threadParam_t* p = (threadParam_t*)param;
    int k = p->k;
    int t_id = p->t_id;
    int i = k + t_id + 1;//获取自己所需的任务
    for (int j = k + 1; j < numsize; ++j)
        A[i][j] = A[i][j] - A[i][k] * A[k][j];
    A[i][k] = 0;
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
    for (int k = 0; k < n; ++k)
    {
        for (int j = k + 1; j < n; j++)
            A[k][j] = A[k][j] / A[k][k];
        A[k][k] = 0;

        int worker_count = n-1-k;//工作线程数
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

    //待测试程序
    auto end_time = chrono::high_resolution_clock::now();

    // 计算执行时间（毫秒）
    double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    // 输出执行时间
    cout << "代码执行时间为：" << time << "ms" << endl;
    return 0;


}
