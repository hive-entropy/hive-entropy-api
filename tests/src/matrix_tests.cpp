#include <iostream>
#include <string>
#include "../include/catch.hpp"
#include "../../heapi/include/Matrix.h"

using namespace std;

SCENARIO("We should be able to create a Matrix and access its elements","[matrix]"){
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
                int* row = a.getRow(1);
                REQUIRE(row[0]==4);
                REQUIRE(row[1]==5);
                REQUIRE(row[2]==6);
            }

            THEN("Its columns should be accessible and be as defined as in the array"){
                int* col = a.getColumn(1);
                REQUIRE(col[0]==2);
                REQUIRE(col[1]==5);
                REQUIRE(col[2]==8);
            }

            THEN("We should be able to access a submatrix"){
                Matrix<int> sub = a.getSubmatrix(1,1,2,2);

                REQUIRE(sub.get(0,0)==5);
                REQUIRE(sub.get(0,1)==6);
                REQUIRE(sub.get(1,0)==8);
                REQUIRE(sub.get(1,1)==9);
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

            THEN("We should be able to access a submatrix with the element's new value"){
                Matrix<int> sub = a.getSubmatrix(1,1,2,2);

                REQUIRE(sub.get(0,0)==42);
                REQUIRE(sub.get(0,1)==6);
                REQUIRE(sub.get(1,0)==8);
                REQUIRE(sub.get(1,1)==9);
            }
        }

        WHEN("We put a whole column"){
            int tab[] = {1,2,3};
            a.putColumn(1,tab);

            THEN("We should be able to access the new elements individually"){
                REQUIRE(a.get(0,1)==1);
                REQUIRE(a.get(1,1)==2);
                REQUIRE(a.get(2,1)==3);
            }

            THEN("We should be able to access the new elements in each rows"){
                int* row1 = a.getRow(0);
                int* row2 = a.getRow(1);
                int* row3 = a.getRow(2);

                REQUIRE(row1[1]==1);
                REQUIRE(row2[1]==2);
                REQUIRE(row3[1]==3);
            }

            THEN("We should be able to access the new elements in the column"){
                int* col = a.getColumn(1);

                REQUIRE(col[0]==1);
                REQUIRE(col[1]==2);
                REQUIRE(col[2]==3);
            }

            THEN("We should be able to access the new elements in a submatrix"){
                Matrix<int> sub = a.getSubmatrix(0,1,2,2);

                REQUIRE(sub.get(0,0)==1);
                REQUIRE(sub.get(1,0)==2);
                REQUIRE(sub.get(2,0)==3);
            }
        }

        WHEN("We put a whole row"){
            int tab[] = {1,2,3};
            a.putRow(1,tab);

            THEN("We should be able to access the new elements individually"){
                REQUIRE(a.get(1,0)==1);
                REQUIRE(a.get(1,1)==2);
                REQUIRE(a.get(1,2)==3);
            }

            THEN("We should be able to access the new elements in the row"){
                int* row = a.getRow(1);

                REQUIRE(row[0]==1);
                REQUIRE(row[1]==2);
                REQUIRE(row[2]==3);
            }

            THEN("We should be able to access the new elements in each column"){
                int* col1 = a.getColumn(0);
                int* col2 = a.getColumn(1);
                int* col3 = a.getColumn(2);

                REQUIRE(col1[1]==1);
                REQUIRE(col2[1]==2);
                REQUIRE(col3[1]==3);
            }

            THEN("We should be able to access the new elements in a submatrix"){
                Matrix<int> sub = a.getSubmatrix(1,0,2,2);

                REQUIRE(sub.get(0,0)==1);
                REQUIRE(sub.get(0,1)==2);
                REQUIRE(sub.get(0,2)==3);
            }
        }

        WHEN("We put a submatrix inside the matrix"){
            int tab[] = {0,1,2,3};
            Matrix<int> m(2,2,tab);
            a.putSubmatrix(1,1,m);

            THEN("We should be able to access the elements directly"){
                REQUIRE(a.get(1,1)==0);
                REQUIRE(a.get(1,2)==1);
                REQUIRE(a.get(2,1)==2);
                REQUIRE(a.get(2,2)==3);
            }

            THEN("We should be able to restituate the submatrix"){
                Matrix<int> res = a.getSubmatrix(1,1,2,2);

                REQUIRE(res.get(0,0)==m.get(0,0));
                REQUIRE(res.get(0,1)==m.get(0,1));
                REQUIRE(res.get(1,0)==m.get(1,0));
                REQUIRE(res.get(1,1)==m.get(1,1));
            }

            THEN("We should be able to access the new elements inside a row"){
                int* row1 = a.getRow(1);
                int* row2 = a.getRow(2);

                REQUIRE(row1[1]==0);
                REQUIRE(row1[2]==1);
                REQUIRE(row2[1]==2);
                REQUIRE(row2[2]==3);
            }

            THEN("We should be able to access the new elements inside a column"){
                int* col1 = a.getColumn(1);
                int* col2 = a.getColumn(2);

                REQUIRE(col1[1]==0);
                REQUIRE(col1[2]==2);
                REQUIRE(col2[1]==1);
                REQUIRE(col2[2]==3);
            }
        }

        WHEN("We put elements,columns,rows,submatrices outside of its bounds or of the wrong size"){
            THEN("We throw an OutOfBoundException"){
                //...
            }
        }
    }

    GIVEN("Matrices of int defined using  archetypes"){
        Matrix<int> a(3,3,MatrixArchetype::ZEROS);
        Matrix<int> b(3,3,MatrixArchetype::ONES);
        Matrix<int> c(3,3,MatrixArchetype::ID);

        THEN("Their elements should correspond to the archetype pattern"){
            REQUIRE(a.get(0,0)==0);
            REQUIRE(a.get(0,1)==0);
            REQUIRE(a.get(0,2)==0);
            REQUIRE(a.get(1,0)==0);
            REQUIRE(a.get(1,1)==0);
            REQUIRE(a.get(1,2)==0);
            REQUIRE(a.get(2,0)==0);
            REQUIRE(a.get(2,1)==0);
            REQUIRE(a.get(2,2)==0);

            REQUIRE(b.get(0,0)==1);
            REQUIRE(b.get(0,1)==1);
            REQUIRE(b.get(0,2)==1);
            REQUIRE(b.get(1,0)==1);
            REQUIRE(b.get(1,1)==1);
            REQUIRE(b.get(1,2)==1);
            REQUIRE(b.get(2,0)==1);
            REQUIRE(b.get(2,1)==1);
            REQUIRE(b.get(2,2)==1);

            REQUIRE(c.get(0,0)==1);
            REQUIRE(c.get(0,1)==0);
            REQUIRE(c.get(0,2)==0);
            REQUIRE(c.get(1,0)==0);
            REQUIRE(c.get(1,1)==1);
            REQUIRE(c.get(1,2)==0);
            REQUIRE(c.get(2,0)==0);
            REQUIRE(c.get(2,1)==0);
            REQUIRE(c.get(2,2)==1);
        }
    }

    GIVEN("A matrix of int defined using another matrix"){
        int tab[] = {1,2,3,4,5,6,7,8,9};
        Matrix<int> a(3,3,tab);
        Matrix<int> b = a;

        THEN("The elements of both matrices should be equal"){
            REQUIRE(a.get(0,0)==b.get(0,0));
            REQUIRE(a.get(0,1)==b.get(0,1));
            REQUIRE(a.get(0,2)==b.get(0,2));
            REQUIRE(a.get(1,0)==b.get(1,0));
            REQUIRE(a.get(1,1)==b.get(1,1));
            REQUIRE(a.get(1,2)==b.get(1,2));
            REQUIRE(a.get(2,0)==b.get(2,0));
            REQUIRE(a.get(2,1)==b.get(2,1));
            REQUIRE(a.get(2,2)==b.get(2,2));
        }

        THEN("The dimensions of the matrix should be equal"){
            REQUIRE(a.getColumns()==b.getColumns());
            REQUIRE(a.getRows()==b.getRows());
        }
    }
}

SCENARIO("We should be able to do mathematical operations on matrices","[matrix-math]"){
    GIVEN("Two matrices of int"){
        int tabA[] = {1,2,3,4,5,6,7,8,9};
        int tabB[] = {23,56,21,78,99,456,856,3,-45};
        Matrix<int> a(3,3,tabA);
        Matrix<int> b(3,3,MatrixArchetype::ONES);

        WHEN("We add them together in a third matrix"){

            Matrix<int> c = a + b;

            THEN("Their elements should be summed"){
                REQUIRE(c.get(0,0)==a.get(0,0)+b.get(0,0));
                REQUIRE(c.get(0,1)==a.get(0,1)+b.get(0,1));
                REQUIRE(c.get(0,2)==a.get(0,2)+b.get(0,2));
                REQUIRE(c.get(1,0)==a.get(1,0)+b.get(1,0));
                REQUIRE(c.get(1,1)==a.get(1,1)+b.get(1,1));
                REQUIRE(c.get(1,2)==a.get(1,2)+b.get(1,2));
                REQUIRE(c.get(2,0)==a.get(2,0)+b.get(2,0));
                REQUIRE(c.get(2,1)==a.get(2,1)+b.get(2,1));
                REQUIRE(c.get(2,2)==a.get(2,2)+b.get(2,2));
            }
        }

        WHEN("We subtract them in a third matrix"){
            Matrix<int> c = a - b;

            THEN("Their elements should be subtracted"){
                REQUIRE(c.get(0,0)==a.get(0,0)-b.get(0,0));
                REQUIRE(c.get(0,1)==a.get(0,1)-b.get(0,1));
                REQUIRE(c.get(0,2)==a.get(0,2)-b.get(0,2));
                REQUIRE(c.get(1,0)==a.get(1,0)-b.get(1,0));
                REQUIRE(c.get(1,1)==a.get(1,1)-b.get(1,1));
                REQUIRE(c.get(1,2)==a.get(1,2)-b.get(1,2));
                REQUIRE(c.get(2,0)==a.get(2,0)-b.get(2,0));
                REQUIRE(c.get(2,1)==a.get(2,1)-b.get(2,1));
                REQUIRE(c.get(2,2)==a.get(2,2)-b.get(2,2));
            }
        }

        WHEN("We multiplay them in a third matrix"){

            Matrix<int> c = a*b;

            THEN("Their elements should equate"){
                REQUIRE(c.get(0,0)==a.get(0,0)*b.get(0,0)+a.get(0,1)*b.get(1,0)+a.get(0,2)*b.get(2,0));
                REQUIRE(c.get(0,1)==a.get(0,0)*b.get(0,1)+a.get(0,1)*b.get(1,1)+a.get(0,2)*b.get(2,1));
                REQUIRE(c.get(0,2)==a.get(0,0)*b.get(0,2)+a.get(0,1)*b.get(1,2)+a.get(0,2)*b.get(2,2));
                REQUIRE(c.get(1,0)==a.get(1,0)*b.get(0,0)+a.get(1,1)*b.get(1,0)+a.get(1,2)*b.get(2,0));
                REQUIRE(c.get(1,1)==a.get(1,0)*b.get(0,1)+a.get(1,1)*b.get(1,1)+a.get(1,2)*b.get(2,1));
                REQUIRE(c.get(1,2)==a.get(1,0)*b.get(0,2)+a.get(1,1)*b.get(1,2)+a.get(1,2)*b.get(2,2));
                REQUIRE(c.get(2,0)==a.get(2,0)*b.get(0,0)+a.get(2,1)*b.get(1,0)+a.get(2,2)*b.get(2,0));
                REQUIRE(c.get(2,1)==a.get(2,0)*b.get(0,1)+a.get(2,1)*b.get(1,1)+a.get(2,2)*b.get(2,1));
                REQUIRE(c.get(2,2)==a.get(2,0)*b.get(0,2)+a.get(2,1)*b.get(1,2)+a.get(2,2)*b.get(2,2));
            }
        }

        WHEN("We add them and put the result in the first operand"){
            Matrix<int> c(a);
            a += b;

            THEN("Their elements should be summed"){
                REQUIRE(a.get(0,0)==c.get(0,0)+b.get(0,0));
                REQUIRE(a.get(0,1)==c.get(0,1)+b.get(0,1));
                REQUIRE(a.get(0,2)==c.get(0,2)+b.get(0,2));
                REQUIRE(a.get(1,0)==c.get(1,0)+b.get(1,0));
                REQUIRE(a.get(1,1)==c.get(1,1)+b.get(1,1));
                REQUIRE(a.get(1,2)==c.get(1,2)+b.get(1,2));
                REQUIRE(a.get(2,0)==c.get(2,0)+b.get(2,0));
                REQUIRE(a.get(2,1)==c.get(2,1)+b.get(2,1));
                REQUIRE(a.get(2,2)==c.get(2,2)+b.get(2,2));
            }
        }

        WHEN("We subtract them and put the result in the first operand"){
            Matrix<int> c(a);
            a -= b;

            THEN("Their elements should be summed"){
                REQUIRE(a.get(0,0)==c.get(0,0)-b.get(0,0));
                REQUIRE(a.get(0,1)==c.get(0,1)-b.get(0,1));
                REQUIRE(a.get(0,2)==c.get(0,2)-b.get(0,2));
                REQUIRE(a.get(1,0)==c.get(1,0)-b.get(1,0));
                REQUIRE(a.get(1,1)==c.get(1,1)-b.get(1,1));
                REQUIRE(a.get(1,2)==c.get(1,2)-b.get(1,2));
                REQUIRE(a.get(2,0)==c.get(2,0)-b.get(2,0));
                REQUIRE(a.get(2,1)==c.get(2,1)-b.get(2,1));
                REQUIRE(a.get(2,2)==c.get(2,2)-b.get(2,2));
            }
        }

        WHEN("We multiply them and put the result in the first operand"){
            Matrix<int> c(a);

            c*=b;

             THEN("Their elements should equate"){
                REQUIRE(c.get(0,0)==a.get(0,0)*b.get(0,0)+a.get(0,1)*b.get(1,0)+a.get(0,2)*b.get(2,0));
                REQUIRE(c.get(0,1)==a.get(0,0)*b.get(0,1)+a.get(0,1)*b.get(1,1)+a.get(0,2)*b.get(2,1));
                REQUIRE(c.get(0,2)==a.get(0,0)*b.get(0,2)+a.get(0,1)*b.get(1,2)+a.get(0,2)*b.get(2,2));
                REQUIRE(c.get(1,0)==a.get(1,0)*b.get(0,0)+a.get(1,1)*b.get(1,0)+a.get(1,2)*b.get(2,0));
                REQUIRE(c.get(1,1)==a.get(1,0)*b.get(0,1)+a.get(1,1)*b.get(1,1)+a.get(1,2)*b.get(2,1));
                REQUIRE(c.get(1,2)==a.get(1,0)*b.get(0,2)+a.get(1,1)*b.get(1,2)+a.get(1,2)*b.get(2,2));
                REQUIRE(c.get(2,0)==a.get(2,0)*b.get(0,0)+a.get(2,1)*b.get(1,0)+a.get(2,2)*b.get(2,0));
                REQUIRE(c.get(2,1)==a.get(2,0)*b.get(0,1)+a.get(2,1)*b.get(1,1)+a.get(2,2)*b.get(2,1));
                REQUIRE(c.get(2,2)==a.get(2,0)*b.get(0,2)+a.get(2,1)*b.get(1,2)+a.get(2,2)*b.get(2,2));
            }
        }
    }

    GIVEN("Two matrices of double"){
        double tabA[] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0};
        double tabB[] = {23.0,56.0,21.0,78.0,99.0,456.0,856.0,3.0,-45.0};
        Matrix<double> a(3,3,tabA);
        Matrix<double> b(3,3,MatrixArchetype::ONES);

        WHEN("We add them together in a third matrix"){

            Matrix<double> c = a + b;

            THEN("Their elements should be summed"){
                REQUIRE(c.get(0,0)==a.get(0,0)+b.get(0,0));
                REQUIRE(c.get(0,1)==a.get(0,1)+b.get(0,1));
                REQUIRE(c.get(0,2)==a.get(0,2)+b.get(0,2));
                REQUIRE(c.get(1,0)==a.get(1,0)+b.get(1,0));
                REQUIRE(c.get(1,1)==a.get(1,1)+b.get(1,1));
                REQUIRE(c.get(1,2)==a.get(1,2)+b.get(1,2));
                REQUIRE(c.get(2,0)==a.get(2,0)+b.get(2,0));
                REQUIRE(c.get(2,1)==a.get(2,1)+b.get(2,1));
                REQUIRE(c.get(2,2)==a.get(2,2)+b.get(2,2));
            }
        }

        WHEN("We subtract them in a third matrix"){
            Matrix<double> c = a - b;

            THEN("Their elements should be subtracted"){
                REQUIRE(c.get(0,0)==a.get(0,0)-b.get(0,0));
                REQUIRE(c.get(0,1)==a.get(0,1)-b.get(0,1));
                REQUIRE(c.get(0,2)==a.get(0,2)-b.get(0,2));
                REQUIRE(c.get(1,0)==a.get(1,0)-b.get(1,0));
                REQUIRE(c.get(1,1)==a.get(1,1)-b.get(1,1));
                REQUIRE(c.get(1,2)==a.get(1,2)-b.get(1,2));
                REQUIRE(c.get(2,0)==a.get(2,0)-b.get(2,0));
                REQUIRE(c.get(2,1)==a.get(2,1)-b.get(2,1));
                REQUIRE(c.get(2,2)==a.get(2,2)-b.get(2,2));
            }
        }

        WHEN("We multiplay them in a third matrix"){

            Matrix<double> c = a*b;

            THEN("Their elements should equate"){
                REQUIRE(c.get(0,0)==a.get(0,0)*b.get(0,0)+a.get(0,1)*b.get(1,0)+a.get(0,2)*b.get(2,0));
                REQUIRE(c.get(0,1)==a.get(0,0)*b.get(0,1)+a.get(0,1)*b.get(1,1)+a.get(0,2)*b.get(2,1));
                REQUIRE(c.get(0,2)==a.get(0,0)*b.get(0,2)+a.get(0,1)*b.get(1,2)+a.get(0,2)*b.get(2,2));
                REQUIRE(c.get(1,0)==a.get(1,0)*b.get(0,0)+a.get(1,1)*b.get(1,0)+a.get(1,2)*b.get(2,0));
                REQUIRE(c.get(1,1)==a.get(1,0)*b.get(0,1)+a.get(1,1)*b.get(1,1)+a.get(1,2)*b.get(2,1));
                REQUIRE(c.get(1,2)==a.get(1,0)*b.get(0,2)+a.get(1,1)*b.get(1,2)+a.get(1,2)*b.get(2,2));
                REQUIRE(c.get(2,0)==a.get(2,0)*b.get(0,0)+a.get(2,1)*b.get(1,0)+a.get(2,2)*b.get(2,0));
                REQUIRE(c.get(2,1)==a.get(2,0)*b.get(0,1)+a.get(2,1)*b.get(1,1)+a.get(2,2)*b.get(2,1));
                REQUIRE(c.get(2,2)==a.get(2,0)*b.get(0,2)+a.get(2,1)*b.get(1,2)+a.get(2,2)*b.get(2,2));
            }
        }

        WHEN("We add them and put the result in the first operand"){
            Matrix<double> c(a);
            a += b;

            THEN("Their elements should be summed"){
                REQUIRE(a.get(0,0)==c.get(0,0)+b.get(0,0));
                REQUIRE(a.get(0,1)==c.get(0,1)+b.get(0,1));
                REQUIRE(a.get(0,2)==c.get(0,2)+b.get(0,2));
                REQUIRE(a.get(1,0)==c.get(1,0)+b.get(1,0));
                REQUIRE(a.get(1,1)==c.get(1,1)+b.get(1,1));
                REQUIRE(a.get(1,2)==c.get(1,2)+b.get(1,2));
                REQUIRE(a.get(2,0)==c.get(2,0)+b.get(2,0));
                REQUIRE(a.get(2,1)==c.get(2,1)+b.get(2,1));
                REQUIRE(a.get(2,2)==c.get(2,2)+b.get(2,2));
            }
        }

        WHEN("We subtract them and put the result in the first operand"){
            Matrix<double> c(a);
            a -= b;

            THEN("Their elements should be summed"){
                REQUIRE(a.get(0,0)==c.get(0,0)-b.get(0,0));
                REQUIRE(a.get(0,1)==c.get(0,1)-b.get(0,1));
                REQUIRE(a.get(0,2)==c.get(0,2)-b.get(0,2));
                REQUIRE(a.get(1,0)==c.get(1,0)-b.get(1,0));
                REQUIRE(a.get(1,1)==c.get(1,1)-b.get(1,1));
                REQUIRE(a.get(1,2)==c.get(1,2)-b.get(1,2));
                REQUIRE(a.get(2,0)==c.get(2,0)-b.get(2,0));
                REQUIRE(a.get(2,1)==c.get(2,1)-b.get(2,1));
                REQUIRE(a.get(2,2)==c.get(2,2)-b.get(2,2));
            }
        }

        WHEN("We multiply them and put the result in the first operand"){
            Matrix<double> c(a);

            c*=b;

             THEN("Their elements should equate"){
                REQUIRE(c.get(0,0)==a.get(0,0)*b.get(0,0)+a.get(0,1)*b.get(1,0)+a.get(0,2)*b.get(2,0));
                REQUIRE(c.get(0,1)==a.get(0,0)*b.get(0,1)+a.get(0,1)*b.get(1,1)+a.get(0,2)*b.get(2,1));
                REQUIRE(c.get(0,2)==a.get(0,0)*b.get(0,2)+a.get(0,1)*b.get(1,2)+a.get(0,2)*b.get(2,2));
                REQUIRE(c.get(1,0)==a.get(1,0)*b.get(0,0)+a.get(1,1)*b.get(1,0)+a.get(1,2)*b.get(2,0));
                REQUIRE(c.get(1,1)==a.get(1,0)*b.get(0,1)+a.get(1,1)*b.get(1,1)+a.get(1,2)*b.get(2,1));
                REQUIRE(c.get(1,2)==a.get(1,0)*b.get(0,2)+a.get(1,1)*b.get(1,2)+a.get(1,2)*b.get(2,2));
                REQUIRE(c.get(2,0)==a.get(2,0)*b.get(0,0)+a.get(2,1)*b.get(1,0)+a.get(2,2)*b.get(2,0));
                REQUIRE(c.get(2,1)==a.get(2,0)*b.get(0,1)+a.get(2,1)*b.get(1,1)+a.get(2,2)*b.get(2,1));
                REQUIRE(c.get(2,2)==a.get(2,0)*b.get(0,2)+a.get(2,1)*b.get(1,2)+a.get(2,2)*b.get(2,2));
            }
        }
    }
}

SCENARIO("We should be able to compare matrices","[matrix-logic]"){
    GIVEN("A matrix of int"){
        int tab[] = {1,2,3,4,5,6,7,8,9,10,11,12};
        Matrix<int> a(3,4,tab);

        WHEN("We copy it in a second matrix"){
            Matrix<int> b(a);

            THEN("The two matrices should be compared as equal"){
                REQUIRE(a==b);
                REQUIRE_FALSE(a!=b);
            }
        }

        WHEN("We use the same array to build another matrix of the same dimensions"){
            Matrix<int> b(3,4,tab);

            THEN("The two matrices should be compared as equal"){
                REQUIRE(a==b);
                REQUIRE_FALSE(a!=b);
            }
        }

        WHEN("We use the same array to build another matrix of different dimensions"){
            Matrix<int> b(2,6,tab);

            THEN("The two matrices should be compared as not equal"){
                REQUIRE_FALSE(a==b);
                REQUIRE(a!=b);
            }
        }
    }
}