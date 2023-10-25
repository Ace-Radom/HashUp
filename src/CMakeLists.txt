add_library(hashuplib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/calchash.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hufo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/speedwatch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cppfignore.cpp
)
target_link_libraries(hashuplib
    OpenSSL::SSL
    OpenSSL::Crypto
)
if(UNIX)
target_link_libraries(hashuplib
    pthread
)
endif()