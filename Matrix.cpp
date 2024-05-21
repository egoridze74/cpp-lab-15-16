#include "Matrix.h"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <thread>
#include <stack>
#include <future>
#include <chrono>


//Constructors
template<typename T>
Matrix<T>::Matrix() //default constructor
{
    this->height = 0;
    this->width = 0;
    this->rows = NULL;
}

template<typename T>
Matrix<T>::Matrix(const Matrix& other) //copy constructor
{
    this->height = other.height;
    this->width = other.width;
    this->rows = new T * [height];
    for (int i = 0; i < height; i++)
    {
        this->rows[i] = new T[width];
        for (int j = 0; j < width; j++)
            this->rows[i][j] = other.rows[i][j];
    }
}

template<typename T>
Matrix<T>::Matrix(size_t dimension) //one-argument constructor
{
    this->height = dimension;
    this->width = dimension;
    this->rows = new T * [dimension];
    for (int i = 0; i < dimension; i++)
    {
        this->rows[i] = new T[width];
        for (int j = 0; j < width; j++)
            this->rows[i][j] = 0;
    }
}
template<typename T>
Matrix<T>::Matrix(size_t height, size_t width) //two-arguments constructor
{
    this->height = height;
    this->width = width;
    this->rows = new T * [height];
    for (int i = 0; i < height; i++)
    {
        this->rows[i] = new T[width];
        for (int j = 0; j < width; j++)
            this->rows[i][j] = 0;
    }
}

template<typename T>
Matrix<T>::Matrix(size_t height, size_t width, T** rows) //normal constuctor
{
    this->height = height;
    this->width = width;
    this->rows = new T * [height];
    for (int i = 0; i < height; i++)
    {
        this->rows[i] = new T[width];
        for (int j = 0; j < width; j++)
            this->rows[i][j] = 0;
    }
}

template<typename T>
Matrix<T> Matrix<T>::identity(size_t dimension)
{
    Matrix result(dimension);
    for (size_t i = 0; i < dimension; i++)
        result.set_value(i, i, 1);
    return result;
}

template<typename T>
Matrix<T>::~Matrix()
{
    for (int i = 0; i < this->height; i++)
        delete[] this->rows[i];
    delete[] this->rows;
}


//Getters, setters
template<typename T>
inline size_t Matrix<T>::Height() const
{
    return this->height;
}

template<typename T>
inline size_t Matrix<T>::Width() const
{
    return this->width;
}

template<typename T>
const T& Matrix<T>::get_value(size_t row, size_t column) const
{
    return this->rows[row][column];
}

template<typename T>
void Matrix<T>::set_value(size_t row, size_t column, T value)
{
    this->rows[row][column] = value;
}

template<typename T>
void Matrix<T>::set_size(size_t height, size_t width)
{
    for (int i = 0; i < this->height; i++)
        delete[] this->rows[i];
    delete[] this->rows;
    this->height = height;
    this->width = width;
    this->rows = new T * [height];
    for (int i = 0; i < height; i++)
    {
        this->rows[i] = new T[width];
        for (int j = 0; j < width; j++)
            this->set_value(i, j, 0);
    }
}


//Comparisons
template<typename T>
bool Matrix<T>::operator==(const Matrix<T>& other) const
{
    for (size_t i = 0; i < this->Height(); i++)
        for (size_t j = 0; j < this->Width(); j++)
            if (this->get_value(i, j) != other.get_value(i, j))
                return false;
    return true;
}

template<typename T>
bool Matrix<T>::operator!=(const Matrix<T>& other) const
{
    return !(*this == other);
}

template<typename T>
bool Matrix<T>::operator==(const T& k) const
{
    return *this == Matrix<T>::identity(this->Height()) * k;
}

template<typename T>
bool Matrix<T>::operator!=(const T& k) const
{
    return !(*this == k);
}


//Matrix operations
template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& other) //assignment
{
    if (this != &other)
    {
        this->set_size(other.Height(), other.Width());
        for (int i = 0; i < this->Height(); i++)
        {
            this->rows[i] = new T[width];
            for (int j = 0; j < this->Width(); j++)
                this->set_value(i, j, other.get_value(i, j));
        }
    }
    return *this;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const T& k) const //multiplication by number
{
    Matrix result(this->Height(), this->Width());
    for (size_t i = 0; i < this->Height(); i++)
    {
        for (size_t j = 0; j < this->Width(); j++)
        {
            result.set_value(i, j, this->get_value(i, j) * k);
        }
    }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::operator+(const Matrix<T>& other) const
{
    if (this->Height() != other.Height() || this->Width() != other.Width())
        throw std::invalid_argument("Matrixes must have same dimension to do this operation");
    Matrix result(this->Height(), this->Width());
    for (size_t i = 0; i < this->Height(); i++)
    {
        for (size_t j = 0; j < this->Width(); j++)
        {
            T value1 = this->get_value(i, j);
            T value2 = other.get_value(i, j);
            result.set_value(i, j, value1 + value2);
        }
    }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::sum_parallel(const Matrix<T>& other) const
{
    if (this->Height() != other.Height() || this->Width() != other.Width())
        throw std::invalid_argument("Matrixes must have same dimension to do this operation");

    unsigned int threads_max = (this->height <= std::thread::hardware_concurrency() ? this->height
            : std::thread::hardware_concurrency());
    unsigned int opers_per_thread = this->height / threads_max;
    std::stack<std::thread> threads;

    Matrix result(this->Height(), this->Width());

    for (unsigned int i = 0; i < threads_max; ++i)
    {
        size_t j = opers_per_thread * i;
        size_t k = (i == threads_max - 1 ? this->height : j + opers_per_thread);

        threads.emplace([j, k, &result, this, &other]()
            {
                for (size_t l = j; l < k; ++l)
                    for (size_t m = 0; m < this->width; ++m)
                        result.set_value(l, m, this->get_value(l, m) + other.get_value(l, m));
            });
    }

    for (unsigned int i = 0; i < threads_max; ++i)
    {
        threads.top().join();
        threads.pop();
    }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::sum_parallel(const Matrix<T>& other, unsigned int blocks) const
{
    if (this->Height() != other.Height() || this->Width() != other.Width())
        throw std::invalid_argument("Matrixes must have same dimension to do this operation");

    Matrix result(this->Height(), this->Width());

    std::stack<std::future<void>> futures;

    if (this->height < blocks)
        blocks = this->height;
    unsigned int opers_per_thread = this->height / blocks;
    for (unsigned int i = 0; i < blocks; ++i)
    {
        size_t j = opers_per_thread * i;
        size_t k = (i == blocks - 1 ? this->height : j + opers_per_thread);

        futures.emplace(std::async(std::launch::async, [j, k, &result, this, &other]() mutable
        {
            for (size_t l = j; l < k; ++l)
                for (size_t m = 0; m < this->width; ++m)
                    result.set_value(l, m, this->get_value(l, m) + other.get_value(l, m));
        }));
    }


        for (unsigned int i = 0; i < futures.size(); ++i)
        {
            futures.top().wait();
            futures.pop();
        }
        return result;
}


template<typename T>
Matrix<T> Matrix<T>::operator-(const Matrix<T>& other) const
{
    if (this->Height() != other.Height() || this->Width() != other.Width())
        throw std::invalid_argument("Matrixes must have same dimension to do this operation");

    Matrix result(this->Height(), this->Width());
    result = *this + (other * (-1));
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::deduct_parallel(const Matrix<T>& other) const
{
    if (this->Height() != other.Height() || this->Width() != other.Width())
        throw std::invalid_argument("Matrixes must have same dimension to do this operation");

    unsigned int threads_max = (this->height <= std::thread::hardware_concurrency() ? this->height
            : std::thread::hardware_concurrency());
    unsigned int opers_per_thread = this->height / threads_max;
    std::stack<std::thread> threads;


    Matrix result(this->Height(), this->Width());

    for (unsigned int i = 0; i < threads_max; ++i)
    {
        size_t j = opers_per_thread * i;
        size_t k = (i == threads_max - 1 ? this->height : j + opers_per_thread);

        threads.emplace([j, k, &result, this, &other]()
                        {
                            for (size_t l = j; l < k; ++l)
                            {
                                for (size_t m = 0; m < this->width; ++m)
                                    result.set_value(l, m, this->get_value(l, m) - other.get_value(l, m));
                            }
                        });
    }

    for (unsigned int i = 0; i < threads_max; ++i)
    {
        threads.top().join();
        threads.pop();
    }
}

template<typename T>
Matrix<T> Matrix<T>::deduct_parallel(const Matrix<T>& other, unsigned int blocks) const
{
    if (this->Height() != other.Height() || this->Width() != other.Width())
        throw std::invalid_argument("Matrixes must have same dimension to do this operation");

    Matrix result(this->Height(), this->Width());

    std::stack<std::future<void>> futures;

    if (this->height < blocks)
        blocks = this->height;
    unsigned int opers_per_thread = this->height / blocks;
    for (unsigned int i = 0; i < blocks; ++i)
    {
        size_t j = opers_per_thread * i;
        size_t k = (i == blocks - 1 ? this->height : j + opers_per_thread);

        futures.emplace(std::async(std::launch::async, [j, k, &result, this, &other]() mutable
        {
            for (size_t l = j; l < k; ++l)
                for (size_t m = 0; m < this->width; ++m)
                    result.set_value(l, m, this->get_value(l, m) - other.get_value(l, m));
        }));
    }

    for (unsigned int i = 0; i < futures.size(); ++i)
    {
        futures.top().wait();
        futures.pop();
    }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T>& other) const
{
    if (this->Width() != other.Height())
        throw std::invalid_argument("Width of first matrix must be equal to height of second matrix"
                                    " to do this operation");
    Matrix result(this->Height(), other.Width());
    for (size_t i = 0; i < this->Height(); i++)
        for (size_t j = 0; j < other.Width(); j++)
        {
            T value_sum = 0;
            for (size_t k = 0; k < this->Width(); k++)
            {
                T value1;
                T value2;
                value1 = this->get_value(i, k);
                value2 = other.get_value(k, j);
                value_sum += value1 * value2;
            }
            result.set_value(i, j, value_sum);
        }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::multiply_parallel(const Matrix<T>& other) const
{
    if (this->Width() != other.Height())
        throw std::invalid_argument("Width of first matrix must be equal to height of second matrix"
                                    " to do this operation");
    unsigned int threads_max = (this->height <= std::thread::hardware_concurrency() ? this->height
            : std::thread::hardware_concurrency());
    unsigned int opers_per_thread = this->height / threads_max;
    std::stack<std::thread> threads;

    Matrix result(this->Height(), this->Width());

    for (unsigned int i = 0; i < threads_max; ++i)
    {
        size_t j = opers_per_thread * i;
        size_t k = (i == threads_max - 1 ? this->height : j + opers_per_thread);

        threads.emplace([j, k, &result, this, &other]()
                        {
                            for (size_t l = j; l < k; ++l)
                                for (size_t m = 0; m < this->width; ++m)
                                {
                                    T value_sum = 0;
                                    for (size_t n = 0; n< this->Width(); ++n)
                                    {
                                        T value1;
                                        T value2;
                                        value1 = this->get_value(l, n);
                                        value2 = other.get_value(n, m);
                                        value_sum += value1 * value2;
                                    }
                                    result.set_value(l, m, value_sum);
                                }
                        });
    }

    for (unsigned int i = 0; i < threads_max; ++i)
    {
        threads.top().join();
        threads.pop();
    }
    return result;
}

template<typename T>
Matrix<T> Matrix<T>::multiply_parallel(const Matrix<T>& other, unsigned int blocks) const
{
    if (this->Width() != other.Height())
        throw std::invalid_argument("Width of first matrix must be equal to height of second matrix"
                                    " to do this operation");
    Matrix result(this->Height(), this->Width());

    std::stack<std::future<void>> futures;

    if (this->height < blocks)
        blocks = this->height;
    unsigned int opers_per_thread = this->height / blocks;
    for (unsigned int i = 0; i < blocks; ++i)
    {
        size_t j = opers_per_thread * i;
        size_t k = (i == blocks - 1 ? this->height : j + opers_per_thread);

        futures.emplace(std::async(std::launch::async, [j, k, &result, this, &other]() mutable
                        {
                            for (size_t l = j; l < k; ++l)
                                for (size_t m = 0; m < this->width; ++m)
                                {
                                    T value_sum = 0;
                                    for (size_t n = 0; n< this->Width(); ++n)
                                    {
                                        T value1;
                                        T value2;
                                        value1 = this->get_value(l, n);
                                        value2 = other.get_value(n, m);
                                        value_sum += value1 * value2;
                                    }
                                    result.set_value(l, m, value_sum);
                                }
                        }));
    }


    for (unsigned int i = 0; i < futures.size(); ++i)
    {
        futures.top().wait();
        futures.pop();
    }
    return result;
}

template<typename T>
Matrix<T> operator*(const T& k, const Matrix<T>& m) {
    return m * k;
}


//Elementary operations
template<typename T>
void Matrix<T>::swap_rows(size_t row1, size_t row2)
{
    if (row1 >= this->Height() || row2 >= this->Height())
        throw std::invalid_argument("Row index is incorrect");
    std::swap(this->rows[row1], this->rows[row2]);
}

template<typename T>
void Matrix<T>::multiply_row(size_t row, T k)
{
    if (row > this->Height())
        throw std::invalid_argument("Row index is incorrect");
    if (k == 0)
        throw std::invalid_argument("k mustn't be 0");
    for (size_t j = 0; j < this->Width(); j++)
    {
        T value = this->get_value(row, j);
        this->set_value(row, j, value * k);
    }
}

template<typename T>
void Matrix<T>::add_multiplied_row(size_t row1, size_t row2, T k)
{
    if (row1 > this->height || row2 > this->width)
        throw std::invalid_argument("Row index is incorrect");
    if (row1 == row2)
        throw std::invalid_argument("You must pick different rows");
    for (size_t j = 0; j < this->width; j++)
    {
        T value1 = this->get_value(row1, j);
        T value2 = this->get_value(row2, j);
        this->set_value(row1, j, value1 + value2 * k);
    }
}


//Hard operations
template<typename T>
Matrix<T> Matrix<T>::Transpose() const
{
    Matrix result(this->Width(), this->Height());
    for (size_t i = 0; i < this->Width(); i++)
        for (size_t j = 0; j < this->Height(); j++)
            result.set_value(j, i, this->get_value(i, j));
    return result;
}

template<typename T>
double Matrix<T>::determinant() const
{
    size_t size = this->Height();
    if (size != this->Width())
        throw std::invalid_argument("Matrix must be square");
    else if (size == 2)
    {
        T a = this->get_value(0, 0);
        T b = this->get_value(0, 1);
        T c = this->get_value(1, 0);
        T d = this->get_value(1, 1);
        return (a * d - b * c);
    }
    else
    {
        Matrix m(*this);
        T result = 1;
        for (size_t i = 0; i < size; i++)
        {
            size_t k = i;
            T max_abs = m.get_value(k, i);
            for (size_t j = i + 1; j < size; j++)
                if (std::abs(m.get_value(j, i)) > std::abs(max_abs))
                {
                    k = j;
                    max_abs = m.get_value(k, i);
                }
            if (max_abs == 0)
                return 0;
            if (i != k)
                result *= -1;
            m.swap_rows(i, k);
            result *= max_abs;
            m.multiply_row(i, 1 / max_abs);
            for (size_t j = i + 1; j < size; j++)
            {
                if (std::abs(m.get_value(j, i)) == 0)
                    continue;
                T coef = -m.get_value(j, i);
                m.add_multiplied_row(j, i, coef);
            }
        }
        return result;
    }
}

template<typename T>
Matrix<T> Matrix<T>::operator!() const
{
    size_t size = this->Height();
    const T det = this->determinant();
    if (this->determinant() == 0)
    {
        Matrix m = Matrix();
        return m;
    }
    else if (size == 2)
    {
        T a = this->get_value(0, 0);
        T b = this->get_value(0, 1);
        T c = this->get_value(1, 0);
        T d = this->get_value(1, 1);

        Matrix m(size);
        m.set_value(0, 0, d);
        m.set_value(0, 1, -b);
        m.set_value(1, 0, -c);
        m.set_value(1, 1, a);

        return (1 / det) * m;
    }
    else
    {
        Matrix m = Matrix(*this);
        Matrix result = Matrix::identity(size);

        for (size_t i = 0; i < size; i++)
        {
            size_t k = i;
            T max_abs = m.get_value(k, i);
            for (size_t j = i + 1; j < size; j++)
                if (std::abs(m.get_value(j, i)) > std::abs(max_abs))
                {
                    k = j;
                    max_abs = m.get_value(k, i);
                }
            m.swap_rows(i, k);
            result.swap_rows(i, k);

            m.multiply_row(i, 1 / max_abs);
            result.multiply_row(i, 1 / max_abs);
            for (size_t j = 0; j < size; j++)
            {
                if (j == i || std::abs(m.get_value(j, i)) == 0)
                    continue;
                T coef = -m.get_value(j, i);
                m.add_multiplied_row(j, i, coef);
                result.add_multiplied_row(j, i, coef);
            }
        }
        return result;
    }
}


//Input, Output
template<typename T>
std::istream& operator>>(std::istream& in, Matrix<T>& m)
{
    size_t height, width;
    in >> height >> width;
    m.set_size(height, width);
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            T value = 0;
            in >> value;
            m.set_value(i, j, value);
        }
    }
    return in;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& m)
{
    const size_t height = m.Height();
    const size_t width = m.Width();
    if (height == 0) out << "empty";
    else
    {
        for (size_t i = 0; i < height; i++)
        {
            for (size_t j = 0; j < width; j++)
                out << m.get_value(i, j) << ' ';
            out << std::endl;
        }
    }
    return out;
}
