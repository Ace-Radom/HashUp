cmake_minimum_required(VERSION 3.10)
project(HashUp)

set(CXX_STANDARD_REQUIRED ON)
set(CXX_STANDARD 11)

# ============================================
#                search OpenSSL
# ============================================

find_package(OpenSSL REQUIRED)
if(NOT OPENSSL_FOUND)
    message(FATAL "Cannot found OpenSSL, please check environment")
endif()
include_directories(${OPENSSL_INCLUDE_DIR})

if(UNIX)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
endif()
if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(src)