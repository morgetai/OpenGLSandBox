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
    
find_path(STB_INCLUDE_DIRS "stb_image.h")

set (PROJECT_SOURCES 
    #Core
    src/GLCore.h
    src/GLCoreUtils.h
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
    src/GLCore/Core/MouseCodes.h
    #events
    src/GLCore/Events/ApplicationEvent.h
    src/GLCore/Events/Event.h
    src/GLCore/Events/KeyEvent.h
    src/GLCore/Events/MouseEvent.h
    #imgui
    src/GLCore/ImGui/ImGuiLayer.cpp
    src/GLCore/ImGui/ImGuiLayer.h
    #Util
    src/GLCore/Util/OpenGLDebug.cpp
    src/GLCore/Util/OpenGLDebug.h
    src/GLCore/Util/ProjectionCamera.cpp
    src/GLCore/Util/ProjectionCamera.h
    src/GLCore/Util/OrthographicCamera.cpp
    src/GLCore/Util/OrthographicCamera.h
    src/GLCore/Util/EditorCamera.h
    src/GLCore/Util/EditorCamera.cpp
    src/GLCore/Util/CameraController.h
    src/GLCore/Util/CameraController.cpp
    #Renderer
    src/GLCore/Renderer/Shader.cpp
    src/GLCore/Renderer/Shader.h
    src/GLCore/Renderer/OpenGLTexture.h
    src/GLCore/Renderer/OpenGLTexture.cpp
    src/GLCore/Renderer/Renderer2D.cpp
    src/GLCore/Renderer/Renderer2D.h
    src/GLCore/Renderer/Renderer3D.h
    src/GLCore/Renderer/Renderer3D.cpp
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

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})
add_compile_definitions(${PROJECT_NAME} ENABLE_ASSERTS)

target_link_libraries(${PROJECT_NAME}
PUBLIC
    imgui::imgui 
PRIVATE
    glfw 
    ${GLEW_LIBRARIES} 
    GLEW::GLEW 
    OpenGL::GL
    glm::glm
    spdlog::spdlog 
    spdlog::spdlog_header_only)

target_include_directories(${PROJECT_NAME}  
PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GLCore"
    ${STB_INCLUDE_DIRS}
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GLCore/Core"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GLCore/Events"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GLCore/ImGui"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/Windows")

#add_custom_command(TARGET ${PROJECT_NAME} 
#    COMMAND ${CMAKE_COMMAND}  ARGS -E copy_directory "${CMAKE_SOURCE_DIR}/assets/shaders/" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/shaders/")