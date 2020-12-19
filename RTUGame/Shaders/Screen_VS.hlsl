struct VSIn
{
	float4 Pos	: POSITION;			//Position
};

struct PSIn
{
	float4 Pos	: SV_POSITION;		//Position
	float2 uv	: TEXCOORD;
};

PSIn main(VSIn input)
{
	PSIn output;
	output.Pos = float4(input.Pos.xyz, 1);
	output.Pos.z = 1 - 1e-6;

	float2 uv = input.Pos.xy * 0.5 + 0.5;
	uv.y = 1 - uv.y;
	output.uv = uv;

	return output;
}