#include "ExampleLayer.h"
#include <Util/OpenGLDebug.h>
//#include <GLCore/Renderer/OpenGLTexture.h>

#include <array>

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace GLCore;
using namespace GLCore::Utils;

namespace
{
/* 	GLuint LoadTexture(const std::string &path)
	{
		int w, h, bits;

		stbi_set_flip_vertically_on_load(1);
		auto data = stbi_load(path.c_str(), &w, &h, &bits, 4);

		GLuint tex_id;
		GLCall(glGenTextures(1, &tex_id));
		GLCall(glBindTexture(GL_TEXTURE_2D, tex_id));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));

		GLCall(glBindTexture(GL_TEXTURE_2D, 0));

		if (data)
			stbi_image_free(data);

		return tex_id;
	} */
}

ExampleLayer::ExampleLayer()
	: m_CameraController(16.0f / 9.0f),
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_renderer.BeginScene(m_CameraController.GetCamera(),glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

	m_renderer.DrawQuad(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), m_Pic1, 0.5);
	//m_renderer.DrawQuad(m_quad_pos,{0.25,0.25},{0.3,0.2,0.5,0.75});
	//m_renderer.DrawCircle(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)),{1.0,0.5,0.5,0.75}, 1.0f, 1.0f );

	m_renderer.EndScene();
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Controls");
	// if (ImGui::ColorEdit4("Square Base Color", glm::value_ptr(m_SquareBaseColor)))
	//	m_SquareColor = m_SquareBaseColor;
	// ImGui::ColorEdit4("Square Alternate Color", glm::value_ptr(m_SquareAlternateColor));

	ImGui::DragFloat2("Quad pos", &m_quad_pos.x, 0.1f);
	ImGui::End();
}
