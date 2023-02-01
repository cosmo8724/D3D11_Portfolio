#include "stdafx.h"
#include "..\Public\Cursor.h"
#include "GameInstance.h"

CCursor::CCursor(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CCursor::CCursor(const CCursor & rhs)
	: CUI(rhs)
{
}

HRESULT CCursor::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CCursor::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	::SetCursor(NULL);

	m_fSizeX = 32.f;
	m_fSizeY = 32.f;
	m_fX = 0.f;
	m_fY = 0.f;

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (nullptr != pArg)
		memcpy(&TransformDesc, pArg, sizeof(CTransform::TRANSFORMDESC));
	else
	{
		TransformDesc.dSpeedPerSec = 5.0;
		TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(45.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	return S_OK;
}

void CCursor::Tick(_double dTimeDelta)
{
	//__super::Tick(dTimeDelta);

	POINT		pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	m_fX = (_float)pt.x - g_iWinSizeX * 0.5f;
	m_fY = -(_float)pt.y + g_iWinSizeY * 0.5f;

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	/*if (m_MouseMoveX = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_X))
		m_fX += _double(dTimeDelta * m_pTransformCom->Get_TransformDesc().dSpeedPerSec * m_MouseMoveX * 0.1);

	if (m_MouseMoveY = CGameInstance::GetInstance()->Get_DIMouseMove(DIMS_Y))
		m_fY += _double(dTimeDelta * m_pTransformCom->Get_TransformDesc().dSpeedPerSec * m_MouseMoveY * 0.1);

	if (m_MouseMoveX || m_MouseMoveY)
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
*/
	m_Rect = { _long(((g_iWinSizeX * 0.5f) + m_fX) - m_fSizeY * 0.5f), _long(((g_iWinSizeY * 0.5f) - m_fY) - m_fSizeX * 0.5f),
		_long(((g_iWinSizeX * 0.5f) + m_fX) + m_fSizeY * 0.5f), _long(((g_iWinSizeY * 0.5f) - m_fY) + m_fSizeX * 0.5f) };

	XMStoreFloat4x4(&m_matView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
}

void CCursor::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCursor::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	m_pShaderCom->Begin(6);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CCursor::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Texture_Cursor", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CCursor::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld");
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture", (_uint)m_eState);

	return S_OK;
}

CCursor * CCursor::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CCursor*	pInstance = new CCursor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCursor::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CCursor*	pInstance = new CCursor(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCursor::Free()
{
	__super::Free();
}
