#ifndef STUDENT_HPP
#define STUDENT_HPP

#include "User.hpp"

class Student : public User {
public:
    Student(std::string id, std::string username, std::string password);
    bool can_borrow_book(Date current_date) override;
    bool borrow_book(Book* book, Date date) override;
    int return_book(Book* book, Date date) override;
};

#endif // STUDENT_HPP