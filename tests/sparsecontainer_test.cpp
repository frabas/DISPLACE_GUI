//
// Created by happycactus on 21/01/22.
//

#include <boost/test/unit_test.hpp>

#include "SparseContainer.h"

BOOST_AUTO_TEST_SUITE(SparseContainerTest)

BOOST_AUTO_TEST_CASE(HashCalc)
{
    SparseContainer<double, uint8_t, uint16_t, uint8_t> sp;

    BOOST_CHECK_EQUAL(sp.hashKey(2, 3, 1), 0x01000302);
    BOOST_CHECK_EQUAL(sp.hashKey(32, 1000, 0), 0x0003e820);
    BOOST_CHECK_EQUAL(sp.hashKey(0, 0, 0), 0x00000000);
    BOOST_CHECK_EQUAL(sp.hashKey(255, 65535, 255), 0xffffffff);
    BOOST_CHECK_EQUAL(sp.hashKey(0, 65535, 0), 0x00ffff00);
    BOOST_CHECK_EQUAL(sp.hashKey(255, 0, 0), 0x000000ff);
    BOOST_CHECK_EQUAL(sp.hashKey(0, 0, 128), 0x80000000);
    BOOST_CHECK_EQUAL(sp.hashKey(0, 0, 127), 0x7f000000);
}

BOOST_AUTO_TEST_CASE(Access)
{
    SparseContainer<double, uint8_t, uint16_t, uint8_t> sp;

    BOOST_CHECK_NO_THROW(sp(3, 1000, 2) = 2.0);
    BOOST_CHECK_NO_THROW(sp(3, 0, 128) = 128.0);
    BOOST_CHECK_NO_THROW(sp(222, 0, 0) = -222.0);

    BOOST_CHECK_EQUAL(sp(3, 1000, 2), 2.0);
    BOOST_CHECK_EQUAL(sp(3, 0, 128), 128.0);
    BOOST_CHECK_EQUAL(sp(222, 0, 0), -222.0);

    BOOST_CHECK_EQUAL(sp.allocated_size(), 3);

    BOOST_CHECK(sp.hasValue(3, 1000, 2));
    BOOST_CHECK(sp.hasValue(3, 0, 128));
    BOOST_CHECK(!sp.hasValue(0, 0, 0));

    auto b = sp.value(1, 1, 1);
    BOOST_CHECK_EQUAL(b, 0.0);
    BOOST_CHECK_EQUAL(sp.allocated_size(), 3);

    BOOST_CHECK_EQUAL(sp(1, 1, 1), 0.0);
    BOOST_CHECK_EQUAL(sp.allocated_size(), 4);
}

BOOST_AUTO_TEST_CASE(Remove)
{
    SparseContainer<double, uint8_t, uint16_t, uint8_t> sp;

    for (size_t i = 0; i < 1000; ++i)
        sp(3, i, 2) = 2.0;
    BOOST_CHECK_EQUAL(sp.allocated_size(), 1000);
    BOOST_CHECK(sp.hasValue(3, 500, 2));

    for (size_t i = 1; i < 999; ++i)
        sp.zero(3, i, 2);
    BOOST_CHECK(!sp.hasValue(3, 500, 2));

    BOOST_CHECK_EQUAL(sp.allocated_size(), 2);
}

BOOST_AUTO_TEST_CASE(Clear)
{
    SparseContainer<double, uint8_t, uint16_t, uint8_t> sp;

    for (size_t i = 0; i < 1000; ++i)
        sp(3, i, 2) = 2.0;
    BOOST_CHECK_EQUAL(sp.allocated_size(), 1000);

    BOOST_CHECK_NO_THROW(sp.clear());
    BOOST_CHECK_EQUAL(sp.allocated_size(), 0);
}

BOOST_AUTO_TEST_CASE(Dimensions)
{
    SparseContainer<int, uint16_t, uint8_t, uint8_t> sp(4, 5, 6);

    BOOST_CHECK_EQUAL(sp.dimension(0), 4);
    BOOST_CHECK_EQUAL(sp.dimension(1), 5);
    BOOST_CHECK_EQUAL(sp.dimension(2), 6);

    sp(0, 3, 0) = 1;
    BOOST_CHECK_EQUAL(sp.dimension(0), 4);
    BOOST_CHECK_EQUAL(sp.dimension(1), 5);
    BOOST_CHECK_EQUAL(sp.dimension(2), 6);

    sp(9, 9, 9) = 1000;

    BOOST_CHECK_EQUAL(sp.dimension(0), 10);
    BOOST_CHECK_EQUAL(sp.dimension(1), 10);
    BOOST_CHECK_EQUAL(sp.dimension(2), 10);
}

BOOST_AUTO_TEST_SUITE_END()
