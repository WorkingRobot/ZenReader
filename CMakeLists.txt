cmake_minimum_required (VERSION 3.8)

project(ZenReader)

## Options ##

option(ENABLE_TRACY "Adds tracy profiler (MSVC compiler and initialized git submodule is required)" OFF)
option(USE_JSON "Requires rapidjson, allows the use of FModel's and JohnWickParse's mappings" OFF)


## Use /MT instead of /MD ##

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()


## Force MSVC to have conformance with other compilers ##

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/permissive-")


## Give non MSVC compilers some intrinsics (AES is required) ##

else()
    add_compile_options("-march=native")
endif()


## Dependencies ##

link_libraries(
    "${CMAKE_CURRENT_SOURCE_DIR}\\src\\Helpers\\oo2core_8_win64.lib"
)

if (USE_JSON)
    message("USING JSON")
    add_compile_definitions(USE_JSON)
    find_package(RapidJSON CONFIG REQUIRED)
    include_directories(${RAPIDJSON_INCLUDE_DIRS})
endif()


## Source Files ##

function(ADD_DIR PROJECT TARGET_DIR)
    aux_source_directory(${TARGET_DIR} TARGET_DIR_SOURCES)
    list(APPEND ${PROJECT}_SOURCES ${TARGET_DIR_SOURCES})
    set(${PROJECT}_SOURCES ${${PROJECT}_SOURCES} PARENT_SCOPE)
endfunction()

function(ADD_FILE PROJECT TARGET_FILE)
    list(APPEND ${PROJECT}_SOURCES ${TARGET_FILE})
    set(${PROJECT}_SOURCES ${${PROJECT}_SOURCES} PARENT_SCOPE)
endfunction()

function(ADD_DIR_RECURSIVE PROJECT TARGET_DIR)
    file(GLOB_RECURSE CHILDREN ${TARGET_DIR}/*)
    foreach(CHILDPATH ${CHILDREN})
        get_filename_component(CHILD ${CHILDPATH} DIRECTORY)
        if(IS_DIRECTORY ${CHILD})
            list(APPEND DIRLIST ${CHILD})
        endif()
    endforeach()
    list(REMOVE_DUPLICATES DIRLIST)

    foreach(DIRPATH ${DIRLIST})
        add_dir(${PROJECT} ${DIRPATH})
    endforeach()
    set(${PROJECT}_SOURCES ${${PROJECT}_SOURCES} PARENT_SCOPE)
endfunction()

add_dir_recursive(ZenReader src)


## Tracy option handling ##

if(ENABLE_TRACY AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message("Tracy profiling is enabled")
    list(APPEND ZenReader_SOURCES "tracy/TracyClient.cpp")
    include_directories(tracy)
    add_definitions(-DTRACY_ENABLE -DTRACY_NO_EXIT)
endif()


## Define target ##

add_executable(ZenReader ${ZenReader_SOURCES} "src/Exports/UObject.h")


## Properties ##

# C++20
set_property(TARGET ZenReader PROPERTY CXX_STANDARD 20)

# Add .pdb for release builds
if(CMAKE_BUILD_TYPE MATCHES "Release")
   target_compile_options(ZenReader PRIVATE /Zi)
   set_target_properties(ZenReader PROPERTIES
        LINK_FLAGS "/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF"
        COMPILE_PDB_NAME ZenReader 
        COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR}
   )
endif()
