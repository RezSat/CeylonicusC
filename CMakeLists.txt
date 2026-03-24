cmake_minimum_required(VERSION 3.10)
project(ceylonicus C)

set(CMAKE_C_STANDARD 99)

# LLVM integration via DLL
set(LLVM_ROOT "C:/CompInfra/clang_llvm")
include_directories("${LLVM_ROOT}/include")

add_executable(ceylonicus 
    src/main.c 
    src/lexer.c
    src/utf8.c
    src/keywords.c
)

# 3. Include Directories
# This replaces the old include_directories and targets the specific executable
target_include_directories(ceylonicus PRIVATE 
    src
    src/include           # Path to your header files
    "${LLVM_ROOT}/include" # Path to LLVM headers
)

# 4. Linking LLVM
# Ensure the target name matches 'ceylonicus'
target_link_libraries(ceylonicus PRIVATE "${LLVM_ROOT}/lib/LLVM-C.lib")

# Optional: Add compiler flags for Windows (MSVC) or GCC/Clang
if(MSVC)
    target_compile_options(ceylonicus PRIVATE /W4)
else()
    target_compile_options(ceylonicus PRIVATE -Wall -Wextra)
endif()