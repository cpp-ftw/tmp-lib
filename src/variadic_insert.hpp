#ifndef TMP_LIB_VARIADIC_INSERT_HPP_INCLUDED
#define TMP_LIB_VARIADIC_INSERT_HPP_INCLUDED

#include "utilities.hpp"

namespace tmp_lib
{

template<unsigned n, typename HEAD, typename... ARGS>
struct get_nth_param_class
{
    static constexpr auto get(HEAD&& head, ARGS&&... args)
        -> TMP_LIB_RETURN(get_nth_param_class<n-1, ARGS...>::get(std::forward<ARGS>(args)...))
};

template<typename HEAD, typename... ARGS>
struct get_nth_param_class<0, HEAD, ARGS...>
{
    static constexpr auto get(HEAD&& head, ARGS&&... args)
        -> TMP_LIB_RETURN(std::forward<HEAD>(head))
};

template<int POS, typename INSERTED, int... before, int... after,
         typename FUNC, typename... ARGS>
constexpr auto variadic_insert_helper(seq<before...>, seq<after...>,
                                INSERTED&& inserted, FUNC func, ARGS&&... args)
    -> TMP_LIB_RETURN(
        func(get_nth_param_class<before, ARGS...>::get(std::forward<ARGS>(args)...)...,
        std::forward<INSERTED>(inserted),
        get_nth_param_class<after + POS, ARGS...>::get(std::forward<ARGS>(args)...)...))

template<int POS, typename INSERTED, typename FUNC, typename... ARGS>
constexpr auto variadic_insert(INSERTED&& inserted, FUNC func, ARGS&&... args)
    -> TMP_LIB_RETURN(
        variadic_insert_helper<POS>(make_seq<POS>(),
                                    make_seq<sizeof...(args)-POS>(),
        std::forward<INSERTED>(inserted), func, std::forward<ARGS>(args)...))

}


#endif // TMP_LIB_VARIADIC_INSERT_HPP_INCLUDED
