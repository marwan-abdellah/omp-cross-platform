cmake_minimum_required(VERSION 3.20)
project(OpenMP_MacOS_Demo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_OPENMP "Enable OpenMP support" ON)

add_executable(array_multiply main.cpp)
target_compile_options(array_multiply PRIVATE -O3)

if(NOT ENABLE_OPENMP)
    message(STATUS "OpenMP disabled by user")
    return()
endif()

include(CheckCXXCompilerFlag)

# ---- macOS + AppleClang ------------------------------------------------------
if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    if(EXISTS "/opt/homebrew/opt/libomp")
        set(OpenMP_ROOT "/opt/homebrew/opt/libomp")
    elseif(EXISTS "/usr/local/opt/libomp")
        set(OpenMP_ROOT "/usr/local/opt/libomp")
    else()
        message(FATAL_ERROR
            "AppleClang detected, but libomp was not found.\n"
            "Install it with: brew install libomp")
    endif()

    if(NOT EXISTS "${OpenMP_ROOT}/include/omp.h")
        message(FATAL_ERROR "OpenMP header not found at: ${OpenMP_ROOT}/include/omp.h")
    endif()

    find_library(OPENMP_RUNTIME_LIBRARY
        NAMES omp libomp
        PATHS "${OpenMP_ROOT}/lib"
        NO_DEFAULT_PATH
    )

    if(NOT OPENMP_RUNTIME_LIBRARY)
        message(FATAL_ERROR "OpenMP runtime library not found under: ${OpenMP_ROOT}/lib")
    endif()

    target_compile_options(array_multiply PRIVATE -Xpreprocessor -fopenmp)
    target_include_directories(array_multiply PRIVATE "${OpenMP_ROOT}/include")
    target_link_libraries(array_multiply PRIVATE "${OPENMP_RUNTIME_LIBRARY}")

    message(STATUS "Using AppleClang + libomp from ${OpenMP_ROOT}")

# ---- Other platforms / compilers ---------------------------------------------
else()
    # First try the standard CMake mechanism.
    find_package(OpenMP QUIET COMPONENTS CXX)

    if(OpenMP_CXX_FOUND)
        target_link_libraries(array_multiply PRIVATE OpenMP::OpenMP_CXX)
        message(STATUS "Using OpenMP via CMake imported target")
    else()
        message(WARNING "find_package(OpenMP) failed, trying manual fallback")

        # GCC usually uses -fopenmp and links libgomp automatically.
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            check_cxx_compiler_flag("-fopenmp" COMPILER_SUPPORTS_FOPENMP)
            if(NOT COMPILER_SUPPORTS_FOPENMP)
                message(FATAL_ERROR "GCC detected, but -fopenmp is not supported")
            endif()

            target_compile_options(array_multiply PRIVATE -fopenmp)
            target_link_options(array_multiply PRIVATE -fopenmp)

            message(STATUS "Using manual GCC OpenMP fallback (-fopenmp)")

        # Upstream Clang on Linux often needs libomp available.
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            check_cxx_compiler_flag("-fopenmp" COMPILER_SUPPORTS_FOPENMP)
            if(COMPILER_SUPPORTS_FOPENMP)
                target_compile_options(array_multiply PRIVATE -fopenmp)
                target_link_options(array_multiply PRIVATE -fopenmp)
                message(STATUS "Using manual Clang OpenMP fallback (-fopenmp)")
            else()
                # Common Linux fallback locations for libomp
                find_path(OPENMP_INCLUDE_DIR
                    NAMES omp.h
                    PATHS
                        /usr/include
                        /usr/local/include
                        /usr/lib/llvm/include
                        /usr/lib64/llvm/include
                        /usr/lib/llvm-18/include
                        /usr/lib/llvm-17/include
                        /usr/lib/llvm-16/include
                        /usr/lib/llvm-15/include
                )

                find_library(OPENMP_RUNTIME_LIBRARY
                    NAMES omp libomp
                    PATHS
                        /usr/lib
                        /usr/local/lib
                        /usr/lib64
                        /usr/lib/llvm/lib
                        /usr/lib64/llvm/lib
                        /usr/lib/llvm-18/lib
                        /usr/lib/llvm-17/lib
                        /usr/lib/llvm-16/lib
                        /usr/lib/llvm-15/lib
                )

                if(OPENMP_INCLUDE_DIR AND OPENMP_RUNTIME_LIBRARY)
                    target_compile_options(array_multiply PRIVATE -fopenmp)
                    target_include_directories(array_multiply PRIVATE "${OPENMP_INCLUDE_DIR}")
                    target_link_libraries(array_multiply PRIVATE "${OPENMP_RUNTIME_LIBRARY}")
                    message(STATUS "Using manual Clang OpenMP fallback with explicit libomp")
                else()
                    message(FATAL_ERROR
                        "Clang detected, but OpenMP was not found.\n"
                        "Install libomp/libomp-devel, or use a compiler/toolchain with OpenMP enabled.")
                endif()
            endif()

        else()
            message(FATAL_ERROR
                "OpenMP configuration failed for compiler: ${CMAKE_CXX_COMPILER_ID}")
        endif()
    endif()
endif()