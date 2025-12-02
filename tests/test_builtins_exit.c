#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include "../incs/42sh.h"
#include "../incs/env.h"
#include "../incs/main.h"
#include "../incs/builtins.h"

void setup_exit_tests(void) {
	cr_redirect_stderr();
}

void teardown_exit_tests(void) {
	cr_redirect_stderr();
}

static t_shell *create_test_shell(void) {
	t_shell *shell = calloc(1, sizeof(t_shell));
	if (!shell)
		return NULL;
	
	shell->vars = init_var_table();
	if (!shell->vars) {
		free(shell);
		return NULL;
	}
	
	set_variable(shell->vars, "?", "0", 0, 1);
	
	return shell;
}

static void cleanup_test_shell(t_shell *shell) {
	if (!shell)
		return;
	
	if (shell->vars) {
		free_var_table(shell->vars);
		shell->vars = NULL;
	}
	free(shell);
}

TestSuite(exit_builtin, .init = setup_exit_tests, .fini = teardown_exit_tests);

// Note: exit() cannot be fully tested without special handling

Test(exit_builtin, exit_invalid_numeric_argument) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"exit", "abc", NULL};
	
	int result = builtin_exit(shell, argv);
	
	cr_assert_eq(result, 1, "exit with invalid argument should return error");
	
	cleanup_test_shell(shell);
}

Test(exit_builtin, exit_empty_argument) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"exit", "", NULL};
	
	int result = builtin_exit(shell, argv);
	
	cr_assert_eq(result, 1, "exit with empty argument should return error");
	
	cleanup_test_shell(shell);
}

Test(exit_builtin, exit_too_many_arguments) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"exit", "0", "1", NULL};
	
	int result = builtin_exit(shell, argv);
	
	cr_assert_eq(result, 1, "exit with too many arguments should return error");
	
	cleanup_test_shell(shell);
}

Test(exit_builtin, exit_mixed_alphanumeric) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"exit", "42abc", NULL};
	
	int result = builtin_exit(shell, argv);
	
	cr_assert_eq(result, 1, "exit with mixed alphanumeric should return error");
	
	cleanup_test_shell(shell);
}

Test(exit_builtin, exit_argument_with_spaces_mixed) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"exit", "4 2", NULL};
	
	int result = builtin_exit(shell, argv);

	cr_assert_eq(result, 1, "exit with space in middle should return error");
	
	cleanup_test_shell(shell);
}

Test(exit_builtin, exit_whitespace_only) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"exit", "   ", NULL};
	
	int result = builtin_exit(shell, argv);
	
	cr_assert_eq(result, 1, "exit with whitespace only should return error");
	
	cleanup_test_shell(shell);
}

Test(exit_builtin, exit_invalid_number_format) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"exit", "++42", NULL};
	
	int result = builtin_exit(shell, argv);
	
	cr_assert_eq(result, 1, "exit with invalid number format should return error");
	
	cleanup_test_shell(shell);
}

Test(exit_builtin, exit_error_messages) {
	t_shell *shell = create_test_shell();
	
	char *argv1[] = {"exit", "abc", NULL};
	builtin_exit(shell, argv1);
	
	FILE *stderr_file = cr_get_redirected_stderr();
	fseek(stderr_file, 0, SEEK_END);
	long length = ftell(stderr_file);
	fseek(stderr_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, stderr_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "numeric argument required") != NULL,
				  "Should show 'numeric argument required' error");
		cr_assert(strstr(output, "abc") != NULL,
				  "Error message should include the invalid argument");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(exit_builtin, exit_too_many_arguments_error) {
	t_shell *shell = create_test_shell();
	
	char *argv[] = {"exit", "0", "1", NULL};
	builtin_exit(shell, argv);
	
	FILE *stderr_file = cr_get_redirected_stderr();
	fseek(stderr_file, 0, SEEK_END);
	long length = ftell(stderr_file);
	fseek(stderr_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, stderr_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "too many arguments") != NULL,
				  "Should show 'too many arguments' error");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(exit_builtin, exit_null_arguments) {
	t_shell *shell = create_test_shell();
	
	int result = builtin_exit(shell, NULL);

	cr_assert_eq(result, 1, "exit should handle NULL arguments gracefully");
	
	cleanup_test_shell(shell);
}

Test(exit_builtin, exit_null_shell) {
	char *argv[] = {"exit", "0", NULL};
	
	int result = builtin_exit(NULL, argv);

	cr_assert_eq(result, 1, "exit should handle NULL shell gracefully");
}

Test(exit_builtin, exit_validation_logic) {
	t_shell *shell = create_test_shell();
	
	char *invalid_cases[] = {
		"abc",      // Non-numeric
		"",         // Empty
		"   ",      // Whitespace only  
		"4 2",      // Space in middle
		"42abc",    // Mixed
		"++42",     // Invalid format
		NULL
	};
	
	for (int i = 0; invalid_cases[i]; i++) {
		char *test_argv[] = {"exit", invalid_cases[i], NULL};
		int result = builtin_exit(shell, test_argv);
		
		cr_assert_eq(result, 1, "Invalid case '%s' should return error", 
					 invalid_cases[i]);
	}
	
	char *too_many[] = {"exit", "0", "1", NULL};
	int result = builtin_exit(shell, too_many);
	cr_assert_eq(result, 1, "Too many arguments should return error");
	
	cleanup_test_shell(shell);
}

