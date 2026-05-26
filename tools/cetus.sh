#!/usr/bin/env bash

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/cmake-build-debug}"
CETUS_BIN="${CETUS_BIN:-$BUILD_DIR/cetus}"

INFO=false
INPUT_FILE="$ROOT_DIR/test.txt"
for argument in "$@"; do
    if [[ "$argument" == "--info" ]]; then
        INFO=true
    else
        INPUT_FILE="$argument"
    fi
done

INPUT_BASENAME="$(basename "$INPUT_FILE")"
OUTPUT_BASENAME="${INPUT_BASENAME%.*}"
ASM_FILE="./$OUTPUT_BASENAME.asm"
EXECUTABLE_FILE="./$OUTPUT_BASENAME"

if [[ ! -x "$CETUS_BIN" ]]; then
    echo "cetus binary was not found: $CETUS_BIN"
    exit 1
fi

if [[ ! -f "$INPUT_FILE" ]]; then
    echo "Input source file was not found: $INPUT_FILE"
    exit 1
fi

if [[ "$INFO" == true ]]; then
    echo "== Source =="
    cat "$INPUT_FILE"
    echo

    echo "== AST =="
    "$CETUS_BIN" --parse-ast "$INPUT_FILE"
    echo

    echo "== Assembly generation =="
fi

"$CETUS_BIN" --asm "$INPUT_FILE" "$ASM_FILE"

if [[ "$INFO" == true ]]; then
    echo "Generated: $ASM_FILE"
    echo

    echo "== Assembly =="
    cat "$ASM_FILE"
    echo

    echo "== GCC =="
fi

gcc -no-pie -x assembler "$ASM_FILE" -o "$EXECUTABLE_FILE"

if [[ "$INFO" == true ]]; then
    echo "Executable: $EXECUTABLE_FILE"
    echo

    echo "== Program output =="
fi

"$EXECUTABLE_FILE"
