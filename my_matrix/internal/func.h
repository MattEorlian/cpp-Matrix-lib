#pragma once

namespace my{
void** alloc2d(int Row,int Col,int Size);//申请row * col个T大小的二维数组
void free2d(void** arr,int row) noexcept;
template<typename T> void rollback2d(T** arr,int N_row,int N_col,int r,int c);//(r,c)处失败；回滚析构(r,c)前的元素，并释放数组
}