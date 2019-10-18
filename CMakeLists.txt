cmake_minimum_required(VERSION 2.8)

project(kuszli-gb)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "inc/*.h*")

set(EXECUTABLE_OUTPUT_PATH ..)

add_executable (kuszli-gb ${SOURCES} ${HEADERS})
target_include_directories(kuszli-gb PRIVATE inc)
target_link_libraries(kuszli-gb sfml-graphics sfml-window sfml-audio sfml-system)
