#include "stdafx.h"

#include "ImGuiManager.h"
#include "GameInstance.h"
#include "Terrain.h"

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

	io.FontDefault = io.Fonts->AddFontFromFileTTF("../Bin/Resources/Fonts/Quicksand-Medium.ttf", 16.0f);

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

void CImGuiManager::Render()
{
	// Frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DrawEditor();
	DrawOverlayWindow();

	ImGui::EndFrame();

	// Render
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGuiManager::DrawEditor()
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
			DrawMapTool();
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

		// Picking
		ImGui::Text("Current Position: %.2f, %.2f, %.2f", m_vPositionPicked.x, m_vPositionPicked.y, m_vPositionPicked.z);
	}

	ImGui::End();
}

void CImGuiManager::DrawMapTool()
{
	ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;

	// Map
	if (ImGui::CollapsingHeader("Map", TreeNodeFlags))
	{
		ImGui::Text("New Map");
		
		// Inputs
		ImGui::PushItemWidth(120);
		ImGui::InputInt("X-Axis Vertices", &m_iXVertex);
		ImGui::PushItemWidth(120);
		ImGui::InputInt("Z-Axis Vertices", &m_iZVertex);

		// Buttons
		if (ImGui::Button("Create Map"))
			ImGui::OpenPopup("Create Map");
		
		ImGui::NewLine();
		ImGui::Separator();
		
		// Map List
		ImGui::Text("Saved Maps");
		if (ImGui::BeginListBox("##Maps", ImVec2(-FLT_MIN, 4 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (int i = 0; i < m_vMaps.size(); i++)
			{
				const bool bIsSelected = (m_iSelectedMap == i);
				if (ImGui::Selectable(m_vMaps[i].c_str(), bIsSelected))
					m_iSelectedMap = i;

				if (bIsSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Load Map"))
			ImGui::OpenPopup("Load Map");

		ImGui::NewLine();
		ImGui::Separator();
	}

	// Object
	if (ImGui::CollapsingHeader("Object", TreeNodeFlags))
	{
		// Objects List
		{
			ImGui::BeginChild("Objects List", ImVec2(0, 100), true);
			for (int i = 0; i < m_vObjects.size(); i++)
			{
				if (ImGui::Selectable(m_vObjects[i].c_str(), m_iSelectedObject == i))
					m_iSelectedObject = i;
			}
			ImGui::EndChild();
		}

		if (ImGui::Button("Create Object"))
		{
			// TODO: ..
		}
		ImGui::NewLine();
		ImGui::Separator();

		// Created Objects
		ImGui::Text("Created Objects");
		if (ImGui::BeginListBox("##Created Objects", ImVec2(-FLT_MIN, 4 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (int i = 0; i < m_vCreatedObjects.size(); i++)
			{
				const bool bIsSelected = (m_iSelectedCreatedObject == i);
				if (ImGui::Selectable(m_vCreatedObjects[i].c_str(), bIsSelected))
					m_iSelectedCreatedObject = i;

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (bIsSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}

		if (ImGui::Button("Delete Object"))
		{
			auto& iter = m_vCreatedObjects.begin();
			iter += m_iSelectedCreatedObject;

			m_vCreatedObjects.erase(iter);
		}

		ImGui::NewLine();

		// Transformations
		if (ImGui::RadioButton("Translation", m_eObjAction == OBJ_TRANSLATION)) 
			m_eObjAction = OBJ_TRANSLATION; 
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotation", m_eObjAction == OBJ_ROTATION))
			m_eObjAction = OBJ_ROTATION;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", m_eObjAction == OBJ_SCALE))
			m_eObjAction = OBJ_SCALE;

		static float fX = 1.f, fY = 1.f, fZ = 1.f;
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##X", &fX, 0.005f, 0.f, 0.f, "X: %.03f");
		ImGui::SameLine(); 
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Y", &fY, 0.005f, 0.f, 0.f, "Y: %.03f");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Z", &fZ, 0.005f, 0.f, 0.f, "Z: %.03f");
		ImGui::NewLine();
		
		// Created Mesh
		// TODO: ..
	}
	ImGui::Separator();

	// Navigation Mesh
	if (ImGui::CollapsingHeader("Navigation Mesh"))
	{
		
	}
	ImGui::Separator();
	ImGui::NewLine();

	ImGui::Text("Current Map:");
	if (ImGui::Button("Save Map"))
		ImGui::OpenPopup("Save Map");
	
	DrawMapModals();

	ImGui::EndTabItem();
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
	// Create Modal
	ImVec2 pCenter = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Create Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Insert Map Name");
		static char sMapName[128] = "";
		ImGui::InputText("##Map Name", sMapName, sizeof(sMapName));
		ImGui::NewLine();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			// Destroy current Map
			CGameObject* pObject = pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Terrain"));
			pGameInstance->Delete_GameObject(LEVEL_TOOL, TEXT("Layer_Terrain"));

			if (pObject)
				Safe_Release(pObject);

			// Create Terrain				 
			CComponent* pComponent = pGameInstance->Find_Component_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"));
			CVIBuffer_Terrain* pVIBufferTerrain = dynamic_cast<CVIBuffer_Terrain*>(pComponent);
			if (!pVIBufferTerrain)
				return;

			pVIBufferTerrain->Set_NumVerticesX(m_iXVertex);
			pVIBufferTerrain->Set_NumVerticesZ(m_iZVertex);
			pVIBufferTerrain->Refresh_Vertices();

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_TOOL, TEXT("Layer_Terrain"), nullptr)))
				return;

			Safe_Release(pGameInstance);

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	// Save Modal
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Save Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Map Saved!\n");
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			// TODO: ..

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}

	// Load Modal
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Load Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Map Loaded!\n");
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
			ImGui::CloseCurrentPopup();

		
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
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