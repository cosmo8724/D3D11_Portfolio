matrix			g_matWorld, g_matView, g_matProj;
matrix			g_matSocket;
texture2D		g_DiffuseTexture;
texture2D		g_NormalTexture;

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
	float4		vTangent	: TANGENT;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	vector		vPosition = mul(float4(In.vPosition, 1.f), matWVP);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;
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
	Out.vTangent = (vector)0.f;

	return Out;
}


struct PS_IN
{
	float4		vPosition	: SV_POSITION;
	float4		vNormal	: NORMAL;
	float2		vTexUV	: TEXCOORD0;
	float4		vTangent	: TANGENT;
};

struct PS_OUT
{
	float4		vColor		: SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

	return Out;
}



technique11 DefaultTechinque
{
	pass Default
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	

	pass Socket
	{
		VertexShader = compile vs_5_0 VS_MAIN_SOCKET();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	
}

