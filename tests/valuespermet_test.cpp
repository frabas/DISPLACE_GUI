//
// Created by happycactus on 07/02/22.
//

#include <boost/test/unit_test.hpp>

#include <stdexcept>

#include "ValuesPerMetPerPopContainer.h"

BOOST_AUTO_TEST_SUITE(ValuesPerMetPerPopContainerTest)

BOOST_AUTO_TEST_CASE(Initialization)
{
    ValuesPerMetPerPopContainer conInitialized(3, 4, 5);

    BOOST_CHECK_THROW(conInitialized.init(1, 2, 3), std::runtime_error);
    BOOST_CHECK_EQUAL(conInitialized.dimension(0), 3);
    BOOST_CHECK_EQUAL(conInitialized.dimension(1), 4);
    BOOST_CHECK_EQUAL(conInitialized.dimension(2), 5);

    ValuesPerMetPerPopContainer conNotInitialized;
    BOOST_CHECK_EQUAL(conNotInitialized.dimension(0), 0);
    BOOST_CHECK_EQUAL(conNotInitialized.dimension(1), 0);
    BOOST_CHECK_EQUAL(conNotInitialized.dimension(2), 0);

    BOOST_CHECK_NO_THROW(conNotInitialized.init(3, 4, 5));
    BOOST_CHECK_EQUAL(conNotInitialized.dimension(0), 3);
    BOOST_CHECK_EQUAL(conNotInitialized.dimension(1), 4);
    BOOST_CHECK_EQUAL(conNotInitialized.dimension(2), 5);
}


BOOST_AUTO_TEST_CASE(Access)
{
    ValuesPerMetPerPopContainer c(3, 4, 5);

    BOOST_CHECK_EQUAL(c.hasValue(0, 1, 0), false);
    BOOST_CHECK_EQUAL(c.value(0, 1, 0), 0);

    BOOST_CHECK_NO_THROW(c(0, 1, 0) = 1);
    BOOST_CHECK_EQUAL(c(0, 1, 0), 1);
    BOOST_CHECK_EQUAL(c(0, 0, 0), 0);
    BOOST_CHECK_EQUAL(c.value(2, 1, 3), 0);

    BOOST_CHECK_EQUAL(c.hasValue(0, 1, 0), true);
    BOOST_CHECK_EQUAL(c.hasValue(2, 1, 3), false);
}

BOOST_AUTO_TEST_SUITE_END()
