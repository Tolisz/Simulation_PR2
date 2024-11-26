cmake_minimum_required(VERSION 3.20)

project(Simulation_PR2 VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)

option(PR2_EMBEDDED_SHADERS "All shaders will be build in into program binary" OFF)

add_subdirectory(externals)
add_subdirectory(utils)

# ===============================


# function(embed_shader source_file output_file)
#     message(${CMAKE_COMMAND})

#     add_custom_command(
#         OUTPUT ${output_file}
#         COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shaders
#         # COMMAND ${CMAKE_COMMAND} -E echo "// Auto-generated file containing shader source" > ${output_file}
#         # COMMAND ${CMAKE_COMMAND} -E echo "#include <string>" >> ${output_file}
#         # COMMAND ${CMAKE_COMMAND} -E echo "const std::string shader_${source_file} = R\"(" >> ${output_file}
#         # COMMAND ${CMAKE_COMMAND} -E cat ${source_file} >> ${output_file}
#         # COMMAND ${CMAKE_COMMAND} -E echo ")\";" >> ${output_file}
#         DEPENDS ${source_file}
#         COMMENT "Embedding shader: ${source_file}"
#         VERBATIM
#     )
# endfunction()

# foreach(shader_file IN LISTS SHADER_FILES)
#     get_filename_component(shader_name ${shader_file} NAME_WE)
#     message(${shader_name})
#     message(${CMAKE_CURRENT_BINARY_DIR})
#     set(output_cpp "${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader_name}.cpp")
#     embed_shader(${shader_file} ${output_cpp})
#     list(APPEND GENERATED_SHADER_SOURCES ${output_cpp})
# endforeach()

# ===============================

set(PR2_LINK_LIBS 	PUBLIC glm
              		PUBLIC glfw
              		PUBLIC glad
					PUBLIC imgui)

set(PR2_EXECUTABLE_NAME SimulationPr2)
file(GLOB RP2_SOURCE_FILES source/*.cpp)

add_executable(${PR2_EXECUTABLE_NAME} ${RP2_SOURCE_FILES})
target_link_libraries(${PR2_EXECUTABLE_NAME} ${PR2_LINK_LIBS})

shaders_to_embedded_hpp(${PR2_EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/shaders)

if (MSVC)
	# target_compile_options(${PR2_EXECUTABLE_NAME} PRIVATE /W4)
else()
	target_compile_options(${PR2_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
