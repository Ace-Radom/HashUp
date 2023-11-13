#include"hashup.h"

int main(){

    RENALOG_INIT(
        std::filesystem::path( rena::get_current_exe_path() ).parent_path() ,
        CPTEXT( "hash_calc_test" ) ,
        4 ,
        rena::renalog::RENALOGSEVERITY::DEBUG
    );

    if (
        rena::calc_file_md5( TEST_SINGLE_FILE_PATH )      == CPATOWCONV( "1c9a06711a8076d5d47a3ecb6ceb2431" )                                                                                                 &&
        rena::calc_file_sha1( TEST_SINGLE_FILE_PATH )     == CPATOWCONV( "1a96f7a27006c30ebd764b5490b3d5d681d40008" )                                                                                         &&
        rena::calc_file_sha224( TEST_SINGLE_FILE_PATH )   == CPATOWCONV( "45f6d1b138a5179e0fa16ab2758b55edbaf3a3b1d118438538bc920a" )                                                                         &&
        rena::calc_file_sha256( TEST_SINGLE_FILE_PATH )   == CPATOWCONV( "00ebeee73921a1888ee246195202ea57087b68f81c776217e2c6939cd43c3413" )                                                                 &&
        rena::calc_file_sha384( TEST_SINGLE_FILE_PATH )   == CPATOWCONV( "237f5aff00b8881429b91b682d183d596f7df4b28d54f67eaa29376eaaddcb51c8e52ce3135a8ac42ca24ed19b9543a8" )                                 &&
        rena::calc_file_sha512( TEST_SINGLE_FILE_PATH )   == CPATOWCONV( "8b09adfd5d74298eedee748d64d2eadc883f0210903614dbd7300c911f96a8e5925fffb654381a0d8be99bd632479a2596e5c24c598106a9cfb4b42504a0fcd9" )
#ifdef USE_OPENSSL_EVP
        &&
        rena::calc_file_sha3_224( TEST_SINGLE_FILE_PATH ) == CPATOWCONV( "416d9330016685d0efa88bfe71e9180da63df8cfb50918a11c69d688" )                                                                         &&
        rena::calc_file_sha3_256( TEST_SINGLE_FILE_PATH ) == CPATOWCONV( "671ab4d006457fb34ebdd22367fbc3e524d52621ee0540c5158ed10469443c13" )                                                                 &&
        rena::calc_file_sha3_384( TEST_SINGLE_FILE_PATH ) == CPATOWCONV( "34adb0a51b911f6880fa3187910c3abf715c4b8fc927fbb12e73997779445407591cde02ef483c35f23fd1438d80ec79" )                                 &&
        rena::calc_file_sha3_512( TEST_SINGLE_FILE_PATH ) == CPATOWCONV( "1b15b3282934b705fa31b880131d0fbdca4c080df1ddaa1c1a95adfadde1796c96f740c4bc30751a7a8c62a59b506cc711e0d9c8008518f12f92ef3661fe9f7f" ) &&
        rena::calc_file_shake128( TEST_SINGLE_FILE_PATH ) == CPATOWCONV( "55792982cdea2858cce6ca25bb58d272" )                                                                                                 &&
        rena::calc_file_shake256( TEST_SINGLE_FILE_PATH ) == CPATOWCONV( "29dfe7131294bd3806c98cd6478c3864d6f1f1b767f7fc4e51cf1a06516b8a52" )
#endif
    )
    {
        RENALOG_FREE();
        return 0;
    }
    else
    {
        RENALOG_FREE();
        return 1;
    }    
}