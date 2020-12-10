cmake_minimum_required (VERSION 3.8)

project(ZenReader)

## Options ##

option(USE_JSON "Requires RapidJSON, allows the use of FModel's, FunGames's, and JohnWickParse's mappings" OFF)
option(USE_BROTLI "Requires brotli, allows the use of brotli compressed usmap files" OFF)
option(USE_BROTLI_STATIC "Requires USE_BROTLI to be ON, uses the static build of brotli" OFF)


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


## Define target ##

add_library(ZenReader STATIC ${ZenReader_SOURCES})
target_include_directories(ZenReader PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS ZenReader DESTINATION lib)
install(FILES ${ZenReader_SOURCES} DESTINATION include)


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
    target_compile_options(ZenReader PRIVATE "/permissive-")


## Give non MSVC compilers some intrinsics (AES is required) ##

else()
    target_compile_options(ZenReader PRIVATE "-march=native")
endif()


## Dependencies ##

target_link_libraries(ZenReader PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}\\src\\Helpers\\oo2core_8_win64.lib"
)

if (USE_JSON)
    target_compile_definitions(ZenReader PRIVATE USE_JSON)
    find_package(RapidJSON CONFIG REQUIRED)
    target_include_directories(ZenReader PRIVATE ${RAPIDJSON_INCLUDE_DIRS})
endif()

if (USE_BROTLI)
    target_compile_definitions(ZenReader PRIVATE USE_BROTLI)
    find_package(unofficial-brotli CONFIG REQUIRED)
    if (USE_BROTLI_STATIC)
        target_link_libraries(ZenReader PRIVATE unofficial::brotli::brotlidec-static unofficial::brotli::brotlienc-static unofficial::brotli::brotlicommon-static)
    else()
         target_link_libraries(ZenReader PRIVATE unofficial::brotli::brotlidec unofficial::brotli::brotlienc unofficial::brotli::brotlicommon)
    endif()
endif()


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
