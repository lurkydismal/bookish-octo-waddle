#!/bin/bash
export SCAN_BUILD=
export BUILD_TYPE=3
./build.sh 2>&1 |\
    ack -i 'scan-view' |\
    sed -n "s/^.*\(\/tmp\/scan-build[^']*\)'.*/scan-view \1/p"
