float3x3 Kx = { -1, 0, 1,
-2, 0, 2,
-1, 0, 1 };

float3x3 Ky = { 1, 2, 1,
0, 0, 0,
-1,-2,-1 };

sampler	LinearSampler = sampler_state
{
	filter = min_mag_mip_Linear;
AddressU = wrap;
AddressV = wrap;
};

sampler	PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
AddressU = wrap;
AddressV = wrap;
};

sampler WaterBumpSampler = sampler_state
{
	filter = min_mag_mip_Linear;
	AddressU = mirror;
	AddressV = mirror;
};

/* 
D3D11_FILL_MODE fillMode,
D3D11_CULL_MODE cullMode,
BOOL frontCounterClockwise,
INT depthBias,
FLOAT depthBiasClamp,
FLOAT slopeScaledDepthBias,
BOOL depthClipEnable,
BOOL scissorEnable,
BOOL multisampleEnable,
BOOL antialiasedLineEnable
*/
RasterizerState	RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	DepthClipEnable = true;
	FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState	RS_CW
{
	CullMode = Front;
	FrontCounterClockwise = false;
};

RasterizerState RS_NonCulling
{
	FillMode = solid;
	CullMode = none;
	FrontCounterClockwise = false;
};

DepthStencilState DS_Default
{
	DepthEnable = true;
DepthWriteMask = all;
DepthFunc = less_equal;
};

DepthStencilState DS_ZEnable_ZWriteEnable_FALSE
{
	DepthEnable = false;
DepthWriteMask = zero;
};

BlendState BS_Default
{
	BlendEnable[0] = false;


//bool BlendEnable;
//D3D11_BLEND SrcBlend;
//D3D11_BLEND DestBlend;
//D3D11_BLEND_OP BlendOp;
//D3D11_BLEND SrcBlendAlpha;
//D3D11_BLEND DestBlendAlpha;
//D3D11_BLEND_OP BlendOpAlpha;
//UINT8 RenderTargetWriteMask;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
SrcBlend = SRC_ALPHA;
DestBlend = INV_SRC_ALPHA;
BlendOp = Add;
};

BlendState BS_One
{
	BlendEnable[0] = true;
SrcBlend = SRC_ALPHA;
DestBlend = ONE;
BlendOp = Add;
};