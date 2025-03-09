#ifndef ID_MANAGER_HPP
#define ID_MANAGER_HPP

#include <string>
#include <unordered_set>

class ID_Manager {
private:
    int id;
    std::string id_prefix;
    std::string generate_next_id();

public:
    ID_Manager();
    void set_id_prefix(std::string id_prefix);
    std::string get_id(const std::unordered_set<std::string>& existing_ids); // New method to get unique ID
};

#endif // ID_MANAGER_HPP