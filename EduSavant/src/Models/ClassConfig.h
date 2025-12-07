#pragma once
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

class ClassConfig {
public:
    // Key: Class Name (e.g., "10"), Value: List of Sections (e.g., "A", "B")
    std::map<std::string, std::vector<std::string>> classesAndSections;
    
    // Key: Class Name -> Section Name -> List of Subjects
    std::map<std::string, std::map<std::string, std::vector<std::string>>> sectionSubjects;

    static ClassConfig& Get() {
        static ClassConfig instance;
        return instance;
    }

    void AddClass(const std::string& className) {
        if (classesAndSections.find(className) == classesAndSections.end()) {
            classesAndSections[className] = std::vector<std::string>(); // Empty sections
        }
    }

    void AddSection(const std::string& className, const std::string& sectionName) {
        if (classesAndSections.find(className) != classesAndSections.end()) {
            auto& sections = classesAndSections[className];
            if (std::find(sections.begin(), sections.end(), sectionName) == sections.end()) {
                sections.push_back(sectionName);
                std::sort(sections.begin(), sections.end());
            }
            // Ensure map entry exists
            if (sectionSubjects[className].find(sectionName) == sectionSubjects[className].end()) {
                 sectionSubjects[className][sectionName] = std::vector<std::string>();
            }
        }
    }

    void AddSubject(const std::string& className, const std::string& sectionName, const std::string& subjectName) {
         // Ensure class and section exist
         if (sectionSubjects.find(className) == sectionSubjects.end()) return;
         
         auto& secMap = sectionSubjects[className];
         // Ensure entry for section (auto-created by [])
         auto& subjects = secMap[sectionName];
         
         if (std::find(subjects.begin(), subjects.end(), subjectName) == subjects.end()) {
            subjects.push_back(subjectName);
         }
    }

    const std::vector<std::string>& GetSections(const std::string& className) {
        return classesAndSections[className];
    }

    const std::vector<std::string>& GetSubjects(const std::string& className, const std::string& sectionName) {
        return sectionSubjects[className][sectionName];
    }
};
