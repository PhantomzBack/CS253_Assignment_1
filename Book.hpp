#ifndef BOOK_HPP
#define BOOK_HPP

#include <string>

class User; // Forward declaration

class Book {
private:
    std::string id;
    std::string title, author, publisher, ISBN;
    User *issued_to;
    User *reserved_by;

public:
    enum Status {
        AVAILABLE,
        ISSUED,
        RESERVED
    };
    
    Status status;

    Book(std::string id, std::string title, std::string author, std::string publisher, std::string ISBN);
    
    std::string get_title();
    std::string get_author();
    std::string get_publisher();
    std::string get_ISBN();
    Status get_status();
    bool issue_book(User *user);
    User *get_issued_to();
    bool is_issued_to(User *user);
    bool return_book();
    bool reserve_book(User *user);
    User* get_reserved_by();
    void clear_reservation();

    friend class Library;
};

#endif // BOOK_HPP