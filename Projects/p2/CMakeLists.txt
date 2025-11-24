cmake_minimum_required(VERSION 3.16)
project(lemondb LANGUAGES CXX)

# Force clang-18 if available
set(CMAKE_CXX_COMPILER clang++-18)
set(CMAKE_C_COMPILER clang-18)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use libc++ for all targets (required for MemorySanitizer)
add_compile_options(-stdlib=libc++)
add_link_options(-stdlib=libc++ -lc++abi)

# Compiler warnings
add_compile_options(-Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wsign-conversion -Wno-unused-result)

# Organize sources by component: db, query, utils, and the top-level main
file(GLOB DB_SOURCES ${CMAKE_SOURCE_DIR}/src/db/*.cpp)
file(GLOB QUERY_SOURCES
    ${CMAKE_SOURCE_DIR}/src/query/*.cpp
    ${CMAKE_SOURCE_DIR}/src/query/data/*.cpp
    ${CMAKE_SOURCE_DIR}/src/query/management/*.cpp
)
file(GLOB UTIL_SOURCES ${CMAKE_SOURCE_DIR}/src/utils/*.cpp)
set(MAIN_SRC ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Create libraries only if sources are present; otherwise create INTERFACE targets
add_library(db STATIC ${DB_SOURCES})
add_library(query STATIC ${QUERY_SOURCES})
target_include_directories(db PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(query PUBLIC ${CMAKE_SOURCE_DIR})

# query depends on db; link them so transitive symbols are resolved
target_link_libraries(query PUBLIC db)

if(UTIL_SOURCES)
    add_library(utils STATIC ${UTIL_SOURCES})
    target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR})
else()
    add_library(utils INTERFACE)
    target_include_directories(utils INTERFACE ${CMAKE_SOURCE_DIR})
endif()

# ================================
# Memory-sanitized libraries
# ================================
add_library(db_msan STATIC ${DB_SOURCES})
add_library(query_msan STATIC ${QUERY_SOURCES})
target_include_directories(db_msan PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(query_msan PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(query_msan PUBLIC db_msan)

target_compile_options(db_msan PUBLIC
    -fsanitize=memory
    -fsanitize=undefined
    -fsanitize=signed-integer-overflow
    -fno-omit-frame-pointer
    -fPIE
)

target_compile_options(query_msan PUBLIC
    -fsanitize=memory
    -fsanitize=undefined
    -fsanitize=signed-integer-overflow
    -fno-omit-frame-pointer
    -fPIE
)

if(UTIL_SOURCES)
    add_library(utils_msan STATIC ${UTIL_SOURCES})
    target_include_directories(utils_msan PUBLIC ${CMAKE_SOURCE_DIR})
    target_compile_options(utils_msan PUBLIC
        -fsanitize=memory
        -fsanitize=undefined
        -fsanitize=signed-integer-overflow
        -fno-omit-frame-pointer
        -fPIE
    )
else()
    add_library(utils_msan INTERFACE)
    target_include_directories(utils_msan INTERFACE ${CMAKE_SOURCE_DIR})
endif()


add_executable(lemondb ${MAIN_SRC})
target_link_libraries(lemondb PRIVATE utils query)

# Place resulting binary in build/bin
set_target_properties(lemondb PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Convenience: allow out-of-source builds
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Memory check executable with sanitizers
add_executable(lemondb_memory_check ${MAIN_SRC})
target_compile_options(lemondb_memory_check PUBLIC
    -DDEBUG -g
    -fsanitize=memory
    -fsanitize=undefined
    -fsanitize=signed-integer-overflow
    -fno-omit-frame-pointer
    -fPIE
    -O2
)

target_link_libraries(lemondb_memory_check PRIVATE utils_msan query_msan
    -fsanitize=memory
    -fsanitize=undefined
    -fsanitize=signed-integer-overflow
)
set_target_properties(lemondb_memory_check PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# ================================
# AddressSanitizer build
# ================================
add_executable(lemondb_asan ${MAIN_SRC})
target_link_libraries(lemondb_asan PRIVATE utils query)

target_compile_options(lemondb_asan PUBLIC
    -fsanitize=address
    -fno-omit-frame-pointer
)

target_link_options(lemondb_asan PUBLIC
    -fsanitize=address
)

set_target_properties(lemondb_asan PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# ================================
# UndefinedBehaviorSanitizer build
# ================================
add_executable(lemondb_ubsan ${MAIN_SRC})
target_link_libraries(lemondb_ubsan PRIVATE utils query)

target_compile_options(lemondb_ubsan PUBLIC
    -fsanitize=undefined
    -fno-omit-frame-pointer
)

target_link_options(lemondb_ubsan PUBLIC
    -fsanitize=undefined
)

set_target_properties(lemondb_ubsan PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
