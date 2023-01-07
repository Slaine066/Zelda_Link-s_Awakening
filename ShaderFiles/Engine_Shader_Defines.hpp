/*
*.hpp Files are Headers which can be included by Shader Files (*.hlsl)
*/

#pragma region Samplers
sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_Linear;
};

sampler DepthSampler = sampler_state
{
	filter = MIN_MAG_LINEAR_MIP_POINT;
AddressU = clamp;
AddressV = clamp;
};

sampler ShadowDepthSampler = sampler_state
{
	filter = MIN_MAG_LINEAR_MIP_POINT;
AddressU = clamp;
AddressV = clamp;
};
#pragma endregion

#pragma region RasterizerStates
RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};
#pragma endregion

#pragma region BlendStates
BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_LightBlending
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = add;
};
#pragma endregion 

#pragma region DepthStencilStates
DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_ZEnable_Disable_ZWrite_Disable
{
	DepthEnable = false;
	DepthWriteMask = zero;
};
#pragma endregion