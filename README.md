# Cpp-Match

A very simple pattern matching library.


## Introduction

This library was created as part of my final project for UIUC MCS CS 421.
It has a relatively simple tempalte-based implementation
and should cover many basic use cases.
It is not intended to be a feature-rich pattern matching library.
If you are looking for one of those, you may want to turn to:
- [mpark/patterns](https://github.com/mpark/patterns)
- [solodon4/Mach7](https://github.com/solodon4/Mach7)
- [jbandela/simple\_match](https://github.com/jbandela/simple_match)


## Basic Syntax

The basic syntax is modeled after
[mpark/patterns](https://github.com/mpark/patterns).

```cpp
using namespace mat;

match(<expr>) (
  pattern(<pattern>) = [<capture>...](<binding>...) { /*...*/ },
  pattern(<pattern>) | pattern(<pattern>) = [<capture>...](<binding>...) { /*...*/ },
  // ...
);
```

The return type of the match expression is determined by
the lambda expressions on the right hand side of the `=`.
If the return type is not `void`, then the match expression will throw
`mat::exceptions::NonVoidNoMatch` if there are no matches.


## Supported Patterns

### Expression Pattern

An _expression pattern_ matches if `<expr> == <pattern>`.

#### Requirements

The type of the expression pattern `U` must satisfy the constraint
`std::equality_comparable_with<T>` where `T` is the type of `<expr>`.

(Note: You can exploit this and extend this library by defining classes
that are equality comparable with what you want to match.)

#### Example

```cpp
int fib(int n) {
  using namespace mat;
  return match(n) (
           pattern(0) = [] { return 0; },
           pattern(1) = [] { return 1; },
           pattern(_) = [n] { return fib(n-1) + fib(n-2); });
}
``` 

### Wildcard Pattern

A _wildcard pattern_ matches any value and does not pass it to
the lambda on the right hand side.

#### Syntax
`_`(underscore)

#### Example
See the Fibonacci example above.

### Capture Pattern

A _capture pattern_ matches any value and passes the value to
the lambda on the right hand side.

#### Syntax
`cpt`

#### Example
```cpp
int fib(int n) {
  using namespace mat;
  return match(n) (
           pattern(0)   = [] { return 0; },
           pattern(1)   = [] { return 1; },
           pattern(cpt) = [](auto n) { return fib(n-1) + fib(n-2); });
}
```

### Destructure Pattern

A _destructure pattern_ matches a tuple-like value where
each of the component pattern of the destructure pattern
matches its corresponding value in the tuple-like value.

#### Requirements
- `std::tuple_size_v<T>` equals the number of patterns
in the destructure pattern where `T` is the type the expression being matched.
- Either the `I`th pattern in the destructure pattern is one of the two
(`_`, `cpt`), or the `I`th pattern in the destructure pattern can be equality
compared with `std::get<I>(<expr>)` where `<expr>` is
the expression being matched.

#### Syntax
`ds(<pattern>...)`

#### Example
```cpp
#include <tuple>
#include <iostream>

using namespace mat;

int main() {
  match(std::make_tuple(1, false, 2)) (
    pattern(ds(1, true, cpt))  = [](auto) { std::cout << "Should not match.\n"; },
    pattern(ds(_, _, 1))       = [] { std::cout << "Should not match.\n"; },
    pattern(ds(cpt, false, _)) = [](auto n) { std::cout << "Matches and I have: " << n << '\n'; });
}
```

### Variant Pattern
A _variant pattern_ matches a `std::variant` if the variant holds
the specified alternative.
The pattern passes the unwrapped value to the lambda on the right hand side.

#### Requirements
The expression being matched is a `std::variant` and
the variant you specify is one of the alternatives the variant can hold.

#### Syntax
`var<T>`

#### Example
```cpp
#include <variant>
#include <iostream>

using namespace mat;

int main() {
  match(std::variant<int, double>{1}) (
    pattern(var<int>)    = [](auto i) { std::cout << "int: " << i << '\n'; },
    pattern(var<double>) = [](auto d) { std::cout << "double: " << d << '\n'; });
}
```

### Alternative Pattern
An _alternative pattern_ matches if any of the patterns in it matches.
Whether the alternative pattern passes any value to the labmda
on the right hand side depends on the first from left-to-right that matches.

#### Requirements
Depends on the patterns you specify.

#### Syntax
`pattern(<pattern>) | pattern(<pattern>) | ...`

#### Example
```cpp
int factorial(int n) {
  using namespace mat;
  return match(n) (
           pattern(0) | pattern(1) = [] { return 1; },
           pattern(cpt) = [](auto n) { return n * factorial(n-1); });
}
```


## Build and Test

### Requirements

- CMake >= 3.14
- Compiler that supports C++20

### Steps

Once you are in the project directory,
you can do the following to build and test the project:

```
$ mkdir build && cd build
$ cmake ..
$ cmake --build . && ctest
```
