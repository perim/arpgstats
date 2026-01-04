# Repository Guidelines

## Project Structure & Module Organization
- Root headers contain the public library surface: `arpgstats.h`, `mods.h`, `item.h`, `base.h`.
- Implementation code is mostly header-based, with `mods.cpp` providing CSV-backed modifier loading.
- Tests live in `tests/` (simple C++ executables).
- Documentation lives in `doc/` (written in markdown).
- Data and tooling for CSV modifier sources live under `data/niflhel/`.
- Third-party helpers are vendored in `external/` (e.g., `external/libdicey`).
- CMake build output is expected in `build/` (not required, but standard).

## Build, Test, and Development Commands
- Configure: `cmake -S . -B build` — generates build files in `build/`.
- Build: `cmake --build build` — compiles test executables and any targets.
- Run tests via CTest: `ctest --test-dir build` — currently registers `stats_test`.
- Run tests directly: `./build/stats_test` and `./build/mods_test` — useful since `mods_test` is not registered in CTest.
- `build/mods_test` requires a CSV file as input. `data/niflhel/combined.csv` can be used, if it exists.
- The test data file `data/niflhel/combined.csv` can be created by entering the directory `data/niflhel` and running the scripts `pull.sh` then `combine.py`.

## Coding Style & Naming Conventions
- Language: C++ (up to C++17 style features can be used).
- Indentation: tabs for blocks (match existing headers and `mods.cpp`).
- Braces: Allman style (opening brace on its own line).
- Naming: lower_snake_case for functions and variables, `ALL_CAPS` for macros/constants, `snake_case` enums.
- No enforced formatter; keep changes consistent with nearby code.

## Testing Guidelines
- Tests use plain `assert()` in standalone executables.
- Add new tests under `tests/` and include them in `CMakeLists.txt` via `ADD_EXECUTABLE`.
- Name tests by behavior, e.g., `test_damage_distribution()` in `tests/stats_test.cpp`.

## Commit & Pull Request Guidelines
- Commit messages are short, imperative summaries (e.g., “Fix combine script”, “Code cleanup”).
- PRs should include: a short description, commands run (or “not run”), and any data changes under `data/niflhel/` with notes on regeneration steps if applicable.

## Data & Tooling Notes
- CSV definitions live in `data/niflhel/*.csv`; `data/niflhel/combine.py` produces `combined.csv`.
- If you modify CSVs, document the regeneration command and any assumptions.
