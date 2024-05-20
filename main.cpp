#include "Matrix.h"
#include "Matrix.cpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <chrono>

int main() {
    int k1 = 3, k2 = 4;
    Matrix<double> matrix1, matrix2, matrix3, matrix4;
    std::cin >> matrix1;
    std::cin >> matrix2;
    std::cout << matrix1.sum_async(matrix2);
/*
    std::cout << "Insert matrix1" << std::endl;
        3 3
        5 2 -1
        3 0 7
        6 1 -2

    std::cin >> matrix1;
    std::cout << std::endl << "Original Matrix A" << std::endl;
    std::cout << matrix1 << std::endl;
    matrix2 = !matrix1;
    std::cout << "Original Matrix A-1" << std::endl;
    std::cout << matrix2 << std::endl;
    matrix1.add_multiplied_row(1, 0, 2);
    std::cout << "New Matrix A" << std::endl;
    std::cout << matrix1;
    matrix2 = !matrix1;
    std::cout << "New Matrix A-1" << std::endl;
    std::cout << matrix2;
    std::cin >> matrix2;
    std::cout << matrix1 << std::endl;
    std::cout << matrix2 * matrix1 << std::endl;
    std::cout << "We get matrix2 from input.txt" << std::endl;
    std::ifstream in("input.txt");
    in >> matrix2;
    in.close();
    std::cout << matrix2 << std::endl;

    std::cout << "If matrix1 != k1 we multiply them" << std::endl;
    if (matrix1 != k1)
        matrix1 = matrix1 * k1;
    std::cout << matrix1 << std::endl;

    std::cout << "If matrix2 != k2 we multiply them" << std::endl;
    if (matrix2 != k2)
        matrix2 = matrix2 * k2;
    std::cout << matrix2 << std::endl;

    std::cout << "We make some elementary operations on matrix1:" << std::endl;
    std::cout << "Swap rows 0 and 2" << std::endl;
    matrix1.swap_rows(0, 2);
    std::cout << matrix1 << std::endl;

    std::cout << "Multiply row 1 by 0.5" << std::endl;
    matrix1.multiply_row(1, 0.5);
    std::cout << matrix1 << std::endl;

    std::cout << "Add row 0 multiplied by -1 to row 1" << std::endl;
    matrix1.add_multiplied_row(1, 0, -1);
    std::cout << matrix1 << std::endl;

    std::cout << "We multiply matrix2 by matrix1 inverse" << std::endl;
    matrix3 = matrix1 * !matrix2;
    std::cout << matrix3 << std::endl;

    std::cout << "We print resulting matrix and it's discriminant to the console" << std::endl;
    std::cout << matrix3 << std::endl;
    std::cout << matrix3.determinant() << std::endl;

    std::cout << "We sum up matrix2 and matrix3 and print it to the file" << std::endl;
    matrix4 = matrix2 + matrix3;
    std::ofstream out("output.txt");
    out << matrix4 << std::endl;
    out.close();
*/
    return 0;
}
