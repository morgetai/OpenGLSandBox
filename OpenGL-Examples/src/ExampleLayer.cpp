#include "ExampleLayer.h"
#include <Util/OpenGLDebug.h>
//#include <GLCore/Renderer/OpenGLTexture.h>

#include <array>

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

using namespace GLCore;
using namespace GLCore::Utils;

ExampleLayer::ExampleLayer()
	//: m_CameraController(16.0f/9.0f),
	: m_CameraController(1280.f, 720.f),
	  m_Pic1("assets/textures/pic1.jpg"),
	  m_Pic2("assets/textures/pic2.png")
{
}

ExampleLayer::~ExampleLayer()
{
}

void ExampleLayer::OnAttach()
{
	m_renderer.Init();
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnEvent(Event &event)
{
	m_CameraController.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<MouseButtonPressedEvent>(
		[&](MouseButtonPressedEvent &e)
		{
			m_SquareColor = m_SquareAlternateColor;
			return false;
		});
	dispatcher.Dispatch<MouseButtonReleasedEvent>(
		[&](MouseButtonReleasedEvent &e)
		{
			m_SquareColor = m_SquareBaseColor;
			return false;
		});
}

void ExampleLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	static float rotate = 0.5f;
	static float rotate_step = 0.5f;

	if (rotate > 90.f)
	{
		rotate_step =  -0.5f;
	}
	else if(rotate < -90.f)
	{
		rotate_step = +0.5f;
	}
	m_renderer.BeginScene(m_CameraController.GetViewProjMatrix(),
						  glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

	m_renderer.DrawRotatedCube({0.5f,0.5f,-0.5f},{1.0f,1.0f,1.0f},{rotate, rotate, 0.0}, m_Pic1);

	m_renderer.DrawRotatedCube({0.5f,0.5f,-3.5f},{1.0f,1.0f,1.0f},{30.0, 25.0, 0.0}, m_Pic1);

   	for (float y = -10.0f; y < 10.0f; y += 0.25f)
	{
		for (float x = -10.0f; x < 10.0f; x += 0.25f)
		{
			glm::vec4 color{(x + 10) / 20.0f, 0.2f, (y + 10) / 20.0f, 1.0f};
			m_renderer.DrawCube({x, y , 0.f}, {0.2f, 0.2f, 0.2f}, color);
		}
	}

	rotate += rotate_step;

	m_renderer.EndScene();
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Controls");
	// if (ImGui::ColorEdit4("Square Base Color", glm::value_ptr(m_SquareBaseColor)))
	//	m_SquareColor = m_SquareBaseColor;
	// ImGui::ColorEdit4("Square Alternate Color", glm::value_ptr(m_SquareAlternateColor));

	ImGui::DragFloat2("Quad pos", &m_quad_pos.x, 0.1f);
	ImGui::Text("Vertex %d draw calls %d ", m_renderer.GetStats().GetTotalVertexCount(), m_renderer.GetStats().DrawCalls);
	ImGui::End();

	m_renderer.ResetStats();
}
