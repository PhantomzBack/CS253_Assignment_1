#ifndef DATE_HPP
#define DATE_HPP

#include <string>
#include <ctime>

class Date {
private:
    time_t timestamp;  // Unix timestamp for more accurate date representation
    
public:
    Date(int day);  // Keep for backward compatibility
    Date(const std::string& dateStr);  // New constructor for string input
    Date();  // Default constructor for current date
    
    // Existing functions
    int get_date_as_int();
    int get_day_difference(const Date &date);
    
    // New functions
    bool is_future_date(const Date& referenceDate = Date()) const;
    std::string to_string() const;
    time_t get_timestamp() const;
    
    // Static utility function
    static bool validate_date_string(const std::string& dateStr);
};

#endif // DATE_HPP