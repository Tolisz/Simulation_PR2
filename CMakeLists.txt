cmake_minimum_required(VERSION 3.20)

project(Simulation_PR2 VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)

option(PR2_EMBEDDED_SHADERS "All shaders will be build in into program binary" ON)

add_subdirectory(externals)
add_subdirectory(utils)

# ===============================

set(PR2_LINK_LIBS 	PUBLIC glm
              		PUBLIC glfw
              		PUBLIC glad
					PUBLIC imgui)

set(PR2_EXECUTABLE_NAME SimulationPr2)
file(GLOB RP2_SOURCE_FILES source/*.cpp)

add_executable(${PR2_EXECUTABLE_NAME} ${RP2_SOURCE_FILES})
target_link_libraries(${PR2_EXECUTABLE_NAME} ${PR2_LINK_LIBS})

if (PR2_EMBEDDED_SHADERS)
    shaders_to_embedded_hpp(${PR2_EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/shaders)
endif()

if (MSVC)
	# target_compile_options(${PR2_EXECUTABLE_NAME} PRIVATE /W4)
else()
	target_compile_options(${PR2_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
