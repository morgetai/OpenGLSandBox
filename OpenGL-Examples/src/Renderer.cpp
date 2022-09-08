#include "Renderer.h"
#include <GLCore.h>
#include <GLCoreUtils.h>
#include <Util/OpenGLDebug.h>
using namespace GLCore;
using namespace GLCore::Utils;

namespace
{
    constexpr size_t MAX_QUAD_COUNT = 1000;
    constexpr size_t MAX_VERT_COUNT = MAX_QUAD_COUNT * 4;
    constexpr size_t MAX_IND_COUNT = MAX_QUAD_COUNT * 6;
    constexpr size_t MAX_TEXTURES = 16;
}

void Renderer::Init()
{
    GLCall(glGenVertexArrays(1, &m_data.quad_va));
    GLCall(glBindVertexArray(m_data.quad_va));

    GLCall(glGenBuffers(1, &m_data.quad_vb));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_data.quad_vb));
    GLCall(glBufferData(GL_ARRAY_BUFFER, MAX_VERT_COUNT * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW));

    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, pos)));
    GLCall(glEnableVertexAttribArray(0));

    GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color))); // color offset is 3 * sizeof(floats)
    GLCall(glEnableVertexAttribArray(1));

    GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, tex_coord))); // color offset is 3 * sizeof(floats)
    GLCall(glEnableVertexAttribArray(2));

    GLCall(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, tex_id))); // color offset is 3 * sizeof(floats)
    GLCall(glEnableVertexAttribArray(3));

    uint32_t indices[MAX_IND_COUNT];
    uint32_t offset{0};

    for (auto i = 0; i < MAX_IND_COUNT; i += 6)
    {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;

        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;

        offset += 4;
    }

    glGenBuffers(1, &m_data.quad_ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data.quad_ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 1x1 white texture
    GLCall(glGenTextures(1, &m_data.white_texture));
    
    GLCall(glBindTexture(GL_TEXTURE_2D, m_data.white_texture));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    uint32_t color = 0xffffffff;
    GLCall(glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color));

    
    m_data.texture_slots[0] = m_data.white_texture;
}

void Renderer::BeginBatch()
{
    m_data.QuadBufferPtr = m_data.QuadBuffer.begin();
}

void Renderer::EndBatch()
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_data.quad_vb));
    auto size = std::distance(m_data.QuadBuffer.begin(), m_data.QuadBufferPtr);

    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(Vertex), m_data.QuadBuffer.data()));
}

void Renderer::Flush()
{
    for (uint32_t i = 0; i < m_data.texture_slot_ind; i++)
    {
        GLCall(glActiveTexture(GL_TEXTURE0 + i));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_data.texture_slots[i]));
    }

    glBindVertexArray(m_data.quad_va);
    glDrawElements(GL_TRIANGLES, m_data.index_cout, GL_UNSIGNED_INT, nullptr);
    m_data.index_cout = 0;
    m_data.texture_slot_ind = 1;
    std::fill(m_data.texture_slots.begin() + 1, m_data.texture_slots.end(), 0);
}

void Renderer::DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color)
{
    if (m_data.index_cout >= MAX_IND_COUNT)
    {
        EndBatch();
        Flush();
        BeginBatch();
    }

assert(m_data.QuadBuffer.end() != m_data.QuadBufferPtr);
    float tex_index = 0.0f;

    m_data.QuadBufferPtr->pos = {pos.x, pos.y, 0.0f};
    m_data.QuadBufferPtr->color = color;
    m_data.QuadBufferPtr->tex_coord = {0.0, 0.0};
    m_data.QuadBufferPtr->tex_id = {tex_index};
    m_data.QuadBufferPtr++;

assert(m_data.QuadBuffer.end() != m_data.QuadBufferPtr);
    m_data.QuadBufferPtr->pos = {pos.x + size.x, pos.y, 0.0f};
    m_data.QuadBufferPtr->color = color;
    m_data.QuadBufferPtr->tex_coord = {1.0, 0.0};
    m_data.QuadBufferPtr->tex_id = {tex_index};
    m_data.QuadBufferPtr++;
    assert(m_data.QuadBuffer.end() != m_data.QuadBufferPtr);

    m_data.QuadBufferPtr->pos = {pos.x + size.x, pos.y + size.y, 0.0f};
    m_data.QuadBufferPtr->color = color;
    m_data.QuadBufferPtr->tex_coord = {1.0, 1.0};
    m_data.QuadBufferPtr->tex_id = {tex_index};
    m_data.QuadBufferPtr++;
    assert(m_data.QuadBuffer.end() != m_data.QuadBufferPtr);

    m_data.QuadBufferPtr->pos = {pos.x, pos.y + size.y, 0.0f};
    m_data.QuadBufferPtr->color = color;
    m_data.QuadBufferPtr->tex_coord = {0.0, 1.0};
    m_data.QuadBufferPtr->tex_id = {tex_index};
    m_data.QuadBufferPtr++;

    m_data.index_cout += 6;

    
}

void Renderer::DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const uint32_t texture_id)
{
    if (m_data.index_cout >= MAX_IND_COUNT || m_data.texture_slot_ind > 15)
    {
        EndBatch();
        Flush();
        BeginBatch();
    }

    const glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};

    float tex_index = 0.0f;

    auto end = std::next(m_data.texture_slots.begin(), m_data.texture_slot_ind + 1);
    auto tex_it = std::find(m_data.texture_slots.begin(), end, texture_id);

    float tex_ind = 0.0f;
    if (tex_it == end)
    {
        tex_ind = static_cast<float>( m_data.texture_slot_ind);
        m_data.texture_slots[m_data.texture_slot_ind] = texture_id;
        m_data.texture_slot_ind++;
    }else{
        tex_ind = static_cast<float>( std::distance(m_data.texture_slots.begin(),tex_it));
    }

    m_data.QuadBufferPtr->pos = {pos.x, pos.y, 0.0f};
    m_data.QuadBufferPtr->color = color;
    m_data.QuadBufferPtr->tex_coord = {0.0, 0.0};
    m_data.QuadBufferPtr->tex_id = tex_ind;
    m_data.QuadBufferPtr++;

    m_data.QuadBufferPtr->pos = {pos.x + size.x, pos.y, 0.0f};
    m_data.QuadBufferPtr->color = color;
    m_data.QuadBufferPtr->tex_coord = {1.0, 0.0};
    m_data.QuadBufferPtr->tex_id = tex_ind;
    m_data.QuadBufferPtr++;

    m_data.QuadBufferPtr->pos = {pos.x + size.x, pos.y + size.y, 0.0f};
    m_data.QuadBufferPtr->color = color;
    m_data.QuadBufferPtr->tex_coord = {1.0, 1.0};
    m_data.QuadBufferPtr->tex_id = tex_ind;
    m_data.QuadBufferPtr++;

    m_data.QuadBufferPtr->pos = {pos.x, pos.y + size.y, 0.0f};
    m_data.QuadBufferPtr->color = color;
    m_data.QuadBufferPtr->tex_coord = {0.0, 1.0};
    m_data.QuadBufferPtr->tex_id = tex_ind;
    m_data.QuadBufferPtr++;

    m_data.index_cout += 6;
}