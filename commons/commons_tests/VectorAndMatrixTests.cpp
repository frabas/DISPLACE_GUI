//
// Created by Federico Fuga <fuga@studiofuga.com> on 12/12/20.
//

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "containers/Vector.h"
#include "containers/Matrix.h"

BOOST_AUTO_TEST_CASE (test_vectors_of_int)
{
    Vector<int> vectorOfInts;

    int v, ev = 3;
    BOOST_CHECK_NO_THROW(vectorOfInts.at(2) = ev);
    BOOST_CHECK_NO_THROW(v = vectorOfInts.at(2));
    BOOST_CHECK_EQUAL(v, ev);
    BOOST_CHECK_NO_THROW(v = vectorOfInts[2]);
    BOOST_CHECK_EQUAL(v, ev);
    BOOST_CHECK_NO_THROW(v = vectorOfInts[0]);
    BOOST_CHECK_EQUAL(v, 0);
    BOOST_CHECK_NO_THROW(v = vectorOfInts.at(0));
    BOOST_CHECK_EQUAL(v, 0);
    BOOST_CHECK_THROW(v = vectorOfInts[3], std::out_of_range);
}

BOOST_AUTO_TEST_CASE (test_matrix_of_int)
{
    Matrix<int> matrixOfInts;

    int v, ev = 3;
    BOOST_CHECK_NO_THROW(matrixOfInts.at(2, 2) = ev);
    BOOST_CHECK_NO_THROW(v = matrixOfInts.at(2, 2));
    BOOST_CHECK_EQUAL(v, ev);
    BOOST_CHECK_NO_THROW(v = matrixOfInts[2][2]);
    BOOST_CHECK_EQUAL(v, ev);
    BOOST_CHECK_THROW(v = matrixOfInts[0][2], std::out_of_range);
    BOOST_CHECK_NO_THROW(v = matrixOfInts[2][0]);
    BOOST_CHECK_EQUAL(v, 0);
    BOOST_CHECK_NO_THROW(v = matrixOfInts.at(0, 2));
    BOOST_CHECK_EQUAL(v, 0);
    BOOST_CHECK_THROW(v = matrixOfInts[3][0], std::out_of_range);
}


