cmake_minimum_required(VERSION 3.10)

project(LSMTreeDBMS VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(LSMTreeDBMS src/main.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCES "src/*.cpp")
target_sources(LSMTreeDBMS PRIVATE ${SOURCES})

# target_link_libraries(LSMTreeDBMS <library_name>)