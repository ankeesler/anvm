#!/bin/bash

log() {
    echo "[`basename $0`] $1"
}

cleanup() {
    rm -f a.out
}

fail() {
    cleanup
    exit 1
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
        fail
    fi
    log "PASS"
}

expect_file() {
    file="$1"
    log "RUNNING: -f $file"
    if [[ ! -f $file ]]; then
        log "ERROR: Expected file $file to exist"
        fail
    fi
    log "PASS"
    rm $file
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
        fail
    fi
    log "PASS"
}

bazel build //src/assembler:anasm
if [[ $? -ne 0 ]]; then
    log "ERROR: Failed to build anasm"
    fail
fi
anasm="bazel-bin/src/assembler/anasm"

expect_success ./$anasm test/assembler/integration/fixtures/basic.asm
expect_file a.out

expect_success ./$anasm test/assembler/integration/fixtures/basic.asm --output custom-file.out
expect_file custom-file.out

expect_failure ./$anasm test/assembler/integration/fixtures/bad.asm
expect_failure ./$anasm
expect_failure ./$anasm --debug --output foo.asm

cleanup
