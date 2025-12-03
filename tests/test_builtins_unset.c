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

void setup_unset_tests(void) {
	cr_redirect_stdout();
}

void teardown_unset_tests(void) {
	cr_redirect_stdout();
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
	
	// Initialize with basic environment variables
	char *current_pwd = getcwd(NULL, 0);
	if (current_pwd) {
		set_variable(shell->vars, "PWD", current_pwd, 1, 1);
		free(current_pwd);
	}
	set_variable(shell->vars, "HOME", getenv("HOME"), 1, 1);
	
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

TestSuite(unset_builtin, .init = setup_unset_tests, .fini = teardown_unset_tests);

Test(unset_builtin, unset_no_args_returns_0) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"unset", NULL};
	
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 0, "unset with no args should return 0");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_existing_variable) {
	t_shell *shell = create_test_shell();
	
	// Create variable
	set_variable(shell->vars, "TESTVAR", "testvalue", 1, 1);
	
	// Verify it exists
	char *value = get_variable(shell->vars, "TESTVAR");
	cr_assert_not_null(value, "TESTVAR should exist before unset");
	cr_assert_str_eq(value, "testvalue", "TESTVAR should have correct value");
	
	// Unset it
	char *argv[] = {"unset", "TESTVAR", NULL};
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 0, "unset should succeed");
	
	// Verify it no longer exists
	value = get_variable(shell->vars, "TESTVAR");
	cr_assert_null(value, "TESTVAR should not exist after unset");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_nonexistent_variable) {
	t_shell *shell = create_test_shell();
	
	char *argv[] = {"unset", "NONEXISTENT", NULL};
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 0, "unset of nonexistent variable should succeed");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_readonly_variable_fails) {
	t_shell *shell = create_test_shell();
	
	cr_redirect_stderr();
	
	// Create readonly variable
	set_variable(shell->vars, "READONLY_VAR", "value", 1, 1);
	mark_variable_as_readonly(shell->vars, "READONLY_VAR");
	
	// Try to unset it
	char *argv[] = {"unset", "READONLY_VAR", NULL};
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 1, "unset of readonly variable should fail");
	
	// Verify it still exists
	char *value = get_variable(shell->vars, "READONLY_VAR");
	cr_assert_not_null(value, "READONLY_VAR should still exist");
	cr_assert_str_eq(value, "value", "READONLY_VAR should preserve value");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_invalid_variable_names) {
	t_shell *shell = create_test_shell();
	
	cr_redirect_stderr();
	
	// Test invalid starting character (digit)
	char *argv1[] = {"unset", "123VAR", NULL};
	int result1 = builtin_unset(shell, argv1);
	cr_assert_eq(result1, 1, "unset should fail for variable starting with digit");
	
	// Test invalid character (hyphen)
	char *argv2[] = {"unset", "VAR-NAME", NULL};
	int result2 = builtin_unset(shell, argv2);
	cr_assert_eq(result2, 1, "unset should fail for variable with hyphen");
	
	// Test empty variable name
	char *argv3[] = {"unset", "", NULL};
	int result3 = builtin_unset(shell, argv3);
	cr_assert_eq(result3, 1, "unset should fail for empty variable name");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_multiple_variables) {
	t_shell *shell = create_test_shell();
	
	// Create multiple variables
	set_variable(shell->vars, "VAR1", "value1", 1, 1);
	set_variable(shell->vars, "VAR2", "value2", 0, 1);
	set_variable(shell->vars, "VAR3", "value3", 1, 1);
	
	// Verify they exist
	cr_assert_not_null(get_variable(shell->vars, "VAR1"), "VAR1 should exist");
	cr_assert_not_null(get_variable(shell->vars, "VAR2"), "VAR2 should exist");
	cr_assert_not_null(get_variable(shell->vars, "VAR3"), "VAR3 should exist");
	
	// Unset all of them
	char *argv[] = {"unset", "VAR1", "VAR2", "VAR3", NULL};
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 0, "unset should succeed");
	
	// Verify they no longer exist
	cr_assert_null(get_variable(shell->vars, "VAR1"), "VAR1 should not exist");
	cr_assert_null(get_variable(shell->vars, "VAR2"), "VAR2 should not exist");
	cr_assert_null(get_variable(shell->vars, "VAR3"), "VAR3 should not exist");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_mixed_valid_invalid) {
	t_shell *shell = create_test_shell();
	
	cr_redirect_stderr();
	
	// Create some valid variables
	set_variable(shell->vars, "VALID1", "value1", 1, 1);
	set_variable(shell->vars, "VALID2", "value2", 1, 1);
	
	// Mix valid and invalid variable names
	char *argv[] = {"unset", "VALID1", "123INVALID", "VALID2", NULL};
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 1, "unset should fail due to invalid name");
	
	// Valid variables should be unset regardless of invalid names (continues processing)
	cr_assert_null(get_variable(shell->vars, "VALID1"), "VALID1 should be unset");
	cr_assert_null(get_variable(shell->vars, "VALID2"), "VALID2 should be unset");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_exported_variable) {
	t_shell *shell = create_test_shell();
	
	// Create exported variable
	set_variable(shell->vars, "EXPORTED_VAR", "exported_value", 1, 1);
	
	// Verify it's exported
	cr_assert_eq(is_variable_exported(shell->vars, "EXPORTED_VAR"), 1, "Variable should be exported");
	
	// Unset it
	char *argv[] = {"unset", "EXPORTED_VAR", NULL};
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 0, "unset should succeed");
	
	// Verify it no longer exists
	char *value = get_variable(shell->vars, "EXPORTED_VAR");
	cr_assert_null(value, "EXPORTED_VAR should not exist after unset");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_variable_with_special_characters_in_value) {
	t_shell *shell = create_test_shell();
	
	// Create variable with special characters in value
	set_variable(shell->vars, "SPECIAL_VAR", "value with spaces and $pecial chars!", 1, 1);
	
	// Verify it exists
	char *value = get_variable(shell->vars, "SPECIAL_VAR");
	cr_assert_not_null(value, "SPECIAL_VAR should exist");
	cr_assert_str_eq(value, "value with spaces and $pecial chars!", "SPECIAL_VAR should have correct value");
	
	// Unset it
	char *argv[] = {"unset", "SPECIAL_VAR", NULL};
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 0, "unset should succeed");
	
	// Verify it no longer exists
	value = get_variable(shell->vars, "SPECIAL_VAR");
	cr_assert_null(value, "SPECIAL_VAR should not exist after unset");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_readonly_mixed_with_valid) {
	t_shell *shell = create_test_shell();
	
	cr_redirect_stderr();
	
	// Create variables - one readonly, one normal
	set_variable(shell->vars, "NORMAL_VAR", "normal", 1, 1);
	set_variable(shell->vars, "READONLY_VAR", "readonly", 1, 1);
	mark_variable_as_readonly(shell->vars, "READONLY_VAR");
	
	// Try to unset both
	char *argv[] = {"unset", "NORMAL_VAR", "READONLY_VAR", NULL};
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 1, "unset should fail due to readonly variable");
	
	// Normal variable should be unset
	cr_assert_null(get_variable(shell->vars, "NORMAL_VAR"), "NORMAL_VAR should be unset");
	
	// Readonly variable should still exist
	char *value = get_variable(shell->vars, "READONLY_VAR");
	cr_assert_not_null(value, "READONLY_VAR should still exist");
	cr_assert_str_eq(value, "readonly", "READONLY_VAR should preserve value");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_valid_variable_names) {
	t_shell *shell = create_test_shell();
	
	// Create variables with valid names (edge cases)
	set_variable(shell->vars, "_", "underscore", 1, 1);
	set_variable(shell->vars, "_VAR", "underscore_start", 1, 1);
	set_variable(shell->vars, "VAR_123", "with_numbers", 1, 1);
	set_variable(shell->vars, "a", "single_char", 1, 1);
	
	// Unset all valid names
	char *argv[] = {"unset", "_", "_VAR", "VAR_123", "a", NULL};
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 0, "unset should succeed for all valid names");
	
	// Verify all are unset
	cr_assert_null(get_variable(shell->vars, "_"), "_ should be unset");
	cr_assert_null(get_variable(shell->vars, "_VAR"), "_VAR should be unset");
	cr_assert_null(get_variable(shell->vars, "VAR_123"), "VAR_123 should be unset");
	cr_assert_null(get_variable(shell->vars, "a"), "a should be unset");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_null_arguments) {
	t_shell *shell = create_test_shell();
	
	int result = builtin_unset(shell, NULL);
	cr_assert_eq(result, 1, "unset should fail with NULL arguments");
	
	cleanup_test_shell(shell);
}

Test(unset_builtin, unset_null_shell) {
	char *argv[] = {"unset", "VAR", NULL};
	
	int result = builtin_unset(NULL, argv);
	cr_assert_eq(result, 1, "unset should fail with NULL shell");
}

Test(unset_builtin, unset_preserves_other_variables) {
	t_shell *shell = create_test_shell();
	
	// Create multiple variables
	set_variable(shell->vars, "KEEP1", "keep_this", 1, 1);
	set_variable(shell->vars, "REMOVE", "remove_this", 1, 1);
	set_variable(shell->vars, "KEEP2", "keep_this_too", 0, 1);
	
	// Unset only one
	char *argv[] = {"unset", "REMOVE", NULL};
	int result = builtin_unset(shell, argv);
	
	cr_assert_eq(result, 0, "unset should succeed");
	
	// Verify only the target was removed
	char *value1 = get_variable(shell->vars, "KEEP1");
	char *value2 = get_variable(shell->vars, "KEEP2");
	char *removed = get_variable(shell->vars, "REMOVE");
	
	cr_assert_not_null(value1, "KEEP1 should still exist");
	cr_assert_str_eq(value1, "keep_this", "KEEP1 should preserve value");
	
	cr_assert_not_null(value2, "KEEP2 should still exist");
	cr_assert_str_eq(value2, "keep_this_too", "KEEP2 should preserve value");
	
	cr_assert_null(removed, "REMOVE should be unset");
	
	cleanup_test_shell(shell);
}
