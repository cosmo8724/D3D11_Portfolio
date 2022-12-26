#include "Shader_Define.h"

matrix			g_matWorld, g_matView, g_matProj;
vector			g_vCamPosition;

/* Light Info */
vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

/* Material Info */
texture2D		g_DiffuseTexture[2];
vector			g_vMaterialAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMaterialSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* Terrain Shading */
texture2D		g_BrushTexture;
vector			g_vBrushPos;
float			g_fBrushRange = 5.f;
texture2D		g_FilterTexture;

struct VS_IN
{
	float3		vPosition		: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vWorldPos	: TEXCOORD1;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_matWorld);
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_matWorld);

	return Out;
}

struct PS_IN
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vWorldPos	: TEXCOORD1;
};

struct PS_OUT
{
	float4		vColor		: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector		vSourDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexUV* 30.f);
	vector		vDestDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexUV * 30.f);
	vector		vFilter = g_FilterTexture.Sample(LinearSampler, In.vTexUV);

	vector		vBrush = (vector)0.f;

	if (g_vBrushPos.x - g_fBrushRange <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fBrushRange
		&& g_vBrushPos.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fBrushRange)
	{
		float2		vUV;

		vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);
		vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

		vBrush = g_BrushTexture.Sample(LinearSampler, vUV);
	}

	vector		vMaterialDiffuse = vSourDiffuse * (1.f - vFilter.r) + vDestDiffuse * vFilter.r + vBrush;

	vector		vDiffuse = g_vLightDiffuse * vMaterialDiffuse;

	float		fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	vector		vLook = In.vWorldPos - g_vCamPosition;

	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	Out.vColor = vDiffuse * saturate(fShade + (g_vLightAmbient * g_vMaterialAmbient)) + fSpecular * (g_vLightSpecular * g_vMaterialSpecular);

	return Out;
}

technique11 DefaultTechnique
{
	pass Terrain
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader	= compile ps_5_0 PS_MAIN();
	}
}