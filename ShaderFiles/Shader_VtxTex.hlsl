
#include "Client_Shader_Defines.hpp"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_DissolveTexture;

float g_EffectTimer;
float g_EffectLifespan;

/* Used by "UI_ItemSlot" class. */
bool g_Hovered;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

/* When the DrawIndexed() function gets called Vertices gets processed by VS_MAIN. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	/* 정점의 위치에 월드 뷰 투영행렬을 곱한다. 현재 정점은 ViewSpace에 존재하낟. */
	/* 투영행렬까지 곱하면 정점위치의 w에 뷰스페이스 상의 z를 보관한다. == Out.vPosition이 반드시 float4이어야하는 이유. */
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

/* 1. TriangleList로 그렸다면 정점 세개가 VS_MAIN의 수행을 모두 마친 이후. */
/* 2. w나누기 연산을 수행하여 투영스페이스로 간다. */

/* !!! 잠깐. 투영영역을 벗어난 정점에 대해서는 이후 과정을 생략한다. */

/* 3. 뷰포트스페이스(클라이언트영역 == 윈도우좌표)로의 변환을 수행한다. */
/* 4. 래스터라이즈를 수행한다. == 정점의 정보를 기반으로하여 픽셀을 생성한다. VS_OUT == PS_IN이 같은 이유. */

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_IN_SOFTEFFECT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	
	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	
	if (Out.vColor.a <= 0.1f)
		discard;

	if (g_Hovered)
		Out.vColor += .2;

	return Out;
}

PS_OUT PS_MAIN_SMOKE_EFFECT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.y;
	Out.vColor.yz = Out.vColor.x;

	if (Out.vColor.a != 0)
	{
		float fLerpAlpha = lerp(Out.vColor.a, 0, g_EffectTimer / g_EffectLifespan);
		Out.vColor.a = fLerpAlpha;
	}

	return Out;
}

PS_OUT PS_MAIN_HIT_FLASH_EFFECT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.y;
	Out.vColor.yz = Out.vColor.x;

	if (Out.vColor.a != 0)
	{
		float3 vFirstColor = float3(1.f, .4f, .2f);		/* Orange (when Alpha is 0) */
		float3 vSecondColor = float3(1.f, 1.f, .8f);	/* Yellow (when Alpha is 1) */

		float3 vLerpColor = lerp(vFirstColor, vSecondColor, Out.vColor.a);

		Out.vColor.xyz = vLerpColor;

		float startAfter = g_EffectLifespan / 2;
		if (g_EffectTimer >= startAfter)
		{
			float fLerpAlpha = lerp(Out.vColor.a, 0, (g_EffectTimer - startAfter) / (g_EffectLifespan - startAfter));
			Out.vColor.a = fLerpAlpha;
		}
	}

	return Out;
}

PS_OUT PS_MAIN_BOMB_FLASH_EFFECT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.g;
	Out.vColor.gb = Out.vColor.r;

	if (Out.vColor.a != 0)
	{
		float3 vFirstColor = float3(1.f, .4f, .2f);		/* Orange (when Alpha is 0) */
		float3 vSecondColor = float3(1.f, 1.f, .8f);	/* Yellow (when Alpha is 1) */

		float3 vLerpColor = lerp(vFirstColor, vSecondColor, Out.vColor.a);

		Out.vColor.rgb = vLerpColor;

		float startAfter = g_EffectLifespan / 2;
		if (g_EffectTimer >= startAfter)
		{
			float fLerpAlpha = lerp(Out.vColor.a, 0, (g_EffectTimer - startAfter) / (g_EffectLifespan - startAfter));
			Out.vColor.a = fLerpAlpha;
		}
	}

	return Out;
}

PS_OUT PS_MAIN_BOMB_EXPLOSION_EFFECT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.g;

	float4 dissolveColor = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);
	dissolveColor.a = dissolveColor.g;
	dissolveColor.gb = dissolveColor.r;

	if (Out.vColor.a != 0)
	{
		float3 vFirstColor = float3(1.f, .2f, 0.f);		
		float3 vSecondColor = float3(1.f, .81f, .44f);

		float3 vLerpColor = lerp(vFirstColor, vSecondColor, dissolveColor.r);
		Out.vColor.rgb = vLerpColor;

		float startAfter = g_EffectLifespan / 2;
		if (g_EffectTimer >= startAfter)
		{
			float fLerpAlpha = lerp(Out.vColor.a, 0, (g_EffectTimer - startAfter) / (g_EffectLifespan - startAfter));
			Out.vColor.a = fLerpAlpha;
		}
	}

	return Out;
}

PS_OUT PS_MAIN_GET_ITEM_EFFECT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.g;

	if (Out.vColor.a != 0)
	{
		float3 vFirstColor = float3(1.f, 1.f, 1.f);		
		float3 vSecondColor = float3(1.f, 1.f, .8f);

		float3 vLerpColor = lerp(vFirstColor, vSecondColor, Out.vColor.a);

		Out.vColor.rgb = vLerpColor;

		float startAfter = g_EffectLifespan / 2;
		if (g_EffectTimer >= startAfter)
		{
			float fLerpAlpha = lerp(Out.vColor.a, 0, (g_EffectTimer - startAfter) / (g_EffectLifespan - startAfter));
			Out.vColor.a = fLerpAlpha;
		}
	}

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

	pass UI
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Priority, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	
	pass UI_Blend
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Effect_Smoke
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_SMOKE_EFFECT();
	}

	pass Effect_HitFlash
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_HIT_FLASH_EFFECT();
	}

	pass Effect_BombFlash
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BOMB_FLASH_EFFECT();
	}

	pass Effect_BombExplosion
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BOMB_EXPLOSION_EFFECT();
	}

	pass Effect_GetItem
	{
		SetRasterizerState(RS_Default_NoCull);
		SetBlendState(BS_AlphaBlending, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_GET_ITEM_EFFECT();
	}
}