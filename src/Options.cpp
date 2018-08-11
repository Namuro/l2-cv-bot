#include "Options.h"

#include <sstream>

std::string Options::String(const std::string &option, const std::string &default) const
{
    const auto found = Find(option);
    return found.has_value() ? found.value() : default;
}

int Options::Int(const std::string &option, int default) const
{
    const auto found = Find(option);
    
    if (!found.has_value()) {
        return default;
    }
    
    try {
        return std::stoi(found.value());
    } catch (...) {
        return default;
    }
}

double Options::Double(const std::string &option, double default) const
{
    const auto found = Find(option);

    if (!found.has_value()) {
        return default;
    }

    try {
        return std::stod(found.value());
    } catch (...) {
        return default;
    }
}

bool Options::Bool(const std::string &option, bool default) const
{
    const auto found = Find(option);

    if (!found.has_value()) {
        return default;
    }

    const auto value = found.value();

    if (value == "1" || value == "true" || value == "yes" || value == "y" || value == "on") {
        return true;
    } else if (value == "0" || value == "false" || value == "no" || value == "n" || value == "off") {
        return false;
    }

    return default;
}

std::vector<std::string> Options::StringVector(const std::string &option, const std::vector<std::string> &default) const
{
    const auto found = Find(option);

    if (!found.has_value()) {
        return default;
    }

    std::stringstream ss(found.value());
    std::vector<std::string> vector;

    while (ss.good()) {
        std::string string;
        std::getline(ss, string, ',');
        vector.push_back(string);
    }

    return vector;
}

std::vector<int> Options::IntVector(const std::string &option, const std::vector<int> &default) const
{
    const auto strings = StringVector(option);

    if (strings.empty()) {
        return default;
    }

    std::vector<int> vector;

    for (const auto &string : strings) {
        try {
            vector.push_back(std::stoi(string));
        } catch (...) {
            return default;
        }
    }

    return vector;
}

std::optional<std::string> Options::Find(const std::string &option, bool check_next) const
{
    auto found = std::find(m_options.begin(), m_options.end(), option);

    if (found != m_options.end() && (!check_next || ++found != m_options.end())) {
        return *found;
    }

    return {};
}
