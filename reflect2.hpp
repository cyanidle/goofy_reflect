
template<typename R, typename T, typename...Args>
auto ret(T(R::*)(Args...)) -> R;

template<typename...Ts>
struct TypeList {};

template<int i> struct ITag{};

template<int id, auto ptr, typename...A>
struct Info {
    static constexpr auto Id = id;
    static constexpr auto Ptr = ptr;
    using Attributes = TypeList<A...>;
    const char16_t* Name;
};

template<typename T>
struct Reflect {
    static constexpr int n_methods = T::_iface_end - T::_iface_begin;
    template<int N>
    static constexpr auto info = T::_infer(ITag<N>{});
};


#define BEGIN_IFACE(name) \
class __##name##_info { }; \
    class name { \
        template<typename> friend class Reflect;\
        private: \
        using _ = name;\
        static constexpr int _iface_begin = __COUNTER__ + 1;

/* these */ // parts are written by user outside of the macro

#define METHOD(ret, name, ...) METHOD_(ret, name, __COUNTER__, ##__VA_ARGS__)
#define METHOD_(ret, name, id, ...) \
private: static constexpr auto _infer(ITag<id - _iface_begin>) { \
    return Info<id - _iface_begin, &_::name, ##__VA_ARGS__>{u""#name};\
} \
public: virtual ret name /*(args...) = 0*/


#define END_IFACE() private: static constexpr int _iface_end = __COUNTER__; }

////////////////////// TEST!

BEGIN_IFACE(Test)
    METHOD(void, foo)(int myIntArg) = 0;
    METHOD(void, bar)(bool myArg1, int myArg2) = 0;
END_IFACE();

constexpr auto methods = Reflect<Test>::n_methods;
constexpr auto name_0 = Reflect<Test>::info<0>.Name;
constexpr auto name_1 = Reflect<Test>::info<1>.Name;

