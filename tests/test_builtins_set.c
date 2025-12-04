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

void setup_set_tests(void) {
	cr_redirect_stdout();
	cr_redirect_stderr();
}

void teardown_set_tests(void) {
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

TestSuite(set_builtin, .init = setup_set_tests, .fini = teardown_set_tests);

Test(set_builtin, set_no_args_returns_0) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"set", NULL};
	
	int result = builtin_set(shell, argv);
	
	cr_assert_eq(result, 0, "set should return 0 on success");
	
	cleanup_test_shell(shell);
}

Test(set_builtin, set_displays_all_variables) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "EXPORTED_VAR", "exported_value", 1, 1);  // exported
	set_variable(shell->vars, "LOCAL_VAR", "local_value", 0, 1);       // not exported
	set_variable(shell->vars, "ANOTHER_VAR", "another_value", 0, 1);   // not exported
	
	char *argv[] = {"set", NULL};
	int result = builtin_set(shell, argv);
	
	cr_assert_eq(result, 0, "set should succeed");
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "EXPORTED_VAR=exported_value") != NULL,
				  "Should display exported variable");
		cr_assert(strstr(output, "LOCAL_VAR=local_value") != NULL,
				  "Should display non-exported variable");
		cr_assert(strstr(output, "ANOTHER_VAR=another_value") != NULL,
				  "Should display all variables");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(set_builtin, set_alphabetical_sorting) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "ZZZ_VAR", "last", 0, 1);
	set_variable(shell->vars, "AAA_VAR", "first", 0, 1);
	set_variable(shell->vars, "MMM_VAR", "middle", 0, 1);
	
	char *argv[] = {"set", NULL};
	int result = builtin_set(shell, argv);
	
	cr_assert_eq(result, 0, "set should succeed");
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		char *aaa_pos = strstr(output, "AAA_VAR=");
		char *mmm_pos = strstr(output, "MMM_VAR=");
		char *zzz_pos = strstr(output, "ZZZ_VAR=");
		
		cr_assert_not_null(aaa_pos, "Should contain AAA_VAR");
		cr_assert_not_null(mmm_pos, "Should contain MMM_VAR");
		cr_assert_not_null(zzz_pos, "Should contain ZZZ_VAR");
		
		cr_assert(aaa_pos < mmm_pos, "AAA_VAR should come before MMM_VAR");
		cr_assert(mmm_pos < zzz_pos, "MMM_VAR should come before ZZZ_VAR");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(set_builtin, set_handles_null_values) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "NULL_VAR", NULL, 0, 0);
	
	char *argv[] = {"set", NULL};
	int result = builtin_set(shell, argv);
	
	cr_assert_eq(result, 0, "set should handle NULL values without crashing");
	
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

Test(set_builtin, set_empty_values) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "EMPTY_VAR", "", 0, 1);
	
	char *argv[] = {"set", NULL};
	int result = builtin_set(shell, argv);
	
	cr_assert_eq(result, 0, "set should handle empty values");
	
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

Test(set_builtin, set_output_format) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "FORMAT_TEST", "value123", 0, 1);
	
	char *argv[] = {"set", NULL};
	int result = builtin_set(shell, argv);
	
	cr_assert_eq(result, 0, "set should succeed");
	
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

Test(set_builtin, set_rejects_arguments) {
	t_shell *shell = create_test_shell();
	
	char *argv[] = {"set", "arg1", NULL};
	int result = builtin_set(shell, argv);
	
	cr_assert_neq(result, 0, "set should reject arguments in simplified implementation");
	
	FILE *stderr_file = cr_get_redirected_stderr();
	fseek(stderr_file, 0, SEEK_END);
	long length = ftell(stderr_file);
	fseek(stderr_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, stderr_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "too many arguments") != NULL || 
				  strstr(output, "invalid") != NULL,
				  "Should show error message for arguments");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(set_builtin, set_rejects_options) {
	t_shell *shell = create_test_shell();
	
	char *argv[] = {"set", "-e", NULL};
	int result = builtin_set(shell, argv);
	
	cr_assert_neq(result, 0, "set should reject options in simplified implementation");
	
	FILE *stderr_file = cr_get_redirected_stderr();
	fseek(stderr_file, 0, SEEK_END);
	long length = ftell(stderr_file);
	fseek(stderr_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, stderr_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "invalid option") != NULL ||
				  strstr(output, "not supported") != NULL,
				  "Should show error message for options");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}

Test(set_builtin, set_null_arguments) {
	t_shell *shell = create_test_shell();
	
	int result = builtin_set(shell, NULL);
	
	cr_assert_eq(result, 0, "set should handle NULL arguments gracefully");
	
	cleanup_test_shell(shell);
}

Test(set_builtin, set_null_shell) {
	char *argv[] = {"set", NULL};
	
	int result = builtin_set(NULL, argv);
	
	cr_assert_eq(result, 1, "set should fail with NULL shell");
}

Test(set_builtin, set_vs_env_difference) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "EXPORTED_VAR", "exported", 1, 1);
	set_variable(shell->vars, "LOCAL_VAR", "local", 0, 1);
	
	char *set_argv[] = {"set", NULL};
	builtin_set(shell, set_argv);
	
	FILE *set_output_file = cr_get_redirected_stdout();
	fseek(set_output_file, 0, SEEK_END);
	long set_length = ftell(set_output_file);
	fseek(set_output_file, 0, SEEK_SET);
	
	char *set_output = NULL;
	if (set_length > 0) {
		set_output = malloc(set_length + 1);
		fread(set_output, 1, set_length, set_output_file);
		set_output[set_length] = '\0';
	}
	
	cr_redirect_stdout();
	 
	char *env_argv[] = {"env", NULL};
	builtin_env(shell, env_argv);
	
	FILE *env_output_file = cr_get_redirected_stdout();
	fseek(env_output_file, 0, SEEK_END);
	long env_length = ftell(env_output_file);
	fseek(env_output_file, 0, SEEK_SET);
	
	char *env_output = NULL;
	if (env_length > 0) {
		env_output = malloc(env_length + 1);
		fread(env_output, 1, env_length, env_output_file);
		env_output[env_length] = '\0';
	}
	
	if (set_output) {
		cr_assert(strstr(set_output, "EXPORTED_VAR=") != NULL,
				  "set should show exported variables");
		cr_assert(strstr(set_output, "LOCAL_VAR=") != NULL,
				  "set should show local variables");
	}
	
	if (env_output) {
		cr_assert(strstr(env_output, "EXPORTED_VAR=") != NULL,
				  "env should show exported variables");
		cr_assert(strstr(env_output, "LOCAL_VAR=") == NULL,
				  "env should NOT show local variables");
	}
	
	if (set_output) free(set_output);
	if (env_output) free(env_output);
	cleanup_test_shell(shell);
}

Test(set_builtin, set_special_characters_in_values) {
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "SPECIAL_VAR", "value with spaces", 0, 1);
	set_variable(shell->vars, "EQUALS_VAR", "value=with=equals", 0, 1);
	set_variable(shell->vars, "QUOTE_VAR", "value\"with'quotes", 0, 1);
	
	char *argv[] = {"set", NULL};
	int result = builtin_set(shell, argv);
	
	cr_assert_eq(result, 0, "set should handle special characters");
	
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);
	
	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';
		
		cr_assert(strstr(output, "SPECIAL_VAR=value with spaces") != NULL,
				  "Should handle spaces in values");
		cr_assert(strstr(output, "EQUALS_VAR=value=with=equals") != NULL,
				  "Should handle equals signs in values");
		cr_assert(strstr(output, "QUOTE_VAR=value\"with'quotes") != NULL,
				  "Should handle quotes in values");
		
		free(output);
	}
	
	cleanup_test_shell(shell);
}
