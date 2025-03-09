#include "Date.hpp"
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

// Default constructor - uses current date
Date::Date() {
    timestamp = time(nullptr);
}

// Constructor using integer representation (for backward compatibility)
Date::Date(int day) {
    // Special case: -1 represents a special date (e.g., for reserved books)
    if (day == -1) {
        // Set to a special timestamp value to indicate this is a special date
        timestamp = -1;
        return;
    }
    
    // Convert the integer format (YYYYMMDD) to a timestamp
    std::tm timeinfo = {};
    int year = day / 10000;
    int month = (day % 10000) / 100;
    int dayOfMonth = day % 100;
    
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = dayOfMonth;
    timeinfo.tm_hour = 0;
    timeinfo.tm_min = 0;
    timeinfo.tm_sec = 0;
    
    timestamp = mktime(&timeinfo);
}

// Constructor using string representation (new functionality)
Date::Date(const std::string& dateStr) {
    std::tm timeinfo = {};
    std::istringstream ss(dateStr);
    
    // Try to parse date in format dd-mm-yyyy
    ss >> std::get_time(&timeinfo, "%d-%m-%Y");
    
    // If parsing failed, try DDMMYYYY format
    if (ss.fail()) {
        ss.clear();
        ss.str(dateStr);
        int dateInt;
        if (ss >> dateInt) {
            // Special case: -1 represents a special date
            if (dateInt == -1) {
                timestamp = -1;
                return;
            }
            
            int year = dateInt / 10000;
            int month = (dateInt % 10000) / 100;
            int dayOfMonth = dateInt % 100;
            
            timeinfo.tm_year = year - 1900;
            timeinfo.tm_mon = month - 1;
            timeinfo.tm_mday = dayOfMonth;
        } else {
            // Default to current date if parsing fails
            std::time_t t = std::time(nullptr);
            timeinfo = *std::localtime(&t);
        }
    }
    timeinfo.tm_hour = 0;
    timeinfo.tm_min = 0;
    timeinfo.tm_sec = 0;
    
    timestamp = mktime(&timeinfo);
}

// Get date as integer in format YYYYMMDD (for backward compatibility)
int Date::get_date_as_int() {
    // Special case: handle -1 timestamp
    if (timestamp == -1) {
        return -1;
    }
    
    std::tm* timeinfo = std::localtime(&timestamp);
    int year = timeinfo->tm_year + 1900;
    int month = timeinfo->tm_mon + 1;
    int day = timeinfo->tm_mday;
    
    return year * 10000 + month * 100 + day;
}

// Calculate difference in days between two dates
int Date::get_day_difference(const Date& date) {
    // Handle special cases
    if (timestamp == -1 || date.timestamp == -1) {
        return 0; // Special dates have no meaningful difference
    }
    
    // Calculate difference in seconds and convert to days
    double seconds = difftime(timestamp, date.timestamp);
    return static_cast<int>(seconds / (60 * 60 * 24));
}

// Check if this date is in the future compared to reference date
bool Date::is_future_date(const Date& referenceDate) const {
    // Handle special cases
    if (timestamp == -1) {
        return true; // Special date is always considered "in the future"
    }
    if (referenceDate.timestamp == -1) {
        return false; // Nothing is in the future compared to a special date
    }
    
    return difftime(timestamp, referenceDate.timestamp) > 0;
}

// Convert date to string (dd-mm-yyyy format)
std::string Date::to_string() const {
    // Handle special case
    if (timestamp == -1) {
        return "Reserved"; // Or another appropriate string for the special date
    }
    
    std::tm* timeinfo = std::localtime(&timestamp);
    std::ostringstream ss;
    ss << std::put_time(timeinfo, "%d-%m-%Y");
    return ss.str();
}

// Get raw timestamp
time_t Date::get_timestamp() const {
    return timestamp;
}

// Validate date string format
bool Date::validate_date_string(const std::string& dateStr) {
    // Special case: -1 is a valid special date
    if (dateStr == "-1") {
        return true;
    }
    
    std::tm timeinfo = {};
    std::istringstream ss(dateStr);
    
    // Try to parse date in format dd-mm-yyyy
    ss >> std::get_time(&timeinfo, "%d-%m-%Y");
    if (!ss.fail()) {
        return true;
    }
    
    // Try DDMMYYYY format
    ss.clear();
    ss.str(dateStr);
    int dateInt;
    if (ss >> dateInt) {
        if (dateInt == -1) {
            return true; // Special date is valid
        }
        
        int year = dateInt / 10000;
        int month = (dateInt % 10000) / 100;
        int day = dateInt % 100;
        
        // Basic validation
        return (year >= 1900 && year <= 9999 && 
                month >= 1 && month <= 12 && 
                day >= 1 && day <= 31);
    }
    
    return false;
}