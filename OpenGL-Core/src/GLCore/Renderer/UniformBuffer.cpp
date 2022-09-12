#include "Renderer/UniformBuffer.h"
#include "Util/OpenGLDebug.h"
#include <GL/glew.h>
#include <string>


GLUniformBlock::~GLUniformBlock()
{
    glDeleteBuffers(1, &m_RendererID);
}

GLUniformBlock::GLUniformBlock(uint32_t size, uint32_t binding, std::string_view name):
m_binding(binding),
m_name(name)
{
    //gen buffer
    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
    GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
};

void GLUniformBlock::Bind(uint32_t program)
{
    //get blocks index
    const GLchar *sourceCStr = m_name.c_str();
    GLCall(const auto ind = glGetUniformBlockIndex(program, sourceCStr));
    GLCall(glUniformBlockBinding(program, ind, m_binding));
    GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, m_binding, m_RendererID));
}

void GLUniformBlock::SetData(const void *data, uint32_t size, uint32_t offset)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    GLCall(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
