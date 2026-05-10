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
 "include/Utils/GameConstans.h")

# 4. Указываем папки с заголовочными файлами (пока это src и include)
target_include_directories(SOLIS PRIVATE src include)

# 5. Линкуем библиотеки (модули SFML и JSON)
target_link_libraries(SOLIS PRIVATE 
    SFML::Graphics 
    SFML::Window 
    SFML::System 
    nlohmann_json::nlohmann_json
)