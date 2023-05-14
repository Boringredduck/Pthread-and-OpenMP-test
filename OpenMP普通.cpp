#include<pthread.h>
#include <iostream>
#include<chrono>
#include<semaphore.h>
#include<omp.h>

using namespace std;
int numsize;
const int NUM_THREADS = 5;//定义线程数量
double** A;//数组

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

        //并行划分部分
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
            //离开for循环时，各个线程默认同步，进入下一行的处理
        }
    }
    

    //待测试程序
    auto end_time = chrono::high_resolution_clock::now();

    // 计算执行时间（毫秒）
    double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    // 输出执行时间
    cout << "代码执行时间为：" << time << "ms" << endl;
    return 0;


}
