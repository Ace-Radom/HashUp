cmake_minimum_required(VERSION 3.10)
project(HashUp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
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