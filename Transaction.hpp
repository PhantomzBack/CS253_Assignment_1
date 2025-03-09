#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include "Book.hpp"
#include "Date.hpp"

class User; // Forward declaration

struct Transaction {
    Book *book;
    User *user;
    Date borrow_date;
    Date return_date;
    int fine_paid;
    int pending_fine;   
};

#endif // TRANSACTION_HPP