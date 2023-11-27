add_library(rich STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/rich.cpp
)
add_library(renalog SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/renalog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renalog_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/renalog_cap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/crashdumper.cpp
)
target_link_libraries(renalog
    rich  # for functions in rich.h and utils.h
)
target_compile_definitions(renalog
    PRIVATE
        RENALOGAPI_EXPORT
)
set_target_properties(renalog
    PROPERTIES
        VERSION   1.0.0
        SOVERSION 1
)
add_library(hashuplib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/calchash.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hufo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/speedwatch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cppfignore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kbs.cpp
)
target_link_libraries(hashuplib
    rich
    renalog
    OpenSSL::Crypto
)
if(UNIX)
    target_link_libraries(hashuplib
        pthread
    )
endif()
