cbuffer cb0 : register(b0)
{
	float4x4 g_wpMatrix;
	float4x4 g_pwMatrix;
	float3 g_cameraPosition;
	float g_cameraAspectRatio;
}
cbuffer cb1 : register(b1)
{
	float4x4 g_lwMatrix;
}
struct VSIn
{
	float3 Position	: POSITION;
	float3 Normal : NORMAL;
	float2 UV	: TEXCOORD;
};
struct PSIn
{
	float4 Position	: SV_POSITION;
	float3 Normal : NORMAL;
	float2 UV	: TEXCOORD;
};
PSIn main(VSIn input)
{
	PSIn output;
	output.Position = mul(mul(float4(input.Position.xyz, 1), g_lwMatrix), g_wpMatrix);
	output.Normal = input.Normal;
	output.UV = input.UV;

	return output;
}