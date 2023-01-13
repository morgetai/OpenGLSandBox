#include "ExampleLayer.h"
#include <Util/OpenGLDebug.h>
#include <GLCore/Events/KeyEvent.h>
#include <GLCore/Core/KeyCodes.h>
#include <Renderer/Geometry.h>

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <array>

using namespace GLCore;
using namespace GLCore::Utils;

ExampleLayer::ExampleLayer()
	: m_CameraController(GLCore::Utils::OrthographicCamera(16.f / 9.f),
						 EditorCamera(30.0f, 1.778f, 0.1f, 100.0f)),
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
	// m_CameraController.OnEvent(event);

	EventDispatcher dispatcher(event);
	/*dispatcher.Dispatch<MouseButtonPressedEvent>(
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
		}); */

	dispatcher.Dispatch<GLCore::KeyPressedEvent>(
		[&](GLCore::KeyPressedEvent &e)
		{
			auto key = e.GetKeyCode();
			if (!e.Handled && key == Key::R)
			{
				m_CameraController.SwitchCamera();
			}
			return true;
		});

	m_CameraController.OnEvent(event);
}

void ExampleLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	static float rotate = 0.5f;
	static float rotate_step = 0.5f;

	if (rotate > 90.f)
	{
		rotate_step = -0.5f;
	}
	else if (rotate < -90.f)
	{
		rotate_step = +0.5f;
	}
	m_renderer.BeginScene(m_CameraController.GetPosition(),
						  m_CameraController.GetViewProjMatrix(),
						  glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
						  m_quad_pos,
						  {1.0f, 1.0f, 1.0f},
						  0.1f);

	/* 	m_renderer.DrawRotatedCube({0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {rotate, rotate, 0.0}, m_Pic1);

		m_renderer.DrawRotatedCube({0.5f, 0.5f, -3.5f}, {1.0f, 1.0f, 1.0f}, {30.0, 25.0, 0.0}, m_Pic1);

		glm::vec4 lightColor(1.f, 1.f, 1.f, 1.0f);

		float ambientStrength = 0.1;
		auto ambient = ambientStrength * lightColor;
		ambient.z = 1.0f;


		m_renderer.DrawCube({3.f, 5.f, 3.5f}, {1.0f, 1.0f, 1.0f}, lightColor);*/

	// for (float y = -5.0f; y < 5.0f; y += 0.25f)
	// {
	// 	for (float x = -5.0f; x < 5.0f; x += 0.25f)
	// 	{
	// 		glm::vec4 color{1.0f, 0.5f, 0.31f, 1.0};
	// 		Geometry::Cube cube {{x, y, 0.f}, {0.2f, 0.2f, 0.2f}, color};
	// 		m_renderer.DrawGeometry(cube);
	// 	}
	// }

	Geometry::Line line {{0.5, 0.5, 0.f}, {0.8f, 0.7f, 0.f}, {0.23f, 0.45, 0.11,1.f}};

	m_renderer.DrawGeometry(line);

	Geometry::Circle circle{{0.5, 0.5, 0.f}, 2, {0.23f, 0.45, 0.11,1.f}};

	m_renderer.DrawGeometry(circle);

	rotate += rotate_step;

	m_renderer.EndScene();
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Controls");

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
	// if (ImGui::ColorEdit4("Square Base Color", glm::value_ptr(m_SquareBaseColor)))
	//	m_SquareColor = m_SquareBaseColor;
	// ImGui::ColorEdit4("Square Alternate Color", glm::value_ptr(m_SquareAlternateColor));

	ImGui::DragFloat2("Light x", &m_quad_pos.x, 0.1f);
	ImGui::DragFloat2("Light y", &m_quad_pos.y, 0.1f);

	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", m_renderer.GetStats().DrawCalls);
	ImGui::Text("Vertices: %d", m_renderer.GetStats().GetTotalVertexCount());
	ImGui::End();

	m_renderer.ResetStats();
}
