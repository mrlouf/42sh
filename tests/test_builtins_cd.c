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

// Test suite with setup and teardown
void setup_cd_tests(void) {
    cr_redirect_stderr();
}

void teardown_cd_tests(void) {
    cr_redirect_stderr();
}

TestSuite(cd_builtin, .init = setup_cd_tests, .fini = teardown_cd_tests);

static t_shell *create_test_shell(void) {
    t_shell *shell = calloc(1, sizeof(t_shell));
    if (!shell)
        return NULL;
    
    shell->vars = calloc(1, sizeof(t_var_table));
    if (!shell->vars) {
        free(shell);
        return NULL;
    }
    
    // Initialize with basic environment variables
    char *current_pwd = getcwd(NULL, 0);
    if (current_pwd) {
        set_variable(shell->vars, "PWD", current_pwd, 1);
        free(current_pwd);
    }
    set_variable(shell->vars, "HOME", getenv("HOME"), 1);
    
    return shell;
}

static void cleanup_test_shell(t_shell *shell) {
    if (!shell)
        return;
    
    if (shell->vars)
        free(shell->vars);
    free(shell);
}

Test(cd_builtin, changes_to_valid_directory) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    
    // Use a directory that should exist and be accessible on most systems
    char *test_dir = "/tmp";
    char *argv[] = {"cd", test_dir, NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 0, "cd /tmp should succeed");
    
    char current_dir[1024];
    getcwd(current_dir, sizeof(current_dir));
    
    // Get the realpath of /tmp to handle symlinks (like /tmp -> /private/tmp on macOS)
    char resolved_tmp[1024];
    if (realpath("/tmp", resolved_tmp) != NULL) {
        cr_assert_str_eq(current_dir, resolved_tmp, "Should be in resolved /tmp directory");
    } else {
        // Fallback: just check that we're no longer in the original directory
        cr_assert_neq(strcmp(current_dir, original_dir), 0, "Should have changed from original directory");
    }
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, updates_pwd_environment_variable) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    
    char *argv[] = {"cd", "/tmp", NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 0, "cd should succeed");
    
    char *pwd_value = get_variable(shell->vars, "PWD");
    cr_assert_not_null(pwd_value, "PWD should be set");
    
    // Get the realpath of /tmp to handle symlinks
    char resolved_tmp[1024];
    if (realpath("/tmp", resolved_tmp) != NULL) {
        cr_assert_str_eq(pwd_value, resolved_tmp, "PWD should be updated to resolved /tmp path");
    } else {
        // Fallback: just check that PWD changed from original
        cr_assert_neq(strcmp(pwd_value, original_dir), 0, "PWD should have changed from original directory");
    }

    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, handles_nonexistent_directory) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char *argv[] = {"cd", "/nonexistent/directory", NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_neq(result, 0, "cd to nonexistent directory should fail");
    cr_assert_stderr_neq_str("", "Should output error message to stderr");
    
    cleanup_test_shell(shell);
}

Test(cd_builtin, error_message_format) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char *argv[] = {"cd", "/nonexistent/path/12345", NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 1, "Should return error code 1");
    
    cr_assert_stderr_neq_str("", "Should output error message to stderr");
    
    cleanup_test_shell(shell);
}

Test(cd_builtin, changes_to_home_when_no_arguments) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    
    chdir("/tmp");
    
    char *argv[] = {"cd", NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 0, "cd without arguments should succeed");
    
    char current_dir[1024];
    getcwd(current_dir, sizeof(current_dir));
    
    char *home_value = get_variable(shell->vars, "HOME");
    cr_assert_not_null(home_value, "HOME should be set");
    cr_assert_str_eq(current_dir, home_value, "Should be in HOME directory");
    
    // Cleanup
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, handles_missing_home_variable) {
    t_shell *shell = malloc(sizeof(t_shell));
    cr_assert_not_null(shell, "Failed to allocate test shell");
    
    shell->vars = malloc(sizeof(t_var_table));
    cr_assert_not_null(shell->vars, "Failed to allocate variable table");
    
    for (int i = 0; i < VAR_HASH_SIZE; i++) {
        shell->vars->buckets[i] = NULL;
    }
    
    char *argv[] = {"cd", NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 1, "cd without arguments and no HOME should fail");

    cr_assert_stderr_neq_str("", "Should output error message about HOME not set");
    
    cleanup_test_shell(shell);
}

Test(cd_builtin, handles_relative_paths) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    
    // First go to a known directory
    chdir("/tmp");
    char tmp_dir[1024];
    getcwd(tmp_dir, sizeof(tmp_dir));
    
    char *argv[] = {"cd", "..", NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 0, "cd .. should succeed");
    
    char current_dir[1024];
    getcwd(current_dir, sizeof(current_dir));
    
    // Get the parent directory of where we were (/tmp/..)
    char expected_parent[1024];
    strcpy(expected_parent, tmp_dir);
    char *last_slash = strrchr(expected_parent, '/');
    if (last_slash != NULL && last_slash != expected_parent) {
        *last_slash = '\0';
    } else {
        strcpy(expected_parent, "/");
    }
    
    cr_assert_str_eq(current_dir, expected_parent, "Should be in parent directory");
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, handles_permission_denied) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char *argv[] = {"cd", "/root", NULL};
    
    int result = builtin_cd(shell, argv);
    if (access("/root", X_OK) != 0) {
        cr_assert_neq(result, 0, "cd to /root should fail for non-root users");
        cr_assert_stderr_neq_str("", "Should output error message");
    }
    
    cleanup_test_shell(shell);
}

Test(cd_builtin, preserves_original_directory_on_failure) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    
    char *original_pwd = get_variable(shell->vars, "PWD");
    cr_assert_not_null(original_pwd, "PWD should be set initially");
    
    char *argv[] = {"cd", "/nonexistent/directory", NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_neq(result, 0, "cd should fail");
    
    char current_dir[1024];
    getcwd(current_dir, sizeof(current_dir));
    cr_assert_str_eq(current_dir, original_dir, "Directory should be unchanged after failed cd");
    
    char *current_pwd = get_variable(shell->vars, "PWD");
    cr_assert_str_eq(current_pwd, original_pwd, "PWD should be unchanged after failed cd");
    
    cleanup_test_shell(shell);
}

Test(cd_builtin, handles_multiple_arguments_correctly) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    
    char *argv[] = {"cd", "/tmp", "/usr", "ignored", NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 0, "cd should succeed with first argument");
    
    char current_dir[1024];
    getcwd(current_dir, sizeof(current_dir));
    
    // Get the realpath of /tmp to handle symlinks
    char resolved_tmp[1024];
    if (realpath("/tmp", resolved_tmp) != NULL) {
        cr_assert_str_eq(current_dir, resolved_tmp, "Should be in resolved /tmp (first argument)");
    } else {
        // Fallback: just check that we changed from original directory
        cr_assert_neq(strcmp(current_dir, original_dir), 0, "Should have changed from original directory");
    }
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}
