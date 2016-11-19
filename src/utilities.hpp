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



namespace internal
{
#if __cplusplus < 201402L
    template<typename T, std::size_t N, typename COMP>
    constexpr std::size_t bound_search_helper(const std::array<T, N> arr, COMP comp, std::size_t i)
    {
        return i == N-1 ? i :
                    comp(arr[i], arr[bound_search_helper(arr, comp, i+1)]) ?
                        i :
                        bound_search_helper(arr, comp, i+1);
    }
#else
    template<typename T, std::size_t N, typename COMP>
    constexpr std::size_t bound_search_helper(const std::array<T, N> arr, COMP comp, std::size_t)
    {
        std::size_t index = 0;
        for(std::size_t i = 1; i < N; ++i)
            if(comp(arr[i], arr[index]))
                index = i;

        return index;
    }
#endif // __cplusplus < 201402L

}

template<typename T, std::size_t N, typename COMP>
constexpr std::size_t bound_search(std::array<T, N> arr, COMP comp)
{
    return internal::bound_search_helper(arr, comp, 0);
}



namespace internal
{
#if __cplusplus < 201402L
    template<typename T, std::size_t N, typename COMP, typename S>
    constexpr T linear_search_helper(const std::array<T, N> arr, COMP comp, S s, S def, std::size_t i)
    {
        return i == N ? def :
                    comp(s, arr[i]) ?
                        arr[i] :
                        linear_search_helper(arr, comp, s, def, i+1);
    }
#else
    template<typename T, std::size_t N, typename COMP, typename S>
    constexpr T linear_search_helper(const std::array<T, N> arr, COMP comp, S s, S def, std::size_t)
    {
        for(std::size_t i = 0; i < N; ++i)
            if(comp(s, arr[i]))
                return arr[i];

        return def;
    }
#endif // __cplusplus < 201402L
}

template<typename T, std::size_t N, typename COMP, typename S>
constexpr T linear_search(std::array<T, N> arr, COMP comp, S s, S def)
{
    return internal::linear_search_helper(arr, comp, s, def, 0);
}



namespace internal
{

    template<typename T, typename COMP>
    struct selection_sort_cmp
    {
        T min_elem;
        COMP comp;

        constexpr bool operator()(T new_val, T old_val) const
        {
            return !comp(min_elem, old_val) || (comp(min_elem, new_val) && comp(new_val, old_val));
        }
    };

    template<typename Q, typename T, std::size_t N, typename COMP>
    constexpr std::pair<T, std::size_t> selection_sort_helper_helper_helper(const std::array<T, N> arr, COMP comp)
    {
        return { arr[bound_search(arr, comp)], bound_search(arr, comp) };
    }

    template<typename Q, typename T, std::size_t N, typename COMP, int... SEQ>
    struct selection_sort_helper_helper_class
    {
        const std::array<std::pair<T, std::size_t>, N> value;

        constexpr selection_sort_helper_helper_class(const std::array<T, N> arr, COMP comp, Q arr_min)
            : value {{
                selection_sort_helper_helper_helper<Q>(arr, selection_sort_cmp<T, COMP>{arr_min, comp}),
                selection_sort_helper_helper_helper<Q>(arr, selection_sort_cmp<T, COMP>{arr[value[SEQ].second], comp})...
            }}
        { }
    };

    template<typename Q, typename T, std::size_t N, typename COMP, int... SEQ>
    constexpr const std::array<std::pair<T, std::size_t>, N> selection_sort_helper_helper(const std::array<T, N> arr, COMP comp, seq<SEQ...>, Q arr_min)
    {
        return selection_sort_helper_helper_class<Q, T, N, COMP, SEQ...>{arr, comp, arr_min}.value;
    }

    template<typename Q, typename T, std::size_t N, typename COMP, int... SEQ>
    constexpr std::array<T, N> selection_sort_helper(const std::array<T, N> arr, COMP comp, seq<SEQ...>, Q arr_min)
    {
        return {{
            arr[bound_search(arr, comp)],
            (selection_sort_helper_helper<Q>(arr, comp, make_seq<N-1>(), arr[bound_search(arr, comp)])
                [SEQ].first)...
        }};
    }
}

template<typename Q, typename T, std::size_t N, typename COMP>
constexpr std::array<T, N> selection_sort(const std::array<T, N> arr, COMP comp)
{
    return internal::selection_sort_helper<Q>(arr, comp, make_seq<N-1>(), arr[bound_search(arr, comp)]);
}

}

#endif // TMP_LIB_UTILITIES_HPP_INCLUDED
