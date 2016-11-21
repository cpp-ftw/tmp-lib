#ifndef TMP_LIB_UTILITIES_HPP_INCLUDED
#define TMP_LIB_UTILITIES_HPP_INCLUDED

#define TMP_LIB_RETURN(...) decltype(__VA_ARGS__) { return (__VA_ARGS__); }

#include <array>
#include <cstddef>

namespace tmp_lib
{

template<int ...> struct seq { };
template<int N, int ...S> struct gens : gens<N-1, N-1, S...> { };
template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };

template<int N>
static constexpr auto make_seq()
    -> TMP_LIB_RETURN(typename gens<N>::type())



template<typename T>
struct my_remove_const
{
    using type = T;
};

template<typename T>
struct my_remove_const<const T>
{
    using type = T;
};


namespace internal
{
    template<typename T, std::size_t N, int... I>
    constexpr std::array<T, N> to_array_helper(T (&a)[N], seq<I...>)
    {
        return { {a[I]...} };
    }
}

template<typename T, std::size_t N>
constexpr std::array<T, N> to_array(T (&a)[N])
{
    return internal::to_array_helper(a, make_seq<N>());
}


}

#endif // TMP_LIB_UTILITIES_HPP_INCLUDED
