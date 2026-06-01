cmake_minimum_required(VERSION 3.20)

# Название проекта и используемый язык
project(SOLIS VERSION 1.0 LANGUAGES CXX)

# Требуем C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Подключаем модуль для скачивания зависимостей
include(FetchContent)

# 1. Скачиваем nlohmann/json
message(STATUS "Fetching nlohmann/json...")
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# 2. Скачиваем SFML 3.0.2
message(STATUS "Fetching SFML 3.0.2...")
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
)
# Отключаем сборку примеров и документации SFML для скорости
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SFML)

# 3. Добавляем наш исполняемый файл
add_executable(SOLIS 
    src/main.cpp
 "include/Utils/GameConstans.h" "include/Core/Events.h" "include/Core/EventBus.h"
 "include/Data/ResourceManager.h" "src/Data/ResourceManager.cpp" "include/Data/DataDefs.h"
 "include/Data/DataLoader.h" "src/Data/DataLoader.cpp" "include/Data/SaveProfile.h" "src/Data/SaveManager.cpp"
 "include/Data/SaveManager.h" "include/Gameplay/Player.h" "src/Gameplay/Player.cpp" "include/Gameplay/Enemy.h" "src/Gameplay/Enemy.cpp" "src/Gameplay/CombatSystem.cpp" "include/Gameplay/RunManager.h" "src/Gameplay/RunManager.cpp" "include/UI/IUIElement.h" "src/UI/IUIElement.cpp"  "src/UI/UIManager.cpp" "include/UI/UITextLog.h" "src/UI/UITextLog.cpp" "src/UI/UIProgressBar.cpp" "include/UI/UIProgressBar.h" "include/UI/UIButtonMenu.h" "src/UI/UIButtonMenu.cpp" "include/UI/ScreenRenderer.h" "src/UI/ScreenRenderer.cpp")

# 4. Указываем папки с заголовочными файлами (пока это src и include)
target_include_directories(SOLIS PRIVATE src include)

# 5. Линкуем библиотеки (модули SFML и JSON)
target_link_libraries(SOLIS PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System 
    nlohmann_json::nlohmann_json
)