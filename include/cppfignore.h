#ifndef _CPPFIGNORE_H_
#define _CPPFIGNORE_H_

#include<iostream>
#include<fstream>
#include<regex>
#include<filesystem>

namespace rena {

    class rena_exception : public std::exception {

        public:
            rena_exception( std::string msg ) : _msg( msg ){};
            const char* what() const noexcept {
                return this -> _msg.c_str();
            }

        private:
            std::string _msg;

    }; // struct rena_exception

    class cppfignore {

        public:
            typedef struct {
                std::regex regex;
                bool       ignore_when_match;
            } ignore_list;

        public:
            cppfignore(){};
            ~cppfignore();

            void open( std::filesystem::path ifp );
            int parse();
            bool check( std::filesystem::path path );

        private:
            std::filesystem::path   _ifp;       // ignore file path
            std::fstream            _rwFile;
            std::vector<std::regex> _rlist;     // regex list

            bool _check_pp( std::filesystem::path path );

    }; // class cppfignore

}; // namespace rena

#endif