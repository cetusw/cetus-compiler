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
    actual="$(grep -v -E '^(--- Trace Execution ---|\[|[0-9]{4} OP_|$)' "${SMOKE_STDOUT}" || true)"
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

for file in "${ROOT_DIR}"/tests/smoke/positive/*.cetus; do
    run_positive "$file"
done

run_negative "${ROOT_DIR}/tests/smoke/negative/missing_main.cetus" "Program entry point main is not declared"
run_negative "${ROOT_DIR}/tests/smoke/negative/duplicate_local.cetus" "Variable is already declared in current scope: value"
run_negative "${ROOT_DIR}/tests/smoke/negative/float_modulo.cetus" "Modulo operator expects int operands"

run_positive_output "${ROOT_DIR}/tests/smoke/positive/runtime_numbers.cetus" $'1\n1.5\n1\n3\n3.5'
run_positive_output "${ROOT_DIR}/tests/smoke/positive/string_literal.cetus" "hello"
run_positive_output "${ROOT_DIR}/tests/smoke/positive/string_operations.cetus" $'abc\ntrue\ntrue\n3'

echo "[smoke] ok"
