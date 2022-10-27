#include "Shader.h"

CShader::CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_Passes(rhs.m_Passes)
{
	for (auto& PassDesc : m_Passes)
		Safe_AddRef(PassDesc.pInputLayout);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{	
	_uint iHlslFlag = 0; 

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC TechniqueDesc;
	pTechnique->GetDesc(&TechniqueDesc);

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		PASSDESC PassDesc;
		ZeroMemory(&PassDesc, sizeof(PASSDESC));
		PassDesc.pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC Pass;
		PassDesc.pPass->GetDesc(&Pass);		

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, Pass.pIAInputSignature, Pass.IAInputSignatureSize, &PassDesc.pInputLayout)))
			return E_FAIL;

		m_Passes.push_back(PassDesc);
	}

	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_Passes.size())
		return E_FAIL;

	m_Passes[iPassIndex].pPass->Apply(0, m_pContext);
	m_pContext->IASetInputLayout(m_Passes[iPassIndex].pInputLayout);

	return S_OK; 
}

HRESULT CShader::Set_RawValue(const char* pConstantName, const void* pData, _uint iLength)
{
	if (m_pEffect == nullptr)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iLength);
}

HRESULT CShader::Set_MatrixArray(const char * pConstantName, const _float4x4* pData, _uint iNumMatrices)
{
	if (m_pEffect == nullptr)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (pVariable == nullptr)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	return pMatrixVariable->SetMatrixArray(&pData->_11, 0, iNumMatrices);
}

HRESULT CShader::Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	if (m_pEffect == nullptr)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pShaderResourceVariable =  pVariable->AsShaderResource();
	if (nullptr == pShaderResourceVariable)
		return E_FAIL;

	return pShaderResourceVariable->SetResource(pSRV);	
}

HRESULT CShader::Set_ShaderResourceViewArray(const char * pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTexture)
{
	if (m_pEffect == nullptr)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;	

	ID3DX11EffectShaderResourceVariable* pShaderResourceVariable = pVariable->AsShaderResource();
	if (nullptr == pShaderResourceVariable)
		return E_FAIL;

	return pShaderResourceVariable->SetResourceArray(ppSRV, 0, iNumTexture);	
}

CShader* CShader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
	{
		ERR_MSG(TEXT("Failed to Created : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void * pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& PassDesc : m_Passes)
		Safe_Release(PassDesc.pInputLayout);

	Safe_Release(m_pEffect);
}