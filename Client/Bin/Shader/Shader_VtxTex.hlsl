<<<<<<< HEAD
matrix			g_matWorld, g_matView, g_matProj;
texture2D		g_Texture;

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
=======
matrix		g_matWorld, g_matView, g_matProj;
>>>>>>> parent of 72a95805 (Review 22.11.15)

struct VS_IN
{
	float3		vPosition		: POSITION;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition		: SV_POSITION;
	float2		vTexUV		: TEXCOORD0;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition	: SV_POSITION;
	float2		vTexUV	: TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor		: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

<<<<<<< HEAD
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
=======
	Out.vColor = vector(1.f, 0.f, 0.f, 1.f);
>>>>>>> parent of 72a95805 (Review 22.11.15)

	return Out;
}

technique11 DefaultTechnique
{
	pass Rect
	{
		VertexShader	= compile vs_5_0 VS_MAIN();
		PixelShader	= compile ps_5_0 PS_MAIN();
	}
}