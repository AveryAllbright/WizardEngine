cbuffer externalData : register(b0)
{
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 sampleDir	: TEXCOORD;
};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	matrix viewNoTranslate = view;
	viewNoTranslate._41 = 0;
	viewNoTranslate._42 = 0;
	viewNoTranslate._43 = 0;

	matrix viewProj = mul(viewNoTranslate, projection);
	output.position = mul(float4(input.position, 1.0f), viewProj);

	output.position.z = output.position.w;

	output.sampleDir = input.position;

	return output;
}