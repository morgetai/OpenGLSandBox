#pragma once

#include "Util/OrthographicCamera.h"
#include "Renderer/OpenGLTexture.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/UniformBuffer.h"
#include <glm/glm.hpp>
#include <array>
#include <memory>

struct SceneData
{
    glm::mat4 ViewProjectionMatrix;
};

class Renderer2D
{
public:
    void Init();
    void Shutdown();

    void BeginScene(const GLCore::Utils::OrthographicCamera &camera, const glm::mat4 &transform);
    // void BeginScene(const EditorCamera &camera);
    //void BeginScene(const OrthographicCamera &camera); // TODO: Remove
    void EndScene();
    void Flush();

    // Primitives
    void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
    void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color);
    void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const OpenGLTexture2D &texture, float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
    void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const OpenGLTexture2D &texture, float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

    void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color, int entityID = -1);
    void DrawQuad(const glm::mat4 &transform, const OpenGLTexture2D &texture, float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);

    void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color);
    void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color);
    void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation, const OpenGLTexture2D &texture, float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
    void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation, const OpenGLTexture2D &texture, float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

    void DrawCircle(const glm::mat4 &transform, const glm::vec4 &color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

    void DrawLine(const glm::vec3 &p0, glm::vec3 &p1, const glm::vec4 &color, int entityID = -1);

    void DrawRect(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID = -1);
    void DrawRect(const glm::mat4 &transform, const glm::vec4 &color, int entityID = -1);

    float GetLineWidth();
    void SetLineWidth(float width);

    // basic
    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    void SetClearColor(const glm::vec4 &color);
    void Clear();

    void DrawIndexed(const OpenGLVertexArray &vertexArray, uint32_t indexCount = 0);
    void DrawLines(const OpenGLVertexArray &vertexArray, uint32_t vertexCount);

    //void SetLineWidth(float width);

    // Stats
    struct Statistics
    {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;

        uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
        uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
    };
    void ResetStats();
    Statistics GetStats();

private:
    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        float TilingFactor;

        // Editor-only
        int EntityID;
    };

    struct CircleVertex
    {
        glm::vec3 WorldPosition;
        glm::vec3 LocalPosition;
        glm::vec4 Color;
        float Thickness;
        float Fade;

        // Editor-only
        int EntityID;
    };

    struct LineVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;

        // Editor-only
        int EntityID;
    };

    struct CameraData
    {
        glm::mat4 ViewProjection;
    };

    struct Renderer2DData
    {
        static constexpr uint32_t MaxQuads = 20000;
        static constexpr uint32_t MaxVertices = MaxQuads * 4;
        static constexpr uint32_t MaxIndices = MaxQuads * 6;
        static constexpr uint32_t MaxTextureSlots = 16; // TODO: RenderCaps

        template <typename T, uint32_t size = Renderer2DData::MaxQuads>
        using DataArray = std::array<T, size>;

        Renderer2DData();
        //
        OpenGLVertexArray QuadVertexArray;
        OpenGLVertexBuffer QuadVertexBuffer;
        GLCore::Utils::OpenGLShader QuadShader;
        std::unique_ptr<OpenGLIndexBuffer> QuadIndexBuffer;
        OpenGLTexture2D WhiteTexture;
        //
        OpenGLVertexArray CircleVertexArray;
        OpenGLVertexBuffer CircleVertexBuffer;
        GLCore::Utils::OpenGLShader CircleShader;
        //
        OpenGLVertexArray LineVertexArray;
        OpenGLVertexBuffer LineVertexBuffer;
        GLCore::Utils::OpenGLShader LineShader;
        //
        uint32_t QuadIndexCount = 0;
        DataArray<QuadVertex> QuadVertexBufferBase{};
        DataArray<QuadVertex>::iterator QuadVertexBufferPtr{};
        //
        uint32_t CircleIndexCount = 0;
        DataArray<CircleVertex> CircleVertexBufferBase{};
        DataArray<CircleVertex>::iterator CircleVertexBufferPtr{};
        //
        uint32_t LineVertexCount = 0;
        DataArray<LineVertex> LineVertexBufferBase{};
        DataArray<LineVertex>::iterator LineVertexBufferPtr{};
        float LineWidth = 2.0f;
        //
        DataArray<const OpenGLTexture2D*, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture
        //
        glm::vec4 QuadVertexPositions[4];
        //
        Renderer2D::Statistics Stats;
        CameraData CameraBuffer;
        OpenGLUniformBuffer CameraUniformBuffer;
    };

    std::unique_ptr<Renderer2DData> m_renderer_data;

    void StartBatch();
    void NextBatch();
};