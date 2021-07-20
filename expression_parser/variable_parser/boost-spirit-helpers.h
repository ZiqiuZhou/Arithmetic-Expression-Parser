#pragma once

#ifndef INCLUDED_CPP_PRACTICE_EX08_BOOST_SPIRIT_HELPERS_HPP_
#define INCLUDED_CPP_PRACTICE_EX08_BOOST_SPIRIT_HELPERS_HPP_


#include <string>
#include <cstddef>      // for size_t
#include <algorithm>    // for fill()
#include <stdexcept>    // for runtime_error
#include <string_view>

//#include <fmt/core.h>  // for format()

// Disable some Visual C++ warnings for Boost.Spirit X3.
#ifdef _MSC_VER
# pragma warning( push )
# pragma warning( disable: 4127 )  // conditional expression is constant
# pragma warning( disable: 4459 )  // declaration of '%s' hides global declaration
# pragma warning( disable: 4828 )  // The file contains a character starting at offset %x that is illegal in the current source character set
#endif // _MSC_VER

#include <boost/regex/pending/unicode_iterator.hpp>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

#ifdef _MSC_VER
# pragma warning( pop )
#endif // _MSC_VER


namespace asc::cpp_practice_ws20::ex08 {

    namespace x3 = boost::spirit::x3;

    namespace detail {


        using BaseIterator = std::string_view::const_iterator;
        using Iterator = boost::u8_to_u32_iterator<BaseIterator, char32_t>;


        std::size_t countCodePoints(Iterator start, Iterator pos);

        inline std::size_t
            countGraphemeClusters(Iterator start, Iterator pos)
        {
            // We wrongly assume that code points == grapheme clusters here. Screw us.
            return countCodePoints(start, pos);
        }

        [[noreturn]] void
            doReportError(std::string_view expr, std::string_view errorAnnotation, std::string_view msg);

        template <typename RangeT>
        [[noreturn]] void
            reportError(std::string_view expr, RangeT range, std::string_view msg)
        {
            std::size_t annotationSize = countGraphemeClusters(Iterator(expr.begin()), Iterator(expr.end()));
            std::size_t annotationFirst = countGraphemeClusters(Iterator(expr.begin()), range.begin());
            std::size_t annotationLast = countGraphemeClusters(Iterator(expr.begin()), range.end());
            auto errorAnnotation = std::string(annotationSize + 1, ' ');
            std::fill(errorAnnotation.begin() + annotationFirst, errorAnnotation.begin() + annotationLast, '~');
            doReportError(expr, errorAnnotation, msg);
        }

        [[noreturn]] void
            reportError(std::string_view expr, Iterator where, std::string_view msg);


    } // namespace detail


    template<typename T>
    constexpr auto
        as = [](auto p)
    {
        return x3::rule<class _, T>{ "as" } = x3::as_parser(p);
    };


    struct PositionCacheTag;


    class SemanticError : public std::runtime_error, public x3::position_tagged
    {
    public:
        SemanticError(std::string const& msg, x3::position_tagged posTag)
            : std::runtime_error(msg), x3::position_tagged(posTag)
        {
        }
    };


    template <typename ParserT, typename... ArgsT>
    void parseAndReportErrors(std::string_view text, ParserT&& parser, ArgsT&&... args)
    {
        auto first = detail::Iterator(text.begin());
        auto last = detail::Iterator(text.end());

        auto positions = boost::spirit::x3::position_cache<std::vector<detail::Iterator>>{ first, last };
        auto const parserWithPosCache = x3::with<PositionCacheTag>(std::ref(positions))[std::forward<ParserT>(parser)];
        bool r;
        try
        {
            r = phrase_parse(first, last, parserWithPosCache, std::forward<ArgsT>(args)...);
        }
        catch (SemanticError const& e)
        {
            detail::reportError(text, positions.position_of(e), e.what());
        }
        catch (boost::spirit::x3::expectation_failure<detail::Iterator> const& e)
        {
            detail::reportError(text, e.where(), e.which());
        }
        if (!r || first != last)
        {
            detail::reportError(text, first, "Parse failure");
        }
    }


} // namespace asc::cpp_practice_ws20::ex08


#endif // INCLUDED_CPP_PRACTICE_EX08_BOOST_SPIRIT_HELPERS_HPP_
