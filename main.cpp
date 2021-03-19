
#include <iostream>
#include <stdexcept>      // for runtime_error
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include "expression.h"
#include "expression-evaluate.h"

#include "utf8-console.h"
#include "variable-parse.h"

#include "lines.h"


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

std::vector<std::string>
preprocess2(
    FileReader fileReader,
    std::vector<std::string> const& commentPrefixes)
{
    std::vector<std::string> result;
    for (std::string_view line : linesInFile(std::move(fileReader)))
    {
        if (std::none_of(commentPrefixes.begin(), commentPrefixes.end(),
            [line](std::string const& commentPrefix)
            {
                return line.starts_with(commentPrefix);
            }))
        {
            result.push_back(std::string(line));
        }
    }
    return result;
}

std::vector<std::string> splitExpression(const std::string_view& expr)
{
    std::vector<std::string> res;
    std::string exprBody;
    std::size_t index = 0;
    while (index < expr.size() && expr[index] != ' ')
    {
        exprBody.push_back(expr[index++]);
    }
    res.push_back(std::move(exprBody));
    ++index;
    std::string variable_value_pair;
    while (index < expr.size())
    {
        while (index < expr.size() && expr[index] != ' ')
        {
            variable_value_pair.push_back(expr[index++]);
        }
        res.push_back(std::move(variable_value_pair));
        ++index;
    }
    return res;
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
    
    auto filename = std::filesystem::path(argv[1]);
    auto commentPrefixes = std::vector<std::string>{ "//", "#" };
    auto preprocessedLines = preprocess2(
        FileReader::open(filename, FileMode::text),
        commentPrefixes);

    for (const auto& line : preprocessedLines)
    {
        if (line[0] == '\n') continue;
        auto splittedLine = splitExpression(line);
        
        // We can assume `argc > 1` henceforth.
        auto e = expr::Expression::parse(splittedLine[0]);
        std::cout << "Expression: " << e << '\n'; // print expression

        auto variableSubstitutions = std::unordered_map<std::string, double>{ };
        for (int i = 1; i < splittedLine.size(); ++i)
        {
            variableSubstitutions.insert(parseVariableSubstitution(splittedLine[i]));
        }
        if (!variableSubstitutions.empty())
        {
            std::cout << "Substitutions: ";
            printVariableSubstitutions(variableSubstitutions);
            std::cout << "\n";
        }

        double value = evaluate(e, variableSubstitutions);
        std::cout << "Value: " << value << '\n' << std::endl;
    }  
}
catch (std::runtime_error const& e)
{
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
}
