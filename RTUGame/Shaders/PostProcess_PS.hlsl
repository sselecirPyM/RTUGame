
Texture2D texture0 :register(t0);
SamplerState s0 : register(s0);

float luminance(float3 rgb)
{
	return dot(rgb, float3(0.299f, 0.587f, 0.114f));
}

float3 TonemapACES(float3 x)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;
	return (x * (A * x + B)) / (x * (C * x + D) + E);
}

float3 TonemapHable(float3 x)
{
	const float A = 0.22;
	const float B = 0.30;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.01;
	const float F = 0.30;
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}
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
	float4 Pos	: SV_POSITION;		//Position
	float2 uv : TEXCOORD;
};

float4 main(PSIn input) : SV_TARGET
{
	float2 uv = input.uv;
	float4 color = texture0.Sample(s0, uv);
	color.rgb = pow(max(color.rgb,0.001), 1 / 2.2f);
	//color.rgb = normalize(NormalDecode(color.rg)) * 0.5 + 0.5;
	return color;
}