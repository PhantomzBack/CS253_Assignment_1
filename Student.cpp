#include "Student.hpp"
#include <cassert>

Student::Student(std::string id, std::string username, std::string password) 
    : User(id, username, password, 3, 14, 10) {}

bool Student::can_borrow_book(Date current_date)
{
    // Use the library's current date and check with the account
    // Count actual borrowed books (excluding reserved books that appear with -1 date)
    int actual_borrowed_count = 0;
    auto borrowed_books = personal_account.get_currently_borrowed_books();
    for (auto& transaction : borrowed_books) {
        if (transaction.borrow_date.get_date_as_int() != -1) {
            actual_borrowed_count++;
            // Check if any book is overdue
            if (current_date.get_day_difference(transaction.borrow_date) > MAX_BORROW_PERIOD) {
                return false;
            }
        }
    }
    
    // Add count of reserved books
    int reserved_count = personal_account.get_reserved_books().size();
    int total_books = actual_borrowed_count + reserved_count;
    
    return total_books < BORROW_LIMIT && personal_account.calculate_fine(current_date) == 0;
}

bool Student::borrow_book(Book* book, Date date)
{
    assert(can_borrow_book(date));
    personal_account.borrow_book(book, date);
    borrowed_book++;
    return true;
}

int Student::return_book(Book* book, Date date)
{
    int fine = personal_account.return_book(book, date);
    if(fine >= 0){
        borrowed_book--;
    }
    return fine;
}