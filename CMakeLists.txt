cmake_minimum_required(VERSION 3.8)

Project(CPP_Chess)

set(CMAKE_CXX_STANDARD 14)

find_package(SFML 2.5.1 COMPONENTS system window graphics)

include_directories(external test src)

add_executable(${PROJECT_NAME} 
    main.cpp
    src/board.cpp
    src/game.cpp
)

target_link_libraries (${PROJECT_NAME} sfml-graphics sfml-window sfml-system)



