#ifndef LIBRARIAN_HPP
#define LIBRARIAN_HPP

#include "User.hpp"
#include "Book.hpp"
#include "Date.hpp"

class Library; // Forward declaration

class Librarian : public User {
private:
    Library *library;
    
public:
    Librarian(Library* _library, std::string id, std::string username, std::string password);
    bool add_book(std::string title, std::string author, std::string publisher, std::string ISBN);
    bool add_user(std::string username, std::string password);
    bool can_borrow_book(Date date) override;
    bool borrow_book(Book* book, Date date) override;
    int return_book(Book* book, Date date) override;
};

#endif // LIBRARIAN_HPP