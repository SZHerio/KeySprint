cmake_minimum_required(VERSION 3.15)
project(KeySprint CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Conan dependencies
find_package(raylib REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/core/Game.cpp
    src/core/TypingLogic.cpp
    src/core/TextUtils.cpp
    src/core/LessonLibrary.cpp
    src/core/ProgressManager.cpp
    src/core/AudioManager.cpp
    src/states/MainMenuState.cpp
    src/states/LessonSelectState.cpp
    src/states/ProgressState.cpp
    src/states/TypingState.cpp
    src/states/SettingsState.cpp
    src/states/ResultsState.cpp
)

# Copy assets folder to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE raylib nlohmann_json::nlohmann_json)
