#!/bin/bash

set -ex

bazel build //src/assembler:anasm
./bazel-bin/src/assembler/anasm -o test/anvm/integration/fixtures/basic.out test/assembler/integration/fixtures/basic.asm
