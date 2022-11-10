#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in vec3 a_Normal;

layout(std140) uniform Camera
{
	mat4 u_ViewProjection;
	mat4 u_Model;
	vec3 u_CameraPos;
};

layout(std140) uniform Light
{
	vec3 u_LightPos;
	vec3 u_LightColor;
	float u_AmbientStr;
	float u_SpecularStrength;
};

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TilingFactor;
	vec3 Normal;
	vec3 WorlPos;
};

layout (location = 0) out VertexOutput v_Output;
layout (location = 6) flat out float v_TexIndex;
layout (location = 7) flat out vec3 v_LightPos;
layout (location = 8) flat out float v_AmbientStr;
layout (location = 9) flat out vec3 v_LightColor;
layout (location = 10) flat out vec3 v_CameraPos;
layout (location = 11) flat out float v_SpecularStrength;

void main()
{
	v_Output.Color = a_Color;
	v_Output.TexCoord = a_TexCoord;
	v_Output.TilingFactor = a_TilingFactor;
	v_Output.Normal = a_Normal;
	v_Output.WorlPos = vec3(u_Model * vec4(a_Position, 1.0));
	v_TexIndex = a_TexIndex;

	v_LightPos = vec3(u_Model * vec4(u_LightPos, 1.0));
	v_AmbientStr = u_AmbientStr;
	v_LightColor = u_LightColor;

	v_CameraPos = vec3(u_Model * vec4(u_CameraPos,1.0));

	v_SpecularStrength = u_SpecularStrength;

	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 410 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TilingFactor;
	vec3 Normal;
	vec3 WorlPos;
};

layout (location = 0) in VertexOutput v_Output;
layout (location = 6) flat in float v_TexIndex;
layout (location = 7) flat in vec3 v_LightPos;
layout (location = 8) flat in float v_AmbientStr;
layout (location = 9) flat in vec3 v_LightColor;
layout (location = 10) flat in vec3 v_CameraPos;
layout (location = 11) flat in float v_SpecularStrength;

uniform sampler2D u_Textures[16];

void main()
{
	//
	vec3 norm = normalize(v_Output.Normal);
	vec3 light_dir = normalize(v_LightPos - v_Output.WorlPos);
	//
	vec3 ambient = v_AmbientStr * v_LightColor;
	float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * v_LightColor;
	//
	vec3 viewDir = normalize(v_CameraPos - v_Output.WorlPos);
	vec3 reflectDir = reflect(-light_dir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = v_SpecularStrength * spec * v_LightColor;   
	//
	vec3 fong_light = ambient + diffuse + specular;
	vec4 texColor = vec4(fong_light,1.0) * v_Output.Color;

	switch(int(v_TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], v_Output.TexCoord * v_Output.TilingFactor); break;
		case  1: texColor *= texture(u_Textures[ 1], v_Output.TexCoord * v_Output.TilingFactor); break;
		case  2: texColor *= texture(u_Textures[ 2], v_Output.TexCoord * v_Output.TilingFactor); break;
		case  3: texColor *= texture(u_Textures[ 3], v_Output.TexCoord * v_Output.TilingFactor); break;
		case  4: texColor *= texture(u_Textures[ 4], v_Output.TexCoord * v_Output.TilingFactor); break;
		case  5: texColor *= texture(u_Textures[ 5], v_Output.TexCoord * v_Output.TilingFactor); break;
		case  6: texColor *= texture(u_Textures[ 6], v_Output.TexCoord * v_Output.TilingFactor); break;
		case  7: texColor *= texture(u_Textures[ 7], v_Output.TexCoord * v_Output.TilingFactor); break;
		case  8: texColor *= texture(u_Textures[ 8], v_Output.TexCoord * v_Output.TilingFactor); break;
		case  9: texColor *= texture(u_Textures[ 9], v_Output.TexCoord * v_Output.TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], v_Output.TexCoord * v_Output.TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], v_Output.TexCoord * v_Output.TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], v_Output.TexCoord * v_Output.TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], v_Output.TexCoord * v_Output.TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], v_Output.TexCoord * v_Output.TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], v_Output.TexCoord * v_Output.TilingFactor); break;
	}

	if (texColor.a == 0.0)
		discard;
		
	o_Color = texColor;
}