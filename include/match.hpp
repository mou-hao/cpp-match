#pragma once

#include <concepts>
#include <exception>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>

namespace mat {

namespace detail {

class Any {};
class Capture {};

template <typename... Ts>
class Destructure {
    std::tuple<Ts...> ts;
public:
    constexpr Destructure(Ts... ts_) : ts{ts_...} {}
    constexpr std::tuple<Ts...>& get_tuple() {
        return ts;
    };
};

}

const detail::Any _ = detail::Any();
const detail::Capture cpt = detail::Capture();

template <typename... Ts>
constexpr detail::Destructure<Ts...> ds(Ts... ts) {
    return detail::Destructure(std::forward<Ts>(ts)...);
}

namespace detail {

template <typename P, typename F>
class Arm;

template <typename F, typename... Ts>
class DsArm;

template <typename T>
class Pattern {
    const T expr;
public:
    Pattern(const T& expr_) : expr{expr_} {}

    template <std::equality_comparable_with<T> U>
    bool matches(const U& mat) const {
        return mat == expr;
    }

    template <typename F>
    decltype(auto) operator=(F&& f) const {
        return Arm(*this, [=](auto) { return f(); });
    } 
};

template <>
class Pattern<Any> {
public:
    Pattern(Any) {}
    
    template <typename U>
    bool matches(U) const {
        return true;
    }

    template <typename F>
    decltype(auto) operator=(F&& f) const {
        return Arm(*this, [=](auto) { return f(); });
    }
};

template <>
class Pattern<Capture> {
public:
    Pattern(Capture) {}
    
    template <typename U>
    bool matches(U) const {
        return true;
    }

    template <typename F>
    decltype(auto) operator=(F&& f) const {
        return Arm(*this, std::forward<F>(f));
    }
};

template <typename... Ts>
class Pattern<Destructure<Ts...>> {
    const std::tuple<Ts...> tup;

    template <std::size_t I, typename T1, typename T2>
    bool tup_comp(const T1& t1, const T2& t2, bool accum) const {
        if (!accum) return false;
        if constexpr (I < std::tuple_size_v<T1>) {
            if constexpr (
                    std::is_same_v<std::tuple_element_t<I, T1>, Any> ||
                    std::is_same_v<std::tuple_element_t<I, T1>, Capture>) {
                return tup_comp<I+1, T1, T2>(t1, t2, accum);
            } else {
                return tup_comp<I+1, T1, T2>(t1, t2, std::get<I>(t1) == std::get<I>(t2));
            }
        } else {
            return accum;
        }
    }

public:
    Pattern(Destructure<Ts...> ds) : tup{std::move(ds.get_tuple())} {}

    template <typename U>
    bool matches(const U& mat) const {
        if constexpr (sizeof...(Ts) != std::tuple_size_v<U>) {
            return false;
        }
        return tup_comp<0, std::tuple<Ts...>, U>(tup, mat, true);    
    }

    template <typename F>
    decltype(auto) operator=(F&& f) const {
        return DsArm<F, Ts...>(*this, std::forward<F>(f));
    }
};

}

template <typename T>
inline detail::Pattern<T> pattern(const T& expr) {
    return detail::Pattern(std::forward<const T>(expr));
}

inline detail::Pattern<detail::Any> pattern(detail::Any) {
    return detail::Pattern(_);
}

inline detail::Pattern<detail::Capture> pattern(detail::Capture) {
    return detail::Pattern(cpt);
}

inline detail::Pattern<std::string> pattern(const char* str) {
    return detail::Pattern(std::string(str));
}

template <typename... Ts>
inline detail::Pattern<detail::Destructure<Ts...>> pattern(detail::Destructure<Ts...>&& ds) {
    return detail::Pattern(std::forward<detail::Destructure<Ts...>>(ds));
}

namespace detail {

template <typename P>
class ArmBase {
    const P& pat;
public:
    ArmBase(const P& pat_) : pat{pat_} {}

    template <typename U>
    bool matches(const U& mat) const {
        return pat.matches(mat);
    }
};

template <typename P, typename F>
class Arm : public ArmBase<P> {
    F f;
public:
    Arm(const P& pat_, F&& f_) : ArmBase<P>{pat_}, f{f_} {}
    
    template <typename T>
    decltype(auto) operator()(const T& mat) const {
        return f(mat);
    }
};

template <typename F, typename... Ts>
class DsArm : public ArmBase<Pattern<Destructure<Ts...>>> {
    using P = Pattern<Destructure<Ts...>>;
    F f;
public:
    DsArm(const P& pat_, F&& f_) : ArmBase<P>{pat_}, f{f_} {}

    template <typename T>
    decltype(auto) operator()(const T& mat) const {
        return f();
    }
};

template <std::regular T>
class Match {
    const T mat;
public:
    Match(const T& mat_) : mat{mat_} {}

    template <typename Arm, typename... Arms>
    decltype(auto) operator()(Arm&& arm, Arms&&... arms) const {
        if (arm.matches(mat)) {
            return arm(mat);
        } else {
            if constexpr (sizeof...(arms) == 0) {
                std::terminate();
            } else {
                return operator()(std::forward<Arms>(arms)...);
            }
        }
        
    }
};

}

template <std::regular T>
inline detail::Match<T> match(const T& mat) {
    return detail::Match(std::forward<const T>(mat));
}

inline detail::Match<std::string> match(const char* str) {
    return detail::Match(std::string(str));
}

}
