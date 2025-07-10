#!/bin/bash
export SCAN_BUILD=
export ENABLE_HOT_RELOAD=
./build.sh 2>&1 |\
    ack -i 'scan-view' |\
    sed -n "s/^.*\(\/tmp\/scan-build[^']*\)'.*/scan-view \1/p"
