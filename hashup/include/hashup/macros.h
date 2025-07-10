#ifndef __HASHUP_MACROS_H__
#define __HASHUP_MACROS_H__

#define DECLARE_STATIC_CLASS(class)             \
    class() = delete;                           \
    class(const class&) = delete;               \
    class(class&&) = delete;                    \
    class& operator=(const class&) = delete;    \
    class& operator=(class&&) = delete;         \
    ~class() = delete

#endif // __HASHUP_MACROS_H__
