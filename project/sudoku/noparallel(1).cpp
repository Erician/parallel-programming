/*************************************************************************
	> File Name: noparallel.cpp
	> Author: 
	> Mail: 
	> Created Time: Sun 09 Jul 2017 10:37:13 AM CST
 ************************************************************************/

#include<iostream>
#include<vector>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <sys/time.h>

#define N 9

using namespace std;
//题目
int matrix[N][N]={
    0,6,0,5,9,3,0,0,0,\
    9,0,1,0,0,0,5,0,0,\
    0,3,0,4,0,0,0,9,0,\
    1,0,8,0,2,0,0,0,4,\
    4,0,0,3,0,9,0,0,1,\
    2,0,0,0,1,0,6,0,9,\
    0,8,0,0,0,6,0,2,0,\
    0,0,4,0,0,0,8,0,7,\
    0,0,0,7,8,5,0,1,0
    };
//定义一个struct，用来记录未填数字的位置
struct blankPos
{
    int x;
    int y;
};
vector<blankPos> v;

bool isRight(int x,int y)
{
    //检查行列
    for(int i=0;i<9;i++)
    if((matrix[x][i] == matrix[x][y]&&y!=i)||\
       (matrix[i][y] == matrix[x][y]&&x!=i))
        return false;
    //检查小格
    int x1,y1;
    x1 = (x/3)*3;
    y1 = (y/3)*3;
    for(int i=0;i<3;i++)
    for(int j=0;j<3;j++)
    if(matrix[x1+i][y1+j] == matrix[x][y]&&(x1+i!=x||y1+j!=y))
        return false;
    return true;
}

bool solve(int index)
{
    //cout<<index<<endl;
    if(index==v.size())
        return true;
    int x = v[index].x;
    int y = v[index].y;
    for(int i=1;i<=9;i++)
    {
        matrix[x][y] = i;
        //测试是否满足条件
        if(isRight(x,y))
        {
            //找到
            if(solve(index+1))
                return true;
        }
        matrix[x][y] = 0;
    }
    return false;
}

int main()
{
    
    struct timeval start,finish;
    double costTime;
    //start time
    gettimeofday(&start,0);
	//记录哪些位置没有填入数字
    for(int i=0;i<N;i++)
    for(int j=0;j<N;j++)
        if(matrix[i][j] == 0)
            v.push_back(blankPos{i,j});
    solve(0);
    //cout<<v.size()<<endl;
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
            cout<<matrix[i][j]<<' ';
        cout<<endl;
    }
    gettimeofday(&finish,0);
    costTime = 1000000*(finish.tv_sec-start.tv_sec)+finish.tv_usec-start.tv_usec;
    printf("cost time:%lf us.\n",costTime);
    return 0;
}



