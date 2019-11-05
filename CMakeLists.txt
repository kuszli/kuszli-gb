cmake_minimum_required(VERSION 2.8)

#Enable Release build by defeult. It must be done before project() statement.
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)")
endif()
#You can also set it with -DCMAKE_BUILD_TYPE=build_type

project(kuszli-gb)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "inc/*.h*")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#If you want to specify output path for executable file you can do it here or you can pass it to cmake with -DEXECUTABLE_OUTPUT_PATH=your_path.
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

#You should specify sfml path. You could do it here with set command or passing -DSFML_DIR=your_sfml_path option to cmake.
#set(SFML_DIR your_sfml_path)

if(NOT SFML_DIR)
	message(FATAL_ERROR "Sfml directory not specified")
endif()

set(SFML_STATIC_LIBRARIES FALSE)

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)

if(SFML_FOUND)
	add_executable (kuszli-gb ${SOURCES} ${HEADERS})
	target_include_directories(kuszli-gb PRIVATE inc)
	target_link_libraries(kuszli-gb sfml-graphics sfml-audio)
endif()
