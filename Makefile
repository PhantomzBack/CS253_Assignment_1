CXX = g++
CXXFLAGS = -std=c++11 -g
TARGET = library_management_system
SRC_DIR = .

# List all source files
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/Book.cpp \
       $(SRC_DIR)/Date.cpp \
       $(SRC_DIR)/Account.cpp \
       $(SRC_DIR)/User.cpp \
       $(SRC_DIR)/Student.cpp \
       $(SRC_DIR)/Faculty.cpp \
       $(SRC_DIR)/Librarian.cpp \
       $(SRC_DIR)/Library.cpp \
       $(SRC_DIR)/ID_Manager.cpp \
       $(SRC_DIR)/Session.cpp \
       $(SRC_DIR)/util.cpp

# Generate object file names
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Header file dependencies (now outdated since i haven't included all the header files)
Book.o: Book.hpp User.hpp
Date.o: Date.hpp
Transaction.o: Transaction.hpp Book.hpp Date.hpp User.hpp
Account.o: Account.hpp Transaction.hpp Book.hpp Date.hpp User.hpp
User.o: User.hpp Account.hpp Book.hpp Date.hpp Transaction.hpp
Student.o: Student.hpp User.hpp
Faculty.o: Faculty.hpp User.hpp
Librarian.o: Librarian.hpp User.hpp Library.hpp
Library.o: Library.hpp Book.hpp User.hpp Student.hpp Faculty.hpp Librarian.hpp ID_Manager.hpp Date.hpp
ID_Manager.o: ID_Manager.hpp
Session.o: Session.hpp Library.hpp User.hpp util.hpp
util.o: util.hpp
main.o: Session.hpp

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run