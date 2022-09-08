#version 410 core

layout (location = 0) out vec4 o_Color;

uniform vec4 u_Color;
uniform sampler2D u_Textures[16];
in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexId;

void main()
{
	int index = int(v_TexId);
	o_Color = texture(u_Textures[index], v_TexCoord) * v_Color;
}