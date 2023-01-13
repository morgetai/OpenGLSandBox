#type vertex
#version 410 core

layout(location = 0) in vec3 a_WorldPosition;
layout(location = 1) in vec3 a_LocalPosition;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_Thickness;
layout(location = 4) in float a_Fade;

layout(std140) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) out VertexOutput v_Output;

void main()
{
	v_Output.LocalPosition = a_LocalPosition;
	v_Output.Color = a_Color;
	v_Output.Thickness = a_Thickness;
	v_Output.Fade = a_Fade;

	gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
}

#type fragment
#version 410 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) in VertexOutput v_Output;

void main()
{
    // Calculate distance and fill circle with white
    float distance = length(v_Output.LocalPosition);
    float circle = smoothstep(0.0, v_Output.Fade, distance);
    circle *= smoothstep(v_Output.Thickness + v_Output.Fade, v_Output.Thickness, distance);

	if (circle == 0.0)
		discard;

    // Set output color
    o_Color = v_Output.Color;
	o_Color.a *= circle;
}