#version 440

// Vertex attrib
layout(location = 0) in vec3 POSITION0;
layout(location = 1) in vec4 COLOR0;

// Output
out gl_PerVertex
{
    vec4 gl_Position;
};
out struct
{
	vec3 POSITION0;
	vec4 COLOR0;
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
	VSInput.COLOR0   = COLOR0;
};