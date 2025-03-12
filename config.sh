#!/bin/bash
declare -a partsToBuild=(
    "main"
    "glad"
    "stdfunc"
)
declare -a testsToBuild=(
    "stdfunc"
)

export testsMainPackage='test'
