cmake_minimum_required(VERSION 3.8)

project(CPP_Chess)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SFML_VERSION "2.6.0")
Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
        sfml
        GIT_REPOSITORY "https://github.com/SFML/SFML.git"
        GIT_TAG        "${SFML_VERSION}"
        GIT_PROGRESS TRUE
)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
endif()

find_package(SFML ${SFML_VERSION} COMPONENTS system window graphics)

include_directories(test include)

add_executable(Server
    main_server.cpp
    src/board.cpp
    src/server.cpp
    src/message.cpp
    src/cooldown.cpp
)

add_executable(Client
    main_client.cpp
    src/client.cpp
    src/board.cpp
    src/game.cpp
    src/message.cpp
)

target_link_libraries (Client sfml-graphics sfml-window sfml-system)



