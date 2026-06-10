# Project SOLIS - Project Context & Instructions

This document provides foundational context, architectural mandates, and development workflows for **SOLIS**, a data-driven C++ RPG/Roguelike.

## 1. Project Overview
**SOLIS** is a C++17 game where the player navigates a dark world, managing their "Heat" (Jhar) and engaging in both physical and social combat. The game utilizes a **Data-Driven** architecture, where most game logic, entities, and content are defined in JSON files.

### Core Tech Stack
- **Language:** C++17
- **Framework:** SFML 3.0.2 (Window management, rendering, input)
- **Data Handling:** [nlohmann/json](https://github.com/nlohmann/json)
- **Build System:** CMake (FetchContent is used for dependency management)

## 2. Architecture & Design
The project follows a **State Machine** pattern for high-level flow and a modular UI system.

### State Machine
- `MainMenuState`: Game entry point.
- `HubState` (Ancestral Hearth): Meta-progression and equipment.
- `ExplorationState`: Room-to-room navigation.
- `CombatState`: Turn-based combat (Physical & Social).
- `EndingState`: Calculates and displays endings based on player choices.

### Key Components
- `Game`: Core engine class owning the window and State Machine.
- `ResourceManager`: Singleton for font and ASCII art loading.
- `DataManager`: Handles parsing of all JSON databases (`enemies.json`, `rooms.json`, `lore.json`, etc.).
- `RunManager`: Manages the state of the current "run" (level, rooms, ideology score).

### Data-Driven mandates
All content MUST be externalized to JSON in the `data/` directory. Do not hardcode enemy stats, room descriptions, or lore text.

## 3. Building and Running

### Prerequisites
- C++17 compliant compiler (MSVC 2019+, GCC 9+, Clang 9+).
- CMake 3.20+.
- Internet connection (first build) to fetch SFML and nlohmann/json via `FetchContent`.

### Build Commands
```powershell
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build . --config Release
```

### Running
Execute the `SOLIS` binary from the root directory to ensure asset paths (e.g., `data/`, `assets/`) are correctly resolved.

## 4. Development Conventions

### SFML 3.0.2 Specifics
- **Event Handling:** Uses `std::variant`. `pollEvent()` returns `std::optional<sf::Event>`.
- **Vectors:** Many functions now strictly take `sf::Vector2<T>` (e.g., `VideoMode({1920, 1080})`).
- **Angles:** Uses `sf::Angle` (e.g., `sf::degrees(90)`).
- **Resources:** Default constructors for `sf::Text`, `sf::Sprite`, and `sf::Sound` are removed; they require a reference to their resource (Font, Texture, Buffer).

### Naming & Style
- Follow the existing CamelCase convention for classes and files.
- Header files in `include/`, source files in `src/`.
- **Note:** Be aware of existing typos like `GameConstans.h` (Utils) and maintain consistency with current file names unless a refactor is requested.

### Documentation
- `documents/Technical_documentation.md`: The definitive Technical Design Document (TDD).
- `documents/MVP.md`: The roadmap for implementation phases (MVP 0 to MVP 5).

## 5. Implementation Roadmap (Reference)
The project is currently in the **MVP 0 / MVP 1** phase.
1. **MVP 0:** Technical Frame & UI (State Machine, UIBox, Word Wrap).
2. **MVP 1:** Data Parsing & Navigation (DataManager, RunManager, ExplorationState).
3. **MVP 2:** Combat System (Player/Enemy entities, CombatState).
4. **MVP 3:** Social System & Lore (Empathy, DialogueGenerator).
5. **MVP 4:** Meta-progression (SaveManager, HubState).
6. **MVP 5:** Narrative & Endings (Quests, Bosses, Final Logic).

---
*Last Updated: June 2026*
