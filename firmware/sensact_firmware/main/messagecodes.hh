#pragma once
#include <inttypes.h>

namespace messagecodes
{
#define DEF(thecode, thestring) thecode
#define DEF_(thecode) thecode
    enum class C:uint32_t
    {
#include "messagecodes_default.inc"
#if __has_include(<messagecodes_user.inc>)
#include <messagecodes_user.inc>
#endif
    };
#undef DEF
#undef DEF_
#define DEF(thecode, thestring) thestring
#define DEF_(thecode) #thecode
    constexpr const char *N[] = {
#include "messagecodes_default.inc"
#if __has_include(<messagecodes_user.inc>)
#include <messagecodes_user.inc>
#endif

    };
#undef DEF
#undef DEF_
}
