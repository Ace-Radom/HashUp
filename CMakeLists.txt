cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(hashup VERSION 2.0.0 LANGUAGES CXX)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0077 "NEW")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENSSL_MIN_VER "3.0.0")
find_package(OpenSSL ${OPENSSL_MIN_VER} COMPONENTS Crypto SSL REQUIRED)

add_subdirectory(third_party)
add_subdirectory(hashup)
