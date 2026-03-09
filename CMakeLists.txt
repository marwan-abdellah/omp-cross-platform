cmake_minimum_required(VERSION 3.20)
project(OpenMP_MacOS_Demo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(array_multiply main.cpp)

if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Homebrew libomp paths (ARM: /opt/homebrew, Intel: /usr/local)
    if(EXISTS "/opt/homebrew/opt/libomp")
        set(OpenMP_ROOT "/opt/homebrew/opt/libomp")
    else()
        set(OpenMP_ROOT "/usr/local/opt/libomp")
    endif()

    target_compile_options(array_multiply PRIVATE -Xpreprocessor -fopenmp -O3)
    target_include_directories(array_multiply PRIVATE "${OpenMP_ROOT}/include")
    target_link_directories(array_multiply PRIVATE "${OpenMP_ROOT}/lib")
    target_link_libraries(array_multiply PRIVATE omp)
else()
    # Linux and other platforms
    find_package(OpenMP REQUIRED)
    target_link_libraries(array_multiply PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(array_multiply PRIVATE -O3)
endif()