add_library(hashuplib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/calchash.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hufo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/speedwatch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cppfignore.cpp
)
if(UNIX)
    target_link_libraries(hashuplib
        pthread
    )
endif()
add_library(inireader STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ini.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/INIReader.cpp
)