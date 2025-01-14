
template<typename R, typename T, typename...Args>
auto ret(T(R::*)(Args...)) -> R;

template<typename...Ts>
struct TypeList {};

template<typename T> struct Tag{};

template<int i> struct ITag : ITag<i - 1> { static constexpr int value = i; };
template<> struct ITag<0>{ static constexpr int value = 0; };

template<int id, auto ptr, typename...A>
struct Info {
    static constexpr auto Id = id;
    static constexpr auto Ptr = ptr;
    using Attributes = TypeList<A...>;
    const char16_t* Name;
};

template<typename T>
struct Reflect {
    static constexpr int count = decltype(_check_n(static_cast<T*>(0), ITag<100>{}))::value + 1;
    template<int N>
    static constexpr auto info = _infer(static_cast<T*>(0), ITag<(N > count ? count : N)>{});
};

template<typename...Args>
constexpr int bases_count = (0 + ... + Reflect<Args>::count);

#define INTERFACE(name, ...) \
class name; \
struct _iface_##name { \
using _ = name; \
static constexpr int _iface_begin = __COUNTER__ + 1 - bases_count<__VA_ARGS__>; \
}; class name : private _iface_##name


#define INTERFACE_1(name, base1) \
    INTERFACE(name, base1), public base1

/* these */ // parts are written by user outside of the macro

#define METHOD(ret, name, ...) METHOD_(ret, name, __COUNTER__, ##__VA_ARGS__)
#define METHOD_(ret, name, id, ...) \
friend auto _check_n(_*, ITag<id - _iface_begin>) -> ITag<id - _iface_begin>; \
friend constexpr auto _infer(_*, ITag<id - _iface_begin>) { \
    return Info<id - _iface_begin, &_::name, ##__VA_ARGS__>{u""#name};\
} \
virtual ret name /*(args...) = 0*/

////////////////////// TEST!

struct A {
    static void kek(int);
};

struct B : A{
    static void kek(char);
};

INTERFACE(Base) {
public:
    METHOD(void, x)() = 0;
};

INTERFACE(Test) {
public:
    METHOD(void, foo)(int myIntArg) = 0;
    METHOD(bool, bar)(bool myArg1, int myArg2) = 0;
};

void check(Test* t) {
    //t->x();
    t->foo(312);
    t->bar(true, 123);
}

constexpr auto methods = Reflect<Test>::count;
constexpr auto name_0 = Reflect<Test>::info<0>.Name;
constexpr auto name_1 = Reflect<Test>::info<1>.Name;

