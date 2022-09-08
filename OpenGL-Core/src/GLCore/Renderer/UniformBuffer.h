#pragma once
#include <cstdint>
class OpenGLUniformBuffer
{
public:
    OpenGLUniformBuffer(uint32_t size, uint32_t binding);
    virtual ~OpenGLUniformBuffer();

    void SetData(const void *data, uint32_t size, uint32_t offset = 0);

private:
    uint32_t m_RendererID = 0;
};