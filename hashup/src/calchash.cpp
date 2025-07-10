#include"hashup/calchash.h"

#include<fstream>

#include"openssl/evp.h"

namespace fs = std::filesystem;

/**
 * Calculate a specific hash of a file. File path & a `EVP_MD` pointer should be provided.
 * If fails, returns an empty string. Error msgs are written to log.
 */
std::string _calc_file_hash(const std::filesystem::path& __c_fp_path, const EVP_MD* __p__md){
    std::ifstream rfile(__c_fp_path, std::ios::binary);
    if (!rfile.is_open())
    {
        // here: log & warning should be shown
        return "";
    } // failed to open file

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (ctx == nullptr)
    {
        // here: log & warning should be shown
        rfile.close();
        return "";
    } // create mdctx failed
    int ret = EVP_DigestInit_ex(ctx, __p__md, nullptr);
    if (ret == 0)
    {
        // here: log & warning should be shown
        rfile.close();
        EVP_MD_CTX_free(ctx);
        return "";
    } // init digest failed
    char buf[1024];
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashlen;
    while (!rfile.eof())
    {
        rfile.read(buf, sizeof(buf));
        std::size_t size = rfile.gcount();
        ret = EVP_DigestUpdate(ctx, buf, size);
        if (ret == 0)
        {
            // here: log & warning should be shown
            rfile.close();
            EVP_MD_CTX_free(ctx);
            return "";
        } // update digest failed
    }
    ret = EVP_DigestFinal_ex(ctx, hash, &hashlen);
    if (ret == 0)
    {
        // here: log & warning should be shown
        rfile.close();
        EVP_MD_CTX_free(ctx);
        return "";
    } // finalize digest failed
    rfile.close();
    EVP_MD_CTX_free(ctx);
    
    std::string out;
    for (int i = 0 ; i < hashlen ; i++)
    {
        char hexstr[3];
        memset(hexstr, '\0', sizeof(hexstr));
        sprintf(hexstr, "%02x", hash[i]);
        out += hexstr;
    }

    return out;
}

/**
 * Calculate a specific hash of a string. File path & a `EVP_MD` pointer should be provided.
 * If fails, returns an empty string. Error msgs are written to log.
 */
std::string _calc_str_hash(const std::string& __c_s_str, const EVP_MD* __p__md){
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (ctx == nullptr)
    {
        // here: log & warning should be shown
        return "";
    } // create mdctx failed
    int ret = EVP_DigestInit_ex(ctx, __p__md, nullptr);
    if (ret == 0)
    {
        // here: log & warning should be shown
        EVP_MD_CTX_free(ctx);
        return "";
    } // init digest failed
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashlen;
    ret = EVP_DigestUpdate(ctx, __c_s_str.data(), __c_s_str.size());
    if (ret == 0)
    {
        // here: log & warning should be shown
        EVP_MD_CTX_free(ctx);
        return "";
    } // update digest failed
    ret = EVP_DigestFinal_ex(ctx, hash, &hashlen);
    if (ret == 0)
    {
        // here: log & warning should be shown
        EVP_MD_CTX_free(ctx);
        return "";
    } // finalize digest failed
    EVP_MD_CTX_free(ctx);

    std::string out;
    for (int i = 0 ; i < hashlen ; i++)
    {
        char hexstr[3];
        memset(hexstr, '\0', sizeof(hexstr));
        sprintf(hexstr, "%02x", hash[i]);
        out += hexstr;
    }

    return out;
}

std::string hashup::calc_file_hash_md5(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_md5());
}

std::string hashup::calc_file_hash_sha1(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_sha1());
}

std::string hashup::calc_file_hash_sha224(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_sha224());
}

std::string hashup::calc_file_hash_sha256(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_sha256());
}

std::string hashup::calc_file_hash_sha384(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_sha384());
}

std::string hashup::calc_file_hash_sha512(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_sha512());
}

std::string hashup::calc_file_hash_sha3_224(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_sha3_224());
}

std::string hashup::calc_file_hash_sha3_256(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_sha3_256());
}

std::string hashup::calc_file_hash_sha3_384(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_sha3_384());
}

std::string hashup::calc_file_hash_sha3_512(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_sha3_512());
}

std::string hashup::calc_file_hash_shake128(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_shake128());
}

std::string hashup::calc_file_hash_shake256(const fs::path& __c_fp_path){
    return _calc_file_hash(__c_fp_path, EVP_shake256());
}

std::string hashup::calc_str_hash_md5(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_md5());
}

std::string hashup::calc_str_hash_sha1(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_sha1());
}

std::string hashup::calc_str_hash_sha224(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_sha224());
}

std::string hashup::calc_str_hash_sha256(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_sha256());
}

std::string hashup::calc_str_hash_sha384(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_sha384());
}

std::string hashup::calc_str_hash_sha512(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_sha512());
}

std::string hashup::calc_str_hash_sha3_224(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_sha3_224());
}

std::string hashup::calc_str_hash_sha3_256(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_sha3_256());
}

std::string hashup::calc_str_hash_sha3_384(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_sha3_384());
}

std::string hashup::calc_str_hash_sha3_512(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_sha3_512());
}

std::string hashup::calc_str_hash_shake128(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_shake128());
}

std::string hashup::calc_str_hash_shake256(const std::string& __c_s_str){
    return _calc_str_hash(__c_s_str, EVP_shake256());
}
