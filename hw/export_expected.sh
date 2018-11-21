#!/usr/bin/env bash

if [ $# -ne 3 ]; then
    echo "Correct command \"export_expected.sh <BinFile> <QuackFile> <ExpectedOutFolder>\""
    exit 0 
fi

BIN_FILE=$1
TEST_FILE=$2
EXPECTED_OUTPUT_FOLDER=$3

BASE_FILENAME=$( echo "${TEST_FILE}" | rev | cut -d "." -f 2- )

SAMPLES_FOLDER=$( echo "${BASE_FILENAME}" | cut -d "/" -f 2- | rev)
BUILTINS="${SAMPLES_FOLDER}/builtins.c"

printf "Delete the existing compiled C file ${COMPILED_C_FILE} (if it exists)\n"
COMPILED_C_FILE=$( echo "${BASE_FILENAME}" | rev )
COMPILED_C_FILE="${COMPILED_C_FILE}.c"
rm -rf ${COMPILED_C_FILE} &> /dev/null

BASE_FILENAME=$( echo "${BASE_FILENAME}" | cut -d "/" -f 1 | rev )

printf "Compile file $TEST_FILE to $COMPILED_C_FILE\n"
$BIN_FILE $TEST_FILE &> /dev/null

printf "Run GCC on ${COMPILED_C_FILE}\n"
COMPILED_BIN="${SAMPLES_FOLDER}/a.out"
gcc ${COMPILED_C_FILE}  ${BUILTINS} -o ${COMPILED_BIN} &> /dev/null

OUTPUT_FILE="${EXPECTED_OUTPUT_FOLDER}/${BASE_FILENAME}.txt"
rm -rf ${OUTPUT_FILE} &> /dev/null
$COMPILED_BIN > ${OUTPUT_FILE}
printf "Write output to $OUTPUT_FILE\n"

