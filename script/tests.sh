#!/bin/bash

set -e

scons tests
./tests

scons parser_tests
./parser_tests
