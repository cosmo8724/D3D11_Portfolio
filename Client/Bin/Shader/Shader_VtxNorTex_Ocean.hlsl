#include "Shader_Define.h"

matrix			g_matWorld, g_matView, g_matProj;
matrix			g_matViewInverse, g_matProjInverse;
vector			g_vCamPosition;

/* Light Info */
vector			g_vLightDir;
vector			g_vLightPos;
float			g_fLightRange;
vector			g_vLightDiffuse;
vector			g_vLightAmbient;
vector			g_vLightSpecular;

/* Material Info */
texture2D		g_DiffuseTexture;
vector			g_vMaterialAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector			g_vMaterialSpecular = vector(1.f, 1.f, 1.f, 1.f);

/* UV Moving */
texture2D		g_HeightTexture;
float			g_Time;
float			g_WaveHeight;
float			g_Speed;
float			g_WaveFrequency;
float			g_UVSpeed;

/* Brush Shading */
texture2D		g_BrushTexture;

float			g_WorldHeight;

struct VS_IN
{
	float3		vPosition		: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float3		vTangent		: TANGENT;
};

struct VS_OUT
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vTangent		: TANGENT;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_matWorld);
	Out.vTexUV = In.vTexUV;
	Out.vTangent = (vector)0.f;

	return Out;
}

VS_OUT	VS_MAIN_UVMOVE(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	float		vHeight = g_HeightTexture.SampleLevel(LinearSampler, In.vPosition.xz, 0).y;
	//In.vPosition.y = vHeight.x;

	float		fCosTime = g_WaveHeight * cos(g_Time * g_Speed + (In.vTexUV.x + In.vTexUV.y) * 0.5f * g_WaveFrequency);
	In.vPosition.y += fCosTime;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_matWorld);
	Out.vTexUV = In.vTexUV + float2(0.f, g_Time * g_UVSpeed * -1.f);
	Out.vTangent = (vector)0.f;

	return Out;
}

struct PS_IN
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vTangent		: TANGENT;
};

struct PS_OUT
{
	float4		vColor			: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	Out.vColor = vector(1.f, 1.f, 1.f, 1.f);

	return Out;


	/*vector		vMaterialDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 30.f);

	vMaterialDiffuse = g_vLightDiffuse * vMaterialDiffuse;

	float		fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)));

	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
	vector		vLook = In.vWorldPos - g_vCamPosition;

	float		fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	Out.vColor = vMaterialDiffuse * saturate(fShade + (g_vLightAmbient * g_vMaterialAmbient)) + fSpecular * (g_vLightSpecular * g_vMaterialSpecular);*/
}

PS_OUT   PS_MAIN_UVMOVE(PS_IN In)
{
	PS_OUT   Out = (PS_OUT)0;

	float      fShade = saturate(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)));

	vector      vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));

	vector      vPos = mul(In.vPosition, g_matProjInverse);
	vPos = mul(vPos, g_matViewInverse);

	vector      vLook = vPos - g_vCamPosition;

	float      fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	vector      vBrush = (vector)1.f;

	if (vPos.y > 700.f)
	{
		vBrush = g_BrushTexture.Sample(LinearSampler, In.vTexUV * 20.f);
		vBrush.w = 1.f;
	}

	vector      vMaterialDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV * 30.f) * vBrush;

	vector      vDiffuse = (g_vLightDiffuse * vMaterialDiffuse);

	Out.vColor = vDiffuse * saturate(fShade + (g_vLightAmbient * g_vMaterialAmbient)) + fSpecular * (g_vLightSpecular * g_vMaterialSpecular - 0.5f);

	float4      vMixBlue = Out.vColor + float4(0.2f, 0.2f, 0.9f, 1.f);

	//vMixBlue.a = 1.f;

	Out.vColor = vMixBlue;

	return Out;
}
technique11 DefaultTechnique
{
	pass Terrain
	{
		SetRasterizerState(RS_Wireframe);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader	= compile ps_5_0 PS_MAIN();
	}

	pass UVMOVE
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_UVMOVE();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_UVMOVE();
	}
}