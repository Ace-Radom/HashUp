cmake_minimum_required(VERSION 3.10)
project(HashUp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(src)
