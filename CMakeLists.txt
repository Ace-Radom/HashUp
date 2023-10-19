cmake_minimum_required(VERSION 3.10)
project(HashUp VERSION 1.0.0 LANGUAGES CXX)

option(USE_OPENSSL_EVP "Use OpenSSL EVP functions to calculate file hashes and enable sha3" ON)

# ============================================
#                 build config
# ============================================

find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

string(TIMESTAMP BUILD_TIME %Y.%m.%d)

configure_file(
    BUILD_CONFIG.in
    ${CMAKE_SOURCE_DIR}/include/build_config.h
    @ONLY
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

if(${USE_OPENSSL_EVP})
    add_definitions(-DUSE_OPENSSL_EVP)
endif()

# ============================================
#                search OpenSSL
# ============================================

find_package(OpenSSL REQUIRED)
if(NOT OPENSSL_FOUND)
    message(FATAL "Cannot found OpenSSL, please check environment")
endif()
include_directories(${OPENSSL_INCLUDE_DIR})

# ============================================
#                     main
# ============================================

add_definitions(-DSHOW_PROGRESS_DETAIL)
include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(src)

add_executable(hashup main.cpp)
target_link_libraries(hashup
    hashuplib
)

install(
    TARGETS hashup
    RUNTIME DESTINATION bin
)