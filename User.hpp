#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include "Account.hpp"
#include "Date.hpp"
#include "Transaction.hpp"
#include "Book.hpp"

class User {
protected:
    std::string id;
    std::string username, password;
    int BORROW_LIMIT, MAX_BORROW_PERIOD, borrowed_book, borrow_count, OVERDUE_FINE;
    Account personal_account;

private:
    std::vector<std::string> notifications; // Add this line

public:
    User(std::string id, std::string username, std::string password, int BORROW_LIMIT, int MAX_BORROW_PERIOD, int _OVERDUE_FINE, int _borrowed_book = 0, int _borrow_count = 0);
    
    virtual bool can_borrow_book(Date current_date) = 0;
    virtual bool borrow_book(Book* book, Date date) = 0;
    virtual int return_book(Book* book, Date date) = 0;
    
    int get_borrow_limit();
    int get_max_borrow_period();
    int get_dues(Date date);
    void pay_dues(int amount);
    int get_borrowed_books_count();
    int get_overdue_rate();
    std::string get_username();
    std::vector<Transaction> get_borrowed_books();
    std::vector<Transaction> get_history();
    bool compare_password(std::string password);
    bool change_password(std::string old_password, std::string new_password);
    void change_username(std::string new_username);
    void add_notification(std::string message);
    void view_notifications();
    std::vector<std::string> get_notifications();
    std::vector<Book*> get_reserved_books();
    bool reserve_book(Book* book);
    bool unreserve_book(Book* book); // New method to remove a book from reserved list
    bool issue_reserved_book(Book* book, Date date);

    virtual ~User(); // To make it polymorphic

    friend class Library;
};

#endif // USER_HPP