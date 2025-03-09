#include "Librarian.hpp"
#include "Library.hpp"

Librarian::Librarian(Library* _library, std::string id, std::string username, std::string password) 
    : User(id, username, password, 0, 0, 0), library(_library) {}

bool Librarian::add_book(std::string title, std::string author, std::string publisher, std::string ISBN)
{
    return library->add_book(title, author, publisher, ISBN);
}

bool Librarian::add_user(std::string username, std::string password)
{
    return library->add_student(username, password);
}

bool Librarian::can_borrow_book(Date date)
{
    return false;
}

bool Librarian::borrow_book(Book* book, Date date)
{
    return false;
}

int Librarian::return_book(Book* book, Date date)
{
    return -1;
}