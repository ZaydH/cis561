#!/usr/bin/env bash

FILE_ROOT=$1
BIN=code_generator/bin/code_generator
SAMPLES_DIR=demo
EXPECTED_DIR=$SAMPLES_DIR/expected
TESTS_FILE=${SAMPLES_DIR}/all_tests.csv

if [[ $# -ne 1 ]] ; then
    echo "Correct command \"add_new_quack_file.sh <GoodFileSuffix>\" where in directory \"${SAMPLES_DIR}\" there should be a file \"good_<GoodFileSuffix>.qk\""
    exit 1
fi

echo "good_${FILE_ROOT}.qk,PASS" >> ${TESTS_FILE}
git add ${SAMPLES_DIR}/good_${FILE_ROOT}.qk

./export_expected.sh $BIN $SAMPLES_DIR/good_${FILE_ROOT}.qk ${EXPECTED_DIR}
git add ${EXPECTED_DIR}/good_${FILE_ROOT}.txt

cat ${TESTS_FILE} | sort | uniq | sponge ${TESTS_FILE}

