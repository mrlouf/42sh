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

Test(echo_builtin, no_escape_by_default) {
    char *argv[] = {"echo", "hello\\nworld", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0, "echo should return 0");
    cr_assert_stdout_eq_str("hello\\nworld\n", "echo should NOT interpret escapes by default (POSIX behavior)");
}

Test(echo_builtin, combined_flags_ne) {
    char *argv[] = {"echo", "-ne", "hello\\tworld", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0, "echo -ne should return 0");
    cr_assert_stdout_eq_str("hello\tworld", "echo -ne should process escapes AND suppress newline");
}

Test(echo_builtin, combined_flags_en) {
    char *argv[] = {"echo", "-en", "hello\\tworld", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0, "echo -en should return 0");
    cr_assert_stdout_eq_str("hello\tworld", "echo -en should process escapes AND suppress newline (order shouldn't matter)");
}

Test(echo_builtin, c_escape_stops_processing) {
    char *argv[] = {"echo", "-e", "hello\\cworld", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0, "echo -e should return 0");
    cr_assert_stdout_eq_str("hello", "echo -e with \\c should stop processing and suppress newline");
}
