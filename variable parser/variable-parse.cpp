
#include <string>
#include <utility>      // for pair<>
#include <string_view>


// Disable some Visual C++ warnings for Boost.Spirit X3.
#ifdef _MSC_VER
# pragma warning( push )
# pragma warning( disable: 4127 )  // conditional expression is constant
# pragma warning( disable: 4459 )  // declaration of '%s' hides global declaration
# pragma warning( disable: 4828 )  // The file contains a character starting at offset %x that is illegal in the current source character set
#endif // _MSC_VER

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>

#include <boost/fusion/include/std_pair.hpp>  // to make Boost.Fusion `std::pair<>`-aware

#ifdef _MSC_VER
# pragma warning( pop )
#endif // _MSC_VER

#include "unicode.h"
#include "variable-parse.h"
#include "boost-spirit-helpers.h"


namespace asc::cpp_practice_ws20::ex08 {

    namespace x3 = boost::spirit::x3;


    std::pair<std::string, double>
        parseVariableSubstitution(std::string_view str)
    {
        // grammar definition
        static auto const variable = x3::rule<class VariableId, std::u32string>{ "variable" } =
            x3::raw[x3::lexeme[x3::alpha >> *(x3::alnum | '_')]];
        static auto const value = x3::rule<class ValueId, double>{ "value" } =
            x3::double_;
        static auto const variableSubstitution =
            variable > '=' > value;

        boost::spirit::x3::ascii::space_type space;
        auto theParser = variableSubstitution;
        auto result = std::pair<std::u32string, double>{ };
        parseAndReportErrors(str, theParser, space, result);
        return { to_utf8(result.first), result.second };
    }


} // namespace asc::cpp_practice_ws20::ex08
