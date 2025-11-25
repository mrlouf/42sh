#!/usr/bin/env bash
# tests/builtins.sh
# Unit tests for 42sh builtin commands

MAXISHELL='./42sh'
TOTAL_TESTS=0
PASSED_TESTS=0

# run_pty: run $MAXISHELL under a pty and feed commands; capture output and exit status
run_pty() {
    local cmds="$1"
    LAST_OUT=$(script -q /dev/null -c "$MAXISHELL" <<EOF
$cmds
EOF
)
    LAST_RET=$?
    # Remove carriage returns for consistent output
    LAST_OUT=$(printf "%s" "$LAST_OUT" | tr -d '\r')
}

report_ok() {
    echo "✅ Test $1: PASSED"
    PASSED_TESTS=$((PASSED_TESTS + 1))
}

report_fail() {
    echo "❌ Test $1: FAILED - $2"
    echo "   Expected: $3"
    echo "   Got: $4"
}

echo "=== 42sh Builtin Commands Test Suite ==="
echo

# =====================================
# EXIT BUILTIN TESTS
# =====================================
echo "--- EXIT BUILTIN TESTS ---"

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: exit with no argument"
run_pty $'exit\n'
if [ $LAST_RET -eq 0 ]; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "exit should return code 0" "0" "$LAST_RET"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: exit prints 'exit'"
run_pty $'exit\n'
if printf "%s" "$LAST_OUT" | grep -q "exit"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "exit should print 'exit'" "exit" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: exit with numeric argument"
run_pty $'exit 42\n'
if [ $LAST_RET -eq 42 ]; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "exit 42 should return code 42" "42" "$LAST_RET"
fi

# =====================================
# ENV BUILTIN TESTS
# =====================================
echo
echo "--- ENV BUILTIN TESTS ---"

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: env shows PATH variable"
run_pty $'env\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "PATH="; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "env should show PATH variable" "PATH=" "missing"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: env shows HOME variable"
run_pty $'env\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "HOME="; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "env should show HOME variable" "HOME=" "missing"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: env shows USER variable"
run_pty $'env\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "USER="; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "env should show USER variable" "USER=" "missing"
fi

# =====================================
# CD BUILTIN TESTS  
# =====================================
echo
echo "--- CD BUILTIN TESTS ---"

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: cd to /tmp"
run_pty $'cd /tmp\npwd\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "/tmp"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "cd /tmp should change to /tmp" "/tmp" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: cd with no arguments (go to HOME)"
run_pty $'cd\npwd\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "$HOME"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "cd should go to HOME directory" "$HOME" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: cd to invalid directory"
run_pty $'cd /nonexistent/directory\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "No such file or directory"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "cd to invalid dir should show error" "No such file or directory" "$LAST_OUT"
fi

# =====================================
# ECHO BUILTIN TESTS
# =====================================
echo
echo "--- ECHO BUILTIN TESTS ---"

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: echo simple text"
run_pty $'echo hello world\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "hello world"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "echo should output text" "hello world" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: echo -n (no newline)"
run_pty $'echo -n hello\necho world\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "helloworld"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "echo -n should suppress newline" "helloworld" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: echo -e with tab escape"
run_pty $'echo -e "hello\\tworld"\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q $'hello\tworld'; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "echo -e should process tab escape" "hello<TAB>world" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: echo -e with newline escape"
run_pty $'echo -e "hello\\nworld"\nexit\n'
if printf "%s" "$LAST_OUT" | grep -A1 "hello" | grep -q "world"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "echo -e should process newline escape" "hello<NEWLINE>world" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: echo -E (disable escapes)"
run_pty $'echo -E "hello\\tworld"\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "hello\\\\tworld"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "echo -E should not process escapes" "hello\\tworld" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: echo -e with backslash escape"
run_pty $'echo -e "hello\\\\world"\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "hello\\\\world"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "echo -e should process backslash escape" "hello\\world" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: echo -e with \\c (stop processing)"
run_pty $'echo -e "hello\\cworld"\nexit\n'
# \c should stop processing and suppress newline
if printf "%s" "$LAST_OUT" | grep -q "hello" && ! printf "%s" "$LAST_OUT" | grep -q "world"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "echo -e with \\c should stop processing" "hello" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: echo -e with octal escape \\101 (A)"
run_pty $'echo -e "\\101"\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q "A"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "echo -e \\101 should output A" "A" "$LAST_OUT"
fi

TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: echo combined flags -ne"
run_pty $'echo -ne "hello\\tworld"\necho next\nexit\n'
if printf "%s" "$LAST_OUT" | grep -q $'hello\tworld' && printf "%s" "$LAST_OUT" | grep -q "worldnext"; then
    report_ok $TOTAL_TESTS
else
    report_fail $TOTAL_TESTS "echo -ne should process escapes and suppress newline" "hello<TAB>worldnext" "$LAST_OUT"
fi

# =====================================
# SUMMARY
# =====================================
echo
echo "=== TEST SUMMARY ==="
echo "Total tests: $TOTAL_TESTS"
echo "Passed: $PASSED_TESTS"
echo "Failed: $((TOTAL_TESTS - PASSED_TESTS))"

if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo "🎉 All tests passed!"
    exit 0
else
    echo "💥 Some tests failed!"
    exit 1
fi