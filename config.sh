#!/bin/bash
declare -a partsToBuild=(
    "main"
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

export testsMainPackage='test'
