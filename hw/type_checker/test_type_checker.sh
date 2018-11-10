#!/usr/bin/env bash

if [[ $# -ne 3 ]] ; then
    echo "Correct command \"test_type_checker.sh <BinFile> <TestCsvFile> <SamplesFolder>\""
    exit 1
fi

BIN=$1
ALL_TESTS=$2
SAMPLES_FOLDER=$3

PASSING_CNT=0
TOTAL_TESTS=0

RED='\033[0;31m'
GREEN='\033[1;32m'
NOCOLOR='\033[0m'

test_code_file () {
    ((TOTAL_TESTS++))
    local TEST_FILE=$1

    get_exit_code $2
    local EXIT_CODE=$?

    ${BIN} ${SAMPLES_FOLDER}/${TEST_FILE} &> /dev/null
    RETURN_CODE=$?
    if [[ ${RETURN_CODE} = ${EXIT_CODE} ]]; then
        ((PASSING_CNT++))
        printf "Test #${TOTAL_TESTS}: ${TEST_FILE} passed with return code ${RETURN_CODE}\n"
    else

        printf "Test #${TOTAL_TESTS}: ${TEST_FILE} ${RED}FAILED${NOCOLOR} with return code ${RETURN_CODE}\n"
    fi
}

get_exit_code() {
    FAILURE_MODE=$1
    case "${FAILURE_MODE}" in
    PASS)
        return 0
        ;;
    SCANNER)
        return 4
        ;;
    PARSER)
        return 8
        ;;
    CLASS_HIERARCHY)
        return 16
        ;;
    INIT_BEFORE_USE)
        return 32
        ;;
    TYPE_INF)
        return 64
        ;;
    *)
        echo "Unknown failure mode \"${FAILURE_MODE}\""
        ;;
    esac
}

for TEST in $( cat ${ALL_TESTS} ) ; do
    IFS="," read TEST_FILE EXIT_TYPE <<< "${TEST}"
    test_code_file ${TEST_FILE} ${EXIT_TYPE}
done


if [[ ${TOTAL_TESTS} = ${PASSING_CNT} ]] ; then
    printf "${GREEN}All ${TOTAL_TESTS} tests passed.${NOCOLOR}\n"
else
    NUM_FAIL=$((TOTAL_TESTS - PASSING_CNT))
    printf "${RED}${NUM_FAIL} of ${TOTAL_TESTS} test failed.${NOCOLOR}\n"
fi
