
#include <iostream>
#include <stdexcept>      // for runtime_error
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include "expression.h"
#include "expression-evaluate.h"

#include "utf8-console.h"
#include "variable-parse.h"


using namespace asc::cpp_practice_ws20::ex08;


void
printVariableSubstitutions(
    std::unordered_map<std::string, double> const& variableSubstitutions)
{
    bool first = true;
    for (auto const& variableSubstitution : variableSubstitutions)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            std::cout << ", ";
        }
        std::cout << variableSubstitution.first << " = " << variableSubstitution.second;
    }
}


int main(int argc, char* argv[])
try
{
    enableUTF8Console();

    if (argc < 2 || std::string_view(argv[1]) == "-h")
    {
        static constexpr std::string_view helpString =
            R"raw(Usage:
  expr <expr> [<substitutions> ...]

Simplifies and evaluates the arithmetic expression given as command-line argument.
Variable substitutions have the form <name>=<value>.
)raw";

        std::cout << helpString;
        return 0;
    }

    // We can assume `argc > 1` henceforth.
    auto e = expr::Expression::parse(argv[1]);
    std::cout << "Expression: " << e << '\n'; // print expression

    auto variableSubstitutions = std::unordered_map<std::string, double>{ };
    for (int i = 2; i < argc; ++i)
    {
        variableSubstitutions.insert(parseVariableSubstitution(argv[i]));
    }
    if (!variableSubstitutions.empty())
    {
        std::cout << "Substitutions: ";
        printVariableSubstitutions(variableSubstitutions);
        std::cout << "\n";
    }

    double value = evaluate(e, variableSubstitutions);
    std::cout << "Value: " << value << '\n';
}
catch (std::runtime_error const& e)
{
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
}
