#include "Library.hpp"
#include "Librarian.hpp"
#include <fstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_set>
#include "json.hpp"

Library::Library(std::string _librarian_username, std::string _librarian_pwd, 
                 std::string _student_id_prefix, std::string _faculty_id_prefix, std::string _book_id_prefix) 
    : student_id_prefix(_student_id_prefix), faculty_id_prefix(_faculty_id_prefix), book_id_prefix(_book_id_prefix), current_date()
{
    core_librarian = new Librarian(this, "L", _librarian_username, _librarian_pwd);
    student_id_manager.set_id_prefix(student_id_prefix);
    faculty_id_manager.set_id_prefix(faculty_id_prefix);
    book_id_manager.set_id_prefix(book_id_prefix);
    users.push_back(core_librarian);
}

void Library::set_current_date(Date new_date)
{
    assert(new_date.get_date_as_int() >= current_date.get_date_as_int());
    current_date = new_date;
}

Date Library::get_current_date()
{
    return current_date;
}

bool Library::borrow_book(User *user, Book *book)
{
    if (user->borrow_book(book, current_date))
    {
        book->issue_book(user);
        return true;
    }
    return false;
}

int Library::return_book(User* user, Book *book)
{
    int fine = user->return_book(book, current_date);
    if (fine >= 0)
    {
        // First check if this user had reserved this book
        if (book->get_reserved_by() == user)
        {
            // User is returning their own reserved book, so unreserve it
            user->unreserve_book(book);
            book->clear_reservation();
        }
        else if (book->get_reserved_by() != nullptr)
        {
            // Book is reserved by another user
            User* reserved_user = book->get_reserved_by();
            if (reserved_user->issue_reserved_book(book, current_date))
            {
                // Issue the book directly to the reserved user
                book->issue_book(reserved_user);
                book->clear_reservation(); // Clear the reservation after issuing the book
                notify_user(reserved_user, "The book '" + book->get_title() + "' has been issued to you automatically.");
            }
        }
        else
        {
            // No reservation, just return it normally
            book->return_book();
        }
        return fine;
    }
    return -1;
}

Library* Library::load_from_file(std::string filename)
{
    try
    {
        Library *library = new Library();
        std::ifstream file(filename);
        if (file.is_open())
        {
            nlohmann::json j = nlohmann::json::parse(file);

            if (j["current_date_str"].is_string())
            {
                library->current_date = Date(j["current_date_str"].get<std::string>());
            }
            else
            {
                library->current_date = Date(j["current_date"].get<int>());
            }

            library->student_id_prefix = j["student_id_prefix"];
            library->faculty_id_prefix = j["faculty_id_prefix"];
            library->book_id_prefix = j["book_id_prefix"];

            if (!j["books"].is_array())
            {
                std::cout << "Error: books should be an array" << std::endl;
                delete library;
                return nullptr;
            }

            std::unordered_set<std::string> existing_book_ids;
            for (int i = 0; i < j["books"].size(); i++)
            {
                Book *book = new Book(j["books"][i]["id"].get<std::string>(), j["books"][i]["title"].get<std::string>(),
                                      j["books"][i]["author"].get<std::string>(), j["books"][i]["publisher"].get<std::string>(),
                                      j["books"][i]["ISBN"].get<std::string>());
                existing_book_ids.insert(book->id);

                if (j["books"][i].contains("reserved_by"))
                {
                    std::string reserved_by_id = j["books"][i]["reserved_by"].get<std::string>();
                    User *reserved_by_user = library->search_user(reserved_by_id);
                    if (reserved_by_user)
                    {
                        book->reserve_book(reserved_by_user);
                        reserved_by_user->reserve_book(book);
                    }
                }

                library->books.push_back(book);
            }
            // library->book_id_manager.set_existing_ids(existing_book_ids);

            std::unordered_set<std::string> existing_user_ids;
            for (int i = 0; i < j["users"].size(); i++)
            {
                if (j["users"][i]["id"].get<std::string>().rfind(library->student_id_prefix, 0) != std::string::npos)
                {
                    User *user = new Student(j["users"][i]["id"].get<std::string>(), j["users"][i]["username"].get<std::string>(),
                                             j["users"][i]["password"].get<std::string>());
                    existing_user_ids.insert(user->id);
                    user->personal_account.due_fine = j["users"][i]["dues"].get<int>();
                    for(int k = 0; k < j["users"][i]["notifications"].size(); k++)
                    {
                        user->add_notification(j["users"][i]["notifications"][k].get<std::string>());
                    }
                    std::vector<Transaction> borrowed_books, history;
                    for (int k = 0; k < j["users"][i]["transaction_data"]["reserved_books"].size(); k++)
                    {
                        std::string book_id = j["users"][i]["transaction_data"]["reserved_books"][k]["book"].get<std::string>();
                        Book *book = library->search_book_by_ID(book_id);
                        book->reserve_book(user);
                        user->reserve_book(book);
                    }
                    for (int k = 0; k < j["users"][i]["transaction_data"]["borrowed_books"].size(); k++)
                    {
                        std::string book_id = j["users"][i]["transaction_data"]["borrowed_books"][k]["book"].get<std::string>();
                        Book *book = library->search_book_by_ID(book_id);

                        Date borrow_date;
                        if (j["users"][i]["transaction_data"]["borrowed_books"][k]["borrow_date_str"].is_string())
                        {
                            borrow_date = Date(j["users"][i]["transaction_data"]["borrowed_books"][k]["borrow_date_str"].get<std::string>());
                        }
                        else
                        {
                            borrow_date = Date(j["users"][i]["transaction_data"]["borrowed_books"][k]["borrow_date"].get<int>());
                        }

                        Date return_date;
                        if (j["users"][i]["transaction_data"]["borrowed_books"][k]["return_date_str"].is_string())
                        {
                            return_date = Date(j["users"][i]["transaction_data"]["borrowed_books"][k]["return_date_str"].get<std::string>());
                        }
                        else
                        {
                            return_date = Date(j["users"][i]["transaction_data"]["borrowed_books"][k]["return_date"].get<int>());
                        }

                        Transaction transaction = {book, user, borrow_date, return_date,
                                                   j["users"][i]["transaction_data"]["borrowed_books"][k]["fine_paid"].get<int>(),
                                                   j["users"][i]["transaction_data"]["borrowed_books"][k]["pending_fine"].get<int>()};
                        book->issue_book(user);
                        borrowed_books.emplace_back(transaction);
                    }

                    for (int k = 0; k < j["users"][i]["transaction_data"]["history"].size(); k++)
                    {
                        std::string book_id = j["users"][i]["transaction_data"]["history"][k]["book"].get<std::string>();
                        Book *book = library->search_book_by_ID(book_id);

                        Date borrow_date;
                        if (j["users"][i]["transaction_data"]["history"][k]["borrow_date_str"].is_string())
                        {
                            borrow_date = Date(j["users"][i]["transaction_data"]["history"][k]["borrow_date_str"].get<std::string>());
                        }
                        else
                        {
                            borrow_date = Date(j["users"][i]["transaction_data"]["history"][k]["borrow_date"].get<int>());
                        }

                        Date return_date;
                        if (j["users"][i]["transaction_data"]["history"][k]["return_date_str"].is_string())
                        {
                            return_date = Date(j["users"][i]["transaction_data"]["history"][k]["return_date_str"].get<std::string>());
                        }
                        else
                        {
                            return_date = Date(j["users"][i]["transaction_data"]["history"][k]["return_date"].get<int>());
                        }

                        Transaction transaction = {book, user, borrow_date, return_date,
                                                   j["users"][i]["transaction_data"]["history"][k]["fine_paid"].get<int>(),
                                                   j["users"][i]["transaction_data"]["history"][k]["pending_fine"].get<int>()};
                        history.emplace_back(transaction);
                    }

                    user->personal_account.borrowed_books = borrowed_books;
                    user->personal_account.history = history;
                    library->users.push_back(user);
                }
                else if (j["users"][i]["id"].get<std::string>().rfind(library->faculty_id_prefix, 0) != std::string::npos)
                {
                    User *user = new Faculty(j["users"][i]["id"].get<std::string>(), j["users"][i]["username"].get<std::string>(),
                                             j["users"][i]["password"].get<std::string>());
                    existing_user_ids.insert(user->id);
                    library->users.push_back(user);
                }
                else if (j["users"][i]["id"].get<std::string>().at(0) == 'L')
                {
                    User *user = new Librarian(library, j["users"][i]["id"].get<std::string>(), j["users"][i]["username"].get<std::string>(),
                                               j["users"][i]["password"].get<std::string>());
                    existing_user_ids.insert(user->id);
                    library->users.push_back(user);
                }
            }
            // library->student_id_manager.set_existing_ids(existing_user_ids);
            // library->faculty_id_manager.set_existing_ids(existing_user_ids);

            library->core_librarian = new Librarian(library, j["core_librarian"]["id"], j["core_librarian"]["username"], j["core_librarian"]["password"]);
            file.close();
            return library;
        }
        return nullptr;
    }
    catch (...)
    {
        return nullptr;
    }
}

bool Library::add_book(std::string title, std::string author, std::string publisher, std::string ISBN)
{
    for (int i = 0; i < books.size(); i++)
    {
        if (books[i]->get_ISBN() == ISBN || books[i]->get_title() == title)
            return false;
    }
    
    // Collect existing book IDs
    std::unordered_set<std::string> existing_ids;
    for (const auto& book : books) {
        existing_ids.insert(book->id);
    }
    
    // Generate a unique ID for the new book
    std::string new_book_id = book_id_manager.get_id(existing_ids);
    Book *book = new Book(new_book_id, title, author, publisher, ISBN);
    books.push_back(book);
    return true;
}

bool Library::add_student(std::string username, std::string password)
{
    for (int i = 0; i < users.size(); i++)
    {
        if (users[i]->get_username() == username)
            return false;
    }
    
    // Collect existing student IDs
    std::unordered_set<std::string> existing_ids;
    for (const auto& user : users) {
        if (dynamic_cast<Student*>(user)) {
            existing_ids.insert(user->id);
        }
    }
    
    // Generate a unique ID for the new student
    std::string new_student_id = student_id_manager.get_id(existing_ids);
    User *user = new Student(new_student_id, username, password);
    users.push_back(user);
    return true;
}

bool Library::add_faculty(std::string username, std::string password)
{   
    for (int i = 0; i < users.size(); i++)
    {
        if (users[i]->get_username() == username)
            return false;
    }
    
    // Collect existing faculty IDs
    std::unordered_set<std::string> existing_ids;
    for (const auto& user : users) {
        if (dynamic_cast<Faculty*>(user)) {
            existing_ids.insert(user->id);
        }
    }
    
    // Generate a unique ID for the new faculty
    std::string new_faculty_id = faculty_id_manager.get_id(existing_ids);
    User *user = new Faculty(new_faculty_id, username, password);
    users.push_back(user);
    return true;
}

std::vector<User*> Library::get_users()
{
    return users;
}

Book* Library::search_book(std::string ISBN)
{
    for (int i = 0; i < books.size(); i++)
    {
        if (books[i]->get_ISBN() == ISBN)
            return books[i];
    }
    return NULL;
}

Book* Library::search_book_by_ID(std::string id)
{
    for (int i = 0; i < books.size(); i++)
    {
        if (books[i]->id == id)
            return books[i];
    }
    return NULL;
}

std::vector<Book*> Library::search_books_by_title_author_publisher(std::string expression)
{
    std::vector<Book*> return_books;
    for(auto i = 0; i < books.size(); i++)
    {
        std::string book_title = books[i]->get_title();
        std::string book_author = books[i]->get_author();
        std::string book_publisher = books[i]->get_publisher();
        std::transform(book_title.begin(), book_title.end(), book_title.begin(), ::tolower);
        std::transform(book_author.begin(), book_author.end(), book_author.begin(), ::tolower);
        std::transform(book_publisher.begin(), book_publisher.end(), book_publisher.begin(), ::tolower);
        std::transform(expression.begin(), expression.end(), expression.begin(), ::tolower);
        if(book_title.find(expression) != std::string::npos || book_author.find(expression) != std::string::npos || book_publisher.find(expression) != std::string::npos)
        {
            return_books.emplace_back(books[i]);
        }
    }
    return return_books;
}

User* Library::search_user(std::string username)
{
    for (int i = 0; i < users.size(); i++)
    {
        if (users[i]->get_username() == username)
            return users[i];
    }
    return NULL;
}

User* Library::login(std::string username, std::string password)
{
    User *user = search_user(username);
    if (user && user->compare_password(password))
    {
        return user;
    }
    return NULL;
}

bool Library::write_to_file(std::string filename)
{
    std::cout << "Starting to write to file: " << filename << std::endl;
    nlohmann::json j;
    j["current_date"] = current_date.get_date_as_int();
    j["current_date_str"] = current_date.to_string();
    
    for (int i = 0; i < books.size(); i++)
    {
        nlohmann::json book_json = {{"id", books[i]->id}, {"title", books[i]->get_title()}, {"author", books[i]->get_author()}, 
                            {"publisher", books[i]->get_publisher()}, {"ISBN", books[i]->get_ISBN()}};
        
        if (books[i]->get_reserved_by() != nullptr) {
            book_json["reserved_by"] = books[i]->get_reserved_by()->id;
        }
        
        j["books"].push_back(book_json);
    }
    std::cout << "Books serialized" << std::endl;
    for (int i = 0; i < users.size(); i++)
    {
        if(dynamic_cast<Librarian*>(users[i])){
            continue;
        }
        nlohmann::json transaction_data;
        transaction_data["borrowed_books"] = nlohmann::json::array();
        transaction_data["history"] = nlohmann::json::array();
        transaction_data["notifications"] = nlohmann::json::array();
        transaction_data["reserved_books"] = nlohmann::json::array();
        for(int j = 0; j < users[i]->notifications.size(); j++){
            transaction_data["notifications"].push_back(users[i]->notifications[j]);
        }
        for(int j = 0; j < users[i]->personal_account.history.size(); j++)
        {
            nlohmann::json transaction;
            transaction["book"] = users[i]->personal_account.history[j].book->id;
            
            transaction["borrow_date"] = users[i]->personal_account.history[j].borrow_date.get_date_as_int();
            transaction["borrow_date_str"] = users[i]->personal_account.history[j].borrow_date.to_string();
            
            transaction["return_date"] = users[i]->personal_account.history[j].return_date.get_date_as_int();
            transaction["return_date_str"] = users[i]->personal_account.history[j].return_date.to_string();
            
            transaction["fine_paid"] = users[i]->personal_account.history[j].fine_paid;
            transaction["pending_fine"] = users[i]->personal_account.history[j].pending_fine;
            transaction_data["history"].push_back(transaction);
        }
        
        for(int j = 0; j < users[i]->personal_account.reserved_books.size(); j++){
            nlohmann::json transaction;
            transaction["book"] = users[i]->personal_account.reserved_books[j]->id;
            transaction_data["reserved_books"].push_back(transaction);
        }
        for(int j = 0; j < users[i]->personal_account.borrowed_books.size(); j++)
        {
            nlohmann::json transaction;
            transaction["book"] = users[i]->personal_account.borrowed_books[j].book->id;
            
            transaction["borrow_date"] = users[i]->personal_account.borrowed_books[j].borrow_date.get_date_as_int();
            transaction["borrow_date_str"] = users[i]->personal_account.borrowed_books[j].borrow_date.to_string();
            
            transaction["return_date"] = users[i]->personal_account.borrowed_books[j].return_date.get_date_as_int();
            transaction["return_date_str"] = users[i]->personal_account.borrowed_books[j].return_date.to_string();
            
            transaction["fine_paid"] = users[i]->personal_account.borrowed_books[j].fine_paid;
            transaction["pending_fine"] = users[i]->personal_account.borrowed_books[j].pending_fine;
            transaction_data["borrowed_books"].push_back(transaction);
        }
        
        j["users"].push_back({{"id", users[i]->id}, {"username", users[i]->get_username()}, {"password", users[i]->password}, 
                           {"dues", users[i]->get_dues(current_date)}, {"transaction_data", transaction_data}});
    }
    std::cout << "Users serialized" << std::endl;
    j["student_id_prefix"] = student_id_prefix;
    j["faculty_id_prefix"] = faculty_id_prefix;
    j["book_id_prefix"] = book_id_prefix;
    j["core_librarian"] = {{"id", core_librarian->id}, {"username", core_librarian->get_username()}, {"password", core_librarian->password}};
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << j.dump(4);
        file.close();
        std::cout << "Successfully wrote to file" << std::endl;
        return true;
    }
    std::cout << "Failed to open file" << std::endl;
    return false;
}

Library::UserType Library::get_user_type(User* user)
{
    if(dynamic_cast<Student*>(user)){
        return UserType::STUDENT;
    }
    else if(dynamic_cast<Faculty*>(user)){
        return UserType::FACULTY;
    }
    else if(dynamic_cast<Librarian*>(user)){
        return UserType::LIBRARIAN;
    }
    return UserType::USER_NULL;
}

bool Library::update_username(User* user, std::string new_username)
{
    // Check if username already exists
    for (int i = 0; i < users.size(); i++)
    {
        if (users[i]->get_username() == new_username)
            return false; // Username already exists
    }
    
    // Update the username
    user->change_username(new_username);
    return true;
}

bool Library::update_password(User* user, std::string old_password, std::string new_password)
{
    return user->change_password(old_password, new_password);
}

bool Library::remove_user(std::string username)
{
    // Don't allow removing the core librarian
    if (username == core_librarian->get_username()) {
        return false;
    }
    
    for (auto it = users.begin(); it != users.end(); ++it) {
        if ((*it)->get_username() == username) {
            // Check if user has borrowed books
            if ((*it)->get_borrowed_books_count() > 0) {
                return false; // Can't remove users with borrowed books
            }
            
            // Free memory 
            delete *it;
            users.erase(it);
            return true;
        }
    }
    return false; // User not found
}

bool Library::remove_book(std::string ISBN)
{
    for (auto it = books.begin(); it != books.end(); ++it) {
        if ((*it)->get_ISBN() == ISBN) {
            // Check if book is borrowed or reserved
            if ((*it)->get_issued_to() != nullptr || (*it)->get_reserved_by() != nullptr) {
                return false; 
            }
            
            // Free memory
            delete *it;
            books.erase(it);
            return true;
        }
    }
    return false; // Book not found
}

bool Library::reserve_book(User *user, Book *book)
{
    if (book->reserve_book(user)) {
        // Also add the book to the user's reserved books
        user->reserve_book(book);
        return true;
    }
    return false;
}

void Library::notify_user(User *user, std::string message)
{
    user->add_notification(message);
}