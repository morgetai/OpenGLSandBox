#pragma once

#include <GLCore/Core/Layer.h>
#include <GLCore/Util/CameraController.h>
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

	CameraConroller m_CameraController;
	Renderer2D m_renderer;
	OpenGLTexture2D m_Pic1, m_Pic2;
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	
	GLuint m_QuadVA, m_QuadVB, m_QuadIB, m_Pic1_id, m_Pic2_id;

	glm::vec3 m_quad_pos{3.f, 5.f, 3.5f};

	glm::vec4 m_SquareBaseColor = { 0.8f, 0.2f, 0.3f, 1.0f };
	glm::vec4 m_SquareAlternateColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	glm::vec4 m_SquareColor = m_SquareBaseColor;
};