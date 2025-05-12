#include "Core/Editor/Editor.h"

Editor* Editor::m_instance;

Editor::Editor() {
    this->m_bMenuOpen = false;
    this->m_sceneMgr = nullptr;
    this->m_inspectorObj = nullptr;
    this->m_nWidth = 0;
    this->m_nHeight = 0;
}

void Editor::Init(UINT nWidth, UINT nHeight) {
    this->m_sceneMgr = SceneManager::GetInstance();

    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig fontCfg = { };
    fontCfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF(Roboto, nRobotoSize, 20.f, &fontCfg);

    /*
        https://github.com/ocornut/imgui/issues/707#issuecomment-2732535348
    */
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.93f, 0.94f, 1.00f);                  // Light grey text for readability
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.52f, 0.54f, 1.00f);          // Subtle grey for disabled text
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);              // Dark background with a hint of blue
    colors[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);               // Slightly lighter for child elements
    colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);               // Popup background
    colors[ImGuiCol_Border] = ImVec4(0.28f, 0.29f, 0.30f, 0.60f);                // Soft border color
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);          // No border shadow
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);               // Frame background
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.24f, 0.26f, 1.00f);        // Frame hover effect
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.26f, 0.28f, 1.00f);         // Active frame background
    colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);               // Title background
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);         // Active title background
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);      // Collapsed title background
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);             // Menu bar background
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);           // Scrollbar background
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.26f, 0.28f, 1.00f);         // Dark accent for scrollbar grab
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.30f, 0.32f, 1.00f);  // Scrollbar grab hover
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.32f, 0.34f, 0.36f, 1.00f);   // Scrollbar grab active
    colors[ImGuiCol_CheckMark] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);             // Dark blue checkmark
    colors[ImGuiCol_SliderGrab] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);            // Dark blue slider grab
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);      // Active slider grab
    colors[ImGuiCol_Button] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);                // Dark blue button
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);         // Button hover effect
    colors[ImGuiCol_ButtonActive] = ImVec4(0.32f, 0.42f, 0.52f, 1.00f);          // Active button
    colors[ImGuiCol_Header] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);                // Header color similar to button
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);         // Header hover effect
    colors[ImGuiCol_HeaderActive] = ImVec4(0.32f, 0.42f, 0.52f, 1.00f);          // Active header
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);             // Separator color
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);      // Hover effect for separator
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);       // Active separator
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);            // Resize grip
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);     // Hover effect for resize grip
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.44f, 0.54f, 0.64f, 1.00f);      // Active resize grip
    colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);                   // Inactive tab
    colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);            // Hover effect for tab
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);             // Active tab color
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);          // Unfocused tab
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);    // Active but unfocused tab
    colors[ImGuiCol_PlotLines] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);             // Plot lines
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);      // Hover effect for plot lines
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);         // Histogram color
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);  // Hover effect for histogram
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);         // Table header background
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);     // Strong border for tables
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.24f, 0.25f, 0.26f, 1.00f);      // Light border for tables
    colors[ImGuiCol_TableRowBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);            // Table row background
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.22f, 0.24f, 0.26f, 1.00f);         // Alternate row background
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.34f, 0.44f, 0.35f);        // Selected text background
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.46f, 0.56f, 0.66f, 0.90f);        // Drag and drop target
    colors[ImGuiCol_NavHighlight] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);          // Navigation highlight
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f); // Windowing highlight
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);     // Dim background for windowing
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);      // Dim background for modal windows

    // Style adjustments
    style->WindowPadding = ImVec2(8.00f, 8.00f);
    style->FramePadding = ImVec2(5.00f, 2.00f);
    style->CellPadding = ImVec2(6.00f, 6.00f);
    style->ItemSpacing = ImVec2(6.00f, 6.00f);
    style->ItemInnerSpacing = ImVec2(6.00f, 6.00f);
    style->TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style->IndentSpacing = 25;
    style->ScrollbarSize = 11;
    style->GrabMinSize = 10;
    style->WindowBorderSize = 1;
    style->ChildBorderSize = 1;
    style->PopupBorderSize = 1;
    style->FrameBorderSize = 1;
    style->TabBorderSize = 1;
    style->WindowRounding = 7;
    style->ChildRounding = 4;
    style->FrameRounding = 3;
    style->PopupRounding = 4;
    style->ScrollbarRounding = 9;
    style->GrabRounding = 3;
    style->LogSliderDeadzone = 4;
    style->TabRounding = 4;

    this->m_nWidth = nWidth;
    this->m_nHeight = nHeight;
}

void Editor::Update() {
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Create")) {
        }
        if (ImGui::MenuItem("Open", "Ctrl+O")) {
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
        }
        if (ImGui::MenuItem("Save as..")) {
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("GameObject")) {

    }
    if (ImGui::BeginMenu("Build")) {

    }
    ImGui::EndMainMenuBar();

    if (this->m_sceneMgr) {
        Scene* scene = this->m_sceneMgr->GetCurrentScene();
        Camera* camera = scene->GetCurrentCamera();
        Transform cameraTransform = camera->transform;

        XMVECTOR eye = XMVectorSet(cameraTransform.location.x, cameraTransform.location.y, cameraTransform.location.z, 1.0f);
        float pitch = XMConvertToRadians(cameraTransform.rotation.x);
        float yaw = XMConvertToRadians(cameraTransform.rotation.y);
        XMVECTOR forward = XMVectorSet(cosf(pitch) * sinf(yaw), -sinf(pitch), -cosf(pitch) * cosf(yaw), 0.0f);
        XMVECTOR at = XMVectorAdd(eye, forward);
        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        this->m_wvp.View = XMMatrixLookAtLH(eye, at, up);
        this->m_wvp.Projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(70.f), static_cast<float>(this->m_nWidth) / this->m_nHeight, 0.01f, 3000.f);

        float viewMatrix[16];
        float projectionMatrix[16];
        XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(viewMatrix), this->m_wvp.View);
        XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(projectionMatrix), this->m_wvp.Projection);

        ImGuizmo::Enable(true);
        ImGui::SetNextWindowSize(ImVec2{ 300.f, 600.f });
        ImGui::Begin("Scene");

        for (const auto& [name, object] : scene->m_gameObjects) {
            if (ImGui::Button(name.c_str())) {
                this->m_inspectorObj = object;
            }
        }

        ImGui::End();

        ImGui::SetNextWindowSize(ImVec2{ 300.f, 600.f });
        ImGui::SetNextWindowPos(ImVec2{ 1575, 180 });
        ImGui::Begin("Inspector");

        if (this->m_inspectorObj) {
            Transform* transform = &this->m_inspectorObj->transform;

            XMMATRIX worldMatrixXM = XMMatrixScaling(transform->scale.x, transform->scale.y, transform->scale.z) *
                XMMatrixRotationX(XMConvertToRadians(transform->rotation.x)) *
                XMMatrixRotationY(XMConvertToRadians(transform->rotation.y)) *
                XMMatrixRotationZ(XMConvertToRadians(transform->rotation.z)) *
                XMMatrixTranslation(transform->location.x, transform->location.y, transform->location.z);

            float worldMatrix[16];
            XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(worldMatrix), worldMatrixXM);

            ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, worldMatrix);

            if (ImGuizmo::IsUsing()) {
                float loc[3], rot[3], scale[3];
                ImGuizmo::DecomposeMatrixToComponents(worldMatrix, loc, rot, scale);
                transform->location = Vector3{ loc[0], loc[1], loc[2] };
                transform->rotation = Vector3{ rot[0], rot[1], rot[2] };
                transform->scale = Vector3{ scale[0], scale[1], scale[2] };
            }

            ImGui::Text(this->m_inspectorObj->m_name.c_str());

            ImGui::Text("Location");
            ImGui::PushItemWidth(100);
            ImGui::InputFloat("Loc X", &transform->location.x, 0.1f, 1.f, "%.2f");
            ImGui::InputFloat("Loc Y", &transform->location.y, 0.1f, 1.f, "%.2f");
            ImGui::InputFloat("Loc Z", &transform->location.z, 0.1f, 1.f, "%.2f");

            ImGui::Text("Rotation");
            ImGui::InputFloat("Rot X", &transform->rotation.x, 5.f, 10.f, "%.2f");
            ImGui::InputFloat("Rot Y", &transform->rotation.y, 5.f, 10.f, "%.2f");
            ImGui::InputFloat("Rot Z", &transform->rotation.z, 5.f, 10.f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::Text("Components");
            for (Component* component : this->m_inspectorObj->m_components) {
                ImGui::Button(component->m_name.c_str());
            }
        }
        else {
            ImGui::Text("Select an object...");
        }

        ImGui::End();
    }

}

Editor* Editor::GetInstance() {
	if (Editor::m_instance == nullptr)
		Editor::m_instance = new Editor();
	return Editor::m_instance;
}