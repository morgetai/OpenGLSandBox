#version 410 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in float a_TexId;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexId;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexId = a_TexId;
	gl_Position =  u_ViewProjection * u_Transform * vec4(a_Position, 1.0f) ;
}