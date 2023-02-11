#include "Shader_Define.h"

matrix			g_matWorld, g_matView, g_matProj;
matrix			g_matSocket;
matrix			g_matReflectView;
texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;
texture2D		g_MaskTexture;
texture2D		g_HPTexture;

float4			g_vClipPlane;

float			g_fFadeAlpha = 1.f;
bool			g_bHairMask;
vector			g_vHairColor;

float			g_fHPRatio;
float			g_fGlobalRatio;

int				g_WidthFrame, g_HeightFrame;
int				g_WidthCount, g_HeightCount;

struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal	: NORMAL;
	float2		vTexUV	: TEXCOORD0;
	float3		vTangent	: TANGENT;
};

struct VS_OUT
{
	float4		vPosition	: SV_POSITION;
	float4		vNormal	: NORMAL;
	float2		vTexUV	: TEXCOORD0;
	float4		vProjPos	: TEXCOORD1;
	float4		vTangent	: TANGENT;
};

//[clipplanes(g_vClipPlane)]
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = (vector)0.f;

	return Out;
}

VS_OUT VS_MAIN_SOCKET(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matVP;

	
	matVP = mul(g_matView, g_matProj);

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_matWorld);
	vPosition = mul(vPosition, g_matSocket);
	vPosition = mul(vPosition, matVP);

	vector		vNormal = mul(float4(In.vNormal, 0.f), g_matWorld);
	vNormal = mul(vNormal, g_matSocket);
	vNormal = normalize(vNormal);

	Out.vPosition = vPosition;
	Out.vNormal = vNormal;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = (vector)0.f;

	return Out;
}

[clipplanes(g_vClipPlane)]
VS_OUT VS_MAIN_REFLECT(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWRV, matWVP;

	matWRV = mul(g_matWorld, g_matReflectView);
	matWVP = mul(matWRV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = (vector)0.f;

	return Out;
}

VS_OUT VS_MAIN_REFLECT_SOCKET(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matRVP;

	
	matRVP = mul(g_matReflectView, g_matProj);

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_matWorld);
	vPosition = mul(vPosition, g_matSocket);
	vPosition = mul(vPosition, matRVP);

	vector		vNormal = mul(float4(In.vNormal, 0.f), g_matWorld);
	vNormal = mul(vNormal, g_matSocket);
	vNormal = normalize(vNormal);

	Out.vPosition = vPosition;
	Out.vNormal = vNormal;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = (vector)0.f;

	return Out;
}

struct PS_IN
{
	float4		vPosition	: SV_POSITION;
	float4		vNormal	: NORMAL;
	float2		vTexUV	: TEXCOORD0;
	float4		vProjPos	: TEXCOORD1;
	float4		vTangent	: TANGENT;
};

struct PS_OUT
{
	float4		vDiffuse	: SV_TARGET0;
	float4		vNormal	: SV_TARGET1;
	float4		vDepth	: SV_TARGET2;
};

struct PS_OUT_SHADOWDEPTH
{
	float4		vLightDepth	: SV_TARGET0;
};

struct PS_OUT_REFLECT
{
	float4		vDiffuse	: SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	if (0.1f > vDiffuse.a)
		discard;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MAIN_EFFECT_SIGRID_DASH(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vColor = vector(0.141f, 0.788f, 1.f, 0.3f * g_fFadeAlpha);

	Out.vDiffuse = vDiffuse * vColor;
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

PS_OUT PS_MAIN_FUZZYEARS(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	if (0.1f > vDiffuse.a)
		discard;

	Out.vDiffuse = vDiffuse;

	vector		vMask = g_MaskTexture.Sample(LinearSampler, In.vTexUV);

	// Hair Color
	if (g_bHairMask == true)
	{
		if (vMask.r == 1.f && vMask.g == 1.f && vMask.b == 0.f)
		{
			Out.vDiffuse.r = g_vHairColor.r;
			Out.vDiffuse.g = g_vHairColor.g;
			Out.vDiffuse.b = g_vHairColor.b;
		}
		if (vMask.r == 0.f && vMask.g == 0.f && vMask.b == 1.f)
		{
			//Out.vDiffuse.a = 0.f;
		}
	}
	else
		Out.vDiffuse *= g_vHairColor;

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MAIN_SIGRID_GROUNDSLAM(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2		vTexUV;
	vTexUV.x = (In.vTexUV.x + 1.f * g_WidthFrame) / g_WidthCount;
	vTexUV.y = (In.vTexUV.y + g_HeightFrame) / g_HeightCount;;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	vector		vMask = g_MaskTexture.Sample(LinearSampler, vTexUV);
	vector		vColor = vector(1.f, 0.f, 0.f, 1.f);
	
	//vMask.a = 1.f;
	//vDiffuse = vDiffuse * vMask;
	//vDiffuse.a = vMask.r;
	if (0.1f > vMask.r)
		discard;

	Out.vDiffuse = vMask;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f) * -1.f;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MAIN_EFFECT_SPRITE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2		vTexUV;
	vTexUV.x = (In.vTexUV.x + 1.f * g_WidthFrame) / g_WidthCount;
	vTexUV.y = (In.vTexUV.y + g_HeightFrame) / g_HeightCount;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vTexUV);
	vector		vColor = vector(1.f, 0.5f, 0.2f, 1.f * g_fFadeAlpha);

	vDiffuse = vDiffuse * vColor;
	Out.vDiffuse = vDiffuse;

	if (vDiffuse.a < 0.01f)
		discard;

	if (vDiffuse.r < 0.05f && vDiffuse.g < 0.05f && vDiffuse.b < 0.05f)
		discard;
	
	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f) * -1.f;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MAIN_LEVIATHAN_HP(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2		vUV = In.vTexUV;

	vUV.x *= g_fGlobalRatio;

	vector		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vUV);

	float2		vHPUV = In.vTexUV;

	//if (vHPUV.x > g_fHPRatio)
	//	vHPUV.x += g_fHPRatio;

	vector		vHP = g_HPTexture.Sample(LinearSampler, vHPUV);

	if (g_fHPRatio > 0.32 && g_fHPRatio < 0.58)
		vHP.rgb = float3(1.f, 0.6f, 0.1f);
	else if(g_fHPRatio < 0.32)
		vHP.rgb = float3(1.f, 0.f, 0.f);
	
	if (vDiffuse.a == 0.f && (1.f - vHPUV.x) < g_fHPRatio)
	{
		vDiffuse = vDiffuse * vHP;
		vDiffuse.a = 1.f;
	}

	if (vDiffuse.r == 0.f && vDiffuse.g == 0.f && vDiffuse.b == 0.f)
		discard;

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

	return Out;
}

technique11 DefaultTechinque
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	// 0

	pass Socket
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	// 1
	
	pass Effect_Sigrid_Dash
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_SIGRID_DASH();
	}	// 2

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
	}	// 3

	pass Shadow_Write_Socket
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SHADOW_WRITE();
	}	// 4

	pass Hat_FuzzyEars
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FUZZYEARS();
	}	// 5

	pass Effect_Sigrid_GroundSlam
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SIGRID_GROUNDSLAM();
	}	// 6

	pass Reflect
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_REFLECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	// 7

	pass Reflect_Socket
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_REFLECT_SOCKET();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	// 8

	pass Reflect_Socket_Hat_FuzzyEars
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_REFLECT_SOCKET();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_FUZZYEARS();
	}	// 9

	pass Effect_Sprite
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_One, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_SPRITE();
	}	// 10

	pass Reflect_Effect_Sprite
	{
		SetRasterizerState(RS_NonCulling);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_One, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_REFLECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_SPRITE();
	}	// 11

	pass Leviathan_HP
	{
		SetRasterizerState(RS_CW);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LEVIATHAN_HP();
	}	// 12
}

