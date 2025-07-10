#pragma once

#ifndef __HASHUP_CALCHASH_H__
#define __HASHUP_CALCHASH_H__

#include<filesystem>
#include<string>

namespace hashup {

    std::string calc_file_hash_md5(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_sha1(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_sha224(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_sha256(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_sha384(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_sha512(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_sha3_224(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_sha3_256(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_sha3_384(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_sha3_512(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_shake128(const std::filesystem::path& __c_fp_path);
    std::string calc_file_hash_shake256(const std::filesystem::path& __c_fp_path);
    std::string calc_str_hash_md5(const std::string& __c_s_str);
    std::string calc_str_hash_sha1(const std::string& __c_s_str);
    std::string calc_str_hash_sha224(const std::string& __c_s_str);
    std::string calc_str_hash_sha256(const std::string& __c_s_str);
    std::string calc_str_hash_sha384(const std::string& __c_s_str);
    std::string calc_str_hash_sha512(const std::string& __c_s_str);
    std::string calc_str_hash_sha3_224(const std::string& __c_s_str);
    std::string calc_str_hash_sha3_256(const std::string& __c_s_str);
    std::string calc_str_hash_sha3_384(const std::string& __c_s_str);
    std::string calc_str_hash_sha3_512(const std::string& __c_s_str);
    std::string calc_str_hash_shake128(const std::string& __c_s_str);
    std::string calc_str_hash_shake256(const std::string& __c_s_str);

} // namespace hashup

#endif // __HASHUP_CALCHASH_H__
