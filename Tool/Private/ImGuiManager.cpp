#include "stdafx.h"

#include "ImGuiManager.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "Mesh.h"
#include "Layer.h"

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
	
	_tchar cResourcesPath[MAX_PATH] = TEXT("../../Resources/Zelda_Meshes/");
	Read_Objects_Name(cResourcesPath);

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

	ImGui::EndFrame();

	// Render
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGuiManager::DrawEditor(_float fTimeDelta)
{
	ImGui::Begin("Editor");

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
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

	if (ImGui::Begin("Debug Info", nullptr, window_flags))
	{
		ImGui::Text("Debug Info");
		ImGui::Separator();

		// FramesPerSecond
		ImGui::Text("FPS: %d", m_iFramesPerSecond);
	}

	ImGui::End();
}

void CImGuiManager::DrawMapTool(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;

	// Create New Map
	ImGui::Text("New Map");
	static char cMapName[MAX_PATH];
	ImGui::InputText("##InputMap", cMapName, MAX_PATH);
	ImGui::SameLine();
	if (ImGui::Button("Create Map"))
	{
		m_sCurrentMap = cMapName; // Set Current Map
		m_sCurrentMap += ".dat";

		ImGui::OpenPopup("Create Map");
	}
	ImGui::NewLine();
	ImGui::Separator();

	// Map List
	ImGui::Text("Saved Maps");
	if (ImGui::BeginListBox("##Maps", ImVec2(-FLT_MIN, 4 * ImGui::GetTextLineHeightWithSpacing())))
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

	if (ImGui::Button("Load Map"))
	{
		if (LoadData())
		{
			m_sCurrentMap = m_vMaps[m_iSelectedMap];
			ImGui::OpenPopup("Load Map");
		}
	}
	ImGui::NewLine();
	ImGui::Separator();

	ImGui::Text("Current Map: %s", m_sCurrentMap.c_str());

	if (ImGui::Button("Save Map"))
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

			ImGui::OpenPopup("Save Map");
		}
	}

	ImGui::NewLine();

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
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

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

			Safe_Release(pGameInstance);
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
			for (auto& iter = m_mObjects.begin(); iter != m_mObjects.end(); iter++)
			{
				if (ImGui::Selectable(iter->first.c_str(), m_sSelectedObject == iter->first))
					m_sSelectedObject = iter->first;

				if (m_sSelectedObject == iter->first)
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

		if (ImGui::Button("Add", ImVec2(0, 23)))
			ImGui::OpenPopup("Add Layer");

		if (ImGui::Button("Create Object"))
			Create_Object();

		ImGui::NewLine();
		ImGui::Separator();

		// Created Objects
		ImGui::Text("Created Objects");
		if (ImGui::BeginTable("CreatedObjectTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 6)))
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
					ImGui::TableNextColumn();

					wstring wsObjId = wstring(pObject->Get_ObjId());
					string sObjId = string(wsObjId.begin(), wsObjId.end());
					if (ImGui::Selectable(sObjId.c_str(), !wcscmp(pObject->Get_ObjId(), m_pSelectedCreatedObject ? m_pSelectedCreatedObject->Get_ObjId() : TEXT("")), ImGuiSelectableFlags_SpanAllColumns))
					{
						m_pSelectedCreatedObject = pObject;

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
							m_fX = m_pTransform->Get_RotationDegree(XMVectorSet(1.f, 0.f, 0.f, 0.f));
							m_fY = m_pTransform->Get_RotationDegree(XMVectorSet(0.f, 1.f, 0.f, 0.f));
							m_fZ = m_pTransform->Get_RotationDegree(XMVectorSet(0.f, 0.f, 1.f, 0.f));
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

		if (m_pSelectedCreatedObject)
		{
			wstring wsCurrentObject = wstring(m_pSelectedCreatedObject->Get_ObjId());
			string sCurrentObject = string(wsCurrentObject.begin(), wsCurrentObject.end());
			ImGui::Text("Current Object: %s", sCurrentObject.c_str());
		}
		else
			ImGui::Text("Current Object:");

		ImGui::NewLine();

		// Transformations
		_float3 vScale, vRotation, vTranslation;

		if (ImGui::RadioButton("Scale", m_eObjAction == TRANS_SCALE))
		{
			m_eObjAction = TRANS_SCALE;

			m_fX = m_pTransform->Get_Scale(CTransform::STATE::STATE_RIGHT);
			m_fY = m_pTransform->Get_Scale(CTransform::STATE::STATE_UP);
			m_fZ = m_pTransform->Get_Scale(CTransform::STATE::STATE_LOOK);
		}
		ImGui::SameLine();

		if (ImGui::RadioButton("Rotation", m_eObjAction == TRANS_ROTATION))
		{
			m_eObjAction = TRANS_ROTATION;
						
			m_fX = m_pTransform->Get_RotationDegree(XMVectorSet(1.f, 0.f, 0.f, 0.f));
			m_fY = m_pTransform->Get_RotationDegree(XMVectorSet(0.f, 1.f, 0.f, 0.f));
			m_fZ = m_pTransform->Get_RotationDegree(XMVectorSet(0.f, 0.f, 1.f, 0.f));
		}
		ImGui::SameLine();

		if (ImGui::RadioButton("Translation", m_eObjAction == TRANS_TRANSLATION))
		{
			m_eObjAction = TRANS_TRANSLATION;

			_float3 vCurrentTranslation;
			XMStoreFloat3(&vCurrentTranslation, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
			m_fX = vCurrentTranslation.x;
			m_fY = vCurrentTranslation.y;
			m_fZ = vCurrentTranslation.z;
		}

		ImGui::SetNextItemWidth(80);
		if (ImGui::DragFloat("##X", &m_fX, m_eObjAction == TRANS_ROTATION ? 1 : 0.05f, m_eObjAction == TRANS_ROTATION ? -360.f : 0.f, m_eObjAction == TRANS_ROTATION ? 360.f : 0.f, "X: %.03f"))
		{
			switch (m_eObjAction)
			{
			case TRANS_SCALE:
			{
				m_pTransform->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fX);
				break;
			}
			case TRANS_ROTATION:
			{
				_vector vRotationX = XMVectorSet(1.f, 0.f, 0.f, 0.f);
				m_pTransform->Set_Rotation(_float3(m_fX, m_fY, m_fZ));
				break;
			}
			case TRANS_TRANSLATION:
			{
				XMStoreFloat3(&vTranslation, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
				_vector vNewTranslation = XMVectorSet(m_fX, vTranslation.y, vTranslation.z, 1.f);
				m_pTransform->Set_State(CTransform::STATE::STATE_TRANSLATION, vNewTranslation);
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
					m_pTransform->Set_Scale(CTransform::STATE::STATE_UP, m_fY);
					break;
				}
				case TRANS_ROTATION:
				{
					m_pTransform->Set_Rotation(_float3(m_fX, m_fY, m_fZ));
					break;
				}
				case TRANS_TRANSLATION:
				{	
					XMStoreFloat3(&vTranslation, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
					_vector vNewTranslation = XMVectorSet(vTranslation.x, m_fY, vTranslation.z, 1.f);
					m_pTransform->Set_State(CTransform::STATE::STATE_TRANSLATION, vNewTranslation);
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
					m_pTransform->Set_Scale(CTransform::STATE::STATE_LOOK, m_fZ);
					break;
				}
				case TRANS_ROTATION:
				{
					m_pTransform->Set_Rotation(_float3(m_fX, m_fY, m_fZ));
					break;
				}
				case TRANS_TRANSLATION:
				{
					XMStoreFloat3(&vTranslation, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
					_vector vNewTranslation = XMVectorSet(vTranslation.x, vTranslation.y, m_fZ, 1.f);
					m_pTransform->Set_State(CTransform::STATE::STATE_TRANSLATION, vNewTranslation);
					break;
				}
			}
		}
		ImGui::NewLine();
		
		if (ImGui::Button("Delete Object"))
		{
			wstring wsCurrentLayer = wstring(m_sCurrentLayer.begin(), m_sCurrentLayer.end());
			pGameInstance->Delete_GameObject(m_pSelectedCreatedObject, LEVEL_TOOL, wsCurrentLayer.c_str());
			m_pSelectedCreatedObject = nullptr;
		}
	}
	
	// Navigation Mesh
	/*if (ImGui::CollapsingHeader("Navigation Mesh"))
	{
		ImGui::Separator();
	}*/
	
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
	if (ImGui::BeginPopupModal("Create Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Map Created!\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
			ImGui::CloseCurrentPopup();

		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}

	// Load Map Modal
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Load Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Map Loaded!\n");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
			ImGui::CloseCurrentPopup();

		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}

	// Save Map Modal
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Save Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Map Saved!\n");
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
}

void CImGuiManager::Read_Maps_Name()
{
	WIN32_FIND_DATA fileData;
	HANDLE hDir = FindFirstFile(TEXT("../../Data/*"), &fileData);

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

void CImGuiManager::Read_Objects_Name(_tchar* cFolderPath)
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
			Read_Objects_Name(subFilePath);
		}
		else // File
		{
			_tchar szFileExt[MAX_PATH];

			_wsplitpath_s(fileData.cFileName, nullptr, 0, nullptr, 0, nullptr, 0, szFileExt, MAX_PATH);

			if (!wcscmp(szFileExt, TEXT(".fbx")))
			{
				wstring wsFileName(fileData.cFileName);
				string sFileName(wsFileName.begin(), wsFileName.end());

				wstring wsFilePath(filePath);
				string sFilePath(wsFilePath.begin(), wsFilePath.end());

				m_mObjects.insert(pair<string, string>(sFileName, sFilePath));
			}
		}
	} while (FindNextFile(hDir, &fileData));

	FindClose(hDir);
}

void CImGuiManager::Create_Object()
{
	// Get FileName without extension (Ex: "Fiona.fbx" > "Fiona")
	map<string, string>::iterator iter;
	iter = m_mObjects.find(m_sSelectedObject);

	if (iter != m_mObjects.end())
	{
		// LayerId (Ex: "Layer_Player")
		wstring wsLayer = wstring(m_sCurrentLayer.begin(), m_sCurrentLayer.end());
		unordered_set<wstring>::iterator layerIter = m_vLayersTemp.find(wsLayer);
		const _tchar* wcLayer = layerIter->c_str();

		// FileName (Ex: "Fiona.fbx" > "Fiona")
		string sFileName = iter->first;
		wstring wsFileName(sFileName.begin(), sFileName.end());
		_tchar wcFileName[MAX_PATH];
		_wsplitpath_s(wsFileName.c_str(), nullptr, 0, nullptr, 0, wcFileName, MAX_PATH, nullptr, 0);
		
		// Use FileName to make the Prototype_Id (Ex: Prototype_Component_Model_"Fiona")
		_tchar wcPrototypeId[MAX_PATH] = TEXT("Prototype_Component_Model_");
		wcscat_s(wcPrototypeId, MAX_PATH, wcFileName);

		wstring wsPrototypeId = wstring(wcPrototypeId);
		m_vPrototypesId.push_back(wsPrototypeId);
		list<wstring>::iterator prototypeIter = find(m_vPrototypesId.begin(), m_vPrototypesId.end(), wsPrototypeId);

		// Check if Prototype_Id is already present
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CComponent* pComponent = pGameInstance->Find_Component_Prototype(LEVEL_TOOL, prototypeIter->c_str());
		if (pComponent)
		{
			// If Yes: Clone
			if (FAILED(pGameInstance->Add_GameObject(wcFileName, TEXT("Prototype_GameObject_Mesh"), LEVEL_TOOL, wcLayer, (void*)prototypeIter->c_str())))
				return; 
		}
		else
		{
			// If No: Create Prototype and Clone
			char sMeshPath[MAX_PATH];
			strcpy_s(sMeshPath, MAX_PATH, iter->second.c_str());
			strcat_s(sMeshPath, MAX_PATH, iter->first.c_str());

			// Prototype Component (Model)
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, prototypeIter->c_str(), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, sMeshPath/*, PivotMatrix*/))))
				return;

			// Clone GameObject (Mesh)
			if (FAILED(pGameInstance->Add_GameObject(wcFileName, TEXT("Prototype_GameObject_Mesh"), LEVEL_TOOL, wcLayer, (void*)prototypeIter->c_str())))
				return;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

_bool CImGuiManager::SaveData()
{
	// TODO: ..
	return true;
}

_bool CImGuiManager::LoadData()
{
	// TODO: ..
	return true;
}

void CImGuiManager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// CleanupDeviceD3D();
	//DestroyWindow(g_hWnd);
	//UnregisterClass(wc.lpszClassName, wc.hInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}