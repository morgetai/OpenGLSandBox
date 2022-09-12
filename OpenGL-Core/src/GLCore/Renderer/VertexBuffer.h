#pragma once
#include "Renderer/Buffer.h"

class OpenGLVertexBuffer
{
public:
    OpenGLVertexBuffer(uint32_t size);
    OpenGLVertexBuffer(float *vertices, uint32_t size);
    virtual ~OpenGLVertexBuffer();

    void Bind() const;
    void Unbind() const;

    void SetData(const void *data, uint32_t size);

    const BufferLayout &GetLayout() const { return m_Layout; }
    void SetLayout(const BufferLayout& layout) { m_Layout = layout;}

private:
    uint32_t m_RendererID;
    BufferLayout m_Layout{};
};

class OpenGLIndexBuffer
{
public:
    OpenGLIndexBuffer(uint32_t *indices, uint32_t count);
    virtual ~OpenGLIndexBuffer();

    void Bind() const;
    void Unbind() const;

    uint32_t GetCount() const { return m_Count; }

private:
    uint32_t m_RendererID;
    uint32_t m_Count;
};