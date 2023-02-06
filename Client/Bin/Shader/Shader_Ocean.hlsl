#include "Shader_Define.h"

matrix			g_matWorld, g_matView, g_matProj;
matrix          g_matReflectionView;
matrix          g_matWinDirection;

vector			g_vLightDir;
float4          g_vCamPosition;

float3          g_vWaterColor;
float           g_fWaveLength = 0.1f;
float           g_fWaveHeight = 0.4f;

float           g_fTime;
float           g_fWindForce = 20.f;

int             g_iFresnelMode = 0;

float           g_fSpecularPerturb = 4.f;
float           g_fSpecularPower = 364.f;

float           g_fDullBlendFactor = 0.3f; // 0.f ~ 1.f

texture2D       g_DiffuseTexture;
texture2D       g_ReflectionTexture;
texture2D       g_RefractionTexture;
texture2D       g_WaterBumpTexture;

struct VS_IN
{
    float3  vPosition   : POSITION;
    float2  vPSize      : PSIZE;
};

struct VS_OUT
{
    float3  vPosition   : Position;
    float2  vPSize      : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT  Out = (VS_OUT)0;

    Out.vPosition = In.vPosition;
    Out.vPSize = In.vPSize;

    return Out;
}

struct GS_IN
{
    float3  vPosition   : Position;
    float2  vPSize      : PSIZE;
};

struct GS_OUT
{
    float4  vPosition   : SV_Position;
    float4  vReflectionMapSamplingPos : TEXCOORD0;      // 반사 맵 샘플링 포지션
    float2  vBumpMapSamplingPos : TEXCOORD1;            // 노말 맵 샘플링 포지션
    float4  vRefractionMapSamplingPos   : TEXCOORD2;    // 굴절 맵 샘플링 포지션
    float4  vWorldPosition  : TEXCOORD3;                    // CamPos로부터 방향 계산에 필요
    float4  vProjPos        : TEXCOORD4;
    float2  vTexUV      : TEXCOORD5;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
    GS_OUT  Out[4];

    float3  vRight = float3(1.f, 0.f, 0.f);
    float3  vUp = float3(0.f, 1.f, 0.f);
    float3  vLook = float3(0.f, 0.f, 1.f);

    matrix  matWV, matWVP, matWRV, matWRVP;

    float4  AbsoluteTexCoord;
    float4  RotatedTexCoord;
    float2  MoveVector;

    matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);
    matWRV = mul(g_matWorld, g_matReflectionView);
    matWRVP = mul(matWRV, g_matProj);

    float3  vPosition;

    vPosition = In[0].vPosition - vRight + vLook;
    Out[0].vPosition = mul(vector(vPosition, 1.f), matWVP);
    Out[0].vTexUV = float2(0.f, 0.f);
    Out[0].vWorldPosition = mul(vector(vPosition, 1.f), g_matWorld);
    Out[0].vProjPos = Out[0].vPosition;

    AbsoluteTexCoord = float4(Out[0].vTexUV, 0.f, 1.f);
    RotatedTexCoord = mul(AbsoluteTexCoord, g_matWinDirection);
    MoveVector = float2(1.f, -0.5f);

    Out[0].vReflectionMapSamplingPos = mul(vector(vPosition, 1.f), matWRVP);
    Out[0].vRefractionMapSamplingPos = mul(vector(vPosition, 1.f), matWVP);
    Out[0].vBumpMapSamplingPos = RotatedTexCoord.xy / g_fWaveLength + g_fTime * g_fWindForce * MoveVector.xy;

    vPosition = In[0].vPosition + vRight + vLook;
	Out[1].vPosition = mul(vector(vPosition, 1.f), matWVP);
	Out[1].vTexUV = float2(1.f, 0.f);

    Out[1].vWorldPosition = mul(vector(vPosition, 1.f), g_matWorld);
    Out[1].vProjPos = Out[1].vPosition;

    AbsoluteTexCoord = float4(Out[1].vTexUV, 0.f, 1.f);
    RotatedTexCoord = mul(AbsoluteTexCoord, g_matWinDirection);
    MoveVector = float2(1.f, -0.5f);

    Out[1].vReflectionMapSamplingPos = mul(vector(vPosition, 1.f), matWRVP);
    Out[1].vRefractionMapSamplingPos = mul(vector(vPosition, 1.f), matWVP);
    Out[1].vBumpMapSamplingPos = RotatedTexCoord.xy / g_fWaveLength + g_fTime * g_fWindForce * MoveVector.xy;

	vPosition = In[0].vPosition + vRight - vLook;
	Out[2].vPosition = mul(vector(vPosition, 1.f), matWVP);
	Out[2].vTexUV = float2(1.f, 1.f);

    Out[2].vWorldPosition = mul(vector(vPosition, 1.f), g_matWorld);
    Out[2].vProjPos = Out[2].vPosition;

    AbsoluteTexCoord = float4(Out[2].vTexUV, 0.f, 1.f);
    RotatedTexCoord = mul(AbsoluteTexCoord, g_matWinDirection);
    MoveVector = float2(1.f, -0.5f);

    Out[2].vReflectionMapSamplingPos = mul(vector(vPosition, 1.f), matWRVP);
    Out[2].vRefractionMapSamplingPos = mul(vector(vPosition, 1.f), matWVP);
    Out[2].vBumpMapSamplingPos = RotatedTexCoord.xy / g_fWaveLength + g_fTime * g_fWindForce * MoveVector.xy;

	vPosition = In[0].vPosition - vRight - vLook;
	Out[3].vPosition = mul(vector(vPosition, 1.f), matWVP);
	Out[3].vTexUV = float2(0.f, 1.f);

    Out[3].vWorldPosition = mul(vector(vPosition, 1.f), g_matWorld);
    Out[3].vProjPos = Out[3].vPosition;

    AbsoluteTexCoord = float4(Out[3].vTexUV, 0.f, 1.f);
    RotatedTexCoord = mul(AbsoluteTexCoord, g_matWinDirection);
    MoveVector = float2(1.f, -0.5f);

    Out[3].vReflectionMapSamplingPos = mul(vector(vPosition, 1.f), matWRVP);
    Out[3].vRefractionMapSamplingPos = mul(vector(vPosition, 1.f), matWVP);
    Out[3].vBumpMapSamplingPos = RotatedTexCoord.xy / g_fWaveLength + g_fTime * g_fWindForce * MoveVector.xy;

    Vertices.Append(Out[0]);
	Vertices.Append(Out[1]);
	Vertices.Append(Out[2]);
	Vertices.RestartStrip();

	Vertices.Append(Out[0]);
	Vertices.Append(Out[2]);
	Vertices.Append(Out[3]);
	Vertices.RestartStrip();
}

struct PS_IN
{
    float4  vPosition   : SV_Position;
    float4  vReflectionMapSamplingPos : TEXCOORD0;      // 반사 맵 샘플링 포지션
    float2  vBumpMapSamplingPos : TEXCOORD1;            // 노말 맵 샘플링 포지션
    float4  vRefractionMapSamplingPos   : TEXCOORD2;    // 굴절 맵 샘플링 포지션
    float4  vWorldPosition  : TEXCOORD3;                    // CamPos로부터 방향 계산에 필요
    float4  vProjPos        : TEXCOORD4;
    float2  vTexUV      : TEXCOORD5;
};

struct PS_OUT
{
    float4  vDiffuse  : SV_TARGET0;
    float4  vNormal   : SV_TARGET1;
    float4  vDepth    : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT  Out = (PS_OUT)0;

    /* Sampling Texture */
    float2  vProjectedTexCoord;
    vProjectedTexCoord.x = In.vReflectionMapSamplingPos.x / In.vReflectionMapSamplingPos.w / 2.f + 0.5f;
    vProjectedTexCoord.y = -In.vReflectionMapSamplingPos.y / In.vReflectionMapSamplingPos.w / 2.f + 0.5f;

    float4  vBumpColor = g_WaterBumpTexture.Sample(WaterBumpSampler, In.vBumpMapSamplingPos);

    float2  vPerturbation = g_fWaveHeight * (vBumpColor.rg - 0.5f);

    float2  vPerturvatedTexCoord = vProjectedTexCoord + vPerturbation;
    float4  vReflectiveColor = g_ReflectionTexture.Sample(WaterBumpSampler, vPerturvatedTexCoord);

    //if (0.1f > vReflectiveColor.a)
	//	discard;

    vector  vFoamColor = g_DiffuseTexture.Sample(LinearSampler, (In.vBumpMapSamplingPos + vPerturvatedTexCoord) * 1.5f);

    //if(vFoamColor.a < 0.1f)
    //    discard;

    vector  vWaterColor = vector(g_vWaterColor, 1.f - vFoamColor.a);

    //if (vFoamColor.a < 0.2f)
    vFoamColor = vFoamColor * vWaterColor;
    vFoamColor = vFoamColor * 0.5f;

    vReflectiveColor.rgb = vReflectiveColor.rgb + vFoamColor.rgb;

    float2  vProjectedRefractionTexCoord;
    vProjectedRefractionTexCoord.x = In.vRefractionMapSamplingPos.x / In.vRefractionMapSamplingPos.w / 2.f + 0.5f;
    vProjectedRefractionTexCoord.y = -In.vRefractionMapSamplingPos.y / In.vRefractionMapSamplingPos.w / 2.f + 0.5f;

    float2  vPerturbatedRefrationTexCoord = vProjectedRefractionTexCoord + vPerturbation;
    float4  vRefractiveColor = g_RefractionTexture.Sample(WaterBumpSampler, vPerturbatedRefrationTexCoord);

    float3  vEyeDirection = normalize(g_vCamPosition - In.vWorldPosition);
    float3  vNormal = float3(0.f, 1.f, 0.f);

    /* FresnelTerm */
    float   fFresnelTerm = 0.f;

    if (g_iFresnelMode == 0)
        fFresnelTerm = 0.8f + 0.97f * pow((1 - dot(vEyeDirection, vNormal)), 5.f);
	else if (g_iFresnelMode == 1)
        fFresnelTerm = 1 - dot(vEyeDirection, vNormal) * 1.3f;

    fFresnelTerm = fFresnelTerm < 0 ? 0 : fFresnelTerm;
    fFresnelTerm = fFresnelTerm > 1 ? 1 : fFresnelTerm;

    float4  vCombinedColor = vRefractiveColor * (1.f - fFresnelTerm) + vReflectiveColor * fFresnelTerm;

    /* Water Coloring */
    float4  vDullColor = float4(0.1f, 0.1f, 0.2f, 1.f);
    float   fDullBlendFactor = g_fDullBlendFactor;

    Out.vDiffuse = (fDullBlendFactor * vDullColor + (1.f - fDullBlendFactor) * vCombinedColor);
    Out.vNormal = (float4)0;//float4(vNormal, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

    /* Specular */
    float4  vSpecular;

    float3  vLightDir = g_vLightDir.xyz;

    float3  vHalfVec = normalize(vEyeDirection + vLightDir + float3(vPerturbation.x * g_fSpecularPerturb, vPerturbation.y * g_fSpecularPerturb, 0.f));
    float3  vTemp = (float3)0;

    vTemp.x = pow(abs(dot(vHalfVec, vNormal)), g_fSpecularPower);

    vSpecular = float4(0.98f, 0.97f, 0.7f, 0.6f);
    vSpecular = vSpecular * vTemp.x;
    vSpecular = float4(vSpecular.x * vSpecular.w, vSpecular.y * vSpecular.w, vSpecular.z * vSpecular.w, 0.f);
    

    Out.vDiffuse = Out.vDiffuse + vSpecular;

    return Out;
}

technique11 DefaultTechnique
{
    pass ObjectRayOrigin
    {
        SetRasterizerState(RS_Default);
		SetDepthStencilState(DS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader	= compile ps_5_0 PS_MAIN();
    }
}