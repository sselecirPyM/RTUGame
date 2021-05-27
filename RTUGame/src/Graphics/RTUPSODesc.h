#pragma once
enum RTU_BLEND_MODES
{
	RTU_BLEND_MODE_NONE = 0,
	RTU_BLEND_MODE_ALPHA = 1,
	RTU_BLEND_MODE_ADD = 2,
};
enum struct RTU_INPUT_LAYOUT
{
	POSITION_ONLY = 0,
	P_N_UV_T = 1,
	IMGUI = 2,
};
struct RTUPipelineStateDesc
{
	int renderTargetCount;
	RTU_INPUT_LAYOUT eInputLayout;
	int depthBias;
	DXGI_FORMAT rtvFormat;
	DXGI_FORMAT dsvFormat;
	D3D_PRIMITIVE_TOPOLOGY primitiveType;
	RTU_BLEND_MODES blendmode;
};

