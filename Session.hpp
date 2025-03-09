#ifndef SESSION_HPP
#define SESSION_HPP

#include <string>
#include <vector>
#include "Library.hpp"
#include "User.hpp"

class Session {
private:
    User *user;
    Library::UserType user_type;
    bool is_logged_in;
    std::string pre_message_string = "Welcome to the Library Management System!";
    Library *library;
    
    enum STATE {
        LOAD_OR_CREATE_LIBRARY,
        LOGGED_OUT_MAIN_MENU,
        LOGIN,
        REGISTER,
        LOGGED_IN_MAIN_MENU,
        SEARCH_BOOK,
        VIEW_ISSUED_BOOKS,
        BORROW_BOOK,
        RETURN_BOOK,
        ADD_BOOK,
        ADD_USER,
        VIEW_USERS,
        REMOVE_USER,
        REMOVE_BOOK,
        ISSUE_BOOK,
        VIEW_AND_PAY_FINES,
        MAIN_MENU,
        LOGOUT,
        CHANGE_SIMULATED_DATE,
        DUMMY_INPUT,
        USER_UPDATE_PROFILE,
        RESERVE_BOOK, 
        VIEW_NOTIFICATIONS,
        EXIT,
    };

    STATE state = LOAD_OR_CREATE_LIBRARY;
    std::vector<std::string> options;

    void show_notifications();
    bool can_faculty_borrow_book(const Date& current_date);

public:
    Session();
    void construct_options();
    int get_option(bool pre_message = true);
    void start_session();
    bool login(std::string username, std::string password);
    void search_book(bool limit_to_available = false);
};

#endif // SESSION_HPP