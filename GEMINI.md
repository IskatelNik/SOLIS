# SOLIS C++ Project Context

## Project Overview
**SOLIS** is a console-style graphical RPG built in C++17, utilizing a custom UI layer on top of **SFML 3.0.2**. It heavily relies on data-driven design using **nlohmann/json**. The game features turn-based combat, a unique narrative dialogue system (Social Combat), heat/energy management, and roguelite progression (runs, metacurrency, hub, run-based modifiers).

## Key Technologies
- **Language:** C++17
- **Build System:** CMake (3.20+)
- **Graphics/System/Windowing:** SFML 3.0.2 (fetched via FetchContent)
- **Data Parsing:** nlohmann/json 3.11.3 (fetched via FetchContent)

## Directory Structure & Architecture
- `src/` & `include/`: Source and header files logically split into subsystems.
  - `Core/`: Main Game loop, StateMachine, and ResourceManager.
  - `Entities/`: Player, Enemy, BossEnemy.
  - `Managers/`: 
    - `DataManager` (Singleton for parsing all JSON content).
    - `RunManager` (Manages current run data: rooms, events, health resets).
    - `SaveManager` (Handles persistent progress in `save.json`).
  - `Mechanics/`: Combat calculations, Skill processing, DialogueGenerator, Artifacts.
  - `States/`: Specific game phases (`MainMenuState`, `ExplorationState`, `CombatState`, `HubState`, `EndingState`).
  - `UI/`: Custom rendering components, heavily relying on `UIBox` (handles borders, background, and Word Wrap).
- `data/`: JSON databases (`rooms.json`, `enemies.json`, `skills.json`, `save.json`, etc.). 
- `assets/`: Fonts and other visual/audio assets.
- `documents/`: Contains the Technical Design Documents (TDD) and detailed MVP milestones.

## Development Conventions & Constraints
- **State Machine Pattern:** Use `State` inheritance to implement separate screens. Never mix logic of different states.
- **Data-Driven Approach:** Avoid hardcoding game logic values. Content (Lore, Enemies, Upgrades) MUST be read dynamically via `DataManager`.
- **Error Handling:** Protect against missing assets (fonts, JSON files) with graceful fallbacks or console errors to prevent runtime segfaults.
- **UI Modularization:** UI layout relies on `UIBox` configuration (e.g., TopBar, MainDisplay, ActionMenu) rather than absolute pixel plotting for individual texts.
- **Post-Build Sync:** CMake is configured to copy `data/` and `assets/` to the build directory automatically upon successful build.

## Building and Running
```bash
mkdir build
cd build
cmake ..
cmake --build .
# Run the executable generated in the build directory.
# Assets and data folders will be automatically copied next to the binary.
```