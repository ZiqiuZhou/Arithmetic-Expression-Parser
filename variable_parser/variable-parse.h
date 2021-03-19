#pragma once

#ifndef INCLUDED_CPP_PRACTICE_EX08_VARIABLE_PARSE_H_
#define INCLUDED_CPP_PRACTICE_EX08_VARIABLE_PARSE_H_


#include <utility>      // for pair<>
#include <string_view>


namespace asc::cpp_practice_ws20::ex08 {


    // Parses the given variable substitution in the format <name>=<value>.
    // Throws `std::runtime_error` if the argument cannot be parsed.
    std::pair<std::string, double>
        parseVariableSubstitution(std::string_view str);


} // namespace asc::cpp_practice_ws20::ex08


#endif // INCLUDED_CPP_PRACTICE_EX08_VARIABLE_PARSE_HPP_
