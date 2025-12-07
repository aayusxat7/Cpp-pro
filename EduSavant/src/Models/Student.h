#pragma once
#include "Person.h"
#include <vector>
#include <numeric>
#include <map> // Added for std::map

class Student : public Person {
private:
    std::string className; // e.g., "10"
    std::string section;   // e.g., "A"
    int rollNumber;
    std::string fatherName;
    std::string phone; // Moved from Person to Student in this model
    float attendance;

    // Term (1-4) -> Subject -> Mark
    std::map<int, std::map<std::string, int>> academicRecord;

public:
    Student(int id, std::string name, std::string email, std::string phone, std::string className, std::string section, std::string fatherName)
        : Person(id, name, email, phone), className(className), section(section), fatherName(fatherName), phone(phone), attendance(0.0f) {
        rollNumber = 0; // Assigned later
    }

    // Getters
    std::string getClassName() const { return className; }
    std::string getSection() const { return section; }
    std::string getFatherName() const { return fatherName; }
    std::string getPhone() const { return phone; }
    int getRollNumber() const { return rollNumber; }
    float getAttendance() const { return attendance; }

    // Setters
    void setName(const std::string& n) { Person::setName(n); } // Expose base setter
    void setFatherName(const std::string& f) { fatherName = f; }
    void setPhone(const std::string& p) { phone = p; Person::setPhone(p); } // Update both
    void setEmail(const std::string& e) { Person::setEmail(e); }
    
    void setRollNumber(int r) { rollNumber = r; }
    void setAttendance(float a) { attendance = a; }

    void setMark(int term, const std::string& subject, int mark) {
        academicRecord[term][subject] = mark;
    }

    int getMark(int term, const std::string& subject) {
        if (academicRecord.count(term) && academicRecord[term].count(subject)) {
            return academicRecord[term][subject];
        }
        return 0; // Default if not found
    }
    
    // Virtual implementations
    std::string getRole() const override { return "Student"; }

    void displayInfo() const override {
        // Debug
    }

    // For persistence helper
    const std::map<int, std::map<std::string, int>>& getAcademicRecord() const { return academicRecord; }
    void loadAcademicRecord(const std::map<int, std::map<std::string, int>>& record) { academicRecord = record; }
};
