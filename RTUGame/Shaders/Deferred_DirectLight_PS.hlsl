static const float PI = 3.141592653589793238f;
cbuffer cb0 : register(b0)
{
	float4x4 g_wpMatrix;
	float4x4 g_pwMatrix;
	float3 g_cameraPosition;
	float g_cameraAspectRatio;
}
cbuffer cb1 : register(b1)
{
	float4x4 g_lightMatrice[4];
	float4 g_lightColor;
	float3 g_lightDirection;
}
Texture2D texture0 :register(t0);
Texture2D texture1 :register(t1);
Texture2D gbufferDepth : register (t2);
Texture2D shadowDepth : register (t3);
SamplerState s0 : register(s0);
SamplerComparisonState sampleShadowMap0 : register(s2);
SamplerState s3 : register(s3);

float3 NormalDecode(float2 enc)
{
	float4 nn = float4(enc * 2, 0, 0) + float4(-1, -1, 1, -1);
	float l = dot(nn.xyz, -nn.xyw);
	nn.z = l;
	nn.xy *= sqrt(max(l, 1e-6));
	return nn.xyz * 2 + float3(0, 0, -1);
}
struct PSIn
{
	float4 Position	: SV_POSITION;
	float2 uv	: TEXCOORD;
};
float4 main(PSIn input) : SV_TARGET
{
	float2 uv = input.uv;
	float2 uv1 = input.uv * 2 - 1;
	uv1.y = -uv1.y;

	float depth1 = gbufferDepth.SampleLevel(s3, uv, 0).r;
	float4 wPos = mul(float4(uv1, depth1, 1), g_pwMatrix);
	wPos /= wPos.w;
	float3 V = normalize(g_cameraPosition - wPos.xyz);

	if (depth1 != 1.0)
	{
		float4 buffer0Color = texture0.Sample(s3, uv);
		float4 buffer1Color = texture1.Sample(s3, uv);
		float3 albedo = buffer0Color.rgb;
		float metallic = buffer0Color.a;
		float roughness = buffer1Color.b;
		float3 N = normalize(NormalDecode(buffer1Color.rg));

		float inShadow = 1.0f;
		{
			float4 sPos = mul(wPos, g_lightMatrice[0]);
			sPos = sPos / sPos.w;

			float2 shadowTexCoords;
			shadowTexCoords.x = 0.5f + (sPos.x * 0.5f);
			shadowTexCoords.y = 0.5f - (sPos.y * 0.5f);
			if (sPos.x >= -1 && sPos.x <= 1 && sPos.y >= -1 && sPos.y <= 1)
				inShadow = shadowDepth.SampleCmpLevelZero(sampleShadowMap0, shadowTexCoords, sPos.z).r;
			//if (sPos.x >= -1 && sPos.x <= 1 && sPos.y >= -1 && sPos.y <= 1)
			//	return float4(sPos.zzz/2, 1.0f);
		}
		return float4(saturate(dot(N, g_lightDirection)) * g_lightColor * albedo * inShadow, 1.0f);
		//return float4(wPos.xyz, 1.0f);
	}
	else
	{
		return float4(0, 0, 0, 1.0f);
	}
}