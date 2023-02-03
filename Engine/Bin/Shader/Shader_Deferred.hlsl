matrix			g_matWorld, g_matView, g_matProj;
matrix			g_matViewInv, g_matProjInv;

vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;

vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

matrix			g_matLightView;
matrix			g_matLightProj;

vector			g_vCamPosition;

vector			g_vMaterialAmbient = (vector)1.f;
vector			g_vMaterialSpecular = (vector)1.f;

texture2D		g_Texture;
texture2D		g_NormalTexture;
texture2D		g_DepthTexture;
texture2D		g_DiffuseTexture;
texture2D		g_ShadeTexture;
texture2D		g_SpecularTexture;
texture2D		g_ShadowDepthTexture;

sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = wrap;
	AddressV = wrap;
};

RasterizerState	RS_Default
{
	FillMode = Solid;
	CullMode = Back;
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
	BlendEnable[1] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = Add;
};

struct VS_IN
{
	float3		vPosition	: POSITION;	
	float2		vTexUV	: TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition	: SV_POSITION;	
	float2		vTexUV	: TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
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

struct PS_OUT_LIGHT
{
	float4		vShade		: SV_TARGET0;
	float4		vSpecular	: SV_TARGET1;
};

PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	
	return Out;
}

PS_OUT_LIGHT	PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * 3000.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * ceil(saturate(saturate(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal))) + (g_vLightAmbient * g_vMaterialAmbient)) * 5.f) / 5.f;
	Out.vShade.a = 1.f;

	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	vWorldPos *= fViewZ;

	vWorldPos = mul(vWorldPos, g_matProjInv);
	vWorldPos = mul(vWorldPos, g_matViewInv);

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMaterialSpecular) * ceil(pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 100.f) * 0.7f * 30.f) / 30.f;
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT_LIGHT	PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT	Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

	float		fViewZ = vDepthDesc.y * 3000.f;

	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.x;
	vWorldPos.w = 1.f;

	vWorldPos *= fViewZ;

	vWorldPos = mul(vWorldPos, g_matProjInv);
	vWorldPos = mul(vWorldPos, g_matViewInv);

	vector		vLightDir = vWorldPos - g_vLightPos;

	float		fDistance = length(vLightDir);

	float		fAtt = max((g_fLightRange - fDistance), 0.f) / g_fLightRange;

	Out.vShade = g_vLightDiffuse * saturate(saturate(dot(normalize(vLightDir) * -1.f, normalize(vNormal))) + (g_vLightAmbient * g_vMaterialAmbient)) * fAtt;
	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(vLightDir), normalize(vNormal));
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightSpecular * g_vMaterialSpecular) * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f) * fAtt;
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT	PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
	vector		vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vDiffuse * vShade + vSpecular;

	vector		vDepthInfo = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
	float		fViewZ = vDepthInfo.y * 3000.f;

	vector		vPos;
	vPos.x = (In.vTexUV.x * 2.f - 1.f);
	vPos.y = (In.vTexUV.y * -2.f + 1.f);
	vPos.z = vDepthInfo.x;
	vPos.w = 1.f;

	vPos *= fViewZ;
	
	vPos = mul(vPos, g_matProjInv);
	vPos = mul(vPos, g_matViewInv);
	vPos = mul(vPos, g_matLightView);

	vector		vUVPos = mul(vPos, g_matLightProj);
	float2		vLightUV = float2((vUVPos.x / vUVPos.w) * 0.5f + 0.5f, (vUVPos.y / vUVPos.w) * -0.5f + 0.5f);

	vector		vShadowDepthInfo = g_ShadowDepthTexture.Sample(LinearSampler, vLightUV);

	if (vPos.z - 0.1f > vShadowDepthInfo.x * 1000.f)
		Out.vColor = Out.vColor * vector(0.3f, 0.3f, 0.3f, 1.f);

	if (0.f == Out.vColor.a)
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Light_Directional
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_One, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	pass Blend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}
}
