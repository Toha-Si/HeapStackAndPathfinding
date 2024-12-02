cmake_minimum_required(VERSION 2.8.12)
project(HSP)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Osmium REQUIRED io)

if(NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif()

find_package(Protozero REQUIRED)

if(NOT PROTOZERO_FOUND)
    message(WARNING "Protozero not found!\n")
endif()

find_package(Boost REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

set(IMGUI_DIR ../../imgui)

set(SOURCES
    main.cpp
    appStateMachine.cpp
    fileMapHandler.cpp
    graph.cpp
    controller.cpp
    renderer.cpp
    widgets.cpp
    algos.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(HEADERS
    appStateMachine.hpp
    fileMapHandler.hpp
    graph.hpp
    controller.hpp
    renderer.hpp
    widgets.hpp
    algos.hpp
    inputDataGL.hpp
)

include_directories(${OSMIUM_INCLUDE_DIRS} 
                    ${PROTOZERO_INCLUDE_DIR} 
                    ${OPENGL_INCLUDE_DIRS} 
                    ${GLUT_UNCLUDE_DIRS} 
                    ${IMGUI_DIR} ${IMGUI_DIR}/backends
                    ${Boost_INCLUDE_DIRS}
                    ${CMAKE_SOURCE_DIR})

add_executable(Pathfinding ${SOURCES} ${HEADERS})

#target_include_directories(Pathfinding PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(Pathfinding 
                    ${OSMIUM_IO_LIBRARIES} 
                    ${OPENGL_LIBRARIES} 
                    ${GLUT_LIBRARIES}
                    ${GLEW_INCLUDE_DIRS}
                    glfw GLEW::GLEW
                    ${CMAKE_DL_LIBS})