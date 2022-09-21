cmake_minimum_required(VERSION 3.20)

project(OpenGlExample
    VERSION 1.0.0 
    LANGUAGES CXX)

find_path(STB_INCLUDE_DIRS "stb_image.h")

set (PROJECT_SOURCES 
    src/ExampleApp.cpp
    src/ExampleLayer.h
    src/ExampleLayer.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} 
PRIVATE  
    OpenGlCore)

target_include_directories(${PROJECT_NAME}  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${STB_INCLUDE_DIRS}")
add_compile_definitions(${PROJECT_NAME} ENABLE_ASSERTS)
add_custom_command(TARGET ${PROJECT_NAME} 
    COMMAND ${CMAKE_COMMAND}  ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/textures/"
    COMMAND ${CMAKE_COMMAND}  ARGS -E copy_directory "${CMAKE_SOURCE_DIR}/OpenGL-Core/assets/shaders/" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/shaders/")