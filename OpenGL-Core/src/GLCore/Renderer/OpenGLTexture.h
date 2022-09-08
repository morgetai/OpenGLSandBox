#pragma once

#include <GL/glew.h>
#include <string>

class OpenGLTexture2D
{
public:
    OpenGLTexture2D(uint32_t width, uint32_t height);
    OpenGLTexture2D(const std::string &path);
    virtual ~OpenGLTexture2D();

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
    uint32_t GetRendererID() const { return m_RendererID; }

    const std::string &GetPath() const { return m_Path; }

    void SetData(void *data, uint32_t size);

    void Bind(uint32_t slot = 0) const;

    bool IsLoaded() const { return m_IsLoaded; }

    bool operator==(const OpenGLTexture2D &other) const
    {
        return m_RendererID == other.GetRendererID();
    }

private:
    std::string m_Path;
    bool m_IsLoaded = false;
    uint32_t m_Width, m_Height;
    uint32_t m_RendererID;
    GLenum m_InternalFormat, m_DataFormat;
};