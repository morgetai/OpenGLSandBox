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
	// EnableGLDebugging();

	//m_Shader = Shader::FromGLSLTextFiles(
	//	"assets/shaders/test.vert.glsl",
	//	"assets/shaders/test.frag.glsl");

/* 	glUseProgram(m_Shader->GetRendererID());

	auto loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Textures");
	int samplers[16];
	for (int i = 0; i < 16; i++)
	{
		samplers[i] = i;
	}

	glUniform1iv(loc, 16, samplers); */

	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	constexpr auto max_quad_count = 1000;
	constexpr auto max_vert_count = 4 * max_quad_count;
	constexpr auto max_ind_count = max_quad_count * 6;

	/* 	std::array<float, 80> vertices =
			{						//color			 //texture   //texture id
				-1.5, -0.5, 0.0f, 0.18,0.6,0.96,1.0,  0.0, 0.0,  0.0,
				-0.5, -0.5, 0.0f, 0.18,0.6,0.96,1.0,  1.0, 0.0,  0.0,
				-0.5, 0.5, 0.0f,  0.18,0.6,0.96,1.0,  1.0, 1.0,  0.0,
				-1.5, 0.5, 0.0f,  0.18,0.6,0.96,1.0,  0.0, 1.0,  0.0,
				//second
				0.5, -0.5, 0.0f,  1.0,0.6,0.96,1.0,   0.0, 0.0,  1.0,
				1.5, -0.5, 0.0f,  1.0,0.6,0.96,1.0,   1.0, 0.0,  1.0,
				1.5, 0.5, 0.0f,   1.0,0.6,0.96,1.0,   1.0, 1.0,  1.0,
				0.5, 0.5, 0.0f,   1.0,0.6,0.96,1.0,   0.0, 1.0,  1.0,}; */

	/* 	GLCall(glGenBuffers(1, &m_QuadVB));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB));
		GLCall(glBufferData(GL_ARRAY_BUFFER, max_vert_count * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));

		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex,pos)));
		GLCall(glEnableVertexAttribArray(0));

		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),(const void*) offsetof(Vertex,color)));//color offset is 3 * sizeof(floats)
		GLCall(glEnableVertexAttribArray(1));

		GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),(const void*) offsetof(Vertex,tex_coord)));//color offset is 3 * sizeof(floats)
		GLCall(glEnableVertexAttribArray(2));

		GLCall(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),(const void*) offsetof(Vertex,tex_id)));//color offset is 3 * sizeof(floats)
		GLCall(glEnableVertexAttribArray(3)); */

	/* 	std::array<uint32_t, 12> indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4}; */

	// uint32_t indices[max_ind_count];
	//  std::array<uint32_t, max_ind_count> indices;
	//  uint32_t offset{0};
	//  for(auto i = 0; i < max_ind_count; i+=6)
	//  {
	//  	indices[i + 0] = 0 + offset;
	//  	indices[i + 1] = 1 + offset;
	//  	indices[i + 2] = 2 + offset;

	// 	indices[i + 3] = 2 + offset;
	// 	indices[i + 4] = 3 + offset;
	// 	indices[i + 5] = 0 + offset;

	// 	offset += 4;
	// }

	// glGenBuffers(1, &m_QuadIB);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(typename std::array<uint32_t, 12>::value_type), indices.data(), GL_STATIC_DRAW);

	//m_Pic1_id = LoadTexture("assets/pic1.jpg");
	//m_Pic2_id = LoadTexture("assets/pic2.png");

	m_renderer.Init();
}

void ExampleLayer::OnDetach()
{
	// glDeleteVertexArrays(1, &m_QuadVA);
	// glDeleteBuffers(1, &m_QuadVB);
	// glDeleteBuffers(1, &m_QuadIB);
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

	m_renderer.DrawQuad({-0.5,-0.5,1.0},{0.25,0.25},{0.3,0.2,0.5,0.75});

	m_renderer.EndScene();

/* 	glUseProgram(m_Shader->GetRendererID());

	m_renderer.BeginBatch();

	for (int y = 0; y < 5; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			GLuint tex = (x + y) % 2 == 0 ? m_Pic1_id : m_Pic2_id;
			m_renderer.DrawQuad({x, y}, {1.0f, 1.0f}, tex);
		}
	}

	m_renderer.DrawQuad({m_quad_pos[0], m_quad_pos[1]}, {1.0f, 1.0f}, m_Pic1_id);

	for (float y = -10.0f; y < 10.0f; y += 0.25f)
	{
		for (float x = -10.0f; x < 10.0f; x += 0.25f)
		{
			glm::vec4 color{(x + 10) / 20.0f, 0.2f, (y + 10) / 20.0f, 1.0f};
			m_renderer.DrawQuad({x, y}, {0.2f, 0.2f}, color);
		}
	}

	m_renderer.EndBatch();

	glUniformMatrix4fv(glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection"),
					   1, GL_FALSE, glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(m_Shader->GetRendererID(), "u_Transform"),
					   1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))));

	m_renderer.Flush(); */
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Controls");
	// if (ImGui::ColorEdit4("Square Base Color", glm::value_ptr(m_SquareBaseColor)))
	//	m_SquareColor = m_SquareBaseColor;
	// ImGui::ColorEdit4("Square Alternate Color", glm::value_ptr(m_SquareAlternateColor));

	ImGui::DragFloat2("Quad pos", m_quad_pos, 0.1f);
	ImGui::End();
}
