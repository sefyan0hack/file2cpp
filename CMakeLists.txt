cmake_minimum_required(VERSION 3.28)

project(
    file2cpp 
    LANGUAGES CXX 
    VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_USE_RELATIVE_PATHS true)

####################################################################################################
# Emscripten-specific setup 
####################################################################################################

if(EMSCRIPTEN)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    add_compile_options(
        -Oz
        -Wno-character-conversion -Wno-dollar-in-identifier-extension
        -msimd128
        -fPIC
    )

    add_link_options(
        -Oz
        -sALLOW_MEMORY_GROWTH=1
        -sDISABLE_EXCEPTION_THROWING=0
        -sDISABLE_EXCEPTION_CATCHING=0
        --embed-file ${CMAKE_SOURCE_DIR}/res
    )
endif()


####################################################################################################
# Build Exe 
####################################################################################################

add_executable(${PROJECT_NAME} main.cpp)
