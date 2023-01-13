#include "Renderer2D.h"
#include "Renderer/Buffer.h"
#include "Util/OpenGLDebug.h"
#include <glm/gtc/matrix_transform.hpp>
#include <numeric>

namespace
{
	constexpr uint32_t QUAD_INDICES = 6;
	constexpr uint32_t QUAD_VERTICES = 4;

	template <class... Ts>
	struct overloaded : Ts...
	{
		using Ts::operator()...;
	};
	template <class... Ts>
	overloaded(Ts...) -> overloaded<Ts...>;
}

Renderer2D::Renderer2DData::Renderer2DData() : QuadVertexBuffer(Renderer2D::Renderer2DData::MaxVertices * sizeof(Renderer2D::QuadVertex)),
											   WhiteTexture(1, 1),
											   LineVertexBuffer(Renderer2D::Renderer2DData::MaxVertices * sizeof(Renderer2D::QuadVertex)),
											   CircleVertexBuffer(Renderer2D::Renderer2DData::MaxVertices * sizeof(Renderer2D::QuadVertex)),
											   QuadShader(GLCore::Utils::OpenGLShader("assets/shaders/Renderer2D_Quad.glsl")),
											   CircleShader(GLCore::Utils::OpenGLShader("assets/shaders/Renderer2D_Circle.glsl")),
											   LineShader(GLCore::Utils::OpenGLShader("assets/shaders/Renderer2D_Line.glsl")),
											   LightShader(GLCore::Utils::OpenGLShader("assets/shaders/Renderer2D_Light.glsl")),
											   CameraUniformBuffer(sizeof(Renderer2D::CameraData), 0, "Camera"),
											   LightVertexBuffer(30 * sizeof(LightVertex)),
											   LightUniformBuffer(sizeof(LightData), 1, "Light")
{
	CameraUniformBuffer.Bind(QuadShader.GetRendererID());
	CameraUniformBuffer.Bind(CircleShader.GetRendererID());
	CameraUniformBuffer.Bind(LineShader.GetRendererID());
	LightUniformBuffer.Bind(QuadShader.GetRendererID());

	uint32_t quadIndices[Renderer2D::Renderer2DData::MaxIndices];
	uint32_t offset{0};
	for (uint32_t i = 0; i < Renderer2D::Renderer2DData::MaxIndices; i += QUAD_INDICES)
	{
		quadIndices[i] = offset;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset;

		offset += QUAD_VERTICES;
	}

	std::array<int, MaxTextureSlots> samplers{};
	std::iota(std::begin(samplers), std::end(samplers), 0);

	QuadShader.Bind();
	QuadShader.UploadUniformIntArray("u_Textures", samplers.data(), samplers.size());
	QuadShader.Unbind();

	QuadIndexBuffer = std::make_unique<OpenGLIndexBuffer>(quadIndices, Renderer2D::Renderer2DData::MaxIndices);
	QuadVertexArray.SetIndexBuffer(*QuadIndexBuffer);
}

void Renderer2D::Init()
{

	m_renderer_data = std::make_unique<Renderer2DData>();

	// Quad
	m_renderer_data->QuadVertexBuffer.SetLayout({{ShaderDataType::Float3, "a_Position"},
												 {ShaderDataType::Float4, "a_Color"},
												 {ShaderDataType::Float2, "a_TexCoord"},
												 {ShaderDataType::Float, "a_TexIndex"},
												 {ShaderDataType::Float, "a_TilingFactor"},
												 {ShaderDataType::Float3, "a_Normal"}});
	m_renderer_data->QuadVertexArray.AddVertexBuffer(m_renderer_data->QuadVertexBuffer);

	// Circles
	m_renderer_data->CircleVertexBuffer.SetLayout({{ShaderDataType::Float3, "a_WorldPosition"},
												   {ShaderDataType::Float3, "a_LocalPosition"},
												   {ShaderDataType::Float4, "a_Color"},
												   {ShaderDataType::Float, "a_Thickness"},
												   {ShaderDataType::Float, "a_Fade"}});
	m_renderer_data->CircleVertexArray.AddVertexBuffer(m_renderer_data->CircleVertexBuffer);
	m_renderer_data->CircleVertexArray.SetIndexBuffer(*m_renderer_data->QuadIndexBuffer); // Use quad IB

	// Lines
	m_renderer_data->LineVertexBuffer.SetLayout({{ShaderDataType::Float3, "a_Position"},
												 {ShaderDataType::Float4, "a_Color"}});
	m_renderer_data->LineVertexArray.AddVertexBuffer(m_renderer_data->LineVertexBuffer);

	// Light
	m_renderer_data->LightVertexBuffer.SetLayout({{ShaderDataType::Float3, "a_Position"},
												  {ShaderDataType::Float3, "a_Color"}});
	m_renderer_data->LightVertexArray.AddVertexBuffer(m_renderer_data->LightVertexBuffer);
	m_renderer_data->LightVertexArray.SetIndexBuffer(*m_renderer_data->QuadIndexBuffer);

	// white texture
	uint32_t whiteTextureData = 0xffffffff;
	m_renderer_data->WhiteTexture.SetData(&whiteTextureData, sizeof(uint32_t));

	// Set first texture slot to 0
	m_renderer_data->TextureSlots[0] = &m_renderer_data->WhiteTexture;

	m_renderer_data->QuadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
	m_renderer_data->QuadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
	m_renderer_data->QuadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
	m_renderer_data->QuadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

	m_renderer_data->QuadVertexNormals[0] = {0.0f, 0.0f, 1.0f};
	m_renderer_data->QuadVertexNormals[1] = {0.0f, 0.0f, 1.0f};
	m_renderer_data->QuadVertexNormals[2] = {0.0f, 0.0f, 1.0f};
	m_renderer_data->QuadVertexNormals[3] = {0.0f, 0.0f, 1.0f};

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer2D::DrawGeometry(const Geometry::GeometryObj &obj)
{
	std::visit(overloaded{[this](const Geometry::Rect &arg)
						  { this->DrawRect(arg.position, arg.size, arg.color); },
						  [this](const Geometry::Line &arg)
						  { this->DrawLine(arg.p0, arg.p1, arg.color); },
						  [this](const Geometry::Quad &arg)
						  { this->DrawRotatedQuad(arg.position, arg.size, arg.rotation, arg.color); },
						  [this](const Geometry::Cube &arg)
						  { this->DrawRotatedCube(arg.position, arg.size, arg.rotation, arg.color); },
						  [this](const Geometry::Circle &arg)
						  { this->DrawCircle(arg.position, arg.radius, arg.color); }},
			   obj);
}

void Renderer2D::DrawGeometry(const Geometry::GeometryObj &obj,
							  const OpenGLTexture2D &texture)
{
	std::visit(overloaded{[this](const Geometry::Rect &arg)
						  { ; },
						  [this](const Geometry::Line &arg)
						  { ; },
						  [this](const Geometry::Circle &arg)
						  { ; },
						  [this, texture](const Geometry::Quad &arg)
						  { this->DrawRotatedQuad(arg.position, arg.size, arg.rotation, texture); },
						  [this, texture](const Geometry::Cube &arg)
						  { this->DrawRotatedCube(arg.position, arg.size, arg.rotation, texture); }},
			   obj);
}

glm::vec3 Renderer2D::DrawLight(const glm::vec3 &light_pos, const glm::vec3 &light_color, const float ambient_str)
{
	glm::vec3 size{1.f, 1.f, 1.f};

	std::vector<glm::mat4> sides{glm::translate(glm::mat4(1.0f), light_pos) * glm::scale(glm::mat4(1.0f), size),
								 glm::translate(glm::mat4(1.0f), {light_pos.x, light_pos.y, light_pos.z - size.z}) * glm::scale(glm::mat4(1.0f), size),
								 glm::translate(glm::mat4(1.0f), {light_pos.x - size.x * 0.5, light_pos.y, light_pos.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {0.0f, 1.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size),
								 glm::translate(glm::mat4(1.0f), {light_pos.x + size.x * 0.5, light_pos.y, light_pos.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {0.0f, 1.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size),
								 glm::translate(glm::mat4(1.0f), {light_pos.x, light_pos.y + size.y * 0.5, light_pos.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size),
								 glm::translate(glm::mat4(1.0f), {light_pos.x, light_pos.y - size.y * 0.5, light_pos.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size)};

	glm::vec3 center{0.0, 0.0, 0.0};
	size_t vert = 0;
	for (size_t i = 0; i < 6; i++)
	{
		m_renderer_data->LightBuffer[vert].Position = sides[i] * m_renderer_data->QuadVertexPositions[0];
		m_renderer_data->LightBuffer[vert++].Color = light_color;
		m_renderer_data->LightBuffer[vert].Position = sides[i] * m_renderer_data->QuadVertexPositions[1];
		m_renderer_data->LightBuffer[vert++].Color = light_color;
		m_renderer_data->LightBuffer[vert].Position = sides[i] * m_renderer_data->QuadVertexPositions[2];
		m_renderer_data->LightBuffer[vert++].Color = light_color;
		m_renderer_data->LightBuffer[vert].Position = sides[i] * m_renderer_data->QuadVertexPositions[3];
		m_renderer_data->LightBuffer[vert++].Color = light_color;
	}

	for (size_t i = 0; i < 8; i++)
	{
		center.x += m_renderer_data->LightBuffer[i].Position.x;
		center.y += m_renderer_data->LightBuffer[i].Position.y;
		center.z += m_renderer_data->LightBuffer[i].Position.z;
	}

	center.x = center.x / 8.f;
	center.y = center.y / 8.f;
	center.z = center.z / 8.f;

	m_renderer_data->LightVertexBuffer.SetData(m_renderer_data->LightBuffer.data(), sizeof(LightVertex) * m_renderer_data->LightBuffer.size());

	m_renderer_data->LightShader.Bind();
	DrawIndexed(m_renderer_data->LightVertexArray, 6 * QUAD_INDICES);

	return center;
}

void Renderer2D::Shutdown()
{
}

void Renderer2D::BeginScene(const glm::vec3 &camera_pos, const glm::mat4 &viewproj, const glm::mat4 &transform,
							const glm::vec3 &light_pos, const glm::vec3 &light_color, const float ambient_str)
{
	m_renderer_data->CameraBuffer.ViewProjection = viewproj;
	m_renderer_data->CameraBuffer.Model = glm::inverse(transform);
	m_renderer_data->CameraBuffer.Position = camera_pos;
	m_renderer_data->CameraUniformBuffer.SetData(&m_renderer_data->CameraBuffer, sizeof(Renderer2D::CameraData));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto center = DrawLight(light_pos, light_color, ambient_str);
	m_renderer_data->Light.AmbientStr = ambient_str;
	m_renderer_data->Light.LightColor = light_color;
	m_renderer_data->Light.LightPos = center;
	m_renderer_data->Light.SpecularStrength = 0.5f;
	m_renderer_data->LightUniformBuffer.SetData(&m_renderer_data->Light, sizeof(LightData));

	StartBatch();
}

void Renderer2D::EndScene()
{
	Flush();
}

void Renderer2D::StartBatch()
{
	m_renderer_data->QuadIndexCount = 0;
	m_renderer_data->QuadVertexBufferPtr = m_renderer_data->QuadVertexBufferBase.begin();

	m_renderer_data->CircleIndexCount = 0;
	m_renderer_data->CircleVertexBufferPtr = m_renderer_data->CircleVertexBufferBase.begin();

	m_renderer_data->LineVertexCount = 0;
	m_renderer_data->LineVertexBufferPtr = m_renderer_data->LineVertexBufferBase.begin();

	m_renderer_data->TextureSlotIndex = 1;
}

void Renderer2D::Flush()
{
	if (m_renderer_data->QuadIndexCount)
	{
		uint32_t dataSize = sizeof(Renderer2D::QuadVertex) *
							static_cast<uint32_t>(std::distance(m_renderer_data->QuadVertexBufferBase.begin(), m_renderer_data->QuadVertexBufferPtr));
		m_renderer_data->QuadVertexBuffer.SetData(m_renderer_data->QuadVertexBufferBase.data(), dataSize);

		// Bind textures
		for (uint32_t i = 0; i < m_renderer_data->TextureSlotIndex; i++)
			m_renderer_data->TextureSlots[i]->Bind(i);

		m_renderer_data->QuadShader.Bind();
		DrawIndexed(m_renderer_data->QuadVertexArray, m_renderer_data->QuadIndexCount);
		m_renderer_data->Stats.DrawCalls++;
	}

	if (m_renderer_data->CircleIndexCount)
	{
		uint32_t dataSize = sizeof(Renderer2D::CircleVertex) *
							static_cast<uint32_t>(std::distance(m_renderer_data->CircleVertexBufferBase.begin(), m_renderer_data->CircleVertexBufferPtr));
		m_renderer_data->CircleVertexBuffer.SetData(m_renderer_data->CircleVertexBufferBase.data(), dataSize);

		m_renderer_data->CircleShader.Bind();
		DrawIndexed(m_renderer_data->CircleVertexArray, m_renderer_data->CircleIndexCount);
		m_renderer_data->Stats.DrawCalls++;
	}

	if (m_renderer_data->LineVertexCount)
	{
		uint32_t dataSize = sizeof(Renderer2D::LineVertex) *
							static_cast<uint32_t>(std::distance(m_renderer_data->LineVertexBufferBase.begin(), m_renderer_data->LineVertexBufferPtr));
		m_renderer_data->LineVertexBuffer.SetData(m_renderer_data->LineVertexBufferBase.data(), dataSize);

		m_renderer_data->LineShader.Bind();
		SetLineWidth(m_renderer_data->LineWidth);
		DrawLines(m_renderer_data->LineVertexArray, m_renderer_data->LineVertexCount);
		m_renderer_data->Stats.DrawCalls++;
	}
}

void Renderer2D::NextBatch()
{
	Flush();
	StartBatch();
}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	DrawQuad(transform, color);
}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const OpenGLTexture2D &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	DrawQuad(transform, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::mat4 &transform, const glm::vec4 &color)
{
	const float textureIndex = 0.0f; // White Texture
	constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
	const float tilingFactor = 1.0f;

	if (m_renderer_data->QuadIndexCount >= Renderer2DData::MaxIndices)
		NextBatch();

	for (size_t i = 0; i < QUAD_VERTICES; i++)
	{
		m_renderer_data->QuadVertexBufferPtr->Position = transform * m_renderer_data->QuadVertexPositions[i];
		m_renderer_data->QuadVertexBufferPtr->Color = color;
		m_renderer_data->QuadVertexBufferPtr->TexCoord = textureCoords[i];
		m_renderer_data->QuadVertexBufferPtr->TexIndex = textureIndex;
		m_renderer_data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
		m_renderer_data->QuadVertexBufferPtr->Normal = m_renderer_data->QuadVertexNormals[i];
		m_renderer_data->QuadVertexBufferPtr++;
	}

	m_renderer_data->QuadIndexCount += QUAD_INDICES;

	m_renderer_data->Stats.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::mat4 &transform, const OpenGLTexture2D &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

	if (m_renderer_data->QuadIndexCount >= Renderer2DData::MaxIndices)
		NextBatch();

	float textureIndex = 0.0f;
	for (uint32_t i = 1; i < m_renderer_data->TextureSlotIndex; i++)
	{
		if (m_renderer_data->TextureSlots[i] == &texture)
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f)
	{
		if (m_renderer_data->TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
			NextBatch();

		textureIndex = (float)m_renderer_data->TextureSlotIndex;
		m_renderer_data->TextureSlots[m_renderer_data->TextureSlotIndex] = &texture;
		m_renderer_data->TextureSlotIndex++;
	}

	for (size_t i = 0; i < QUAD_VERTICES; i++)
	{
		m_renderer_data->QuadVertexBufferPtr->Position = transform * m_renderer_data->QuadVertexPositions[i];
		m_renderer_data->QuadVertexBufferPtr->Color = tintColor;
		m_renderer_data->QuadVertexBufferPtr->TexCoord = textureCoords[i];
		m_renderer_data->QuadVertexBufferPtr->TexIndex = textureIndex;
		m_renderer_data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
		m_renderer_data->QuadVertexBufferPtr->Normal = m_renderer_data->QuadVertexNormals[i];
		m_renderer_data->QuadVertexBufferPtr++;
	}

	m_renderer_data->QuadIndexCount += QUAD_INDICES;

	m_renderer_data->Stats.QuadCount++;
}

void Renderer2D::DrawCube(const glm::vec3 &position, const glm::vec3 &size, const glm::vec4 &color)
{
	glm::mat4 front = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 back = glm::translate(glm::mat4(1.0f), {position.x, position.y, position.z - size.z}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 left = glm::translate(glm::mat4(1.0f), {position.x - size.x * 0.5, position.y, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {0.0f, 1.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 right = glm::translate(glm::mat4(1.0f), {position.x + size.x * 0.5, position.y, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {0.0f, 1.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 top = glm::translate(glm::mat4(1.0f), {position.x, position.y + size.y * 0.5, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 bottom = glm::translate(glm::mat4(1.0f), {position.x, position.y - size.y * 0.5, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);

	DrawQuad(front, color);
	DrawQuad(back, color);
	DrawQuad(left, color);
	DrawQuad(right, color);
	DrawQuad(top, color);
	DrawQuad(bottom, color);
}

void Renderer2D::DrawCube(const glm::vec3 &position, const glm::vec3 &size, const OpenGLTexture2D &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	glm::mat4 front = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 back = glm::translate(glm::mat4(1.0f), {position.x, position.y, position.z - size.z}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 left = glm::translate(glm::mat4(1.0f), {position.x - size.x * 0.5, position.y, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {0.0f, 1.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 right = glm::translate(glm::mat4(1.0f), {position.x + size.x * 0.5, position.y, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {0.0f, 1.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 top = glm::translate(glm::mat4(1.0f), {position.x, position.y + size.y * 0.5, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 bottom = glm::translate(glm::mat4(1.0f), {position.x, position.y - size.y * 0.5, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);

	DrawQuad(front, texture);
	DrawQuad(back, texture);
	DrawQuad(left, texture);
	DrawQuad(right, texture);
	DrawQuad(top, texture);
	DrawQuad(bottom, texture);
}

void Renderer2D::DrawRotatedCube(const glm::vec3 &position, const glm::vec3 &size, const glm::vec3 &rotation, const glm::vec4 &color)
{
	glm::mat4 all_axis_rot =
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) * // X axis
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) * // Y axis
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));	 // Z axis

	glm::mat4 front = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 back = glm::translate(glm::mat4(1.0f), {position.x, position.y, position.z - size.z}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 left = glm::translate(glm::mat4(1.0f), {position.x - size.x * 0.5, position.y, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {0.0f, 1.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 right = glm::translate(glm::mat4(1.0f), {position.x + size.x * 0.5, position.y, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {0.0f, 1.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 top = glm::translate(glm::mat4(1.0f), {position.x, position.y + size.y * 0.5, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 bottom = glm::translate(glm::mat4(1.0f), {position.x, position.y - size.y * 0.5, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);

	DrawQuad(all_axis_rot * front, color);
	DrawQuad(all_axis_rot * back, color);
	DrawQuad(all_axis_rot * left, color);
	DrawQuad(all_axis_rot * right, color);
	DrawQuad(all_axis_rot * top, color);
	DrawQuad(all_axis_rot * bottom, color);
}

void Renderer2D::DrawRotatedCube(const glm::vec3 &position, const glm::vec3 &size, const glm::vec3 &rotation, const OpenGLTexture2D &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	glm::mat4 all_axis_rot =
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) * // X axis
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) * // Y axis
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));	 // Z axis

	glm::mat4 front = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 back = glm::translate(glm::mat4(1.0f), {position.x, position.y, position.z - size.z}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 left = glm::translate(glm::mat4(1.0f), {position.x - size.x * 0.5, position.y, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {0.0f, 1.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 right = glm::translate(glm::mat4(1.0f), {position.x + size.x * 0.5, position.y, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {0.0f, 1.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 top = glm::translate(glm::mat4(1.0f), {position.x, position.y + size.y * 0.5, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);
	glm::mat4 bottom = glm::translate(glm::mat4(1.0f), {position.x, position.y - size.y * 0.5, position.z - size.z * 0.5}) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), {1.0f, 0.0f, 0.0f}) * glm::scale(glm::mat4(1.0f), size);

	DrawQuad(all_axis_rot * front, texture, tilingFactor, tintColor);
	DrawQuad(all_axis_rot * back, texture, tilingFactor, tintColor);
	DrawQuad(all_axis_rot * left, texture, tilingFactor, tintColor);
	DrawQuad(all_axis_rot * right, texture, tilingFactor, tintColor);
	DrawQuad(all_axis_rot * top, texture, tilingFactor, tintColor);
	DrawQuad(all_axis_rot * bottom, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color)
{

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	DrawQuad(transform, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const OpenGLTexture2D &texture, float tilingFactor, const glm::vec4 &tintColor)
{

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	DrawQuad(transform, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawCircle(const glm::vec3 &position,const float radius, const glm::vec4 &color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/)
{

	// TODO: implement for circles
	// if (m_renderer_data->QuadIndexCount >= Renderer2DData::MaxIndices)
	// 	NextBatch();

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {radius, radius, 1.0f});

	for (size_t i = 0; i < QUAD_VERTICES; i++)
	{
		m_renderer_data->CircleVertexBufferPtr->WorldPosition = transform * m_renderer_data->QuadVertexPositions[i];
		m_renderer_data->CircleVertexBufferPtr->LocalPosition = m_renderer_data->QuadVertexPositions[i] *2.f;
		m_renderer_data->CircleVertexBufferPtr->Color = color;
		m_renderer_data->CircleVertexBufferPtr->Thickness = thickness;
		m_renderer_data->CircleVertexBufferPtr->Fade = fade;
		m_renderer_data->CircleVertexBufferPtr++;
	}

	m_renderer_data->CircleIndexCount += QUAD_INDICES;

	m_renderer_data->Stats.QuadCount++;
}

void Renderer2D::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
{
	m_renderer_data->LineVertexBufferPtr->Position = p0;
	m_renderer_data->LineVertexBufferPtr->Color = color;
	m_renderer_data->LineVertexBufferPtr++;

	m_renderer_data->LineVertexBufferPtr->Position = p1;
	m_renderer_data->LineVertexBufferPtr->Color = color;
	m_renderer_data->LineVertexBufferPtr++;

	m_renderer_data->LineVertexCount += 2;
}

void Renderer2D::DrawRect(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
{
	glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
	glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

	DrawLine(p0, p1, color);
	DrawLine(p1, p2, color);
	DrawLine(p2, p3, color);
	DrawLine(p3, p0, color);
}

void Renderer2D::DrawRect(const glm::mat4 &transform, const glm::vec4 &color)
{
	glm::vec3 lineVertices[4];
	for (size_t i = 0; i < 4; i++)
		lineVertices[i] = transform * m_renderer_data->QuadVertexPositions[i];

	DrawLine(lineVertices[0], lineVertices[1], color);
	DrawLine(lineVertices[1], lineVertices[2], color);
	DrawLine(lineVertices[2], lineVertices[3], color);
	DrawLine(lineVertices[3], lineVertices[0], color);
}

float Renderer2D::GetLineWidth()
{
	return m_renderer_data->LineWidth;
}

void Renderer2D::SetLineWidth(float width)
{
	m_renderer_data->LineWidth = width;
}

void Renderer2D::ResetStats()
{
	memset(&m_renderer_data->Stats, 0, sizeof(Statistics));
}

Renderer2D::Statistics Renderer2D::GetStats()
{
	return m_renderer_data->Stats;
}

void Renderer2D::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void Renderer2D::SetClearColor(const glm::vec4 &color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}
void Renderer2D::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer2D::DrawIndexed(const OpenGLVertexArray &vertexArray, uint32_t indexCount)
{
	vertexArray.Bind();
	uint32_t count = indexCount ? indexCount : vertexArray.GetIndexBuffer()->GetCount();
	GLCall(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
}
void Renderer2D::DrawLines(const OpenGLVertexArray &vertexArray, uint32_t vertexCount)
{
	vertexArray.Bind();
	glDrawArrays(GL_LINES, 0, vertexCount);
}

void SetLineWidth(float width)
{
	glLineWidth(width);
}