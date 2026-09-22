//
// Created by arch on 9/22/26.
//

#ifndef PASSWORD_MANAGER_CSV_PARSER_HPP
#define PASSWORD_MANAGER_CSV_PARSER_HPP
#include <expected>
#include <vector>

#include "domain/service.hpp"
#include "domain/error/error.hpp"


class CSVParser
{
public:
    static std::expected<std::vector<Service>, err::Error> parseCSV(std::string file_path);
    static std::expected<void, err::Error> exportCSV(std::string file_path, const std::unordered_map<std::uint32_t,Service>& services);
};


#endif //PASSWORD_MANAGER_CSV_PARSER_HPP
