cmake_minimum_required(VERSION 3.20)

project(OpenGlExample
    VERSION 1.0.0 
    LANGUAGES CXX)

#find_package(glfw3 REQUIRED)
#find_package(GLEW REQUIRED)
#find_package(OpenGL REQUIRED COMPONENTS OpenGL)
#find_package(glm CONFIG REQUIRED)
#find_package(imgui CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb_image.h")
#find_library(SPIRV_TOOL_LIB SPIRV-Tools)

set (PROJECT_SOURCES 
    src/ExampleApp.cpp
    src/ExampleLayer.h
    src/ExampleLayer.cpp)
    #src/Renderer.h
    #src/Renderer.cpp)


add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE  
OpenGlCore    

 #${SPIRV-Tools}
 
)


#target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${GLEW_LIBRARIES} GLEW::GLEW OpenGL::GL glm::glm imgui::imgui) 
target_include_directories(${PROJECT_NAME}  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${STB_INCLUDE_DIRS}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND}  ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/textures/")