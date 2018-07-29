#!/bin/bash

set -e

scons tests
./tests

scons parser_tests
./parser_tests

scons writer_tests
./writer_tests

./test/assembler/integration/cli.sh
