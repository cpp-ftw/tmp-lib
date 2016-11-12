#ifndef TMP_LIB_CONSTEXPR_STR_HPP_INCLUDED
#define TMP_LIB_CONSTEXPR_STR_HPP_INCLUDED

#include <array>
#include <iostream>

namespace tmp_lib
{

template<char... CHARS>
struct constexpr_str
{
    static constexpr char value[] = {CHARS..., '\0'};
};

template<char... CHARS>
constexpr char constexpr_str<CHARS...>::value[];

template <int... N, typename STR>
constexpr constexpr_str<STR::get(N)...> constexpr_str_make(seq<N...>, STR)
{
    return {};
}

#define TMP_LIB_CONSTEXPR_STR(LITERAL)                                          \
    tmp_lib::constexpr_str_make(tmp_lib::make_seq<sizeof(LITERAL)-1>(), []{     \
        struct Literal                                                          \
        {                                                                       \
            static constexpr char get(int i) { return LITERAL[i]; }             \
        };                                                                      \
        return Literal{};                                                       \
    }())

}

#endif // TMP_LIB_CONSTEXPR_STR_HPP_INCLUDED
