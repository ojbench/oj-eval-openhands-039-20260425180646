
#ifndef CSR_MATRIX_HPP
#define CSR_MATRIX_HPP

#include <vector>
#include <exception>
#include <algorithm>

namespace sjtu {

class size_mismatch : public std::exception {
public:
    const char *what() const noexcept override {
        return "Size mismatch";
    }
};

class invalid_index : public std::exception {
public:
    const char *what() const noexcept override {
        return "Index out of range";
    }
};

template <typename T>
class CSRMatrix {

private:
    size_t n_rows, n_cols;
    std::vector<size_t> indptr;
    std::vector<size_t> indices;
    std::vector<T> data;

public:
    // Assignment operators are deleted
    CSRMatrix &operator=(const CSRMatrix &other) = delete;
    CSRMatrix &operator=(CSRMatrix &&other) = delete;

    // Constructor for empty matrix with dimensions
    CSRMatrix(size_t n, size_t m) : n_rows(n), n_cols(m), indptr(n + 1, 0) {}

    // Constructor with pre-built CSR components
    CSRMatrix(size_t n, size_t m, size_t count,
        const std::vector<size_t> &indptr, 
        const std::vector<size_t> &indices,
        const std::vector<T> &data) : n_rows(n), n_cols(m), indptr(indptr), indices(indices), data(data) {
        
        // Validate dimensions
        if (indptr.size() != n + 1) {
            throw size_mismatch();
        }
        if (indices.size() != count || data.size() != count) {
            throw size_mismatch();
        }
        
        // Validate indptr values
        if (indptr[0] != 0) {
            throw size_mismatch();
        }
        for (size_t i = 1; i <= n; ++i) {
            if (indptr[i] < indptr[i-1]) {
                throw size_mismatch();
            }
        }
        
        // Validate indices values
        for (size_t i = 0; i < count; ++i) {
            if (indices[i] >= m) {
                throw size_mismatch();
            }
        }
    }

    // Copy constructor
    CSRMatrix(const CSRMatrix &other) = default;

    // Move constructor
    CSRMatrix(CSRMatrix &&other) = default;

    // Constructor from dense matrix format (given as vector of vectors)
    CSRMatrix(size_t n, size_t m, const std::vector<std::vector<T>> &dense_data) : n_rows(n), n_cols(m) {
        // Validate dimensions
        if (dense_data.size() != n) {
            throw size_mismatch();
        }
        for (const auto &row : dense_data) {
            if (row.size() != m) {
                throw size_mismatch();
            }
        }

        // Build CSR format
        indptr.resize(n + 1, 0);
        size_t nnz = 0;
        
        // First pass: count non-zero elements in each row
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < m; ++j) {
                if (dense_data[i][j] != T{}) {
                    ++nnz;
                }
            }
            indptr[i + 1] = nnz;
        }
        
        // Allocate space for indices and data
        indices.resize(nnz);
        data.resize(nnz);
        
        // Second pass: fill indices and data
        size_t idx = 0;
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < m; ++j) {
                if (dense_data[i][j] != T{}) {
                    indices[idx] = j;
                    data[idx] = dense_data[i][j];
                    ++idx;
                }
            }
        }
    }

    // Destructor
    ~CSRMatrix() = default;

    // Get dimensions and non-zero count
    size_t getRowSize() const {
        return n_rows;
    }

    size_t getColSize() const {
        return n_cols;
    }

    size_t getNonZeroCount() const {
        return data.size();
    }

    // Element access
    T get(size_t i, size_t j) const {
        // Check bounds
        if (i >= n_rows || j >= n_cols) {
            throw invalid_index();
        }
        
        // Binary search in the row i
        size_t start = indptr[i];
        size_t end = indptr[i + 1];
        
        // Linear search since the number of non-zeros per row is typically small
        for (size_t k = start; k < end; ++k) {
            if (indices[k] == j) {
                return data[k];
            }
        }
        
        // If not found, return default value
        return T{};
    }

    void set(size_t i, size_t j, const T &value) {
        // Check bounds
        if (i >= n_rows || j >= n_cols) {
            throw invalid_index();
        }
        
        // Find the position in the row i
        size_t start = indptr[i];
        size_t end = indptr[i + 1];
        
        // Binary search for the column index since indices are sorted
        size_t pos = start;
        size_t left = start, right = end;
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            if (indices[mid] < j) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        pos = left;
        
        if (pos < end && indices[pos] == j) {
            // Element exists, update it
            data[pos] = value;
        } else {
            // Element doesn't exist, insert it
            if (value != T{}) {
                // Insert only if value is non-zero
                indices.insert(indices.begin() + pos, j);
                data.insert(data.begin() + pos, value);
                
                // Update indptr for all subsequent rows
                for (size_t r = i + 1; r <= n_rows; ++r) {
                    ++indptr[r];
                }
            }
            // If value is zero, do nothing (don't insert)
        }
    }

    // Access CSR components
    const std::vector<size_t> &getIndptr() const {
        return indptr;
    }

    const std::vector<size_t> &getIndices() const {
        return indices;
    }

    const std::vector<T> &getData() const {
        return data;
    }

    // Convert to dense matrix format
    std::vector<std::vector<T>> getMatrix() const {
        std::vector<std::vector<T>> result(n_rows, std::vector<T>(n_cols, T{}));
        
        for (size_t i = 0; i < n_rows; ++i) {
            size_t start = indptr[i];
            size_t end = indptr[i + 1];
            for (size_t k = start; k < end; ++k) {
                result[i][indices[k]] = data[k];
            }
        }
        
        return result;
    }

    // Matrix-vector multiplication
    std::vector<T> operator*(const std::vector<T> &vec) const {
        // Check dimensions
        if (vec.size() != n_cols) {
            throw size_mismatch();
        }
        
        std::vector<T> result(n_rows, T{});
        
        for (size_t i = 0; i < n_rows; ++i) {
            size_t start = indptr[i];
            size_t end = indptr[i + 1];
            T sum = T{};
            for (size_t k = start; k < end; ++k) {
                sum += data[k] * vec[indices[k]];
            }
            result[i] = sum;
        }
        
        return result;
    }

    // Row slicing
    CSRMatrix getRowSlice(size_t l, size_t r) const {
        // Check bounds
        if (l >= r || r > n_rows) {
            throw invalid_index();
        }
        
        // Calculate number of non-zero elements in the slice
        size_t count = indptr[r] - indptr[l];
        
        // Create new indptr for the slice
        std::vector<size_t> new_indptr(r - l + 1);
        for (size_t i = 0; i <= r - l; ++i) {
            new_indptr[i] = indptr[l + i] - indptr[l];
        }
        
        // Create new indices and data
        std::vector<size_t> new_indices(indices.begin() + indptr[l], indices.begin() + indptr[r]);
        std::vector<T> new_data(data.begin() + indptr[l], data.begin() + indptr[r]);
        
        return CSRMatrix(r - l, n_cols, count, new_indptr, new_indices, new_data);
    }
};

}

#endif // CSR_MATRIX_HPP
