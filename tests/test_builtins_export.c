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

void setup_export_tests(void) {
	cr_redirect_stdout();
}

void teardown_export_tests(void) {
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

TestSuite(export_builtin, .init = setup_export_tests, .fini = teardown_export_tests);


Test(export_builtin, export_no_args_returns_0) {
	t_shell *shell = create_test_shell();
	char *argv[] = {"export", NULL};
	
	int result = builtin_export(shell, argv);
	
	cr_assert_eq(result, 0, "export should return 0 on success");
	
	cleanup_test_shell(shell);
}

Test(export_builtin, export_no_args_returns_ordered_list)
{
	t_shell *shell = create_test_shell();
	
	set_variable(shell->vars, "ZZZ", "last", 1, 1);
	set_variable(shell->vars, "AAA", "first", 1, 1);
	set_variable(shell->vars, "MMM", "middle", 1, 1);

	size_t  count = count_stored_env_variables(shell->vars);
	t_var   **sorted_refs = get_sorted_variable_refs(shell->vars, &count);

	cr_assert_not_null(sorted_refs);

	cr_assert_eq(count, 5);

	cr_assert_gt(ft_strcmp(sorted_refs[count - 1]->name, sorted_refs[count - 2]->name), 0, "Comparison should return a value larger than 0");
	cr_assert_lt(ft_strcmp(sorted_refs[count - 3]->name, sorted_refs[count - 1]->name), 0, "Comparison should return a value lower than 0");

	free(sorted_refs);
	cleanup_test_shell(shell);
}

Test(export_builtin, export_format_structure)
{
    t_shell *shell = create_test_shell();
    
    set_variable(shell->vars, "TEST_VAR", "test_value", 1, 1);
    
    char *argv[] = {"export", NULL};
    int result = builtin_export(shell, argv);
    
    cr_assert_eq(result, 0, "export should return 0");
    
    FILE *output_file = cr_get_redirected_stdout();
    fseek(output_file, 0, SEEK_END);
    long length = ftell(output_file);
    fseek(output_file, 0, SEEK_SET);
    
    if (length > 0) {
        char *output = malloc(length + 1);
        fread(output, 1, length, output_file);
        output[length] = '\0';
        
        cr_assert(strstr(output, "declare -x TEST_VAR=\"test_value\"") != NULL,
                  "Output should contain 'declare -x TEST_VAR=test_value'");
        
        char *line_start = output;
        char *line_end;
        while ((line_end = strchr(line_start, '\n')) != NULL) {
            if (line_end > line_start) {
                cr_assert(strncmp(line_start, "declare -x ", 11) == 0,
                          "Each line should start with 'declare -x '");
                
                char *equals = memchr(line_start, '=', line_end - line_start);
                cr_assert(equals != NULL, "Each line should contain '='");
            }
            line_start = line_end + 1;
        }
        
        free(output);
    }
    
    cleanup_test_shell(shell);
}


Test(export_builtin, export_variable_with_value)
{
    t_shell *shell = create_test_shell();
    
    char *argv[] = {"export", "MYVAR=myvalue", NULL};
    int result = builtin_export(shell, argv);
    
    cr_assert_eq(result, 0, "export should succeed");
    
    char *value = get_variable(shell->vars, "MYVAR");
    cr_assert_not_null(value, "MYVAR should be set");
    cr_assert_str_eq(value, "myvalue", "MYVAR should have correct value");
    
    cleanup_test_shell(shell);
}

Test(export_builtin, export_bare_variable)
{
    t_shell *shell = create_test_shell();
    
    char *argv[] = {"export", "BAREVAR", NULL};
    int result = builtin_export(shell, argv);
    
    cr_assert_eq(result, 0, "export should succeed");
    
    char *value = get_variable(shell->vars, "BAREVAR");
    cr_assert_null(value, "BAREVAR should have no value (NULL)");
    
    char *export_argv[] = {"export", NULL};
    builtin_export(shell, export_argv);
    
    FILE *output_file = cr_get_redirected_stdout();
    fseek(output_file, 0, SEEK_END);
    long length = ftell(output_file);
    fseek(output_file, 0, SEEK_SET);
    
    if (length > 0) {
        char *output = malloc(length + 1);
        fread(output, 1, length, output_file);
        output[length] = '\0';
        
        cr_assert(strstr(output, "declare -x BAREVAR\n") != NULL,
                  "Output should contain bare variable without value");
        
        free(output);
    }
    
    cleanup_test_shell(shell);
}

Test(export_builtin, export_multiple_variables)
{
    t_shell *shell = create_test_shell();
    
    char *argv[] = {"export", "VAR1=value1", "VAR2", "VAR3=value3", NULL};
    int result = builtin_export(shell, argv);
    
    cr_assert_eq(result, 0, "export should succeed");
    
    char *value1 = get_variable(shell->vars, "VAR1");
    cr_assert_str_eq(value1, "value1", "VAR1 should have correct value");
    
    char *value2 = get_variable(shell->vars, "VAR2");
    cr_assert_null(value2, "VAR2 should have no value");
    
    char *value3 = get_variable(shell->vars, "VAR3");
    cr_assert_str_eq(value3, "value3", "VAR3 should have correct value");
    
    cleanup_test_shell(shell);
}

Test(export_builtin, export_empty_value)
{
    t_shell *shell = create_test_shell();
    
    char *argv[] = {"export", "EMPTYVAR=", NULL};
    int result = builtin_export(shell, argv);
    
    cr_assert_eq(result, 0, "export should succeed");
    
    char *value = get_variable(shell->vars, "EMPTYVAR");
    cr_assert_not_null(value, "EMPTYVAR should be set");
    cr_assert_str_eq(value, "", "EMPTYVAR should have empty value");
    
    cleanup_test_shell(shell);
}

Test(export_builtin, export_invalid_variable_names)
{
    t_shell *shell = create_test_shell();
    
    cr_redirect_stderr();
    
    char *argv1[] = {"export", "123VAR=value", NULL};
    int result1 = builtin_export(shell, argv1);
    cr_assert_eq(result1, 1, "export should fail for variable starting with digit");
    
    char *argv2[] = {"export", "VAR-NAME=value", NULL};
    int result2 = builtin_export(shell, argv2);
    cr_assert_eq(result2, 1, "export should fail for variable with hyphen");
    
    char *argv3[] = {"export", "=value", NULL};
    int result3 = builtin_export(shell, argv3);
    cr_assert_eq(result3, 1, "export should fail for empty variable name");
    
    cleanup_test_shell(shell);
}

Test(export_builtin, export_mixed_valid_invalid)
{
    t_shell *shell = create_test_shell();
    
    cr_redirect_stderr();
    
    char *argv[] = {"export", "VALID1=good", "123INVALID=bad", "VALID2=good", NULL};
    int result = builtin_export(shell, argv);
    
    cr_assert_eq(result, 1, "export should return error code due to invalid variable");
    
    char *value1 = get_variable(shell->vars, "VALID1");
    cr_assert_str_eq(value1, "good", "Valid variable should be set");
    
    char *value2 = get_variable(shell->vars, "VALID2");
    cr_assert_str_eq(value2, "good", "Valid variable should be set");
    
    char *invalid = get_variable(shell->vars, "123INVALID");
    cr_assert_null(invalid, "Invalid variable should not be set");
    
    cleanup_test_shell(shell);
}

Test(export_builtin, export_underscore_variables)
{
    t_shell *shell = create_test_shell();
    
    char *argv[] = {"export", "_UNDERSCORE=value", "VAR_WITH_UNDER=value2", "___TRIPLE=value3", NULL};
    int result = builtin_export(shell, argv);
    
    cr_assert_eq(result, 0, "export should succeed for underscore variables");
    
    char *value1 = get_variable(shell->vars, "_UNDERSCORE");
    cr_assert_str_eq(value1, "value", "Underscore variable should be set");
    
    char *value2 = get_variable(shell->vars, "VAR_WITH_UNDER");
    cr_assert_str_eq(value2, "value2", "Variable with underscore should be set");
    
    char *value3 = get_variable(shell->vars, "___TRIPLE");
    cr_assert_str_eq(value3, "value3", "Triple underscore variable should be set");
    
    cleanup_test_shell(shell);
}


Test(export_builtin, export_variables_with_numbers)
{
    t_shell *shell = create_test_shell();
    
    char *argv[] = {"export", "VAR123=value", "TEST2VAR=value2", "A1B2C3=value3", NULL};
    int result = builtin_export(shell, argv);
    
    cr_assert_eq(result, 0, "export should succeed for variables with numbers");
    
    char *value1 = get_variable(shell->vars, "VAR123");
    cr_assert_str_eq(value1, "value", "Variable with numbers should be set");
    
    char *value2 = get_variable(shell->vars, "TEST2VAR");
    cr_assert_str_eq(value2, "value2", "Variable with embedded number should be set");
    
    cleanup_test_shell(shell);
}

Test(export_builtin, export_overwrite_existing)
{
    t_shell *shell = create_test_shell();
    
    char *argv1[] = {"export", "OVERWRITE=original", NULL};
    builtin_export(shell, argv1);
    
    char *original = get_variable(shell->vars, "OVERWRITE");
    cr_assert_str_eq(original, "original", "Original value should be set");
    
    char *argv2[] = {"export", "OVERWRITE=new", NULL};
    int result = builtin_export(shell, argv2);
    
    cr_assert_eq(result, 0, "export should succeed");
    
    char *new_value = get_variable(shell->vars, "OVERWRITE");
    cr_assert_str_eq(new_value, "new", "Variable should have new value");
    
    cleanup_test_shell(shell);
}

Test(export_builtin, export_value_with_equals)
{
    t_shell *shell = create_test_shell();
    
    char *argv[] = {"export", "EQUATION=x=y+z", "URL=http://example.com/?param=value", NULL};
    int result = builtin_export(shell, argv);
    
    cr_assert_eq(result, 0, "export should succeed");
    
    char *equation = get_variable(shell->vars, "EQUATION");
    cr_assert_str_eq(equation, "x=y+z", "Variable should contain equals signs in value");
    
    char *url = get_variable(shell->vars, "URL");
    cr_assert_str_eq(url, "http://example.com/?param=value", "URL with equals should be handled correctly");
    
    cleanup_test_shell(shell);
}

Test(export_builtin, export_null_arguments)
{
    t_shell *shell = create_test_shell();
    
    int result = builtin_export(shell, NULL);
    cr_assert_eq(result, 1, "export should fail with NULL arguments");
    
    cleanup_test_shell(shell);
}

Test(export_builtin, export_null_shell)
{
    char *argv[] = {"export", "VAR=value", NULL};
    
    int result = builtin_export(NULL, argv);
    cr_assert_eq(result, 1, "export should fail with NULL shell");
}
