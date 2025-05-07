#!/bin/bash
declare -a partsToBuild=(
    "gameState_t"
    "FPS"
    "stdogl"
    "GLTF_t"
    "settings_t"
    "window_t"
    "vsync"
    "texture_t"
    "font_t"
    "image_t"
    "asset_t"
    "log"
    "stdfunc"
)
declare -a testsToBuild=(
    "stdfunc"
)
declare -a staticParts=(
    "glad"
    "yyjson"
)

export executableMainPackage='main'
export testsMainPackage='test'
