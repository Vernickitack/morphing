cmake_minimum_required(VERSION 3.10)
project(morphing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

set(Qt5_DIR "C:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5" CACHE PATH "Qt5 directory" FORCE)
find_package(Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/tinygltf)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/glm)

set(SOURCES
        src/app/main.cpp
        src/app/Window.cpp
        src/base/GLWidget.cpp
        src/base/Camera.cpp
        src/base/MorphModel.cpp
        thirdparty/tinygltf/tiny_gltf.cc
        )


qt5_wrap_cpp(MOC_SOURCES
        src/app/Window.h
        src/base/GLWidget.h
        src/base/Camera.h
        src/base/MorphModel.h
        )

qt5_add_resources(SHADERS src/app/shaders/shaders.qrc)

add_executable(morphing
        ${SOURCES}
        ${MOC_SOURCES}
        ${SHADERS}
        )

target_compile_options(morphing PRIVATE /W1)

file(COPY src/app/targets DESTINATION ${CMAKE_BINARY_DIR}/Release)
file(COPY src/app/shaders DESTINATION ${CMAKE_BINARY_DIR}/Release)

target_link_libraries(morphing
        Qt5::Core
        Qt5::Widgets
        Qt5::OpenGL
        OpenGL::GL
        )