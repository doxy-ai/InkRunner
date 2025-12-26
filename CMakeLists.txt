cmake_minimum_required(VERSION 3.28)
project(InkRunner CXX)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(inkcpp)
add_subdirectory(embed)

set(INK_RUNNER_GENERATOR_DIR "" CACHE PATH "Path to the directory containing the compile_ink_bin executable.")
message("Generator Directory: ${INK_RUNNER_GENERATOR_DIR}")
if(INK_RUNNER_GENERATOR_DIR STREQUAL "")
	add_executable(compile_ink_bin generators/compile_ink_bin.cpp)
	target_link_libraries(compile_ink_bin PUBLIC inkcpp inkcpp_shared inkcpp_compiler)

	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/story.ink.bin" 
		COMMAND compile_ink_bin "${PROJECT_SOURCE_DIR}/story.ink.json"
		DEPENDS "${PROJECT_SOURCE_DIR}/story.ink.json"
	)
else()
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/story.ink.bin" 
		COMMAND ${INK_RUNNER_GENERATOR_DIR}/compile_ink_bin "${PROJECT_SOURCE_DIR}/story.ink.json"
		DEPENDS "${PROJECT_SOURCE_DIR}/story.ink.json"
	)
endif()

set(ABSOLUTE_STORY_INK_BIN_PATH "${CMAKE_CURRENT_BINARY_DIR}/story.ink.bin")
cmake_path(RELATIVE_PATH ABSOLUTE_STORY_INK_BIN_PATH OUTPUT_VARIABLE RELATIVE_STORY_INK_BIN_PATH)
add_library(embeds STATIC generators/empty.cpp)
b_embed(embeds ${RELATIVE_STORY_INK_BIN_PATH})
add_compile_definitions(embeds PUBLIC -DRELATIVE_STORY_INK_BIN_PATH="${RELATIVE_STORY_INK_BIN_PATH}")

add_executable(main main.cpp)
target_link_libraries(main inkcpp inkcpp_shared inkcpp_compiler embeds)

if(EMSCRIPTEN)
	include(FetchContent)
	FetchContent_Declare(
		emscripten_pty
		URL https://unpkg.com/xterm-pty/emscripten-pty.js
		DOWNLOAD_NO_EXTRACT TRUE
	)
	FetchContent_MakeAvailable(emscripten_pty)

	em_link_js_library(main ${emscripten_pty_SOURCE_DIR}/emscripten-pty.js)
	target_link_options(main PUBLIC -sFORCE_FILESYSTEM -sASYNCIFY)

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)