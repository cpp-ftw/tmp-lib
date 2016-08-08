#ifndef BIND_THIS_HPP_INCLUDED
#define BIND_THIS_HPP_INCLUDED

#include <utility>
#include <memory>
#include <type_traits>

#include "utilities.hpp"

namespace tmp_lib
{

template<typename CLASS, typename RET, typename... ARGS>
class bind_this_t
{
public:
  using mptr_t = typename std::conditional<std::is_const<CLASS>::value,
                  RET (CLASS::*)(ARGS... args) const,
                  RET (CLASS::*)(ARGS... args)>::type;

  constexpr bind_this_t(CLASS& self, mptr_t mptr) :
    self(std::addressof(self)),
    mptr(mptr)
  { }

  constexpr RET operator()(ARGS... args)
  {
    return ((*self).*mptr)(std::forward<ARGS>(args)...);
  }

private:

  CLASS* self;
  mptr_t mptr;

};

template<typename CLASS, typename PTR, typename RET, typename... ARGS>
constexpr auto bind_this(RET (CLASS::*mptr)(ARGS... args), PTR self)
    -> TMP_LIB_RETURN(bind_this_t<CLASS, RET, ARGS...>(*self, mptr))

template<typename CLASS, typename PTR, typename RET, typename... ARGS>
constexpr auto bind_this(RET (CLASS::*mptr)(ARGS... args) const, PTR self)
    -> TMP_LIB_RETURN(bind_this_t<const CLASS, RET, ARGS...>(*self, mptr))

template<typename CLASS, typename RET, typename... ARGS>
constexpr auto bind_this(RET (CLASS::*mptr)(ARGS... args), CLASS& self)
    -> TMP_LIB_RETURN(bind_this_t<CLASS, RET, ARGS...>(self, mptr))

template<typename CLASS, typename RET, typename... ARGS>
constexpr auto bind_this(RET (CLASS::*mptr)(ARGS... args) const, const CLASS& self)
    -> TMP_LIB_RETURN(bind_this_t<const CLASS, RET, ARGS...>(self, mptr))

}

#endif // BIND_THIS_HPP_INCLUDED
