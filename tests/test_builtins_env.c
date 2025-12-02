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

void setup_env_tests(void) {
	cr_redirect_stdout();
	cr_redirect_stderr();
}

void teardown_env_tests(void) {
	cr_redirect_stdout();
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

TestSuite(env_builtin, .init = setup_env_tests, .fini = teardown_env_tests);

Test(env_builtin, env_no_args_returns_0) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"env", NULL};
	
	int result = builtin_env(shell, argv);
	
	cr_assert_eq(result, 0, "env should return 0 on success");
	
	cleanup_test_shell(shell);
}

Test(env_builtin, env_displays_exported_variables) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "TEST_VAR", "test_value", 1, 1);  // exported
	set_variable(shell->vars, "LOCAL_VAR", "local_value", 0, 1); // not exported
	
	char *argv[] = {"env", NULL};
	int result = builtin_env(shell, argv);
	
	cr_assert_eq(result, 0, "env should succeed");
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "TEST_VAR=test_value") != NULL,
				  "Should display exported variable");
		cr_assert(strstr(output, "LOCAL_VAR=") == NULL,
				  "Should NOT display non-exported variable");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(env_builtin, env_handles_null_values) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "NULL_VAR", NULL, 1, 0);
	
	char *argv[] = {"env", NULL};
	int result = builtin_env(shell, argv);
	
	cr_assert_eq(result, 0, "env should handle NULL values without crashing");
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "NULL_VAR=") != NULL,
				  "Should display variable with NULL value as NAME=");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(env_builtin, env_rejects_invalid_options) {
	t_shell *shell = create_test_shell();
	
	char *argv[] = {"env", "-i", NULL};
	int result = builtin_env(shell, argv);
	
	cr_assert_eq(result, 125, "env should reject invalid options with exit code 125");
	
	FILE *stderr_file = cr_get_redirected_stderr();
	fseek(stderr_file, 0, SEEK_END);
	long length = ftell(stderr_file);
	fseek(stderr_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, stderr_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "invalid option") != NULL,
				  "Should show invalid option error message");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(env_builtin, env_rejects_command_execution) {
	t_shell *shell = create_test_shell();
	
	char *argv[] = {"env", "ls", NULL};
	int result = builtin_env(shell, argv);
	
	cr_assert_eq(result, 127, "env should reject command execution with exit code 127");
	
	FILE *stderr_file = cr_get_redirected_stderr();
	fseek(stderr_file, 0, SEEK_END);
	long length = ftell(stderr_file);
	fseek(stderr_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, stderr_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "command not found") != NULL,
				  "Should show command not found error");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(env_builtin, env_null_arguments) {
	t_shell *shell = create_test_shell();
	
	int result = builtin_env(shell, NULL);
	
	cr_assert_eq(result, 0, "env should handle NULL arguments gracefully");
	
	cleanup_test_shell(shell);
}

Test(env_builtin, env_null_shell) {
	char *argv[] = {"env", NULL};
	
	int result = builtin_env(NULL, argv);
	
	cr_assert_eq(result, 1, "env should fail with NULL shell");
}

Test(env_builtin, env_empty_values) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "EMPTY_VAR", "", 1, 1);
	
	char *argv[] = {"env", NULL};
	int result = builtin_env(shell, argv);
	
	cr_assert_eq(result, 0, "env should handle empty values");
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "EMPTY_VAR=") != NULL,
				  "Should display variable with empty value");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(env_builtin, env_output_format) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "FORMAT_TEST", "value123", 1, 1);
	
	char *argv[] = {"env", NULL};
	int result = builtin_env(shell, argv);
	
	cr_assert_eq(result, 0, "env should succeed");
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "FORMAT_TEST=value123\n") != NULL,
				  "Should use NAME=value\\n format");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}
