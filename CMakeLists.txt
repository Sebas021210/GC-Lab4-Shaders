cmake_minimum_required(VERSION 3.25)
project(GC_Lab4_Shaders)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SDL2_INCLUDE_DIR "C:/Libs/SDL/include")
set(SDL2_LIB_DIR "C:/Libs/SDL/lib/x64")
set(SDL2_image_DIR "C:/Libs/SDL2_image/include")
set(SDL2_image_LIB_DIR "C:/Libs/SDL2_image/lib/x64")

# Agregar la ruta de inclusión de la biblioteca glm
set(GLM_INCLUDE_DIR "C:/Users/50242/Documents/Universidad/TercerAño/GraficasComputadora/GC-Lab4-Shaders/glm")

# Utiliza GLM_INCLUDE_DIR en lugar de CMAKE_INCLUDE_PATH
include_directories(${SDL2_INCLUDE_DIR} ${GLM_INCLUDE_DIR} ${SDL2_image_DIR} ${SDL2_image_LIB_DIR})
link_directories(${SDL2_LIB_DIR} ${SDL2_image_DIR} ${SDL2_image_LIB_DIR})

add_executable(GC_Lab4_Shaders main.cpp camera.h color.h FastNoise.h FastNoiseLite.h fragment.h framebuffer.h line.h noise.h ObjLoader.cpp print.h shaders.h texture.h triangle.h uniforms.h)
target_link_libraries(GC_Lab4_Shaders SDL2main SDL2 SDL2_image)
