#include "Shader_Define.h"

matrix			g_matWorld, g_matView, g_matProj;
matrix			g_matBones[600];
matrix			g_matReflectView;

float2			g_WinSize = { 0.f, 0.f };
float4			g_vClipPlane;

texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_MaskTexture;

float4			g_HairColor;

struct VS_IN
{
	float3		vPosition		: POSITION;
	float3		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float3		vTangent		: TANGENT;

	uint4		vBlendIndex	: BLENDINDEX;
	float4		vBlendWeight	: BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vProjPos		: TEXCOORD1;
	float4		vTangent		: TANGENT;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		matBone = g_matBones[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_matBones[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_matBones[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_matBones[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), matBone);
	vector		vNormal = mul(float4(In.vNormal, 0.f), matBone);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_matWorld));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = (vector)0.f;

	return Out;
}

[clipplanes(g_vClipPlane)]
VS_OUT	VS_MAIN_REFLECT(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	matrix		matWRV, matWRVP;

	matWRV = mul(g_matWorld, g_matReflectView);
	matWRVP = mul(matWRV, g_matProj);

	float		fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

	matrix		matBone = g_matBones[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_matBones[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_matBones[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_matBones[In.vBlendIndex.w] * fWeightW;

	vector		vPosition = mul(float4(In.vPosition, 1.f), matBone);
	vector		vNormal = mul(float4(In.vNormal, 0.f), matBone);

	Out.vPosition = mul(vPosition, matWRVP);
	Out.vNormal = normalize(mul(vNormal, g_matWorld));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = (vector)0.f;

	return Out;
}

struct PS_IN
{
	float4		vPosition		: SV_POSITION;
	float4		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vProjPos		: TEXCOORD1;
	float4		vTangent		: TANGENT;
};

struct PS_OUT
{
	float4		vDiffuse		: SV_TARGET0;
	float4		vNormal		: SV_TARGET1;
	float4		vDepth		: SV_TARGET2;
};

struct PS_OUT_SHADOWDEPTH
{
	float4		vLightDepth	: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	if (0.1f > vDiffuse.a)
		discard;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

	return Out;
}

PS_OUT	PS_MAIN_OUTLINE(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	vDiffuse.a = vDiffuse.a * 0.5f;

	if (vDiffuse.a < 0.1f)
		discard;

	float Lx = 0;
	float Ly = 0;

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			float2 offset = float2(x, y) * float2(1 / g_WinSize.x, 1 / g_WinSize.y);
			float3 tex = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + offset).rgb;
			float luminance = dot(tex, float3(0.3, 0.59, 0.11));

			Lx += luminance * Kx[y + 1][x + 1];
			Ly += luminance * Ky[y + 1][x + 1];
		}
	}
	float L = sqrt((Lx*Lx) + (Ly*Ly));

	if (L < 0.1) // �̰� �� ���� �ϸ� �� �β� ���� ����
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	}
	else
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV) * 0.3f;
	}

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

	return Out;
}

PS_OUT_SHADOWDEPTH	PS_MAIN_SHADOW_WRITE(PS_IN In)
{
    PS_OUT_SHADOWDEPTH Out = (PS_OUT_SHADOWDEPTH) 0;

    Out.vLightDepth = vector(In.vProjPos.w / 1000.f, 0.f, 0.f, 1.f);

	return Out;
}

PS_OUT	PS_MAIN_HAIRMASK(PS_IN In)
{
	PS_OUT         Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	vDiffuse.a = vDiffuse.a * 0.5f;

	if (vDiffuse.a < 0.1f)
		discard;

	float Lx = 0;
	float Ly = 0;

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			float2 offset = float2(x, y) * float2(1 / g_WinSize.x, 1 / g_WinSize.y);
			float3 tex = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV + offset).rgb;
			float luminance = dot(tex, float3(0.3, 0.59, 0.11));

			Lx += luminance * Kx[y + 1][x + 1];
			Ly += luminance * Ky[y + 1][x + 1];
		}
	}
	float L = sqrt((Lx*Lx) + (Ly*Ly));

	if (L < 0.1) // �̰� �� ���� �ϸ� �� �β� ���� ����
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	}
	else
	{
		Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV) * 0.3f;
	}

	vector		vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	// Hair Color
	if (vMask.r == 1.f && vMask.g == 1.f && vMask.b == 0.f)
	{
		Out.vDiffuse.r = g_HairColor.r;
		Out.vDiffuse.g = g_HairColor.g;
		Out.vDiffuse.b = g_HairColor.b;
	}
	if (vMask.r == 0.f && vMask.g == 0.f && vMask.b == 1.f)
	{

	}

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader	= compile ps_5_0 PS_MAIN();
	}	// 0

	pass Outline
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
	}	// 1

	pass Shadow_Write
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW_WRITE();
	}	// 2

	pass Sigrid_HairMask
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HAIRMASK();
	}	// 3

	pass Reflect_Outline
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_REFLECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_OUTLINE();
	}	// 4

	pass Reflect_Sigrid_HairMask
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_REFLECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HAIRMASK();
	}	// 5
}