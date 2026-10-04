# Contributing

Work is planned in Jira (project **SCRUM** on vibhor-dev.atlassian.net):
Epics contain Features, and Features contain Subtasks. Each Subtask carries an
`agent:<role>` label saying which area of the code it belongs to.

## Workflow

1. Pick a ticket whose "is blocked by" links are all Done and move it to
   **In Progress**.
2. Make the change. Every behaviour change ships with tests in `tests/`.
3. Run the checks below locally; all must pass.
4. Commit one finished ticket per commit (see message format), push, and move
   the ticket to **Done**.

## Commit messages

```
SCRUM-<epic>:SCRUM-<feature> <what the commit does>

Jira: https://vibhor-dev.atlassian.net/browse/SCRUM-<subtask>
```

Example:

```
SCRUM-5:SCRUM-8 Add unit tests for tokenizer

Jira: https://vibhor-dev.atlassian.net/browse/SCRUM-23
```

When one commit completes several Subtasks, list one `Jira:` line per ticket.

## Local checks

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

# formatting and lint (same as CI)
find include src apps tests \( -name '*.cpp' -o -name '*.h' \) -print0 | xargs -0 clang-format --dry-run --Werror
find src apps -name '*.cpp' -print0 | xargs -0 clang-tidy -p build
```

The build uses `-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror`; fix
warnings rather than silencing them.

## Releases

Bump `VERSION` in the top-level `CMakeLists.txt`, add a `CHANGELOG.md` entry,
then a maintainer tags the commit (`git tag v0.2.0 && git push origin v0.2.0`).
The release workflow builds Linux and macOS archives and publishes them.
