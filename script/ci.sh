#!/bin/bash

docker run --rm -it -v $PWD:/etc/anvm ubuntu "bash -c 'cd /etc/anvm; ./script/bootstrap.sh && exec bash'"
