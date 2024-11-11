cmake_minimum_required(VERSION 3.20)

project(Simulation_PR1 VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(externals)

# ===============================

set(PR1_LINK_LIBS 	PUBLIC glm
              		PUBLIC glfw
              		PUBLIC glad
					PUBLIC imgui)

set(PR1_EXECUTABLE_NAME SimulationPr2)
file(GLOB RP1_SOURCE_FILES source/*.cpp)

add_executable(${PR1_EXECUTABLE_NAME} ${RP1_SOURCE_FILES})
target_link_libraries(${PR1_EXECUTABLE_NAME} ${PR1_LINK_LIBS})
