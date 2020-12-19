#pragma once
enum RTU_BLEND_MODES
{
	RTU_BLEND_MODE_NONE = 0,
	RTU_BLEND_MODE_ALPHA = 1,
	RTU_BLEND_MODE_ADD = 2,
};
enum RTU_INPUT_LAYOUT
{
	RTU_INPUT_LAYOUT_POSITION_ONLY = 0,
	RTU_INPUT_LAYOUT_P_N_UV_T = 1,
};
struct RTUPipelineStateDesc
{
	int renderTargetCount;
	uint32_t eInputLayout;
	int depthBias;
	DXGI_FORMAT rtvFormat;
	DXGI_FORMAT dsvFormat;
	D3D_PRIMITIVE_TOPOLOGY primitiveType;
	RTU_BLEND_MODES blendmode;
};

