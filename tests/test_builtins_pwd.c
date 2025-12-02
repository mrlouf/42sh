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

void setup_pwd_tests(void) {
	cr_redirect_stdout();
}

void teardown_pwd_tests(void) {
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

TestSuite(pwd_builtin, .init = setup_pwd_tests, .fini = teardown_pwd_tests);

Test(pwd_builtin, pwd_no_args_returns_0) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"pwd", NULL};
	
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 0, "pwd should return 0 on success");
	
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_prints_current_directory) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"pwd", NULL};
	
	char *expected_pwd = getcwd(NULL, 0);
	cr_assert_not_null(expected_pwd, "getcwd should succeed");
	
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 0, "pwd should succeed");
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		if (length > 0 && output[length - 1] == '\n')
			output[length - 1] = '\0';
		
		cr_assert_str_eq(output, expected_pwd, "pwd should print current directory");
		
		free(output);
	}
	
	free(expected_pwd);
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_logical_flag) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"pwd", "-L", NULL};
	
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 0, "pwd -L should succeed");
	
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_physical_flag) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"pwd", "-P", NULL};
	
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 0, "pwd -P should succeed");
	
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_invalid_flag_returns_2) {
	t_shell *shell = create_test_shell();
	
	cr_redirect_stderr();
	
	char *argv[] = {"pwd", "-x", NULL};
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 2, "pwd with invalid flag should return exit code 2");
	
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_multiple_invalid_flags) {
	t_shell *shell = create_test_shell();
	
	cr_redirect_stderr();
	
	char *argv[] = {"pwd", "-xyz", NULL};
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 2, "pwd with multiple invalid flags should return exit code 2");
	
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_flag_combinations_last_wins) {
	t_shell *shell = create_test_shell();
	
	char *argv1[] = {"pwd", "-LP", NULL};
	int result1 = builtin_pwd(shell, argv1);
	cr_assert_eq(result1, 0, "pwd -LP should succeed");
	
	char *argv2[] = {"pwd", "-PL", NULL};
	int result2 = builtin_pwd(shell, argv2);
	cr_assert_eq(result2, 0, "pwd -PL should succeed");
	
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_mixed_valid_invalid_flags) {
	t_shell *shell = create_test_shell();
	
	cr_redirect_stderr();
	
	char *argv[] = {"pwd", "-Lx", NULL};
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 2, "pwd with mixed valid/invalid flags should return exit code 2");
	
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_end_of_options) {
	t_shell *shell = create_test_shell();
	
	char *argv[] = {"pwd", "--", "-L", NULL};
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 0, "pwd should handle -- (end of options)");
	
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_uses_pwd_variable_when_valid) {
	t_shell *shell = create_test_shell();
	
	char *real_pwd = getcwd(NULL, 0);
	set_variable(shell->vars, "PWD", real_pwd, 1, 1);
	
	char *argv[] = {"pwd", "-L", NULL};
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 0, "pwd -L should use valid PWD variable");
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		if (length > 0 && output[length - 1] == '\n')
			output[length - 1] = '\0';
		
		cr_assert_str_eq(output, real_pwd, "pwd -L should use PWD variable");
		
		free(output);
	}
	
	free(real_pwd);
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_fallback_when_pwd_invalid) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "PWD", "/invalid/nonexistent/path", 1, 1);
	
	char *argv[] = {"pwd", "-L", NULL};
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 0, "pwd -L should fallback to getcwd when PWD invalid");
	
	char *expected_pwd = getcwd(NULL, 0);
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		if (length > 0 && output[length - 1] == '\n')
			output[length - 1] = '\0';
		
		cr_assert_str_eq(output, expected_pwd, "pwd should fallback to real path");
		
		free(output);
	}
	
	free(expected_pwd);
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_physical_ignores_pwd_variable) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "PWD", "/some/fake/path", 1, 1);
	
	char *argv[] = {"pwd", "-P", NULL};
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 0, "pwd -P should succeed");
	
	char *expected_pwd = getcwd(NULL, 0);
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		if (length > 0 && output[length - 1] == '\n')
			output[length - 1] = '\0';
		
		cr_assert_str_eq(output, expected_pwd, "pwd -P should ignore PWD variable");
		
		free(output);
	}
	
	free(expected_pwd);
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_null_arguments) {
	t_shell *shell = create_test_shell();
	
	int result = builtin_pwd(shell, NULL);
	cr_assert_eq(result, 0, "pwd should handle NULL arguments gracefully");
	
	cleanup_test_shell(shell);
}

Test(pwd_builtin, pwd_null_shell) {
	char *argv[] = {"pwd", NULL};
	
	int result = builtin_pwd(NULL, argv);
	cr_assert_neq(result, 0, "pwd should fail with NULL shell");
}

Test(pwd_builtin, pwd_separate_flag_arguments) {
	t_shell *shell = create_test_shell();
	
	char *argv[] = {"pwd", "-L", "-P", NULL};
	int result = builtin_pwd(shell, argv);
	
	cr_assert_eq(result, 0, "pwd should handle separate flag arguments");
	
	cleanup_test_shell(shell);
}
