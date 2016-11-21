#ifndef SMART_ENUM_HPP_INCLUDED
#define SMART_ENUM_HPP_INCLUDED

#include "utilities.hpp"
#include "algorithm.hpp"

#include <stdexcept>

namespace tmp_lib
{

namespace internal
{
    template<typename T>
    constexpr bool lookup_find_element(std::pair<T, const char*> a, std::pair<T, const char*> b)
    {
        return a.first == b.first;
    }

    template<std::size_t N, typename T2, std::size_t N2, typename ENUM, typename S, int... SEQ>
    constexpr std::array<const char*, N> lookup_generate_helper(const std::array<T2, N2> arr, ENUM m_min, ENUM m_max, S def, seq<SEQ...>)
    {
        return { {(tmp_lib::linear_search(arr, lookup_find_element<ENUM>, arr[SEQ - m_min], def).second)...} };
    }

    template<std::size_t N, typename T2, std::size_t N2, typename ENUM, typename DEF>
    constexpr std::array<const char*, N> lookup_generate(const std::array<T2, N2> arr, ENUM m_min, ENUM m_max, DEF def)
    {
        return lookup_generate_helper<N>(arr, m_min, m_max, def, make_seq<N>());
    }

    constexpr unsigned char str_cmp_sign(char c)
    {
        return c < 0 ? -c : c;
    }
#if __cplusplus < 201402L
    constexpr int str_cmp(const char * s1, const char * s2)
    {
        return *s1 && (*s1 == *s2) ?
            str_cmp(s1 + 1, s2 + 1) :
            str_cmp_sign(*s1) - str_cmp_sign(*s2);
    }
#else
    constexpr int str_cmp(const char * s1, const char * s2)
    {
        while(*s1 && (*s1 == *s2))
            s1++, s2++;
        return str_cmp_sign(*s1) - str_cmp_sign(*s2);
    }
#endif // __cplusplus < 201402L

    template<typename ENUM>
    constexpr bool str_pair_cmp(std::pair<ENUM, const char*> a, std::pair<ENUM, const char*> b)
    {
        return str_cmp(a.second, b.second) < 0;
    }

    template<typename ENUM>
    constexpr bool str_mix_cmp(const char* a, std::pair<ENUM, const char*> b)
    {
        return str_cmp(a, b.second) < 0;
    }

    template<typename ENUM, std::size_t N>
    std::pair<ENUM, const char*> str_binary_search(const std::array<const std::pair<ENUM, const char*>, N> arr, const char * key)
    {
        std::size_t start = 0;
        std::size_t end = N;

        while (start < end)
        {
            std::size_t mid = start + (end - start) / 2;

            int result = str_cmp(key, arr[mid].second);
            if (result < 0)
                end = mid;
            else if (result > 0)
                start = mid + 1;
            else
                return arr[mid];
        }

        return key;
    }
}

template<typename TRAITS>
class smart_enum : public TRAITS
{
    static constexpr std::size_t m_size = sizeof(TRAITS::mapping) / sizeof(TRAITS::mapping[0]);
    using enum_t = typename TRAITS::enum_t;
    using element_t = const std::pair<enum_t, const char*>;

    static constexpr std::array<element_t, m_size> m_arr = to_array(TRAITS::mapping);

    static constexpr bool m_min_f(element_t a, element_t b)
    {
        return a.first < b.first;
    }
    static constexpr bool m_max_f(element_t a, element_t b)
    {
        return a.first > b.first;
    }

    static constexpr enum_t m_min = m_arr[bound_search(m_arr, m_min_f)].first;
    static constexpr enum_t m_max = m_arr[bound_search(m_arr, m_max_f)].first;

    static constexpr std::pair<enum_t, const char*> default_pair{enum_t(0), nullptr};

    static constexpr std::size_t l_size = m_max - m_min + 1;

    static constexpr std::array<const char*, l_size> l_arr = internal::lookup_generate<l_size>(m_arr, m_min, m_max, default_pair);

    static constexpr std::array<element_t, m_size> t_arr = selection_sort(m_arr, internal::str_pair_cmp<enum_t>);

private:

    enum_t val;

public:

    constexpr smart_enum(enum_t val)
        : val(val)
    { }

    constexpr const char* to_string() const
    {
        return (val >= m_min && val <= m_max && l_arr[val - m_min] != nullptr) ?
            l_arr[val - m_min] :
            throw std::invalid_argument("smart_enum: value is invalid");
    }

    constexpr enum_t value() const
    {
        return val;
    }

    static constexpr smart_enum from_str(const char* str)
    {
        return binary_search_int(t_arr, str, default_pair, internal::str_mix_cmp<enum_t>).second != nullptr ?
            binary_search_int(t_arr, str, default_pair, internal::str_mix_cmp<enum_t>).first :
                (throw std::invalid_argument("smart_enum: value is invalid"));
    }

};

template<typename TRAITS>
constexpr std::array<typename smart_enum<TRAITS>::element_t, smart_enum<TRAITS>::m_size> smart_enum<TRAITS>::m_arr;

template<typename TRAITS>
constexpr std::array<typename smart_enum<TRAITS>::element_t, smart_enum<TRAITS>::m_size> smart_enum<TRAITS>::t_arr;

template<typename TRAITS>
constexpr std::array<const char*, smart_enum<TRAITS>::l_size> smart_enum<TRAITS>::l_arr;

template<typename TRAITS>
constexpr std::pair<typename smart_enum<TRAITS>::enum_t, const char*> smart_enum<TRAITS>::default_pair;


template<typename TRAITS>
constexpr bool operator==(smart_enum<TRAITS> a, smart_enum<TRAITS> b)
{
    return a.value() == b.value();
}

template<typename TRAITS>
constexpr bool operator!=(smart_enum<TRAITS> a, smart_enum<TRAITS> b)
{
    return a.value() != b.value();
}


}

#endif // SMART_ENUM_HPP_INCLUDED
