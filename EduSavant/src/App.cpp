#include "App.h"
#include "UI/Theme.h"
#include "imgui_internal.h"
#include <cstdlib>
#include <cstring>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

App::App() {
    Init();
}

App::~App() {
    Shutdown();
}

void App::Init() {
    glfwSetErrorCallback(glfw_error_callback);
    
    printf("--- EduSavant (Debug Build) ---\n");
    printf("DISPLAY: %s\n", getenv("DISPLAY"));
    printf("Setting GLFW_PLATFORM = x11\n");
    
    // Force X11 backend using environment variable
    setenv("GLFW_PLATFORM", "x11", 1);

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW. Ensure X11/Wayland is correctly configured.\n");
        std::exit(1);
    }

    // GL 3.0 + GLSL 130
    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1280, 800, "EduSavant - School Management System", nullptr, nullptr);
    if (window == nullptr)
        std::exit(1);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    Theme::Setup();
    
    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void App::Run() {
    ImGuiIO& io = ImGui::GetIO();
    ImVec4 clear_color = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create main DockSpace
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpaceOverViewport(dockspace_id, ImGui::GetMainViewport());

        static bool first_time = true;
        if (first_time)
        {
            first_time = false;
            
            ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

            ImGuiID dock_main_id = dockspace_id;
            ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
            
            ImGui::DockBuilderDockWindow("Navigation", dock_id_left);
            ImGui::DockBuilderDockWindow("Dashboard", dock_main_id);
            ImGui::DockBuilderDockWindow("Student Management", dock_main_id);
            ImGui::DockBuilderDockWindow("Staff Management", dock_main_id);
            ImGui::DockBuilderDockWindow("Settings", dock_main_id);
            
            ImGui::DockBuilderFinish(dockspace_id);
        }

        RenderSidebar();
        
        switch (currentScreen) {
            case Screen::Dashboard: RenderDashboard(); break;
            case Screen::Students:  RenderStudentList(); break;
            case Screen::Teachers:  RenderStaffList(); break; // Still using "Teachers" enum screen, but rendering Staff
            case Screen::Settings:  RenderSettings(); break;
        }

        if(showAddStudentModal) ShowAddStudentModal();
        if(showAddTeacherModal) ShowAddStaffModal(); // Using boolean to trigger Staff modal

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }
}



void App::RenderSidebar() {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::Begin("Navigation", nullptr, window_flags);
    
    // Scale up the logo text
    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextDisabled("EDU SAVANT");
    ImGui::SetWindowFontScale(1.0f);
    
    ImGui::Separator();
    ImGui::Spacing();

    // Use larger buttons
    if (ImGui::Button("Dashboard", ImVec2(-1, 50))) { 
        currentScreen = Screen::Dashboard;
        ImGui::SetWindowFocus("Dashboard"); 
    }
    ImGui::Spacing();
    if (ImGui::Button("Students", ImVec2(-1, 50))) { 
        currentScreen = Screen::Students;
        ImGui::SetWindowFocus("Student Management"); 
    }
    ImGui::Spacing();
    if (ImGui::Button("Teachers", ImVec2(-1, 50))) { 
        currentScreen = Screen::Teachers;
        ImGui::SetWindowFocus("Teacher Management"); 
    }
    ImGui::Spacing();
    ImGui::Dummy(ImVec2(0, 20)); // Spacer
    if (ImGui::Button("Settings", ImVec2(-1, 50))) { 
        currentScreen = Screen::Settings;
        ImGui::SetWindowFocus("Settings"); 
    }

    ImGui::End();
}

void App::RenderDashboard() {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::Begin("Dashboard", nullptr, window_flags);
    
    ImGui::SetWindowFontScale(2.0f);
    ImGui::Text("Welcome, Principal.");
    ImGui::SetWindowFontScale(1.1f);      
    ImGui::Separator();
    ImGui::Spacing();

    // Stats Cards
    ImGui::Columns(3, "dashboard_stats", false);
    
    ImGui::BeginGroup();
    ImGui::Text("Total Students");
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "%lu", dataManager.students.size());
    ImGui::EndGroup();
    ImGui::NextColumn();

    ImGui::BeginGroup();
    ImGui::Text("Total Staff");
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.9f, 1.0f), "%lu", dataManager.staffMembers.size());
    ImGui::EndGroup();
    ImGui::NextColumn();

    ImGui::BeginGroup();
    ImGui::Text("Average Attendance");
    float totalAttendance = 0.0f;
    for(const auto& s : dataManager.students) totalAttendance += s.getAttendance();
    float avg = dataManager.students.empty() ? 0.0f : totalAttendance / dataManager.students.size();
    ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.2f, 1.0f), "%.1f%%", avg);
    ImGui::EndGroup();
    
    ImGui::Columns(1);
    ImGui::Spacing();
    ImGui::Separator();

    ImGui::End();
}

void App::RenderStaffList() {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::Begin("Staff Management", nullptr, window_flags);

    ImGui::SetWindowFontScale(1.1f);

    if (ImGui::Button("Add New Staff", ImVec2(200, 40))) {
        showAddTeacherModal = true;
        // Reset inputs
        memset(inputStaffName, 0, sizeof(inputStaffName));
        memset(inputStaffEmail, 0, sizeof(inputStaffEmail));
        memset(inputStaffPhone, 0, sizeof(inputStaffPhone));
        memset(inputStaffRole, 0, sizeof(inputStaffRole));
        memset(inputStaffSubject, 0, sizeof(inputStaffSubject));
        strcpy(inputStaffRole, "Teacher");
    }
    
    ImGui::SameLine();
    static char searchBuffer[128] = "";
    ImGui::InputTextWithHint("##searchStaff", "Search by name...", searchBuffer, sizeof(searchBuffer));

    ImGui::Spacing();

    // Get full available region
    ImVec2 availRegion = ImGui::GetContentRegionAvail();
    
    // Wrap table in child window to fill available space
    if (ImGui::BeginChild("StaffTableRegion", availRegion, false, ImGuiWindowFlags_None)) {
        if (ImGui::BeginTable("staff_table", 6, 
            ImGuiTableFlags_Borders | 
            ImGuiTableFlags_RowBg | 
            ImGuiTableFlags_Resizable | 
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY | 
            ImGuiTableFlags_SizingStretchSame)) {
            
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Role", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Detail", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Email", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableHeadersRow();

            for (auto& s : dataManager.staffMembers) {
                if (strlen(searchBuffer) > 0 && s.getName().find(searchBuffer) == std::string::npos)
                    continue;

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%d", s.getId());
                
                ImGui::TableNextColumn();
                ImGui::Text("%s", s.getName().c_str());

                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.9f, 1.0f), "%s", s.getRole().c_str());

                ImGui::TableNextColumn();
                if (s.getRole() == "Teacher") {
                    ImGui::Text("Sub: %s", s.getSubject().c_str());
                } else {
                     ImGui::Text("Ph: %s", s.getPhone().c_str());
                }

                ImGui::TableNextColumn();
                ImGui::Text("%s", s.getEmail().c_str());

                ImGui::TableNextColumn();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                if (ImGui::Button(("Del##S" + std::to_string(s.getId())).c_str())) {
                    ImGui::OpenPopup(("DeleteStaff?" + std::to_string(s.getId())).c_str());
                }
                ImGui::PopStyleColor();

                if (ImGui::BeginPopupModal(("DeleteStaff?" + std::to_string(s.getId())).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Delete staff %s?", s.getName().c_str());
                    ImGui::Separator();
                    if (ImGui::Button("Yes", ImVec2(100,0))) {
                        dataManager.DeleteStaff(s.getId());
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", ImVec2(100,0))) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void App::RenderSettings() {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::Begin("Settings", nullptr, window_flags);
    
    ImGui::SetWindowFontScale(1.5f);
    ImGui::Text("Settings");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Separator();
    ImGui::Spacing();

    // THEME
    ImGui::TextDisabled("APPEARANCE");
    static int theme_current = 0;
    if (ImGui::Combo("Theme Mode", &theme_current, "Modern Dark\0Clean Light\0")) {
        Theme::Setup(theme_current == 0);
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ACADEMIC CONFIGURATION
    ImGui::TextDisabled("ACADEMIC CONFIGURATION");
    
    ImGui::Text("Manage Classes");
    ImGui::InputTextWithHint("##newclass", "New Class Name (e.g. 10)", inputNewClassName, sizeof(inputNewClassName));
    ImGui::SameLine();
    if (ImGui::Button("Add Class")) {
        if (strlen(inputNewClassName) > 0) {
            ClassConfig::Get().AddClass(inputNewClassName);
            dataManager.SaveClassConfig();
            memset(inputNewClassName, 0, sizeof(inputNewClassName));
        }
    }

    ImGui::Spacing();
    
    // Select Class to Configure
    auto& mapClasses = ClassConfig::Get().classesAndSections;
    std::vector<std::string> classNames;
    for(auto const& [name, _] : mapClasses) classNames.push_back(name);
    
    if (!classNames.empty()) {
        if (selectedClassConfigIndex >= classNames.size()) selectedClassConfigIndex = 0;
        
        // Create a C-string array for Combo
        // This is a bit hacky in immediate mode, loops are better for custom combos
        if (ImGui::BeginCombo("Select Class", classNames[selectedClassConfigIndex].c_str())) {
            for (int n = 0; n < classNames.size(); n++) {
                bool is_selected = (selectedClassConfigIndex == n);
                if (ImGui::Selectable(classNames[n].c_str(), is_selected))
                    selectedClassConfigIndex = n;
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        std::string currentClass = classNames[selectedClassConfigIndex];

        ImGui::Columns(2, "config_cols", false);
        
        // Sections Column
        ImGui::BeginGroup();
        ImGui::Text("Sections for Class %s", currentClass.c_str());
        ImGui::InputTextWithHint("##newsection", "New Section (e.g. A)", inputNewSectionName, sizeof(inputNewSectionName));
        ImGui::SameLine();
        if (ImGui::Button("Add##Sec")) {
            if (strlen(inputNewSectionName) > 0) {
                ClassConfig::Get().AddSection(currentClass, inputNewSectionName);
                dataManager.SaveClassConfig();
                memset(inputNewSectionName, 0, sizeof(inputNewSectionName));
            }
        }
        ImGui::TextDisabled("Current Sections:");
        auto sections = ClassConfig::Get().GetSections(currentClass);
        for(const auto& sec : sections) {
            ImGui::BulletText("%s", sec.c_str());
        }
        ImGui::EndGroup();

        ImGui::NextColumn();

        // Subjects Column (Now Dependent on Section)
        ImGui::BeginGroup();
        if (sections.empty()) {
             ImGui::TextColored(ImVec4(1,0,0,1), "Add a Section first!");
        } else {
             // Section Selector for Subjects
             static int selectedSectionIdx = 0;
             if (selectedSectionIdx >= sections.size()) selectedSectionIdx = 0;
             
             std::string currentSection = sections[selectedSectionIdx];

             ImGui::Text("Subjects for %s-%s", currentClass.c_str(), currentSection.c_str());
             
             // Simple combo to switch section focus
             if (ImGui::BeginCombo("Select Section##Subj", currentSection.c_str())) {
                for (int n = 0; n < sections.size(); n++) {
                    bool is_selected = (selectedSectionIdx == n);
                    if (ImGui::Selectable(sections[n].c_str(), is_selected))
                        selectedSectionIdx = n;
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

             ImGui::InputTextWithHint("##newsubj", "New Subject", inputNewSubjectName, sizeof(inputNewSubjectName));
             ImGui::SameLine();
             if (ImGui::Button("Add##Sub")) {
                 if (strlen(inputNewSubjectName) > 0) {
                    ClassConfig::Get().AddSubject(currentClass, currentSection, inputNewSubjectName);
                    dataManager.SaveClassConfig();
                    memset(inputNewSubjectName, 0, sizeof(inputNewSubjectName));
                }
            }
            ImGui::TextDisabled("Current Subjects:");
            for(const auto& sub : ClassConfig::Get().GetSubjects(currentClass, currentSection)) {
                ImGui::BulletText("%s", sub.c_str());
            }
        }
       
        ImGui::EndGroup();

        ImGui::Columns(1);
    } else {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "No classes defined. Add a class first.");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Data Management
    ImGui::TextDisabled("DATA MANAGEMENT");
    if (ImGui::Button("Reset All Data", ImVec2(200, 45))) {
        ImGui::OpenPopup("Confirm Reset");
    }

    if (ImGui::BeginPopupModal("Confirm Reset", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete all students and teachers?");
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "This action cannot be undone.");
        ImGui::Separator();

        if (ImGui::Button("Yes, Delete All", ImVec2(120, 0))) {
            dataManager.students.clear();
            dataManager.staffMembers.clear();
            dataManager.SaveStudents();
            dataManager.SaveStaff();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // About
    ImGui::TextDisabled("ABOUT");
    ImGui::Text("EduSavant v2.0 (Phase 2)");
    ImGui::Text("Advanced Academic Management System.");
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Made By Mr. Aayush Bhandari");

    ImGui::End();
}

void App::ShowAddStaffModal() {
    ImGui::OpenPopup("Add Staff");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Add Staff", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Name", inputStaffName, sizeof(inputStaffName));
        ImGui::InputText("Email", inputStaffEmail, sizeof(inputStaffEmail));
        ImGui::InputText("Phone", inputStaffPhone, sizeof(inputStaffPhone));
        
        const char* roles[] = { "Principal", "Teacher", "Admin", "Clerk", "Peon" };
        static int current_role_idx = 1; // Default Teacher
        if (ImGui::Combo("Role", &current_role_idx, roles, IM_ARRAYSIZE(roles))) {
            strcpy(inputStaffRole, roles[current_role_idx]);
        }
        
        // Subject only if Teacher
        if (current_role_idx == 1) { // Teacher
            ImGui::InputText("Subject", inputStaffSubject, sizeof(inputStaffSubject));
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            int maxId = 0;
            for(const auto& s : dataManager.staffMembers) if(s.getId() > maxId) maxId = s.getId();
            int newId = maxId + 1;
            
            // Should sanitize role from combo
            std::string roleStr = roles[current_role_idx];
            
            Staff s(newId, inputStaffName, inputStaffEmail, inputStaffPhone, roleStr, inputStaffSubject);
            dataManager.AddStaff(s);
            showAddTeacherModal = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}


// Helper to get subjects for student's class
const std::vector<std::string> GetSubjectsForStudent(const Student& s) {
    return ClassConfig::Get().GetSubjects(s.getClassName(), s.getSection());
}

void App::ShowStudentProfileModal() {
    if (selectedStudentId == -1) return;

    // Find student pointer
    Student* currentStudent = nullptr;
    for(auto& s : dataManager.students) {
        if(s.getId() == selectedStudentId) {
            currentStudent = &s;
            break;
        }
    }
    
    if(!currentStudent) {
        selectedStudentId = -1; // Invalid
        return;
    }

    ImGui::OpenPopup("Student Profile");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(800, 600));

    if (ImGui::BeginPopupModal("Student Profile", NULL, ImGuiWindowFlags_NoResize)) {
        
        // Header
        ImGui::TextDisabled("STUDENT ID: %d", currentStudent->getId());
        ImGui::SameLine();
        ImGui::TextDisabled("| ROLL NO: %d", currentStudent->getRollNumber());
        
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("%s", currentStudent->getName().c_str());
        ImGui::SetWindowFontScale(1.0f);
        
        ImGui::Separator();

        if (ImGui::BeginTabBar("ProfileTabs")) {
            
            // TAB 1: Personal Info
            if (ImGui::BeginTabItem("Personal")) {
                ImGui::Spacing();
                
                // Editable Fields
                static char editName[128];
                static char editFather[128];
                static char editPhone[128];
                static char editEmail[128];
                
                // Initialize buffers (only once per open ideally, but here doing per frame if changed? 
                // Better: init when modal opens. For now, simple direct copy)
                if (ImGui::IsWindowAppearing()) {
                    strcpy(editName, currentStudent->getName().c_str());
                    strcpy(editFather, currentStudent->getFatherName().c_str());
                    strcpy(editPhone, currentStudent->getPhone().c_str());
                    strcpy(editEmail, currentStudent->getEmail().c_str());
                }

                ImGui::Columns(2, "profile_info", false);
                ImGui::Text("Class:"); ImGui::NextColumn(); ImGui::Text("%s", currentStudent->getClassName().c_str()); ImGui::NextColumn();
                ImGui::Text("Section:"); ImGui::NextColumn(); ImGui::Text("%s", currentStudent->getSection().c_str()); ImGui::NextColumn();
                
                ImGui::Text("Name:"); ImGui::NextColumn(); 
                if(ImGui::InputText("##name", editName, sizeof(editName))) currentStudent->setName(editName);
                ImGui::NextColumn();

                ImGui::Text("Father's Name:"); ImGui::NextColumn(); 
                if(ImGui::InputText("##father", editFather, sizeof(editFather))) currentStudent->setFatherName(editFather);
                ImGui::NextColumn();
                
                ImGui::Text("Contact:"); ImGui::NextColumn(); 
                if(ImGui::InputText("##phone", editPhone, sizeof(editPhone))) currentStudent->setPhone(editPhone);
                ImGui::NextColumn();
               
                ImGui::Text("Email:"); ImGui::NextColumn(); 
                if(ImGui::InputText("##email", editEmail, sizeof(editEmail))) currentStudent->setEmail(editEmail);
                ImGui::NextColumn();
                
                ImGui::Columns(1);
                
                ImGui::Spacing();
                if (ImGui::Button("Save Details")) {
                    dataManager.RecalculateRollNumbers(); // Name change affects roll no
                    dataManager.SaveStudents();
                }
                
                ImGui::EndTabItem();
            }

            // TAB 2-5: Terms
            for (int term = 1; term <= 4; ++term) {
                std::string tabName = "Term " + std::to_string(term);
                if (ImGui::BeginTabItem(tabName.c_str())) {
                    ImGui::Spacing();
                    
                    std::vector<std::string> subjects = GetSubjectsForStudent(*currentStudent);
                    if (subjects.empty()) {
                        ImGui::TextColored(ImVec4(1, 1, 0, 1), "No subjects configured for Class %s", currentStudent->getClassName().c_str());
                    } else {
                        ImGui::BeginTable("marks_table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
                        ImGui::TableSetupColumn("Subject");
                        ImGui::TableSetupColumn("Marks (Out of 100)");
                        ImGui::TableHeadersRow();

                        for (const auto& sub : subjects) {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", sub.c_str());
                            
                            ImGui::TableNextColumn();
                            int currentMark = currentStudent->getMark(term, sub);
                            int startMark = currentMark; // For change detection
                            
                            std::string id = "##" + std::to_string(term) + sub;
                            if (ImGui::InputInt(id.c_str(), &currentMark, 0, 0)) {
                                if (currentMark < 0) currentMark = 0;
                                if (currentMark > 100) currentMark = 100;
                                currentStudent->setMark(term, sub, currentMark);
                            }
                        }
                        ImGui::EndTable();
                        
                        if (ImGui::Button("Save Marks")) {
                            dataManager.SaveStudents();
                        }
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }

        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Close", ImVec2(120, 0))) {
             selectedStudentId = -1;
             ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void App::RenderStudentList() {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::Begin("Student Management", nullptr, window_flags);

    ImGui::SetWindowFontScale(1.1f);

    if (ImGui::Button("Add New Student", ImVec2(200, 40))) {
        showAddStudentModal = true;
        // Reset inputs
        memset(inputName, 0, sizeof(inputName));
        memset(inputEmail, 0, sizeof(inputEmail));
        memset(inputPhone, 0, sizeof(inputPhone));
        inputClassIndex = 0;
        inputSectionIndex = 0;
        memset(inputFatherName, 0, sizeof(inputFatherName));
    }
    
    ImGui::SameLine();
    static char searchBuffer[128] = "";
    ImGui::InputTextWithHint("##search", "Search by name...", searchBuffer, sizeof(searchBuffer));

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Class and Section Filter
    ImGui::Text("Filter by:");
    ImGui::SameLine();
    
    auto& classMap = ClassConfig::Get().classesAndSections;
    std::vector<std::string> classNames;
    for(auto const& [name, _] : classMap) classNames.push_back(name);
    
    if (!classNames.empty()) {
        // Class Selector
        if (selectedFilterClassIndex >= classNames.size()) selectedFilterClassIndex = 0;
        std::string currentFilterClass = classNames[selectedFilterClassIndex];
        
        ImGui::SetNextItemWidth(150);
        if (ImGui::BeginCombo("Class##Filter", currentFilterClass.c_str())) {
            for (int n = 0; n < classNames.size(); n++) {
                bool is_selected = (selectedFilterClassIndex == n);
                if (ImGui::Selectable(classNames[n].c_str(), is_selected))
                    selectedFilterClassIndex = n;
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        
        ImGui::SameLine();
        
        // Section Selector
        auto sections = ClassConfig::Get().GetSections(currentFilterClass);
        if (!sections.empty()) {
            if (selectedFilterSectionIndex >= sections.size()) selectedFilterSectionIndex = 0;
            std::string currentFilterSection = sections[selectedFilterSectionIndex];
            
            ImGui::SetNextItemWidth(100);
            if (ImGui::BeginCombo("Section##Filter", currentFilterSection.c_str())) {
                for (int n = 0; n < sections.size(); n++) {
                    bool is_selected = (selectedFilterSectionIndex == n);
                    if (ImGui::Selectable(sections[n].c_str(), is_selected))
                        selectedFilterSectionIndex = n;
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        } else {
            ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "No sections for this class!");
        }
    } else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No classes configured! Go to Settings.");
    }

    ImGui::Spacing();

    // Get full available region
    ImVec2 availRegion = ImGui::GetContentRegionAvail();
    
    // Wrap table in child window to fill available space
    if (ImGui::BeginChild("StudentTableRegion", availRegion, false, ImGuiWindowFlags_None)) {
        if (ImGui::BeginTable("students_table", 6, 
            ImGuiTableFlags_Borders | 
            ImGuiTableFlags_RowBg | 
            ImGuiTableFlags_Resizable | 
            ImGuiTableFlags_ScrollX |
            ImGuiTableFlags_ScrollY | 
            ImGuiTableFlags_SizingStretchSame)) {
            
            ImGui::TableSetupColumn("Roll", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Class", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Section", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Father's Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableHeadersRow();

            for (auto& s : dataManager.students) {
                // Filter by selected class and section
                auto& classMap = ClassConfig::Get().classesAndSections;
                std::vector<std::string> classNames;
                for(auto const& [name, _] : classMap) classNames.push_back(name);
                
                if (!classNames.empty()) {
                    std::string filterClass = classNames[selectedFilterClassIndex];
                    auto sections = ClassConfig::Get().GetSections(filterClass);
                    
                    if (!sections.empty()) {
                        std::string filterSection = sections[selectedFilterSectionIndex];
                        
                        // Only show students from selected class and section
                        if (s.getClassName() != filterClass || s.getSection() != filterSection)
                            continue;
                    }
                }
                
                // Simple filter by name
                if (strlen(searchBuffer) > 0 && s.getName().find(searchBuffer) == std::string::npos)
                    continue;

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%d", s.getRollNumber());
                
                ImGui::TableNextColumn();
                ImGui::Text("%s", s.getName().c_str());

                ImGui::TableNextColumn();
                ImGui::Text("%s", s.getClassName().c_str());

                ImGui::TableNextColumn();
                ImGui::Text("%s", s.getSection().c_str());
                
                ImGui::TableNextColumn();
                ImGui::Text("%s", s.getFatherName().c_str());

                ImGui::TableNextColumn();
                if (ImGui::Button(("Profile##" + std::to_string(s.getId())).c_str())) {
                    selectedStudentId = s.getId();
                    ImGui::OpenPopup("Student Profile");
                }
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                if (ImGui::Button(("Del##" + std::to_string(s.getId())).c_str())) {
                    ImGui::OpenPopup(("Delete?" + std::to_string(s.getId())).c_str());
                }
                ImGui::PopStyleColor();

                if (ImGui::BeginPopupModal(("Delete?" + std::to_string(s.getId())).c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::Text("Delete student %s?", s.getName().c_str());
                    ImGui::Separator();
                    if (ImGui::Button("Yes", ImVec2(100,0))) {
                        dataManager.DeleteStudent(s.getId());
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", ImVec2(100,0))) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
    
    if (selectedStudentId != -1) {
        ShowStudentProfileModal();
    }

    ImGui::End();
}

void App::ShowAddStudentModal() {
    ImGui::OpenPopup("Add Student");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Add Student", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Name", inputName, sizeof(inputName));
        ImGui::InputText("Father's Name", inputFatherName, sizeof(inputFatherName));
        ImGui::InputText("Email", inputEmail, sizeof(inputEmail));
        ImGui::InputText("Contact", inputPhone, sizeof(inputPhone));
        
        // Class Selection
        auto& mapClasses = ClassConfig::Get().classesAndSections;
        std::vector<std::string> classNames;
        for(auto const& [name, _] : mapClasses) classNames.push_back(name);

        if (!classNames.empty()) {
            if (inputClassIndex >= classNames.size()) inputClassIndex = 0;
            if (ImGui::Combo("Class", &inputClassIndex, [](void* data, int idx, const char** out_text) {
                auto& v = *static_cast<std::vector<std::string>*>(data);
                if (idx < 0 || idx >= v.size()) return false;
                *out_text = v[idx].c_str();
                return true;
            }, &classNames, classNames.size())) {
                inputSectionIndex = 0; // Reset section on class change
            }
            
            // Section Selection
            std::string currentClass = classNames[inputClassIndex];
            auto sections = ClassConfig::Get().GetSections(currentClass);
            
             if (!sections.empty()) {
                if (inputSectionIndex >= sections.size()) inputSectionIndex = 0;
                ImGui::Combo("Section", &inputSectionIndex, [](void* data, int idx, const char** out_text) {
                    auto& v = *static_cast<std::vector<std::string>*>(data);
                    if (idx < 0 || idx >= v.size()) return false;
                    *out_text = v[idx].c_str();
                    return true;
                }, &sections, sections.size());
            } else {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "No sections! Add in Settings.");
            }
        } else {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "No classes! Add in Settings.");
        }


        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            if (!classNames.empty()) {
                std::string cls = classNames[inputClassIndex];
                std::string sec = ClassConfig::Get().GetSections(cls).empty() ? "A" : ClassConfig::Get().GetSections(cls)[inputSectionIndex];
                
                Student s(dataManager.getNextStudentId(), inputName, inputEmail, inputPhone, cls, sec, inputFatherName);
                
                // Auto generate roll number (Simulated: just next sequential ID for now)
                s.setRollNumber(s.getId()); 
                
                dataManager.AddStudent(s);
                showAddStudentModal = false;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showAddStudentModal = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void App::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
