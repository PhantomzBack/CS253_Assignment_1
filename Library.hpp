#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <string>
#include <vector>
#include "Book.hpp"
#include "User.hpp"
#include "Student.hpp"
#include "Faculty.hpp"
#include "ID_Manager.hpp"
#include "Date.hpp"

// Forward declaration
class Librarian;

class Library {
private:
    std::string student_id_prefix = "S";
    std::string faculty_id_prefix = "F";
    std::string book_id_prefix = "B";
    std::vector<Book*> books;
    std::vector<User*> users;
    ID_Manager student_id_manager, faculty_id_manager, book_id_manager;
    Librarian* core_librarian;
    Date current_date;

public:
    Library(std::string _librarian_username = "admin", std::string _librarian_pwd = "admin", 
            std::string _student_id_prefix = "S", std::string _faculty_id_prefix = "F", std::string _book_id_prefix = "B");

    enum class UserType {
        STUDENT,
        FACULTY,
        LIBRARIAN,
        USER_NULL
    };

    void set_current_date(Date new_date);
    Date get_current_date();
    
    bool borrow_book(User *user, Book *book);
    int return_book(User* user, Book *book); // Return the fine or -1 if failure
    
    static Library* load_from_file(std::string filename);

    /*All three are boolean to avoid duplicated entries*/
    bool add_book(std::string title, std::string author, std::string publisher, std::string ISBN);
    bool add_student(std::string username, std::string password);
    bool add_faculty(std::string username, std::string password);

    bool remove_book(std::string ISBN);
    bool remove_user(std::string username); 
    
    std::vector<User*> get_users();
    Book *search_book(std::string ISBN);
    Book *search_book_by_ID(std::string id);
    std::vector<Book*> search_books_by_title_author_publisher(std::string expression);
    User *search_user(std::string username);
    User *login(std::string username, std::string password);
    bool write_to_file(std::string filename);
    UserType get_user_type(User* user);
    
    bool update_username(User* user, std::string new_username);
    bool update_password(User* user, std::string old_password, std::string new_password);
    
    bool reserve_book(User *user, Book *book);
    void notify_user(User *user, std::string message);
};

#endif // LIBRARY_HPP