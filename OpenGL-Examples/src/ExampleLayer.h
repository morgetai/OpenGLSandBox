#pragma once

//#include <GLCore.h>
//include <GLCoreUtils.h>
#include <GLCore/Core/Layer.h>
#include <GLCore/Util/OrthographicCameraController.h>
#include <GLCore/Renderer/Renderer2D.h>
#include <GL/glew.h>

class ExampleLayer : public GLCore::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:
	//GLCore::Utils::Shader* m_Shader;
	//GLCore::Utils::OrthographicCameraController<GLCore::Utils::OrthographicCamera> m_CameraController;
	GLCore::Utils::OrthographicCameraController<GLCore::Utils::ProjectionCamera> m_CameraController;
	Renderer2D m_renderer;
	OpenGLTexture2D m_Pic1, m_Pic2;
	
	GLuint m_QuadVA, m_QuadVB, m_QuadIB, m_Pic1_id, m_Pic2_id;
	//float m_quad_pos[2] {-1.5, -0.5};
	glm::vec2 m_quad_pos{-0.5, -0.5};

	glm::vec4 m_SquareBaseColor = { 0.8f, 0.2f, 0.3f, 1.0f };
	glm::vec4 m_SquareAlternateColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	glm::vec4 m_SquareColor = m_SquareBaseColor;
};