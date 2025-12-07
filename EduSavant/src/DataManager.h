#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include "Models/Student.h"
#include "Models/Staff.h" 
#include "Models/ClassConfig.h"

class DataManager {
public:
    std::vector<Student> students;
    std::vector<Staff> staffMembers;

    DataManager() {
        LoadClassConfig();
        LoadStudents();
        LoadStaff();
    }

    // --- Students ---
    void AddStudent(const Student& s) {
        students.push_back(s);
        RecalculateRollNumbers(); // Auto-sort and assign roll nos
        SaveStudents();
    }
    
    void DeleteStudent(int id) {
        students.erase(std::remove_if(students.begin(), students.end(), 
            [id](const Student& s){ return s.getId() == id; }), students.end());
        
        RecalculateRollNumbers(); // Re-assign roll nos after delete
        SaveStudents();
    }

    void RecalculateRollNumbers() {
        // 1. Sort global list by Name (Ascending)
        std::sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
            // Sort by Class -> Section -> Name
            if (a.getClassName() != b.getClassName()) return a.getClassName() < b.getClassName();
            if (a.getSection() != b.getSection()) return a.getSection() < b.getSection();
            return a.getName() < b.getName();
        });

        // 2. Assign Roll Numbers sequentially per section
        std::map<std::string, int> sectionRollCounters; // Key: "Class-Section"
        
        for (auto& s : students) {
            std::string key = s.getClassName() + "-" + s.getSection();
            sectionRollCounters[key]++;
            s.setRollNumber(sectionRollCounters[key]);
        }
    }

    int getNextStudentId() {
        int maxId = 0;
        for(const auto& s : students) if(s.getId() > maxId) maxId = s.getId();
        return maxId + 1;
    }
    
    // --- Staff ---
    void AddStaff(const Staff& s) {
        staffMembers.push_back(s);
        SaveStaff();
    }
    
    void DeleteStaff(int id) {
        staffMembers.erase(std::remove_if(staffMembers.begin(), staffMembers.end(), 
            [id](const Staff& s){ return s.getId() == id; }), staffMembers.end());
        SaveStaff();
    }
    
    // --- Class Config ---
    void SaveClassConfig() {
        std::ofstream file("class_config.db");
        if (!file.is_open()) return;
        
        // Format: CLASS|ClassName|Section1,Section2,...
        // Format: SUBJECT|ClassName|SectionName|Sub1,Sub2,...
        
        for (auto const& [className, sections] : ClassConfig::Get().classesAndSections) {
            file << "CLASS|" << className << "|";
            for (size_t i = 0; i < sections.size(); ++i) {
                file << sections[i] << (i == sections.size() - 1 ? "" : ",");
            }
            file << "\n";
        }

        for (auto const& [className, secMap] : ClassConfig::Get().sectionSubjects) {
            for (auto const& [sectionName, subjects] : secMap) {
                file << "SUBJECT|" << className << "|" << sectionName << "|";
                for (size_t i = 0; i < subjects.size(); ++i) {
                    file << subjects[i] << (i == subjects.size() - 1 ? "" : ",");
                }
                file << "\n";
            }
        }
        file.close();
    }

    void LoadClassConfig() {
        std::ifstream file("class_config.db");
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string type, className, sectionOrContent;
            
            std::getline(ss, type, '|');
            std::getline(ss, className, '|');

            ClassConfig::Get().AddClass(className);

            if (type == "CLASS") {
                std::string content; 
                std::getline(ss, content, '|');
                std::stringstream contentSS(content);
                std::string item;
                while (std::getline(contentSS, item, ',')) {
                    if(!item.empty()) ClassConfig::Get().AddSection(className, item);
                }
            } else if (type == "SUBJECT") {
                std::string sectionName;
                std::getline(ss, sectionName, '|'); // Section comes third now
                
                std::string content;
                std::getline(ss, content, '|');
                std::stringstream contentSS(content);
                std::string item;
                while (std::getline(contentSS, item, ',')) {
                    if(!item.empty()) ClassConfig::Get().AddSubject(className, sectionName, item);
                }
            }
        }
        file.close();
    }

    // --- Persistence ---
    void SaveStudents() {
        std::ofstream file("students.db");
        if (!file.is_open()) return;

        // V2 Format: ID|Name|Email|Phone|Class|Section|RollNo|FatherName|Attendance|MARK_DATA
        // MARK_DATA: Term:Sub:Score;Term:Sub:Score...
        for (const auto& s : students) {
            file << s.getId() << "|" << s.getName() << "|" << s.getEmail() << "|" 
                 << s.getPhone() << "|" << s.getClassName() << "|" << s.getSection() << "|"
                 << s.getRollNumber() << "|" << s.getFatherName() << "|" << s.getAttendance() << "|";
            
            // Serialize Marks
            auto records = s.getAcademicRecord();
            for(auto const& [term, subjects] : records) {
                for(auto const& [sub, mark] : subjects) {
                    file << term << ":" << sub << ":" << mark << ";";
                }
            }
            file << "\n";
        }
        file.close();
    }

    void LoadStudents() {
        students.clear();
        std::ifstream file("students.db");
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> parts;
            while(std::getline(ss, segment, '|')) parts.push_back(segment);

            if (parts.size() >= 9) { // Ensure basic fields exist
                int id = std::stoi(parts[0]);
                Student s(id, parts[1], parts[2], parts[3], parts[4], parts[5], parts[7]);
                s.setRollNumber(std::stoi(parts[6]));
                s.setAttendance(std::stof(parts[8]));

                // Parse marks if present (parts[9])
                if (parts.size() > 9) {
                    std::stringstream marksSS(parts[9]);
                    std::string markEntry;
                    while(std::getline(marksSS, markEntry, ';')) {
                        if(markEntry.empty()) continue;
                        // Term:Sub:Score
                        size_t firstColon = markEntry.find(':');
                        size_t secondColon = markEntry.rfind(':');
                        if (firstColon != std::string::npos && secondColon != std::string::npos) {
                            int term = std::stoi(markEntry.substr(0, firstColon));
                            std::string sub = markEntry.substr(firstColon + 1, secondColon - firstColon - 1);
                            int score = std::stoi(markEntry.substr(secondColon + 1));
                            s.setMark(term, sub, score);
                        }
                    }
                }
                students.push_back(s);
            }
        }
        file.close();
    }

    void SaveStaff() {
        std::ofstream file("staff.db");
        if (!file.is_open()) return;
        // Format: ID|Name|Email|Phone|Role|Subject
        for (const auto& t : staffMembers) {
            file << t.getId() << "|" << t.getName() << "|" << t.getEmail() << "|" 
                 << t.getPhone() << "|" << t.getRole() << "|" << t.getSubject() << "\n";
        }
        file.close();
    }

    void LoadStaff() {
        staffMembers.clear();
        std::ifstream file("staff.db");
        if (!file.is_open()) return;
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> parts;
            while(std::getline(ss, segment, '|')) parts.push_back(segment);
            
            if (parts.size() >= 6) {
                int id = std::stoi(parts[0]);
                Staff st(id, parts[1], parts[2], parts[3], parts[4], parts[5]);
                staffMembers.push_back(st);
            }
        }
        file.close();
    }
};
