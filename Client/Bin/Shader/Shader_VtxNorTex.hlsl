matrix			g_matWorld, g_matView, g_matProj;
vector			g_vCamPosition;

/* Light Info */
vector			g_vLightDir;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

/* Material Info */
texture2D		g_DiffuseTexture;
vector			g_vMaterialAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMaterialSpecular = vector(1.f, 1.f, 1.f, 1.f);

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

struct VS_IN
{
	float3		vPosition		: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition		: SV_POSITION;
	float		fShade			: COLORD0;
	float		fSpecular		: COLORD1;
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

	vector		vWorldPosition = mul(float4(In.vPosition, 1.f), g_matWorld);
	vector		vWorldNormal = mul(float4(In.vNormal, 0.f), g_matWorld);

	Out.fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vWorldNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
	vector		vLook = vWorldPosition - g_vCamPosition;

	Out.fSpecular = saturate(dot(normalize(vReflect) * -1.f, normalize(vLook)));

	return Out;
}

struct PS_IN
{
	float4		vPosition		: SV_POSITION;
	float		fShade			: COLORD0;
	float		fSpecular		: COLORD1;
	float2		vTexUV		: TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor		: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector		vMaterialDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 30.f);

	vector		vDiffuse = g_vLightDiffuse * vMaterialDiffuse;

	Out.vColor = vDiffuse * saturate(In.fShade + (g_vLightAmbient * g_vMaterialAmbient)) + In.fSpecular * (g_vLightSpecular * g_vMaterialSpecular);
	//Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return Out;
}

RasterizerState rsWireFrame { FillMode = WireFrame; };

technique11 DefaultTechnique
{
	pass Terrain
	{
		//SetRasterizerState(rsWireFrame);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader	= compile ps_5_0 PS_MAIN();
	}
}