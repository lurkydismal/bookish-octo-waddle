#!/bin/bash
export SCRIPT_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export BUILD_DIRECTORY_NAME='out'
export BUILD_DIRECTORY="$SCRIPT_DIRECTORY/$BUILD_DIRECTORY_NAME"

# 0 - Debug
# 1 - Release
# 2 - Profile
# 3 - Tests
export BUILD_TYPE=3

export BUILD_C_FLAGS="-flto=jobserver -std=gnu99 -march=native -ffunction-sections -fdata-sections -fPIC -fopenmp-simd -fno-ident -fshort-enums -Wall -Wextra"
export BUILD_C_FLAGS_DEBUG="-Og -g"
export BUILD_C_FLAGS_RELEASE="-Ofast -funroll-loops -fno-asynchronous-unwind-tables"
export BUILD_C_FLAGS_PROFILE="$BUILD_C_FLAGS_RELEASE"

export declare BUILD_DEFINES=(
)

export declare BUILD_DEFINES_DEBUG=(
    "DEBUG"
)

export declare BUILD_DEFINES_RELEASE=(
    "RELEASE"
)

export declare BUILD_DEFINES_PROFILE=(
    "PROFILE"
)

export declare BUILD_INCLUDES=(
    "glad/include"
    "stdfunc/include"
    "main/applicationState_t/include"
)

export declare BUILD_INCLUDES_TEST=(
    "test/include"
)

export LINK_FLAGS="-flto -fPIC -fuse-ld=mold -Wl,-O1 -Wl,--gc-sections -Wl,--no-eh-frame-hdr"
export LINK_FLAGS_DEBUG="-g"
export LINK_FLAGS_RELEASE="-s"
export LINK_FLAGS_PROFILE="$LINK_FLAGS_DEBUG"

export declare LIBRARIES_TO_LINK=(
    "glfw"
    "dl"
    "mimalloc"
)
export LINKER="ccache gcc"
export EXECUTABLE_NAME="main.out"
export EXECUTABLE_NAME_TEST="$EXECUTABLE_NAME"'_test'
export declare EXECUTABLE_SECTIONS_TO_STRIP=(
    ".note.gnu.build-id"
    ".note.gnu.property"
    ".comment"
    ".eh_frame"
    ".eh_frame_hdr"
    ".relro_padding"
)

clear

source './config.sh' && {

mkdir -p "$BUILD_DIRECTORY"

if [ $BUILD_TYPE -eq 0 ]; then
    echo -e '\033[1;35m''Debug build''\033[0m'

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_DEBUG"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_DEBUG"
    BUILD_DEFINES+=( "${BUILD_DEFINES_DEBUG[@]}" )

elif [ $BUILD_TYPE -eq 1 ]; then
    echo -e '\033[1;35m''Release build''\033[0m'

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_RELEASE"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_RELEASE"
    BUILD_DEFINES+=( "${BUILD_DEFINES_RELEASE[@]}" )

elif [ $BUILD_TYPE -eq 2 ]; then
    echo -e '\033[1;35m''Profile build''\033[0m'

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_PROFILE"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_PROFILE"
    BUILD_DEFINES+=( "${BUILD_DEFINES_PROFILE[@]}" )

elif [ $BUILD_TYPE -eq 3 ]; then
    echo -e '\033[1;35m''Building tests''\033[0m'

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_DEBUG"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_DEBUG"
    BUILD_DEFINES+=( "${BUILD_DEFINES_DEBUG[@]}" )
fi

if [ ${#BUILD_DEFINES[@]} -ne 0 ]; then
    printf -v definesAsString -- "-D %s " "${BUILD_DEFINES[@]}"
    echo -e '\033[1;36m'"$definesAsString"'\033[0m'
fi

if [ ${#BUILD_INCLUDES[@]} -ne 0 ]; then
    printf -v includesAsString -- "-I $SCRIPT_DIRECTORY/%s " "${BUILD_INCLUDES[@]}"
    echo -e '\033[1;34m'"$includesAsString"'\033[0m'
fi

for partToBuild in "${partsToBuild[@]}"; do
    source "$partToBuild/config.sh" && './build_general.sh' "$partToBuild" "$BUILD_C_FLAGS" "$definesAsString" "$includesAsString"

    BUILD_STATUS=$?

    if [ $BUILD_STATUS -ne 0 ]; then
        break
    fi
done

# Build main executable
if [ $BUILD_STATUS -eq 0 ]; then
    if [ ${#partsToBuild[@]} -ne 0 ]; then
        printf -v partsToBuildAsString -- "$BUILD_DIRECTORY/lib%s.a " "${partsToBuild[@]}"
        echo -e '\033[1;34m'"$partsToBuildAsString"'\033[0m'
    fi

    if [ ${#LIBRARIES_TO_LINK[@]} -ne 0 ]; then
        printf -v librariesToLinkAgainst -- "-l%s " "${LIBRARIES_TO_LINK[@]}"
        echo  -e '\033[1;34m'"$librariesToLinkAgainst"'\033[0m'
    fi

    $LINKER $LINK_FLAGS $partsToBuildAsString $librariesToLinkAgainst -o "$BUILD_DIRECTORY/$EXECUTABLE_NAME"

    if [ ! -z "${NEED_STRIP_EXECUTABLE+x}" ]; then
        if [ ${#EXECUTABLE_SECTIONS_TO_STRIP[@]} -ne 0 ]; then
            printf -v sectionsToStripAsString -- "--remove-section %s " "${EXECUTABLE_SECTIONS_TO_STRIP[@]}"
            echo  -e '\033[1;34m'"$sectionsToStripAsString"'\033[0m'
        fi

        objcopy "$BUILD_DIRECTORY/$EXECUTABLE_NAME" $sectionsToStripAsString

        strip --strip-section-headers "$BUILD_DIRECTORY/$EXECUTABLE_NAME"
    fi
fi

# Build tests
if [ $BUILD_TYPE -eq 3 ]; then
    if [ ${#BUILD_INCLUDES[@]} -ne 0 ]; then
        printf -v testIncludesAsString -- "-I $SCRIPT_DIRECTORY/%s " "${BUILD_INCLUDES_TEST[@]}"
        echo  -e '\033[1;34m'"$testIncludesAsString"'\033[0m'
    fi

    for testToBuild in "${testsToBuild[@]}"; do
        source "tests/$testToBuild/config.sh" && './build_general.sh' "tests/$testToBuild" "$BUILD_C_FLAGS" "$definesAsString" "$includesAsString""$testIncludesAsString"

        BUILD_STATUS=$?

        if [ $BUILD_STATUS -ne 0 ]; then
            break
        fi
    done

    # Build tests main package
    if [ $BUILD_STATUS -eq 0 ]; then
        source "$testsMainPackage/config.sh" && './build_general.sh' "$testsMainPackage" "$BUILD_C_FLAGS" "$definesAsString" "$includesAsString""$testIncludesAsString"

        BUILD_STATUS=$?
    fi

    if [ $BUILD_STATUS -eq 0 ]; then
        if [ ${#testsToBuild[@]} -ne 0 ]; then
            printf -v testsToBuildAsString -- "$BUILD_DIRECTORY/lib%s_test.a " "${testsToBuild[@]}"
            echo  -e '\033[1;34m'"$testsToBuildAsString"'\033[0m'
        fi

        $LINKER $LINK_FLAGS "$BUILD_DIRECTORY/"'lib'"$testsMainPackage"'.a' $testsToBuildAsString $librariesToLinkAgainst -o "$BUILD_DIRECTORY/$EXECUTABLE_NAME_TEST"

        if [ ! -z "${NEED_STRIP_EXECUTABLE+x}" ]; then
            if [ ${#EXECUTABLE_SECTIONS_TO_STRIP[@]} -ne 0 ]; then
                printf -v sectionsToStripAsString -- "--remove-section %s " "${EXECUTABLE_SECTIONS_TO_STRIP[@]}"
                echo  -e '\033[1;34m'"$sectionsToStripAsString"'\033[0m'
            fi

            objcopy "$BUILD_DIRECTORY/$EXECUTABLE_NAME_TEST" $sectionsToStripAsString

            strip --strip-section-headers "$BUILD_DIRECTORY/$EXECUTABLE_NAME_TEST"
        fi
    fi
fi

}
