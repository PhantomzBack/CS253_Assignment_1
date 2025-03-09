#include "ID_Manager.hpp"
#include <unordered_set>

ID_Manager::ID_Manager() : id(0) {}

std::string ID_Manager::generate_next_id()
{
    return std::to_string(id++);
}

void ID_Manager::set_id_prefix(std::string id_prefix)
{
    this->id_prefix = id_prefix;
}

std::string ID_Manager::get_id(const std::unordered_set<std::string>& existing_ids)
{
    std::string new_id;
    do {
        new_id = id_prefix + generate_next_id();
    } while (existing_ids.find(new_id) != existing_ids.end());
    return new_id;
}