add_library(hash STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/hash.cpp
)
target_link_libraries(hash
    OpenSSL::SSL
    OpenSSL::Crypto
)