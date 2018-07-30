#!/bin/bash

set -e

bazel test --test_output=errors //test/...

./test/assembler/integration/cli.sh
./test/anvm/integration/cli.sh
# TODO: fuzz test assembler
# TODO: cycle read/write assembly and compare
