#include <iostream>
#include <vector>
#include <iomanip>

namespace Util{
    void printTable(const std::vector<std::vector<std::string>>& table) {
        if (table.empty()) return;

        // Determine column widths
        std::vector<size_t> colWidths(table[0].size(), 0);
        for (const auto& row : table) {
            for (size_t i = 0; i < row.size(); ++i) {
                colWidths[i] = std::max(colWidths[i], row[i].size());
            }
        }

        // Function to print horizontal separator
        auto printSeparator = [&]() {
            std::cout << "+";
            for (size_t width : colWidths) {
                std::cout << std::string(width + 2, '-') << "+";
            }
            std::cout << "\n";
        };

        // Print the table
        printSeparator();
        for (const auto& row : table) {
            std::cout << "|";
            for (size_t i = 0; i < row.size(); ++i) {
                std::cout << " " << std::setw(colWidths[i]) << std::left << row[i] << " |";
            }
            std::cout << "\n";
            printSeparator();
        }
    }

    std::string password_hash(std::string raw_password){
        /*TODO: Add MD5 code*/
        return raw_password;
    }
}