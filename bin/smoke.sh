#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${CETUS_BUILD_DIR:-${ROOT_DIR}/cmake-build-debug}"
CETUS_BIN="${BUILD_DIR}/cetus"
SMOKE_OUT_DIR="${BUILD_DIR}/smoke"
SMOKE_STDOUT="${SMOKE_OUT_DIR}/stdout.txt"
SMOKE_STDERR="${SMOKE_OUT_DIR}/stderr.txt"

cmake --build "${BUILD_DIR}"
mkdir -p "${SMOKE_OUT_DIR}"

normalize_stdout() {
    sed -E 's/\[ <fn.*$//' "${SMOKE_STDOUT}" \
        | grep -v -E '^(--- Trace Execution ---|[0-9]{4} OP_|$)' || true
}

run_positive() {
    local file="$1"
    echo "[smoke] positive: ${file#${ROOT_DIR}/}"
    "${CETUS_BIN}" --run-expr "$file" >"${SMOKE_STDOUT}" 2>"${SMOKE_STDERR}"
}

run_positive_output() {
    local file="$1"
    local expected="$2"
    run_positive "$file"

    local actual
    actual="$(normalize_stdout)"
    if [[ "${actual}" != "${expected}" ]]; then
        echo "Unexpected stdout for: $file" >&2
        echo "expected:" >&2
        printf '%s\n' "${expected}" >&2
        echo "actual:" >&2
        printf '%s\n' "${actual}" >&2
        echo "stderr:" >&2
        cat "${SMOKE_STDERR}" >&2
        exit 1
    fi
}

run_positive_output_stdin() {
    local file="$1"
    local input="$2"
    local expected="$3"
    echo "[smoke] positive: ${file#${ROOT_DIR}/}"
    printf '%s' "${input}" | "${CETUS_BIN}" --run-expr "$file" >"${SMOKE_STDOUT}" 2>"${SMOKE_STDERR}"

    local actual
    actual="$(normalize_stdout)"
    if [[ "${actual}" != "${expected}" ]]; then
        echo "Unexpected stdout for: $file" >&2
        echo "expected:" >&2
        printf '%s\n' "${expected}" >&2
        echo "actual:" >&2
        printf '%s\n' "${actual}" >&2
        echo "stderr:" >&2
        cat "${SMOKE_STDERR}" >&2
        exit 1
    fi
}

run_negative() {
    local file="$1"
    local expected="$2"
    echo "[smoke] negative: ${file#${ROOT_DIR}/}"
    if "${CETUS_BIN}" --run-expr "$file" >"${SMOKE_STDOUT}" 2>"${SMOKE_STDERR}"; then
        echo "Expected failure, but command succeeded: $file" >&2
        cat "${SMOKE_STDOUT}" >&2
        cat "${SMOKE_STDERR}" >&2
        exit 1
    fi

    if ! grep -q "$expected" "${SMOKE_STDERR}" "${SMOKE_STDOUT}"; then
        echo "Expected diagnostic not found: $expected" >&2
        echo "stdout:" >&2
        cat "${SMOKE_STDOUT}" >&2
        echo "stderr:" >&2
        cat "${SMOKE_STDERR}" >&2
        exit 1
    fi
}

run_typecheck_positive() {
    local file="$1"
    echo "[smoke] typecheck positive: ${file#${ROOT_DIR}/}"
    "${CETUS_BIN}" --typecheck "$file" >"${SMOKE_STDOUT}" 2>"${SMOKE_STDERR}"
}

run_parse_positive() {
    local file="$1"
    echo "[smoke] parse positive: ${file#${ROOT_DIR}/}"
    "${CETUS_BIN}" --parse-ast "$file" >"${SMOKE_STDOUT}" 2>"${SMOKE_STDERR}"
}

run_typecheck_negative() {
    local file="$1"
    local expected="$2"
    echo "[smoke] typecheck negative: ${file#${ROOT_DIR}/}"
    if "${CETUS_BIN}" --typecheck "$file" >"${SMOKE_STDOUT}" 2>"${SMOKE_STDERR}"; then
        echo "Expected typecheck failure, but command succeeded: $file" >&2
        cat "${SMOKE_STDOUT}" >&2
        cat "${SMOKE_STDERR}" >&2
        exit 1
    fi

    if ! grep -q "$expected" "${SMOKE_STDERR}" "${SMOKE_STDOUT}"; then
        echo "Expected diagnostic not found: $expected" >&2
        echo "stdout:" >&2
        cat "${SMOKE_STDOUT}" >&2
        echo "stderr:" >&2
        cat "${SMOKE_STDERR}" >&2
        exit 1
    fi
}

for file in "${ROOT_DIR}"/tests/smoke/positive/*.cetus; do
    run_positive "$file"
done

run_negative "${ROOT_DIR}/tests/smoke/negative/missing_main.cetus" "Program entry point main is not declared"
run_negative "${ROOT_DIR}/tests/smoke/negative/duplicate_local.cetus" "Variable is already declared in current scope: value"
run_negative "${ROOT_DIR}/tests/smoke/negative/float_modulo.cetus" "Modulo operator expects int operands"
run_negative "${ROOT_DIR}/tests/smoke/negative/break_outside_loop.cetus" "Break statement is not allowed outside loop"
run_negative "${ROOT_DIR}/tests/smoke/negative/continue_outside_loop.cetus" "Continue statement is not allowed outside loop"
run_negative "${ROOT_DIR}/tests/smoke/negative/for_post_declaration.cetus" "For post statement cannot declare variables"
run_negative "${ROOT_DIR}/tests/smoke/negative/ref_temporary.cetus" "Pointer parameter expects address of assignable identifier argument"
run_negative "${ROOT_DIR}/tests/smoke/negative/function_arity.cetus" "Function call argument count does not match function parameters"
run_negative "${ROOT_DIR}/tests/smoke/negative/array_bounds.cetus" "VM execution failed"
run_negative "${ROOT_DIR}/tests/smoke/negative/string_index_bounds.cetus" "VM execution failed"

run_positive_output "${ROOT_DIR}/tests/smoke/positive/array_basic.cetus" $'0\n7\n3\n5\n0'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/append_basic.cetus" "[1, 2, 3]"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/bubble_sort.cetus" $'До сортировки: [64, 34, 25, 12, 22, 11, 90]\nПосле сортировки: [11, 12, 22, 25, 34, 64, 90]'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/if_nested_empty.cetus" $'1\n2'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/index_swap.cetus" "[2, 1, 3]"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/len_slice.cetus" "4"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/increment.cetus" "3"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/decrement.cetus" "2"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/else_if.cetus" "2"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/for_condition.cetus" $'0\n1\n2'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/for_classic.cetus" $'0\n1\n2'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/for_break_continue.cetus" $'0\n2\n3'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/ref_parameter.cetus" "2"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/runtime_numbers.cetus" $'1\n1.5\n1\n3\n3.5'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/semicolon_insertion.cetus" "3"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/slice_expression.cetus" $'[1, 9, 3, 4]\n[1, 9]\n[3, 4]\n[1, 9, 3, 4]'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/slice_literal.cetus" $'3\n34\n3'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/slice_parameter.cetus" "7"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/sieve_of_eratosthenes.cetus" $'2\n3\n5\n7'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/string_index.cetus" $'a\nc\ntrue'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/string_literal.cetus" "hello"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/string_operations.cetus" $'abc\ntrue\ntrue\n3'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/palindrome.cetus" "true"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/variadic_println_array.cetus" $'Before: [64, 34, 25]\n[[1, 2], [3, 4]]'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/var_typed_composite.cetus" $'[4, 9, 1, 7, 3]\n3'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/method_basic.cetus" "5"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/method_ref_receiver.cetus" "5"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/struct_basic.cetus" "1"
run_positive_output "${ROOT_DIR}/tests/programs/max.cetus" "9"
run_positive_output "${ROOT_DIR}/tests/programs/linear_search.cetus" "2"
run_positive_output "${ROOT_DIR}/tests/programs/binary_search.cetus" "4"
run_positive_output "${ROOT_DIR}/tests/programs/fizz_buzz.cetus" $'1\n2\nFizz\n4\nBuzz\nFizz\n7\n8\nFizz\nBuzz\n11\nFizz\n13\n14\nFizzBuzz\n16\n17\nFizz\n19\nBuzz\nFizz\n22\n23\nFizz\nBuzz\n26\nFizz\n28\n29\nFizzBuzz'
run_positive_output "${ROOT_DIR}/tests/programs/selection_sort.cetus" "[11, 12, 22, 25, 64]"
run_positive_output "${ROOT_DIR}/tests/programs/insertion_sort.cetus" "[1, 2, 3, 4, 5, 6]"
run_positive_output "${ROOT_DIR}/tests/programs/valid_brackets.cetus" "true"
run_positive_output_stdin "${ROOT_DIR}/tests/smoke/positive/scan_input.cetus" $'cetus\n42\n2.5\ntrue\n' $'cetus\n42\n2.5\ntrue'
run_typecheck_positive "${ROOT_DIR}/tests/smoke/typecheck/array_type.cetus"
run_typecheck_positive "${ROOT_DIR}/tests/smoke/typecheck/slice_type.cetus"
run_parse_positive "${ROOT_DIR}/tests/smoke/typecheck/struct_declaration.cetus"
run_typecheck_positive "${ROOT_DIR}/tests/smoke/typecheck/struct_type_symbol.cetus"
run_typecheck_positive "${ROOT_DIR}/tests/smoke/typecheck/struct_literal_type.cetus"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/array_index_non_int.cetus" "Index must have int type"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/array_assignment_type.cetus" "Cannot assign value of different type to assignment target"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/array_literal_type_mismatch.cetus" "Array literal element type does not match literal element type"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/array_literal_length_mismatch.cetus" "Array literal element count does not match array length"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/increment_non_int.cetus" "Increment target must have int type"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/decrement_non_int.cetus" "Decrement target must have int type"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/string_index_non_int.cetus" "Index must have int type"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/string_index_assignment.cetus" "Cannot assign to string index"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/struct_duplicate_field.cetus" "Struct field is already declared"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/struct_unknown_type.cetus" "Unknown type in variable declaration"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/struct_unknown_field.cetus" "Struct field is not declared"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/struct_literal_unknown_field.cetus" "Struct literal field is not declared"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/struct_literal_duplicate_field.cetus" "Struct literal field is initialized more than once"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/struct_literal_type_mismatch.cetus" "Struct literal field initializer type does not match declared field type"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/method_unknown.cetus" "Method is not declared"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/method_duplicate.cetus" "Method is already declared"
run_typecheck_negative "${ROOT_DIR}/tests/smoke/typecheck/method_ref_receiver_non_identifier.cetus" "Pointer method receiver expects assignable identifier receiver"

echo "[smoke] ok"
