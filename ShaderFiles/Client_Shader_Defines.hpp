

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


RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
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

//BOOL BlendEnable;
//D3D11_BLEND SrcBlend;
//D3D11_BLEND DestBlend;
//D3D11_BLEND_OP BlendOp;
//D3D11_BLEND SrcBlendAlpha;
//D3D11_BLEND DestBlendAlpha;
//D3D11_BLEND_OP BlendOpAlpha;
//UINT8 RenderTargetWriteMask;

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


// D3D11_COMPARISON_FUNC
//BOOL DepthEnable;
//D3D11_DEPTH_WRITE_MASK DepthWriteMask;
//D3D11_COMPARISON_FUNC DepthFunc;

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