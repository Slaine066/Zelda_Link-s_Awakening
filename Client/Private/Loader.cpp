#include "stdafx.h"

#include "Loader.h"
#include "GameInstance.h"
#include "Navigation.h"
#include "Camera_Player.h"
#include "Camera_Dungeon.h"
#include "Camera_Static.h"
#include "TriggerBox_Dynamic.h"
#include "Sky.h"
#include "StaticObject.h"
#include "Effect.h"
#include "Treasure.h"
#include "Player.h"
#include "Tarin.h"
#include "Marin.h"
#include "Owl.h"
#include "Bed.h"
#include "Sword.h"
#include "MoriblinSword.h"
#include "MoriblinSpear.h"
#include "Bossblin.h"
#include "DungeonDoor.h"
#include "Weapon.h"
#include "Projectile.h"
#include "Item.h"
#include "UI.h"
#include "UI_Heart.h"
#include "UI_Inventory.h"
#include "UI_ItemSlot.h"
#include "UI_ItemChip.h"
#include "UI_InventoryItem.h"
#include "UI_Chat.h"
#include "UI_ScreenFade.h"
#include "UI_RupeesFont.h"
#include "UI_LevelName.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY Thread_Main(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForStaticLevel();
		break;
	case LEVEL_FIELD:
		pLoader->Loading_ForField();
		break;
	case LEVEL_MORIBLINCAVE:
		pLoader->Loading_ForMoriblinCave();
		break;
	case LEVEL_BOTTLEGROTTO:
		pLoader->Loading_ForBottleGrotto();
		break;
	case LEVEL_MARINHOUSE:
		pLoader->Loading_ForMarinHouse();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	CoInitializeEx(nullptr, 0);

	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
		
	return S_OK;
}

HRESULT CLoader::Loading_ForStaticLevel()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (!pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

#pragma region Loading_Textures
	lstrcpy(m_szLoadingText, TEXT("Loading Textures.."));

	/*For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/SkyBox/Sky_%d.dds"), 3))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_ScreenFade */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ScreenFade"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/ScreenFade_%02d.png"), 2))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_LevelName */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelName"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/LevelName_%02d.png"), 2))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Number */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Number_%02d.png"), 10))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Heart */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Heart"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Heart_%d.png"), 3))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_ItemSlot */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemSlot"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/ItemSlot_%02d.png"), 7))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_ItemSlot_Number */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemChip"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/ItemChip.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Inventory */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Inventory"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Inventory.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_InteractButton_Open */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InteractButton_Open"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Interact_Button_Open.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_InteractButton_Talk */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InteractButton_Talk"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Interact_Button_Talk.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_InteractButton_Use */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InteractButton_Use"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Interact_Button_Use.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_InteractButton_Take */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InteractButton_Take"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Interact_Button_Take.png"), 1))))
		return E_FAIL;
	
	/* Item Icons */
	/*For.Prototype_Component_Texture_Rupee */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Rupee"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/Rupee.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Item_Bomb */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Bomb"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/ItemBomb.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Item_RocsFeather */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_RocsFeather"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/ItemRocsFeather.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Item_Bow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Bow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/ItemBow.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Item_Ocarina */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Ocarina"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/ItemOcarina.png"), 1))))
		return E_FAIL;

	/* Equipment Icons */
	/*For.Prototype_Component_Texture_Equipment_Cloth */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment_Cloth"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/EqCloth_%02d.png"), 3))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Equipment_Flippers */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment_Flippers"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/EqFlippers_%02d.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Equipment_PegasusBoots */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment_PegasusBoots"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/EqPegasusBoots_%02d.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Equipment_PowerBracelet */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment_PowerBracelet"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/EqPowerBracelet_%02d.png"), 2))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Equipment_Shield */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment_Shield"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/EqShield_%02d.png"), 2))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Equipment_Sword */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment_Sword"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/UI/EqSword_%02d.png"), 2))))
		return E_FAIL;

	/*For.Prototye_Component_Texture_Dissolve */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effects/Dissolve_%02d.dds"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Smoke */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effects/Smoke.dds"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Hit_Flash */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Flash"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Effects/Hit_Flash_%02d.dds"), 6))))
		return E_FAIL;

	/* Chat Dialogs */
	/*For.Prototype_Component_Texture_Chat_Marin_Line_1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chat_Marin_Line_1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Chat/Marin/Line_1/Chat_%02d.png"), 4))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Chat_Tarin_Line_1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chat_Tarin_Line_1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Chat/Tarin/Line_1/Chat_%02d.png"), 3))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Chat_Tarin_Line_2 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chat_Tarin_Line_2"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Chat/Tarin/Line_2/Chat_%02d.png"), 2))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Chat_Owl_Line_1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chat_Owl_Line_1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Chat/Owl/Line_1/Chat_%02d.png"), 5))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Chat_Bossblin_Line_1 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chat_Bossblin_Line_1"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Chat/Bossblin/Chat_%02d.png"), 2))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Chat_Get_Item_Shield */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chat_Get_Item_Shield"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Chat/Get_Item/Get_Item_Shield.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Chat_Get_Item_Sword */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chat_Get_Item_Sword"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Chat/Get_Item/Get_Item_Sword.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Chat_Get_Item_RocsFeather */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chat_Get_Item_RocsFeather"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Chat/Get_Item/Get_Item_RocsFeather.png"), 1))))
		return E_FAIL;
	/*For.Prototype_Component_Texture_Chat_Get_Item_Ocarina */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chat_Get_Item_Ocarina"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Textures/Chat/Get_Item/Get_Item_Ocarina.png"), 1))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Textures

#pragma region Loading_Components
	lstrcpy(m_szLoadingText, TEXT("Loading Components.."));
	
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Transform */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Components

#pragma region Loading_Models
	lstrcpy(m_szLoadingText, TEXT("Loading Models.."));

	_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/*For.Prototype_Component_Model_Link*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Link"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Player_Link/Link.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_MoriblinSword*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MoriblinSword"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Enemy_MoriblinSword/MoriblinSword.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_MoriblinSpear*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MoriblinSpear"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Enemy_MoriblinSpear/MoriblinSpear.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Spear*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Spear"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Object_MoriblinSpear/Spear.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_BossblinSpear*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BossblinSpear"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Object_BossblinSpear/BossblinSpear.fbx", false))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Treasure*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Treasure"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Object_Treasure/Treasure.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Shield*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Shield"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Item_Shield/Shield.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Sword*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sword"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Item_Sword/Sword.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_RupeeGreen*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RupeeGreen"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Item_RupeeGreen/RupeeGreen.fbx", false))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Bomb*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bomb"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Item_Bomb/Bomb.fbx", false))))
		return E_FAIL;
	/*For.Prototype_Component_Model_RocsFeather*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RocsFeather"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Item_RocsFeather/RocsFeather.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Ocarina*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ocarina"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Item_Ocarina/Ocarina.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_HitRing*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_HitRing"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Effects/HitRing/HitRing.fbx", false))))
		return E_FAIL;
	/*For.Prototype_Component_Model_HitFlash*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_HitFlash"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Effects/HitFlash/HitFlash.fbx", false))))
		return E_FAIL;
	/*For.Prototype_Component_Model_GuardFlash*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_GuardFlash"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Effects/GuardFlash/GuardFlash.fbx", false, XMMatrixMultiply(XMMatrixRotationZ(XMConvertToRadians(-90.0f)), XMMatrixRotationY(XMConvertToRadians(-90.0f)))))))
		return E_FAIL;
	/*For.Prototype_Component_Model_SwordSlash*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SwordSlash"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Effects/SwordSlash/SwordSlash.fbx", false))))
		return E_FAIL;
	/*For.Prototype_Component_Model_ShockwaveRing*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShockwaveRing"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Effects/ShockwaveRing/ShockwaveRing.fbx", false))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Shockwave*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Shockwave"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Effects/Shockwave/Shockwave.fbx", false, XMMatrixMultiply(XMMatrixRotationZ(XMConvertToRadians(-90.0f)), XMMatrixRotationY(XMConvertToRadians(-90.0f)))))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Star*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Star"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Effects/Star/Star.fbx", false, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Field_** */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_2A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_2A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_2B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_2B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_2C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_2C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_2D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_2D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_2E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_2E.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_2F"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_2F.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_2G"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_2G.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_2H"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_2H.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_3A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_3A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_3B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_3B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_3C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_3C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_3D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_3D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_3E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_3E.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_3F"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_3F.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_3G"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_3G.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_3H"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_3H.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_4A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_4A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_4B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_4B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_4C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_4C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_4D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_4D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_4E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_4E.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_4F"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_4F.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_4G"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_4G.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_4H"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_4H.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_5A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_5A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_5B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_5B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_5C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_5C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_5D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_5D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_5E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_5E.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_5F"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_5F.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_5G"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_5G.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_5H"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_5H.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_6A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_6A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_6B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_6B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_6C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_6C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_6D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_6D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_6E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_6E.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_6F"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_6F.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_6G"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_6G.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_6H"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_6H.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_7A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_7A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_7B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_7B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_7C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_7C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_7D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_7D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_7E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_7E.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_7F"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_7F.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_7G"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_7G.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_7H"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_7H.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_8A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_8A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_8B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_8B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_8C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_8C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_8D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_8D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_8E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_8E.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_8F"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_8F.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_8G"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_8G.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_8H"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_8H.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_9A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_9A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_9B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_9B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_9C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_9C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_9D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_9D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_9E"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_9E.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_9F"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_9F.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_9G"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_9G.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_9H"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_9H.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_10A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_10A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_10B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_10B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_10C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_10C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_10D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_10D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_11A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_11A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_11B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_11B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_11C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_11C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_11D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_11D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_12A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_12A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_12B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_12B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_12C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_12C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_12D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_12D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_13A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_13A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_13B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_13B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_13C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_13C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_13D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_13D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_14A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_14A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_14B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_14B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_14C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_14C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_14D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_14D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_15A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_15A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_15B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_15B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_15C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_15C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_15D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_15D.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_16A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_16A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_16B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_16B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_16C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_16C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Field_16D"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Field/Field_16D.fbx"))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Models

#pragma region Loading_Shaders
	lstrcpy(m_szLoadingText, TEXT("Loading Shaders.."));	

	/* For.Prototype_Component_Shader_VtxTex*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Shaderfiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Shaderfiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Shaderfiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxCubeTexture */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCubeTexture"), CShader::Create(m_pDevice, m_pContext, TEXT("../../Shaderfiles/Shader_VtxCubeTexture.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Shaders

	if (FAILED(Load_GameObject_Prototypes()))
		return E_FAIL;

	m_isFinished = true;
	lstrcpy(m_szLoadingText, TEXT("Loading Completed."));
	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForField()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (pGameInstance == nullptr)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

#pragma region Loading_Textures
	lstrcpy(m_szLoadingText, TEXT("Loading Textures.."));

	// >
	// .. Add Above ..
#pragma endregion Loading_Textures

#pragma region Loading_Components
	lstrcpy(m_szLoadingText, TEXT("Loading Components.."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NavigationData/Field.dat")))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Components

#pragma region Loading_Models
	lstrcpy(m_szLoadingText, TEXT("Loading Models.."));

	_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/*For.Prototype_Component_Model_Owl*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_FIELD, TEXT("Prototype_Component_Model_Owl"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Npc_Owl/Owl.fbx", false, PivotMatrix))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Models

#pragma region Loading_Shaders
	lstrcpy(m_szLoadingText, TEXT("Loading_Shaders.."));

	// >
	// .. Add Above ..
#pragma endregion Loading_Shaders

	m_isFinished = true;
	lstrcpy(m_szLoadingText, TEXT("Loading Completed."));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForMoriblinCave()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (pGameInstance == nullptr)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

#pragma region Loading_Textures
	lstrcpy(m_szLoadingText, TEXT("Loading Textures.."));

	// >
	// .. Add Above ..
#pragma endregion Loading_Textures

#pragma region Loading_Components
	lstrcpy(m_szLoadingText, TEXT("Loading Components.."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NavigationData/MoriblinCave.dat")))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Components

#pragma region Loading_Models
	lstrcpy(m_szLoadingText, TEXT("Loading Models.."));

	_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/*For.Prototype_Component_Model_Bossblin*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Model_Bossblin"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Enemy_Bossblin/Bossblin.fbx", false, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_MoriblinCave_** */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Model_MoriblinCave_1A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/MoriblinCave/MoriblinCave_1A.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Model_MoriblinCave_1B"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/MoriblinCave/MoriblinCave_1B.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Model_MoriblinCave_1C"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/MoriblinCave/MoriblinCave_1C.fbx"))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Model_MoriblinCave_2A"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/MoriblinCave/MoriblinCave_2A.fbx"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_MoriblinCaveRock */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Model_CaveRock"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/Object_MoriblinCaveRock/CaveRock.fbx", false))))
		return E_FAIL;
	/* For.Prototype_Component_Model_DungeonDoor */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Model_DungeonDoor"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Object_DungeonDoor/DungeonDoor.fbx", false, PivotMatrix))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Models

#pragma region Loading_Shaders
	lstrcpy(m_szLoadingText, TEXT("Loading_Shaders.."));

	// >
	// .. Add Above ..
#pragma endregion Loading_Shaders

	m_isFinished = true;
	lstrcpy(m_szLoadingText, TEXT("Loading Completed."));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForBottleGrotto()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (pGameInstance == nullptr)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

#pragma region Loading_Textures
	lstrcpy(m_szLoadingText, TEXT("Loading Textures.."));

	// >
	// .. Add Above ..
#pragma endregion Loading_Textures

#pragma region Loading_Components
	lstrcpy(m_szLoadingText, TEXT("Loading Components.."));

	// >
	// .. Add Above ..
#pragma endregion Loading_Components

#pragma region Loading_Models
	lstrcpy(m_szLoadingText, TEXT("Loading Models.."));

	_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	// >
	// .. Add Above ..
#pragma endregion Loading_Models

#pragma region Loading_Shaders
	lstrcpy(m_szLoadingText, TEXT("Loading_Shaders.."));

	// >
	// .. Add Above ..
#pragma endregion Loading_Shaders

	m_isFinished = true;
	lstrcpy(m_szLoadingText, TEXT("Loading Completed."));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForMarinHouse()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (pGameInstance == nullptr)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

#pragma region Loading_Textures
	lstrcpy(m_szLoadingText, TEXT("Loading Textures.."));

	// >
	// .. Add Above ..
#pragma endregion Loading_Textures

#pragma region Loading_Components
	lstrcpy(m_szLoadingText, TEXT("Loading Components.."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/NavigationData/MarinHouse.dat")))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Components

#pragma region Loading_Models
	lstrcpy(m_szLoadingText, TEXT("Loading Models.."));

	_matrix PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/*For.Prototype_Component_Model_Tarin*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_Tarin"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Npc_Tarin/Tarin.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Marin*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_Marin"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Npc_Marin/Marin.fbx", false, PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_MarinHouse */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_MarinHouse"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Resources/Meshes/NonAnim/MarinHouse/MarinHouse.fbx"))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Bed */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MARINHOUSE, TEXT("Prototype_Component_Model_Bed"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Resources/Meshes/Anim/Object_Bed/Bed.fbx", false, PivotMatrix))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Models

#pragma region Loading_Shaders
	lstrcpy(m_szLoadingText, TEXT("Loading_Shaders.."));

	// >
	// .. Add Above ..
#pragma endregion Loading_Shaders

	m_isFinished = true;
	lstrcpy(m_szLoadingText, TEXT("Loading Completed."));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Load_GameObject_Prototypes()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance == nullptr)
		return E_FAIL;

#pragma region Loading_Objects
	lstrcpy(m_szLoadingText, TEXT("Loading_Object.."));

	/* For.Prototype_GameObject_Camera_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"), CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Camera_Dungeon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dungeon"), CCamera_Dungeon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Camera_Static */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Static"), CCamera_Static::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_TriggerBox_Dynamic */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TriggerBox_Dynamic"), CTriggerBox_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Effect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"), CEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_StaticObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticObject"), CStaticObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Sword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sword"), CSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Treasure */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Treasure"), CTreasure::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Bed */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bed"), CBed::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Tarin */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tarin"), CTarin::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Marin */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Marin"), CMarin::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Owl */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Owl"), COwl::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_MoriblinSword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MoriblinSword"), CMoriblinSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_MoriblinSpear */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MoriblinSpear"), CMoriblinSpear::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Bossblin */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bossblin"), CBossblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_DungeonDoor */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DungeonDoor"), CDungeonDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Weapon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"), CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Projectile */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Projectile"), CProjectile::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Item */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item"), CItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"), CUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Heart */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Heart"), CUI_Heart::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Inventory */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Inventory"), CUI_Inventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_ItemSlot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ItemSlot"), CUI_ItemSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_ItemChip */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ItemChip"), CUI_ItemChip::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_InventoryItem */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_InventoryItem"), CUI_InventoryItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Chat */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Chat"), CUI_Chat::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_ScreenFade */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_ScreenFade"), CUI_ScreenFade::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Rupees_Font */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Rupees_Font"), CUI_RupeesFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_UI_Level_Name */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Level_Name"), CUI_LevelName::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// >
	// .. Add Above ..
#pragma endregion Loading_Objects

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		ERR_MSG(TEXT("Failed to Create: CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}