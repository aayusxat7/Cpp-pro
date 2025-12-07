#pragma once
#include <string>
#include <iostream>

class Person {
protected:
    int id;
    std::string name;
    std::string email;
    std::string phone;

public:
    Person(int id, std::string name, std::string email, std::string phone) 
        : id(id), name(name), email(email), phone(phone) {}
    
    virtual ~Person() {}

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    std::string getPhone() const { return phone; }

    // Setters
    void setName(const std::string& n) { name = n; }
    void setEmail(const std::string& e) { email = e; }
    void setPhone(const std::string& p) { phone = p; }

    // Virtual function for polymorphism
    virtual std::string getRole() const = 0;
    virtual void displayInfo() const {
        std::cout << "ID: " << id << "\nName: " << name << "\nEmail: " << email << "\nPhone: " << phone << std::endl;
    }
};
