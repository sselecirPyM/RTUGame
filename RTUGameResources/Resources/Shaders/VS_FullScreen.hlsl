struct VSIn
{
	float3 Pos	: POSITION;
};

struct PSIn
{
	float4 Pos	: SV_POSITION;
	float2 uv   : TEXCOORD;
};

PSIn main(VSIn input)
{
	PSIn output;
	output.Pos = float4(input.Pos, 1);
	output.uv = float2(input.Pos.x*0.5+0.5, -input.Pos.y*0.5+0.5);
	return output;
}