add_library(hashup-core SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/calchash.cpp
)
target_include_directories(hashup-core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${OPENSSL_INCLUDE_DIR}
)
