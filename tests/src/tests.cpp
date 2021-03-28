#define CATCH_CONFIG_MAIN

#include <iostream>
#include <string>
#include "../include/catch.hpp"
#include "../../heapi/include/Matrix.h"

using namespace std;

SCENARIO("We should be able to create a Matrix and access its elements","[matrix-creation]"){
    GIVEN("A matrix of int defined using an array"){
        int tab[] = {1,2,3,4,5,6,7,8,9};
        Matrix<int> a(3,3,tab);
        WHEN("We do nothing"){
            THEN("Its elements should be accessible and be as defined as in the array"){
                REQUIRE(a.get(0,0)==1);
                REQUIRE(a.get(0,1)==2);
                REQUIRE(a.get(0,2)==3);
                REQUIRE(a.get(1,0)==4);
                REQUIRE(a.get(1,1)==5);
                REQUIRE(a.get(1,2)==6);
                REQUIRE(a.get(2,0)==7);
                REQUIRE(a.get(2,1)==8);
                REQUIRE(a.get(2,2)==9);
            }

            THEN("Its rows should be accessible and be as defined as in the array"){
                //...
            }

            THEN("Its columns should be accessible and be as defined as in the array"){
                //...
            }
        }

        WHEN("We change an element's value"){
            a.put(1,1,42);
            THEN("We should be able to access this element with its new value"){
                REQUIRE(a.get(1,1)==42);
            }
            THEN("We should be able to access the element's line and column with its new value"){
                int* col = a.getColumn(1);
                int* row = a.getRow(1);

                REQUIRE(col[0]==2);
                REQUIRE(col[1]==42);
                REQUIRE(col[2]==8);
                REQUIRE(row[0]==4);
                REQUIRE(row[1]==42);
                REQUIRE(row[2]==6);
            }
        }

        WHEN("We put a whole column"){
            //...
        }

        WHEN("We put a whole row"){
            //...
        }

        WHEN("We put a submatrix inside the matrix"){
            //...
        }

        WHEN("We put elements,columns,rows,submatrices outside of its bounds or of the wrong size"){
            THEN("We throw an OutOfBoundException"){
                //...
            }
        }
    }

    GIVEN("A matrix of int defined using an archetype"){
        //...
    }

    GIVEN("A matrix of int defined using another matrix"){
        //...
    }
}