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

expect_success ./$anvm run test/anvm/integration/fixtures/basic.out
expect_no_file a.cpu

#expect_success ./$anvm test/assembler/integration/fixtures/basic.asm --output custom-file.out
#expect_file custom-file.out
#
#expect_failure ./$anvm test/assembler/integration/fixtures/bad.asm
expect_failure ./$anvm
expect_failure ./$anvm run
#expect_failure ./$anvm --debug --output foo.asm
expect_failure ./$anvm run read-mem
