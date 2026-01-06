#define BOOST_TEST_MODULE MinMaxTest
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <windows.h>
#include "../src/main.cpp"

// òåñò íà êîððåêòíîå îïðåäåëåíèå min è max
BOOST_AUTO_TEST_CASE(MinMaxValues) {
    array = { 1, 2, 3, 4, 5 };
    min_val = (std::numeric_limits<int>::max)();
    max_val = (std::numeric_limits<int>::min)();

    MinMaxThread(nullptr);

    BOOST_CHECK_EQUAL(min_val, 1);
    BOOST_CHECK_EQUAL(max_val, 5);
}