#!/bin/bash
shopt -s nullglob

trap 'echo "Line $LINENO: $BASH_COMMAND";echo; exit 1' ERR

export SCRIPT_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export BUILD_DIRECTORY_NAME="out"
export TESTS_DIRECTORY_NAME="tests"
export BUILD_DIRECTORY="$SCRIPT_DIRECTORY/$BUILD_DIRECTORY_NAME"
export TESTS_DIRECTORY="$SCRIPT_DIRECTORY/$TESTS_DIRECTORY_NAME"

export HASH_FUNCTION="sha512sum"

# 0 - Debug
# 1 - Release
# 2 - Profile
# 3 - Tests
export BUILD_TYPE=${BUILD_TYPE:-0}

export BUILD_C_FLAGS="-flto=jobserver -std=gnu99 -march=native -ffunction-sections -fdata-sections -fPIC -fopenmp-simd -fno-ident -fno-short-enums -Wall -Wextra -Wno-gcc-compat"
export BUILD_C_FLAGS_DEBUG="-Og -ggdb3"
export BUILD_C_FLAGS_RELEASE="-fprofile-instr-use -O3 -ffast-math -funroll-loops -fno-asynchronous-unwind-tables"
export BUILD_C_FLAGS_PROFILE="-fprofile-instr-generate -pg -O3 -ffast-math -funroll-loops -fno-asynchronous-unwind-tables"
export BUILD_C_FLAGS_TESTS="$BUILD_C_FLAGS_DEBUG -fopenmp"

export BUILD_CPP_FLAGS="$BUILD_C_FLAGS -std=gnu++26 -Wno-enum-enum-conversion -Wno-deprecated"
export BUILD_CPP_FLAGS_DEBUG="$BUILD_C_FLAGS_DEBUG"
export BUILD_CPP_FLAGS_RELEASE="$BUILD_C_FLAGS_RELEASE"
export BUILD_CPP_FLAGS_PROFILE="$BUILD_C_FLAGS_PROFILE"
export BUILD_CPP_FLAGS_TESTS="$BUILD_C_FLAGS_TESTS"

# TODO: checker alpha
export SCAN_BUILD_FLAGS="-enable-checker core,security,nullability,deadcode,unix,optin"

export declare BUILD_DEFINES=(
    "_GNU_SOURCE"
    "INI_ALLOW_INLINE_COMMENTS=1"
    "INI_STOP_ON_FIRST_ERROR=1"
    "INI_CALL_HANDLER_ON_NEW_SECTION=1"
)

export declare BUILD_DEFINES_DEBUG=(
    "DEBUG"
    "LOG_WATCH"
)

export declare BUILD_DEFINES_RELEASE=(
    "RELEASE"
)

export declare BUILD_DEFINES_PROFILE=(
    "PROFILE"
)

export declare BUILD_DEFINES_TESTS=(
    "${BUILD_DEFINES_DEBUG[@]}"
    "TESTS"
)

export declare BUILD_DEFINES_HOT_RELOAD=(
    "HOT_RELOAD"
)

export declare BUILD_INCLUDES=(
    "runtime/include"
    "runtime/applicationState_t/include"
    "controls_t/include"
    "input/include"
    "FPS/include"
    "player_t/include"
    "object_t/include"
    "camera_t/include"
    "state_t/include"
    "animation_t/include"
    "boxes_t/include"
    "color_t/include"
    "settings_t/include"
    "window_t/include"
    "vsync/include"
    "config_t/include"
    "background_t/include"
    "HUD_t/include"
    "character_t/include"
    "asset_t/include"
    "watch_t/include"
    "log/include"
    "stdfunc/include"
    "inih/include"
    "plthook/include"
)

export declare BUILD_INCLUDES_TESTS=(
    "test/include"
)

export LINK_FLAGS="-flto -fPIC -fuse-ld=mold -Wl,-O1 -Wl,--gc-sections -Wl,--no-eh-frame-hdr"
export LINK_FLAGS_DEBUG="-rdynamic -Wl,-rpath,\$ORIGIN"
export LINK_FLAGS_RELEASE="-s"
export LINK_FLAGS_PROFILE=""
export LINK_FLAGS_TESTS="-fopenmp $LINK_FLAGS_DEBUG"
export LINK_FLAGS_HOT_RELOAD=""

export declare LIBRARIES_TO_LINK=(
    "mimalloc"
    "elf"
)
export declare EXTERNAL_LIBRARIES_TO_LINK=(
    "snappy"
    "sdl3"
    "sdl3-image"
    "libcjson"
)
export declare LIBRARIES_TO_LINK_TESTS=(
    "m"
)
export C_COMPILER="gcc"
export CPP_COMPILER="g++"

if [ ! -z "${DISABLE_OPTIMIZATIONS+x}" ]; then
    BUILD_C_FLAGS_DEBUG+=" -O0"
    BUILD_C_FLAGS_RELEASE+=" -O0"
    BUILD_C_FLAGS_PROFILE+=" -O0"
    BUILD_C_FLAGS_TESTS+=" -O0"
fi

if [ ! -z "${ENABLE_MUSL+x}" ]; then
    # Musl does not work with Clang
    export DISABLE_CLANG=1

    C_COMPILER="musl-gcc"
    CPP_COMPILER="musl-g++"

    BUILD_C_FLAGS="${BUILD_C_FLAGS/-Wno-gcc-compat/}"
    BUILD_CPP_FLAGS="${BUILD_CPP_FLAGS/-Wno-gcc-compat/}"
    LINK_FLAGS="${LINK_FLAGS/-fuse-ld=mold/}"

    if [ -z "${ENABLE_MUSL_STATIC+x}" ]; then
        LINK_FLAGS_RELEASE+=" -static"
    fi
fi

if [ -z "${DISABLE_CLANG+x}" ]; then
    C_COMPILER="clang"
    CPP_COMPILER="clang++"

    BUILD_C_FLAGS+=" -Wno-c23-extensions -Wno-gnu-folding-constant"

    # Debug or Tests
    if [ $BUILD_TYPE -eq 0 ] || [ $BUILD_TYPE -eq 3 ]; then
        BUILD_C_FLAGS_PROFILE+=" -fprofile-instr-generate -fcoverage-mapping"
        BUILD_C_FLAGS_TESTS+=" -fprofile-instr-generate -fcoverage-mapping"

        BUILD_CPP_FLAGS_PROFILE+=" -fprofile-instr-generate -fcoverage-mapping"
        BUILD_CPP_FLAGS_TESTS+=" -fprofile-instr-generate -fcoverage-mapping"

        LINK_FLAGS_PROFILE+=" -fprofile-instr-generate -fcoverage-mapping"
        LINK_FLAGS_TESTS+=" -fprofile-instr-generate -fcoverage-mapping"
    fi

    if [ ! -z "${ENABLE_SANITIZERS+x}" ]; then
        BUILD_C_FLAGS_DEBUG+=" -fsanitize=address,undefined,leak"
        BUILD_C_FLAGS_TESTS+=" -fsanitize=address,undefined,leak"

        BUILD_CPP_FLAGS_DEBUG+=" -fsanitize=address,undefined,leak"
        BUILD_CPP_FLAGS_TESTS+=" -fsanitize=address,undefined,leak"

        LINK_FLAGS_DEBUG+=" -fsanitize=address,undefined,leak"
        LINK_FLAGS_TESTS+=" -fsanitize=address,undefined,leak"
    fi
fi

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
    # Release
    if [ $BUILD_TYPE -eq 1 ]; then
        fd -I -e o -x rm {}

    else
        if [[ -n "${staticParts[@]}" ]]; then
            printf -v staticPartsAsExcludeString -- "-E %s " "${staticParts[@]}"

        else
            staticPartsAsExcludeString=""
        fi

        fd -I -e o $staticPartsAsExcludeString -x rm {}

        unset staticPartsAsExcludeString
    fi
}

# Debug
if [ $BUILD_TYPE -eq 0 ]; then
    echo -e "$BUILD_TYPE_COLOR"'Debug build'"$RESET_COLOR"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_DEBUG"
    BUILD_CPP_FLAGS="$BUILD_CPP_FLAGS $BUILD_CPP_FLAGS_DEBUG"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_DEBUG"
    BUILD_DEFINES+=( "${BUILD_DEFINES_DEBUG[@]}" )

# Release
elif [ $BUILD_TYPE -eq 1 ]; then
    echo -e "$BUILD_TYPE_COLOR"'Release build'"$RESET_COLOR"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_RELEASE"
    BUILD_CPP_FLAGS="$BUILD_CPP_FLAGS $BUILD_CPP_FLAGS_RELEASE"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_RELEASE"
    BUILD_DEFINES+=( "${BUILD_DEFINES_RELEASE[@]}" )

# Profile
elif [ $BUILD_TYPE -eq 2 ]; then
    echo -e "$BUILD_TYPE_COLOR"'Profile build'"$RESET_COLOR"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_PROFILE"
    BUILD_CPP_FLAGS="$BUILD_CPP_FLAGS $BUILD_CPP_FLAGS_PROFILE"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_PROFILE"
    BUILD_DEFINES+=( "${BUILD_DEFINES_PROFILE[@]}" )

# Tests
elif [ $BUILD_TYPE -eq 3 ]; then
    echo -e "$BUILD_TYPE_COLOR"'Building tests'"$RESET_COLOR"

    BUILD_C_FLAGS="$BUILD_C_FLAGS $BUILD_C_FLAGS_TESTS"
    BUILD_CPP_FLAGS="$BUILD_CPP_FLAGS $BUILD_CPP_FLAGS_TESTS"
    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_TESTS"
    BUILD_DEFINES+=( "${BUILD_DEFINES_TESTS[@]}" )
fi

# Hot reload
if [ ! -z "${ENABLE_HOT_RELOAD+x}" ]; then
    echo -e "$BUILD_TYPE_COLOR"'Building with hot reload'"$RESET_COLOR"

    LINK_FLAGS="$LINK_FLAGS $LINK_FLAGS_HOT_RELOAD"
    BUILD_DEFINES+=( "${BUILD_DEFINES_HOT_RELOAD[@]}" )
fi

# Set BUILD_FLAGS and COMPILER
if [ ! -z "${CPP_PROJECT+x}" ]; then
    BUILD_FLAGS="$BUILD_CPP_FLAGS"
    COMPILER="$CPP_COMPILER"

else
    BUILD_FLAGS="$BUILD_C_FLAGS"
    COMPILER="$C_COMPILER"
fi

export BUILD_FLAGS
export COMPILER

if [ -z "${DISABLE_BUILD_CACHE+x}" ]; then
    COMPILER="ccache $COMPILER"
fi

if [ ! -z "${SCAN_BUILD+x}" ]; then
    COMPILER="scan-build $SCAN_BUILD_FLAGS $COMPILER"
fi

if [ ${#BUILD_DEFINES[@]} -ne 0 ]; then
    printf -v definesAsString -- "-D %s " "${BUILD_DEFINES[@]}"
    echo -e "$DEFINES_COLOR""$definesAsString""$RESET_COLOR"
fi

if [ ${#BUILD_INCLUDES[@]} -ne 0 ]; then
    printf -v includesAsString -- "-I $SCRIPT_DIRECTORY/%s " "${BUILD_INCLUDES[@]}"
    echo -en "$INCLUDES_COLOR"
    printf -- "-I %s " "${BUILD_INCLUDES[@]}"
    echo -e "$RESET_COLOR"
fi

if [ ${#EXTERNAL_LIBRARIES_TO_LINK[@]} -ne 0 ]; then
    printf -v externalLibrariesAsString -- "%s " "${EXTERNAL_LIBRARIES_TO_LINK[@]}"

    echo -e '\n'"$EXTERNAL_LIBRARIES_COLOR""$externalLibrariesAsString""$RESET_COLOR"
    externalLibrariesBuildCFlagsAsString="$(pkg-config --static --cflags $externalLibrariesAsString)"' '

    SEARCH_STATUS=$?

    if [ $SEARCH_STATUS -ne 0 ]; then
        exit $SEARCH_STATUS
    fi

    echo -e "$INCLUDES_COLOR""$externalLibrariesBuildCFlagsAsString""$RESET_COLOR"
    externalLibrariesLinkFlagsAsString="$(pkg-config --static --libs $externalLibrariesAsString)"' '

    SEARCH_STATUS=$?

    if [ $SEARCH_STATUS -ne 0 ]; then
        exit $SEARCH_STATUS
    fi

    echo -e "$LIBRARIES_COLOR""$externalLibrariesLinkFlagsAsString""$RESET_COLOR"

    unset externalLibrariesAsString
fi

processedFiles=()
processedFilesStatic=()
declare -A processedFilesHashes=()
processIDs=()
processStatuses=()
BUILD_STATUS=0

if [ ${#partsToBuild[@]} -ne 0 ]; then
    printf -v partsToBuildAsString -- "$BUILD_DIRECTORY/lib%s"'.a ' "${partsToBuild[@]}"
    echo -en "$PARTS_TO_BUILD_COLOR"
    printf -- "lib%s"'.a ' "${partsToBuild[@]}"
    echo -e "$RESET_COLOR"
fi

for partToBuild in "${partsToBuild[@]}"; do
    source "$partToBuild/config.sh" && {
        OUTPUT_FILE='lib'"$partToBuild"'.a'

        processedFiles+=("$OUTPUT_FILE")

        if [ -f "$BUILD_DIRECTORY/$OUTPUT_FILE" ]; then
            processedFilesHashes["$OUTPUT_FILE"]="$($HASH_FUNCTION "$BUILD_DIRECTORY/$OUTPUT_FILE" | cut -d ' ' -f1)"
        fi

        OUTPUT_FILE="$OUTPUT_FILE" \
            './build_general.sh' \
                "$partToBuild" \
                "$BUILD_FLAGS $externalLibrariesBuildCFlagsAsString" \
                "$definesAsString" \
                "$includesAsString" &

        processIDs+=($!)

        unset FILES_TO_INCLUDE FILES_TO_COMPILE
    }

    BUILD_STATUS=$?

    if [ $BUILD_STATUS -ne 0 ]; then
        exit
    fi
done

if [ $BUILD_STATUS -eq 0 ]; then
    if [ ${#staticParts[@]} -ne 0 ]; then
        printf -v staticPartsAsString -- "$BUILD_DIRECTORY/lib%s.a " "${staticParts[@]}"
        echo -en "$PARTS_TO_BUILD_COLOR"
        printf -- "lib%s"'.a ' "${staticParts[@]}"
        echo -e "$RESET_COLOR"
    fi

    for staticPart in "${staticParts[@]}"; do
        source "$staticPart/config.sh" && {
            OUTPUT_FILE='lib'"$staticPart"'.a'

            processedFilesStatic+=("$OUTPUT_FILE")

            if [ -f "$BUILD_DIRECTORY/$OUTPUT_FILE" ]; then
                processedFilesHashes["$OUTPUT_FILE"]="$($HASH_FUNCTION "$BUILD_DIRECTORY/$OUTPUT_FILE" | cut -d ' ' -f1)"
            fi

            if [ -z "${REBUILD_STATIC_PARTS+x}" ]; then
                if [ -f "$BUILD_DIRECTORY/$OUTPUT_FILE" ]; then
                    echo -e "$SKIPPING_PART_IN_BUILD_COLOR""Skipping static '$staticPart' — '$OUTPUT_FILE' already exists.""$RESET_COLOR"

                    continue
                fi
            fi

            OUTPUT_FILE="$OUTPUT_FILE" \
                './build_general.sh' \
                    "$staticPart" \
                    "$BUILD_FLAGS $externalLibrariesBuildCFlagsAsString" \
                    "$definesAsString" \
                    "$includesAsString" &

            processIDs+=($!)

            unset FILES_TO_INCLUDE FILES_TO_COMPILE
        }

        BUILD_STATUS=$?

        if [ $BUILD_STATUS -ne 0 ]; then
            exit
        fi
    done
fi

for processID in "${processIDs[@]}"; do
    wait "$processID"

    processStatuses+=($?)
done

BUILD_STATUS=0

for processStatus in "${processStatuses[@]}"; do
    if [[ "$processStatus" -ne 0 ]]; then
        BUILD_STATUS=$processStatus

        break
    fi
done

processIDs=()
processStatuses=()

if [ $BUILD_STATUS -ne 0 ]; then
    exit
fi

# Debug
if [ $BUILD_TYPE -eq 0 ] && [ ! -z "${ENABLE_HOT_RELOAD+x}" ]; then
    if [ $BUILD_STATUS -eq 0 ]; then
        # Convert static to shared objects
        for processedFile in "${processedFilesStatic[@]}"; do
            outputFile="${processedFile%.a}.so"

            if [ -z "${REBUILD_STATIC_PARTS+x}" ]; then
                if [ -f "$BUILD_DIRECTORY/$outputFile" ]; then
                    continue
                fi
            fi

            if [ ! -f "$BUILD_DIRECTORY/$outputFile" ] || [ "$($HASH_FUNCTION "$BUILD_DIRECTORY/$processedFile" | cut -d ' ' -f1)" != "${processedFilesHashes["$processedFile"]}" ]; then
                echo "Linking static $outputFile"

                $COMPILER -shared -nostdlib $LINK_FLAGS '-Wl,--whole-archive' "$BUILD_DIRECTORY/$processedFile" '-Wl,--no-whole-archive' -o "$BUILD_DIRECTORY/""$outputFile" &

                export NEED_HOT_RELOAD

                processIDs+=($!)
            fi
        done

        for processID in "${processIDs[@]}"; do
            wait "$processID"

            processStatuses+=($?)
        done

        BUILD_STATUS=0

        for processStatus in "${processStatuses[@]}"; do
            if [[ "$processStatus" -ne 0 ]]; then
                BUILD_STATUS=$processStatus

                break
            fi
        done

        if [ $BUILD_STATUS -ne 0 ]; then
            exit
        fi

        processIDs=()
        processStatuses=()

        # Convert to shared objects
        for processedFile in "${processedFiles[@]}"; do
            outputFile="${processedFile%.a}.so"

            if [ ! -f "$BUILD_DIRECTORY/$outputFile" ] || [ "$($HASH_FUNCTION "$BUILD_DIRECTORY/$processedFile" | cut -d ' ' -f1)" != "${processedFilesHashes["$processedFile"]}" ]; then
                echo "Linking $outputFile"

                cd "$BUILD_DIRECTORY"

                $COMPILER -shared -nostdlib $LINK_FLAGS '-Wl,--whole-archive' "$BUILD_DIRECTORY/$processedFile" '-Wl,--no-whole-archive' -o "$BUILD_DIRECTORY/""$outputFile" &

                cd - > '/dev/null'

                export NEED_HOT_RELOAD

                processIDs+=($!)
            fi
        done

        for processID in "${processIDs[@]}"; do
            wait "$processID"

            processStatuses+=($?)
        done

        BUILD_STATUS=0

        for processStatus in "${processStatuses[@]}"; do
            if [[ "$processStatus" -ne 0 ]]; then
                BUILD_STATUS=$processStatus

                break
            fi
        done

        if [ $BUILD_STATUS -ne 0 ]; then
            exit
        fi

        processIDs=()
        processStatuses=()

        if [ -z "${NEED_HOT_RELOAD+x}" ]; then
            # Link root that will have DT_NEEDED for all shared objects
            source "$rootSharedObjectName/config.sh" && {
                OUTPUT_FILE="$rootSharedObjectName"'.a'

                OUTPUT_FILE="$OUTPUT_FILE" \
                    './build_general.sh' \
                    "$rootSharedObjectName" \
                    "$BUILD_FLAGS" \
                    "$definesAsString" \
                    "$includesAsString"

                outputFile="$rootSharedObjectName"'.so'

                echo "Linking $outputFile"

                cd "$BUILD_DIRECTORY"

                $COMPILER -shared $LINK_FLAGS '-Wl,--whole-archive' "$BUILD_DIRECTORY/$OUTPUT_FILE" '-Wl,--no-whole-archive' ${processedFiles[@]/%.a/.so} -o "$BUILD_DIRECTORY/$outputFile"

                BUILD_STATUS=$?

                cd - > '/dev/null'

                if [ $BUILD_STATUS -ne 0 ]; then
                    exit
                fi
            }
        fi

        BUILD_STATUS=$?

        if [ $BUILD_STATUS -ne 0 ]; then
            exit
        fi
    fi
fi

unset processedFilesHashes

# Build main executable
if [ $BUILD_STATUS -eq 0 ]; then
    # Build executable main package
    if [ $BUILD_STATUS -eq 0 ]; then
        source "$executableMainPackage/config.sh" && {
            OUTPUT_FILE='lib'"$executableMainPackage"'.a'

            OUTPUT_FILE="$OUTPUT_FILE" \
                './build_general.sh' \
                    "$executableMainPackage" \
                    "$BUILD_FLAGS $externalLibrariesBuildCFlagsAsString" \
                    "$definesAsString" \
                    "$includesAsString"

            BUILD_STATUS=$?

            if [ $BUILD_STATUS -ne 0 ]; then
                exit
            fi

            unset FILES_TO_INCLUDE FILES_TO_COMPILE
        }

        BUILD_STATUS=$?

        if [ $BUILD_STATUS -ne 0 ]; then
            exit
        fi
    fi

    # Not Tests
    if [ $BUILD_TYPE -ne 3 ]; then
        if [ $BUILD_STATUS -eq 0 ]; then
            if [ ${#LIBRARIES_TO_LINK[@]} -ne 0 ]; then
                printf -v librariesToLinkAsString -- "-l%s " "${LIBRARIES_TO_LINK[@]}"
                echo  -e "$LIBRARIES_COLOR""$librariesToLinkAsString""$RESET_COLOR"
            fi

            if [ -z "${SCAN_BUILD+x}" ]; then
                # Debug
                if [ $BUILD_TYPE -eq 0 ] && [ ! -z "${ENABLE_HOT_RELOAD+x}" ]; then
                    cd "$BUILD_DIRECTORY"

                    $COMPILER $LINK_FLAGS "$BUILD_DIRECTORY/"'lib'"$executableMainPackage"'.a' ${processedFilesStatic[@]/%.a/.so} ${processedFiles[@]/%.a/.so} $librariesToLinkAsString $externalLibrariesLinkFlagsAsString -o "$BUILD_DIRECTORY/$EXECUTABLE_NAME"

                    BUILD_STATUS=$?

                    cd - > '/dev/null'

                else
                    $COMPILER $LINK_FLAGS "$BUILD_DIRECTORY/"'lib'"$executableMainPackage"'.a' $staticPartsAsString $partsToBuildAsString $librariesToLinkAsString $externalLibrariesLinkFlagsAsString -o "$BUILD_DIRECTORY/$EXECUTABLE_NAME"

                    BUILD_STATUS=$?
                fi
            fi

            if [ $BUILD_STATUS -eq 0 ]; then
                echo  -e "$BUILT_EXECUTABLE_COLOR""$EXECUTABLE_NAME""$RESET_COLOR"

                if [ ! -z "${STRIP_EXECUTABLE+x}" ]; then
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
fi

# Build tests
if [ $BUILD_STATUS -eq 0 ]; then
    # Tests
    if [ $BUILD_TYPE -eq 3 ]; then
        if [ ${#BUILD_INCLUDES_TESTS[@]} -ne 0 ]; then
            printf -v testIncludesAsString -- "-I $SCRIPT_DIRECTORY/%s " "${BUILD_INCLUDES_TESTS[@]}"
            echo  -e "$INCLUDES_COLOR""$testIncludesAsString""$RESET_COLOR"
        fi

        for testToBuild in "${testsToBuild[@]}"; do
            source "$TESTS_DIRECTORY/$testToBuild/config.sh" && {
                OUTPUT_FILE='lib'"$testToBuild"'_test.a'

                OUTPUT_FILE="$OUTPUT_FILE" \
                    './build_general.sh' \
                        "$TESTS_DIRECTORY/$testToBuild" \
                        "$BUILD_FLAGS $externalLibrariesBuildCFlagsAsString" \
                        "$definesAsString" \
                        "$includesAsString""$testIncludesAsString" &

                processIDs+=($!)

                unset FILES_TO_INCLUDE FILES_TO_COMPILE
            }

            BUILD_STATUS=$?

            if [ $BUILD_STATUS -ne 0 ]; then
                exit
            fi
        done

        for processID in "${processIDs[@]}"; do
            wait "$processID"

            processStatuses+=($?)
        done

        BUILD_STATUS=0

        for processStatus in "${processStatuses[@]}"; do
            if [[ "$processStatus" -ne 0 ]]; then
                BUILD_STATUS=$processStatus

                break
            fi
        done

        if [ $BUILD_STATUS -ne 0 ]; then
            exit
        fi

        processIDs=()
        processStatuses=()

        # Build tests main package
        if [ $BUILD_STATUS -eq 0 ]; then
            source "$testsMainPackage/config.sh" && {
                OUTPUT_FILE='lib'"$testsMainPackage"'.a' \
                    './build_general.sh' \
                        "$testsMainPackage" \
                        "$BUILD_FLAGS $externalLibrariesBuildCFlagsAsString" \
                        "$definesAsString" \
                        "$includesAsString""$testIncludesAsString"

                BUILD_STATUS=$?

                if [ $BUILD_STATUS -ne 0 ]; then
                    exit
                fi

                unset FILES_TO_INCLUDE FILES_TO_COMPILE
            }

            BUILD_STATUS=$?

            if [ $BUILD_STATUS -ne 0 ]; then
                exit
            fi
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

            if [ -z "${SCAN_BUILD+x}" ]; then
                $COMPILER $LINK_FLAGS '-Wl,--whole-archive' "$BUILD_DIRECTORY/"'lib'"$testsMainPackage"'.a' $testsToBuildAsString $staticPartsAsString $partsToBuildAsString '-Wl,--no-whole-archive' $librariesToLinkAsString $externalLibrariesLinkFlagsAsString $testsLibrariesToLinkAsString -o "$BUILD_DIRECTORY/$EXECUTABLE_NAME_TESTS"

                BUILD_STATUS=$?
            fi


            if [ $BUILD_STATUS -eq 0 ]; then
                echo  -e "$BUILT_EXECUTABLE_COLOR""$EXECUTABLE_NAME_TESTS""$RESET_COLOR"

                if [ ! -z "${STRIP_EXECUTABLE+x}" ]; then
                    if [ ${#EXECUTABLE_SECTIONS_TO_STRIP[@]} -ne 0 ]; then
                        printf -v sectionsToStripAsString -- "--remove-section %s " "${EXECUTABLE_SECTIONS_TO_STRIP[@]}"
                        echo  -e "$SECTIONS_TO_STRIP_COLOR""$sectionsToStripAsString""$RESET_COLOR"
                    fi

                    objcopy "$BUILD_DIRECTORY/$EXECUTABLE_NAME_TESTS" $sectionsToStripAsString

                    strip --strip-section-headers "$BUILD_DIRECTORY/$EXECUTABLE_NAME_TESTS"
                fi
            fi
        fi
    fi
fi

}
