#ifndef _CPPFIGNORE_H_
#define _CPPFIGNORE_H_

#include<iostream>
#include<fstream>
#include<regex>
#include<utility>
#include<filesystem>

#include"renalog.h"
#include"utils.h"

namespace rena {

    class cppfignore {

        public:
            typedef enum {
                IS_DIR,
                IS_FILE
            } FILETYPE;

        public:
            cppfignore(){};
            ~cppfignore();

            int open( std::filesystem::path ifp );
            int parse();
            bool check( std::filesystem::path path , FILETYPE ft );

        private:
            std::filesystem::path                          _ifp;       // ignore file path
            std::fstream                                   _rwFile;
            std::vector<std::pair<std::regex,std::string>> _rlist;     // regex list (pattern,original pattern str)
            // save original str for logger (std::basic_regex does not provide .str() any more [N1499])

            bool _check_pp( std::filesystem::path path , FILETYPE ft );

    }; // class cppfignore

}; // namespace rena

#endif