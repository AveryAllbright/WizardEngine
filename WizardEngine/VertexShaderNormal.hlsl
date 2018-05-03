
// Constant Buffer
cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	matrix lightView; // for shadow map
	matrix lightProjection; // for shadow map
	float2 uvTiling;
};

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;     // XYZ position
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
};

// Struct representing the data we're sending down the pipeline
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
	float4 shadowPos	: SHADOWPOS;
};

VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	matrix worldViewProj = mul(mul(world, view), projection);

	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	matrix shadowMatrix = mul(mul(world, lightView), lightProjection);
	output.shadowPos = mul(float4(input.position, 1.0f), shadowMatrix);

	output.normal = normalize(mul(input.normal, (float3x3)world));

	output.tangent = normalize(mul(input.tangent, (float3x3)world));

	//output.uv = input.uv;

	output.uv.x = input.uv.x * uvTiling.x;
	output.uv.y = input.uv.y * uvTiling.y;

	return output;
}