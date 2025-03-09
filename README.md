# Library Management System

This is a command-line library management system implemented in C++ as a part of the course CS253. The system provides functionality for managing books, users, and transactions for use in a library like setting.

## Features

- User management (Students, Faculty, Librarian)
- Book management (Add, Search, Borrow, Return)
- Fine calculation and payment for overdue books
- Persistent storage using JSON file format
- Different borrowing policies for different user types
- Date simulation for testing features
- Hashing is removed so that for testing, it is easy to see credentials and use the software. 

## Assumptions
- The ISBN is used as the unique identifier apart from ID (I was not aware that this wasn't the case until late and hence couldn't change this)
- The username is used as the unique indentifier for the user. ID's are only used for interal management in the file storage
- **PLEASE NOTE:** The file changes are only saved when logging out and selecting the option for close library. The file changes are not synced before that. I let this be on purpose so that the file isn't constantly updated and that the person testing the code need not constantly create copies manually if they wish to go back to some point in the past.
- md5.h and picohash.h can be used to introduce hashing capabilities for storing the password moree securely.


## How to Run

### Prerequisites

- C++11 compatible compiler (g++ recommended, change in makefile if using anything else)
- Make (for using the provided Makefile, which I've copied from my usual makefile template and added header dependencies which might now be outdated)

### Compilation

To compile the project, navigate to the project directory and run:

```bash
make
```

This will generate an executable named `library_management_system`.

### Running the Application

To run the application after compilation:

```bash
make run
```

Or directly:

```bash
./library_management_system
```

### Initial Login

The default librarian credentials are:
- Username: admin
- Password: admin

## Class Overview

### Core Classes


## The Session:
This class acts as interface between the library classes and the user by implementing the command line interface. This is done to seperate the interface code from the management code to the best extent possible, so that the management code could be reused as required with a different interface.

1. **Library**: Central class that manages books, users, and transactions. Also handles file I/O operations ensuring consistency between runs by serializing data onto files and loading later on.

2. **Book**: Represents a book with properties like title, author, publisher, and ISBN. Tracks availability status.

3. **User**: Base class for all user types with common functionality.
   - **Student**: Users with limited borrowing privileges and subject to fines.
   - **Faculty**: Users with extended borrowing privileges.
   - **Librarian**: Administrative users who can add books and users.

4. **Account**: Manages user-specific borrowing activities and history.

5. **Transaction**: A struct representing a book borrowing transaction with dates and fine information.

### Utility Classes

1. **Date**: Handles date representation and calculations for due dates and fines.

2. **ID_Manager**: Generates and manages unique IDs for books and users.

3. **Session**: Manages the user interface and interaction flow.

4. **Util**: Contains utility functions like password hashing and table formatting.

## Data Storage

The system uses JSON for data persistence. By default, it looks for a file named `library.json` to load data from, but this can be changed during runtime.

## External Libraries

This project uses the nlohmann/json library for JSON parsing:
- **json.hpp**: A modern C++ JSON parsing library by Niels Lohmann
- Source: https://github.com/nlohmann/json (MIT License)

## License
This project is for educational purposes for the course assignment of course CS253 @ IITK.