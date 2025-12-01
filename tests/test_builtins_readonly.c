#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>  // For PATH_MAX
#include "../incs/42sh.h"
#include "../incs/env.h"
#include "../incs/main.h"
#include "../incs/builtins.h"

void setup_readonly_tests(void) {
	cr_redirect_stdout();
}

void teardown_readonly_tests(void) {
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

TestSuite(readonly_builtin, .init = setup_readonly_tests, .fini = teardown_readonly_tests);


Test(readonly_builtin, readonly_no_args_returns_0) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"export", NULL};
	
	int result = builtin_readonly(shell, argv);
	
	cr_assert_eq(result, 0, "readonly should return 0 on success");
	
	cleanup_test_shell(shell);
}

Test(readonly_builtin, readonly_no_args_returns_ordered_list)
{
    t_shell *shell = create_test_shell();

    set_variable(shell->vars, "VAR1", "test_var", 1, 1);
    set_variable(shell->vars, "VAR2", "test_var", 1, 1);
    set_variable(shell->vars, "VAR3", "test_var", 1, 1);
    mark_variable_as_readonly(shell->vars, "VAR1");
    mark_variable_as_readonly(shell->vars, "VAR2");
    mark_variable_as_readonly(shell->vars, "VAR3");

    size_t	count = 5;
    t_var   **ordered = get_sorted_readonly_refs(shell->vars, &count);

    cr_assert_not_null(ordered, "Sorted variable of readonly variables can' be null");

    cr_assert_gt(ft_strcmp(ordered[2]->name, ordered[1]->name), 0, "Returned variable list must be alphabetically ordered");
    cr_assert_lt(ft_strcmp(ordered[1]->name, ordered[2]->name), 0, "Returned variable list must be alphabetically ordered");

	free(ordered);
	cleanup_test_shell(shell);
}

Test(readonly_builtin, readonly_format_structure)
{
    t_shell *shell = create_test_shell();
    
    set_variable(shell->vars, "TEST_VAR", "test_value", 1, 1);
    mark_variable_as_readonly(shell->vars, "TEST_VAR");

    char *argv[] = {"readonly", NULL};
    int result = builtin_readonly(shell, argv);
    
    cr_assert_eq(result, 0, "export should return 0");
    
    FILE *output_file = cr_get_redirected_stdout();
    fseek(output_file, 0, SEEK_END);
    long length = ftell(output_file);
    fseek(output_file, 0, SEEK_SET);
    
    if (length > 0) {
        char *output = malloc(length + 1);
        fread(output, 1, length, output_file);
        output[length] = '\0';
        
        cr_assert(strstr(output, "declare -r TEST_VAR=\"test_value\"") != NULL,
                  "Output should contain 'declare -r TEST_VAR=test_value'");
        
        char *line_start = output;
        char *line_end;
        while ((line_end = strchr(line_start, '\n')) != NULL) {
            if (line_end > line_start) {
                cr_assert(strncmp(line_start, "declare -r ", 11) == 0,
                          "Each line should start with 'declare -r '");
                
                char *equals = memchr(line_start, '=', line_end - line_start);
                cr_assert(equals != NULL, "Each line should contain '='");
            }
            line_start = line_end + 1;
        }
        
        free(output);
    }
    
    cleanup_test_shell(shell);
}


Test(readonly_builtin, readonly_variable_with_value)
{
    t_shell *shell = create_test_shell();
    
    char *argv[] = {"readonly", "MYVAR=myvalue", NULL};
    int result = builtin_readonly(shell, argv);
    
    cr_assert_eq(result, 0, "export should succeed");
    
    char *value = get_variable(shell->vars, "MYVAR");
    cr_assert_not_null(value, "MYVAR should be set");
    cr_assert_str_eq(value, "myvalue", "MYVAR should have correct value");

	size_t	count = 1;
	t_var	**sorted = get_sorted_readonly_refs(shell->vars, &count);
	cr_assert_eq(sorted[0]->readonly, 1, "Variable set as readonly should have readonly value of 1");
	
	free(sorted);
    cleanup_test_shell(shell);
}

Test(readonly_builtin, readonly_overwrite_existing)
{
    t_shell *shell = create_test_shell();
    
    int result = set_variable(shell->vars, "OVERWRITE", "test", 1, 1);
	cr_assert_eq(result, 0, "set_variable should not fail");

	result = mark_variable_as_readonly(shell->vars, "OVERWRITE");
	cr_assert_eq(result, 0, "mark_variable_as_readonly should not fail");
    
    char *argv[] = {"readonly", "OVERWRITE=TEST2", NULL};
	result = builtin_readonly(shell, argv);
    cr_assert_eq(result, 1, "Re-assignation of readonly variable should fail");
    
    cleanup_test_shell(shell);
}

Test(readonly_builtin, export_null_arguments)
{
    t_shell *shell = create_test_shell();
    
    int result = builtin_readonly(shell, NULL);
    cr_assert_eq(result, 1, "readonly should fail with NULL arguments");
    
    cleanup_test_shell(shell);
}
