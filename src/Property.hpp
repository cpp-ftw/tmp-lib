#ifndef PROPERTY_HPP_INCLUDED
#define PROPERTY_HPP_INCLUDED

// mire jó: getter és setter helyettesítése egy publikus, fiktív adattaggal.

#define PROPERTY( parent, return_type, name) \
                    struct property_offset_##name;\
                    Property<parent, return_type, (& parent :: set ##name), (& parent :: get ##name), property_offset_##name> name{}; \
                    struct property_offset_##name { static constexpr uintptr_t value = offset_of(&parent::name); \
                    static_assert(value != uintptr_t(-1), "Unable to calculate offset of" #parent "::" #name ); };
#define PROPERTY_(parent, return_type, name) \
                    struct property_offset_##name;\
                    Property<parent, return_type, (& parent :: set_##name), (& parent :: get_##name), property_offset_##name> name{}; \
                    struct property_offset_##name { static constexpr uintptr_t value = offset_of(&parent::name); \
                    static_assert(value != uintptr_t(-1), "Unable to calculate offset of" #parent "::" #name ); };


template <typename CLASS, typename T>
struct offset_of_helper
{
    static union U
    {
        char c[sizeof(CLASS)];
        CLASS o;

        constexpr U() : c{0} {}
        ~U() {}
    } u;

#if defined(__GNUC__) && !defined(__clang__)
    static constexpr uintptr_t f (T CLASS::*member)
    {
        return (uintptr_t) & (u.o.*member) - (uintptr_t) & (u.o);
    }
#else // defined(__GNUC__) && !defined(__clang__)

#if __cplusplus >= 201402L // no recursion depth limit in a loop
    static constexpr uintptr_t f (T CLASS::*member)
    {
        for (uintptr_t i = 0; i != sizeof (CLASS); ++i)
            if (static_cast<void const*>(&(u.c[i])) == &(u.o.*member) )
                return i;
        return -1;
    }
#else // __cplusplus >= 201402L
    static constexpr uintptr_t offset_r(T CLASS::*member, uintptr_t i)
    {
        return static_cast<void const*>(&(u.c[i])) == &(u.o.*member) ? i : i == sizeof(CLASS) ? -1 : offset_r(member, i+1);
    }

    static constexpr uintptr_t f (T CLASS::*member)
    {
        return offset_r(member, 0);
    }
#endif // __cplusplus >= 201402L

#endif // defined(__GNUC__) && !defined(__clang__)

};

template <typename CLASS, typename T>
typename offset_of_helper<CLASS, T>::U offset_of_helper<CLASS, T>::u;

template<typename CLASS, typename T>
constexpr uintptr_t offset_of(T CLASS::* mptr)
{
    return offset_of_helper<CLASS, T>::f(mptr);
}

template<typename CLASS, int>
class PropertyCRTP;

template <typename CLASS, typename T,
         void (CLASS::*SETTERPTR) (T const& newvalue),
         T (CLASS::*GETTERPTR) () const,
         typename OFFSET_HOLDER>
class Property
{
    public:

    // private + friend, azért, hogy ne lehessen leírni:
    // auto x = foo.property;
    constexpr Property() = default;
    constexpr Property(Property const&) = default;
    constexpr Property(Property&&) = default;
    friend CLASS;

public:

    // property beállítása
    Property& operator= (T const& newvalue)
    {
        CLASS * obj = reinterpret_cast<CLASS*>(reinterpret_cast<char*>(this) - OFFSET_HOLDER::value);
        ((*obj).*SETTERPTR) (newvalue);
        return *this;
    }

    // property lekérdezése
    operator T () const
    {
        const CLASS * obj = reinterpret_cast<const CLASS*>(reinterpret_cast<const char*>(this) - OFFSET_HOLDER::value);
        return ((*obj).*GETTERPTR) ();
    }

    // property = property eset kezelése
    Property& operator= (Property const& the_other)
    {
        CLASS * obj = reinterpret_cast<CLASS*>(reinterpret_cast<char*>(this) - OFFSET_HOLDER::value);

        if(obj->property_is_in_op_assign())
            obj->property_cnt++;
        else
            ((*obj).*SETTERPTR) (the_other.operator T());

        return *this;
    }
};

// ebből kell leszármaznia a Property-t tartalmazó osztálynak
template<typename Derived, int N>
class PropertyCRTP
{
    int property_cnt = N;

    template <typename CLASS, typename T,
         void (CLASS::*SETTERPTR) (T const& newvalue),
         T (CLASS::*GETTERPTR) () const,
         typename>
    friend class Property;

    constexpr bool property_is_in_op_assign() const
    {
        return property_cnt != N;
    }

    template<typename T>
    struct property_offset {};

protected:
    PropertyCRTP& operator=(PropertyCRTP const& old) {
        property_cnt = 0;
        return *this;
    }
    PropertyCRTP& operator=(PropertyCRTP&& old) {
        property_cnt = 0;
        return *this;
    }

    constexpr PropertyCRTP() = default;
    constexpr PropertyCRTP(PropertyCRTP const&) = default;
    constexpr PropertyCRTP(PropertyCRTP&&) = default;
    ~PropertyCRTP() = default;

};

#endif // PROPERTY_HPP_INCLUDED
