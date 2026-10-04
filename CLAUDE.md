# CLAUDE.md

Guidance for Claude (and other agents) working in this repository.

## Commands

```sh
cmake -S . -B build                          # configure (Debug by default)
cmake --build build -j                       # build library, CLI and tests
ctest --test-dir build --output-on-failure   # run all tests
./build/calc "2 * (3 + 4)"                   # try the CLI

# formatting and lint, exactly as CI runs them
find include src apps tests \( -name '*.cpp' -o -name '*.h' \) -print0 | xargs -0 clang-format --dry-run --Werror
find src apps -name '*.cpp' -print0 | xargs -0 clang-tidy -p build
```

Sources are globbed: a new `src/*.cpp`, `apps/cli/*.cpp` or `tests/*_test.cpp`
file is picked up automatically on the next configure.

## Layout and ownership

Each Jira Subtask has an `agent:<role>` label. Stay inside your role's files;
if a change needs another area, note it on the ticket instead.

| Label            | Owns                                                       |
|------------------|------------------------------------------------------------|
| `agent:build`    | `CMakeLists.txt`, `cmake/`, `tests/CMakeLists.txt`         |
| `agent:errors`   | `include/calc/error.h`, `src/error.cpp`                    |
| `agent:tokenizer`| `include/calc/tokenizer.h`, `src/tokenizer.cpp`            |
| `agent:parser`   | `include/calc/ast.h`, `include/calc/parser.h`, `src/ast.cpp`, `src/parser.cpp` |
| `agent:evaluator`| `include/calc/evaluator.h`, `src/evaluator.cpp`, `include/calc/format.h`, `src/format.cpp` |
| `agent:cli`      | `apps/cli/`                                                |
| `agent:test`     | `tests/` (`*_test.cpp`, `tests/cli/`)                      |
| `agent:ci`       | `.github/workflows/ci.yml`, `.clang-format`, `.clang-tidy` |
| `agent:release`  | `.github/workflows/release.yml`, install/CPack rules       |
| `agent:docs`     | `README.md`, `CONTRIBUTING.md`, `CHANGELOG.md`, `CLAUDE.md`|

Pipeline: `tokenize()` -> `parse()` -> `evaluate()`; `Context` holds variables
and `ans`. The library does no I/O and has no global mutable state.

## Conventions

- C++17, Google-based style from `.clang-format`, 100-column limit.
- Warnings are errors (`-Wall -Wextra -Wpedantic -Wshadow -Wconversion`).
- Every library failure is a `calc::CalcError` with a `Kind` and a 0-based
  column; never return NaN or infinity silently.
- Every behaviour change ships with tests in `tests/`. CLI behaviour is tested
  end to end in `tests/cli/CMakeLists.txt`.
- Commit one finished Jira ticket per commit:
  `SCRUM-<epic>:SCRUM-<feature> <summary>` plus a `Jira:` link line per ticket
  (see CONTRIBUTING.md). Move the ticket to Done after pushing.
- Never push tags or trigger releases; a human tags releases.
