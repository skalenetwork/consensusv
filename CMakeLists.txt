cmake_minimum_required(VERSION 2.8)

project(consensusv)
add_executable(consensusv main.cpp)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(consensusv ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )
