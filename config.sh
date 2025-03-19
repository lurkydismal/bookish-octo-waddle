#!/bin/bash
declare -a partsToBuild=(
    "main"
    "asset_t"
    "log"
    "stdfunc"
    "glad"
)
declare -a testsToBuild=(
    "stdfunc"
)

export testsMainPackage='test'
