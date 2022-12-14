#include "stdafx.h"

#include "ImGuiManager.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "Mesh.h"
#include "Layer.h"
#include "DebugDraw.h"

IMPLEMENT_SINGLETON(CImGuiManager)

CImGuiManager::CImGuiManager()
{
}

HRESULT CImGuiManager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); 
	(void)io;

	io.FontDefault = io.Fonts->AddFontFromFileTTF("../../Resources/Fonts/Quicksand-Medium.ttf", 16.0f);

	//m_temp = io.ConfigFlags;
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	//ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	ImGui_SetupCustomStyle();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	Read_Maps_Name();
	
	// Change Path below
	_tchar cNonAnimResourcesPath[MAX_PATH] = TEXT("../../Resources/Meshes/NonAnim/");
	Read_NonAnimModels_Name(cNonAnimResourcesPath);

	_tchar cAnimResourcePath[MAX_PATH] = TEXT("../../Resources/Meshes/Anim/");
	Read_AnimModels_Name(cAnimResourcePath);

	// Initialize Variables for drawing Navigation Mesh.
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void*	pShaderbyteCode = nullptr;
	size_t iShaderByteCodeLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderbyteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderbyteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	return S_OK;
}

void CImGuiManager::ImGui_SetupCustomStyle()
{
	static ImVec3 color_for_text = ImVec3(236.f / 255.f, 240.f / 255.f, 241.f / 255.f);
	static ImVec3 color_for_head = ImVec3(6.f / 255.f, 29.f / 255.f, 51.f / 255.f);
	static ImVec3 color_for_area = ImVec3(57.f / 255.f, 79.f / 255.f, 105.f / 255.f);
	static ImVec3 color_for_body = ImVec3(40.f / 255.f, 60.f / 255.f, 80.f / 255.f);
	static ImVec3 color_for_pops = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
	ApplyStyle(color_for_text, color_for_head, color_for_area, color_for_body, color_for_pops);
}

void CImGuiManager::ApplyStyle(ImVec3 color_for_text, ImVec3 color_for_head, ImVec3 color_for_area, ImVec3 color_for_body, ImVec3 color_for_pops)
{
	ImGuiStyle& style = ImGui::GetStyle();
	
	style.WindowRounding = 6;
	style.FrameRounding = 4;
	style.ScrollbarSize = 10;
	style.ScrollbarRounding = 6;
	style.ItemSpacing = ImVec2(10, 4);

	style.Colors[ImGuiCol_Text] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.58f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.95f);
	style.Colors[ImGuiCol_Border] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.20f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.47f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.21f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.80f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.60f);
	style.Colors[ImGuiCol_Button] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.15f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.43f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(color_for_pops.x, color_for_pops.y, color_for_pops.z, 0.92f);
	style.Colors[ImGuiCol_Tab] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.20f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.60f);
}

void CImGuiManager::Render(_float fTimeDelta)
{
	// Frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DrawEditor(fTimeDelta);
	DrawOverlayWindow();

	if (m_bShowNavigationMesh)
		DrawNavigationMesh();

	if (m_vShowTriggerBox)
		DrawTriggerBox();

	ImGui::EndFrame();

	// Render
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGuiManager::DrawEditor(_float fTimeDelta)
{
	ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	// Editor Alpha
	ImGui::Text("Settings");
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 WindowColor = style.Colors[ImGuiCol_WindowBg];
	ImGui::SliderFloat("Editor Opacity", &m_fEditorAlpha, 0.0f, 1.0f);
	const ImVec4 NewColor = ImVec4(WindowColor.x, WindowColor.y, WindowColor.z, m_fEditorAlpha);
	style.Colors[ImGuiCol_WindowBg] = NewColor;
	ImGui::NewLine();

	// Tabs
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Map Tool"))
			DrawMapTool(fTimeDelta);
		if (ImGui::BeginTabItem("UI Tool"))
			DrawUITool();
		if (ImGui::BeginTabItem("Cam Tool"))
			DrawCamTool();
	
		ImGui::EndTabBar();
	}
	
	ImGui::End();
}

void CImGuiManager::DrawOverlayWindow()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

	if (ImGui::Begin("Debug Info", nullptr, window_flags))
	{
		ImGui::Text("Debug Info");
		ImGui::Separator();

		// FramesPerSecond
		ImGui::Text("%d FPS", m_iFramesPerSecond);

		if (m_pSelectedCreatedObject && m_pTransform)
		{
			wstring wsCurrentObject = wstring(m_pSelectedCreatedObject->Get_ObjId());
			string sCurrentObject = string(wsCurrentObject.begin(), wsCurrentObject.end());
			ImGui::Text("%s", sCurrentObject.c_str());
		}

		ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", m_vPositionPicked.x, m_vPositionPicked.y, m_vPositionPicked.z);
	}

	ImGui::End();
}

void CImGuiManager::DrawNavigationMesh()
{
	if (m_vNavigationCells.empty() && m_vCurrentCell.empty())
		return;

	m_pBatch->Begin();
	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->SetWorld(XMMatrixIdentity());

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pEffect->SetView(pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	RELEASE_INSTANCE(CGameInstance);

	m_pEffect->Apply(m_pContext);

	// Render Green Triangles
	_vector	vGreen = XMVectorSet(.31f, .78f, .47f, 1.f);
	_vector	vOrange = XMVectorSet(1.f, .5f, .31f, 1.f);

	for (auto& vPoint : m_vCurrentCell)
	{
		_float3 vCellPoint = vPoint;
		vCellPoint.y += .02f;
		DX::DrawRing(m_pBatch, XMLoadFloat3(&vCellPoint), XMVectorSet(.05f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, .05f, 0.f), vGreen);
	}

	/* Firstly, Draw Default Cells. */
	for (auto& vCell : m_vNavigationCells)
	{
		if (vCell.first == CELL_TYPE::CELL_DEFAULT)
		{
			_float3 vCellPointA = (_float3)vCell.second.m[0];
			vCellPointA.y += .02f;
			_float3 vCellPointB = (_float3)vCell.second.m[1];
			vCellPointB.y += .02f;
			_float3 vCellPointC = (_float3)vCell.second.m[2];
			vCellPointC.y += .02f;

			_vector vPointA = XMLoadFloat3(&vCellPointA);;
			_vector vPointB = XMLoadFloat3(&vCellPointB);;
			_vector vPointC = XMLoadFloat3(&vCellPointC);;

			DX::DrawTriangle(m_pBatch, vPointA, vPointB, vPointC, vGreen);
		}
	}

	/* Then, Draw Fall Cells. */
	for (auto& vCell : m_vNavigationCells)
	{
		if (vCell.first == CELL_TYPE::CELL_FALL)
		{
			_float3 vCellPointA = (_float3)vCell.second.m[0];
			vCellPointA.y += .02f;
			_float3 vCellPointB = (_float3)vCell.second.m[1];
			vCellPointB.y += .02f;
			_float3 vCellPointC = (_float3)vCell.second.m[2];
			vCellPointC.y += .02f;

			_vector vPointA = XMLoadFloat3(&vCellPointA);;
			_vector vPointB = XMLoadFloat3(&vCellPointB);;
			_vector vPointC = XMLoadFloat3(&vCellPointC);;

			DX::DrawTriangle(m_pBatch, vPointA, vPointB, vPointC, vOrange);
		}
	}

	m_pBatch->End();
}

void CImGuiManager::DrawTriggerBox()
{
	if (m_vTriggers.empty() && !m_bCurrentActive)
		return;

	m_pBatch->Begin();
	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->SetWorld(XMMatrixIdentity());

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pEffect->SetView(pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	RELEASE_INSTANCE(CGameInstance);

	m_pEffect->Apply(m_pContext);

	// Render Blue Cubes
	_vector	vColor = XMVectorSet(.39f, .58f, .92f, 1.f);

	if (m_bCurrentActive)
	{
		_float3 vTriggerCenter = m_vCurrentTrigger;
		vTriggerCenter.y += .02f;
		DX::DrawRing(m_pBatch, XMLoadFloat3(&vTriggerCenter), XMVectorSet(.05f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, .05f, 0.f), vColor);
	}
	
	for (auto& vTriggerBox : m_vTriggers)
		DX::Draw(m_pBatch, vTriggerBox.pTriggerBox, vColor);

	m_pBatch->End();
}

void CImGuiManager::DrawMapTool(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;
	
	// Level
	if (ImGui::CollapsingHeader("Level"/*, TreeNodeFlags*/))
	{
		// Create New Map
		ImGui::Text("New Level");
		static char cMapName[MAX_PATH];
		ImGui::InputText("##InputLevel", cMapName, MAX_PATH);
		ImGui::SameLine();
		if (ImGui::Button("Create Level"))
		{
			if (strcmp(cMapName, ""))
			{
				m_sCurrentMap = cMapName; // Set Current Map
				m_sCurrentMap += ".dat";

				memset(cMapName, 0, MAX_PATH);

				ImGui::OpenPopup("Create Level");
			}
		}
		ImGui::NewLine();
		ImGui::Separator();

		// Map List
		ImGui::Text("Saved Levels");
		if (ImGui::BeginListBox("##Levels", ImVec2(-FLT_MIN, 4 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (int i = 0; i < m_vMaps.size(); i++)
			{
				if (ImGui::Selectable(m_vMaps[i].c_str(), m_iSelectedMap == i))
					m_iSelectedMap = i;

				if (m_iSelectedMap == i)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Load Level"))
		{
			if (LoadData())
			{
				m_sCurrentMap = m_vMaps[m_iSelectedMap];
				ImGui::OpenPopup("Load Level");
			}
		}
		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Text("Current Level: %s", m_sCurrentMap.c_str());

		if (ImGui::Button("Save Level"))
		{
			if (SaveData())
			{
				_bool bNewMap = true;
				for (auto& map : m_vMaps)
				{
					if (m_sCurrentMap == map)
					{
						bNewMap = false;
						break;
					}
				}

				if (bNewMap)
					m_vMaps.push_back(m_sCurrentMap);

				ImGui::OpenPopup("Save Level");
			}
		}

		ImGui::NewLine();
	}

	// Terrain
	if (ImGui::CollapsingHeader("Terrain"/*, TreeNodeFlags*/))
	{
		ImGui::Text("New Terrain");
		// Inputs
		ImGui::PushItemWidth(120);
		ImGui::InputInt("X-Axis Vertices", &m_iXVertex);
		ImGui::InputInt("Z-Axis Vertices", &m_iZVertex);
		ImGui::PopItemWidth();

		// Buttons
		if (ImGui::Button("Create Terrain"))
		{
			// Destroy current Map
			CGameObject* pObject = pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Terrain"));
			if (pObject)
				pGameInstance->Delete_GameObject(pObject, LEVEL_TOOL, TEXT("Layer_Terrain"));

			// Create Terrain				 
			CComponent* pComponent = pGameInstance->Find_Component_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"));
			CVIBuffer_Terrain* pVIBufferTerrain = dynamic_cast<CVIBuffer_Terrain*>(pComponent);
			if (!pVIBufferTerrain)
				return;

			pVIBufferTerrain->Set_NumVerticesX(m_iXVertex);
			pVIBufferTerrain->Set_NumVerticesZ(m_iZVertex);
			pVIBufferTerrain->Refresh_Vertices();

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Terrain"), TEXT("Prototype_GameObject_Terrain"), LEVEL_TOOL, TEXT("Layer_Terrain"), nullptr)))
				return;
		}
		
		ImGui::NewLine();
	}

	// Object
	if (ImGui::CollapsingHeader("Model"/*, TreeNodeFlags*/))
	{
		ImGui::Text("New Object");

		// Objects List
		if (ImGui::BeginListBox("##Objects List", ImVec2(-FLT_MIN, 4 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (auto& iter = m_lObjects.begin(); iter != m_lObjects.end(); iter++)
			{
				wstring wsFileName = wstring(iter->wcFileName);
				string sFileName = string(wsFileName.begin(), wsFileName.end());

				if (ImGui::Selectable(sFileName.c_str(), m_sSelectedObject == sFileName))
					m_sSelectedObject = sFileName;

				if (m_sSelectedObject == sFileName)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		ImGui::NewLine();
		ImGui::Text("Layer");
		ImGui::SameLine();

		// Get Layers
		CObject_Manager::LAYERS mLayers = pGameInstance->Get_Layers(LEVEL_TOOL);

		// Populate Layers
		if (!mLayers.empty())
		{
			for (auto& iter = mLayers.begin(); iter != mLayers.end(); iter++)
			{
				wstring wsLayerTag = wstring(iter->first);
				string sLayerTag = string(wsLayerTag.begin(), wsLayerTag.end());
				m_vLayers.insert(sLayerTag.c_str());

				m_vLayersTemp.insert(iter->first);
			}
		}

		m_sCurrentLayer = m_sCurrentLayer.empty() ? 
				m_vLayers.empty() ? "" : *m_vLayers.begin()
			: m_sCurrentLayer;

		// Layers Combo
		if (ImGui::BeginCombo("##ChooseLayer", m_sCurrentLayer.c_str()))
		{
			_uint iCounter = 0;
			for (auto& iter = m_vLayers.begin(); iter != m_vLayers.end(); iter++)
			{
				if (ImGui::Selectable(iter->c_str(), iter->c_str() == m_sCurrentLayer))
				{
					m_sCurrentLayer = *iter;
				}

				if (iter->c_str() == m_sCurrentLayer)
					ImGui::SetItemDefaultFocus();

				iCounter++;
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();

		if (ImGui::Button("Add", ImVec2(0, 24)))
			ImGui::OpenPopup("Add Layer");

		if (ImGui::Button("Create Object"))
			Create_Object();

		ImGui::NewLine();
		ImGui::Separator();

		// Created Objects
		ImGui::Text("Created Objects");
		if (ImGui::BeginTable("CreatedObjectTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable, ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 6)))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("Object");
			ImGui::TableSetupColumn("Layer");
			ImGui::TableHeadersRow();
			
			// Iterate Layers
			for (auto& iter = mLayers.begin(); iter != mLayers.end(); iter++)
			{
				CLayer* pLayer = iter->second;
				CLayer::GAMEOBJECTS vGameObjects = pLayer->Get_Objects();

				ImGui::TableNextRow();

				wstring wsLayer = wstring(iter->first);
				string sLayer = string(wsLayer.begin(), wsLayer.end());

				for (auto& pObject : vGameObjects)
				{
					// Don't save the Camera Object to File
					if (!wcscmp(pObject->Get_ObjName(), TEXT("Camera_Dynamic")))
						continue;

					ImGui::TableNextColumn();

					wstring wsObjId = wstring(pObject->Get_ObjId());
					string sObjId = string(wsObjId.begin(), wsObjId.end());
					if (ImGui::Selectable(sObjId.c_str(), !wcscmp(pObject->Get_ObjId(), m_pSelectedCreatedObject ? m_pSelectedCreatedObject->Get_ObjId() : TEXT("")), ImGuiSelectableFlags_SpanAllColumns))
					{
						CMesh* pToolMesh = dynamic_cast<CMesh*>(m_pSelectedCreatedObject);
						if (pToolMesh)
							pToolMesh->Set_IsSelected(false);

						m_pSelectedCreatedObject = pObject;

						pToolMesh = dynamic_cast<CMesh*>(m_pSelectedCreatedObject);
						if (pToolMesh)
							pToolMesh->Set_IsSelected(true);

						// Set Transform of the Selected Object
						CComponent* pComponent = m_pSelectedCreatedObject->Find_Component(TEXT("Com_Transform"));
						if (!pComponent)
							return;
						CTransform* pTransform = dynamic_cast<CTransform*>(pComponent);
						if (!pTransform)
							return;

						m_pTransform = pTransform;

						// Set Transform Sliders values based on selected Transformation Action
						switch (m_eObjAction)
						{
						case TRANS_SCALE:
						{
							m_fX = m_pTransform->Get_Scale(CTransform::STATE::STATE_RIGHT);
							m_fY = m_pTransform->Get_Scale(CTransform::STATE::STATE_UP);
							m_fZ = m_pTransform->Get_Scale(CTransform::STATE::STATE_LOOK);
							break;
						}
						case TRANS_ROTATION:
						{
							m_fX = m_pTransform->Get_CurrentRotationX();
							m_fY = m_pTransform->Get_CurrentRotationY();
							m_fZ = m_pTransform->Get_CurrentRotationZ();
							break;
						}
						case TRANS_TRANSLATION:
						{
							_float3 vCurrentTranslation;
							XMStoreFloat3(&vCurrentTranslation, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
							m_fX = vCurrentTranslation.x;
							m_fY = vCurrentTranslation.y;
							m_fZ = vCurrentTranslation.z;
							break;
						}
						}
					}

					ImGui::TableNextColumn();
					ImGui::Text(sLayer.c_str());
				}
			}
			
			ImGui::EndTable();
		}

		ImGui::NewLine();

		// Transformations
		_float3 vScale, vRotation, vTranslation;

		if (ImGui::RadioButton("Scale", m_eObjAction == TRANS_SCALE))
		{
			m_eObjAction = TRANS_SCALE;

			if (m_pTransform)
			{
				m_fX = m_pTransform->Get_Scale(CTransform::STATE::STATE_RIGHT);
				m_fY = m_pTransform->Get_Scale(CTransform::STATE::STATE_UP);
				m_fZ = m_pTransform->Get_Scale(CTransform::STATE::STATE_LOOK);
			}
		}
		ImGui::SameLine();

		if (ImGui::RadioButton("Rotation", m_eObjAction == TRANS_ROTATION))
		{
			m_eObjAction = TRANS_ROTATION;
				
			if (m_pTransform)
			{
				m_fX = m_pTransform->Get_CurrentRotationX();
				m_fY = m_pTransform->Get_CurrentRotationY();
				m_fZ = m_pTransform->Get_CurrentRotationZ();
			}
		}
		ImGui::SameLine();

		if (ImGui::RadioButton("Translation", m_eObjAction == TRANS_TRANSLATION))
		{
			m_eObjAction = TRANS_TRANSLATION;

			if (m_pTransform)
			{
				_float3 vCurrentTranslation;
				XMStoreFloat3(&vCurrentTranslation, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
				m_fX = vCurrentTranslation.x;
				m_fY = vCurrentTranslation.y;
				m_fZ = vCurrentTranslation.z;
			}
		}

		ImGui::SetNextItemWidth(80);
		if (ImGui::DragFloat("##X", &m_fX, m_eObjAction == TRANS_ROTATION ? 1 : 0.05f, m_eObjAction == TRANS_ROTATION ? -360.f : 0.f, m_eObjAction == TRANS_ROTATION ? 360.f : 0.f, "X: %.03f"))
		{
			switch (m_eObjAction)
			{
			case TRANS_SCALE:
			{
				if (m_pTransform)
					m_pTransform->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fX);
				break;
			}
			case TRANS_ROTATION:
			{
				if (m_pTransform)
				{
					_vector vRotationX = XMVectorSet(1.f, 0.f, 0.f, 0.f);
					m_pTransform->Set_Rotation(_float3(m_fX, m_fY, m_fZ));
				}
				
				break;
			}
			case TRANS_TRANSLATION:
			{
				if (m_pTransform)
				{
					XMStoreFloat3(&vTranslation, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
					_vector vNewTranslation = XMVectorSet(m_fX, vTranslation.y, vTranslation.z, 1.f);
					m_pTransform->Set_State(CTransform::STATE::STATE_TRANSLATION, vNewTranslation);
				}
				
				break; 
			}	
			}
		}
		ImGui::SameLine(); 

		ImGui::SetNextItemWidth(80);
		if (ImGui::DragFloat("##Y", &m_fY, m_eObjAction == TRANS_ROTATION ? 1 : 0.05f, m_eObjAction == TRANS_ROTATION ? -360.f : 0.f, m_eObjAction == TRANS_ROTATION ? 360.f : 0.f, "Y: %.03f"))
		{
			switch (m_eObjAction)
			{
				case TRANS_SCALE:
				{
					if (m_pTransform)
						m_pTransform->Set_Scale(CTransform::STATE::STATE_UP, m_fY);
					break;
				}
				case TRANS_ROTATION:
				{
					if (m_pTransform)
						m_pTransform->Set_Rotation(_float3(m_fX, m_fY, m_fZ));
					break;
				}
				case TRANS_TRANSLATION:
				{	
					if (m_pTransform)
					{
						XMStoreFloat3(&vTranslation, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
						_vector vNewTranslation = XMVectorSet(vTranslation.x, m_fY, vTranslation.z, 1.f);
						m_pTransform->Set_State(CTransform::STATE::STATE_TRANSLATION, vNewTranslation);
					}
					
					break;
				}
			}
		}
		ImGui::SameLine();

		ImGui::SetNextItemWidth(80);
		if (ImGui::DragFloat("##Z", &m_fZ, m_eObjAction == TRANS_ROTATION ? 1 : 0.05f, m_eObjAction == TRANS_ROTATION ? -360.f : 0.f, m_eObjAction == TRANS_ROTATION ? 360.f : 0.f, "Z: %.03f"))
		{
			switch (m_eObjAction)
			{
				case TRANS_SCALE:
				{
					if (m_pTransform)
						m_pTransform->Set_Scale(CTransform::STATE::STATE_LOOK, m_fZ);
					break;
				}
				case TRANS_ROTATION:
				{
					if (m_pTransform)
						m_pTransform->Set_Rotation(_float3(m_fX, m_fY, m_fZ));
					break;
				}
				case TRANS_TRANSLATION:
				{
					if (m_pTransform)
					{
						XMStoreFloat3(&vTranslation, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
						_vector vNewTranslation = XMVectorSet(vTranslation.x, vTranslation.y, m_fZ, 1.f);
						m_pTransform->Set_State(CTransform::STATE::STATE_TRANSLATION, vNewTranslation);
					}
					break;
				}
			}
		}
		ImGui::NewLine();
		
		if (ImGui::Button("Delete Object"))
		{
			if (m_pSelectedCreatedObject)
			{
				pGameInstance->Delete_GameObject(m_pSelectedCreatedObject, LEVEL_TOOL, m_pSelectedCreatedObject->Get_LayerId());
				m_pSelectedCreatedObject = nullptr;
			}
		}
		ImGui::NewLine();
	}
	
	// Navigation Mesh
	if (ImGui::CollapsingHeader("Navigation Mesh"/*, TreeNodeFlags*/))
	{
		if (ImGui::Checkbox("Show Navigation Mesh", &m_bShowNavigationMesh))
			m_bShowNavigationMesh != m_bShowNavigationMesh;
		ImGui::NewLine();

		if (ImGui::Button(m_bIsNavigationActive ? "End Navigation" : "Start Navigation", ImVec2(0, 24)))
			m_bIsNavigationActive = !m_bIsNavigationActive;			

		if (ImGui::Checkbox("Vertex Snapping", &m_bVertexSnapping))
			m_bVertexSnapping != m_bVertexSnapping;
		ImGui::NewLine();

		if (ImGui::Button("Undo", ImVec2(0, 24)))
		{
			if (!m_vCurrentCell.empty())
				m_vCurrentCell.pop_back();
			else if (!m_vNavigationCells.empty())
				m_vNavigationCells.pop_back();
		}
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Cell Settings");
		ImGui::NewLine();

		if (ImGui::RadioButton("Default", m_eCellType == CELL_DEFAULT))
			m_eCellType = CELL_DEFAULT;

		ImGui::SameLine();

		if (ImGui::RadioButton("Fall", m_eCellType == CELL_FALL))
			m_eCellType = CELL_FALL;

		ImGui::SameLine();
		if (ImGui::Button("Set"))
			Set_CellType(m_iSelectedCellIndex);
		ImGui::NewLine();
	}
	
	// Trigger Box
	if (ImGui::CollapsingHeader("Trigger Box" /*, TreeNodeFlags*/))
	{
		if (ImGui::Checkbox("Show Trigger Box", &m_vShowTriggerBox))
			m_vShowTriggerBox != m_vShowTriggerBox;
		ImGui::NewLine();

		if (ImGui::Button(m_bIsTriggerActive ? "End Trigger" : "Start Trigger", ImVec2(0, 24)))
			m_bIsTriggerActive = !m_bIsTriggerActive;

		if (ImGui::Button("Create Trigger", ImVec2(0, 24)))
			ImGui::OpenPopup("Add Trigger Box");
		ImGui::NewLine();

		ImGui::Separator();

		// Trigger Box List
		ImGui::Text("Created Trigger Boxes");
		if (ImGui::BeginListBox("##Trigger Box List", ImVec2(-FLT_MIN, 4 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (auto& tTrigger : m_vTriggers)
			{
				if (ImGui::Selectable(tTrigger.pTriggerName, m_tSelectedTrigger == &tTrigger))
				{
					m_tSelectedTrigger = &tTrigger;
					m_fTriggerBoxScale = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_tSelectedTrigger->mWorldMatrix).r[0]));
					_float3 vTriggerPosition = (_float3)m_tSelectedTrigger->mWorldMatrix.m[3];
					m_fTriggerBoxX = vTriggerPosition.x;
					m_fTriggerBoxY = vTriggerPosition.y;
					m_fTriggerBoxZ = vTriggerPosition.z;
				}

				if (m_tSelectedTrigger == &tTrigger)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		// Trigger Scale Drag
		ImGui::NewLine();
		ImGui::Text("Scale:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("##TriggerBoxScale", &m_fTriggerBoxScale, 0.005f, 0.f, 0.f, "%.03f"))
		{
			if (m_tSelectedTrigger)
			{
				_float3 vTriggerPosition = (_float3)m_tSelectedTrigger->mWorldMatrix.m[3];

				// Firstly, Reset any Transformation 
				_matrix WorldMatrixInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_tSelectedTrigger->mWorldMatrix));
				m_tSelectedTrigger->pTriggerBox.Transform(m_tSelectedTrigger->pTriggerBox, WorldMatrixInverse);

				_matrix TranslationMatrix = XMMatrixTranslation(vTriggerPosition.x, vTriggerPosition.y, vTriggerPosition.z);
				_matrix ScaleMatrix = XMMatrixScaling(m_fTriggerBoxScale, m_fTriggerBoxScale, m_fTriggerBoxScale);

				_matrix WorldMatrix = ScaleMatrix * TranslationMatrix;

				// Then, apply the new Transformation
				m_tSelectedTrigger->pTriggerBox.Transform(m_tSelectedTrigger->pTriggerBox, WorldMatrix);
				XMStoreFloat4x4(&m_tSelectedTrigger->mWorldMatrix, WorldMatrix);
			}
		}

		// Trigger Translation Drag
		ImGui::Text("Translation:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("##TriggerBoxTranslationX", &m_fTriggerBoxX, 0.005f, 0.f, 0.f, "%.03f"))
		{
			if (m_tSelectedTrigger)
			{
				_float3 vTriggerPosition = (_float3)m_tSelectedTrigger->mWorldMatrix.m[3];

				// Firstly, Reset any Transformation 
				_matrix WorldMatrixInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_tSelectedTrigger->mWorldMatrix));
				m_tSelectedTrigger->pTriggerBox.Transform(m_tSelectedTrigger->pTriggerBox, WorldMatrixInverse);

				// Then, apply the new Transformation
				_matrix TranslationMatrix = XMMatrixTranslation(m_fTriggerBoxX, vTriggerPosition.y, vTriggerPosition.z);
				_matrix ScaleMatrix = XMMatrixScaling(m_fTriggerBoxScale, m_fTriggerBoxScale, m_fTriggerBoxScale);

				_matrix WorldMatrix = ScaleMatrix * TranslationMatrix;

				m_tSelectedTrigger->pTriggerBox.Transform(m_tSelectedTrigger->pTriggerBox, WorldMatrix);
				XMStoreFloat4x4(&m_tSelectedTrigger->mWorldMatrix, WorldMatrix);
			}
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("##TriggerBoxTranslationY", &m_fTriggerBoxY, 0.005f, 0.f, 0.f, "%.03f"))
		{
			if (m_tSelectedTrigger)
			{
				_float3 vTriggerPosition = (_float3)m_tSelectedTrigger->mWorldMatrix.m[3];

				// Firstly, Reset any Transformation 
				_matrix WorldMatrixInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_tSelectedTrigger->mWorldMatrix));
				m_tSelectedTrigger->pTriggerBox.Transform(m_tSelectedTrigger->pTriggerBox, WorldMatrixInverse);

				// Then, apply the new Transformation
				_matrix TranslationMatrix = XMMatrixTranslation(vTriggerPosition.x, m_fTriggerBoxY, vTriggerPosition.z);
				_matrix ScaleMatrix = XMMatrixScaling(m_fTriggerBoxScale, m_fTriggerBoxScale, m_fTriggerBoxScale);

				_matrix WorldMatrix = ScaleMatrix * TranslationMatrix;

				m_tSelectedTrigger->pTriggerBox.Transform(m_tSelectedTrigger->pTriggerBox, WorldMatrix);
				XMStoreFloat4x4(&m_tSelectedTrigger->mWorldMatrix, WorldMatrix);
			}
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		if (ImGui::DragFloat("##TriggerBoxTranslationZ", &m_fTriggerBoxZ, 0.005f, 0.f, 0.f, "%.03f"))
		{
			if (m_tSelectedTrigger)
			{
				_float3 vTriggerPosition = (_float3)m_tSelectedTrigger->mWorldMatrix.m[3];

				// Firstly, Reset any Transformation 
				_matrix WorldMatrixInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_tSelectedTrigger->mWorldMatrix));
				m_tSelectedTrigger->pTriggerBox.Transform(m_tSelectedTrigger->pTriggerBox, WorldMatrixInverse);

				// Then, apply the new Transformation
				_matrix TranslationMatrix = XMMatrixTranslation(vTriggerPosition.x, vTriggerPosition.y, m_fTriggerBoxZ);
				_matrix ScaleMatrix = XMMatrixScaling(m_fTriggerBoxScale, m_fTriggerBoxScale, m_fTriggerBoxScale);

				_matrix WorldMatrix = ScaleMatrix * TranslationMatrix;

				m_tSelectedTrigger->pTriggerBox.Transform(m_tSelectedTrigger->pTriggerBox, WorldMatrix);
				XMStoreFloat4x4(&m_tSelectedTrigger->mWorldMatrix, WorldMatrix);
			}
		}

		ImGui::NewLine();
		if (ImGui::Button("Delete Trigger", ImVec2(0, 24)))
		{
			for (auto& iter = m_vTriggers.begin(); iter != m_vTriggers.end(); iter++)
			{
				if (!strcmp(iter->pTriggerName, m_tSelectedTrigger->pTriggerName))
				{
					m_vTriggers.erase(iter);
					m_tSelectedTrigger = nullptr;
					break;
				}
			}
		}
	}

	DrawMapModals();

	ImGui::EndTabItem();

	RELEASE_INSTANCE(CGameInstance);
}

void CImGuiManager::DrawUITool()
{
	ImGui::EndTabItem();
}

void CImGuiManager::DrawCamTool()
{
	ImGui::EndTabItem();
}

void CImGuiManager::DrawMapModals()
{
	// Create Map Modal
	ImVec2 pCenter = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Create Level", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Level Created!\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
			ImGui::CloseCurrentPopup();

		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}

	// Load Map Modal
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Load Level", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Level Loaded!\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
			ImGui::CloseCurrentPopup();

		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}

	// Save Map Modal
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Save Level", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Level Saved!\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			// TODO: ..

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}

	// Add Layer Modal
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Add Layer", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Layer Name");
		static char cLayer[MAX_PATH] = "";
		ImGui::PushItemWidth(200);
		ImGui::InputText("##AddLayer", cLayer, MAX_PATH);
		ImGui::PopItemWidth();
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			m_vLayers.insert(cLayer);

			string sLayer = string(cLayer);
			wstring wsLayer = wstring(sLayer.begin(), sLayer.end());

			m_vLayersTemp.insert(wsLayer);

			memset(cLayer, 0, MAX_PATH);
		
			if (m_sCurrentLayer.empty())
				m_sCurrentLayer = cLayer;

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}

	// Add Trigger Box Modal
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Add Trigger Box", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Trigger Box Name");
		static char cTriggerBox[MAX_PATH] = "";
		ImGui::PushItemWidth(200);
		ImGui::InputText("##AddTriggerBox", cTriggerBox, MAX_PATH);
		ImGui::PopItemWidth();
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			Add_Trigger(cTriggerBox);

			memset(cTriggerBox, 0, MAX_PATH);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}

void CImGuiManager::Read_Maps_Name()
{
	WIN32_FIND_DATA fileData;
	HANDLE hDir = FindFirstFile(TEXT("../../Data/MapData/*"), &fileData);

	/* No files found */
	if (hDir == INVALID_HANDLE_VALUE)
		return; 

	do {
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		if (!lstrcmp(fileData.cFileName, TEXT(".")) || !lstrcmp(fileData.cFileName, TEXT("..")))
			continue;

		wstring wsFileName(fileData.cFileName);
		string sFileName(wsFileName.begin(), wsFileName.end());

		m_vMaps.push_back(sFileName);
	} while (FindNextFile(hDir, &fileData));

	FindClose(hDir);
}

void CImGuiManager::Read_NonAnimModels_Name(_tchar* cFolderPath)
{
	_tchar filePath[MAX_PATH] = TEXT("");
	wcscpy_s(filePath, MAX_PATH, cFolderPath); // Backup Path used for Sub-folders

	wcscat_s(cFolderPath, MAX_PATH, TEXT("*"));

	WIN32_FIND_DATA fileData;

	HANDLE hDir = FindFirstFile(cFolderPath, &fileData);

	/* No files found */
	if (hDir == INVALID_HANDLE_VALUE) 
		return; 

	do {
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Directory
		{
			if (lstrcmp(fileData.cFileName, TEXT(".")) == 0 || lstrcmp(fileData.cFileName, TEXT("..")) == 0)
				continue;

			_tchar subFilePath[MAX_PATH] = TEXT("");
			wcscpy_s(subFilePath, MAX_PATH, filePath);
			wcscat_s(subFilePath, MAX_PATH, fileData.cFileName);
			wcscat_s(subFilePath, MAX_PATH, TEXT("/"));
			
			// Recursive Function Call
			Read_NonAnimModels_Name(subFilePath);
		}
		else // File
		{
			_tchar szFileName[MAX_PATH];
			_tchar szFileExt[MAX_PATH];
			
			_wsplitpath_s(fileData.cFileName, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szFileExt, MAX_PATH);

			if (!wcscmp(szFileExt, TEXT(".fbx")))
			{
				_tchar wcModelPrototypeId[MAX_PATH] = TEXT("Prototype_Component_Model_");
				wcscat_s(wcModelPrototypeId, MAX_PATH, szFileName);

				_tchar wcObjectPrototypeId[MAX_PATH] = TEXT("Prototype_GameObject_");
				wcscat_s(wcObjectPrototypeId, MAX_PATH, szFileName);

				// Model Infos
				CMesh::MODELDESC tModelDesc;
				wcscpy_s(tModelDesc.wcFileName, MAX_PATH, fileData.cFileName);			// "Fiona.fbx"
				wcscpy_s(tModelDesc.wcFilePath, MAX_PATH, filePath);					// "../../Resources/Meshes/Anim/Fiona/"
				wcscpy_s(tModelDesc.wcModelPrototypeId, MAX_PATH, wcModelPrototypeId);	// "Prototype_Component_Model_Fiona"
				tModelDesc.eModelType = CModel::TYPE::TYPE_NONANIM;						// "TYPE::TYPE_NONANIM" or "TYPE::TYPE_ANIM"

				// Object Infos
				wcscpy_s(tModelDesc.wcObjName, MAX_PATH, szFileName);					// "Fiona"
				wcscpy_s(tModelDesc.wcObjPrototypeId, MAX_PATH, wcObjectPrototypeId);	// "Prototype_GameObject_Player"					

				XMStoreFloat4x4(&tModelDesc.mWorldMatrix, XMMatrixIdentity());
				tModelDesc.vRotation = _float3(0.f, 0.f, 0.f);

				m_lObjects.push_back(tModelDesc);
			}
		}
	} while (FindNextFile(hDir, &fileData));

	FindClose(hDir);
}

void CImGuiManager::Read_AnimModels_Name(_tchar * cFolderPath)
{
	_tchar filePath[MAX_PATH] = TEXT("");
	wcscpy_s(filePath, MAX_PATH, cFolderPath); // Backup Path used for Sub-folders

	wcscat_s(cFolderPath, MAX_PATH, TEXT("*"));

	WIN32_FIND_DATA fileData;

	HANDLE hDir = FindFirstFile(cFolderPath, &fileData);

	/* No files found */
	if (hDir == INVALID_HANDLE_VALUE)
		return;

	do {
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Directory
		{
			if (lstrcmp(fileData.cFileName, TEXT(".")) == 0 || lstrcmp(fileData.cFileName, TEXT("..")) == 0)
				continue;

			_tchar subFilePath[MAX_PATH] = TEXT("");
			wcscpy_s(subFilePath, MAX_PATH, filePath);
			wcscat_s(subFilePath, MAX_PATH, fileData.cFileName);
			wcscat_s(subFilePath, MAX_PATH, TEXT("/"));

			// Recursive Function Call
			Read_AnimModels_Name(subFilePath);
		}
		else // File
		{
			_tchar szFileName[MAX_PATH];
			_tchar szFileExt[MAX_PATH];

			_wsplitpath_s(fileData.cFileName, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szFileExt, MAX_PATH);

			if (!wcscmp(szFileExt, TEXT(".fbx")))
			{
				_tchar wcModelPrototypeId[MAX_PATH] = TEXT("Prototype_Component_Model_");
				wcscat_s(wcModelPrototypeId, MAX_PATH, szFileName);

				_tchar wcObjectPrototypeId[MAX_PATH] = TEXT("Prototype_GameObject_");
				wcscat_s(wcObjectPrototypeId, MAX_PATH, szFileName);

				// Model Infos
				CMesh::MODELDESC tModelDesc;
				wcscpy_s(tModelDesc.wcFileName, MAX_PATH, fileData.cFileName);			// "Fiona.fbx"
				wcscpy_s(tModelDesc.wcFilePath, MAX_PATH, filePath);					// "../../Resources/Meshes/Anim/Fiona/"
				wcscpy_s(tModelDesc.wcModelPrototypeId, MAX_PATH, wcModelPrototypeId);	// "Prototype_Component_Model_Fiona"
				tModelDesc.eModelType = CModel::TYPE::TYPE_ANIM;						// "TYPE::TYPE_NONANIM" or "TYPE::TYPE_ANIM"

				// Object Infos
				wcscpy_s(tModelDesc.wcObjName, MAX_PATH, szFileName);					// "Fiona"
				wcscpy_s(tModelDesc.wcObjPrototypeId, MAX_PATH, wcObjectPrototypeId);	// "Prototype_GameObject_Player"	

				XMStoreFloat4x4(&tModelDesc.mWorldMatrix, XMMatrixIdentity());
				tModelDesc.vRotation = _float3(0.f, 0.f, 0.f);

				m_lObjects.push_back(tModelDesc);
			}
		}
	} while (FindNextFile(hDir, &fileData));

	FindClose(hDir);
}

void CImGuiManager::Create_Object()
{
	if (m_sSelectedObject.empty())
		return;

	list<CMesh::MODELDESC>::iterator iter = find_if(m_lObjects.begin(), m_lObjects.end(), [&](CMesh::MODELDESC tDesc) {
		wstring wsFileName = wstring(tDesc.wcFileName);
		string sFileName = string(wsFileName.begin(), wsFileName.end());

		return m_sSelectedObject == sFileName;
	});

	if (iter != m_lObjects.end())
	{
		// LayerId (Ex: "Layer_Player")
		wstring wsLayer = wstring(m_sCurrentLayer.begin(), m_sCurrentLayer.end());
		unordered_set<wstring>::iterator layerIter = m_vLayersTemp.find(wsLayer);

		// FileName (Ex: "Fiona.fbx" > "Fiona")
		_tchar wcFileName[MAX_PATH];
		_wsplitpath_s(iter->wcFileName, nullptr, 0, nullptr, 0, wcFileName, MAX_PATH, nullptr, 0);
		
		// Pass the Prototype_Id (Ex: "Prototype_Component_Model_Fiona")
		wstring wsModelPrototypeId(iter->wcModelPrototypeId);
		m_lModelPrototypeIds.push_back(wsModelPrototypeId);
		list<wstring>::iterator modelPrototypeIter = find_if(m_lModelPrototypeIds.begin(), m_lModelPrototypeIds.end(), [&](wstring wsProtoId) {
			return wsProtoId == wsModelPrototypeId;
		});

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		// Check if Prototype_Id is already present
		CComponent* pComponent = pGameInstance->Find_Component_Prototype(LEVEL_TOOL, wsModelPrototypeId.c_str());
		if (pComponent)
		{
			// If Yes: Clone
			if (FAILED(pGameInstance->Add_GameObject(wcFileName, TEXT("Prototype_GameObject_Mesh"), LEVEL_TOOL, layerIter->c_str(), &(*iter))))
				return; 
		}
		else
		{
			// If No: Create Prototype and Clone
			char sMeshPath[MAX_PATH];

			wstring wsFilePath = wstring(iter->wcFilePath);
			string sFilePath = string(wsFilePath.begin(), wsFilePath.end());

			wstring wsFileName = wstring(iter->wcFileName);
			string sFileName = string(wsFileName.begin(), wsFileName.end());

			strcpy_s(sMeshPath, MAX_PATH, sFilePath.c_str());
			strcat_s(sMeshPath, MAX_PATH, sFileName.c_str());

			// Prototype Component (Model)
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, modelPrototypeIter->c_str(), CModel::Create(m_pDevice, m_pContext, iter->eModelType, sMeshPath))))
				return;

			// Clone GameObject
			if (FAILED(pGameInstance->Add_GameObject(wcFileName, TEXT("Prototype_GameObject_Mesh"), LEVEL_TOOL, layerIter->c_str(), &(*iter))))
				return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

_bool CImGuiManager::SaveData()
{
	wstring wsCurrentMap = wstring(m_sCurrentMap.begin(), m_sCurrentMap.end()); // "Field"

	// Save Map Data
	HANDLE hFileMap = nullptr;
	_tchar LoadPathMap[MAX_PATH] = TEXT("../../Data/MapData/");
	wcscat_s(LoadPathMap, MAX_PATH, wsCurrentMap.c_str()); // ../../Data/MapData/Field.dat

	hFileMap = CreateFile(LoadPathMap, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFileMap == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwByte = 0;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CObject_Manager::LAYERS mLayers = pGameInstance->Get_Layers(LEVEL_TOOL);

	// Loop over Layers
	for (auto& iter = mLayers.begin(); iter != mLayers.end(); iter++)
	{
		CLayer* pLayer = iter->second;
		CLayer::GAMEOBJECTS vGameObjects = pLayer->Get_Objects();

		// Loop over Layer Objects
		for (auto& pObject : vGameObjects)
		{
			// Don't save the Camera Object to File
			if (!wcscmp(pObject->Get_ObjName(), TEXT("Camera_Dynamic")))
				continue;
			
			list<CMesh::MODELDESC>::iterator iter = find_if(m_lObjects.begin(), m_lObjects.end(), [&](CMesh::MODELDESC tDesc) {
				return !wcscmp(pObject->Get_ObjName(), tDesc.wcObjName);
			});

			// Make a new MODELDESC (based on the already existing MODELDESC of the Object) which will be updated with Layer, WorldMatrix and Rotation Data.
			// (The already existing MODELDESC of the Object cannot be used cause it's universal, setting a specific Layer or WorldMatrix for that Object would be wrong).

			CMesh::MODELDESC tNewModelDesc; 
			ZeroMemory(&tNewModelDesc, sizeof(CMesh::MODELDESC));
			memcpy(&tNewModelDesc, &(*iter), sizeof(CMesh::MODELDESC));

			// Add Layer to New MODELDESC
			wcscpy_s(tNewModelDesc.wcLayerTag, MAX_PATH, pObject->Get_LayerId());

			// Add WorldMatrix of the Object to New MODELDESC
			_float4x4 mWorldMatrix;

			CComponent* pComponent = pObject->Find_Component(TEXT("Com_Transform"));
			if (!pComponent)
				XMStoreFloat4x4(&mWorldMatrix, XMMatrixIdentity());

			CTransform* pTransform = dynamic_cast<CTransform*>(pComponent);
			if (!pTransform)
				XMStoreFloat4x4(&mWorldMatrix, XMMatrixIdentity());

			mWorldMatrix = pTransform->Get_World4x4();
			tNewModelDesc.mWorldMatrix = mWorldMatrix;

			// Add Rotation of the Object to New MODELDESC
			tNewModelDesc.vRotation = _float3(pTransform->Get_CurrentRotationX(), pTransform->Get_CurrentRotationY(), pTransform->Get_CurrentRotationZ());

			WriteFile(hFileMap, &tNewModelDesc, sizeof(CMesh::MODELDESC), &dwByte, nullptr);
		}	
	}

	CloseHandle(hFileMap);

	// Save Navigation Data
	if (!m_vNavigationCells.empty())
	{
		HANDLE hFileNavigation = nullptr;
		_tchar LoadPathNavigation[MAX_PATH] = TEXT("../../Data/NavigationData/");
		wcscat_s(LoadPathNavigation, MAX_PATH, wsCurrentMap.c_str()); // ../../Data/NavigationData/Field.dat

		hFileNavigation = CreateFile(LoadPathNavigation, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFileNavigation == INVALID_HANDLE_VALUE)
			return false;

		dwByte = 0;
		pair<CELL_TYPE, _float3x3> vPoints;

		for (auto& pCell : m_vNavigationCells)
		{
			ZeroMemory(&vPoints, sizeof(pair<CELL_TYPE, _float3x3>));
			vPoints.first = pCell.first;
			memcpy(&vPoints.second.m[0], &pCell.second.m[0], sizeof(_float3));
			memcpy(&vPoints.second.m[1], &pCell.second.m[1], sizeof(_float3));
			memcpy(&vPoints.second.m[2], &pCell.second.m[2], sizeof(_float3));

			WriteFile(hFileNavigation, &vPoints, sizeof(pair<CELL_TYPE, _float3x3>), &dwByte, nullptr);
		}

		CloseHandle(hFileNavigation);
	}

	// Save Trigger Data
	if (!m_vTriggers.empty())
	{
		HANDLE hFileTrigger = nullptr;
		_tchar LoadPathTrigger[MAX_PATH] = TEXT("../../Data/TriggerData/");
		wcscat_s(LoadPathTrigger, MAX_PATH, wsCurrentMap.c_str()); // ../../Data/TriggerData/Field.dat

		hFileTrigger = CreateFile(LoadPathTrigger, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFileTrigger == INVALID_HANDLE_VALUE)
			return false;

		dwByte = 0;
		CTriggerBox::TRIGGERBOXDESC tTriggerBox;

		for (CTriggerBox::TRIGGERBOXDESC tTrigger : m_vTriggers)
		{
			ZeroMemory(&tTriggerBox, sizeof(CTriggerBox::TRIGGERBOXDESC));
			memcpy(&tTriggerBox, &tTrigger, sizeof(CTriggerBox::TRIGGERBOXDESC));
			
			WriteFile(hFileTrigger, &tTriggerBox, sizeof(CTriggerBox::TRIGGERBOXDESC), &dwByte, nullptr);
		}

		CloseHandle(hFileTrigger);
	}
	
	return true;
}

_bool CImGuiManager::LoadData()
{
	// Destroy all Objects in every Layer before Loading from File
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CObject_Manager::LAYERS mLayers = pGameInstance->Get_Layers(LEVEL_TOOL);

	for (auto& iter = mLayers.begin(); iter != mLayers.end(); iter++)
	{
		CLayer* pLayer = iter->second;
		CLayer::GAMEOBJECTS vGameObjects = pLayer->Get_Objects();

		// Loop over Layer Objects
		for (auto& pObject : vGameObjects)
		{
			if (!wcscmp(pObject->Get_ObjName(), TEXT("Camera_Dynamic")))
				continue;

			pGameInstance->Delete_GameObject(pObject, LEVEL_TOOL, iter->first);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	
	wstring wsCurrentMap = wstring(m_vMaps[m_iSelectedMap].begin(), m_vMaps[m_iSelectedMap].end());

	// Load Map Data
	HANDLE hFileMap = nullptr;
	_tchar LoadPathMap[MAX_PATH] = TEXT("../../Data/MapData/");
	wcscat_s(LoadPathMap, MAX_PATH, wsCurrentMap.c_str()); // ../../Data/MapData/Field.dat

	hFileMap = CreateFile(LoadPathMap, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFileMap == INVALID_HANDLE_VALUE)
		return false;

	CMesh::MODELDESC tNewModelDesc;
	ZeroMemory(&tNewModelDesc, sizeof(CMesh::MODELDESC));

	DWORD dwByte = 0;

	while (true)
	{
		ReadFile(hFileMap, &tNewModelDesc, sizeof(CMesh::MODELDESC), &dwByte, nullptr);

		if (!dwByte)
			break;

		// Populate Layers Combo
		wstring wsLayer = wstring(tNewModelDesc.wcLayerTag);
		string sLayer = string(wsLayer.begin(), wsLayer.end());
		m_vLayers.insert(sLayer);
		m_vLayersTemp.insert(tNewModelDesc.wcLayerTag);

		// Get Layer Iterator which will be passed to the Engine (so it doesn't get deleted when out of scope)
		unordered_set<wstring>::iterator layerIter = m_vLayersTemp.find(wsLayer);

		// Populate Model Prototype Ids
		m_lModelPrototypeIds.push_back(tNewModelDesc.wcModelPrototypeId);
		
		// Get Model Prototype Iterator which will be passed to the Engine (so it doesn't get deleted when out of scope)
		list<wstring>::iterator modelPrototypeIter = find_if(m_lModelPrototypeIds.begin(), m_lModelPrototypeIds.end(), [&](wstring wsProtoId) {
			return wsProtoId == tNewModelDesc.wcModelPrototypeId;
		});
	
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CComponent* pComponent = pGameInstance->Find_Component_Prototype(LEVEL_TOOL, tNewModelDesc.wcModelPrototypeId);
		if (pComponent)
		{
			// If Yes: Clone
			if (FAILED(pGameInstance->Add_GameObject(tNewModelDesc.wcObjName, TEXT("Prototype_GameObject_Mesh"), LEVEL_TOOL, layerIter->c_str(), &tNewModelDesc)))
				return false;
		}
		else
		{
			// If No: Create Prototype and Clone
			char sMeshPath[MAX_PATH];

			wstring wsFilePath = wstring(tNewModelDesc.wcFilePath);
			string sFilePath = string(wsFilePath.begin(), wsFilePath.end());

			wstring wsFileName = wstring(tNewModelDesc.wcFileName);
			string sFileName = string(wsFileName.begin(), wsFileName.end());

			strcpy_s(sMeshPath, MAX_PATH, sFilePath.c_str());
			strcat_s(sMeshPath, MAX_PATH, sFileName.c_str());

			// Prototype Component (Model)
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, modelPrototypeIter->c_str(), CModel::Create(m_pDevice, m_pContext, tNewModelDesc.eModelType, sMeshPath))))
				return false;

			// Clone GameObject
			if (FAILED(pGameInstance->Add_GameObject(tNewModelDesc.wcObjName, TEXT("Prototype_GameObject_Mesh"), LEVEL_TOOL, layerIter->c_str(), &tNewModelDesc)))
				return false;
		}

		RELEASE_INSTANCE(CGameInstance);
	}

	CloseHandle(hFileMap);

	// Load Navigation Data
	HANDLE hFileNavigation = nullptr;
	_tchar LoadPathNavigation[MAX_PATH] = TEXT("../../Data/NavigationData/");
	wcscat_s(LoadPathNavigation, MAX_PATH, wsCurrentMap.c_str()); // ../../Data/NavigationData/Field.dat

	hFileNavigation = CreateFile(LoadPathNavigation, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFileNavigation == INVALID_HANDLE_VALUE)
		return false;

	dwByte = 0;

	pair<CELL_TYPE, _float3x3> vPoints;

	while (true)
	{
		ReadFile(hFileNavigation, &vPoints, sizeof(pair<CELL_TYPE, _float3x3>), &dwByte, nullptr);

		if (!dwByte)
			break;

		pair<CELL_TYPE, _float3x3> vCell;
		ZeroMemory(&vCell, sizeof(pair<CELL_TYPE, _float3x3>));
		vCell.first = vPoints.first;
		memcpy(&vCell.second.m[0], &vPoints.second.m[0], sizeof(_float3));
		memcpy(&vCell.second.m[1], &vPoints.second.m[1], sizeof(_float3));
		memcpy(&vCell.second.m[2], &vPoints.second.m[2], sizeof(_float3));

		m_vNavigationCells.push_back(vCell);
	}

	CloseHandle(hFileNavigation);

	// Load Trigger Data
	HANDLE hFileTrigger = nullptr;
	_tchar LoadPathTrigger[MAX_PATH] = TEXT("../../Data/TriggerData/");
	wcscat_s(LoadPathTrigger, MAX_PATH, wsCurrentMap.c_str()); // ../../Data/TriggerData/Field.dat

	hFileTrigger = CreateFile(LoadPathTrigger, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFileTrigger == INVALID_HANDLE_VALUE)
		return false;

	dwByte = 0;
	CTriggerBox::TRIGGERBOXDESC tTriggerBox;

	while (true)
	{
		ZeroMemory(&tTriggerBox, sizeof(CTriggerBox::TRIGGERBOXDESC));
		ReadFile(hFileTrigger, &tTriggerBox, sizeof(CTriggerBox::TRIGGERBOXDESC), &dwByte, nullptr);

		if (!dwByte)
			break;

		m_vTriggers.push_back(tTriggerBox);
	}

	CloseHandle(hFileTrigger);

	return true;
}

void CImGuiManager::Add_NavigationPoint(_float3 vPoint)
{
	/* If the Cell that is getting drawn IS the first Cell. */
	if (m_vNavigationCells.size() == 0)
	{
		/* Add a Point to "m_vCurrentCell". */
		m_vCurrentCell.push_back(vPoint);

		/* If, after adding the Point there are 3 Points in "m_vCurrentCell": Add a Cell to "m_vNavigationCells" and empty "m_vCurrentCell". */
		if (m_vCurrentCell.size() == 3)
		{
			pair<CELL_TYPE, _float3x3> vCell;
			ZeroMemory(&vCell, sizeof(pair<CELL_TYPE, _float3x3>));
			vCell.first = CELL_TYPE::CELL_DEFAULT; /* Set Default Cell by Default (KEKW) */
			memcpy(vCell.second.m[0], &m_vCurrentCell[0], sizeof(_float3));
			memcpy(vCell.second.m[1], &m_vCurrentCell[1], sizeof(_float3));
			memcpy(vCell.second.m[2], &m_vCurrentCell[2], sizeof(_float3));

			m_vNavigationCells.push_back(vCell);
			m_vCurrentCell.clear();
		}
	}
	/* If the Cell that is getting drawn is NOT the first Cell. */
	else
	{
		if (m_bVertexSnapping)
		{
			/* Check if the new Point is supposed to refer to an already existing Point or is a new one. */
			_float3 vExistingPoint;

			/* If the new Point refers to an already existing Point, add the already existing point to "m_vCurrentCell". */
			if (Check_ExistingPoints(vPoint, vExistingPoint))
				m_vCurrentCell.push_back(vExistingPoint);
			/* If not, add the new Point to "m_vCurrentCell". */
			else
				m_vCurrentCell.push_back(vPoint);
		}
		else
			m_vCurrentCell.push_back(vPoint);

		/* If, it's the first Point being added, try to compute the Cell Type. */
		if (m_vCurrentCell.size() == 1)
			Compute_CellType();
		else if (m_vCurrentCell.size() == 3) /* If, after adding the Point there are 3 Points in "m_vCurrentCell": Add a Cell to "m_vNavigationCells" and empty "m_vCurrentCell". */
		{
			pair<CELL_TYPE, _float3x3> vCell;
			ZeroMemory(&vCell, sizeof(pair<CELL_TYPE, _float3x3>));
			vCell.first = CELL_TYPE::CELL_DEFAULT; /* Set Default Cell by Default (KEKW) */
			memcpy(vCell.second.m[0], &m_vCurrentCell[0], sizeof(_float3));
			memcpy(vCell.second.m[1], &m_vCurrentCell[1], sizeof(_float3));
			memcpy(vCell.second.m[2], &m_vCurrentCell[2], sizeof(_float3));

			m_vNavigationCells.push_back(vCell);
			m_vCurrentCell.clear();
		}
	}
}

void CImGuiManager::Compute_CellType()
{
	_float fDistance = 0.f;

	_vector vRayPos = XMLoadFloat3(&m_vCurrentCell.front());
	vRayPos += XMVectorSet(0.f, 10.f, 0.f, 1.f);
	vRayPos = XMVectorSetW(vRayPos, 1.f);

	_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	for (_uint i = 0; i < m_vNavigationCells.size(); i++)
	{
		_vector vPointA = XMLoadFloat3(&(_float3)m_vNavigationCells[i].second.m[0]);
		vPointA = XMVectorSetW(vPointA, 1.f);
		_vector vPointB = XMLoadFloat3(&(_float3)m_vNavigationCells[i].second.m[1]);
		vPointB = XMVectorSetW(vPointB, 1.f);
		_vector vPointC = XMLoadFloat3(&(_float3)m_vNavigationCells[i].second.m[2]);
		vPointC = XMVectorSetW(vPointC, 1.f);

		if (TriangleTests::Intersects(vRayPos, vRayDir, vPointA, vPointB, vPointC, fDistance))
		{
			m_eCellType = m_vNavigationCells[i].first;
			m_iSelectedCellIndex = i;
		}
	}
}

void CImGuiManager::Set_CellType(_uint iCellIndex)
{
	if (m_vCurrentCell.empty())
		return; 

	m_vNavigationCells[iCellIndex].first = m_eCellType;
	m_vCurrentCell.pop_back();
}

_bool CImGuiManager::Check_ExistingPoints(_float3 vNewPoint, OUT _float3& vExistingPoint, _float fDistance)
{
	for (auto& vCell : m_vNavigationCells)
	{
		_float3 vPointA, vPointB, vPointC;
		ZeroMemory(&vPointA, sizeof(_float3));
		ZeroMemory(&vPointB, sizeof(_float3));
		ZeroMemory(&vPointC, sizeof(_float3));
		memcpy(&vPointA, &vCell.second._11, sizeof(_float3));
		memcpy(&vPointB, &vCell.second._21, sizeof(_float3));
		memcpy(&vPointC, &vCell.second._31, sizeof(_float3));

		_float fDistanceFromA = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPointA) - XMLoadFloat3(&vNewPoint)));
		_float fDistanceFromB = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPointB) - XMLoadFloat3(&vNewPoint)));
		_float fDistanceFromC = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPointC) - XMLoadFloat3(&vNewPoint)));

		/* New Point DOES refer to an already existing Point (A). */
		if (fDistanceFromA < fDistance && fDistanceFromA < fDistanceFromB && fDistanceFromA < fDistanceFromC)
		{
			vExistingPoint = vPointA;
			return true;
		}
		/* New Point DOES refer to an already existing Point (B). */
		if (fDistanceFromB < fDistance && fDistanceFromB < fDistanceFromA && fDistanceFromB < fDistanceFromC)
		{
			vExistingPoint = vPointB;
			return true;
		}
		/* New Point DOES refer to an already existing Point (C). */
		if (fDistanceFromC < fDistance && fDistanceFromC < fDistanceFromB && fDistanceFromC < fDistanceFromA)
		{
			vExistingPoint = vPointC;
			return true;
		}
	}
	
	/* New Point does NOT refer to an already existing Point. */
	return false;
}

void CImGuiManager::Set_TriggerPosition(_float3 vPosition)
{
	m_bCurrentActive = true;  
	m_vCurrentTrigger = vPosition;
}

void CImGuiManager::Add_Trigger(const char* pTriggerName)
{
	_matrix	ScaleMatrix, TranslationMatrix;
	ScaleMatrix = XMMatrixIdentity();
	TranslationMatrix = XMMatrixTranslation(m_vCurrentTrigger.x, m_vCurrentTrigger.y, m_vCurrentTrigger.z);

	_matrix WorldMatrix = ScaleMatrix * TranslationMatrix;

	BoundingBox* pTrigger = new BoundingBox(_float3(0.f, 0.5f, 0.f), _float3(0.5f, 0.5f, 0.5f));
	pTrigger->Transform(*pTrigger, WorldMatrix);

	CTriggerBox::TRIGGERBOXDESC tTriggerBoxDesc;
	ZeroMemory(&tTriggerBoxDesc, sizeof(CTriggerBox::TRIGGERBOXDESC));
	strcpy_s((char*)tTriggerBoxDesc.pTriggerName, MAX_PATH, pTriggerName);
	XMStoreFloat4x4(&tTriggerBoxDesc.mWorldMatrix, WorldMatrix);
	tTriggerBoxDesc.pTriggerBox = *pTrigger;

	m_vTriggers.push_back(tTriggerBoxDesc);

	m_bCurrentActive = false;
}

void CImGuiManager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// CleanupDeviceD3D();
	//DestroyWindow(g_hWnd);
	//UnregisterClass(wc.lpszClassName, wc.hInstance);

	Safe_Release(m_pInputLayout);
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}