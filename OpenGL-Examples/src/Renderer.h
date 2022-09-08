#pragma once

#include <array>
#include <memory>

#include <glm/glm.hpp>
#include <GL/glew.h>

struct Vertex
{
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 tex_coord;
    float tex_id;
};

struct RenderData
{
    using TEX_CONTAINER = std::array<uint32_t, 16>;
    using VER_CONTAINER = std::array<Vertex, 4000>;

    GLuint quad_va;
    GLuint quad_vb;
    GLuint quad_ib;
    GLuint white_texture;
    uint32_t white_texture_slot;
    uint32_t index_cout;
    VER_CONTAINER QuadBuffer{};
    VER_CONTAINER::iterator QuadBufferPtr{nullptr};
    TEX_CONTAINER texture_slots{0};
    uint32_t texture_slot_ind = 1;
};

class Renderer final
{
public:
    void Init();
    void Shutdown();

    void BeginBatch();
    void EndBatch();
    void Flush();

    void DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color);
    void DrawQuad(const glm::vec2 &pos, const glm::vec2 &size, const uint32_t texture_id);

private:
    RenderData m_data;
};
