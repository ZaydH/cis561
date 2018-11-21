#!/usr/bin/env bash

FILE_ROOT=$1
BIN=code_generator/bin/code_generator
SAMPLES_DIR=samples
EXPECTED_DIR=$SAMPLES_DIR/expected

echo "good_${FILE_ROOT}.qk,PASS" >> $SAMPLES_DIR/all_tests.csv
git add ${SAMPLES_DIR}/good_${FILE_ROOT}.qk

./export_expected.sh $BIN $SAMPLES_DIR/good_${FILE_ROOT}.qk ${EXPECTED_DIR}
git add ${EXPECTED_DIR}/good_${FILE_ROOT}.txt

