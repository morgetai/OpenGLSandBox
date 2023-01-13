#pragma once
#include <glm/glm.hpp>

struct LightVertex
{
    glm::vec3 Position;
    glm::vec3 Color;
};

struct LightData
{
    glm::vec3 LightPos;
    glm::vec3 LightColor;
    float AmbientStr;
    float SpecularStrength;
};