
#include "Client_Shader_Defines.hpp"

matrix		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_SpecularTexture;

float g_EffectTimer;
float g_EffectLifespan;

/* Used in Tool. */
bool		g_IsSelected; 

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexUV : TEXCOORD0;
	float3 vTangent : TANGENT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = vNormal.xyz;
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = cross(Out.vNormal, Out.vTangent);

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
};

struct PS_OUT
{
	float4		vDiffuse : SV_TARGET0;
	float4		vNormal : SV_TARGET1;
	float4		vDepth : SV_TARGET2;
	float4		vSpecular : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal;

	vNormal = float3(vTextureNormal.x, vTextureNormal.y, sqrt(1 - vTextureNormal.x * vTextureNormal.x - vTextureNormal.y * vTextureNormal.y));

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vDiffuse.a <= 0.3f)
		discard;

	return Out;
}

PS_OUT PS_MAIN_EFFECT_HITRING(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal;

	vNormal = float3(vTextureNormal.x, vTextureNormal.y, sqrt(1 - vTextureNormal.x * vTextureNormal.x - vTextureNormal.y * vTextureNormal.y));

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.g;
	Out.vDiffuse.gb = Out.vDiffuse.r;

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vDiffuse.a != 0)
	{
		float3 vFirstColor = float3(1.f, .4f, .2f);		/* Orange (when Alpha is 0) */
		float3 vSecondColor = float3(1.f, 1.f, .8f);	/* Yellow (when Alpha is 1) */

		float3 vLerpColor = lerp(vFirstColor, vSecondColor, Out.vDiffuse.a);

		Out.vDiffuse.xyz = vLerpColor;

		/* Start disappearing after half duration. */
		float startAfter = g_EffectLifespan / 2;
		if (g_EffectTimer >= startAfter)
		{
			float fLerpAlpha = lerp(Out.vDiffuse.a, 0, (g_EffectTimer - startAfter) / (g_EffectLifespan - startAfter));
			Out.vDiffuse.a = fLerpAlpha;
		}
	}

	return Out;
}

PS_OUT PS_MAIN_EFFECT_HIT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal;

	vNormal = float3(vTextureNormal.x, vTextureNormal.y, sqrt(1 - vTextureNormal.x * vTextureNormal.x - vTextureNormal.y * vTextureNormal.y));

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.gba = Out.vDiffuse.r;

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	float3 vFirstColor = float3(1.f, .63f, .15f);
	float3 vSecondColor = float3(1.f, 1.f, .8f);

	float3 vLerpColor = lerp(vFirstColor, vSecondColor, Out.vDiffuse.a);

	Out.vDiffuse.xyz = vLerpColor;

	/* Start disappearing after half duration. */
	float startAfter = g_EffectLifespan / 2;
	if (g_EffectTimer >= startAfter)
	{
		float fLerpAlpha = lerp(Out.vDiffuse.a, 0, (g_EffectTimer - startAfter) / (g_EffectLifespan - startAfter));
		Out.vDiffuse.a = fLerpAlpha;
	}

	return Out;
}

PS_OUT PS_MAIN_EFFECT_GUARDRING(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal;

	vNormal = float3(vTextureNormal.x, vTextureNormal.y, sqrt(1 - vTextureNormal.x * vTextureNormal.x - vTextureNormal.y * vTextureNormal.y));

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.g;
	Out.vDiffuse.gb = Out.vDiffuse.r;

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	if (Out.vDiffuse.a != 0)
	{
		float3 vFirstColor = float3(.2f, .69f, 1.f);
		float3 vSecondColor = float3(.67f, .88f, 1.f);

		float3 vLerpColor = lerp(vFirstColor, vSecondColor, Out.vDiffuse.a);

		Out.vDiffuse.xyz = vLerpColor;

		/* Start disappearing after half duration. */
		float startAfter = g_EffectLifespan / 2;
		if (g_EffectTimer >= startAfter)
		{
			float fLerpAlpha = lerp(Out.vDiffuse.a, 0, (g_EffectTimer - startAfter) / (g_EffectLifespan - startAfter));
			Out.vDiffuse.a = fLerpAlpha;
		}
	}

	return Out;
}

PS_OUT PS_MAIN_EFFECT_GUARD(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal;

	vNormal = float3(vTextureNormal.x, vTextureNormal.y, sqrt(1 - vTextureNormal.x * vTextureNormal.x - vTextureNormal.y * vTextureNormal.y));

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.gba = Out.vDiffuse.r;

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	float3 vFirstColor = float3(.2f, .69f, 1.f);
	float3 vSecondColor = float3(.67f, .88f, 1.f);

	float3 vLerpColor = lerp(vFirstColor, vSecondColor, Out.vDiffuse.a);

	Out.vDiffuse.xyz = vLerpColor;

	/* Start disappearing after half duration. */
	float startAfter = g_EffectLifespan / 2;
	if (g_EffectTimer >= startAfter)
	{
		float fLerpAlpha = lerp(Out.vDiffuse.a, 0, (g_EffectTimer - startAfter) / (g_EffectLifespan - startAfter));
		Out.vDiffuse.a = fLerpAlpha;
	}

	return Out;
}

PS_OUT PS_MAIN_EFFECT_SHOCKWAVERING(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal;

	vNormal = float3(vTextureNormal.x, vTextureNormal.y, sqrt(1 - vTextureNormal.x * vTextureNormal.x - vTextureNormal.y * vTextureNormal.y));

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;
	Out.vDiffuse.rgb = float3(1.f, 1.f, 1.f);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	/* Start disappearing after half duration. */
	float startAfter = g_EffectLifespan / 2;
	if (g_EffectTimer >= startAfter)
	{
		float fLerpAlpha = lerp(Out.vDiffuse.a, 0, (g_EffectTimer - startAfter) / (g_EffectLifespan - startAfter));
		Out.vDiffuse.a = fLerpAlpha;
	}

	return Out;
}

PS_OUT PS_MAIN_EFFECT_SHOCKWAVE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal;

	vNormal = float3(vTextureNormal.x, vTextureNormal.y, sqrt(1 - vTextureNormal.x * vTextureNormal.x - vTextureNormal.y * vTextureNormal.y));

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.r;
	Out.vDiffuse.rgb = float3(1.f, 1.f, 1.f);

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);

	/* Start disappearing after half duration. */
	float startAfter = g_EffectLifespan / 2;
	if (g_EffectTimer >= startAfter)
	{
		float fLerpAlpha = lerp(Out.vDiffuse.a, 0, (g_EffectTimer - startAfter) / (g_EffectLifespan - startAfter));
		Out.vDiffuse.a = fLerpAlpha;
	}

	return Out;
}

PS_OUT PS_MAIN_EFFECT_STAR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vTextureNormal = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
	float3 vNormal;

	vNormal = float3(vTextureNormal.x, vTextureNormal.y, sqrt(1 - vTextureNormal.x * vTextureNormal.x - vTextureNormal.y * vTextureNormal.y));

	float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
	vNormal = mul(vNormal, WorldMatrix);

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vDiffuse.a = Out.vDiffuse.g;

	Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);

	if (Out.vDiffuse.a != 0)
	{
		float3 vFirstColor = float3(1.f, .5f, .1f);		/* Orange (when Alpha is 0) */
		float3 vSecondColor = float3(1.f, 1.f, .35f);	/* Yellow (when Alpha is 1) */

		float3 vLerpColor = lerp(vFirstColor, vSecondColor, Out.vDiffuse.r);
		Out.vDiffuse.xyz = vLerpColor;

		if (g_EffectTimer < g_EffectLifespan / 8)
		{
			float fInterpFactor = g_EffectTimer / (g_EffectLifespan / 8);
			float fLerpAlpha = lerp(0, Out.vDiffuse.a, fInterpFactor);
			Out.vDiffuse.a = fLerpAlpha;
		}
		else if (g_EffectTimer > g_EffectLifespan - g_EffectLifespan / 8)
		{
			float fInterpFactor = (g_EffectTimer - (g_EffectLifespan - g_EffectLifespan / 8)) / (g_EffectLifespan - (g_EffectLifespan - g_EffectLifespan / 8));
			float fLerpAlpha = lerp(Out.vDiffuse.a, 0, fInterpFactor);
			Out.vDiffuse.a = fLerpAlpha;
		}
	}
	else
		discard;

	return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Effect_HitRing
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_HITRING();
	}
	
	pass Effect_Swish
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_HITRING(); /* TODO: .. */
	}

	pass Effect_SwordSlash
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_HITRING(); /* TODO: .. */
	}

	pass Effect_Hit
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_HIT();
	}

	pass Effect_GuardRing
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_GUARDRING();
	}

	pass Effect_Guard
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_GUARD();
	}

	pass Effect_ShockwaveRing
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_SHOCKWAVERING();
	}

	pass Effect_Shockwave
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_SHOCKWAVE();
	}

	pass Effect_Star
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_EFFECT_STAR();
	}
}