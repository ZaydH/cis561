#!/usr/bin/env bash

FOLDER_NAME=testbench
SAMPLES_FOLDER=samples
SRC_FOLDER=type_checker

TEST_LIST_FILE=all_tests.csv
TESTBENCH_FILE=quack_compiler_testbench.sh
ZIP_NAME=quack_testbench.zip

rm -rf $FOLDER_NAME &> /dev/null
mkdir $FOLDER_NAME

# Copy the test files and move the test list to the folder root
cp -r $SAMPLES_FOLDER $FOLDER_NAME
mv $FOLDER_NAME/$SAMPLES_FOLDER/$TEST_LIST_FILE  $FOLDER_NAME

# Copy the testbench
cp $SRC_FOLDER/$TESTBENCH_FILE $FOLDER_NAME

zip -r $ZIP_NAME $FOLDER_NAME &> /dev/null

