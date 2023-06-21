cmake_minimum_required(VERSION 3.8)

project(CPP_Chess)

include(FetchContent)

set(CMAKE_CXX_STANDARD 14)
set(SFML_VERSION "2.5.1")

FetchContent_Declare(
        sfml
        GIT_REPOSITORY "https://github.com/SFML/SFML.git"
        GIT_TAG        "${SFML_VERSION}"
)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
endif()

find_package(SFML 2.5.1 COMPONENTS system window graphics)

include_directories(external test src)

add_executable(${PROJECT_NAME} 
    main.cpp
    src/board.cpp
    src/game.cpp
    src/server.cpp
    src/client.cpp
)

target_link_libraries (${PROJECT_NAME} sfml-graphics sfml-window sfml-system)



