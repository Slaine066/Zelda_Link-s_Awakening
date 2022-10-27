#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(class CModel* pModel, aiAnimation* pAIAnimation);
	void Invalidate_TransformationMatrix(_float fTimeDelta);

private:
	char				m_szName[MAX_PATH] = "";

	/* 현재 애니메이션 하나를 재생하는데 걸리는 총 시간. */
	_float				m_fDuration = 0.f;

	/* 1초에 재생되어야할 애니메이션의 속도. */	
	_float				m_fTickPerSecond = 0.f;
	_float				m_fCurrentTime = 0.f;

	/* Channel : 뼈. */

	/* 이 애니메이션을 구동하는데 필요한 뼈의 갯수. */
	_uint							m_iNumChannels = 0;

	vector<class CChannel*>			m_Channels;

	_bool							m_isFinished = false;
	_bool							m_isLoop = true;


public:
	static CAnimation* Create(class CModel* pModel, aiAnimation* pAIAnimation);
	virtual void Free() override;
};

END