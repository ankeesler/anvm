#!/bin/bash

log() {
    echo "[`basename $0`] $1"
}

expect_success() {
    command="$@"
    log "RUNNING: $command"
    stdout=`$command`
    status="$?"
    if [[ $status -ne 0 ]]; then
        log "ERROR: Expected success, but got $status"
        log "STDOUT:"
        echo "$stdout"
        exit 1
    fi
    log "PASS"
}

expect_file() {
    file="$1"
    log "RUNNING: -f $file"
    if [[ ! -f $file ]]; then
        log "ERROR: Expected file $file to exist"
        exit 1
    fi
    log "PASS"
    rm $file
}

expect_no_file() {
    file="$1"
    log "RUNNING: ! -f $file"
    if [[ -f $file ]]; then
        log "ERROR: Expected file $file to not exist"
        exit 1
    fi
    log "PASS"
}

expect_failure() {
    command="$@"
    log "RUNNING: $command"
    stdout=`$command`
    status="$?"
    if [[ $status -ne 1 ]]; then
        log "ERROR: Expected failure, but got $status"
        log "STDOUT:"
        echo "$stdout"
        exit 1
    fi
    log "PASS"
}

bazel build //src/anvm:anvm
if [[ $? -ne 0 ]]; then
    log "ERROR: Failed to build anvm"
    exit 1
fi
anvm="bazel-bin/src/anvm/anvm"

# BASIC FAILURE TESTS
#expect_failure ./$anvm test/assembler/integration/fixtures/bad.asm
#expect_failure ./$anvm --debug --output foo.asm
expect_failure ./$anvm

# RUN SUCCESS TESTS
expect_success ./$anvm run test/anvm/integration/fixtures/basic.out
expect_no_file a.cpu
expect_success ./$anvm run test/anvm/integration/fixtures/basic.out -o cpu.out
#expect_file cpu.out
#expect_success ./$anvm read-mem -i cpu.out

# RUN FAILURE TESTS
expect_failure ./$anvm run
