#ifndef TO_FPTR_HPP_INCLUDED
#define TO_FPTR_HPP_INCLUDED

#include <utility>
#include <functional>

namespace tmp_lib
{

template<typename LAMBDA, typename RET, typename... ARGS>
class to_fptr_t
{
    static thread_local std::function<RET(ARGS...)> tl_function;

    static RET fptr_handler(ARGS... args)
    {
        return tl_function(std::forward<ARGS>(args)...);
    }

    static void set_func(std::function<RET(ARGS...)>&& func)
    {
        tl_function = std::move(func);
    }

public:

    template<typename LAMBDA2, typename RET2, typename... ARGS2>
    friend RET2(*to_fptr(std::function<RET2(ARGS2...)>, LAMBDA2))(ARGS2...);
};

template<typename LAMBDA, typename RET, typename... ARGS>
thread_local std::function<RET(ARGS...)> to_fptr_t<LAMBDA, RET, ARGS...>::tl_function = nullptr;

template<typename LAMBDA, typename RET, typename... ARGS>
RET(*to_fptr(std::function<RET(ARGS...)> func, LAMBDA lambda))(ARGS... args)
{
    using T = to_fptr_t<LAMBDA, RET, ARGS...>;
    T::set_func(std::move(func));
    //T::tl_function = std::move(func);
    return &T::fptr_handler;
}

}

#endif // TO_FPTR_HPP_INCLUDED
