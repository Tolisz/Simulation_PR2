cmake_minimum_required(VERSION 3.20)

project(Simulation_PR2 VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(externals)

# ===============================

set(PR2_LINK_LIBS 	PUBLIC glm
              		PUBLIC glfw
              		PUBLIC glad
					PUBLIC imgui)

set(PR2_EXECUTABLE_NAME SimulationPr2)
file(GLOB RP2_SOURCE_FILES source/*.cpp)

add_executable(${PR2_EXECUTABLE_NAME} ${RP2_SOURCE_FILES})
target_link_libraries(${PR2_EXECUTABLE_NAME} ${PR2_LINK_LIBS})

if (MSVC)
	target_compile_options(${PR2_EXECUTABLE_NAME} PRIVATE /W4)
else()
	target_compile_options(${PR2_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
