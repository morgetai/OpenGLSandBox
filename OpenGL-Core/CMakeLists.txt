cmake_minimum_required(VERSION 3.20)

project(OpenGlCore
    VERSION 1.0.0 
    LANGUAGES CXX)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)


#find_package(spirv_cross_reflect CONFIG REQUIRED)
#find_package(spirv_cross_cpp CONFIG REQUIRED)
#find_package(SPIRV-Tools-opt CONFIG REQUIRED)
#find_package(SPIRV-Tools-link CONFIG REQUIRED)
#find_package(glad CONFIG REQUIRED)
    
find_path(STB_INCLUDE_DIRS "stb_image.h")
#find_library(SPIRV_TOOL_LIB SPIRV-Tools)

#find_path(GLAD_INCLUDE_DIR "glad.h")

#message(STATUS "glad dir ${GLAD_INCLUDE_DIR}")

set (PROJECT_SOURCES 
    #Core
    src/GLCore.h
    src/GLCoreUtils.h
    #platform
    #GLCore
    src/GLCore/Core/Application.cpp
    src/GLCore/Core/Application.h
    src/GLCore/Core/Core.h
    src/GLCore/Core/Input.h
    src/GLCore/Core/KeyCodes.h
    src/GLCore/Core/Layer.cpp
    src/GLCore/Core/Layer.h
    src/GLCore/Core/LayerStack.cpp
    src/GLCore/Core/LayerStack.h
    src/GLCore/Core/Log.cpp
    src/GLCore/Core/Log.h
    src/GLCore/Core/MouseButtonCodes.h
    src/GLCore/Core/Timestep.h
    src/GLCore/Core/Window.h
    src/GLCore/Core/Timer.h
    src/GLCore/Core/MyAssert.h
    #events
    src/GLCore/Events/ApplicationEvent.h
    src/GLCore/Events/Event.h
    src/GLCore/Events/KeyEvent.h
    src/GLCore/Events/MouseEvent.h
    #imgui
    #src/GLCore/ImGui/ImGuiBuild.cpp
    src/GLCore/ImGui/ImGuiLayer.cpp
    src/GLCore/ImGui/ImGuiLayer.h
    #Util
    src/GLCore/Util/OpenGLDebug.cpp
    src/GLCore/Util/OpenGLDebug.h
    src/GLCore/Util/OrthographicCamera.cpp
    src/GLCore/Util/OrthographicCamera.h
    src/GLCore/Util/OrthographicCameraController.cpp
    src/GLCore/Util/OrthographicCameraController.h
    #Renderer
    src/GLCore/Renderer/Shader.cpp
    src/GLCore/Renderer/Shader.h
    src/GLCore/Renderer/OpenGLTexture.h
    src/GLCore/Renderer/OpenGLTexture.cpp
    src/GLCore/Renderer/Renderer2D.cpp
    src/GLCore/Renderer/Renderer2D.h
    src/GLCore/Renderer/VertexArray.h
    src/GLCore/Renderer/VertexArray.cpp
    src/GLCore/Renderer/VertexBuffer.h
    src/GLCore/Renderer/VertexBuffer.cpp
    src/GLCore/Renderer/Buffer.h
    src/GLCore/Renderer/Buffer.cpp
    src/GLCore/Renderer/UniformBuffer.h
    src/GLCore/Renderer/UniformBuffer.cpp
    #platform
    src/Platform/Windows/WindowsInput.cpp
    src/Platform/Windows/WindowsInput.h
    src/Platform/Windows/WindowsWindow.cpp
    src/Platform/Windows/WindowsWindow.h)

#set(GLAD_SOURCES 
#glad/src/glad.c
#glad/include/glad/glad.h)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
#add_library(GLAD_LIB STATIC ${GLAD_SOURCES})

#set_target_properties(GLAD_LIB PROPERTIES LINKER_LANGUAGE C)
#message(STATUS "spriv!!! ${SPIRV_TOOL_LIB}")

target_link_libraries(${PROJECT_NAME}
PUBLIC
    imgui::imgui 
 #   "${SPIRV_TOOL_LIB}"
#"${HLSL_LIB}"
#"${SPIRVTOOLS}"
#"${SPIRVTOOLSOPT}"
#spirv-cross-glsl
PRIVATE
    glfw 
    ${GLEW_LIBRARIES} 
    GLEW::GLEW 
    OpenGL::GL
    glm::glm
    #spirv-cross-core
    #spirv-cross-glsl
    #"${HLSL_LIB}"
    #unofficial::shaderc_util::shaderc_util
    #unofficial::shaderc::shaderc
    
 #   spirv-cross-cpp
    
    spdlog::spdlog 
    spdlog::spdlog_header_only)
    #GLAD_LIB)
    #glad::glad) 

target_include_directories(${PROJECT_NAME}  PUBLIC 
    #"${CMAKE_CURRENT_SOURCE_DIR}/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GLCore"
    "/Users/andriihridin/Documents/Programming/OpenGlSandbox/vcpkg/installed/x64-osx/include/"
    PRIVATE
    
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GLCore/Core"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GLCore/Events"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GLCore/ImGui"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows"
    ${STB_INCLUDE_DIRS}  )

    add_custom_command(TARGET ${PROJECT_NAME} 
        POST_BUILD
    COMMAND ${CMAKE_COMMAND}  ARGS -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")