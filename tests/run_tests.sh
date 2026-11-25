#!/usr/bin/env bash

passed=0
failed=0

run_test()
{
    local file="$1"
    local expected="$2"
    local actual

    ./atomc "$file" > /dev/null 2>&1
    actual=$?

    if [[ "$actual" -eq "$expected" ]]; then
        echo "[PASS] $file"
        ((passed++))
    else
        echo "[FAIL] $file (expected $expected, got $actual)"
        ((failed++))
    fi
}

# Valid syntax
run_test tests/basic.atom 0
run_test tests/lexer.atom 0
run_test tests/parser.atom 0
run_test tests/primary.atom 0
run_test tests/unary.atom 0
run_test tests/binary.atom 0
run_test tests/variables.atom 0
run_test tests/control_flow.atom 0
run_test tests/precedence.atom 0
run_test tests/expressions.atom 0
run_test tests/empty.atom 0
run_test tests/semantic_invalid.atom 0

# Invalid syntax
run_test tests/invalid.atom 1
run_test tests/invalid_expr.atom 1
run_test tests/invalid_unary.atom 1
run_test tests/invalid_binary.atom 1
run_test tests/invalid_variable.atom 1
run_test tests/invalid_control_flow.atom 1
run_test tests/missing_semicolon.atom 1
run_test tests/missing_brace.atom 1
run_test tests/invalid_type.atom 1
run_test tests/invalid_arguments.atom 1

echo
echo "Passed: $passed"
echo "Failed: $failed"

if [[ "$failed" -ne 0 ]]; then
    exit 1
fi

exit 0