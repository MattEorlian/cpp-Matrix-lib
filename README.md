As you can see this is a AI generated README. ... too lazy for that ...

my::matrix — A Lightweight C++ Matrix Library
my::matrix is a header‑only (with a companion .cpp) C++ matrix library built with modern C++ practices. It provides RAII resource management, move semantics, strong exception safety, and an intuitive operator interface – all without pulling in heavy dependencies.

✨ Features
RAII memory management – automatic allocation/deallocation with alloc2d / free2d and rollback on construction failure.

Move semantics – move constructors and assignment operators are noexcept for efficient transfers.

Exception safety – strong guarantee in constructors and operations (internal rollback2d cleans up on error).

Mixed‑type operations – matrix<int> * matrix<double> yields matrix<double> automatically (via decltype deduction).

Intuitive operators – +, -, *, +=, -=, unary -, unary +, and | (horizontal concatenation), / (vertical concatenation).

Sub‑matrix extraction – submatrix(up, down, left, right) returns a new matrix.

Transpose – in‑place for square matrices, via trans() for any size.

Type conversion – .convert<NewType>() creates a new matrix with converted elements.

🚀 Quick Example
cpp
#include "my_matrix.h"
#include <iostream>

int main() {
    my::matrix<int> a(3, 3, {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    });

    my::matrix<double> b(3, 3, {
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    });

    // Mixed‑type multiplication -> matrix<double>
    auto c = a * b;

    // Horizontal and vertical concatenation
    auto block = (a | b) / (b | a);   // 6x6 block matrix

    std::cout << c << "\n\n" << block << std::endl;
    return 0;
}
🔧 Building & Integration
Option 1: Add to your CMake project
cmake
add_subdirectory(path/to/my_matrix)
target_link_libraries(your_app PRIVATE my_matrix_lib)
Option 2: Manual compilation
bash
g++ -std=c++17 -O2 my_matrix.cpp your_main.cpp -o your_program
Dependencies: Only the C++ standard library (<new>, <stdexcept>, <initializer_list>, <utility>).

📁 Project Structure
text
my_matrix/
├── CMakeLists.txt
├── README.md
├── my_matrix.h
├── my_matrix.cpp
└── internal/
    └── func.h
📖 API Overview
Function	Description
matrix(int rows, int cols, const T& val)	Fill with val
matrix(int rows, int cols, initializer_list)	Construct from list
matrix(const matrix&)	Copy
matrix(matrix&&)	Move
T* operator[](int r)	Row access (non‑const)
const T* operator[](int r) const	Row access (const)
matrix operator+(const matrix&)	Element‑wise addition
matrix operator-(const matrix&)	Element‑wise subtraction
matrix operator*(const matrix&)	Matrix multiplication
matrix operator|(const matrix&)	Horizontal concatenation
matrix operator/(const matrix&)	Vertical concatenation
matrix submatrix(up,down,left,right) const	Extract sub‑matrix
void transpose()	In‑place transpose
matrix<U> convert() const	Convert element type
friend matrix trans(const matrix&)	Returns transposed copy
📄 License
MIT License – free to use, modify, and distribute.

💬 Feedback
Feel free to open an issue or pull request on GitHub. Contributions and suggestions are welcome!
