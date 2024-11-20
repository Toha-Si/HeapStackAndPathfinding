cmake_minimum_required(VERSION 2.8.12)
project(HSP)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Osmium REQUIRED io)
include_directories(${OSMIUM_INCLUDE_DIRS})

if(NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif()

find_package(Protozero REQUIRED)
include_directories(${PROTOZERO_INCLUDE_DIR})

if(NOT PROTOZERO_FOUND)
    message(WARNING "Protozero not found!\n")
endif()

add_executable(test PathfindingCityMap.cpp)
target_link_libraries(test ${OSMIUM_IO_LIBRARIES})
# find_package(OpenGL REQUIRED)
# find_package(GLUT REQUIRED)

# add_executable(gl testOpenGL.cpp)

# include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_UNCLUDE_DIRS})
# target_link_libraries(gl ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})