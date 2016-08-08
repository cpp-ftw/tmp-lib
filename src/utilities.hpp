#ifndef TMP_LIB_UTILITIES_HPP_INCLUDED
#define TMP_LIB_UTILITIES_HPP_INCLUDED

#define TMP_LIB_RETURN(...) decltype(__VA_ARGS__) { return (__VA_ARGS__); }

namespace tmp_lib
{

template<int ...> struct seq { };
template<int N, int ...S> struct gens : gens<N-1, N-1, S...> { };
template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };

template<int N>
static constexpr auto make_seq()
    -> TMP_LIB_RETURN(typename gens<N>::type())

}

#endif // TMP_LIB_UTILITIES_HPP_INCLUDED
