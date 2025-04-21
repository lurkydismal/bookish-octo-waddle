#!/bin/bash
TARGET_DIRECTORY=$1

INCLUDE_PATHS=$(echo $FILES_TO_INCLUDE | sed "s~[^ ]*~-I $TARGET_DIRECTORY/&~g")
COMPILE_PATHS=$(echo $FILES_TO_COMPILE | sed "s~[^ ]*~$TARGET_DIRECTORY/&~g")

source "$SCRIPT_DIRECTORY/config.sh" && \
    make clean && \
    make -j $(( `nproc` - 1 )) \
        "C_FLAGS=$2" \
        "DEFINES=$3" \
        "INCLUDES=$4" \
        "FILES_TO_INCLUDE=$INCLUDE_PATHS" \
        "FILES_TO_COMPILE=$COMPILE_PATHS" \
        "C_COMPILER=$C_COMPILER" \
    && \
    mv "$OUTPUT_FILE" "$BUILD_DIRECTORY" && \
    cd $TARGET_DIRECTORY && \
    clang-format --style="file:$SCRIPT_DIRECTORY/.clang-format" \
    -i \
    $(echo $FILES_TO_INCLUDE $FILES_TO_COMPILE) && \
    cd "$SCRIPT_DIRECTORY"

# clang-tidy \
    # -quiet \
    # $(echo $FILES_TO_INCLUDE $FILES_TO_COMPILE) \
    # -- \
    # $2 $4 $INCLUDE_PATHS $3 && \
