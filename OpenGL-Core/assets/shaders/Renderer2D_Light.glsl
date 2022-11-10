#type vertex
#version 410 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

layout(std140) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_Model;
};

layout (location = 0) out vec3 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0f);
}

#type fragment
#version 410 core

layout(location = 0) out vec4 o_Color;

layout (location = 0) in vec3 v_Color;

void main()
{
    o_Color = vec4(v_Color, 1.0f);
}