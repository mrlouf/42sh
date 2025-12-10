#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>  // For PATH_MAX
#include "../incs/42sh.h"
#include "../incs/builtins.h"
#include "../incs/env.h" // Añadir este include para las funciones de variables

// Test suite with setup and teardown
void setup_type_tests(void) {
    cr_redirect_stdout(); // type outputs results to stdout
    cr_redirect_stderr();
}

// No specific cleanup needed unless files created
void teardown_type_tests(void) {
    // cr_redirect_stderr();
}

TestSuite(type_builtin, .init = setup_type_tests, .fini = teardown_type_tests);

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
        int result = set_variable(shell->vars, "PWD", current_pwd, 1, 1);
        if (result != 0) {
            printf("DEBUG: Failed to set PWD, error: %d\n", result);
        }
        free(current_pwd);
    }
    
    char *home_env = getenv("HOME");
    if (home_env) {
        int result = set_variable(shell->vars, "HOME", home_env, 1, 1);
        if (result != 0) {
            printf("DEBUG: Failed to set HOME, error: %d\n", result);
        }
    } else {
        set_variable(shell->vars, "HOME", "/tmp", 1, 1);
    }
    
    return shell;
}

static void cleanup_test_shell(t_shell *shell) {
    if (!shell)
        return;
    
    if (shell->vars)
        free(shell->vars);
    free(shell);
}

/*
    TEST 1
    Check if a type intentifies a common command (ls,echo...)
*/
Test(type_builtin, finds_external_command) {
    // Create shell, set PATH
    t_shell *shell = create_test_shell();

    char *argv[] = {"type", "ls", NULL};
    int result = builtin_type(shell, argv);

	cr_assert_eq(result, 0, "type ls should succeed"); // Usar el result aquí

	// Capture the stdout 
	FILE *output_file = cr_get_redirected_stdout();
	fseek(output_file, 0, SEEK_END);
	long length = ftell(output_file);
	fseek(output_file, 0, SEEK_SET);

	if (length > 0) {
		char *output = malloc(length + 1);
		fread(output, 1, length, output_file);
		output[length] = '\0';

		// Check that the info is the expected 
		cr_assert(strstr(output, "ls is /bin/ls") != NULL, 
			"Should display 'ls is /bin/ls'");
		free(output);
	}
	cleanup_test_shell(shell);
}
