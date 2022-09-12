#pragma once
#include <cstdint>
#include <string>
#include <string_view>

class GLUniformBlock
{
public:
    GLUniformBlock(uint32_t size, uint32_t binding, std::string_view name);
    void SetData(const void *data, uint32_t size, uint32_t offset = 0);
    void Bind(uint32_t program);
    virtual ~GLUniformBlock();
private:
    uint32_t m_RendererID{};
    uint32_t m_binding{};
    const std::string m_name;
};
