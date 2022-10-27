#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CShader final : public CComponent
{
public:
	typedef struct TagPassDesc
	{
		ID3D11InputLayout* pInputLayout = nullptr;
		ID3DX11EffectPass* pPass = nullptr;
	}PASSDESC;
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Begin(_uint iPassIndex = 0);
	HRESULT Set_RawValue(const char* pConstantName, const void* pData, _uint iLength);
	HRESULT Set_MatrixArray(const char * pConstantName, const _float4x4* pData, _uint iNumMatrices);
	HRESULT Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Set_ShaderResourceViewArray(const char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTexture);

private:	
	ID3DX11Effect* m_pEffect = nullptr;
	vector<PASSDESC> m_Passes;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END