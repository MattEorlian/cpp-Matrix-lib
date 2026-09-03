#pragma once

#include <vector>
#include <stdexcept>
#include <new>
#include <iterator>
#include <initializer_list>
#include <utility>

#include "internal/func.h"



namespace my{

template<typename T> class matrix
{
private:
    int N_row;
    int N_col;
    T** arr;
private://私有构造函数
    matrix(int Row,int Col,T** ptr)//直接根据三个成员创建矩阵，不对外开放
        :N_row(Row),N_col(Col),arr(ptr)
    {}

public://构造与析构
    matrix(int Row,int Col,const T& Val)//创建row行，col列的矩阵，全部初始化为Val
        :N_row(Row),N_col(Col)
    {
        if(N_row <= 0 || N_col <= 0)
        {
            throw std::invalid_argument("Member \"N_row\" and \"N_col\" must be positive.");
        }
        arr = (T**)alloc2d(N_row,N_col,sizeof(T));
        for(int i =0;i < N_row;i++)
        {
            for(int j = 0;j < N_col;j++)
            {
                try
                {
                    new (&arr[i][j]) T(Val);
                }
                catch(...)
                {
                    rollback2d(arr,N_row,N_col,i,j);
                    throw;
                }
            }
        }
    }

    matrix(int row,int col,const std::initializer_list<T>& list)
        : N_row(row),N_col(col)
    {
        if(N_row <= 0 || N_col <= 0)
        {
            throw std::invalid_argument("Member \"N_row\" and \"N_col\" must be positive.");
        }
        if(list.size() != N_row * N_col)
        {
            throw std::invalid_argument("Size of \"list\" must be row * col.");
        }
        arr = (T**)alloc2d(N_row,N_col,sizeof(T));
        auto cur = list.begin();
        for(int i = 0;i < N_row;i++)
        {
            for(int j = 0;j < N_col;j++)
            {
                try{
                    new (&arr[i][j]) T(std::move((*cur)));
                    cur++;
                }
                catch(...)
                {
                    rollback2d(arr,N_row,N_col,i,j);
                    throw;
                }
            }
        }
    }

    matrix(const matrix& other)
        :N_row(other.N_row),N_col(other.N_col)
    {
        if(N_row <= 0 || N_col <= 0)
        {
            throw std::invalid_argument("Member \"N_row\" and \"N_col\" must be positive.");
        }
        if(!other.arr)
        {
            throw std::invalid_argument("Invalid Source to be copied. Member \"arr\" is a null.");
        }
        arr = (T**)alloc2d(N_row,N_col,sizeof(T));
        for(int i = 0;i < N_row;i++)
        {
            for(int j = 0;j < N_col;j++)
            {
                try{
                    new (&arr[i][j]) T(other.arr[i][j]);
                }
                catch(...)
                {
                    rollback2d(arr,N_row,N_col,i,j);
                    throw;
                }
            }
        }
    }

    matrix(matrix&& other) noexcept//移动构造
        :N_row(other.N_row),N_col(other.N_col),arr(other.arr)
    {
        other.N_row = 0;
        other.N_col = 0;
        other.arr = nullptr;
    }

    ~matrix() noexcept
    {
        destroy();
        arr = nullptr;
        N_row = 0;
        N_col = 0;
    }

private://工具函数
    void destroy() noexcept
    {
        if(N_row <= 0 || N_col <= 0 || arr == nullptr) return;
        for(int i = 0;i < N_row;i++)
        {
            if(!arr[i]) continue;
            for(int j = 0;j < N_col;j++)
            {
                arr[i][j].~T();
            }
            ::operator delete(arr[i]);
        }
        ::operator delete(arr);
        arr = nullptr;
        N_row = 0;
        N_col = 0;
    }
public://赋值
    void swap(matrix& other) noexcept
    {
        std::swap(N_row, other.N_row);
        std::swap(N_col, other.N_col);
        std::swap(arr, other.arr);
    }
    matrix& operator=(const matrix& other)
    {
        if(this == &other) return *this;
        matrix tmp(other);
        swap(tmp);
        return *this;
    }

    matrix& operator=(matrix&& other) noexcept
    {
        if(this == & other) return *this;
        
        destroy();

        N_row = other.N_row;
        N_col = other.N_col;
        arr = other.arr;

        other.N_row = 0;
        other.N_col = 0;
        other.arr = nullptr;
        return *this;
    }

public://读取
    int row() const noexcept{
        return N_row;
    }
    int col() const noexcept{
        return N_col;
    }
    T* operator[](int R) noexcept { return arr[R]; }
    const T* operator[](int R) const noexcept { return arr[R]; }

public://运算
    void transpose()
    {
        if(N_row == N_col)
        {
            for(int i = 0;i < N_row - 1;i++)
            {
                for(int j = i + 1;j < N_col;j++)
                {
                    std::swap(arr[i][j],arr[j][i]);
                }
            }
        }
        else *this = trans(*this);
    }
    template<typename U> void operator*=(const U& Val)
    {
        for(int i = 0;i < N_row;i++)
        {
            for(int j = 0;j < N_col;j++)
            {
                arr[i][j] *= Val;
            }
        }
    }
    void operator+=(const matrix& other)
    {
        if(N_row != other.N_row || N_col != other.N_col)
        {
            throw std::invalid_argument("Matrix size differs when been subdized or added.");
        }
        for(int i = 0;i < N_row;i++)
        {
            for(int j = 0;j < N_col;j++)
            {
                arr[i][j] += other.arr[i][j];
            }
        }
    }
    void operator-=(const matrix& other)
    {
        if(N_row != other.N_row || N_col != other.N_col)
        {
            throw std::invalid_argument("Matrix size differs when been subdized or added.");
        }
        for(int i = 0;i < N_row;i++)
        {
            for(int j = 0;j < N_col;j++)
            {
                arr[i][j] -= other.arr[i][j];
            }
        }
    }
    matrix operator-() const
    {
        matrix tmp(*this);
        for(int i = 0;i < tmp.N_row;i++)
        {
            for(int j = 0;j < tmp.N_col;j++)
            {
                tmp[i][j] = -tmp[i][j];
            }
        }
        return tmp;
    }
    matrix operator+() const
    {
        return *this;
    }
    matrix operator+(const matrix& other) const
    {
        if(N_row != other.N_row || N_col != other.N_col)
        {
            throw std::invalid_argument("Matrix size differs when been subdized or added.");
        }
        matrix tmp(*this);
        tmp += other;
        return tmp;
    }
    matrix operator-(const matrix& other) const
    {
        if(N_row != other.N_row || N_col != other.N_col)
        {
            throw std::invalid_argument("Matrix size differs when been subdized or added.");
        }
        matrix tmp(*this);
        tmp -= other;
        return tmp;
    }
    
    template<typename T_Rhs> auto operator*(const matrix<T_Rhs>& other) const
    {
        if(N_col != other.N_row)
        {
            throw std::invalid_argument("N_col of lhs is not equal to N_row of rhs when been multiplied.");
        }
        int tmp_row = N_row, tmp_col = other.N_col;
        using T_Dst = decltype(arr[0][0] * other[0][0]);
        T_Dst** tmp_arr = (T_Dst**)alloc2d(tmp_row,tmp_col,sizeof(T_Dst));
        for(int i = 0;i < tmp_row;i++)
        {
            for(int j = 0;j < tmp_col;j++)
            {
                try{
                    new (&tmp_arr[i][j]) T_Dst(prod_row_and_col((*this),i,other,j));
                }
                catch(...)
                {
                    rollback2d(tmp_arr,tmp_row,tmp_col,i,j);
                    throw;
                }
            }
        }
        return matrix<T_Dst>(tmp_row,tmp_col,tmp_arr);
    }

    matrix operator|(const matrix& other) const//横向拼接矩阵
    {
        if(N_row != other.N_row) throw std::invalid_argument("N_row differs when connecting.");
        int tmp_row = N_row,tmp_col = N_col + other.N_col;
        T** tmp_arr = (T**)alloc2d(tmp_row,tmp_col,sizeof(T));
        for(int i = 0;i < tmp_row;i++)
        {
            for(int j = 0;j < tmp_col;j++)
            {
                try{
                    new (&tmp_arr[i][j]) T(
                        (j < N_col) ? arr[i][j] : other.arr[i][j - N_col]
                    );
                }
                catch(...)
                {
                    rollback2d(tmp_arr,tmp_row,tmp_col,i,j);
                    throw;
                }
            }
        }
        return matrix(tmp_row,tmp_col,tmp_arr);
    }

    matrix operator/(const matrix& other) const//纵向拼接矩阵
    {
        if(N_col != other.N_col) throw std::invalid_argument("N_col differs when connecting.");
        int tmp_row = N_row + other.N_row,tmp_col = N_col;
        T** tmp_arr = (T**)alloc2d(tmp_row,tmp_col,sizeof(T));
        for(int i = 0;i < tmp_row;i++)
        {
            for(int j = 0;j < tmp_col;j++)
            {
                try{
                    new (&tmp_arr[i][j]) T(
                        (i < N_row) ? arr[i][j] : other.arr[i - N_row][j]
                    );
                }
                catch(...)
                {
                    rollback2d(tmp_arr,tmp_row,tmp_col,i,j);
                    throw;
                }
            }
        }
        return matrix(tmp_row,tmp_col,tmp_arr);
    }
    matrix submatrix(int up,int down,int left,int right) const//子矩阵:取行数[up,down],列数[left,right]的子矩阵
    {
        if(up == 0 && down == N_row - 1 && left == 0 && right == N_col - 1) return *this;
        if(up < 0 || up >= N_row ||
        down < 0 || down >= N_row ||
        left < 0 || left >= N_col ||
        right < 0 || right >= N_col)
        {
            throw std::invalid_argument("row or col index out of range.");
        }
        if(up > down || left > right) throw std::invalid_argument("up > down or left > right.");
        int tmp_row = down - up + 1,tmp_col = right - left + 1;
        T** tmp_arr = (T**)alloc2d(tmp_row,tmp_col,sizeof(T));
        for(int i = 0;i < tmp_row;i++)
        {
            for(int j = 0;j < tmp_col;j++)
            {
                try
                {
                    new (&tmp_arr[i][j]) T(arr[up + i][left + j]);
                }
                catch(...)
                {
                    rollback2d(tmp_arr,tmp_row,tmp_col,i,j);
                    throw;
                }
                
            }
        }
        return matrix(tmp_row,tmp_col,tmp_arr);
    }
public://类型转换
    template<typename T_Dst> matrix<T_Dst> convert() const//static_cast为T_Dst
    {
        int tmp_row = N_row,tmp_col = N_col;
        T_Dst** tmp_arr = (T_Dst**)alloc2d(tmp_row,tmp_col,sizeof(T_Dst));
        for(int i = 0;i < tmp_row;i++)
        {
            for(int j = 0;j < tmp_col;j++)
            {
                try
                {
                    tmp_arr[i][j] = static_cast<T_Dst>(arr[i][j]);
                }
                catch(...)
                {
                    rollback2d(tmp_arr,tmp_row,tmp_col,i,j);
                    throw;
                }
            }
        }
        return matrix<T_Dst>(tmp_row,tmp_col,tmp_arr);
    }
public://友元
    template<typename U> friend class matrix;

    template<typename U>
    friend matrix<U> trans(const matrix<U>& obj);

    template<typename T_Mat,typename T_Dst> 
    friend matrix<T_Dst> convert(const matrix<T_Mat>& src);

    template<typename T_Mat,typename T_Val> 
    friend auto operator*(const matrix<T_Mat>& mat,const T_Val& val);

    template<typename T_Mat,typename T_Val> 
    friend auto operator*(const T_Val& val,const matrix<T_Mat>& mat);
};

template<typename T_Lhs,typename T_Rhs> auto prod_row_and_col(const matrix<T_Lhs>& lhs,int row_idx,const matrix<T_Rhs>& rhs,int col_idx)
{
    if(row_idx < 0 || row_idx >= lhs.row()||
    col_idx < 0 || col_idx >= rhs.col())
    {
        throw std::invalid_argument("Row or column index out of range.");
    }
    if(lhs.col() != rhs.row()) throw std::invalid_argument("Matrix size differs.");
    auto ans = lhs[row_idx][0] * rhs[0][col_idx];
    for(int i = 1;i < lhs.col();i++)
    {
        ans += lhs[row_idx][i] * rhs[i][col_idx];
    }
    return ans;
}

template<typename T> matrix<T> trans(const matrix<T>& obj)//obj的转置矩阵
{
    int tmp_row = obj.col(),tmp_col = obj.row();
    T** tmp_arr = (T**)alloc2d(tmp_row,tmp_col,sizeof(T));
    for(int i = 0;i < tmp_row;i++)
    {
        for(int j = 0;j < tmp_col;j++)
        {
            try
            {
                new (&tmp_arr[i][j]) T(obj[j][i]);
            }
            catch(...)
            {
                rollback2d(tmp_arr,tmp_row,tmp_col,i,j);
                throw;
            }
        }
    }
    return matrix(tmp_row,tmp_col,tmp_arr);
}

template<typename T_Mat,typename T_Val> auto operator*(const matrix<T_Mat>& mat,const T_Val& val)
{
    using T_Dst = decltype(mat[0][0] * val);
    int tmp_row = mat.row();
    int tmp_col = mat.col();
    T_Dst** tmp_arr = (T_Dst**)alloc2d(tmp_row,tmp_col,sizeof(T_Dst));
    for(int i = 0;i < tmp_row;i++)
    {
        for(int j = 0;j < tmp_col;j++)
        {
            try
            {
                new (&tmp_arr[i][j]) T_Dst(mat[i][j] * val);
            }
            catch(...)
            {
                rollback2d(tmp_arr,tmp_row,tmp_col,i,j);
                throw;
            }
        }
    }
    return matrix(tmp_row,tmp_col,tmp_arr);
}

template<typename T_Mat,typename T_Val> auto operator*(const T_Val& val,const matrix<T_Mat>& mat)
{
    using T_Dst = decltype(mat[0][0] * val);
    int tmp_row = mat.row();
    int tmp_col = mat.col();
    T_Dst** tmp_arr = (T_Dst**)alloc2d(tmp_row,tmp_col,sizeof(T_Dst));
    for(int i = 0;i < tmp_row;i++)
    {
        for(int j = 0;j < tmp_col;j++)
        {
            try
            {
                new (&tmp_arr[i][j]) T_Dst(mat[i][j] * val);
            }
            catch(...)
            {
                rollback2d(tmp_arr,tmp_row,tmp_col,i,j);
                throw;
            }
        }
    }
    return matrix(tmp_row,tmp_col,tmp_arr);
}

}