#pragma warning(disable : 4996)
#include <locale>   // for wstring_convert<>
#include <codecvt>  // for codecvt_utf8<>

#include "unicode.h"


namespace asc::cpp_practice_ws20::ex08 {


    std::string
        to_utf8(std::u32string const& s)
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
        return conv.to_bytes(s);
    }


} // namespace asc::cpp_practice_ws20::ex08
