cmake_minimum_required(VERSION 2.8.12)
project(HSP)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Osmium REQUIRED io)
include_directories(${OSMIUM_INCLUDE_DIRS})

if(NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif()

find_package(Protozero REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${PROTOZERO_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS} ${GLUT_UNCLUDE_DIRS})

if(NOT PROTOZERO_FOUND)
    message(WARNING "Protozero not found!\n")
endif()

add_executable(test PathfindingCityMap.cpp)

target_link_libraries(test ${OSMIUM_IO_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_INCLUDE_DIRS} glfw GLEW::GLEW)