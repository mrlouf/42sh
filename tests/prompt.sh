#!/bin/bash

#############################################################################
# This script tests the basic functionalities of the almighty MAXISHELL.	#
#                                                                           #                                           
#############################################################################

BASH='/bin/bash'
MAXISHELL='./42sh'

TOTAL_TESTS=0
PASSED_TESTS=0

# Test 1: Launch the shell and exit immediately
TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: Launch and exit"
exit | $MAXISHELL
if [ $? -ne 0 ]; then
    echo "Test $TOTAL_TESTS Failed: Shell did not exit cleanly."
else
    echo "Test $TOTAL_TESTS Passed."
    PASSED_TESTS=$((PASSED_TESTS + 1))
fi

# Test 2: Check prompt display
TOTAL_TESTS=$((TOTAL_TESTS + 1))
echo "Test $TOTAL_TESTS: Check prompt display"
PROMPT_OUTPUT=$( (echo "exit") | $MAXISHELL | head -n 1 )
EXPECTED_PROMPT="42sh$ >"
if [ "$PROMPT_OUTPUT" != "$EXPECTED_PROMPT" ]; then
    echo "Test $TOTAL_TESTS Failed: Prompt mismatch."
    echo "Expected: '$EXPECTED_PROMPT', Got: '$PROMPT_OUTPUT'"
else
    echo "Test $TOTAL_TESTS Passed."
    PASSED_TESTS=$((PASSED_TESTS + 1))
fi

# Summary
echo "-----------------------------------"
echo "Total Tests: $TOTAL_TESTS"
echo "Passed Tests: $PASSED_TESTS"
echo "Failed Tests: $((TOTAL_TESTS - PASSED_TESTS))"
echo "-----------------------------------"