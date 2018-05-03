cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	float3 position		: POSITION;     // XYZ position
	float2 uv			: TEXCOORD;
	float3 normal       : NORMAL;
	float3 tangent		: TANGENT;
};

float4 main(VertexShaderInput input) : SV_POSITION
{
	matrix worldViewProj = mul(mul(world, view), projection);

	return mul(float4(input.position, 1.0f), worldViewProj);
}