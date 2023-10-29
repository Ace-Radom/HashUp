#ifndef _CPPFIGNORE_H_
#define _CPPFIGNORE_H_

#include<iostream>
#include<fstream>
#include<regex>
#include<filesystem>

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
            std::filesystem::path   _ifp;       // ignore file path
            std::fstream            _rwFile;
            std::vector<std::regex> _rlist;     // regex list

            bool _check_pp( std::filesystem::path path , FILETYPE ft );

    }; // class cppfignore

}; // namespace rena

#endif