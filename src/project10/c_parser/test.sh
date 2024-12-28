#!/bin/bash

set -xe

TOOL_PATH="../../../tools/TextComparer.sh"
TEST_PATH="provided_test"

$TOOL_PATH $TEST_PATH/ExpressionLessSquare/Main.xml $TEST_PATH/ExpressionLessSquare/MainTest.xml
$TOOL_PATH $TEST_PATH/ExpressionLessSquare/Square.xml $TEST_PATH/ExpressionLessSquare/SquareTest.xml
$TOOL_PATH $TEST_PATH/ExpressionLessSquare/SquareGame.xml $TEST_PATH/ExpressionLessSquare/SquareGameTest.xml

$TOOL_PATH $TEST_PATH/Square/Main.xml $TEST_PATH/Square/MainTest.xml
$TOOL_PATH $TEST_PATH/Square/Square.xml $TEST_PATH/Square/SquareTest.xml
$TOOL_PATH $TEST_PATH/Square/SquareGame.xml $TEST_PATH/Square/SquareGameTest.xml

$TOOL_PATH $TEST_PATH/ArrayTest/Main.xml $TEST_PATH/ArrayTest/MainTest.xml
