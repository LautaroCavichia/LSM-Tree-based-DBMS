cmake_minimum_required(VERSION 3.10)

project(LSMTreeDBMS VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(tests    
tests/memtable_test.cpp
tests/sstable_test.cpp
tests/db_test.cpp
)

target_link_libraries(tests gtest_main)

add_executable(LSMTreeDBMS src/main.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCES "src/*.cpp")
target_sources(LSMTreeDBMS PRIVATE ${SOURCES})

enable_testing()


# target_link_libraries(LSMTreeDBMS <library_name>)