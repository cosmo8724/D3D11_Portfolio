#include "Shader_Define.h"

matrix			g_matWorld, g_matView, g_matProj;
texture2D		g_Texture;

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

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a == 0.f)
		discard;

	//Out.vColor = (vector)0;

	return Out;
}

PS_OUT	PS_MAIN_MONSTERDRINK_BLACK(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a == 0.f)
		discard;

	if (Out.vColor.r == 1.f && Out.vColor.g == 1.f && Out.vColor.b == 1.f)
		Out.vColor.rgb = float3(0.f, 0.f, 0.f);

	return Out;
}

PS_OUT	PS_MAIN_MONSTERDRINK_ORANGE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a == 0.f)
		discard;

	if (Out.vColor.r == 1.f && Out.vColor.g == 1.f && Out.vColor.b == 1.f)
		Out.vColor.rgb = float3(1.f, 0.5f, 0.f);

	return Out;
}

PS_OUT	PS_MAIN_MONSTERDRINK_PINK(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a == 0.f)
		discard;

	if (Out.vColor.r == 1.f && Out.vColor.g == 1.f && Out.vColor.b == 1.f)
		Out.vColor.rgb = float3(1.f, 0.3f, 0.5f);

	return Out;
}

PS_OUT	PS_MAIN_MONSTERDRINK_WHITE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a == 0.f)
		discard;

	if (Out.vColor.r == 1.f && Out.vColor.g == 1.f && Out.vColor.b == 1.f)
		Out.vColor.rgb = float3(0.85f, 0.85f, 0.85f);

	return Out;
}

technique11 DefaultTechnique
{
	pass Rect
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

	pass SkyBox
	{
		SetRasterizerState(RS_CW);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass UI_MonsterDrink_Black
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MONSTERDRINK_BLACK();
	}

	pass UI_MonsterDrink_Orange
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MONSTERDRINK_ORANGE();
	}

	pass UI_MonsterDrink_Pink
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MONSTERDRINK_PINK();
	}

	pass UI_MonsterDrink_White
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_MONSTERDRINK_WHITE();
	}

	pass UI_Cursor
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}