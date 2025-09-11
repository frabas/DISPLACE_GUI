#define BOOST_TEST_MAIN
#ifdef HAVE_BOOST_TEST_COMPILED
#include <boost/test/included/unit_test.hpp>
#else
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
