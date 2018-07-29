#!/bin/bash

set -e

bazel test //test/...

./test/assembler/integration/cli.sh
# TODO: fuzz test assembler
# TODO: cycle read/write assembly and compare
