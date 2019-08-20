#version 440
#extension GL_ARB_explicit_uniform_location : enable

// Vertex attrib
layout(location = 0) in vec3 POSITION0;
layout(location = 1) in vec3 NORMAL0;
layout(location = 2) in vec2 TEXCOORD0;

// Output
out gl_PerVertex
{
    vec4 gl_Position;
};
out struct
{
	vec3 POSITION0;
	vec3 NORMAL0;
	vec2 TEXCOORD0;
} VSInput;

// Uniforms
layout(std140, binding = 0) uniform PerObject 
{
    mat4 ModelViewProjection;
};

void main(void)
{
	gl_Position = ModelViewProjection * vec4(POSITION0, 1.0);

	VSInput.POSITION0 = POSITION0;
	VSInput.NORMAL0   = NORMAL0;
	VSInput.TEXCOORD0 = vec2(TEXCOORD0.x, 1.0f - TEXCOORD0.y);
};