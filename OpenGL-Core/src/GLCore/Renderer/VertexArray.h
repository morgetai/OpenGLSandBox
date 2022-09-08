#pragma once
#include "Renderer/VertexBuffer.h"
#include <functional>
#include <vector>

class OpenGLVertexArray
{
public:
    OpenGLVertexArray();
    virtual ~OpenGLVertexArray();

    void Bind() const;
    void Unbind() const;

    void AddVertexBuffer(const std::reference_wrapper<OpenGLVertexBuffer> vertexBuffer);
    void SetIndexBuffer(OpenGLIndexBuffer& indexBuffer);

    const std::vector<std::reference_wrapper<OpenGLVertexBuffer>> GetVertexBuffers() const { return m_VertexBuffers; }
    const auto& GetIndexBuffer() const { return m_IndexBuffer; }

private:
    uint32_t m_RendererID {};
    uint32_t m_VertexBufferIndex {};
    std::vector<std::reference_wrapper<OpenGLVertexBuffer>> m_VertexBuffers{};
    OpenGLIndexBuffer* m_IndexBuffer{};
};