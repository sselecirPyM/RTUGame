//float4 main() : SV_TARGET
//{
//	float4 color = float4(1, 1, 0, 1);
//	return color;
//}

struct PSIn
{
	float4 Pos	: SV_POSITION;		//Position
	float2 uv	: TEXCOORD;
};
//cbuffer cb0 : register(b0)
//{
//	float4x4 g_mWorldToProj;
//	float4x4 g_mProjToWorld;
//	float3   g_vCamPos;
//	float g_skyBoxMultiple;
//};
cbuffer cb0 : register(b0)
{
	float4 g_color1;
};
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(PSIn input) : SV_TARGET
{
	//float4 vx = mul(float4(input.uv,0,1),g_mProjToWorld);
	//float3 viewDir = vx.xyz / vx.w - g_vCamPos;
	////return float4(EnvCube.Sample(s0, viewDir).rgb * g_skyBoxMultiple,1);
	//return vx;


	//float4 color = float4(input.uv, 0, 1);
	float4 color = g_color1;
	return float4( texture0.Sample(sampler0, input.uv).a,1,1,1);
	return color;
}