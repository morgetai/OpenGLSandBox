#include "Renderer/OpenGLTexture.h"
#include "Util/OpenGLDebug.h"
#include "Core/MyAssert.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
    : m_Width(width), m_Height(height)
{

    m_InternalFormat = GL_RGBA8;
    m_DataFormat = GL_RGBA;

    GLCall(glGenTextures(1, &m_RendererID));
    
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, width, height, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

OpenGLTexture2D::OpenGLTexture2D(const std::string &path)
    : m_Path(path)
{

    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc *data = nullptr;
    {
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    }

    if (data)
    {
        m_IsLoaded = true;

        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;

        GLCall(glGenTextures(1, &m_RendererID));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data));
        GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));

        stbi_image_free(data);
    }
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &m_RendererID);
}

void OpenGLTexture2D::SetData(void *data, uint32_t size)
{
    uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
	ASSERT_CUSTOM(size == m_Width * m_Height * bpp, "Data must be entire texture!");

    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}