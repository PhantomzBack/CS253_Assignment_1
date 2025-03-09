#include <iostream>
#include <vector>
#include <iomanip>

namespace Util {
    void printTable(const std::vector<std::vector<std::string>>& table);
    std::string password_hash(std::string raw_password);
}