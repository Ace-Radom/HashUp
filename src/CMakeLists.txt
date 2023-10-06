add_library(hashup STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/calchash.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hufo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
)
target_link_libraries(hashup
    OpenSSL::SSL
    OpenSSL::Crypto
)