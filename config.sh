#!/bin/bash
declare -a partsToBuild=(
    "main"
    "asset_t"
    "log"
    "stdfunc"
    "glad"
)
declare -a testsToBuild=(
    "asset_t"
    "log"
    "stdfunc"
)

export testsMainPackage='test'
