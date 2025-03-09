#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include <vector>
#include "Transaction.hpp"
#include "Date.hpp"
#include "Book.hpp"

class User; // Forward declaration

class Account {
private:
    User *account_user;
    int borrow_limit;
    std::vector<Transaction> borrowed_books;
    std::vector<Transaction> history;
    int due_fine = 0;
    std::vector<Book*> reserved_books; // Add reserved books vector

public:
    Account(User *user, int borrow_limit);
    bool borrow_book(Book *book, Date date);
    int return_book(Book *book, Date return_date); // Return the fine
    std::vector<Transaction> get_currently_borrowed_books();
    std::vector<Transaction> get_borrow_history();
    int calculate_fine(Date date);
    
    // New methods for reserved books
    bool reserve_book(Book *book);
    bool unreserve_book(Book *book);
    std::vector<Book*> get_reserved_books();

    friend class User;
    friend class Library;
};

#endif // ACCOUNT_HPP