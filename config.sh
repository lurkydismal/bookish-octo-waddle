#!/bin/bash
declare -a partsToBuild=(
    "gameState_t"
    "image_t"
    "font_t"
    "settings_t"
    "window_t"
    "vsync"
    "FPS"
    "asset_t"
    "log"
    "stdfunc"
)
declare -a testsToBuild=(
    "stdfunc"
)
declare -a staticParts=(
    "glad"
)

export executableMainPackage='main'
export testsMainPackage='test'
