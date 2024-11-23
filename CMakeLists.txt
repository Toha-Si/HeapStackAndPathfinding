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

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

set(IMGUI_DIR imgui)
set(IMGUI_SRC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

include_directories(${OSMIUM_INCLUDE_DIRS} 
                    ${PROTOZERO_INCLUDE_DIR} 
                    ${OPENGL_INCLUDE_DIRS} 
                    ${GLUT_UNCLUDE_DIRS} 
                    ${IMGUI_DIR} ${IMGUI_DIR}/backends)

add_executable(test PathfindingCityMap.cpp ${IMGUI_SRC})

target_link_libraries(test 
                    ${OSMIUM_IO_LIBRARIES} 
                    ${OPENGL_LIBRARIES} 
                    ${GLUT_LIBRARIES}
                    ${GLEW_INCLUDE_DIRS}
                    glfw GLEW::GLEW
                    ${CMAKE_DL_LIBS})