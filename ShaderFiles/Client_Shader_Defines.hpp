/*
*.hpp Files are Headers which can be included by Shader Files (*.hlsl) 
*/

#pragma region Samplers
sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};
#pragma endregion

#pragma region RasterizerStates
RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Default_NoCull
{
	FillMode = solid;
	CullMode = none;
	FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState RS_SkyBox
{
	FillMode = solid;
	CullMode = front;
	FrontCounterClockwise = false;
};
#pragma endregion

#pragma region BlendStates
BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlending
{
	BlendEnable[0] = true;
	
	SrcBlend = src_alpha;
	DestBlend = inv_Src_Alpha;
	BlendOp = add;
};

BlendState BS_AddBlending
{
	BlendEnable[0] = true;

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

DepthStencilState DSS_Priority
{
	DepthEnable = false;
	DepthWriteMask = zero;	
};
#pragma endregion