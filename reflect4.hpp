template<typename...Ts>
struct TypeList {
    template<typename T>
    using Append = TypeList<Ts..., T>;
};

template<typename T, unsigned N>
constexpr bool method_out_of_range = false;

template<typename T> struct Tag{};

template<unsigned i> struct ITag { static constexpr unsigned value = i; };

template<unsigned id, auto ptr, typename...A>
struct Info {
    static constexpr auto Id = id;
    static constexpr auto Ptr = ptr;
    using Attributes = TypeList<A...>;
    const char16_t* Name;
};

template<typename T>
struct Reflect;


struct Access {
    template<typename...Ifs>
    static constexpr unsigned _check_n(TypeList<Ifs...>) {
        return ((Ifs::_iface_end - Ifs::_iface_begin) + ...);
    }

    template<unsigned N, typename First, typename...Ifaces>
    static constexpr auto _infer(TypeList<First, Ifaces...>) {
        constexpr unsigned curr = Reflect<First>::count;
        if constexpr (N < curr) {
            return First::_infer(ITag<N>{});
        } else if constexpr (sizeof...(Ifaces) > 0) {
            return _infer<N - curr>(TypeList<Ifaces...>{});
        } else {
            static_assert(method_out_of_range<First, N>);
        }
    }
};


template<typename T>
struct Reflect {
    using bases = typename T::_iface_bases;
    using iface_tree = typename bases::template Append<T>;
    static constexpr unsigned count = Access::_check_n(iface_tree{});
    template<unsigned N>
    static constexpr auto info = Access::_infer<N>(iface_tree{});
};

// macros

#define _OPEN_VA(...) __VA_ARGS__
#define _INTERFACE(name, inherit, ...) \
class name _OPEN_VA inherit { \
    friend Reflect<name>; friend Access; \
    using _ = name; \
    using _iface_bases = TypeList<__VA_ARGS__>; \
    static constexpr unsigned _iface_begin = __COUNTER__ + 1; \
    _INTERFACE_BODY

#define _INTERFACE_BODY(...) \
    __VA_ARGS__ \
    private: static constexpr unsigned _iface_end = __COUNTER__; }

#define INTERFACE(name) _INTERFACE(name, ())
#define INTERFACE_1(name, base) _INTERFACE(name, (: public base), base)

/* these */ // parts are written by user outside of the macro

#define METHOD(ret, name, ...) METHOD_(ret, name, __COUNTER__, ##__VA_ARGS__)
#define METHOD_(ret, name, id, ...) \
private: static constexpr auto _infer(ITag<id - _iface_begin>) { \
    return Info<id - _iface_begin, &_::name, ##__VA_ARGS__>{u""#name};} \
public: virtual ret name /*(args...) = 0*/


////////////////////// TEST!

INTERFACE(Base) (
    METHOD(void, x)() = 0;
);

constexpr auto base_methods = Reflect<Base>::count;
constexpr auto base_name_0 = Reflect<Base>::info<0>.Name;

INTERFACE_1(Test, Base) (
public:
    METHOD(void, foo)(int myIntArg) = 0;
    METHOD(bool, bar)(bool myArg1, int myArg2) = 0;
);

void check(Test* t) {
    t->x();
    t->foo(312);
    t->bar(true, 123);
}

constexpr auto methods = Reflect<Test>::count;
constexpr auto name_0 = Reflect<Test>::info<0>.Name;
constexpr auto name_1 = Reflect<Test>::info<1>.Name;
constexpr auto name_2 = Reflect<Test>::info<2>.Name;

