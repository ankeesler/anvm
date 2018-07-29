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

bazel build //src/assembler:anasm
anasm="bazel-bin/src/assembler/anasm"

expect_success ./$anasm test/assembler/integration/fixtures/basic.asm
expect_failure ./$anasm test/assembler/integration/fixtures/bad.asm
expect_failure ./$anasm
expect_failure ./$anasm --debug --output foo.asm
