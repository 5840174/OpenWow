#include "CommonInclude.hlsl"

struct SParticle
{
	float3 Position;
	float __padding0;
	//--------------------------------------------------------------( 16 bytes )

	float2 TexCoordBegin;
	float2 TexCoordEnd;
	//--------------------------------------------------------------( 16 bytes )

	float4 Color;
	//--------------------------------------------------------------( 16 bytes )

	float2 Size;
	float __padding1[2];
	//--------------------------------------------------------------( 16 bytes )
};



//
// Structs
//
struct SVSOutput
{
	uint VertexID : POSITION;
};

struct SPSInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color    : COLOR0;
};


Texture2D DiffuseTexture             : register(t0);
StructuredBuffer<SParticle> Particles : register(t10);



SVSOutput VS_main(uint VertexID : SV_VertexID)
{
	SVSOutput vsOutput;
	vsOutput.VertexID = VertexID;
	return vsOutput;
}


[maxvertexcount(4)]
void GS_Billboard(point SVSOutput VSOutput[1], inout TriangleStream<SPSInput> OutputStream)
{
	SParticle p = Particles[VSOutput[0].VertexID];

	float3 transformePosition = mul(PO.Model, float4(p.Position, 1.0f)).xyz;

	float3 planeNormal = transformePosition - GetCameraPosition();
	planeNormal = normalize(planeNormal);

	float3 upVector = GetCameraUp();

	float3 rightVector = normalize(cross(planeNormal, upVector));
	rightVector *= p.Size.x;
	upVector *= p.Size.y;

	float3 scale = ExtractScaleMatrix(PO.Model);
	float scaleLength = length(scale);
	//rightVector *= scaleLength;
	//upVector *= scaleLength;

	// Create the billboards quad
	float3 vert[4];
	vert[0] = transformePosition - rightVector - upVector; // Get bottom left vertex
	vert[1] = transformePosition + rightVector - upVector; // Get bottom right vertex
	vert[2] = transformePosition - rightVector + upVector; // Get top left vertex
	vert[3] = transformePosition + rightVector + upVector; // Get top right vertex

	// Get billboards texture coordinates
	float2 texCoord[4];
	texCoord[0] = float2(p.TexCoordBegin.x, p.TexCoordEnd.y);
	texCoord[1] = float2(p.TexCoordEnd.x,   p.TexCoordEnd.y);
	texCoord[2] = float2(p.TexCoordBegin.x, p.TexCoordBegin.y);
	texCoord[3] = float2(p.TexCoordEnd.x,   p.TexCoordBegin.y);

	const float4x4 vp = mul(PF.Projection, PF.View);

	// Now we "append" or add the vertices to the outgoing stream list
	for (int i = 0; i < 4; i++)
	{
		SPSInput outputVert = (SPSInput)0;
		outputVert.position = mul(vp, float4(vert[i], 1.0f));
		outputVert.texcoord = texCoord[i];
		outputVert.color = p.Color;

		OutputStream.Append(outputVert);
	}
}

float4 PS_main(SPSInput PSInput) : SV_TARGET
{
	float4 DiffuseColor = DiffuseTexture.Sample(LinearClampSampler, float2(PSInput.texcoord.x, 1.0f - PSInput.texcoord.y));
	//if (DiffuseColor.a < 0.01)
	//	DiffuseColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return DiffuseColor * PSInput.color;
}