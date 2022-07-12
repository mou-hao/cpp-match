#pragma once

#include <any>
#include <concepts>
#include <initializer_list>
#include <functional>
#include <variant>

namespace mat {

class _Any {};
const _Any _ = _Any();

class _Capture {};
const _Capture cpt = _Capture();

template <typename T>
class _Arm {
    std::function<bool(T)> mat_f;
    std::function<void()> f;
public:
    _Arm(std::function<bool(T)> mat_f, std::function<void()> f) 
        : mat_f{mat_f}, f{f} {}
    bool matches(T x) const { return mat_f(x); }
    void operator()() const { f(); }
};

template <>
class _Arm<_Any> {
    std::function<void()> f;
public:
    _Arm(std::function<void()> f) : f{f} {}
    void operator()() const { f(); }
};

template <typename T>
class pattern {
    const T pat;
public:
    pattern(const T& expr) : pat{expr} {};
    _Arm<T> operator=(std::function<void()> f) {
        return _Arm<T>([&](const T mat) { return mat == pat; }, f); 
    };
};

template <>
class pattern<_Any> {
public:
    pattern(_Any) {};
    _Arm<_Any> operator=(std::function<void()> f) {
        return _Arm<_Any>(f);
    };
};

template <std::regular T, typename R = void>
class match {
    const T mat;
public:
    match() = delete;
    match(const T mat) : mat{mat} {}
    R operator()(std::initializer_list<std::variant<_Arm<T>, _Arm<_Any>>> arms) {
        bool loop = true;
        for (auto it = arms.begin(); loop && it != arms.end(); ++it) {
            std::visit([&](auto& arg) {
                using ArmT = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<ArmT, _Arm<T>>) {
                    if (arg.matches(mat)) {
                        arg();
                        loop = false;
                    }
                }
                if constexpr (std::is_same_v<ArmT, _Arm<_Any>>) {
                    arg();
                    loop = false;
                }
            }, *it);
        }
    }
};

}
