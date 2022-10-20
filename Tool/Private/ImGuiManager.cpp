#include "stdafx.h"

#include "ImGuiManager.h"
#include "GameInstance.h"

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

	//m_temp = io.ConfigFlags;
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

void CImGuiManager::Render()
{
	// Frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DrawEditor();

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
	ImGui::Separator();
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

void CImGuiManager::DrawMapTool()
{
	ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;

	// Map
	if (ImGui::CollapsingHeader("Map", TreeNodeFlags))
	{
		ImGui::Text("Maps");

		// Map List
		if (ImGui::BeginListBox("##Maps", ImVec2(-FLT_MIN, 6 * ImGui::GetTextLineHeightWithSpacing())))
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

		// Buttons
		if (ImGui::Button("Create Map"))
			ImGui::OpenPopup("Create Map");
		ImGui::SameLine();

		if (ImGui::Button("Delete Map"))
			ImGui::OpenPopup("Delete Map?");
		ImGui::SameLine();

		if (ImGui::Button("Save Map"))
			ImGui::OpenPopup("Save Map");
		ImGui::SameLine();

		if (ImGui::Button("Load Map"))
			ImGui::OpenPopup("Load Map");
	}
	ImGui::Separator();

	// Object
	if (ImGui::CollapsingHeader("Object", TreeNodeFlags))
	{
		ImGui::Text("Picked: %f, %f, %f", m_vPositionPicked.x, m_vPositionPicked.y, m_vPositionPicked.z);
		ImGui::Separator();
		ImGui::NewLine();

		// Meshes List
		// Created Mesh
	}
	ImGui::Separator();

	// Navigation Mesh
	if (ImGui::CollapsingHeader("Navigation Mesh", TreeNodeFlags))
	{
		
	}
	ImGui::Separator();

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
			// Add Map
			m_vMaps.push_back(sMapName);
										 
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

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

	// Delete Modal
	ImGui::SetNextWindowPos(pCenter, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // Center Window when appearing
	if (ImGui::BeginPopupModal("Delete Map?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Are you sure you want to delete this Map?\nThis operation cannot be undone!\n");
		ImGui::NewLine();
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			m_vMaps.erase(m_vMaps.begin() + m_iSelectedMap); // Delete Map
															 // TODO: ..

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
		{
			// TODO: ..

			ImGui::CloseCurrentPopup();
		}
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