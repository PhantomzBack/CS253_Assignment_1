#include "Book.hpp"
#include "User.hpp"

Book::Book(std::string id, std::string title, std::string author, std::string publisher, std::string ISBN) 
    : id(id), title(title), author(author), publisher(publisher), ISBN(ISBN), reserved_by(nullptr) // Initialize reserved_by
{
    status = AVAILABLE;
    issued_to = NULL;
}

std::string Book::get_title()
{
    return title;
}

std::string Book::get_author()
{
    return author;
}

std::string Book::get_publisher()
{
    return publisher;
}

std::string Book::get_ISBN()
{
    return ISBN;
}

Book::Status Book::get_status()
{
    return status;
}

bool Book::issue_book(User *user)
{
    if (status == AVAILABLE || (status == RESERVED && reserved_by == user))
    {
        status = ISSUED;
        issued_to = user;
        return true;
    }
    return false;
}

User* Book::get_issued_to()
{
    return issued_to;
}

bool Book::is_issued_to(User *user)
{
    return issued_to == user;
}

bool Book::return_book()
{
    if (status == ISSUED)
    {
        status = AVAILABLE;
        issued_to = NULL;
        if (reserved_by != nullptr)
        {
            status = RESERVED;
        }
        return true;
    }
    return false;
}

bool Book::reserve_book(User *user)
{
    if (status == AVAILABLE || status == ISSUED)
    {
        if (reserved_by == nullptr)
        {
            reserved_by = user;
            status = RESERVED;
            return true;
        }
    }
    return false;
}

User* Book::get_reserved_by()
{
    return reserved_by;
}

void Book::clear_reservation()
{
    reserved_by = nullptr;
    if (status == RESERVED)
    {
        status = AVAILABLE;
    }
}