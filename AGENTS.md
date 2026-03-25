# Repository Guidelines

## Project Structure & Module Organization
- Documentation lives in `doc/` (written in markdown).
- Data and tooling for CSV modifier sources live under `data/niflhel/`.
- Third-party helpers are vendored in `external/` (e.g., `external/libdicey`).
- CMake build output is expected in `build/`.

## Build, Test, and Development Commands
- Configure: `cmake -S . -B build` — generates build files in `build/`.
- Build: `cmake --build build` — compiles test executables and any targets.
- Run tests via CTest: `ctest --test-dir build` — currently registers `stats_test`.
- Run tests directly: `./build/stats_test` and `./build/mods_test` — useful since `mods_test` is not registered in CTest.
- `build/mods_test` requires a CSV file as input. `data/niflhel/combined.csv` can be used, if it exists.
- The test data file `data/niflhel/combined.csv` can be created by entering the directory `data/niflhel` and running the scripts `pull.sh` then `combine.py`.

## Coding Style & Naming Conventions
- Language: C++ (up to C++17). Tab indentation for blocks. Otherwise Allman style.
- Naming: lower_snake_case for functions and variables, `ALL_CAPS` for macros/constants, `snake_case` enums.

## Testing Guidelines
- Tests use plain `assert()` in standalone executables.
- Add new tests under `tests/` and include them in `CMakeLists.txt` via `ADD_EXECUTABLE`.
- Name tests by behavior, e.g., `test_damage_distribution()` in `tests/stats_test.cpp`.

## Data & Tooling Notes
- CSV definitions live in `data/niflhel/*.csv`; `data/niflhel/combine.py` produces `combined.csv`.
- If you modify CSVs, document the regeneration command and any assumptions.

## General notes
- Any results that are retained or cached or change game state must be deterministic.
- Results that merely displayed to the user do not have to be fully deterministic.
- We do not trust floating point to be deterministic between all platforms.

## Abilities management
- The code is in `argpstats.h` and documentation in `doc/Abilities.md`. Unit tests in `tests/stats_test.cpp`.
- This code is expected to be called very often, so needs to be fast and should cache results as much as possible.

## Modifiers management
- The code is in `mods.cpp`, interface in `mods.h`, and documentation in `doc/Modifiers.md`. Unit tests in `tests/mods_test.cpp`.

## Item generation
- The code is in `item.cpp`, interface in `item.h`, and documentation in `doc/Items.md`. Unit tests in `item_test.cpp`.
