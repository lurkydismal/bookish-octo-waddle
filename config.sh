#!/bin/bash
declare -a partsToBuild=(
    "main"
    "settings_t"
    "asset_t"
    "log"
    "stdfunc"
    "glad"
)
declare -a testsToBuild=(
    "stdfunc"
)

export testsMainPackage='test'
