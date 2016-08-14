#ifndef PROPERTY_HPP_INCLUDED
#define PROPERTY_HPP_INCLUDED

// mire jó: getter és setter helyettesítése egy publikus, fiktív adattaggal.

#define PROPERTY( parent, return_type, name) Property<parent, return_type, (& parent :: set ##name), (& parent :: get ##name)> name{this}
#define PROPERTY_(parent, return_type, name) Property<parent, return_type, (& parent :: set_##name), (& parent :: get_##name)> name{this}

// 1 db akármilyen típusú member pointert tárol
template<typename CLASS, typename T, T CLASS::* MPTR>
struct MptrHolder
{
    static constexpr auto mptr = MPTR;
};

// n db akármilyen típusú member pointert tárol
template<typename CLASS, typename... MPTRS>
struct MptrCollection
{
// belső használatú fos, hátha jobban olvasható a variadic magic
#define SELF  (static_cast<      CLASS*>(self))
#define CSELF (static_cast<const CLASS*>(self))

    static void set_objs(CLASS * self)
    {
        using swallow = int[];
        (void) swallow {0, ((void)((SELF->*(MPTRS::mptr)).obj = SELF), 0)...};
    }

#undef SELF
#undef CSELF
};

// levezeti 1 db member pointer osztályának típusát
template<typename CLASS, typename T>
CLASS make_mptr_class(T CLASS::* mptr);

// levezeti 1 db member pointer típusát
template<typename CLASS, typename T>
T make_mptr_type(T CLASS::* mptr);

// becsomagol 1 db member pointert egy típusba
#define PROPERTY_MAKE(x) MptrHolder<decltype(make_mptr_class(x)), decltype(make_mptr_type(x)), x>
// becsomagol n db member pointert egy típusba
#define PROPERTY_DECLARE(x, ...) using properties = MptrCollection<Vektor, __VA_ARGS__>

template<typename CLASS>
class PropertyCRTP;

template <typename CLASS, typename T,
         void (CLASS::*SETTERPTR) (T const& newvalue),
         T (CLASS::*GETTERPTR) () const>
class Property
{
    friend class PropertyCRTP<CLASS>;
    template<typename, typename...>
    friend class MptrCollection;

    CLASS * obj;
    constexpr explicit Property (CLASS* obj) : obj (obj) {}

    // no-op: a PropertyCRTP feladata a pointerek beállítgatása, és ezt még
    // azelőtt megteszi, hogy ide eljutnánk, így itt nem szabad rajta piszkálni

    // private + friend, azért, hogy ne lehessen leírni:
    // auto x = foo.property;
    constexpr Property() { }
    constexpr Property(Property const&) { }
    constexpr Property(Property&&) { }
    friend CLASS;

public:

    // property beállítása
    Property& operator= (T const& newvalue)
    {
        ((*obj).*SETTERPTR) (newvalue);
        return *this;
    }

    // property lekérdezése
    operator T () const
    {
        return ((*obj).*GETTERPTR) ();
    }

    // property = property eset kezelése
    Property& operator= (Property const& the_other)
    {
        *this = the_other.operator T();
        return *this;
    }
};

// ebből kell leszármaznia a Property-t tartalmazó osztálynak
template<typename Derived>
class PropertyCRTP
{
    void set_objs()
    {
        using d_prop = typename Derived::properties;
        d_prop::set_objs(static_cast<Derived*>(this));
    }

protected:
    PropertyCRTP() {
        set_objs();
    }
    PropertyCRTP(PropertyCRTP const&) {
        set_objs();
    }
    PropertyCRTP(PropertyCRTP&&) {
        set_objs();
    }
};

#endif // PROPERTY_HPP_INCLUDED
