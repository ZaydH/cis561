#!/usr/bin/env bash

FILE_ROOT=$1
BIN=code_generator/bin/code_generator
SAMPLES_DIR=samples
EXPECTED_DIR=$SAMPLES_DIR/expected

sed -i '' "s/^${FILE_ROOT}.qk/good_${FILE_ROOT}.qk/" $SAMPLES_DIR/all_tests.csv
git mv ${SAMPLES_DIR}/${FILE_ROOT}.qk ${SAMPLES_DIR}/good_${FILE_ROOT}.qk

./export_expected.sh $BIN $SAMPLES_DIR/good_${FILE_ROOT}.qk ${EXPECTED_DIR}


