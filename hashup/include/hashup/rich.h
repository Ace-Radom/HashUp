/**
 * This header is based on renarich.h <https://github.com/renalibs/renarich/blob/main/include/rena/renarich.h>
 * All WINAPI specific codes have been removed, since HashUp uses a global print worker and msgs must be written
 * into a sstream before being written to stdout / stderr. Implementation through WINAPI doesn't work, therefore
 * it has been deleted & rich output can only be shown with a terminal supports ANSI escape codes.
 */

#ifndef __HASHUP_RICH_H__
#define __HASHUP_RICH_H__

#include<memory>
#include<ostream>
#include<stack>

#include"macros.h"

namespace rena {

    typedef struct color_code {
        unsigned char _u8i_ansi;
        unsigned char _u8i_type;
        const char* _s_name;
        std::string to_string() const {
            return std::string(this->_s_name);
        }
    } color_code;
    inline bool operator==(const color_code& __c__lhs, const color_code& __c__rhs){
        return ((__c__lhs._u8i_ansi == __c__rhs._u8i_ansi) &&
                (__c__lhs._u8i_type == __c__rhs._u8i_type));
    }
    inline bool operator!=( const color_code& __c__lhs , const color_code& __c__rhs ){
        return ((__c__lhs._u8i_ansi != __c__rhs._u8i_ansi) ||
                (__c__lhs._u8i_type != __c__rhs._u8i_type ));
    }

    namespace builtin {

        constexpr unsigned char ColorCodeTypeForegroundColor = 0;
        constexpr unsigned char ColorCodeTypeBackgroundColor = 1;
        constexpr unsigned char ColorCodeTypeStyle = 2;

        class ccstack {

            public:
                ccstack(){}
                ~ccstack(){}

                inline void push(const color_code& __c__code){
                    if (this->_b_enable)
                    {
                        this->_cs.push(__c__code);
                    }
                    return;
                }

                inline void pop(){
                    if (this->_b_enable)
                    {
                        this->_cs.pop();
                    }
                    return;
                }

                void clear(){
                    if (this->_b_enable)
                    {
                        while (!(this->_cs.empty()))
                        {
                            this->_cs.pop();
                        }   
                    }
                    return;
                }

                inline bool enable() const noexcept {
                    return this->_b_enable;
                }

                inline void enable(bool __b_enable){
                    this->_b_enable = __b_enable;
                    return;
                }

                inline std::stack<color_code> copy() const {
                    return _cs;
                }

                static inline std::shared_ptr<ccstack> global(){
                    if (!_p__gccstack)
                    {
                        _p__gccstack = std::make_shared<ccstack>();
                    }
                    return _p__gccstack;
                }

            private:
                std::stack<color_code> _cs; // code stack
                bool _b_enable = true;

                static inline std::shared_ptr<ccstack> _p__gccstack = nullptr; // global color code stack

        }; // class ccstack

    } // namespace builtin

    class fcolor {

        public:
            DECLARE_STATIC_CLASS(fcolor);

            static constexpr color_code BLACK = {30, builtin::ColorCodeTypeForegroundColor, "black"};
            static constexpr color_code RED = {31, builtin::ColorCodeTypeForegroundColor, "red"};
            static constexpr color_code GREEN = {32, builtin::ColorCodeTypeForegroundColor, "green"};
            static constexpr color_code YELLOW = {33, builtin::ColorCodeTypeForegroundColor, "yellow"};
            static constexpr color_code BLUE = {34, builtin::ColorCodeTypeForegroundColor, "blue"};
            static constexpr color_code MAGENTA = {35, builtin::ColorCodeTypeForegroundColor, "magenta"};
            static constexpr color_code CYAN = {36, builtin::ColorCodeTypeForegroundColor, "cyan"};
            static constexpr color_code WHITE = {37, builtin::ColorCodeTypeForegroundColor, "white"};
            static constexpr color_code GRAY = {90, builtin::ColorCodeTypeForegroundColor, "gray"};
            static constexpr color_code BRIGHTRED = {91, builtin::ColorCodeTypeForegroundColor, "bright red"};
            static constexpr color_code BRIGHTGREEN = {92, builtin::ColorCodeTypeForegroundColor, "bright green"};
            static constexpr color_code BRIGHTYELLOW = {93, builtin::ColorCodeTypeForegroundColor, "bright yellow"};
            static constexpr color_code BRIGHTBLUE = {94, builtin::ColorCodeTypeForegroundColor, "bright blue"};
            static constexpr color_code BRIGHTMAGENTA = {95, builtin::ColorCodeTypeForegroundColor, "bright magenta"};
            static constexpr color_code BRIGHTCYAN = {96, builtin::ColorCodeTypeForegroundColor, "bright cyan"};
            static constexpr color_code BRIGHTWHITE = {97, builtin::ColorCodeTypeForegroundColor, "bright white"};

    }; // class fcolor

    class bcolor {

        public:
            DECLARE_STATIC_CLASS(bcolor);

            static constexpr color_code BLACK = {40, builtin::ColorCodeTypeBackgroundColor, "black"};
            static constexpr color_code RED = {41, builtin::ColorCodeTypeBackgroundColor, "red"};
            static constexpr color_code GREEN = {42, builtin::ColorCodeTypeBackgroundColor, "green"};
            static constexpr color_code YELLOW = {43, builtin::ColorCodeTypeBackgroundColor, "yellow"};
            static constexpr color_code BLUE = {44, builtin::ColorCodeTypeBackgroundColor, "blue"};
            static constexpr color_code MAGENTA = {45, builtin::ColorCodeTypeBackgroundColor, "magenta"};
            static constexpr color_code CYAN = {46, builtin::ColorCodeTypeBackgroundColor, "cyan"};
            static constexpr color_code WHITE = {47, builtin::ColorCodeTypeBackgroundColor, "white"};
            static constexpr color_code GRAY = {100, builtin::ColorCodeTypeBackgroundColor, "gray"};
            static constexpr color_code BRIGHTRED = {101, builtin::ColorCodeTypeBackgroundColor, "bright red"};
            static constexpr color_code BRIGHTGREEN = {102, builtin::ColorCodeTypeBackgroundColor, "bright green"};
            static constexpr color_code BRIGHTYELLOW = {103, builtin::ColorCodeTypeBackgroundColor, "bright yellow"};
            static constexpr color_code BRIGHTBLUE = {104, builtin::ColorCodeTypeBackgroundColor, "bright blue"};
            static constexpr color_code BRIGHTMAGENTA = {105, builtin::ColorCodeTypeBackgroundColor, "bright magenta"};
            static constexpr color_code BRIGHTCYAN = {106, builtin::ColorCodeTypeBackgroundColor, "bright cyan"};
            static constexpr color_code BRIGHTWHITE = {107, builtin::ColorCodeTypeBackgroundColor, "bright white"};

    }; // class bcolor

    class fstyle {

        public:
            DECLARE_STATIC_CLASS(fstyle);

            static constexpr color_code BOLD = {1, builtin::ColorCodeTypeStyle, "bold"};
            static constexpr color_code DIM = {2, builtin::ColorCodeTypeStyle, "dim"};
            static constexpr color_code ITALIC = {3, builtin::ColorCodeTypeStyle, "italic"};
            static constexpr color_code UNDERLINE = {4, builtin::ColorCodeTypeStyle, "underline"};
            static constexpr color_code OVERLINE = {53, builtin::ColorCodeTypeStyle, "overline"};
            static constexpr color_code STRIKE = {9, builtin::ColorCodeTypeStyle, "strike"};
            static constexpr color_code REVERSE = {7, builtin::ColorCodeTypeStyle, "reverse"};

    }; // class fstyle

    template<class _Elem, class _Traits>
    std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& __os, const color_code& __c__code){
        __os << "\033[" << static_cast<int>(__c__code._u8i_ansi) << "m";
        builtin::ccstack::global()->push(__c__code);
        return __os;
    }

    typedef struct _s_rich_pop {
        unsigned int _ui_num;
    } _s_rich_pop;
    
    inline _s_rich_pop rich_pop(unsigned int __ui_num = 1){
        return { __ui_num };
    }

    template<class _Elem, class _Traits>
    std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& __os, _s_rich_pop __elem){
        __os << "\033[0m" << std::flush;
        for (unsigned int i = 0 ; i < __elem._ui_num ; i++)
        {
            builtin::ccstack::global()->pop();
        }
        auto code_stack = builtin::ccstack::global()->copy();
        builtin::ccstack::global()->enable(false);
        // disable global ccstack before reverting older color codes
        // otherwise older color codes will be pushed into stack again
        while (!code_stack.empty())
        {
            __os << code_stack.top();
            code_stack.pop();
        }
        builtin::ccstack::global()->enable(true);
        return __os;
    }

    template<class _Elem, class _Traits>
    std::basic_ostream<_Elem, _Traits>& rich_reset(std::basic_ostream<_Elem, _Traits>& __os){
        __os << "\033[0m" << std::flush;
        builtin::ccstack::global()->clear();
        return __os;
    }

    template<class _Elem, class _Traits>
    std::basic_ostream<_Elem, _Traits>& clear_line(std::basic_ostream<_Elem, _Traits>& __os){
        __os << "\033[2K\r" << std::flush;
        return __os;
    }

} // namespace rena

#endif // __HASHUP_RICH_H__