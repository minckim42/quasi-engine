cmake_minimum_required(VERSION 3.13)

project(
    quasi_3d
)

set(CMAKE_CXX_STANDARD 20)

set(
    SOURCE_FILES
	source/opengl/buffer.cpp
	source/opengl/framebuffer.cpp
	source/opengl/shader.cpp
	source/opengl/texture.cpp
	source/image_processor/stb_image.cpp
	source/animation.cpp
	source/bone.cpp
	source/camera.cpp
	source/glfw_window.cpp
	source/image.cpp
	source/material.cpp
	source/mesh.cpp
	source/model.cpp
	source/model_load.cpp
	source/move.cpp
	source/renderer.cpp
	main.cpp
)

add_executable(
    quasi_3d
	${SOURCE_FILES}
)

target_include_directories(
    quasi_3d
	PUBLIC
	include
	glad/include
	glfw-3.3.8/include
	assimp/include
)

target_compile_definitions(
    quasi_3d
	PRIVATE
    ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_subdirectory(
	glad
)

add_subdirectory(
	glfw-3.3.8
)

add_subdirectory(
	assimp
)

target_link_libraries(
	quasi_3d
	PRIVATE
	glad
	glfw
	assimp
)