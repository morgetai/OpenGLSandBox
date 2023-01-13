#pragma once
#include <glm/glm.hpp>
#include <variant>

namespace Geometry
{
    struct Quad
    {
        glm::vec3 position;
        glm::vec2 size;
        glm::vec4 color;
        float rotation;
    };

    struct Line
    {
        glm::vec3 p0;
        glm::vec3 p1;
        glm::vec4 color;
    };

    struct Rect
    {
        glm::vec3 position;
        glm::vec2 size;
        glm::vec4 color;
        float rotation;
    };

    struct Cube
    {
        glm::vec3 position; 
        glm::vec3 size;
        glm::vec4 color;
        glm::vec3 rotation;
    };

    struct Circle
    {
        glm::vec3 position {}; 
        float radius {};
        glm::vec4 color {};
    };

    using GeometryObj = std::variant<Quad,Line,Rect,Cube,Circle>;

}