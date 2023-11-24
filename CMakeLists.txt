cmake_minimum_required(VERSION 3.10)
project(HashUp VERSION 1.5.1 LANGUAGES C CXX)

option(USE_OPENSSL_EVP_FUNCS "Use OpenSSL EVP functions to calculate file hashes and enable sha3" ON)
option(MAKE_TEST             "Make tests"                                                         OFF)

# ============================================
#                search OpenSSL
# ============================================

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
if(NOT OPENSSL_FOUND)
    message(FATAL "Cannot find OpenSSL, please check environment")
endif()

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
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
endif()

if(${USE_OPENSSL_EVP_FUNCS})
    if(${OPENSSL_VERSION} VERSION_GREATER_EQUAL "1.1.1")
        add_definitions(-DUSE_OPENSSL_EVP)
    else()
        message(STATUS "Cannot turn on OpenSSL EVP with OpenSSL older than version 1.1.1. If you want to, please edit the CMakeLists.")
    endif()
endif()
add_definitions(-DSHOW_PROGRESS_DETAIL)
add_definitions(-DSOURCE_ROOT_DIR="${CMAKE_SOURCE_DIR}")

# ============================================
#                     main
# ============================================

include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(include)
add_subdirectory(src)

add_executable(hashup main.cpp)
target_link_libraries(hashup
    hashuplib
)

if(${MAKE_TEST})
    enable_testing()
    add_subdirectory(test)
endif()


# ============================================
#                install & pack
# ============================================

install(
    TARGETS hashup
    RUNTIME DESTINATION bin
)
install(
    FILES ${CMAKE_SOURCE_DIR}/hashup.ini
    DESTINATION bin
)
install(
    FILES ${CMAKE_SOURCE_DIR}/LICENSE.txt ${CMAKE_SOURCE_DIR}/README.md
    DESTINATION .
)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Ace-Radom")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
set(CPACK_PACKAGE_DISCRIPTION_SUMMARY "HashUp - a lightweight cross-platform command-line file hash generation and checking tool")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)
set(CPACK_WIX_UPGRADE_GUID "A76C4007-9B84-4CEC-8882-6A5F1B615B76")
set(CPACK_WIX_PRODUCT_GUID "A76C4007-9B84-4CEC-8882-6A5F1B615B68")
set(CPACK_WIX_PRODUCT_ICON ${CMAKE_SOURCE_DIR}/hashup.ico)
set(CPACK_WIX_PATCH_FILE ${CMAKE_SOURCE_DIR}/wix-patch.xml)
include(CPack)