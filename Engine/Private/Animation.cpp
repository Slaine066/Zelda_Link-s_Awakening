#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(CModel* pModel, aiAnimation * pAIAnimation)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		aiNodeAnim* pAIChannel = pAIAnimation->mChannels[i];
		CChannel* pChannel = CChannel::Create(pModel, pAIAnimation->mChannels[i]);

		if (!pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

/* Animate function returns a _bool which tells you if the Current Animation just finished.
Support both Loop and non-Loop Animations. */
_bool CAnimation::Animate(_float fTimeDelta, _bool bIsLoop)
{
	m_fCurrentTime += m_fTickPerSecond * fTimeDelta;

	// If the Animation is finished
	if (m_fCurrentTime >= m_fDuration)
	{
		m_fCurrentTime = 0.f;
		m_bIsFinished = true;
	}

	for (auto& pChannel : m_Channels)
	{
		// If the Animation is finished > Set CurrentAnimationIndex back to 0.
		if (m_bIsFinished)
		{
			pChannel->Reset();
			
			// In case of Not-Loop Animation "Invalidate_TransformationMatrix" since we don't want to start again.
			if (!bIsLoop)
				continue;
		}

		pChannel->Invalidate_TransformationMatrix(m_fCurrentTime);
	}

	// If the Animation is finished and it's a Loop Animation > Toggle back m_isFinished to true (since the Animation needs to start again).
	if (m_bIsFinished)
	{
		m_bIsFinished = false;
		return true;
	}

	return false;
}

CAnimation * CAnimation::Create( CModel* pModel, aiAnimation * pAIAnimation)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, pAIAnimation)))
	{
		ERR_MSG(TEXT("Failed to Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
