#ifndef ALGORITHM_HPP_INCLUDED
#define ALGORITHM_HPP_INCLUDED

#include "utilities.hpp"

namespace tmp_lib
{


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

template<typename T, std::size_t N, typename COMP>
constexpr std::array<T, N> selection_sort(const std::array<T, N> arr, COMP comp)
{
    return internal::selection_sort_helper<typename my_remove_const<T>::type>
        (arr, comp, make_seq<N-1>(), arr[bound_search(arr, comp)]);
}

namespace internal
{
#if __cplusplus < 201402L
    template<typename T, std::size_t N, typename COMP, typename S, typename DEF>
    constexpr T binary_search_int_helper(const std::array<T, N> arr, S key, DEF def, COMP comp, std::size_t start, std::size_t end)
    {
#define TMP_LIB_INTERNAL_MID    (start + (end - start) / 2)
        return comp(key, arr[TMP_LIB_INTERNAL_MID]) == 0 ?
            arr[TMP_LIB_INTERNAL_MID] :
            comp(key, arr[TMP_LIB_INTERNAL_MID]) < 0 ?
                binary_search_int_helper(arr, key, def, comp, start, TMP_LIB_INTERNAL_MID) :
                binary_search_int_helper(arr, key, def, comp, TMP_LIB_INTERNAL_MID + 1, end);
#undef TMP_LIB_INTERNAL_MID
    }
#else
    template<typename T, std::size_t N, typename COMP, typename S, typename DEF>
    constexpr T binary_search_int_helper(const std::array<T, N> arr, S key, DEF def, COMP comp, std::size_t start, std::size_t end)
    {
        while (start < end)
        {
            std::size_t mid = start + (end - start) / 2;

            int result = comp(key, arr[mid]);
            if (result < 0)
                end = mid;
            else if (result > 0)
                start = mid + 1;
            else
                return arr[mid];
        }

        return def;
    }
#endif
}

template<typename T, std::size_t N, typename COMP, typename S, typename DEF>
constexpr T binary_search_int(const std::array<T, N> arr, S key, DEF def, COMP comp)
{
    return internal::binary_search_int_helper(arr, key, def, comp, 0, N);
}


}

#endif // ALGORITHM_HPP_INCLUDED
