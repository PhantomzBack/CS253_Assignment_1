#include "User.hpp"

User::User(std::string id, std::string username, std::string password, int BORROW_LIMIT, int MAX_BORROW_PERIOD, int _OVERDUE_FINE, int _borrowed_book, int _borrow_count) 
    : id(id), username(username), password(password), BORROW_LIMIT(BORROW_LIMIT), MAX_BORROW_PERIOD(MAX_BORROW_PERIOD), 
      borrowed_book(_borrowed_book), borrow_count(_borrow_count), OVERDUE_FINE(_OVERDUE_FINE), personal_account(this, BORROW_LIMIT) {}

User::~User() {}

int User::get_borrow_limit()
{
    return BORROW_LIMIT;
}

int User::get_max_borrow_period()
{
    return MAX_BORROW_PERIOD;
}

int User::get_dues(Date date)
{
    return personal_account.calculate_fine(date);
}

void User::pay_dues(int amount)
{
    personal_account.due_fine -= amount;
}

int User::get_borrowed_books_count()
{
    return borrowed_book;
}

int User::get_overdue_rate()
{
    return OVERDUE_FINE;
}

std::string User::get_username()
{
    return username;
}

std::vector<Transaction> User::get_borrowed_books()
{
    return personal_account.get_currently_borrowed_books();
}

std::vector<Transaction> User::get_history()
{
    return personal_account.get_borrow_history();
}

bool User::compare_password(std::string password)
{
    return this->password == password;
}

bool User::change_password(std::string old_password, std::string new_password)
{
    if (compare_password(old_password))
    {
        password = new_password;
        return true;
    }
    return false;
}

void User::change_username(std::string new_username)
{
    username = new_username;
}

void User::add_notification(std::string message)
{
    notifications.push_back(message);
}

std::vector<std::string> User::get_notifications()
{
    std::vector<std::string> temp_notifications = notifications;
    notifications.clear();
    return temp_notifications;
}

std::vector<Book*> User::get_reserved_books() {
    return personal_account.get_reserved_books();
}

bool User::reserve_book(Book* book) {
    return personal_account.reserve_book(book);
}

bool User::unreserve_book(Book* book) {
    return personal_account.unreserve_book(book);
}

bool User::issue_reserved_book(Book* book, Date date) {
    // Check if user has reached borrow limit
    if (!can_borrow_book(date)) {
        return false;
    }
    
    // Check if the book is in the reserved list and remove it
    if (!personal_account.unreserve_book(book)) {
        return false;
    }
    
    // Borrow the book
    return borrow_book(book, date);
}