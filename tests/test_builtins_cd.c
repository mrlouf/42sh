#include <criterion/criterion.h>
#include <unistd.h>
#include <stdlib.h>
#include "../incs/42sh.h"
#include "../incs/builtins.h"

TestSuite(cd_builtin);

Test(cd_builtin, cd_to_valid_directory) {
	char original_dir[1024];
	
	getcwd(original_dir, sizeof(original_dir));
	
	int result = chdir("/tmp");
	
	cr_assert_eq(result, 0, "cd /tmp should succeed");
	
	chdir(original_dir);
}

Test(cd_builtin, cd_to_invalid_directory) {
	int result = chdir("/nonexistent/directory");
	
	cr_assert_eq(result, -1, "cd to invalid directory should fail");
}
