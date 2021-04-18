#ifndef  SERIALIZER_H
#define SERIALIZER_H

#include <vector>
#include <string>
#include "Matrix.h"

//TODO Implement serialization
class Serializer{
    public:
    /**
     * @brief Unserialises a collection of matrices.
     * 
     * @tparam T The type of the elements of the matrices.
     * @param coded The serialized matrices.
     * @param encoding The type of encoding used for the matrices.
     * 
     * @return std::vector<Matrix<T>> The collection of decoded matrices.
     */
    template<typename T>
    static std::vector<Matrix<T>> unserializeMatrices(std::string coded, std::string encoding=nullptr);

    /**
     * @brief Serializes a collection of matrices.
     * 
     * @tparam T The type of the elements of the matrix.
     * @param list the collection of matrices to serialize.
     * @param encoding The optional encoding type of the elements for the transfer.
     * @return std::string The serialized matrices as a string.
     */
    template<typename T>
    static std::string serialize(std::vector<Matrix<T>> list, std::string encoding=nullptr);

    /**
     * @brief Serializes a (Row, Column) pair. 
     * 
     * @tparam T The type of the elements of the Row and Column.
     * @param row The Row object to serialize.
     * @param col The Column object to serialize.
     * @param encoding The optional encoding type of the elements for the transfer.
     * @return std::string The serialized (Row, Column) pair as a string.
     */
    template<typename T>
    static std::string serialize(Row<T> row ,Column<T> col, std::string encoding=nullptr);

    /**
     * @brief Unserializes a (Row, Column) pair.
     * 
     * @tparam T The type of the elements of the Row and Column.
     * @param coded The serialized pair.
     * @param encoding The optional encoding type of the elements for the transfer.
     * @return std::pair<Row<T>,Column<T>> The unserialized (Row, Column) pair.
     */
    template<typename T>
    static std::pair<Row<T>,Column<T>> unserializeRowColumn(string coded, std::string encoding=nullptr);

    /**
     * @brief Unserializes a matrix.
     * 
     * @tparam T The type of the elements of the matrix.
     * @param coded The serialized matrix.
     * @param encoding The optional encoding type of the elements for the transfer.
     * @return Matrix<T> The Unserialized Matrix object.
     */
    template<typename T>
    static Matrix<T> unserializeMatrix(std::string coded, std::string encoding=nullptr);

    /**
     * @brief Serializes a matrix.
     * 
     * @tparam T the type of the elements of the matrix.
     * @param mat The Matrix object to serialize.
     * @param encoding The optional encoding type of the elements for the transfer.
     * @return std::string The serialized matrix as a string.
     */
    template<typename T>
    static std::string serialize(Matrix<T> mat, std::string encoding=nullptr);
};

#endif