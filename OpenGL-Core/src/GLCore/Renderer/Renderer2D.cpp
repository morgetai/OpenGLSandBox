#include "Renderer2D.h"
#include "Renderer/Buffer.h"
#include "Util/OpenGLDebug.h"
#include <glm/gtc/matrix_transform.hpp>
#include <numeric>

Renderer2D::Renderer2DData::Renderer2DData() : 
QuadVertexBuffer(Renderer2D::Renderer2DData::MaxVertices * sizeof(Renderer2D::QuadVertex)),
											   WhiteTexture(1, 1),
											   LineVertexBuffer(Renderer2D::Renderer2DData::MaxVertices * sizeof(Renderer2D::QuadVertex)),
											   CircleVertexBuffer(Renderer2D::Renderer2DData::MaxVertices * sizeof(Renderer2D::QuadVertex)),
											   QuadShader(GLCore::Utils::OpenGLShader("assets/shaders/Renderer2D_Quad.glsl")),
											   CircleShader(GLCore::Utils::OpenGLShader("assets/shaders/Renderer2D_Circle.glsl")),
											   LineShader(GLCore::Utils::OpenGLShader("assets/shaders/Renderer2D_Line.glsl")),
											   CameraUniformBuffer(sizeof(Renderer2D::CameraData), 0, "Camera")
{
	CameraUniformBuffer.Bind(QuadShader.GetRendererID());
	CameraUniformBuffer.Bind(CircleShader.GetRendererID());
	CameraUniformBuffer.Bind(LineShader.GetRendererID());


	uint32_t quadIndices[Renderer2D::Renderer2DData::MaxIndices];
	uint32_t offset{0};
	for (uint32_t i = 0; i < Renderer2D::Renderer2DData::MaxIndices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	std::array<int, 16> samplers{};
    std::iota(std::begin(samplers), std::end(samplers), 0);

	QuadShader.Bind();
	QuadShader.UploadUniformIntArray( "u_Textures", samplers.data(), samplers.size());
	QuadShader.Unbind();

	QuadIndexBuffer = std::make_unique<OpenGLIndexBuffer>(quadIndices, Renderer2D::Renderer2DData::MaxIndices);
	QuadVertexArray.SetIndexBuffer(*QuadIndexBuffer);
}

void Renderer2D::Init()
{

	m_renderer_data = std::make_unique<Renderer2DData>();

	//Quad
	m_renderer_data->QuadVertexBuffer.SetLayout({{ShaderDataType::Float3, "a_Position"},
												 {ShaderDataType::Float4, "a_Color"},
												 {ShaderDataType::Float2, "a_TexCoord"},
												 {ShaderDataType::Float, "a_TexIndex"},
												 {ShaderDataType::Float, "a_TilingFactor"}});
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

	//white texture
	uint32_t whiteTextureData = 0xffffffff;
	m_renderer_data->WhiteTexture.SetData(&whiteTextureData, sizeof(uint32_t));

	// Set first texture slot to 0
	m_renderer_data->TextureSlots[0] = &m_renderer_data->WhiteTexture;

	m_renderer_data->QuadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
	m_renderer_data->QuadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
	m_renderer_data->QuadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
	m_renderer_data->QuadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer2D::Shutdown()
{

	// delete[] m_renderer_data->QuadVertexBufferBase;
}

/* void Renderer2D::BeginScene(const OrthographicCamera &camera)
{

	m_renderer_data->CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
	m_renderer_data->CameraUniformBuffer->SetData(&m_renderer_data->CameraBuffer, sizeof(Renderer2DData::CameraData));

	StartBatch();
} */

void Renderer2D::BeginScene(const GLCore::Utils::OrthographicCamera &camera, const glm::mat4 &transform)
{

	m_renderer_data->CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix() * glm::inverse(transform);
	m_renderer_data->CameraUniformBuffer.SetData(&m_renderer_data->CameraBuffer, sizeof(Renderer2D::CameraData));

	StartBatch();
}

/* void Renderer2D::BeginScene(const EditorCamera &camera)
{

	m_renderer_data->CameraBuffer.ViewProjection = camera.GetViewProjection();
	m_renderer_data->CameraUniformBuffer->SetData(&m_renderer_data->CameraBuffer, sizeof(Renderer2DData::CameraData));

	StartBatch();
} */

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

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color)
{
	DrawQuad({position.x, position.y, 0.0f}, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	DrawQuad(transform, color);
}

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const OpenGLTexture2D &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	DrawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const OpenGLTexture2D &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	DrawQuad(transform, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawQuad(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
{

	constexpr size_t quadVertexCount = 4;
	const float textureIndex = 0.0f; // White Texture
	constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
	const float tilingFactor = 1.0f;

	if (m_renderer_data->QuadIndexCount >= Renderer2DData::MaxIndices)
		NextBatch();

	for (size_t i = 0; i < quadVertexCount; i++)
	{
		m_renderer_data->QuadVertexBufferPtr->Position = transform * m_renderer_data->QuadVertexPositions[i];
		m_renderer_data->QuadVertexBufferPtr->Color = color;
		m_renderer_data->QuadVertexBufferPtr->TexCoord = textureCoords[i];
		m_renderer_data->QuadVertexBufferPtr->TexIndex = textureIndex;
		m_renderer_data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
		//m_renderer_data->QuadVertexBufferPtr->EntityID = entityID;
		m_renderer_data->QuadVertexBufferPtr++;
	}

	m_renderer_data->QuadIndexCount += 6;

	m_renderer_data->Stats.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::mat4 &transform, const OpenGLTexture2D &texture, float tilingFactor, const glm::vec4 &tintColor, int entityID)
{
	constexpr size_t quadVertexCount = 4;
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

	for (size_t i = 0; i < quadVertexCount; i++)
	{
		m_renderer_data->QuadVertexBufferPtr->Position = transform * m_renderer_data->QuadVertexPositions[i];
		m_renderer_data->QuadVertexBufferPtr->Color = tintColor;
		m_renderer_data->QuadVertexBufferPtr->TexCoord = textureCoords[i];
		m_renderer_data->QuadVertexBufferPtr->TexIndex = textureIndex;
		m_renderer_data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
		//m_renderer_data->QuadVertexBufferPtr->EntityID = entityID;
		m_renderer_data->QuadVertexBufferPtr++;
	}

	m_renderer_data->QuadIndexCount += 6;

	m_renderer_data->Stats.QuadCount++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color)
{
	DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color)
{

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	DrawQuad(transform, color);
}

void Renderer2D::DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const OpenGLTexture2D &texture, float tilingFactor, const glm::vec4 &tintColor)
{
	DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const OpenGLTexture2D &texture, float tilingFactor, const glm::vec4 &tintColor)
{

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	DrawQuad(transform, texture, tilingFactor, tintColor);
}

void Renderer2D::DrawCircle(const glm::mat4 &transform, const glm::vec4 &color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/, int entityID /*= -1*/)
{

	// TODO: implement for circles
	// if (m_renderer_data->QuadIndexCount >= Renderer2DData::MaxIndices)
	// 	NextBatch();

	for (size_t i = 0; i < 4; i++)
	{
		m_renderer_data->CircleVertexBufferPtr->WorldPosition = transform * m_renderer_data->QuadVertexPositions[i];
		m_renderer_data->CircleVertexBufferPtr->LocalPosition = m_renderer_data->QuadVertexPositions[i] * 2.0f;
		m_renderer_data->CircleVertexBufferPtr->Color = color;
		m_renderer_data->CircleVertexBufferPtr->Thickness = thickness;
		m_renderer_data->CircleVertexBufferPtr->Fade = fade;
		//m_renderer_data->CircleVertexBufferPtr->EntityID = entityID;
		m_renderer_data->CircleVertexBufferPtr++;
	}

	m_renderer_data->CircleIndexCount += 6;

	m_renderer_data->Stats.QuadCount++;
}

void Renderer2D::DrawLine(const glm::vec3 &p0, glm::vec3 &p1, const glm::vec4 &color, int entityID)
{
	m_renderer_data->LineVertexBufferPtr->Position = p0;
	m_renderer_data->LineVertexBufferPtr->Color = color;
	//m_renderer_data->LineVertexBufferPtr->EntityID = entityID;
	m_renderer_data->LineVertexBufferPtr++;

	m_renderer_data->LineVertexBufferPtr->Position = p1;
	m_renderer_data->LineVertexBufferPtr->Color = color;
	//m_renderer_data->LineVertexBufferPtr->EntityID = entityID;
	m_renderer_data->LineVertexBufferPtr++;

	m_renderer_data->LineVertexCount += 2;
}

void Renderer2D::DrawRect(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID)
{
	glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
	glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
	glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

	DrawLine(p0, p1, color, entityID);
	DrawLine(p1, p2, color, entityID);
	DrawLine(p2, p3, color, entityID);
	DrawLine(p3, p0, color, entityID);
}

void Renderer2D::DrawRect(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
{
	glm::vec3 lineVertices[4];
	for (size_t i = 0; i < 4; i++)
		lineVertices[i] = transform * m_renderer_data->QuadVertexPositions[i];

	DrawLine(lineVertices[0], lineVertices[1], color, entityID);
	DrawLine(lineVertices[1], lineVertices[2], color, entityID);
	DrawLine(lineVertices[2], lineVertices[3], color, entityID);
	DrawLine(lineVertices[3], lineVertices[0], color, entityID);
}

/* void Renderer2D::DrawSprite(const glm::mat4 &transform, SpriteRendererComponent &src, int entityID)
{
	if (src.Texture)
		DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
	else
		DrawQuad(transform, src.Color, entityID);
} */

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