#include "Shader_Define.h"

matrix			g_matWorld, g_matView, g_matProj;
matrix			g_matReflectView;
texture2D		g_Texture;

float4			g_vClipPlane;
float4			g_vDiffuseColor;

int				g_WidthFrame, g_HeightFrame;
int				g_WidthCount, g_HeightCount;

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

[clipplanes(g_vClipPlane)]
VS_OUT	VS_MAIN_REFLECT(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	matrix		matWRV, matWRVP;
    
	matWRV = mul(g_matWorld, g_matReflectView);
	matWRVP = mul(matWRV, g_matProj);
	
	Out.vPosition = mul(float4(In.vPosition, 1.f), matWRVP);
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

PS_OUT	PS_MAIN_LIGHT(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	if (Out.vColor.a == 0.f)
		discard;

	Out.vColor = Out.vColor * g_vDiffuseColor;

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

PS_OUT	PS_MAIN_SPRITE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float2		vTexUV;
	vTexUV.x = (In.vTexUV.x + 1.f * g_WidthFrame) / g_WidthCount;
	vTexUV.y = (In.vTexUV.y + g_HeightFrame) / g_HeightCount;;

	vector		vColor = vector(0.3f, 0.3f, 1.f, 1.f);
	Out.vColor = g_Texture.Sample(LinearSampler, vTexUV);

	Out.vColor = Out.vColor * vColor;

	//if (Out.vColor.a < 0.1f)
		//discard;

	if (Out.vColor.r < 0.15f || Out.vColor.g < 0.15f || Out.vColor.b < 0.15f)
		discard;

	return Out;
}

PS_OUT	PS_MAIN_SPRITE_REVERSE(PS_IN In)
{
	PS_OUT	Out = (PS_OUT)0;

	float2		vTexUV;
	vTexUV.x = (In.vTexUV.x + 1.f * g_WidthFrame) / g_WidthCount;
	vTexUV.y = (In.vTexUV.y + g_HeightFrame) / g_HeightCount;;

	vector		vColor = vector(0.3f, 0.3f, 1.f, 1.f);
	Out.vColor = g_Texture.Sample(LinearSampler, vTexUV);

	if(Out.vColor.r > 0.3f || Out.vColor.g > 0.3f || Out.vColor.b > 0.3f)
		discard;

	Out.vColor = Out.vColor + vColor;

	//if (Out.vColor.r > 0.3f && Out.vColor.g > 0.3f && Out.vColor.b >= 1.f)
	//	discard;

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
	}	// 0

	pass SkyBox
	{
		SetRasterizerState(RS_CW);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LIGHT();
	}	// 1

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
	}	// 2

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
	}	// 3

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
	}	// 4

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
	}	// 5

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
	}	// 6

	pass Sprite_Image
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPRITE();
	}	// 7

	pass Sprite_Image_Color_Reverse
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SPRITE_REVERSE();
	}	// 8

	pass Reflect
	{
		SetRasterizerState(RS_CW);
		SetDepthStencilState(DS_ZEnable_ZWriteEnable_FALSE, 0);
		SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_REFLECT();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	// 9
}