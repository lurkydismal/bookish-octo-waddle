#!/bin/bash
declare -a partsToBuild=(
    "gameState_t"
    "settings_t"
    "window_t"
    "vsync"
    "FPS"
    "asset_t"
    "log"
    "stdfunc"
    "glad"
)
declare -a testsToBuild=(
    "stdfunc"
)

export executableMainPackage='main'
export testsMainPackage='test'
