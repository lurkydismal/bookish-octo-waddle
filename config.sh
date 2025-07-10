#!/bin/bash
declare -a partsToBuild=(
    "runtime"
    "FPS"
    "settings_t"
    "window_t"
    "vsync"
    "asset_t"
    "watch_t"
    "log"
    "stdfunc"
)
declare -a testsToBuild=(
    # "runtime"
    "FPS"
    "settings_t"
    "window_t"
    "vsync"
    "asset_t"
    # "watch_t"
    "log"
    "stdfunc"
)
declare -a staticParts=(
    "inih"
    "plthook"
)
declare -a rootSharedObjectName="root"

export executableMainPackage='main'
export testsMainPackage='test'
