#define CATCH_CONFIG_MAIN

#include <iostream>
#include <string>

#include "../include/Matrix.h"
#include "../include/catch.hpp"

using namespace std;

SCENARIO("Something should speak when matrix are there","[something]"){
    GIVEN("A matrix of integers"){
        int tab[] = {1,2,3,4,5,6,7,8,9};
        Matrix<int> a(3,3,tab);
        WHEN("Something is called"){
            something();
            THEN("It doesn't matter"){
                REQUIRE(1==1);
            }
        }
    }
}