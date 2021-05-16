#include "catch.hpp"
#include "Serializer.h"

#include <string>

SCENARIO("We should be able to serialize and deserialize objects","[serializer]"){
    GIVEN("A set of matrices"){
        int a[] = {1,2,3,4,5,6,7,8,9};
        float b[] = {1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f,8.0f,9.0f};
        double c[] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0};

        Matrix<int> A(3,3,a);
        Matrix<float> B(3,3,b);
        Matrix<double> C(3,3,c);
        
        WHEN("We serialize and deserialize them"){
            std::string a_ser = Serializer::serialize(A);
            std::string b_ser = Serializer::serialize(B);
            std::string c_ser = Serializer::serialize(C);

            Matrix<int> A1 = Serializer::unserializeMatrix<int>(a_ser);
            Matrix<float> B1 = Serializer::unserializeMatrix<float>(b_ser);
            Matrix<double> C1 = Serializer::unserializeMatrix<double>(c_ser);

            THEN("The matrices should be equal to their original values"){
                REQUIRE(A==A1);
                REQUIRE(B==B1);
                REQUIRE(C==C1);
            }
        }
    }

    GIVEN("A list of int matrices"){
        int a[] = {1,2,3,4,5,6,7,8,9};
        int b[] = {1,2,3,4};
        int c[] = {10,11,12,13,14,15,16,17,18,19};

        Matrix<int> A(3,3,a);
        Matrix<int> B(2,2,b);
        Matrix<int> C(2,5,c);

        std::vector<Matrix<int>> vec = std::vector<Matrix<int>>();
        vec.push_back(A);
        vec.push_back(B);
        vec.push_back(C);

        WHEN("We serialize and deserialize them"){
            std::string serial = Serializer::serialize<int>(vec);
            std::vector<Matrix<int>> vec2 = Serializer::unserializeMatrices<int>(serial);

            THEN("Every matrix in the vector should be equal to their original"){
                REQUIRE(vec.at(0)==vec2.at(0));
                REQUIRE(vec.at(1)==vec2.at(1));
                REQUIRE(vec.at(2)==vec2.at(2));
            }
        }
    }

    GIVEN("Row and column pairs"){
        int ar[] = {1,2,3};
        int ac[] = {4,5,6,7,8};
        float br[] = {1.0f,4.0f,3.0f};
        float bc[] = {15.0f,34.0f};
        double cr[] = {0.5,4.8,17.0,67.2,123.4};
        double cc[] = {10.5,4.3,17.0,4.32,12.34};

        Row<int> aRow(3,12,ar);
        Column<int> aCol(5,12,ac);

        Row<float> bRow(3,2,br);
        Column<float> bCol(2,1,bc);

        Row<double> cRow(5,12,cr);
        Column<double> cCol(5,12,cc);

        WHEN("We serialize and deserialize them"){
            std::string aser = Serializer::serialize(aRow,aCol);
            std::string bser = Serializer::serialize(bRow,bCol);
            std::string cser = Serializer::serialize(cRow,cCol);

            std::pair<Row<int>,Column<int>> a1 = Serializer::unserializeRowColumn<int>(aser);
            std::pair<Row<float>,Column<float>> b1 = Serializer::unserializeRowColumn<float>(bser);
            std::pair<Row<double>,Column<double>> c1 = Serializer::unserializeRowColumn<double>(cser);

            THEN("Every row and column should be equal to the original"){
                REQUIRE(aRow==a1.first);
                REQUIRE(bRow==b1.first);
                REQUIRE(cRow==c1.first);

                REQUIRE(aCol==a1.second);
                REQUIRE(bCol==b1.second);
                REQUIRE(cCol==c1.second);
            }
        }
    }
}