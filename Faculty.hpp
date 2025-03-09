#ifndef FACULTY_HPP
#define FACULTY_HPP

#include "User.hpp"

class Faculty : public User {
public:
    Faculty(std::string id, std::string username, std::string password);
    bool can_borrow_book(Date current_date) override;
    bool borrow_book(Book* book, Date date) override;
    int return_book(Book* book, Date date) override;
};

#endif // FACULTY_HPP