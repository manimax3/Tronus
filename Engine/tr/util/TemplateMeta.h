#pragma once

#include <algorithm>
#include <type_traits>

namespace tr::mp {

template<typename...>
struct is_one_of;

template<typename T>
struct is_one_of<T> {
    static constexpr bool value = false;
};

template<typename T, typename N, typename... R>
struct is_one_of<T, N, R...> {
    static constexpr bool value
        = std::is_same_v<T, N> || is_one_of<T, R...>::value;
};

template<typename... T>
constexpr bool is_one_of_v = is_one_of<T...>::value;

template<typename...>
struct is_unique;

template<>
struct is_unique<> {
    static constexpr bool value = true;
};

template<typename T, typename... R>
struct is_unique<T, R...> {
    static constexpr bool value
        = is_unique<R...>::value && !is_one_of_v<T, R...>;
};

template<typename... T>
constexpr bool is_unique_v = is_unique<T...>::value;

template<typename Container, typename T, typename Pred>
auto SortedInsert(Container &c, T &&value, Pred p)
{
    return c.insert(std::upper_bound(std::begin(c), std::end(c), value, p),
                    std::forward<T>(value));
}
}

#define TR_GENERATE_HAS_MEMBER_TYPE(Type)                                      \
                                                                               \
    template<class T>                                                          \
    class HasMemberType_##Type {                                               \
    private:                                                                   \
        using Yes = char[2];                                                   \
        using No  = char[1];                                                   \
                                                                               \
        struct Fallback {                                                      \
            struct Type {                                                      \
            };                                                                 \
        };                                                                     \
        struct Derived : T, Fallback {                                         \
        };                                                                     \
                                                                               \
        template<class U>                                                      \
        static No &test(typename U::Type *);                                   \
        template<typename U>                                                   \
        static Yes &test(U *);                                                 \
                                                                               \
    public:                                                                    \
        static constexpr bool RESULT                                           \
            = sizeof(test<Derived>(nullptr)) == sizeof(Yes);                   \
    };                                                                         \
                                                                               \
    template<class T>                                                          \
    struct has_member_type_##Type                                              \
        : public std::integral_constant<bool,                                  \
                                        HasMemberType_##Type<T>::RESULT> {     \
    };\

