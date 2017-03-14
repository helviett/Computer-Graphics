cmake_minimum_required(VERSION 3.6)
project(CG)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Shader.cpp Shader.h ShaderProgram.cpp ShaderProgram.h Window.cpp Window.h)
add_executable(CG ${SOURCE_FILES})

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(CG ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})