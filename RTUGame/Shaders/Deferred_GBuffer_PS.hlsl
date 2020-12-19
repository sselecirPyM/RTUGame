struct MRTOutput
{
	float4 color0 : COLOR0;
	float4 color1 : COLOR1;
	float4 color2 : COLOR2;
};
struct PSIn
{
	float4 Position	: SV_POSITION;
	float3 Normal : NORMAL;
	float2 UV	: TEXCOORD;
};
Texture2D texture0 :register(t0);
SamplerState s0 : register(s0);
SamplerState s1 : register(s1);
SamplerState s3 : register(s3);

float2 NormalEncode(float3 n)
{
	float2 nxy;
	if (n.x == 0 && n.y == 0)
	{
		nxy = float2(1, 0);
	}
	else
	{
		nxy = normalize(n.xy);
	}
	float2 enc = nxy * (sqrt(max(-n.z * 0.5 + 0.5,1e-6)));
	enc = enc * 0.5 + 0.5;
	return enc;
}

MRTOutput main(PSIn input) : SV_TARGET
{
	float3 N = normalize(input.Normal);
	float2 encodedNormal = NormalEncode(N);
	float4 color = texture0.Sample(s3,input.UV);
	MRTOutput output;
	output.color0 = float4(color.rgb, 1.0f);
	output.color1 = float4(encodedNormal,0, 1.0f);
	//output.color1 = float4(N*0.5+0.5, 1.0f);
	output.color2 = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return output;
}