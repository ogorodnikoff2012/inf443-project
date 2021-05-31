cmake_minimum_required(VERSION 3.0)
project(INF443)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3 -DDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable")

include_directories(src)
include("${PROJECT_SOURCE_DIR}/lib/inf443-vcl/library/CMakeLists.txt")

set(SRC
        src/Game.cpp
        src/Game.h
        src/GameState.h
        src/SceneContext.cpp
        src/SceneContext.h
        src/GameContext.cpp
        src/GameContext.h
        src/Character.cpp
        src/Character.h
        src/Terrain.cpp src/Terrain.h src/KroshLoader.cpp src/KroshLoader.h src/Animation.cpp src/Animation.h src/Timeline.cpp src/Timeline.h src/Models.cpp src/Models.h)

add_executable(game src/main.cpp ${SRC} ${src_files_vcl} ${src_files_third_party})

target_link_libraries(game ${GLFW_LIBRARIES})
if (UNIX)
    target_link_libraries(game dl) #dlopen is required by Glad on Unix
endif ()