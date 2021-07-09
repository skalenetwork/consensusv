cmake_minimum_required(VERSION 2.8)

# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(consensusv)
add_executable(consensusv main.cpp)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

FIND_PACKAGE( Boost COMPONENTS filesystem REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(consensusv ${OPENGL_LIBRARIES} ${GLUT_LIBRARY}  Boost::filesystem )
