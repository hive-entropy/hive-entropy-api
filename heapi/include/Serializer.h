#ifndef  SERIALIZER_H
#define SERIALIZER_H

#include <vector>
#include <string.h>
#include <spdlog/spdlog.h>

#include "Matrix.h"
#include "Column.h"
#include "Row.h"

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
        static std::vector<Matrix<T>> unserializeMatrices(std::string coded, std::string encoding="same");

        /**
         * @brief Serializes a collection of matrices.
         * 
         * @tparam T The type of the elements of the matrix.
         * @param list the collection of matrices to serialize.
         * @param encoding The optional encoding type of the elements for the transfer.
         * @return std::string The serialized matrices as a string.
         */
        template<typename T>
        static std::string serialize(std::vector<Matrix<T>> list, std::string encoding="same");

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
        static std::string serialize(Row<T> row ,Column<T> col, std::string encoding="same");

        /**
         * @brief Unserializes a (Row, Column) pair.
         * 
         * @tparam T The type of the elements of the Row and Column.
         * @param coded The serialized pair.
         * @param encoding The optional encoding type of the elements for the transfer.
         * @return std::pair<Row<T>,Column<T>> The unserialized (Row, Column) pair.
         */
        template<typename T>
        static std::pair<Row<T>,Column<T>> unserializeRowColumn(std::string coded, std::string encoding="same");

        /**
         * @brief Unserializes a matrix.
         * 
         * @tparam T The type of the elements of the matrix.
         * @param coded The serialized matrix.
         * @param encoding The optional encoding type of the elements for the transfer.
         * @return Matrix<T> The Unserialized Matrix object.
         */
        template<typename T>
        static Matrix<T> unserializeMatrix(std::string coded, std::string encoding="same");

        /**
         * @brief Serializes a matrix.
         * 
         * @tparam T The type of the elements of the matrix.
         * @param mat The Matrix object to serialize.
         * @param encoding The optional encoding type of the elements for the transfer.
         * @return std::string The serialized matrix as a string.
         */
        template<typename T>
        static std::string serialize(Matrix<T> mat, std::string encoding="same");
};

template<typename T>
std::vector<Matrix<T>> Serializer::unserializeMatrices(std::string coded, std::string encoding){
    std::vector<Matrix<T>> list = std::vector<Matrix<T>>();

    if(coded.length()<2*sizeof(uint16_t))
        throw "The serialized list should at least contain one element";

    char* content = (char*) malloc(coded.length());
    memcpy(content,coded.c_str(),coded.length());

    int length = coded.length();

    while(length>0){
        uint16_t dimensions[2];
        memcpy(&dimensions,content,2*sizeof(uint16_t));
        int rows = dimensions[0];
        int cols = dimensions[1];
        spdlog::debug("Found matrix of dimensions {}x{} to unserialize",rows,cols);

        list.push_back(unserializeMatrix<T>(coded.substr(coded.length()-length,rows*cols*sizeof(T)+2*sizeof(uint16_t))));
        length -= sizeof(T)*rows*cols+2*sizeof(uint16_t);
        content += sizeof(T)*rows*cols+2*sizeof(uint16_t);
    }

    return list;
}

template<typename T>
std::string Serializer::serialize(std::vector<Matrix<T>> list, std::string encoding){
    std::string concat="";
    for(Matrix<T> m : list){
        concat += serialize(m);
    }
    return concat;
}

template<typename T>
std::string Serializer::serialize(Row<T> row ,Column<T> col, std::string encoding){

    //Row dimensions
    char* dims_row = (char*) malloc(2);
    uint8_t int_dims_row[] = {(uint8_t) row.getSize(), (uint8_t) row.getPosition()};
    dims_row = reinterpret_cast<char*>(int_dims_row);

    //Column dimensions
    char* dims_col = (char*) malloc(2);
    uint8_t int_dims_col[] = {(uint8_t) col.getSize(), (uint8_t) col.getPosition()};
    dims_col = reinterpret_cast<char*>(int_dims_col);

    if(!std::is_same<T,int>::value&&!std::is_same<T,float>::value&&!std::is_same<T,double>::value)
        throw "Matrix serialization doesn't support matrices of "+std::string(typeid(T).name());

    //Filling row body
    T* t_body_row = (T*) malloc(row.getSize()*sizeof(T));
    char* body_row = (char*) malloc(row.getSize()*sizeof(T));
    for(int i=0;i<row.getSize();i++)
            *(t_body_row+i) = row.get(i);
    body_row = reinterpret_cast<char*>(t_body_row);

    //Filling col body
    T* t_body_col = (T*) malloc(col.getSize()*sizeof(T));
    char* body_col = (char*) malloc(col.getSize()*sizeof(T));
    for(int i=0;i<col.getSize();i++)
            *(t_body_col+i) = col.get(i);
    body_col = reinterpret_cast<char*>(t_body_col);

    std::string ser_dims_row(dims_row,2);
    std::string ser_dims_col(dims_col,2);
    std::string serialized_row(body_row,row.getSize()*sizeof(T));
    std::string serialized_col(body_col,col.getSize()*sizeof(T));
    return ser_dims_row+serialized_row+ser_dims_col+serialized_col;
}

template<typename T>
std::pair<Row<T>,Column<T>> Serializer::unserializeRowColumn(std::string coded, std::string encoding){
    if(coded.length()<1)
        throw "The serialized row should at least contain its size and position";

    char* content = (char*) malloc(coded.length());
    memcpy(content,coded.c_str(),coded.length());

    //Row dimensions parsing
    int rowsize = static_cast<uint8_t>(content[0]);
    int rowpos = static_cast<uint8_t>(content[1]);

    if(coded.length()-2<rowsize*sizeof(T))
        throw "There are not enough coded elements to describe the whole row of size "+rowsize;

    //Row body parsing
    content += 2;
    T* t_content_row = reinterpret_cast<T*>(content);
    Row<T> row(rowsize,rowpos,t_content_row);

    content += rowsize*sizeof(T);

    //Row dimensions parsing
    int colsize = static_cast<uint8_t>(content[0]);
    int colpos = static_cast<uint8_t>(content[1]);

    if(coded.length()-2<colsize*sizeof(T))
        throw "There are not enough coded elements to describe the whole column of size "+rowsize;

    //Row body parsing
    content += 2;
    T* t_content_col = reinterpret_cast<T*>(content);
    Column<T> col(colsize,colpos,t_content_col);

    return std::pair<Row<T>,Column<T>>(row,col);
}


template<typename T>
Matrix<T> Serializer::unserializeMatrix(std::string coded, std::string encoding){
    if(coded.length()<2*sizeof(uint16_t))
        throw "The serialized object should at least contain the dimensions";

    char* content = (char*) malloc(coded.length());
    memcpy(content,coded.c_str(),coded.length());

    //Dimensions parsing
    uint16_t dimensions[2];
    memcpy(dimensions,content,2*sizeof(uint16_t));
    int rows = dimensions[0];
    int cols = dimensions[1];

    if(coded.length()-2*sizeof(uint16_t)!=rows*cols*sizeof(T)){
        spdlog::error("The serialized size doesn't correspond to the dimensions (received {}, needed {}x{}={})",coded.length()-2*sizeof(uint16_t),rows,cols,rows*cols*sizeof(T));
        throw "The body of the serialized matrix must be equal to its dimensions mutliplied together ("+std::to_string(rows)+"x"+std::to_string(cols)+")";
    }

    Matrix<T> deserialized(rows,cols);
    content = content+2*sizeof(uint16_t);
    T* t_content = reinterpret_cast<T*>(content);

    //Body parsing
    for(int i=0;i<rows;i++){
        for(int j=0;j<cols;j++){
            deserialized.put(i,j,*(t_content+i*cols+j));
        }
    }

    return deserialized;
}


template<typename T>
std::string Serializer::serialize(Matrix<T> mat, std::string encoding){
    char* dims = (char*) malloc(2*sizeof(uint16_t));
    uint16_t int_dims[] = {(uint16_t) mat.getRows(), (uint16_t) mat.getColumns()};
    memcpy(dims,int_dims,2*sizeof(uint16_t));

    if(!std::is_same<T,int>::value&&!std::is_same<T,float>::value&&!std::is_same<T,double>::value&&!std::is_same<T,unsigned short>::value)
        throw "Matrix serialization doesn't support matrices of "+std::string(typeid(T).name());

    T* t_body = (T*) malloc(mat.getColumns()*mat.getRows()*sizeof(T));
    char* body = (char*) malloc(mat.getColumns()*mat.getRows()*sizeof(T));
    for(int i=0;i<mat.getRows();i++)
        for(int j=0;j<mat.getColumns();j++)
            *(t_body+i*mat.getColumns()+j) = mat.get(i,j);
    body = reinterpret_cast<char*>(t_body);

    std::string ser_dims(dims,2*sizeof(uint16_t));
    std::string serialized(body,mat.getRows()*mat.getColumns()*sizeof(T));
    return ser_dims+serialized;
}

#endif