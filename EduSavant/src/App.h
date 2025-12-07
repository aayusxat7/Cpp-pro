#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include "DataManager.h"


class App {
public:
    App();
    ~App();

    void Run();

private:
    void Init();
    void Shutdown();
    
    GLFWwindow* window = nullptr;
    const char* glsl_version = "#version 130";

    // Application Data
    DataManager dataManager;
    
    // UI State
    enum class Screen { Dashboard, Students, Teachers, Settings };
    Screen currentScreen = Screen::Dashboard;

    bool showAddStudentModal = false;
    bool showAddTeacherModal = false;
    int selectedStudentId = -1;

    // Temporary variables for input - Students
    char inputName[128] = "";
    char inputEmail[128] = "";
    char inputPhone[128] = "";
    char inputFatherName[128] = "";
    int inputClassIndex = 0; // Index in available classes
    int inputSectionIndex = 0; // Index in available sections

    // Temporary variables for input - Staff
    char inputStaffName[128] = "";
    char inputStaffEmail[128] = "";
    char inputStaffPhone[128] = "";
    char inputStaffRole[128] = "";   // ComboBox
    char inputStaffSubject[128] = ""; // Only if role == Teacher

    // Temporary variables for Class/Subject Config
    char inputNewClassName[64] = "";
    char inputNewSectionName[64] = "";
    char inputNewSubjectName[64] = "";
    int selectedClassConfigIndex = 0;
    
    // Student List Filter State
    int selectedFilterClassIndex = 0;
    int selectedFilterSectionIndex = 0;

    void RenderDashboard();
    void RenderStudentList();
    void RenderStaffList(); // Renamed from TeacherList
    void RenderSettings();
    void RenderSidebar();
    
    void ShowAddStudentModal();
    void ShowAddStaffModal(); // Renamed
    void ShowStudentProfileModal(); // New
};

