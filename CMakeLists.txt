cmake_minimum_required(VERSION 3.20)

project(Simulation_PR2 VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)

option(PR2_EMBEDDED_SHADERS "All shaders will be embedded into program binary" ON)
option(PR2_SHOW_DEBUG_CONSOLE "If On should open only window without a console" OFF)

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
	message("-- RP2: PR2_EMBEDDED_SHADERS option is ON")

	target_compile_definitions(${PR2_EXECUTABLE_NAME} PRIVATE "EMBEDDED_SHADERS")
    shaders_to_embedded_hpp(${PR2_EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/shaders)
endif()

if (MSVC)
	target_compile_options(${PR2_EXECUTABLE_NAME} PRIVATE /W4)

	if (NOT PR2_SHOW_DEBUG_CONSOLE)

		message("-- RP2: PR2_SHOW_DEBUG_CONSOLE option is OFF")
		set_target_properties(${PR2_EXECUTABLE_NAME} PROPERTIES 
			LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
	endif()

else()
	target_compile_options(${PR2_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
