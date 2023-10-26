find_package(Python3 REQUIRED)
if(NOT PYTHON3_FOUND)
    message(FATAL "Cannot find Python3, cannot make tests")
endif()

add_definitions(-DTEST_SINGLE_FILE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/TEST_SINGLE")
add_executable(hash_calc_test
    ${CMAKE_CURRENT_SOURCE_DIR}/hash_calc_test.cpp
)
target_link_libraries(hash_calc_test
    hashuplib
)

add_test(
    NAME Check_All_Hash_Functions
    COMMAND $<TARGET_FILE:hash_calc_test>
)
add_test(
    NAME Generate_Test_Directory
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/testfile_gen.py ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME Call_Hash_Create
    COMMAND $<TARGET_FILE:hashup> -w -f ${CMAKE_CURRENT_SOURCE_DIR}/test_dir/test_dir.md5 -m md5 --overlay
)
add_test(
    NAME Call_Hash_Check
    COMMAND $<TARGET_FILE:hashup> -r -f ${CMAKE_CURRENT_SOURCE_DIR}/test_dir/test_dir.md5 -m md5
)
add_test(
    NAME Check_Created_Hash_List
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/check_hashlist.py ${CMAKE_CURRENT_SOURCE_DIR}/test_dir
)
add_test(
    NAME Call_File_Ignore_Function
    COMMAND $<TARGET_FILE:hashup> -w -f ${CMAKE_CURRENT_SOURCE_DIR}/test_dir/test_dir.md5 -m md5 --overlay --ignore ${CMAKE_CURRENT_SOURCE_DIR}/test_dir/.hashupignore
)
add_test(
    NAME Test_File_Ignore_Function
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/check_ignore_func.py ${CMAKE_CURRENT_SOURCE_DIR}/test_dir
)