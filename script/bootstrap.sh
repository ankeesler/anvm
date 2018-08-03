#!/bin/bash

set -ex

scriptname="bazel-0.16.0-installer-linux-x86_64.sh"
script="https://github.com/bazelbuild/bazel/releases/download/0.16.0/$scriptname"
sha="https://github.com/bazelbuild/bazel/releases/download/0.16.0/$scriptname.sha256"

shaoutput=`curl -L $sha`
if [[ "$(echo $shaoutput | awk '{print $2}')" != "$scriptname" ]]; then
  echo "sha ($sha) does not refer to script ($scriptname)"
  exit 1
fi

curl -L $script -o $scriptname
if [[ "$(echo $shaoutput | awk '{print $1}')" != "$(shasum -a 256 $scriptname | awk '{print $1}')" ]]; then
  echo "sha does not match sha of script"
  exit 1
fi

