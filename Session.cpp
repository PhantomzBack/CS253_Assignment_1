#include "Session.hpp"
#include "util.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

#ifdef _WIN32
#define CLEAR_TERMINAL() system("cls")
#else
#define CLEAR_TERMINAL() system("clear")
#endif

Session::Session() : is_logged_in(false) {}

int Session::get_option(bool pre_message)
{ 
    while(true){
        if(pre_message){
            std::cout << pre_message_string << std::endl;
        }
        std::cout << "Choose an option" << std::endl;
        for (auto i = 0; i < options.size(); i++)
        {
            std::cout << i + 1 << ". " << options[i] << std::endl;
        }
        std::string option;
        getline(std::cin, option);
        try{
            int option_int = stoi(option);
            if(option_int >= 1 && option_int <= options.size()){
                return option_int;
            }
            std::cout << "Invalid option. Please try again" << std::endl;
        }
        catch(const std::exception& e){
            std::cout << "Invalid option. Please try again" << std::endl;
            continue;   
        }
    }
}

bool Session::login(std::string username, std::string password)
{
    user = library->login(username, password);
    if (user)
    {
        is_logged_in = true;
        return true;
    }
    return false;
}

void Session::search_book(bool limit_to_available)
{
    CLEAR_TERMINAL();
    std::cout << "Search by title, author or publisher (empty lists all)" << std::endl;
    std::string title;
    getline(std::cin, title);
    std::vector<Book*> books = library->search_books_by_title_author_publisher(title);
    if(books.size() == 0){
        std::cout << "No books found!" << std::endl;
        std::cin.ignore();
        return;
    }
    std::vector<Book*> final_book_vec;
    if(limit_to_available){
        for(auto i = 0; i < books.size(); i++){
            if(books[i]->get_status() == Book::Status::AVAILABLE){
                final_book_vec.emplace_back(books[i]);
            }
        }
    }
    else{
        final_book_vec = books;
    }
    std::cout << "Found " << final_book_vec.size() << " books" << std::endl << std::endl;
    std::vector<std::vector<std::string>> table;
    std::vector<std::string> header = {"Book Number", "Title", "Author", "Publisher", "ISBN", "Status"};
    table.push_back(header);
    for(auto i = 0; i < final_book_vec.size(); i++){
        if(limit_to_available && final_book_vec[i]->get_status() != Book::Status::AVAILABLE){
            continue;
        }
        std::vector<std::string> row;
        row.push_back(std::to_string(i+1));
        row.push_back(final_book_vec[i]->get_title());
        row.push_back(final_book_vec[i]->get_author());
        row.push_back(final_book_vec[i]->get_publisher());
        row.push_back(final_book_vec[i]->get_ISBN());
        row.push_back(final_book_vec[i]->get_status() == Book::Status::AVAILABLE ? "Available" : (final_book_vec[i]->get_status() == Book::Status::RESERVED ? "Reserved" : "Borrowed"));
        table.push_back(row);
    }
    Util::printTable(table);
    getchar();
    return; 
}

void Session::construct_options()
{
    options.clear();
    switch(state)
    {
        case LOAD_OR_CREATE_LIBRARY:
        {
            options = {"Load Library", "Create Library", "Quit Application"};
            int option = get_option();
            if(option == 1){
                std::cout << "Please enter the filename to load from (default is library.json)" << std::endl;
                std::string filename;
                getline(std::cin, filename);
                if(filename == ""){
                    filename = "library.json";
                }
                library = Library::load_from_file(filename);
                if(library == NULL){
                    std::cout << "Error loading from file. Please specify another file!" << std::endl;
                }
                else{
                    std::cout << "Successfully loaded from file!" << std::endl;
                    state = LOGGED_OUT_MAIN_MENU;
                }
            }
            else if(option == 2){
                library = new Library("admin", "admin", "S", "F", "B");
                std::cout << "Successfully created a new library! Admin librarian username is admin and password is admin" << std::endl;
                state = LOGGED_OUT_MAIN_MENU;
            }
            // else if(option == 3){
            //     std::cout << "Please enter the filename to write to (default is library.json)" << std::endl;
            //     std::string filename;
            //     getline(std::cin, filename);
            //     if(filename == ""){
            //         filename = "library.json";
            //     }
            //     if(library->write_to_file(filename)){
            //         std::cout << "Successfully wrote to file!" << std::endl;
            //     }
            //     else{
            //         std::cout << "Error writing to file!" << std::endl;
            //     }
            // }
            else if(option == 3){
                state = EXIT;
            }
            break;
        }
        case LOGGED_OUT_MAIN_MENU:
        {
            options = {"Login", "Register", "Close Library"};
            int option = get_option();
            if(option == 1){
                state = LOGIN;
            }
            else if(option == 2){
                state = REGISTER;
            }
            else if(option == 3){
                options = {"Write To File", "Close Without Saving"};
                int option = get_option();
                if(option == 1){
                    std::cout << "Please enter the filename to write to (default is library.json)" << std::endl;
                    std::string filename;
                    getline(std::cin, filename);
                    if(filename == ""){
                        filename = "library.json";
                    }
                    if(library->write_to_file(filename)){
                        std::cout << "Successfully wrote to file!" << std::endl;
                    }
                    else{
                        std::cout << "Error writing to file!" << std::endl;
                    }
                }
                delete library;
                CLEAR_TERMINAL();
                state = LOAD_OR_CREATE_LIBRARY;
            }
            break;
        }
        case LOGGED_IN_MAIN_MENU:
        {
            switch(user_type){
                case Library::UserType::STUDENT:
                {
                    options = {"Search Book", "Borrow Book", "View Issued, Reserved Books or History", "Return Book", "View and Pay Fines", "Update Profile", "View Notifications", "Logout", "Change Simulated Date"};
                    int option = get_option();
                    if(option == 1){
                        state = SEARCH_BOOK;
                    }
                    else if(option == 2){
                        state = BORROW_BOOK;
                    }
                    else if(option == 3){
                        state = VIEW_ISSUED_BOOKS;
                    }
                    else if(option == 4){
                        state = RETURN_BOOK;
                    }
                    else if(option == 5){
                        state = VIEW_AND_PAY_FINES;
                    }
                    else if(option == 6){
                        state = USER_UPDATE_PROFILE;
                    }
                    else if(option == 7){
                        state = VIEW_NOTIFICATIONS;
                    }
                    else if(option == 8){
                        state = LOGOUT;
                    }
                    else if(option == 9){
                        state = CHANGE_SIMULATED_DATE;
                    }
                    break;
                }
                case Library::UserType::FACULTY:
                {
                    options = {"Search Book", "Borrow Book", "View Issued, Reserved Books or History", "Return Book", "Update Profile", "View Notifications", "Logout", "Change Simulated Date"};
                    int option = get_option();
                    if(option == 1){
                        state = SEARCH_BOOK;
                    }
                    else if(option == 2){
                        state = BORROW_BOOK;
                    }
                    else if(option == 3){
                        state = VIEW_ISSUED_BOOKS;
                    }
                    else if(option == 4){
                        state = RETURN_BOOK;
                    }
                    else if(option == 5){
                        state = USER_UPDATE_PROFILE;
                    }
                    else if(option == 6){
                        state = VIEW_NOTIFICATIONS;
                    }
                    else if(option == 7){
                        state = LOGOUT;
                    }
                    else if(option == 8){
                        state = CHANGE_SIMULATED_DATE;
                    }
                    break;
                }
                case Library::UserType::LIBRARIAN:
                {
                    options = {"See/Search books", "Add Book", "View Users", "Add User", "Remove User", "Remove Book", "Update Profile", "Logout", "Change Simulated Date"};
                    int option = get_option();
                    option--;
                    if (option == 0){
                        state = SEARCH_BOOK;
                    }
                    else if(option == 1){
                        state = ADD_BOOK;
                    }
                    else if(option == 2){
                        state = VIEW_USERS;
                    }
                    else if(option == 3){
                        state = ADD_USER;
                    }
                    else if(option == 4){
                        state = REMOVE_USER;
                    } 
                    else if(option == 5){
                        state = REMOVE_BOOK;
                    }
                    else if(option == 6){
                        state = USER_UPDATE_PROFILE;
                    }
                    else if(option == 7){
                        state = LOGOUT;
                    }
                    else if(option == 8){
                        state = CHANGE_SIMULATED_DATE;
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case LOGIN:
        {
            CLEAR_TERMINAL();
            std::cout << "Enter username" << std::endl;
            std::string username;
            getline(std::cin, username);
            CLEAR_TERMINAL();
            std::cout << "Enter password" << std::endl;
            std::string password;
            getline(std::cin, password);
            if(login(username, password)){
                user_type = library->get_user_type(user);
                CLEAR_TERMINAL();
                std::cout << "Successfully logged in!" << std::endl;
                switch(user_type){
                    case Library::UserType::STUDENT:
                        pre_message_string = "Hi " + user->get_username() + "! You are logged in as a student";
                        // pre_message_string = "Welcome Student " + user->get_username() + "!";
                        break;
                    case Library::UserType::FACULTY:
                        pre_message_string = "Hi " + user->get_username() + "! You are logged in as a faculty";
                        // pre_message_string = "Welcome Faculty " + user->get_username() + "!";
                        break;
                    case Library::UserType::LIBRARIAN:
                        pre_message_string = "Hi " + user->get_username() + "! You are logged in as a librarian";
                        // pre_message_string = "Welcome Librarian " + user->get_username() + "!";
                        break;
                    default:
                        break;
                }
                state = LOGGED_IN_MAIN_MENU;
            }
            else{
                std::cout << "Invalid username or password. Please try again" << std::endl;
                std::string dummy;
                getline(std::cin, dummy);
                state = LOGGED_OUT_MAIN_MENU;
                break;
            }
            break;
        }
        case SEARCH_BOOK:
        {
            pre_message_string = "Limit to available books?";
            options = {"Yes", "No"};
            int option = get_option();
            if(option == 2){
                search_book();
            }
            else{
                search_book(true);
            }
            state = LOGGED_IN_MAIN_MENU;
            break;
        }
        case BORROW_BOOK:
        {
            CLEAR_TERMINAL();
            // if (!user->can_borrow_book(library->get_current_date())) {
            //     std::cout << "You cannot borrow right now! Please ensure you have no overdue books and have not exhausted your borrow limit!" << std::endl;
            //     state = DUMMY_INPUT;
            //     break;
            // }
            if(!user->can_borrow_book(library->get_current_date())){
                std::cout << "You cannot borrow right now! Please ensure you have no pending fines and have not exhausted your borrow limit!" << std::endl;
                switch(user_type){
                    case Library::UserType::STUDENT:{
                        std::cout << "You've borrowed " << user->get_borrowed_books_count() << " books" << std::endl;
                        std::cout << "You have a fine of " << user->get_dues(library->get_current_date()) << std::endl;
                        options = {"Return Books", "View and Pay Fines", "Return to Main Menu"};
                        int option = get_option();
                        if(option == 1){
                            state = RETURN_BOOK;
                        }
                        else if(option == 2){
                            state = VIEW_AND_PAY_FINES;
                        }
                        else if(option == 3){
                            state = LOGGED_IN_MAIN_MENU;
                        }
                        break;
                    }
                    case Library::UserType::FACULTY:
                        std::cout << "Please return some books to borrow more or ensure you have no overdues!" << std::endl;
                        state = DUMMY_INPUT;
                        break;
                    case Library::UserType::LIBRARIAN:
                        std::cout << "Librarians cannot borrow books" << std::endl;
                        state = DUMMY_INPUT;
                        break;
                    default:
                        break;
                }
                break;
            }
            
            pre_message_string = "Would you like to search the books first?";
            options = {"Yes", "No"};
            int option = get_option();
            if(option == 1){
                search_book();
            }
            
            std::cout << "Enter the ISBN of the book you want to borrow" << std::endl;
            std::string ISBN;
            getline(std::cin, ISBN);
            CLEAR_TERMINAL();
            Book* book = library->search_book(ISBN);
            if(book == NULL){
                std::cout << "Book not found!" << std::endl;
                state = LOGGED_IN_MAIN_MENU;
                break;
            }
            else if(book->get_status() == Book::Status::AVAILABLE){
                library->borrow_book(user, book);
                std::cout << "Successfully borrowed book! You can borrow books for upto " << user->get_max_borrow_period() << " days" << std::endl;
                state = DUMMY_INPUT;
            }
            else if(book->get_status() == Book::Status::RESERVED && book->get_reserved_by() != user){
                std::cout << "Book is already reserved by another user!" << std::endl;
                state = DUMMY_INPUT;
            }
            else if(book->is_issued_to(user)){
                std::cout << "You have already borrowed this book!" << std::endl;
                state = DUMMY_INPUT;
            }
            else{
                pre_message_string = "Book is not available currently! Would you like to reserve it?";
                options = {"Yes", "No"};
                int reserve_option = get_option();
                if(reserve_option == 1){
                    if(library->reserve_book(user, book)){
                        std::cout << "Successfully reserved book!" << std::endl;
                    } else {
                        std::cout << "Failed to reserve book!" << std::endl;
                    }
                }
                state = DUMMY_INPUT;
            }
            break;
        }
        case VIEW_ISSUED_BOOKS:
        {
            CLEAR_TERMINAL();
            options = {"View Currently Issued Books", "View Previously Issued Books", "View Both", "Return to Main Menu"};
            int option = get_option();
            if(option == 1 || option == 3){
                std::cout << "Currently Issued Books:" << std::endl;
                auto borrowed_books = user->get_borrowed_books();
                auto reserved_books = user->get_reserved_books();
                
                // Filter out books that are only reserved (with special date -1) but not actually borrowed
                std::vector<Transaction> actual_borrowed_books;
                for (auto& transaction : borrowed_books) {
                    if (transaction.borrow_date.get_date_as_int() != -1) {
                        actual_borrowed_books.push_back(transaction);
                    }
                }
                
                if(actual_borrowed_books.size() == 0 && reserved_books.size() == 0){
                    std::cout << "You have no currently issued or reserved books!" << std::endl;
                }
                else{
                    // Display borrowed books
                    if (actual_borrowed_books.size() > 0) {
                        std::cout << "\nBorrowed Books:" << std::endl;
                        std::vector<std::vector<std::string>> table;
                        table.push_back({"Book Title", "ISBN", "Author", "Publisher", "Borrowed On"});
                        for (auto it = actual_borrowed_books.begin(); it != actual_borrowed_books.end(); it++) {
                            std::vector<std::string> row;
                            row.push_back(it->book->get_title());
                            row.push_back(it->book->get_ISBN());
                            row.push_back(it->book->get_author());
                            row.push_back(it->book->get_publisher());
                            row.push_back(it->borrow_date.to_string());
                            table.push_back(row);
                        }
                        Util::printTable(table);
                    } else {
                        std::cout << "\nNo borrowed books." << std::endl;
                    }
                    
                    if (reserved_books.size() > 0) {
                        std::cout << "\nReserved Books:" << std::endl;
                        std::vector<std::vector<std::string>> table;
                        table.push_back({"Book Title", "ISBN", "Author", "Publisher"});
                        for (auto book : reserved_books) {
                            std::vector<std::string> row;
                            row.push_back(book->get_title());
                            row.push_back(book->get_ISBN());
                            row.push_back(book->get_author());
                            row.push_back(book->get_publisher());
                            table.push_back(row);
                        }
                        Util::printTable(table);
                    } else {
                        std::cout << "\nNo reserved books." << std::endl;
                    }
                }
            }
            if(option == 2 || option == 3){
                std::cout << "Previously Issued Books:" << std::endl;
                auto history = user->get_history();
                if(history.size() == 0){
                    std::cout << "You have no history!" << std::endl; 
                }
                else{
                    std::vector<std::vector<std::string>> table;
                    table.push_back({"Book Title", "ISBN", "Author", "Publisher", "Borrowed on", "Returned on", "Fine"});
                    for(auto it = history.begin(); it != history.end(); it++){
                        std::vector<std::string> row;
                        row.push_back(it->book->get_title());
                        row.push_back(it->book->get_ISBN());
                        row.push_back(it->book->get_author());
                        row.push_back(it->book->get_publisher());
                        row.push_back(it->borrow_date.to_string());
                        row.push_back(it->return_date.to_string());
                        row.push_back(std::to_string(it->pending_fine));
                        table.push_back(row);
                    }
                    Util::printTable(table);
                }
            }
            if(option == 4){
                CLEAR_TERMINAL();
                state = LOGGED_IN_MAIN_MENU;
                break;
            }
            state = DUMMY_INPUT;
            break;
        }
        case RETURN_BOOK:
        {
            CLEAR_TERMINAL();
            auto borrowed_books = user->get_borrowed_books();
            auto reserved_books = user->get_reserved_books();
            
            // Filter out books that are only reserved (with special date -1) but not actually borrowed
            std::vector<Transaction> actual_borrowed_books;
            for (auto& transaction : borrowed_books) {
                if (transaction.borrow_date.get_date_as_int() != -1) {
                    actual_borrowed_books.push_back(transaction);
                }
            }
            
            if (actual_borrowed_books.size() == 0 && reserved_books.size() == 0) {
                std::cout << "You have no currently issued or reserved books!" << std::endl;
                state = DUMMY_INPUT;
                break;
            }
            
            // Display borrowed books
            if (actual_borrowed_books.size() > 0) {
                std::cout << "\nBorrowed Books:" << std::endl;
                std::vector<std::vector<std::string>> table;
                table.push_back({"Book Title", "ISBN", "Author", "Publisher", "Borrowed On"});
                for (auto it = actual_borrowed_books.begin(); it != actual_borrowed_books.end(); it++) {
                    std::vector<std::string> row;
                    row.push_back(it->book->get_title());
                    row.push_back(it->book->get_ISBN());
                    row.push_back(it->book->get_author());
                    row.push_back(it->book->get_publisher());
                    row.push_back(it->borrow_date.to_string());
                    table.push_back(row);
                }
                Util::printTable(table);
            } else {
                std::cout << "\nNo borrowed books." << std::endl;
            }
            
            // Display reserved books
            if (reserved_books.size() > 0) {
                std::cout << "\nReserved Books:" << std::endl;
                std::vector<std::vector<std::string>> table;
                table.push_back({"Book Title", "ISBN", "Author", "Publisher"});
                for (auto book : reserved_books) {
                    std::vector<std::string> row;
                    row.push_back(book->get_title());
                    row.push_back(book->get_ISBN());
                    row.push_back(book->get_author());
                    row.push_back(book->get_publisher());
                    table.push_back(row);
                }
                Util::printTable(table);
            } else {
                std::cout << "\nNo reserved books." << std::endl;
            }
            
            std::cout << "\nEnter the ISBN of the book you want to return or cancel reservation" << std::endl;
            std::string ISBN;
            getline(std::cin, ISBN);
            Book* book = library->search_book(ISBN);
            if(book == NULL){
                std::cout << "Book not found!" << std::endl;
                state = LOGGED_IN_MAIN_MENU;
                break;
            }
            else{
                // Check if the book is either borrowed by the user or reserved by the user
                bool is_borrowed = book->is_issued_to(user);
                bool is_reserved = (book->get_reserved_by() == user);
                
                if(!is_borrowed && !is_reserved){
                    std::cout << "You have not borrowed or reserved this book!" << std::endl;
                    state = LOGGED_IN_MAIN_MENU;
                    break;
                }
                
                if (is_borrowed) {
                    // Handle book return
                    int fine = library->return_book(user, book);
                    std::cout << "Successfully returned book!" << std::endl;
                    
                    if (is_reserved) {
                        // Inform the user that their reservation was also canceled
                        std::cout << "Your reservation for this book has been canceled as well." << std::endl;
                    }
                    
                    if(fine > 0){
                        std::cout << "This book was overdue, so you have a fine of " << fine << std::endl;
                    }
                } else if (is_reserved) {
                    // Handle reservation cancellation
                    user->unreserve_book(book);
                    book->clear_reservation();
                    std::cout << "Your reservation for this book has been canceled." << std::endl;
                }
                
                state = DUMMY_INPUT;
            }
            break;   
        }
        case VIEW_AND_PAY_FINES:
        {
            CLEAR_TERMINAL();
            std::cout << "Your current dues are: " << user->get_dues(library->get_current_date()) << std::endl;
            if(user->get_dues(library->get_current_date()) == 0){

                state = DUMMY_INPUT;
                break;
            }
            // std::cout << "Would you like to pay your dues?" << std::endl;
            pre_message_string = "Would you like to pay your dues?";
            options = {"Yes", "No"};
            int option = get_option();
            if(option == 1){
                std::cout << "Please enter the amount you would like to pay" << std::endl;
                std::string amount;
                getline(std::cin, amount);
                try{
                    int amount_int = stoi(amount);
                    if(amount_int < 0){
                        std::cout << "Invalid amount" << std::endl;
                        state = LOGGED_IN_MAIN_MENU;
                        break;
                    }
                    int dues = user->get_dues(library->get_current_date());
                    if(amount_int > dues){
                        std::cout << "You cannot pay more than your dues" << std::endl;
                        state = LOGGED_IN_MAIN_MENU;
                        break;
                    }
                    user->pay_dues(amount_int);
                    std::cout << "Successfully paid dues!" << std::endl;
                    std::cout << "Pending dues: " << user->get_dues(library->get_current_date()) << std::endl;
                    state = LOGGED_IN_MAIN_MENU;
                }
                catch(const std::exception& e){
                    std::cout << "Invalid amount" << std::endl;
                    state = LOGGED_IN_MAIN_MENU;
                    break;
                }
            }
            else{
                state = LOGGED_IN_MAIN_MENU;
            }
            break;
        }
        case ADD_BOOK:{
            std::cout << "Enter title" << std::endl;
            std::string title;
            getline(std::cin, title);
            std::cout << "Enter author" << std::endl;
            std::string author;
            getline(std::cin, author);
            std::cout << "Enter publisher" << std::endl;
            std::string publisher;
            getline(std::cin, publisher);
            std::cout << "Enter ISBN" << std::endl;
            std::string ISBN;
            getline(std::cin, ISBN);
            if(library->add_book(title, author, publisher, ISBN)){
                std::cout << "Successfully added book!" << std::endl;
            }
            else{
                std::cout << "Book already exists!" << std::endl;
            }
            state = DUMMY_INPUT;
            break;
        }
        case VIEW_USERS:{
            CLEAR_TERMINAL();
            std::vector<User*> users = library->get_users();
            if(users.size() == 0){
                std::cout << "No users found!" << std::endl;
            }
            else{
                std::vector<std::vector<std::string>> table;
                table.push_back({"Username", "Type"});
                for(auto i = 0; i < users.size(); i++){
                    std::vector<std::string> row;
                    row.push_back(users[i]->get_username());
                    switch(library->get_user_type(users[i])){
                        case Library::UserType::STUDENT:
                            row.push_back("Student");
                            break;
                        case Library::UserType::FACULTY:
                            row.push_back("Faculty");
                            break;
                        case Library::UserType::LIBRARIAN:
                            row.push_back("Librarian");
                            break;
                        default:
                            break;
                    }
                    table.push_back(row);
                }
                Util::printTable(table);
            }
            state = DUMMY_INPUT;
            break;
        }
        case ADD_USER:{
            CLEAR_TERMINAL();
            pre_message_string = "Add Student or Faculty?";
            options = {"Add Student", "Add Faculty"};
            int option = get_option();
            std::cout << "Enter username" << std::endl;
            std::string username;
            getline(std::cin, username);
            std::cout << "Enter password" << std::endl;
            std::string password;
            getline(std::cin, password);
            password = Util::password_hash(password);
            if(option == 1){
                if(library->add_student(username, password)){
                    pre_message_string = "Successfully added student!";
                }
                else{
                    pre_message_string = "Student already exists!";
                }
            }
            else if(option == 2){
                if(library->add_faculty(username, password)){
                    pre_message_string = "Successfully added faculty!";
                }
                else{
                    pre_message_string = "Faculty already exists!";
                }
            } 
            CLEAR_TERMINAL();
            state = LOGGED_IN_MAIN_MENU;
            break;
        }
        case REMOVE_USER:{
            CLEAR_TERMINAL(); 
            std::cout << "Enter username" << std::endl;
            std::string username;
            getline(std::cin, username);
            if(library->remove_user(username)){
                std::cout << "Removed user successfully!" << std::endl;
            } 
            else{
                std::cout << "Ensure user exists and has no borrowed/reserved books" << std::endl;
            }
            state = DUMMY_INPUT;
            break;
        }
        case REMOVE_BOOK:{
            CLEAR_TERMINAL();
            std::cout << "Enter ISBN of the book to remove" << std::endl;
            std::string ISBN;
            getline(std::cin, ISBN);
            if(library->remove_book(ISBN)){
                pre_message_string = "Successfully removed book!";
            }
            else{
                pre_message_string = "Book does not exist or it is borrowed or reserved!";
            }
            state = LOGGED_IN_MAIN_MENU;
            break;
        }
        case REGISTER:
        {
            pre_message_string = "Register as student or faculty?";
            options = {"Student", "Faculty"};
            int option = get_option();

            std::cout << "Enter username" << std::endl;
            std::string username;
            getline(std::cin, username);
            std::cout << "Enter password" << std::endl;
            std::string password;
            getline(std::cin, password);
            if(option == 1){
                if(library->add_student(username, Util::password_hash(password))){
                    std::cout << "Successfully registered!" << std::endl;
                    state = LOGGED_OUT_MAIN_MENU;
                }
                else{
                    std::cout << "Username already exists. Please try again" << std::endl;
                }
            }
            else if(option == 2){
                if(library->add_faculty(username, Util::password_hash(password))){
                    std::cout << "Successfully registered!" << std::endl;
                    state = LOGGED_OUT_MAIN_MENU;
                }
                else{
                    std::cout << "Username already exists. Please try again" << std::endl;
                }
            } 
            break;
        }
        case LOGOUT:
        {
            is_logged_in = false;
            user = NULL;
            pre_message_string = "Welcome to the Library Management System!";
            state = LOGGED_OUT_MAIN_MENU;
            break;
        }
        case USER_UPDATE_PROFILE:
        {
            CLEAR_TERMINAL();
            pre_message_string = "Update Profile Options";
            options = {"Change Username", "Change Password", "Return to Main Menu"};
            int option = get_option();
            if(option == 1){
                CLEAR_TERMINAL();
                std::cout << "Current Username: " << user->get_username() << std::endl;
                std::cout << "Enter new username: " << std::endl;
                std::string new_username;
                getline(std::cin, new_username);
                
                if(library->update_username(user, new_username)){
                    std::cout << "Username updated successfully!" << std::endl;
                    pre_message_string = "Hi " + user->get_username() + "! ";
                    
                    // Append the user type to the pre_message_string
                    switch(user_type){
                        case Library::UserType::STUDENT:
                            pre_message_string += "You are logged in as a student";
                            break;
                        case Library::UserType::FACULTY:
                            pre_message_string += "You are logged in as a faculty";
                            break;
                        case Library::UserType::LIBRARIAN:
                            pre_message_string += "You are logged in as a librarian";
                            break;
                        default:
                            break;
                    }
                    pre_message_string += "! \n The date is " + library->get_current_date().to_string();
                }
                else{
                    std::cout << "Username already exists. Please try a different username." << std::endl;
                }
                state = DUMMY_INPUT;
            }
            else if(option == 2){
                CLEAR_TERMINAL();
                std::cout << "Enter current password: " << std::endl;
                std::string old_password;
                getline(std::cin, old_password);
                
                std::cout << "Enter new password: " << std::endl;
                std::string new_password;
                getline(std::cin, new_password);
                
                // Hash the passwords before comparing
                std::string hashed_old_password = Util::password_hash(old_password);
                std::string hashed_new_password = Util::password_hash(new_password);
                
                if(library->update_password(user, hashed_old_password, hashed_new_password)){
                    std::cout << "Password updated successfully!" << std::endl;
                }
                else{
                    std::cout << "Incorrect current password. Password update failed." << std::endl;
                }
                state = DUMMY_INPUT;
            }
            else if(option == 3){
                state = LOGGED_IN_MAIN_MENU;
            }
            break;
        }
        
        case CHANGE_SIMULATED_DATE:
        {
            Date currentDate = library->get_current_date();
            std::cout << "Current date is " << currentDate.to_string() << " (" << currentDate.get_date_as_int() << ")" << std::endl;
            std::cout << "Enter the new date (DD-MM-YYYY format):" << std::endl;
            std::string new_date;
            getline(std::cin, new_date);
            
            try {
                // Check if the input is empty
                if(new_date.empty()) {
                    std::cout << "Date cannot be empty. Please try again." << std::endl;
                    state = LOGGED_IN_MAIN_MENU;
                    break;
                }
                
                // Create a Date object from the string input
                Date newDateObj(new_date);
                
                // Check if the new date is in the future
                if(!newDateObj.is_future_date(currentDate)) {
                    std::cout << "New date cannot be in the past. Please set a future date." << std::endl;
                    state = LOGGED_IN_MAIN_MENU;
                    break;
                }
                
                // Set the new date
                library->set_current_date(newDateObj);
                std::cout << "Successfully changed the date to " << newDateObj.to_string() 
                          << " (" << newDateObj.get_date_as_int() << ")" << std::endl;
                state = DUMMY_INPUT;
            }
            catch(const std::exception& e) {
                std::cout << "Invalid date format. Please use DD-MM-YYYY format." << std::endl;
                state = LOGGED_IN_MAIN_MENU;
            }
            break;
        }
        case VIEW_NOTIFICATIONS:
        {
            show_notifications();
            state = DUMMY_INPUT;
            break;
        }
        case DUMMY_INPUT:
        {
            std::cout << "Press enter to return to main menu!" << std::endl;
            std::string dummy;
            getline(std::cin, dummy);
            CLEAR_TERMINAL();
            std::string user_string = user_type == Library::UserType::STUDENT ? "student" : user_type == Library::UserType::FACULTY ? "faculty" : "librarian";
            pre_message_string = "Hi " + user->get_username() + "! You are logged in as a " + user_string + "! \n The date is " + library->get_current_date().to_string();
            state = LOGGED_IN_MAIN_MENU;
        }
        default:
            break;
    }
}

void Session::show_notifications()
{
    CLEAR_TERMINAL();
    std::vector<std::string> notifications = user->get_notifications();
    if (notifications.empty()) {
        std::cout << "No notifications." << std::endl;
    } else {
        std::cout << "Notifications:" << std::endl;
        for (const auto& notification : notifications) {
            std::cout << "- " << notification << std::endl;
        }
    }
    std::cout << "Press enter to return to main menu!" << std::endl;
    std::string dummy;
    getline(std::cin, dummy);
    CLEAR_TERMINAL();
    state = LOGGED_IN_MAIN_MENU;
}

void Session::start_session()
{
    CLEAR_TERMINAL();
    state = LOAD_OR_CREATE_LIBRARY;
    while(state != EXIT){
        construct_options();
    }
}

bool Session::can_faculty_borrow_book(const Date& current_date) {
    auto borrowed_books = user->get_borrowed_books();
    for (const auto& transaction : borrowed_books) {
        if (transaction.return_date.get_timestamp() < current_date.get_timestamp()) {
            return false;
        }
    }
    return true;
}