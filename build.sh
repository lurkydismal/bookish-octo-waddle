#!/bin/bash
shopt -s nullglob

export SCRIPT_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export BUILD_DIRECTORY_NAME='out'
export TESTS_DIRECTORY_NAME='tests'
export BUILD_DIRECTORY="$SCRIPT_DIRECTORY/$BUILD_DIRECTORY_NAME"
export TESTS_DIRECTORY="$SCRIPT_DIRECTORY/$TESTS_DIRECTORY_NAME"

# 0 - Debug
# 1 - Release
# 2 - Profile
# 3 - Tests
export BUILD_TYPE=${BUILD_TYPE:-0}

export BUILD_C_FLAGS="-fopenmp -flto=jobserver -std=gnu99 -march=native -ffunction-sections -fdata-sections -fPIC -fopenmp-simd -fno-ident -fshort-enums -Wall -Wextra"
export BUILD_C_FLAGS_DEBUG="-Og -g"
export BUILD_C_FLAGS_RELEASE="-fprofile-use -Ofast -funroll-loops -fno-asynchronous-unwind-tables"
export BUILD_C_FLAGS_PROFILE="-fprofile-generate $BUILD_C_FLAGS_RELEASE"
export BUILD_C_FLAGS_TESTS="$BUILD_C_FLAGS_DEBUG"

export declare BUILD_DEFINES=(
    "YYJSON_DISABLE_WRITER"
    "YYJSON_DISABLE_UTILS"
    "YYJSON_DISABLE_NON_STANDARD"
    "YYJSON_DISABLE_UTF8_VALIDATION"
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
    "main/applicationState_t/include"
    "gameState_t/include"
    "GLTF_t/include"
    "image_t/include"
    "font_t/include"
    "settings_t/include"
    "window_t/include"
    "vsync/include"
    "FPS/include"
    "asset_t/include"
    "log/include"
    "stdfunc/include"
    "glad/include"
    "yyjson/include"
)

export declare BUILD_INCLUDES_TESTS=(
    "test/include"
)

export LINK_FLAGS="-fopenmp -flto -fPIC -fuse-ld=mold -Wl,-O1 -Wl,--gc-sections -Wl,--no-eh-frame-hdr"
export LINK_FLAGS_DEBUG="-g"
export LINK_FLAGS_RELEASE="-s"
export LINK_FLAGS_PROFILE="$LINK_FLAGS_DEBUG"
export LINK_FLAGS_TESTS="$LINK_FLAGS_DEBUG"

export declare LIBRARIES_TO_LINK=(
    "glfw"
    "mimalloc"
    "m"
)
export declare EXTERNAL_LIBRARIES_TO_LINK=(
    "snappy"
)
export declare LIBRARIES_TO_LINK_TESTS=(
    "m"
)
export C_COMPILER="ccache gcc"
export EXECUTABLE_NAME="main.out"
export EXECUTABLE_NAME_TESTS="$EXECUTABLE_NAME"'_test'
export declare EXECUTABLE_SECTIONS_TO_STRIP=(
    ".note.gnu.build-id"
    ".note.gnu.property"
    ".comment"
    ".eh_frame"
    ".eh_frame_hdr"
    ".relro_padding"
)

export readonly RED_LIGHT_COLOR='\e[1;31m'
export readonly GREEN_LIGHT_COLOR='\e[1;32m'
export readonly YELLOW_COLOR='\e[1;33m'
export readonly BLUE_LIGHT_COLOR='\e[1;34m'
export readonly PURPLE_LIGHT_COLOR='\e[1;35m'
export readonly CYAN_LIGHT_COLOR='\e[1;36m'
export readonly RESET_COLOR='\e[0m'

export BUILD_TYPE_COLOR="$PURPLE_LIGHT_COLOR"
export DEFINES_COLOR="$CYAN_LIGHT_COLOR"
export INCLUDES_COLOR="$BLUE_LIGHT_COLOR"
export LIBRARIES_COLOR="$BLUE_LIGHT_COLOR"
export EXTERNAL_LIBRARIES_COLOR="$BLUE_LIGHT_COLOR"
export PARTS_TO_BUILD_COLOR="$YELLOW_COLOR"
export SKIPPING_PART_IN_BUILD_COLOR="$GREEN_LIGHT_COLOR"
export BUILT_EXECUTABLE_COLOR="$GREEN_LIGHT_COLOR"
export SECTIONS_TO_STRIP_COLOR="$RED_LIGHT_COLOR"

clear

source './config.sh' && {

mkdir -p "$BUILD_DIRECTORY"

# Remove all object files
{
    if [ $BUILD_TYPE -eq 1 ]; then
        fd -I '\.o$' -x rm {}

    else
        printf -v staticPartsAsExcludeString -- "-E %s " "${staticParts[@]}"

        fd -I '\.o$' $staticPartsAsExcludeString -x rm {}

        unset staticPartsAsExcludeString
    fi
}

if [ $BUILD_TYPE -eq 0 ]; then
    echo -e "$BUILD_TYPE_COLOR"'Debug build'"$RESET_COLOR"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_DEBUG"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_DEBUG"
    BUILD_DEFINES+=( "${BUILD_DEFINES_DEBUG[@]}" )

elif [ $BUILD_TYPE -eq 1 ]; then
    echo -e "$BUILD_TYPE_COLOR"'Release build'"$RESET_COLOR"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_RELEASE"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_RELEASE"
    BUILD_DEFINES+=( "${BUILD_DEFINES_RELEASE[@]}" )

elif [ $BUILD_TYPE -eq 2 ]; then
    echo -e "$BUILD_TYPE_COLOR"'Profile build'"$RESET_COLOR"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_PROFILE"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_PROFILE"
    BUILD_DEFINES+=( "${BUILD_DEFINES_PROFILE[@]}" )

elif [ $BUILD_TYPE -eq 3 ]; then
    echo -e "$BUILD_TYPE_COLOR"'Building tests'"$RESET_COLOR"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_TESTS"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_TESTS"
    BUILD_DEFINES+=( "${BUILD_DEFINES_DEBUG[@]}" )
fi

if [ ${#BUILD_DEFINES[@]} -ne 0 ]; then
    printf -v definesAsString -- "-D %s " "${BUILD_DEFINES[@]}"
    echo -e "$DEFINES_COLOR""$definesAsString""$RESET_COLOR"
fi

if [ ${#BUILD_INCLUDES[@]} -ne 0 ]; then
    printf -v includesAsString -- "-I $SCRIPT_DIRECTORY/%s " "${BUILD_INCLUDES[@]}"
    echo -e "$INCLUDES_COLOR""$includesAsString""$RESET_COLOR"
fi

if [ ${#EXTERNAL_LIBRARIES_TO_LINK[@]} -ne 0 ]; then
    printf -v externalLibrariesAsString -- "%s " "${EXTERNAL_LIBRARIES_TO_LINK[@]}"
    echo -e "$EXTERNAL_LIBRARIES_COLOR""$externalLibrariesAsString""$RESET_COLOR"
    externalLibrariesBuildCFlagsAsString="$(pkg-config --static --cflags $externalLibrariesAsString)"' '
    echo -e "$INCLUDES_COLOR""$externalLibrariesBuildCFlagsAsString""$RESET_COLOR"
    externalLibrariesLinkFlagsAsString="$(pkg-config --static --libs $externalLibrariesAsString)"' '
    echo -e "$LIBRARIES_COLOR""$externalLibrariesLinkFlagsAsString""$RESET_COLOR"
    unset externalLibrariesAsString
fi

if [ ${#partsToBuild[@]} -ne 0 ]; then
    printf -v partsToBuildAsString -- "$BUILD_DIRECTORY/lib%s.a " "${partsToBuild[@]}"
    echo -e "$PARTS_TO_BUILD_COLOR""$partsToBuildAsString""$RESET_COLOR"
fi

for partToBuild in "${partsToBuild[@]}"; do
    source "$partToBuild/config.sh" && {
        export OUTPUT_FILE='lib'"$partToBuild"'.a'

        './build_general.sh' "$partToBuild" "$BUILD_C_FLAGS $externalLibrariesBuildCFlagsAsString" "$definesAsString" "$includesAsString"

        BUILD_STATUS=$?

        unset FILES_TO_INCLUDE FILES_TO_COMPILE
    }

    if [ $BUILD_STATUS -ne 0 ]; then
        break
    fi
done

if [ $BUILD_STATUS -eq 0 ]; then
    if [ ${#staticParts[@]} -ne 0 ]; then
        printf -v staticPartsAsString -- "$BUILD_DIRECTORY/lib%s.a " "${staticParts[@]}"
        echo -e "$PARTS_TO_BUILD_COLOR""$staticPartsAsString""$RESET_COLOR"
    fi

    for staticPart in "${staticParts[@]}"; do
        source "$staticPart/config.sh" && {
            export OUTPUT_FILE='lib'"$staticPart"'.a'

            if [ -z "${NEED_REBUILD_STATIC_PARTS+x}" ]; then
                if [ -f "$BUILD_DIRECTORY/$OUTPUT_FILE" ]; then
                    echo -e "$SKIPPING_PART_IN_BUILD_COLOR""Skipping static '$staticPart' — '$OUTPUT_FILE' already exists.""$RESET_COLOR"

                    continue
                fi
            fi

            './build_general.sh' "$staticPart" "$BUILD_C_FLAGS $externalLibrariesBuildCFlagsAsString" "$definesAsString" "$includesAsString"

            BUILD_STATUS=$?

            unset FILES_TO_INCLUDE FILES_TO_COMPILE
        }

        if [ $BUILD_STATUS -ne 0 ]; then
            break
        fi
    done
fi

# Build main executable
if [ $BUILD_STATUS -eq 0 ]; then
    # Build executable main package
    if [ $BUILD_STATUS -eq 0 ]; then
        source "$executableMainPackage/config.sh" && {
            export OUTPUT_FILE='lib'"$executableMainPackage"'.a'

            './build_general.sh' "$executableMainPackage" "$BUILD_C_FLAGS $externalLibrariesBuildCFlagsAsString" "$definesAsString" "$includesAsString"

            BUILD_STATUS=$?

            unset FILES_TO_INCLUDE FILES_TO_COMPILE
        }
    fi

    if [ $BUILD_TYPE -ne 3 ]; then
        if [ $BUILD_STATUS -eq 0 ]; then
            if [ ${#LIBRARIES_TO_LINK[@]} -ne 0 ]; then
                printf -v librariesToLinkAsString -- "-l%s " "${LIBRARIES_TO_LINK[@]}"
                echo  -e "$LIBRARIES_COLOR""$librariesToLinkAsString""$RESET_COLOR"
            fi

            $C_COMPILER $LINK_FLAGS "$BUILD_DIRECTORY/"'lib'"$executableMainPackage"'.a' $staticPartsAsString $partsToBuildAsString $librariesToLinkAsString $externalLibrariesLinkFlagsAsString -o "$BUILD_DIRECTORY/$EXECUTABLE_NAME"
            echo  -e "$BUILT_EXECUTABLE_COLOR""$EXECUTABLE_NAME""$RESET_COLOR"

            if [ ! -z "${NEED_STRIP_EXECUTABLE+x}" ]; then
                if [ ${#EXECUTABLE_SECTIONS_TO_STRIP[@]} -ne 0 ]; then
                    printf -v sectionsToStripAsString -- "--remove-section %s " "${EXECUTABLE_SECTIONS_TO_STRIP[@]}"
                    echo  -e "$SECTIONS_TO_STRIP_COLOR""$sectionsToStripAsString""$RESET_COLOR"
                fi

                objcopy "$BUILD_DIRECTORY/$EXECUTABLE_NAME" $sectionsToStripAsString

                strip --strip-section-headers "$BUILD_DIRECTORY/$EXECUTABLE_NAME"
            fi
        fi
    fi
fi

# Build tests
if [ $BUILD_TYPE -eq 3 ]; then
    if [ ${#BUILD_INCLUDES_TESTS[@]} -ne 0 ]; then
        printf -v testIncludesAsString -- "-I $SCRIPT_DIRECTORY/%s " "${BUILD_INCLUDES_TESTS[@]}"
        echo  -e "$INCLUDES_COLOR""$testIncludesAsString""$RESET_COLOR"
    fi

    for testToBuild in "${testsToBuild[@]}"; do
        source "$TESTS_DIRECTORY/$testToBuild/config.sh" && {
            export OUTPUT_FILE='lib'"$testToBuild"'_test.a'

            './build_general.sh' "$TESTS_DIRECTORY/$testToBuild" "$BUILD_C_FLAGS $externalLibrariesBuildCFlagsAsString" "$definesAsString" "$includesAsString""$testIncludesAsString"

            BUILD_STATUS=$?

            unset FILES_TO_INCLUDE FILES_TO_COMPILE
        }

        if [ $BUILD_STATUS -ne 0 ]; then
            break
        fi
    done

    # Build tests main package
    if [ $BUILD_STATUS -eq 0 ]; then
        source "$testsMainPackage/config.sh" && {
            export OUTPUT_FILE='lib'"$testsMainPackage"'.a'

            './build_general.sh' "$testsMainPackage" "$BUILD_C_FLAGS $externalLibrariesBuildCFlagsAsString" "$definesAsString" "$includesAsString""$testIncludesAsString"

            BUILD_STATUS=$?

            unset FILES_TO_INCLUDE FILES_TO_COMPILE
        }
    fi

    if [ $BUILD_STATUS -eq 0 ]; then
        if [ ${#testsToBuild[@]} -ne 0 ]; then
            printf -v testsToBuildAsString -- "$BUILD_DIRECTORY/lib%s_test.a " "${testsToBuild[@]}"
            echo  -e "$PARTS_TO_BUILD_COLOR""$testsToBuildAsString""$RESET_COLOR"
        fi

        if [ ${#LIBRARIES_TO_LINK_TESTS[@]} -ne 0 ]; then
            printf -v testsLibrariesToLinkAsString -- "-l%s " "${LIBRARIES_TO_LINK_TESTS[@]}"
            echo  -e "$LIBRARIES_COLOR""$testsLibrariesToLinkAsString""$RESET_COLOR"
        fi

        $C_COMPILER $LINK_FLAGS '-Wl,--whole-archive' "$BUILD_DIRECTORY/"'lib'"$testsMainPackage"'.a' $testsToBuildAsString $staticPartsAsString $partsToBuildAsString '-Wl,--no-whole-archive' $librariesToLinkAsString $externalLibrariesLinkFlagsAsString $testsLibrariesToLinkAsString -o "$BUILD_DIRECTORY/$EXECUTABLE_NAME_TESTS"
        echo  -e "$BUILT_EXECUTABLE_COLOR""$EXECUTABLE_NAME_TESTS""$RESET_COLOR"

        if [ ! -z "${NEED_STRIP_EXECUTABLE+x}" ]; then
            if [ ${#EXECUTABLE_SECTIONS_TO_STRIP[@]} -ne 0 ]; then
                printf -v sectionsToStripAsString -- "--remove-section %s " "${EXECUTABLE_SECTIONS_TO_STRIP[@]}"
                echo  -e "$SECTIONS_TO_STRIP_COLOR""$sectionsToStripAsString""$RESET_COLOR"
            fi

            objcopy "$BUILD_DIRECTORY/$EXECUTABLE_NAME_TESTS" $sectionsToStripAsString

            strip --strip-section-headers "$BUILD_DIRECTORY/$EXECUTABLE_NAME_TESTS"
        fi
    fi
fi

}
