#include <criterion/criterion.h>

TestSuite(simple_tests);

Test(simple_tests, basic_assertion) {
    cr_assert_eq(1, 1, "1 should equal 1");
}

Test(simple_tests, string_test) {
    cr_assert_str_eq("hello", "hello", "Strings should be equal");
}
