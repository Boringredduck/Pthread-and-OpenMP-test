#include<pthread.h>
#include <iostream>
#include<chrono>
using namespace std;


void LUlinecount(double** A, int n)//串行程序计算
{
    for (int k = 0; k < n; k++) // 外层循环
    {
        for (int j = k + 1; j < n; j++) // 第一个内层循环
        {
            A[k][j] /= A[k][k]; // 除法操作：将第 k 行之后的第 j 列元素除以第 k 行的第 k 列元素
        }
        A[k][k] = 1.0; // 将第 k 行的第 k 列元素设为 1

        for (int i = k + 1; i < n; i++) // 第二个内层循环
        {
            for (int j = k + 1; j < n; j++)
            {
                A[i][j] -= A[i][k] * A[k][j]; // 消元操作：将第 i 行之后的第 j 列元素减去第 i 行的第 k 列元素乘以第 k 行之后的第 j 列元素
            }
            A[i][k] = 0.0; // 将第 i 行的第 k 列元素设为 0
        }
    }
}

double** NewArray(int n) {
    // 创建二维数组
    double** arr = new double*[n];
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
    double** A;
    A = NewArray(n);
    auto start_time = chrono::high_resolution_clock::now();
    LUlinecount(A, n);
    auto end_time = chrono::high_resolution_clock::now();

    // 计算执行时间（毫秒）
    double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    // 输出执行时间
    cout << "代码执行时间为：" << time << "ms" << endl;
    return 0;


}
