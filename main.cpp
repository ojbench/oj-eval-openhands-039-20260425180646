
#include <iostream>
#include <vector>
#include "CSRMatrix.hpp"

int main() {
    // The problem doesn't specify input/output format, so we'll implement basic operations
    // This is a placeholder main function that can be used for testing
    
    // Read operation type
    std::string op;
    while (std::cin >> op) {
        if (op == "CREATE") {
            // Create a matrix with given dimensions
            size_t n, m;
            std::cin >> n >> m;
            sjtu::CSRMatrix<int> matrix(n, m);
            
            // Store matrix in a map for later use (in a real implementation)
            // For now, just acknowledge creation
            std::cout << "MATRIX_CREATED" << std::endl;
        }
        else if (op == "GET") {
            // Get element at (i,j)
            size_t i, j;
            std::cin >> i >> j;
            // In a real implementation, we would have a matrix instance
            // For now, just acknowledge the operation
            std::cout << "GET_OPERATION" << std::endl;
        }
        else if (op == "SET") {
            // Set element at (i,j) to value
            size_t i, j, value;
            std::cin >> i >> j >> value;
            // In a real implementation, we would have a matrix instance
            // For now, just acknowledge the operation
            std::cout << "SET_OPERATION" << std::endl;
        }
        else if (op == "MULTIPLY") {
            // Matrix-vector multiplication
            // In a real implementation, we would have a matrix and vector
            // For now, just acknowledge the operation
            std::cout << "MULTIPLY_OPERATION" << std::endl;
        }
        else if (op == "SLICE") {
            // Row slicing
            size_t l, r;
            std::cin >> l >> r;
            // In a real implementation, we would have a matrix instance
            // For now, just acknowledge the operation
            std::cout << "SLICE_OPERATION" << std::endl;
        }
        else if (op == "EXIT") {
            break;
        }
    }
    
    return 0;
}
