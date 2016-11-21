#include <iostream>
#include <csignal>

#include "variadic_insert.hpp"
#include "bind_this.hpp"
#include "to_fptr.hpp"
#include "constexpr_str.hpp"
#include "smart_enum.hpp"

double foo(double d1, double d2, double d3, double d4)
{
    std::cout << d1 << ' ' << d2 << ' ' << d3 << ' ' << d4 << std::endl;
    return d1 + d2 + d3 + d4;
}

void sample_variadic_insert()
{
    std::cout << tmp_lib::variadic_insert<1>(4.2, foo, 1.2, 3.5, 8.13) << std::endl;
}

struct A
{
    int m;
    int f(int a, int b, int c) const
    {
        return m * (a + b - c);
    }
};

void sample_bind_this()
{
    A a{2};
    auto f = tmp_lib::bind_this(&A::f, a);
    std::cout << f(4, 3, 2) << std::endl;
}

void sample_to_fptr()
{
    std::function<void(int)> sh = [](int sig)
    {
        std::cout << "Signal " << sig << " caught." << std::endl;
    };

    signal(SIGINT, tmp_lib::to_fptr(sh, []{}));

    std::cin.ignore();
}

void sample_constexpr_str()
{
    auto cstr = TMP_LIB_CONSTEXPR_STR("hello");
    std::cout << cstr.value << std::endl;
}

struct sample_enum_traits
{
    enum enum_t
    {
        A, B, C
    };

    static constexpr std::pair<enum_t, const char*> mapping[] =
    {
        { A, "A" },
        { B, "B" },
        { C, "C" },
    };
};

using sample_enum = tmp_lib::smart_enum<sample_enum_traits>;

constexpr bool intcmp(int a, int b)
{
    return a < b;
}

void sample_smart_enum()
{
    constexpr sample_enum a = sample_enum::A;

    constexpr const char * astr = a.to_string();

    std::cout << astr << std::endl;
    std::cout << sample_enum(sample_enum::B).to_string() << std::endl;

    constexpr int arr[] = {3, 4, -1, 0};
    constexpr auto arr2 = tmp_lib::to_array(arr);

    constexpr auto arr3 = tmp_lib::selection_sort(arr2, intcmp);

    constexpr sample_enum b = sample_enum::from_str("B");
    std::cout << b.value() << std::endl;

    for(auto i : arr3)
        std::cout << i << ' ';
    std::cout << std::endl;
}

int main()
{
    sample_smart_enum();
    sample_constexpr_str();
    sample_variadic_insert();
    sample_bind_this();
    sample_to_fptr();
}


