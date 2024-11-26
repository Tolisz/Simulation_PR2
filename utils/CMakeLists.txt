
function(shaders_to_embedded_hpp TARGET_NAME SHADER_FOLDER)

	set(SHADER_TARGET_NAME "${TARGET_NAME}_shaders")
	file(GLOB SHADER_FILES ${SHADER_FOLDER}/*)
	set(COMPILED_SHADER_FILES)

	foreach(FILE IN LISTS SHADER_FILES)
		message(${FILE})

		cmake_path(GET FILE FILENAME FILE_NAME)
		string(REPLACE "." "_" OUTPUT_FILE_NAME ${FILE_NAME})
		string(APPEND OUTPUT_FILE_NAME ".hpp")
		string(PREPEND OUTPUT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

		add_custom_command(
			OUTPUT ${OUTPUT_FILE_NAME}
			COMMAND ${CMAKE_COMMAND} -E echo "// AUTO-GENERATED FILE CONTAINING SHADER SOURCE" > ${OUTPUT_FILE_NAME}
			COMMAND ${CMAKE_COMMAND} -E echo "#pragma once" >> ${OUTPUT_FILE_NAME}
			COMMAND ${CMAKE_COMMAND} -E echo "#include <string>" >> ${OUTPUT_FILE_NAME}
			DEPENDS ${FILE}
			COMMENT "[shader file generation]: ${OUTPUT_FILE_NAME}"
		)

		list(APPEND COMPILED_SHADER_FILES ${OUTPUT_FILE_NAME})

	endforeach()
	
	add_custom_target(${SHADER_TARGET_NAME} DEPENDS ${COMPILED_SHADER_FILES})
	add_dependencies(${TARGET_NAME} ${SHADER_TARGET_NAME})


endfunction()
