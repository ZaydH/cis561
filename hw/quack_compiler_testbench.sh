#!/usr/bin/env bash
# Quack Compiler Testbench
#
# Written by: Zayd Hammoudeh
# (zhammoud@uoregon.edu)
#
# Test bench for the Quack compiler although it should be portable to other languages as well.
# It reads an input CSV formatted as rows in the form "<test_file>,<exit_code>", where <exit_code>
# is the expected return code of the compiler on different inputs.  Each stage in the compilation
# process (e.g, lexer, parser, well-formed class hierarchy, initialized before use, and type
# inference) each has exit code in the compiler as defined in the function "get_exit_code".  If your
# program does not use that convention, you can change that function as needed.


if [[ $# -ne 4 ]] ; then
    echo "Correct command \"test_type_checker.sh <BinFile> <TestCsvFile> <SamplesFolder> <ExpectedOutFolder>\""
    exit 1
fi

BIN=$1
ALL_TESTS=$2
SAMPLES_FOLDER=$3
EXPECTED_OUT_FOLDER=$4

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

    BASE_FILENAME=$( echo "${TEST_FILE}" | rev | cut -d '.' -f 2- | rev )
    COMPILED_C_FILE="${SAMPLES_FOLDER}/${BASE_FILENAME}.c"
    rm ${COMPILED_C_FILE} &> /dev/null    
    
    ${BIN} ${SAMPLES_FOLDER}/${TEST_FILE} &> /dev/null
    local RETURN_CODE=$?
    if [[ ${RETURN_CODE} == ${TEST_PASSED} ]]; then
        COMPILE_PASSED=true
    else
        COMPILE_PASSED=false
    fi
    #echo $COMPILE_PASSED
    
    printf "Test #${TOTAL_TESTS}: ${TEST_FILE} "
    if [[ ${RETURN_CODE} = ${EXIT_CODE} ]]; then
        
        if ${COMPILE_PASSED}; then
            COMPILED_PROG=${SAMPLES_FOLDER}/a.out
            rm -rf a.out ${COMPILED_PROG} &> /dev/null      
            gcc ${COMPILED_C_FILE} ${SAMPLES_FOLDER}/builtins.c -o ${COMPILED_PROG} &> /dev/null
            if [[ $? -ne 0 ]]; then
                printf "generated output ${RED}does not compile${NOCOLOR}.\n"
                return;
            fi

            local PROG_OUT=${SAMPLES_FOLDER}/prog_out
            rm -rf ${PROG_OUT} &> /dev/null
            ${COMPILED_PROG} > ${PROG_OUT}

            if [[ $? -ne 0 ]]; then
                printf "generated code ${RED}exited with an error${NOCOLOR}.\n"
                return;
            fi

            EXPECTED_FILE="${EXPECTED_OUT_FOLDER}/${BASE_FILENAME}.txt"

            if ! [[ -f ${EXPECTED_FILE} ]]; then
                printf "expected file \"${EXPECTED_FILE}\" ${RED}does not exist${NOCOLOR}.\n"
                return
            fi

            DIFF_OUT=$( diff -w ${PROG_OUT} ${EXPECTED_FILE} ) 
            #echo "${DIFF_OUT}"
        fi
        if ! ${COMPILE_PASSED} || [[ -z ${DIFF_OUT} ]]; then
            ((PASSING_CNT++))
            printf "${GREEN}passed${NOCOLOR} with return code ${RETURN_CODE}\n"
        else
            cat ${PROG_OUT}
            printf "compiled but output ${RED}does not match${NOCOLOR} expected output.\n"
            echo ${DIFF_OUT}
        fi
    else
        printf "Test #${TOTAL_TESTS}: ${TEST_FILE} ${RED}FAILED${NOCOLOR} with return code ${RETURN_CODE}\n"
        # Rerun the command so the error message is visible.  Can comment out.
        ${BIN} ${SAMPLES_FOLDER}/${TEST_FILE}
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

# Exit code if the compiler passes
get_exit_code "PASS"
TEST_PASSED=$?

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
