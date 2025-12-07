#pragma once
#include "Person.h"
#include <vector>

class Staff : public Person {
private:
    std::string role; // e.g., "Principal", "Teacher", "Clerk"
    std::string subject; // Optional, only for Teachers
    std::string phone; // Storing phone here as well for now

public:
    Staff(int id, std::string name, std::string email, std::string phone, std::string role, std::string subject = "")
        : Person(id, name, email, phone), role(role), subject(subject), phone(phone) {}

    std::string getRole() const override { return role; }
    std::string getSubject() const { return subject; }
    std::string getPhone() const { return phone; }

    void setRole(const std::string& r) { role = r; }
    void setSubject(const std::string& s) { subject = s; }

    void displayInfo() const override {
        // Debug info
    }
};
