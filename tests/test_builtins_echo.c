#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "../incs/42sh.h"
#include "../incs/builtins.h"

void setup_echo_tests(void) {
    cr_redirect_stdout();
}

void teardown_echo_tests(void) {
    cr_redirect_stdout();
}

TestSuite(echo_builtin, .init = setup_echo_tests, .fini = teardown_echo_tests);

Test(echo_builtin, simple_text) {
    char *argv[] = {"echo", "hello", "world", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0, "echo should return 0 on success");
    cr_assert_stdout_eq_str("hello world\n", "echo should output text with newline");
}

Test(echo_builtin, suppress_newline) {
    char *argv[] = {"echo", "-n", "hello", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0, "echo -n should return 0");
    cr_assert_stdout_eq_str("hello", "echo -n should suppress newline");
}

Test(echo_builtin, enable_escapes) {
    char *argv[] = {"echo", "-e", "hello\\nworld", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0, "echo -e should return 0");
    cr_assert_stdout_eq_str("hello\nworld\n", "echo -e should interpret escape sequences");
}

Test(echo_builtin, tab_escape) {
    char *argv[] = {"echo", "-e", "hello\\tworld", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0, "echo -e should return 0");
    cr_assert_stdout_eq_str("hello\tworld\n", "echo -e should interpret \\t as tab");
}

Test(echo_builtin, backslash_escape) {
    char *argv[] = {"echo", "-e", "hello\\\\world", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0, "echo -e should return 0");
    cr_assert_stdout_eq_str("hello\\world\n", "echo -e should interpret \\\\ as single backslash");
}
