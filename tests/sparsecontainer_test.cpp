//
// Created by happycactus on 21/01/22.
//

#include <boost/test/unit_test.hpp>

#include "SparseContainer.h"

BOOST_AUTO_TEST_SUITE(SparseContainerTest)

BOOST_AUTO_TEST_CASE(HashCalc)
{
    SparseContainer<double, uint8_t, uint16_t, uint8_t> sp;

    BOOST_CHECK_EQUAL(sp.hashKey(uint8_t{2}, uint16_t{3}, uint8_t{1}), 0x01000302);
    BOOST_CHECK_EQUAL(sp.hashKey(uint8_t{32}, uint16_t{1000}, uint8_t{0}), 0x0003e820);
    BOOST_CHECK_EQUAL(sp.hashKey(uint8_t{0}, uint16_t{0}, uint8_t{0}), 0x00000000);
    BOOST_CHECK_EQUAL(sp.hashKey(uint8_t{255}, uint16_t{65535}, uint8_t{255}), 0xffffffff);
    BOOST_CHECK_EQUAL(sp.hashKey(uint8_t{0}, uint16_t{65535}, uint8_t{0}), 0x00ffff00);
    BOOST_CHECK_EQUAL(sp.hashKey(uint8_t{255}, uint16_t{0}, uint8_t{0}), 0x000000ff);
    BOOST_CHECK_EQUAL(sp.hashKey(uint8_t{0}, uint16_t{0}, uint8_t{128}), 0x80000000);
    BOOST_CHECK_EQUAL(sp.hashKey(uint8_t{0}, uint16_t{0}, uint8_t{127}), 0x7f000000);
}

BOOST_AUTO_TEST_SUITE_END()
