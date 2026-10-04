# Changelog

## v0.1.0 (unreleased)

- Expression library `calc_core`: tokenizer, recursive-descent parser,
  evaluator with built-in functions, constants, variables and `ans`.
- Typed errors (`CalcError`) with column positions and caret formatting.
- `calc` command line: one-shot mode, interactive REPL, piped input,
  `--help` and `--version`.
- CMake build with install target, GoogleTest unit tests and CLI tests.
- GitHub Actions CI (GCC, Clang, macOS, clang-format, clang-tidy) and a
  tag-triggered release workflow.
