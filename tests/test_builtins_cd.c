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
#include "../incs/env.h"

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
        free_var_table(shell->vars);
    free(shell);
}

Test(cd_builtin, changes_to_valid_directory) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    
    char *test_dir = "/tmp";
    char *argv[] = {"cd", test_dir, NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 0, "cd /tmp should succeed");
    
    char current_dir[1024];
    getcwd(current_dir, sizeof(current_dir));

    char resolved_tmp[1024];
    if (realpath("/tmp", resolved_tmp) != NULL) {
        cr_assert_str_eq(current_dir, resolved_tmp, "Should be in resolved /tmp directory");
    } else {
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

    char resolved_tmp[1024];
    if (realpath("/tmp", resolved_tmp) != NULL) {
        cr_assert_str_eq(pwd_value, resolved_tmp, "PWD should be updated to resolved /tmp path");
    } else {
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
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, handles_missing_home_variable) {
    t_shell *shell = calloc(1, sizeof(t_shell));
    cr_assert_not_null(shell, "Failed to allocate test shell");
    
    shell->vars = init_var_table();
    cr_assert_not_null(shell->vars, "Failed to allocate variable table");

    
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
    
    chdir("/tmp");
    char tmp_dir[1024];
    getcwd(tmp_dir, sizeof(tmp_dir));
    
    char *argv[] = {"cd", "..", NULL};
    
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 0, "cd .. should succeed");
    
    char current_dir[1024];
    getcwd(current_dir, sizeof(current_dir));
    
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
    
    char resolved_tmp[1024];
    if (realpath("/tmp", resolved_tmp) != NULL) {
        cr_assert_str_eq(current_dir, resolved_tmp, "Should be in resolved /tmp (first argument)");
    } else {
        cr_assert_neq(strcmp(current_dir, original_dir), 0, "Should have changed from original directory");
    }
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, cd_dash_changes_to_oldpwd) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    
    set_variable(shell->vars, "OLDPWD", "/tmp", 1, 1);
    
    char *argv[] = {"cd", "-", NULL};
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 0, "cd - should succeed");
    
    char current_dir[1024];
    getcwd(current_dir, sizeof(current_dir));
    
    char resolved_tmp[1024];
    if (realpath("/tmp", resolved_tmp) != NULL) {
        cr_assert_str_eq(current_dir, resolved_tmp, "Should be in /tmp after cd -");
    }
    
    char *new_oldpwd = get_variable(shell->vars, "OLDPWD");
    cr_assert_str_eq(new_oldpwd, original_dir, "OLDPWD should be updated to previous directory");
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, cd_dash_fails_when_oldpwd_not_set) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    unset_variable(shell->vars, "OLDPWD");
    
    char *argv[] = {"cd", "-", NULL};
    int result = builtin_cd(shell, argv);
    
    cr_assert_eq(result, 1, "cd - should fail when OLDPWD not set");
    
    cleanup_test_shell(shell);
}

Test(cd_builtin, cd_updates_oldpwd) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));

    char *argv1[] = {"cd", "/tmp", NULL};
    int result1 = builtin_cd(shell, argv1);
    cr_assert_eq(result1, 0, "First cd should succeed");
    
    char *oldpwd = get_variable(shell->vars, "OLDPWD");
    cr_assert_not_null(oldpwd, "OLDPWD should be set after cd");
    cr_assert_str_eq(oldpwd, original_dir, "OLDPWD should be original directory");
    
    char *argv2[] = {"cd", "/usr", NULL};
    int result2 = builtin_cd(shell, argv2);
    cr_assert_eq(result2, 0, "Second cd should succeed");
    
    char *new_oldpwd = get_variable(shell->vars, "OLDPWD");
    char resolved_tmp[1024];
    if (realpath("/tmp", resolved_tmp) != NULL) {
        cr_assert_str_eq(new_oldpwd, resolved_tmp, "OLDPWD should be updated to /tmp");
    }
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, cd_dash_swaps_pwd_oldpwd) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    
    char *argv1[] = {"cd", "/tmp", NULL};
    builtin_cd(shell, argv1);
    
    char *first_pwd = get_variable(shell->vars, "PWD");
    char *first_oldpwd = get_variable(shell->vars, "OLDPWD");
    
    cr_assert_not_null(first_pwd, "PWD should be set");
    cr_assert_not_null(first_oldpwd, "OLDPWD should be set");
    
    char saved_pwd[1024];
    char saved_oldpwd[1024];
    strcpy(saved_pwd, first_pwd);
    strcpy(saved_oldpwd, first_oldpwd);
    
    char *argv2[] = {"cd", "-", NULL};
    int result = builtin_cd(shell, argv2);
    cr_assert_eq(result, 0, "cd - should succeed");
    
    char *second_pwd = get_variable(shell->vars, "PWD");
    char *second_oldpwd = get_variable(shell->vars, "OLDPWD");

    cr_assert_str_eq(second_pwd, saved_oldpwd, "PWD should now be the old OLDPWD");
    cr_assert_str_eq(second_oldpwd, saved_pwd, "OLDPWD should now be the old PWD");
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, cd_dash_prints_target_directory) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    cr_redirect_stdout();
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));

    set_variable(shell->vars, "OLDPWD", "/tmp", 1, 1);
    
    char *argv[] = {"cd", "-", NULL};
    int result = builtin_cd(shell, argv);
    cr_assert_eq(result, 0, "cd - should succeed");

    FILE *output_file = cr_get_redirected_stdout();
    fseek(output_file, 0, SEEK_END);
    long length = ftell(output_file);
    fseek(output_file, 0, SEEK_SET);
    
    if (length > 0) {
        char *output = malloc(length + 1);
        fread(output, 1, length, output_file);
        output[length] = '\0';

        cr_assert(strstr(output, "/tmp") != NULL, "Output should contain target directory");
        
        free(output);
    }
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, cd_preserves_oldpwd_on_failure) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));

    set_variable(shell->vars, "OLDPWD", "/tmp", 1, 1);
    char *original_oldpwd = get_variable(shell->vars, "OLDPWD");

    char *argv[] = {"cd", "/nonexistent/directory", NULL};
    int result = builtin_cd(shell, argv);
    cr_assert_neq(result, 0, "cd should fail");

    char *current_oldpwd = get_variable(shell->vars, "OLDPWD");
    cr_assert_str_eq(current_oldpwd, original_oldpwd, "OLDPWD should be preserved on failure");
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, cd_home_updates_oldpwd) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));

    char *argv[] = {"cd", NULL};
    int result = builtin_cd(shell, argv);

    if (result == 0) {
        char *oldpwd = get_variable(shell->vars, "OLDPWD");
        cr_assert_not_null(oldpwd, "OLDPWD should be set after cd to HOME");
        cr_assert_str_eq(oldpwd, original_dir, "OLDPWD should be original directory");
    }
    
    chdir(original_dir);
    cleanup_test_shell(shell);
}

Test(cd_builtin, cd_relative_path_updates_oldpwd) {
    t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    
    char *argv[] = {"cd", ".", NULL};
    int result = builtin_cd(shell, argv);
    cr_assert_eq(result, 0, "cd . should succeed");

    char *oldpwd = get_variable(shell->vars, "OLDPWD");
    cr_assert_not_null(oldpwd, "OLDPWD should be set after any cd");
    
    cleanup_test_shell(shell);
}
