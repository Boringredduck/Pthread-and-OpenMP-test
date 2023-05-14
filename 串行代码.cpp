#include<pthread.h>
#include <iostream>
#include<chrono>
using namespace std;


void LUlinecount(double** A, int n)//���г������
{
    for (int k = 0; k < n; k++) // ���ѭ��
    {
        for (int j = k + 1; j < n; j++) // ��һ���ڲ�ѭ��
        {
            A[k][j] /= A[k][k]; // �������������� k ��֮��ĵ� j ��Ԫ�س��Ե� k �еĵ� k ��Ԫ��
        }
        A[k][k] = 1.0; // ���� k �еĵ� k ��Ԫ����Ϊ 1

        for (int i = k + 1; i < n; i++) // �ڶ����ڲ�ѭ��
        {
            for (int j = k + 1; j < n; j++)
            {
                A[i][j] -= A[i][k] * A[k][j]; // ��Ԫ���������� i ��֮��ĵ� j ��Ԫ�ؼ�ȥ�� i �еĵ� k ��Ԫ�س��Ե� k ��֮��ĵ� j ��Ԫ��
            }
            A[i][k] = 0.0; // ���� i �еĵ� k ��Ԫ����Ϊ 0
        }
    }
}

double** NewArray(int n) {
    // ������ά����
    double** arr = new double*[n];
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
    double** A;
    A = NewArray(n);
    auto start_time = chrono::high_resolution_clock::now();
    LUlinecount(A, n);
    auto end_time = chrono::high_resolution_clock::now();

    // ����ִ��ʱ�䣨���룩
    double time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    // ���ִ��ʱ��
    cout << "����ִ��ʱ��Ϊ��" << time << "ms" << endl;
    return 0;


}
