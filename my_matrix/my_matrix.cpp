#include<new>

#include "my_matrix.h"

void** my::alloc2d(int Row,int Col,int Size)//申请row * col个T大小的二维数组
{
    if(Row < 0 || Col < 0) throw std::invalid_argument("Argument \"row\" and \"col\" can't be negative.");
    void** arr = (void**)::operator new(Row * sizeof(void*));
    for(int i = 0; i < Row;i++)
    {
        try
        {
            arr[i] = ::operator new(Col * Size);
        }
        catch(...)
        {
            for(int j = 0;j < i;j++)//回滚防止溢出
            {
                ::operator delete(arr[j]);
            }
            throw;
        }
    }
    return arr;
}

void my::free2d(void** arr,int row) noexcept
{
    if(!arr || row < 0) return;
    for(int i = 0; i < row;i++)
    {
        if(arr[i]) ::operator delete(arr[i]);
    }
    ::operator delete(arr);
}

template<typename T> void my::rollback2d(T** arr,int N_row,int N_col,int r,int c)//回滚析构(r,c)之前的元素，并释放数组
{
    if(!arr || N_row <=0 || N_col <= 0 || r < 0 || r >= N_row || c < 0 || c >= N_col)
        return;
    for(int i = 0;i < r;i++)
    {
        for(int j = 0;j < N_col;j++)
        {
            arr[i][j].~T();
        }
    }
    for(int i = 0;i < c;i++) arr[r][i].~T();
    free2d((void**)arr,N_row);
}