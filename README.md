# calc

A command-line calculator and a small C++17 expression library (`calc_core`).

```
$ calc "2 * (3 + 4)"
14
$ calc "sqrt(16) + pi"
7.14159265358979
$ calc "1 / 0"
division by zero: cannot divide by zero
  1 / 0
    ^
```

## Build and test

Requires CMake 3.16+ and a C++17 compiler (GCC 9+, Clang 10+, or recent MSVC).

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Tests use GoogleTest. An installed copy (for example `libgtest-dev`) is used
when found; otherwise CMake downloads a pinned release.

Install the binary, library and headers:

```sh
cmake --install build --prefix ~/.local
```

## Usage

```
calc "<expression>"   evaluate one expression and print the result
calc                  interactive session (or read lines from a pipe)
calc --help           show help
calc --version        show the version
```

Exit codes: `0` success, `1` an expression failed, `2` bad usage.

Interactive session:

```
$ calc
calc 0.1.0 - type 'help' for help, 'quit' to exit
> x = 3
3
> x ^ 2 + ans
12
> quit
```

Piped input evaluates one expression per line and shares variables between
lines:

```sh
printf 'r = 2\npi * r ^ 2\n' | calc
```

## Syntax

| Kind       | Supported                                                   |
|------------|-------------------------------------------------------------|
| Numbers    | `42`, `3.14`, `.5`, `1e-3`, `2.5E+4`                        |
| Operators  | `+ - * / %` and `^` (power), unary `+ -`, parentheses       |
| Functions  | `sin cos tan sqrt log ln abs` (1 arg), `pow` (2), `min max` (1 or more) |
| Constants  | `pi`, `e`                                                   |
| Variables  | `name = expr`; `ans` holds the last result                  |

Precedence, lowest to highest: assignment, `+ -`, `* / %`, unary `+ -`, `^`.
`^` is right-associative and binds tighter than unary minus, so
`2 ^ 3 ^ 2` is `512` and `-2 ^ 2` is `-4`. `log` is base 10; `ln` is natural.

Errors point at the offending column: syntax errors, unknown identifiers,
division by zero, domain errors (`sqrt(-1)`, `ln(0)`, overflow) and wrong
argument counts.

## Using the library

```cpp
#include "calc/evaluator.h"

calc::Context ctx;
double v = calc::evaluate("x = 2 ^ 10", ctx);  // 1024, and x is now set
```

Link against `calc_core` (CMake target `calc::core`). All failures throw
`calc::CalcError`; use `calc::format_error(input, err)` for a caret message.

## Layout

```
include/calc/   public headers
src/            library sources (calc_core)
apps/cli/       the calc command-line app
tests/          GoogleTest unit tests; tests/cli/ runs the built binary
.github/        CI and release workflows
```

See [CONTRIBUTING.md](CONTRIBUTING.md) for the development workflow and
[CHANGELOG.md](CHANGELOG.md) for release notes.
