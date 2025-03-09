#include "Account.hpp"
#include "User.hpp"

#define FINE_PER_DAY 10

Account::Account(User *user, int borrow_limit) : account_user(user), borrow_limit(borrow_limit) {}

bool Account::borrow_book(Book *book, Date date)
{
    if (borrowed_books.size() < borrow_limit)
    {
        // Calculate a future date for the expected return date (current date + max borrow period)
        Date futureDate = date;  // Use the current date as base
        // We'll use a special value in integer form to indicate it's not yet returned
        // This will be handled properly by our enhanced Date class
        Transaction _transaction = {book, account_user, date, Date(99991231), 0, 0};
        borrowed_books.emplace_back(_transaction);
        return true;
    }
    return false;
}

int Account::return_book(Book *book, Date return_date)
{
    for (int i = 0; i < borrowed_books.size(); i++)
    {
        if (borrowed_books[i].book == book)
        {
            auto transaction = borrowed_books[i];
            transaction.return_date = return_date;
            if (transaction.return_date.get_day_difference(transaction.borrow_date) > account_user->get_max_borrow_period())
            {
                transaction.pending_fine = account_user->get_overdue_rate() * (transaction.return_date.get_day_difference(transaction.borrow_date) - account_user->get_max_borrow_period());
                due_fine+=transaction.pending_fine;
            }
            history.emplace_back(transaction);
            borrowed_books.erase(borrowed_books.begin() + i);
            return transaction.pending_fine;
        }
    }
    return -1;
}

std::vector<Transaction> Account::get_currently_borrowed_books()
{
    std::vector<Transaction> all_books = borrowed_books;
    
    // Add reserved books as transactions with special dates
    for (auto& book : reserved_books) {
        Transaction temp_transaction;
        temp_transaction.book = book;
        temp_transaction.user = account_user;
        temp_transaction.borrow_date = Date(-1); // Special date for reserved books
        temp_transaction.return_date = Date(-1);
        temp_transaction.fine_paid = 0;
        temp_transaction.pending_fine = 0;
        all_books.push_back(temp_transaction);
    }
    
    return all_books;
}

std::vector<Transaction> Account::get_borrow_history()
{
    return history;
}

int Account::calculate_fine(Date date)
{
    return due_fine; // We update the due fine only when the book is returned
    // int fine = 0;
    // for (int i = 0; i < borrowed_books.size(); i++)
    // {
    //     int overdue_days = borrowed_books[i].borrow_date.get_day_difference(date) - account_user->get_max_borrow_period();
    //     if (overdue_days > )
    //     {
    //         fine += overdue_days * FINE_PER_DAY;
    //     }
    // }
    // return fine;
}

// Implement the new methods for reserved books

bool Account::reserve_book(Book *book)
{
    // Check if the book is already in the reserved list
    for (auto it = reserved_books.begin(); it != reserved_books.end(); ++it) {
        if (*it == book) {
            return false; // Already reserved
        }
    }
    reserved_books.push_back(book);
    return true;
}

bool Account::unreserve_book(Book *book)
{
    // Find the book in the reserved list
    for (auto it = reserved_books.begin(); it != reserved_books.end(); ++it) {
        if (*it == book) {
            // Remove the book from the reserved list
            reserved_books.erase(it);
            return true;
        }
    }
    return false; // Book not found in reserved list
}

std::vector<Book*> Account::get_reserved_books()
{
    return reserved_books;
}